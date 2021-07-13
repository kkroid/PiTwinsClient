include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/PiRPC/evpp/public)

file(GLOB pirpc_s_srcs ${CMAKE_SOURCE_DIR}/ThirdParty/PiRPC/server/private/*.cpp)
file(GLOB pirpc_c_srcs ${CMAKE_SOURCE_DIR}/ThirdParty/PiRPC/client/private/*.cpp)

add_subdirectory(evpp)

add_library(PiRPC SHARED ${pirpc_s_srcs} ${pirpc_c_srcs})
target_link_libraries(PiRPC
        evpp)