// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      cstatusbar.cpp
// Contents:  
// Revisions: 14-Jun-2001: Created (jeffsim)
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
// Function:  CStatusBar::CStatusBar
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CStatusBar::Init(CWindow *pwindow)
{
    m_nW = 100;
    // Create the status bar
    m_hwnd = CreateStatusWindow(WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP, NULL,
                                pwindow->m_hwnd, 2);

    if (!m_hwnd)
        return false;

    SetNumParts(1);

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStatusBar::~CStatusBar
// Purpose:   
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CStatusBar::~CStatusBar()
{
    if (m_hwnd)
        DestroyWindow(m_hwnd);
}

void CStatusBar::SetNumParts(int nParts)
{
    m_nParts = nParts;
    
    int rgn1[] = {-1};
    int rgn2[] = {m_nW/2-1, -1};
    if (nParts == 1)
        SendMessage(m_hwnd, SB_SETPARTS, nParts, (LPARAM) (LPINT) rgn1);
    else
        SendMessage(m_hwnd, SB_SETPARTS, nParts, (LPARAM) (LPINT) rgn2);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStatusBar::SetText
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CStatusBar::SetText(int iRegion, char *szText)
{
    SendMessage(m_hwnd, SB_SETTEXT, iRegion, (LPARAM) szText);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStatusBar::MoveTo
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CStatusBar::MoveTo(int nX, int nY, int nW, int nH)
{
    MoveWindow(m_hwnd, nX, nY, nW, nH, FALSE);

    m_nW = nW;
    // Set the size of the status bar areas
    int rgnRegionSizes[2];
    if (m_nParts == 1)
        rgnRegionSizes[0] = -1;
    else
    {
        rgnRegionSizes[0] = nW / 2-2;
        rgnRegionSizes[1] = -1;
    }
    SendMessage(m_hwnd, SB_SETPARTS, m_nParts, (LPARAM)rgnRegionSizes);
}
