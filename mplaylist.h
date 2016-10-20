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

#ifndef MPLAYLIST_H
#define MPLAYLIST_H

#include <maudiostream.h>
#include <array>
#include <nonstd/signal>

class M_EXPORT MPlaylist
{
public:
    /**
     *  Constructs an empty playlist.
     */
    MPlaylist ();

    /**
     *  Clears and destructs the playlist.
     */
    ~MPlaylist ();

    /**
     *  Puts @a stream to the end of the playlist.
     */
    template < typename _Stream >
    void insert ( _Stream stream ) { insert(size(), stream); }

    /**
     *  Inserts @a stream before @index.
     */
    void insert ( std::size_t index, MAudioStream* stream );

    /**
     *  Constructs a new audio stream from @a stream and inserts it before @index.
     */
    void insert ( std::size_t index, std::istream* stream ) { insert(index, new MAudioStream{stream}); }

    /**
     *  Constructs a new audio stream from @a streambuf and inserts it before @index.
     */
    void insert ( std::size_t index, std::streambuf* streambuf ) { insert(index, new MAudioStream{streambuf}); }

    /**
     *  Loads a song and inserts it before @index.
     *  @param  file Path to the song.
     */
    void insert ( std::size_t index, const std::string& file ) { insert(index, new MAudioStream{file}); }

    /**
     *  Loads a song and inserts it before @index.
     *  @param  file Path to the song.
     */
    void insert ( std::size_t index, const char* file ) { insert(index, new MAudioStream{file}); }

    /**
     *  Removes a song.
     *  If the song is selected it selects the next song.
     */
    void remove ( std::size_t index );

    /**
     *  Removes all the songs from the playlist.
     */
    void clear ();

    /**
     *  @return  Index of the current songs or size() if no song is selected.
     */
    std::size_t getCurrentIndex ();

    /**
     *  Sets the current song.
     */
    void setCurrentIndex ( std::size_t index );

    /**
     *  @return  The number of songs in the playlist.
     */
    std::size_t size () { return m_playlist.size(); }

    /**
     *  @return  True if the playlist is empty.
     */
    bool empty () { return m_playlist.empty(); }

    /**
     *  Plays the song at @a index.
     */
    void play ( std::size_t index );

    /**
     *  Plays the current song.
     */
    void playCurrent ();

    /**
     *  Selects the next song and plays it.
     *  If no song is selected it plays the first song in the playlist.
     *  If the current song is the last in the playlist and @c loop is false it calls @c stop() and returns.
     *  If the current song is the last in the playlist and @c loop is true it plays the first song in the playlist.
     */
    void playNext ();

    /**
     *  Selects a random song and plays it.
     */
    void playRandom () { play(rand() % m_playlist.size()); }

    /**
     *  Stops playing and rewinds the current song.
     */
    void stop () { m_stop = true; }

    /**
     *  Pauses the current song.
     */
    void pause () { m_pause = true; }

    /**
     *  Resumes the current song.
     */
    void resume ();

    /**
     *  @return  True if a song is playing.
     */
    bool playing ();

    bool stopped () { return !m_stream; }

    bool paused () { return !playing() && !stopped(); }

    bool loop = false;

    float volume = 1;

    /**
     *  A song finished playing.
     *  @param  1 True if it was stopped using @c stop().
     */
    non_std::signal<bool> finished{this};

    void seek ( std::chrono::duration < double > seconds );

    std::chrono::duration < double > tell ();

    /**
     *  Stops playing after @a count songs.
     */
    void stopAfter ( std::size_t count ) { m_stopAfter = count; }

    /**
     *  @return  Number of songs to be played before the playlist will stop.
     */
    std::size_t stoppingAfter () { return m_stopAfter; }

private:
    void playSync ();
    void resumeSync ();

    volatile bool m_pause;
    volatile bool m_stop;
    std::thread m_thread;
    unsigned int m_source;
    std::array<unsigned int, 8> m_buffers;
    std::list<MAudioStream*> m_playlist;
    std::list<MAudioStream*>::iterator m_current = m_playlist.end();
    MAudioStream* volatile m_stream = nullptr;
    std::size_t m_stopAfter = -1;
};

#endif // MPLAYLIST_H
