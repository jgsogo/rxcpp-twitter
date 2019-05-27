cmake_minimum_required(VERSION 3.0.1)

find_package(rxcpp REQUIRED)
find_package(jsonformoderncpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(oauth REQUIRED)

set(SOURCES tweet.cpp stream.cpp request.cpp parse.cpp logger.cpp)
set(HEADERS ../../../include/rx/twitter/tweet.h)

add_library(twitter ${SOURCES} ${HEADERS})
target_link_libraries(twitter
    PUBLIC
        utils
        rxcpp::rxcpp
        jsonformoderncpp::jsonformoderncpp
        spdlog::spdlog
        oauth::oauth)
target_include_directories(twitter
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_options(twitter PRIVATE -Werror)

generate_export_header(twitter EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/twitter_export.h)

create_logger_files(${CMAKE_CURRENT_BINARY_DIR}/twitter_export.h)
