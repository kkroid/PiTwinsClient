cmake_minimum_required(VERSION 3.15)
project(PiTwinsClient)
MESSAGE(STATUS "Pitwins:operation system is ${CMAKE_SYSTEM}")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

add_library(zmq SHARED IMPORTED)
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    MESSAGE(STATUS "current platform: Linux ")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    MESSAGE(STATUS "current platform: Windows")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    MESSAGE(STATUS "current platform: Darwin")
    set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/ThirdParty/OpenCV/mac)
    set_target_properties(zmq PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/ThirdParty/zmq/mac/libzmq.5.2.4.dylib)
ENDIF ()

#add_definitions(-DUSE_MJPEG=1)

find_package(Threads)
find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 14)
include_directories(ThirdParty/zmq/include)
include_directories(ThirdParty/spdlog)
include_directories(ThirdParty/cptl)
include_directories(ThirdParty/json)
include_directories(ThirdParty/libevent/include)
include_directories(ThirdParty/evpp/public)
include_directories(Public)

add_subdirectory(ThirdParty/evpp)

add_executable(PiTwinsClient
        main.cpp)

target_link_libraries(
        PiTwinsClient
        ${OpenCV_LIBS}
        ${CMAKE_THREAD_LIBS_INIT}
        zmq
        evpp
)
