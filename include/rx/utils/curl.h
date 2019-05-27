
#pragma once

#include "rx/utils/rx_utils_export.h"
#include <curl/curl.h>
#include <rxcpp/rx.hpp>


namespace rx::utils {

    struct RX_UTILS_EXPORT rxcurl_state
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

    struct RX_UTILS_EXPORT http_state
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

    struct RX_UTILS_EXPORT http_exception : std::runtime_error
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

    struct RX_UTILS_EXPORT http_response
    {
        const http_request request;
        http_body body;

        CURLcode code() const;
        int httpStatus() const;

        std::shared_ptr<http_state> state;
    };

    RX_UTILS_EXPORT size_t rxcurlhttpCallback(char* ptr, size_t size, size_t nmemb, rxcpp::subscriber<std::string>* out);

    struct RX_UTILS_EXPORT rxcurl
    {
        std::shared_ptr<rxcurl_state> state;
        rxcpp::observable<http_response> create(http_request request) const;
    };

    RX_UTILS_EXPORT rxcurl create_rxcurl();

    enum class errorcodeclass {
        Invalid,
        TcpRetry,
        ErrorRetry,
        StatusRetry,
        RateLimited
    };

    RX_UTILS_EXPORT errorcodeclass errorclassfrom(const http_exception& ex);
}
