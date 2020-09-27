// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CSplitter.cpp
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

CSplitter *g_psplitterTemp = NULL;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  SplitterWndProc
// Purpose:   Subclassed window procedure for the Splitter controls.
// Arguments: hwnd              -- The handle to the window that the message is
//                                 intended for.
//            uMsg              -- The message being reported.
//            wparam            -- Parameter 1 (message-specific)
//            lparam            -- Parameter 2 (message-specific)
// Return:    Result of the message processing - depends on the message sent.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LONG FAR PASCAL SplitterWndProc(HWND hwnd, UINT uMsg, WORD wparam, LONG lparam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    
//    TrackMessage(uMsg, wparam, lparam, "Splitter");

    // Determine which CSplitter this message is actually intended for.
    CSplitter *psplitter = (CSplitter*)GetWindowLong(hwnd, GWL_USERDATA);
    if (!psplitter)
    {
        assert(g_psplitterTemp);
        SetWindowLong(hwnd, GWL_USERDATA, (long)g_psplitterTemp);
        psplitter = g_psplitterTemp;
        g_psplitterTemp = NULL;
    }

    switch(uMsg)
    {
    case WM_LBUTTONDOWN:
        psplitter->HandleMouseDown((signed short) LOWORD(lparam), (signed short) HIWORD(lparam));
        break;

    case WM_LBUTTONUP:
        psplitter->HandleMouseUp((signed short) LOWORD(lparam), (signed short) HIWORD(lparam));
        break;

    case WM_MOUSEMOVE:
        psplitter->HandleMouseMove((signed short) LOWORD(lparam), (signed short) HIWORD(lparam));
        break;

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, GetSysColorBrush(COLOR_BTNFACE));
        EndPaint(hwnd, &ps);
        break;
    }

    return DefWindowProc(hwnd, uMsg, wparam, lparam);
}


void CSplitter::HandleMouseDown(int nClientX, int nClientY)
{
    RECT rcParent;
    GetClientRect(m_hwndParent, &rcParent);

    // start drag
    m_fDragging = true;
    m_nDragDeltaX = nClientX;
    SetCapture(m_hwnd);
}

void CSplitter::HandleMouseMove(int nClientX, int nClientY)
{
    if (!m_fDragging)
        return;

    RECT rcParent;
    GetClientRect(m_hwndParent, &rcParent);

    POINT pt = {nClientX, nClientY};
    MapWindowPoints(m_hwnd, m_hwndParent, &pt, 1);

    pt.x = max(50, pt.x);
    pt.x = min(rcParent.right - 50, pt.x);
    m_nPos = pt.x - m_nDragDeltaX;
    m_nPermPos = pt.x - m_nDragDeltaX;
    m_pvmParent->Resize(rcParent.right - rcParent.left, rcParent.bottom - rcParent.top);
    UpdateWindow(m_hwnd);
}

void CSplitter::HandleMouseUp(int nClientX, int nClientY)
{
    // stop drag
    m_fDragging = false;
    ReleaseCapture();
}

bool gs_fRegedSplitter = false;
BOOL CSplitter::Init(CViewMode *pvmParent, HWND hwndParent, int nStartPos)
{
    WNDCLASSEX wcex;

    m_nWidth = 4;
    m_fDragging = false;
    m_hwndParent = hwndParent;
    m_pvmParent = pvmParent;
    m_nPos = nStartPos;
    m_nPermPos = nStartPos;

    if (!gs_fRegedSplitter)
    {
        // Register the class for the app's window.
        wcex.cbSize        = sizeof(WNDCLASSEX); 
        wcex.style         = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc   = (WNDPROC)SplitterWndProc;
        wcex.hInstance     = g_hInst;
        wcex.hCursor       = LoadCursor(NULL, IDC_SIZEWE);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
        wcex.lpszClassName = "MySplitter";
        wcex.cbClsExtra    = 0;
        wcex.cbWndExtra    = 0;
        wcex.hIcon         = 0;
        wcex.lpszMenuName  = NULL;
        wcex.hIconSm       = 0;
        if (!RegisterClassEx(&wcex))
            return NULL;
        gs_fRegedSplitter = true;
    }

    assert(g_psplitterTemp == NULL);
    g_psplitterTemp = this;
    m_hwnd = CreateWindow("MySplitter", "", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, 0, 0,
                          hwndParent, (HMENU)WNDID_SPLITTER, g_hInst, NULL);
    
    return TRUE;
}

void CSplitter::Resize(RECT *prc)
{
    MoveWindow(m_hwnd, prc->left, prc->top, prc->right - prc->left,
               prc->bottom - prc->top, TRUE);
}
