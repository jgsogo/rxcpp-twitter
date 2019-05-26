cmake_minimum_required(VERSION 3.0.1)

find_package(rxcpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(oauth REQUIRED)

set(SOURCES logger.cpp split.cpp curl.cpp)
set(HEADERS ../../include/rx/utils/curl.h)

add_library(utils ${SOURCES} ${HEADERS})
target_link_libraries(utils
    PUBLIC
        rxcpp::rxcpp
        spdlog::spdlog
        oauth::oauth)
target_include_directories(utils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include/>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
