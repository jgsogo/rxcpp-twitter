
#pragma once

#include <rxcpp/rx.hpp>

#include "rx/twitter/tweet.h"


namespace rx::twitter {

    struct parseerror {
        std::exception_ptr ep;
    };

    struct parsedtweets {
        rxcpp::observable<Tweet> tweets;
        rxcpp::observable<parseerror> errors;
    };

    std::function<rxcpp::observable<parsedtweets>(rxcpp::observable<std::string>)>
    parse(rxcpp::observe_on_one_worker worker, rxcpp::observe_on_one_worker tweetthread);


}