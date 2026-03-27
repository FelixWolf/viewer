# -*- cmake -*-

set(lscript_HEADER_FILES
    llscriptresource.h
    llscriptresourceconsumer.h
    llscriptresourcepool.h
    lscript_alloc.h
    lscript_byteconvert.h
    lscript_byteformat.h
    lscript_execute.h
    lscript_export.h
    lscript_http.h
    lscript_library.h
    lscript_rt_interface.h
    )

add_subdirectory(lscript_compile)
add_subdirectory(lscript_execute)
add_subdirectory(lscript_stdlib)

add_subdirectory(lscript_library)

add_library(lscript INTERFACE)
target_include_directories(lscript INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/lscript_compile
    ${CMAKE_CURRENT_SOURCE_DIR}/lscript_execute
    ${CMAKE_CURRENT_SOURCE_DIR}/lscript_library
    ${CMAKE_CURRENT_SOURCE_DIR}/lscript_stdlib
)
target_link_libraries(lscript INTERFACE lscript_compile)
target_link_libraries(lscript INTERFACE lscript_execute)
target_link_libraries(lscript INTERFACE lscript_library)
target_link_libraries(lscript INTERFACE lscript_stdlib)