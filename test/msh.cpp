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
#include <nonstd/filesystem>
#include <nonstd/xterm>
#include <functional>
#include <list>
#include <vector>
#include <nonstd/tokenizer>
#include <boost/tokenizer.hpp>

using namespace std;
using namespace non_std;

#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

struct termios_handler {
    termios oldt;
    termios_handler() {
        tcgetattr ( STDIN_FILENO, &oldt );
        termios newt = oldt;
        newt.c_lflag &= ~ICANON;
        newt.c_lflag &= ~ECHO;
        tcsetattr ( STDIN_FILENO, TCSANOW, &newt );
    }
    ~termios_handler() {
        tcsetattr ( STDIN_FILENO, TCSANOW, &oldt );
    }
};

static function< void ( const string& ) > waiting;

static set< string > commands;

void refresh_commands() {
    commands.insert ( "cd" );
    commands.insert ( "ls" );
    commands.insert ( "pwd" );
    for ( file f: directory ( "/bin" ) )
        if ( f.is_regular() && f.mode & 0100 )
            commands.insert ( f.name );
}

void fprint_list ( const list< pair< string, size_t > > files, bool interactive );

winsize termsize() {
    winsize w;
    ioctl ( STDOUT_FILENO, TIOCGWINSZ, &w );
    return w;
}

void autocomplete_fini ( const string& com, short unsigned int rows, short unsigned int cols, const string& position ) {
    short unsigned int col, row;
    sscanf ( position.c_str(), "%hu:%hu", &row, &col );
    list< pair< string, size_t > > files;
    for ( set<string>::iterator i = commands.lower_bound ( com ); i != commands.end() && i->compare ( 0, com.size(), com ) == 0; i++ )
        files.push_back ( { *i, i->size() } );
    size_t longest = 0;
    for ( const pair<string,size_t>& f: files ) {
        if ( f.second > longest )
            longest = f.second;
    }
    winsize w = termsize();
    int wcols = w.ws_col / longest;
    int wrows = files.size() / wcols + 1;
    if ( wrows >= w.ws_row ) {
        cerr << xterm::escape ( "s" ) << "do you wish to see all " << files.size() << " possibilities (" << wrows << " lines)?";
        char r;
        cin >> r;
	cerr << xterm::escape ( "u" );
        if ( r == 'y' )
            cerr << endl;
        else {
            cerr << xterm::escape ( "K" );
            return;
        }
    }
    if ( rows - row < wrows ) {
    string srows = to_string ( wrows );
            cerr << xterm::escape ( srows + "S" ) << xterm::escape ( srows + "A" );
    }
    fprint_list ( files, true );
}

void autocomplete ( const string& com ) {
    winsize w = termsize();
    cerr << xterm::escape ( "6n" );
    waiting = bind ( autocomplete_fini, com, w.ws_row, w.ws_col, placeholders::_1 );
}

