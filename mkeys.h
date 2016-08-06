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
#include <xkbcommon/xkbcommon.h>

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
        M_KEY_SPACE              = XKB_KEY_space,
        M_KEY_EXCLAIM            = XKB_KEY_exclam,
        M_KEY_QUOTEDBL           = XKB_KEY_quotedbl,
        M_KEY_HASH               = XKB_KEY_numbersign,
        M_KEY_DOLLAR             = XKB_KEY_dollar,
        M_KEY_AMPERSAND          = XKB_KEY_ampersand,
        M_KEY_QUOTE              = XKB_KEY_apostrophe,
        M_KEY_LEFTPAREN          = XKB_KEY_parenleft,
        M_KEY_RIGHTPAREN         = XKB_KEY_parenright,
        M_KEY_ASTERISK           = XKB_KEY_asterisk,
        M_KEY_PLUS               = XKB_KEY_plus,
        M_KEY_COMMA              = XKB_KEY_comma,
        M_KEY_MINUS              = XKB_KEY_minus,
        M_KEY_PERIOD             = XKB_KEY_period,
        M_KEY_SLASH              = XKB_KEY_slash,
        M_KEY_0                 = XKB_KEY_0,
        M_KEY_1                 = XKB_KEY_1,
        M_KEY_2                 = XKB_KEY_2,
        M_KEY_3                 = XKB_KEY_3,
        M_KEY_4                 = XKB_KEY_4,
        M_KEY_5                 = XKB_KEY_5,
        M_KEY_6                 = XKB_KEY_6,
        M_KEY_7                 = XKB_KEY_7,
        M_KEY_8                 = XKB_KEY_8,
        M_KEY_9                 = XKB_KEY_9,
        M_KEY_COLON              = XKB_KEY_colon,
        M_KEY_SEMICOLON          = XKB_KEY_semicolon,
        M_KEY_LESS               = XKB_KEY_less,
        M_KEY_EQUALS             = XKB_KEY_equal,
        M_KEY_GREATER            = XKB_KEY_greater,
        M_KEY_QUESTION           = XKB_KEY_question,
        M_KEY_AT                 = XKB_KEY_at,
        /* 
           Skip uppercase letters
         */
        M_KEY_LEFTBRACKET        = XKB_KEY_bracketleft,
        M_KEY_BACKSLASH          = XKB_KEY_backslash,
        M_KEY_RIGHTBRACKET       = XKB_KEY_bracketright,
        M_KEY_CARET              = XKB_KEY_asciicircum,
        M_KEY_UNDERSCORE         = XKB_KEY_underscore,
        M_KEY_BACKQUOTE          = XKB_KEY_grave,
        M_KEY_a                  = XKB_KEY_a,
        M_KEY_b                  = XKB_KEY_b,
        M_KEY_c                  = XKB_KEY_c,
        M_KEY_d                  = XKB_KEY_d,
        M_KEY_e                  = XKB_KEY_e,
        M_KEY_f                  = XKB_KEY_f,
        M_KEY_g                  = XKB_KEY_g,
        M_KEY_h                  = XKB_KEY_h,
        M_KEY_i                  = XKB_KEY_i,
        M_KEY_j                  = XKB_KEY_j,
        M_KEY_k                  = XKB_KEY_k,
        M_KEY_l                  = XKB_KEY_l,
        M_KEY_m                  = XKB_KEY_m,
        M_KEY_n                  = XKB_KEY_n,
        M_KEY_o                  = XKB_KEY_o,
        M_KEY_p                  = XKB_KEY_p,
        M_KEY_q                  = XKB_KEY_q,
        M_KEY_r                  = XKB_KEY_r,
        M_KEY_s                  = XKB_KEY_s,
        M_KEY_t                  = XKB_KEY_t,
        M_KEY_u                  = XKB_KEY_u,
        M_KEY_v                  = XKB_KEY_v,
        M_KEY_w                  = XKB_KEY_w,
        M_KEY_x                  = XKB_KEY_x,
        M_KEY_y                  = XKB_KEY_y,
        M_KEY_z                  = XKB_KEY_z,
        M_KEY_GRAVE              = XKB_KEY_bar,
        M_KEY_DELETE             = 127,
        /* End of ASCII mapped keysyms */
        /*@}*/

        /** @name International keyboard syms */
        /*@{*/
        M_KEY_WORLD_0            = XKB_KEY_nobreakspace,          /* 0xA0 */
        M_KEY_WORLD_1            = XKB_KEY_exclamdown,
        M_KEY_WORLD_2            = XKB_KEY_cent,
        M_KEY_WORLD_3            = XKB_KEY_sterling,
        M_KEY_WORLD_4            = XKB_KEY_currency,
        M_KEY_WORLD_5            = XKB_KEY_yen,
        M_KEY_WORLD_6            = XKB_KEY_brokenbar,
        M_KEY_WORLD_7            = XKB_KEY_section,
        M_KEY_WORLD_8            = XKB_KEY_diaeresis,
        M_KEY_WORLD_9            = XKB_KEY_copyright,
        M_KEY_WORLD_10           = XKB_KEY_ordfeminine,
        M_KEY_WORLD_11           = XKB_KEY_guillemotleft,
        M_KEY_WORLD_12           = XKB_KEY_notsign,
        M_KEY_WORLD_13           = XKB_KEY_hyphen,
        M_KEY_WORLD_14           = XKB_KEY_registered,
        M_KEY_WORLD_15           = XKB_KEY_macron,
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
        M_KEY_WORLD_95           = 255,
        /*@}*/

        /** @name Numeric keypad */
        /*@{*/
        M_KEY_KP0                = XKB_KEY_KP_0,
        M_KEY_KP1                = XKB_KEY_KP_1,
        M_KEY_KP2                = XKB_KEY_KP_2,
        M_KEY_KP3                = XKB_KEY_KP_3,
        M_KEY_KP4                = XKB_KEY_KP_4,
        M_KEY_KP5                = XKB_KEY_KP_5,
        M_KEY_KP6                = XKB_KEY_KP_6,
        M_KEY_KP7                = XKB_KEY_KP_7,
        M_KEY_KP8                = XKB_KEY_KP_8,
        M_KEY_KP9                = XKB_KEY_KP_9,
        M_KEY_KP_PERIOD          = XKB_KEY_KP_Decimal,
        M_KEY_KP_DIVIDE          = XKB_KEY_KP_Divide,
        M_KEY_KP_MULTIPLY        = XKB_KEY_KP_Multiply,
        M_KEY_KP_MINUS           = XKB_KEY_KP_Subtract,
        M_KEY_KP_PLUS            = XKB_KEY_KP_Add,
        M_KEY_KP_ENTER           = XKB_KEY_KP_Enter,
        M_KEY_KP_EQUALS          = XKB_KEY_KP_Equal,
        /*@}*/

        /** @name Arrows + Home/End pad */
        /*@{*/
        M_KEY_UP                 = XKB_KEY_Up,
        M_KEY_DOWN               = XKB_KEY_Down,
        M_KEY_RIGHT              = XKB_KEY_Right,
        M_KEY_LEFT               = XKB_KEY_Left,
        M_KEY_INSERT             = XKB_KEY_Insert,
        M_KEY_HOME               = XKB_KEY_Home,
        M_KEY_END                = XKB_KEY_End,
        M_KEY_PAGEUP             = XKB_KEY_Page_Up,
        M_KEY_PAGEDOWN           = XKB_KEY_Page_Down,
        /*@}*/

        /** @name Function keys */
        /*@{*/
        M_KEY_F1                 = XKB_KEY_F1,
        M_KEY_F2                 = XKB_KEY_F2,
        M_KEY_F3                 = XKB_KEY_F3,
        M_KEY_F4                 = XKB_KEY_F4,
        M_KEY_F5                 = XKB_KEY_F5,
        M_KEY_F6                 = XKB_KEY_F6,
        M_KEY_F7                 = XKB_KEY_F7,
        M_KEY_F8                 = XKB_KEY_F8,
        M_KEY_F9                 = XKB_KEY_F9,
        M_KEY_F10                = XKB_KEY_F10,
        M_KEY_F11                = XKB_KEY_F11,
        M_KEY_F12                = XKB_KEY_F12,
        M_KEY_F13                = XKB_KEY_F13,
        M_KEY_F14                = XKB_KEY_F14,
        M_KEY_F15                = XKB_KEY_F15,
        M_KEY_F16                = XKB_KEY_F16,
        M_KEY_F17,
        M_KEY_F18,
        M_KEY_F19,
        M_KEY_F20,
        M_KEY_F21,
        M_KEY_F22,
        M_KEY_F23,
        M_KEY_F24,
        /*@}*/

        /** @name Key state modifier keys */
        /*@{*/
        M_KEY_NUMLOCK = XKB_KEY_Num_Lock,
        M_KEY_CAPSLOCK = XKB_KEY_Caps_Lock,
        M_KEY_SCROLLOCK = XKB_KEY_Scroll_Lock,
        M_KEY_SCROLLLOCK = M_KEY_SCROLLOCK,
        M_KEY_RSHIFT = XKB_KEY_Shift_R,
        M_KEY_LSHIFT = XKB_KEY_Shift_L,
        M_KEY_RCTRL = XKB_KEY_Control_R,
        M_KEY_LCTRL = XKB_KEY_Control_L,
        M_KEY_RALT = XKB_KEY_Alt_R,
        M_KEY_LALT = XKB_KEY_Alt_L,
        M_KEY_RMETA = XKB_KEY_Meta_R,
        M_KEY_LMETA = XKB_KEY_Meta_L,
        M_KEY_LSUPER = XKB_KEY_Super_L,
        M_KEY_RSUPER = XKB_KEY_Super_R,
        M_KEY_MODE = XKB_KEY_Mode_switch,
        M_KEY_ALTGR = M_KEY_MODE,
        M_KEY_COMPOSE = XKB_KEY_Multi_key,
        M_KEY_SELECT = XKB_KEY_Select,
        M_KEY_EXECUTE = XKB_KEY_Execute,
        /*@}*/

        /** @name Miscellaneous function keys */
        /*@{*/
        M_KEY_HELP = XKB_KEY_Help,
        M_KEY_PRINT = XKB_KEY_Print,
        M_KEY_SYSREQ = XKB_KEY_Sys_Req,
        M_KEY_BREAK = XKB_KEY_Break,
        M_KEY_MENU = XKB_KEY_Menu,
        M_KEY_EURO = XKB_KEY_EuroSign,
        M_KEY_UNDO = XKB_KEY_Undo,
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

inline bool m_key_is_character ( MKey key ) {
    return ( key >= 32 && key < 127 ) || ( key >= 0xa0 && key < 0xf000);
}

inline wchar_t m_key_to_wchar ( MKey key ) {
    return xkb_keysym_to_utf32 ( key );
}

#endif // MKEYS_H
