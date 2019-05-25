cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 REQUIRED)

set(DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/data")
message(STATUS "Tests will use DATA_PATH: ${DATA_PATH}")
configure_file(data.h.in data.h @ONLY)

add_executable(test_tweet 
    main.cpp
    logger_sink.cpp
    test_tweet.cpp)
target_link_libraries(test_tweet
    PUBLIC
        rxcpp_twitter
        Catch2::Catch2)
target_include_directories(test_tweet
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

# In order to add tests, we can use the common 'add_test' or the Catch2 magic below
# add_test(NAME test_tweet COMMAND test_tweet)

include(CTest)
include(Catch)
catch_discover_tests(test_tweet  EXTRA_ARGS -r compact)
