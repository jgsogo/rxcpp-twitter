
#include "rx/utils/curl.h"

namespace rx::utils {

    rxcurl create_rxcurl() {
        rxcurl r{std::make_shared<rxcurl_state>()};
        return r;
    };

    size_t rxcurlhttpCallback(char* ptr, size_t size, size_t nmemb, rxcpp::subscriber<std::string>* out) {
        int iRealSize = size * nmemb;

        std::string chunk;
        chunk.assign(ptr, iRealSize);
        out->on_next(chunk);

        return iRealSize;
    }

    rxcurl_state::rxcurl_state() : thread(rxcpp::observe_on_new_thread()), worker(), curlm(curl_multi_init()) {
        worker = rxcpp::observable<>::create<CURLMsg*>([this](rxcpp::subscriber<CURLMsg*> out){
            while(out.is_subscribed()) {
                int running = 0;
                curl_multi_perform(curlm, &running);
                for(;;) {
                    CURLMsg *message = nullptr;
                    int remaining = 0;
                    message = curl_multi_info_read(curlm, &remaining);
                    out.on_next(message);
                    if (!!message && remaining > 0) {
                        continue;
                    }
                    break;
                }
                int handlecount = 0;
                curl_multi_wait(curlm, nullptr, 0, 500, &handlecount);
                if (handlecount == 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
            out.on_completed();
        }) |
                 rxcpp::operators::subscribe_on(thread) |
                 rxcpp::operators::finally([](){std::cerr << "rxcurl worker exit" << std::endl;}) |
                 rxcpp::operators::publish() |
                 rxcpp::operators::connect_forever();
    }

    rxcurl_state::~rxcurl_state(){
        if (curlm != nullptr) {
            worker.as_blocking().subscribe();
            curl_multi_cleanup(curlm);
            curlm = nullptr;
        }
    }

    http_state::http_state(std::shared_ptr<rxcurl_state> m, http_request r) : rxcurl(m), request(r), code(CURLE_OK), httpStatus(0), curl(nullptr), headers(nullptr) {
        error.resize(CURL_ERROR_SIZE);
    }

    http_state::~http_state() {
        if (curl != nullptr) {
            // remove on worker thread
            auto localcurl = curl;
            auto localheaders = headers;
            auto localrxcurl = rxcurl;
            auto localRequest = request;
            chunkbus.get_subscription().unsubscribe();
            rxcpp::subscriber<std::string>* localChunkout = chunkout.release();
            rxcurl->worker
                    .take(1)
                    .tap([=](CURLMsg*){
                        //cerr << "rxcurl request destroy: " << localRequest.method << " - " << localRequest.url << endl;
                        curl_multi_remove_handle(localrxcurl->curlm, localcurl);
                        curl_easy_cleanup(localcurl);
                        curl_slist_free_all(localheaders);
                        delete localChunkout;
                    })
                    .subscribe();

            curl = nullptr;
            headers = nullptr;
        }
    }


    http_exception::http_exception(const std::shared_ptr<http_state>& s) : runtime_error(s->error), state(s) {
    }

    CURLcode http_exception::code() const {
        return state->code;
    }
    int http_exception::httpStatus() const {
        return state->httpStatus;
    }

    CURLcode http_response::code() const {
        return state->code;
    }
    int http_response::httpStatus() const {
        return state->httpStatus;
    }

    rxcpp::observable<http_response> rxcurl::create(http_request request) const {
        return rxcpp::observable<>::create<http_response>([=](rxcpp::subscriber<http_response>& out){

            auto requestState = std::make_shared<http_state>(state, request);

            http_response r{request, http_body{}, requestState};

            r.body.chunks = r.state->chunkbus.get_observable()
                    .tap([requestState](const std::string&){}); // keep connection alive

            r.body.complete = r.state->chunkbus.get_observable()
                    .tap([requestState](const std::string&){}) // keep connection alive
                    .start_with(std::string{})
                    .sum()
                    .replay(1)
                    .ref_count();

            // subscriber must subscribe during the on_next call to receive all marbles
            out.on_next(r);
            out.on_completed();

            auto localState = state;

            // start on worker thread
            state->worker
                    .take(1)
                    .tap([r, localState](CURLMsg*){

                        auto curl = curl_easy_init();

                        auto& request = r.state->request;

                        //cerr << "rxcurl request: " << request.method << " - " << request.url << endl;

                        // ==== cURL Setting
                        curl_easy_setopt(curl, CURLOPT_URL, request.url.c_str());

                        if (request.method == "POST") {
                            // - POST data
                            curl_easy_setopt(curl, CURLOPT_POST, 1L);
                            // - specify the POST data
                            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
                        }

                        auto& strings = r.state->strings;
                        auto& headers = r.state->headers;
                        for (auto& h : request.headers) {
                            strings.push_back(h.first + ": " + h.second);
                            headers = curl_slist_append(headers, strings.back().c_str());
                        }

                        if (!!headers) {
                            /* set our custom set of headers */
                            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                        }

                        // - User agent name
                        curl_easy_setopt(curl, CURLOPT_USERAGENT, "rxcpp curl client 1.1");
                        // - HTTP STATUS >=400 ---> ERROR
                        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);

                        // - Callback function
                        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, rxcurlhttpCallback);
                        // - Write data
                        r.state->chunkout.reset(new rxcpp::subscriber<std::string>(r.state->chunkbus.get_subscriber()));
                        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)r.state->chunkout.get());

                        // - keep error messages
                        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, &r.state->error[0]);

                        r.state->curl = curl;
                        curl_multi_add_handle(localState->curlm, curl);
                    })
                    .subscribe();

