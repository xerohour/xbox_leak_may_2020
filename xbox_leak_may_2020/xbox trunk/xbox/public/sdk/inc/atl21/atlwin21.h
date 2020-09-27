// This is a part of the Active Template Library.
// Copyright (C) 1996-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLWIN21_H__
#define __ATLWIN21_H__

#ifndef __cplusplus
    #error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifdef __ATLWIN_H__
    #error atlwin21.h should be included instead of atlwin.h
#endif

#if (_ATL_VER < 0x0200) && (_ATL_VER >= 0x0300)
    #error atlwin21.h should be used only with ATL 2.0/2.1
#endif //(_ATL_VER < 0x0200) && (_ATL_VER >= 0x0300)

// Redefine class names and include old atlwin.h

#define CWindow     CWindowOld
#define _WndProcThunk   _WndProcThunkOld
#define _FuncDesc   _FuncDescOld
#define CWndProcThunk   CWndProcThunkOld
#define CWindowImplBase CWindowImplBaseOld
#define CWindowImpl CWindowImplOld
#define CDialogImplBase CDialogImplBaseOld
#define CDialogImpl CDialogImplOld

#include <atlwin.h>

#undef CWindow
#undef _FuncDesc
#undef _WndProcThunk
#undef _FuncDesc
#undef CWndProcThunk
#undef CWindowImplBase
#undef CWindowImpl
#undef CDialogImplBase
#undef CDialogImpl


#ifndef ATLASSERT
#define ATLASSERT(expr) _ASSERTE(expr)
#endif

#ifndef ATLTRACE2
#define ATLTRACE2(cat, lev, msg)    ATLTRACE(msg)
#endif

namespace ATL
{

#pragma pack(push, _ATL_PACKING)


/////////////////////////////////////////////////////////////////////////////
// CWindow - client side for a Windows window

class CWindow : public CWindowOld
{
public:
    static RECT rcDefault;

// Construction and creation
    CWindow(HWND hWnd = NULL)
    {
        m_hWnd = hWnd;
    }

    CWindow& operator=(HWND hWnd)
    {
        m_hWnd = hWnd;
        return *this;
    }

    HWND Create(LPCTSTR lpstrWndClass, HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
            DWORD dwStyle = 0, DWORD dwExStyle = 0,
            UINT nID = 0, LPVOID lpCreateParam = NULL)
    {
        m_hWnd = ::CreateWindowEx(dwExStyle, lpstrWndClass, szWindowName,
            dwStyle, rcPos.left, rcPos.top, rcPos.right - rcPos.left,
            rcPos.bottom - rcPos.top, hWndParent, (HMENU)(UINT_PTR)nID,
            _Module.GetModuleInstance(), lpCreateParam);
        return m_hWnd;
    }

    HWND Create(LPCTSTR lpstrWndClass, HWND hWndParent, LPRECT lpRect = NULL, LPCTSTR szWindowName = NULL,
            DWORD dwStyle = 0, DWORD dwExStyle = 0,
            HMENU hMenu = NULL, LPVOID lpCreateParam = NULL)
    {
        if(lpRect == NULL)
            lpRect = &rcDefault;
        m_hWnd = ::CreateWindowEx(dwExStyle, lpstrWndClass, szWindowName,
            dwStyle, lpRect->left, lpRect->top, lpRect->right - lpRect->left,
            lpRect->bottom - lpRect->top, hWndParent, hMenu,
            _Module.GetModuleInstance(), lpCreateParam);
        return m_hWnd;
    }

// Attributes
    operator HWND() const { return m_hWnd; }

    static LPCTSTR GetWndClassName()
    {
        return NULL;
    }

// Operations
    // support for C style macros
    static LRESULT SendMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        ATLASSERT(::IsWindow(hWnd));
        return ::SendMessage(hWnd, message, wParam, lParam);
    }

