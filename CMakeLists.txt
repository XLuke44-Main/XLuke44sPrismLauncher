cmake_minimum_required(VERSION 3.15)   

project(Launcher VERSION 1.0.3.2)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BUILD_DIR}" IS_IN_SOURCE_BUILD)
if(IS_IN_SOURCE_BUILD)
    message(FATAL_ERROR "You are building the Launcher in-source. Please separate the build tree from the source tree.")
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
if(UNIX)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
endif()
set(CMAKE_JAVA_TARGET_OUTPUT_DIR ${PROJECT_BINARY_DIR}/jars)

set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_C_STANDARD_REQUIRED true)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
include(GenerateExportHeader)
if(MSVC)

    set(CMAKE_CXX_FLAGS "/GS /permissive- /W4 ${CMAKE_CXX_FLAGS}")

    set(CMAKE_CXX_FLAGS "/EHsc ${CMAKE_CXX_FLAGS}")

    set(CMAKE_EXE_LINKER_FLAGS "/LTCG /MANIFEST:NO /STACK:8388608 ${CMAKE_EXE_LINKER_FLAGS}")

    foreach(lang C CXX)
        set("CMAKE_${lang}_FLAGS_RELEASE" "/GL /Gw /Gy /guard:cf")
    endforeach()

    foreach(config DEBUG RELWITHDEBINFO)
        foreach(lang C CXX)
            set(flags_var "CMAKE_${lang}_FLAGS_${config}")
            string(REGEX REPLACE "/Z[Ii]" "/Z7" ${flags_var} "${${flags_var}}")
        endforeach()
    endforeach()

    if(CMAKE_MSVC_RUNTIME_LIBRARY STREQUAL "MultiThreadedDLL")
        set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG Release "")
        set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release "")
    endif()
else()
    set(CMAKE_CXX_FLAGS "-Wall -pedantic -fstack-protector-strong --param=ssp-buffer-size=4 ${CMAKE_CXX_FLAGS}")

    if(WIN32)
        set(CMAKE_EXE_LINKER_FLAGS "-Wl,--stack,8388608 ${CMAKE_EXE_LINKER_FLAGS}")
    endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_DEPRECATED_WARNINGS=Y")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_DISABLE_DEPRECATED_BEFORE=0x050C00")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTOML_ENABLE_FLOAT16=0")

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -D_FORTIFY_SOURCE=2 ${CMAKE_CXX_FLAGS_RELEASE}")

option(DEBUG_ADDRESS_SANITIZER "Enable Address Sanitizer in Debug builds" OFF)

