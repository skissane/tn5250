/* 
 * Copyright (C) 2003 Marc-Antoine Ruel at Cardinal Health
 * 
 * This file is part of TN5250
 *
 * TN5250 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or (at your option)
 * any later version.
 * 
 * TN5250 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA
 *
 */
// Some structures and defines

#pragma once


#define TN5250_WNDCLASS "tn5250-win32-terminal"
#define WM_TN5250_STREAM_DATA (WM_USER+2000)
#define WM_TN5250_KEY_DATA (WM_USER+2001)

enum EColSepStyle
{
    COLSEPSTYLE_NONE,
    COLSEPSTYLE_FULL,
    COLSEPSTYLE_DOTS
};

enum ECopyMode
{
    Both,
    Text,
    Picture
};

// Colors
#define A_5250_GREEN		0
#define A_5250_WHITE		1
#define A_5250_RED		2
#define A_5250_TURQ		3
#define A_5250_YELLOW		4
#define A_5250_PINK		5
#define A_5250_BLUE		6
#define A_5250_BLACK		7
#define A_5250_RULER_COLOR	8
#define A_5250_STATUS_COLOR	9
#define A_5250_NB_COLORS	10


// Styles
#define A_UNDERLINE  0x01
#define A_BLINK      0x02
#define A_NONDISPLAY 0x04
#define A_VERTICAL   0x08
#define A_REVERSE    0x10


struct Tn5250Win32Attribute
{
    int colorindex;
    UINT flags;
};

struct keystroke_to_msg
{
    SHORT keystate;
    DWORD win32_key;
    DWORD func_key;
    BYTE   ctx;
    BYTE   ext;
};

struct win32_key_map
{
    DWORD win32_key;
    DWORD tn5250_key;
};