    // this one is here just so it's not hidden
    LRESULT SendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::SendMessage(m_hWnd, message, wParam, lParam);
    }

    BOOL GetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::GetScrollInfo(m_hWnd, nBar, lpScrollInfo);
    }
    BOOL SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw = TRUE)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::SetScrollInfo(m_hWnd, nBar, lpScrollInfo, bRedraw);
    }
    BOOL IsDialogMessage(LPMSG lpMsg)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::IsDialogMessage(m_hWnd, lpMsg);
    }

    void NextDlgCtrl() const
        {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, WM_NEXTDLGCTL, 0, 0L);
    }
    void PrevDlgCtrl() const
        {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, WM_NEXTDLGCTL, 1, 0L);
    }
    void GotoDlgCtrl(HWND hWndCtrl) const
        {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, WM_NEXTDLGCTL, (WPARAM)hWndCtrl, 1L);
    }

    BOOL ResizeClient(int nWidth, int nHeight, BOOL bRedraw = TRUE)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        RECT rcWnd;
        if(!GetClientRect(&rcWnd))
            return FALSE;

        if(nWidth != -1)
            rcWnd.right = nWidth;
        if(nHeight != -1)
            rcWnd.bottom = nHeight;

        if(!::AdjustWindowRectEx(&rcWnd, GetStyle(), (!(GetStyle() & WS_CHILD) && (GetMenu() != NULL)), GetExStyle()))
            return FALSE;

        UINT uFlags = SWP_NOZORDER | SWP_NOMOVE;
        if(!bRedraw)
            uFlags |= SWP_NOREDRAW;

        return SetWindowPos(NULL, 0, 0, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, uFlags);
    }

#ifndef UNDER_CE
    int GetWindowRgn(HRGN hRgn)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::GetWindowRgn(m_hWnd, hRgn);
    }
    int SetWindowRgn(HRGN hRgn, BOOL bRedraw = FALSE)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::SetWindowRgn(m_hWnd, hRgn, bRedraw);
    }
    HDWP DeferWindowPos(HDWP hWinPosInfo, HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT uFlags)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::DeferWindowPos(hWinPosInfo, m_hWnd, hWndInsertAfter, x, y, cx, cy, uFlags);
    }
#endif //!UNDER_CE
    DWORD GetWindowThreadID()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::GetWindowThreadProcessId(m_hWnd, NULL);
    }
    DWORD GetWindowProcessID()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        DWORD dwProcessID;
        ::GetWindowThreadProcessId(m_hWnd, &dwProcessID);
        return dwProcessID;
    }
    BOOL IsWindow()
    {
        return ::IsWindow(m_hWnd);
    }
#ifndef UNDER_CE
    BOOL IsWindowUnicode()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::IsWindowUnicode(m_hWnd);
    }
    BOOL ShowWindowAsync(int nCmdShow)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::ShowWindowAsync(m_hWnd, nCmdShow);
    }
#endif //!UNDER_CE
};

_declspec(selectany) RECT CWindow::rcDefault = { CW_USEDEFAULT, CW_USEDEFAULT, 0, 0 };

/////////////////////////////////////////////////////////////////////////////
// WindowProc thunks

#if defined(_M_PPC)
// For PPC we will stick the this pointer into r3, which is where
// the HWND is.  However, we don't actually need the HWND so this is OK.
#pragma pack(push,4)
struct _WndProcThunk //this should come out to 32 bytes
{
    DWORD entry;    //00000000: 00000000
    DWORD toc;      //00000004: 00000000
    DWORD lisr0;    //00000008: 3C000000 lis         r0,0
    DWORD orir0;    //0000000C: 60000000 ori         r0,r0,0
    DWORD mtctr;    //00000010: 7C0903A6 mtctr       r0
    DWORD lisr3;    //00000014: 3C600000 lis         r3,0
    DWORD orir3;    //00000018: 60630000 ori         r3,r3,0
    DWORD bctr;     //0000001C: 4E800420 bctr
};
#pragma pack(pop)
#elif defined (_M_ALPHA)
    #if defined(_WIN64)
        // For ALPHA we will stick the this pointer into a0, which is where
        // the HWND is.  However, we don't actually need the HWND so this is OK.
        #pragma pack(push,4)
        struct _WndProcThunk //this should come out to 36 bytes
        {
            DWORD ldah3_at;     //  ldah    at, LOWORD((func >> 32))
            DWORD sll_at;       //  sll     at, 32, at
            DWORD ldah_at;      //  ldah    at, HIWORD(func)
            DWORD lda_at;       //  lda     at, LOWORD(func)(at)
            DWORD ldah3_a0;     //  ldah    a0, LOWORD((this >> 32))
            DWORD sll_a0;       //  sll     a0, 32, a0
            DWORD ldah_a0;      //  ldah    a0, HIWORD(this)
            DWORD lda_a0;       //  lda     a0, LOWORD(this)(a0)
            DWORD jmp;          //  jmp     zero,(at),0
        };
        #pragma pack(pop)
    #else
        // For ALPHA we will stick the this pointer into a0, which is where
        // the HWND is.  However, we don't actually need the HWND so this is OK.
        #pragma pack(push,4)
        struct _WndProcThunk //this should come out to 20 bytes
        {
            DWORD ldah_at;      //  ldah    at, HIWORD(func)
            DWORD ldah_a0;      //  ldah    a0, HIWORD(this)
            DWORD lda_at;       //  lda     at, LOWORD(func)(at)
            DWORD lda_a0;       //  lda     a0, LOWORD(this)(a0)
            DWORD jmp;          //  jmp     zero,(at),0
        };
        #pragma pack(pop)
    #endif
