cmake_minimum_required(VERSION 3.10)
project(launcher Java)
find_package(Java 1.7 REQUIRED COMPONENTS Development)

include(UseJava)
set(CMAKE_JAVA_JAR_ENTRY_POINT JavaCheck)
set(CMAKE_JAVA_COMPILE_FLAGS -target 7 -source 7 -Xlint:deprecation -Xlint:unchecked -Xlint:-options)

set(SRC
    JavaCheck.java
)

add_jar(JavaCheck ${SRC})
if(WIN32)
    install_jar(JavaCheck DESTINATION "${JARS_DEST_DIR}" COMPONENT Runtime)
else()
    install_jar(JavaCheck "${JARS_DEST_DIR}")
endif()
