cmake_minimum_required(VERSION 3.0.1)

find_package(rxcpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(oauth REQUIRED)

set(SOURCES logger.cpp split.cpp curl.cpp)
set(HEADERS ../../../include/rx/utils/curl.h)

add_library(utils ${SOURCES} ${HEADERS})
target_link_libraries(utils
    PUBLIC
        rxcpp::rxcpp
        spdlog::spdlog
        oauth::oauth)
target_include_directories(utils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/rx/utils/>)
target_compile_options(utils PRIVATE -Werror)

generate_export_header(utils EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/utils_export.h)

create_logger_files(${CMAKE_CURRENT_BINARY_DIR}/utils_export.h)