#elif defined (_M_IX86)
#pragma pack(push,1)
struct _WndProcThunk
{
    DWORD   m_mov;          // mov dword ptr [esp+0x4], pThis (esp+0x4 is hWnd)
    DWORD   m_this;         //
    BYTE    m_jmp;          // jmp WndProc
    DWORD   m_relproc;      // relative jmp
};
#pragma pack(pop)
#elif defined(_SH3_)
#pragma pack(push,4)
struct _WndProcThunk // this should come out to 16 bytes
{
    WORD    m_mov_r0;       // mov.l    pFunc,r0
    WORD    m_mov_r1;       // mov.l    pThis,r1
    WORD    m_jmp;          // jmp      @r0
    WORD    m_nop;          // nop
    DWORD   m_pFunc;
    DWORD   m_pThis;
};
#pragma pack(pop)
#elif defined(_MIPS_)
#pragma pack(push,4)
struct _WndProcThunk
{
    WORD    m_pFuncHi;
    WORD    m_lui_t0;       // lui      t0,PFUNC_HIGH
    WORD    m_pFuncLo;
    WORD    m_ori_t0;       // ori      t0,t0,PFUNC_LOW
    WORD    m_pThisHi;
    WORD    m_lui_a0;       // lui      a0,PTHIS_HIGH
    DWORD   m_jr_t0;        // jr       t0
    WORD    m_pThisLo;
    WORD    m_ori_a0;       // ori      a0,PTHIS_LOW
};
#pragma pack(pop)
#elif defined (_M_IA64)
#pragma pack(push,8)
struct _FuncDesc
{
   void* pfn;
   void* gp;
};
struct _WndProcThunk
{
   _FuncDesc funcdesc;
   void* pRealWndProcDesc;
   void* pThis;
};
#pragma pack(pop)
#else
#error Only PPC, ALPHA, ALPHA64, SH3, MIPS, IA64, and X86 supported
#endif

