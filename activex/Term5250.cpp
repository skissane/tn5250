/* 
 * Copyright (C) 2001 Scott Klement
 * Copyright (C) 2003 Marc-Antoine Ruel at Cardinal Health
 * 
 * Some parts of This file is part of TN5250
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
 * You should have received a copy of the *GNU Lesser General Public License*
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA
 *
 */
// Term5250.cpp : Implementation of CTerm5250

#include "stdafx.h"
#include "Term5250.h"
#include <algorithm>
#include "Term5250.h"


// CTerm5250
CTerm5250::CTerm5250() throw()
:	m_bBorderVisible(true), 
m_bEnabled(true),
m_bstrCaption(),
m_clrBorderColor(GetSysColor(COLOR_ACTIVEBORDER)), // COLOR_WINDOWFRAME
m_HostName(L"127.0.0.1"),
m_nAppearance(0),
m_nBorderWidth(1),
m_nReadyState(false),
sess(NULL),
stream(NULL),
display(NULL),
config(NULL),
macro(NULL),
m_hFont(NULL),
m_hScreenBMP(NULL),
m_hMemDC(NULL),
m_hCaretBMP(NULL),
m_hBackgroundBrush(NULL),
m_ColSepStyle(COLSEPSTYLE_FULL),
m_ThreadId(0),
m_FontName(L"Courier New"),	// Terminal  
m_Last_size(0,0),
m_FontSize(0,0),
m_CaretStyle(CARETSTYLE_BLINK),
m_rctSelection(0,0,0,0),
m_bHandledKey(false),
m_bCaretOK(false),
m_bResized(true),
m_bDisplayRuler(false),
m_bSelecting(false),
m_bSelected(false),
m_bUnixLikeCopy(false),
m_KbBuf_len(0),
m_bMouseMoveCursor(false),
m_bSelectionEnabled(true),
m_CopyMode(Both),
m_hTimer(NULL)
{
    FUNC_ENTER();
    dbgSetThreadName("CTerm5250 Thread");
    term.data = (_Tn5250TerminalPrivate *)this;     // Should be fixed one day...
    term.conn_fd = (SOCKET)-1;
    // They are never called anyway!
    term.init = NULL;
    term.term = NULL;
    term.destroy = NULL;
    term.width = NULL;
    term.height = NULL;
    // They are called
    term.flags = ::win32_terminal_flags;
    term.update = ::win32_terminal_update;
    term.update_indicators = ::win32_terminal_update_indicators;
    term.waitevent = ::win32_terminal_waitevent;
    term.getkey = ::win32_terminal_getkey;
    term.beep = ::win32_terminal_beep;
    term.config = NULL;

    m_ColorList[A_5250_GREEN		] = RGB(0,   255,   0);
    m_ColorList[A_5250_WHITE		] = RGB(255, 255, 255);
    m_ColorList[A_5250_RED			] = RGB(255,   0,   0);
    m_ColorList[A_5250_TURQ			] = RGB(0,   128, 128);
    m_ColorList[A_5250_YELLOW		] = RGB(255, 255,   0);
    m_ColorList[A_5250_PINK			] = RGB(255,   0, 255);
    m_ColorList[A_5250_BLUE			] = RGB(0,   255, 255);
    m_ColorList[A_5250_BLACK		] = RGB(0,     0,   0);
    m_ColorList[A_5250_RULER_COLOR	] = RGB(192,   0,   0);
    //	m_ColorList[A_5250_STATUS_COLOR	] = RGB(192, 128, 128);
    m_ColorList[A_5250_STATUS_COLOR	] = RGB(128, 255, 128);
    m_clrForeColor = m_ColorList[A_5250_WHITE];
    m_clrBackColor = m_ColorList[A_5250_BLACK];

    // Dont support windowless usage 'cause of caret support... (entr'autre)
    m_bWindowOnly = true;

    // Don't allow a resize
    m_bAutoSize = true;
    // Set the initial size
    SIZEL size = {648, 384};
    AtlPixelToHiMetric( &size, &m_sizeExtent );
    // Make sure the natural extent is the correct size too
    m_sizeNatural = m_sizeExtent;

    // Tell that we want to be alerted when a resize occurs
    //	m_bRecomposeOnResize = true;
}

HRESULT CTerm5250::FinalConstruct() throw()
{
    FUNC_ENTER();

    // Load winsock
    WSADATA wsadata;
    int RetVal = WSAStartup(MAKEWORD(1, 1), &wsadata);
    if ( RetVal )
        return HRESULT_FROM_WIN32(RetVal);

    if (LOBYTE(wsadata.wVersion) != 1 || HIBYTE(wsadata.wVersion) != 1)
    {
        // Not a good idea if we're inside a web browser.
        //WSACleanup();
        return E_FAIL;
    }

    config = tn5250_config_new();
    if (tn5250_config_load_default (config) == -1)
        return E_FAIL;

    // Setup MemDC
    m_hMemDC = CreateCompatibleDC(NULL);
    SetBkMode(m_hMemDC, OPAQUE);
    SetTextAlign(m_hMemDC, TA_TOP|TA_LEFT|TA_NOUPDATECP);

    // set up display terminal
    // SHOULD BE DONE AFTER CONNECT
    display = tn5250_display_new();
    display->sign_key_hack = 0;
    if (tn5250_display_config(display, config) == -1)
        return E_FAIL;

    macro = tn5250_macro_init();
    tn5250_macro_attach(display, macro);

    tn5250_display_set_terminal(display, &term);
    m_hBackgroundBrush = CreateSolidBrush(GetColor(A_5250_BLACK));

    m_hTimer = CreateWaitableTimer(NULL, FALSE, NULL);

    return S_OK;
}

void CTerm5250::FinalRelease() throw()
{
    FUNC_ENTER();

    // Terminate the thread if still connected
    Disconnect();

    if ( m_hFont )
        DeleteObject(m_hFont);
    if ( m_hCaretBMP )
        DeleteObject(m_hCaretBMP);
    if ( m_hScreenBMP )
        DeleteObject(m_hScreenBMP);
    if ( m_hMemDC )
        DeleteDC(m_hMemDC);
    if ( m_hBackgroundBrush )
        DeleteObject(m_hBackgroundBrush);
    if ( m_hTimer )
        CloseHandle(m_hTimer);
    //	if (pd != NULL)
    //		win32_destroy_printer_info(This);

    if (macro != NULL)
        tn5250_macro_exit(macro);
    if (config != NULL)
    {
        tn5250_config_unref(config);
        config = NULL;
    }
}


// we do all drawing to the screen buffer, then bitblt that to the
// actual window when WM_PAINT occurs
HRESULT CTerm5250::OnDraw(ATL_DRAWINFO & di)
{
    if ( !m_bSelecting )
        FUNC_ENTER();

    // We need to know if we've been resized!
    if ( m_bResized )
    {
        m_bResized = false;
    }

    m_bCaretOK = false;

    // Do the BitBlt!
    const int w = (di.prcBounds->right - di.prcBounds->left) + 1;
    const int h = (di.prcBounds->bottom - di.prcBounds->top) + 1;
    // Blit the buffer
    if ( !BitBlt(di.hdcDraw, di.prcBounds->left, di.prcBounds->top, w, h, 
        m_hMemDC, di.prcBounds->left, di.prcBounds->top, SRCCOPY))
    {
        // BitBlt may fail if the OnDraw is called too often during one screen refresh (~1/60 sec)
        const DWORD Err = GetLastError();
        Break();
        if ( Err )
            return HRESULT_FROM_WIN32(Err);
    }
    // Draw blinking caret
    if (m_CaretStyle != CARETSTYLE_NOBLINK)
    {
        if (m_hWnd == GetFocus()) 
            MoveCaret(di.hdcDraw);
    }

    // Draw a rectangle if there's a selection
    if (m_bSelected)
    {
        const int savemode = SetROP2(di.hdcDraw, R2_NOT);
        SelectObject(di.hdcDraw, GetStockObject(m_bSelecting?NULL_BRUSH:WHITE_BRUSH) );
        Rectangle(di.hdcDraw, m_rctSelection.left, m_rctSelection.top, m_rctSelection.right, m_rctSelection.bottom);
        SetROP2(di.hdcDraw, savemode);
    }
    return S_OK;
}


