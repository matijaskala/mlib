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

#include <mvideointerface.h>
#include <mtexture.h>
#include <FTGL/ftgl.h>
#include <GL/gl.h>

int main() {
    FTGLPixmapFont font ( DATADIR "fonts/DejaVuSans.ttf" );
    if ( font.Error() )
        return 1;
    const MTexture* tex;
    MVideoInterface* video = MPlugin::load<MVideoInterface> ( "msdl" );
    video->init();
    if ( !MTexture::load ( DATADIR "images/sample.png" ) )
        return 1;
    tex = MTexture::get ( DATADIR "images/sample.png" );
    video->setVideoMode(200,200);
    for(;;) {
        video->handleEvents();
        video->beginPaint();
        MSize size = video->screen_size;
        tex->draw(0,0,size.width(),size.height());
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glColor4d(0,0,1,1);
        glRasterPos2i(72,72);
        font.FaceSize(72);
        font.Render("Hello World!");
        glPopAttrib();
        video->endPaint();
    }
}