class CWndProcThunk
{
public:
    union
    {
        _AtlCreateWndData cd;
        _WndProcThunk thunk;
    };
    void Init(WNDPROC proc, void* pThis)
    {
    #if defined(_M_PPC)
        DWORD* p = (DWORD*)proc;
        thunk.entry = (DWORD)&thunk.lisr0;                  //00000000: 00000000
        thunk.toc = p[1];                           //00000004: 00000000
        thunk.lisr0 = 0x3C000000 | HIWORD(p[0]);    //00000008: 3C000000 lis         r0,0
        thunk.orir0 = 0x60000000 | LOWORD(p[0]);    //0000000C: 60000000 ori         r0,r0,0
        thunk.mtctr = 0x7C0903A6;                   //00000010: 7C0903A6 mtctr       r0
        thunk.lisr3 = 0x3C600000 | HIWORD(pThis);   //00000014: 3C600000 lis         r3,0
        thunk.orir3 = 0x60630000 | LOWORD(pThis);   //00000018: 60630000 ori         r3,r3,0
        thunk.bctr =  0x4E800420;                   //0000001C: 4E800420 bctr
    #elif defined (_M_ALPHA)
        #ifdef _WIN64
            thunk.ldah3_at = 0x239f0000 | LOWORD(((LONG_PTR)proc >> 32) + (((HIWORD(proc)) + ((LOWORD(proc))>>15)) >> 15));
            thunk.sll_at = 0x04B84173c;
            thunk.ldah_at  = (0x279c0000 | HIWORD(proc)) + ((LOWORD(proc)>>15));
            thunk.lda_at = 0x239c0000 | LOWORD(proc);
            thunk.ldah3_a0 = 0x221f0000 | LOWORD(((LONG_PTR)pThis >> 32) + (((HIWORD(pThis)) + ((LOWORD(pThis))>>15)) >> 15));
            thunk.sll_a0 = 0x4A041730;
            thunk.ldah_a0  = (0x26100000 | HIWORD(pThis)) + (LOWORD(pThis)>>15);
            thunk.lda_a0 = 0x22100000 | LOWORD(pThis);
            thunk.jmp = 0x6bfc0000;
        #else
            thunk.ldah_at = (0x279f0000 | HIWORD(proc)) + (LOWORD(proc)>>15);
            thunk.ldah_a0 = (0x261f0000 | HIWORD(pThis)) + (LOWORD(pThis)>>15);
            thunk.lda_at = 0x239c0000 | LOWORD(proc);
            thunk.lda_a0 = 0x22100000 | LOWORD(pThis);
            thunk.jmp = 0x6bfc0000;
        #endif
    #elif defined (_M_IX86)
        thunk.m_mov = 0x042444C7;  //C7 44 24 0C
        thunk.m_this = (DWORD)pThis;
        thunk.m_jmp = 0xe9;
        thunk.m_relproc = (int)proc - ((int)this+sizeof(_WndProcThunk));
    #elif defined (_SH3_)
        thunk.m_mov_r0 = 0xd001;
        thunk.m_mov_r1 = 0xd402;
        thunk.m_jmp = 0x402b;
        thunk.m_nop = 0x0009;
        thunk.m_pFunc = (DWORD)proc;
        thunk.m_pThis = (DWORD)pThis;
    #elif defined (_MIPS_)
        thunk.m_pFuncHi = HIWORD(proc);
        thunk.m_lui_t0  = 0x3c08;
        thunk.m_pFuncLo = LOWORD(proc);
        thunk.m_ori_t0  = 0x3508;
        thunk.m_pThisHi = HIWORD(pThis);
        thunk.m_lui_a0  = 0x3c04;
        thunk.m_jr_t0   = 0x01000008;
        thunk.m_pThisLo = LOWORD(pThis);
        thunk.m_ori_a0  = 0x3484;
    #endif
        // write block from data cache and
        //  flush from instruction cache
        FlushInstructionCache(GetCurrentProcess(), &thunk, sizeof(thunk));
    }
};


/////////////////////////////////////////////////////////////////////////////
// New message map macros

// Empty message map macro

#define DECLARE_EMPTY_MSG_MAP() \
public: \
    BOOL ProcessWindowMessage(HWND, UINT, WPARAM, LPARAM, LRESULT&, DWORD) \
    { \
        return FALSE; \
    }

// Message reflection macros

#define REFLECT_NOTIFICATIONS() \
    { \
        bHandled = TRUE; \
        lResult = ReflectNotifications(uMsg, wParam, lParam, bHandled); \
        if(bHandled) \
            return TRUE; \
    }

#define DEFAULT_REFLECTION_HANDLER() \
    if(DefaultReflectionHandler(hWnd, uMsg, wParam, lParam, lResult)) \
        return TRUE;


/////////////////////////////////////////////////////////////////////////////
// CWinTraits - Defines various default values for a window

template <DWORD t_dwStyle = 0, DWORD t_dwExStyle = 0>
class CWinTraits
{
public:
    static DWORD GetWndStyle(DWORD dwStyle)
    {
        return dwStyle == 0 ? t_dwStyle : dwStyle;
    }
    static DWORD GetWndExStyle(DWORD dwExStyle)
    {
        return dwExStyle == 0 ? t_dwExStyle : dwExStyle;
    }
};

typedef CWinTraits<WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0>        CControlWinTraits;
#ifndef UNDER_CE
typedef CWinTraits<WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE>         CFrameWinTraits;
#else
typedef CWinTraits<WS_OVERLAPPED | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_CAPTION | WS_SYSMENU | WS_BORDER, WS_EX_WINDOWEDGE>   CFrameWinTraits;
#endif //!UNDER_CE
typedef CWinTraits<WS_OVERLAPPEDWINDOW | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_MDICHILD> CMDIChildWinTraits;

typedef CWinTraits<0, 0> CNullTraits;

