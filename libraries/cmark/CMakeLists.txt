cmake_minimum_required(VERSION 3.10)

if(POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW)
endif()
if(POLICY CMP0092)
  cmake_policy(SET CMP0092 NEW)
endif()
if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

project(cmark
  LANGUAGES C CXX
  VERSION 0.31.0)

if(CMAKE_BUILD_TYPE MATCHES "asan|ubsan")
  list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
  include(FindAsan)
endif()

include(CMakePackageConfigHelpers)
include(GenerateExportHeader)
include(GNUInstallDirs)

if(NOT MSVC OR CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
  include(InstallRequiredSystemLibraries)
endif()

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "Do not build in-source.\nPlease remove CMakeCache.txt and the CMakeFiles/ directory.\nThen: mkdir build ; cd build ; cmake .. ; make")
endif()

set(_CMARK_BUILD_SHARED_LIBS_DEFAULT NO)
if(DEFINED CMARK_SHARED)
  message(AUTHOR_WARNING [=[
'CMARK_SHARED' has been replaced with the standard 'BUILD_SHARED_LIBS' to control the library type.
]=])
  set(_CMARK_BUILD_SHARED_LIBS_DEFAULT ${CMARK_SHARED})
endif()
if(DEFINED CMARK_STATIC)
  message(AUTHOR_WARNING [=[
'CMARK_STATIC' has been replaced with the standard 'BUILD_SHARED_LIBS' to control the library type.
]=])
  if(NOT CMARK_STATIC)
    set(_CMARK_BUILD_SHARED_LIBS_DEFAULT YES)
  else()
    set(_CMARK_BUILD_SHARED_LIBS_DEFAULT NO)
  endif()
endif()

option(BUILD_SHARED_LIBS "Build the CMark library as shared"
  ${_CMARK_BUILD_SHARED_LIBS_DEFAULT})

if(NOT MSVC)
  set(CMAKE_C_STANDARD 99)
  set(CMAKE_C_STANDARD_REQUIRED YES)
  set(CMAKE_C_EXTENSIONS NO)
endif()

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_compile_options($<$<CONFIG:Debug>:-DCMARK_DEBUG_NODES>)

function(cmark_add_compile_options target)
  if(MSVC)
    target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS)
  else()
    target_compile_options(${target} PRIVATE
            -Wall -Wextra -pedantic
            $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>)
  endif()
  if(CMAKE_BUILD_TYPE MATCHES profile)
    target_compile_options(${target} PRIVATE -pg)
  endif()
  if(CMAKE_BUILD_TYPE MATCHES ubsan)
    target_compile_options(${target} PRIVATE -fsanitize=undefined)
  endif()
endfunction()

add_subdirectory(src)

if(NOT CMAKE_SYSTEM_NAME STREQUAL Windows)
  add_subdirectory(man)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
  "Choose the type of build, options are: Debug Profile Release Asan Ubsan." FORCE)
endif(NOT CMAKE_BUILD_TYPE)
