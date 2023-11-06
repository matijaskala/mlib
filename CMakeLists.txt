project(mlib)

cmake_minimum_required(VERSION 2.6)

find_package(ECM 1.3.0 REQUIRED NO_MODULE)

find_package(Freetype REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

pkg_check_modules(OPUSFILE REQUIRED opusfile)
pkg_check_modules(VORBIS REQUIRED vorbis vorbisfile)

set(CMAKE_MODULE_PATH ${ECM_MODULE_DIR})

include(GNUInstallDirs)
include(ECMUninstallTarget)

include_directories(${FREETYPE_INCLUDE_DIRS} ${OPENAL_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${OPUSFILE_INCLUDE_DIRS})

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
    mcairo.cpp
    mdebug.cpp
    mdl.cpp
    melapsedtimer.cpp
    meventhandler.cpp
    mfont.cpp
    mglobal.cpp
    mimage.cpp
    mlist.cpp
    mmouse.cpp
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
add_library(mjpg MODULE mjpg.cpp)
target_link_libraries(mjpg mlib ${JPEG_LIBRARIES})
add_library(mpng MODULE mpng.cpp)
target_link_libraries(mpng mlib ${PNG_LIBRARIES})
add_library(mtype MODULE mtype.cpp)
target_link_libraries(mtype mlib ${FREETYPE_LIBRARIES})
add_library(mopus MODULE mopus.cpp)
target_link_libraries(mopus mlib ${OPUSFILE_LIBRARIES})
add_library(mvorbis MODULE mvorbis.cpp)
target_link_libraries(mvorbis mlib ${VORBIS_LIBRARIES})
add_library(msdl MODULE msdl.cpp)
if(WIN32)
target_link_libraries(msdl mlib SDL dxguid winmm)
add_library(mdib MODULE mdib.cpp)
target_link_libraries(mdib mlib)
endif(WIN32)
if(NOT WIN32)
target_link_libraries(msdl mlib SDL)
add_library(mxlib MODULE mxlib.cpp)
target_link_libraries(mxlib mlib ${X11_Xrandr_LIB})
add_library(mwl MODULE mwl.cpp)
target_link_libraries(mwl mlib wayland-egl EGL xkbcommon)
endif(NOT WIN32)

install(TARGETS mjpg mpng mtype mopus mvorbis msdl LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/mlib)
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
    mcairo.h
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
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(DIRECTORY data/ DESTINATION share/mlib)

add_subdirectory(test)
add_subdirectory(wml)
