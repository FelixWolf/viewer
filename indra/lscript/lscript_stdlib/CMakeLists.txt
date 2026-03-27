# -*- cmake -*-

project(lscript_execute)

include(00-Common)
include(LLCommon)
include(LLMath)
include(LScript)

include_directories(
    ${LLCOMMON_INCLUDE_DIRS}
    ${LLMATH_INCLUDE_DIRS}
    ${LSCRIPT_INCLUDE_DIRS}
    )
include_directories(SYSTEM
    ${LLCOMMON_SYSTEM_INCLUDE_DIRS}
    )

set(lscript_stdlib_SOURCE_FILES
    lscript_stdlib.cpp
    lscript_experimental.cpp
    )

set(lscript_stdlib_HEADER_FILES
    CMakeLists.txt

    lscript_stdlib.h
    lscript_experimental.h
    )

set_source_files_properties(${lscript_stdlib_HEADER_FILES}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND lscript_stdlib_SOURCE_FILES ${lscript_stdlib_HEADER_FILES})

add_library (lscript_stdlib ${lscript_stdlib_SOURCE_FILES})

target_link_libraries(lscript_stdlib
        llmath
        llcommon
        lscript
        )