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
// Term5250.h : Declaration of the CTerm5250
//
// You can get the charmaps at http://std.dkuug.dk/i18n/charmaps/
// or http://std.dkuug.dk/i18n/charmaps.646/
// Page: EBCDIC-US
// Bound to a site.
// http://support.microsoft.com/default.aspx?scid=kb;en-us;196061
// Packaging ATL for distribution over internet
// http://support.microsoft.com/default.aspx?scid=kb;en-us;249233

#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "A5250.h"
#include "Term.h"
#include "Term5250CP.h"

extern "C"
{
#define _TN5250_TERMINAL_PRIVATE_DEFINED
#include "tn5250-private.h"
}

// CTerm5250
struct ATL_NO_VTABLE CTerm5250 : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CStockPropImpl<CTerm5250, ITerm5250>,
    public IPersistStreamInitImpl<CTerm5250>,
    public IOleControlImpl<CTerm5250>,
    public IOleObjectImpl<CTerm5250>,
    public IOleInPlaceActiveObjectImpl<CTerm5250>,
    public IViewObjectExImpl<CTerm5250>,
    public IOleInPlaceObjectWindowlessImpl<CTerm5250>,
    public IConnectionPointContainerImpl<CTerm5250>,
    public CProxy_ITerm5250Events<CTerm5250>,
    public CComCoClass<CTerm5250, &CLSID_Term5250>,
    public CComControl<CTerm5250>,
    public IObjectSafetyImpl<CTerm5250, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
    public ISupportErrorInfo,
    public IProvideClassInfo2Impl<&CLSID_Term5250, &DIID__ITerm5250Events, &LIBID_Term5250Lib>
    //  public IPropertyNotifySinkCP<CTerm5250>
    //  public IPersistStorageImpl<CTerm5250>,
    //  public IDataObjectImpl<CTerm5250>,
    //  public ISpecifyPropertyPagesImpl<CTerm5250>,
    //  public IQuickActivateImpl<CTerm5250>, // must not use...
{
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
        COM_INTERFACE_ENTRY(IPersistStreamInit)
        COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
        COM_INTERFACE_ENTRY(IConnectionPointContainer)
        COM_INTERFACE_ENTRY(IObjectSafety)
        COM_INTERFACE_ENTRY(ISupportErrorInfo)
        COM_INTERFACE_ENTRY(IProvideClassInfo)
        COM_INTERFACE_ENTRY(IProvideClassInfo2)
        //COM_INTERFACE_ENTRY(IQuickActivate)           // Must not use
        //COM_INTERFACE_ENTRY(IPersistStorage)
        //COM_INTERFACE_ENTRY(IDataObject)
        //COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
    END_COM_MAP()

    // Persistent properties
    BEGIN_PROP_MAP(CTerm5250)
        PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
        PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
        PROP_ENTRY("Appearance", DISPID_APPEARANCE, CLSID_NULL)
        PROP_ENTRY("AutoSize", DISPID_AUTOSIZE, CLSID_NULL)
        PROP_ENTRY("BackColor", DISPID_BACKCOLOR, CLSID_StockColorPage)
        PROP_ENTRY("BorderColor", DISPID_BORDERCOLOR, CLSID_StockColorPage)
        PROP_ENTRY("BorderVisible", DISPID_BORDERVISIBLE, CLSID_NULL)
        PROP_ENTRY("BorderWidth", DISPID_BORDERWIDTH, CLSID_NULL)
        PROP_ENTRY("Caption", DISPID_CAPTION, CLSID_NULL)
        PROP_ENTRY("Enabled", DISPID_ENABLED, CLSID_NULL)
        PROP_ENTRY("ForeColor", DISPID_FORECOLOR, CLSID_StockColorPage)
        PROP_ENTRY("HostName", 1, CLSID_NULL)
        PROP_ENTRY("MouseMoveCursor", 2, CLSID_NULL)
        PROP_ENTRY("CaretStyle", 3, CLSID_NULL)
        PROP_ENTRY("SelectionEnabled", 5, CLSID_NULL)
        PROP_ENTRY("HotSpot", 6, CLSID_NULL)
        // Ex: PROP_PAGE(CLSID_StockColorPage)
    END_PROP_MAP()

    BEGIN_CONNECTION_POINT_MAP(CTerm5250)
        CONNECTION_POINT_ENTRY(__uuidof(_ITerm5250Events))
        //CONNECTION_POINT_ENTRY(DIID__ITerm5250Events)
        //      CONNECTION_POINT_ENTRY(IID_IPropertyNotifySink)
    END_CONNECTION_POINT_MAP()

    // Message handler (only when the controls has a window, i.e. not windowless)
    BEGIN_MSG_MAP(CTerm5250)
        MESSAGE_HANDLER(WM_TN5250_STREAM_DATA, ForwardMsg)
        MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
        MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
        //MESSAGE_HANDLER(WM_SIZE, OnSize2)
        //MESSAGE_HANDLER(WM_NCCALCSIZE, OnCalcSize)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)

        // In case somebody do a SendMessage() on us.
        // Same as PreTranslateAccelerator dispatcher
        MESSAGE_HANDLER(WM_SYSKEYUP, OnKeyUp)
        MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
        MESSAGE_HANDLER(WM_SYSKEYDOWN, OnKeyDown)
        MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
        MESSAGE_HANDLER(WM_CHAR, OnChar)

        // To be sure we get focus when needed and mouse selection
        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
        //MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
        MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)

        //MESSAGE_HANDLER(WM_CREATE, OnCreate)
        //WM_CANCELMODE 
        CHAIN_MSG_MAP(CComControl<CTerm5250>)
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()

    // Handlers
    LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    //LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    //LRESULT OnSize2(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT ForwardMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnDoNothing(UINT, WPARAM, LPARAM, BOOL&) { return 0; }
    LRESULT OnMouseActivate(UINT, WPARAM, LPARAM, BOOL&);
    //LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&);

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);
    // IViewObjectEx
    DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)


    // ITerm5250
    // methods
    STDMETHOD(Connect)();
    STDMETHOD(Disconnect)();
    STDMETHOD(SendVKey)(DWORD VKey);
    STDMETHOD(SendChar)(USHORT Character);

    // properties (stock)
    void OnAutoSizeChanged();
    void OnAppearanceChanged();
    void OnBackColorChanged();
    void OnBorderColorChanged();
    void OnBorderVisibleChanged();
    void OnBorderWidthChanged() { FUNC_ENTER(); m_nBorderWidth = 1; }
    void OnCaptionChanged();
    void OnEnabledChanged();
    void OnForeColorChanged();
    // custom properties
    STDMETHOD(get_HostName)(BSTR* pVal);
    STDMETHOD(put_HostName)(BSTR newVal);
    STDMETHOD(get_MouseMoveCursor)(VARIANT_BOOL* pVal);
    STDMETHOD(put_MouseMoveCursor)(VARIANT_BOOL newVal);
    STDMETHOD(get_CaretStyle)(ECaretStyle* pVal);
    STDMETHOD(put_CaretStyle)(ECaretStyle newVal);
    STDMETHOD(get_Colors)(OLE_COLOR *[10]);         // TBR
    STDMETHOD(put_Colors)(const OLE_COLOR [10]);    // TBR
    STDMETHOD(get_SelectionEnabled)(VARIANT_BOOL* pVal);
    STDMETHOD(put_SelectionEnabled)(VARIANT_BOOL newVal);
    STDMETHOD(get_HotSpot)(VARIANT_BOOL* pVal);
    STDMETHOD(put_HotSpot)(VARIANT_BOOL newVal);
    STDMETHOD(get_About)(BSTR* pVal);


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
    HRESULT IOleObject_SetExtent(DWORD dwDrawAspect, SIZEL *psizel);
    HRESULT IOleInPlaceObject_SetObjectRects(LPCRECT prcPos, LPCRECT prcClip);

    // member callbacks
    void RefreshScreenBuffer(bool FireViewEvent = true);
    int  GetKey();
    const CRect & GetRect() const throw()       { return static_cast<const CRect&>(super::m_rcPos); }
    const CSize & GetFontSize() const throw()   { return m_FontSize; }
    void PostBufferRefresh() const throw();
    HANDLE GetTimerHandle() const throw()       { return m_hTimer; }
    int IsReady() const throw()                { return m_nReadyState; }

