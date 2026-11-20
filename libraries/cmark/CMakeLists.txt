cmake_minimum_required(VERSION 3.10)
project(cmark VERSION 0.31.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(GenerateExportHeader)
configure_file(src/cmark_version.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmark_version.h @ONLY)

add_library(cmark STATIC
    src/blocks.c
    src/buffer.c
    src/cmark.c
    src/cmark_ctype.c
    src/houdini_href_e.c
    src/houdini_html_e.c
    src/houdini_html_u.c
    src/html.c
    src/inlines.c
    src/iterator.c
    src/node.c
    src/references.c
    src/scanners.c
    src/utf8.c
)

target_include_directories(cmark PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_compile_definitions(cmark PUBLIC CMARK_STATIC_DEFINE)
if(MSVC)
    target_compile_definitions(cmark PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(cmark PRIVATE -Wall -Wextra -pedantic $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>)
endif()
set_target_properties(cmark PROPERTIES POSITION_INDEPENDENT_CODE ON)
generate_export_header(cmark BASE_NAME cmark)
