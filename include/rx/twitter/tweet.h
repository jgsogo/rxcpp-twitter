
#pragma once

#include <string>
#include <string_view>

#include <nlohmann/json.hpp>


namespace rx::twitter {

    // Contains the information from a single tweet
    class Tweet {
    public:
        Tweet(const Tweet&);
        Tweet(Tweet&&); // = default;
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
