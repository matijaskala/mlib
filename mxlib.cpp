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

#include <mglobal.h>
#include <MVideoInterface>
#include <MEventHandler>
#include <MKeys>
#include <GL/glx.h>
#include <unistd.h>

class XlibVideoInterface : public MVideoInterface
{
    virtual void beginPaint() override;
    virtual void endPaint() override;
    virtual void handleEvents() override;
    virtual bool init() override;
    virtual void fini() override;
    virtual bool setVideoMode ( int x, int y ) override;
    MKey getKey ( unsigned int keycode );
    Display* xdisplay;
    Window xwindow;
    GLXContext context;
    MKey keymap[0x100];
    void keymap_init();
};

static Atom WM_DELETE_WINDOW;

void XlibVideoInterface::beginPaint()
{
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void XlibVideoInterface::endPaint()
{
    glFlush();
    usleep ( 20 );
}

void XlibVideoInterface::handleEvents()
{
    MEventHandler& handler = MEventHandler::handlers.top();
    while ( XPending ( xdisplay ) ) {
        XEvent ev;
        XNextEvent ( xdisplay, &ev );
        switch ( ev.type ) {
            case Expose:
                break;
            case ClientMessage:
                if ( ev.xclient.data.l[0] == WM_DELETE_WINDOW ) {
                    handler.quit();
                }
                break;
            case ConfigureNotify:
                screen_size = MSize ( ev.xconfigure.width, ev.xconfigure.height );
    glViewport ( 0, 0, screen_size.width(), screen_size.height() );
    glMatrixMode ( GL_PROJECTION );
    glLoadIdentity();
    glOrtho ( 0, screen_size.width(), screen_size.height(), 0, -1, 1 );
    glMatrixMode ( GL_MODELVIEW );
                break;
            case KeyPress:
                handler.key_pressed ( getKey ( ev.xkey.keycode ), /*TODO*/0 );
                break;
            case KeyRelease:
                if ( XEventsQueued ( xdisplay, QueuedAfterReading ) ) {
                    XEvent nev;
                    XPeekEvent(xdisplay, &nev);
                    if ( nev.type == KeyPress && nev.xkey.time == ev.xkey.time && nev.xkey.keycode == ev.xkey.keycode ) {
                        XNextEvent(xdisplay, &ev);
                        break;
                    }
                }
                handler.key_released ( getKey ( ev.xkey.keycode ), /*TODO*/0 );
                break;
        }
    }
}

bool XlibVideoInterface::init()
{
    xdisplay = XOpenDisplay ( nullptr );
    if ( !xdisplay )
        return false;

    keymap_init();

    WM_DELETE_WINDOW = XInternAtom ( xdisplay, "WM_DELETE_WINDOW", False );

    int attributeList[] = { 
          GLX_RGBA, 
          GLX_RED_SIZE,   1, 
          GLX_GREEN_SIZE, 1, 
          GLX_BLUE_SIZE,  1, 
          None };
    XVisualInfo* vi = glXChooseVisual ( xdisplay, DefaultScreen(xdisplay), attributeList );
    Window xroot = RootWindow ( xdisplay, vi->screen );
    Colormap xcolormap = XCreateColormap ( xdisplay, xroot, vi->visual, AllocNone );

    XSetWindowAttributes attr;
    attr.colormap = xcolormap;
    attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask;
    xwindow = XCreateWindow ( xdisplay, xroot, 0, 0, 1, 1, CopyFromParent, vi->depth, InputOutput, vi->visual, CWEventMask | CWColormap, &attr );
    XMapWindow ( xdisplay, xwindow );
    XSetWMProtocols ( xdisplay, xwindow, &WM_DELETE_WINDOW, 1 );

    context = glXCreateContext ( xdisplay, vi, 0, GL_TRUE );
    glXMakeCurrent ( xdisplay, xwindow, context );

    return MVideoInterface::init();
}

void XlibVideoInterface::fini()
{
    XUnmapWindow ( xdisplay, xwindow );
    XDestroyWindow ( xdisplay, xwindow );
}

bool XlibVideoInterface::setVideoMode ( int x, int y )
{
    screen_size = MSize ( x, y );
    XResizeWindow ( xdisplay, xwindow, x, y );
    glViewport ( 0, 0, screen_size.width(), screen_size.height() );
    glMatrixMode ( GL_PROJECTION );
    glLoadIdentity();
    glOrtho ( 0, screen_size.width(), screen_size.height(), 0, -1, 1 );
    glMatrixMode ( GL_MODELVIEW );
    return true;
}

MKey XlibVideoInterface::getKey ( unsigned int keycode )
{
    KeySym xsym = XKeycodeToKeysym ( xdisplay, keycode, 0 );
    if ( ( xsym & 0xff00 ) == 0xff00 )
        return keymap[xsym&0xff];
    else
        return MKey(xsym&0xff);
}

void XlibVideoInterface::keymap_init()
{
    for ( MKey& key: keymap )
        key = MKey::UNKNOWN;
    keymap[XK_BackSpace&0xFF] = MKey::BACKSPACE;
    keymap[XK_Tab&0xFF] = MKey::TAB;
    keymap[XK_Clear&0xFF] = MKey::CLEAR;
    keymap[XK_Return&0xFF] = MKey::RETURN;
    keymap[XK_Pause&0xFF] = MKey::PAUSE;
    keymap[XK_Escape&0xFF] = MKey::ESCAPE;
    keymap[XK_Delete&0xFF] = MKey::DELETE;

    keymap[XK_KP_0&0xFF] = MKey::KP0;           /* Keypad 0-9 */
    keymap[XK_KP_1&0xFF] = MKey::KP1;
    keymap[XK_KP_2&0xFF] = MKey::KP2;
    keymap[XK_KP_3&0xFF] = MKey::KP3;
    keymap[XK_KP_4&0xFF] = MKey::KP4;
    keymap[XK_KP_5&0xFF] = MKey::KP5;
    keymap[XK_KP_6&0xFF] = MKey::KP6;
    keymap[XK_KP_7&0xFF] = MKey::KP7;
    keymap[XK_KP_8&0xFF] = MKey::KP8;
    keymap[XK_KP_9&0xFF] = MKey::KP9;
    keymap[XK_KP_Insert&0xFF] = MKey::KP0;
    keymap[XK_KP_End&0xFF] = MKey::KP1; 
    keymap[XK_KP_Down&0xFF] = MKey::KP2;
    keymap[XK_KP_Page_Down&0xFF] = MKey::KP3;
    keymap[XK_KP_Left&0xFF] = MKey::KP4;
    keymap[XK_KP_Begin&0xFF] = MKey::KP5;
    keymap[XK_KP_Right&0xFF] = MKey::KP6;
    keymap[XK_KP_Home&0xFF] = MKey::KP7;
    keymap[XK_KP_Up&0xFF] = MKey::KP8;
    keymap[XK_KP_Page_Up&0xFF] = MKey::KP9;
    keymap[XK_KP_Delete&0xFF] = MKey::KP_PERIOD;
    keymap[XK_KP_Decimal&0xFF] = MKey::KP_PERIOD;
    keymap[XK_KP_Divide&0xFF] = MKey::KP_DIVIDE;
    keymap[XK_KP_Multiply&0xFF] = MKey::KP_MULTIPLY;
    keymap[XK_KP_Subtract&0xFF] = MKey::KP_MINUS;
    keymap[XK_KP_Add&0xFF] = MKey::KP_PLUS;
    keymap[XK_KP_Enter&0xFF] = MKey::KP_ENTER;
    keymap[XK_KP_Equal&0xFF] = MKey::KP_EQUALS;

    keymap[XK_Up&0xFF] = MKey::UP;
    keymap[XK_Down&0xFF] = MKey::DOWN;
    keymap[XK_Right&0xFF] = MKey::RIGHT;
    keymap[XK_Left&0xFF] = MKey::LEFT;
    keymap[XK_Insert&0xFF] = MKey::INSERT;
    keymap[XK_Home&0xFF] = MKey::HOME;
    keymap[XK_End&0xFF] = MKey::END;
    keymap[XK_Page_Up&0xFF] = MKey::PAGEUP;
    keymap[XK_Page_Down&0xFF] = MKey::PAGEDOWN;

    keymap[XK_F1&0xFF] = MKey::F1;
    keymap[XK_F2&0xFF] = MKey::F2;
    keymap[XK_F3&0xFF] = MKey::F3;
    keymap[XK_F4&0xFF] = MKey::F4;
    keymap[XK_F5&0xFF] = MKey::F5;
    keymap[XK_F6&0xFF] = MKey::F6;
    keymap[XK_F7&0xFF] = MKey::F7;
    keymap[XK_F8&0xFF] = MKey::F8;
    keymap[XK_F9&0xFF] = MKey::F9;
    keymap[XK_F10&0xFF] = MKey::F10;
    keymap[XK_F11&0xFF] = MKey::F11;
    keymap[XK_F12&0xFF] = MKey::F12;
    keymap[XK_F13&0xFF] = MKey::F13;
    keymap[XK_F14&0xFF] = MKey::F14;
    keymap[XK_F15&0xFF] = MKey::F15;

    keymap[XK_Num_Lock&0xFF] = MKey::NUMLOCK;
    keymap[XK_Caps_Lock&0xFF] = MKey::CAPSLOCK;
    keymap[XK_Scroll_Lock&0xFF] = MKey::SCROLLOCK;
    keymap[XK_Shift_R&0xFF] = MKey::RSHIFT;
    keymap[XK_Shift_L&0xFF] = MKey::LSHIFT;
    keymap[XK_Control_R&0xFF] = MKey::RCTRL;
    keymap[XK_Control_L&0xFF] = MKey::LCTRL;
    keymap[XK_Alt_R&0xFF] = MKey::RALT;
    keymap[XK_Alt_L&0xFF] = MKey::LALT;
    keymap[XK_Meta_R&0xFF] = MKey::RMETA;
    keymap[XK_Meta_L&0xFF] = MKey::LMETA;
    keymap[XK_Super_L&0xFF] = MKey::LSUPER; /* Left "Windows" */
    keymap[XK_Super_R&0xFF] = MKey::RSUPER; /* Right "Windows */
    keymap[XK_Mode_switch&0xFF] = MKey::MODE; /* "Alt Gr" key */
    keymap[XK_Multi_key&0xFF] = MKey::COMPOSE; /* Multi-key compose */

    keymap[XK_Help&0xFF] = MKey::HELP;
    keymap[XK_Print&0xFF] = MKey::PRINT;
    keymap[XK_Sys_Req&0xFF] = MKey::SYSREQ;
    keymap[XK_Break&0xFF] = MKey::BREAK;
    keymap[XK_Menu&0xFF] = MKey::MENU;
    keymap[XK_Hyper_R&0xFF] = MKey::MENU;   /* Windows "Menu" key */
}

M_EXPORT XlibVideoInterface interface;