if ((CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo") AND DEBUG_ADDRESS_SANITIZER)
    message(STATUS "Address Sanitizer enabled for Debug builds, Turn it off with -DDEBUG_ADDRESS_SANITIZER=off")
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")

            message(STATUS "Address Sanitizer available on Clang MSVC frontend")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address /Oy-")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /fsanitize=address /Oy-")
        else()

            message(STATUS "Address Sanitizer available on Clang")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
        endif()
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

        message(STATUS "Address Sanitizer available on GCC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
        link_libraries("asan")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        message(STATUS "Address Sanitizer available on MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address /Oy-")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /fsanitize=address /Oy-")
    else()
        message(STATUS "Address Sanitizer not available on compiler ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()

option(ENABLE_LTO "Enable Link Time Optimization" off)

if(ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)

    if(ipo_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL TRUE)
        if(CMAKE_BUILD_TYPE)
            if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
                message(STATUS "IPO / LTO enabled")
            else()
                message(STATUS "Not enabling IPO / LTO on debug builds")
            endif()
        else()
            message(STATUS "IPO / LTO will only be enabled for release builds")
        endif()
    else()
        message(STATUS "IPO / LTO not supported: <${ipo_error}>")
    endif()
endif()

find_package(ECM QUIET NO_MODULE)
if(NOT ECM_FOUND)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libraries/extra-cmake-modules/modules/ECMQtDeclareLoggingCategory.cmake" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libraries/extra-cmake-modules/modules/ECMQueryQt.cmake")
        message(STATUS "Using bundled ECM")
        set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libraries/extra-cmake-modules/modules;${CMAKE_MODULE_PATH}")
    else()
        message(FATAL_ERROR
            " Could not find the required ECM modules\n \n"
            " Install Extra CMake Modules or restore the bundled fallback files under libraries/extra-cmake-modules/modules.")
    endif()
else()
    set(CMAKE_MODULE_PATH "${ECM_MODULE_PATH};${CMAKE_MODULE_PATH}")
endif()

set(Launcher_HELP_URL "https://prismlauncher.org/wiki/help-pages/%1" CACHE STRING "URL (with arg %1 to be substituted with page-id) that gets opened when the user requests help")

set(Launcher_VERSION_NAME "${Launcher_VERSION}")
set(Launcher_VERSION_NAME4 "${Launcher_VERSION}")
set(Launcher_VERSION_NAME4_COMMA "${Launcher_VERSION_MAJOR},${Launcher_VERSION_MINOR},${Launcher_VERSION_PATCH},${Launcher_VERSION_TWEAK}")

set(Launcher_BUILD_PLATFORM "unknown" CACHE STRING "A short string identifying the platform that this build was built for. Only used to display in the about dialog.")

set(Launcher_META_URL "https://meta.prismlauncher.org/v1/" CACHE STRING "URL to fetch Launcher's meta files from.")

set(Launcher_IMGUR_CLIENT_ID "" CACHE STRING "Optional Imgur Client ID for screenshot uploads")

set(Launcher_BUG_TRACKER_URL "https://github.com/PrismLauncher/PrismLauncher/issues" CACHE STRING "URL for the bug tracker.")

set(Launcher_MATRIX_URL "https://prismlauncher.org/matrix" CACHE STRING "URL to the Matrix Space")

set(Launcher_DISCORD_URL "https://prismlauncher.org/discord" CACHE STRING "URL for the Discord guild.")

set(Launcher_SUBREDDIT_URL "https://prismlauncher.org/reddit" CACHE STRING "URL for the subreddit.")

set(Launcher_FORCE_BUNDLED_LIBS OFF CACHE BOOL "Prevent using system libraries, if they are available as submodules")
set(Launcher_QT_VERSION_MAJOR "6" CACHE STRING "Major Qt version to build against")

if(UNIX AND APPLE)
    set(Launcher_GLFW_LIBRARY_NAME "libglfw.dylib" CACHE STRING "Name of native glfw library")
    set(Launcher_OPENAL_LIBRARY_NAME "libopenal.dylib" CACHE STRING "Name of native openal library")
elseif(UNIX)
    set(Launcher_GLFW_LIBRARY_NAME "libglfw.so" CACHE STRING "Name of native glfw library")
    set(Launcher_OPENAL_LIBRARY_NAME "libopenal.so" CACHE STRING "Name of native openal library")
elseif(WIN32)
    set(Launcher_GLFW_LIBRARY_NAME "glfw.dll" CACHE STRING "Name of native glfw library")
    set(Launcher_OPENAL_LIBRARY_NAME "OpenAL.dll" CACHE STRING "Name of native openal library")
endif()

# By using this key in your builds you accept the terms of use laid down in
# https://docs.microsoft.com/en-us/legal/microsoft-identity-platform/terms-of-use
set(Launcher_MSA_CLIENT_ID "c36a9fb6-4f2a-41ff-90bd-ae7cc92031eb" CACHE STRING "Client ID you can get from Microsoft Identity Platform when you register an application")

# By using a key in your builds you accept the terms and conditions laid down in
# https://support.curseforge.com/en/support/solutions/articles/9000207405-curse-forge-3rd-party-api-terms-and-conditions

set(Launcher_CURSEFORGE_API_KEY "$2a$10$wuAJuNZuted3NORVmpgUC.m8sI.pv1tOPKZyBgLFGjxFp/br0lZCC" CACHE STRING "API key for the CurseForge platform")
set(Launcher_COMPILER_NAME ${CMAKE_CXX_COMPILER_ID})
set(Launcher_COMPILER_VERSION ${CMAKE_CXX_COMPILER_VERSION})
set(Launcher_COMPILER_TARGET_SYSTEM ${CMAKE_SYSTEM_NAME})
set(Launcher_COMPILER_TARGET_SYSTEM_VERSION ${CMAKE_SYSTEM_VERSION})
set(Launcher_COMPILER_TARGET_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR})

include(GetGitRevisionDescription)
git_get_exact_tag(Launcher_GIT_TAG)
get_git_head_revision(Launcher_GIT_REFSPEC Launcher_GIT_COMMIT)

message(STATUS "Git commit: ${Launcher_GIT_COMMIT}")
message(STATUS "Git tag: ${Launcher_GIT_TAG}")
message(STATUS "Git refspec: ${Launcher_GIT_REFSPEC}")

string(TIMESTAMP TODAY "%Y-%m-%d")
set(Launcher_BUILD_TIMESTAMP "${TODAY}")

if(NOT Launcher_FORCE_BUNDLED_LIBS AND NOT FORCE_BUNDLED_ZLIB)
    find_package(ZLIB QUIET)
endif()
if(NOT ZLIB_FOUND)
    set(FORCE_BUNDLED_ZLIB TRUE CACHE BOOL "")
    mark_as_advanced(FORCE_BUNDLED_ZLIB)
endif()

include(QtVersionlessBackport)
if(Launcher_QT_VERSION_MAJOR EQUAL 5)
    set(QT_VERSION_MAJOR 5)
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets Concurrent Network Xml)

    if(NOT Launcher_FORCE_BUNDLED_LIBS)
        find_package(QuaZip-Qt5 1.3 QUIET)
    endif()
    if (NOT QuaZip-Qt5_FOUND)
        set(QUAZIP_QT_MAJOR_VERSION ${QT_VERSION_MAJOR} CACHE STRING "Qt version to use (4, 5 or 6), defaults to ${QT_VERSION_MAJOR}" FORCE)
        set(FORCE_BUNDLED_QUAZIP 1)
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNICODE -D_UNICODE")
elseif(Launcher_QT_VERSION_MAJOR EQUAL 6)
    set(QT_VERSION_MAJOR 6)
    find_package(Qt6 REQUIRED COMPONENTS Core CoreTools Widgets Concurrent Network Xml Core5Compat)
    list(APPEND Launcher_QT_LIBS Qt6::Core5Compat)

    if(NOT Launcher_FORCE_BUNDLED_LIBS)
        find_package(QuaZip-Qt6 1.3 QUIET)
    endif()
    if (NOT QuaZip-Qt6_FOUND)
        set(QUAZIP_QT_MAJOR_VERSION ${QT_VERSION_MAJOR} CACHE STRING "Qt version to use (4, 5 or 6), defaults to ${QT_VERSION_MAJOR}" FORCE)
        set(FORCE_BUNDLED_QUAZIP 1)
    endif()