// Create a bitmap act as a screen buffer.  we will draw all of
// our data to this buffer, then BitBlt it to the screen when we
// need to re-draw the screen.   This'll make it easy for us to
// re-paint sections of the screen when necessary
// Create/Resize the bitmap that we use as the screen buffer.
void CTerm5250::RegenScreenBuffer()
{
    FUNC_ENTER1(" %s", m_hWnd?"success":"failure");
    if ( m_hWnd )
    {
        if (m_hScreenBMP) 
            DeleteObject(m_hScreenBMP);
        HDC hdc = GetDC(); // Must not use CreateCompatibleDC otherwise it will be monochrome!
        const CRect & MyRect = GetRect();
        m_hScreenBMP = CreateCompatibleBitmap(hdc, MyRect.Width(), MyRect.Height());
        ReleaseDC(hdc);
        // Select it right now to save a bit of time when drawing
        SelectObject(m_hMemDC, m_hScreenBMP);

        LoadTerminalFont();
        RefreshScreenBuffer();
    }
}


// we do all drawing to the screen buffer (m_hMemDC), then bitblt that to the
// actual window when WM_PAINT occurs
void CTerm5250::RefreshScreenBuffer(bool FireViewEvent)
{
    FUNC_ENTER();
    assert( display );

    if ( !m_hScreenBMP )
        return;

    // clear the screen buffer (one big black rectangle, with a border)
    HPEN MyPen = CreatePen(PS_SOLID, 0, (m_bBorderVisible&&!m_nAppearance)?m_clrBorderColor:GetColor(A_5250_BLACK) );
    HGDIOBJ oldpen = SelectObject(m_hMemDC, MyPen);
    HGDIOBJ oldbrush = SelectObject(m_hMemDC, m_hBackgroundBrush);
    Rectangle(m_hMemDC, 0, 0, GetRect().Width(), GetRect().Height());
    SelectObject(m_hMemDC, oldbrush);
    SelectObject(m_hMemDC, oldpen);
    DeleteObject(MyPen);

    // If resized, but it's too late
    if (   m_Last_size.cy != tn5250_display_height(display)
        || m_Last_size.cx != tn5250_display_width(display))
    {
        m_bResized = false;
        if (m_bAutoSize)
        {
            // Did screen size increase?
            if ( m_Last_size.cx > m_Last_size.cx )
            {

                // TBA Try to keep the same superficy and resize

                // So grow larger
                Break();	// TBA
            }
            //Resize();
        }
        LoadTerminalFont();
        m_Last_size.cy = tn5250_display_height(display);
        m_Last_size.cx = tn5250_display_width (display);
    }
    else if (m_bResized)
    {
        LoadTerminalFont();
        m_bResized = false;
    }

    char attr = 0x20;
    int len = 0;
    char text[132*27];
    int mx = 0;
    int my = 0;
    for (int y = 0; y < tn5250_display_height(display); y++)
    {
        for (int x = 0; x < tn5250_display_width(display); x++)
        {
            char c = tn5250_display_char_at(display, y, x);
            if ((c & 0xe0) == 0x20)
            {
                // ATTRIBUTE
                if (len>0) 
                    DrawTextAttrib(attr, text, len, mx, my);
                len = 0;
                attr = (c & 0xff);	// ???
            }
            else
            {
                // DATA
                if (len==0)
                {
                    mx = x; 
                    my = y;
                }
                if ((c==0x1f) || (c==0x3F))
                {
                    if (len>0)
                        DrawTextAttrib(attr, text, len, mx, my);
                    len = 0;
                    c = ' ';
                    DrawTextAttrib(0x21, &c, 1, x, y);
                }
                else if ((c < 0x40 && c > 0x00) || (c == 0xff))
                {
                    text[len] = ' ';
                    ++len;
                }
                else
                {
                    text[len] = tn5250_char_map_to_local(tn5250_display_char_map(display), c);
                    ++len;
                }
            }			
        }

        if (len>0) 
            DrawTextAttrib(attr, text, len, mx, my);
        len = 0;
    }

    // Draw the status line
    DrawStatusLine();

    // Tell the world to draw me!
    if ( FireViewEvent )
        FireViewChange();
}


void CTerm5250::DrawStatusLine()
{
    FUNC_ENTER();
    const int inds = tn5250_display_indicators(display);

    // m_hScreenBMP and m_hFont are already selected

    char ind_buf[81];
    memset(ind_buf, ' ', sizeof(ind_buf));
    ind_buf[80] = 0;	// closes it
    memcpy(ind_buf, "5250", 4);
    if ((inds & TN5250_DISPLAY_IND_INHIBIT) != 0)
        memcpy(ind_buf + 9, "X II", 4);
    else if ((inds & TN5250_DISPLAY_IND_X_CLOCK) != 0)
        memcpy(ind_buf + 9, "X CLOCK", 7);
    else if ((inds & TN5250_DISPLAY_IND_X_SYSTEM) != 0)
        memcpy(ind_buf + 9, "X SYSTEM", 8);
    if ((inds & TN5250_DISPLAY_IND_MESSAGE_WAITING) != 0)
        memcpy(ind_buf + 23, "MW", 2);
    if ((inds & TN5250_DISPLAY_IND_INSERT) != 0)
        memcpy(ind_buf + 30, "IM", 2);
    if ((inds & TN5250_DISPLAY_IND_FER) != 0)
        memcpy(ind_buf + 33, "FER", 3);
    if ((inds & TN5250_DISPLAY_IND_MACRO) != 0)
        memcpy(ind_buf + 72-12, tn5250_macro_printstate (display), 11);
    // TBM If connected
    BSTR swName = L"Disconnected";
    if ( m_bstrCaption.m_str )
    {
        swName = m_bstrCaption;
    }
    else if ( m_nReadyState && m_HostName.m_str )
    {
        swName = m_HostName;
    }
    {
        CW2A szName(swName);
        int Length = strlen(szName);
        if ( Length > (72-37) )
            Length = (72-37);
        memcpy(ind_buf + 37, szName, Length);
    }
    wsprintf(ind_buf+73, _T("%03.3d/%03.3d"),
        tn5250_display_cursor_x(display)+1,
        tn5250_display_cursor_y(display)+1);

    DrawTextAttrib(GetColor(A_5250_STATUS_COLOR), GetColor(A_5250_BLACK), ind_buf, strlen(ind_buf), 0, tn5250_display_height(display));
    //DrawTextAttrib(0x22, ind_buf, strlen(ind_buf), 0, tn5250_display_height(display));

    if (m_bDisplayRuler)
    {
        const int Delta =  m_bBorderVisible?m_nBorderWidth:0;
        CPoint P = GetCaretPos();
        P.x = (P.x+1)*m_FontSize.cx+Delta;
        P.y = (P.y+1)*m_FontSize.cy+Delta;
        HPEN MyPen = CreatePen(PS_SOLID, 0, GetColor(A_5250_RULER_COLOR));
        HPEN savepen = (HPEN)SelectObject(m_hMemDC, MyPen);
        HGDIOBJ hGDI = SelectObject(m_hMemDC, GetStockObject(WHITE_PEN));
        MoveToEx(m_hMemDC, P.x, m_rcPos.top, NULL);
        LineTo  (m_hMemDC, P.x, m_rcPos.bottom);
        MoveToEx(m_hMemDC, m_rcPos.left , P.y, NULL);
        LineTo  (m_hMemDC, m_rcPos.right, P.y);
        SelectObject(m_hMemDC, savepen);
        SelectObject(m_hMemDC, hGDI);
        DeleteObject(MyPen);
    }

    // TBV To be verified.
    m_bCaretOK = false;

    if (m_CaretStyle == CARETSTYLE_NOBLINK) 
        MoveCaret(m_hMemDC);
}