string get_command ( list<string>& history ) {
    string prompt = xterm::fgcolor ( green ) + "$ " + xterm::reset();
    cerr << xterm::escape ( "G" ) << prompt;
    termios_handler h;
    list<string>::iterator command_it = history.end();
    string command;
    string command_t;
    command.reserve(80);
    size_t cn = command.size();

    for ( ;; ) {
        char c = getchar();
        switch ( c ) {
            case 4: // EOF
                if( command == "" ) {
                    cerr << endl;
                    return "\4";
                }
                break;
            case 9: // TAB
                autocomplete ( command );
                break;
            case 10: // ENTER
                if( command != "" ) {
                    for ( list<string>::iterator i = history.begin(); i != history.end(); i++ )
                        if ( *i == command ) {
                            history.erase ( i-- );
                        }
                    history.push_back ( command );
                }
                cerr << endl;
                return command;
            case 12: // CLS
                cerr << xterm::escape ( "H" ) << xterm::escape ( "2J" ) << prompt << command;
                break;
            case 27: // ESC
                c = getchar();
                if ( c == 91 ) {
                    string seq;
                    c = getchar();
                    while ( ( c < 'a' || c > 'z' ) && ( c < 'A' || c > 'Z' ) ) {
                        seq += c;
                        c = getchar();
                    }
                    switch ( c ) {
                        case 'A':
                            if ( command_it != history.begin() ) {
                                if ( command_it == history.end() )
                                    command_t = command;
                                command_it--;
                                command = *command_it;
                                cn = command.size();
                                cerr << xterm::escape ( "G" ) << prompt << command << xterm::escape ( "K" );
                            }
                            break;
                        case 'B':
                            if ( command_it != history.end() ) {
                                command_it++;
                                if ( command_it == history.end() )
                                    command = command_t;
                                else
                                    command = *command_it;
                                cn = command.size();
                                cerr << xterm::escape ( "G" ) << prompt << command << xterm::escape ( "K" );
                            }
                        case 'C':
                            if ( cn < command.size() ) {
                                cn++;
                                cerr << xterm::escape ( "C" );
                            }
                            break;
                        case 'D':
                            if ( cn > 0 ) {
                                cn--;
                                cerr << xterm::escape ( "D" );
                            }
                            break;
                       case 'R':
                            if ( waiting ) {
                                waiting ( seq );
                                waiting = nullptr;
                            }
                            break;
                        case '~':
                            c = getchar();
                            cerr << c;
                            break;
                        default:
                            cerr << xterm::escape ( seq + c );
                            cerr << "unknown escape: " << c << endl;
                    }
                }
                        else if (c=='O') {
                            c = getchar();
                            if ( c == 'F' ) {
                                if ( cn != command.size() ) {
                                    cerr << xterm::escape ( to_string ( command.size() - cn ) + "C" );
                                    cn = command.size();
                                }
                            }
                            if ( c == 'H' ) {
                                if ( cn != 0 ) {
                                    cerr << xterm::escape ( to_string ( cn ) + "D" );
                                    cn = 0;
                                }
                            }
                            }
 
                break;
            case 127: // BACKSPACE
                if ( cn > 0 ) {
                    cerr << '\b';
                    cerr << xterm::escape ( "s" );
                    cerr << command.substr ( cn );
                    cerr << ' ';
                    cerr << xterm::escape ( "u" );
                    cn--;
                    command.erase ( cn, 1 );
                }
                break;
            default:
                if ( c > 31 ) {
                    command.insert ( cn, 1, c );
                    cerr << xterm::escape ( "s" );
                    cerr << command.substr ( cn );
                    cerr << xterm::escape ( "u" ) << c;
                    cn++;
                }
                else
                    cerr << "\b" << (int)c;
        }
    }
}

void ls_files ( const directory& dir ) {
    if ( dir.empty() )
        return;
    list< pair< string, size_t > > files;
    for ( const file& f: dir ) {
        string format;
        if ( f.mode & 0100 )
            format += xterm::bold();
        switch ( f.mode & file::type ) {
            case file::regular:
                if ( f.mode & 0100 )
                    format += xterm::fgcolor ( green );
                break;
            case file::symlink:
                format += xterm::fgcolor ( cyan );
                break;
            case file::directory:
                format += xterm::fgcolor ( blue );
                break;
            case file::socket:
                format += xterm::fgcolor ( magenta );
                break;
            case file::character_device:
            case file::block_device:
                format += xterm::bold();
            case file::FIFO:
                format += xterm::fgcolor ( yellow );
                break;
            case file::type:
                format += xterm::fgcolor ( white ) + xterm::bgcolor ( red );
                break;
        }
        if ( f.mode & 01000 )
            format = xterm::fgcolor ( black ) + xterm::bgcolor ( green );
        char indicator = ' ';
        if ( f.mode & 0100 )
            indicator = '*';
        switch ( f.mode & file::type ) {
            case file::symlink:
                indicator = '@';
                break;
            case file::directory:
                indicator = '/';
                break;
            case file::socket:
                indicator = '=';
                break;
            case file::character_device:
                indicator = '%';
                break;
            case file::block_device:
                indicator = '#';
                break;
            case file::FIFO:
                indicator = '|';
                break;
        }
        string out = format + f.name + xterm::reset() + indicator;
        files.push_back ( { out, f.name.size() } );
        // cout << xterm::escape ( "40G" ) << oct << xterm::fgcolor ( cyan ) << ( xfile.mode & ~file::type ) << xterm::reset() << endl;
    }
    fprint_list ( files, false );
}