else()
    message(FATAL_ERROR "Qt version ${Launcher_QT_VERSION_MAJOR} is not supported")
endif()

if(Launcher_QT_VERSION_MAJOR EQUAL 5)
    include(ECMQueryQt)
    ecm_query_qt(QT_PLUGINS_DIR  QT_INSTALL_PLUGINS)
    ecm_query_qt(QT_LIBS_DIR     QT_INSTALL_LIBS)
    ecm_query_qt(QT_LIBEXECS_DIR QT_INSTALL_LIBEXECS)
else()
    set(QT_PLUGINS_DIR  ${QT${QT_VERSION_MAJOR}_INSTALL_PREFIX}/${QT${QT_VERSION_MAJOR}_INSTALL_PLUGINS})
    set(QT_LIBS_DIR     ${QT${QT_VERSION_MAJOR}_INSTALL_PREFIX}/${QT${QT_VERSION_MAJOR}_INSTALL_LIBS})
    set(QT_LIBEXECS_DIR ${QT${QT_VERSION_MAJOR}_INSTALL_PREFIX}/${QT${QT_VERSION_MAJOR}_INSTALL_LIBEXECS})
endif()

if (Qt5_POSITION_INDEPENDENT_CODE)
    SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(NOT Launcher_FORCE_BUNDLED_LIBS)

    find_package(tomlplusplus 3.2.0 QUIET)

    find_package(ghc_filesystem QUIET)

    find_package(cmark QUIET)