/// This draws text on the terminal in the specified attribute (in MemDC)
/// \note Assumes 1 line string.
/// \sample DrawTextAttrib(a, "Hello", 5, 12, 5);
/// \params
///   int               attr         - 5250 attribute byte
///   const char *      text         - text to draw
///   int               len          - length of text
///   int               x            - position to start (along x axis)
///   int               y            - position to start (along y axis)
void CTerm5250::DrawTextAttrib(int attr, const char * text, int len
                               , int CaretX, int CaretY
                               , Tn5250Win32Attribute *map, int ox, int oy
                               ) const
{
    assert(m_hMemDC);
    UINT flags = attribute_map[attr-0x20].flags;

    // hmm..  how _do_ you draw something that's invisible?
    if (flags&A_NONDISPLAY) 
        return;
    if (flags&A_VERTICAL && m_ColSepStyle==COLSEPSTYLE_NONE)
    {
        flags &= ~A_VERTICAL;
        flags |= A_UNDERLINE;
    }

    // set up colors for this drawing style
    COLORREF fg = GetColor(attribute_map[attr-0x20].colorindex);
    COLORREF bg = GetColor(A_5250_BLACK);
    if (flags&A_REVERSE)
        std::swap(fg, bg);

    DrawTextAttrib(fg, bg, text, len, CaretX, CaretY, flags, map, ox, oy);
}


void CTerm5250::DrawTextAttrib(OLE_COLOR fg, OLE_COLOR bg, const char * text, int len
                               , int CaretX, int CaretY, int flags
                               , Tn5250Win32Attribute *map, int ox, int oy
                               ) const
{

    SetBkColor(m_hMemDC, bg);
    SetTextColor(m_hMemDC, fg);

    // create a rect to "opaque" our text.  (defines the background area
    // that the text is painted on)
    RECT rect;
    rect.left   = CaretX * m_FontSize.cx + (m_bBorderVisible?m_nBorderWidth:0) + ox;
    rect.top    = CaretY * m_FontSize.cy + (m_bBorderVisible?m_nBorderWidth:0) + oy;
    rect.right  = rect.left + (m_FontSize.cx * len);
    rect.bottom = rect.top + m_FontSize.cy;

    // draw the text
    if ( !ExtTextOut(m_hMemDC, rect.left, rect.top, ETO_CLIPPED|ETO_OPAQUE, &rect, text, len, m_pSpacing) )
        return;	// we can't do anything about it.

    if ( flags&A_UNDERLINE || (flags&A_VERTICAL && m_ColSepStyle!=COLSEPSTYLE_NONE) )
    {
        HPEN MyPen = CreatePen(PS_SOLID, 0, fg);
        HGDIOBJ oldpen = SelectObject(m_hMemDC, MyPen);

        // We don't use the underlining capability of the font itself because on
        // some fonts, it changes the font height, and that would just mess us up.
        if (flags&A_UNDERLINE)
        {
            // draw underlines
            MoveToEx(m_hMemDC, rect.left, rect.bottom-1, NULL);
            LineTo(m_hMemDC, rect.right, rect.bottom-1);
        }
        if (flags&A_VERTICAL && m_ColSepStyle==COLSEPSTYLE_FULL)
        { 
            for ( int x=rect.left; x<=rect.right; x += m_pSpacing[0] )
            {
                MoveToEx(m_hMemDC, x, rect.top, NULL);
                LineTo  (m_hMemDC, x, rect.bottom);
            }
            MoveToEx(m_hMemDC, rect.right, rect.top, NULL);
            LineTo  (m_hMemDC, rect.right, rect.bottom);
        }
        if (flags&A_VERTICAL && m_ColSepStyle==COLSEPSTYLE_DOTS)
        {
            for ( int x=rect.left; x<=rect.right; x += m_pSpacing[0] )
            {
                MoveToEx(m_hMemDC, x, rect.bottom-2, NULL);
                LineTo  (m_hMemDC, x, rect.bottom);
            }
            MoveToEx(m_hMemDC, rect.right, rect.bottom-2, NULL);
            LineTo  (m_hMemDC, rect.right, rect.bottom);
        }
        SelectObject(m_hMemDC, oldpen);
        DeleteObject(MyPen);
    }
}

// Connect Part //
STDMETHODIMP CTerm5250::Connect()
{
    FUNC_ENTER();

    Disconnect();

    if ( !m_HostName.m_str || !m_HostName.Length() )
        return E_INVALIDARG;

    // create connection to host
    {
        CW2A szBuffer( m_HostName );
        stream = tn5250_stream_open(szBuffer, config);
    }
    if (stream == NULL)
        return HRESULT_FROM_WIN32(GetLastError());

    // Create a new TN5250 session!
    sess = tn5250_session_new();
    tn5250_display_set_session(display, sess);
    term.conn_fd = tn5250_stream_socket_handle(stream);
    if ( term.conn_fd == -1 )
        return HRESULT_FROM_WIN32(GetLastError());
    // We are now connected
    m_nReadyState = true;
    // Anyway, we must change it.
    if ( CanEdit(DISPID_READYSTATE) )
        ValueChanged(DISPID_READYSTATE);
    tn5250_session_set_stream(sess, stream);

    if (tn5250_session_config(sess, config) == -1) 
        return E_FAIL;

    // Set the message forwarding
    //	if (WSAAsyncSelect(term.conn_fd, m_hWnd, WM_TN5250_STREAM_DATA, FD_READ) == SOCKET_ERROR)
    //		return HRESULT_FROM_WIN32(GetLastError());

    // Create main loop
    m_hThread.Attach(CreateThread(NULL, 0, ThreadFunc, this, 0, &m_ThreadId));
    if ( m_hThread.m_h == NULL )
        return HRESULT_FROM_WIN32(GetLastError());

    Fire_Connected();

    //FireViewChange();	// RefreshScreen buffer is already called in tn5250_session_set_stream
    return S_OK;
}


STDMETHODIMP CTerm5250::Disconnect()
{
    FUNC_ENTER();

    // Will fire an event in InternalFlush()
    if ( m_hThread.m_h )
    {
        // Post a QUIT message to the thread and wait until it dies
        // It's a must to wake up the thread with a message
        PostThreadMessage(m_ThreadId, WM_QUIT, 0, 0);
        if ( WaitForSingleObject(m_hThread, 2000) != WAIT_OBJECT_0 )
        {
            FUNC_ENTER0(" killed worker thread");
            TerminateThread(m_hThread, 0);
        }
    }
    InternalFlush();
    return S_OK;
}

void CTerm5250::InternalFlush()
{
    m_ThreadId = 0;
    m_hThread.Close();

    if (sess != NULL)
    {
        tn5250_session_destroy(sess);
        sess = NULL;
        stream = NULL;	// session destroys stream...
    }
    // Flush keyboard
    {
        CAutoLock Lock(m_KbBufCS);
        m_KbBuf_len = 0;
    }
    // clear the screen.
    tn5250_display_clear_unit(display);

    const LONG WasConnected = m_nReadyState;
    m_nReadyState = false;
    if ( WasConnected )
    {
        // Anyway, we must change it.
        if ( CanEdit(DISPID_READYSTATE) )
            ValueChanged(DISPID_READYSTATE);
        Fire_Disconnected();
    }

    RefreshScreenBuffer();
}

COLORREF CTerm5250::GetColor(int iColor) const throw()
{
    COLORREF ColorRef = RGB(0,255,0);	// Defaults to green.
    ASSERT( iColor >= 0 && iColor < A_5250_NB_COLORS );
    OleTranslateColor(m_ColorList[iColor], NULL, &ColorRef);
    return ColorRef;
}


STDMETHODIMP CTerm5250::get_HostName(BSTR* pVal)
{
    //	FUNC_ENTER();
    m_HostName.CopyTo(pVal);
    return S_OK;
}


STDMETHODIMP CTerm5250::put_HostName(BSTR newVal)
{
    FUNC_ENTER1(" \"%ls\"", newVal);
    // TBA Check validity, either IP or DNS.
    if ( !newVal || newVal[0] == '[' )
        return E_INVALIDARG;
    if ( !CanEdit(1) )
        return S_FALSE;
    m_HostName = newVal;
    ValueChanged(1);
    return S_OK;
}


