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

#ifndef MKEYS_H
#define MKEYS_H

#include <cstdint>

/** What we really want is a mapping of every raw key on the keyboard.
 *  To support international keyboards, we use the range 0xA1 - 0xFF
 *  as international virtual keycodes.  We'll follow in the footsteps of X11...
 *  @brief The names of the keys
 */
enum MKey : std::uint32_t {
        /** @name ASCII mapped keysyms
         *  The keyboard syms have been cleverly chosen to map to ASCII
         */
        /*@{*/
        M_KEY_UNKNOWN            = 0,
        M_KEY_FIRST              = 0,
        M_KEY_BACKSPACE          = 8,
        M_KEY_TAB                = 9,
        M_KEY_CLEAR              = 12,
        M_KEY_RETURN             = 13,
        M_KEY_PAUSE              = 19,
        M_KEY_ESCAPE             = 27,
        M_KEY_SPACE              = 32,
        M_KEY_EXCLAIM            = 33,
        M_KEY_QUOTEDBL           = 34,
        M_KEY_HASH               = 35,
        M_KEY_DOLLAR             = 36,
        M_KEY_AMPERSAND          = 38,
        M_KEY_QUOTE              = 39,
        M_KEY_LEFTPAREN          = 40,
        M_KEY_RIGHTPAREN         = 41,
        M_KEY_ASTERISK           = 42,
        M_KEY_PLUS               = 43,
        M_KEY_COMMA              = 44,
        M_KEY_MINUS              = 45,
        M_KEY_PERIOD             = 46,
        M_KEY_SLASH              = 47,
        M_KEY_0                 = 48,
        M_KEY_1                 = 49,
        M_KEY_2                 = 50,
        M_KEY_3                 = 51,
        M_KEY_4                 = 52,
        M_KEY_5                 = 53,
        M_KEY_6                 = 54,
        M_KEY_7                 = 55,
        M_KEY_8                 = 56,
        M_KEY_9                 = 57,
        M_KEY_COLON              = 58,
        M_KEY_SEMICOLON          = 59,
        M_KEY_LESS               = 60,
        M_KEY_EQUALS             = 61,
        M_KEY_GREATER            = 62,
        M_KEY_QUESTION           = 63,
        M_KEY_AT                 = 64,
        /* 
           Skip uppercase letters
         */
        M_KEY_LEFTBRACKET        = 91,
        M_KEY_BACKSLASH          = 92,
        M_KEY_RIGHTBRACKET       = 93,
        M_KEY_CARET              = 94,
        M_KEY_UNDERSCORE         = 95,
        M_KEY_BACKQUOTE          = 96,
        M_KEY_a                  = 97,
        M_KEY_b                  = 98,
        M_KEY_c                  = 99,
        M_KEY_d                  = 100,
        M_KEY_e                  = 101,
        M_KEY_f                  = 102,
        M_KEY_g                  = 103,
        M_KEY_h                  = 104,
        M_KEY_i                  = 105,
        M_KEY_j                  = 106,
        M_KEY_k                  = 107,
        M_KEY_l                  = 108,
        M_KEY_m                  = 109,
        M_KEY_n                  = 110,
        M_KEY_o                  = 111,
        M_KEY_p                  = 112,
        M_KEY_q                  = 113,
        M_KEY_r                  = 114,
        M_KEY_s                  = 115,
        M_KEY_t                  = 116,
        M_KEY_u                  = 117,
        M_KEY_v                  = 118,
        M_KEY_w                  = 119,
        M_KEY_x                  = 120,
        M_KEY_y                  = 121,
        M_KEY_z                  = 122,
        M_KEY_DELETE             = 127,
        /* End of ASCII mapped keysyms */
        /*@}*/

