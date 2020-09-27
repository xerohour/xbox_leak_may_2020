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

#include "wbndgui.h"

BEGIN_DEFINE_STRUCT()
    CWindow *   pWindow;
    LPVOID      pvContext;
END_DEFINE_STRUCT(WINDOWCREATECTX);


/****************************************************************************
 *
 *  FormatStringResource
 *
 *  Description:
 *      Loads a string template and formats it with the additional 
 *      parameters.
 *
 *  Arguments:
 *      LPTSTR [out]: receives formatted text.
 *      HINSTANCE [in]: instance handle.
 *      UINT [in]: resource identifier.
 *      ...
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "FormatStringResource"

BOOL
FormatStringResource
(
    LPTSTR                  pszText, 
    HINSTANCE               hInstance, 
    UINT                    nResourceId, 
    ...
)
{
    TCHAR                   szFormat[0x100];
    va_list                 va;

    if(!LoadString(hInstance, nResourceId, szFormat, NUMELMS(szFormat)))
    {
        return FALSE;
    }

    va_start(va, nResourceId);
    vsprintf(pszText, szFormat, va);
    va_end(va);

    return TRUE;
}


/****************************************************************************
 *
 *  CenterWindow
 *
 *  Description:
 *      Centers one window over another.
 *
 *  Arguments:
 *      HWND [in]: window handle.
 *      HWND [in]: parent window handle, or NULL to center over the desktop.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CenterWindow"

void
CenterWindow
(
    HWND                    hwnd,
    HWND                    hwndParent
)
{
    RECT                    rc;
    RECT                    rcParent;
    HMONITOR                hmon;
    MONITORINFO             mi;
    
    mi.cbSize = sizeof(mi);
    
    GetWindowRect(hwnd, &rc);
    
    if(hwndParent)
    {
        GetWindowRect(hwndParent, &rcParent);
    }
    else
    {
        hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);

        GetMonitorInfo(hmon, &mi);

        rcParent = mi.rcWork;
    }

    rc.right -= rc.left;
    rc.bottom -= rc.top;
    
    rc.left = rcParent.left + (((rcParent.right - rcParent.left) - rc.right) / 2);
    rc.top = rcParent.top + (((rcParent.bottom - rcParent.top) - rc.bottom) / 2);

    MoveWindow(hwnd, rc.left, rc.top, rc.right, rc.bottom, TRUE);
}


/****************************************************************************
 *
 *  CWindow
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

CWindow::CWindow
(
    void
)
:   m_dwSignature(WINDOW_SIGNATURE)
{
    m_hwnd = NULL;
    m_hmenu = NULL;
    m_haccel = NULL;
    m_fEnableAccel = TRUE;
    
    InitializeListHead(&m_lstChildren);
    InitializeListHead(&m_leChild);
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

CWindow::~CWindow
(
    void
)
{
    if(m_hwnd)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = NULL;
    }

    if(m_hmenu)
    {
        DestroyMenu(m_hmenu);
        m_hmenu = NULL;
    }

    if(m_haccel)
    {
        DestroyAcceleratorTable(m_haccel);
        m_haccel = NULL;
    }

    SetParent(NULL);
}


/****************************************************************************
 *
 *  Create
 *
 *  Description:
 *      Creates a window.
 *
 *  Arguments:
 *      LPCWNDCREATE [in]: window creation data.
 *      LPVOID [in]: window creation context (passed to OnCreate).
 *      int [in]: window visibility.
 *      CWindow * [in]: parent window.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

BOOL
CWindow::Create
(
    LPCWNDCREATE            pCreate,
    LPVOID                  pvContext,
    CWindow *               pParent
)
{
    BOOL                    fSuccess    = TRUE;
    WNDCLASSEX              wc;
    WINDOWCREATECTX         ctx;

    ASSERT(pCreate);
    
    ZeroMemory(&wc, sizeof(wc));

    wc.cbSize = sizeof(wc);
    wc.style = pCreate->dwClassStyle;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = pCreate->dwClassExtra;
    wc.cbWndExtra = pCreate->dwWndExtra;
    wc.hInstance = g_hInstance;
    wc.hIcon = pCreate->hIcon;
    wc.hCursor = pCreate->hCursor;
    wc.hbrBackground = pCreate->hbrBackground;
    wc.lpszClassName = pCreate->pszClassName;
    wc.hIconSm = pCreate->hIconSm;

    if(pCreate->pszMenu)
    {
        m_hmenu = LoadMenu(g_hInstance, pCreate->pszMenu);
    }

    if(pCreate->pszAccelerator)
    {
        m_haccel = LoadAccelerators(g_hInstance, pCreate->pszAccelerator);
    }

    if(pParent)
    {
        SetParent(pParent);
    }
    
    if(!::RegisterClassEx(&wc))
    {
        fSuccess = FALSE;
    }

    if(fSuccess)
    {
        ctx.pWindow = this;
        ctx.pvContext = pvContext;
        
        if(!(m_hwnd = CreateWindowEx(pCreate->dwExStyle, pCreate->pszClassName, pCreate->pszWindowName, pCreate->dwStyle, pCreate->x, pCreate->y, pCreate->nWidth, pCreate->nHeight, pParent ? pParent->m_hwnd : NULL, NULL, wc.hInstance, &ctx)))
        {
            fSuccess = FALSE;
        }
    }

    return fSuccess;
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

LPARAM CALLBACK
CWindow::WindowProc
(
    HWND                    hwnd,
    UINT                    uMsg,
    WPARAM                  wParam,
    LPARAM                  lParam
)
{
    LPCREATESTRUCT          pCreate;
    LPCWINDOWCREATECTX      pCtx;
    CWindow *               pWindow;
    LRESULT                 lResult;
    BOOL                    fHandled;
    
    //
    // Get the "this" pointer from the window
    //

    if(WM_CREATE == uMsg)
    {
        pCreate = (LPCREATESTRUCT)lParam;
        pCtx = (LPCWINDOWCREATECTX)pCreate->lpCreateParams;
        pWindow = pCtx->pWindow;

        SetWindowLong(hwnd, GWL_USERDATA, (LONG)pWindow);

        pWindow->m_hwnd = hwnd;
    }
    else
    {
        pWindow = (CWindow *)GetWindowLong(hwnd, GWL_USERDATA);
    }

    if(pWindow)
    {
        if(WINDOW_SIGNATURE != pWindow->m_dwSignature)
        {
            DPF_INFO("Bogus this pointer from message %4.4x", uMsg);
            pWindow = NULL;
        }
    }

    //
    // Hand off to the dispatcher
    //

    if(pWindow)
    {
        fHandled = pWindow->HandleMessage(uMsg, wParam, lParam, &lResult);
    }
    else
    {
        fHandled = FALSE;
    }

    //
    // Handle destruction
    //
    
    if((WM_DESTROY == uMsg) && pWindow)
    {
        SetWindowLong(hwnd, GWL_USERDATA, NULL);

        pWindow->m_hwnd = NULL;
    }

    //
    // Hand off to the default handler
    //

    if(!fHandled)
    {
        lResult = DefaultMessageHandler(hwnd, uMsg, wParam, lParam);
    }

    return lResult;
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

BOOL 
CWindow::HandleMessage
(
    UINT                    uMsg, 
    WPARAM                  wParam, 
    LPARAM                  lParam, 
    LRESULT *               plResult
)
{
    LPCREATESTRUCT          pCreate;
    LPCWINDOWCREATECTX      pCtx;
    BOOL                    fHandled;
    
    switch(uMsg)
    {
        case WM_CREATE:
            pCreate = (LPCREATESTRUCT)lParam;
            pCtx = (LPCWINDOWCREATECTX)pCreate->lpCreateParams;
            fHandled = OnCreate(pCtx->pvContext, plResult);
            break;
        
        case WM_CLOSE:
            fHandled = OnClose(plResult);
            break;
        
        case WM_DESTROY:
            fHandled = OnDestroy(plResult);
            break;

        case WM_SIZE:
            fHandled = OnSize(wParam, LOWORD(lParam), HIWORD(lParam), plResult);
            break;

        case WM_COMMAND:
            fHandled = OnCommand(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, plResult);
            break;
        
        case WM_NOTIFY:
            fHandled = OnNotify((LPNMHDR)lParam, plResult);
            break;

        case WM_DPF:
            fHandled = OnDebug(HIWORD(wParam), LOWORD(wParam), (LPCSTR)lParam, plResult);
            break;

        case WM_CONTEXTMENU:
            fHandled = OnContextMenu((HWND)wParam, LOWORD(lParam), HIWORD(lParam), plResult);
            break;

        case WM_MENUSELECT:
            fHandled = OnMenuSelect(LOWORD(wParam), HIWORD(wParam), (HMENU)lParam, plResult);
            break;
        
        default:
            fHandled = FALSE;
            break;
    }

    return fHandled;
}


/****************************************************************************
 *
 *  SetParent
 *
 *  Description:
 *      Creates a parent/child relationship.
 *
 *  Arguments:
 *      CWindow * [in]: parent window.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

void
CWindow::SetParent
(
    CWindow *               pParent
)
{
    RemoveEntryList(&m_leChild);

    if(m_pParent = pParent)
    {
        InsertTailList(&pParent->m_lstChildren, &m_leChild);
    }
}


/****************************************************************************
 *
 *  PumpMessages
 *
 *  Description:
 *      Pumps messages for the current thread.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

void
CWindow::PumpMessages
(
    void
)
{
    MSG                     msg;

    while(::GetMessage(&msg, NULL, 0, 0))
    {
        if(!DispatchMessage(&msg))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
}


/****************************************************************************
 *
 *  DispatchMessage
 *
 *  Description:
 *      Dispatches the next message in this window's queue.
 *
 *  Arguments:
 *      LPMSG [in]: message.
 *
 *  Returns:  
 *      BOOL: TRUE if the message has been dispatched.  FALSE if the caller
 *            should dispatch it.
 *
 ****************************************************************************/

