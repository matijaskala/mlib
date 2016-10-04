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

#include "maudiofile.h"

#include <thread>
#include <al.h>

namespace al {
    enum format {
        MONO8 = AL_FORMAT_MONO8,
        MONO16 = AL_FORMAT_MONO16,
        STEREO8 = AL_FORMAT_STEREO8,
        STEREO16 = AL_FORMAT_STEREO16,
    };
}

using namespace std;
using namespace this_thread;
using namespace al;

void MAudioFile::m_playSync (bool stereo, int freq, Buffer buffer)
{
    al::format format = stereo ? STEREO16 : MONO16;
    unsigned int source;
    ALuint buf;
    int state;

    alGenSources(1, &source);
    alGenBuffers(1, &buf);

    alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSourcef(source, AL_ROLLOFF_FACTOR, 0 );

    alSourcei(source, AL_BUFFER, 0);

    alBufferData(buf, format, buffer->data(), buffer->size(), freq);
    alSourceQueueBuffers(source, 1, &buf);

    alSourcePlay(source);

    do {
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        sleep_for(0.2s);
    } while (state != AL_STOPPED);

    alDeleteBuffers(1, &buf);
    alDeleteSources(1, &source);
}

void MAudioFile::playSync ()
{
    m_playSync(stereo, freq, buffer);
}

void MAudioFile::play ()
{
    thread{m_playSync, stereo, freq, buffer}.detach();
}
