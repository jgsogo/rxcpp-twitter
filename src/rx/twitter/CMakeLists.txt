cmake_minimum_required(VERSION 3.0.1)

set(SOURCES tweet.cpp stream.cpp request.cpp parse.cpp logger.cpp)
set(HEADERS ../../../include/rx/twitter/tweet.h)

add_library(rx_twitter ${SOURCES} ${HEADERS})
target_link_libraries(rx_twitter
    PUBLIC
        rx_utils
        rxcpp::rxcpp
        jsonformoderncpp::jsonformoderncpp
        spdlog::spdlog
    PRIVATE
        utils
        oauth::oauth)
target_include_directories(rx_twitter
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/>
        #$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/rx/twitter>)
target_compile_options(rx_twitter PRIVATE -Werror)

# Dynamically generated files
generate_export_header(rx_twitter EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/rx_twitter_export.h)
create_logger_files(${CMAKE_CURRENT_BINARY_DIR}/rx_twitter_export.h RX_TWITTER_EXPORT)
