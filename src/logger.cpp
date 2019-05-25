
#include "rxcpp/twitter/logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace rx { namespace twitter {
    static const std::string logger_name = "rx_twitter";

    std::shared_ptr<spdlog::logger> get_logger() {
        static std::shared_ptr<spdlog::logger> logger = spdlog::get(logger_name);
        if (not logger) {
            logger = spdlog::stdout_color_mt(logger_name);
        }
        return logger;
    }

    std::shared_ptr<spdlog::logger> setup_logger(std::vector<spdlog::sink_ptr> sinks) {
        auto logger = get_logger();
        for (auto it: sinks) {
            logger->sinks().push_back(it);
        }
        return logger;
    }

}}
