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
#include <GL/gl.h>
#include <GL/glext.h>
#include <filesystem>
#include <mdebug.h>
#include <mkeys.h>
#include <mfont.h>
#include <mimage.h>
#include <mvideointerface.h>
#include <mwindow.h>
#include <maudio.h>
#include <maudiofile.h>
#include <mplaylist.h>

namespace fs = std::filesystem;

static MFont* font = nullptr;
MTexture* tex = nullptr;


class Drawable {
public:
    virtual void draw() = 0;
};

#include <cstring>

#define STIRIINSESTDESET 64

struct Menu : public Drawable, public sigxx::object {
    uint16_t current = 0;
    Menu ( MWindow* w ) {
        SIGXX_CONNECT4(w, keyPressed, this, onKeyPress);
    }
    void onKeyPress ( MKey key, std::uint32_t mod ) {
                if ( key == M_KEY_UP ) {
                    if ( current == 0 )
                        current = items.size();
                    current--;
                }
                else if ( key == M_KEY_DOWN ) {
                    current++;
                    if ( current == items.size() )
                        current = 0;
                }
                else if ( key == M_KEY_RETURN )
                    activated ( current );
                render(font);
            }
    virtual void draw();
    std::vector< std::string > items;
    std::vector< MTexture* > textures;
    sigxx::signal<int> activated{this};
    void render(MFont* font) {
        font->setSize(20);
        for ( auto tex: textures )
            delete tex;
        textures.resize(items.size());
        for ( unsigned int i = 0; i < items.size(); i++ ) {
            std::string text = items[i];
            textures[i] = font->render(text);
        }
    }
};

class Listener : public sigxx::object {
    Menu* menu;
    void activated ( int z ) {
        menu->items.push_back ( "ACTIVATED: " + menu->items[z] );
        menu->render(font);
    }
public:
    Listener ( Menu* menu ) : menu{menu} {
        menu->activated.connect(this, &Listener::activated);
    }
};

class Text : public Drawable {
    void onChanged();
    virtual void draw();
    MTexture* texture = new MTexture;
public:
    Text();
    sigxx::slot<> changed{[this] { onChanged(); }};
    std::wstring text;
};

class Texture : public Drawable {
    virtual void draw();
};

void Menu::draw()
{
        int x = 100;
        int y = 100;
        int i = 0;
        for ( std::string item: items ) {
            if ( current == i )
                glColor4d(1,0,0,1);
            else
                glColor4d(0,1,1,1);
            textures[i]->draw(x, y, x + textures[i]->size().width(), y + textures[i]->size().height());
            i++;
            y += 20;
        }
}

Text::Text()
{
    text = L"Hello World!";
    changed();
}

void Text::onChanged()
{
    font->setSize(72);
    texture = font->render(text);
}

void Text::draw()
{
    glPushMatrix();
    glColor4d(0,0,1,1);
    glTranslated(50,20,0);
    texture->draw(0,0,texture->size().width(),texture->size().height());
    glPopMatrix();
}

MWindow* w;
void Texture::draw()
{
    MSize size = w->size;
    tex->draw(0,0,size.width(),size.height());
}

#include <chrono>
using namespace std::chrono;
Text* text;
int main ( int argc, char** argv ) {
    MLib::init ( argc, argv );
    auto i = MVideoInterface::get();
    w = i->createWindow(800,600,M_VIDEO_FLAGS_RESIZABLE);
    w->setTitle("Test");
    class EventHandler : public MEventHandler {
        virtual void quit() { MLib::quit(); }
        virtual void keyPressed ( MKey key, uint32_t mods )
        {
            if ( key == M_KEY_BACKSPACE && !text->text.empty() )
                text->text.pop_back();
            else if ( key == '\t' )
                text->text += L'»';
            else if ( m_key_to_wchar ( key ) )
                text->text += m_key_to_wchar ( key );
            text->changed();
        };
    };
    w->eventHandlers.push<EventHandler>();
    for ( auto f: fs::directory_iterator ( MLIB_DATA_DIR "images" ) ) {
        if ( f.path().filename().string().front() == '.' )
            continue;
        MDebug debug;
        debug << "Loading image " << f.path().filename() << " ... ";
        if ( MResource::load ( f.path().string() ) )
            debug << "done";
        else
            debug << "failed";
    }
    for ( auto f: fs::directory_iterator ( MLIB_DATA_DIR "fonts" ) ) {
        if ( f.path().filename().string().front() == '.' )
            continue;
        MDebug debug;
        debug << "Loading font " << f.path().filename() << " ... ";
        if ( MResource::load ( f.path().string() ) )
            debug << "done";
        else
            debug << "failed";
    }
    for ( auto f: fs::directory_iterator ( MLIB_DATA_DIR "sound" ) ) {
        if ( f.path().filename().string().front() == '.' )
            continue;
        MDebug debug;
        debug << "Loading sound " << f.path().filename() << " ... ";
        if ( MResource::load(f.path().string()) )
            debug << "done";
        else
            debug << "failed";
    }
    MAudio::setVolume(0.8);
    MAudioStream* audio = new MAudioStream{"/usr/share/games/supertuxkart/data/music/menutheme.ogg"};
    MPlaylist playlist;
    playlist.loop = true;
    playlist.insert("/usr/share/games/supertuxkart/data/sfx/goal_scored.ogg");
    playlist.stopAfter(0);
    playlist.insert(audio);
    playlist.insert("/usr/share/games/supertuxkart/data/music/egypt.ogg");
    playlist.playNext();
    auto img = MResource::get<MImage> ( MLIB_DATA_DIR "images/sample.png" );
    tex = img->createTexture();
    font = MResource::get<MFont> ( MLIB_DATA_DIR "fonts/DejaVuSans.ttf" );
    text = new Text;
    Menu* menu = new Menu{w};
    menu->items.push_back("ITEM1");
    menu->items.push_back("ITEM2");
    menu->items.push_back("ITEM3");
    menu->render(font);
    new Listener ( menu );
    std::list<Drawable*> drawables;
    drawables.push_back(menu);
    drawables.push_back(text);
    drawables.push_back(new Texture);
    for(;;) {
        i->handleEvents();
        w->beginPaint();
                auto start = system_clock::now();
        for ( Drawable* drawable: drawables ) {
            drawable->draw();
        }
                mDebug() << duration_cast<microseconds>(system_clock::now() - start).count();
        w->endPaint();
    }
}
