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
// You can get the charmaps at http://std.dkuug.dk/i18n/charmaps/
// or http://std.dkuug.dk/i18n/charmaps.646/
// Page: EBCDIC-US
// Bound to a site.
// http://support.microsoft.com/default.aspx?scid=kb;en-us;196061
// Packaging ATL for distribution over internet
// http://support.microsoft.com/default.aspx?scid=kb;en-us;249233

#pragma once
#include "resource.h"       // main symbols
#include "A5250.h"
#include "KeysHandling.h"
#include "Term5250CP.h"
#include "MsgLog.h"
#include "TypeLib.h"
#include "FunkyMap.h"

extern "C"
{
#define _TN5250_TERMINAL_PRIVATE_DEFINED
#include "tn5250-private.h"
}

struct ITerminal
{
    virtual int GetKey() = 0;
    virtual int IsReady() const = 0;
    virtual HWND GetHwnd() const = 0;
    virtual void PostBufferRefresh(bool Regen = false) = 0;
};

#pragma warning(push)
#pragma warning(disable:4640) // '' : construction of local static object is not thread-safe

// The activex class
struct ATL_NO_VTABLE CTerm5250
    : public ATL::CComObjectRootEx<ATL::CComSingleThreadModel>
    , public ATL::CStockPropImpl<CTerm5250, ITerm5250>
    //, public ATL::IPersistStreamInitImpl<CTerm5250>
    , public ATL::IOleControlImpl<CTerm5250>
    , public ATL::IOleObjectImpl<CTerm5250>
    , public ATL::IOleInPlaceActiveObjectImpl<CTerm5250>
    , public ATL::IViewObjectExImpl<CTerm5250>
    , public ATL::IOleInPlaceObjectWindowlessImpl<CTerm5250>
    , public ATL::IConnectionPointContainerImpl<CTerm5250>
    , public CProxy_ITerm5250Events<CTerm5250>
    , public ATL::CComCoClass<CTerm5250, &CLSID_Term5250>
    , public ATL::CComControl<CTerm5250>
    , public ATL::IObjectSafetyImpl<CTerm5250, INTERFACESAFE_FOR_UNTRUSTED_CALLER>
    , public ISupportErrorInfo
    , public ATL::IProvideClassInfo2Impl<&CLSID_Term5250, &DIID__ITerm5250Events, &LIBID_Term5250Lib>
    //, public ATL::IPropertyNotifySinkCP<CTerm5250>
    //, public ATL::IPersistStorageImpl<CTerm5250>
    , public ATL::IDataObjectImpl<CTerm5250>
    , public ATL::ISpecifyPropertyPagesImpl<CTerm5250>
    , public ITerminal
{
    enum
    {
        ItemSendResetSignal = 20100,
        ItemResetConnection,
        ItemConnect,
        ItemDisconnect,
        ItemSelectable,
        ItemMouseMove,
        ItemHotSpot,
        ItemDisplayRuler,
        ItemSetColors,
        ItemSetFont,
        ItemAbout,
        ItemCaretBlink,
        ItemCaretLine,
        ItemCaretNoBlink,
        ItemHelp
    };

#if _DEBUG
    EnumCache m_VKCache;
    EnumCache m_KCache;
#endif

public:
    DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE|OLEMISC_CANTLINKINSIDE|OLEMISC_INSIDEOUT|OLEMISC_ACTIVATEWHENVISIBLE|OLEMISC_SETCLIENTSITEFIRST)
    DECLARE_REGISTRY_RESOURCEID(IDR_TERM5250)

    // Supported interfaces
    BEGIN_COM_MAP(CTerm5250)
        COM_INTERFACE_ENTRY(ITerm5250)
        COM_INTERFACE_ENTRY(IDispatch)
        COM_INTERFACE_ENTRY(IViewObjectEx)
        COM_INTERFACE_ENTRY(IViewObject2)
        COM_INTERFACE_ENTRY(IViewObject)
        COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
        COM_INTERFACE_ENTRY(IOleInPlaceObject)
        COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
        COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
        COM_INTERFACE_ENTRY(IOleControl)
        COM_INTERFACE_ENTRY(IOleObject)
        //COM_INTERFACE_ENTRY(IPersistStreamInit)
        //COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
        COM_INTERFACE_ENTRY(IConnectionPointContainer)
        COM_INTERFACE_ENTRY(IObjectSafety)
        COM_INTERFACE_ENTRY(ISupportErrorInfo)
        COM_INTERFACE_ENTRY(IProvideClassInfo)
        COM_INTERFACE_ENTRY(IProvideClassInfo2)
        //COM_INTERFACE_ENTRY(IPersistStorage)
        COM_INTERFACE_ENTRY(IDataObject)
        COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
    END_COM_MAP()

    // Persistent properties
    BEGIN_PROP_MAP(CTerm5250)
        PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
        PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
        PROP_ENTRY("Appearance", DISPID_APPEARANCE, CLSID_NULL)   // VT_UI4
        PROP_ENTRY("BackColor", DISPID_BACKCOLOR, CLSID_StockColorPage)
        PROP_ENTRY("BorderColor", DISPID_BORDERCOLOR, CLSID_StockColorPage)
        PROP_ENTRY("BorderVisible", DISPID_BORDERVISIBLE, CLSID_NULL) // VT_BOOL
        PROP_ENTRY("BorderWidth", DISPID_BORDERWIDTH, CLSID_NULL) // VT_UI4
        PROP_ENTRY("Caption", DISPID_CAPTION, CLSID_NULL) // VT_BSTR
        PROP_ENTRY("Enabled", DISPID_ENABLED, CLSID_NULL) // VT_BOOL
        PROP_ENTRY("ForeColor", DISPID_FORECOLOR, CLSID_StockColorPage)
        PROP_ENTRY("HostName", 1, CLSID_NULL) // VT_BSTR
        PROP_ENTRY("MouseMoveCursor", 2, CLSID_NULL) // VT_BOOL
        PROP_ENTRY("CaretStyle", 3, CLSID_NULL)
        // 4 = Color
        PROP_ENTRY("SelectionEnabled", 5, CLSID_NULL) // VT_BOOL
        PROP_ENTRY("HotSpot", 6, CLSID_NULL) // VT_BOOL
        // 7 = About
        PROP_ENTRY("Enhanced", 8, CLSID_NULL) // VT_BOOL
        PROP_ENTRY("ContextualMenu", 9, CLSID_NULL) // VT_BOOL
        PROP_ENTRY("ShowConnect", 10, CLSID_NULL) // VT_BOOL
        PROP_ENTRY("DisplayRuler", 11, CLSID_NULL) // VT_BOOL
        PROP_ENTRY("FontName", 12, CLSID_NULL) // VT_BSTR
        PROP_ENTRY("DeviceName", 13, CLSID_NULL) // VT_BSTR
        PROP_ENTRY("CodePage", 14, CLSID_NULL) // VT_UI4
        PROP_ENTRY("Wide", 15, CLSID_NULL) // VT_BOOL
        PROP_ENTRY("UseComputerName", 16, CLSID_NULL)
        // Ex: PROP_PAGE(CLSID_StockColorPage)
    END_PROP_MAP()

    BEGIN_CONNECTION_POINT_MAP(CTerm5250)
        CONNECTION_POINT_ENTRY(__uuidof(_ITerm5250Events))
        //CONNECTION_POINT_ENTRY(IID_IPropertyNotifySink)
    END_CONNECTION_POINT_MAP()

    // Message handler (only when the controls has a window, i.e. not windowless)
    BEGIN_MSG_MAP(thisClass)
        //MsgLog(uMsg, wParam, lParam);
        MESSAGE_HANDLER(WM_TN5250_STREAM_DATA, ForwardMsg)

        // So the screen don't fliker
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        // WM_PAINT is handled by ATL::CComControl<>
        MESSAGE_HANDLER(WM_TIMER, OnTimer)  // Used to postpone drawing

        // There is no other way to know that our container lost the focus
        MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
        MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
        MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)

        // In case somebody do a SendMessage() on us.
        // Same as PreTranslateAccelerator dispatcher
        MESSAGE_HANDLER(WM_SYSKEYUP, OnKeyUp)
        MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
        MESSAGE_HANDLER(WM_SYSKEYDOWN, OnKeyDown)
        MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
        MESSAGE_HANDLER(WM_CHAR, OnChar)

        // Mouse selection and contextual menu (plus Reset Signal)
        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
        MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
        MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)


        // So we can use F1 !
        MESSAGE_HANDLER(WM_HELP, OnHelpCommand)
        COMMAND_ID_HANDLER(ItemSendResetSignal, OnSendResetSignal)
        COMMAND_ID_HANDLER(ItemResetConnection, OnResetConnection)
        COMMAND_ID_HANDLER(ItemConnect,         OnConnect)
        COMMAND_ID_HANDLER(ItemDisconnect,      OnDisconnect)
        COMMAND_ID_HANDLER(ItemSelectable,      OnToggleSelectable)
        COMMAND_ID_HANDLER(ItemMouseMove,       OnMouseMove)
        COMMAND_ID_HANDLER(ItemHotSpot,         OnHotSpot)
        COMMAND_ID_HANDLER(ItemDisplayRuler,    OnItemDisplayRuler)
        COMMAND_ID_HANDLER(ItemSetColors,       OnSetColors)
        COMMAND_ID_HANDLER(ItemSetFont,         OnItemSetFont)
        COMMAND_ID_HANDLER(ItemAbout,           OnAbout)
        COMMAND_ID_HANDLER(ItemCaretBlink,      OnItemCaretBlink)
        COMMAND_ID_HANDLER(ItemCaretLine,       OnItemCaretLine)
        COMMAND_ID_HANDLER(ItemCaretNoBlink,    OnItemCaretNoBlink)
        COMMAND_ID_HANDLER(ItemHelp,            OnItemHelp)
        CHAIN_MSG_MAP(ATL::CComControl<CTerm5250>)
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()

    // Handlers
    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT ForwardMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnDoNothing(UINT, WPARAM, LPARAM, BOOL&) { return 0; }
    LRESULT OnSendResetSignal(WORD wCode, WORD wId, HWND hCtl, BOOL & bHandled);
    LRESULT OnResetConnection(WORD wCode, WORD wId, HWND hCtl, BOOL & bHandled);
    LRESULT OnToggleSelectable(WORD wCode, WORD wId, HWND hCtl, BOOL & bHandled);
    LRESULT OnMouseMove(WORD wCode, WORD wId, HWND hCtl, BOOL & bHandled);
    LRESULT OnHotSpot(WORD wCode, WORD wId, HWND hCtl, BOOL & bHandled);
    LRESULT OnItemDisplayRuler(WORD wCode, WORD wId, HWND hCtl, BOOL & bHandled);
    LRESULT OnSetColors(WORD wCode, WORD wId, HWND hCtl, BOOL & bHandled);
    LRESULT OnItemSetFont(WORD wCode, WORD wId, HWND hCtl, BOOL & bHandled);
    LRESULT OnConnect(WORD wCode, WORD wId, HWND hCtl, BOOL & bHandled);
    LRESULT OnDisconnect(WORD wCode, WORD wId, HWND hCtl, BOOL & bHandled);
    LRESULT OnAbout(WORD wCode, WORD wId, HWND hCtl, BOOL & bHandled);
    LRESULT OnItemCaretBlink(WORD wCode, WORD wId, HWND hCtl, BOOL & bHandled);
    LRESULT OnItemCaretLine(WORD wCode, WORD wId, HWND hCtl, BOOL & bHandled);
    LRESULT OnItemCaretNoBlink(WORD wCode, WORD wId, HWND hCtl, BOOL & bHandled);
    LRESULT OnItemHelp(WORD wCode, WORD wId, HWND hCtl, BOOL & bHandled);

    // So F1 works. We don't support help.
    STDMETHOD(OnHelpCommand)(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)  { return 0; }

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);
    // IViewObjectEx
    DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)
    // IOleControl
	STDMETHOD(GetControlInfo)(CONTROLINFO * pCI);
	STDMETHOD(OnMnemonic)(MSG * pMsg);
    // IDataObject
	//STDMETHOD(GetData)(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);
	//STDMETHOD(GetDataHere)(FORMATETC* /* pformatetc */, STGMEDIUM* /* pmedium */);
	//STDMETHOD(QueryGetData)(FORMATETC* /* pformatetc */);
	STDMETHOD(GetCanonicalFormatEtc)(FORMATETC* /* pformatectIn */,FORMATETC* /* pformatetcOut */);
	//STDMETHOD(SetData)(FORMATETC* /* pformatetc */, STGMEDIUM* /* pmedium */, BOOL /* fRelease */);
	//STDMETHOD(EnumFormatEtc)(DWORD /* dwDirection */, IEnumFORMATETC** /* ppenumFormatEtc */);

    // ITerm5250
    // methods
    STDMETHOD(Connect)();
    STDMETHOD(Disconnect)();
    STDMETHOD(SendVKey)(EVirtualKeyCodes VKey, long Modifier);
    STDMETHOD(SendChar)(USHORT Character);
    STDMETHOD(BindKey)(EVirtualKeyCodes VKey, long Modifiers, long KValue);
    STDMETHOD(ConnectDialog)();

    // properties (stock)
    void OnAppearanceChanged();
    void OnBackColorChanged();
    void OnBorderColorChanged();
    void OnBorderVisibleChanged();
    void OnBorderWidthChanged();
    void OnCaptionChanged();
    void OnEnabledChanged();
    void OnForeColorChanged();
    // custom properties
    typedef CTerm5250 T;
    STDMETHOD(get_HostName)(BSTR * pVal);    // 1
    STDMETHOD(put_HostName)(BSTR newVal);
    IMPLEMENT_BOOL_STOCKPROP(MouseMoveCursor, bMouseMoveCursor, 2)
    void OnMouseMoveCursorChanged();
    IMPLEMENT_STOCKPROP(ECaretStyle, CaretStyle, eCaretStyle, 3)
    void OnCaretStyleChanged();
    STDMETHOD(get_Colors)(OLE_COLOR *[10]);         // 4 TBR
    STDMETHOD(put_Colors)(const OLE_COLOR [10]);    // TBR
    IMPLEMENT_BOOL_STOCKPROP(SelectionEnabled, bSelectionEnabled, 5)
    IMPLEMENT_BOOL_STOCKPROP(HotSpot, bHotSpots, 6)
    STDMETHOD(get_About)(BSTR * pVal);   // 7
    IMPLEMENT_BOOL_STOCKPROP(Enhanced, bEnhanced, 8)
    void OnEnhancedChanged();
    IMPLEMENT_BOOL_STOCKPROP(ContextualMenu, bContextualMenu, 9)
    IMPLEMENT_BOOL_STOCKPROP(ShowConnect, bShowConnect, 10)
    IMPLEMENT_BOOL_STOCKPROP(DisplayRuler, bDisplayRuler, 11)
    void OnDisplayRulerChanged();
    STDMETHOD(get_FontName)(BSTR * pVal);       // 12
    STDMETHOD(put_FontName)(BSTR newVal);
    STDMETHOD(get_DeviceName)(BSTR * pVal);     // 13
    STDMETHOD(put_DeviceName)(BSTR newVal);
    STDMETHOD(get_CodePage)(long * pVal);       // 14
    STDMETHOD(put_CodePage)(long newVal);
    STDMETHOD(get_Wide)(VARIANT_BOOL * pVal);   // 15
    STDMETHOD(put_Wide)(VARIANT_BOOL newVal);
    STDMETHOD(get_UseComputerName)(VARIANT_BOOL * pVal);    // 16
    STDMETHOD(put_UseComputerName)(VARIANT_BOOL newVal);
    STDMETHOD(get_CursorPositionX)(long * pVal);    // 17
    STDMETHOD(put_CursorPositionX)(long newVal);
    STDMETHOD(get_CursorPositionY)(long * pVal);    // 18
    STDMETHOD(put_CursorPositionY)(long newVal);
    STDMETHOD(get_ScreenWidth)(long * pVal);        // 19
    STDMETHOD(get_ScreenHeight)(long * pVal);       // 20
    STDMETHOD(ScreenScrape)(long Y, long X, long Length, BSTR * pString);

    // The rest
    CTerm5250() throw();
    ~CTerm5250() throw() { FUNC_ENTER(); }

    DECLARE_PROTECT_FINAL_CONSTRUCT()
    HRESULT FinalConstruct() throw();
    void    FinalRelease() throw();

    // Draw the control
    HRESULT OnDraw(ATL_DRAWINFO& di);
    // Overload
    BOOL PreTranslateAccelerator(LPMSG pMsg, HRESULT & hRet) throw();

    // Control resizing
    HRESULT IOleObject_SetExtent(DWORD dwDrawAspect, SIZEL * psizel);
    // Control caret appearence
    HRESULT IOleInPlaceObject_UIDeactivate();
    HRESULT OnPostVerbUIActivate();

    // member callbacks
    int  GetKey();
    CSize GetSize() const throw();
    const CSize & GetFontSize() const throw()   { return m_FontSize; }
    void PostBufferRefresh(bool Regen = false) throw();
    int IsReady() const throw()                 { return m_nReadyState; }
    int GetDelta() const throw()                { return m_bBorderVisible?m_nBorderWidth:0; }
    HWND GetHwnd() const throw()                { return m_hWnd; }
    void RefreshScreenBuffer(bool FireViewEvent = true);

