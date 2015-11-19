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

#include <array>
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
using namespace al;

static volatile bool m_pause{};
static volatile bool m_stop{};
static thread m_thread;
static unsigned int source{};

void MMusic::play_sync ( MAudioStream* stream )
{
    stream->initRead();

    al::format format = stream->stereo() ? STEREO16 : MONO16;
    constexpr size_t n_buffers = 8;
    int state;
    array<unsigned int,n_buffers> buffers;

    alGenSources(1, &source);
    alGenBuffers(n_buffers, buffers.data());

    alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSourcef(source, AL_ROLLOFF_FACTOR, 0 );

    alSourcei(source, AL_BUFFER, 0);

    for ( ALuint buffer: buffers ) {
        stream->waitRead();
        alBufferData(buffer, format, stream->buffer, stream->buffer_size, stream->freq());
        stream->initRead();
    }
    alSourceQueueBuffers(source, n_buffers, buffers.data());

    if (!m_pause)
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

        if (m_pause && state == AL_PLAYING)
            alSourcePause(source);

        if (!m_pause && state != AL_PLAYING)
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

        if (m_pause && state == AL_PLAYING)
            alSourcePause(source);

        if (!m_pause && state != AL_PLAYING)
            alSourcePlay(source);

        this_thread::sleep_for(0.2s);
    }

    alDeleteBuffers(n_buffers, buffers.data());
    alDeleteSources(1, &source);
    source = 0;
}

void MMusic::play ( MAudioStream* stream )
{
    if (source)
        stop();
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
    m_pause = true;
}

void MMusic::resume()
{
    m_pause = false;
}

bool MMusic::playing()
{
    int state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}
