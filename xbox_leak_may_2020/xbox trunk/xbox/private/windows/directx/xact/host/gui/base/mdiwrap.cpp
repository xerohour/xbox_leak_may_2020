/***************************************************************************
 *
 *  Copyright (C) 11/8/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mdiwrap.cpp
 *  Content:    Windows wrapper classes (aka My Mini MFC)
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/8/2001   dereks  Created.
 *
 ****************************************************************************/

#include "xactgui.h"


/****************************************************************************
 *
 *  CMDIFrameWindow
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      UINT [in]: menu resource identifier, or 0.
 *      UINT [in]: accelerator resource identifier, or 0.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMDIFrameWindow::CMDIFrameWindow"

CMDIFrameWindow::CMDIFrameWindow
(
    UINT                    nMenuId,
    UINT                    nAccelId
)
:   CWindow(nMenuId, nAccelId, WNDTYPE)
{
}


/****************************************************************************
 *
 *  ~CMDIFrameWindow
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
#define DPF_FNAME "CMDIFrameWindow::~CMDIFrameWindow"

CMDIFrameWindow::~CMDIFrameWindow
(
    void
)
{
}


/****************************************************************************
 *
 *  Create
 *
 *  Description:
 *      Creates a window.
 *
 *  Arguments:
 *      LPCTSTR [in]: window class name.
 *      LPCTSTR [in]: window title.
 *      DWORD [in]: window extended style.
 *      DWORD [in]: window basic style.
 *      int [in]: window x-coordinate.
 *      int [in]: window y-coordinate.
 *      int [in]: window width.
 *      int [in]: window height.
 *      LPVOID [in]: creation context.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMDIFrameWindow::Create"

BOOL
CMDIFrameWindow::Create
(
    LPCTSTR                 pszClassName, 
    LPCTSTR                 pszWindowName, 
    DWORD                   dwExStyle, 
    DWORD                   dwStyle, 
    int                     x, 
    int                     y, 
    int                     nWidth, 
    int                     nHeight, 
    LPVOID                  pvParam
)
{
    return CWindow::Create(NULL, pszClassName, pszWindowName, dwExStyle, dwStyle, x, y, nWidth, nHeight, 0, pvParam);
}


/****************************************************************************
 *
 *  DefWindowProc
 *
 *  Description:
 *      Default message handler.
 *
 *  Arguments:
 *      HWND [in]: window handle.
 *      UINT [in]: message identifier.
 *      WPARAM [in]: message parameter.
 *      LPARAM [in]: message parameter.
 *
 *  Returns:  
 *      LRESULT: message result.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMDIFrameWindow::DefWindowProc"

LRESULT
CMDIFrameWindow::DefWindowProc
(
    HWND                    hWnd, 
    UINT                    uMsg, 
    WPARAM                  wParam, 
    LPARAM                  lParam
)
{
    switch(uMsg)
    {
        case WM_SIZE:
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);

        default:
            return ::DefFrameProc(hWnd, GetHWND(m_pMDIClient), uMsg, wParam, lParam);
    }
}


/****************************************************************************
 *
 *  CMDIClientWindow
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMDIClientWindow::CMDIClientWindow"

CMDIClientWindow::CMDIClientWindow
(
    void
)
:   CWindow(-1, -1, WNDTYPE)
{
}


/****************************************************************************
 *
 *  ~CMDIClientWindow
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
#define DPF_FNAME "CMDIClientWindow::~CMDIClientWindow"

CMDIClientWindow::~CMDIClientWindow
(
    void
)
{
    CMDIFrameWindow *           pParent = CWindowCast<CMDIFrameWindow>::Cast(m_pParent);

    if(pParent)
    {
        pParent->m_pMDIClient = NULL;
    }
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
 *      UINT [in]: identifier of the first MDI child window.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMDIClientWindow::Create"

BOOL
CMDIClientWindow::Create
(
    CMDIFrameWindow *           pParent, 
    DWORD                       dwExStyle, 
    DWORD                       dwStyle, 
    int                         x,
    int                         y,
    int                         nWidth,
    int                         nHeight,
    UINT                        nControlId,
    const CLIENTCREATESTRUCT *  pccs
)
{
    ASSERT(pParent);
    
    dwExStyle |= WS_EX_CLIENTEDGE;
    dwStyle |= WS_CHILD;

    //
    // Create the window.  The 
    //

    if(!CWindow::Create(pParent, TEXT("MDICLIENT"), NULL, dwExStyle, dwStyle, x, y, nWidth, nHeight, nControlId, (LPVOID)pccs))
    {
        return FALSE;
    }

    //
    // Give the parent frame a pointer to us
    //

    pParent->m_pMDIClient = this;

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
#define DPF_FNAME "CMDIClientWindow::DispatchMessage"

BOOL
CMDIClientWindow::DispatchMessage
(
    LPMSG                   pMsg
)
{
    BOOL                    fDispatched;

    //
    // If none of our children handle the message, it might be an MDI system
    // accelerator
    //

    if(!(fDispatched = CWindow::DispatchMessage(pMsg)))
    {
        fDispatched = TranslateMDISysAccel(m_hWnd, pMsg);
    }

    return fDispatched;
}


/****************************************************************************
 *
 *  GetActiveChild
 *
 *  Description:
 *      Gets the active workspace child.
 *
 *  Arguments:
 *      CMDIChildWindow ** [out]: child window.
 *      LPBOOL [out]: TRUE if the window is maximized.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMDIClientWindow::GetActiveChild"

BOOL
CMDIClientWindow::GetActiveChild
(
    CMDIChildWindow **      ppWindow,
    LPBOOL                  pfMaximized
)
{
    CMDIChildWindow *       pWindow;
    HWND                    hWnd;

    if(!(hWnd = (HWND)SendMessage(WM_MDIGETACTIVE, 0, (LPARAM)pfMaximized)))
    {
        return FALSE;
    }

    if(!(pWindow = CWindowCast<CMDIChildWindow>::Cast(GetWindowContext(hWnd))))
    {
        return FALSE;
    }

    if(ppWindow)
    {
        *ppWindow = pWindow;
    }

    return TRUE;
}


/****************************************************************************
 *
 *  IsActiveChild
 *
 *  Description:
 *      Determines if a child window is currently the active one.
 *
 *  Arguments:
 *      CMDIChildWindow * [in]: child window.
 *
 *  Returns:  
 *      BOOL: TRUE if the window is active.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMDIClientWindow::IsActiveChild"

BOOL
CMDIClientWindow::IsActiveChild
(
    CMDIChildWindow *       pWindow
)
{
    HWND                    hWnd;

    if(!(hWnd = (HWND)SendMessage(WM_MDIGETACTIVE)))
    {
        return FALSE;
    }

    return hWnd == pWindow->m_hWnd;
}


/****************************************************************************
 *
 *  CMDIChildWindow
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMDIChildWindow::CMDIChildWindow"

CMDIChildWindow::CMDIChildWindow
(
    void
)
:   CWindow(-1, -1, WNDTYPE)
{
}


/****************************************************************************
 *
 *  ~CMDIChildWindow
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
#define DPF_FNAME "CMDIChildWindow::~CMDIChildWindow"

CMDIChildWindow::~CMDIChildWindow
(
    void
)
{
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
 *      LPVOID [in]: creation context.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMDIChildWindow::Create"

BOOL 
CMDIChildWindow::Create
(
    CMDIClientWindow *      pParent, 
    LPCTSTR                 pszClassName, 
    LPCTSTR                 pszWindowName, 
    DWORD                   dwExStyle, 
    DWORD                   dwStyle, 
    int                     x, 
    int                     y, 
    int                     nWidth, 
    int                     nHeight, 
    LPVOID                  pvParam
)
{
    BOOL                    fMaximized;
    
    ASSERT(pParent);

    dwExStyle |= WS_EX_MDICHILD;
    dwStyle |= WS_CHILD;

    //
    // Get the window state of the currently active MDI child (if any)
    // and use it to override the state passed in.
    //
    
    if(pParent->GetActiveChild(NULL, &fMaximized))
    {
        dwStyle &= ~(WS_MAXIMIZE | WS_MINIMIZE);
        
        if(fMaximized)
        {
            dwStyle |= WS_MAXIMIZE;
        }
    }

    //
    // Create the window
    //

    return CWindow::Create(pParent, pszClassName, pszWindowName, dwExStyle, dwStyle, x, y, nWidth, nHeight, 0, pvParam);
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
#define DPF_FNAME "CMDIChildWindow::HandleMessage"

BOOL 
CMDIChildWindow::HandleMessage
(
    UINT                    uMsg, 
    WPARAM                  wParam, 
    LPARAM                  lParam, 
    LRESULT *               plResult
)
{
    BOOL                    fHandled;
    
    switch(uMsg)
    {
        case WM_STYLECHANGED:
            fHandled = OnStyleChanged(wParam, (LPSTYLESTRUCT)lParam, plResult);
            break;

        case WM_USER_FORCERESIZE:
            fHandled = OnForceResize(plResult);
            break;

        case WM_MDIACTIVATE:
            fHandled = OnMdiActivate((HWND)wParam, (HWND)lParam, plResult);
            break;

        default:
            fHandled = CWindow::HandleMessage(uMsg, wParam, lParam, plResult);
            break;
    }

    return fHandled;
}


/****************************************************************************
 *
 *  OnSize
 *
 *  Description:
 *      Handles WM_SIZE messages.
 *
 *  Arguments:
 *      UINT [in]: resize type.
 *      UINT [in]: client width.
 *      UINT [in]: client height.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMDIChildWindow::OnSize"

BOOL
CMDIChildWindow::OnSize
(
    UINT                    nType, 
    UINT                    nWidth, 
    UINT                    nHeight, 
    LRESULT *               plResult
)
{
    static const DWORD      dwFlags     = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED;
    DWORD                   dwExStyle;
    RECT                    rc;
    
    if(CWindow::OnSize(nType, nWidth, nHeight, plResult))
    {
        return TRUE;
    }

    //
    // Toggle the WS_EX_CLIENTEDGE style so we don't look goofy when maximized
    // inside the MDI client window.  Because of this trick, make sure child
    // windows don't have the WS_EX_CLIENTEDGE style.
    //
    // This is all a massive hack.  See OnStyleChanged and OnForceResize for
    // more information.
    //

    if(SIZE_MAXIMIZED == nType)
    {
        if((dwExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE)) & WS_EX_CLIENTEDGE)
        {
            SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle & ~WS_EX_CLIENTEDGE);
        }
    }
    else
    {
        if(!((dwExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE)) & WS_EX_CLIENTEDGE))
        {
            SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle | WS_EX_CLIENTEDGE);
        }
    }

    return FALSE;
}


/****************************************************************************
 *
 *  OnStyleChanged
 *
 *  Description:
 *      Handles WM_STYLECHANGED messages.
 *
 *  Arguments:
 *      UINT [in]: style that changed.
 *      LPSTYLESTRUCT [in]: style data.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMDIChildWindow::OnStyleChanged"

BOOL
CMDIChildWindow::OnStyleChanged
(
    UINT                    nStyle, 
    LPSTYLESTRUCT           pStyleData,
    LRESULT *               plResult
)
{
    static const DWORD      dwFlags             = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED;
    static const DWORD      dwFrameChangeMask   = WS_EX_CLIENTEDGE;

    //
    // Whenever the frame style changes, Windows won't automatically redraw
    // the window, so we'll do it from here
    //
    
    if(GWL_EXSTYLE == nStyle)
    {
        if((pStyleData->styleNew ^ pStyleData->styleOld) & dwFrameChangeMask)
        {
            ::SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, dwFlags);
            ::PostMessage(m_hWnd, WM_USER_FORCERESIZE, 0, 0);
        }
    }

    return FALSE;
}


/****************************************************************************
 *
 *  OnForceResize
 *
 *  Description:
 *      Handles WM_USER_FORCERESIZE messages.
 *
 *  Arguments:
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMDIChildWindow::OnForceResize"

BOOL
CMDIChildWindow::OnForceResize
(
    LRESULT *               plResult
)
{
    RECT                    rc;

    // 
    // This sucks, but I'm anal enough to want a recessed MDI client window
    // and a recessed MDI child window without having a doubly-recessed
    // window when the child is maximized.
    //
    // Force a resize of the MDI client window so it in turn resizes the
    // MDI child properly.
    //
    // Sometimes that's not enough, so we'll force a resize of the child
    // as well.  If there isn't a better way to do this, someone should
    // lose their job.  Regardless, I can't figure it out.
    //

    ASSERT(m_pParent);
    ASSERT(m_pParent->m_pParent);
    
    m_pParent->GetWindowRect(&rc);
    m_pParent->m_pParent->ScreenToClient(&rc);

    m_pParent->MoveWindow(0, 0, 0, 0, FALSE);
    m_pParent->MoveWindow(&rc, FALSE);

    GetWindowRect(&rc);
    m_pParent->ScreenToClient(&rc);

    InvalidateRect(m_pParent->m_hWnd, NULL, TRUE);
    InvalidateRect(m_hWnd, NULL, TRUE);

    MoveWindow(0, 0, 0, 0, FALSE);
    MoveWindow(&rc, TRUE);

    *plResult = 0;

    return TRUE;
}


/****************************************************************************
 *
 *  OnMdiActivate
 *
 *  Description:
 *      Handles WM_MDIACTIVATE messages.
 *
 *  Arguments:
 *      HWND [in]: inactive window.
 *      HWND [in]: active window.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMDIChildWindow::OnMdiActivate"

BOOL
CMDIChildWindow::OnMdiActivate
(
    HWND                    hWndInactive,
    HWND                    hWndActive,
    LRESULT *               plResult
)
{
    if(hWndInactive == m_hWnd)
    {
        OnDeactivate();
    }
    else if(hWndActive == m_hWnd)
    {
        OnActivate();
    }
    else
    {
        DPF_ERROR("Received WM_MDIACTIVATE with unexpected handles");
    }

    return FALSE;
}


/****************************************************************************
 *
 *  OnDestroy
 *
 *  Description:
 *      Handles WM_DESTROY messages.
 *
 *  Arguments:
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMDIChildWindow::OnDestroy"

BOOL
CMDIChildWindow::OnDestroy
(
    LRESULT *               plResult
)
{
    if(CWindow::OnDestroy(plResult))
    {
        return TRUE;
    }

    //
    // If we're the last child to go, make sure the OnDeactivate method gets
    // called.  The MDIACTIVATE message isn't sent unless there's a new window
    // waiting to take focus.
    //

    if(!((CMDIClientWindow *)m_pParent)->GetActiveChild())
    {
        OnDeactivate();
    }

    return FALSE;
}


/****************************************************************************
 *
 *  DefWindowProc
 *
 *  Description:
 *      Default message handler.
 *
 *  Arguments:
 *      HWND [in]: window handle.
 *      UINT [in]: message identifier.
 *      WPARAM [in]: message parameter.
 *      LPARAM [in]: message parameter.
 *
 *  Returns:  
 *      LRESULT: message result.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMDIChildWindow::DefWindowProc"

LRESULT
CMDIChildWindow::DefWindowProc
(
    HWND                    hWnd, 
    UINT                    uMsg, 
    WPARAM                  wParam, 
    LPARAM                  lParam
)
{
    return ::DefMDIChildProc(hWnd, uMsg, wParam, lParam);
}


