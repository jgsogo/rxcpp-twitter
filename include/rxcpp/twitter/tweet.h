
#include <string>
#include <string_view>

#include <nlohmann/json.hpp>


namespace twitter {

    // Contains the information from a single tweet
    class Tweet {
    public:
        static Tweet create(nlohmann::json tweet);
    protected:
        Tweet(nlohmann::json tweet);
        ~Tweet() = default;
        Tweet(const Tweet&) = delete;
        Tweet(Tweet&&) = default;
        Tweet& operator=(Tweet&&) = default;

        std::string_view text() const;
    protected:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };

}
