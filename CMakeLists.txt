cmake_minimum_required(VERSION 3.15)
project(PiTwinsClient)
MESSAGE(STATUS "Pitwins:operation system is ${CMAKE_SYSTEM}")

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    MESSAGE(STATUS "current platform: Linux ")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    MESSAGE(STATUS "current platform: Windows")
ENDIF()
    MESSAGE(STATUS "current platform: Darwin")

#add_definitions(-DUSE_MJPEG=1)

find_package(Threads)
find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 14)
include_directories(ThirdParty/zmq/include)
include_directories(ThirdParty/spdlog)
include_directories(ThirdParty/cptl)
include_directories(ThirdParty/json)
include_directories(public)

add_executable(PiTwinsClient
        main.cpp
        public/Snowflak.h)

add_library(zmq STATIC IMPORTED)
set_target_properties(zmq PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/ThirdParty/zmq/mac/libzmq.a)

target_link_libraries(
        PiTwinsClient
        ${OpenCV_LIBS}
        ${CMAKE_THREAD_LIBS_INIT}
        zmq
)
