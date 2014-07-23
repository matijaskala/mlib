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
#include <meventhandler.h>
#include <FTGL/ftgl.h>
#include <GL/gl.h>

class EventHandler : public MEventHandler {
    virtual void quit();
    virtual void key_pressed ( uint64_t keysym );
    virtual void key_released ( uint64_t keysym );
};

static std::string text = "Hello World!";

void EventHandler::quit()
{
    exit(0);
}

MVideoInterface* video;
void EventHandler::key_pressed ( uint64_t keysym )
{
    MKey key = video->getKey(keysym);
    if ( key == MKey::BACKSPACE && !text.empty() )
        text.pop_back();
    else if ( static_cast<int> ( key ) >= 32 && static_cast<int> ( key ) < 256 )
        text += static_cast<char> ( key );
}

void EventHandler::key_released ( uint64_t keysym )
{
    MKey key = video->getKey(keysym);
}

int main() {
    FTGLPixmapFont font ( DATADIR "fonts/DejaVuSans.ttf" );
    if ( font.Error() )
        return 1;
    MEventHandler::handlers.push ( EventHandler() );
    const MTexture* tex;
    video = MPlugin::load<MVideoInterface> ( "msdl" );
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
        font.Render(text.c_str());
        glPopAttrib();
        video->endPaint();
    }
}
