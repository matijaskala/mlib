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
#include <filesystem>
#include <termcolor.hh>

using namespace std;

void list ( const filesystem::file_status& f, string n ) {
    if ( f.type() == filesystem::file_type::symlink )
        cerr << termcolor::bold << termcolor::fg ( termcolor::cyan );
    else if ( f.type() == filesystem::file_type::directory )
        cerr << termcolor::bold << termcolor::fg ( termcolor::blue );
    else if ( static_cast<int>(f.permissions()) & 0111 )
        cerr << termcolor::bold << termcolor::fg ( termcolor::green );
    cerr << n << termcolor::reset << ' ';
}

int main ( int argc, char** argv ) {
    vector<string> t(argc > 1 ? argc - 1 : 1);
    if ( argc > 1 )
        for ( int i = 1; i < argc; i++ )
            t[i-1] = argv[i];
    else
        t[0] = ".";
    for ( auto n: t ) {
        filesystem::file_status d = filesystem::status(n);
        if ( d.type() == filesystem::file_type::directory ) {
            for ( auto f: filesystem::directory_iterator{n} ) {
                list ( f.status(), f.path() );
            }
        }
        else
            list ( d, n );
        cerr << endl;
    }
    return 0;
}
