// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CViewMode_Explorer.cpp
// Contents:  
// Revisions: 31-Oct-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"

CViewMode_Explorer g_vmExplorer;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL CViewMode_Explorer::Init(HWND hwndParent)
{
    RECT rc;

    // Initialize the treeview and listview.
    if (!m_treeview.Init(hwndParent))
        return FALSE;
    if (!m_listview.Init(hwndParent))
        return FALSE;

    // By default, split is halfway through window.
    if (!m_splitter.Init(this, hwndParent, g_nSplitterX))
        return FALSE;

    GetClientRect(hwndParent, &rc); 
    Resize(rc.right - rc.left, rc.bottom - rc.top);
    
    return TRUE;
}

void CViewMode_Explorer::Show()
{
    g_tbm.CheckButton(TB_GLOBAL, IDM_VIEW_FILE, TRUE);
    HMENU hmenu = GetMenu(g_hwndMain);
    CheckMenuItem(hmenu, IDM_VIEW_FILE, MF_CHECKED);
    m_splitter.SetVisible(TRUE);
    m_treeview.SetVisible(TRUE);
    m_listview.SetVisible(TRUE);
}

void CViewMode_Explorer::Hide()
{
    HMENU hmenu = GetMenu(g_hwndMain);
    CheckMenuItem(hmenu, IDM_VIEW_FILE, MF_UNCHECKED);
    m_splitter.SetVisible(FALSE);
    m_treeview.SetVisible(FALSE);
    m_listview.SetVisible(FALSE);
}

void CViewMode_Explorer::Resize(int nW, int nH)
{
    // Resize the various controls
    int nSplitterW = m_splitter.GetWidth();
    int nSplitterX = m_splitter.GetPos();
    int nRowHeight = g_tbm.GetRowsHeight();
    g_nSplitterX = nSplitterX;

    int nStatusH = g_statusbar.GetHeight();

    RECT rcTreeView = {0, nRowHeight, nSplitterX, nH-nStatusH};
    RECT rcListView = {nSplitterX + nSplitterW,  nRowHeight, nW, nH-nStatusH};
    RECT rcSplitter = {nSplitterX, nRowHeight, nSplitterX + nSplitterW, nH-nStatusH};

    m_splitter.Resize(&rcSplitter);
    m_treeview.Resize(&rcTreeView);
    m_listview.Resize(&rcListView);
}

void CViewMode_Explorer::UpdateIfDirty()
{
    m_listview.UpdateIfDirty();

    // treeview automatically refreshes
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  HandleEvent
// Purpose:   
// Arguments: 
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CViewMode_Explorer::HandleEvent(eEvent event, CFileObject *pfo)
{
    switch(event)
    {
    case EVENT_ADDED:
        if (pfo->m_fIsDir)
            m_treeview.AddObject(pfo);
        m_listview.AddObject(pfo);
        break;

    case EVENT_REMOVED:
        // Remove the object from the treeview and listview as appropriate
        if (pfo->m_fIsDir)
            m_treeview.RemoveObject(pfo);
        m_listview.RemoveObject(pfo);
        break;
    
    case EVENT_TIMECHANGED:
    case EVENT_SIZECHANGED:
        m_listview.RefreshObject(pfo);
        break;
    }
}

void CViewMode_Explorer::Clear()
{
    m_listview.Clear();
    m_treeview.Clear();
}

void CViewMode_Explorer::HandleMenuCommand(WORD command)
{
    switch(command)
    {
    case IDM_EDIT_SELECTALL:
        m_listview.SelectAllItems();
        SetFocus(m_listview.m_hwnd);
        break;
    }
}