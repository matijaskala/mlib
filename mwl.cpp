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

#include <MVideoInterface>
#include <mglobal.h>
#include <MDebug>
#include <MKeys>
#include <wayland-egl.h>
#include <dlfcn.h>
#include <unistd.h>
#include <EGL/egl.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <xkbcommon/xkbcommon.h>

using namespace std;

class WaylandWindow : public MWindow
{
public:
    struct wl_shell_surface *wl_shell_surface;
    struct wl_surface *wl_surface;
    struct wl_egl_window *wl_egl_window;
    EGLDisplay egl_display;
    EGLSurface egl_surface;
    EGLContext egl_context;

    WaylandWindow ( int width, int height, struct wl_shell_surface* _wl_shell_surface,
                    struct wl_surface* _wl_surface, struct wl_egl_window* _wl_egl_window,
                    EGLDisplay _egl_display, EGLSurface _egl_surface, EGLContext _egl_context );
    ~WaylandWindow();
    virtual void makeCurrent();
    virtual void resize () override;
    virtual void swapBuffers() override;
};

static WaylandWindow* kbFocused = nullptr;
static WaylandWindow* ptrFocused = nullptr;

WaylandWindow::WaylandWindow ( int width, int height, struct wl_shell_surface* _wl_shell_surface,
                               struct wl_surface* _wl_surface, struct wl_egl_window* _wl_egl_window,
                               EGLDisplay _egl_display, EGLSurface _egl_surface, EGLContext _egl_context )
                             : MWindow{width,height}
{
    wl_surface = _wl_surface;
    wl_egl_window = _wl_egl_window;
    wl_shell_surface = _wl_shell_surface;
    egl_display = _egl_display;
    egl_surface = _egl_surface;
    egl_context = _egl_context;

    static const struct wl_shell_surface_listener wl_shell_surface_listener = {
        [] ( void* data, struct wl_shell_surface *wl_shell_surface, uint32_t serial ) {
            wl_shell_surface_pong ( wl_shell_surface, serial );
        },
        [] ( void* data, struct wl_shell_surface *wl_shell_surface, uint32_t edges, int32_t width, int32_t height ) {
        },
        [] ( void* data, struct wl_shell_surface *wl_shell_surface ) {
        },
    };

    wl_shell_surface_add_listener ( wl_shell_surface, &wl_shell_surface_listener, this );

    wl_shell_surface_set_title ( wl_shell_surface, "program" );
    wl_shell_surface_set_toplevel ( wl_shell_surface );
}

WaylandWindow::~WaylandWindow()
{
    eglDestroySurface ( egl_display, egl_surface );

    wl_egl_window_destroy ( wl_egl_window );
    wl_surface_destroy ( wl_surface );
}

void WaylandWindow::makeCurrent()
{
    eglMakeCurrent ( egl_display, egl_surface, egl_surface, egl_context );
}

void WaylandWindow::resize ()
{
    wl_egl_window_resize ( wl_egl_window, size.width(), size.height(), 0, 0 );
}

void WaylandWindow::swapBuffers()
{
    eglSwapBuffers ( egl_display, egl_surface );
}

class WaylandVideoInterface : public MVideoInterface
{
    std::list<WaylandWindow*> window_list;
    virtual void handleEvents();
    virtual bool init();
    virtual void fini();
    virtual MWindow* createWindow ( int width, int height );
    virtual void destroyWindow ( MWindow* window );
    struct wl_display *wl_display;
    struct wl_shell *wl_shell;
    struct wl_compositor *wl_compositor;
    struct wl_seat *wl_seat;
    struct wl_pointer *wl_pointer;
    struct wl_keyboard *wl_keyboard;
    struct xkb_context *xkb_context;
    struct xkb_keymap *xkb_keymap;
    struct xkb_state *xkb_state;
    EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;
    MKey keymap[0x100];
    void keymap_init();
};


void WaylandVideoInterface::handleEvents()
{
    MVideoInterface::handleEvents();
    wl_display_dispatch_pending ( wl_display );
}