void fprint_list ( const list< pair< string, size_t > > files, bool interactive ) {
    if ( files.empty() )
        return;
    ostream& out = interactive ? cerr : cout;
    size_t longest = 0;
    for ( const pair<string,size_t>& f: files ) {
        if ( f.second > longest )
            longest = f.second;
    }
    winsize w = termsize();
    int cols = w.ws_col / longest;
    int rows = files.size() / cols + 1;
    auto it = files.begin();
    if ( interactive ) {
        cerr << xterm::escape ( "s" ) << '\n';
    }
    for ( int _d = files.size(); _d > 0; _d-- ) {
        out << it->first << string ( longest - it->second, ' ' );
        for ( int i = 0; i < rows; i++ ) {
            it++;
            if ( it == files.end() ) {
                it = files.begin();
                i -= files.size() % rows;
                if ( i < 0 )
                    i += rows;
                it++;
                out << endl;
            }
        }
    }
    if ( interactive )
        cerr << xterm::escape ( "u" );
}

vector<string> expand_command ( string command ) {
    vector<string> args;
    command += ' ';
    size_t a = 0;
    char b = 0;
    for ( size_t i = 0; i < command.size(); i++ ) {
        char c = command[i];
        if ( b ) {
            if ( b == c )
                b = 0;
            continue;
        }
        if ( c == '\"' || c == '\'' )
            b = c;
        if ( c == ' ' ) {
            string arg = command.substr ( a, i-a );
            string::iterator t = arg.begin();
            while ( t != arg.end() && *t == ' ' )
                arg.erase ( t++ );
            if ( arg.size() != 0 )
                args.push_back ( arg );
            a = i;
        }
    }
    return args;
}

int main() {
    refresh_commands();
    tokenizer t ( "" );
    for ( string s: t );
    list<string> history;

    for ( string com; com != "\4"; com = get_command ( history ) ) {
        int e = com.find ( ' ' );
        char* cwd = getcwd(NULL,0);
        string working_dir = cwd;
        free(cwd);
        vector<string> args = expand_command ( com );
        if ( args.empty() )
            continue;
        else if ( args.front() == "ls-F" ) {
	    if ( args.size() == 1 )
	        ls_files ( working_dir );
            else
                for ( size_t i = 1; i < args.size(); i++ )
                    ls_files ( args[i] );
        }
        else if ( args.front() == "cd" ) {
            if ( args.size() > 2 ) {
                cerr << "cd: Too many arguments" << endl;
                continue;
            }
            const char* filename;
            if ( args.size() == 1 )
                filename = getenv ( "HOME" );
            else
                filename = args[1].c_str();
            if ( chdir ( filename ) < 0 )
                switch ( errno ) {
                    case EFAULT:
                        cerr << filename << ": input/outpur error." << endl;
                        break;
                    case ELOOP:
                        cerr << filename << ": Too many levels of symbolic links." << endl;
                        break;
                    case ENAMETOOLONG:
                        cerr << filename << ": File name too long." << endl;
                        break;
                    case ENOENT:
                        cerr << filename << ": No such file or directory." << endl;
                        break;
                    case ENOTDIR:
                        cerr << filename << ": Not a directory." << endl;
                        break;
                }
        }
        else if ( args.front() == "pwd" )
            cout << working_dir << endl;
        else if ( args.front() == "echo" )
            cout << com.substr ( e + 1 ) << endl;
        else
            system ( com.c_str() );
    }

    return 0;
}
