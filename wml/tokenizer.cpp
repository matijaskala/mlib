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

#include "tokenizer.h"

using namespace std;
using namespace wml;

static bool is_alnum ( int c ) {
    return c == '_' || isalnum ( c );
}

const token& tokenizer::next_token()
{
    prev_token = curr_token;
    curr_token.value.clear();
    while ( isblank ( current ) )
        next_char();
    startlineno = lineno;
    switch ( current ) {
        case traits::eof():
            curr_token.type = token::END;
            break;
        case '<':
            if ( stream.peek() != '<' ) {
                curr_token.type = token::MISC;
                curr_token.value += current;
                break;
            }
            curr_token.type = token::LITERAL;
            for (;;) {
                if ( current == '\n' )
                    lineno++;
                next_char();
                if ( current == traits::eof() ) {
                    curr_token.type = token::UNTERMINATED_LITERAL;
                    break;
                }
                if ( current == '>' && stream.peek() == '>' ) {
                    next_char();
                    break;
                }
                curr_token.value += current;
            }
            break;
        case '"':
            curr_token.type = token::LITERAL;
            for (;;) {
                if ( current == '\n' )
                    lineno++;
                next_char();
                if ( current == traits::eof() ) {
                    curr_token.type = token::UNTERMINATED_LITERAL;
                    break;
                }
                if ( current == '"' ) {
                    if ( stream.peek() != '"' )
                        break;
                    next_char();
                }
                curr_token.value += current;
            }
            break;
        case '#':
            curr_token.type = token::COMMENT;
            for (;;) {
                next_char();
                if ( current == '\n' || current == traits::eof() )
                    return curr_token;
                curr_token.value += current;
            }
            break;
        case '[':
            curr_token.type = token::OPEN_BRACKET;
            curr_token.value = "[";
            break;
        case ']':
            curr_token.type = token::CLOSE_BRACKET;
            curr_token.value = "]";
            break;
        case '/':
            curr_token.type = token::SLASH;
            curr_token.value = "/";
            break;
        case '\n':
            curr_token.type = token::LINE_FEED;
            curr_token.value = "\n";
            lineno++;
            break;
        case '=':
            curr_token.type = token::EQUALS;
            curr_token.value = "=";
            break;
        case '+':
            curr_token.type = token::PLUS;
            curr_token.value = "+";
            break;
        case '_':
            if ( !is_alnum ( stream.peek() ) ) {
                curr_token.type = token::UNDERSCORE;
                curr_token.value = '_';
                break;
            }
        default:
            if ( is_alnum ( current ) ) {
                curr_token.type = token::WORD;
                do {
                    curr_token.value += current;
                    next_char();
                } while ( is_alnum ( current ) );
            }
            else {
                curr_token.type = token::MISC;
                curr_token.value += current;
                next_char();
            }
            return curr_token;
    }
    if ( current != traits::eof() )
        next_char();
    return curr_token;
}

