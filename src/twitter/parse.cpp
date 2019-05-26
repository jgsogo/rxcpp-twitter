
#include "rx/twitter/parse.h"

#include "rx/utils/split.h"

namespace rx::twitter {

    auto isEndOfTweet = [](const std::string& s){
        if (s.size() < 2) return false;
        auto it0 = s.begin() + (s.size() - 2);
        auto it1 = s.begin() + (s.size() - 1);
        return *it0 == '\r' && *it1 == '\n';
    };

    std::function<rxcpp::observable<parsedtweets>(rxcpp::observable<std::string>)>
    parsetweets(rxcpp::observe_on_one_worker worker, rxcpp::observe_on_one_worker tweetthread) {
        return [=](rxcpp::observable<std::string> chunks) -> rxcpp::observable<parsedtweets> {
            return rxcpp::rxs::create<parsedtweets>([=](rxcpp::subscriber<parsedtweets> out){
                // create strings split on \r
                auto strings = chunks |
                               rxcpp::operators::concat_map([](const std::string& s){
                                   auto splits = utils::split(s, "\r\n");
                                   return rxcpp::sources::iterate(move(splits));
                               }) |
                               rxcpp::operators::filter([](const std::string& s){
                                   return !s.empty();
                               }) |
                               rxcpp::operators::publish() |
                               rxcpp::operators::ref_count();

                // filter to last string in each line
                auto closes = strings |
                              rxcpp::operators::filter(isEndOfTweet) |
                              rxcpp::rxo::map([](const std::string&){return 0;});

                // group strings by line
                auto linewindows = strings |
                                   window_toggle(closes | rxcpp::operators::start_with(0), [=](int){return closes;});

                // reduce the strings for a line into one string
                auto lines = linewindows |
                             rxcpp::operators::flat_map([](const rxcpp::observable<std::string>& w) {
                                 return w | rxcpp::operators::start_with<std::string>("") | rxcpp::operators::sum();
                             });

                int count = 0;
                rxcpp::rxsub::subject<parseerror> errorconduit;
                rxcpp::observable<Tweet> tweets = lines |
                                                  rxcpp::operators::filter([](const std::string& s){
                                                      return s.size() > 2 && s.find_first_not_of("\r\n") != std::string::npos;
                                                  }) |
                                                  rxcpp::operators::group_by([count](const std::string&) mutable -> int {
                                                      return ++count % std::thread::hardware_concurrency();}) |
                                                  rxcpp::rxo::map([=](rxcpp::observable<std::string> shard) {
                                                      return shard |
                                                             rxcpp::operators::observe_on(worker) |
                                                             rxcpp::rxo::map([=](const std::string& line) -> rxcpp::observable<Tweet> {
                                                                 try {
                                                                     auto tweet = nlohmann::json::parse(line);
                                                                     return rxcpp::rxs::from(Tweet::create(tweet));
                                                                 } catch (...) {
                                                                     errorconduit.get_subscriber().on_next(parseerror{std::current_exception()});
                                                                 }
                                                                 return rxcpp::rxs::empty<Tweet>();
                                                             }) |
                                                             rxcpp::operators::merge() |
                                                             rxcpp::operators::as_dynamic();
                                                  }) |
                                                  rxcpp::operators::merge(tweetthread) |
                                                  rxcpp::operators::tap([](const Tweet&){},[=](){
                                                      errorconduit.get_subscriber().on_completed();
                                                  }) |
                                                  rxcpp::operators::finally([=](){
                                                      errorconduit.get_subscriber().unsubscribe();
                                                  });

                out.on_next(parsedtweets{tweets, errorconduit.get_observable()});
                out.on_completed();

                return out.get_subscription();
            });
        };
    }

}
