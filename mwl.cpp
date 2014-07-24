/*
 * This file is part of MLib
 * Copyright (C) 2014  Matija Skala <mskala@gmx.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "mvideointerface.h"
#include "meventhandler.h"
#include "mdebug.h"
#include <wayland-egl.h>
#include <dlfcn.h>
#include <unistd.h>
#include <EGL/egl.h>
#include <GL/gl.h>
#include <linux/input.h>

using namespace std;

class WaylandVideoInterface : public MVideoInterface
{
    virtual ~WaylandVideoInterface();
    virtual void beginPaint();
    virtual void endPaint();
    virtual void handleEvents();
    virtual bool init();
    virtual bool setVideoMode ( int x, int y );
    virtual MKey getKey ( uint64_t keysym ) { return MKey::UNKNOWN; }
    struct wl_display *wl_display;
    struct wl_compositor *wl_compositor;
    struct wl_seat *wl_seat;
    struct wl_shell *wl_shell;
    struct wl_shell_surface *wl_shell_surface;
    struct wl_surface *wl_surface;
    struct wl_egl_window *wl_egl_window;
    struct wl_pointer *wl_pointer;
    struct wl_keyboard *wl_keyboard;
    EGLDisplay egl_display;
    EGLSurface egl_surface;
    EGLConfig egl_config;
    EGLContext egl_context;
};

WaylandVideoInterface::~WaylandVideoInterface()
{
    eglMakeCurrent ( egl_display, nullptr, nullptr, egl_context );
    eglDestroySurface ( egl_display, egl_surface );
    eglDestroyContext ( egl_display, egl_context );
    wl_egl_window_destroy ( wl_egl_window );
    wl_shell_surface_destroy ( wl_shell_surface );
    wl_surface_destroy ( wl_surface );
    wl_display_disconnect ( wl_display );
}

void WaylandVideoInterface::beginPaint()
{
    MVideoInterface::beginPaint();
}

void WaylandVideoInterface::endPaint()
{
    MVideoInterface::endPaint();
    eglSwapBuffers ( egl_display, egl_surface );
}

void WaylandVideoInterface::handleEvents()
{
    MVideoInterface::handleEvents();
    wl_display_dispatch_pending ( wl_display );
}

bool WaylandVideoInterface::init()
{
    static const struct wl_pointer_listener wl_pointer_listener = {
        [] ( void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y ) {},
        [] ( void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface ) {},
        [] ( void *data, struct wl_pointer *wl_pointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y ) {},
        [] ( void *data, struct wl_pointer *wl_pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state ) {
            auto _This = static_cast<WaylandVideoInterface*> ( data );
            if ( button == 274 && state == WL_POINTER_BUTTON_STATE_RELEASED ) {
                MEventHandler::handlers.top().quit();
            }
        },
        [] ( void *data, struct wl_pointer *wl_pointer, uint32_t time, uint32_t axis, wl_fixed_t value ) {},
    };
    static const struct wl_keyboard_listener wl_keyboard_listener = {
        [] ( void *data, struct wl_keyboard *wl_keyboard, uint32_t format, int32_t fd, uint32_t size ) {},
        [] ( void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys ) {},
        [] ( void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, struct wl_surface *surface ) {},
        [] ( void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state ) {
            auto _This = static_cast<WaylandVideoInterface*> ( data );
            if ( key == KEY_ESC ) {
                MEventHandler::handlers.top().quit();
            }
        },
        [] ( void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group ) {},
    };
    static const struct wl_seat_listener wl_seat_listener = {
        [] ( void *data, struct wl_seat *wl_seat, uint32_t capabilities ) {
            auto _This = static_cast<WaylandVideoInterface*> ( data );
            if ( ( capabilities & WL_SEAT_CAPABILITY_POINTER ) && !_This->wl_pointer ) {
                _This->wl_pointer = wl_seat_get_pointer ( wl_seat );
                wl_pointer_add_listener ( _This->wl_pointer, &wl_pointer_listener, _This );
            }
            else if ( !( capabilities & WL_SEAT_CAPABILITY_POINTER ) && _This->wl_pointer ) {
                wl_pointer_destroy ( _This->wl_pointer );
                _This->wl_pointer = nullptr;
            }
            if ( ( capabilities & WL_SEAT_CAPABILITY_KEYBOARD ) && !_This->wl_keyboard ) {
                _This->wl_keyboard = wl_seat_get_keyboard ( wl_seat );
                wl_keyboard_add_listener ( _This->wl_keyboard, &wl_keyboard_listener, _This );
            }
            else if ( !( capabilities & WL_SEAT_CAPABILITY_KEYBOARD ) && _This->wl_keyboard ) {
                wl_keyboard_destroy ( _This->wl_keyboard );
                _This->wl_keyboard = nullptr;
            }
        },
        [] ( void *data, struct wl_seat *wl_seat, const char *name ) {},
    };
    static const struct wl_registry_listener wl_registry_listener = {
        [] ( void *data, struct wl_registry *wl_registry, uint32_t name, const char *interface, uint32_t version ) {
            auto _This = static_cast<WaylandVideoInterface*> ( data );
            string interface_name = interface;
            mDebug() << interface;
            if ( interface_name == "wl_compositor" ) {
                _This->wl_compositor = static_cast<struct wl_compositor*> ( wl_registry_bind ( wl_registry, name, &wl_compositor_interface, version ) );
            }
            else if ( interface_name == "wl_shell" ) {
                _This->wl_shell = static_cast<struct wl_shell*> ( wl_registry_bind ( wl_registry, name, &wl_shell_interface, version ) );
            }
            else if ( interface_name == "wl_seat" ) {
                _This->wl_seat = static_cast<struct wl_seat*> ( wl_registry_bind ( wl_registry, name, &wl_seat_interface, version ) );
                wl_seat_add_listener ( _This->wl_seat, &wl_seat_listener, _This );
            }
        },
        [] ( void* data, struct wl_registry *wl_registry, uint32_t name ) {},
    };
    static const struct wl_shell_surface_listener wl_shell_surface_listener = {
        [] ( void* data, struct wl_shell_surface *wl_shell_surface, uint32_t serial ) {
            wl_shell_surface_pong ( wl_shell_surface, serial );
        },
        [] ( void* data, struct wl_shell_surface *wl_shell_surface, uint32_t edges, int32_t width, int32_t height ) {
        },
        [] ( void* data, struct wl_shell_surface *wl_shell_surface ) {
        },
    };

    wl_display = wl_display_connect ( nullptr );
    if ( !wl_display )
        return false;
    struct wl_registry *wl_registry = wl_display_get_registry ( wl_display );
    wl_registry_add_listener ( wl_registry, &wl_registry_listener, this );
    wl_display_dispatch ( wl_display );
    wl_surface = wl_compositor_create_surface ( wl_compositor );
    wl_shell_surface = wl_shell_get_shell_surface ( wl_shell, wl_surface );
    wl_shell_surface_add_listener ( wl_shell_surface, &wl_shell_surface_listener, this );
    wl_egl_window = nullptr;

    egl_display = eglGetDisplay ( wl_display );
    EGLint major, minor;
    if ( !eglInitialize ( egl_display, &major, &minor ) )
        return false;
    if ( !eglBindAPI ( EGL_OPENGL_API ) )
        return false;

    EGLint n;
    EGLint attr[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_DEPTH_SIZE, 1,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE,
    };
    if ( !eglChooseConfig ( egl_display, attr, &egl_config, 1, &n ) )
        return false;
    egl_context = eglCreateContext ( egl_display, egl_config, EGL_NO_CONTEXT, nullptr );
    if ( !egl_context )
        return false;
    if ( !eglMakeCurrent ( egl_display, nullptr, nullptr, egl_context ) )
        return false;

    return MVideoInterface::init();
}

bool WaylandVideoInterface::setVideoMode ( int x, int y )
{
    screen_size = MSize ( x, y );
    wl_shell_surface_set_title ( wl_shell_surface, "program" );
    if ( wl_egl_window ) {
        wl_egl_window_resize ( wl_egl_window, screen_size.width(), screen_size.height(), 0, 0 );
    }
    else {
        wl_egl_window = wl_egl_window_create ( wl_surface, screen_size.width(), screen_size.height() );
        egl_surface = eglCreateWindowSurface ( egl_display, egl_config, wl_egl_window, nullptr );
        if ( !eglMakeCurrent ( egl_display, egl_surface, egl_surface, egl_context ) )
            return false;
        wl_shell_surface_set_toplevel ( wl_shell_surface );
    }

    glViewport ( 0, 0, screen_size.width(), screen_size.height() );
    glMatrixMode ( GL_PROJECTION );
    glLoadIdentity();
    glOrtho ( 0, screen_size.width(), screen_size.height(), 0, -1, 1 );
    glMatrixMode ( GL_MODELVIEW );

    wl_region* region = wl_compositor_create_region ( wl_compositor );
    wl_region_add ( region, 0, 0, screen_size.width(), screen_size.height() );
    wl_surface_set_opaque_region ( wl_surface, region );
    wl_region_destroy ( region );
    return true;
}

M_PLUGIN_EXPORT ( MVideoInterface, WaylandVideoInterface )


