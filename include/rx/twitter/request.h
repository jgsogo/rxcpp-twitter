
#pragma once

#include "rx/twitter/twitter_export.h"
#include <rxcpp/rx.hpp>

#include "rx/utils/curl.h"


namespace rx::twitter {

    TWITTER_EXPORT
    rxcpp::observable<std::string> request(rxcpp::observe_on_one_worker tweetthread, utils::rxcurl factory,
                                           std::string URL, std::string method,
                                           std::string CONS_KEY, std::string CONS_SEC,
                                           std::string ATOK_KEY, std::string ATOK_SEC);

}
