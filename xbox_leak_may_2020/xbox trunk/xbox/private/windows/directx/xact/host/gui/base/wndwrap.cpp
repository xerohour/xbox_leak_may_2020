/***************************************************************************
 *
 *  Copyright (C) 11/8/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       wndwrap.cpp
 *  Content:    Windows wrapper classes (aka My Mini MFC)
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/8/2001   dereks  Created.
 *
 ****************************************************************************/

#include "xactgui.h"

const HINSTANCE g_hInstance = GetModuleHandle(NULL);

TCHAR g_szAppTitle[0x100] = { 0 };

const DWORD CWindow::m_dwValidSignature = ' dnw';

#ifndef WNDWRAP_USE_SET_WINDOW_LONG

LIST_ENTRY CWindow::m_lstWindows = { 0 };

#endif // WNDWRAP_USE_SET_WINDOW_LONG


/****************************************************************************
 *
 *  CWindow
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      UINT [in]: menu resource identifier, or 0.
 *      UINT [in]: accelerator resource identifier, or 0.
 *      UINT [in]: window type.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWindow::CWindow"

CWindow::CWindow
(
    UINT                    nMenuId,
    UINT                    nAccelId,
    UINT                    nWindowType
)
:   m_dwSignature(m_dwValidSignature),
    m_nWindowType(nWindowType)
{

#ifndef WNDWRAP_USE_SET_WINDOW_LONG

    if(!m_lstWindows.Flink || !m_lstWindows.Blink)
    {
        ASSERT(!m_lstWindows.Flink && !m_lstWindows.Blink);
        InitializeListHead(&m_lstWindows);
    }
    
    InitializeListHead(&m_leWindows);

#endif // WNDWRAP_USE_SET_WINDOW_LONG

    InitializeListHead(&m_lstChildren);
    InitializeListHead(&m_leChildren);

    m_pParent = NULL;
    m_hWnd = NULL;
    m_hMenu = NULL;
    m_hAccel = NULL;
    m_fRedraw = TRUE;

    if(-1 != nMenuId)
    {
        m_hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(nMenuId));
    }

    if(-1 != nAccelId)
    {
        m_hAccel = LoadAccelerators(g_hInstance, MAKEINTRESOURCE(nAccelId));
    }
}


/****************************************************************************
 *
 *  ~CWindow
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWindow::~CWindow"

CWindow::~CWindow
(
    void
)
{
    //
    // Destroy the window
    //
    
    Destroy();
}


/****************************************************************************
 *
 *  Create
 *
 *  Description:
 *      Creates a window.
 *
 *  Arguments:
 *      CWindow * [in]: parent window.
 *      LPCTSTR [in]: window class name.
 *      LPCTSTR [in]: window title.
 *      DWORD [in]: window extended style.
 *      DWORD [in]: window basic style.
 *      int [in]: window x-coordinate.
 *      int [in]: window y-coordinate.
 *      int [in]: window width.
 *      int [in]: window height.
 *      UINT [in]: child control identifier.
 *      LPVOID [in]: creation context.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWindow::Create"

BOOL
CWindow::Create
(
    CWindow *               pParent, 
    LPCTSTR                 pszClassName, 
    LPCTSTR                 pszWindowName, 
    DWORD                   dwExStyle, 
    DWORD                   dwStyle, 
    int                     x, 
    int                     y, 
    int                     nWidth, 
    int                     nHeight, 
    UINT                    nControlId,
    LPVOID                  pvParam
)
{
    WNDCLASSEX              wc  = { 0 };
    WINDOWCREATECTX         ctx;

    ASSERT(!m_pParent);
    m_pParent = pParent;

    //
    // Check to see if the window class uses our WindowProc to process
    // messages.  If it doesn't, pass the creation context directly to
    // CreateWindowEx instead of munging it.  We'll never get a WM_CREATE
    // message, so there's no point in changing the parameter. Also, some 
    // windows require specific data be passed to their WM_CREATE handler
    // (MDICLIENT for example).
    //

    wc.cbSize = sizeof(wc);
    
    if(GetClassInfoEx(g_hInstance, pszClassName, &wc))
    {
        if(WindowProc == wc.lpfnWndProc)
        {
            ctx.dwSignature = m_dwValidSignature;
            ctx.pWindow = this;
            ctx.pvContext = pvParam;

            pvParam = &ctx;
        }
    }
    
    if(!(m_hWnd = CreateWindowEx(dwExStyle, pszClassName, pszWindowName, dwStyle, x, y, nWidth, nHeight, GetHWND(pParent), m_hMenu ? m_hMenu : (HMENU)nControlId, g_hInstance, pvParam)))
    {
        return FALSE;
    }

    if(m_pParent)
    {
        InsertTailList(&m_pParent->m_lstChildren, &m_leChildren);
    }

    return TRUE;
}


/****************************************************************************
 *
 *  DispatchMessage
 *
 *  Description:
 *      Dispatches a message to the appropriate handler.
 *
 *  Arguments:
 *      LPMSG [in]: message.
 *
 *  Returns:  
 *      BOOL: TRUE if the message was dispatched.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWindow::DispatchMessage"

BOOL
CWindow::DispatchMessage
(
    LPMSG                   pMsg
)
{
    BOOL                    fDispatched = FALSE;
    PLIST_ENTRY             pleEntry;
    CWindow *               pChild;

    //
    // See if any of our children can do anything with this message.  If they
    // cant, we don't have anything special to do.
    //

    for(pleEntry = m_lstChildren.Flink; pleEntry != &m_lstChildren; pleEntry = pleEntry->Flink)
    {
        pChild = CONTAINING_RECORD(pleEntry, CWindow, m_leChildren);

        if(fDispatched = pChild->DispatchMessage(pMsg))
        {
            break;
        }
    }

    if(!fDispatched && m_hAccel)
    {
        fDispatched = TranslateAccelerator(m_hWnd, m_hAccel, pMsg);
    }

    return fDispatched;
}


/****************************************************************************
 *
 *  WindowProc
 *
 *  Description:
 *      Main window message handler.
 *
 *  Arguments:
 *      HWND [in]: window handle.
 *      UINT [in]: message identifier.
 *      WPARAM [in]: message parameter 1.
 *      LPARAM [in]: message parameter 2.
 *
 *  Returns:  
 *      LRESULT: message result.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWindow::WindowProc"

LPARAM CALLBACK
CWindow::WindowProc
(
    HWND                    hWnd,
    UINT                    uMsg,
    WPARAM                  wParam,
    LPARAM                  lParam
)
{
    LRESULT                 lResult     = 0;
    CWindow *               pWindow;

    //
    // Get the "this" pointer from the window object
    // 
    // We're special-casing WM_CREATE because it actually contains the window
    // handle we use to add ourselves to the global window list.
    //
    
    if(WM_CREATE == uMsg)
    {
        pWindow = OnInitialCreate(hWnd, &lParam);
    }
    else
    {
        pWindow = GetWindowContext(hWnd);
    }

    if(!pWindow)
    {
        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    //
    // Hand off to the message dispatcher
    //

    if(!pWindow->HandleMessage(uMsg, wParam, lParam, &lResult))
    {
        lResult = pWindow->DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    //
    // Special-case WM_DESTROY to delete the window object
    //

    if(WM_DESTROY == uMsg)
    {
        pWindow->Destroy();
        pWindow->Release();
    }

    return lResult;
}


/****************************************************************************
 *
 *  SetWindowContext
 *
 *  Description:
 *      Sets up HWND => CWindow mapping data.
 *
 *  Arguments:
 *      HWND [in]: window handle.
 *      CWindow * [in]: window object.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWindow::SetWindowContext"

void
CWindow::SetWindowContext
(
    HWND                    hWnd,
    CWindow *               pWindow
)
{
    ASSERT(hWnd);
    ASSERT(pWindow);

#ifdef WNDWRAP_USE_SET_WINDOW_LONG

    SetWindowLong(hWnd, GWL_USERDATA, (LONG)pWindow);

#else // WNDWRAP_USE_SET_WINDOW_LONG

    InsertTailList(&m_lstWindows, &pWindow->m_leWindows);

#endif // WNDWRAP_USE_SET_WINDOW_LONG

}


/****************************************************************************
 *
 *  FreeWindowContext
 *
 *  Description:
 *      Frees HWND => CWindow mapping data.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWindow::FreeWindowContext"

void
CWindow::FreeWindowContext
(
    void
)
{

#ifdef WNDWRAP_USE_SET_WINDOW_LONG

    SetWindowLong(m_hWnd, GWL_USERDATA, 0);

#else // WNDWRAP_USE_SET_WINDOW_LONG

    RemoveEntryList(&m_leWindows);

#endif // WNDWRAP_USE_SET_WINDOW_LONG

}


/****************************************************************************
 *
 *  GetWindowContext
 *
 *  Description:
 *      Retrieves HWND => CWindow mapping data.
 *
 *  Arguments:
 *      HWND [in]: window handle.
 *
 *  Returns:  
 *      CWindow *: window object.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWindow::GetWindowContext"

CWindow *
CWindow::GetWindowContext
(
    HWND                    hWnd
)
{
    CWindow *               pWindow;
    PLIST_ENTRY             pleEntry;

#ifdef WNDWRAP_USE_SET_WINDOW_LONG

    pWindow = (CWindow *)GetWindowLong(hWnd, GWL_USERDATA);

#else //  WNDWRAP_USE_SET_WINDOW_LONG

    pleEntry = m_lstWindows.Flink;
    pWindow = NULL;
    
    while(pleEntry != &m_lstWindows)
    {
        pWindow = CONTAINING_RECORD(pleEntry, CWindow, m_leWindows);

        if(pWindow->m_hWnd)
        {
            if(hWnd == pWindow->m_hWnd)
            {
                break;
            }
        }

        pleEntry = pleEntry->Flink;
        pWindow = NULL;
    }

#endif //  WNDWRAP_USE_SET_WINDOW_LONG

    if(pWindow)
    {
        ASSERT(m_dwValidSignature == pWindow->m_dwSignature);
    }

    return pWindow;
}


/****************************************************************************
 *
 *  OnInitialCreate
 *
 *  Description:
 *      Peforms internal handling of the WM_CREATE message.
 *
 *  Arguments:
 *      HWND [in]: window handle.
 *      LPARAM * [in/out]: message parameter 2.
 *
 *  Returns:  
 *      CWindow *: window object.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWindow::OnInitialCreate"

CWindow *
CWindow::OnInitialCreate
(
    HWND                    hWnd,
    LPARAM *                plParam
)
{
    CWindow *               pWindow     = NULL;
    LRESULT                 lResult     = 0;
    LPCREATESTRUCT          pCreate;
    LPMDICREATESTRUCT       pMdiCreate;
    LPCWINDOWCREATECTX      pCtx;
    BOOL                    fHandled;

    ASSERT(hWnd);
    ASSERT(plParam);

    //
    // Get the CWindow object from the message parameters
    //

    pCreate = (LPCREATESTRUCT)*plParam;
    ASSERT(pCreate);

    if(pCreate->dwExStyle & WS_EX_MDICHILD)
    {
        pMdiCreate = (LPMDICREATESTRUCT)pCreate->lpCreateParams;
        ASSERT(pMdiCreate);

        pCtx = (LPCWINDOWCREATECTX)pMdiCreate->lParam;
        ASSERT(pCtx);
    }
    else
    {
        pCtx = (LPCWINDOWCREATECTX)pCreate->lpCreateParams;
        ASSERT(pCtx);
    }

    ASSERT(m_dwValidSignature == pCtx->dwSignature);

    pWindow = pCtx->pWindow;
    ASSERT(pWindow);

    *plParam = (LPARAM)pCtx->pvContext;

    ASSERT(!pWindow->m_hWnd);
    pWindow->m_hWnd = hWnd;

    //
    // Store the HWND => CWindow lookup data
    //

    SetWindowContext(hWnd, pWindow);

    //
    // Increment the window object's reference count
    //

    pWindow->AddRef();

    return pWindow;
}


/****************************************************************************
 *
 *  Destroy
 *
 *  Description:
 *      Destroys the window.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWindow::Destroy"

void
CWindow::Destroy
(
    void
)
{
    LRESULT                 lResult     = 0;
    PLIST_ENTRY             pleEntry;
    CWindow *               pChild;

    //
    // Call the OnDestroy handler
    //

    if(m_hWnd)
    {
        OnDestroy(&lResult);
    }

    //
    // Free the HWND => CWindow lookup data so we don't get any more messages
    //

    if(m_hWnd)
    {
        FreeWindowContext();
    }

    //
    // Detach all child windows
    //

    while((pleEntry = RemoveEntryList(m_lstChildren.Flink)) != &m_lstChildren)
    {
        pChild = CONTAINING_RECORD(pleEntry, CWindow, m_leChildren);

        pChild->Destroy();
    }

    //
    // Destroy the window
    //

    if(m_hWnd)
    {
        if((WNDTYPE_MDICHILD == m_nWindowType) && m_pParent)
        {
            m_pParent->SendMessage(WM_MDIDESTROY, (WPARAM)m_hWnd);
        }
        else
        {
            DestroyWindow(m_hWnd);
        }

        m_hWnd = NULL;
    }

    //
    // Detach from the parent
    //

    RemoveEntryList(&m_leChildren);
    m_pParent = NULL;

    //
    // Free resources
    //

    DestroyMenu(m_hMenu);
    m_hMenu = NULL;

    DestroyAcceleratorTable(m_hAccel);
    m_hMenu = NULL;
}


/****************************************************************************
 *
 *  HandleMessage
 *
 *  Description:
 *      Dispatches a window message to the appropriate handler.
 *
 *  Arguments:
 *      UINT [in]: message identifier.
 *      WPARAM [in]: message parameter 1.
 *      LPARAM [in]: message parameter 2.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWindow::HandleMessage"

BOOL 
CWindow::HandleMessage
(
    UINT                    uMsg, 
    WPARAM                  wParam, 
    LPARAM                  lParam, 
    LRESULT *               plResult
)
{
    BOOL                    fHandled;
    
    // DPF_ABSOLUTE("%x %x %x %x", m_hWnd, uMsg, wParam, lParam);
    
    switch(uMsg)
    {
        //
        // Basic handlers
        //
        
        case WM_CREATE:
            fHandled = OnCreate((LPVOID)lParam, plResult);
            break;
        
        case WM_CLOSE:
            fHandled = OnClose(plResult);
            break;
        
        case WM_SIZE:
            fHandled = OnSize(wParam, (short)LOWORD(lParam), (short)HIWORD(lParam), plResult);
            break;

        case WM_COMMAND:
            fHandled = OnCommand(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, plResult);
            break;
        
        case WM_SYSCOMMAND:
            fHandled = OnSysCommand(wParam, plResult);
            break;
        
        case WM_NOTIFY:
            fHandled = OnNotify((LPNMHDR)lParam, plResult);
            break;

        case WM_ERASEBKGND:
            fHandled = OnEraseBackground((HDC)wParam, plResult);
            break;

        case WM_PAINT:
            fHandled = OnPaint(plResult);
            break;

        case WM_CONTEXTMENU:
            fHandled = OnContextMenu((HWND)wParam, (short)LOWORD(lParam), (short)HIWORD(lParam), plResult);
            break;

        case WM_DROPFILES:
            fHandled = OnDropFiles((HDROP)wParam, plResult);
            break;
        
        //
        // Advanced handlers
        //

        case WM_HSCROLL:
            fHandled = OnScroll(SB_HORZ, LOWORD(wParam), (HWND)lParam, plResult);
            break;
        
        case WM_VSCROLL:
            fHandled = OnScroll(SB_VERT, LOWORD(wParam), (HWND)lParam, plResult);
            break;

        //
        // No handler
        //

        default:
            fHandled = FALSE;
            break;
    }

    return fHandled;
}


/****************************************************************************
 *
 *  OnScroll
 *
 *  Description:
 *      Handles WM_HSCROLL and WM_VSCROLL messages.  This method does partial
 *      handling of the message before handing off to the default handler.
 *
 *  Arguments:
 *      UINT [in]: scroll bar type (SB_HORZ/SB_VERT).
 *      UINT [in]: scroll request.
 *      HWND [in]: scroll bar window handle.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWindow::OnScroll"

BOOL
CWindow::OnScroll
(
    UINT                    nScrollBarType, 
    UINT                    nRequest, 
    HWND                    hWndScrollBar, 
    LRESULT *               plResult
)
{
    SCROLLINFO              si;

    if(SB_ENDSCROLL == nRequest)
    {
        return FALSE;
    }

    //
    // The WM_H/VSCROLL message sucks ass.  It doesn't actually update the
    // position or tell us anything valuable, so we have to do it manually.
    //

    si.cbSize = sizeof(si);
    si.fMask = SIF_ALL;

    if(!GetScrollInfo(m_hWnd, nScrollBarType, &si))
    {
        return FALSE;
    }

    switch(nRequest)
    {
        case SB_BOTTOM:
            si.nPos = si.nMin;
            break;

        case SB_TOP:
            si.nPos = si.nMax;
            break;

        case SB_LINEDOWN:
            si.nPos++;
            break;

        case SB_LINEUP:
            si.nPos--;
            break;

        case SB_PAGEDOWN:
            si.nPos += si.nPage;
            break;

        case SB_PAGEUP:
            si.nPos -= si.nPage;
            break;

        case SB_THUMBTRACK:
            si.nPos = si.nTrackPos;
            break;
    }

    si.nPos = min(si.nPos, si.nMax);
    si.nPos = max(si.nPos, si.nMin);

    SetScrollPos(m_hWnd, nScrollBarType, si.nPos, TRUE);

    //
    // Call the handler
    //

    OnScroll(nScrollBarType, hWndScrollBar, si.nPos, plResult);

    return TRUE;
}


/****************************************************************************
 *
 *  SetRedraw
 *
 *  Description:
 *      Toggles the redraw flag for this window.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to allow redraws.
 *
 *  Returns:  
 *      BOOL: prior value.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "SetRedraw"

BOOL 
CWindow::SetRedraw
(
    BOOL                    fRedraw
)
{
    const BOOL              fPrevious   = m_fRedraw;

    if(fPrevious != fRedraw)
    {
        m_fRedraw = fRedraw;

        SendMessage(WM_SETREDRAW, fRedraw);

        if(fRedraw)
        {
            InvalidateRect(m_hWnd, NULL, FALSE);
            UpdateWindow(m_hWnd);
        }
    }

    return fPrevious;
}


