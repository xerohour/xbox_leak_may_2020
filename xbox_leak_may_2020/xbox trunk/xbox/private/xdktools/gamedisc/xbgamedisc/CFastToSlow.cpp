// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CFastToSlow.cpp
// Contents:  Contains the class definition for the CFastToSlow object.
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ColorBarWndProc
// Purpose:   Subclassed window procedure for the color bar static controls.
// Arguments: hwnd              -- The handle to the window that the message is
//                                 intended for.
//            uMsg              -- The message being reported.
//            wparam            -- Parameter 1 (message-specific)
//            lparam            -- Parameter 2 (message-specific)
// Return:    Result of the message processing - depends on the message sent.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LONG FAR PASCAL ColorBarWndProc(HWND hwnd, UINT uMsg, WORD wparam, LONG lparam)
{
    PAINTSTRUCT ps;

    // Determine which CFastToSlow this message is actually intended for.
    CFastToSlow *pcb = (CFastToSlow*)GetWindowLong(hwnd, GWL_USERDATA);

    switch(uMsg)
    {
    case WM_PAINT:
        BeginPaint(hwnd, &ps);
        pcb->Repaint(ps.hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    // If we didn't handle the message above, then pass the message on to the
    // original wndproc for this control.
    return CallWindowProc(pcb->m_pfnStaticWndProc, hwnd, uMsg, wparam, lparam);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFastToSlow::CFastToSlow
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CFastToSlow::Init(HWND hwndParent, COLORREF rgbTopColor, COLORREF rgbBotColor)
{
    // Create the hwnd
    m_hwnd = CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
                          hwndParent, NULL, g_hInst, NULL);
    if (!m_hwnd)
        return false;

    // Subclass the colorbar so that we can override erasebackground messages
    m_pfnStaticWndProc = (WNDPROC)SetWindowLong(m_hwnd, GWL_WNDPROC, (long)ColorBarWndProc);

    // Point our hwnd at 'this' object
    SetWindowLong(m_hwnd, GWL_USERDATA, (long)this);

    m_hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
                            WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, m_hwnd, NULL, g_hInst, NULL);

    
    SetWindowPos(m_hwndTip, HWND_TOPMOST,0, 0, 0, 0,
             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    m_rgbTopColor = rgbTopColor;
    m_rgbBotColor = rgbBotColor;

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFastToSlow::~CFastToSlow
// Purpose:   CFastToSlow destructor
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CFastToSlow::~CFastToSlow()
{
    if (m_hwnd)
        DestroyWindow(m_hwnd);
    if (m_hwndTip)
        DestroyWindow(m_hwndTip);
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFastToSlow::Resize
// Purpose:   Called when the colorbar's owning window is resized, and this
//            object needs to resize (and repaint) itself
// Arguments: prc       -- New dimensions of the object
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CFastToSlow::Resize(RECT *prc)
{
    m_rc = *prc;
    MoveWindow(m_hwnd, prc->left, prc->top, prc->right - prc->left,
               prc->bottom - prc->top, TRUE);
    InvalidateRect(m_hwnd, NULL, TRUE);

    // Update our tooltip's position
    TOOLINFO ti;
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = g_hwndMain;
    ti.hinst = g_hInst;
    ti.uId = 0;
    ti.lpszText = MAKEINTRESOURCE(IDS_COLORBAR_TOOLTIP);
    ti.rect = m_rc;

    SendMessage(m_hwndTip, TTM_SETMAXTIPWIDTH, 0, 80);

 
    // Delete Previous tool (if any)
    SendMessage(m_hwndTip, TTM_DELTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

    // Send an ADDTOOL message to the tooltip control window
    SendMessage(m_hwndTip, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFastToSlow::Repaint
// Purpose:   Called when the colorbar needs to refresh its onscreen display
// Arguments: hdc               -- Device context to render to.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CFastToSlow::Repaint(HDC hdc)
{
    // Setup the necessary structures for the Win32 'GradientFill' call.
    TRIVERTEX rgtv[] = {0, 0,
                        GetRValue(m_rgbTopColor)<<8,
                        GetGValue(m_rgbTopColor)<<8,
                        GetBValue(m_rgbTopColor)<<8, 0,
                        m_rc.right-m_rc.left, m_rc.bottom-m_rc.top,
                        GetRValue(m_rgbBotColor)<<8,
                        GetGValue(m_rgbBotColor)<<8,
                        GetBValue(m_rgbBotColor)<<8, 0};

    GRADIENT_RECT rggr = {0, 1};
    GradientFill(hdc, rgtv, 2, &rggr, 1, GRADIENT_FILL_RECT_V);

    if (m_rc.left == 0)
    {
        RECT rc = {m_rc.right-1, 0, m_rc.right, m_rc.bottom-m_rc.top};
        FillRect(hdc, &rc, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
    }
    else
    {
        RECT rc = {0, 0, 1, m_rc.bottom-m_rc.top};
        FillRect(hdc, &rc, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
    }
}

