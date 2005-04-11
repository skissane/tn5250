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
// Should have been named Tables.h...
#pragma once
#include <utility.h>

#define TN5250_WNDCLASS "tn5250-win32-terminal"
#define WM_TN5250_STREAM_DATA (WM_USER+1)
#define WM_TN5250_KEY_DATA (WM_USER+2)


enum ECopyMode
{
    Both,
    Text,
    Picture
};

// Colors
enum
{
    A_5250_GREEN	,
    A_5250_WHITE	,
    A_5250_RED		,
    A_5250_TURQ		,
    A_5250_YELLOW	,
    A_5250_PINK		,
    A_5250_BLUE		,
    A_5250_BLACK	,
    A_5250_RULER_COLOR	,
    A_5250_STATUS_COLOR	,
    A_5250_NB_COLORS	
};

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
    int   Modifiers;
    int   win32_key;
    int   tn5250_key;
};

struct win32_modifier_map {
    int Modifier;
    int VirtualKey;
};

struct SimilarKey {
    int key1;
    int key2;
};

// externs
extern const Tn5250Win32Attribute attribute_map[];
extern const keystroke_to_msg key2msg[];
extern const SimilarKey k_SimilarKeys[];
extern Tn5250CharMap IdentityMap;
extern const size_t attribute_map_size;
extern const size_t key2msg_size;
extern const size_t k_ModifierToVK_size;
extern const size_t k_SimilarKeys_size;
