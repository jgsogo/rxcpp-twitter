cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 REQUIRED)

set(DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/data")
message(STATUS "Tests will use DATA_PATH: ${DATA_PATH}")
configure_file(data.h.in data.h @ONLY)

add_subdirectory(test_twitter)