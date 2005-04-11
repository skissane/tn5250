// Cardinal Health AIS Canada (c) 2003
// (c) 2003 Marc-Antoine Ruel
#include "stdafx.h"
#define MSGLOG_MAKE_DEFINES
#include "MsgLog.h"
#pragma warning(disable:4710) // '' : function not inlined
#pragma warning(disable:4668) // 'ISOLATION_AWARE_ENABLED' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#include <commctrl.h>
#include "utils.h"

// Only active in DEBUG build
#ifdef _DEBUG
#pragma optimize("agt", on)

#pragma warning(disable:4512) // '' : assignment operator could not be generated
#pragma warning(disable:4820) // '' : 'N' bytes padding added after member ''
#include "DDE.h"

// Handles default messages
void DefMsgCracker(HWND hWnd, UINT, const TCHAR * String, WPARAM wParam, LPARAM lParam)
{
    DbgPrintf(( _T("0x%06X %-20s 0x%08X 0x%08X\n"), hWnd, String, wParam, lParam));
}

// Don't display anything
void SkipMsgCracker(HWND, UINT, const TCHAR *, WPARAM, LPARAM)
{
}

// For WM_STYLECHANGING and WM_STYLECHANGED
void WM_STYLE_Cracker(HWND hWnd, UINT, const TCHAR * String, WPARAM wParam, LPARAM lParam)
{
    DbgPrintf(( _T("0x%06X %-20s %s Old:0x%08X New:0x%08X\n"), hWnd, String, wParam==GWL_EXSTYLE?_T("GWL_EXSTYLE"):_T("GWL_STYLE"), ((STYLESTRUCT*)lParam)->styleOld, ((STYLESTRUCT*)lParam)->styleNew));
}

void WM_WINDOWPOS_Cracker(HWND hWnd, UINT, const TCHAR * String, WPARAM /*wParam*/, LPARAM lParam)
{
    const WINDOWPOS & w = *(WINDOWPOS*)lParam;
    DbgPrintf(( _T("0x%06X %-20s 0x%06X 0x%06X (%d,%d) (%d,%d) 0x%X0\n"),
        hWnd, String, w.hwnd, w.hwndInsertAfter, w.x, w.y, w.cx, w.cy, w.flags));
}

// Forward declaration
void WM_NOTIFY_Cracker(HWND hWnd, UINT, const TCHAR * String, WPARAM wParam, LPARAM lParam);

// Handles default WM_NOTIFY
void DefMsgCracker(HWND hWnd, UINT, const TCHAR * String, UINT Id)
{
    DbgPrintf(( _T("0x%06X %-20s idCtrl:%-5d %s\n"), hWnd, _T("WM_NOTIFY"), Id, String ));
}

// Don't display anything (for WM_NOTIFY)
void SkipMsgCracker(HWND, UINT, const TCHAR *, UINT)
{
}



// Show the default message
#define DMSG1(_X_) _X_, _T( #_X_ ), DefMsgCracker,
// Hide that message
#define DMSGN(_X_) _X_, _T( #_X_ ), SkipMsgCracker,
// Call a custom handler
#define DMSG2(_X_, _Y_) _X_, _T( #_X_ ), _Y_,