BOOL
CWindow::DispatchMessage
(
    LPMSG                   pmsg
)
{
    PLIST_ENTRY             pleChild;

    if(m_haccel && m_fEnableAccel)
    {
        if(TranslateAccelerator(m_hwnd, m_haccel, pmsg))
        {
            return TRUE;
        }
    }

    for(pleChild = m_lstChildren.Flink; pleChild != &m_lstChildren; pleChild = pleChild->Flink)
    {
        if(CONTAINING_RECORD(pleChild, CWindow, m_leChild)->DispatchMessage(pmsg))
        {
            return TRUE;
        }
    }
    
    return FALSE;
}


/****************************************************************************
 *
 *  OnCreate
 *
 *  Description:
 *      Handles creation of the window.
 *
 *  Arguments:
 *      LPVOID [in]: window creation context.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWindow::OnCreate"

BOOL
CWindow::OnCreate
(
    LPVOID                  pvContext,
    LRESULT *               plResult
)
{
    if(m_hmenu)
    {
        SetMenu(m_hwnd, m_hmenu);
    }

    return FALSE;
}


/****************************************************************************
 *
 *  OnDebug
 *
 *  Description:
 *      Handles debug messages sent to the window.
 *
 *  Arguments:
 *      UINT [in]: signature.
 *      UINT [in]: debug level.
 *      LPCSTR [in]: debug text.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWindow::OnDebug"

BOOL
CWindow::OnDebug
(
    UINT                    nSignature,
    UINT                    nLevel,
    LPCSTR                  pszMessage,
    LRESULT *               plResult
)
{
    if((WM_DPF_SIGNATURE == nSignature) && (nLevel >= DPFLVL_FIRST) && (nLevel <= DPFLVL_LAST))
    {
        switch(nLevel)
        {
            case DPFLVL_ERROR:
            case DPFLVL_RESOURCE:
                MessageBox(pszMessage, MB_OK | MB_ICONERROR);
                break;

            case DPFLVL_WARNING:
                MessageBox(pszMessage, MB_OK | MB_ICONWARNING);
                break;

            default:
                // MessageBox(pszMessage, MB_OK | MB_ICONINFORMATION);
                break;
        }
    }

    *plResult = 0;

    return TRUE;
}


/****************************************************************************
 *
 *  CDialog
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

CDialog::CDialog
(
    void
)
{
}


/****************************************************************************
 *
 *  ~CDialog
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

CDialog::~CDialog
(
    void
)
{
    if(m_hwnd)
    {
        EndDialog(m_hwnd, 0);
        m_hwnd = NULL;
    }
}


/****************************************************************************
 *
 *  DispatchMessage
 *
 *  Description:
 *      Dispatches the next message in this window's queue.
 *
 *  Arguments:
 *      LPMSG [in]: message.
 *
 *  Returns:  
 *      BOOL: TRUE if the message has been dispatched.  FALSE if the caller
 *            should dispatch it.
 *
 ****************************************************************************/

