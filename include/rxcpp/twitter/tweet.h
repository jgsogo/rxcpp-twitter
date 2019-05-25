
#include <string>
#include <string_view>

#include <nlohmann/json.hpp>


namespace twitter {

    // Contains the information from a single tweet
    class Tweet {
    public:
        static Tweet create(nlohmann::json tweet);
        ~Tweet();

        std::string_view text() const;
    protected:
        Tweet(nlohmann::json tweet);
        Tweet(const Tweet&) = delete;
        Tweet(Tweet&&) = default;
        Tweet& operator=(Tweet&&) = default;

    protected:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };

}
