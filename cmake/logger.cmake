
function(create_logger_files EXPORT_FILENAME EXPORT_MACRO)
    file(RELATIVE_PATH REL_PATH ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

    get_filename_component(EXPORT_FILENAME ${EXPORT_FILENAME} NAME)
    set(EXPORT_FILENAME ${REL_PATH}/${EXPORT_FILENAME})
    string(REGEX REPLACE "/" "." LOGGER_NAME "${REL_PATH}")
    set(INCLUDE_PATH ${REL_PATH})
    string(REGEX REPLACE "/" "::" NAMESPACE "${REL_PATH}")

    configure_file(${CMAKE_SOURCE_DIR}/src/logger.cpp.in logger.cpp @ONLY)
    configure_file(${CMAKE_SOURCE_DIR}/src/logger.h.in logger.h @ONLY)
endfunction()
