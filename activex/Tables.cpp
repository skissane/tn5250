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
// Some CONST global variables.

#include "stdafx.h"
#include "Term5250.h"
#include "Term.h"


const char About[] =
    PACKAGE " version " VERSION ":\n"
    "Copyright (C) 1997-2003 by Michael Madore,"
    " Jason M. Felice, and Scott Klement\n"
    " and a bit Marc-Antoine Ruel\n"
    "\n"
    "Portions of this software were contributed "
    "by many people.  See the AUTHORS.txt file for\n"
    "details.\n"
    "\n"
    "For license information, see the COPYING.txt file "
    "that was installed with this software.\n"
    "\n"
    "OpenSSL:\n"
    "This product includes software developed by the "
    "OpenSSL Project for use in the OpenSSL Toolkit "
    "(http://www.openssl.org/)\n"
    "This product includes cryptographic software "
    "written by Eric Young (eay@crypsoft.com).\n"
    "This product includes software written by Tim "
    "Hudson (tjh@cryptsoft.com).\n"
    "\n"
    "For OpenSSL license information, see the "
    "COPYING.txt file that was installed with "
    "this software.";


// The first column is automatically set by the colorlist[colorindex].ref values
const Tn5250Win32Attribute attribute_map[] = 
{
    { A_5250_GREEN, 0 },
    { A_5250_GREEN, A_REVERSE },
    { A_5250_WHITE, 0 },
    { A_5250_WHITE, A_REVERSE },
    { A_5250_GREEN, A_UNDERLINE },
    { A_5250_GREEN, A_REVERSE|A_UNDERLINE },
    { A_5250_WHITE, A_UNDERLINE },
    { A_5250_BLACK, A_NONDISPLAY },
    { A_5250_RED,   0 },
    { A_5250_RED,   A_REVERSE },
    { A_5250_RED,   A_BLINK },
    { A_5250_RED,   A_REVERSE|A_BLINK },
    { A_5250_RED,   A_UNDERLINE },
    { A_5250_RED,   A_REVERSE|A_UNDERLINE },
    { A_5250_RED,   A_UNDERLINE|A_BLINK },
    { A_5250_BLACK, A_NONDISPLAY },
    { A_5250_TURQ,  A_VERTICAL },
    { A_5250_TURQ,  A_REVERSE|A_VERTICAL },
    { A_5250_YELLOW,A_VERTICAL },
    { A_5250_YELLOW,A_REVERSE|A_VERTICAL },
    { A_5250_TURQ,  A_VERTICAL|A_UNDERLINE },
    { A_5250_TURQ,  A_REVERSE|A_VERTICAL|A_UNDERLINE },
    { A_5250_YELLOW,A_VERTICAL|A_UNDERLINE },
    { A_5250_BLACK, A_REVERSE|A_NONDISPLAY },
    { A_5250_PINK,  0 },
    { A_5250_PINK,  A_REVERSE },
    { A_5250_BLUE,  0 },
    { A_5250_BLUE,  A_REVERSE },
    { A_5250_PINK,  A_UNDERLINE },
    { A_5250_PINK,  A_REVERSE|A_UNDERLINE },
    { A_5250_BLUE,  A_UNDERLINE },
    { A_5250_BLACK, A_NONDISPLAY },
    { (DWORD)-1, (DWORD)-1 },
};


