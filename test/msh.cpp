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
#include <termcolor.hh>
#include <filesystem>
#include <functional>
#include <list>
#include <set>
#include <vector>
#include <mchar.h>

#include <cstring>
#include <sys/wait.h>

using namespace std;
using namespace filesystem;
using namespace termcolor;









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
        value fgcolor = default_color;
        value bgcolor = default_color;
        bool bold = false;
        bool blink = false;
        char suffix = ' ';
        string::size_type length() const { return name.length(); }
};
  template<typename _CharT, typename _Traits>
    inline std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& __os, const autocompletion_string& s) {
        if (s.bold)
            __os << bold;
        if (s.blink)
            __os << escape("5m");
        __os << fg(s.fgcolor);
        __os << bg(s.bgcolor);
        __os << s.name;
        __os << reset;
        __os << s.suffix;
        return __os;
    }

static function< void ( const string& ) > waiting;

static set< string > commands;

void refresh_commands() {
    commands.insert ( "cd" );
    commands.insert ( "ls" );
    commands.insert ( "pwd" );
    for ( auto&& f: directory_iterator ( "/bin" ) )
        if ( f.symlink_status().type() == file_type::regular && ( f.status().permissions() & perms::owner_exec ) != perms::none )
            commands.insert ( f.path().filename() );
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
        cerr << escape ( "s" ) << '\n';
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
        cerr << escape ( "u" );
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
        cerr << escape ( "s" ) << "do you wish to see all " << files.size() << " possibilities (" << wrows << " lines)?";
        char r = getchar();
	cerr << escape ( "u" );
        if ( r == 'y' )
            cerr << endl;
        else {
            cerr << escape ( "K" );
            return;
        }
    }
    if ( rows - row < wrows ) {
        string srows = to_string ( wrows - rows + row );
            cerr << escape ( srows + "S" ) << escape ( srows + "A" );
    }
    fprint_list ( files, true );
    if ( rows - row <= wrows )
        cerr << escape ( "T" );
}

void autocomplete ( const string& com ) {
    winsize w = termsize();
    cerr << escape ( "6n" ) << escape ( "J" );
    waiting = bind ( autocomplete_fini, com, w.ws_row, w.ws_col, placeholders::_1 );
}

string get_command ( list<mstring>& history ) {
    char cwd[60];
    getcwd(cwd, 60);
    string prompt = fg ( magenta ) + cwd + fg ( green ) + " $ " + escape ( "m" );
    cerr << escape ( "G" ) << prompt;
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
                cerr << escape ( "J" );
                autocomplete ( to_string(command) );
                break;
            case 10: // ENTER
                cerr << escape ( "J" );
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
                cerr << escape ( "H" ) << escape ( "2J" ) << prompt << command;
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
                                cerr << escape ( "G" ) << prompt << command << escape ( "K" );
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
                                cerr << escape ( "G" ) << prompt << command << escape ( "K" );
                            }
                            break;
                        case 'C':
                            if ( cn < command.size() ) {
                                cn++;
                                cerr << escape ( "C" );
                            }
                            break;
                        case 'D':
                            if ( cn > 0 ) {
                                cn--;
                                cerr << escape ( "D" );
                            }
                            break;
                        case 'F':
                            if ( cn != command.size() ) {
                                cerr << escape ( to_string ( command.size() - cn ) + "C" );
                                cn = command.size();
                            }
                            break;
                        case 'H':
                            if ( cn != 0 ) {
                                cerr << escape ( to_string ( cn ) + "D" );
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
                            cerr << escape ( seq + c );
                            cerr << "unknown escape: " << c << endl;
                    }
                }
                        else if (c=='O') {
                            cin >> c;
                            if ( c == 'F' ) {
                                if ( cn != command.size() ) {
                                    cerr << escape ( to_string ( command.size() - cn ) + "C" );
                                    cn = command.size();
                                }
                            }
                            if ( c == 'H' ) {
                                if ( cn != 0 ) {
                                    cerr << escape ( to_string ( cn ) + "D" );
                                    cn = 0;
                                }
                            }
                            } else { cerr << "^[" << c << endl; }
 
                break;
            case 127: // BACKSPACE
                if ( cn > 0 ) {
                    cerr << '\b';
                    cerr << escape ( "s" );
                    cerr << command.substr ( cn );
                    cerr << ' ';
                    cerr << escape ( "u" );
                    cn--;
                    command.erase ( cn, 1 );
                }
                break;
            default:
                if ( c > 31 ) {
                    command.insert ( cn, 1, c );
                    cerr << escape ( "s" );
                    cerr << command.substr ( cn );
                    cerr << escape ( "u" ) << c;
                    cn++;
                }
                else
                    cerr << "<" << c.value << ">";
        }
    }
}

void ls_files ( string dir ) {
    list< autocompletion_string > files;
    for ( auto&& entry: directory_iterator(dir) ) {
        autocompletion_string out;
        auto perms = entry.symlink_status().permissions();
        auto type = entry.symlink_status().type();
        if ( ( perms & perms::owner_exec ) != perms::none )
            out.bold = true;
        switch ( type ) {
            case file_type::regular:
                if ( ( perms & perms::owner_exec ) != perms::none )
                    out.fgcolor = green;
                break;
            case file_type::symlink:
                out.fgcolor = cyan;
                {
                    if ( entry.status().type() == file_type::not_found ) {
                        out.fgcolor = white;
                        out.bgcolor = red;
                        out.blink = true;
                    }
                }
                break;
            case file_type::directory:
                out.fgcolor = blue;
                break;
            case file_type::socket:
                out.fgcolor = magenta;
                break;
            case file_type::character:
            case file_type::block:
                out.bold = true;
                [[fallthrough]];
            case file_type::fifo:
                out.fgcolor = yellow;
                break;
            case file_type::unknown:
                break;
        }
        if ( (perms & perms::sticky_bit) != perms::none ) {
            out.fgcolor = black;
            out.bgcolor = green;
        }
        char indicator;
        switch ( type ) {
            case file_type::regular:
                indicator = (perms & perms::owner_exec) != perms::none ? '*' : ' ';
                break;
            case file_type::symlink:
                indicator = '@';
                break;
            case file_type::directory:
                indicator = '/';
                break;
            case file_type::socket:
                indicator = '=';
                break;
            case file_type::character:
                indicator = '%';
                break;
            case file_type::block:
                indicator = '#';
                break;
            case file_type::fifo:
                indicator = '|';
                break;
        }
        out.name = entry.path().filename();
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
