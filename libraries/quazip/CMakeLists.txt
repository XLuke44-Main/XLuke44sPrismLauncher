
cmake_minimum_required(VERSION 3.15...3.18)

project(QuaZip VERSION 1.4)

include(cmake/clone-repo.cmake)

set(QUAZIP_LIB_VERSION ${QuaZip_VERSION})
set(QUAZIP_LIB_SOVERSION 1.4.0)

if(EMSCRIPTEN)

  option(BUILD_SHARED_LIBS "" OFF)
  option(QUAZIP_INSTALL "" OFF)
  option(QUAZIP_USE_QT_ZLIB "" OFF)
else()
  option(BUILD_SHARED_LIBS "" ON)
  option(QUAZIP_INSTALL "" ON)
  option(QUAZIP_USE_QT_ZLIB "" OFF)
endif()

option(QUAZIP_BZIP2 "Enables BZIP2 compression" ON)
option(QUAZIP_BZIP2_STDIO "Output BZIP2 errors to stdio" ON)

option(QUAZIP_FETCH_LIBS "Enables fetching third-party libraries if not found" ${WIN32})
option(QUAZIP_FORCE_FETCH_LIBS "Enables fetching third-party libraries always" OFF)

find_package(
  QT NAMES Qt6 Qt5
  QUIET COMPONENTS Core
)
if (NOT QT_FOUND)
  find_package(Qt4 QUIET COMPONENTS QtCore)
  if (Qt4_FOUND)
    set(QT_VERSION_MAJOR 4)
  else()

    set(QT_VERSION_MAJOR 5)
  endif()
endif()

set(QUAZIP_QT_MAJOR_VERSION ${QT_VERSION_MAJOR} CACHE STRING "Qt version to use (4, 5 or 6), defaults to ${QT_VERSION_MAJOR}")

if (QUAZIP_QT_MAJOR_VERSION EQUAL 6)
    set(CMAKE_CXX_STANDARD 17)
else()
    set(CMAKE_CXX_STANDARD 14)
endif()

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_DEBUG_POSTFIX d)

