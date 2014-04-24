project(mlib)

cmake_minimum_required(VERSION 2.6)

if(__${CMAKE_BUILD_TYPE}__ STREQUAL __Debug__)
    add_definitions(-D__DEBUG__)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic")

add_library(mlib SHARED
    mdebug.cpp
    meventhandler.cpp
    mobject.cpp
    mplugin.cpp
    mreflection.cpp
    nonstd/cxxabi.cpp
    nonstd/filesystem.cpp
    nonstd/module.cpp
    nonstd/xterm.cpp
)
add_library(mimageloader MODULE mimageloader.cpp)
#add_library(mpng MODULE mpng.cpp)
#target_link_libraries(mpng png)

target_link_libraries(mlib dl)

install(TARGETS mlib LIBRARY DESTINATION lib)

add_subdirectory(test)