protected:
    bool CanEdit(int Id) throw();
    void ValueChanged(int Id) throw();
    void SetSize(int cx, int cy);
    void SetSizeL(SIZEL & szlMetric);
    void GrabMaxSpace();
    void OnSize();
    bool CalcMaxFontSize(const SIZE & sz, int cols, int rows, SIZE & o_fontSize);
    bool LoadTerminalFont(SIZE & NewSize);
    void QueueKey(int key);
    void RegenScreenBuffer(bool Refresh = true);
    void RegenCaret();
    void DrawStatusLine();
    void DrawTextAttrib(int attr, const char *text, int len, int CaretX, int CaretY, Tn5250Win32Attribute *map = NULL) const;
    void DrawTextAttrib(OLE_COLOR fg, OLE_COLOR bg, const char * text, int len, int CaretX, int CaretY, int flags = 0, Tn5250Win32Attribute *map = NULL) const;
    void MakeCaret();
    void MoveCaret(HDC hdc);
    void HideCaret();
    void ExpandTextSelection();
    void CopyTextSelection();
    void PasteTextSelection();
    int GetCols() const throw();
    int GetRows() const throw();
    static DWORD WINAPI ThreadFunc(LPVOID);
    void PointToCursorPos(const POINT & in_Pixels, POINT & out_CaretPos) const throw();
    void CursorPosToPoint(const POINT & in_CaretPos, POINT & out_Pixels) const throw();
    // in characters
    CPoint GetCaretPos() const throw();
    char GetChar(int x, int y) const;
    void InternalFlush();
    COLORREF GetColor(int iColor) const throw();
    /* TBM If you want to print, load the activex in a web page and print from there!
    // It is MUCH simpler.
    PRINTDLG * win32_get_printer_info(Tn5250Terminal *This);
    void win32_destroy_printer_info(Tn5250Terminal *This);
    void win32_print_screen(Tn5250Terminal *This, Tn5250Display *display);
    static void win32_do_terminal_update(HDC hdc, Tn5250Terminal *This,
        Tn5250Display *display, Tn5250Win32Attribute *map, int ox, int oy); */


    // Variables
