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
#include "mdebug.h"
#include "meventhandler.h"
#include "msize.h"
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

class SDLVideoInterface : public MVideoInterface
{
    virtual ~SDLVideoInterface() override;
    virtual void beginPaint() override;
    virtual void endPaint() override;
    virtual void handleEvents() override;
    virtual bool init() override;
    virtual bool setVideoMode ( int x, int y ) override;
    virtual MKey getKey ( uint64_t keysym ) override;
};

void SDLVideoInterface::handleEvents()
{
    SDL_Event ev;
    MEventHandler* handler = MEventHandler::current();
    while ( SDL_PollEvent ( &ev ) )
        switch ( ev.type ) {
            case SDL_QUIT:
                handler->quit();
                break;
            case SDL_VIDEORESIZE:
                setVideoMode(ev.resize.w,ev.resize.h);
                break;
            case SDL_KEYDOWN:
                handler->key_pressed(ev.key.keysym.sym << 16 | ev.key.keysym.mod);
                break;
            case SDL_KEYUP:
                handler->key_released(ev.key.keysym.sym << 16 | ev.key.keysym.mod);
                break;
        }
}

SDLVideoInterface::~SDLVideoInterface()
{
    SDL_Quit();
}

void SDLVideoInterface::beginPaint()
{
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void SDLVideoInterface::endPaint()
{
    SDL_GL_SwapBuffers();
}

bool SDLVideoInterface::init()
{
    if ( SDL_Init ( SDL_INIT_VIDEO ) < 0 ) {
        mDebug ( ERROR ) << SDL_GetError();
        return false;
    }

    if ( SDL_GL_SetAttribute ( SDL_GL_DOUBLEBUFFER, 1 ) < 0 ) {
        mDebug ( ERROR ) << SDL_GetError();
    }
    SDL_SetVideoMode ( 1, 1, 0, SDL_OPENGL );
    return true;
}

bool SDLVideoInterface::setVideoMode ( int x, int y )
{
    SDL_Surface* screen = SDL_SetVideoMode ( x, y, 32, SDL_OPENGL | SDL_RESIZABLE );
    if ( !screen ) {
        mDebug ( ERROR ) << SDL_GetError();
        return false;
    }

    SDL_Event ev;
    while ( SDL_PollEvent ( &ev ) );

    screen_size = { screen->w, screen->h };

    glViewport ( 0, 0, screen_size.width(), screen_size.height() );
    glMatrixMode ( GL_PROJECTION );
    glLoadIdentity();
    glOrtho ( 0, screen_size.width(), screen_size.height(), 0, -1, 1 );
    glMatrixMode ( GL_MODELVIEW );
    glEnable(GL_BLEND);
    glEnable ( GL_TEXTURE_2D );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

MKey SDLVideoInterface::getKey ( uint64_t keysym )
{
    return MKey ( keysym >> 16 );
}

M_PLUGIN_EXPORT ( MVideoInterface, SDLVideoInterface )


