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

/** What we really want is a mapping of every raw key on the keyboard.
 *  To support international keyboards, we use the range 0xA1 - 0xFF
 *  as international virtual keycodes.  We'll follow in the footsteps of X11...
 *  @brief The names of the keys
 */
typedef enum {
        /** @name ASCII mapped keysyms
         *  The keyboard syms have been cleverly chosen to map to ASCII
         */
        /*@{*/
        MKEY_UNKNOWN            = 0,
        MKEY_FIRST              = 0,
        MKEY_BACKSPACE          = 8,
        MKEY_TAB                = 9,
        MKEY_CLEAR              = 12,
        MKEY_RETURN             = 13,
        MKEY_PAUSE              = 19,
        MKEY_ESCAPE             = 27,
        MKEY_SPACE              = 32,
        MKEY_EXCLAIM            = 33,
        MKEY_QUOTEDBL           = 34,
        MKEY_HASH               = 35,
        MKEY_DOLLAR             = 36,
        MKEY_AMPERSAND          = 38,
        MKEY_QUOTE              = 39,
        MKEY_LEFTPAREN          = 40,
        MKEY_RIGHTPAREN         = 41,
        MKEY_ASTERISK           = 42,
        MKEY_PLUS               = 43,
        MKEY_COMMA              = 44,
        MKEY_MINUS              = 45,
        MKEY_PERIOD             = 46,
        MKEY_SLASH              = 47,
        MKEY_0                  = 48,
        MKEY_1                  = 49,
        MKEY_2                  = 50,
        MKEY_3                  = 51,
        MKEY_4                  = 52,
        MKEY_5                  = 53,
        MKEY_6                  = 54,
        MKEY_7                  = 55,
        MKEY_8                  = 56,
        MKEY_9                  = 57,
        MKEY_COLON              = 58,
        MKEY_SEMICOLON          = 59,
        MKEY_LESS               = 60,
        MKEY_EQUALS             = 61,
        MKEY_GREATER            = 62,
        MKEY_QUESTION           = 63,
        MKEY_AT                 = 64,
        /* 
           Skip uppercase letters
         */
        MKEY_LEFTBRACKET        = 91,
        MKEY_BACKSLASH          = 92,
        MKEY_RIGHTBRACKET       = 93,
        MKEY_CARET              = 94,
        MKEY_UNDERSCORE         = 95,
        MKEY_BACKQUOTE          = 96,
        MKEY_a                  = 97,
        MKEY_b                  = 98,
        MKEY_c                  = 99,
        MKEY_d                  = 100,
        MKEY_e                  = 101,
        MKEY_f                  = 102,
        MKEY_g                  = 103,
        MKEY_h                  = 104,
        MKEY_i                  = 105,
        MKEY_j                  = 106,
        MKEY_k                  = 107,
        MKEY_l                  = 108,
        MKEY_m                  = 109,
        MKEY_n                  = 110,
        MKEY_o                  = 111,
        MKEY_p                  = 112,
        MKEY_q                  = 113,
        MKEY_r                  = 114,
        MKEY_s                  = 115,
        MKEY_t                  = 116,
        MKEY_u                  = 117,
        MKEY_v                  = 118,
        MKEY_w                  = 119,
        MKEY_x                  = 120,
        MKEY_y                  = 121,
        MKEY_z                  = 122,
        MKEY_DELETE             = 127,
        /* End of ASCII mapped keysyms */
        /*@}*/

        /** @name International keyboard syms */
        /*@{*/
        MKEY_WORLD_0            = 160,          /* 0xA0 */
        MKEY_WORLD_1            = 161,
        MKEY_WORLD_2            = 162,
        MKEY_WORLD_3            = 163,
        MKEY_WORLD_4            = 164,
        MKEY_WORLD_5            = 165,
        MKEY_WORLD_6            = 166,
        MKEY_WORLD_7            = 167,
        MKEY_WORLD_8            = 168,
        MKEY_WORLD_9            = 169,
        MKEY_WORLD_10           = 170,
        MKEY_WORLD_11           = 171,
        MKEY_WORLD_12           = 172,
        MKEY_WORLD_13           = 173,
        MKEY_WORLD_14           = 174,
        MKEY_WORLD_15           = 175,
        MKEY_WORLD_16           = 176,
        MKEY_WORLD_17           = 177,
        MKEY_WORLD_18           = 178,
        MKEY_WORLD_19           = 179,
        MKEY_WORLD_20           = 180,
        MKEY_WORLD_21           = 181,
        MKEY_WORLD_22           = 182,
        MKEY_WORLD_23           = 183,
        MKEY_WORLD_24           = 184,
        MKEY_WORLD_25           = 185,
        MKEY_WORLD_26           = 186,
        MKEY_WORLD_27           = 187,
        MKEY_WORLD_28           = 188,
        MKEY_WORLD_29           = 189,
        MKEY_WORLD_30           = 190,
        MKEY_WORLD_31           = 191,
        MKEY_WORLD_32           = 192,
        MKEY_WORLD_33           = 193,
        MKEY_WORLD_34           = 194,
        MKEY_WORLD_35           = 195,
        MKEY_WORLD_36           = 196,
        MKEY_WORLD_37           = 197,
        MKEY_WORLD_38           = 198,
        MKEY_WORLD_39           = 199,
        MKEY_WORLD_40           = 200,
        MKEY_WORLD_41           = 201,
        MKEY_WORLD_42           = 202,
        MKEY_WORLD_43           = 203,
        MKEY_WORLD_44           = 204,
        MKEY_WORLD_45           = 205,
        MKEY_WORLD_46           = 206,
        MKEY_WORLD_47           = 207,
        MKEY_WORLD_48           = 208,
        MKEY_WORLD_49           = 209,
        MKEY_WORLD_50           = 210,
        MKEY_WORLD_51           = 211,
        MKEY_WORLD_52           = 212,
        MKEY_WORLD_53           = 213,
        MKEY_WORLD_54           = 214,
        MKEY_WORLD_55           = 215,
        MKEY_WORLD_56           = 216,
        MKEY_WORLD_57           = 217,
        MKEY_WORLD_58           = 218,
        MKEY_WORLD_59           = 219,
        MKEY_WORLD_60           = 220,
        MKEY_WORLD_61           = 221,
        MKEY_WORLD_62           = 222,
        MKEY_WORLD_63           = 223,
        MKEY_WORLD_64           = 224,
        MKEY_WORLD_65           = 225,
        MKEY_WORLD_66           = 226,
        MKEY_WORLD_67           = 227,
        MKEY_WORLD_68           = 228,
        MKEY_WORLD_69           = 229,
        MKEY_WORLD_70           = 230,
        MKEY_WORLD_71           = 231,
        MKEY_WORLD_72           = 232,
        MKEY_WORLD_73           = 233,
        MKEY_WORLD_74           = 234,
        MKEY_WORLD_75           = 235,
        MKEY_WORLD_76           = 236,
        MKEY_WORLD_77           = 237,
        MKEY_WORLD_78           = 238,
        MKEY_WORLD_79           = 239,
        MKEY_WORLD_80           = 240,
        MKEY_WORLD_81           = 241,
        MKEY_WORLD_82           = 242,
        MKEY_WORLD_83           = 243,
        MKEY_WORLD_84           = 244,
        MKEY_WORLD_85           = 245,
        MKEY_WORLD_86           = 246,
        MKEY_WORLD_87           = 247,
        MKEY_WORLD_88           = 248,
        MKEY_WORLD_89           = 249,
        MKEY_WORLD_90           = 250,
        MKEY_WORLD_91           = 251,
        MKEY_WORLD_92           = 252,
        MKEY_WORLD_93           = 253,
        MKEY_WORLD_94           = 254,
        MKEY_WORLD_95           = 255,          /* 0xFF */
        /*@}*/

        /** @name Numeric keypad */
        /*@{*/
        MKEY_KP0                = 256,
        MKEY_KP1                = 257,
        MKEY_KP2                = 258,
        MKEY_KP3                = 259,
        MKEY_KP4                = 260,
        MKEY_KP5                = 261,
        MKEY_KP6                = 262,
        MKEY_KP7                = 263,
        MKEY_KP8                = 264,
        MKEY_KP9                = 265,
        MKEY_KP_PERIOD          = 266,
        MKEY_KP_DIVIDE          = 267,
        MKEY_KP_MULTIPLY        = 268,
        MKEY_KP_MINUS           = 269,
        MKEY_KP_PLUS            = 270,
        MKEY_KP_ENTER           = 271,
        MKEY_KP_EQUALS          = 272,
        /*@}*/

        /** @name Arrows + Home/End pad */
        /*@{*/
        MKEY_UP                 = 273,
        MKEY_DOWN               = 274,
        MKEY_RIGHT              = 275,
        MKEY_LEFT               = 276,
        MKEY_INSERT             = 277,
        MKEY_HOME               = 278,
        MKEY_END                = 279,
        MKEY_PAGEUP             = 280,
        MKEY_PAGEDOWN           = 281,
        /*@}*/

        /** @name Function keys */
        /*@{*/
        MKEY_F1                 = 282,
        MKEY_F2                 = 283,
        MKEY_F3                 = 284,
        MKEY_F4                 = 285,
        MKEY_F5                 = 286,
        MKEY_F6                 = 287,
        MKEY_F7                 = 288,
        MKEY_F8                 = 289,
        MKEY_F9                 = 290,
        MKEY_F10                = 291,
        MKEY_F11                = 292,
        MKEY_F12                = 293,
        MKEY_F13                = 294,
        MKEY_F14                = 295,
        MKEY_F15                = 296,
        /*@}*/

        /** @name Key state modifier keys */
        /*@{*/
        MKEY_NUMLOCK            = 300,
        MKEY_CAPSLOCK           = 301,
        MKEY_SCROLLOCK          = 302,
        MKEY_RSHIFT             = 303,
        MKEY_LSHIFT             = 304,
        MKEY_RCTRL              = 305,
        MKEY_LCTRL              = 306,
        MKEY_RALT               = 307,
        MKEY_LALT               = 308,
        MKEY_RMETA              = 309,
        MKEY_LMETA              = 310,
        MKEY_LSUPER             = 311,          /**< Left "Windows" key */
        MKEY_RSUPER             = 312,          /**< Right "Windows" key */
        MKEY_MODE               = 313,          /**< "Alt Gr" key */
        MKEY_COMPOSE            = 314,          /**< Multi-key compose key */
        /*@}*/

        /** @name Miscellaneous function keys */
        /*@{*/
        MKEY_HELP               = 315,
        MKEY_PRINT              = 316,
        MKEY_SYSREQ             = 317,
        MKEY_BREAK              = 318,
        MKEY_MENU               = 319,
        MKEY_POWER              = 320,          /**< Power Macintosh power key */
        MKEY_EURO               = 321,          /**< Some european keyboards */
        MKEY_UNDO               = 322,          /**< Atari keyboard has Undo */
        /*@}*/

        /* Add any other keys here */

        MKEY_LAST
} MKey;

/** Enumeration of valid key mods (possibly OR'd together) */
typedef enum {
        KMOD_NONE  = 0x0000,
        KMOD_LSHIFT= 0x0001,
        KMOD_RSHIFT= 0x0002,
        KMOD_LCTRL = 0x0040,
        KMOD_RCTRL = 0x0080,
        KMOD_LALT  = 0x0100,
        KMOD_RALT  = 0x0200,
        KMOD_LMETA = 0x0400,
        KMOD_RMETA = 0x0800,
        KMOD_NUM   = 0x1000,
        KMOD_CAPS  = 0x2000,
        KMOD_MODE  = 0x4000,
        KMOD_RESERVED = 0x8000
} KMod;

#define KMOD_CTRL       (KMOD_LCTRL|KMOD_RCTRL)
#define KMOD_SHIFT      (KMOD_LSHIFT|KMOD_RSHIFT)
#define KMOD_ALT        (KMOD_LALT|KMOD_RALT)
#define KMOD_META       (KMOD_LMETA|KMOD_RMETA)


#endif // MKEYS_H
