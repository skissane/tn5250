// Cardinal Health AIS Canada (c) 2003-2004
// (c) 2003 Marc-Antoine Ruel
#pragma once


#if _WIN32_WINNT != 0x0501
#error You must #define _WIN32_WINNT 0x0501 before #include <windows.h>
#endif

// Usage:
//  MsgLog(MessageId, wParam, lParam)
// or in MFC:
//  DECLARE_MESSAGE_LOG() in the class definition.
#ifdef _DEBUG

    void MsgLog(UINT Msg, WPARAM wParam, LPARAM lParam, HWND hWnd = 0);
    #define DECLARE_MESSAGE_LOG() virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) { MsgLog(message, wParam, lParam); return super::OnWndMsg(message, wParam, lParam, pResult); }
    void MsgLogDisable(UINT Msg);

#else

    #define MsgLog(_X_, _Y_, _Z_) __noop()
    #define DECLARE_MESSAGE_LOG()
    #define MsgLogDisable(_X_) __noop()

#endif


#ifdef MSGLOG_MAKE_DEFINES

#define WM_SYSTIMER 0x118

// Add the missing defines if not compiling for W2K or XP...
#if(WINVER < 0x0500)
#define WM_GETOBJECT                    0x003D
#define WM_MENURBUTTONUP                0x0122
#define WM_MENUDRAG                     0x0123
#define WM_MENUGETOBJECT                0x0124
#define WM_UNINITMENUPOPUP              0x0125
#define WM_MENUCOMMAND                  0x0126
#define WM_IME_REQUEST                  0x0288
#define WM_NCMOUSEHOVER                 0x02A0
#define WM_NCMOUSELEAVE                 0x02A2
#endif
#if(_WIN32_WINNT < 0x0500)
#define WM_NCXBUTTONDOWN                0x00AB
#define WM_NCXBUTTONUP                  0x00AC
#define WM_NCXBUTTONDBLCLK              0x00AD
#define WM_CHANGEUISTATE                0x0127
#define WM_UPDATEUISTATE                0x0128
#define WM_QUERYUISTATE                 0x0129
#define WM_XBUTTONDOWN                  0x020B
#define WM_XBUTTONUP                    0x020C
#define WM_XBUTTONDBLCLK                0x020D
#define WM_APPCOMMAND                   0x0319
#endif
#if(_WIN32_WINNT < 0x0501)
#define WM_INPUT                        0x00FF
#define WM_WTSSESSION_CHANGE            0x02B1
#define WM_TABLET_FIRST                 0x02c0
#define WM_TABLET_LAST                  0x02df
#define WM_THEMECHANGED                 0x031A
#endif
#if !defined(_WIN32_IE) || (_WIN32_IE < 0x0500)
#define TBN_RESTORE             (TBN_FIRST - 21)
#define TBN_SAVE                (TBN_FIRST - 22)
#define TBN_INITCUSTOMIZE       (TBN_FIRST - 23)
#define    TBNRF_HIDEHELP       0x00000001
#define    TBNRF_ENDCUSTOMIZE   0x00000002
#endif
#if !defined(_WIN32_IE) || (_WIN32_IE < 0x0401)
#define NM_TOOLTIPSCREATED      (NM_FIRST-19)   // notify of when the tooltips window is create
#endif
#if !defined(_WIN32_IE) || (_WIN32_IE < 0x0500)
#define NM_LDOWN                (NM_FIRST-20)
#define NM_RDOWN                (NM_FIRST-21)
#define NM_THEMECHANGED         (NM_FIRST-22)
#endif
#if !defined(_WIN32_IE) || (_WIN32_IE < 0x0500)
#define HDN_FILTERCHANGE        (HDN_FIRST-12)
#define HDN_FILTERBTNCLICK      (HDN_FIRST-13)
#endif

#define WM_ISACTIVEICON                 0x0313

// Taken from afxpriv.h
#define WM_QUERYAFXWNDPROC  0x0360  // lResult = 1 if processed by AfxWndProc
#define WM_SIZEPARENT       0x0361  // lParam = &AFX_SIZEPARENTPARAMS
#define WM_SETMESSAGESTRING 0x0362  // wParam = nIDS (or 0), lParam = lpszOther (or NULL)
#define WM_IDLEUPDATECMDUI  0x0363  // wParam == bDisableIfNoHandler
#define WM_INITIALUPDATE    0x0364  // (params unused) - sent to children
#define WM_COMMANDHELP      0x0365  // lResult = TRUE/FALSE, lParam = dwContext
#define WM_HELPHITTEST      0x0366  // lResult = dwContext, lParam = MAKELONG(x,y)
#define WM_EXITHELPMODE     0x0367  // (params unused)
#define WM_RECALCPARENT     0x0368  // force RecalcLayout on frame window (only for inplace frame windows)
#define WM_SIZECHILD        0x0369  // special notify from COleResizeBar wParam = ID of child window lParam = lpRectNew (new position/size)
#define WM_KICKIDLE         0x036A  // (params unused) causes idles to kick in
#define WM_QUERYCENTERWND   0x036B  // lParam = HWND to use as centering parent
#define WM_DISABLEMODAL     0x036C  // lResult = 0, disable during modal state lResult = 1, don't disable
#define WM_FLOATSTATUS      0x036D  // wParam combination of FS_* flags below
// WM_ACTIVATETOPLEVEL is like WM_ACTIVATEAPP but works with hierarchies of mixed processes (as is the case with OLE in-place activation)
#define WM_ACTIVATETOPLEVEL 0x036E  // wParam = nState (like WM_ACTIVATE) lParam = pointer to HWND[2] lParam[0] = hWnd getting WM_ACTIVATE lParam[1] = hWndOther
#define WM_RESERVED_036F	0x036F  // was WM_QUERY3DCONTROLS (now not used)
#define WM_RESERVED_0370    0x0370  // Note: Messages 0x0370, 0x0371, and 0x372 were incorrectly used by
#define WM_RESERVED_0371    0x0371  //  some versions of Windows.  To remain compatible, MFC does not
#define WM_RESERVED_0372    0x0372  //  use messages in that range.
#define WM_SOCKET_NOTIFY    0x0373  // WM_SOCKET_NOTIFY and WM_SOCKET_DEAD are used internally by MFC's
#define WM_SOCKET_DEAD      0x0374  // Windows sockets implementation.  For more information, see sockcore.cpp
#define WM_POPMESSAGESTRING 0x0375  // same as WM_SETMESSAGESTRING except not popped if IsTracking()
#define WM_HELPPROMPTADDR	0x0376  // WM_HELPPROMPTADDR is used internally to get the address of m_dwPromptContext from the associated frame window. This is used during message boxes to setup for F1 help while that msg box is displayed. lResult is the address of m_dwPromptContext.
#define WM_QUEUE_SENTINEL   0x0379  // Marker used while rearranging the message queue
#define WM_RESERVED_037C    0x037C  // Note: Messages 0x037C - 0x37E reserved for future MFC use.
#define WM_RESERVED_037D    0x037D
#define WM_RESERVED_037E    0x037E
#define WM_FORWARDMSG		0x037F  // WM_FORWARDMSG - used by ATL to forward a message to another window for processing; WPARAM - DWORD dwUserData - defined by user; LPARAM - LPMSG pMsg - a pointer to the MSG structure; return value - 0 if the message was not processed, nonzero if it was

#ifndef WM_CTLCOLOR
#define WM_CTLCOLOR                             0x0019
#endif

#endif  // #ifdef MSGLOG_MAKE_DEFINES
