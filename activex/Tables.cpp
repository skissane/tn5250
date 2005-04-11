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


const char g_szAbout[] =
    PACKAGE _T(" version ") VERSION _T(":\n")
    _T("Copyright (C) 1997-2003 by Michael Madore,")
    _T(" Jason M. Felice, and Scott Klement")
    _T(" and a bit Marc-Antoine Ruel\n")
    _T("\n")
    _T("Portions of this software were contributed ")
    _T("by many people.  See the AUTHORS.txt file for")
    _T("details.\n")
    _T("\n")
    _T("For license information, see the COPYING.txt file ")
    _T("that was installed with this software.\n")
    _T("Or look at http://tn5250.sf.net for more informations\n")
#if HAVE_LIBSSL
    _T("\n")
    _T("OpenSSL:\n")
    _T("This product includes software developed by the ")
    _T("OpenSSL Project for use in the OpenSSL Toolkit ")
    _T("(http://www.openssl.org/)\n")
    _T("This product includes cryptographic software ")
    _T("written by Eric Young (eay@crypsoft.com).\n")
    _T("This product includes software written by Tim ")
    _T("Hudson (tjh@cryptsoft.com).\n")
    _T("\n")
    _T("For OpenSSL license information, see the ")
    _T("COPYING.txt file that was installed with ")
    _T("this software.\n")
#endif
    ;

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
    { -1,           (UINT)-1 },
};


// This array converts windows "keystrokes" into character messages
// for those that aren't handled by the "TranslateMessage" function.
//
// The smaller this array is, the better the keyboard handling will
// perform :)
const keystroke_to_msg key2msg[] =
{
//  KeyState    Win32 VirtKey    5250 key
   { M_Shift,    VK_TAB,          K_BACKTAB    },
   { M_Shift,    VK_F1,           K_F13        },
   { M_Shift,    VK_F2,           K_F14        },
   { M_Shift,    VK_F3,           K_F15        },
   { M_Shift,    VK_F4,           K_F16        },
   { M_Shift,    VK_F5,           K_F17        },
   { M_Shift,    VK_F6,           K_F18        },
   { M_Shift,    VK_F7,           K_F19        },
   { M_Shift,    VK_F8,           K_F20        },
   { M_Shift,    VK_F9,           K_F21        },
   { M_Shift,    VK_F10,          K_F22        },
   { M_Shift,    VK_F11,          K_F23        },
   { M_Shift,    VK_F12,          K_F24        },
   { M_Shift,    VK_INSERT,       K_PASTE_TEXT },
   { M_Shift,    VK_DELETE,       K_COPY_TEXT  },
   { M_Ctrl,     VK_LEFT,         K_PREVFLD    },
   { M_Ctrl,     VK_RIGHT,        K_NEXTFLD    },
   { M_Shift,    VK_RETURN,       K_NEWLINE    },
   { 0,          VK_NUMPAD_RETURN,K_NEWLINE    },
   { 0,          VK_RETURN,       K_ENTER      },
   { 0,          VK_F1,           K_F1         },
   { 0,          VK_F2,           K_F2         },
   { 0,          VK_F3,           K_F3         },
   { 0,          VK_F4,           K_F4         },
   { 0,          VK_F5,           K_F5         },
   { 0,          VK_F6,           K_F6         },
   { 0,          VK_F7,           K_F7         },
   { 0,          VK_F8,           K_F8         },
   { 0,          VK_F9,           K_F9         },
   { 0,          VK_F10,          K_F10        },
   { 0,          VK_F11,          K_F11        },
   { 0,          VK_F12,          K_F12        },
   { 0,          VK_F13,          K_F13        },
   { 0,          VK_F14,          K_F14        },
   { 0,          VK_F15,          K_F15        },
   { 0,          VK_F16,          K_F16        },
   { 0,          VK_F17,          K_F17        },
   { 0,          VK_F18,          K_F18        },
   { 0,          VK_F19,          K_F19        },
   { 0,          VK_F20,          K_F20        },
   { 0,          VK_F21,          K_F21        },
   { 0,          VK_F22,          K_F22        },
   { 0,          VK_F23,          K_F23        },
   { 0,          VK_F24,          K_F24        },
   { 0,          VK_PRIOR,        K_ROLLDN     },
   { 0,          VK_NEXT,         K_ROLLUP     },
   { 0,          VK_UP,           K_UP         },
   { 0,          VK_DOWN,         K_DOWN       },
   { 0,          VK_LEFT,         K_LEFT       },
   { 0,          VK_RIGHT,        K_RIGHT      },
   { 0,          VK_INSERT,       K_INSERT     },
   { 0,          VK_DELETE,       K_DELETE     },
   { 0,          VK_HOME,         K_HOME       },
   { 0,          VK_END,          K_END        },
   { 0,          VK_ADD,          K_FIELDPLUS  },
   { 0,          VK_SUBTRACT,     K_FIELDMINUS },
   { 0,          VK_SCROLL,       K_HELP       },
   { M_Ctrl,     VK_A,            K_ATTENTION  },
   { M_Ctrl,     VK_B,            K_ROLLDN     },
   { M_Ctrl,     VK_C,            K_COPY_TEXT  },
   { M_Ctrl,     VK_D,            K_ROLLUP     },
   { M_Ctrl,     VK_E,            K_ERASE      },
   { M_Ctrl,     VK_F,            K_ROLLUP     },
   { 0,          VK_BACK,         K_BACKSPACE  },
   { M_Ctrl,     VK_K,            K_FIELDEXIT  },
   { M_Ctrl,     VK_L,            K_REFRESH    },
   { M_Ctrl,     VK_O,            K_HOME       },
   { M_Ctrl,     VK_P,            K_PRINT      },
   { M_Ctrl,     VK_R,            K_RESET      },
   { M_Ctrl,     VK_S,            K_MEMO       },
   { M_Ctrl,     VK_T,            K_TESTREQ    },
   { M_Ctrl,     VK_U,            K_ROLLDN     },
   { M_Ctrl,     VK_V,            K_PASTE_TEXT },
   { M_Ctrl,     VK_W,            K_EXEC       },
   { M_Ctrl,     VK_X,            K_FIELDPLUS  },
   { 0,          VK_ESCAPE,       K_ATTENTION  },
   // Used on key up only. The program automatically detects this case.
   { 0,          VK_LCONTROL,     K_RESET      },
   { 0,          VK_RCONTROL,     K_FIELDEXIT  },
   { M_Alt,      VK_SNAPSHOT,     K_SYSREQ,    },
   { 0,          VK_SNAPSHOT,     K_PRINT,     },
};