// Array for all the known messages
#pragma warning(disable:4510) // '' : default constructor could not be generated
#pragma warning(disable:4610) // struct '' can never be instantiated - user defined constructor required
struct MessageCracker
{
    const UINT Message;
    const TCHAR * const String;
    void (* fn)(HWND, UINT, const TCHAR *, WPARAM, LPARAM);
};
MessageCracker Msgs[] =
{
    DMSG1(WM_NULL           )
    DMSG1(WM_CREATE         )
    DMSG1(WM_DESTROY        )
    DMSG1(WM_MOVE           )
    DMSG1(WM_SIZE           )
    DMSG1(WM_ACTIVATE       )
    DMSG1(WM_SETFOCUS       )
    DMSG1(WM_KILLFOCUS      )
    DMSG1(WM_ENABLE         )
    DMSG1(WM_SETREDRAW      )
    DMSG1(WM_SETTEXT        )
    DMSG1(WM_GETTEXT        )
    DMSG1(WM_GETTEXTLENGTH  )
    DMSG1(WM_PAINT          )
    DMSG1(WM_CLOSE          )
    DMSG1(WM_QUERYENDSESSION)
    DMSG1(WM_QUIT           )
    DMSG1(WM_QUERYOPEN      )
    DMSG1(WM_ERASEBKGND     )
    DMSG1(WM_SYSCOLORCHANGE )
    DMSG1(WM_ENDSESSION     )
    DMSG1(WM_SHOWWINDOW     )
    DMSG1(WM_CTLCOLOR       )
    DMSG1(WM_WININICHANGE   )
    DMSG1(WM_DEVMODECHANGE  )
    DMSG1(WM_ACTIVATEAPP    )
    DMSG1(WM_FONTCHANGE     )
    DMSG1(WM_TIMECHANGE     )
    DMSG1(WM_CANCELMODE     )
    DMSG1(WM_SETCURSOR      )
    DMSG1(WM_MOUSEACTIVATE  )
    DMSG1(WM_CHILDACTIVATE  )
    DMSG1(WM_QUEUESYNC      )
    DMSG1(WM_GETMINMAXINFO  )
    DMSG1(WM_PAINTICON      )
    DMSG1(WM_ICONERASEBKGND )
    DMSG1(WM_NEXTDLGCTL     )
    DMSG1(WM_SPOOLERSTATUS  )
    DMSG1(WM_DRAWITEM       )
    DMSG1(WM_MEASUREITEM    )
    DMSG1(WM_DELETEITEM     )
    DMSG1(WM_VKEYTOITEM     )
    DMSG1(WM_CHARTOITEM     )
    DMSG1(WM_SETFONT        )
    DMSG1(WM_GETFONT        )
    DMSG1(WM_SETHOTKEY      )
    DMSG1(WM_GETHOTKEY      )
    DMSG1(WM_QUERYDRAGICON  )
    DMSG1(WM_COMPAREITEM    )
    DMSG1(WM_GETOBJECT        )
    DMSG1(WM_COMPACTING       )
    DMSG1(WM_COMMNOTIFY       )
    DMSG2(WM_WINDOWPOSCHANGING, WM_WINDOWPOS_Cracker)
    DMSG2(WM_WINDOWPOSCHANGED , WM_WINDOWPOS_Cracker)
    DMSG1(WM_POWER            )
    DMSG1(WM_COPYDATA         )
    DMSG1(WM_CANCELJOURNAL    )
    DMSG2(WM_NOTIFY, WM_NOTIFY_Cracker)
    DMSG1(WM_INPUTLANGCHANGEREQUEST)
    DMSG1(WM_INPUTLANGCHANGE       )
    DMSG1(WM_TCARD                 )
    DMSG1(WM_HELP                  )
    DMSG1(WM_USERCHANGED           )
    DMSG1(WM_NOTIFYFORMAT          )
    DMSG1(WM_CONTEXTMENU           )
    DMSG2(WM_STYLECHANGING, WM_STYLE_Cracker)
    DMSG2(WM_STYLECHANGED , WM_STYLE_Cracker)
    DMSG1(WM_DISPLAYCHANGE         )
    DMSG1(WM_GETICON               )
    DMSG1(WM_SETICON               )
    DMSG1(WM_NCCREATE       )
    DMSG1(WM_NCDESTROY      )
    DMSG1(WM_NCCALCSIZE     )
    DMSG1(WM_NCHITTEST      )
    DMSG1(WM_NCPAINT        )
    DMSG1(WM_NCACTIVATE     )
    DMSG1(WM_GETDLGCODE     )
    DMSG1(WM_SYNCPAINT      )
    DMSG1(WM_NCMOUSEMOVE    )
    DMSG1(WM_NCLBUTTONDOWN  )
    DMSG1(WM_NCLBUTTONUP    )
    DMSG1(WM_NCLBUTTONDBLCLK)
    DMSG1(WM_NCRBUTTONDOWN  )
    DMSG1(WM_NCRBUTTONUP    )
    DMSG1(WM_NCRBUTTONDBLCLK)
    DMSG1(WM_NCMBUTTONDOWN  )
    DMSG1(WM_NCMBUTTONUP    )
    DMSG1(WM_NCMBUTTONDBLCLK)
    DMSG1(WM_NCXBUTTONDOWN  )
    DMSG1(WM_NCXBUTTONUP    )
    DMSG1(WM_NCXBUTTONDBLCLK)
    DMSG1(WM_INPUT      )
  //DMSG1(WM_KEYFIRST   )
    DMSG1(WM_KEYDOWN    )
    DMSG1(WM_KEYUP      )
    DMSG1(WM_CHAR       )
    DMSG1(WM_DEADCHAR   )
    DMSG1(WM_SYSKEYDOWN )
    DMSG1(WM_SYSKEYUP   )
    DMSG1(WM_SYSCHAR    )
    DMSG1(WM_SYSDEADCHAR)
    DMSG1(WM_UNICHAR    )
  //DMSG1(WM_KEYLAST    )
    DMSG1(WM_IME_STARTCOMPOSITION)
    DMSG1(WM_IME_ENDCOMPOSITION  )
    DMSG1(WM_IME_COMPOSITION     )
  //DMSG1(WM_IME_KEYLAST         )
    DMSG1(WM_INITDIALOG   )
    DMSG1(WM_COMMAND      )
    DMSG1(WM_SYSCOMMAND   )
    DMSG1(WM_TIMER        )
    DMSG1(WM_HSCROLL      )
    DMSG1(WM_VSCROLL      )
    DMSG1(WM_INITMENU     )
    DMSG1(WM_INITMENUPOPUP)
    DMSG1(WM_SYSTIMER     )
    DMSG1(WM_MENUSELECT   )
    DMSG1(WM_MENUCHAR     )
    DMSG1(WM_ENTERIDLE    )
    DMSG1(WM_MENURBUTTONUP  )
    DMSG1(WM_MENUDRAG       )
    DMSG1(WM_MENUGETOBJECT  )
    DMSG1(WM_UNINITMENUPOPUP)
    DMSG1(WM_MENUCOMMAND    )
    DMSG1(WM_CHANGEUISTATE  )
    DMSG1(WM_UPDATEUISTATE  )
    DMSG1(WM_QUERYUISTATE   )
    DMSG1(WM_CTLCOLORMSGBOX   )
    DMSG1(WM_CTLCOLOREDIT     )
    DMSG1(WM_CTLCOLORLISTBOX  )
    DMSG1(WM_CTLCOLORBTN      )
    DMSG1(WM_CTLCOLORDLG      )
    DMSG1(WM_CTLCOLORSCROLLBAR)
    DMSG1(WM_CTLCOLORSTATIC   )
    DMSG1(MN_GETHMENU         )
  //DMSG1(WM_MOUSEFIRST    )
    DMSG1(WM_MOUSEMOVE     )
    DMSG1(WM_LBUTTONDOWN   )
    DMSG1(WM_LBUTTONUP     )
    DMSG1(WM_LBUTTONDBLCLK )
    DMSG1(WM_RBUTTONDOWN   )
    DMSG1(WM_RBUTTONUP     )
    DMSG1(WM_RBUTTONDBLCLK )
    DMSG1(WM_MBUTTONDOWN   )
    DMSG1(WM_MBUTTONUP     )
    DMSG1(WM_MBUTTONDBLCLK )
    DMSG1(WM_MOUSEWHEEL    )
    DMSG1(WM_XBUTTONDOWN   )
    DMSG1(WM_XBUTTONUP     )
    DMSG1(WM_XBUTTONDBLCLK )
  //DMSG1(WM_MOUSELAST     )
    DMSG1(WM_PARENTNOTIFY  )
    DMSG1(WM_ENTERMENULOOP )
    DMSG1(WM_EXITMENULOOP  )
    DMSG1(WM_NEXTMENU      )
    DMSG1(WM_SIZING        )
    DMSG1(WM_CAPTURECHANGED)
    DMSG1(WM_MOVING        )
    DMSG1(WM_POWERBROADCAST)
    DMSG1(WM_DEVICECHANGE  )
    DMSG1(WM_MDICREATE     )
    DMSG1(WM_MDIDESTROY    )
    DMSG1(WM_MDIACTIVATE   )
    DMSG1(WM_MDIRESTORE    )
    DMSG1(WM_MDINEXT       )
    DMSG1(WM_MDIMAXIMIZE   )
    DMSG1(WM_MDITILE       )
    DMSG1(WM_MDICASCADE    )
    DMSG1(WM_MDIICONARRANGE)
    DMSG1(WM_MDIGETACTIVE  )
    DMSG1(WM_MDISETMENU    )
    DMSG1(WM_ENTERSIZEMOVE )
    DMSG1(WM_EXITSIZEMOVE  )
    DMSG1(WM_DROPFILES     )
    DMSG1(WM_MDIREFRESHMENU)
    DMSG1(WM_IME_SETCONTEXT     )
    DMSG1(WM_IME_NOTIFY         )
    DMSG1(WM_IME_CONTROL        )
    DMSG1(WM_IME_COMPOSITIONFULL)
    DMSG1(WM_IME_SELECT         )
    DMSG1(WM_IME_CHAR           )
    DMSG1(WM_IME_REQUEST      )
    DMSG1(WM_IME_KEYDOWN      )
    DMSG1(WM_IME_KEYUP        )
    DMSG1(WM_NCMOUSEHOVER     )
    DMSG1(WM_MOUSEHOVER       )
    DMSG1(WM_NCMOUSELEAVE     )
    DMSG1(WM_MOUSELEAVE       )
    DMSG1(WM_WTSSESSION_CHANGE)
    DMSG1(WM_TABLET_FIRST     )
    DMSG1(WM_TABLET_LAST      )
    DMSG1(WM_CUT              )
    DMSG1(WM_COPY             )
    DMSG1(WM_PASTE            )
    DMSG1(WM_CLEAR            )
    DMSG1(WM_UNDO             )
    DMSG1(WM_RENDERFORMAT     )
    DMSG1(WM_RENDERALLFORMATS )
    DMSG1(WM_DESTROYCLIPBOARD )
    DMSG1(WM_DRAWCLIPBOARD    )
    DMSG1(WM_PAINTCLIPBOARD   )
    DMSG1(WM_VSCROLLCLIPBOARD )
    DMSG1(WM_SIZECLIPBOARD    )
    DMSG1(WM_ASKCBFORMATNAME  )
    DMSG1(WM_CHANGECBCHAIN    )
    DMSG1(WM_HSCROLLCLIPBOARD )
    DMSG1(WM_QUERYNEWPALETTE  )
    DMSG1(WM_PALETTEISCHANGING)
    DMSG1(WM_PALETTECHANGED   )
    DMSG1(WM_HOTKEY           )
    DMSG1(WM_ISACTIVEICON     )
    DMSG1(WM_PRINT            )
    DMSG1(WM_PRINTCLIENT      )
    DMSG1(WM_APPCOMMAND       )
    DMSG1(WM_THEMECHANGED     )
    DMSG1(WM_HANDHELDFIRST    )
    DMSG1(WM_HANDHELDLAST     )
    DMSG1(WM_QUERYAFXWNDPROC    )
    DMSG1(WM_SIZEPARENT         )
    DMSG1(WM_SETMESSAGESTRING   )
    DMSG1(WM_IDLEUPDATECMDUI    )
    DMSG1(WM_INITIALUPDATE      )
    DMSG1(WM_COMMANDHELP        )
    DMSG1(WM_HELPHITTEST        )
    DMSG1(WM_EXITHELPMODE       )
    DMSG1(WM_RECALCPARENT       )
    DMSG1(WM_SIZECHILD          )
    DMSG1(WM_KICKIDLE           )
    DMSG1(WM_QUERYCENTERWND     )
    DMSG1(WM_DISABLEMODAL       )
    DMSG1(WM_FLOATSTATUS        )
    DMSG1(WM_ACTIVATETOPLEVEL   )
    DMSG1(WM_RESERVED_036F      )
    DMSG1(WM_RESERVED_0370      )
    DMSG1(WM_RESERVED_0371      )
    DMSG1(WM_RESERVED_0372      )
    DMSG1(WM_SOCKET_NOTIFY      )
    DMSG1(WM_SOCKET_DEAD        )
    DMSG1(WM_POPMESSAGESTRING   )
    DMSG1(WM_HELPPROMPTADDR     )
    DMSG1(WM_QUEUE_SENTINEL     )
    DMSG1(WM_RESERVED_037C      )
    DMSG1(WM_RESERVED_037D      )
    DMSG1(WM_RESERVED_037E      )
    DMSG1(WM_FORWARDMSG         )
    DMSG1(WM_PENWINFIRST        )
    DMSG1(WM_PENWINLAST         )
    DMSG1(WM_DDE_INITIATE       )
    DMSG1(WM_DDE_TERMINATE      )
    DMSG1(WM_DDE_ADVISE	        )
    DMSG1(WM_DDE_UNADVISE       )
    DMSG1(WM_DDE_ACK	        )
    DMSG1(WM_DDE_DATA	        )
    DMSG1(WM_DDE_REQUEST	    )
    DMSG1(WM_DDE_POKE	        )
    DMSG1(WM_DDE_EXECUTE	    )
    DMSG1(WM_USER+0)
    DMSG1(WM_USER+1)
    DMSG1(WM_USER+2)
    DMSG1(WM_USER+3)
    DMSG1(WM_USER+4)
    DMSG1(WM_USER+5)
    DMSG1(WM_USER+6)
    DMSG1(WM_USER+7)
    DMSG1(WM_USER+8)
    DMSG1(WM_USER+9)
    DMSG1(WM_USER+10)
    DMSG1(WM_USER+11)
    DMSG1(WM_USER+12)
    DMSG1(WM_USER+13)
    DMSG1(WM_USER+14)
    DMSG1(WM_USER+15)
    DMSG1(WM_USER+16)
    DMSG1(WM_USER+17)
    DMSG1(WM_USER+18)
    DMSG1(WM_USER+19)
    DMSG1(WM_USER+20)
    DMSG1(WM_USER+21)
    DMSG1(WM_USER+22)
    DMSG1(WM_USER+23)
    DMSG1(WM_USER+24)
    DMSG1(WM_USER+25)
    DMSG1(WM_USER+26)
    DMSG1(WM_USER+27)
    DMSG1(WM_USER+28)
    DMSG1(WM_USER+29)
    DMSG1(WM_PARENTNOTIFY+0x2000)
    DMSG1(WM_APP    )
    DMSG1(0xFFFFFFFF)
};