template <DWORD t_dwStyle = 0, DWORD t_dwExStyle = 0, class TWinTraits = CControlWinTraits>
class CWinTraitsOR
{
public:
    static DWORD GetWndStyle(DWORD dwStyle)
    {
        return dwStyle | t_dwStyle | TWinTraits::GetWndStyle(dwStyle);
    }
    static DWORD GetWndExStyle(DWORD dwExStyle)
    {
        return dwExStyle | t_dwExStyle | TWinTraits::GetWndExStyle(dwExStyle);
    }
};


/////////////////////////////////////////////////////////////////////////////
// CWindowImpl - Implements a window

template <class TBase = CWindow>
class ATL_NO_VTABLE CWindowImplRoot : public TBase, public CMessageMap
{
public:
    CWndProcThunk m_thunk;

// Destructor
    ~CWindowImplRoot()
    {
        ATLASSERT(m_hWnd == NULL);  // should be cleared in WindowProc
    }

// Message reflection support
    LRESULT ReflectNotifications(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    static BOOL DefaultReflectionHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
};

template <class TBase>
LRESULT CWindowImplRoot< TBase >::ReflectNotifications(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    HWND hWndChild = NULL;

    switch(uMsg)
    {
    case WM_COMMAND:
        if(lParam != NULL)  // not from a menu
            hWndChild = (HWND)lParam;
        break;
    case WM_NOTIFY:
        hWndChild = ((LPNMHDR)lParam)->hwndFrom;
        break;
#ifndef UNDER_CE
    case WM_PARENTNOTIFY:
        switch(LOWORD(wParam))
        {
        case WM_CREATE:
        case WM_DESTROY:
            hWndChild = (HWND)lParam;
            break;
        default:
            hWndChild = GetDlgItem(HIWORD(wParam));
            break;
        }
        break;
#endif //!UNDER_CE
    case WM_DRAWITEM:
        if(wParam)  // not from a menu
            hWndChild = ((LPDRAWITEMSTRUCT)lParam)->hwndItem;
        break;
    case WM_MEASUREITEM:
        if(wParam)  // not from a menu
            hWndChild = GetDlgItem(((LPMEASUREITEMSTRUCT)lParam)->CtlID);
        break;
    case WM_COMPAREITEM:
        if(wParam)  // not from a menu
            hWndChild = GetDlgItem(((LPCOMPAREITEMSTRUCT)lParam)->CtlID);
        break;
    case WM_DELETEITEM:
        if(wParam)  // not from a menu
            hWndChild = GetDlgItem(((LPDELETEITEMSTRUCT)lParam)->CtlID);
        break;
    case WM_VKEYTOITEM:
    case WM_CHARTOITEM:
    case WM_HSCROLL:
    case WM_VSCROLL:
        hWndChild = (HWND)lParam;
        break;
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
        hWndChild = (HWND)lParam;
        break;
    default:
        break;
    }

    if(hWndChild == NULL)
    {
        bHandled = FALSE;
        return 1;
    }

    ATLASSERT(::IsWindow(hWndChild));
    return ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
}

template <class TBase>
BOOL CWindowImplRoot< TBase >::DefaultReflectionHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
    switch(uMsg)
    {
    case OCM_COMMAND:
    case OCM_NOTIFY:
#ifndef UNDER_CE
    case OCM_PARENTNOTIFY:
#endif //!UNDER_CE
    case OCM_DRAWITEM:
    case OCM_MEASUREITEM:
    case OCM_COMPAREITEM:
    case OCM_DELETEITEM:
    case OCM_VKEYTOITEM:
    case OCM_CHARTOITEM:
    case OCM_HSCROLL:
    case OCM_VSCROLL:
    case OCM_CTLCOLORBTN:
    case OCM_CTLCOLORDLG:
    case OCM_CTLCOLOREDIT:
    case OCM_CTLCOLORLISTBOX:
    case OCM_CTLCOLORMSGBOX:
    case OCM_CTLCOLORSCROLLBAR:
    case OCM_CTLCOLORSTATIC:
        lResult = ::DefWindowProc(hWnd, uMsg - OCM__BASE, wParam, lParam);
        return TRUE;
    default:
        break;
    }
    return FALSE;
}

template <class TBase = CWindow, class TWinTraits = CControlWinTraits>
class ATL_NO_VTABLE CWindowImplBaseT : public CWindowImplRoot< TBase >
{
public:
    WNDPROC m_pfnSuperWindowProc;