endif()

include(ECMQtDeclareLoggingCategory)

set(Launcher_APP_BINARY_NAME "xluke44-prismlauncher" CACHE STRING "Name of the Launcher binary")
add_subdirectory(program_info)

if(NOT (UNIX AND APPLE))

    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${Launcher_Portable_File}" DESTINATION "." COMPONENT portable EXCLUDE_FROM_ALL)
endif()

if(UNIX AND APPLE)
    set(BINARY_DEST_DIR "${Launcher_Name}.app/Contents/MacOS")
    set(LIBRARY_DEST_DIR "${Launcher_Name}.app/Contents/MacOS")
    set(PLUGIN_DEST_DIR "${Launcher_Name}.app/Contents/MacOS")
    set(FRAMEWORK_DEST_DIR "${Launcher_Name}.app/Contents/Frameworks")
    set(RESOURCES_DEST_DIR "${Launcher_Name}.app/Contents/Resources")
    set(JARS_DEST_DIR "${Launcher_Name}.app/Contents/MacOS/jars")

    set(APPS "\${CMAKE_INSTALL_PREFIX}/${Launcher_Name}.app")

    set(MACOSX_BUNDLE_BUNDLE_NAME "${Launcher_DisplayName}")
    set(MACOSX_BUNDLE_INFO_STRING "${Launcher_DisplayName}: A custom launcher for Minecraft that allows you to easily manage multiple installations of Minecraft at once.")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "org.prismlauncher.${Launcher_Name}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${Launcher_VERSION_NAME}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${Launcher_VERSION_NAME}")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${Launcher_VERSION_NAME}")
    set(MACOSX_BUNDLE_ICON_FILE ${Launcher_Name}.icns)
    set(MACOSX_BUNDLE_COPYRIGHT "${Launcher_Copyright_Mac}")

    set(INSTALL_BUNDLE "full" CACHE STRING "Use fixup_bundle to bundle dependencies")

    install(FILES ${Launcher_Branding_ICNS} DESTINATION ${RESOURCES_DEST_DIR} RENAME ${Launcher_Name}.icns)

