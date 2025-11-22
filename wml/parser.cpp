/*
 * Wesnoth Markup Language parser
 * Copyright (C) 2014-2016  Matija Skala <mskala@gmx.com>
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

#include "parser.h"

#include <fstream>

using namespace wml;

exception::exception ( std::string message )
    : message{message} {}

const char* exception::what() const noexcept
{
    return message.c_str();
}

parser::parser ( std::string file )
    : elements{}, stream{new std::ifstream{file}}, tok{stream.get()}
{

}

parser::parser ( std::streambuf* streambuf )
    : elements{}, stream{new std::istream{streambuf}}, tok{stream.get()}
{

}

parser::parser ( std::istream* stream )
    : elements{}, stream{}, tok{stream}
{

}

void parser::operator() ( config& cfg )
{
    cfg.clear();
    elements.push({cfg, 0});

    do {
        switch ( tok.next_token().type ) {
            case token::LINE_FEED:
            case token::COMMENT:
                continue;
            case token::OPEN_BRACKET:
                parse_element();
                break;
            case token::WORD:
                parse_variable();
                break;
            default:
            case token::END:
                break;
        }
    } while ( tok.current_token().type != token::END );

    if ( elements.size() != 1 )
        throw exception{"Missing closing tag for tag [" + elements.top().first.name +
                        "] opened at " + std::to_string(elements.top().second)};
    elements.pop();
}

void parser::parse_element()
{
    std::string elname;
    config& cfg = elements.top().first;
    switch ( tok.next_token().type ) {
        case token::WORD:
            elname = tok.current_token().value;
            if ( tok.next_token().type != token::CLOSE_BRACKET )
                throw exception{"Unterminated [element] tag"};
            cfg.children.push_back(elname);
            elements.push({cfg.children.back(), tok.start_line()});
            break;
        case token::PLUS:
            if ( tok.next_token().type != token::WORD )
                throw exception{"Invalid tag name"};
            elname = tok.current_token().value;
            if ( tok.next_token().type != token::CLOSE_BRACKET )
                throw exception{"Unterminated [+element] tag"};
            for ( auto i = cfg.children.rbegin(); i != cfg.children.rend(); i++ )
                if ( i->name == elname ) {
                    elements.push({*i, tok.start_line()});
                    return;
                }
            cfg.children.push_back(elname);
            elements.push({cfg.children.back(), tok.start_line()});
            break;
        case token::SLASH:
            if ( tok.next_token().type != token::WORD )
                throw exception{"Invalid closing tag name"};
            elname = tok.current_token().value;
            if ( tok.next_token().type != token::CLOSE_BRACKET )
                throw exception{"Unterminated closing tag"};
            if ( elements.size() == 1 )
                throw exception{"Unexpected closing tag"};
            if ( elname != cfg.name )
                throw exception{"Found invalid closing tag [/" + elname + "] for tag [" +
                                cfg.name + "] opened at " + std::to_string(elements.top().second) +
                                " closed at " + std::to_string(tok.start_line())};
            elements.pop();
            break;
        default:
            throw exception{"Invalid tag name"};
    }
}

void parser::parse_variable()
{
    config& cfg = elements.top().first;
    std::deque<std::string> vars{{}};
    for ( int token_type = tok.current_token().type; token_type != token::EQUALS; token_type = tok.next_token().type )
        switch ( token_type ) {
            case token::WORD:
                if ( !vars.back().empty() )
                    vars.back() += ' ';
                vars.back() += tok.current_token().value;
                break;
            case token::COMMA:
                if(vars.back().empty())
                    throw exception{"Empty variable name"};
                else
                    vars.push_back({});
                break;
            default:
                throw exception{"Unexpected characters after variable name (expected , or =)"};
                break;
        }
    if ( vars.back().empty() )
        throw exception{"Empty variable name"};

    for ( std::string var: vars )
        cfg[var] = "";

    std::string buffer;
    auto curvar = vars.begin();

    bool ignore_next_newlines = false, previous_string = false;
    for (;;) {
        switch ( tok.next_token().type ) {
            case token::COMMA:
                if ( curvar + 1 != vars.end() ) {
                    cfg[*curvar] = buffer;
                    buffer.clear();
                    curvar++;
                }
                else
                    buffer += ',';
                break;
            case token::UNDERSCORE:
                switch ( tok.next_token().type ) {
                    default:
                        buffer += '_';
                        buffer += tok.current_token().value;
                        break;
                    case token::LINE_FEED:
                    case token::END:
                        cfg[*curvar] = buffer + '_';
                        return;
                }
                break;
            case token::PLUS:
                ignore_next_newlines = true;
                continue;
            case token::UNTERMINATED_LITERAL:
                throw exception{"Unterminated literal"};
            case token::LINE_FEED:
                if ( ignore_next_newlines )
                    continue;
                [[fallthrough]];
            case token::END:
                cfg[*curvar] = buffer;
                return;
            case token::WORD:
                if ( previous_string )
                    buffer += ' ';
                [[fallthrough]];
            default:
                buffer += tok.current_token().value;
                break;
        }
        previous_string = tok.current_token().type == token::WORD;
        ignore_next_newlines = false;
    }
}
