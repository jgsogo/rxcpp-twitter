
#pragma once

#include "rx/twitter/twitter_export.h"
#include <string>
#include <string_view>

#include <nlohmann/json.hpp>


namespace rx::twitter {

    // Contains the information from a single tweet
    class TWITTER_EXPORT Tweet {
    public:
        Tweet(const Tweet&);
        Tweet(Tweet&&) noexcept; // = default;
        Tweet& operator=(Tweet);

        static Tweet create(nlohmann::json tweet);
        ~Tweet();

        std::string_view text() const;
        time_t timestamp() const;
    protected:
        explicit Tweet(nlohmann::json tweet);

        //Tweet& operator=(Tweet&&) = default;

    protected:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };

}