        /** @name International keyboard syms */
        /*@{*/
        M_KEY_WORLD_0            = 160,          /* 0xA0 */
        M_KEY_WORLD_1            = 161,
        M_KEY_WORLD_2            = 162,
        M_KEY_WORLD_3            = 163,
        M_KEY_WORLD_4            = 164,
        M_KEY_WORLD_5            = 165,
        M_KEY_WORLD_6            = 166,
        M_KEY_WORLD_7            = 167,
        M_KEY_WORLD_8            = 168,
        M_KEY_WORLD_9            = 169,
        M_KEY_WORLD_10           = 170,
        M_KEY_WORLD_11           = 171,
        M_KEY_WORLD_12           = 172,
        M_KEY_WORLD_13           = 173,
        M_KEY_WORLD_14           = 174,
        M_KEY_WORLD_15           = 175,
        M_KEY_WORLD_16           = 176,
        M_KEY_WORLD_17           = 177,
        M_KEY_WORLD_18           = 178,
        M_KEY_WORLD_19           = 179,
        M_KEY_WORLD_20           = 180,
        M_KEY_WORLD_21           = 181,
        M_KEY_WORLD_22           = 182,
        M_KEY_WORLD_23           = 183,
        M_KEY_WORLD_24           = 184,
        M_KEY_WORLD_25           = 185,
        M_KEY_WORLD_26           = 186,
        M_KEY_WORLD_27           = 187,
        M_KEY_WORLD_28           = 188,
        M_KEY_WORLD_29           = 189,
        M_KEY_WORLD_30           = 190,
        M_KEY_WORLD_31           = 191,
        M_KEY_WORLD_32           = 192,
        M_KEY_WORLD_33           = 193,
        M_KEY_WORLD_34           = 194,
        M_KEY_WORLD_35           = 195,
        M_KEY_WORLD_36           = 196,
        M_KEY_WORLD_37           = 197,
        M_KEY_WORLD_38           = 198,
        M_KEY_WORLD_39           = 199,
        M_KEY_WORLD_40           = 200,
        M_KEY_WORLD_41           = 201,
        M_KEY_WORLD_42           = 202,
        M_KEY_WORLD_43           = 203,
        M_KEY_WORLD_44           = 204,
        M_KEY_WORLD_45           = 205,
        M_KEY_WORLD_46           = 206,
        M_KEY_WORLD_47           = 207,
        M_KEY_WORLD_48           = 208,
        M_KEY_WORLD_49           = 209,
        M_KEY_WORLD_50           = 210,
        M_KEY_WORLD_51           = 211,
        M_KEY_WORLD_52           = 212,
        M_KEY_WORLD_53           = 213,
        M_KEY_WORLD_54           = 214,
        M_KEY_WORLD_55           = 215,
        M_KEY_WORLD_56           = 216,
        M_KEY_WORLD_57           = 217,
        M_KEY_WORLD_58           = 218,
        M_KEY_WORLD_59           = 219,
        M_KEY_WORLD_60           = 220,
        M_KEY_WORLD_61           = 221,
        M_KEY_WORLD_62           = 222,
        M_KEY_WORLD_63           = 223,
        M_KEY_WORLD_64           = 224,
        M_KEY_WORLD_65           = 225,
        M_KEY_WORLD_66           = 226,
        M_KEY_WORLD_67           = 227,
        M_KEY_WORLD_68           = 228,
        M_KEY_WORLD_69           = 229,
        M_KEY_WORLD_70           = 230,
        M_KEY_WORLD_71           = 231,
        M_KEY_WORLD_72           = 232,
        M_KEY_WORLD_73           = 233,
        M_KEY_WORLD_74           = 234,
        M_KEY_WORLD_75           = 235,
        M_KEY_WORLD_76           = 236,
        M_KEY_WORLD_77           = 237,
        M_KEY_WORLD_78           = 238,
        M_KEY_WORLD_79           = 239,
        M_KEY_WORLD_80           = 240,
        M_KEY_WORLD_81           = 241,
        M_KEY_WORLD_82           = 242,
        M_KEY_WORLD_83           = 243,
        M_KEY_WORLD_84           = 244,
        M_KEY_WORLD_85           = 245,
        M_KEY_WORLD_86           = 246,
        M_KEY_WORLD_87           = 247,
        M_KEY_WORLD_88           = 248,
        M_KEY_WORLD_89           = 249,
        M_KEY_WORLD_90           = 250,
        M_KEY_WORLD_91           = 251,
        M_KEY_WORLD_92           = 252,
        M_KEY_WORLD_93           = 253,
        M_KEY_WORLD_94           = 254,
        M_KEY_WORLD_95           = 255,          /* 0xFF */
        /*@}*/

