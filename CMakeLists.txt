project(mlib)

cmake_minimum_required(VERSION 2.6)

if(__${CMAKE_BUILD_TYPE}__ STREQUAL __Debug__)
    add_definitions(-D__DEBUG__)
endif()

find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${FREETYPE_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic")

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR lib)
endif()

add_definitions(-DLIBDIR="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/mlib/")
add_definitions(-DDATADIR="${CMAKE_INSTALL_PREFIX}/share/mlib/")

include_directories(include .)
add_library(mlib SHARED
    mdataloader.cpp
    mdebug.cpp
    melapsedtimer.cpp
    meventhandler.cpp
    mfont.cpp
    mglobal.cpp
    mobject.cpp
    mplugin.cpp
    mreflection.cpp
    mtexture.cpp
    mtextureloader.cpp
    mvideo.cpp
    mvideointerface.cpp
)
target_link_libraries(mlib nonstdcxx)
add_library(mpng MODULE mpng.cpp)
target_link_libraries(mpng png)
add_library(msdl MODULE msdl.cpp)
target_link_libraries(msdl SDL)
add_library(mxlib MODULE mxlib.cpp)
target_link_libraries(mxlib X11)
add_library(mwl MODULE mwl.cpp)
target_link_libraries(mwl wayland-egl EGL xkbcommon)

install(TARGETS mpng msdl mxlib mwl LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/mlib)

target_link_libraries(mlib dl ${OPENGL_gl_LIBRARY} ${FREETYPE_LIBRARIES} ftgl)

install(TARGETS mlib LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY data/ DESTINATION share/mlib)

add_subdirectory(test)
add_subdirectory(wml)
add_subdirectory(nonstd)
