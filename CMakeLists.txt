cmake_minimum_required(VERSION 3.10)

project(mlib)

find_package(ECM 1.3.0 REQUIRED NO_MODULE)

find_package(Freetype REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(X11 REQUIRED)

pkg_check_modules(OpusFile REQUIRED opusfile IMPORTED_TARGET)
pkg_check_modules(VorbisFile REQUIRED vorbisfile IMPORTED_TARGET)

set(CMAKE_MODULE_PATH ${ECM_MODULE_DIR})
set(CMAKE_CXX_STANDARD 17)

include(GNUInstallDirs)
include(ECMUninstallTarget)

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
    mmouse.cpp
    mmusic.cpp
    mplaylist.cpp
    mreflection.cpp
    mresource.cpp
    mresourceloader.cpp
    mtexture.cpp
    mvideointerface.cpp
    mwindow.cpp
)
add_library(mjpg MODULE mjpg.cpp)
target_link_libraries(mjpg mlib JPEG::JPEG)
add_library(mpng MODULE mpng.cpp)
target_link_libraries(mpng mlib PNG::PNG)
add_library(mtype MODULE mtype.cpp)
target_link_libraries(mtype mlib Freetype::Freetype)
add_library(mopus MODULE mopus.cpp)
target_link_libraries(mopus mlib PkgConfig::OpusFile)
add_library(mvorbis MODULE mvorbis.cpp)
target_link_libraries(mvorbis mlib PkgConfig::VorbisFile)
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
    mplaylist.h
    mreflection.h
    mresourceloader.h
    msize.h
    mtexture.h
    mvariant.h
    mvideointerface.h
    mwindow.h
DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mlib)

target_link_libraries(mlib ${CMAKE_DL_LIBS} OpenAL::OpenAL OpenGL::OpenGL)

install(TARGETS mlib
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(DIRECTORY data/ DESTINATION share/mlib)

add_subdirectory(test)
add_subdirectory(wml)
