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

#include "stdafx.h"
#include "Term5250.h"
#include "Term.h"

// Glue
#ifndef NDEBUG
extern "C" int tn5250_debug_stream_init (Tn5250Stream *This)
{
    return 0;
}
#endif


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
    FUNC_ENTER();
    return ((CTerm5250*)This->data)->GetKey();
}
void win32_terminal_update(Tn5250Terminal * This, Tn5250Display * /*display*/)
{
    FUNC_ENTER();
    // Do not refresh directly since when queuing lots of keys, it overload the system uselessly
    ((CTerm5250*)This->data)->PostBufferRefresh();
}

void win32_terminal_update_indicators(Tn5250Terminal * This, Tn5250Display * /*display*/)
{
    FUNC_ENTER();
    // Do not refresh directly since when queuing lots of keys, it overload the system uselessly
    ((CTerm5250*)This->data)->PostBufferRefresh();
}

int win32_terminal_waitevent(Tn5250Terminal * This)
{
    FUNC_ENTER();

    if (This->conn_fd != -1)
    {
        if (WSAAsyncSelect(This->conn_fd, ((CTerm5250*)This->data)->m_hWnd, WM_TN5250_STREAM_DATA, FD_READ) == SOCKET_ERROR)
        {
            FUNC_ENTER1(" WSAASyncSelect failed, reason: %d", WSAGetLastError());
            return TN5250_TERMINAL_EVENT_QUIT;
        }
    }

    __try
    {
        DWORD RetVal;
        HANDLE Event = ((CTerm5250*)This->data)->GetTimerHandle();
        for (;;)
        {
            if (!((CTerm5250*)This->data)->m_nReadyState)
                return TN5250_TERMINAL_EVENT_QUIT;

            RetVal = MsgWaitForMultipleObjects(1, &Event, FALSE, INFINITE, QS_ALLINPUT);

            if (!((CTerm5250*)This->data)->m_nReadyState)
                return TN5250_TERMINAL_EVENT_QUIT;

            if ( RetVal == WAIT_OBJECT_0 )
            {
                // We got a refresh event!
                FUNC_ENTER0(" got REFRESH");
                ((CTerm5250*)This->data)->RefreshScreenBuffer();
                // continue to loop
                continue;
            }
            else if ( RetVal == (WAIT_OBJECT_0+1) )
            {
                // Read all of the messages in this next loop,
                // removing each message as we read it.
                MSG msg;
                while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                {
                    // If it is a quit message, exit.
                    if (msg.message == WM_QUIT)
                    {
                        FUNC_ENTER0(" got WM_QUIT");
                        return TN5250_TERMINAL_EVENT_QUIT;
                    }
                    // Otherwise, dispatch the message.
                    if ( msg.message == WM_TN5250_STREAM_DATA )
                    {
                        FUNC_ENTER0(" got STREAM_DATA");
                        return TN5250_TERMINAL_EVENT_DATA;
                    }
                    if ( msg.message == WM_TN5250_KEY_DATA )
                    {
                        FUNC_ENTER0(" got KEY_DATA");
                        return TN5250_TERMINAL_EVENT_KEY;
                    }
                    FUNC_ENTER1(" got msg %d ??", msg.message);
                } // End of PeekMessage while loop.
                continue;
            }
            // Got an error, just quit.
            break;
        }

        // if abandoned or error
        FUNC_ENTER1(" ERROR: got %d", RetVal);
        return TN5250_TERMINAL_EVENT_QUIT;
    }
    __finally
    {
        if (This->conn_fd != -1) 
            WSAAsyncSelect(This->conn_fd, ((CTerm5250*)This->data)->m_hWnd, 0, 0);
    }
}


