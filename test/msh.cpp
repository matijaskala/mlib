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
#include <nonstd/file>
#include <nonstd/xterm>
#include <functional>
#include <list>
#include <set>
#include <vector>
#include <mchar.h>

#include <cstring>
#include <sys/wait.h>

using namespace std;
using namespace non_std;









typedef std::basic_string<mchar_t> mstring;
  string to_string ( const mstring& __ms ) {
    string __s;
    for ( mchar_t __c: __ms ) {
      __s += __c;
    }
    return __s;
  }

template<typename _CharT, typename _Traits>
  inline std::basic_ostream<_CharT, _Traits>&
  operator<<(std::basic_ostream<_CharT, _Traits>& __os, mstring __ms) {
      for ( mchar_t __c: __ms )
          __os << __c;
      return __os;
  }

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

struct autocompletion_string {
        string name;
        color fgcolor = default_color;
        color bgcolor = default_color;
        bool bold = false;
        char suffix = ' ';
        string::size_type length() const { return name.length(); }
};
  template<typename _CharT, typename _Traits>
    inline std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& __os, const autocompletion_string& s) {
        if (s.bold)
            __os << xterm::bold;
        __os << xterm::fgcolor(s.fgcolor);
        __os << xterm::bgcolor(s.bgcolor);
        __os << s.name;
        __os << xterm::reset;
        __os << s.suffix;
        return __os;
    }

static function< void ( const string& ) > waiting;

static set< string > commands;

void refresh_commands() {
    commands.insert ( "cd" );
    commands.insert ( "ls" );
    commands.insert ( "pwd" );
    for ( file f: directory ( "/bin" ) )
        if ( f.type() == file::regular && f.mode() & 0100 )
            commands.insert ( f.name() );
}

winsize termsize() {
    winsize w;
    ioctl ( STDOUT_FILENO, TIOCGWINSZ, &w );
    return w;
}

