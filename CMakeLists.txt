PROJECT(smallcppsoft)
CMAKE_MINIMUM_REQUIRED(VERSION 3.6.0)

set(CMAKE_AUTOMOC ON)#
set(CMAKE_INCLUDE_CURRENT_DIR ON)#

add_definitions("-Wall")

FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Widgets  REQUIRED)
FIND_PACKAGE(Qt5SerialPort REQUIRED)
FIND_PACKAGE(Qt5Sql REQUIRED)

SET(CMAKE_BUILD_TYPE Debug)
SET(EXECUTABLE_OUTPUT_PATH ./bin/${CMAKE_BUILD_TYPE})

SET(MAIN_DIR ./src/Main)

include_directories(
#${MAIN_DIR}
#${QT_INCLUDE_DIR}
#${CMAKE_CURRENT_BINARY_DIR}
./src
./src/interface
)

file(
 GLOB_RECURSE
 CPPLIST
# *.cpp
./src/*/*/*/*.cpp
./src/*/*/*.cpp
./src/*/*.cpp
./src/*.cpp
)

file(
 GLOB_RECURSE
 HEADERSLIST
 *.h
)

file(
 GLOB_RECURSE
 UILIST
 *.ui
)


SET(CMAKE_AUTOMOC ON)

qt5_wrap_ui(qtproject_UIS_H ${UILIST})

#include_directories(${CMAKE_CURRENT_BINARY_DIR})


ADD_EXECUTABLE( ${PROJECT_NAME} ${CPPLIST} ${HEADERSLIST} ${qtproject_UIS_H} )#${LIBLIST}

target_compile_features( ${PROJECT_NAME} PRIVATE cxx_range_for)

qt5_use_modules( ${PROJECT_NAME} Widgets  SerialPort Sql)
target_link_libraries( ${PROJECT_NAME}  Qt5::Core Qt5::Widgets Qt5::SerialPort Qt5::Sql)

#target_link_libraries( ${PROJECT_NAME} )



