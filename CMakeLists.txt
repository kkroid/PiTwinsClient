cmake_minimum_required(VERSION 3.5)

project(PiTwinsClient LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 用于在纯cmake下编译本项目时能够找到Qt相关的库
set(Qt5_DIR /Users/willzhang/Qt5.14.2/5.14.2/clang_64/lib/cmake/Qt5)

add_library(OpenCV SHARED IMPORTED)
set_target_properties(OpenCV PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/ThirdParty/OpenCV/mac/libopencv_world.4.5.2.dylib)

find_package(Threads)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(ThirdParty/spdlog)
include_directories(ThirdParty/cptl)
include_directories(ThirdParty/json)
include_directories(ThirdParty/OpenCV/include)
include_directories(ThirdParty/PiRPC/evpp/public)
include_directories(ThirdParty/PiRPC/common/public)
include_directories(ThirdParty/PiRPC/server/public)
include_directories(ThirdParty/PiRPC/client/public)
include_directories(Public)


add_definitions(-DPiRPC_TYPE=1)
add_subdirectory(ThirdParty/PiRPC)

add_executable(PiTwinsClient
        main.cpp
        mainwindow.cpp
        mainwindow.ui
        )

target_link_libraries(PiTwinsClient PRIVATE
        Qt5::Widgets
        OpenCV
        PiRPC
        )