LRESULT CTerm5250::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    FUNC_ENTER();
    MakeNewCaret();
    HDC hdc = GetDC();
    m_bCaretOK = false;
    if (m_CaretStyle != CARETSTYLE_NOBLINK)
    {
        MoveCaret(hdc);
        ShowCaret();
    }
    ReleaseDC(hdc);
    bHandled = FALSE;
    return 0;
}

LRESULT CTerm5250::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    FUNC_ENTER();
    HideCaret();
    bHandled = FALSE;
    return 0;
}


LRESULT CTerm5250::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    //FUNC_ENTER();
    return 1;	// Just don't erase the background!
}


LRESULT CTerm5250::OnKeyUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = false;
    if ( !m_bEnabled || !m_nReadyState )
        return 0;

    //FUNC_ENTER1(" %d", wParam);
    const int ext = (HIWORD (lParam) & 0x0100) >> 8;
    BYTE KeyboardState[256] = { 0 };
    GetKeyboardState(KeyboardState); 
    KeyboardState[0] = 0xff;   // so that keystate=0 always works.

    for ( int x = 0; keyup2msg[x].win32_key != -1; ++x )
    {
        if (   (int)wParam == keyup2msg[x].win32_key
            && (keyup2msg[x].ctx || !m_bHandledKey)
            && keyup2msg[x].ext == ext
            && (KeyboardState[keyup2msg[x].keystate]&0x80))
        {
            QueueKey(keyup2msg[x].func_key);
            break;
        }
    }
    return 0;
}

LRESULT CTerm5250::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = false;
    if ( !m_bEnabled || !m_nReadyState )
        return 0;

    //FUNC_ENTER1(" %d", wParam);
    //	const int ctx = HIWORD (lParam) & 0x2000;
    //	const int ext = (HIWORD (lParam) & 0x0100) >> 8;
    BYTE KeyboardState[256] = { 0 };
    GetKeyboardState(KeyboardState); 
    KeyboardState[0] = 0xff;   // so that keystate=0 always works.

    m_bHandledKey = false;
    for ( int x = 0; keydown2msg[x].win32_key != -1; ++x )
    {
        if (   (DWORD)wParam == keydown2msg[x].win32_key
            //		&& keydown2msg[x].ctx == ctx		// otherwise the Virtual Keyboard doesn't work.
            //		&& keydown2msg[x].ext == ext
            && (KeyboardState[keydown2msg[x].keystate]&0x80))
        {
            for (int repeat = LOWORD(lParam); repeat>0; --repeat)
                QueueKey(keydown2msg[x].func_key);

            TN5250_LOG(("WM_KEYDOWN: handling key\n"));
            m_bHandledKey = true;
            return 1;
        }
    }

    return 0;
}

LRESULT CTerm5250::OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
    bHandled = false;
    if ( !m_bEnabled || !m_nReadyState )
        return 0;

    //FUNC_ENTER1(" %d", wParam);
    // Search for conversion
    for ( int x=0; win_kb[x].win32_key != -1; ++x )
    {
        if (wParam == win_kb[x].win32_key)
        {
            wParam = win_kb[x].tn5250_key;
            break;
        }
    }
    m_bHandledKey = true;
    TN5250_LOG(("WM_CHAR: handling key\n"));
    QueueKey((int)wParam);
    return 0;
}

/*
LRESULT CTerm5250::OnRButtonDown(UINT /*uMsg* /, WPARAM /*wParam* /, LPARAM /*lParam* /, BOOL& /*bHandled* /)
{
//FUNC_ENTER();
if (m_bUnixLikeCopy)
{
PasteTextSelection();
}
// TBA Contextual menu??
return 0;
}
*/

LRESULT CTerm5250::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    if ( !m_bEnabled )
        return 0;

    FUNC_ENTER();

    // Retrieve focus
    //m_spInPlaceSite->SetFocus(TRUE);	// It CRASHES!! !?!?
    if ( m_hWndCD && GetFocus() != m_hWndCD )
    {
        SetFocus();
    }
    // We don't want to move the mouse on activation click...
    else if ( m_bMouseMoveCursor && m_nReadyState )
    {
        // Determine the new cursor position and the needed VK to 
        // move the cursor there
        const POINT P = { LOWORD(lParam), HIWORD(lParam) };
        CPoint NewCaretPos;
        PointToCursorPos(P, NewCaretPos);
        CPoint Diff( NewCaretPos - GetCaretPos() );

        ASSERT( abs(Diff.x) < tn5250_display_width(display)
            &&  abs(Diff.y) < tn5250_display_height(display) );

        for ( ; Diff.x < 0; ++Diff.x )
            QueueKey(K_LEFT);
        for ( ; Diff.x > 0; --Diff.x )
            QueueKey(K_RIGHT);
        for ( ; Diff.y < 0; ++Diff.y )
            QueueKey(K_UP);
        for ( ; Diff.y > 0; --Diff.y )
            QueueKey(K_DOWN);
    }
    else if ( m_bSelectionEnabled )
    {
        m_bSelecting = true;
        m_bSelected = true;
        m_rctSelection.left = (short)LOWORD(lParam);
        m_rctSelection.top = (short)HIWORD(lParam);
        m_rctSelection.right = m_rctSelection.left;
        m_rctSelection.bottom = m_rctSelection.top;

        if ( m_hWnd )
            SetCapture();

        SetCursor(LoadCursor(NULL, IDC_CROSS));
        FireViewChange();	// No need to refresh the screen buffer
    }

    return 0;
}


LRESULT CTerm5250::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    if ( !m_bEnabled )
        return 0;

    if (m_bSelecting)
    {
        //FUNC_ENTER();
        m_rctSelection.right = (short)LOWORD(lParam);
        m_rctSelection.bottom = (short)HIWORD(lParam);
        FireViewChange();	// No need to refresh the screen buffer
    }
    return 0;
}


LRESULT CTerm5250::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    if ( !m_bEnabled )
        return 0;

    FUNC_ENTER();
    if (m_bSelecting)
    {
        ReleaseCapture();
        SetCursor(LoadCursor(NULL, IDC_ARROW));

        m_bSelecting = false;
        ASSERT(m_bSelected == true);
        m_rctSelection.right = (short)LOWORD(lParam);
        m_rctSelection.bottom = (short)HIWORD(lParam);
        // If the selection is too small, don't select at all
        if ( abs(m_rctSelection.Height()) < 3 && abs(m_rctSelection.Width()) < 3 )
        {
            m_bSelected = false;
        }
        else
        {
            ExpandTextSelection();
            if (m_bUnixLikeCopy)
                CopyTextSelection();
        }
        // TBA Should invalidate only selected part
        FireViewChange();
    }
    return 0;
}


LRESULT CTerm5250::ForwardMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    FUNC_ENTER();
    PostThreadMessage(m_ThreadId, uMsg, wParam, lParam);
    return 0;
}

/*
On WM_CREATE
OnActivateInPlace() <- request activation
// http://support.microsoft.com/default.aspx?scid=kb;en-us;168777
LRESULT CTerm5250::OnCreate(UINT, WPARAM, LPARAM, BOOL&)
{
// MFC...
OnActivateInPlace (TRUE, NULL); // == UI-Activate the control
return 0;
}
*/

LRESULT CTerm5250::OnMouseActivate(UINT, WPARAM, LPARAM, BOOL&)
{
    // Manually activate the control
    InPlaceActivate(OLEIVERB_UIACTIVATE);

    return 0;
}

// Controls which message to giveup to windows.
BOOL CTerm5250::PreTranslateAccelerator(LPMSG /*pMsg*/, HRESULT & hRet) throw()
{
    /*	if ( pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP )
    {
    switch (pMsg->wParam)
    {
    case VK_F1 :		case VK_F2 :		case VK_F3 :		case VK_F4 :		case VK_F5 :		case VK_F6 :		case VK_F7 :		case VK_F8 :		case VK_F9 :		case VK_F10:		case VK_F11:		case VK_F12:		case VK_F13:		case VK_F14:		case VK_F15:		case VK_F16:		case VK_F17:		case VK_F18:		case VK_F19:		case VK_F20:		case VK_F21:		case VK_F22:		case VK_F23:		case VK_F24:
    case VK_PRIOR:		case VK_NEXT :		case VK_END  :		case VK_HOME :		case VK_LEFT :		case VK_UP   :		case VK_RIGHT:		case VK_DOWN :		case VK_INSERT:		case VK_DELETE:
    hRet = S_FALSE;	return TRUE;
    default:
    }
    }
    return FALSE;	*/

    // NONE! :)
    //FUNC_ENTER3(" %u %u %u", pMsg->message, pMsg->wParam, pMsg->lParam);
    //hRet = S_FALSE;
    hRet = E_FAIL;
    return TRUE;
}


