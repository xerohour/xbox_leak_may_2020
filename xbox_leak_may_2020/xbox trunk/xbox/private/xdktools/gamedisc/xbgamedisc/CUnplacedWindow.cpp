// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CUnplacedWindow.cpp
// Contents:  
// Revisions: 31-Oct-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- stdafxiled header file
#include "stdafx.h"


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CUnplacedWindow *g_punplacedTemp = NULL;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  UnplacedWndProc
// Purpose:   Subclassed window procedure for the unplaced window.
// Arguments: hwnd              -- The handle to the window that the message is
//                                 intended for.
//            uMsg              -- The message being reported.
//            wparam            -- Parameter 1 (message-specific)
//            lparam            -- Parameter 2 (message-specific)
// Return:    Result of the message processing - depends on the message sent.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LONG FAR PASCAL UnplacedWndProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam)
{
    RECT rc = {0};
    WINDOWPLACEMENT wp;

    // Determine which CUnplacedWindow this message is actually intended for.
    CUnplacedWindow *punplaced = (CUnplacedWindow*)GetWindowLong(hwnd, GWL_USERDATA);
    if (!punplaced)
    {
        assert(g_punplacedTemp);
        SetWindowLong(hwnd, GWL_USERDATA, (long)g_punplacedTemp);
        punplaced = g_punplacedTemp;
        g_punplacedTemp = NULL;
    }

    switch (uMsg)
    {
    case WM_SIZE:
        rc.right = LOWORD(lparam);
        rc.bottom = HIWORD(lparam) - punplaced->m_statusbar.GetHeight();
        punplaced->m_listview.Resize(&rc);
        punplaced->m_statusbar.Resize(LOWORD(lparam), HIWORD(lparam));        
        // fallthru

    case WM_MOVE:
        wp.length = sizeof wp;
        GetWindowPlacement(hwnd, &wp);

        g_nUnplacedWindowX = wp.rcNormalPosition.left;
        g_nUnplacedWindowY = wp.rcNormalPosition.top;
        g_nUnplacedWindowW = wp.rcNormalPosition.right  - wp.rcNormalPosition.left;
        g_nUnplacedWindowH = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
        break;  
        
 //   case WM_SHOWWINDOW:
   //     if (wparam == TRUE)
     //       SetForegroundWindow(punplaced->m_hwndParent);
  //          ShowWindow(punplaced->m_hwndParent, SW_SHOW);
  //      break;

    case WM_CLOSE:
        // User clicked on the 'close' button on the unplaced window.  Tell the
        // main window to hide us
        SendMessage(punplaced->m_hwndParent, WM_COMMAND, IDM_VIEW_UNPLACED, 0);
        g_tbm.CheckButton(TB_LAYER, IDM_VIEW_UNPLACED, 0);

        // Return zero so that we tell the OS "we handled the message; DON'T
        // close the window"
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wparam, lparam);
}

BOOL CUnplacedWindow::Init(CViewMode *pvmParent, HWND hwndParent)
{
    WNDCLASSEX wcex;
    static bool sfRegedUnplaced = false;

    m_hwndParent = hwndParent;
    m_pvmParent = pvmParent;
    m_fVisible = FALSE;
    
    if (!sfRegedUnplaced)
    {
        // Register the class for the app's window.
        memset(&wcex, 0, sizeof wcex);
        wcex.cbSize        = sizeof wcex; 
        wcex.lpfnWndProc   = UnplacedWndProc;
        wcex.hInstance     = g_hInst;
        wcex.lpszClassName = "UnplacedWindow";
        if (!RegisterClassEx(&wcex))
            return NULL;
        sfRegedUnplaced = true;
    }

    assert(g_punplacedTemp == NULL);
    g_punplacedTemp = this;
    m_hwnd = CreateWindowEx(WS_EX_TOOLWINDOW,
                            "UnplacedWindow", "Unplaced Files Window",
                            WS_POPUP | WS_CLIPCHILDREN | WS_CAPTION |
                            WS_THICKFRAME | WS_CLIPSIBLINGS | WS_SYSMENU,
                            g_nUnplacedWindowX, g_nUnplacedWindowY,
                            g_nUnplacedWindowW, g_nUnplacedWindowH,
                            hwndParent, NULL, g_hInst, NULL);

    if (!m_listview.Init(m_hwnd))
        return FALSE;

    // Initialize the status bar
    m_statusbar.Init(m_hwnd);
    m_statusbar.SetNumParts(1);
    
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    m_statusbar.Resize(rc.right, rc.bottom);
    
    rc.bottom -= m_statusbar.GetHeight();
    m_listview.Resize(&rc);
    return TRUE;
}