    CWindowImplBaseT() : m_pfnSuperWindowProc(::DefWindowProc)
    {}

    static DWORD GetWndStyle(DWORD dwStyle)
    {
        return TWinTraits::GetWndStyle(dwStyle);
    }
    static DWORD GetWndExStyle(DWORD dwExStyle)
    {
        return TWinTraits::GetWndExStyle(dwExStyle);
    }

    virtual WNDPROC GetWindowProc()
    {
        return WindowProc;
    }
    static LRESULT CALLBACK StartWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName,
            DWORD dwStyle, DWORD dwExStyle, UINT nID, ATOM atom, LPVOID lpCreateParam = NULL);
    BOOL DestroyWindow()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::DestroyWindow(m_hWnd);
    }
    BOOL SubclassWindow(HWND hWnd);
    HWND UnsubclassWindow(BOOL bForce = FALSE);

    LRESULT DefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
#ifdef STRICT
        return ::CallWindowProc(m_pfnSuperWindowProc, m_hWnd, uMsg, wParam, lParam);
#else
        return ::CallWindowProc((FARPROC)m_pfnSuperWindowProc, m_hWnd, uMsg, wParam, lParam);
#endif
    }

    virtual void OnFinalMessage(HWND /*hWnd*/)
    {
        // override to do something, if needed
    }
};

typedef CWindowImplBaseT<CWindow>   CWindowImplBase;

template <class TBase, class TWinTraits>
LRESULT CALLBACK CWindowImplBaseT< TBase, TWinTraits >::StartWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CWindowImplBaseT< TBase, TWinTraits >* pThis = (CWindowImplBaseT< TBase, TWinTraits >*)_Module.ExtractCreateWndData();
    ATLASSERT(pThis != NULL);
    pThis->m_hWnd = hWnd;
    pThis->m_thunk.Init(pThis->GetWindowProc(), pThis);
    WNDPROC pProc = (WNDPROC)&(pThis->m_thunk.thunk);
    WNDPROC pOldProc = (WNDPROC)::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)pProc);
#ifdef _DEBUG
    // check if somebody has subclassed us already since we discard it
    if(pOldProc != StartWindowProc)
        ATLTRACE(_T("ATL: Subclassing through a hook discarded.\n"));
#else
    pOldProc;   // avoid unused warning
#endif
    return pProc(hWnd, uMsg, wParam, lParam);
}

template <class TBase, class TWinTraits>
LRESULT CALLBACK CWindowImplBaseT< TBase, TWinTraits >::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CWindowImplBaseT< TBase, TWinTraits >* pThis = (CWindowImplBaseT< TBase, TWinTraits >*)hWnd;
    LRESULT lRes;
    if(pThis->ProcessWindowMessage(pThis->m_hWnd, uMsg, wParam, lParam, lRes, 0) == FALSE)
    {
#ifndef UNDER_CE
        if(uMsg != WM_NCDESTROY)
#else // CE specific
        if(uMsg != WM_DESTROY)
#endif //!UNDER_CE
            lRes = pThis->DefWindowProc(uMsg, wParam, lParam);
        else
        {
            // unsubclass, if needed
            LONG_PTR pfnWndProc = ::GetWindowLongPtr(pThis->m_hWnd, GWLP_WNDPROC);
            lRes = pThis->DefWindowProc(uMsg, wParam, lParam);
            if(pThis->m_pfnSuperWindowProc != ::DefWindowProc && ::GetWindowLongPtr(pThis->m_hWnd, GWLP_WNDPROC) == pfnWndProc)
                ::SetWindowLongPtr(pThis->m_hWnd, GWLP_WNDPROC, (LONG_PTR)pThis->m_pfnSuperWindowProc);
            // clear out window handle
            HWND hWnd = pThis->m_hWnd;
            pThis->m_hWnd = NULL;
            // clean up after window is destroyed
            pThis->OnFinalMessage(hWnd);
        }
    }
    return lRes;
}