// Should be call in the worker thread
void CTerm5250::PostBufferRefresh() const throw()
{
    // Set a 10 ms timer to redraw, otherwise when queuing keys, it refreshes
    // uselessly too often.
    //SetTimer(m_hWnd, 0, 0, TimerProc);
    LARGE_INTEGER DueTime;
    DueTime.QuadPart = -10000 * 10;	// in 100nsec units		(10 ms)
    SetWaitableTimer(m_hTimer, &DueTime, 0, NULL, NULL, FALSE);
}


// simulates the SetControlSize of MFC's COleControl
// Updates the values to count the border size.
void CTerm5250::SetSize(int cx, int cy)
{
    //AdjustWindowRectEx(&Rect, Style, FALSE, Ex);
    //	GetSystemMetrics(SM_CXBORDER);SM_CXEDGE
    //	GetSystemMetrics(SM_CYBORDER);SM_CYEDGE
    CRect rctW;
    GetWindowRect(&rctW);
    CRect rctC;
    GetClientRect(&rctC);
    ASSERT(rctW.Height() == rctC.Height()
        && rctW.Width() == rctC.Width());

    FUNC_ENTER2(" %dx%d", cx, cy);
    // Check for size change
    if ( cx == GetRect().Width() && cy == GetRect().Height() )
        return;
    const SIZEL szlPixels = { cx, cy };
    SIZEL szlMetric;
    AtlPixelToHiMetric(&szlPixels, &szlMetric);
    // IOleObjectImpl
    SetExtent(DVASPECT_CONTENT, &szlMetric);

    // update control sizing...
    m_rcPos.right = m_rcPos.left + cx;
    m_rcPos.bottom = m_rcPos.top + cy;
    if (m_spInPlaceSite != NULL)
    {
        // needed for IE to accept the resizing
        m_spInPlaceSite->OnPosRectChange(&m_rcPos);
    }
}


// Hide the caret, usually done when the window loses focus
void CTerm5250::HideCaret()
{
    FUNC_ENTER();
    if ( m_hWnd )
        super::HideCaret();
    DestroyCaret();
}

void CTerm5250::OnEnabledChanged()
{
    FUNC_ENTER();
    if ( !m_bEnabled )
    {
        m_bSelecting = false;
        m_bSelected = false;
    }
}


STDMETHODIMP CTerm5250::SendVKey(DWORD VKey)
{
    FUNC_ENTER1(" %d", VKey);
    if ( m_nReadyState )
    {
        for ( int x = 0; keydown2msg[x].win32_key != -1; ++x )
        {
            if (   VKey == keydown2msg[x].win32_key
                //	&& keydown2msg[x].ctx == ctx		// otherwise the Virtual Keyboard doesn't work.
                //	&& keydown2msg[x].ext == ext
                && !keydown2msg[x].keystate)
            {
                QueueKey(keydown2msg[x].func_key);
                return S_OK;
            }
        }
    }
    return S_FALSE;
}


STDMETHODIMP CTerm5250::SendChar(USHORT Character)
{
    FUNC_ENTER1(" %d", Character);
    if ( m_nReadyState )
    {
        // Search for conversion
        for ( int x=0; win_kb[x].win32_key != -1; ++x )
        {
            if (Character == win_kb[x].win32_key)
            {
                Character = (USHORT)win_kb[x].tn5250_key;
                break;
            }
        }
        QueueKey((int)Character);
        return S_OK;
    }
    else
        return S_FALSE;
}


void CTerm5250::OnCaptionChanged()
{
    FUNC_ENTER();
    RefreshScreenBuffer(false);
}

// Almost same as CComControlBase::IOleObject_SetExtent
HRESULT CTerm5250::IOleObject_SetExtent(DWORD dwDrawAspect, SIZEL *psizel)
{
    FUNC_ENTER3(" %d  %dx%d", dwDrawAspect, psizel->cx, psizel->cy);
    if (dwDrawAspect != DVASPECT_CONTENT)
        return DV_E_DVASPECT;
    if (psizel == NULL)
        return E_POINTER;

    const BOOL bSizeMatchesNatural = memcmp(psizel, &m_sizeNatural, sizeof(SIZE)) == 0;

    // m_bAutoSize doesn't have the "same" meaning here
    //	if (m_bAutoSize) //object can't do any other size
    //		return (bSizeMatchesNatural) ? S_OK : E_FAIL;

    if (memcmp(psizel, &m_sizeExtent, sizeof(SIZE)) != 0)
    {
        m_sizeExtent = *psizel;
        m_bResized = true;
    }
    if (m_bResizeNatural && !bSizeMatchesNatural)
    {
        m_sizeNatural = *psizel;
        m_bResized = true;
    }

    if (m_bRecomposeOnResize && m_bResized)
    {
        // Try to adapt this size for our use
        if ( m_bAutoSize )
        {
            RECT MySize = {0};
            AtlHiMetricToPixel(psizel, (SIZE*)(&MySize.right));	// Crosse
            SIZE FontSize;
            const int NbCols = GetCols();
            const int NbRows = GetRows();
            const bool Fit = CalcMaxFontSize(MySize, NbCols, NbRows, FontSize);
            Fit;
            ASSERT( !Fit );

            // TBM !!!

            // Set the window a bit smaller than told to us
            MySize.right  = FontSize.cx*NbCols + (m_bBorderVisible?(m_nBorderWidth*2):0);
            MySize.bottom = FontSize.cy*NbRows + (m_bBorderVisible?(m_nBorderWidth*2):0);

            AtlPixelToHiMetric((SIZE*)(&MySize.right), psizel);
        }

        // Repaint (the screen buffer will be regenerated in IOleInPlaceObject_SetObjectRects)
        SendOnDataChange();
        FireViewChange();
        //	return E_FAIL;
    }
    return S_OK;
}

HRESULT CTerm5250::IOleInPlaceObject_SetObjectRects(LPCRECT prcPos, LPCRECT prcClip)
{
    // Will be opt'ed out
    const CRect * const pRect = (const CRect * const)prcPos;
    if ( pRect->Height() != GetRect().Height() || pRect->Width() != GetRect().Width() )
        FUNC_ENTER2(" %dx%d", prcPos->right-prcPos->left, prcPos->bottom-prcPos->top);

    CRect Old(m_rcPos);
    HRESULT RetVal = super::IOleInPlaceObject_SetObjectRects(prcPos, prcClip);
    if ( SUCCEEDED(RetVal) )
    {
        // If the size changed
        if ( GetRect().Width() != Old.Width() || GetRect().Height() != Old.Height() )
            RegenScreenBuffer();
    }
    return RetVal;
}

void CTerm5250::OnBorderVisibleChanged()
{
    FUNC_ENTER1(" %s", m_bBorderVisible?_T("TRUE"):_T("FALSE"));
    // It affects the size of the object
    RegenScreenBuffer();
}

void CTerm5250::OnAutoSizeChanged()
{
    FUNC_ENTER1(" %s", m_bAutoSize?_T("TRUE"):_T("FALSE"));
    if ( m_bAutoSize )
    {
        // Try to resize the window
        RegenScreenBuffer();
    }
}