// Those keys are considered "similar"
// For instance, a binding for VK_PRIOR will work for VK_NUMPAD_PRIOR too if no specific binding is done to VK_NUMPAD_PRIOR.
const SimilarKey k_SimilarKeys[] = {
    { VK_SHIFT,   VK_LSHIFT         },
    { VK_SHIFT,   VK_RSHIFT         },
    { VK_CONTROL, VK_LCONTROL       },
    { VK_CONTROL, VK_RCONTROL       },
    { VK_MENU,    VK_LMENU          },
    { VK_MENU,    VK_RMENU          },
    { VK_INSERT,  VK_NUMPAD_INSERT, },
    { VK_DELETE,  VK_NUMPAD_DELETE, },
    { VK_PRIOR,   VK_NUMPAD_PRIOR,  },
    { VK_NEXT,    VK_NUMPAD_NEXT,   },
    { VK_END,     VK_NUMPAD_END,    },
    { VK_HOME,    VK_NUMPAD_HOME,   },
    { VK_LEFT,    VK_NUMPAD_LEFT,   },
    { VK_UP,      VK_NUMPAD_UP,     },
    { VK_RIGHT,   VK_NUMPAD_RIGHT,  },
    { VK_DOWN,    VK_NUMPAD_DOWN,   },
    { VK_RETURN,  VK_NUMPAD_RETURN, },
};

unsigned char const IdentityTable[256] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

Tn5250CharMap IdentityMap =
{
    "0", IdentityTable, IdentityTable
};

const size_t attribute_map_size  = lengthof(attribute_map);
const size_t key2msg_size        = lengthof(key2msg);
const size_t k_SimilarKeys_size  = lengthof(k_SimilarKeys);
