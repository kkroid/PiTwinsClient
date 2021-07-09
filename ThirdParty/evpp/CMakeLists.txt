set(CMAKE_MACOSX_RPATH 0)

include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/evpp/public)
file(GLOB evpp_srcs ${CMAKE_SOURCE_DIR}/ThirdParty/evpp/private/*.cc)

add_library(libevent SHARED IMPORTED)
set_target_properties(libevent PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/ThirdParty/libevent/lib/libevent-2.1.7.dylib)

add_library(evpp SHARED ${evpp_srcs})
target_link_libraries(evpp
        libevent
        pthread)

#add_subdirectory(examples)