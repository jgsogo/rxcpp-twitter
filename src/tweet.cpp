
#include "rxcpp/twitter/tweet.h"

#include <optional>
#include <spdlog/spdlog.h>

#include "lazy.h"


namespace twitter {

    struct Tweet::Impl {
        Impl(nlohmann::json tw) : tweet{std::move(tw)} {}
        nlohmann::json tweet;
        std::optional<std::string> text;
    };

    Tweet::Tweet(nlohmann::json tweet) : pImpl{std::make_unique<Impl>(std::move(tweet))} {

    }

    Tweet::~Tweet() {}

    Tweet Tweet::create(nlohmann::json tweet) {
        auto ret = Tweet{std::move(tweet)};
        return ret;
    }

    // Members are returned lazy ;)
    std::string_view Tweet::text() const {
        return pImpl->text.value_or(utils::Lazy{[=]()->std::string {
            spdlog::debug("Lazy evaluation of Tweet::text");
            std::string value{};
            if (!!pImpl->tweet.count("extended_tweet")) {
                auto ex = pImpl->tweet["extended_tweet"];
                if (!!ex.count("full_text") && ex["full_text"].is_string()) {
                    value = ex["full_text"];
                }
            }
            if (!!pImpl->tweet.count("text") && pImpl->tweet["text"].is_string()) {
                value = pImpl->tweet["text"];
            }
            pImpl->text = value;
            return value;
        }});
    }
}
