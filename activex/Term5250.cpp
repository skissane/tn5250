/* 
 * Copyright (C) 2001 Scott Klement
 * Copyright (C) 2003 Marc-Antoine.Ruel at Cardinal.com
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
#include "stdafx.h"
#include "Term5250.h"
#include "DlgConnect.h"

template<typename T>
inline void LTR(T & a, T & b)
{
    if (a>b)
        std::swap(a, b);
}

// See "Code-Page Identifiers" in MSDN: http://msdn.microsoft.com/library/en-us/intl/unicode_81rn.asp 
DWORD CodePageToCharSet(DWORD CodePage)
{
    /* Some supported charsets and their corresponding code page:
    THAI_CHARSET        874 
    SHIFTJIS_CHARSET    932 
    GB2312_CHARSET      936 
    HANGEUL_CHARSET     949 
    CHINESEBIG5_CHARSET 950 
    EASTEUROPE_CHARSET  1250
    RUSSIAN_CHARSET     1251
    ANSI_CHARSET        1252
    GREEK_CHARSET       1253
    TURKISH_CHARSET     1254
    HEBREW_CHARSET      1255
    ARABIC_CHARSET      1256
    BALTIC_CHARSET      1257
    JOHAB_CHARSET       1361
    */

    // All this depends on the code pages of transmaps.h
    switch (CodePage)
    {
        // 870  IBM EBCDIC "Latin-2" Eastern Europe
    case 870:
        return EASTEUROPE_CHARSET;  // 1250

        // 905  IBM EBCDIC Turkish - Latin3
        // 1026 IBM EBCDIC Turkish - Latin5
    case 905:
    case 1026:
        return TURKISH_CHARSET;     // 1254

        // 880  IBM EBCDIC Cyrillic
    case 880:
        return RUSSIAN_CHARSET;     // 1251

        // 420  IBM EBCDIC Arabic
    case 420:
        return ARABIC_CHARSET;      // 1256

        // 875  IBM EBCDIC Greek
    case 875:
        return GREEK_CHARSET;       // 1253

        // 424  IBM EBCDIC Hebrew
    case 424:
        return HEBREW_CHARSET;      // 1255

/*
        // 277  IBM EBCDIC Denmark, Norway
        // 278  IBM EBCDIC Finland, Sweden
    case 277:
    case 278:
        return BALTIC_CHARSET;

        // 256  IBM EBCDIC Netherlands
        // 273  IBM EBCDIC Austria, Germany
        // 280  IBM EBCDIC Italy
        // 284  IBM EBCDIC Spain, Latin America
        // 285  IBM EBCDIC United Kingdom
    case 256:
    case 273:
    case 280:
    case 284:
    case 285:
        return EASTEUROPE_CHARSET;

    // 290? IBM EBCDIC "Katakana Extended" Japan
    case 290:
        return SHIFTJIS_CHARSET;
        
    // 297  IBM EBCDIC France
    case 297:
        return DEFAULT_CHARSET;

        // 500  IBM EBCDIC "500V1" Belgium, Canada, Switzerland
        // 871  IBM EBCDIC Icelandic
    case 500:
    case 871:
        return EASTEUROPE_CHARSET;

        // 37   IBM EBCDIC US, Canada, Netherlands, Portugal, Brazil, Australia, New Zealand (default)
    case 37:
        */
    default:
        return ANSI_CHARSET;
        //
        //DEFAULT_CHARSET
        //CHINESEBIG5_CHARSET
        //GB2312_CHARSET
        //HANGUL_CHARSET
        //VIETNAMESE_CHARSET
        //JOHAB_CHARSET  // (Korean)
        //THAI_CHARSET 
    }
}

// CTerm5250
CTerm5250::CTerm5250() throw()
: m_nAppearance(0)
, m_clrBorderColor(GetSysColor(COLOR_ACTIVEBORDER)) // COLOR_WINDOWFRAME
, m_bBorderVisible(true)
, m_nBorderWidth(1)
, m_bstrCaption()
, m_bEnabled(true)
, m_nReadyState(false)
, m_HostName(L"127.0.0.1")
, m_Device(L"")
, m_CodePage(37)
, m_eCaretStyle(CARETSTYLE_BLINK)
, m_CopyMode(Both)
, m_FontName(_T("Lucida Console"))	// Terminal  Courier New
, config(NULL)
, sess(NULL)
, stream(NULL)
, display(NULL)
, macro(NULL)
, m_ThreadId(0)
, m_hFont(NULL)
, m_hScreenBMP(NULL)
, m_hMemDC(NULL)
, m_hCaretBMP(NULL)
, m_hBackgroundBrush(NULL)
, m_ColSepStyle(COLSEPSTYLE_NONE)
, m_KbBuf_len(0)
, m_FontSize(1,1)
, m_Last_size(GetCols(),GetRows())
, m_rctSelection(0,0,0,0)
//, m_PD(NULL)
//, m_bLocalPrint(false)
, m_bHandledVirtualKey(false)
, m_bSelecting(false)
, m_bSelected(false)
, m_bSelectionEnabled(true)
, m_bUnixLikeCopy(false)
, m_bHotSpots(true)
, m_bDisplayRuler(false)
, m_bMouseMoveCursor(true)
, m_bEnhanced(false)
, m_bContextualMenu(true)
, m_bFirstClear(false)
, m_bShowConnect(true)
, m_bFocus(false)
, m_bFontChanged(false)
, m_bUseComputerName(false)
, m_bWide(false)
, m_ScreenBufferState(Regen)
#if _DEBUG
, m_VKCache(TypeLib(), _T("EVirtualKeyCodes"))
, m_KCache(TypeLib(), _T("EIBMKey"))
#endif
{
    FUNC_ENTER();
    dbgSetThreadName("CTerm5250 Thread");
    term.data = (_Tn5250TerminalPrivate *)this;     // Should be fixed one day...
    term.conn_fd = (SOCKET)-1;
    // They are never called anyway!
    term.init = NULL;
    term.term = NULL;
    term.destroy = win32_terminal_destroy;
    term.width = NULL;
    term.height = NULL;
    // They are called
    term.flags = ::win32_terminal_flags;
    term.update = ::win32_terminal_update;
    term.update_indicators = ::win32_terminal_update_indicators;
    term.waitevent = ::win32_terminal_waitevent;
    term.getkey = ::win32_terminal_getkey;
    term.beep = ::win32_terminal_beep;
    term.enhanced = ::win32_terminal_enhanced;
    term.config = NULL;
    term.create_window = ::win32_terminal_create_window;
    term.destroy_window = ::win32_terminal_destroy_window;
    term.create_scrollbar = ::win32_terminal_create_scrollbar;
    term.destroy_scrollbar = ::win32_terminal_destroy_scrollbar;

    m_ColorList[A_5250_GREEN            ] = RGB(0,   255,   0);
    m_ColorList[A_5250_WHITE            ] = RGB(255, 255, 255);
    m_ColorList[A_5250_RED              ] = RGB(255,   0,   0);
    m_ColorList[A_5250_TURQ             ] = RGB(0,   128, 128);
    m_ColorList[A_5250_YELLOW           ] = RGB(255, 255,   0);
    m_ColorList[A_5250_PINK             ] = RGB(255,   0, 255);
    m_ColorList[A_5250_BLUE             ] = RGB(0,   255, 255);
    m_ColorList[A_5250_BLACK            ] = RGB(0,     0,   0);
    m_ColorList[A_5250_RULER_COLOR      ] = RGB(192,   0,   0);
    //  m_ColorList[A_5250_STATUS_COLOR ] = RGB(192, 128, 128);
    m_ColorList[A_5250_STATUS_COLOR     ] = RGB(128, 255, 128);
    m_clrForeColor = m_ColorList[A_5250_WHITE];
    m_clrBackColor = m_ColorList[A_5250_BLACK];

    // Initialize keys arrays
    for (size_t i = 0; i < key2msg_size; ++i)
        m_Key2Msg.insert(key2msg[i], key2msg[i].tn5250_key);

    // Dont support windowless usage 'cause of caret support... (entr'autre)
    m_bWindowOnly = true;

    // Set the initial size
    SIZEL size = {648, 384};
    ATL::AtlPixelToHiMetric( &size, &m_sizeExtent );
    // Make sure the natural extent is the correct size too
    m_sizeNatural = m_sizeExtent;
    m_bResizeNatural = true;
    m_bRecomposeOnResize = true;
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
    // On devrait l'effacer je crois
    if (tn5250_config_load_default(config) == -1)
        return E_FAIL;

    // Setup MemDC
    m_hMemDC = CreateCompatibleDC(NULL);
    SetBkMode(m_hMemDC, OPAQUE);
    SetTextAlign(m_hMemDC, TA_TOP|TA_LEFT|TA_NOUPDATECP);

    m_hBackgroundBrush = CreateSolidBrush(GetColor(A_5250_BLACK));
    return S_OK;
}

void CTerm5250::FinalRelease() throw()
{
    FUNC_ENTER();

    // Terminate the thread if still connected
    Disconnect();

    // Wait for the other thread to quit.
    WaitForSingleObject(m_hThread, INFINITE);

    // TBA removes resource leak...
    if ( m_hMemDC )
        DeleteDC(m_hMemDC);
    if ( m_hFont )
        DeleteObject(m_hFont);
    if ( m_hCaretBMP )
        DeleteObject(m_hCaretBMP);
    if ( m_hScreenBMP )
        DeleteObject(m_hScreenBMP);
    if ( m_hBackgroundBrush )
        DeleteObject(m_hBackgroundBrush);
    //if (pd != NULL)
    //	win32_destroy_printer_info(This);

    if (config != NULL)
    {
        tn5250_config_unref(config);
        config = NULL;
    }
}


