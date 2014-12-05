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

#ifndef MFILESYSTEM_H
#define MFILESYSTEM_H

#include <MGlobal>
#include <set>
#include <string>

class MFile
{
public:
    enum Type {
        PIPE,
        CHARACTER_DEVICE,
        DIRECTORY,
        BLOCK_DEVICE,
        REGULAR,
        SYMLINK,
        SOCKET,
    };

    MFile ( std::string fullname );
    MFile ( std::string name, std::string path );

    void refresh();

    const std::string& name() const { return m_name; }
    const std::string& path() const { return m_path; }
    std::string fullname() const { return path() + "/" + name(); }

    std::uint16_t mode() const { return m_mode; }
    Type type() const { return m_type; }

private:
    std::string m_name;
    std::string m_path;

    std::uint16_t m_mode;
    Type m_type;

};

class M_EXPORT MDirectory
{
public:
    class Entry {
    public:
        Entry ( std::string name, std::string path ) : m_name{name}, m_path{path} {}
        operator MFile() const { return {name(),path()}; }

        const std::string& name() const { return m_name; }
        const std::string& path() const { return m_path; }
        std::string fullname() const { return path() + "/" + name(); }

    private:
        std::string m_name;
        std::string m_path;
    };

    MDirectory ( std::string path );

    auto begin() const { return m_entries.begin(); }
    auto end() const { return m_entries.end(); }

    const std::string& path() const { return m_path; }

private:
    struct before : std::binary_function<Entry,Entry,bool> {
        bool operator() (const Entry& x, const Entry& y) { return x.name() < y.name(); }
    };

    std::set<Entry,before> m_entries;
    std::string m_path;

};

#endif // MFILESYSTEM_H
