cmake_minimum_required(VERSION 3.0.1)

set(SOURCES split.cpp logger.cpp)
set(HEADERS ../../include/utils/split.h)

add_library(utils ${SOURCES} ${HEADERS})
target_link_libraries(utils
    PUBLIC
        spdlog::spdlog)
target_include_directories(utils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/>
        #$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/utils/>)
target_compile_options(utils PRIVATE -Werror)

# Dynamically generated files
generate_export_header(utils EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/utils_export.h)
create_logger_files(${CMAKE_CURRENT_BINARY_DIR}/utils_export.h UTILS_EXPORT)
