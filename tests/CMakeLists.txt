cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 REQUIRED)

add_executable(tests main.cpp
                     test_tweet.cpp)
target_link_libraries(tests
    PUBLIC
        rxcpp_twitter
        Catch2::Catch2)
target_include_directories(tests
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

set(DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/data")
message(STATUS "Tests will use DATA_PATH: ${DATA_PATH}")
configure_file(data.h.in data.h @ONLY)

include(CTest)
include(Catch)
catch_discover_tests(tests)


