#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <spdlog/sinks/ostream_sink.h>

#include "rx/twitter/logger.h"
#include "logger_sink.h"

int main( int argc, char* argv[] ) {
    // Capture the logging, some tests need to use it to check some behaviours
    auto& oss = get_ostream_sink();
    auto ostream_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
    ostream_sink->set_pattern("[ostream] %v");
    std::vector<spdlog::sink_ptr> sinks{ostream_sink};
    auto logger = rx::twitter::setup_logger(sinks);
    logger->set_level(spdlog::level::debug);
    
    int result = Catch::Session().run( argc, argv );

    // global clean-up...

    return result;
}