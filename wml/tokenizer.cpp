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

#include <istream>

using namespace std;
using namespace wml;

tokenizer::tokenizer ( istream* stream )
    : stream{stream}, next{stream->get()}
{
    next_char();
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
            return curr_token;
        case '<':
            if ( next != '<' ) {
                curr_token.type = token::MISC;
                curr_token.value += current;
                break;
            }
            next_char();
            curr_token.type = token::LITERAL;
            for (;;) {
                next_char();
                if ( current == traits::eof() ) {
                    curr_token.type = token::UNTERMINATED_LITERAL;
                    break;
                }
                if ( current == '>' && next == '>' ) {
                    next_char();
                    break;
                }
                if ( current == '\n' )
                    lineno++;
                curr_token.value += current;
            }
            break;
        case '"':
            curr_token.type = token::LITERAL;
            for (;;) {
                next_char();
                if ( current == traits::eof() ) {
                    curr_token.type = token::UNTERMINATED_LITERAL;
                    break;
                }
                if ( current == '"' ) {
                    if ( next != '"' )
                        break;
                    next_char();
                }
                if ( current == '\n' )
                    lineno++;
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
        case '\n':
            lineno++;
        case '[':
        case ']':
        case '/':
        case '=':
        case ',':
        case '+':
            curr_token.type = current;
            curr_token.value = current;
            break;
        case '_':
            if ( !isalnum ( next ) && next != '_' ) {
                curr_token.type = current;
                curr_token.value = current;
                break;
            }
        default:
            if ( isalnum ( current ) || current == '_' ) {
                curr_token.type = token::WORD;
                do {
                    curr_token.value += current;
                    next_char();
                } while ( isalnum ( current ) || current == '_' );
                return curr_token;
            }
            else {
                curr_token.type = token::MISC;
                curr_token.value += current;
            }
    }
    next_char();
    return curr_token;
}

void tokenizer::next_char()
{
    current = next;
    if ( buffer_offset < buffer_length )
        next = traits::to_int_type(buffer[buffer_offset++]);
    else {
        buffer_offset = 0;
        if ( stream->peek() != traits::eof() && stream->readsome ( buffer, sizeof(buffer) ) == 0 )
            stream->get(*buffer);
        buffer_length = stream->gcount();
        if ( buffer_length )
            next = *buffer;
        else
            next = traits::eof();
    }
}