        /** @name Numeric keypad */
        /*@{*/
        M_KEY_KP0                = 256,
        M_KEY_KP1                = 257,
        M_KEY_KP2                = 258,
        M_KEY_KP3                = 259,
        M_KEY_KP4                = 260,
        M_KEY_KP5                = 261,
        M_KEY_KP6                = 262,
        M_KEY_KP7                = 263,
        M_KEY_KP8                = 264,
        M_KEY_KP9                = 265,
        M_KEY_KP_PERIOD          = 266,
        M_KEY_KP_DIVIDE          = 267,
        M_KEY_KP_MULTIPLY        = 268,
        M_KEY_KP_MINUS           = 269,
        M_KEY_KP_PLUS            = 270,
        M_KEY_KP_ENTER           = 271,
        M_KEY_KP_EQUALS          = 272,
        /*@}*/

        /** @name Arrows + Home/End pad */
        /*@{*/
        M_KEY_UP                 = 273,
        M_KEY_DOWN               = 274,
        M_KEY_RIGHT              = 275,
        M_KEY_LEFT               = 276,
        M_KEY_INSERT             = 277,
        M_KEY_HOME               = 278,
        M_KEY_END                = 279,
        M_KEY_PAGEUP             = 280,
        M_KEY_PAGEDOWN           = 281,
        /*@}*/

        /** @name Function keys */
        /*@{*/
        M_KEY_F1                 = 282,
        M_KEY_F2                 = 283,
        M_KEY_F3                 = 284,
        M_KEY_F4                 = 285,
        M_KEY_F5                 = 286,
        M_KEY_F6                 = 287,
        M_KEY_F7                 = 288,
        M_KEY_F8                 = 289,
        M_KEY_F9                 = 290,
        M_KEY_F10                = 291,
        M_KEY_F11                = 292,
        M_KEY_F12                = 293,
        M_KEY_F13                = 294,
        M_KEY_F14                = 295,
        M_KEY_F15                = 296,
        /*@}*/

        /** @name Key state modifier keys */
        /*@{*/
        M_KEY_NUMLOCK            = 300,
        M_KEY_CAPSLOCK           = 301,
        M_KEY_SCROLLOCK          = 302,
        M_KEY_RSHIFT             = 303,
        M_KEY_LSHIFT             = 304,
        M_KEY_RCTRL              = 305,
        M_KEY_LCTRL              = 306,
        M_KEY_RALT               = 307,
        M_KEY_LALT               = 308,
        M_KEY_RMETA              = 309,
        M_KEY_LMETA              = 310,
        M_KEY_LSUPER             = 311,          /**< Left "Windows" key */
        M_KEY_RSUPER             = 312,          /**< Right "Windows" key */
        M_KEY_MODE               = 313,          /**< "Alt Gr" key */
        M_KEY_COMPOSE            = 314,          /**< Multi-key compose key */
        /*@}*/

        /** @name Miscellaneous function keys */
        /*@{*/
        M_KEY_HELP               = 315,
        M_KEY_PRINT              = 316,
        M_KEY_SYSREQ             = 317,
        M_KEY_BREAK              = 318,
        M_KEY_MENU               = 319,
        M_KEY_POWER              = 320,          /**< Power Macintosh power key */
        M_KEY_EURO               = 321,          /**< Some european keyboards */
        M_KEY_UNDO               = 322,          /**< Atari keyboard has Undo */
        /*@}*/

        /* Add any other keys here */

        M_KEY_LAST
};

/** Enumeration of valid key mods (possibly OR'd together) */
enum class KMod {
        NONE  = 0x0000,
        LSHIFT= 0x0001,
        RSHIFT= 0x0002,
        LCTRL = 0x0040,
        RCTRL = 0x0080,
        LALT  = 0x0100,
        RALT  = 0x0200,
        LMETA = 0x0400,
        RMETA = 0x0800,
        NUM   = 0x1000,
        CAPS  = 0x2000,
        MODE  = 0x4000,
        RESERVED = 0x8000,

        CTRL = LCTRL | RCTRL,
        SHIFT = LSHIFT | RSHIFT,
        ALT = LALT | RALT,
        META = LMETA | RMETA
};


#endif // MKEYS_H
