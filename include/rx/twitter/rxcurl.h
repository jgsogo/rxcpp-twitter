
#pragma once

#include <curl/curl.h>
#include <rxcpp/rx.hpp>


namespace rxcurl {

    struct rxcurl_state
    {
        rxcurl_state();
        rxcurl_state(const rxcurl_state&) = delete;
        rxcurl_state& operator=(const rxcurl_state&) = delete;
        rxcurl_state(rxcurl_state&&) = delete;
        rxcurl_state& operator=(rxcurl_state&&) = delete;

        ~rxcurl_state();

        rxcpp::observe_on_one_worker thread;
        rxcpp::observable<CURLMsg*> worker;
        CURLM* curlm;
    };

    struct http_request
    {
        std::string url;
        std::string method;
        std::map<std::string, std::string> headers;
        std::string body;
    };

    struct http_state
    {
        explicit http_state(std::shared_ptr<rxcurl_state> m, http_request r);
        http_state(const http_state&) = delete;
        http_state& operator=(const http_state&) = delete;
        http_state(http_state&&) = delete;
        http_state& operator=(http_state&&) = delete;

        ~http_state();

        std::shared_ptr<rxcurl_state> rxcurl;
        http_request request;
        std::string error;
        CURLcode code;
        int httpStatus;
        rxcpp::subjects::subject<std::string> chunkbus;
        std::unique_ptr<rxcpp::subscriber<std::string>> chunkout;
        CURL* curl;
        struct curl_slist *headers;
        std::vector<std::string> strings;
    };

    struct http_exception : std::runtime_error
    {
        explicit http_exception(const std::shared_ptr<http_state>& s);

        CURLcode code() const;
        int httpStatus() const;

        std::shared_ptr<http_state> state;
    };

    struct http_body
    {
        rxcpp::observable<std::string> chunks;
        rxcpp::observable<std::string> complete;
    };

    struct http_response
    {
        const http_request request;
        http_body body;

        CURLcode code() const;
        int httpStatus() const;

        std::shared_ptr<http_state> state;
    };

    size_t rxcurlhttpCallback(char* ptr, size_t size, size_t nmemb, rxcpp::subscriber<std::string>* out);

    struct rxcurl
    {
        std::shared_ptr<rxcurl_state> state;
        rxcpp::observable<http_response> create(http_request request) const;
    };

    rxcurl create_rxcurl();

    enum class errorcodeclass {
        Invalid,
        TcpRetry,
        ErrorRetry,
        StatusRetry,
        RateLimited
    };

    errorcodeclass errorclassfrom(const http_exception& ex);
}