            std::weak_ptr<http_state> wrs = requestState;

            // extract completion and result
            state->worker
                    .filter([wrs](CURLMsg* message){
                        auto rs = wrs.lock();
                        return !!rs && !!message && message->easy_handle == rs->curl && message->msg == CURLMSG_DONE;
                    })
                    .take(1)
                    .tap([wrs](CURLMsg* message){
                        auto rs = wrs.lock();
                        if (!rs) {
                            return;
                        }

                        rs->error.resize(strlen(&rs->error[0]));

                        auto chunkout = rs->chunkbus.get_subscriber();

                        long httpStatus = 0;

                        curl_easy_getinfo(rs->curl, CURLINFO_RESPONSE_CODE, &httpStatus);
                        rs->httpStatus = httpStatus;

                        if(message->data.result != CURLE_OK) {
                            rs->code = message->data.result;
                            if (rs->error.empty()) {
                                rs->error = curl_easy_strerror(message->data.result);
                            }
                            //cerr << "rxcurl request fail: " << httpStatus << " - " << rs->error << endl;
                            rxcpp::observable<>::error<std::string>(http_exception(rs)).subscribe(chunkout);
                            return;
                        } else if (httpStatus > 499) {
                            //cerr << "rxcurl request http fail: " << httpStatus << " - " << rs->error << endl;
                            rxcpp::observable<>::error<std::string>(http_exception(rs)).subscribe(chunkout);
                            return;
                        }

                        //cerr << "rxcurl request complete: " << httpStatus << " - " << rs->error << endl;
                        chunkout.on_completed();
                    })
                    .subscribe();
        });
    }

    errorcodeclass errorclassfrom(const http_exception& ex) {
        switch(ex.code()) {
            case CURLE_COULDNT_RESOLVE_HOST:
            case CURLE_COULDNT_CONNECT:
            case CURLE_OPERATION_TIMEDOUT:
            case CURLE_BAD_CONTENT_ENCODING:
            case CURLE_REMOTE_FILE_NOT_FOUND:
                return errorcodeclass::ErrorRetry;
            case CURLE_GOT_NOTHING:
            case CURLE_PARTIAL_FILE:
            case CURLE_SEND_ERROR:
            case CURLE_RECV_ERROR:
                return errorcodeclass::TcpRetry;
            default:
                if (ex.code() == CURLE_HTTP_RETURNED_ERROR || ex.httpStatus() > 200) {
                    if (ex.httpStatus() == 420) {
                        return errorcodeclass::RateLimited;
                    } else if (ex.httpStatus() == 404 ||
                               ex.httpStatus() == 406 ||
                               ex.httpStatus() == 413 ||
                               ex.httpStatus() == 416) {
                        return errorcodeclass::Invalid;
                    }
                }
        };
        return errorcodeclass::StatusRetry;
    }

}