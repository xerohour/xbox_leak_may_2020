// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CToolbar.cpp
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

DWORD g_rgdwTooltip[] = {IDS_TOOLBAR_NEW, IDS_TOOLBAR_OPEN, IDS_TOOLBAR_SAVE, 0,
                         0, IDS_TOOLBAR_GROUP, IDS_TOOLBAR_UNGROUP, 0,
                         IDS_TOOLBAR_EMULATE, IDS_TOOLBAR_PREMASTER,
                         IDS_TOOLBAR_BFE, 0, IDS_TOOLBAR_VIEW0, IDS_TOOLBAR_VIEW1,
                         IDS_TOOLBAR_VIEWBOTH, IDS_TOOLBAR_VIEWSCRATCH, 0, IDS_TOOLBAR_VIEWRESCAN};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CToolBar::CToolBar
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CToolBar::Init(CWindow *pwindow)
{
    TBBUTTON rgtb[] =  {
        {STD_FILENEW, ID_FILE_NEW, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        {STD_FILEOPEN, ID_FILE_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        {STD_FILESAVE, ID_FILE_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
        {0, ID_EDIT_GROUP, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        {0, ID_EDIT_UNGROUP, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
        {0, ID_VIEW_LAYER0, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP, 0L, 0},
        {0, ID_VIEW_LAYER1, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP, 0L, 0},
        {0, ID_VIEW_BOTHLAYERS, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP, 0L, 0},
        {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
        {0, ID_VIEW_SCRATCHAREA, TBSTATE_ENABLED, TBSTYLE_CHECK, 0L, 0},
        {0, ID_VIEW_RESCANLAYOUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
        {0, ID_TOOLS_EMULATE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        {0, ID_TOOLS_PREMASTERTAPE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
    }; 

    // Create the Tool bar
    m_hwnd = CreateToolbarEx (pwindow->m_hwnd, WS_CHILD | WS_BORDER | WS_VISIBLE | TBSTYLE_TOOLTIPS |TBSTYLE_FLAT, 
                              1, 10, (HINSTANCE)HINST_COMMCTRL, IDB_STD_SMALL_COLOR, 
                              (LPCTBBUTTON)&rgtb, 4, 0, 0, 16, 16, sizeof (TBBUTTON));
    if (!m_hwnd)
        return false;

    // Add the non-standard buttons
    TBADDBITMAP tb;
    tb.hInst = pwindow->m_hinst;
    tb.nID = BMP_TOOLBAR;
    int iNewBmps = SendMessage (m_hwnd, TB_ADDBITMAP, 13, (LPARAM)&tb);

    rgtb[4].iBitmap = iNewBmps  + 0;
    rgtb[5].iBitmap = iNewBmps  + 1;
    rgtb[7].iBitmap = iNewBmps  + 2;
    rgtb[8].iBitmap = iNewBmps  + 3;
    rgtb[9].iBitmap = iNewBmps  + 4;
    rgtb[11].iBitmap = iNewBmps + 5;
    rgtb[12].iBitmap = iNewBmps + 6;
    rgtb[14].iBitmap = iNewBmps + 7;
    rgtb[15].iBitmap = iNewBmps + 8;

    SendMessage (m_hwnd, TB_ADDBUTTONS, 12, (LONG) &rgtb[4]);

    return true;
}

void CToolBar::CheckButton(int idButton, bool fDown)
{
    SendMessage(m_hwnd, TB_CHECKBUTTON, idButton, MAKELONG(fDown, 0));
}

void CToolBar::EnableButton(int idButton, bool fEnabled)
{
    SendMessage(m_hwnd, TB_SETSTATE, idButton, MAKELONG(fEnabled ? TBSTATE_ENABLED : 0, 0));
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CToolBar::~CToolBar
// Purpose:   
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CToolBar::~CToolBar()
{
    if (m_hwnd)
        DestroyWindow(m_hwnd);
}

void CToolBar::MoveTo(int nX, int nY, int nW, int nH)
{
    WINDOWPLACEMENT wp;
    wp.length = sizeof(wp);
    wp.rcNormalPosition.right = nW;
    SetWindowPlacement(m_hwnd, &wp);
}