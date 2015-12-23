project(mlib)

cmake_minimum_required(VERSION 2.6)

if(__${CMAKE_BUILD_TYPE}__ STREQUAL __Debug__)
    add_definitions(-D__DEBUG__)
endif()

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
    maudioloader.cpp
    maudiostream.cpp
    mdebug.cpp
    mdl.cpp
    melapsedtimer.cpp
    meventhandler.cpp
    mevents.cpp
    mfilesystem.cpp
    mfont.cpp
    mglobal.cpp
    mimage.cpp
    mmusic.cpp
    mobject.cpp
    mreflection.cpp
    mresourceloader.cpp
    mtexture.cpp
    mvideo.cpp
    mvideointerface.cpp
)
target_link_libraries(mlib nonstdcxx)
add_library(mpng MODULE mpng.cpp)
target_link_libraries(mpng png)
add_library(mtype MODULE mtype.cpp)
target_link_libraries(mtype ${FREETYPE_LIBRARIES})
add_library(mvorbis MODULE mvorbis.cpp)
target_link_libraries(mvorbis vorbisfile)
add_library(msdl MODULE msdl.cpp)
target_link_libraries(msdl SDL)
add_library(mxlib MODULE mxlib.cpp)
target_link_libraries(mxlib X11)
add_library(mwl MODULE mwl.cpp)
target_link_libraries(mwl wayland-egl EGL xkbcommon)

install(TARGETS mpng mtype mvorbis msdl mxlib mwl LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/mlib)
install(FILES
    maudiofile.h
    maudiostream.h
    mdebug.h
    mdl.h
    melapsedtimer.h
    meventhandler.h
    mevents.h
    mfilesystem.h
    mfont.h
    mglobal.h
    mimage.h
    mkeys.h
    mmusic.h
    mobject.h
    mreflection.h
    mresourceloader.h
    msignal.h
    msize.h
    mtexture.h
    mvariant.h
    mvideo.h
    mvideointerface.h
DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mlib)

target_link_libraries(mlib ${CMAKE_DL_LIBS} ${FREETYPE_LIBRARIES} ${OPENAL_LIBRARY} ${OPENGL_gl_LIBRARY})

install(TARGETS mlib
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib${LIB_SUFFIX}
)
install(DIRECTORY data/ DESTINATION share/mlib)

add_subdirectory(test)
add_subdirectory(wml)
add_subdirectory(nonstd)