// We do all drawing to the screen buffer, then BitBlt that to the actual window when WM_PAINT occurs
HRESULT CTerm5250::OnDraw(ATL_DRAWINFO & di)
{
    FUNC_ENTER1(" %d", m_bUIActive);
    const int Delta = GetDelta();
    // && !m_nAppearance
    if ( Delta )
    {
        HPEN MyPen = CreatePen(PS_SOLID, 0, m_clrBorderColor );
        HGDIOBJ oldpen = SelectObject(di.hdcDraw, MyPen);
        HGDIOBJ oldbrush = SelectObject(di.hdcDraw, GetStockObject(NULL_BRUSH));
        Rectangle(di.hdcDraw, 0, 0, GetSize().cx+2*Delta, GetSize().cy+2*Delta);
        SelectObject(di.hdcDraw, oldbrush);
        SelectObject(di.hdcDraw, oldpen);
        DeleteObject(MyPen);
    }

    KillTimer(0x31337);
    LONG BufferState = InterlockedExchange(&m_ScreenBufferState, Ok);
    if (BufferState == Regen)
        RegenScreenBuffer(true);
    else if (BufferState == Redraw)
        RefreshScreenBuffer(false);

    // Do the BitBlt!
    const int w = (di.prcBounds->right - di.prcBounds->left) + 1;
    const int h = (di.prcBounds->bottom - di.prcBounds->top) + 1;
    // Blit the buffer with displacing
    if ( !BitBlt(di.hdcDraw, di.prcBounds->left+Delta, di.prcBounds->top+Delta, w, h, 
        m_hMemDC, di.prcBounds->left, di.prcBounds->top, SRCCOPY))
    {
        // BitBlt may fail if the OnDraw is called too often during one screen refresh (~1/60 sec)
        const DWORD Err = GetLastError();
        // We must not care too much about that
        if ( Err )
            return HRESULT_FROM_WIN32(Err);
    }

    // We can't use m_bUIActive because it is not consistent
    if (m_bFocus)
    {
        // Update caret position
        MoveCaret(di.hdcDraw);

        // Display an ugly ruler...
        if (m_bDisplayRuler)
        {
            CPoint P = GetCaretPos();
            P.x = (P.x+1)*m_FontSize.cx+Delta-1;
            P.y = (P.y+1)*m_FontSize.cy+Delta-1;
            HPEN MyPen = CreatePen(PS_SOLID, 0, GetColor(A_5250_RULER_COLOR));
            HGDIOBJ savepen = SelectObject(di.hdcDraw, MyPen);
            const int savemode = SetROP2(di.hdcDraw, R2_XORPEN);
            MoveToEx(di.hdcDraw, P.x, di.prcBounds->top, NULL);
            LineTo  (di.hdcDraw, P.x, di.prcBounds->bottom);
            MoveToEx(di.hdcDraw, di.prcBounds->left , P.y, NULL);
            LineTo  (di.hdcDraw, di.prcBounds->right, P.y);
            SetROP2(di.hdcDraw, savemode);
            SelectObject(di.hdcDraw, savepen);
            DeleteObject(MyPen);
        }
    }

    // Draw a rectangle if there's a selection
    if (m_bSelected && m_bSelectionEnabled)
    {
        // No need to select a pen with R2_NOT
        const int savemode = SetROP2(di.hdcDraw, R2_NOT);
        HGDIOBJ hOldObj = SelectObject(di.hdcDraw, GetStockObject(m_bSelecting?NULL_BRUSH:WHITE_BRUSH) );
        Rectangle(di.hdcDraw, m_rctSelection.left, m_rctSelection.top, m_rctSelection.right, m_rctSelection.bottom);
        SetROP2(di.hdcDraw, savemode);
        SelectObject(di.hdcDraw, hOldObj);
    }
    return S_OK;
}


// Create/Resize the bitmap that we use as the screen buffer.
void CTerm5250::RegenScreenBuffer(bool Refresh)
{
    // Create a bitmap act as a screen buffer. We will draw all of
    // our data to this buffer, then BitBlt it to the screen when we
    // need to re-draw the screen. This'll make it easy for us to
    // re-paint sections of the screen when necessary
    FUNC_ENTER1(" %s", m_hWnd?"success":"failure");
    if ( !m_hWnd )
        return;

    if (m_hScreenBMP) 
        DeleteObject(m_hScreenBMP);
    HDC hdc = GetDC(); // Must not use CreateCompatibleDC otherwise it will be monochrome!
    const CSize Size = GetSize();
    m_hScreenBMP = CreateCompatibleBitmap(hdc, Size.cx, Size.cy);
    // Select it right now to save a bit of time when drawing
    SelectObject(m_hMemDC, m_hScreenBMP);
    ReleaseDC(hdc);

    // Don't infinite loop! Special case when the number of rows/cols change
    if ( Refresh )
        RefreshScreenBuffer(false);

    RegenCaret();
    m_Last_size.cx = GetCols();
    m_Last_size.cy = GetRows();
}

void CTerm5250::RegenCaret()
{
    FUNC_ENTER();
    DeleteObject(m_hCaretBMP);
    m_hCaretBMP = NULL;

    // Regen the caret bitmap
    switch ( m_eCaretStyle )
    {
    default:
    case CARETSTYLE_BLINK:
        // for the standard "blinking block", we just use the windows default
        // shape for the caret
        break;

    case CARETSTYLE_LINE:
        {
            // Here we create a small bitmap to use as the caret
            // we simply draw a line at the bottom of the bitmap
            const int bytewidth = (m_FontSize.cx + 15) / 16 * 2;
            const int size = bytewidth * m_FontSize.cy;
            ATL::CAutoPtr<BYTE> bits(new(std::nothrow) BYTE[size]);
            memset(bits, 0x00, size);
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
        }
        break;

    case CARETSTYLE_NOBLINK:
        {
            // We make the Windows Caret invisible, so we can maintain control
            // of the caret without the user seeing it blink
            const int bytewidth = (m_FontSize.cx + 15) / 16 * 2;
            const int size = bytewidth * m_FontSize.cy;
            ATL::CAutoPtr<BYTE> bits(new(std::nothrow) BYTE[size]);
            memset(bits, 0x00, size);
            m_hCaretBMP = CreateBitmap(m_FontSize.cx, m_FontSize.cy, 1, 1, bits);
        }
        break;
    }
    MakeCaret();
}


// Do all drawing to screen buffer (m_hMemDC)
void CTerm5250::RefreshScreenBuffer(bool FireViewEvent)
{
    FUNC_ENTER();
    // BitBlt to the actual window when WM_PAINT (OnDraw) occurs

    // clear the screen buffer (one big black rectangle, with a border)
    HPEN MyPen = CreatePen(PS_SOLID, 0, GetColor(A_5250_BLACK) );
    HGDIOBJ oldpen = SelectObject(m_hMemDC, MyPen);
    HGDIOBJ oldbrush = SelectObject(m_hMemDC, m_hBackgroundBrush);
    const CSize Size = GetSize();
    Rectangle(m_hMemDC, 0, 0, Size.cx, Size.cy);
    SelectObject(m_hMemDC, oldbrush);
    SelectObject(m_hMemDC, oldpen);
    DeleteObject(MyPen);

    // If resized, regen the screen buffer
    if ( !m_hScreenBMP || m_Last_size.cy != GetRows() || m_Last_size.cx != GetCols() )
        RegenScreenBuffer(false);

    if ( display )
    {
        // Do the painting stuff
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
    }

    // Draw the status line
    DrawStatusLine();

    // Tell the world to draw me!
    if ( FireViewEvent )
        FireViewChange();
}


void CTerm5250::DrawStatusLine()
{
    //FUNC_ENTER();
    const int inds = display?tn5250_display_indicators(display):0;

    // m_hScreenBMP and m_hFont are already selected

    char ind_buf[81];
    memset(ind_buf, ' ', sizeof(ind_buf));
    ind_buf[80] = 0;	// closes it
    // TBA Check if in 5250 or telnet mode.
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
        memcpy(ind_buf + 72-12, tn5250_macro_printstate(display), 11);
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
        ATL::CW2A szName(swName);
        int Length = strlen(szName);
        if ( Length > (72-37) )
            Length = (72-37);
        memcpy(ind_buf + 37, szName, Length);
    }
    const CPoint P = GetCaretPos();
    wsprintf(ind_buf+73, _T("%03.3d/%03.3d"), P.x+1, P.y+1);

    DrawTextAttrib(GetColor(A_5250_STATUS_COLOR), GetColor(A_5250_BLACK), ind_buf, strlen(ind_buf), 0, GetRows()-1);
    //DrawTextAttrib(0x22, ind_buf, strlen(ind_buf), 0, tn5250_display_height(display));
}


/// This draws text on the terminal in the specified attribute (in MemDC)
void CTerm5250::DrawTextAttrib(int attr, const char * text, int len
                               , int CaretX, int CaretY, Tn5250Win32Attribute *map) const
{
    /// \note Assumes 1 line string.
    /// \sample DrawTextAttrib(a, "Hello", 5, 12, 5);
    /// \params
    ///   int               attr         - 5250 attribute byte
    ///   const char *      text         - text to draw
    ///   int               len          - length of text
    ///   int               x            - position to start (along x axis)
    ///   int               y            - position to start (along y axis)
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

    DrawTextAttrib(fg, bg, text, len, CaretX, CaretY, flags, map);
}