void CTerm5250::OnAppearanceChanged()
{
    FUNC_ENTER();
    // For now until it gets fixed.
    m_nAppearance = 0;

    if (m_hWnd != NULL)
    {
        // toggle border style and force redraw of border
        DWORD Val = GetExStyle();
        if ( m_nAppearance && m_bBorderVisible )
            Val |= WS_EX_CLIENTEDGE;
        else
            Val &= (~WS_EX_CLIENTEDGE);
        SetWindowLong(GWL_EXSTYLE, Val );
        SetWindowPos(NULL, 0, 0, 0, 0, SWP_DRAWFRAME|SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
    }
    //DrawEdge(); <- here is my solution!!
}

bool CTerm5250::CanEdit(int Id) throw()
{
    return m_nFreezeEvents != 0 || FireOnRequestEdit(Id) == S_OK;
}

void CTerm5250::ValueChanged(int Id) throw()
{
    m_bRequiresSave = TRUE;
    if (m_nFreezeEvents == 0)
        FireOnChanged(Id);
    FireViewChange();
    SendOnDataChange(NULL);
}

void CTerm5250::OnBorderColorChanged()
{
    FUNC_ENTER();
    RefreshScreenBuffer(false);
}

// Returns the next key from the keyboard buffer.
int CTerm5250::GetKey()
{
    // we don't actually read the keyboard here... that's done by win32_terminal_wndproc.
    //FUNC_ENTER();

    // Lock the access
    CAutoLock Lock(m_KbBufCS);

    if (m_KbBuf_len == 0)
        return -1;

    const int Key = m_KbBuf[0];
    --m_KbBuf_len;
    //memmove(m_KbBuf, m_KbBuf+1, m_KbBuf_len);
    for (int j=0; j<m_KbBuf_len; j++) 
        m_KbBuf[j] = m_KbBuf[j+1];

    switch (Key)
    {
    case K_CTRL('Q'):
        {
            // Disconnect from the server by post a quit message to the thread
            PostThreadMessage(m_ThreadId, WM_QUIT, 0, 0);
        }
        return -1;
        /*	case K_PRINT:
        if (local_print)
        {
        win32_print_screen(This, globDisplay);
        ch = K_RESET;
        }
        break;
        */
    case 0x0a:
        // Convert the ENTER key
        return 0x0d;

    default:
        return Key;
    }
}


// Calculate a new font size. Returns true if the size is "fit"
bool CTerm5250::CalcMaxFontSize(const RECT & cr, int cols, int rows, SIZE & o_fontSize)
{
    const int cli_width  = (cr.right  - cr.left) - (m_bBorderVisible?(m_nBorderWidth*2):0);
    const int cli_height = (cr.bottom - cr.top)  - (m_bBorderVisible?(m_nBorderWidth*2):0);
    o_fontSize.cx = cli_width / cols;
    o_fontSize.cy = cli_height / rows;
    const bool Fit = cli_width*cols==o_fontSize.cx && cli_height*rows==o_fontSize.cy;
    FUNC_ENTER7(" %dx%d -> %dx%d = %dx%d %s", cli_width, cli_height, cols, rows, o_fontSize.cx, o_fontSize.cy, (Fit?"Fit":"Not fit") );
    return Fit;
}


// This takes the font name, height & width that you pass and finds the closest match on the system.
// NOT TRUE -> It then re-sizes the terminal window to the size of the font that it used.
void CTerm5250::LoadTerminalFont()
{
    const int cols = GetCols();
    const int rows = GetRows();
    CSize DesiredFontSize;
    // If not fit, we should resize now
    const bool Fit = CalcMaxFontSize(m_rcPos, cols, rows, DesiredFontSize);

    // Ben faut pas reloader pour rien!!
    if ( DesiredFontSize != m_FontSize )
    {
        for (;;)
        {
            // create a font using the size from our config data
            if ( m_hFont )
                DeleteObject(m_hFont);
            m_hFont = CreateFont(DesiredFontSize.cy, DesiredFontSize.cx, 0, 0, FW_DONTCARE, 
                FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
                CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH|FF_DONTCARE, 
                m_FontName);

            // Set the new font as active
            ASSERT(m_hMemDC);
            SelectObject(m_hMemDC, m_hFont);
            TEXTMETRIC tm;
            GetTextMetrics(m_hMemDC, &tm);

            // Calculate the actual size of the font we selected
            m_FontSize.cy = tm.tmHeight + tm.tmExternalLeading;
            m_FontSize.cx = tm.tmAveCharWidth;	// tmMaxCharWidth

            FUNC_ENTER5(" \"%s\" at %dx%d, got %dx%d", m_FontName.GetString(), DesiredFontSize.cx, DesiredFontSize.cy,  m_FontSize.cx, m_FontSize.cy);
            if (m_FontSize.cx <= DesiredFontSize.cx && m_FontSize.cy <= DesiredFontSize.cy)
                break;

            // illisible anyway
            if ( DesiredFontSize.cx < 4 || DesiredFontSize.cy < 4 )
                break;

            // Otherwise, loop until we got a font small enough.
            // Reducing the other dimension that the one is offending.
            // Terminal is the kind of font that gives us trouble...
            if ( m_FontSize.cy > DesiredFontSize.cy )
                --DesiredFontSize.cx;
            else if ( m_FontSize.cx > DesiredFontSize.cx )
                --DesiredFontSize.cy;
        }

        // This builds an array telling Windows how to space the text.
        // Some fonts end up being slightly smaller than the text metrics
        // tell us that they are... so without this, the caret (aka "cursor")
        // will not appear in the right place.
        // So set up the font spacing array for this font
        m_pSpacing.Free();
        m_pSpacing.Allocate(cols+1);
        for (int x=0; x<=cols; ++x) 
            m_pSpacing[x] = m_FontSize.cx;

        if (m_hWnd == GetFocus()) 
            HideCaret();
    }
    else
    {
        FUNC_ENTER0(" font fited");
    }

    // Make the system resize the window
    if (m_bAutoSize && !Fit)
    {
        // TBM Not anymore!! We *think* that WindowRect == ClientRect... (in size I mean)

        // Watch out for infinite loop!!
        ASSERT( m_FontSize.cx > 0 && m_FontSize.cy > 0 );
        if ( m_FontSize.cx > 0 && m_FontSize.cy > 0 )
        {
            const int Delta = m_bBorderVisible?(2*m_nBorderWidth):0;
            SetSize(m_FontSize.cx*cols + Delta, m_FontSize.cy*rows + Delta);
        }
    }

    // draw the new caret
    if (m_hWnd == GetFocus())
    {
        HDC hdc = GetDC();      
        MakeNewCaret();
        if (m_CaretStyle != CARETSTYLE_NOBLINK)
        {
            MoveCaret(hdc);
            ShowCaret();
        }
        ReleaseDC(hdc);
    }
}


// Add a key to the terminal's keyboard buffer
void CTerm5250::QueueKey(int key)
{
    FUNC_ENTER1(" '%c'", key);
    switch (key)
    {
    case K_PASTE_TEXT:
        PasteTextSelection();
        break;

    case K_COPY_TEXT:
        if (m_bSelected)
        {
            ExpandTextSelection();
            CopyTextSelection();
            m_bSelected = false;
            RefreshScreenBuffer();
        }
        break;

    default:
        {
            CAutoLock Lock(m_KbBufCS);
            if (m_KbBuf_len<MAX_K_BUF_LEN)
            {
                m_KbBuf[m_KbBuf_len] = key;
                ++m_KbBuf_len;
            }
            // Signal other thread
            PostThreadMessage(m_ThreadId, WM_TN5250_KEY_DATA, 0, 0);
        }
        break;
    }
}


/* DESCRIPTION
*    If you're wondering, "Caret" is the Windows term for
*    the cursor used when typing at the keyboard.  In Windows
*    terminology, cursor=mouse, caret=keyboard.
*
*    There is only one cursor in Windows, shared by all Windows apps.
*    So, we create it when focus returns to us, and destroy it when
*    we lose focus.  This is where it gets created.
*****/
void CTerm5250::MakeNewCaret()
{
    FUNC_ENTER();
    if (m_CaretStyle == CARETSTYLE_NOBLINK)
    {
        // We make the Windows Caret invisible, so we can maintain control
        // of the caret without the user seeing it blink
        const int bytewidth = (m_FontSize.cx + 15) / 16 * 2;
        const int size = bytewidth * m_FontSize.cy;
        CAutoPtr<BYTE> bits(new BYTE[size]);
        memset(bits, 0x00, size);
        ASSERT( !m_hCaretBMP );
        m_hCaretBMP = CreateBitmap(m_FontSize.cx, m_FontSize.cy, 1, 1, bits);
        ::CreateCaret(m_hWnd, m_hCaretBMP, m_FontSize.cy, m_FontSize.cx);
    }

    // Here we create a small bitmap to use as the caret
    // we simply draw a line at the bottom of the bitmap
    if (m_CaretStyle == CARETSTYLE_LINE )
    {
        const int bytewidth = (m_FontSize.cx + 15) / 16 * 2;
        const int size = bytewidth * m_FontSize.cy;
        CAutoPtr<BYTE> bits(new BYTE[size]);
        memset(bits, 0x00, size);
        ASSERT(!m_hCaretBMP);
        m_hCaretBMP = CreateBitmap(m_FontSize.cx, m_FontSize.cy, 1, 1, bits);
        HDC hdc = CreateCompatibleDC(NULL);
        SelectObject(hdc, m_hCaretBMP);
        HPEN MyPen = CreatePen(PS_SOLID, 0, RGB(255,255,255));
        HGDIOBJ OldPen = SelectObject(hdc, MyPen);
        MoveToEx(hdc, 0, m_FontSize.cy-2, NULL);
        LineTo(hdc, m_FontSize.cx, m_FontSize.cy-2);
        SelectObject(hdc, OldPen);
        DeleteObject(MyPen);
        DeleteDC(hdc);
        ::CreateCaret(m_hWnd, m_hCaretBMP, m_FontSize.cy, m_FontSize.cx);
    }
    else
    {
        // for the standard "blinking block", we just use the windows default
        // shape for the caret
        ::CreateCaret(m_hWnd, NULL, m_FontSize.cx, m_FontSize.cy);
    }
}

// Move the caret to a position on the screen, to the coordinates in caretx, carety
void CTerm5250::MoveCaret(HDC hdc)
{
    //FUNC_ENTER();
    // move the Windows caret
    const int Delta = m_bBorderVisible?m_nBorderWidth:0;
    const CPoint posCaret = GetCaretPos();
    SetCaretPos(posCaret.x*m_FontSize.cx+Delta, posCaret.y*m_FontSize.cy+Delta);

    // Since the Windows caret is invisible, make our own box now
    if ( (m_CaretStyle == CARETSTYLE_NOBLINK) && !m_bCaretOK )
    {
        HGDIOBJ savepen   = SelectObject(hdc, GetStockObject(WHITE_PEN));
        HGDIOBJ savebrush = SelectObject(hdc, GetStockObject(WHITE_BRUSH));
        const int savemode = SetROP2(hdc, R2_NOT);
        const CPoint posCaret = GetCaretPos();
        Rectangle(hdc, 
            (posCaret.x  )*m_FontSize.cx+Delta, (posCaret.y  )*m_FontSize.cy+Delta,
            (posCaret.x+1)*m_FontSize.cx+Delta, (posCaret.y+1)*m_FontSize.cy+Delta);
        SetROP2(hdc, savemode);
        SelectObject(hdc, savepen);
        SelectObject(hdc, savebrush);
        m_bCaretOK = true;
    }
}

// This converts the mouse selection points (defined by selstr & selend)
// to a rectangle of selected text (by aligning the points with the text
// start/end pos)
void CTerm5250::ExpandTextSelection()
{
    FUNC_ENTER();
    ASSERT(m_FontSize.cx>0);

    // change the points so that selstr is the upper left corner 
    // and selend is the lower right corner.
#define TN5250_FLIPEM(a, b)  if (a>b) { const LONG x = a; a = b; b = x; }
    TN5250_FLIPEM(m_rctSelection.left, m_rctSelection.right)
        TN5250_FLIPEM(m_rctSelection.top, m_rctSelection.bottom)
#undef TN5250_FLIPEM

        if ( m_bBorderVisible )
        {
            // displace
            m_rctSelection.left -= m_nBorderWidth;
            m_rctSelection.top -= m_nBorderWidth;
            m_rctSelection.right -= m_nBorderWidth;
            m_rctSelection.bottom -= m_nBorderWidth;
        }

        const int Delta = m_bBorderVisible?m_nBorderWidth:0;

        // constrain the coordinates to the window's client area
        if (m_rctSelection.left < 0)	m_rctSelection.left = 0;
        if (m_rctSelection.top  < 0)	m_rctSelection.top = 0;
        if (m_rctSelection.right  > (GetRect().Width()-2*Delta))	m_rctSelection.right  = (GetRect().Width()-2*Delta);
        if (m_rctSelection.bottom > (GetRect().Height()-2*Delta))	m_rctSelection.bottom = (GetRect().Height()-2*Delta);

        // move selection start position to nearest character
        int cx = m_rctSelection.left / m_FontSize.cx;
        m_rctSelection.left = cx * m_FontSize.cx;
        int cy = m_rctSelection.top / m_FontSize.cy;
        m_rctSelection.top = cy * m_FontSize.cy;

        // move selection end position to nearest character
        cx = m_rctSelection.right / m_FontSize.cx;
        if (m_rctSelection.right % m_FontSize.cx) 
            cx++;
        m_rctSelection.right = cx * m_FontSize.cx;
        cy = m_rctSelection.bottom / m_FontSize.cy;
        if (m_rctSelection.bottom % m_FontSize.cy)
            cy++;
        m_rctSelection.bottom = cy * m_FontSize.cy;

        if ( m_bBorderVisible )
        {
            // displace
            m_rctSelection.left += m_nBorderWidth;
            m_rctSelection.top += m_nBorderWidth;
            m_rctSelection.right += m_nBorderWidth;
            m_rctSelection.bottom += m_nBorderWidth;
        }
}


// This retrieves the text in the selected area and copies it to the global Windows clipboard.
void CTerm5250::CopyTextSelection()
{
    FUNC_ENTER();
    ASSERT(display!=NULL);
    ASSERT(m_FontSize.cx>0);

    if ( m_bBorderVisible )
    {
        // displace
        m_rctSelection.left -= m_nBorderWidth;
        m_rctSelection.top -= m_nBorderWidth;
        m_rctSelection.right -= m_nBorderWidth;
        m_rctSelection.bottom -= m_nBorderWidth;
    }

    // figure out the dimensions (in text chars) and make a global buffer
    int sx = m_rctSelection.left / m_FontSize.cx;
    int ex = m_rctSelection.right / m_FontSize.cx;
    int sy = m_rctSelection.top / m_FontSize.cy;
    int ey = m_rctSelection.bottom / m_FontSize.cy;

    int Height = tn5250_display_height(display);
    if (ey>Height)
        ey = Height;
    if (ey<sy)
        sy=ey;
    const int bufsize = ((ex-sx)+1) * ((ey-sy)+1) + 1;
    HGLOBAL hBuf = GlobalAlloc(GHND|GMEM_SHARE, bufsize);
    ASSERT(hBuf!=NULL);

    // populate the global buffer with the text data, inserting CR/LF 
    // in between each line that was selected
    char * buf = (char*)GlobalLock(hBuf); 
    int bp = -1;
    for (int y = sy; y < ey; y++)
    {
        for (int x = sx; x < ex; x++)
        {
            char c = tn5250_display_char_at(display, y, x);
            if (((c & 0xe0) == 0x20 )||(c < 0x40 && c > 0x00)||(c == 0xff)) 
                c = ' ';
            else 
                c = tn5250_char_map_to_local( tn5250_display_char_map(display), c);
            bp++;
            if (bp==bufsize) break;
            buf[bp] = c;
        }

        if (y != (ey-1))
        {
            bp++;
            if (bp==bufsize) break;
            buf[bp] = '\r';
            bp++;
            if (bp==bufsize) break;
            buf[bp] = '\n';
        }
    }
    GlobalUnlock(hBuf);

    if ( m_bBorderVisible )
    {
        // displace
        m_rctSelection.left += m_nBorderWidth;
        m_rctSelection.top += m_nBorderWidth;
        m_rctSelection.right += m_nBorderWidth;
        m_rctSelection.bottom += m_nBorderWidth;
    }

    // create a bitmap version of the copy buffer as well... 
    // this allows image programs to paste the buffer as a bitmap
    const int cx = (m_rctSelection.right - m_rctSelection.left) + 1; 
    const int cy = (m_rctSelection.bottom - m_rctSelection.top) + 1;

    // Weirdo ?
    HDC hdc = GetDC();
    HBITMAP hBm = CreateCompatibleBitmap(hdc, cx, cy);
    ReleaseDC(hdc);
    hdc = CreateCompatibleDC(NULL);
    SelectObject(hdc, hBm);
    BitBlt(hdc, 0, 0, cx, cy, m_hMemDC, m_rctSelection.left, m_rctSelection.top, SRCCOPY);
    DeleteDC(hdc);

    // finally, copy both the global buffer & the bitmap to the 
    // clipboard.  After this, we should not try to use (or free!)
    // the buffer/bitmap... they're Windows' property now! 
    OpenClipboard();
    EmptyClipboard();

    switch (m_CopyMode)
    {
    case Both:
        SetClipboardData(CF_TEXT, hBuf);
        SetClipboardData(CF_BITMAP, hBm);
        break;
    case Text:  // plain text only
        SetClipboardData(CF_TEXT, hBuf);
        DeleteObject(hBm);
        break;
    case Picture:  // bitmap only
        SetClipboardData(CF_BITMAP, hBm);
        GlobalFree(hBuf);
        break;
    }
    CloseClipboard();
}


// Convert data in the windows clipboard into keystrokes and paste them into the keyboard buffer.
// \note Increasing the MAX_K_BUF_LEN will speed this routine up...
void CTerm5250::PasteTextSelection()
{
    FUNC_ENTER();
    char *pNewBuf = NULL;

    // If there's any data that we can paste, read it from the clipboard. 
    if (IsClipboardFormatAvailable(CF_TEXT))
    {
        OpenClipboard();

        HGLOBAL hBuf = ::GetClipboardData(CF_TEXT);

        if (hBuf != NULL)
        {
            const int size = (int)GlobalSize(hBuf);
            pNewBuf = new char[size];
            ASSERT(pNewBuf!=NULL);
            const char * pBuf = (const char*)GlobalLock(hBuf);
            strncpy(pNewBuf, pBuf, size);
            pNewBuf[size] = '\0'; // just a precaution
            GlobalUnlock(hBuf);
        }
        CloseClipboard();
    }

    // convert text data into keyboard messages,  just as if someone
    // was typing this data at the keyboard.
    if (pNewBuf != NULL)
    {
        const int size = strlen(pNewBuf);
        int thisrow = 0;
        for (int pos=0; pos<size; pos++)
        {
            switch (pNewBuf[pos])
            {
            case '\r':
                // Bâtard
                while (thisrow > 0)
                {
                    QueueKey(K_LEFT);
                    --thisrow;
                    if (m_KbBuf_len == MAX_K_BUF_LEN)
                        tn5250_display_do_keys(display);
                }
                break;
            case '\n':
                thisrow = 0;
                QueueKey(K_DOWN);
                tn5250_display_do_keys(display);
                break;
            default:
                thisrow++;
                QueueKey(pNewBuf[pos]);
                break;
            }
            if (m_KbBuf_len == MAX_K_BUF_LEN)
                tn5250_display_do_keys(display);
        }
        delete [] pNewBuf;
    }
}

void CTerm5250::OnForeColorChanged()
{
    //	FUNC_ENTER();
    m_ColorList[A_5250_WHITE] = m_clrForeColor;
    RefreshScreenBuffer(false);

    // MONOCHROME :
    // set color list for black on white
    //"black_on_white"))
    //for (i=A_5250_GREEN; i<A_5250_BLACK; i++)
    //m_ColorList[i] = RGB(0,0,0);
    //m_ColorList[A_5250_BLACK] = RGB(255,255,255);
    // or
    //"white_on_black"
    //for (i=A_5250_GREEN; i<A_5250_BLACK; i++)
    //m_ColorList[i] = RGB(255,255,255);
    //m_ColorList[A_5250_BLACK] = RGB(0,0,0);
}

void CTerm5250::OnBackColorChanged()
{
    //	FUNC_ENTER();
    m_ColorList[A_5250_BLACK] = m_clrBackColor;

    ASSERT( m_hBackgroundBrush );
    DeleteObject(m_hBackgroundBrush);
    m_hBackgroundBrush = CreateSolidBrush(GetColor(A_5250_BLACK));
    RefreshScreenBuffer(false);
}


STDMETHODIMP CTerm5250::get_MouseMoveCursor(VARIANT_BOOL * pVal)
{
    FUNC_ENTER();
    if ( !pVal )
        return E_INVALIDARG;

    *pVal = m_bMouseMoveCursor?-1:0;
    return S_OK;
}

STDMETHODIMP CTerm5250::put_MouseMoveCursor(VARIANT_BOOL newVal)
{
    FUNC_ENTER1(" %d", newVal);
    if ( !CanEdit(2) )
        return S_FALSE;

    m_bMouseMoveCursor = newVal?true:false;
    if ( m_bMouseMoveCursor )
    {
        m_bSelected = false;
        m_bSelecting = false;
    }
    ValueChanged(2);
    return S_OK;
}


STDMETHODIMP CTerm5250::get_CaretStyle(ECaretStyle * pVal)
{
    if ( !pVal )
        return E_INVALIDARG;

    *pVal = m_CaretStyle;
    return S_OK;
}

STDMETHODIMP CTerm5250::put_CaretStyle(ECaretStyle newVal)
{
    FUNC_ENTER1(" %d", newVal);
    if ( !CanEdit(3) )
        return S_FALSE;

    m_CaretStyle = newVal;
    ValueChanged(3);
    return S_OK;
}


STDMETHODIMP CTerm5250::get_Colors(OLE_COLOR * colors[10])
{
    if ( !colors )
        return E_INVALIDARG;

    memcpy(colors, m_ColorList, sizeof(m_ColorList));
    return S_OK;
}

STDMETHODIMP CTerm5250::put_Colors(const OLE_COLOR colors[10])
{
    FUNC_ENTER1(" %d", colors[0]);
    if ( !CanEdit(4) )
        return S_FALSE;
    memcpy(m_ColorList, colors, sizeof(m_ColorList));
    RefreshScreenBuffer(false);
    ValueChanged(4);
    return S_OK;
}



STDMETHODIMP CTerm5250::get_SelectionEnabled(VARIANT_BOOL* pVal)
{
    if ( !pVal )
        return E_INVALIDARG;

    *pVal = m_bSelectionEnabled?-1:0;
    return S_OK;
}

STDMETHODIMP CTerm5250::put_SelectionEnabled(VARIANT_BOOL newVal)
{
    FUNC_ENTER1(" %d", newVal);
    if ( !CanEdit(5) )
        return S_FALSE;

    m_bSelectionEnabled = newVal?true:false;
    ValueChanged(5);
    return S_OK;
}


void CTerm5250::PointToCursorPos(const POINT & in_Pixels, POINT & out_CaretPos) const throw()
{
    // Calc the deltas
    const int Delta = m_bBorderVisible?m_nBorderWidth:0;

    // move selection start position to nearest character
    out_CaretPos.x = (in_Pixels.x-Delta) / m_FontSize.cx;
    out_CaretPos.y = (in_Pixels.y-Delta) / m_FontSize.cy;
}


CPoint CTerm5250::GetCaretPos() const throw()
{
    ASSERT(tn5250_display_cursor_x(display)>=0 && tn5250_display_cursor_x(display) < tn5250_display_width(display)
        && tn5250_display_cursor_y(display)>=0 && tn5250_display_cursor_y(display) < tn5250_display_height(display) );
    return CPoint(tn5250_display_cursor_x(display), tn5250_display_cursor_y(display));
}


// ISupportsErrorInfo
STDMETHODIMP CTerm5250::InterfaceSupportsErrorInfo(REFIID riid)
{
    // TBA If we support other interface
    static const IID* arr[] = { &IID_ITerm5250 };
    for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
        if (InlineIsEqualGUID(*arr[i], riid))
            return S_OK;
    return S_FALSE;
}

