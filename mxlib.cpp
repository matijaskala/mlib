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
#include <GL/glx.h>
#include <unistd.h>

class XlibVideoInterface : public MVideoInterface
{
    virtual ~XlibVideoInterface() override;
    virtual void beginPaint() override;
    virtual void endPaint() override;
    virtual void handleEvents() override;
    virtual bool init() override;
    virtual bool setVideoMode ( int x, int y ) override;
    Display* xdisplay;
    Window xwindow;
    GLXContext context;
};

static Atom WM_DELETE_WINDOW;

XlibVideoInterface::~XlibVideoInterface()
{
    XUnmapWindow ( xdisplay, xwindow );
    XDestroyWindow ( xdisplay, xwindow );
}

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
    MEventHandler* handler = MEventHandler::current();
    while ( XPending ( xdisplay ) ) {
        XEvent ev;
        XNextEvent ( xdisplay, &ev );
        switch ( ev.type ) {
            case Expose:
                break;
            case ClientMessage:
                if ( ev.xclient.data.l[0] == WM_DELETE_WINDOW ) {
                    handler->quit();
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
        }
    }
}

bool XlibVideoInterface::init()
{
    xdisplay = XOpenDisplay ( nullptr );
    if ( !xdisplay )
        return false;

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
    attr.event_mask = StructureNotifyMask | ExposureMask;
    xwindow = XCreateWindow ( xdisplay, xroot, 0, 0, 1, 1, CopyFromParent, vi->depth, InputOutput, vi->visual, CWEventMask | CWColormap, &attr );
    XMapWindow ( xdisplay, xwindow );
    XSetWMProtocols ( xdisplay, xwindow, &WM_DELETE_WINDOW, 1 );

    context = glXCreateContext ( xdisplay, vi, 0, GL_TRUE );
    glXMakeCurrent ( xdisplay, xwindow, context );

    glEnable ( GL_BLEND );
    glEnable ( GL_TEXTURE_2D );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
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

M_PLUGIN_EXPORT ( MVideoInterface, XlibVideoInterface )

