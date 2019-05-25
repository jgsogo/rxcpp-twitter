
#include <fstream>

#include "catch2/catch.hpp"
#include "data.h"

#include "rxcpp/twitter/tweet.h"


TEST_CASE("Tweet object stores text in cache", "[twitter::tweet]") {
    std::ifstream ifs(single_tweet);
    REQUIRE(ifs.is_open());

    auto tweet_json = nlohmann::json::parse(ifs);
    auto tweet = twitter::Tweet::create(std::move(tweet_json));

    SECTION("text") {
        REQUIRE( tweet.text() == "asdfa" );
    }
}

