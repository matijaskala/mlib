project(mlib)

cmake_minimum_required(VERSION 2.6)

find_package(Freetype REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${FREETYPE_INCLUDE_DIRS} ${OPENAL_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -pedantic")

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR lib)
endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)
if(NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
    set(CMAKE_INSTALL_INCLUDEDIR include)
endif(NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)

add_definitions(-DMLIB_LIBRARY_DIR="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/mlib/")
add_definitions(-DMLIB_DATA_DIR="${CMAKE_INSTALL_PREFIX}/share/mlib/")

include_directories(include .)
add_library(mlib SHARED
    maudio.cpp
    maudiofile.cpp
    maudioloader.cpp
    maudiostream.cpp
    mdebug.cpp
    mdl.cpp
    melapsedtimer.cpp
    meventhandler.cpp
    mfont.cpp
    mglobal.cpp
    mimage.cpp
    mmusic.cpp
    mobject.cpp
    mplaylist.cpp
    mreflection.cpp
    mresource.cpp
    mresourceloader.cpp
    mtexture.cpp
    mvideointerface.cpp
    mwindow.cpp
)
target_link_libraries(mlib nonstdc++)
add_library(mpng MODULE mpng.cpp)
target_link_libraries(mpng mlib png)
add_library(mtype MODULE mtype.cpp)
target_link_libraries(mtype mlib ${FREETYPE_LIBRARIES})
add_library(mvorbis MODULE mvorbis.cpp)
target_link_libraries(mvorbis mlib vorbisfile)
add_library(msdl MODULE msdl.cpp)
if(WIN32)
target_link_libraries(msdl mlib SDL dxguid winmm)
add_library(mdib MODULE mdib.cpp)
target_link_libraries(mdib mlib)
endif(WIN32)
if(NOT WIN32)
target_link_libraries(msdl mlib SDL)
add_library(mxlib MODULE mxlib.cpp)
target_link_libraries(mxlib mlib Xrandr)
add_library(mwl MODULE mwl.cpp)
target_link_libraries(mwl mlib wayland-egl EGL xkbcommon)
endif(NOT WIN32)

install(TARGETS mpng mtype mvorbis msdl LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/mlib)
if(WIN32)
install(TARGETS mdib LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/mlib)
endif(WIN32)
if(NOT WIN32)
install(TARGETS mxlib mwl LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/mlib)
endif(NOT WIN32)
install(FILES
    maudio.h
    maudiofile.h
    maudiostream.h
    mdebug.h
    mdl.h
    melapsedtimer.h
    meventhandler.h
    mfont.h
    mglobal.h
    mimage.h
    mkeys.h
    mmouse.h
    mmusic.h
    mobject.h
    mplaylist.h
    mreflection.h
    mresourceloader.h
    msignal.h
    msize.h
    mtexture.h
    mvariant.h
    mvector.h
    mvideointerface.h
    mwindow.h
DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mlib)

target_link_libraries(mlib ${CMAKE_DL_LIBS} ${OPENAL_LIBRARY} ${OPENGL_gl_LIBRARY})

install(TARGETS mlib
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib${LIB_SUFFIX}
)
install(DIRECTORY data/ DESTINATION share/mlib)

add_subdirectory(test)
add_subdirectory(wml)
