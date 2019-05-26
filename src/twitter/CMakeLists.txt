cmake_minimum_required(VERSION 3.0.1)

find_package(rxcpp REQUIRED)
find_package(jsonformoderncpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(oauth REQUIRED)

set(SOURCES tweet.cpp stream.cpp request.cpp parse.cpp)
set(HEADERS ../../include/rx/twitter/tweet.h)

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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include/>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
