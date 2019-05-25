
#include <string>
#include <string_view>

#include <nlohmann/json.hpp>


namespace rx::twitter {

    // Contains the information from a single tweet
    class Tweet {
    public:
        static Tweet create(nlohmann::json tweet);
        ~Tweet();

        std::string_view text() const;
    protected:
        explicit Tweet(nlohmann::json tweet);
        explicit Tweet(const Tweet&) = delete;
        Tweet(Tweet&&) = default;
        Tweet& operator=(Tweet&&) = default;

    protected:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };

}
