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

#include "mmusic.h"

#include <al.h>

using namespace std;

static volatile bool m_paused{};
static volatile bool m_stop{};
static thread m_thread;

void MMusic::play_sync ( MAudioStream* stream )
{
    stream->initRead();

    ALenum format = stream->stereo() ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
    ALsizei n_buffers = 8;
    ALint state;
    ALuint source;
    ALuint buffers[n_buffers];

    alGenSources(1,&source);
    alGenBuffers(n_buffers, buffers);

    alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSourcef(source, AL_ROLLOFF_FACTOR, 0 );

    alSourcei(source, AL_BUFFER, 0);

    for ( ALuint buffer: buffers ) {
        stream->waitRead();
        alBufferData(buffer, format, stream->buffer, stream->buffer_size, stream->freq());
        stream->initRead();
    }
    alSourceQueueBuffers(source, n_buffers, buffers);

    if (!paused())
        alSourcePlay(source);

    while (!stream->eof()) {
        ALint processed;
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
        ALuint buffers[processed];
        alSourceUnqueueBuffers(source, processed, buffers);

        for ( ALuint buffer: buffers ) {
            stream->waitRead();
            alBufferData(buffer, format, stream->buffer, stream->buffer_size, stream->freq());
            if (stream->eof())
                break;
            stream->initRead();
        }
        alSourceQueueBuffers(source, processed, buffers);

        alGetSourcei(source, AL_SOURCE_STATE, &state);

        if (m_stop) {
            if (state != AL_STOPPED) {
                alSourceStop(source);
                alGetSourcei(source, AL_SOURCE_STATE, &state);
            }
            stream->waitRead();
            break;
        }

        if (state == AL_STOPPED)
            alSourcePlay(source);

        if (paused() && state != AL_PAUSED)
            alSourcePause(source);

        if (!paused() && state == AL_PAUSED)
            alSourcePlay(source);

        if (stream->eof())
            break;

        this_thread::sleep_for(0.2s);
    }

    while (state != AL_STOPPED) {
        if (m_stop) {
            alSourceStop(source);
            break;
        }

        alGetSourcei(source, AL_SOURCE_STATE, &state);

        if (paused() && state != AL_PAUSED)
            alSourcePause(source);

        if (!paused() && state == AL_PAUSED)
            alSourcePlay(source);

        this_thread::sleep_for(0.2s);
    }

    alDeleteBuffers(n_buffers, buffers);
    alDeleteSources(1, &source);
}

void MMusic::play ( MAudioStream* stream )
{
    m_thread = thread{play_sync,stream};
}

void MMusic::stop()
{
    m_stop = true;
    m_thread.join();
    m_stop = false;
    m_thread = thread{};
}

void MMusic::pause()
{
    m_paused = true;
}

void MMusic::resume()
{
    m_paused = false;
}

bool MMusic::paused()
{
    return m_paused;
}
