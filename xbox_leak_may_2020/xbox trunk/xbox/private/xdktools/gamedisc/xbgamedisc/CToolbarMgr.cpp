// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CToolbar.cpp
// Contents:  
// Revisions: 29-Oct-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL InitToolbars()
{
    REBARBANDINFO rbbi;

    TBBUTTON rgtb[] = {
        {STD_FILENEW,  IDM_FILE_NEW,  TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        {STD_FILEOPEN, IDM_FILE_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        {STD_FILESAVE, IDM_FILE_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        {0,            0,            TBSTATE_ENABLED, TBSTYLE_SEP,    0L, 0},
        {0, IDM_VIEW_FILE,           TBSTATE_ENABLED, TBSTYLE_CHECKGROUP, 0L, 0},
        {0, IDM_VIEW_LAYOUT,         TBSTATE_ENABLED, TBSTYLE_CHECKGROUP, 0L, 0},
        {0,            0,            TBSTATE_ENABLED, TBSTYLE_SEP,    0L, 0},
        {0, IDM_TOOLS_EMULATE,       TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        {0, IDM_TOOLS_PREMASTER,     TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        {0, IDM_TOOLS_PERF,          TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0}};

    HWND hwndTB = CreateToolbarEx(g_tbm.GetRebarHwnd(), WS_CHILD | WS_VISIBLE|
                                  TBSTYLE_TOOLTIPS | TBSTYLE_FLAT |
                                  CCS_NODIVIDER | CCS_NORESIZE, 
                                  1, 3, (HINSTANCE)HINST_COMMCTRL,
                                  IDB_STD_SMALL_COLOR,  (LPCTBBUTTON)&rgtb,
                                  4, 0, 22, 16, 16, sizeof (TBBUTTON));

    // Add the non-standard buttons
    TBADDBITMAP tb;
    tb.hInst = g_hInst;
    tb.nID = BMP_TOOLBAR_GEN;
    int iNewBmps = SendMessage (hwndTB, TB_ADDBITMAP, 5, (LPARAM)&tb);

    rgtb[4].iBitmap = iNewBmps  + 0;
    rgtb[5].iBitmap = iNewBmps  + 1;
    rgtb[7].iBitmap = iNewBmps  + 2;
    rgtb[8].iBitmap = iNewBmps  + 3;
    rgtb[9].iBitmap = iNewBmps  + 4;

    SendMessage (hwndTB, TB_ADDBUTTONS, 6, (LONG) &rgtb[4]);
    
    DWORD dwBtnSize = SendMessage(hwndTB, TB_GETBUTTONSIZE, 0, 0);


    ZeroMemory(&rbbi, sizeof(rbbi));
    rbbi.cbSize       = sizeof(REBARBANDINFO);
    rbbi.fMask        = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE |
                        RBBIM_SIZE | RBBIM_ID;
    rbbi.fStyle       = RBBS_CHILDEDGE | RBBS_GRIPPERALWAYS;
    rbbi.cxMinChild   = 0;
    rbbi.cyMinChild   = HIWORD(dwBtnSize);
    rbbi.cx           = 250;
    rbbi.cxMinChild   = 50;
    rbbi.wID          = TB_GLOBAL;
    rbbi.hwndChild    = hwndTB;
    g_tbm.AddBand(&rbbi);

    // And now the 'view toolbar'
    TBBUTTON rgtb2[] = {
        {0, IDM_EDIT_GROUP,   TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        {0, IDM_EDIT_UNGROUP, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        {0, 0,                TBSTATE_ENABLED, TBSTYLE_SEP,    0L, 0},
        {0, IDM_VIEW_LAYER0,  TBSTATE_ENABLED, TBSTYLE_CHECKGROUP, 0L, 0},
        {0, IDM_VIEW_LAYER1,  TBSTATE_ENABLED, TBSTYLE_CHECKGROUP, 0L, 0},
        {0, IDM_VIEW_BOTHLAYERS,TBSTATE_ENABLED, TBSTYLE_CHECKGROUP, 0L, 0},
        {0, IDM_VIEW_UNPLACED,TBSTATE_ENABLED, TBSTYLE_CHECK, 0L, 0},
        {0, 0,                TBSTATE_ENABLED, TBSTYLE_SEP,    0L, 0},
        {0, IDM_EDIT_FIND,    TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        {0, IDM_EDIT_RESCAN,  TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0}};

    hwndTB = CreateToolbarEx(g_tbm.GetRebarHwnd(), WS_CHILD | WS_VISIBLE|
                             TBSTYLE_TOOLTIPS | TBSTYLE_FLAT |
                             CCS_NODIVIDER | CCS_NORESIZE, 
                             0, 0, NULL,
                             IDB_STD_SMALL_COLOR,  (LPCTBBUTTON)&rgtb2,
                             0, 0, 22, 16, 16, sizeof (TBBUTTON));

    // Add the non-standard buttons
    tb.hInst = g_hInst;
    tb.nID = IDB_TOOLBAR_LAYERVIEW;
    iNewBmps = SendMessage (hwndTB, TB_ADDBITMAP, 8, (LPARAM)&tb);

    rgtb2[0].iBitmap = iNewBmps + 0;
    rgtb2[1].iBitmap = iNewBmps + 1;
    rgtb2[3].iBitmap = iNewBmps + 2;
    rgtb2[4].iBitmap = iNewBmps + 3;
    rgtb2[5].iBitmap = iNewBmps + 4;
    rgtb2[6].iBitmap = iNewBmps + 5;
    rgtb2[8].iBitmap = iNewBmps + 6;
    rgtb2[9].iBitmap = iNewBmps + 7;

    SendMessage (hwndTB, TB_ADDBUTTONS, 10, (LONG) rgtb2);
    
    ZeroMemory(&rbbi, sizeof(rbbi));
    rbbi.cbSize       = sizeof(REBARBANDINFO);
    rbbi.fMask        = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE |
                        RBBIM_SIZE | RBBIM_ID;
    rbbi.fStyle       = RBBS_CHILDEDGE | RBBS_GRIPPERALWAYS;
    rbbi.cxMinChild   = 50;
    rbbi.cyMinChild   = HIWORD(dwBtnSize);
    rbbi.cx           = 100;
    rbbi.wID          = TB_LAYER;
    rbbi.hwndChild    = hwndTB;
    g_tbm.AddBand(&rbbi);
    g_tbm.Show(TB_LAYER);

    return TRUE;
}

BOOL CToolbarMgr::Init(HWND hwndParent)
{
    REBARINFO rbi;
 
    InitCommonControls();

    // Create the Rebar itself.  This Win32 object will contain the actual
    // toolbars in 'bands' that are subsequently added
    m_hwnd = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, NULL,
                            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS |
                            RBS_VARHEIGHT | RBS_BANDBORDERS|
                            WS_CLIPCHILDREN | CCS_NODIVIDER,
                            0, 0, 0, 0, hwndParent, (HMENU)WNDID_TOOLBARMGR, g_hInst, NULL);

    if (m_hwnd == NULL)
        return FALSE;

    // Initialize and send the REBARINFO structure.
    rbi.cbSize = sizeof(REBARINFO);
    rbi.fMask  = 0;
    rbi.himl   = (HIMAGELIST)NULL;
    if(!SendMessage(m_hwnd, RB_SETBARINFO, 0, (LPARAM)&rbi))
        return FALSE;

    m_hwndParent = hwndParent;
    return TRUE;
}

BOOL CToolbarMgr::AddBand(REBARBANDINFO *prbbi)
{
    SendMessage(m_hwnd, RB_INSERTBAND, (WPARAM)(UINT) prbbi->wID, (LPARAM)(LPREBARBANDINFO)prbbi);

    // By default bands are visible at the start.
    Show(prbbi->wID);
    
    return TRUE;
}

void CToolbarMgr::Show(DWORD dwToolbar)
{
    // Make toolbar visible
    WPARAM wId = SendMessage(m_hwnd, RB_IDTOINDEX, dwToolbar, 0);
    SendMessage(m_hwnd, RB_SHOWBAND, wId, (LPARAM)TRUE);
}

void CToolbarMgr::Hide(DWORD dwToolbar)
{
    // Make toolbar invisible
    WPARAM wId = SendMessage(m_hwnd, RB_IDTOINDEX, dwToolbar, 0);
    SendMessage(m_hwnd, RB_SHOWBAND, wId, (LPARAM)FALSE);
}

void CToolbarMgr::Resize(WPARAM wparam, LPARAM lparam)
{
    SendMessage(m_hwnd, WM_SIZE, wparam, lparam);
}

DWORD CToolbarMgr::GetRowsHeight()
{
    return SendMessage(m_hwnd, RB_GETBARHEIGHT, 0, 0)+2;
}

void CToolbarMgr::HandleNotify(LPNMHDR pnmhdr)
{
    switch (pnmhdr->code)
    {
    case RBN_HEIGHTCHANGE:
        SendMessage(m_hwndParent, UM_TOOLBARRESIZED, 0, 0);
        break;
    }
}

void CToolbarMgr::CheckButton(int idBand, int idButton, bool fDown)
{
    REBARBANDINFO rbbi;
    rbbi.cbSize = sizeof(REBARBANDINFO);
    rbbi.fMask  = RBBIM_CHILD;
    WPARAM wId = SendMessage(m_hwnd, RB_IDTOINDEX, idBand, 0);
    SendMessage(m_hwnd, RB_GETBANDINFO, wId, (LPARAM)&rbbi);
    SendMessage(rbbi.hwndChild, TB_CHECKBUTTON, idButton, MAKELONG(fDown, 0));
}

void CToolbarMgr::EnableButton(int idBand, int idButton, bool fEnabled)
{
    REBARBANDINFO rbbi;
    rbbi.cbSize = sizeof(REBARBANDINFO);
    rbbi.fMask  = RBBIM_CHILD;
    WPARAM wId = SendMessage(m_hwnd, RB_IDTOINDEX, idBand, 0);
    SendMessage(m_hwnd, RB_GETBANDINFO, wId, (LPARAM)&rbbi);
    DWORD dwState = SendMessage(rbbi.hwndChild, TB_GETSTATE, idButton, 0);
    dwState = (dwState & TBSTATE_CHECKED) | (fEnabled ? TBSTATE_ENABLED : 0);
    SendMessage(rbbi.hwndChild, TB_SETSTATE, idButton, MAKELONG(dwState, 0));
}

BOOL CToolbarMgr::IsButtonChecked(int idBand, int idButton)
{
    REBARBANDINFO rbbi;
    rbbi.cbSize = sizeof(REBARBANDINFO);
    rbbi.fMask  = RBBIM_CHILD;
    WPARAM wId = SendMessage(m_hwnd, RB_IDTOINDEX, idBand, 0);
    SendMessage(m_hwnd, RB_GETBANDINFO, wId, (LPARAM)&rbbi);
    DWORD dwState = SendMessage(rbbi.hwndChild, TB_GETSTATE, idButton, 0);

    return dwState & TBSTATE_CHECKED;
}