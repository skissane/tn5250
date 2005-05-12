/* 
* Copyright (C) 2001 Scott Klement
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
// Cardinal Health AIS Canada (c) 2003-2005
// (c) 2003 Marc-Antoine Ruel

#include "stdafx.h"
#include "Term5250.h"
#include "Term.h"

// Glue
#ifndef NDEBUG
extern "C" int tn5250_debug_stream_init (Tn5250Stream * /*This*/)
{
    return 0;
}
#endif

ITerminal * GetTerm5250(const Tn5250Terminal * This)
{
    return static_cast<ITerminal *>(reinterpret_cast<CTerm5250 *>(This->data));
}

int win32_terminal_flags(Tn5250Terminal *)
{
    FUNC_ENTER();
    return TN5250_TERMINAL_HAS_COLOR;
}
void win32_terminal_beep(Tn5250Terminal *)
{
    FUNC_ENTER();
    MessageBeep((DWORD)-1);
}
int win32_terminal_getkey(Tn5250Terminal *This)
{
    // Read the next key from the terminal, and do any required translations.
    int key = GetTerm5250(This)->GetKey();
    return key;
}
void win32_terminal_update(Tn5250Terminal * This, Tn5250Display * /*display*/)
{
    //FUNC_ENTER();
    // Do not refresh directly since when queuing lots of keys, it overload the system uselessly
    GetTerm5250(This)->PostBufferRefresh();
}

void win32_terminal_update_indicators(Tn5250Terminal * This, Tn5250Display * /*display*/)
{
    //FUNC_ENTER();
    // Do not refresh directly since when queuing lots of keys, it overload the system uselessly
    GetTerm5250(This)->PostBufferRefresh();
}

int win32_terminal_waitevent(Tn5250Terminal * This)
{
    if (This->conn_fd != -1)
    {
        if (WSAAsyncSelect(This->conn_fd, GetTerm5250(This)->GetHwnd(), WM_TN5250_STREAM_DATA, FD_READ) == SOCKET_ERROR)
        {
            FUNC_ENTER1(" WSAASyncSelect failed, reason: %d", WSAGetLastError());
            return TN5250_TERMINAL_EVENT_QUIT;
        }
    }

    DWORD RetVal = TN5250_TERMINAL_EVENT_QUIT;
    if (GetTerm5250(This)->IsReady())
    {
        // Read all of the messages in this next loop, removing each message as we read it.
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0) > 0)
        {
            if ( msg.message == WM_TN5250_STREAM_DATA )
            {
                //FUNC_ENTER0(" got STREAM_DATA");
                RetVal = TN5250_TERMINAL_EVENT_DATA;
                break;
            }
            else if ( msg.message == WM_TN5250_KEY_DATA )
            {
                //FUNC_ENTER0(" got KEY_DATA");
                RetVal = TN5250_TERMINAL_EVENT_KEY;
                break;
            }
            FUNC_ENTER1(" got msg %d ??", msg.message);
            Break();
        }
    }

    if (This->conn_fd != -1) 
        WSAAsyncSelect(This->conn_fd, GetTerm5250(This)->GetHwnd(), 0, 0);

    return RetVal;
}

int filter(unsigned int code, struct _EXCEPTION_POINTERS * /*ep*/, const TCHAR * Function, const TCHAR * File, int Line)
{
    FUNC_ENTER4("(%d, , %s, %s, %d)", code, Function, File, Line);
    UNREFERENCED_PARAMETER(code);
    UNREFERENCED_PARAMETER(Function);
    UNREFERENCED_PARAMETER(File);
    UNREFERENCED_PARAMETER(Line);
    ASSERT("Got an exception" && 0);
    //return EXCEPTION_EXECUTE_HANDLER;
    return EXCEPTION_CONTINUE_SEARCH;
}

#define FILTER filter(GetExceptionCode(), GetExceptionInformation(), __TFUNCTION__, __TFILE__, __LINE__)

// The worker thread
DWORD WINAPI CTerm5250::ThreadFunc(LPVOID ObjectInstance)
{
    dbgSetThreadName("CTerm5250 Worker Thread");
    CTerm5250 & Me = *reinterpret_cast<CTerm5250 * const>(ObjectInstance);
    __try
    {
        tn5250_session_main_loop(Me.sess);
    }
    __except(FILTER)
    {
        // we failed miserably...
    }

    // Don't forget to get disconnected here!
    if ( Me.m_nReadyState )
    {
        // ** Disconnect WON'T get called. **
        Me.InternalFlush();
    }
    // else we quit because Disconnect() has been called before so the flush is done in the other thread.
    return 0;
}

void win32_terminal_destroy(Tn5250Terminal *)
{
    FUNC_ENTER0(" duh!");
}

int win32_terminal_enhanced(Tn5250Terminal *)
{
    FUNC_ENTER();
    return 1;
}

void ForceRedraw(Tn5250Terminal * This)
{
    GetTerm5250(This)->PostBufferRefresh();
}

/* The 5250 screen buffer.  This contains an entry for every character
 * on the screen.  Each element consists of the character that should
 * be displayed, the color to draw in, and the font to use.  Each of
 * these attributes can be found be applying the appropriate mask (e.g
 * A_X5250_COLORMASK and A_X5250_BOLDMASK).
 */
unsigned int ***buf5250;

/* This struct contains all the elements necessary to define a 5250
 * field.  The contents member uses the same masks as buf5250 to
 * define color and font attributes.
 */
