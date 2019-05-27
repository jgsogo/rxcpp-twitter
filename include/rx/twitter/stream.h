
#pragma once

#include "rx/twitter/rx_twitter_export.h"
#include <rxcpp/rx.hpp>

#include "rx/utils/curl.h"
#include "rx/twitter/tweet.h"


namespace rx::twitter {

    RX_TWITTER_EXPORT
    rxcpp::observable<Tweet> stream(rxcpp::observe_on_one_worker tweetthread, utils::rxcurl factory,
                                    std::string URL, std::string method,
                                    std::string CONS_KEY, std::string CONS_SEC,
                                    std::string ATOK_KEY, std::string ATOK_SEC);

    RX_TWITTER_EXPORT
    auto onlytweets() -> std::function<rxcpp::observable<Tweet>(rxcpp::observable<Tweet>)>;
}