void CTerm5250::DrawTextAttrib(OLE_COLOR fg, OLE_COLOR bg, const char * text, int len
                               , int CaretX, int CaretY, int flags, Tn5250Win32Attribute * /*map*/) const
{
    SetBkColor(m_hMemDC, bg);
    SetTextColor(m_hMemDC, fg);

    // create a rect to "opaque" our text.  (defines the background area
    // that the text is painted on)
    RECT rect;
    rect.left   = CaretX * m_FontSize.cx;
    rect.top    = CaretY * m_FontSize.cy;
    rect.right  = rect.left + (m_FontSize.cx * len);
    rect.bottom = rect.top + m_FontSize.cy;

    // draw the text
    if ( !ExtTextOut(m_hMemDC, rect.left, rect.top, ETO_CLIPPED|ETO_OPAQUE, &rect, text, len, m_pSpacing) )
        return;	// we can't do anything about it.

    if ( flags&A_UNDERLINE || (flags&A_VERTICAL && m_ColSepStyle!=COLSEPSTYLE_NONE) )
    {
        // TBA If COLSEPSTYLE_DOTS, use PS_DASH
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

// Connect Part
STDMETHODIMP CTerm5250::Connect()
{
    FUNC_ENTER();
    Disconnect();
    char szText[64];

    if ( !m_HostName.m_str || !m_HostName.Length() )
        return E_INVALIDARG;

    // Saves settings
    tn5250_config_set(config, "enhanced", m_bEnhanced?"true":"false");
    {
        ATL::CW2A szHost(m_HostName);
        tn5250_config_set(config, "host", szHost);
    }

    // Set the device name correctly
    DWORD Length = lengthof(szText);
    if (m_bUseComputerName && GetComputerNameA(szText, &Length) && Length)
    {
        tn5250_config_set(config, "env.DEVNAME", szText);
    }
    else
    {
        if ( m_Device.Length() > 0 )
        {
            ATL::CW2A szDev(m_Device);
            tn5250_config_set(config, "env.DEVNAME", szDev);
        }
        else
            tn5250_config_unset(config, "env.DEVNAME");
    }
    ASSERT(m_CodePage);
    sprintf(szText, "%d", m_CodePage);
    tn5250_config_set(config, "map", szText);
    TCHAR * const TermType = m_bWide ? "IBM-3477-FC" : "IBM-3179-2";
    tn5250_config_set(config, "env.TERM", TermType);

    // set up display terminal
    display = tn5250_display_new();
    display->sign_key_hack = 0;
    if (tn5250_display_config(display, config) == -1)
        return E_FAIL;
    macro = tn5250_macro_init();
    tn5250_macro_attach(display, macro);
    tn5250_display_set_terminal(display, &term);

    // create connection to host
    {
        ATL::CW2A szBuffer( m_HostName );
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
    //if (WSAAsyncSelect(term.conn_fd, m_hWnd, WM_TN5250_STREAM_DATA, FD_READ) == SOCKET_ERROR)
    //	return HRESULT_FROM_WIN32(GetLastError());

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

// Show a dialog box to connect to the server.
STDMETHODIMP CTerm5250::ConnectDialog()
{
    FUNC_ENTER();
    CDlgConnect Dlg;
    Dlg.m_IP = m_HostName;
    Dlg.m_Device = m_Device;
    Dlg.m_CodePage = m_CodePage;
    Dlg.m_Wide = m_bWide!=0;
    if ( IDOK == Dlg.DoModal() )
    {
        ATL::CComBSTR szTxt(Dlg.m_IP);
        HRESULT hr = put_HostName(szTxt);
        if (FAILED(hr))
            return hr;
        szTxt = Dlg.m_Device;
        hr = put_DeviceName(szTxt);
        if (FAILED(hr))
            return hr;
        hr = put_CodePage(Dlg.m_CodePage);
        if (FAILED(hr))
            return hr;
        hr = put_Wide(Dlg.m_Wide);
        if (FAILED(hr))
            return hr;
        return Connect();
    }
    return S_FALSE;
}

// Flush all the tn5250-related stuff to do a "clean restart".
void CTerm5250::InternalFlush()
{
    FUNC_ENTER();
    m_ThreadId = 0;
    m_hThread.Close();

    if (display != NULL)
    {
        tn5250_display_destroy(display);
        display = NULL;
    }
    if (macro != NULL)
    {
        tn5250_macro_exit(macro);
        macro = NULL;
    }
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

    const LONG WasConnected = m_nReadyState;
    m_nReadyState = false;

    // Don't forget to close the socket
    if (term.conn_fd != -1)
        closesocket(term.conn_fd);

    if ( WasConnected )
    {
        // Anyway, we must change it.
        if ( CanEdit(DISPID_READYSTATE) )
            ValueChanged(DISPID_READYSTATE);
        Fire_Disconnected();
    }

    PostBufferRefresh(true);
}


LRESULT CTerm5250::OnEraseBkgnd(UINT, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    // Erase the background only one time
    if ( !m_bFirstClear )
    {
        m_bFirstClear = true;
        return 0;
    }
    return 1;
}

// Used to postpone screen drawing
LRESULT CTerm5250::OnTimer(UINT, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
    if (wParam == 0x31337)
        FireViewChange();
    else
        bHandled = false;
    return 0;
}

LRESULT CTerm5250::OnKillFocus(UINT, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    bHandled = false;
    m_bFocus = false;
    HideCaret();
    return 0;
}

LRESULT CTerm5250::OnSetFocus(UINT, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    bHandled = false;
    m_bFocus = true;
    MakeCaret();
    return 0;
}

LRESULT CTerm5250::OnMouseActivate(UINT, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    bHandled = false;
    m_bFocus = true;
    // Ask to the container to reactivate us. It is a bug in our own software I think...
    SetControlFocus(TRUE);
    MakeCaret();
    return 0;
}
// Controls which message to giveup to windows.
BOOL CTerm5250::PreTranslateAccelerator(LPMSG pMsg, HRESULT & hRet) throw()
{
    FUNC_ENTER3("(%u, %u, %X)", pMsg->message, pMsg->wParam, pMsg->lParam);
    // Signal that we processed the key
    hRet = S_OK;

    // Since TranslateMessage won't be automatically called, we must do your processing here.
    const UINT uMsg = pMsg->message;
    const WPARAM wParam = pMsg->wParam;
    const LPARAM lParam = pMsg->lParam;
    BOOL bHandled = TRUE;

    // Same as message dispatcher
    switch (uMsg)
    {
    case WM_SYSKEYUP:
    case WM_KEYUP:
        OnKeyUp(uMsg, wParam, lParam, bHandled);
        break;
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
        OnKeyDown(uMsg, wParam, lParam, bHandled);
        break;
    case WM_CHAR:
        OnChar(uMsg, wParam, lParam, bHandled);
        break;
    default:
        FUNC_ENTER0("Unknown message!");
        bHandled = FALSE;
        break;
    }

    // So WM_CHAR are sent
    if (!bHandled)
        TranslateMessage(pMsg);

    return TRUE;
}

LRESULT CTerm5250::OnKeyUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#if _DEBUG
    if (FixVKey(wParam, lParam) == (int)wParam)
        FUNC_ENTER2(" %2d %s", wParam, m_VKCache.GetName(wParam));
    else
        FUNC_ENTER4(" %2d %s -> %2d %s", wParam, m_VKCache.GetName(wParam), FixVKey(wParam, lParam), m_VKCache.GetName(FixVKey(wParam, lParam)));
#endif
    bHandled = false;
    if ( !m_bEnabled || !m_nReadyState )
        return 0;

    const int VKey = FixVKey(wParam, lParam);
    if (IsDeadKey(VKey))
    {
        VKey2Msg::const_iterator itr = m_Key2Msg.find< similar<Keystroke2MsgEx> >(VKey);
        if (itr != m_Key2Msg.end())
        {
            QueueKey(itr->second);
            //QueueKey(itr->m_value);
            bHandled = true;
        }
    }
    m_bHandledVirtualKey = false;
    return 0;
}

LRESULT CTerm5250::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#if _DEBUG
    if (FixVKey(wParam, lParam) == (int)wParam)
        FUNC_ENTER2(" %2d %s", wParam, m_VKCache.GetName(wParam));
    else
        FUNC_ENTER4(" %2d %s -> %2d %s", wParam, m_VKCache.GetName(wParam), FixVKey(wParam, lParam), m_VKCache.GetName(FixVKey(wParam, lParam)));
#endif
    bHandled = false;
    if ( !m_bEnabled || !m_nReadyState )
        return 0;

    const int VKey = FixVKey(wParam, lParam);
    if (!IsDeadKey(VKey))
    {
        m_bHandledVirtualKey = false;
        VKey2Msg::const_iterator itr = m_Key2Msg.find< similar<Keystroke2MsgEx> >(VKey);
        if (itr != m_Key2Msg.end())
        {
            for (int repeat = LOWORD(lParam); repeat>0; --repeat)
            {
                QueueKey(itr->second);
                //QueueKey(itr->m_value);
            }
            TN5250_LOG(("WM_KEYDOWN: handling key\n"));
            m_bHandledVirtualKey = true;
            bHandled = true;
        }
    }
    return 0;
}

LRESULT CTerm5250::OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    FUNC_ENTER2(" %d %c", wParam, wParam);
    if ( !m_bEnabled || !m_nReadyState )
        return 0;

    // Assuming that WM_KEYUP is sent after.
    if (m_bHandledVirtualKey)
    {
        FUNC_ENTER0(" skipping");
        return 0;
    }

    SendChar(LOWORD(wParam));
    return 0;
}

LRESULT CTerm5250::OnRButtonDown(UINT, WPARAM, LPARAM lParam, BOOL&)
{
    FUNC_ENTER();
    if ( m_bContextualMenu )
    {
        // Contextual menu
        HMENU hMenu = CreatePopupMenu();
        AppendMenu(hMenu, MF_STRING, ItemSendResetSignal, _T("Send &reset signal"));
        AppendMenu(hMenu, MF_STRING, ItemResetConnection, _T("R&eset connection"));
        if ( m_bShowConnect )
        {
            AppendMenu(hMenu, MF_STRING, ItemConnect, _T("&Connect..."));
            AppendMenu(hMenu, MF_STRING, ItemDisconnect, _T("&Disconnect"));
        }
        AppendMenu(hMenu, MF_SEPARATOR, 0, 0);
        AppendMenu(hMenu, m_bSelectionEnabled?MF_CHECKED|MF_STRING:MF_STRING,
            ItemSelectable, _T("&Selectable"));
        AppendMenu(hMenu, m_bMouseMoveCursor?MF_CHECKED|MF_STRING:MF_STRING,
            ItemMouseMove, _T("&Mouse move"));
        AppendMenu(hMenu, m_bHotSpots?MF_CHECKED|MF_STRING:MF_STRING,
            ItemHotSpot, _T("&Hot spot"));
        AppendMenu(hMenu, m_bDisplayRuler?MF_CHECKED|MF_STRING:MF_STRING,
            ItemDisplayRuler, _T("&Display ruler"));
        AppendMenu(hMenu, MF_STRING,
            ItemSetFont, _T("&Select font..."));

        HMENU hMenuCaret = CreatePopupMenu();
        AppendMenu(hMenuCaret, m_eCaretStyle==CARETSTYLE_BLINK?MF_CHECKED|MF_STRING:MF_STRING,
            ItemCaretBlink, _T("&Blink"));
        AppendMenu(hMenuCaret, m_eCaretStyle==CARETSTYLE_LINE?MF_CHECKED|MF_STRING:MF_STRING,
            ItemCaretLine, _T("&Line"));
        AppendMenu(hMenuCaret, m_eCaretStyle==CARETSTYLE_NOBLINK?MF_CHECKED|MF_STRING:MF_STRING,
            ItemCaretNoBlink, _T("&Block"));

        AppendMenu(hMenu, MF_POPUP|MF_STRING, (UINT_PTR)hMenuCaret, _T("&Caret"));
        AppendMenu(hMenu, MF_SEPARATOR, 0, 0);
        // TBA AppendMenu(hMenu, MF_STRING, ItemSetColors, _T("Set c&olors..."));
        AppendMenu(hMenu, MF_STRING, ItemHelp, _T("Help..."));
        AppendMenu(hMenu, MF_STRING, ItemAbout, _T("About..."));

        CPoint point(lParam);
        ClientToScreen(&point);
        if ( !TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, *this, NULL) )
            DestroyMenu(hMenu);
    }
    else if (m_bUnixLikeCopy)
    {
        PasteTextSelection();
    }
    
    return 0;
}

