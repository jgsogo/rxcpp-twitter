
#pragma once

#include <memory>
#include <vector>
#include <spdlog/logger.h>

namespace rx::utils {

    std::shared_ptr<spdlog::logger> get_logger();
    std::shared_ptr<spdlog::logger> setup_logger(std::vector<spdlog::sink_ptr>& sinks);
    
}
