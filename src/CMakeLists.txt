cmake_minimum_required(VERSION 3.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(rxcpp REQUIRED)
find_package(jsonformoderncpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(oauth REQUIRED)

set(SOURCES tweet.cpp logger.cpp stream.cpp request.cpp parse.cpp split.cpp curl.cpp)
set(HEADERS ../include/rx/twitter/tweet.h)

add_library(rxcpp_twitter ${SOURCES} ${HEADERS})
target_link_libraries(rxcpp_twitter
    PUBLIC
        rxcpp::rxcpp
        jsonformoderncpp::jsonformoderncpp
        spdlog::spdlog
        oauth::oauth)
target_include_directories(rxcpp_twitter
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