char CTerm5250::GetChar(int x, int y) const
{
    if (!display || x < 0 || x >= tn5250_display_width(display) || y < 0 || y >= tn5250_display_height(display))
        return 0;

    return tn5250_char_map_to_local(tn5250_display_char_map(display), tn5250_display_char_at(display, y, x));
}

LRESULT CTerm5250::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    if ( !m_bEnabled )
        return 0;

    FUNC_ENTER();

    // Resets selection.
    m_bSelecting = false;
    m_bSelected = false;
    if (display && (m_bHotSpots || m_bMouseMoveCursor) && m_nReadyState)
    {
        const POINT P = { LOWORD(lParam), HIWORD(lParam) };
        CPoint CaretPos;
        PointToCursorPos(P, CaretPos);

        // If the user clicks on the status line, this must be skipped.
        if (CaretPos.y < tn5250_display_height(display) && CaretPos.x < tn5250_display_width(display)
            && CaretPos.x >= 0 && CaretPos.y >= 0)
        {
            if ( tn5250_display_field_at(display, CaretPos.y, CaretPos.x) )
            {
                // If a field exist there, try to move the cursor there
                if ( m_bMouseMoveCursor )
                {
                    // TBA We don't want to move the mouse on activation click...
                    // ASSERT( m_bUIActive );
                    // We can't know because we receive MOUSEACTIVATE before LBUTTONDOWN
                    tn5250_display_set_cursor(display, CaretPos.y, CaretPos.x);
                    //tn5250_display_update(display);
                    FireViewChange();
                }
            }
            else
            {
                // If there is NO field there, look for a F-key
                if ( m_bHotSpots )
                {
                    // Look for a "F1=" to "F24=" under the spot which was hit
                    char Buffer[8];
                    Buffer[0] = GetChar(CaretPos.x-3, CaretPos.y);
                    Buffer[1] = GetChar(CaretPos.x-2, CaretPos.y);
                    Buffer[2] = GetChar(CaretPos.x-1, CaretPos.y);
                    Buffer[3] = GetChar(CaretPos.x  , CaretPos.y);
                    Buffer[4] = GetChar(CaretPos.x+1, CaretPos.y);
                    Buffer[5] = GetChar(CaretPos.x+2, CaretPos.y);
                    Buffer[6] = GetChar(CaretPos.x+3, CaretPos.y);
                    Buffer[7] = GetChar(CaretPos.x+4, CaretPos.y);

                    /// Scan for F
                    for (int i = 0; i < 5; ++i)
                    {
                        /// \note assuming that K_F2 = K_F1+1 and so on; which is true :)
                        if (Buffer[i] == 'F')
                        {
                            const int N1 = Buffer[i+1]-'0';
                            const int N2 = Buffer[i+2]-'0';

                            // Check for F10-F24
                            if ((Buffer[i+3] == '=')
                                && (N1 == 1 || N1 == 2) && (N2 >= 0 && N2 <= 9) )
                            {
                                const int Val = (N1 * 10) + N2;
                                if (Val >= 10 && Val <= 24)
                                {
                                    // Must be F10-F24
                                    QueueKey(K_F1 - 1 + Val);
                                    return 0;
                                }
                            }

                            // Check for F1-F9
                            if (i > 0 && N1 >= 1 && N1 <= 9 && Buffer[i+2] == '=')
                            {
                                QueueKey(K_F1 - 1 + N1);
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }

    if ( m_bSelectionEnabled )
    {
        m_bSelecting = true;
        m_bSelected = true;
        m_rctSelection.left = (short)LOWORD(lParam);
        m_rctSelection.top = (short)HIWORD(lParam);
        m_rctSelection.right = m_rctSelection.left;
        m_rctSelection.bottom = m_rctSelection.top;

        if ( S_OK != m_spInPlaceSite->SetCapture(TRUE) )
        {
            if ( m_hWndCD )
                SetCapture();
        }

        SetCursor(LoadCursor(NULL, IDC_CROSS));
        FireViewChange();	// No need to refresh the screen buffer
    }

    return 0;
}

LRESULT CTerm5250::OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    if ( !m_bEnabled || !m_nReadyState )
        return 0;
    FUNC_ENTER();
    QueueKey(K_RESET);
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
        if ( S_OK != m_spInPlaceSite->SetCapture(FALSE) )
        {
            ReleaseCapture();
        }
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


// Menu Items
LRESULT CTerm5250::OnSendResetSignal(WORD /*wCode*/, WORD /*wId*/, HWND /*hCtl*/, BOOL & /*bHandled*/)
{
    if ( m_nReadyState )
        QueueKey(K_RESET);
    return 0;
}

LRESULT CTerm5250::OnResetConnection(WORD /*wCode*/, WORD /*wId*/, HWND /*hCtl*/, BOOL & /*bHandled*/)
{
    Connect();
    return 0;
}

LRESULT CTerm5250::OnToggleSelectable(WORD /*wCode*/, WORD /*wId*/, HWND /*hCtl*/, BOOL & /*bHandled*/)
{
    put_SelectionEnabled(!m_bSelectionEnabled);
    return 0;
}

LRESULT CTerm5250::OnMouseMove(WORD /*wCode*/, WORD /*wId*/, HWND /*hCtl*/, BOOL & /*bHandled*/)
{
    put_MouseMoveCursor(!m_bMouseMoveCursor);
    return 0;
}

LRESULT CTerm5250::OnHotSpot(WORD /*wCode*/, WORD /*wId*/, HWND /*hCtl*/, BOOL & /*bHandled*/)
{
    put_HotSpot(!m_bHotSpots);
    return 0;
}

LRESULT CTerm5250::OnItemDisplayRuler(WORD /*wCode*/, WORD /*wId*/, HWND /*hCtl*/, BOOL & /*bHandled*/)
{
    put_DisplayRuler(!m_bDisplayRuler);
    return 0;
}

LRESULT CTerm5250::OnSetColors(WORD /*wCode*/, WORD /*wId*/, HWND /*hCtl*/, BOOL & /*bHandled*/)
{
    // Show a dialog box to choose the 8 colors.
    MessageBox(_T(" TBA Needs to create CDlgColors."));
    /*
    CDlgColors Dlg;
    Dlg.m_Colors = m_ColorList;
    if ( IDOK == Dlg.DoModal() )
    {
        Dlg.m_Colors = m_ColorList;
        PostBufferRefresh(false);
    }
    */
    return 0;
}

LRESULT CTerm5250::OnItemSetFont(WORD /*wCode*/, WORD /*wId*/, HWND /*hCtl*/, BOOL & /*bHandled*/)
{
    // TBA Set the name of the current font in the dialog box?
    LOGFONT lf = { -10 };
    strcpy(lf.lfFaceName, m_FontName);
    lf.lfWeight = FW_REGULAR;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfPitchAndFamily = FIXED_PITCH|FF_MODERN;
    CHOOSEFONT cf = { sizeof(cf) };
    cf.hwndOwner = m_hWnd;
    cf.lpLogFont = &lf;
    cf.iPointSize = 100;
    cf.Flags = CF_FIXEDPITCHONLY|CF_NOVERTFONTS|CF_SCREENFONTS;
    if ( ChooseFont(&cf) )
    {
        m_FontName = lf.lfFaceName;
        m_bFontChanged = 1;
        GrabMaxSpace();
    }
    return 0;
}

LRESULT CTerm5250::OnConnect(WORD /*wCode*/, WORD /*wId*/, HWND /*hCtl*/, BOOL & /*bHandled*/)
{
    ConnectDialog();
    return 0;
}

LRESULT CTerm5250::OnDisconnect(WORD /*wCode*/, WORD /*wId*/, HWND /*hCtl*/, BOOL & /*bHandled*/)
{
    Disconnect();
    return 0;
}

LRESULT CTerm5250::OnAbout(WORD /*wCode*/, WORD /*wId*/, HWND /*hCtl*/, BOOL & /*bHandled*/)
{
    ATL::CString szAbout(g_szAbout);
    szAbout += _T("\nBuilt at ");
    szAbout += k_BuildDate;
    MessageBox(szAbout, _T("About Terminal 5250 ..."), MB_ICONINFORMATION);
    return 0;
}

LRESULT CTerm5250::OnItemCaretBlink(WORD /*wCode*/, WORD /*wId*/, HWND /*hCtl*/, BOOL & /*bHandled*/)
{
    put_CaretStyle(CARETSTYLE_BLINK);
    return 0;
}

LRESULT CTerm5250::OnItemCaretLine(WORD /*wCode*/, WORD /*wId*/, HWND /*hCtl*/, BOOL & /*bHandled*/)
{
    put_CaretStyle(CARETSTYLE_LINE);
    return 0;
}

LRESULT CTerm5250::OnItemCaretNoBlink(WORD /*wCode*/, WORD /*wId*/, HWND /*hCtl*/, BOOL & /*bHandled*/)
{
    put_CaretStyle(CARETSTYLE_NOBLINK);
    return 0;
}

LRESULT CTerm5250::OnItemHelp(WORD /*wCode*/, WORD /*wId*/, HWND /*hCtl*/, BOOL & /*bHandled*/)
{
    MessageBox( _T("Double-click to send a reset signal.\n")
                _T("With Mouse Move activated, use the mouse to move the cursor.\n")
                _T("With Hot Spot activated, click on a F-key to send the key to the server.\n")
                _T("Use Ctrl-C Ctrl-V to copy/paste text.\n")
#if HAVE_LIBSSL
                _T("You can connect to a SSL server by adding ssl:// in front of the server name.")
#endif
                , _T("Help on Terminal 5250 ..."), MB_ICONINFORMATION);
    return 0;
}

// Forward the message to the worker thread. Not efficient but it works.
LRESULT CTerm5250::ForwardMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    //FUNC_ENTER();
    PostThreadMessage(m_ThreadId, uMsg, wParam, lParam);
    return 0;
}


// Should be called in the worker thread
void CTerm5250::PostBufferRefresh(bool Regen) throw()
{
    // Set a 10 ms timer to redraw, otherwise when queuing keys, it refreshes
    // uselessly too often.
    if (m_hWnd)
    {
        if (Regen)
            InterlockedExchange(&m_ScreenBufferState, Redraw);
        else // Do not modify if already a regen
            InterlockedCompareExchange(&m_ScreenBufferState, Redraw, Ok);
        SetTimer(0x31337, 10, NULL);
    }
}

// Simulates the SetControlSize of MFC's COleControl and Updates the values to count the border size.
void CTerm5250::SetSize(int cx, int cy)
{
    // Taken from http://www.widgetware.com/ATLFAQ_Controls.htm#Sizing
    //AdjustWindowRectEx(&Rect, Style, FALSE, Ex);
    //	GetSystemMetrics(SM_CXBORDER);SM_CXEDGE
    //	GetSystemMetrics(SM_CYBORDER);SM_CYEDGE

    /*
    CRect rctW;
    GetWindowRect(&rctW);
    CRect rctC;
    GetClientRect(&rctC);
    ASSERT(rctW.Height()==rctC.Height() && rctW.Width()==rctC.Width());
    */
    FUNC_ENTER2("(%d,%d)", cx, cy);

    // Check for size change
    if ( cx == GetSize().cx && cy == GetSize().cy )
        return;
    SIZEL szlPixels = { cx, cy };
    SIZEL szlMetric;
    ATL::AtlPixelToHiMetric(&szlPixels, &szlMetric);
    SetSizeL(szlMetric);
}

void CTerm5250::SetSizeL(SIZEL & szlMetric)
{
    FUNC_ENTER();
    // IOleObjectImpl
    SetExtent(DVASPECT_CONTENT, &szlMetric);
    SIZEL szlPixels;
    ATL::AtlHiMetricToPixel(&m_sizeExtent, &szlPixels);

    // update control sizing...
    m_rcPos.right = m_rcPos.left + szlPixels.cx;
    m_rcPos.bottom = m_rcPos.top + szlPixels.cy;
    if (m_spInPlaceSite != NULL)
    {
        // needed for IE to accept the resizing
        m_spInPlaceSite->OnPosRectChange(&m_rcPos);
    }
}

// Use all available size.
void CTerm5250::GrabMaxSpace()
{
    // Skip SetExtent
    // SetSizeL(m_sizeNatural);
    SIZEL szlPixels;
    ATL::AtlHiMetricToPixel(&m_sizeNatural, &szlPixels);
    FUNC_ENTER2(" %dx%d", szlPixels.cx, szlPixels.cy);
    // update control sizing...
    m_rcPos.right = m_rcPos.left + szlPixels.cx;
    m_rcPos.bottom = m_rcPos.top + szlPixels.cy;
    if (m_spInPlaceSite != NULL)
    {
        // needed for IE to accept the resizing
        m_spInPlaceSite->OnPosRectChange(&m_rcPos);
    }
    FUNC_ENTER0(" done");
}

// Almost same as CComControlBase::IOleObject_SetExtent
HRESULT CTerm5250::IOleObject_SetExtent(DWORD dwDrawAspect, SIZEL * psizel)
{
    if (dwDrawAspect != DVASPECT_CONTENT)
    {
        FUNC_ENTER0("Bad aspect");
        return DV_E_DVASPECT;
    }
    if (psizel == NULL)
        return E_POINTER;
    ASSERT(!m_bAutoSize && m_bResizeNatural && m_bRecomposeOnResize);
    if (memcmp(psizel, &m_sizeExtent, sizeof(SIZE)) == 0)
    {
        //FUNC_ENTER1(" kept size %d", m_bFontChanged);
        m_bFontChanged = 0;
        return S_OK;
    }

    // If resizing
    SIZE NewSize;
    ATL::AtlHiMetricToPixel(psizel, &NewSize);
    FUNC_ENTER3(" %dx%d %d", NewSize.cx, NewSize.cy, m_bFontChanged);
    if ( !m_bFontChanged )
        m_sizeNatural = *psizel;
    else
        --m_bFontChanged;
    POINT P1, P2;
    PointToCursorPos(m_rctSelection.TopLeft(), P1);
    PointToCursorPos(m_rctSelection.BottomRight(), P2);
    CSize OldSize = m_sizeExtent;
    const bool Fit = LoadTerminalFont(NewSize);
    if ( !Fit )
    {
        // Will need a 2 steps resize because it don't fit well.
        ATL::AtlPixelToHiMetric(&NewSize, &m_sizeExtent);
    }

    bool bRegen = false;

    // If the font size has changed (and screen size)
    if ( OldSize != m_sizeExtent || m_bFontChanged )
    {
        // Fix the selection
        CursorPosToPoint(P1, m_rctSelection.TopLeft());
        CursorPosToPoint(P2, m_rctSelection.BottomRight());
        // The screen buffer size has changed
        bRegen = true;
    }
    if ( m_bRecomposeOnResize )
    {
        SendOnDataChange();
        bRegen = true;
    }

    if (bRegen)
    {
        InterlockedExchange(&m_ScreenBufferState, Regen);
        FireViewChange();
    }
    return Fit?S_OK:E_FAIL;
}


HRESULT CTerm5250::IOleInPlaceObject_UIDeactivate()
{
    FUNC_ENTER();
    m_bFocus = false;
    HideCaret();
    return super::IOleInPlaceObject_UIDeactivate();
}


HRESULT CTerm5250::OnPostVerbUIActivate()
{
    FUNC_ENTER();
    m_bFocus = true;
    if ( !m_hScreenBMP )
        PostBufferRefresh(true);
    MakeCaret();
    return S_OK;
}

STDMETHODIMP CTerm5250::SendVKey(EVirtualKeyCodes VKey, long Modifiers)
{
    FUNC_ENTER3("(%d, %d) %s", VKey, Modifiers, m_VKCache.GetName(VKey));
    if ( m_nReadyState )
    {
        VKey2Msg::const_iterator itr = m_Key2Msg.find< similar<Keystroke2MsgEx> >( Keystroke2MsgEx(VKey, Modifiers));
        if (itr != m_Key2Msg.end())
        {
            QueueKey(itr->second);
            //QueueKey(itr->m_value);
            return S_OK;
        }
        else
        {
            // Virtual Key is not found. Try with characters.
            BYTE KeyState[256] = { 0 };
            if (Modifiers&M_LShift)
                KeyState[VK_LSHIFT] = 0x80;
            if (Modifiers&M_RShift)
                KeyState[VK_RSHIFT] = 0x80;
            if ((Modifiers&M_Shift)==M_Shift)
                KeyState[VK_SHIFT] = 0x80;

            if (Modifiers&M_LCtrl)
                KeyState[VK_LCONTROL] = 0x80;
            if (Modifiers&M_RCtrl)
                KeyState[VK_RCONTROL] = 0x80;
            if ((Modifiers&M_Ctrl)==M_Ctrl)
                KeyState[VK_CONTROL] = 0x80;

            if (Modifiers&M_LAlt)
                KeyState[VK_LMENU] = 0x80;
            if (Modifiers&M_RAlt)
                KeyState[VK_RMENU] = 0x80;
            if ((Modifiers&M_Alt)==M_Alt)
                KeyState[VK_MENU] = 0x80;

            WORD Char = 0;
            int retval = ToAscii(VKey, 0, KeyState, &Char, 0);
            if (retval == 1)
            {
                FUNC_ENTER2(" found char %d %c", Char, Char);
                return SendChar(Char);
            }
            else
            {
                FUNC_ENTER0(" couldn't send this");
                return E_INVALIDARG;
            }
        }
    }
    else
    {
        FUNC_ENTER0(" not connected");
        return E_ABORT;
    }
}


STDMETHODIMP CTerm5250::SendChar(USHORT Character)
{
    FUNC_ENTER2("(%d) %c", Character, Character);
    if (!m_nReadyState)
        return S_FALSE;

    QueueKey((int)Character);
    return S_OK;
}

STDMETHODIMP CTerm5250::BindKey(EVirtualKeyCodes VKey, long Modifiers, long KValue)
{
    FUNC_ENTER5("(%d, %d, %d) %s -> %s", VKey, Modifiers, KValue, m_VKCache.GetName(VKey), m_KCache.GetName(KValue));
    if (KValue == K_NONE)
    {
        // Delete the item if found
        m_Key2Msg.erase(Keystroke2MsgEx(VKey, Modifiers));
        return S_FALSE;
    }
    m_Key2Msg.set(Keystroke2MsgEx(VKey, Modifiers), KValue);
    return S_OK;
}

COLORREF CTerm5250::GetColor(int iColor) const throw()
{
    COLORREF ColorRef = RGB(0,255,0);	// Defaults to green.
    ASSERT( iColor >= 0 && iColor < A_5250_NB_COLORS );
    OleTranslateColor(m_ColorList[iColor], NULL, &ColorRef);
    return ColorRef;
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
            // PostThreadMessage(m_ThreadId, WM_QUIT, 0, 0);
            Disconnect();
        }
        return -1;
        /* TBA
    case K_PRINT:
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

// Retrieve the size of the terminal (control size less the border)
CSize CTerm5250::GetSize() const throw()
{
    CSize Size;
    ATL::AtlHiMetricToPixel(&m_sizeExtent, &Size);
    if ( m_bBorderVisible )
    {
        const int Delta = 2*m_nBorderWidth;
        Size.cx -= Delta;
        Size.cy -= Delta;
    }
    return Size;
}

// Calculate a new font size. Returns true if the size is "fit"
bool CTerm5250::CalcMaxFontSize(const SIZE & sz, int cols, int rows, SIZE & o_fontSize)
{
    const int Delta = 2*GetDelta();
    const int cli_width  = sz.cx - Delta;
    const int cli_height = sz.cy  - Delta;
    o_fontSize.cx = cli_width / cols;
    o_fontSize.cy = cli_height / rows;
    const bool Fit = cli_width==o_fontSize.cx*cols && cli_height==o_fontSize.cy*rows;
    FUNC_ENTER7(" %dx%d -> %dx%d = %dx%d %s", cli_width, cli_height, o_fontSize.cx, o_fontSize.cy, o_fontSize.cx*cols, o_fontSize.cy*rows, (Fit?"Fit":"Not fit") );
    return Fit;
}


// This takes the font name, height&width that you pass and finds the closest match on the system.
bool CTerm5250::LoadTerminalFont(SIZE & NewSize)
{
    const int cols = GetCols();
    const int rows = GetRows();
    CSize DesiredFontSize;
    // If not fit, we should resize now
    const bool Fit = CalcMaxFontSize(NewSize, cols, rows, DesiredFontSize);

    // Don't reload uselessly except when changing font name
    if ( DesiredFontSize != m_FontSize || m_bFontChanged )
    {
        CSize TryFontSize(DesiredFontSize);
        // Try higher because sometimes it doesn't get the right value otherwise
        TryFontSize.cx += 1;
        TryFontSize.cy += 1;
        int Axis = 0;
        for (;;)
        {
            // create a font using the size from our config data
            if ( m_hFont )
                DeleteObject(m_hFont);
            ASSERT( !m_FontName.IsEmpty() );
            m_hFont = CreateFont(TryFontSize.cy, TryFontSize.cx, 0, 0, FW_DONTCARE, 
                FALSE, FALSE, FALSE, CodePageToCharSet(m_CodePage), OUT_DEFAULT_PRECIS, 
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
            FUNC_ENTER5(" \"%s\" at %dx%d, got %dx%d", m_FontName.GetString(), TryFontSize.cx, TryFontSize.cy,  m_FontSize.cx, m_FontSize.cy);
            if (m_FontSize.cx <= DesiredFontSize.cx && m_FontSize.cy <= DesiredFontSize.cy)
                break;

            // illisible anyway
            if ( TryFontSize.cx < 5 || TryFontSize.cy < 5 )
                break;

            // Otherwise, loop until we got a font small enough.
            // Reducing the dimension that is the more offending.
            // Added an hack for "Terminal"
            if ( (Axis<-3) || (Axis<=3&&(m_FontSize.cy-DesiredFontSize.cy) > (m_FontSize.cx-DesiredFontSize.cx)) )
            {
                ++Axis;
                --TryFontSize.cy;
            }
            else
            {
                --Axis;
                --TryFontSize.cx;
            }
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
    }
    else
    {
        FUNC_ENTER3(" \"%s\" %dx%d fited", m_FontName.GetString(), m_FontSize.cx, m_FontSize.cy);
    }

    // Update the correct Size
    const int Delta = 2*GetDelta();
    NewSize.cx = m_FontSize.cx*cols + Delta;
    NewSize.cy = m_FontSize.cy*rows + Delta;
    return Fit;
}


// Add a key to the terminal's keyboard buffer
void CTerm5250::QueueKey(int key)
{
    FUNC_ENTER2("(%d) %s", key, m_KCache.GetName(key));
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
            PostBufferRefresh(false);
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

// Generate the caret to indicate where the keyboard inputs is.
void CTerm5250::MakeCaret()
{
    /* DESCRIPTION
    *    If you're wondering, "Caret" is the Windows term for
    *    the cursor used when typing at the keyboard.  In Windows
    *    terminology, cursor=mouse, caret=keyboard.
    *
    *    There is only one cursor in Windows, shared by all Windows apps.
    *    So, we create it when focus returns to us, and destroy it when
    *    we lose focus.  This is where it gets created.
    *****/
    //FUNC_ENTER();
    if ( !m_bFocus )    //if ( !m_bUIActive )
        return;

    HDC hdc = NULL;
    if ( m_eCaretStyle != CARETSTYLE_NOBLINK )
        ::CreateCaret(m_hWnd, m_hCaretBMP, m_FontSize.cx, m_FontSize.cy);
    else
        hdc = GetDC();
   
    MoveCaret(hdc);

    if ( m_eCaretStyle != CARETSTYLE_NOBLINK )
        ShowCaret();
    else
        ReleaseDC(hdc);
}


// Move the Windows caret to the right position on the screen.
void CTerm5250::MoveCaret(HDC hdc)
{
    //FUNC_ENTER();
    const int Delta = GetDelta();
    const CPoint posCaret = GetCaretPos();
    if ( m_eCaretStyle == CARETSTYLE_NOBLINK )
    {
        // Since the Windows caret is invisible, make our own box now
        HGDIOBJ savepen   = SelectObject(hdc, GetStockObject(WHITE_PEN));
        HGDIOBJ savebrush = SelectObject(hdc, GetStockObject(WHITE_BRUSH));
        const int savemode = SetROP2(hdc, R2_NOT);
        Rectangle(hdc, 
            (posCaret.x  )*m_FontSize.cx+Delta, (posCaret.y  )*m_FontSize.cy+Delta,
            (posCaret.x+1)*m_FontSize.cx+Delta, (posCaret.y+1)*m_FontSize.cy+Delta);
        SetROP2(hdc, savemode);
        SelectObject(hdc, savepen);
        SelectObject(hdc, savebrush);
    }
    else
    {
        // Move the windows caret
        SetCaretPos(posCaret.x*m_FontSize.cx+Delta, posCaret.y*m_FontSize.cy+Delta);
    }
}


// Hide the caret, usually done when the window loses focus
void CTerm5250::HideCaret()
{
    //FUNC_ENTER();
    if ( m_eCaretStyle != CARETSTYLE_NOBLINK )
        DestroyCaret();
}

// This converts the mouse selection points to a rectangle of selected text
void CTerm5250::ExpandTextSelection()
{
    FUNC_ENTER();
    ASSERT(m_FontSize.cx>0);

    // change the points so that selstr is the upper left corner 
    // and selend is the lower right corner.
    LTR(m_rctSelection.left, m_rctSelection.right);
    LTR(m_rctSelection.top, m_rctSelection.bottom);

    CPoint C;
    // move selection start position to nearest character
    PointToCursorPos(m_rctSelection.TopLeft(), C);
    CursorPosToPoint(C, m_rctSelection.TopLeft());
    
    // move selection end position to nearest character
    PointToCursorPos(m_rctSelection.BottomRight(), C);
    C.x++; C.y++;
    CursorPosToPoint(C, m_rctSelection.BottomRight());
}


// This retrieves the text in the selected area and copies it to the global Windows clipboard.
void CTerm5250::CopyTextSelection()
{
    FUNC_ENTER();
    ASSERT(m_FontSize.cx>0);

    CPoint C1;
    CPoint C2;
    PointToCursorPos(m_rctSelection.TopLeft(), C1);
    PointToCursorPos(m_rctSelection.BottomRight(), C2);

    // figure out the dimensions (in text chars) and make a global buffer
    LTR(C1.x, C2.x);
    LTR(C1.y, C2.y);
    if ( C1.x >= GetCols() )    C1.x = GetCols()-1;
    if ( C1.y >= GetRows() )    C1.y = GetRows()-1;
    if ( C2.x >= GetCols() )    C2.x = GetCols()-1;
    if ( C2.y >= GetRows() )    C2.y = GetRows()-1;
    const int bufsize = ((C2.x-C1.x)+1) * ((C2.y-C1.y)+1) + 1;
    HGLOBAL hBuf = GlobalAlloc(GHND|GMEM_SHARE, bufsize);
    ASSERT(hBuf!=NULL);

    // populate the global buffer with the text data, inserting CR/LF in between each line that was selected
    char * buf = (char*)GlobalLock(hBuf); 
    int bp = -1;
    for (int y = C1.y; y < C2.y; y++)
    {
        for (int x = C1.x; x < C2.x; x++)
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

        if (y != (C2.y-1))
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

    // create a bitmap version of the copy buffer as well... 
    // this allows image programs to paste the buffer as a bitmap
    const int cx = m_rctSelection.Width()+1;
    const int cy = m_rctSelection.Height()+1;
    HDC hdc = GetDC();
    HBITMAP hBm = CreateCompatibleBitmap(hdc, cx, cy);
    ReleaseDC(hdc);
    hdc = CreateCompatibleDC(NULL);
    SelectObject(hdc, hBm);
    BitBlt(hdc, 0, 0, cx, cy, m_hMemDC, m_rctSelection.left, m_rctSelection.top, SRCCOPY);
    DeleteDC(hdc);

    /* TBA add RTF support ?
    // Gen RTF format.
    // http://www.microsoft.com/downloads/details.aspx?FamilyID=e5b8ebc2-6ad6-49f0-8c90-e4f763e3f04f&DisplayLang=en
    // http://www.codeproject.com/clipboard/clipnutshell.asp
    //\ul - Enable Underline (scoped within a group) 
    // TBA Codepage
    CString rtf;
    rtf = _T("{\\rtf1\\ansi\\ansicpg1252\\deff0{\\fonttbl{\\f0\\fmodern\\fprq1 ");
    rtf += m_FontName;
    rtf += _T(";}}{\\colortbl");
    // Color table
    CString Colors
    Color.Format(_T("\\red%d\\green%d\\blue%d;"), GetRValue(m_clrBackColor), GetGValue(m_clrBackColor), GetBValue(m_clrBackColor));
    rtf += Color;
    for ( int i = 0; i < A_5250_NB_COLORS; ++i )
    {
        Color.Format(_T("\\red%d\\green%d\\blue%d;"), GetRValue(m_ColorList[i]), GetGValue(m_ColorList[i]), GetBValue(m_ColorList[i]));
        rtf += Color;
    }
    rtf += _T("\\f0");
    // \f1\cb1\cf2 This is colored text. The background is color 1 and the foreground is color 2.}
    rtf += _T("Text}");
    int length = rtf.GetLength()+1;
    HGLOBAL hBufRTF = GlobalAlloc(GHND|GMEM_SHARE, length);
    ASSERT(hBufRTF!=NULL);
    memcpy(GlobalLock(hBufRTF), rtf, length);
    GlobalUnlock(hBufRTF);
    const int iRTF = RegisterClipboardFormat(_T("Rich Text Format"));
*/

    // Finally, copy both the global buffer & the bitmap to the clipboard.
    // After this, we should not try to use (or free!)the buffer/bitmap...
    // They're Windows' property now! 
    OpenClipboard();
    EmptyClipboard();

    // TBM Currently useless
    switch (m_CopyMode)
    {
    case Both:
        SetClipboardData(CF_TEXT, hBuf);
        // TBA SetClipboardData(iRTF, hBufRTF);
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


void CTerm5250::PointToCursorPos(const POINT & in_Pixels, POINT & out_CaretPos) const throw()
{
    // move selection start position to nearest character
    const int Delta = GetDelta();
    out_CaretPos.x = (in_Pixels.x-Delta) / m_FontSize.cx;
    if ( out_CaretPos.x > GetCols() )
        out_CaretPos.x = GetCols();
    out_CaretPos.y = (in_Pixels.y-Delta) / m_FontSize.cy;
    if ( out_CaretPos.y > GetRows() )
        out_CaretPos.y = GetRows();
}

void CTerm5250::CursorPosToPoint(const POINT & in_CaretPos, POINT & out_Pixels) const throw()
{
    // move selection start position to nearest character
    const int Delta = GetDelta();
    out_Pixels.x = in_CaretPos.x*m_FontSize.cx + Delta;
    out_Pixels.y = in_CaretPos.y*m_FontSize.cy + Delta;
}


CPoint CTerm5250::GetCaretPos() const throw()
{
    if ( display )
    {
        ASSERT(tn5250_display_cursor_x(display)>=0 && tn5250_display_cursor_x(display) < tn5250_display_width(display)
            && tn5250_display_cursor_y(display)>=0 && tn5250_display_cursor_y(display) < tn5250_display_height(display) );
        return CPoint(tn5250_display_cursor_x(display), tn5250_display_cursor_y(display));
    }
    else
    {
        return CPoint(0,0);
    }
}


int CTerm5250::GetCols() const throw()
{
    if ( !display )
        return 80;
    int cols = tn5250_display_width(display);
    ASSERT(cols);
    if (!cols)
        return 80;
    return cols;
}

int CTerm5250::GetRows() const throw()
{
    if ( !display )
        return 25;
    int rows = tn5250_display_height(display)+1;
    ASSERT(rows);
    if (!rows)
        return 25;
    return rows;
}

// Convert data in the windows clipboard into keystrokes and paste them into the keyboard buffer.
void CTerm5250::PasteTextSelection()
{
    // \note Increasing the MAX_K_BUF_LEN will speed this routine up...
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
            pNewBuf = new(std::nothrow) char[size+1];
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

void CTerm5250::OnBackColorChanged()
{
    //	FUNC_ENTER();
    m_ColorList[A_5250_BLACK] = m_clrBackColor;

    ASSERT( m_hBackgroundBrush );
    DeleteObject(m_hBackgroundBrush);
    m_hBackgroundBrush = CreateSolidBrush(GetColor(A_5250_BLACK));
    PostBufferRefresh(false);
}

void CTerm5250::OnBorderColorChanged()
{
    FUNC_ENTER();
    PostBufferRefresh(false);
}

// It affects the size of the object
void CTerm5250::OnBorderVisibleChanged()
{
    FUNC_ENTER1(" %s", m_bBorderVisible?_T("TRUE"):_T("FALSE"));
    GrabMaxSpace();
}

// Need to know the old border width!
void CTerm5250::OnBorderWidthChanged()
{
    FUNC_ENTER();
    if ( m_bBorderVisible && m_hWnd )
        GrabMaxSpace();
}

void CTerm5250::OnCaptionChanged()
{
    FUNC_ENTER();
    PostBufferRefresh(false);
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

void CTerm5250::OnForeColorChanged()
{
    //	FUNC_ENTER();
    m_ColorList[A_5250_WHITE] = m_clrForeColor;
    PostBufferRefresh(false);

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

// HostName property
STDMETHODIMP CTerm5250::get_HostName(BSTR* pVal)
{
    //	FUNC_ENTER();
    return m_HostName.CopyTo(pVal);
}


STDMETHODIMP CTerm5250::put_HostName(BSTR newVal)
{
    FUNC_ENTER1("(%ls)", newVal);

    // Check minimal validity, either IP or DNS.
    // Valid caracters are [0-9] [a-z] [A-Z] . - _ : /
    ATL::CStringW TmpVal(newVal);
    TmpVal.Trim();
    const WCHAR * Buffer = TmpVal.GetBuffer();
    const int length = TmpVal.GetLength();
    for ( int i = 0; i < length; i++ )
    {
        const WCHAR w = Buffer[i];
        if (!( (w>=_T('0')&&w<=_T('9')) || (w>=_T('a')&&w<=_T('z'))
            || (w>=_T('A')&&w<=_T('Z'))
            ||  w==_T('-') || w==_T('.') || w==_T('/')
            ||  w==_T('_') || w==_T(':') ))
        {
            return E_INVALIDARG;
        }
    }
    if ( !CanEdit(1) )
        return S_FALSE;
    m_HostName = TmpVal;
    ValueChanged(1);
    return S_OK;
}

void CTerm5250::OnMouseMoveCursorChanged()
{
    FUNC_ENTER();
    if ( m_bMouseMoveCursor )
    {
        m_bSelected = false;
        m_bSelecting = false;
    }
}


void CTerm5250::OnCaretStyleChanged()
{
    FUNC_ENTER1(" %d", m_eCaretStyle);
    HideCaret();
    RegenCaret();
}

// Colors property
STDMETHODIMP CTerm5250::get_Colors(OLE_COLOR * colors[10])
{
    if ( !colors )
        return E_INVALIDARG;

    memcpy(colors, m_ColorList, sizeof(m_ColorList));
    return S_OK;
}

STDMETHODIMP CTerm5250::put_Colors(const OLE_COLOR colors[10])
{
    FUNC_ENTER1("(%d)", colors[0]);
    if ( !CanEdit(4) )
        return S_FALSE;
    memcpy(m_ColorList, colors, sizeof(m_ColorList));
    PostBufferRefresh(false);
    ValueChanged(4);
    return S_OK;
}


STDMETHODIMP CTerm5250::get_About(BSTR* pVal)
{
    //FUNC_ENTER();
    if ( !pVal )
        return E_INVALIDARG;

    ATL::CStringW szAbout(g_szAbout);
    szAbout += _T("\nBuilt at ");
    szAbout += k_BuildDate;
    *pVal = ::SysAllocString(szAbout);

    if (pVal == NULL)
        return E_OUTOFMEMORY;
    return S_OK;
}

void CTerm5250::OnEnhancedChanged()
{
    FUNC_ENTER();
}
void CTerm5250::OnDisplayRulerChanged()
{
    FUNC_ENTER1(" %d", m_bDisplayRuler);
    FireViewChange();
}


// FontName property
STDMETHODIMP CTerm5250::get_FontName(BSTR * pVal)
{
    // TBM Useless double-copy
    ATL::CComBSTR Font(m_FontName);
    return Font.CopyTo(pVal);
}


STDMETHODIMP CTerm5250::put_FontName(BSTR newVal)
{
    FUNC_ENTER1("(%ls)", newVal);

    // Check minimal validity
    // Valid caracters are [0-9] [a-z] [A-Z] " "
    ATL::CStringW TmpVal(newVal);
    TmpVal.Trim();
    const WCHAR * Buffer = TmpVal.GetBuffer();
    const int length = TmpVal.GetLength();
    if ( length < 2 )
        return E_INVALIDARG;
    for ( int i = 0; i < length; i++ )
    {
        const WCHAR w = Buffer[i];
        if (!( (w>=_T('0')&&w<=_T('9')) || (w>=_T('a')&&w<=_T('z'))
            || (w>=_T('A')&&w<=_T('Z')) ||  w==_T(' ') ))
        {
            return E_INVALIDARG;
        }
    }
    if ( !CanEdit(12) )
        return S_FALSE;
    m_FontName = TmpVal;
    m_bFontChanged = 1;
    // Don't trust the user, find the nearest font name
    HFONT hFont = CreateFont(-10, 0, 0, 0, FW_DONTCARE, 
                FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
                CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH|FF_DONTCARE, 
                m_FontName);
    HDC hDC = CreateCompatibleDC(NULL);
    HGDIOBJ hOld = SelectObject(hDC, hFont);
    int Size = GetOutlineTextMetrics(hDC, 0, NULL);
    if ( Size > 0 )
    {
        OUTLINETEXTMETRIC * potm = (OUTLINETEXTMETRIC *)malloc(Size);
        if ( GetOutlineTextMetrics(hDC, Size, potm) )
            m_FontName = (((PSTR)potm)+(DWORD_PTR)potm->otmpFaceName);
        free(potm);
    }
    SelectObject(hDC, hOld);
    DeleteObject(hFont);
    DeleteDC(hDC);
    GrabMaxSpace();
    ValueChanged(12);
    return S_OK;
}




// DeviceName property
STDMETHODIMP CTerm5250::get_DeviceName(BSTR * pVal)
{
    return m_Device.CopyTo(pVal);
}

STDMETHODIMP CTerm5250::put_DeviceName(BSTR newVal)
{
    FUNC_ENTER1("(%ls)", newVal);

    // Check minimal validity
    // Valid caracters are [0-9] [a-z] [A-Z] " "
    ATL::CStringW TmpVal(newVal);
    TmpVal.Trim();
    const WCHAR * Buffer = TmpVal.GetBuffer();
    const int length = TmpVal.GetLength();
    for ( int i = 0; i < length; i++ )
    {
        const WCHAR w = Buffer[i];
        if (!( (w>=_T('0')&&w<=_T('9')) || (w>=_T('a')&&w<=_T('z'))
            || (w>=_T('A')&&w<=_T('Z')) ||  w==_T(' ') ))
        {
            return E_INVALIDARG;
        }
    }
    if ( !CanEdit(13) )
        return S_FALSE;
    m_Device = newVal;
    ValueChanged(13);
    return S_OK;
}

// CodePage property
STDMETHODIMP CTerm5250::get_CodePage(long * pVal)
{
    if ( !pVal )
        return E_POINTER;
    *pVal = m_CodePage;
    return S_OK;
}

STDMETHODIMP CTerm5250::put_CodePage(long newVal)
{
    FUNC_ENTER1("(%d)", newVal);
    if ( !CanEdit(14) || !newVal )
        return S_FALSE;
    
    // Verify it
    char Tmp[64];
    sprintf(Tmp, "%d", newVal);
    for (const Tn5250CharMap * itr = tn5250_transmaps; itr->name; ++itr)
    {
        if ( !strcmp(itr->name, Tmp) )
        {
            m_CodePage = newVal;
            ValueChanged(14);
            PostBufferRefresh(true);    // In case the font needs to be reloaded
            return S_OK;
        }
    }
    return E_INVALIDARG;
}

// Wide property
STDMETHODIMP CTerm5250::get_Wide(VARIANT_BOOL * pVal)
{
    if ( !pVal )
        return E_POINTER;
    *pVal = m_bWide ? VARIANT_TRUE : VARIANT_FALSE;
    return S_OK;
}

STDMETHODIMP CTerm5250::put_Wide(VARIANT_BOOL newVal)
{
    FUNC_ENTER1("(%d)", newVal);
    if ( !CanEdit(15) )
        return S_FALSE;
    m_bWide = newVal!=0;
    ValueChanged(15);
    return S_OK;
}

// Wide property
STDMETHODIMP CTerm5250::get_UseComputerName(VARIANT_BOOL * pVal)
{
    if ( !pVal )
        return E_POINTER;
    *pVal = m_bUseComputerName ? VARIANT_TRUE : VARIANT_FALSE;
    return S_OK;
}

STDMETHODIMP CTerm5250::put_UseComputerName(VARIANT_BOOL newVal)
{
    FUNC_ENTER1("(%d)", newVal);
    if ( !CanEdit(16) )
        return S_FALSE;
    m_bUseComputerName = newVal!=0;
    ValueChanged(16);
    return S_OK;
}

STDMETHODIMP CTerm5250::get_CursorPositionX(long * pVal)
{
    if (!pVal)
        return E_POINTER;
    if (!display)
        return E_UNEXPECTED;
    
    *pVal = tn5250_display_cursor_x(display);
    return S_OK;
}

STDMETHODIMP CTerm5250::put_CursorPositionX(long newVal)
{
    FUNC_ENTER1("(%d)", newVal);
    if (!display)
        return E_UNEXPECTED;
    if (newVal<0 || newVal >= tn5250_display_width(display))
        return E_INVALIDARG;

    tn5250_display_set_cursor(display, tn5250_display_cursor_y(display), newVal);
    FireViewChange();
            
    return tn5250_display_field_at(display, tn5250_display_cursor_y(display), tn5250_display_cursor_x(display))
        ? S_OK : S_FALSE;
}

STDMETHODIMP CTerm5250::get_CursorPositionY(long * pVal)
{
    if (!pVal)
        return E_POINTER;
    if (!display)
        return E_UNEXPECTED;
    *pVal = tn5250_display_cursor_y(display);
    return S_OK;
}

STDMETHODIMP CTerm5250::put_CursorPositionY(long newVal)
{
    FUNC_ENTER1("(%d)", newVal);
    if (!display)
        return E_UNEXPECTED;
    if (newVal<0 || newVal >= tn5250_display_height(display))
        return E_INVALIDARG;

    tn5250_display_set_cursor(display, newVal, tn5250_display_cursor_x(display));
    FireViewChange();

    return tn5250_display_field_at(display, tn5250_display_cursor_y(display), tn5250_display_cursor_x(display))
        ? S_OK : S_FALSE;
}

STDMETHODIMP CTerm5250::get_ScreenWidth(long * pVal)
{
    if (!pVal)
        return E_POINTER;
    if (!display)
        return E_UNEXPECTED;
    *pVal = tn5250_display_width(display);
    return S_OK;
}

STDMETHODIMP CTerm5250::get_ScreenHeight(long * pVal)
{
    if (!pVal)
        return E_POINTER;
    if (!display)
        return E_UNEXPECTED;
    *pVal = tn5250_display_height(display);
    return S_OK;
}

STDMETHODIMP CTerm5250::ScreenScrape(long Y, long X, long Length, BSTR * pString)
{
    FUNC_ENTER3("(%d, %d, %d)", Y, X, Length);
    if (!pString)
        return E_POINTER;
    if (!display)
        return E_UNEXPECTED;
    if (Y<0 || Y >= tn5250_display_height(display) || X+Length >= tn5250_display_width(display) || X<0)
        return E_INVALIDARG;

    ATL::CComBSTR p;
    for (int x = X; x < (X+Length); ++x)
    {
        char c = tn5250_display_char_at(display, Y, x);
        if (((c & 0xe0) == 0x20 )||(c < 0x40 && c > 0x00)||(c == 0xff)) 
            c = ' ';
        else 
            c = tn5250_char_map_to_local( tn5250_display_char_map(display), c);
        wchar_t u;
        if (!MultiByteToWideChar(CP_THREAD_ACP, MB_PRECOMPOSED, &c, 1, &u, 1))
            return E_FAIL;
        HRESULT hr = p.Append(u);
        if (FAILED(hr))
            return hr;
    }

    FUNC_ENTER1(" %ls", p.m_str);
    return p.CopyTo(pString);
}

STDMETHODIMP CTerm5250::GetControlInfo(CONTROLINFO * pCI)
{
    FUNC_ENTER();
    if (!pCI)
        return E_POINTER;

    if (pCI->cb < sizeof(CONTROLINFO))
        return E_INVALIDARG;
    pCI->hAccel = 0;
    pCI->cAccel = 0;
    pCI->dwFlags = CTRLINFO_EATS_RETURN|CTRLINFO_EATS_ESCAPE;
    return S_OK;
}

STDMETHODIMP CTerm5250::OnMnemonic(MSG * /*pMsg*/)
{
    FUNC_ENTER();
    return E_NOTIMPL;
}

STDMETHODIMP CTerm5250::GetCanonicalFormatEtc(FORMATETC * pformatectIn, FORMATETC * pformatetcOut)
{
    FUNC_ENTER2("(%d, %d)", pformatectIn->cfFormat, pformatectIn->dwAspect);
    if (pformatectIn->lindex != -1)
        return DV_E_LINDEX;

    if (pformatectIn->cfFormat == CF_TEXT)
    {
        *pformatetcOut = *pformatectIn;
        pformatetcOut->ptd = NULL;
        return DATA_S_SAMEFORMATETC;
    }
    pformatetcOut->cfFormat = CF_TEXT;
    pformatetcOut->lindex = -1;
    pformatetcOut->ptd = NULL;
    pformatetcOut->tymed = 0;
    pformatetcOut->dwAspect = pformatectIn->dwAspect;
    return S_OK;
}

// ISupportsErrorInfo
STDMETHODIMP CTerm5250::InterfaceSupportsErrorInfo(REFIID riid)
{
    static const IID* arr[] = { &IID_ITerm5250 };
    for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
        if (InlineIsEqualGUID(*arr[i], riid))
            return S_OK;
    return S_FALSE;
}

