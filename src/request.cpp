#include "rx/twitter/request.h"

#include <oauth.h>


namespace rx::twitter {

    std::function<rxcpp::observable<std::string>(rxcpp::observable<long>)> stringandignore() {
        return [](rxcpp::observable<long> s){
            return s.map([](long){return std::string{};}).ignore_elements();
        };
    }

    auto twitter_stream_reconnection(rxcpp::observe_on_one_worker tweetthread) {
        return [=](rxcpp::observable<std::string> chunks){
            return chunks |
                   // https://dev.twitter.com/streaming/overview/connecting
                   rxcpp::operators::timeout(std::chrono::seconds(90), tweetthread) |
                   rxcpp::operators::on_error_resume_next([=](std::exception_ptr ep) -> rxcpp::observable<std::string> {
                       try {rethrow_exception(ep);}
                       catch (const rxcurl::http_exception& ex) {
                           std::cerr << ex.what() << std::endl;
                           switch(rxcurl::errorclassfrom(ex)) {
                               case rxcurl::errorcodeclass::TcpRetry:
                                   std::cerr << "reconnecting after TCP error" << std::endl;
                                   return rxcpp::observable<>::empty<std::string>();
                               case rxcurl::errorcodeclass::ErrorRetry:
                                   std::cerr << "error code (" << ex.code() << ") - ";
                               case rxcurl::errorcodeclass::StatusRetry:
                                   std::cerr << "http status (" << ex.httpStatus() << ") - waiting to retry.." << std::endl;
                                   return rxcpp::observable<>::timer(std::chrono::seconds(5), tweetthread) | stringandignore();
                               case rxcurl::errorcodeclass::RateLimited:
                                   std::cerr << "rate limited - waiting to retry.." << std::endl;
                                   return rxcpp::observable<>::timer(std::chrono::minutes(1), tweetthread) | stringandignore();
                               case rxcurl::errorcodeclass::Invalid:
                                   std::cerr << "invalid request - propagate" << std::endl;
                               default:
                                   std::cerr << "unrecognized error - propagate" << std::endl;
                           };
                       }
                       catch (const rxcpp::timeout_error& ex) {
                           std::cerr << "reconnecting after timeout" << std::endl;
                           return rxcpp::observable<>::empty<std::string>();
                       }
                       catch (const std::exception& ex) {
                           std::cerr << "unknown exception - terminate" << std::endl;
                           std::cerr << ex.what() << std::endl;
                           std::terminate();
                       }
                       catch (...) {
                           std::cerr << "unknown exception - not derived from std::exception - terminate" << std::endl;
                           std::terminate();
                       }
                       return rxcpp::observable<>::error<std::string>(ep, tweetthread);
                   }) |
                   rxcpp::operators::repeat();
        };
    }

    rxcpp::observable<std::string> request(rxcpp::observe_on_one_worker tweetthread, rxcurl::rxcurl factory,
                                           std::string URL, std::string method,
                                           std::string CONS_KEY, std::string CONS_SEC,
                                           std::string ATOK_KEY, std::string ATOK_SEC) {
        std::string url;
        {
            char *signedurl = nullptr;
            RXCPP_UNWIND_AUTO([&]() {
                if (!!signedurl) {
                    free(signedurl);
                }
            });
            signedurl = oauth_sign_url2(
                    URL.c_str(), NULL, OA_HMAC, method.c_str(),
                    CONS_KEY.c_str(), CONS_SEC.c_str(), ATOK_KEY.c_str(), ATOK_SEC.c_str()
            );
            url = signedurl;
        }

        std::cerr << "start twitter stream request" << std::endl;

        return factory.create(rxcurl::http_request{url, method, {}, {}}) |
               rxcpp::rxo::map([](rxcurl::http_response r) {
                   return r.body.chunks;
               }) |
               rxcpp::operators::finally([]() { std::cerr << "end twitter stream request" << std::endl; }) |
               rxcpp::operators::merge(tweetthread)
               /*})*/ |
               twitter_stream_reconnection(tweetthread);
    }

}