template <class TBase, class TWinTraits>
HWND CWindowImplBaseT< TBase, TWinTraits >::Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName,
        DWORD dwStyle, DWORD dwExStyle, UINT nID, ATOM atom, LPVOID lpCreateParam)
{
    static LONG s_nNextChildID = 1;

    ATLASSERT(m_hWnd == NULL);

    if(atom == 0)
        return NULL;

    _Module.AddCreateWndData(&m_thunk.cd, this);

    if(nID == 0 && (dwStyle & WS_CHILD))
    {
#ifdef _WIN64
        nID = InterlockedIncrement( &s_nNextChildID );
        nID |= 0x80000000;
#else
        nID = (UINT)this;
#endif
    }

    HWND hWnd = ::CreateWindowEx(dwExStyle, (LPCTSTR)MAKELONG(atom, 0), szWindowName,
        dwStyle, rcPos.left, rcPos.top, rcPos.right - rcPos.left,
        rcPos.bottom - rcPos.top, hWndParent, (HMENU)nID,
        _Module.GetModuleInstance(), lpCreateParam);

    ATLASSERT(m_hWnd == hWnd);

    return hWnd;
}

template <class TBase, class TWinTraits>
BOOL CWindowImplBaseT< TBase, TWinTraits >::SubclassWindow(HWND hWnd)
{
    ATLASSERT(m_hWnd == NULL);
    ATLASSERT(::IsWindow(hWnd));
    m_thunk.Init(GetWindowProc(), this);
    WNDPROC pProc = (WNDPROC)&(m_thunk.thunk);
    WNDPROC pfnWndProc = (WNDPROC)::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)pProc);
    if(pfnWndProc == NULL)
        return FALSE;
    m_pfnSuperWindowProc = pfnWndProc;
    m_hWnd = hWnd;
    return TRUE;
}

// Use only if you want to subclass before window is destroyed,
// WindowProc will automatically subclass when  window goes away
template <class TBase, class TWinTraits>
HWND CWindowImplBaseT< TBase, TWinTraits >::UnsubclassWindow(BOOL bForce /*= FALSE*/)
{
    ATLASSERT(m_hWnd != NULL);

    WNDPROC pOurProc = (WNDPROC)&(m_thunk.thunk);
    WNDPROC pActiveProc = (WNDPROC)::GetWindowLongPtr(m_hWnd, GWLP_WNDPROC);

    HWND hWnd = NULL;
    if (bForce || pOurProc == pActiveProc)
    {
        if(!::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_pfnSuperWindowProc))
            return NULL;

        m_pfnSuperWindowProc = ::DefWindowProc;
        hWnd = m_hWnd;
        m_hWnd = NULL;
    }
    return hWnd;
}

template <class T, class TBase = CWindow, class TWinTraits = CControlWinTraits>
class ATL_NO_VTABLE CWindowImpl : public CWindowImplBaseT< TBase, TWinTraits >
{
public:
    DECLARE_WND_CLASS(NULL)

    HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
            DWORD dwStyle = 0, DWORD dwExStyle = 0,
            UINT nID = 0, LPVOID lpCreateParam = NULL)
    {
        if (T::GetWndClassInfo().m_lpszOrigName == NULL)
            T::GetWndClassInfo().m_lpszOrigName = GetWndClassName();
        ATOM atom = T::GetWndClassInfo().Register(&m_pfnSuperWindowProc);

        dwStyle = T::GetWndStyle(dwStyle);
        dwExStyle = T::GetWndExStyle(dwExStyle);

        return CWindowImplBaseT< TBase, TWinTraits >::Create(hWndParent, rcPos, szWindowName,
            dwStyle, dwExStyle, nID, atom, lpCreateParam);
    }
};

/////////////////////////////////////////////////////////////////////////////
// CDialogImpl - Implements a dialog box

template <class TBase = CWindow>
class ATL_NO_VTABLE CDialogImplBaseT : public CWindowImplRoot< TBase >
{
public:
    virtual WNDPROC GetDialogProc()
    {
        return DialogProc;
    }
    static LRESULT CALLBACK StartDialogProc(HWND hWnd, UINT uMsg,
        WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    BOOL MapDialogRect(LPRECT lpRect)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::MapDialogRect(m_hWnd, lpRect);
    }
    virtual void OnFinalMessage(HWND /*hWnd*/)
    {
        // override to do something, if needed
    }
    // has no meaning for a dialog, but needed for handlers that use it
    LRESULT DefWindowProc()
    {
        return 0;
    }
};

