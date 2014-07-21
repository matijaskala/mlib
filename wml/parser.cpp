/*
 * <one line to give the program's name and a brief idea of what it does.>
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

#include "parser.h"

using namespace wml;

exception::exception ( std::string message )
    : message(message) {}

const char* exception::what() const noexcept
{
    return message.c_str();
}

parser::parser ( std::string file )
    : tok ( file )
{

}

parser::~parser()
{

}

void parser::operator() ( config& cfg )
{
    cfg.clear();
    elements.push ( element ( cfg, "" ) );

    do {
        switch ( tok.next_token().type ) {
            case token::LINE_FEED:
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
        throw exception ( "Missing closing tag for tag [$tag]" );
}

void parser::parse_element()
{
    std::string elname;
    config& cfg = elements.top().cfg;
    switch ( tok.next_token().type ) {
        case token::WORD:
            elname = tok.current_token().value;
            if ( tok.next_token().type != token::CLOSE_BRACKET )
                throw exception ( "Unterminated [element] tag" );
            cfg.children.push_back ( config ( elname ) );
            elements.push ( element ( cfg.children.back(), elname, tok.start_line() ) );
            break;
        case token::PLUS:
            if ( tok.next_token().type != token::WORD )
                throw exception ( "Invalid tag name" );
            elname = tok.current_token().value;
            if ( tok.next_token().type != token::CLOSE_BRACKET )
                throw exception ( "Unterminated [+element] tag" );
            for ( auto i = cfg.children.rbegin(); i != cfg.children.rend(); i++ )
                if ( i->name == elname ) {
                    elements.push ( element ( *i, elname, tok.start_line() ) );
                    return;
                }
            cfg.children.push_back ( config ( elname ) );
            elements.push ( element ( cfg.children.back(), elname, tok.start_line() ) );
            break;
        case token::SLASH:
            if ( tok.next_token().type != token::WORD )
                throw exception ( "Invalid closing tag name" );
            elname = tok.current_token().value;
            if ( tok.next_token().type != token::CLOSE_BRACKET )
                throw exception ( "Unterminated closing tag" );
            if ( elements.size() == 1 )
                throw exception ( "Unexpected closing tag" );
            if ( elname != cfg.name ) {
                std::map<std::string, std::string> i18n_symbols;
                i18n_symbols["tag1"] = elements.top().name;
                i18n_symbols["tag2"] = elname;
                i18n_symbols["pos1"] = elements.top().start_line;
                i18n_symbols["pos2"] = tok.start_line();
                throw exception ( "Found invalid closing tag [/$tag2] for tag [$tag1] opened at $pos1 closed at $pos2" );
            }
            elements.pop();
            break;
        default:
            throw exception ( "Invalid tag name" );
    }
}

void parser::parse_variable()
{
    config& cfg = elements.top().cfg;
    std::deque<std::string> vars(1);
    for ( int token_type = tok.current_token().type; token_type != token::EQUALS; token_type = tok.next_token().type )
        switch ( token_type ) {
            case token::WORD:
                if ( !vars.back().empty() )
                    vars.back() += ' ';
                vars.back() += tok.current_token().value;
                break;
            case token::COMMA:
                if(vars.back().empty())
                    throw exception ( "Empty variable name" );
                else
                    vars.push_back("");
                break;
            default:
                throw exception ( "Unexpected characters after variable name (expected , or =)" );
                break;
        }
    if ( vars.back().empty() )
        throw exception ( "Empty variable name" );

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
                        buffer += '_';
                }
                break;
            case token::PLUS:
                ignore_next_newlines = true;
                continue;
            case token::UNTERMINATED_LITERAL:
                throw exception ( "Unterminated literal" );
            case token::LINE_FEED:
                if ( ignore_next_newlines )
                    continue;
            case token::END:
                cfg[*curvar] = buffer;
                return;
            case token::WORD:
                if ( previous_string )
                    buffer += ' ';
            default:
                buffer += tok.current_token().value;
                break;
        }
        previous_string = tok.current_token().type == token::WORD;
        ignore_next_newlines = false;
    }
}
