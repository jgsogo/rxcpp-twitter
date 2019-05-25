
#include <fstream>

#include "catch2/catch.hpp"
#include "rxcpp/twitter/tweet.h"

#include "data.h"
#include "logger_sink.h"


TEST_CASE("Tweet object stores text in cache", "[twitter::tweet]") {
    std::ifstream ifs(single_tweet);
    REQUIRE(ifs.is_open());
    auto& os = get_ostream_sink();

    auto tweet_json = nlohmann::json::parse(ifs);
    auto tweet = rx::twitter::Tweet::create(std::move(tweet_json));

    SECTION("text is evaluated first time") {
        REQUIRE(tweet.text() == "single_tweet::text");
        REQUIRE(os.str() == "[ostream] Lazy evaluation of Tweet::text\n");
        os.clear(); os.str(""); // Clear the log
    
        SECTION("text is already cached") {
            REQUIRE(tweet.text() == "single_tweet::text");
            REQUIRE(os.str() == "");
        }
    }
}