bool WaylandVideoInterface::init()
{
    static const struct wl_pointer_listener wl_pointer_listener = {
        [] ( void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y ) {
            auto _This = static_cast<WaylandVideoInterface*> ( data );
            for ( WaylandWindow* win: _This->window_list )
                if ( win->wl_surface == surface )
                    ptrFocused = win;
        },
        [] ( void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface ) {
            ptrFocused = nullptr;
        },
        [] ( void *data, struct wl_pointer *wl_pointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y ) {},
        [] ( void *data, struct wl_pointer *wl_pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state ) {
            auto _This = static_cast<WaylandVideoInterface*> ( data );
            if ( button == BTN_MIDDLE && state == WL_POINTER_BUTTON_STATE_RELEASED ) {
                ptrFocused->quit();
            }
        },
        [] ( void *data, struct wl_pointer *wl_pointer, uint32_t time, uint32_t axis, wl_fixed_t value ) {},
    };
    static const struct wl_keyboard_listener wl_keyboard_listener = {
        [] ( void *data, struct wl_keyboard *wl_keyboard, uint32_t format, int32_t fd, uint32_t size ) {
            auto _This = static_cast<WaylandVideoInterface*> ( data );
            void* map_str = mmap ( nullptr, size, PROT_READ, MAP_SHARED, fd, 0 );
            xkb_keymap_unref ( _This->xkb_keymap );
            xkb_state_unref ( _This->xkb_state );
            _This->xkb_keymap = xkb_keymap_new_from_string ( _This->xkb_context, static_cast<char*> ( map_str ), XKB_KEYMAP_FORMAT_TEXT_V1, static_cast<xkb_keymap_compile_flags> ( 0 ) );
            _This->xkb_state = xkb_state_new ( _This->xkb_keymap );
            munmap ( map_str, size );
        },
        [] ( void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys ) {
            auto _This = static_cast<WaylandVideoInterface*> ( data );
            for ( WaylandWindow* win: _This->window_list )
                if ( win->wl_surface == surface )
                    kbFocused = win;
        },
        [] ( void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, struct wl_surface *surface ) {
            kbFocused = nullptr;
        },
        [] ( void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state ) {
            auto _This = static_cast<WaylandVideoInterface*> ( data );
            const xkb_keysym_t* syms;
            xkb_keysym_t sym = XKB_KEY_NoSymbol;
            if ( xkb_state_key_get_syms ( _This->xkb_state, key + 8, &syms ) == 1 )
                sym = syms[0];
            MKey mKey = ( ( sym & 0xff00 ) == 0xff00 ) ? _This->keymap[sym & 0xff] : MKey ( sym );
            if ( state )
                kbFocused->keyPressed ( mKey, 0 );
            else
                kbFocused->keyReleased ( mKey, 0 );
        },
        [] ( void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group ) {},
        [] ( void *data, struct wl_keyboard *wl_keyboard, int32_t rate, int32_t delay ) {},
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

    wl_display = wl_display_connect ( nullptr );
    if ( !wl_display )
        return false;
    struct wl_registry *wl_registry = wl_display_get_registry ( wl_display );
    wl_registry_add_listener ( wl_registry, &wl_registry_listener, this );
    wl_display_dispatch ( wl_display );

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
    xkb_context = xkb_context_new ( static_cast<xkb_context_flags> ( 0 ) );
    keymap_init();

    return true;
}

void WaylandVideoInterface::fini()
{
    xkb_context_unref ( xkb_context );
    eglMakeCurrent ( egl_display, nullptr, nullptr, egl_context );
    eglDestroyContext ( egl_display, egl_context );
    wl_display_disconnect ( wl_display );
}

void WaylandVideoInterface::keymap_init()
{
    for ( MKey& key: keymap )
        key = M_KEY_UNKNOWN;
    keymap[XKB_KEY_BackSpace&0xFF] = M_KEY_BACKSPACE;
    keymap[XKB_KEY_Tab&0xFF] = M_KEY_TAB;
    keymap[XKB_KEY_Clear&0xFF] = M_KEY_CLEAR;
    keymap[XKB_KEY_Return&0xFF] = M_KEY_RETURN;
    keymap[XKB_KEY_Pause&0xFF] = M_KEY_PAUSE;
    keymap[XKB_KEY_Escape&0xFF] = M_KEY_ESCAPE;
    keymap[XKB_KEY_Delete&0xFF] = M_KEY_DELETE;

    keymap[XKB_KEY_KP_0&0xFF] = M_KEY_KP0;           /* Keypad 0-9 */
    keymap[XKB_KEY_KP_1&0xFF] = M_KEY_KP1;
    keymap[XKB_KEY_KP_2&0xFF] = M_KEY_KP2;
    keymap[XKB_KEY_KP_3&0xFF] = M_KEY_KP3;
    keymap[XKB_KEY_KP_4&0xFF] = M_KEY_KP4;
    keymap[XKB_KEY_KP_5&0xFF] = M_KEY_KP5;
    keymap[XKB_KEY_KP_6&0xFF] = M_KEY_KP6;
    keymap[XKB_KEY_KP_7&0xFF] = M_KEY_KP7;
    keymap[XKB_KEY_KP_8&0xFF] = M_KEY_KP8;
    keymap[XKB_KEY_KP_9&0xFF] = M_KEY_KP9;
    keymap[XKB_KEY_KP_Insert&0xFF] = M_KEY_KP0;
    keymap[XKB_KEY_KP_End&0xFF] = M_KEY_KP1; 
    keymap[XKB_KEY_KP_Down&0xFF] = M_KEY_KP2;
    keymap[XKB_KEY_KP_Page_Down&0xFF] = M_KEY_KP3;
    keymap[XKB_KEY_KP_Left&0xFF] = M_KEY_KP4;
    keymap[XKB_KEY_KP_Begin&0xFF] = M_KEY_KP5;
    keymap[XKB_KEY_KP_Right&0xFF] = M_KEY_KP6;
    keymap[XKB_KEY_KP_Home&0xFF] = M_KEY_KP7;
    keymap[XKB_KEY_KP_Up&0xFF] = M_KEY_KP8;
    keymap[XKB_KEY_KP_Page_Up&0xFF] = M_KEY_KP9;
    keymap[XKB_KEY_KP_Delete&0xFF] = M_KEY_KP_PERIOD;
    keymap[XKB_KEY_KP_Decimal&0xFF] = M_KEY_KP_PERIOD;
    keymap[XKB_KEY_KP_Divide&0xFF] = M_KEY_KP_DIVIDE;
    keymap[XKB_KEY_KP_Multiply&0xFF] = M_KEY_KP_MULTIPLY;
    keymap[XKB_KEY_KP_Subtract&0xFF] = M_KEY_KP_MINUS;
    keymap[XKB_KEY_KP_Add&0xFF] = M_KEY_KP_PLUS;
    keymap[XKB_KEY_KP_Enter&0xFF] = M_KEY_KP_ENTER;
    keymap[XKB_KEY_KP_Equal&0xFF] = M_KEY_KP_EQUALS;

    keymap[XKB_KEY_Up&0xFF] = M_KEY_UP;
    keymap[XKB_KEY_Down&0xFF] = M_KEY_DOWN;
    keymap[XKB_KEY_Right&0xFF] = M_KEY_RIGHT;
    keymap[XKB_KEY_Left&0xFF] = M_KEY_LEFT;
    keymap[XKB_KEY_Insert&0xFF] = M_KEY_INSERT;
    keymap[XKB_KEY_Home&0xFF] = M_KEY_HOME;
    keymap[XKB_KEY_End&0xFF] = M_KEY_END;
    keymap[XKB_KEY_Page_Up&0xFF] = M_KEY_PAGEUP;
    keymap[XKB_KEY_Page_Down&0xFF] = M_KEY_PAGEDOWN;

    keymap[XKB_KEY_F1&0xFF] = M_KEY_F1;
    keymap[XKB_KEY_F2&0xFF] = M_KEY_F2;
    keymap[XKB_KEY_F3&0xFF] = M_KEY_F3;
    keymap[XKB_KEY_F4&0xFF] = M_KEY_F4;
    keymap[XKB_KEY_F5&0xFF] = M_KEY_F5;
    keymap[XKB_KEY_F6&0xFF] = M_KEY_F6;
    keymap[XKB_KEY_F7&0xFF] = M_KEY_F7;
    keymap[XKB_KEY_F8&0xFF] = M_KEY_F8;
    keymap[XKB_KEY_F9&0xFF] = M_KEY_F9;
    keymap[XKB_KEY_F10&0xFF] = M_KEY_F10;
    keymap[XKB_KEY_F11&0xFF] = M_KEY_F11;
    keymap[XKB_KEY_F12&0xFF] = M_KEY_F12;
    keymap[XKB_KEY_F13&0xFF] = M_KEY_F13;
    keymap[XKB_KEY_F14&0xFF] = M_KEY_F14;
    keymap[XKB_KEY_F15&0xFF] = M_KEY_F15;

    keymap[XKB_KEY_Num_Lock&0xFF] = M_KEY_NUMLOCK;
    keymap[XKB_KEY_Caps_Lock&0xFF] = M_KEY_CAPSLOCK;
    keymap[XKB_KEY_Scroll_Lock&0xFF] = M_KEY_SCROLLOCK;
    keymap[XKB_KEY_Shift_R&0xFF] = M_KEY_RSHIFT;
    keymap[XKB_KEY_Shift_L&0xFF] = M_KEY_LSHIFT;
    keymap[XKB_KEY_Control_R&0xFF] = M_KEY_RCTRL;
    keymap[XKB_KEY_Control_L&0xFF] = M_KEY_LCTRL;
    keymap[XKB_KEY_Alt_R&0xFF] = M_KEY_RALT;
    keymap[XKB_KEY_Alt_L&0xFF] = M_KEY_LALT;
    keymap[XKB_KEY_Meta_R&0xFF] = M_KEY_RMETA;
    keymap[XKB_KEY_Meta_L&0xFF] = M_KEY_LMETA;
    keymap[XKB_KEY_Super_L&0xFF] = M_KEY_LSUPER; /* Left "Windows" */
    keymap[XKB_KEY_Super_R&0xFF] = M_KEY_RSUPER; /* Right "Windows */
    keymap[XKB_KEY_Mode_switch&0xFF] = M_KEY_MODE; /* "Alt Gr" key */
    keymap[XKB_KEY_Multi_key&0xFF] = M_KEY_COMPOSE; /* Multi-key compose */

    keymap[XKB_KEY_Help&0xFF] = M_KEY_HELP;
    keymap[XKB_KEY_Print&0xFF] = M_KEY_PRINT;
    keymap[XKB_KEY_Sys_Req&0xFF] = M_KEY_SYSREQ;
    keymap[XKB_KEY_Break&0xFF] = M_KEY_BREAK;
    keymap[XKB_KEY_Menu&0xFF] = M_KEY_MENU;
    keymap[XKB_KEY_Hyper_R&0xFF] = M_KEY_MENU;   /* Windows "Menu" key */
}

MWindow* WaylandVideoInterface::createWindow ( int width, int height )
{
    auto wl_surface = wl_compositor_create_surface ( wl_compositor );
    if ( !wl_surface )
        return nullptr;
    auto wl_egl_window = wl_egl_window_create ( wl_surface, width, height );
    if ( !wl_egl_window )
        return nullptr;
    EGLSurface egl_surface = eglCreateWindowSurface ( egl_display, egl_config, wl_egl_window, nullptr );
    if ( !egl_surface )
        return nullptr;
    auto wl_region = wl_compositor_create_region ( wl_compositor );
    wl_region_add ( wl_region, 0, 0, width, height );
    wl_surface_set_opaque_region ( wl_surface, wl_region );
    wl_region_destroy ( wl_region );

    auto w = new WaylandWindow{width, height, wl_shell_get_shell_surface ( wl_shell, wl_surface ),
                               wl_surface, wl_egl_window, egl_display, egl_surface, egl_context};
    window_list.push_back(w);

    return w;
}

void WaylandVideoInterface::destroyWindow ( MWindow* window )
{
    auto w = dynamic_cast<WaylandWindow*> (window);
    if ( !w )
        return;
    window_list.remove(w);
    MVideoInterface::destroyWindow(window);
}

static WaylandVideoInterface interface;