set(QUAZIP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(QUAZIP_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(QUAZIP_LIB_FILE_NAME quazip${QuaZip_VERSION_MAJOR}-qt${QUAZIP_QT_MAJOR_VERSION})
set(QUAZIP_LIB_TARGET_NAME QuaZip)
set(QUAZIP_DIR_NAME QuaZip-Qt${QUAZIP_QT_MAJOR_VERSION}-${QUAZIP_LIB_VERSION})
set(QUAZIP_PACKAGE_NAME QuaZip-Qt${QUAZIP_QT_MAJOR_VERSION})

if(QUAZIP_QT_MAJOR_VERSION EQUAL 6)
	find_package(Qt6 REQUIRED COMPONENTS Core Core5Compat)

  if(Qt6_VERSION VERSION_GREATER_EQUAL "6.3.1")
    set(QUAZIP_QT_ZLIB_COMPONENT BundledZLIB)
  else()
    set(QUAZIP_QT_ZLIB_COMPONENT Zlib)
  endif()
	set(QUAZIP_LIB_LIBRARIES Qt6::Core Qt6::Core5Compat)
	set(QUAZIP_PKGCONFIG_REQUIRES "zlib, Qt6Core")
elseif(QUAZIP_QT_MAJOR_VERSION EQUAL 5)
	find_package(Qt5 REQUIRED COMPONENTS Core)

  set(QUAZIP_QT_ZLIB_COMPONENT Zlib)
	set(QUAZIP_LIB_LIBRARIES Qt5::Core)
	set(QUAZIP_PKGCONFIG_REQUIRES "zlib, Qt5Core")
elseif(QUAZIP_QT_MAJOR_VERSION EQUAL 4)
	find_package(Qt4 4.5.0 REQUIRED COMPONENTS QtCore)

  set(QUAZIP_QT_ZLIB_COMPONENT Zlib)
	set(QUAZIP_LIB_LIBRARIES Qt4::QtCore)
	set(QUAZIP_PKGCONFIG_REQUIRES "zlib, QtCore")
else()
	message(FATAL_ERROR "Qt version ${QUAZIP_QT_MAJOR_VERSION} is not supported")
endif()

message(STATUS "Using Qt version ${QUAZIP_QT_MAJOR_VERSION}")

set(QUAZIP_QT_ZLIB_USED OFF)
if(QUAZIP_USE_QT_ZLIB)
    find_package(Qt${QUAZIP_QT_MAJOR_VERSION} OPTIONAL_COMPONENTS ${QUAZIP_QT_ZLIB_COMPONENT})
    if(Qt${QUAZIP_QT_MAJOR_VERSION}${QUAZIP_QT_ZLIB_COMPONENT}_FOUND)
        set(QUAZIP_LIB_LIBRARIES ${QUAZIP_LIB_LIBRARIES} Qt${QUAZIP_QT_MAJOR_VERSION}::${QUAZIP_QT_ZLIB_COMPONENT})
        set(QUAZIP_QT_ZLIB_USED ON)
    endif()
endif()

if(QUAZIP_QT_ZLIB_USED AND QUAZIP_QT_ZLIB_COMPONENT STREQUAL BundledZLIB)

  add_compile_definitions(Z_PREFIX)
endif()

if(NOT QUAZIP_QT_ZLIB_USED)

    if(EMSCRIPTEN)
      if(NOT DEFINED ZLIB_LIBRARY)
        message(WARNING "ZLIB_LIBRARY is not set")
      endif()

      if(NOT DEFINED ZLIB_INCLUDE)
        message(WARNING "ZLIB_INCLUDE is not set")
      else()
        include_directories(${ZLIB_INCLUDE})
      endif()

      if(NOT DEFINED ZCONF_INCLUDE)
        message(WARNING "ZCONF_INCLUDE is not set")
      else()
        include_directories(${ZCONF_INCLUDE})
      endif()

      set(QUAZIP_LIB_LIBRARIES ${QUAZIP_LIB_LIBRARIES} ${ZLIB_LIBRARY})
    else()
      find_package(ZLIB REQUIRED)
      set(QUAZIP_LIB_LIBRARIES ${QUAZIP_LIB_LIBRARIES} ZLIB::ZLIB)
    endif()  
endif()

set(QUAZIP_DEP)
set(QUAZIP_INC)
set(QUAZIP_LIB)
set(QUAZIP_LBD)

if(QUAZIP_BZIP2)

    set(QUAZIP_BZIP2 ON)

    if(NOT QUAZIP_FORCE_FETCH_LIBS)
        find_package(BZip2 QUIET)
    endif()

    if(BZIP2_FOUND AND NOT QUAZIP_FORCE_FETCH_LIBS)
        message(STATUS "Using BZIP2 ${BZIP2_VERSION_STRING}")

        list(APPEND QUAZIP_INC ${BZIP2_INCLUDE_DIRS})
        list(APPEND QUAZIP_LIB ${BZIP2_LIBRARIES})
        list(APPEND QUAZIP_LBD ${BZIP2_LIBRARY_DIRS})

        set(PC_PRIVATE_LIBS "${PC_PRIVATE_LIBS} -lbzip2")
    elseif(QUAZIP_FETCH_LIBS)
        clone_repo(bzip2 https://sourceware.org/git/bzip2.git)

        set(BZIP2_SRC
            ${BZIP2_SOURCE_DIR}/blocksort.c
            ${BZIP2_SOURCE_DIR}/bzlib.c
            ${BZIP2_SOURCE_DIR}/compress.c
            ${BZIP2_SOURCE_DIR}/crctable.c
            ${BZIP2_SOURCE_DIR}/decompress.c
            ${BZIP2_SOURCE_DIR}/huffman.c
            ${BZIP2_SOURCE_DIR}/randtable.c)

        set(BZIP2_HDR
            ${BZIP2_SOURCE_DIR}/bzlib.h
            ${BZIP2_SOURCE_DIR}/bzlib_private.h)

        add_library(bzip2 STATIC ${BZIP2_SRC} ${BZIP2_HDR})

        if(NOT QUAZIP_BZIP2_STDIO)
            target_compile_definitions(bzip2 PRIVATE -DBZ_NO_STDIO)
        endif()

        list(APPEND QUAZIP_DEP bzip2)
        list(APPEND QUAZIP_LIB bzip2)
        list(APPEND QUAZIP_INC ${BZIP2_SOURCE_DIR})
    else()
        message(STATUS "BZip2 library not found")

        set(QUAZIP_BZIP2 OFF)
    endif()

    if(QUAZIP_BZIP2)
        find_package(BZip2)
        add_compile_definitions(HAVE_BZIP2)
    endif()
endif()

add_subdirectory(quazip)

