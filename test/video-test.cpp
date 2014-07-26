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
#include <mtexture.h>
#include <meventhandler.h>
#include <FTGL/ftgl.h>
#include <GL/gl.h>
#include <nonstd/filesystem>
#include <mdebug.h>
#include <mobject.h>
#include <mvideo.h>
#include <mkeys.h>
#include <vector>

class EventHandler : public MEventHandler {
    virtual void quit();
    virtual void key_pressed ( MKey key, std::uint32_t mod );
    virtual void key_released ( MKey key, std::uint32_t mod );
};

static std::string text = "Hello World!";

void EventHandler::quit()
{
    exit(0);
}

void EventHandler::key_pressed ( MKey key, std::uint32_t mod )
{
    if ( key == MKey::BACKSPACE && !text.empty() )
        text.pop_back();
    else if ( static_cast<int> ( key ) >= 32 && static_cast<int> ( key ) < 256 )
        text += static_cast<char> ( key );
}

void EventHandler::key_released ( MKey key, std::uint32_t mod )
{
    (void) key;
}

struct Menu : public MObject {
    uint16_t current = 0;
    struct EventHandler : public MEventHandler {
        Menu* menu;
        EventHandler ( Menu* menu ) : menu ( menu ) {}
        virtual void key_pressed ( MKey key, std::uint32_t mod ) override {
            if ( key == MKey::UP ) {
                if ( menu->current == 0 )
                    menu->current = menu->items.size();
                menu->current--;
            }
            else if ( key == MKey::DOWN ) {
                menu->current++;
                if ( menu->current == menu->items.size() )
                    menu->current = 0;
            }
            else if ( key == MKey::RETURN )
                menu->confirmed();
        }
        virtual void quit() {
            exit(0);
        }
    };
    Menu ( MObject* parent = nullptr ) : MObject(parent) {
        MEventHandler::handlers.push<EventHandler> ( this );
    }
    virtual ~Menu() {
        MEventHandler::handlers.pop();
    }
    void confirmed() {
        emit ( activated, current );
    }
    std::vector< std::string > items;
    Signal<int> activated;
    void render(FTGLPixmapFont& font) {
        int x = 100;
        int y = 100;
        int i = 0;
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        for ( std::string item: items ) {
            if ( current == i )
                glColor4d(1,0,0,1);
            else
                glColor4d(0,1,1,1);
            glRasterPos2i(x,y);
            font.FaceSize ( 20 );
            font.Render ( item.c_str() );
            i++;
            y += 20;
        }
        glPopAttrib();
    }
};

class Listener : public MObject {
    void activated(MObject* sender, int z) {
        Menu* menu = dynamic_cast<Menu*> ( sender );
        menu->items.push_back ( "ACTIVATED: " + menu->items[z] );
    }
public:
    Listener ( MObject* parent = nullptr ) : MObject(parent) {
        Menu* menu = dynamic_cast<Menu*> ( parent );
        connect(menu, activated, this, activated);
    }
};

int main ( int argc, char** argv ) {
    M::init ( argc, argv );
    FTGLPixmapFont font ( DATADIR "fonts/DejaVuSans.ttf" );
    if ( font.Error() )
        return 1;
    MEventHandler::handlers.push< EventHandler > ();
    const MTexture* tex;
    MVideo::init();
    for ( non_std::file f: non_std::directory ( DATADIR "images" ) ) {
        if ( f.name[0] == '.' )
            continue;
        MDebug debug;
        debug << "Loading image " << f.name << " ... ";
        if ( MTexture::load ( f.path + "/" + f.name ) )
            debug << "done";
        else
            debug << "failed";
    }
    tex = MTexture::get ( DATADIR "images/sample.png" );
    MVideo::setVideoMode(200,200);
    Menu* menu = new Menu;
    menu->items.push_back("ITEM1");
    menu->items.push_back("ITEM2");
    menu->items.push_back("ITEM3");
    new Listener ( menu );
    for(;;) {
        MVideo::handleEvents();
        MVideo::beginPaint();
        MSize size = MVideo::screenSize();
        tex->draw(0,0,size.width(),size.height());
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glColor4d(0,0,1,1);
        glRasterPos2i(72,72);
        font.FaceSize(72);
        font.Render(text.c_str());
        glPopAttrib();
        menu->render(font);
        MVideo::endPaint();
    }
}
