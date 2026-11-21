
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_LIST_DIR}/QtVersionOption.cmake)
include(CheckLanguage)
check_language(CXX)
if (CMAKE_CXX_COMPILER)

    enable_language(CXX)
endif()

if (QT_MAJOR_VERSION STREQUAL "5")

    find_package(Qt${QT_MAJOR_VERSION}Core QUIET)
    if(TARGET Qt5::qmake)
        get_target_property(_qmake_executable_default Qt5::qmake LOCATION)

        set(QUERY_EXECUTABLE ${_qmake_executable_default}
            CACHE FILEPATH "Location of the Qt5 qmake executable")
        set(_exec_name_text "Qt5 qmake")
        set(_cli_option "-query")
    endif()
elseif(QT_MAJOR_VERSION STREQUAL "6")

    find_package(Qt6 COMPONENTS CoreTools QUIET CONFIG)
    if (TARGET Qt6::qtpaths)
        get_target_property(_qtpaths_executable Qt6::qtpaths LOCATION)

        set(QUERY_EXECUTABLE ${_qtpaths_executable}
            CACHE FILEPATH "Location of the Qt6 qtpaths executable")
        set(_exec_name_text "Qt6 qtpaths")
        set(_cli_option "--query")
    endif()
endif()

function(ecm_query_qt result_variable qt_variable)
    set(options TRY)
    set(oneValueArgs)
    set(multiValueArgs)

    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT QUERY_EXECUTABLE)
        if(ARGS_TRY)
            set(${result_variable} "" PARENT_SCOPE)
            message(STATUS "No ${_exec_name_text} executable found. Can't check ${qt_variable}")
            return()
        else()
            message(FATAL_ERROR "No ${_exec_name_text} executable found. Can't check ${qt_variable} as required")
        endif()
    endif()
    execute_process(
        COMMAND ${QUERY_EXECUTABLE} ${_cli_option} "${qt_variable}"
        RESULT_VARIABLE return_code
        OUTPUT_VARIABLE output
    )
    if(return_code EQUAL 0)
        string(STRIP "${output}" output)
        file(TO_CMAKE_PATH "${output}" output_path)
        set(${result_variable} "${output_path}" PARENT_SCOPE)
    else()
        message(WARNING "Failed call: ${_command} \"${qt_variable}\"")
        message(FATAL_ERROR "${_exec_name_text} call failed: ${return_code}")
    endif()
endfunction()
