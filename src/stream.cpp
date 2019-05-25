
#include "rx/twitter/stream.h"

#include "rx/twitter/request.h"
#include "rx/twitter/parse.h"

namespace rx::twitter {

    rxcpp::observable<Tweet> stream(rxcpp::observe_on_one_worker tweetthread, rxcurl::rxcurl factory,
                                    std::string url, std::string method,
                                    std::string tw_consumer_key, std::string tw_consumer_secret,
                                    std::string tw_access_token, std::string tw_access_token_secret) {
        auto poolthread = rxcpp::observe_on_event_loop();

        rxcpp::observable<std::string> chunks;
        chunks = request(tweetthread, factory, url, method, tw_consumer_key, tw_consumer_secret, tw_access_token, tw_access_token_secret) |
                 // handle invalid requests by waiting for a trigger to try again
                 rxcpp::operators::on_error_resume_next([](std::exception_ptr ep){
                     std::cerr << rxcpp::rxu::what(ep) << std::endl;
                     return rxcpp::rxs::never<std::string>();
                 });

        auto tweets = chunks |
                      parse(poolthread, tweetthread) |
                      rxcpp::rxo::map([](twitter::parsedtweets p){
                          p.errors |
                          rxcpp::operators::tap([](twitter::parseerror e){
                              std::cerr << rxcpp::rxu::what(e.ep) << std::endl;
                          }) |
                          rxcpp::operators::subscribe<twitter::parseerror>();
                          return p.tweets;
                      }) |
                      rxcpp::operators::merge(tweetthread);

        auto batch_tweets = tweets |
                            onlytweets() |
                            rxcpp::rxo::publish() |
                            rxcpp::rxo::ref_count() |
                            rxcpp::rxo::as_dynamic();

        return batch_tweets;
    }


    auto onlytweets() -> std::function<rxcpp::observable<Tweet>(rxcpp::observable<Tweet>)> {
        return [](rxcpp::observable<Tweet> s){
            return s | rxcpp::rxo::filter([](const Tweet& tw) {
                return true; // TODO: Check for actual tweet
                //return !!tweet.count("timestamp_ms");
            });
        };
    }
}