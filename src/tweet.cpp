
#include "rx/twitter/tweet.h"

#include <optional>
#include <spdlog/spdlog.h>

#include "lazy.h"
#include "rx/utils/logger.h"

static auto logger = rx::utils::get_logger();

namespace rx::twitter {

    struct Tweet::Impl {
        explicit Impl(nlohmann::json tw) : tweet{std::move(tw)} {}
        nlohmann::json tweet;
        std::optional<std::string> text;
        std::optional<time_t> timestamp;
    };

    Tweet::Tweet(nlohmann::json tweet) : pImpl{std::make_unique<Impl>(std::move(tweet))} {}

    Tweet::Tweet(const Tweet& tw) : pImpl{std::make_unique<Impl>(tw.pImpl->tweet)} {}

    Tweet::Tweet(Tweet&& tw) : pImpl(std::move(tw.pImpl)) {}

    Tweet& Tweet::operator=(Tweet tw) {
        pImpl = std::move(tw.pImpl);
        return *this;
    }

    Tweet::~Tweet() {}

    Tweet Tweet::create(nlohmann::json tweet) {
        auto ret = Tweet{std::move(tweet)};
        return ret;
    }

    // Members are returned lazy ;)
    std::string_view Tweet::text() const {
        return pImpl->text.value_or(utils::Lazy{[=]()->std::string {
            logger->debug("Lazy evaluation of Tweet::text");
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

    time_t Tweet::timestamp() const {
        return pImpl->timestamp.value_or(utils::Lazy{[=]()->time_t {
            auto t = std::stoll(pImpl->tweet["timestamp_ms"].get<std::string>());
            pImpl->timestamp = t;
            return t;
        }});
    }
}