elseif(UNIX)
    include(KDEInstallDirs)

    set(BINARY_DEST_DIR "bin")
    set(LIBRARY_DEST_DIR "lib${LIB_SUFFIX}")
    set(JARS_DEST_DIR "share/${Launcher_Name}")

    set(INSTALL_BUNDLE "nodeps" CACHE STRING "Use fixup_bundle to bundle dependencies")

    SET(Launcher_BINARY_RPATH "$ORIGIN/../${LIBRARY_DEST_DIR}")
    set(PLUGIN_DEST_DIR "plugins")
    set(RESOURCES_DEST_DIR "${BINARY_DEST_DIR}")
    set(APPS "\${CMAKE_INSTALL_PREFIX}/${BINARY_DEST_DIR}/${Launcher_APP_BINARY_NAME}")
    set(DIRS ${QT_LIBS_DIR} ${QT_LIBEXECS_DIR} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${Launcher_Desktop} DESTINATION ${KDE_INSTALL_APPDIR})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${Launcher_MetaInfo} DESTINATION ${KDE_INSTALL_METAINFODIR})
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${Launcher_SVG} DESTINATION "${KDE_INSTALL_ICONDIR}/hicolor/scalable/apps")
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${Launcher_mrpack_MIMEInfo} DESTINATION ${KDE_INSTALL_MIMEDIR})

    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/launcher/qtlogging.ini" DESTINATION "share/${Launcher_Name}")

    if(Launcher_ManPage)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${Launcher_ManPage} DESTINATION "${KDE_INSTALL_MANDIR}/man6")
    endif()

    configure_file(launcher/Launcher.in "${CMAKE_CURRENT_BINARY_DIR}/LauncherScript" @ONLY)
    install(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/LauncherScript" DESTINATION "." RENAME ${Launcher_Name} COMPONENT portable EXCLUDE_FROM_ALL)

elseif(WIN32)
    set(BINARY_DEST_DIR ".")
    set(LIBRARY_DEST_DIR ".")
    set(PLUGIN_DEST_DIR ".")
    set(RESOURCES_DEST_DIR ".")
    set(JARS_DEST_DIR "jars")

    set(APPS "\${CMAKE_INSTALL_PREFIX}/${Launcher_Name}.exe")

    set(DIRS ${QT_LIBS_DIR} ${QT_LIBEXECS_DIR} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

    set(INSTALL_BUNDLE "full" CACHE STRING "Use fixup_bundle to bundle dependencies")
else()
    message(FATAL_ERROR "Platform not supported")
endif()

include(ExternalProject)
set_directory_properties(PROPERTIES EP_BASE External)

option(NBT_BUILD_SHARED "Build NBT shared library" OFF)
option(NBT_USE_ZLIB "Build NBT library with zlib support" OFF)
add_subdirectory(libraries/libnbtplusplus)

add_subdirectory(libraries/systeminfo)  
add_subdirectory(libraries/launcher)  
add_subdirectory(libraries/javacheck)  
if(FORCE_BUNDLED_ZLIB)
    message(STATUS "Using bundled zlib")

    set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)  
    set(SKIP_INSTALL_ALL ON)
    add_subdirectory(libraries/zlib EXCLUDE_FROM_ALL)

    check_include_file(unistd.h NEED_GENERATED_ZCONF)
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libraries/zlib/zconf.h.included" AND NOT NEED_GENERATED_ZCONF)

        message(STATUS "Undoing Rename")
        message(STATUS "    ${CMAKE_CURRENT_SOURCE_DIR}/libraries/zlib/zconf.h")
        file(RENAME "${CMAKE_CURRENT_SOURCE_DIR}/libraries/zlib/zconf.h.included" "${CMAKE_CURRENT_SOURCE_DIR}/libraries/zlib/zconf.h")
    endif()

    set(ZLIB_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/libraries/zlib" "${CMAKE_CURRENT_SOURCE_DIR}/libraries/zlib" CACHE STRING "" FORCE)
    set_target_properties(zlibstatic PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${ZLIB_INCLUDE_DIR}")
    add_library(ZLIB::ZLIB ALIAS zlibstatic)
    set(ZLIB_LIBRARY ZLIB::ZLIB CACHE STRING "zlib library name")

    find_package(ZLIB REQUIRED)
else()
    message(STATUS "Using system zlib")
endif()
if (FORCE_BUNDLED_QUAZIP)
    message(STATUS "Using bundled QuaZip")
    set(BUILD_SHARED_LIBS 0)   
    set(QUAZIP_INSTALL 0)
    add_subdirectory(libraries/quazip)  
else()
    message(STATUS "Using system QuaZip")
endif()
add_subdirectory(libraries/rainbow)  
add_subdirectory(libraries/LocalPeer)  
if(NOT tomlplusplus_FOUND)
    message(STATUS "Using bundled tomlplusplus")
    add_subdirectory(libraries/tomlplusplus)  
else()
    message(STATUS "Using system tomlplusplus")
endif()
if(NOT cmark_FOUND)
    message(STATUS "Using bundled cmark")
    set(BUILD_SHARED_LIBS 0) 
    add_subdirectory(libraries/cmark EXCLUDE_FROM_ALL)  
    add_library(cmark::cmark ALIAS cmark)
else()
    message(STATUS "Using system cmark")
endif()
add_subdirectory(libraries/katabasis)  
add_subdirectory(libraries/gamemode)
add_subdirectory(libraries/murmur2)  
if (NOT ghc_filesystem_FOUND)
    message(STATUS "Using bundled ghc_filesystem")
    add_subdirectory(libraries/filesystem)  
else()
    message(STATUS "Using system ghc_filesystem")
endif()
add_subdirectory(libraries/qdcss)  

add_subdirectory(buildconfig)

add_subdirectory(launcher)