BOOL
CDialog::DispatchMessage
(
    LPMSG                   pmsg
)
{
    if(IsDialogMessage(m_hwnd, pmsg))
    {
        return TRUE;
    }
    
    return CWindow::DispatchMessage(pmsg);
}


/****************************************************************************
 *
 *  DialogProc
 *
 *  Description:
 *      Dialog mesage handler.
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

BOOL CALLBACK
CDialog::DialogProc
(
    HWND                    hwnd,
    UINT                    uMsg,
    WPARAM                  wParam,
    LPARAM                  lParam
)
{
    LPCWINDOWCREATECTX      pCtx;
    CWindow *               pWindow;
    LRESULT                 lResult;
    BOOL                    fHandled;
    
    //
    // Get the "this" pointer from the window
    //

    if(WM_INITDIALOG == uMsg)
    {
        pCtx = (LPCWINDOWCREATECTX)lParam;
        pWindow = pCtx->pWindow;

        SetWindowLong(hwnd, DWL_USER, (LONG)pWindow);

        pWindow->m_hwnd = hwnd;
    }
    else
    {
        pWindow = (CWindow *)GetWindowLong(hwnd, DWL_USER);
    }

    if(pWindow)
    {
        if(WINDOW_SIGNATURE != pWindow->m_dwSignature)
        {
            DPF_INFO("Bogus this pointer from message %4.4x", uMsg);
            pWindow = NULL;
        }
    }

    //
    // Hand off to the dispatcher
    //

    if(pWindow)
    {
        fHandled = pWindow->HandleMessage(uMsg, wParam, lParam, &lResult);
    }
    else
    {
        fHandled = FALSE;
    }

    //
    // Handle destruction
    //
    
    if((WM_DESTROY == uMsg) && pWindow)
    {
        SetWindowLong(hwnd, DWL_USER, NULL);

        pWindow->m_hwnd = NULL;
    }

    //
    // Hand off to the default handler
    //

    if(!fHandled)
    {
        lResult = DefaultMessageHandler(hwnd, uMsg, wParam, lParam);
    }

    return lResult;
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

BOOL 
CDialog::HandleMessage
(
    UINT                    uMsg, 
    WPARAM                  wParam, 
    LPARAM                  lParam, 
    LRESULT *               plResult
)
{
    LPCWINDOWCREATECTX      pCtx;
    BOOL                    fHandled;
    
    switch(uMsg)
    {
        case WM_INITDIALOG:
            pCtx = (LPCWINDOWCREATECTX)lParam;
            fHandled = OnInitDialog(pCtx->pvContext, plResult);
            break;
        
        default:
            fHandled = CWindow::HandleMessage(uMsg, wParam, lParam, plResult);
            break;
    }

    return fHandled;
}


/****************************************************************************
 *
 *  OnCommand
 *
 *  Description:
 *      Command message dispatcher.
 *
 *  Arguments:
 *      UINT [in]: command identifier.
 *      UINT [in]: control identifier.
 *      HWND [in]: control window handle.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDialog::OnCommand"

BOOL
CDialog::OnCommand
(
    UINT                    nCommandId,
    UINT                    nControlId,
    HWND                    hwndControl,
    LRESULT *               plResult
)
{
    BOOL                    fHandled;

    fHandled = CWindow::OnCommand(nCommandId, nControlId, hwndControl, plResult);

    switch(nControlId)
    {
        case IDOK:
            OnOk();
            break;

        case IDCANCEL:
            OnCancel();
            break;
    }

    return fHandled;
}


/****************************************************************************
 *
 *  CModalDialog
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

CModalDialog::CModalDialog
(
    void
)
{
}


/****************************************************************************
 *
 *  ~CModalDialog
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

CModalDialog::~CModalDialog
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
 *      UINT [in]: dialog resource identifier.
 *      LPVOID [in]: window creation context (passed to OnInitDialog).
 *      CWindow * [in]: parent window handle.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

BOOL
CModalDialog::Create
(
    UINT                    nResourceId,
    LPVOID                  pvContext,
    CWindow *               pParent
)
{
    WINDOWCREATECTX         ctx;
    
    if(pParent)
    {
        SetParent(pParent);
    }

    ctx.pWindow = this;
    ctx.pvContext = pvContext;
    
    return DialogBoxParam(g_hInstance, MAKEINTRESOURCE(nResourceId), pParent ? pParent->m_hwnd : NULL, DialogProc, (LPARAM)&ctx);
}


/****************************************************************************
 *
 *  CModelessDialog
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

CModelessDialog::CModelessDialog
(
    void
)
{
}


/****************************************************************************
 *
 *  ~CModelessDialog
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

CModelessDialog::~CModelessDialog
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
 *      UINT [in]: dialog resource identifier.
 *      LPVOID [in]: window creation context (passed to OnInitDialog).
 *      CWindow * [in]: parent window handle.
 *
 *  Returns:  
 *      HWND: dialog window handle.
 *
 ****************************************************************************/

HWND
CModelessDialog::Create
(
    UINT                    nResourceId,
    LPVOID                  pvContext,
    CWindow *               pParent
)
{
    WINDOWCREATECTX         ctx;
    
    if(pParent)
    {
        SetParent(pParent);
    }

    ctx.pWindow = this;
    ctx.pvContext = pvContext;
    
    return CreateDialogParam(g_hInstance, MAKEINTRESOURCE(nResourceId), pParent ? pParent->m_hwnd : NULL, DialogProc, (LPARAM)&ctx);
}


