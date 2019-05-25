cmake_minimum_required(VERSION 3.0.1)
project(rxcpp_twitter VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build ${project} unit tests" OFF)
option(BUILD_EXAMPLES "Build ${project} examples" OFF)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_info.cmake" OPTIONAL)

add_subdirectory(src)

if(BUILD_TESTS)
    message("Building tests")
    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
    message("Building examples")
    add_subdirectory(examples)
endif()
