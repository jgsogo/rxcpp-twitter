cmake_minimum_required(VERSION 3.0.1)

project(rxcpp_twitter VERSION 0.1.0)
include(${CMAKE_SOURCE_DIR}/cmake/logger.cmake)

find_package(rxcpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(oauth REQUIRED)
find_package(jsonformoderncpp REQUIRED)

add_subdirectory(utils)
add_subdirectory(rx)