template <class TBase>
LRESULT CALLBACK CDialogImplBaseT< TBase >::StartDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CDialogImplBaseT< TBase >* pThis = (CDialogImplBaseT< TBase >*)_Module.ExtractCreateWndData();
    ATLASSERT(pThis != NULL);
    pThis->m_hWnd = hWnd;
    pThis->m_thunk.Init(pThis->GetDialogProc(), pThis);
    WNDPROC pProc = (WNDPROC)&(pThis->m_thunk.thunk);
    WNDPROC pOldProc = (WNDPROC)::SetWindowLongPtr(hWnd, DWLP_DLGPROC, (LONG_PTR)pProc);
#ifdef _DEBUG
    // check if somebody has subclassed us already since we discard it
    if(pOldProc != StartDialogProc)
        ATLTRACE(_T("ATL: Subclassing through a hook discarded.\n"));
#endif
    return pProc(hWnd, uMsg, wParam, lParam);
}

template <class TBase>
LRESULT CALLBACK CDialogImplBaseT< TBase >::DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CDialogImplBaseT< TBase >* pThis = (CDialogImplBaseT< TBase >*)hWnd;
    LRESULT lRes;
    if(pThis->ProcessWindowMessage(pThis->m_hWnd, uMsg, wParam, lParam, lRes, 0))
    {
        switch (uMsg)
        {
        case WM_COMPAREITEM:
        case WM_VKEYTOITEM:
        case WM_CHARTOITEM:
        case WM_INITDIALOG:
        case WM_QUERYDRAGICON:
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
            return lRes;
            break;
        }
        ::SetWindowLongPtr(pThis->m_hWnd, DWLP_MSGRESULT, lRes);
        return TRUE;
    }
#ifndef UNDER_CE
    if(uMsg == WM_NCDESTROY)
#else // CE specific
    if(uMsg == WM_DESTROY)
#endif //!UNDER_CE
    {
        // clear out window handle
        HWND hWnd = pThis->m_hWnd;
        pThis->m_hWnd = NULL;
        // clean up after dialog is destroyed
        pThis->OnFinalMessage(hWnd);
    }
    return FALSE;
}

typedef CDialogImplBaseT<CWindow>   CDialogImplBase;

template <class T, class TBase = CWindow>
class ATL_NO_VTABLE CDialogImpl : public CDialogImplBaseT< TBase >
{
public:
#ifdef _DEBUG
    bool m_bModal;
    CDialogImpl() : m_bModal(false) { }
#endif //_DEBUG
    // modal dialogs
    INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow(), LPARAM dwInitParam = NULL)
    {
        ATLASSERT(m_hWnd == NULL);
        _Module.AddCreateWndData(&m_thunk.cd, (CDialogImplBaseT< TBase >*)this);
#ifdef _DEBUG
        m_bModal = true; // set to true for _DEBUG only
#endif // _DEBUG
        return ::DialogBoxParam(_Module.GetResourceInstance(), MAKEINTRESOURCE(T::IDD),
                    hWndParent, (DLGPROC)T::StartDialogProc, dwInitParam);
    }
    BOOL EndDialog(int nRetCode)
    {
        ATLASSERT(::IsWindow(m_hWnd));
#ifdef _DEBUG
        ATLASSERT(m_bModal);    // must be a modal dialog
#endif // _DEBUG
        return ::EndDialog(m_hWnd, nRetCode);
    }
    // modeless dialogs
    HWND Create(HWND hWndParent, LPARAM dwInitParam = NULL)
    {
        ATLASSERT(m_hWnd == NULL);
        _Module.AddCreateWndData(&m_thunk.cd, (CDialogImplBaseT< TBase >*)this);
#ifdef _DEBUG
        m_bModal = false; // set to false for _DEBUG only
#endif // _DEBUG
        HWND hWnd = ::CreateDialogParam(_Module.GetResourceInstance(), MAKEINTRESOURCE(T::IDD),
                    hWndParent, (DLGPROC)T::StartDialogProc, dwInitParam);
        ATLASSERT(m_hWnd == hWnd);
        return hWnd;
    }
    // for CComControl
    HWND Create(HWND hWndParent, RECT&, LPARAM dwInitParam = NULL)
    {
        return Create(hWndParent, dwInitParam);
    }
    BOOL DestroyWindow()
    {
        ATLASSERT(::IsWindow(m_hWnd));
#ifdef _DEBUG
        ATLASSERT(!m_bModal);   // must not be a modal dialog
#endif // _DEBUG
        return ::DestroyWindow(m_hWnd);
    }
};

}; //namespace ATL

#endif // __ATLWIN21_H__