struct NotifyCracker
{
    const UINT Code;
    const TCHAR * const String;
    void (* fn)(HWND, UINT, const TCHAR *, UINT);
};
NotifyCracker Notify[] =
{
    DMSG1(TBN_GETBUTTONINFOA)
    DMSG1(TBN_BEGINDRAG    )
    DMSG1(TBN_ENDDRAG      )
    DMSG1(TBN_BEGINADJUST  )
    DMSG1(TBN_ENDADJUST    )
    DMSG1(TBN_RESET        )
    DMSG1(TBN_QUERYINSERT  )
    DMSG1(TBN_QUERYDELETE  )
    DMSG1(TBN_TOOLBARCHANGE)
    DMSG1(TBN_CUSTHELP     )
    DMSG1(TBN_DROPDOWN     )
    DMSG1(TBN_GETOBJECT     )
    DMSG1(TBN_HOTITEMCHANGE )
    DMSG1(TBN_DRAGOUT       )
    DMSG1(TBN_DELETINGBUTTON)
    DMSG1(TBN_GETDISPINFOA  )
    DMSG1(TBN_GETDISPINFOW  )
    DMSG1(TBN_GETINFOTIPA   )
    DMSG1(TBN_GETINFOTIPW   )
    DMSG1(TBN_GETBUTTONINFOW)
    DMSG1(TBN_RESTORE      )
    DMSG1(TBN_SAVE         )
    DMSG1(TBN_INITCUSTOMIZE)
    DMSG1(LVN_ITEMCHANGING   )
    DMSG1(LVN_ITEMCHANGED    )
    DMSG1(LVN_INSERTITEM     )
    DMSG1(LVN_DELETEITEM     )
    DMSG1(LVN_DELETEALLITEMS )
    DMSG1(LVN_BEGINLABELEDITA)
    DMSG1(LVN_BEGINLABELEDITW)
    DMSG1(LVN_ENDLABELEDITA  )
    DMSG1(LVN_ENDLABELEDITW  )
    DMSG1(LVN_COLUMNCLICK    )
    DMSG1(LVN_BEGINDRAG      )
    DMSG1(LVN_BEGINRDRAG     )
    DMSG1(LVN_ODCACHEHINT)
    DMSG1(LVN_ODFINDITEMA)
    DMSG1(LVN_ODFINDITEMW)
    DMSG1(LVN_ITEMACTIVATE  )
    DMSG1(LVN_ODSTATECHANGED)
    DMSG1(LVN_HOTTRACK)
    DMSG1(LVN_GETDISPINFOA)
    DMSG1(LVN_GETDISPINFOW)
    DMSG1(LVN_SETDISPINFOA)
    DMSG1(LVN_SETDISPINFOW)
    DMSG1(LVIF_DI_SETITEM)
    DMSG1(LVN_KEYDOWN)
    DMSG1(NM_OUTOFMEMORY)
    DMSG1(NM_CLICK    )
    DMSG1(NM_DBLCLK   )
    DMSG1(NM_RETURN   )
    DMSG1(NM_RCLICK   )
    DMSG1(NM_RDBLCLK  )
    DMSG1(NM_SETFOCUS )
    DMSG1(NM_KILLFOCUS)
    DMSG1(NM_CUSTOMDRAW)
    DMSG1(NM_HOVER     )
    DMSG1(NM_NCHITTEST      )
    DMSG1(NM_KEYDOWN        )
    DMSG1(NM_RELEASEDCAPTURE)
    DMSG1(NM_SETCURSOR      )
    DMSG1(NM_CHAR           )
    DMSG1(NM_TOOLTIPSCREATED)
    DMSG1(NM_LDOWN       )
    DMSG1(NM_RDOWN       )
    DMSG1(NM_THEMECHANGED)
    DMSG1(HDN_ITEMCHANGINGA   )
    DMSG1(HDN_ITEMCHANGINGW   )
    DMSG1(HDN_ITEMCHANGEDA    )
    DMSG1(HDN_ITEMCHANGEDW    )
    DMSG1(HDN_ITEMCLICKA      )
    DMSG1(HDN_ITEMCLICKW      )
    DMSG1(HDN_ITEMDBLCLICKA   )
    DMSG1(HDN_ITEMDBLCLICKW   )
    DMSG1(HDN_DIVIDERDBLCLICKA)
    DMSG1(HDN_DIVIDERDBLCLICKW)
    DMSG1(HDN_BEGINTRACKA     )
    DMSG1(HDN_BEGINTRACKW     )
    DMSG1(HDN_ENDTRACKA       )
    DMSG1(HDN_ENDTRACKW       )
    DMSG1(HDN_TRACKA          )
    DMSG1(HDN_TRACKW          )
    DMSG1(HDN_GETDISPINFOA)
    DMSG1(HDN_GETDISPINFOW)
    DMSG1(HDN_BEGINDRAG   )
    DMSG1(HDN_ENDDRAG     )
    DMSG1(HDN_FILTERCHANGE   )
    DMSG1(HDN_FILTERBTNCLICK )
    DMSG1(TTN_GETDISPINFOA)
    DMSG1(TTN_GETDISPINFOW)
    DMSG1(TTN_SHOW        )
    DMSG1(TTN_POP         )
    DMSG1(TTN_LINKCLICK   )
    //DMSG1(LVN_BEGINSCROLL)
    //DMSG1(LVN_ENDSCROLL)
};

