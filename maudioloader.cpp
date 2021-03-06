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

#include <MResourceLoader>
#include <MAudioStream>
#include <MAudioFile>
#include <MDebug>

#include <fstream>

static class MAudioLoader : public MResourceLoader
{
    virtual bool valid ( std::string file ) override;
    virtual MResource* load ( std::string file ) override;
    virtual MResource::Type type() override;
    virtual std::string name() override;
} audioLoader;

bool MAudioLoader::valid ( std::string file )
{
    std::ifstream stream{file};
    if ( !stream.is_open() )
        mDebug() << file << ": No such file or directory.";
    if ( !stream.good() )
        return false;
    for ( auto iface: MAudioStreamInterface::interfaces() )
        if ( iface->valid ( &stream ) )
            return true;
    return false;
}

MResource* MAudioLoader::load ( std::string file )
{
    auto fileStream = new std::ifstream{file, std::ios_base::binary};
    if ( !fileStream->is_open() ) {
        delete fileStream;
        return nullptr;
    }
    MAudioStream audioStream{fileStream};
    if ( !audioStream.valid() )
        return nullptr;
    auto audioFile = new MAudioFile;
    audioFile->stereo = audioStream.stereo();
    audioFile->freq = audioStream.freq();
    while ( !audioStream.eof() ) {
        audioStream.initRead();
        audioStream.waitRead();
        audioFile->buffer->insert ( audioFile->buffer->end(), audioStream.buffer, audioStream.buffer + audioStream.buffer_size );
    }
    return audioFile;
}

MResource::Type MAudioLoader::type()
{
    return MResource::Audio;
}

std::string MAudioLoader::name()
{
    return "audio";
}