template<typename _string>
void fprint_list ( const list< _string > files, bool interactive ) {
    if ( files.empty() )
        return;
    ostream& out = interactive ? cerr : cout;
    size_t longest = 0;
    for ( const _string& f: files ) {
        if ( f.length() > longest )
            longest = f.length();
    }
    winsize w = termsize();
    int cols = w.ws_col / ( longest + 1 );
    int rows = files.size() / cols + 1;
    auto it = files.begin();
    if ( interactive ) {
        cerr << xterm::escape ( "s" ) << '\n';
    }
    for ( int _d = files.size(); _d > 0; _d-- ) {
        out << *it << string ( longest - it->length(), ' ' );
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

void autocomplete_fini ( const string& com, short unsigned int rows, short unsigned int cols, const string& position ) {
    short unsigned int col, row;
    sscanf ( position.c_str(), "%hu:%hu", &row, &col );
    list< string > files;
    for ( set<string>::iterator i = commands.lower_bound ( com ); i != commands.end() && i->compare ( 0, com.size(), com ) == 0; i++ )
        files.push_back ( *i );
    size_t longest = 0;
    for ( const string& f: files ) {
        if ( f.size() > longest )
            longest = f.size();
    }
    winsize w = termsize();
    int wcols = w.ws_col / ( longest + 1 );
    int wrows = files.size() / wcols + 1;
    if ( wrows >= w.ws_row ) {
        cerr << xterm::escape ( "s" ) << "do you wish to see all " << files.size() << " possibilities (" << wrows << " lines)?";
        char r = getchar();
	cerr << xterm::escape ( "u" );
        if ( r == 'y' )
            cerr << endl;
        else {
            cerr << xterm::escape ( "K" );
            return;
        }
    }
    if ( rows - row < wrows ) {
        string srows = to_string ( wrows - rows + row );
            cerr << xterm::escape ( srows + "S" ) << xterm::escape ( srows + "A" );
    }
    fprint_list ( files, true );
    if ( rows - row <= wrows )
        cerr << xterm::escape ( "T" );
}

void autocomplete ( const string& com ) {
    winsize w = termsize();
    cerr << xterm::escape ( "6n" ) << xterm::escape ( "J" );
    waiting = bind ( autocomplete_fini, com, w.ws_row, w.ws_col, placeholders::_1 );
}

string get_command ( list<mstring>& history ) {
    autocompletion_string prompt;
    prompt.fgcolor = green;
    prompt.name = "$";
    cerr << xterm::escape ( "G" ) << prompt;
    termios_handler h;
    list<mstring>::iterator command_it = history.end();
    mstring command;
    mstring command_t;
    command.reserve(80);
    size_t cn = command.size();

    for ( ;; ) {
        mchar_t c = getmchar();
                    //cerr << "<" << c.to_int() << ">";continue;
        switch ( c.value ) {
            case 4: // EOF
                if( command.empty() ) {
                    cerr << endl;
                    return "\4";
                }
                break;
            case 9: // TAB
                cerr << xterm::escape ( "J" );
                autocomplete ( to_string(command) );
                break;
            case 10: // ENTER
                cerr << xterm::escape ( "J" );
                //if( !command.empty() ) {
                    for ( list<mstring>::iterator i = history.begin(); i != history.end(); i++ )
                        if ( *i == command ) {
                            history.erase ( i-- );
                        }
                    history.push_back ( command );
                //}
                cerr << endl;
                return to_string(command);
            case 12: // CLS
                cerr << xterm::escape ( "H" ) << xterm::escape ( "2J" ) << prompt << command;
                break;
            case 27: // ESC
                cin >> c;
                if ( c == 91 ) {
                    string seq;
                    cin >> c;
                    while ( ( c < 'a' || c > 'z' ) && ( c < 'A' || c > 'Z' ) ) {
                        seq += c;
                        cin >> c;
                    }
                    switch ( c.value ) {
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
                            break;
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
                        case 'F':
                            if ( cn != command.size() ) {
                                cerr << xterm::escape ( to_string ( command.size() - cn ) + "C" );
                                cn = command.size();
                            }
                            break;
                        case 'H':
                            if ( cn != 0 ) {
                                cerr << xterm::escape ( to_string ( cn ) + "D" );
                                cn = 0;
                            }
                            break;
                        case 'R':
                            if ( waiting ) {
                                waiting ( seq );
                                waiting = nullptr;
                            }
                            break;
                        case '~':
                            cin >> c;
                            cerr << c;
                            break;
                        default:
                            cerr << xterm::escape ( seq + c );
                            cerr << "unknown escape: " << c << endl;
                    }
                }
                        else if (c=='O') {
                            cin >> c;
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
                            } else { cerr << "^[" << c << endl; }
 
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
                    cerr << "<" << c.value << ">";
        }
    }
}

void ls_files ( const directory& dir ) {
    list< autocompletion_string > files;
    for ( file f: dir ) {
        autocompletion_string out;
        if ( f.mode() & 0100 )
            out.bold = true;
        switch ( f.type() ) {
            case file::regular:
                if ( f.mode() & 0100 )
                    out.fgcolor = green;
                break;
            case file::symlink:
                out.fgcolor = cyan;
                break;
            case file::directory:
                out.fgcolor = blue;
                break;
            case file::socket:
                out.fgcolor = magenta;
                break;
            case file::character_device:
            case file::block_device:
                out.bold = true;
            case file::pipe:
                out.fgcolor = yellow;
                break;
            case -1:
                out.fgcolor = white;
                out.bgcolor = red;
                break;
        }
        if ( f.mode() & 01000 ) {
            out.fgcolor = black;
            out.bgcolor = green;
        }
        char indicator;
        switch ( f.type() ) {
            case file::regular:
                indicator = (f.mode() & 0100) ? '*' : ' ';
                break;
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
            case file::pipe:
                indicator = '|';
                break;
        }
        out.name = f.name();
        out.suffix = indicator;
        files.push_back ( out );
    }
    fprint_list ( files, false );
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
    list<mstring> history;

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
                        cerr << filename << ": Input/Output error." << endl;
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
        else {
            pid_t pid = fork();
            switch ( pid ) {
                case 0:
                {
                    int argc = args.size();
                    char** argv = new char*[argc+1];
                    for ( int i = 0; i < argc; i++ )
                        argv[i] = strdup ( args[i].c_str() );
                    argv[argc] = nullptr;
                    execvp ( args.front().c_str(), argv );
                    system ( com.c_str() );
                    break;
                }
                default:
                    waitpid ( pid, NULL, 0 );
                    break;
            }
        }
    }

    return 0;
}