protected:
    // Stock properties.
    friend class ATL::CStockPropImpl;
    SHORT                   m_nAppearance;
    OLE_COLOR               m_clrBackColor;
    OLE_COLOR               m_clrBorderColor;
    BOOL                    m_bBorderVisible;
    LONG                    m_nBorderWidth;
    ATL::CComBSTR           m_bstrCaption;
    BOOL                    m_bEnabled;
    OLE_COLOR               m_clrForeColor;
    LONG                    m_nReadyState;

    // Other properties
    ATL::CComBSTR           m_HostName;
    ATL::CComBSTR           m_Device;
    long                    m_CodePage;
    ECaretStyle             m_eCaretStyle;
    ECopyMode               m_CopyMode;    // TBA or TBR :)
    CString                 m_FontName;

    Tn5250Config *          config;
    Tn5250Session *         sess;
    Tn5250Stream *          stream;
    Tn5250Display *         display;
    Tn5250Terminal          term;
    Tn5250Macro *           macro;
    DWORD                   m_ThreadId;
    ATL::CHandle            m_hThread;
    HFONT                   m_hFont;
    // The all mighty screen buffer. Selection nor caret are in this buffer. They are redrawn at each OnDraw.
    HBITMAP                 m_hScreenBMP;
    HDC                     m_hMemDC;
    HBITMAP                 m_hCaretBMP;
    HBRUSH                  m_hBackgroundBrush;
    EColSepStyle            m_ColSepStyle;
    enum { Ok, Redraw, Regen };
    LONG                    m_ScreenBufferState;
    OLE_COLOR               m_ColorList[A_5250_NB_COLORS];
    
    enum{ MAX_K_BUF_LEN = 64};
    ATL::CComAutoCriticalSection m_KbBufCS;
    DWORD                   m_KbBuf[MAX_K_BUF_LEN];     // FIFO Buffer
    int                     m_KbBuf_len;
    CSize                   m_FontSize;                 // In pixels
    CSize                   m_Last_size;                // In characters (with cy=height-1)
    ATL::CAutoVectorPtr<int> m_pSpacing;
    CRect                   m_rctSelection;             // Current selection in pixels in our rect, 0 based, and not container's rect

    typedef FunkyMap<Keystroke2MsgEx, int> VKey2Msg;
    VKey2Msg                m_Key2Msg;

    // TBM 
    /*Window*/int win;
    /*Window*/int subwindows[10];
    /*XFontStruct*/void * font_info;
    unsigned int borderwidth;	/* four pixels */
