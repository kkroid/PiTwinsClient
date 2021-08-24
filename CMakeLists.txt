cmake_minimum_required(VERSION 3.5)

project(PiTwinsClient LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(USE_PiRPC_CODE 0)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 用于在纯cmake下编译本项目时能够找到Qt相关的库
#set(Qt5_DIR /Users/willzhang/Qt5.14.2/5.14.2/clang_64/lib/cmake/Qt5)
set(Qt5_DIR /Users/willzhang/Qt5/5.15.2/clang_64/lib/cmake/Qt5)

add_library(OpenCV SHARED IMPORTED)
set_target_properties(OpenCV PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/ThirdParty/OpenCV/mac/libopencv_world.4.5.2.dylib)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(ThirdParty/OpenCV/include)
include_directories(ThirdParty/PiRPC/include)
include_directories(Public)

add_executable(PiTwinsClient
        main.cpp
        mainwindow.cpp
        mainwindow.ui
        )

if (${USE_PiRPC_CODE})
    add_subdirectory(ThirdParty/PiRPC)
    set(PiRPC PiRPC)
else()
    add_library(PiRPC SHARED IMPORTED)
    add_library(libevent SHARED IMPORTED)
    set(PiRPC_DIR ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/PiRPC)
    IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set_target_properties(PiRPC PROPERTIES IMPORTED_LOCATION ${PiRPC_DIR}/lib/pi/libPiRPC.so)
        set_target_properties(libevent PROPERTIES IMPORTED_LOCATION ${PiRPC_DIR}/evpp/libevent/lib/pi/libevent-2.1.7.so)
    ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set_target_properties(PiRPC PROPERTIES IMPORTED_LOCATION ${PiRPC_DIR}/lib/macos/libPiRPC.dylib)
        set_target_properties(libevent PROPERTIES IMPORTED_LOCATION ${PiRPC_DIR}/evpp/libevent/lib/macos/libevent-2.1.7.dylib)
    ENDIF()
    set(PiRPC PiRPC libevent)
endif()
target_link_libraries(PiTwinsClient PRIVATE
        Qt5::Widgets
        OpenCV
        ${PiRPC})