void VerifyMsgs()
{
    ASSERT( Msgs[0].Message == 0 && Msgs[lengthof(Msgs)-1].Message == 0xFFFFFFFF);
    UINT Msg = 0;
    for ( unsigned int i = 1; i < lengthof(Msgs); ++i )
    {
        // Exception for WM_NULL
        if ( Msgs[i].Message <= Msg )
        {
            TCHAR Buffer[256];
            wsprintf(Buffer, _T("%s(%d) is not at its place"), Msgs[i].String, Msgs[i].Message);
            MessageBox(NULL, Buffer, NULL, 0);
        }
        Msg = Msgs[i].Message;
    }
}

MessageCracker * FindMsg(UINT Msg)
{
    // O(log(N))
    MessageCracker * pMsgBgn = Msgs;
    MessageCracker * pMsgEnd = Msgs + lengthof(Msgs) - 1;
    MessageCracker * pMsg = pMsgBgn + (pMsgEnd-pMsgBgn+1)/2;
    while ( pMsgBgn!=pMsgEnd && pMsg->Message!=Msg )
    {
        if ( pMsg->Message > Msg && pMsgEnd != pMsg)
            pMsgEnd = pMsg;
        else
            pMsgBgn = pMsg;
        pMsg = pMsgBgn + (pMsgEnd-pMsgBgn+1)/2;
    }
    return pMsg;
}

