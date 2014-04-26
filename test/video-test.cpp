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

int main() {
    const MTexture* tex;
    MVideoInterface* video = MPlugin::load<MVideoInterface> ( "../libmsdl" );
    video->init();
    if ( !MTexture::load ( "../../../AI/data/images/apple-gray.png" ) )
        return 1;
    tex = MTexture::get ( "../../../AI/data/images/apple-gray.png" );
    video->setVideoMode(200,200);
    for(;;) {
        video->handleEvents();
        video->beginPaint();
        tex->draw(0,0,200,200);
        video->endPaint();
    }
}