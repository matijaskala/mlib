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
#include <MTexture>
#include <MEvents>
#include <GL/gl.h>
#include <nonstd/filesystem>
#include <MDebug>
#include <MSignal>
#include <MVideo>
#include <MKeys>
#include <MFont>
#include <MImage>
#include <vector>

static std::string text = "Hello World!";
static MFont* font = nullptr;
MTexture* tex = nullptr;


class Drawable {
public:
    virtual void draw() = 0;
};

struct Menu : public Drawable {
    uint16_t current = 0;
    Menu (  ) {
        MEvents::keyPressed.push(
            [this] ( MKey key, std::uint32_t mod ) {
                if ( key == MKey::UP ) {
                    if ( current == 0 )
                        current = items.size();
                    current--;
                }
                else if ( key == MKey::DOWN ) {
                    current++;
                    if ( current == items.size() )
                        current = 0;
                }
                else if ( key == MKey::RETURN )
                    activated ( current );
            }
        );
    }
    virtual ~Menu() {
        MEvents::keyPressed.pop();
    }
    virtual void draw();
    std::vector< std::string > items;
    MSignal<int> activated{this};
    void render(MFont* font) {
        int x = 100;
        int y = 100;
        int i = 0;
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        for ( std::string item: items ) {
        glPushMatrix();
            if ( current == i )
                glColor4d(1,0,0,1);
            else
                glColor4d(0,1,1,1);
            glTranslated(x,y,0);
            glRotated(180,1,0,0);
            font->setFaceSize ( 20 );
            font->render ( item.c_str() );
            i++;
            y += 20;
        glPopMatrix();
        }
        glPopAttrib();
    }
};

class Listener {
    Menu* menu;
    void activated ( int z ) {
        menu->items.push_back ( "ACTIVATED: " + menu->items[z] );
    }
    MSlot<int> slotActivated{&Listener::activated, this};
public:
    Listener ( Menu* menu ) : menu{menu} {
#undef connect
        menu->activated.connect(slotActivated);
    }
};

class Text : public Drawable {
    virtual void draw();
};

class Texture : public Drawable {
    virtual void draw();
};

void Menu::draw()
{
    render(font);
}

void Text::draw()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();
    glColor4d(0,0,1,1);
    glTranslated(0,72,0);
    glRotated(180,1,0,0);
    font->setFaceSize(72);
    font->render(text);
    glPopMatrix();
    glPopAttrib();
}

void Texture::draw()
{
    MSize size = MVideo::screenSize();
    tex->draw(0,0,size.width(),size.height());
}

int main ( int argc, char** argv ) {
    M::init ( argc, argv );
    MVideo::init();
    MEvents::keyPressed.push(
        [] ( MKey key, std::uint32_t mod )
        {
            if ( key == MKey::BACKSPACE && !text.empty() )
                text.pop_back();
            else if ( static_cast<int> ( key ) >= 32 && static_cast<int> ( key ) < 256 )
                text += static_cast<char> ( key );
        }
    );
    for ( non_std::file f: non_std::directory ( DATADIR "images" ) ) {
        if ( f.name[0] == '.' )
            continue;
        MDebug debug;
        debug << "Loading image " << f.name << " ... ";
        if ( MImage::load ( f.path + "/" + f.name ) )
            debug << "done";
        else
            debug << "failed";
    }
    for ( non_std::file f: non_std::directory ( DATADIR "fonts" ) ) {
        if ( f.name[0] == '.' )
            continue;
        MDebug debug;
        debug << "Loading font " << f.name << " ... ";
        if ( MFont::load ( f.path + "/" + f.name ) )
            debug << "done";
        else
            debug << "failed";
    }
    auto img = MImage::get ( DATADIR "images/sample.png" );
    tex = img->createTexture();
    font = MFont::get ( DATADIR "fonts/DejaVuSans.ttf" );
    MVideo::setVideoMode(200,200);
    Menu* menu = new Menu;
    menu->items.push_back("ITEM1");
    menu->items.push_back("ITEM2");
    menu->items.push_back("ITEM3");
    new Listener ( menu );
    std::list<Drawable*> drawables;
    drawables.push_back(menu);
    drawables.push_back(new Text);
    drawables.push_back(new Texture);
    for(;;) {
        MVideo::handleEvents();
        MVideo::beginPaint();
        for ( Drawable* drawable: drawables )
            drawable->draw();
        MVideo::endPaint();
    }
}