public:
    bool                    scrollbar;
protected:

    // TBA Printing support
    //PRINTDLG *              m_PD;
    //unsigned                m_bLocalPrint:1;
    unsigned                m_bHandledVirtualKey:1;
    unsigned                m_bSelecting:1;
    unsigned                m_bSelected:1;
    unsigned                m_bSelectionEnabled:1;
    unsigned                m_bUnixLikeCopy:1;
    unsigned                m_bHotSpots:1;
    unsigned                m_bDisplayRuler:1;
    unsigned                m_bMouseMoveCursor:1;
    unsigned                m_bEnhanced:1;
    unsigned                m_bContextualMenu:1;
    unsigned                m_bFirstClear:1;
    unsigned                m_bShowConnect:1;
    unsigned                m_bFocus:1;
    unsigned                m_bFontChanged:2;
    unsigned                m_bUseComputerName:1;
    unsigned                m_bWide:1;
};
#pragma warning(pop)


// Callbacks
int  win32_terminal_flags(Tn5250Terminal * This);
void win32_terminal_update(Tn5250Terminal * This, Tn5250Display * display);
void win32_terminal_update_indicators(Tn5250Terminal * This, Tn5250Display * display);
int  win32_terminal_waitevent(Tn5250Terminal * This);
int  win32_terminal_getkey(Tn5250Terminal * This);
void win32_terminal_beep(Tn5250Terminal * This);
void win32_terminal_destroy(Tn5250Terminal *);
int win32_terminal_enhanced(Tn5250Terminal *);
void win32_terminal_create_window(Tn5250Terminal * This, Tn5250Display * d, Tn5250Window * w);
void win32_terminal_destroy_window(Tn5250Terminal * This, Tn5250Display * d);
void win32_terminal_create_scrollbar (Tn5250Terminal * This, Tn5250Display * d, Tn5250Scrollbar * s);
void win32_terminal_destroy_scrollbar (Tn5250Terminal * This, Tn5250Display * d);

extern const char g_szAbout[];
extern const char * const k_BuildDate;
__declspec(selectany) const char * const k_BuildDate = __TIME__ " on " __DATE__;


// Used to guard a section
class CAutoLock
{
public:
    CAutoLock(volatile const ATL::CComCriticalSection & Lock )
        : m_Lock(const_cast<ATL::CComCriticalSection &>(Lock))
    {
        m_Lock.Lock();
    }
    ~CAutoLock()
    {
        m_Lock.Unlock();
    }
    ATL::CComCriticalSection & m_Lock;
};
