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

#include <MVideoInterface>
#include <MDebug>
#include <MSize>
#include <mwindow.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

class SDLWindow : public MWindow
{
public:
    SDLWindow ( int width, int height );
    virtual void makeCurrent() {}
    virtual void resize ();
    virtual void swapBuffers();
};

static MWindow* win;
SDLWindow::SDLWindow ( int width, int height ) : MWindow{width,height}
{
    SDL_Event ev;
    while ( SDL_PollEvent ( &ev ) );
}

void SDLWindow::resize ()
{
    SDL_SetVideoMode ( size.width(), size.height(), 32, SDL_OPENGL );
}

void SDLWindow::swapBuffers()
{
    SDL_GL_SwapBuffers();
}

class SDLVideoInterface : public MVideoInterface
{
    virtual void handleEvents() override;
    virtual bool init() override;
    virtual void fini() override;
    virtual MWindow* createWindow ( int width, int height );
    virtual void destroyWindow ( MWindow* window );
};

void SDLVideoInterface::handleEvents()
{
    SDL_Event ev;
    while ( SDL_PollEvent ( &ev ) )
        switch ( ev.type ) {
            case SDL_QUIT:
                win->quit();
                break;
            case SDL_VIDEORESIZE:
                win->resize(ev.resize.w,ev.resize.h);
                break;
            case SDL_KEYDOWN:
                win->keyPressed ( static_cast<MKey> ( ev.key.keysym.sym ), ev.key.keysym.mod );
                break;
            case SDL_KEYUP:
                win->keyReleased ( static_cast<MKey> ( ev.key.keysym.sym ), ev.key.keysym.mod );
                break;
        }
}

bool SDLVideoInterface::init()
{
    return false;

    if ( SDL_Init ( SDL_INIT_VIDEO ) < 0 ) {
        mDebug ( ERROR ) << SDL_GetError();
        return false;
    }

    if ( SDL_GL_SetAttribute ( SDL_GL_DOUBLEBUFFER, 1 ) < 0 ) {
        mDebug ( ERROR ) << SDL_GetError();
        return false;
    }

    if ( SDL_SetVideoMode ( 1, 1, 0, SDL_OPENGL ) == nullptr ) {
        mDebug ( ERROR ) << SDL_GetError();
        return false;
    }

    return true;
}

void SDLVideoInterface::fini()
{
    SDL_Quit();
}

MWindow* SDLVideoInterface::createWindow ( int width, int height )
{
    SDL_Surface* screen = SDL_SetVideoMode ( width, height, 32, SDL_OPENGL );
    if ( !screen ) {
        mDebug ( ERROR ) << SDL_GetError();
        return nullptr;
    }

    auto w = new SDLWindow{width,height};
    win = w;
    return w;
}

void SDLVideoInterface::destroyWindow ( MWindow* window )
{

}

static SDLVideoInterface iface;


