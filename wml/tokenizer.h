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

#ifndef WML_TOKENIZER_H
#define WML_TOKENIZER_H

#include <MGlobal>
#include <fstream>

namespace wml
{

struct token
{
    enum {
        WORD,
        LITERAL,
        UNTERMINATED_LITERAL,
        MISC,

        LINE_FEED = '\n',
        EQUALS = '=',
        COMMA = ',',
        PLUS = '+',
        SLASH = '/',
        OPEN_BRACKET = '[',
        CLOSE_BRACKET = ']',
        UNDERSCORE = '_',
        COMMENT = '#',
        END
    };

    int type = END;
    std::string value = "";
};

class M_EXPORT tokenizer
{
    using traits = std::char_traits< char >;

public:
    tokenizer ( std::string file )
        : stream{file}, next{stream.get()} {
        next_char();
    }
    const token& next_token();
    const token& current_token() const {
        return curr_token;
    }
    const token& previous_token() const {
        return prev_token;
    }
    int start_line() const {
        return startlineno;
    }

private:
    token curr_token{};
    token prev_token{};
    int lineno = 1;
    int startlineno = 0;

    void next_char();
    char buffer[0x400];
    std::uint16_t buffer_offset = 0;
    std::uint16_t buffer_length = 0;
    std::ifstream stream;
    int current = EOF;
    int next;
};
}

#endif // WML_TOKENIZER_H
