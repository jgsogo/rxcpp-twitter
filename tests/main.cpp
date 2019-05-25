#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <spdlog/sinks/ostream_sink.h>

#include "rxcpp/twitter/logger.h"
#include "logger_sink.h"

int main( int argc, char* argv[] ) {
    auto& oss = get_ostream_sink();
    auto ostream_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
    //ostream_sink->set_level(spdlog::level::debug);
    ostream_sink->set_pattern("[ostream] %v");
    auto logger = rx::twitter::setup_logger({ostream_sink});
    logger->set_level(spdlog::level::debug);
    
    int result = Catch::Session().run( argc, argv );

    // global clean-up...

    return result;
}