typedef struct _field5250
{
  unsigned int fieldid;		/* Numeric ID of this field */
  short inputcapable;		/* Boolean input capable */
  short continuous;		/* Boolean is a continuous field */
  short wordwrap;		/* Boolean is a word wrap field */
  unsigned int attributes;	/* Field attributes (same as the
				 * attributes of buf5250).  These are
				 * for the whole field, but may be
				 * overridden by the contents member */
  unsigned int row;		/* Row field starts on */
  unsigned int column;		/* Column field starts on */
  unsigned int length;		/* length (in characters) of field */
  unsigned int nextfieldprogressionid;	/* Field ID of next field cursor
					 * should move to */
  unsigned int contents[3564];	/* Contents of field (same as the
				 * contents of buf5250 for the
				 * same row and column) */
} field5250;


/* This struct contains all the elements necessary to define a 5250
 * window.
 */
typedef struct _window5250
{
  unsigned int windowid;	/* Numeric ID of this window */
  unsigned int row;		/* Row window starts on */
  unsigned int column;		/* Column window starts on */
  unsigned int height;		/* height (in characters) of window */
  unsigned int width;		/* width (in characters) of window */
  unsigned int border[4];	/* Characters used to create borders
				 * Uses the masks as buf5250 */
} window5250;

/* This struct contains all the fields that appear (either on top or
 * obscured by windows) on a screen.
 *
 * According to the docs the maximum number of input fields that can
 * exist on a screen is 256.
 */
typedef struct _screenfields
{
  unsigned int totalfields;
  field5250 fields[256];
} screenfields;

/* The 5250 field buffer.  This contains an entry for every screen and
 * window.
 */
screenfields *fields5250 = NULL;

int subwindowcount;

/* This is used to define 5250 windows.  We use actual subwindows for these.
* That is cool because we could change these to be independant windows that
* could be moved around the desktop.  But we don't use the borders that the
* iSeries instructs us to use.  Too bad, those borders suck anyway ;)
*/
window5250 *win5250 = NULL;

void win32_terminal_create_window (Tn5250Terminal * This, Tn5250Display * /*d*/, Tn5250Window * window)
{
    FUNC_ENTER4(" window width, height : x, y: %d, %d : %d, %d", window->width, window->height, window->column, window->row);
    static int depth = 0;
    // destroy_screen (subwindowcount);
    ForceRedraw(This);

    if (subwindowcount > (depth - 1))
        win5250 = (window5250 *) realloc ((void *) win5250, (depth + 1) * sizeof (window5250));

    win5250[subwindowcount].windowid = window->id;
    win5250[subwindowcount].row = window->row;
    win5250[subwindowcount].column = window->column;
    win5250[subwindowcount].height = window->height;
    win5250[subwindowcount].width = window->width;
    win5250[subwindowcount].border[0] = ' ' | A_5250_BLUE | A_REVERSE;
    win5250[subwindowcount].border[1] = ' ' | A_5250_BLUE | A_REVERSE;
    win5250[subwindowcount].border[2] = ' ' | A_5250_BLUE | A_REVERSE;
    win5250[subwindowcount].border[3] = ' ' | A_5250_BLUE | A_REVERSE;

    /*newsubwindow(reinterpret_cast<CTerm5250 *>(This->data)->win,
        reinterpret_cast<CTerm5250 *>(This->data)->subwindows,
        reinterpret_cast<CTerm5250 *>(This->data)->font_info,
        reinterpret_cast<CTerm5250 *>(This->data)->borderwidth);*/
    ++subwindowcount;

    if (subwindowcount > depth)
    {
        depth = subwindowcount;
        buf5250 = (unsigned int ***)realloc (buf5250, (depth + 1) * sizeof (unsigned int **));
        buf5250[depth] = (unsigned int **)malloc (27 * sizeof (unsigned int *));
        for (int i = 0; i < 27; i++)
        {
            buf5250[depth][i] = (unsigned int *)malloc (133 * sizeof (unsigned int));
            memset (buf5250[depth][i], 0, 133 * sizeof (unsigned int));
        }
        fields5250 = (screenfields *) realloc ((void *) fields5250, (depth + 1) * sizeof (screenfields));
    }
}

void win32_terminal_destroy_window (Tn5250Terminal * This, Tn5250Display * /*d*/, Tn5250Window *)
{
    FUNC_ENTER();
    ForceRedraw(This);
    if (subwindowcount > 0)
    {
        for (int i = (subwindowcount - 1); i >= 0; i--)
        {
            //XDestroyWindow(display, reinterpret_cast<CTerm5250 *>(This->data)->subwindows[i]);
            win5250[i].windowid = 0;
            win5250[i].row = 0;
            win5250[i].column = 0;
            win5250[i].height = 0;
            win5250[i].width = 0;
            win5250[i].border[0] = 0;
            win5250[i].border[1] = 0;
            win5250[i].border[2] = 0;
            win5250[i].border[3] = 0;
        }
        subwindowcount = 0;
    }
}

void win32_terminal_create_scrollbar (Tn5250Terminal * This, Tn5250Display * /*d*/, Tn5250Scrollbar * /*scrollbar*/)
{
    reinterpret_cast<CTerm5250 *>(This->data)->scrollbar = true;
}

void win32_terminal_destroy_scrollbar (Tn5250Terminal * This, Tn5250Display * /*d*/)
{
    reinterpret_cast<CTerm5250 *>(This->data)->scrollbar = false;
}
