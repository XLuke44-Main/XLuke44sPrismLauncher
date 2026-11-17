
# SPDX-License-Identifier: BSD-3-Clause

include(CMakeParseArguments)

set(_ECM_QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_CPP "${CMAKE_CURRENT_LIST_DIR}/ECMQtDeclareLoggingCategory.cpp.in")
set(_ECM_QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_H   "${CMAKE_CURRENT_LIST_DIR}/ECMQtDeclareLoggingCategory.h.in")

function(ecm_qt_export_logging_category)
    set(options)
    set(oneValueArgs IDENTIFIER CATEGORY_NAME DEFAULT_SEVERITY EXPORT DESCRIPTION)
    set(multiValueArgs OLD_CATEGORY_NAMES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unexpected arguments to ecm_qt_export_logging_category: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    if(NOT ARG_IDENTIFIER)
        message(FATAL_ERROR "Missing IDENTIFIER argument for ecm_qt_export_logging_category")
    endif()
    if(NOT ARG_CATEGORY_NAME)
        message(FATAL_ERROR "Missing CATEGORY_NAME argument for ecm_qt_export_logging_category")
    endif()
    if(NOT ARG_DEFAULT_SEVERITY)
        set(ARG_DEFAULT_SEVERITY Info)
        set(is_explicite_default_severity FALSE)
    else()
        set(acceptible_severities Debug Info Warning Critical Fatal)
        list(FIND acceptible_severities "${ARG_DEFAULT_SEVERITY}" pos)
        if (pos EQUAL -1)
            message(FATAL_ERROR "Unknown DEFAULT_SEVERITY ${pos}")
        endif()
        set(is_explicite_default_severity TRUE)
    endif()
    if(NOT ARG_EXPORT)
        message(FATAL_ERROR "Missing EXPORT argument for ecm_qt_export_logging_category.")
    endif()
    if(NOT ARG_DESCRIPTION)
        message(FATAL_ERROR "Missing DESCRIPTION argument for ecm_qt_export_logging_category.")
    endif()

    set(_propertyprefix "ECM_QT_LOGGING_CATEGORY_${ARG_EXPORT}")
    set_property(GLOBAL APPEND PROPERTY "${_propertyprefix}_CATEGORIES" ${ARG_CATEGORY_NAME})
    set_property(GLOBAL PROPERTY "${_propertyprefix}_IDENTIFIER_${ARG_CATEGORY_NAME}" "${ARG_IDENTIFIER}")
    set_property(GLOBAL PROPERTY "${_propertyprefix}_DESCRIPTION_${ARG_CATEGORY_NAME}" "${ARG_DESCRIPTION}")
    set_property(GLOBAL PROPERTY "${_propertyprefix}_OLD_NAMES_${ARG_CATEGORY_NAME}" "${ARG_OLD_CATEGORY_NAMES}")
    if (is_explicite_default_severity)
        set_property(GLOBAL PROPERTY "${_propertyprefix}_DEFAULT_SEVERITY_${ARG_CATEGORY_NAME}" "${ARG_DEFAULT_SEVERITY}")
    endif()
endfunction()

function(ecm_qt_declare_logging_category sources_var)
    set(options)
    set(oneValueArgs HEADER IDENTIFIER CATEGORY_NAME DEFAULT_SEVERITY EXPORT DESCRIPTION)
    set(multiValueArgs OLD_CATEGORY_NAMES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unexpected arguments to ecm_qt_declare_logging_category: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    if(NOT ARG_HEADER)
        message(FATAL_ERROR "Missing HEADER argument for ecm_qt_declare_logging_category")
    endif()
    if(NOT ARG_IDENTIFIER)
        message(FATAL_ERROR "Missing IDENTIFIER argument for ecm_qt_declare_logging_category")
    endif()
    if(NOT ARG_CATEGORY_NAME)
        message(FATAL_ERROR "Missing CATEGORY_NAME argument for ecm_qt_declare_logging_category")
    endif()
    if(NOT ARG_DEFAULT_SEVERITY)
        set(ARG_DEFAULT_SEVERITY Info)
        set(is_explicite_default_severity FALSE)
    else()
        set(acceptible_severities Debug Info Warning Critical Fatal)
        list(FIND acceptible_severities "${ARG_DEFAULT_SEVERITY}" pos)
        if (pos EQUAL -1)
            message(FATAL_ERROR "Unknown DEFAULT_SEVERITY ${pos}")
        endif()
        set(is_explicite_default_severity TRUE)
    endif()
    if(ARG_EXPORT AND NOT ARG_DESCRIPTION)
        message(FATAL_ERROR "Missing DESCRIPTION argument for ecm_qt_declare_logging_category.")
    endif()
    if (TARGET ${sources_var})
        get_target_property(aliased_target ${sources_var} ALIASED_TARGET)
        if(aliased_target)
            message(FATAL_ERROR "Target argument passed to ecm_qt_declare_logging_category must not be an alias: ${sources_var}")
        endif()
    endif()

    if (NOT IS_ABSOLUTE "${ARG_HEADER}")
        set(ARG_HEADER "${CMAKE_CURRENT_BINARY_DIR}/${ARG_HEADER}")
    endif()

    string(REPLACE "::" ";" namespaces "${ARG_IDENTIFIER}")
    list(LENGTH namespaces len)
    math(EXPR last_pos "${len} - 1")
    list(GET namespaces ${last_pos} IDENTIFIER)
    list(REMOVE_AT namespaces ${last_pos})

    set(OPEN_NAMESPACES)
    set(CLOSE_NAMESPACES)
    foreach(ns ${namespaces})
        set(OPEN_NAMESPACES "${OPEN_NAMESPACES} namespace ${ns} {")
        set(CLOSE_NAMESPACES "} ${CLOSE_NAMESPACES}")
    endforeach()

    string(FIND "${ARG_HEADER}" "." pos REVERSE)
    if (pos EQUAL -1)
        set(cpp_filename "${ARG_HEADER}.cpp")
    else()
        string(SUBSTRING "${ARG_HEADER}" 0 ${pos} cpp_filename)
        set(cpp_filename "${cpp_filename}.cpp")
    endif()

    get_filename_component(HEADER_NAME "${ARG_HEADER}" NAME)

    string(REGEX REPLACE "[^a-zA-Z0-9]" "_" GUARD_NAME "${HEADER_NAME}")
    string(REPLACE "::" "_" GUARD_PREFIX "ECM_QLOGGINGCATEGORY_${ARG_IDENTIFIER}")
    string(TOUPPER "${GUARD_PREFIX}_${GUARD_NAME}" GUARD_NAME)

    if (NOT _ECM_QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_CPP)
       message(FATAL_ERROR "You must include(ECMQtDeclareLoggingCategory) before using ecm_qt_declare_logging_category")
    endif()

    configure_file("${_ECM_QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_CPP}" "${cpp_filename}")
    configure_file("${_ECM_QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_H}" "${ARG_HEADER}")

    if(TARGET ${sources_var})
        target_sources(${sources_var} PRIVATE ${cpp_filename} "${ARG_HEADER}")
    else()
        set(sources "${${sources_var}}")
        list(APPEND sources "${cpp_filename}")
        set(${sources_var} "${sources}" PARENT_SCOPE)
    endif()

    if (ARG_EXPORT)
        set(_default_severity)
        if (is_explicite_default_severity)
            set(_default_severity DEFAULT_SEVERITY ${ARG_DEFAULT_SEVERITY})
        endif()
        set(_old_category_name)
        if (ARG_OLD_CATEGORY_NAMES)
            set(_old_category_names OLD_CATEGORY_NAMES ${ARG_OLD_CATEGORY_NAMES})
        endif()
        ecm_qt_export_logging_category(
            IDENTIFIER ${ARG_IDENTIFIER}
            CATEGORY_NAME ${ARG_CATEGORY_NAME}
            ${_old_category_names}
            ${_default_severity}
            EXPORT ${ARG_EXPORT}
            DESCRIPTION "${ARG_DESCRIPTION}"
        )
    endif()
endfunction()

function(ecm_qt_install_logging_categories)

    if (ANDROID)
        return()
    endif()

    set(options SORT)
    set(oneValueArgs FILE EXPORT DESTINATION COMPONENT)
    set(multiValueArgs)

    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT ARGS_EXPORT)
        message(FATAL_ERROR "Missing EXPORT argument for ecm_qt_install_logging_categories")
    endif()

    if(NOT ARGS_DESTINATION)
        message(FATAL_ERROR "Missing DESTINATION argument for ecm_qt_install_logging_categories")
    endif()

    if(NOT ARGS_FILE)
        string(TOLOWER "${ARGS_EXPORT}.categories" ARGS_FILE)
    endif()

    set(_propertyprefix "ECM_QT_LOGGING_CATEGORY_${ARGS_EXPORT}")
    get_property(has_category GLOBAL PROPERTY "${_propertyprefix}_CATEGORIES" SET)

    if (NOT has_category)
        message(AUTHOR_WARNING "No Qt logging categories exported for \"${ARGS_EXPORT}\", generating & installing an empty file.")
    endif()

    get_property(_categories GLOBAL PROPERTY "${_propertyprefix}_CATEGORIES")
    if (ARGS_SORT)
        list(SORT _categories)
    endif()

    set(_renamed_categories)

    if (NOT IS_ABSOLUTE "${ARGS_FILE}")
        set(ARGS_FILE "${CMAKE_CURRENT_BINARY_DIR}/${ARGS_FILE}")
    endif()

    set(_categories_content
"# KDebugSettings data file
# This file was generated by ecm_qt_install_logging_categories(). DO NOT EDIT!

")

    foreach(_category IN LISTS _categories)
        get_property(_description GLOBAL PROPERTY "${_propertyprefix}_DESCRIPTION_${_category}")
        get_property(_identifier GLOBAL PROPERTY "${_propertyprefix}_IDENTIFIER_${_category}")
        get_property(_default_severity GLOBAL PROPERTY "${_propertyprefix}_DEFAULT_SEVERITY_${_category}")
        if (_default_severity)
            string(TOUPPER "${_default_severity}" _default_severity)
            set(_default_severity "DEFAULT_SEVERITY [${_default_severity}] ")  
        endif()
        get_property(_old_category_names GLOBAL PROPERTY "${_propertyprefix}_OLD_NAMES_${_category}")
        if (_old_category_names)
            list(APPEND _renamed_categories ${_category})
        endif()

        string(APPEND _categories_content "${_category} ${_description} ${_default_severity}IDENTIFIER [${_identifier}]\n")
    endforeach()

    file(GENERATE
        OUTPUT ${ARGS_FILE}
        CONTENT ${_categories_content}
    )

    set(_renamed_cats_file)
    if (_renamed_categories)
        get_filename_component(_dir ${ARGS_FILE} DIRECTORY)
        get_filename_component(_base_name ${ARGS_FILE} NAME_WE)
        set(_renamed_cats_file "${_dir}/${_base_name}.renamecategories")
        set(_renamed_cats_content
"# KDebugSettings data file
# This file was generated by ecm_qt_install_logging_categories(). DO NOT EDIT!

")

        foreach(_category IN LISTS _renamed_categories)
            get_property(_category_name_history GLOBAL PROPERTY "${_propertyprefix}_OLD_NAMES_${_category}")

            list(APPEND _category_name_history ${_category})
            list(GET _category_name_history 0 _old_category_name)
            list(REMOVE_AT _category_name_history 0)
            foreach(_category_name IN LISTS _category_name_history)

                string(APPEND _renamed_cats_content "${_old_category_name} ${_category_name}\n")
                set(_old_category_name ${_category_name})
            endforeach()
        endforeach()

        file(GENERATE
            OUTPUT ${_renamed_cats_file}
            CONTENT ${_renamed_cats_content}
        )
    endif()

    set(_component_install)
    if (ARGS_COMPONENT)
        set(_component_install COMPONENT ${ARGS_COMPONENT})
    endif()
    install(
        FILES ${ARGS_FILE} ${_renamed_cats_file}
        DESTINATION "${ARGS_DESTINATION}"
        ${_component_install}
    )
endfunction()