protected:
    bool CanEdit(int Id) throw();
    void ValueChanged(int Id) throw();
    void SetSize(int cx, int cy);
    void OnSize();
    void LoadTerminalFont();
    void QueueKey(int key);
    void RegenScreenBuffer();
    void DrawStatusLine();
    void DrawTextAttrib(int attr, const char *text, int len, int CaretX, int CaretY
        , Tn5250Win32Attribute *map = NULL, int AbsoluteOffsetX = 0, int AbsoluteOffsetY = 0
        ) const;
    void DrawTextAttrib(OLE_COLOR fg, OLE_COLOR bg, const char * text, int len, int CaretX, int CaretY, int flags = 0
        , Tn5250Win32Attribute *map = NULL, int AbsoluteOffsetX = 0, int AbsoluteOffsetY = 0) const;
    void MakeNewCaret();
    void MoveCaret(HDC hdc);    // TBM
    void HideCaret();
    void ExpandTextSelection();
    void CopyTextSelection();
    void PasteTextSelection();
    bool CalcMaxFontSize(const RECT & cr, int cols, int rows, SIZE & o_fontSize);
    int GetCols() const throw() { int cols = tn5250_display_width(display);        if (!cols) cols = 80; return cols; }
    int GetRows() const throw() { int rows = tn5250_display_height(display)+1; if (!rows) rows = 25; return rows; }
    static DWORD WINAPI ThreadFunc(LPVOID);
    void PointToCursorPos(const POINT & in_Pixels, POINT & out_CaretPos) const throw();
    // in characters
    CPoint GetCaretPos() const throw();
    void InternalFlush();
    COLORREF GetColor(int iColor) const throw();

    PRINTDLG * win32_get_printer_info(Tn5250Terminal *This);
    void win32_destroy_printer_info(Tn5250Terminal *This);
    void win32_print_screen(Tn5250Terminal *This, Tn5250Display *display);
    static void win32_do_terminal_update(HDC hdc, Tn5250Terminal *This,
        Tn5250Display *display, Tn5250Win32Attribute *map,
        int ox, int oy);


    // Variables
