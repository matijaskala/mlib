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

#include "mwindow.h"

#include <GL/gl.h>
#include <thread>

using namespace std;

MWindow::MWindow ( int width, int height ) : size{width,height}
{
    quit.connect(eventHandlers.quit);
    keyPressed.connect(eventHandlers.keyPressed);
    keyReleased.connect(eventHandlers.keyReleased);

    glViewport ( 0, 0, width, height );
    glMatrixMode ( GL_PROJECTION );
    glLoadIdentity();
    glOrtho ( 0, width, height, 0, -1, 1 );
    glMatrixMode ( GL_MODELVIEW );
}

void MWindow::beginPaint()
{
    makeCurrent();

    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void MWindow::endPaint()
{
    glFlush();
    this_thread::sleep_for ( 20us );

    swapBuffers();
}

void MWindow::resize ( int width, int height )
{
    if ( size == MSize{width,height} )
        return;
    size = {width,height};
    resize();

    glViewport ( 0, 0, width, height );
    glMatrixMode ( GL_PROJECTION );
    glLoadIdentity();
    glOrtho ( 0, width, height, 0, -1, 1 );
    glMatrixMode ( GL_MODELVIEW );
}
