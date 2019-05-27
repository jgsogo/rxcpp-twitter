cmake_minimum_required(VERSION 3.0.1)

set(SOURCES logger.cpp curl.cpp)
set(HEADERS ../../../include/rx/utils/curl.h)

add_library(rx_utils ${SOURCES} ${HEADERS})
target_link_libraries(rx_utils
    PUBLIC
        rxcpp::rxcpp
        spdlog::spdlog
    PRIVATE
        )
target_include_directories(rx_utils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/>
        #$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/rx/utils/>)
target_compile_options(utils PRIVATE -Werror)

# Dynamically generated files
generate_export_header(rx_utils EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/rx_utils_export.h)
create_logger_files(${CMAKE_CURRENT_BINARY_DIR}/rx_utils_export.h RX_UTILS_EXPORT)
