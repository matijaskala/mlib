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

#include <iostream>
#include <vector>
#include <nonstd/filesystem>
#include <nonstd/xterm>

using namespace std;
using namespace non_std;

void list ( const file& f ) {
    if ( f.is_symlink() )
        cerr << xterm::bold << xterm::fgcolor ( cyan );
    else if ( f.is_directory() )
        cerr << xterm::bold << xterm::fgcolor ( blue );
    else if ( f.mode & 0x40 )
        cerr << xterm::bold << xterm::fgcolor ( green );
    cerr << f.name << xterm::reset << ' ';
}

int main ( int argc, char** argv ) {
    vector<string> t(argc > 1 ? argc - 1 : 1);
    if ( argc > 1 )
        for ( int i = 1; i < argc; i++ )
            t[i-1] = argv[i];
    else
        t[0] = ".";
    for ( file d: t ) {
        if ( d.is_directory() ) {
            directory dir = d.name;
            for ( file f: dir ) {
                list ( f );
            }
        }
        else
            list ( d );
        cerr << endl;
    }
    return 0;
}
