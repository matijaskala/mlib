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
#include <MEvents>
#include <MKeys>
#include <GL/glx.h>
#include <unistd.h>

class XlibVideoInterface;
class XlibWindow : public MWindow
{
    XlibVideoInterface* iface;
public:
    XlibWindow ( int width, int height, Window window, XlibVideoInterface* iface );
    virtual void flush();
    virtual void resize();
    Window xwindow;
};

class XlibVideoInterface : public MVideoInterface
{
public:
    std::list<XlibWindow*> window_list;
    virtual void handleEvents() override;
    virtual bool init() override;
    virtual void fini() override;
    virtual MWindow* createWindow ( int width, int height );
    virtual void destroyWindow ( MWindow* window );
    MKey getKey ( unsigned int keycode );
    Display* xdisplay;
    GLXContext context;
    MKey keymap[0x100];
    void keymap_init();
};

XlibWindow::XlibWindow ( int width, int height, Window window, XlibVideoInterface* iface ) : MWindow ( width, height )
{
    xwindow = window;
    this->iface = iface;
}

void XlibWindow::flush()
{

}

void XlibWindow::resize()
{
    XSizeHints* sizehints = XAllocSizeHints();
    sizehints->min_width = sizehints->max_width = size.width();
    sizehints->min_height = sizehints->max_height = size.height();
    sizehints->flags = PMinSize | PMaxSize;
    XSetWMNormalHints ( iface->xdisplay, xwindow, sizehints );
    XFree(sizehints);
}

static Atom WM_DELETE_WINDOW;

