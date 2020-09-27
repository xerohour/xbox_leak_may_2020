// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CColorbar.cpp
// Contents:  Contains the class definition for the CColorbar object.
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

    // Determine which CColorbar this message is actually intended for.
    CColorBar *pcb = (CColorBar*)GetWindowLong(hwnd, GWL_USERDATA);

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
// Function:  CColorBar::SetVisible
// Purpose:   Sets the visible state of the control to visible or hidden.
// Arguments: fVisible          -- True if the control should be shown.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CColorBar::SetVisible(bool fVisible)
{
    ShowWindow(m_hwnd, fVisible ? SW_SHOW : SW_HIDE);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CColorBar::CColorBar
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CColorBar::Init(CWindow *pwindow, COLORREF rgbTopColor, COLORREF rgbBotColor)
{
    // Create the hwnd
    m_hwnd = CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
                          pwindow->m_hwnd, NULL, pwindow->m_hinst, NULL);
    if (!m_hwnd)
        return false;

    // Subclass the colorbar so that we can override erasebackground messages
    m_pfnStaticWndProc = (WNDPROC)SetWindowLong(m_hwnd, GWL_WNDPROC, (long)ColorBarWndProc);

    // Point our hwnd at 'this' object
    SetWindowLong(m_hwnd, GWL_USERDATA, (long)this);

    m_hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
                            WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, m_hwnd, NULL, pwindow->m_hinst, NULL);

    
    SetWindowPos(m_hwndTip, HWND_TOPMOST,0, 0, 0, 0,
             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    SetColors(rgbTopColor, rgbBotColor);

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CColorBar::~CColorBar
// Purpose:   CColorBar destructor
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CColorBar::~CColorBar()
{
    if (m_hwnd)
        DestroyWindow(m_hwnd);
    if (m_hwndTip)
        DestroyWindow(m_hwndTip);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CColorBar::SetColors
// Purpose:   Sets the top and bottom colors that are interpolated across the
//            colorbar.
// Arguments: rgbTopColor       -- Color to display at the top of the object
//            rgbBotColor       -- Color to display at the bottom of the object
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CColorBar::SetColors(COLORREF rgbTopColor, COLORREF rgbBotColor)
{
    m_rgbTopColor = rgbTopColor;
    m_rgbBotColor = rgbBotColor;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CColorBar::MoveTo
// Purpose:   Called when the colorbar's owning window is resized, and this
//            object needs to resize (and repaint) itself
// Arguments: nX, nY            -- New location of the object
//            nW, nH            -- New dimensions of the object
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CColorBar::MoveTo(int nX, int nY, int nW, int nH)
{
    RECT rc = {0, 0, nW, nH};

    // Keep track of our new coordinates/dimensions
    m_rc = rc;

    // Move the Win32 window that encapsulates this object
    MoveWindow(m_hwnd, nX, nY, nW, nH, FALSE);

    // Repaint the window
    HDC hdc = GetDC(m_hwnd);
    Repaint(hdc);
    ReleaseDC(m_hwnd, hdc);

    // Update our tooltip's position
    TOOLINFO ti;

    char strTT[30] = "This is your tooltip string.";
    LPTSTR lptstr = strTT;
    
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = m_hwnd;
    ti.hinst = g_hinst;
    ti.uId = 0;
    ti.lpszText = LPSTR_TEXTCALLBACK;//"Green = Faster\nRed = Slower";
    ti.rect = rc;
    
    // Delete Previous tool (if any)
//    SendMessage(m_hwndTip, TTM_DELTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

    // Send an ADDTOOL message to the tooltip control window
    SendMessage(m_hwndTip, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CColorBar::Repaint
// Purpose:   Called when the colorbar needs to refresh its onscreen display
// Arguments: hdc               -- Device context to render to.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CColorBar::Repaint(HDC hdc)
{
    // Setup the necessary structures for the Win32 'GradientFill' call.
    TRIVERTEX rgtv[] = {m_rc.left, m_rc.top,
                        GetRValue(m_rgbTopColor)<<8,
                        GetGValue(m_rgbTopColor)<<8,
                        GetBValue(m_rgbTopColor)<<8, 0,
                        m_rc.right, m_rc.bottom,
                        GetRValue(m_rgbBotColor)<<8,
                        GetGValue(m_rgbBotColor)<<8,
                        GetBValue(m_rgbBotColor)<<8, 0};

    GRADIENT_RECT rggr = {0, 1};
    GradientFill(hdc, rgtv, 2, &rggr, 1, GRADIENT_FILL_RECT_V);
}

