/*
 * This file is part of MLib
 * Copyright (C) 2016  Matija Skala <mskala@gmx.com>
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

#include "mplaylist.h"

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
using namespace sigxx;
using namespace chrono;
using namespace this_thread;
using namespace al;

static slot<bool> slotFinished = [] ( bool stop ) {
    auto playlist = slotFinished.userdata<MPlaylist>();
    if ( playlist->stoppingAfter() != 0 && stop == false)
        playlist->playNext();
};

MPlaylist::MPlaylist ()
{
    finished.connect(slotFinished);
}

MPlaylist::~MPlaylist ()
{
    clear();
}

void MPlaylist::playSync ()
{
    if ( stopped() )
        return;

    al::format format = m_stream->stereo() ? STEREO16 : MONO16;

    m_stream->initRead();

    alGenSources(1, &m_source);
    alGenBuffers(m_buffers.size(), m_buffers.data());

    alSourcei(m_source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSourcef(m_source, AL_ROLLOFF_FACTOR, 0 );

    alSourcei(m_source, AL_BUFFER, 0);

    for ( ALuint buffer: m_buffers ) {
        m_stream->waitRead();
        alBufferData(buffer, format, m_stream->buffer, m_stream->buffer_size, m_stream->freq());
        m_stream->initRead();
    }

    if ( m_stop ) {
        m_stream->waitRead();
        m_stream->seek(0s);
        m_stream = nullptr;
        alDeleteBuffers(m_buffers.size(), m_buffers.data());
        alDeleteSources(1, &m_source);
        m_thread.detach();
        return;
    }

    alSourceQueueBuffers(m_source, m_buffers.size(), m_buffers.data());

    m_thread.detach();
    if ( !m_pause )
        resume();
}

void MPlaylist::resumeSync ()
{
    if ( stopped() )
        return;

    int state = AL_PLAYING;
    alSourcePlay(m_source);

    while ( !m_stream->eof() ) {
        al::format format = m_stream->stereo() ? STEREO16 : MONO16;
        alSourcef(m_source, AL_GAIN, volume);
        ALint processed;
        alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);
        decltype(m_buffers) buffers;

        alSourceUnqueueBuffers(m_source, processed, buffers.data());
        for ( int i = 0; i < processed; i++ ) {
            m_stream->waitRead();
            alBufferData(buffers[i], format, m_stream->buffer, m_stream->buffer_size, m_stream->freq());
            if (m_stream->eof())
                break;
            m_stream->initRead();
        }
        alSourceQueueBuffers(m_source, processed, buffers.data());

        alGetSourcei(m_source, AL_SOURCE_STATE, &state);

        if ( m_stop ) {
            if ( state != AL_STOPPED ) {
                alSourceStop(m_source);
                state = AL_STOPPED;
            }
            m_stream->waitRead();
            break;
        }

        if ( m_pause ) {
            alSourcePause(m_source);
            m_thread.detach();
            return;
        }

        if ( state != AL_PLAYING )
            alSourcePlay(m_source);

        if ( !m_stream->eof() )
            this_thread::sleep_for(0.1s);
    }

    while ( state != AL_STOPPED ) {
        if ( m_stop ) {
            alSourceStop(m_source);
            break;
        }

        if ( m_pause ) {
            alSourcePause(m_source);
            m_thread.detach();
            return;
        }

        alGetSourcei(m_source, AL_SOURCE_STATE, &state);

        if ( !m_pause && state != AL_PLAYING )
            alSourcePlay(m_source);

        sleep_for(0.2s);
    }

    alDeleteBuffers(m_buffers.size(), m_buffers.data());
    alDeleteSources(1, &m_source);

    m_stream->waitRead();
    m_stream->seek(0s);
    m_stream = nullptr;
    m_thread.detach();
    finished(m_stop);
}

size_t MPlaylist::getCurrentIndex ()
{
    int index = 0;
    auto i = m_playlist.begin();
    while ( i++ != m_current )
        index++;
    return index;
}

void MPlaylist::setCurrentIndex ( size_t index )
{
    index %= m_playlist.size();
    m_current = m_playlist.begin();
    while ( index --> 0 )
        m_current++;
}

void MPlaylist::insert ( size_t index, MAudioStream* stream )
{
    if ( index >= m_playlist.size() )
        m_playlist.push_back(stream);
    else {
        auto i = m_playlist.begin();
        while ( index --> 0 )
            i++;
        m_playlist.insert(i, stream);
    }
}

void MPlaylist::remove ( size_t index )
{
    if ( empty() )
        return;
    auto i = m_playlist.begin();
    while ( index --> 0 ) {
        i++;
        if ( i == m_playlist.end() )
            return;
    }
    if ( m_current == i )
        m_current++;
    if ( m_stream == *i ) {
        stop();
        while ( playing() )
            sleep_for(0.1s);
    }
    delete *i;
    m_playlist.erase(i);
}

void MPlaylist::clear ()
{
    stop();
    while ( playing() )
        sleep_for(0.1s);
    for ( auto stream: m_playlist )
        delete stream;
    m_playlist.clear();
}

void MPlaylist::playCurrent ()
{
    stop();
    if ( m_current == m_playlist.end() )
        return;
    while ( playing() )
        sleep_for(0.1s);
    m_pause = false;
    m_stop = false;
    m_stream = *m_current;
    m_thread = thread{&MPlaylist::playSync,this};
}

void MPlaylist::playNext ()
{
    stop();
    if ( m_current == m_playlist.end() )
        m_current = m_playlist.begin();
    else
        m_current++;
    if ( m_current == m_playlist.end() ) {
        if ( loop )
            m_current = m_playlist.begin();
        else {
            m_current--;
            return;
        }
    }
    if ( m_stopAfter && m_stopAfter + 1 )
        m_stopAfter--;
    playCurrent();
}

void MPlaylist::play ( size_t index )
{
    stop();
    setCurrentIndex(index);
    playCurrent();
}

void MPlaylist::seek ( duration < double > seconds )
{
    if ( stopped() )
        playCurrent();
    if ( stopped() )
        return;
    m_stream->seek(seconds);
}

duration < double > MPlaylist::tell ()
{
    if ( stopped() )
        return 0s;
    return m_stream->tell();
}

void MPlaylist::resume ()
{
    m_pause = false;
    if ( !paused() )
        return;
    m_thread = thread{&MPlaylist::resumeSync,this};
}

bool MPlaylist::playing ()
{
    return m_thread.joinable ();
}
