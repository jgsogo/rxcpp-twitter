cmake_minimum_required(VERSION 2.8.12)

add_executable(test_twitter
        main.cpp
        logger_sink.cpp
        test_tweet.cpp)
target_link_libraries(test_twitter
    PUBLIC
        rx_twitter
        Catch2::Catch2)
target_include_directories(test_twitter
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>)

# In order to add tests, we can use the common 'add_test' or the Catch2 magic below
# add_test(NAME test_tweet COMMAND test_tweet)

include(CTest)
include(Catch)
catch_discover_tests(test_twitter  EXTRA_ARGS -r compact)
