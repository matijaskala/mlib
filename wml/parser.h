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

#ifndef WML_PARSER_H
#define WML_PARSER_H

#include <stack>
#include "tokenizer.h"
#include "config.h"

namespace wml
{

struct exception : public std::exception
{
    std::string message;
    exception ( std::string message );
    virtual const char* what() const noexcept override;
};

class parser
{
public:
    parser ( std::string file );
    ~parser();
    tokenizer tok;
    void operator() ( config& cfg );
private:
    void parse_element();
    void parse_variable();
    struct element {
        element ( config& cfg, std::string name, int start_line = 0 )
            : cfg ( cfg ), name ( name ), start_line ( start_line ) {}
        config& cfg;
        std::string name;
        int start_line;
    };
    std::stack<element> elements;
};
}

#endif // WML_PARSER_H