protected:
    // Stock properties.
    friend class ATL::CStockPropImpl;
    SHORT                   m_nAppearance;
    OLE_COLOR               m_clrBackColor;
    OLE_COLOR               m_clrBorderColor;
    BOOL                    m_bBorderVisible;
    LONG                    m_nBorderWidth;
    CComBSTR                m_bstrCaption;
    BOOL                    m_bEnabled;
    OLE_COLOR               m_clrForeColor;
    LONG                    m_nReadyState;

    // Other properties
    CComBSTR                m_HostName;
    ECaretStyle             m_CaretStyle;
    ECopyMode               m_CopyMode;    // TBA
    CString                 m_FontName;    // TBA

    Tn5250Config *          config;
    Tn5250Session *         sess;
    Tn5250Stream *          stream;
    Tn5250Display *         display;
    Tn5250Terminal          term;
    Tn5250Macro *           macro;
    DWORD                   m_ThreadId;
    CHandle                 m_hThread;
    HFONT                   m_hFont;
    // The all mighty screen buffer. The selection square is NOT in this buffer but redrawn at each frame.
    HBITMAP                 m_hScreenBMP;
    HDC                     m_hMemDC;
    HBITMAP                 m_hCaretBMP;
    HBRUSH                  m_hBackgroundBrush;
    EColSepStyle            m_ColSepStyle;
    HANDLE                  m_hTimer;
    OLE_COLOR               m_ColorList[A_5250_NB_COLORS];
    
    enum{ MAX_K_BUF_LEN = 64};
    CComAutoCriticalSection m_KbBufCS;
    DWORD                   m_KbBuf[MAX_K_BUF_LEN];     // FIFO Buffer
    int                     m_KbBuf_len;
    CSize                   m_FontSize;                 // In pixels
    CSize                   m_Last_size;                // In characters (with cy=height-1)
    CAutoVectorPtr<int>     m_pSpacing;

    CRect                   m_rctSelection;             // Current selection in pixels in our rect, 0 based, and not container's rect

    PRINTDLG *              m_PD;
    bool                    m_bLocalPrint;

    bool                    m_bHandledKey;
    bool                    m_bCaretOK;                 // TBA Check all the places where it is used and verify if it's useful.
    bool                    m_bResized;
    bool                    m_bSelecting;
    bool                    m_bSelected;
    bool                    m_bSelectionEnabled;
    bool                    m_bUnixLikeCopy;
    bool                    m_bHotSpots;
    bool                    m_bDisplayRuler;
    bool                    m_bMouseMoveCursor;
};


extern const Tn5250Win32Attribute attribute_map[];
extern const keystroke_to_msg keydown2msg[];
extern const keystroke_to_msg keyup2msg[];
extern const win32_key_map win_kb[];


// Callbacks
int  win32_terminal_flags(Tn5250Terminal * This);
void win32_terminal_update(Tn5250Terminal * This, Tn5250Display * display);
void win32_terminal_update_indicators(Tn5250Terminal * This, Tn5250Display * display);
int  win32_terminal_waitevent(Tn5250Terminal * This);
int  win32_terminal_getkey(Tn5250Terminal * This);
void win32_terminal_beep(Tn5250Terminal * This);


extern const wchar_t About[];


// Used to guard a section
class CAutoLock
{
public:
    CAutoLock( const CComCriticalSection & Lock )
        : m_Lock(Lock)
    {
        m_Lock.Lock();
    }
    ~CAutoLock()
    {
        m_Lock.Unlock();
    }
    CComCriticalSection m_Lock;
};