// This array converts windows "keystrokes" into character messages
// for those that aren't handled by the "TranslateMessage" function.
//
// The smaller this array is, the better the keyboard handling will
// perform :)
const keystroke_to_msg keydown2msg[] =
{
//  KeyState    Win32 VirtKey    5250 key   ctx ext
   { VK_SHIFT,   VK_TAB,          K_BACKTAB, 0, 0 },
   { VK_SHIFT,   VK_F1,           K_F13    , 0, 0 },   
   { VK_SHIFT,   VK_F2,           K_F14    , 0, 0 },   
   { VK_SHIFT,   VK_F3,           K_F15    , 0, 0 },   
   { VK_SHIFT,   VK_F4,           K_F16    , 0, 0 },   
   { VK_SHIFT,   VK_F5,           K_F17    , 0, 0 },   
   { VK_SHIFT,   VK_F6,           K_F18    , 0, 0 },   
   { VK_SHIFT,   VK_F7,           K_F19    , 0, 0 },   
   { VK_SHIFT,   VK_F8,           K_F20    , 0, 0 },   
   { VK_SHIFT,   VK_F9,           K_F21    , 0, 0 },   
   { VK_SHIFT,   VK_F10,          K_F22    , 0, 0 },   
   { VK_SHIFT,   VK_F11,          K_F23    , 0, 0 },   
   { VK_SHIFT,   VK_F12,          K_F24    , 0, 0 },   
   { VK_SHIFT,   VK_INSERT,       K_PASTE_TEXT, 0, 1 },   
   { VK_SHIFT,   VK_DELETE,       K_COPY_TEXT, 0, 1 },   
   { VK_CONTROL, VK_LEFT,         K_PREVFLD, 0, 1 },
   { VK_CONTROL, VK_RIGHT,        K_NEXTFLD, 0, 1 },
   { VK_SHIFT,    K_ENTER,        K_NEWLINE, 0, 1 },
   { 0,          VK_F1,           K_F1     , 0, 0 },   
   { 0,          VK_F2,           K_F2     , 0, 0 },   
   { 0,          VK_F3,           K_F3     , 0, 0 },   
   { 0,          VK_F4,           K_F4     , 0, 0 },   
   { 0,          VK_F5,           K_F5     , 0, 0 },   
   { 0,          VK_F6,           K_F6     , 0, 0 },   
   { 0,          VK_F7,           K_F7     , 0, 0 },   
   { 0,          VK_F8,           K_F8     , 0, 0 },   
   { 0,          VK_F9,           K_F9     , 0, 0 },   
   { 0,          VK_F10,          K_F10    , 0, 0 },   
   { 0,          VK_F11,          K_F11    , 0, 0 },   
   { 0,          VK_F12,          K_F12    , 0, 0 },   
   { 0,          VK_F13,          K_F13    , 0, 0 },   
   { 0,          VK_F14,          K_F14    , 0, 0 },   
   { 0,          VK_F15,          K_F15    , 0, 0 },   
   { 0,          VK_F16,          K_F16    , 0, 0 },   
   { 0,          VK_F17,          K_F17    , 0, 0 },   
   { 0,          VK_F18,          K_F18    , 0, 0 },   
   { 0,          VK_F19,          K_F19    , 0, 0 },   
   { 0,          VK_F20,          K_F20    , 0, 0 },   
   { 0,          VK_F21,          K_F21    , 0, 0 },   
   { 0,          VK_F22,          K_F22    , 0, 0 },   
   { 0,          VK_F23,          K_F23    , 0, 0 },   
   { 0,          VK_F24,          K_F24    , 0, 0 },   
   { 0,          VK_PRIOR,        K_ROLLDN , 0, 1 },
   { 0,          VK_NEXT,         K_ROLLUP , 0, 1 },
   { 0,          VK_UP,	          K_UP     , 0, 1 },
   { 0,          VK_DOWN,         K_DOWN   , 0, 1 },
   { 0,          VK_LEFT,         K_LEFT   , 0, 1 },
   { 0,          VK_RIGHT,        K_RIGHT  , 0, 1 },
   { 0,          VK_INSERT,       K_INSERT , 0, 1 },
   { 0,          VK_DELETE,       K_DELETE , 0, 1 },
   { 0,          VK_HOME,         K_HOME   , 0, 1 },
   { 0,          VK_END,          K_END    , 0, 1 },
   { 0,          VK_ADD,          K_FIELDPLUS  , 0, 0 },
   { 0,          VK_SUBTRACT,     K_FIELDMINUS , 0, 0 },
   { 0,          VK_SCROLL,       K_HELP       , 0, 0 },
   { (SHORT)-1, (DWORD)-1, (DWORD)-1, (BYTE)-1, (BYTE)-1 },
};


const keystroke_to_msg keyup2msg[] =
{
//  KeyState    Win32 VirtKey    5250 key     ctx ext
   { 0,          VK_CONTROL,      K_RESET    , 0, 0 },
   { 0,          VK_CONTROL,      K_FIELDEXIT, 0, 1 },
   { VK_MENU,    VK_SNAPSHOT,     K_SYSREQ,    1, 0 },
   { 0,          VK_SNAPSHOT,     K_PRINT,     1, 0 },
   { (SHORT)-1, (DWORD)-1 },
};

// This array translates Windows "Character messages"  into 5250
// keys. Some function keys are not available as character messages,
// so those need to be handled in the array above.
const win32_key_map win_kb[] =
{
// Win32 CharMsg  5250 key
   { 0x01,   K_ATTENTION },   // CTRL-A
   { 0x02,   K_ROLLDN },      // CTRL-B
   { 0x03,   K_COPY_TEXT },   // CTRL-C
   { 0x04,   K_ROLLUP },      // CTRL-D
   { 0x05,   K_ERASE },       // CTRL-E
   { 0x06,   K_ROLLUP },      // CTRL-F
   { VK_BACK,K_BACKSPACE },   // CTRL-H (backspace key)
   { 0x0b,   K_FIELDEXIT },   // CTRL-K
   { 0x0c,   K_REFRESH },     // CTRL-L
   { 0x0f,   K_HOME },        // CTRL-O
   { 0x10,   K_PRINT },       // CTRL-P
   { 0x12,   K_RESET },       // CTRL-R
   { 0x13,   K_MEMO },        /* CTRL-S */
   { 0x14,   K_TESTREQ },     // CTRL-T
   { 0x15,   K_ROLLDN },      // CTRL-U
   { 0x16,   K_PASTE_TEXT },  // Ctrl-V
   { 0x17,   K_EXEC },        /* Ctrl-W */
   { 0x18,   K_FIELDPLUS },   // CTRL-X
   { 0x1b,   K_ATTENTION },   // ESCAPE
   { (DWORD)-1, (DWORD)-1 },
};


