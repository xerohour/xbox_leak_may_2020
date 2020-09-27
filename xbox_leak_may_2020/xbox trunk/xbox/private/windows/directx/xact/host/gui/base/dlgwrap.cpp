/***************************************************************************
 *
 *  Copyright (C) 2/20/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dlgwrap.cpp
 *  Content:    Dialog wrapper classes.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/20/2002   dereks  Created.
 *
 ****************************************************************************/

#include "xactgui.h"


/****************************************************************************
 *
 *  CDialog
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      UINT [in]: dialog resource identifier.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDialog::CDialog"

CDialog::CDialog
(
    UINT                    nDialogResourceId
)
:   CWindow(-1, -1, WNDTYPE),
    m_nDialogResourceId(nDialogResourceId)
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

#undef DPF_FNAME
#define DPF_FNAME "CDialog::~CDialog"

CDialog::~CDialog
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
 *      Creates the dialog.
 *
 *  Arguments:
 *      CWindow * [in]: parent window.
 *      LPVOID [in]: creation parameter.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDialog::Create"

BOOL
CDialog::Create
(
    CWindow *               pParent,
    LPVOID                  pvContext
)
{
    WINDOWCREATECTX         ctx;

    m_pParent = pParent;

    ctx.dwSignature = m_dwValidSignature;
    ctx.pWindow = this;
    ctx.pvContext = pvContext;

    return MAKEBOOL(CreateDialogParam(g_hInstance, MAKEINTRESOURCE(m_nDialogResourceId), GetHWND(pParent), DialogProc, (LPARAM)&ctx));
}


/****************************************************************************
 *
 *  DoModal
 *
 *  Description:
 *      Creates the dialog.
 *
 *  Arguments:
 *      CWindow * [in]: parent window.
 *      LPVOID [in]: creation parameter.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDialog::DoModal"

BOOL
CDialog::DoModal
(
    CWindow *               pParent,
    LPVOID                  pvContext
)
{
    HWND                    hWndFocus;
    WINDOWCREATECTX         ctx;
    BOOL                    fSuccess;

    m_pParent = pParent;

    //
    // Save the window with the keyboard focus so we can restore it later
    //

    hWndFocus = GetFocus();

    //
    // Show the dialog
    //

    ctx.dwSignature = m_dwValidSignature;
    ctx.pWindow = this;
    ctx.pvContext = pvContext;

    fSuccess = DialogBoxParam(g_hInstance, MAKEINTRESOURCE(m_nDialogResourceId), GetHWND(pParent), DialogProc, (LPARAM)&ctx);

    //
    // Restore focus
    //

    SetFocus(hWndFocus);

    return fSuccess;
}


/****************************************************************************
 *
 *  OnCommand
 *
 *  Description:
 *      Handles WM_COMMAND messages.
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
    LPCTSTR                 pszPath;
    
    if(CWindow::OnCommand(nCommandId, nControlId, hwndControl, plResult))
    {
        return TRUE;
    }

    switch(nControlId)
    {
        case IDOK:
            OnCmdOK();
            break;
        
        case IDCANCEL:
            OnCmdCancel();
            break;
    }

    return FALSE;
}


/****************************************************************************
 *
 *  OnClose
 *
 *  Description:
 *      Handles WM_CLOSE messages.
 *
 *  Arguments:
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDialog::OnClose"

BOOL
CDialog::OnClose
(
    LRESULT *               plResult
)
{
    OnCmdCancel();

    *plResult = 0;
    return TRUE;
}


/****************************************************************************
 *
 *  OnCmdOK
 *
 *  Description:
 *      Handles the OK command.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDialog::OnCmdOK"

void
CDialog::OnCmdOK
(
    void
)
{
    EndDialog(m_hWnd, TRUE);
}


/****************************************************************************
 *
 *  OnCmdCancel
 *
 *  Description:
 *      Handles the Cancel command.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDialog::OnCmdCancel"

void
CDialog::OnCmdCancel
(
    void
)
{
    EndDialog(m_hWnd, FALSE);
}


/****************************************************************************
 *
 *  DialogProc
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
#define DPF_FNAME "CDialog::DialogProc"

int CALLBACK
CDialog::DialogProc
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
    
    if(WM_INITDIALOG == uMsg)
    {
        pWindow = OnInitialCreate(hWnd, &lParam);
    }
    else
    {
        pWindow = GetWindowContext(hWnd);
    }

    if(!pWindow)
    {
        return FALSE;
    }

    //
    // Hand off to the message dispatcher
    //

    if(pWindow->HandleMessage(uMsg, wParam, lParam, &lResult))
    {
        SetWindowLong(hWnd, DWL_MSGRESULT, lResult);
        lResult = TRUE;
    }
    else
    {
        lResult = FALSE;
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
#define DPF_FNAME "CDialog::OnInitialCreate"

CWindow *
CDialog::OnInitialCreate
(
    HWND                    hWnd,
    LPARAM *                plParam
)
{
    CWindow *               pWindow     = NULL;
    LRESULT                 lResult     = 0;
    LPCWINDOWCREATECTX      pCtx;
    BOOL                    fHandled;

    ASSERT(hWnd);
    ASSERT(plParam);

    //
    // Get the CWindow object from the message parameters
    //

    pCtx = (LPCWINDOWCREATECTX)*plParam;
    ASSERT(pCtx);

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
#define DPF_FNAME "CDialog::HandleMessage"

BOOL 
CDialog::HandleMessage
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
        case WM_CREATE:
            break;

        case WM_INITDIALOG:
            fHandled = OnCreate((LPVOID)lParam, plResult);
            break;
        
        default:
            fHandled = CWindow::HandleMessage(uMsg, wParam, lParam, plResult);
            break;
    }

    return fHandled;
}