void XlibVideoInterface::handleEvents()
{
    while ( XPending ( xdisplay ) ) {
        XEvent ev;
        XNextEvent ( xdisplay, &ev );
        MWindow* win = nullptr;
        for ( XlibWindow* w: window_list )
            if ( w->xwindow == ev.xany.window )
                win = w;
        switch ( ev.type ) {
            case Expose:
                break;
            case ClientMessage:
                if ( ev.xclient.data.l[0] == WM_DELETE_WINDOW ) {
                    win->quit();
                }
                break;
            case ConfigureNotify:
                win->resize ( ev.xconfigure.width, ev.xconfigure.height );
                break;
            case KeyPress:
                win->keyPressed ( getKey ( ev.xkey.keycode ), /*TODO*/0 );
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
                win->keyReleased ( getKey ( ev.xkey.keycode ), /*TODO*/0 );
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

    return true;
}

void XlibVideoInterface::fini()
{
    XCloseDisplay(xdisplay);
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
        key = M_KEY_UNKNOWN;
    keymap[XK_BackSpace&0xFF] = M_KEY_BACKSPACE;
    keymap[XK_Tab&0xFF] = M_KEY_TAB;
    keymap[XK_Clear&0xFF] = M_KEY_CLEAR;
    keymap[XK_Return&0xFF] = M_KEY_RETURN;
    keymap[XK_Pause&0xFF] = M_KEY_PAUSE;
    keymap[XK_Escape&0xFF] = M_KEY_ESCAPE;
    keymap[XK_Delete&0xFF] = M_KEY_DELETE;

    keymap[XK_KP_0&0xFF] = M_KEY_KP0;           /* Keypad 0-9 */
    keymap[XK_KP_1&0xFF] = M_KEY_KP1;
    keymap[XK_KP_2&0xFF] = M_KEY_KP2;
    keymap[XK_KP_3&0xFF] = M_KEY_KP3;
    keymap[XK_KP_4&0xFF] = M_KEY_KP4;
    keymap[XK_KP_5&0xFF] = M_KEY_KP5;
    keymap[XK_KP_6&0xFF] = M_KEY_KP6;
    keymap[XK_KP_7&0xFF] = M_KEY_KP7;
    keymap[XK_KP_8&0xFF] = M_KEY_KP8;
    keymap[XK_KP_9&0xFF] = M_KEY_KP9;
    keymap[XK_KP_Insert&0xFF] = M_KEY_KP0;
    keymap[XK_KP_End&0xFF] = M_KEY_KP1; 
    keymap[XK_KP_Down&0xFF] = M_KEY_KP2;
    keymap[XK_KP_Page_Down&0xFF] = M_KEY_KP3;
    keymap[XK_KP_Left&0xFF] = M_KEY_KP4;
    keymap[XK_KP_Begin&0xFF] = M_KEY_KP5;
    keymap[XK_KP_Right&0xFF] = M_KEY_KP6;
    keymap[XK_KP_Home&0xFF] = M_KEY_KP7;
    keymap[XK_KP_Up&0xFF] = M_KEY_KP8;
    keymap[XK_KP_Page_Up&0xFF] = M_KEY_KP9;
    keymap[XK_KP_Delete&0xFF] = M_KEY_KP_PERIOD;
    keymap[XK_KP_Decimal&0xFF] = M_KEY_KP_PERIOD;
    keymap[XK_KP_Divide&0xFF] = M_KEY_KP_DIVIDE;
    keymap[XK_KP_Multiply&0xFF] = M_KEY_KP_MULTIPLY;
    keymap[XK_KP_Subtract&0xFF] = M_KEY_KP_MINUS;
    keymap[XK_KP_Add&0xFF] = M_KEY_KP_PLUS;
    keymap[XK_KP_Enter&0xFF] = M_KEY_KP_ENTER;
    keymap[XK_KP_Equal&0xFF] = M_KEY_KP_EQUALS;

    keymap[XK_Up&0xFF] = M_KEY_UP;
    keymap[XK_Down&0xFF] = M_KEY_DOWN;
    keymap[XK_Right&0xFF] = M_KEY_RIGHT;
    keymap[XK_Left&0xFF] = M_KEY_LEFT;
    keymap[XK_Insert&0xFF] = M_KEY_INSERT;
    keymap[XK_Home&0xFF] = M_KEY_HOME;
    keymap[XK_End&0xFF] = M_KEY_END;
    keymap[XK_Page_Up&0xFF] = M_KEY_PAGEUP;
    keymap[XK_Page_Down&0xFF] = M_KEY_PAGEDOWN;

    keymap[XK_F1&0xFF] = M_KEY_F1;
    keymap[XK_F2&0xFF] = M_KEY_F2;
    keymap[XK_F3&0xFF] = M_KEY_F3;
    keymap[XK_F4&0xFF] = M_KEY_F4;
    keymap[XK_F5&0xFF] = M_KEY_F5;
    keymap[XK_F6&0xFF] = M_KEY_F6;
    keymap[XK_F7&0xFF] = M_KEY_F7;
    keymap[XK_F8&0xFF] = M_KEY_F8;
    keymap[XK_F9&0xFF] = M_KEY_F9;
    keymap[XK_F10&0xFF] = M_KEY_F10;
    keymap[XK_F11&0xFF] = M_KEY_F11;
    keymap[XK_F12&0xFF] = M_KEY_F12;
    keymap[XK_F13&0xFF] = M_KEY_F13;
    keymap[XK_F14&0xFF] = M_KEY_F14;
    keymap[XK_F15&0xFF] = M_KEY_F15;

    keymap[XK_Num_Lock&0xFF] = M_KEY_NUMLOCK;
    keymap[XK_Caps_Lock&0xFF] = M_KEY_CAPSLOCK;
    keymap[XK_Scroll_Lock&0xFF] = M_KEY_SCROLLOCK;
    keymap[XK_Shift_R&0xFF] = M_KEY_RSHIFT;
    keymap[XK_Shift_L&0xFF] = M_KEY_LSHIFT;
    keymap[XK_Control_R&0xFF] = M_KEY_RCTRL;
    keymap[XK_Control_L&0xFF] = M_KEY_LCTRL;
    keymap[XK_Alt_R&0xFF] = M_KEY_RALT;
    keymap[XK_Alt_L&0xFF] = M_KEY_LALT;
    keymap[XK_Meta_R&0xFF] = M_KEY_RMETA;
    keymap[XK_Meta_L&0xFF] = M_KEY_LMETA;
    keymap[XK_Super_L&0xFF] = M_KEY_LSUPER; /* Left "Windows" */
    keymap[XK_Super_R&0xFF] = M_KEY_RSUPER; /* Right "Windows */
    keymap[XK_Mode_switch&0xFF] = M_KEY_MODE; /* "Alt Gr" key */
    keymap[XK_Multi_key&0xFF] = M_KEY_COMPOSE; /* Multi-key compose */

    keymap[XK_Help&0xFF] = M_KEY_HELP;
    keymap[XK_Print&0xFF] = M_KEY_PRINT;
    keymap[XK_Sys_Req&0xFF] = M_KEY_SYSREQ;
    keymap[XK_Break&0xFF] = M_KEY_BREAK;
    keymap[XK_Menu&0xFF] = M_KEY_MENU;
    keymap[XK_Hyper_R&0xFF] = M_KEY_MENU;   /* Windows "Menu" key */
}

MWindow* XlibVideoInterface::createWindow ( int width, int height )
{
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
    auto xwindow = XCreateWindow ( xdisplay, xroot, 0, 0, width, height, CopyFromParent, vi->depth, InputOutput, vi->visual, CWEventMask | CWColormap, &attr );
    XMapWindow ( xdisplay, xwindow );
    XSetWMProtocols ( xdisplay, xwindow, &WM_DELETE_WINDOW, 1 );

    context = glXCreateContext ( xdisplay, vi, 0, GL_TRUE );
    glXMakeCurrent ( xdisplay, xwindow, context );

    XSizeHints* sizehints = XAllocSizeHints();
    sizehints->min_width = sizehints->max_width = width;
    sizehints->min_height = sizehints->max_height = height;
    sizehints->flags = PMinSize | PMaxSize;
    XSetWMNormalHints ( xdisplay, xwindow, sizehints );
    XFree(sizehints);

    auto w = new XlibWindow{width,height,xwindow,this};
    window_list.push_back(w);
    return w;
}

void XlibVideoInterface::destroyWindow ( MWindow* window )
{
    auto w = dynamic_cast<XlibWindow*> ( window );
    if ( !w )
        return;
    window_list.remove(w);
    XUnmapWindow ( xdisplay, w->xwindow );
    XDestroyWindow ( xdisplay, w->xwindow );
}

static XlibVideoInterface interface;

