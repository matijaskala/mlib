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

#include "maudiostream.h"

#include <fstream>

class MIStream : public std::istream {
public:
    explicit MIStream ( const std::string& file ) : MIStream{new std::filebuf} {
        if ( !static_cast<std::filebuf*> (rdbuf())->open ( file, in | binary ) )
            setstate ( failbit );
        else
            clear();
    }
    explicit MIStream ( const char* file ) : MIStream{new std::filebuf} {
        if ( !static_cast<std::filebuf*> (rdbuf())->open ( file, in | binary ) )
            setstate ( failbit );
        else
            clear();
    }
    explicit MIStream ( std::streambuf* streambuf ) : std::istream{streambuf} {}
    virtual ~MIStream() { delete rdbuf(); }
};

MAudioStream::MAudioStream ( std::istream* stream ) : m_interface{nullptr}, m_stream{stream}
{
    for ( auto iface: MAudioStreamInterface::interfaces() )
        if ( iface->valid ( m_stream ) ) {
            iface->init ( this );
            if ( valid() ) {
                m_interface = iface;
                break;
            }
        }
}

MAudioStream::MAudioStream ( std::streambuf* streambuf ) : MAudioStream{new MIStream{streambuf}}
{
}

MAudioStream::MAudioStream ( const std::string& file ) : MAudioStream{new MIStream{file}}
{
}

MAudioStream::MAudioStream ( const char* file ) : MAudioStream{new MIStream{file}}
{
}

MAudioStream::~MAudioStream()
{
    waitRead();
    if ( m_interface )
        m_interface->fini ( this );
    delete m_stream;
}

void MAudioStream::initRead()
{
    m_thread = std::thread { [this] {
        if ( valid() )
            m_interface->read(this);
    } };
}

void MAudioStream::waitRead()
{
    if ( m_thread.joinable() )
        m_thread.join();
}

void MAudioStream::seek ( std::chrono::duration< double > seconds )
{
    if ( !valid() )
        return;
    m_eof = false;
    m_interface->seek(this, seconds.count());
}

std::chrono::duration< double > MAudioStream::tell ()
{
    if ( !valid() )
        return {};
    return std::chrono::duration< double > { m_interface->tell(this) };
}

std::list<std::string> MAudioStream::getTag ( MAudioTag tag )
{
    if ( !valid() )
        return {};
    return m_interface->getTag(this, tag);
}

std::list<MAudioStreamInterface*>& MAudioStreamInterface::interfaces()
{
    static std::list<MAudioStreamInterface*> interfaces;
    return interfaces;
}

MAudioStreamInterface::MAudioStreamInterface()
{
    interfaces().push_back ( this );
}

MAudioStreamInterface::~MAudioStreamInterface()
{
    interfaces().remove ( this );
}

std::list<std::string> MAudioStreamInterface::getTag ( MAudioStream* audioStream, MAudioTag tag ) const
{
    (void)audioStream;
    (void)tag;

    return {};
}