// Search to do the message log
void MsgLog(UINT Msg, WPARAM wParam, LPARAM lParam, HWND hWnd)
{
    static bool s_VerifyOnFirstPass = 0;
    if (!s_VerifyOnFirstPass)
    {
        s_VerifyOnFirstPass = true;
        VerifyMsgs();
    }

    const MessageCracker * pMsg = FindMsg(Msg);
    if (pMsg->Message == Msg)
        pMsg->fn(hWnd, Msg, pMsg->String, wParam, lParam);
    else
        DbgPrintf(( _T("0x%06X 0x%04X               0x%08X 0x%08X\n"), hWnd, Msg, wParam, lParam));
}

// Search to do the WM_NOTIFY log
void WM_NOTIFY_Cracker(HWND hWnd, UINT, const TCHAR * String, WPARAM wParam, LPARAM lParam)
{
    // O(N)
    const UINT Code = ((LPNMHDR)lParam)->code;
    const NotifyCracker * const pMsgEnd = Notify + lengthof(Notify);
    for (const NotifyCracker * itr = Notify; itr != pMsgEnd; ++itr )
    {
        if (itr->Code == Code)
        {
            itr->fn(hWnd, Code, itr->String, (UINT)wParam);
            return;
        }
    }
    DbgPrintf(( _T("0x%06X %-20s idCtrl:%-5d code:%d\n"), hWnd, String, wParam, Code));
}

void MsgLogDisable(UINT Msg)
{
    MessageCracker * pMsg = FindMsg(Msg);
    if (pMsg->Message == Msg)
        pMsg->fn = SkipMsgCracker;
}

#pragma optimize("", on)
#endif