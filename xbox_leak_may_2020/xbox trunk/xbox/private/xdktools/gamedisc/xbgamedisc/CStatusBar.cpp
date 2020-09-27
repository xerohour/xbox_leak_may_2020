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
bool CStatusBar::Init(HWND hwndParent)
{
    m_nW = 100;

    // Create the status bar
    m_hwnd = CreateStatusWindow(WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP, NULL,
                                hwndParent, 2);

    if (!m_hwnd)
        return false;

    m_fDeferUpdate = false;
    SetNumParts(2);

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

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CStatusBar::SetNumParts(int nParts)
{
    m_nParts = nParts;
    
    int rgn2[] = {(int)((float)m_nW*.85), -1};
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
    strcpy(m_rgszText[iRegion], szText);
    if (!m_fDeferUpdate)
        SendMessage(m_hwnd, SB_SETTEXT, iRegion, (LPARAM) szText);
}

void CStatusBar::Update()
{
    SendMessage(m_hwnd, SB_SETTEXT, 0, (LPARAM) m_rgszText[0]);
    SendMessage(m_hwnd, SB_SETTEXT, 1, (LPARAM) m_rgszText[1]);
    m_fDeferUpdate = false;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStatusBar::Resize
// Purpose:   
// Arguments: 
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CStatusBar::Resize(int nW, int nH)
{
    MoveWindow(m_hwnd, 0, nH - 20, nW, nH, TRUE);

    m_nW = nW;
    // Set the size of the status bar areas
    int rgnRegionSizes[2];
    if (m_nParts == 1)
        rgnRegionSizes[0] = -1;
    else
    {
        rgnRegionSizes[0] = (int)((float)m_nW * .85) - 2;
        rgnRegionSizes[1] = -1;
    }
    SendMessage(m_hwnd, SB_SETPARTS, m_nParts, (LPARAM)rgnRegionSizes);
}

int CStatusBar::GetHeight()
{
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    return rc.bottom - rc.top;
}
