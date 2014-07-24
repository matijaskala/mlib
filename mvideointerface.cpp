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

#include <GL/gl.h>
#include <unistd.h>

void MVideoInterface::beginPaint()
{
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void MVideoInterface::endPaint()
{
    glFlush();
    usleep ( 20 );
}

bool MVideoInterface::init()
{
    glEnable ( GL_BLEND );
    glEnable ( GL_TEXTURE_2D );
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    return true;
}

void MVideoInterface::test ()
{
        glBegin(GL_QUADS);
        struct { int x = 200; int y = 200; } m_rect;
        int w = 80, h = 80;
        glTexCoord2d(0, 0);
        glVertex2d(m_rect.x-w, m_rect.y-h);
        glTexCoord2d(0, 1);
        glVertex2d(m_rect.x-w, m_rect.y+h);
        glTexCoord2d(1, 1);
        glVertex2d(m_rect.x+w, m_rect.y+h);
        glTexCoord2d(1, 0);
        glVertex2d(m_rect.x+w, m_rect.y-h);
        glEnd();
        glFlush();
}