// The worker thread
DWORD WINAPI CTerm5250::ThreadFunc(LPVOID ObjectInstance)
{
    dbgSetThreadName("CTerm5250 Worker Thread");
    CTerm5250 & Me = *reinterpret_cast<CTerm5250*const>(ObjectInstance);
    __try
    {
        tn5250_session_main_loop(Me.sess);
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        // we failed miserably...
        ASSERT( "Got an error" && 0 );
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


/*
// Returns the current width (in chars) of the terminal.
int win32_terminal_width(Tn5250Terminal *This)
{
FUNC_ENTER();
ASSERT(((CTerm5250*)This->data)->m_hWnd);
// TBM Bugged
//return ( ((CTerm5250*)This->data)->GetRect().Width() / ((CTerm5250*)This->data)->GetFontSize().cx ) + 1;	// Why + 1 ??
return tn5250_display_width(display);
}

// Returns the current height of the terminal.
int win32_terminal_height(Tn5250Terminal *This)
{
FUNC_ENTER();
ASSERT(((CTerm5250*)This->data)->m_hWnd);
// TBM Bugged
//return ( ((CTerm5250*)This->data)->GetRect().Height() / (((CTerm5250*)This->data)->GetFontSize().cy) ) + 1;	// Why + 1 ??
return tn5250_display_height(display);
}
*/

/* TBA or TBR
//, class Object
template< typename T, int Id, typename fnC, typename fnV, fnC CanChange, fnV Changed>
class AutoFire
{
public:
operator const T &() const { return m_Obj; }
void operator = (const T & rhs) { if ( CanChange(Id) ) { m_Obj = rhs; Changed(Id); } }
protected:
T	m_Obj;
};

std::for_each(a, b, c);

bool C1(int i)
{
return true;
}
void V1(int i)
{
}

AutoFire<int, 3, bool (int), void (int), C1, V1  > Object;
AutoFire<int, 3, bool (int), void (int), C1, V1 > Object;
*/





/****i* lib5250/win32_get_printer_info
* NAME
*    win32_get_printer_info
* SYNOPSIS
*    win32_get_printer_info (globTerm);
* INPUTS
*    Tn5250Terminal  *          This    -
* DESCRIPTION
*    This displays a standard Windows printer dialog allowing
*    the user to choose which printer he would like to print to
*    and stores a pointer to the resulting PRINTDLG structure 
*    in This->data->pd.
*
*    If you call this again without first calling the
*    win32_destroy_printer_info function, no dialog is displayed, 
*    and the same pointer is returned. 
*****/
PRINTDLG * win32_get_printer_info(Tn5250Terminal *This) {

#if 0
    PRINTDLG *l_pd; 

    if (This->data->pd != NULL) 
        return This->data->pd;

    This->data->pd = (PRINTDLG *) g_malloc(sizeof(PRINTDLG));

    l_pd = This->data->pd;  /* save a little typing */

    memset(l_pd, 0, sizeof(PRINTDLG));
    l_pd->lStructSize = sizeof(PRINTDLG);
    l_pd->hwndOwner   = This->data->hwndMain;
    l_pd->hDevMode    = NULL;   /* windows will make one. */
    l_pd->hDevNames   = NULL;   /* windows will make one. */
    l_pd->Flags       = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC
        | PD_NOPAGENUMS | PD_NOSELECTION | PD_ALLPAGES;
    l_pd->nCopies     = 1;
    l_pd->nMinPage    = 1;
    l_pd->nMaxPage    = 1;

    if (PrintDlg(This->data->pd) == 0) {
        TN5250_LOG (("PrintDlg() error %d\n", CommDlgExtendedError()));
        g_free(This->data->pd);
        This->data->pd = NULL;
        return NULL;
    }

    if (!(GetDeviceCaps(l_pd->hDC, RASTERCAPS) & RC_STRETCHBLT)) {
        win32_destroy_printer_info(This);
        TN5250_LOG (("WIN32: StretchBlt not available for this printer.\n"));
        msgboxf("This printer does not support the StretchBlt function.\r\n"
            "Printing cancelled.");
    }

    return This->data->pd;
#endif
    return NULL;
}


/****i* lib5250/win32_destroy_printer_info
* NAME
*    win32_destroy_printer_info
* SYNOPSIS
*    win32_destroy_printer_info (globTerm);
* INPUTS
*    Tn5250Terminal  *          This    -
* DESCRIPTION 
*    This frees up the data allocated by the function 
*    win32_get_printer_info()
*****/
void win32_destroy_printer_info(Tn5250Terminal *This) {

#if 0
    if (This->data->pd->hDC != NULL)
        DeleteDC(This->data->pd->hDC);
    if (This->data->pd->hDevMode != NULL)
        g_free(This->data->pd->hDevMode);
    if (This->data->pd->hDevNames != NULL)
        g_free(This->data->pd->hDevNames);
    g_free(This->data->pd);
    This->data->pd = NULL;
#endif
    return;
}



/****i* lib5250/win32_print_screen
* NAME
*    win32_print_screen
* SYNOPSIS
*    win32_print_screen(globTerm, globDisplay);
* INPUTS
*    Tn5250Display   *          This    -  TN5250 terminal object
*    Tn5250Display   *          display -  TN5250 display object
* DESCRIPTION
*    This builds a B&W bitmap of our current display buffer, and
*    sends it to the printer.
*****/
void win32_print_screen(Tn5250Terminal *This, Tn5250Display *display) {

#if 0
    PRINTDLG *pd;
    DOCINFO di;
    HBITMAP bmap;
    HDC screenDC, hdc;
    float pelsX1, pelsX2;
    float scaleX, pixMax;
    int rc;
    int x, y, h, w, h2, w2;
    int i, size;
    RECT rect;
    LOGBRUSH lb;
    HBRUSH oldbrush;
    HPEN oldpen;
    Tn5250Win32Attribute *mymap;

    /* get info about the printer.   The GDI device context will
    be in pd->hDC.  We need this to print.  */

    if ((pd = win32_get_printer_info(This)) == NULL) {
        return;
    }
    TN5250_ASSERT ( pd->hDC != NULL );


    /* Get screen size & horizontal resolution.   We need this to
    scale the screen output so that it looks good on the printer. */

    GetClientRect(m_hWnd, &rect);
    x = rect.left;
    y = rect.top;
    h = (rect.bottom - rect.top) + 7;
    w = (rect.right - rect.left) + 7;
    screenDC = GetDC(This->data->hwndMain);
    pelsX1 = (float) GetDeviceCaps(screenDC, LOGPIXELSX);


    /* create a bitmap to draw the screen into.   We want to redraw the
    screen in black & white and put a border around it */

    bmap = CreateCompatibleBitmap(screenDC, w+6, h+6);
    hdc  = CreateCompatibleDC(NULL);
    SelectObject(hdc, bmap);


    /* fill the bitmap by making a white rectangle with a black border */

    lb.lbStyle = BS_SOLID;
    lb.lbColor = RGB(255,255,255);
    lb.lbHatch = 0;

    oldbrush = SelectObject(hdc, CreateBrushIndirect(&lb));
    oldpen = SelectObject(hdc, CreatePen(PS_SOLID, 0, RGB(0,0,0)));
    Rectangle(hdc, 0, 0, w, h);
    SelectObject(hdc, oldbrush);
    oldpen = SelectObject(hdc, oldpen);
    DeleteObject(oldpen);
    oldbrush = SelectObject(hdc, oldbrush);
    DeleteObject(oldbrush);

    /* create a black on white attribute map, so that win32_do_terminal_update
    will paint the screen in our colors. */

    i = 0;
    while (attribute_map[i].colorindex != -1)
        i++;
    size = (i+1) * sizeof(Tn5250Win32Attribute);
    mymap = (Tn5250Win32Attribute *) g_malloc(size);
    memcpy(mymap, attribute_map, size);
    for (i=0; mymap[i].colorindex != -1; i++) {
        if ( mymap[i].colorindex == A_5250_BLACK )
            mymap[i].fg = RGB(255,255,255);
        else
            mymap[i].fg = RGB(0,0,0);
    }

    /* re-draw the screen into our new bitmap */

    win32_do_terminal_update(hdc, This, display, mymap, 3, 3);


    /* start a new printer document */

    memset(&di, 0, sizeof(DOCINFO));
    di.cbSize = sizeof(DOCINFO);
    di.lpszDocName = "TN5250 Print Screen";
    di.lpszOutput  = (LPTSTR) NULL;
    di.lpszDatatype= (LPTSTR) NULL;
    di.fwType = 0;

    rc = StartDoc(pd->hDC, &di);
    if (rc == SP_ERROR) {
        msgboxf("StartDoc() ended in error.\r\n");
        win32_destroy_printer_info(This);       
        return;
    }

    rc = StartPage(pd->hDC);
    if (rc <= 0) {
        msgboxf("StartPage() ended in error.\r\n");
        win32_destroy_printer_info(This);       
        return;
    }


    /* calculate the scaling factor:
    a) If possible, scale the screen image so that it uses the same
    number of logical inches on the printout as it did on the screen.
    (we do this by dividing the printer's logical pixels per inch
    by the screen's logical pixels per inch)
    b) If that doesn't fit on the page, then just scale it to the width
    of the page.
    */

    pelsX2 = (float) GetDeviceCaps(pd->hDC, LOGPIXELSX);
    pixMax = (float) GetDeviceCaps(pd->hDC, HORZRES);

    TN5250_LOG (("WIN32: PrintKey: Screen is %f pix/in, Printer is %f pix/in"
        " and %f pix wide\n", pelsX1, pelsX2, pixMax));

    if (pelsX1 > pelsX2)
        scaleX = (pelsX1 / pelsX2);
    else
        scaleX = (pelsX2 / pelsX1);   

    w2 = w * scaleX;
    if (w2 > pixMax) 
        scaleX = pixMax / w;
    w2 = w * scaleX;
    h2 = h * scaleX;

    TN5250_LOG (("WIN32: PrintKey: Since Window is %d pixels wide, we'll "
        "make the printer image %d by %d\n", w, w2, h2));

    /* This will stretch the bitmap to the new height & width while (at the
    same time) copying it to the printer */

    if (StretchBlt(pd->hDC, 0, 0, w2, h2, hdc, x, y, w, h, SRCCOPY)==0) {
        TN5250_LOG (("StretchBlt error %d\n", GetLastError ));
        TN5250_ASSERT( FALSE );
    }

    /* close printer document */

    EndPage(pd->hDC);
    EndDoc(pd->hDC);

    /* notify user */

    MessageBox(This->data->hwndMain, "Print screen successful!",  "TN5250",
        MB_OK|MB_ICONINFORMATION);
#endif
}
