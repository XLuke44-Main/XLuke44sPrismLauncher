
# SPDX-License-Identifier: BSD-3-Clause

if (DEFINED QT_MAJOR_VERSION)
    return()
endif()

if (TARGET Qt5::Core)
    set(QT_MAJOR_VERSION 5)
elseif (TARGET Qt6::Core)
    set(QT_MAJOR_VERSION 6)
else()
    option(BUILD_WITH_QT6 "Build against Qt 6" OFF)

    if (BUILD_WITH_QT6)
        set(QT_MAJOR_VERSION 6)
    else()
        set(QT_MAJOR_VERSION 5)
    endif()
endif()
