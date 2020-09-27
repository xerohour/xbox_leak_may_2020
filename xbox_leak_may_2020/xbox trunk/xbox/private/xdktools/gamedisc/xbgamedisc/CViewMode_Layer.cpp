// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CViewMode_Layer.cpp
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

// g_vmLayer    -- The 'layer' view mode.  Contains several listviews
CViewMode_Layer g_vmLayer;

CFastToSlow m_ftosLeft;
CFastToSlow m_ftosRight;


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL CViewMode_Layer::Init(HWND hwndParent)
{
    RECT rc;

    if (!m_rglv[LV_LAYER0].Init(hwndParent))
        return FALSE;
    if (!m_rglv[LV_LAYER1].Init(hwndParent))
        return FALSE;
    if (!m_rglv[LV_UNPLACED].Init(hwndParent))
        return FALSE;

    if (!m_ftosLeft.Init(hwndParent, RGB(255, 0, 0), RGB(0, 255, 0)))
        return FALSE;
    if (!m_ftosRight.Init(hwndParent, RGB(0, 255, 0), RGB(255, 0, 0)))
        return FALSE;

    if (!m_splitter.Init(this, hwndParent, g_nSplitterX2))
        return FALSE;

    if (!m_unplaced.Init(this, hwndParent))
        return FALSE;

    GetClientRect(hwndParent, &rc); 
    m_nW = rc.right - rc.left;
    m_nH = rc.bottom - rc.top;

    HandleMenuCommand(IDM_VIEW_LAYER0);
    
    return TRUE;
}

void CViewMode_Layer::Show()
{
    // Enable the layer view controls on the toolbar and menu
    g_tbm.CheckButton(TB_GLOBAL, IDM_VIEW_LAYOUT, TRUE);

    HMENU hmenu = GetMenu(g_hwndMain);
    CheckMenuItem(hmenu, IDM_VIEW_LAYOUT,      MF_CHECKED);

    EnableMenuItem(hmenu, IDM_EDIT_GROUP,      MF_ENABLED);
    EnableMenuItem(hmenu, IDM_EDIT_UNGROUP,    MF_ENABLED);
    EnableMenuItem(hmenu, IDM_VIEW_LAYER0,     MF_ENABLED);
    EnableMenuItem(hmenu, IDM_VIEW_LAYER1,     MF_ENABLED);
    EnableMenuItem(hmenu, IDM_VIEW_BOTHLAYERS, MF_ENABLED);
    EnableMenuItem(hmenu, IDM_VIEW_UNPLACED,   MF_ENABLED);

    g_tbm.EnableButton(TB_LAYER, IDM_EDIT_GROUP, TRUE);
    g_tbm.EnableButton(TB_LAYER, IDM_EDIT_UNGROUP, TRUE);
    g_tbm.EnableButton(TB_LAYER, IDM_VIEW_LAYER0, TRUE);
    g_tbm.EnableButton(TB_LAYER, IDM_VIEW_LAYER1, TRUE);
    g_tbm.EnableButton(TB_LAYER, IDM_VIEW_BOTHLAYERS, TRUE);
    g_tbm.EnableButton(TB_LAYER, IDM_VIEW_UNPLACED, TRUE);

    m_unplaced.SetVisible(g_tbm.IsButtonChecked(TB_LAYER, IDM_VIEW_UNPLACED));

    m_fVisible = true;
    Resize();
}

void CViewMode_Layer::Hide()
{
    // Disable the layer view controls on the toolbar and menu
    g_tbm.EnableButton(TB_LAYER, IDM_EDIT_GROUP, FALSE);
    g_tbm.EnableButton(TB_LAYER, IDM_EDIT_UNGROUP, FALSE);
    g_tbm.EnableButton(TB_LAYER, IDM_VIEW_LAYER0, FALSE);
    g_tbm.EnableButton(TB_LAYER, IDM_VIEW_LAYER1, FALSE);
    g_tbm.EnableButton(TB_LAYER, IDM_VIEW_BOTHLAYERS, FALSE);
    g_tbm.EnableButton(TB_LAYER, IDM_VIEW_UNPLACED, FALSE);

    HMENU hmenu = GetMenu(g_hwndMain);
    CheckMenuItem(hmenu, IDM_VIEW_LAYOUT,      MF_UNCHECKED);
    EnableMenuItem(hmenu, IDM_EDIT_GROUP,      MF_GRAYED);
    EnableMenuItem(hmenu, IDM_EDIT_UNGROUP,    MF_GRAYED);
    EnableMenuItem(hmenu, IDM_VIEW_LAYER0,     MF_GRAYED);
    EnableMenuItem(hmenu, IDM_VIEW_LAYER1,     MF_GRAYED);
    EnableMenuItem(hmenu, IDM_VIEW_BOTHLAYERS, MF_GRAYED);
    EnableMenuItem(hmenu, IDM_VIEW_UNPLACED,   MF_GRAYED);

    m_splitter.SetVisible(FALSE);
    m_rglv[LV_LAYER0].SetVisible(FALSE);
    m_rglv[LV_LAYER1].SetVisible(FALSE);
    m_ftosLeft.SetVisible(FALSE);
    m_ftosRight.SetVisible(FALSE);
    m_unplaced.SetVisible(FALSE);
    m_fVisible = false;
}

void CViewMode_Layer::Resize()
{
    Resize(m_nW, m_nH);
}

// Resize the various controls
void CViewMode_Layer::Resize(int nW, int nH)
{
    int nRowHeight = g_tbm.GetRowsHeight();
    int nStatusH = g_statusbar.GetHeight();
    int nFastToSlowW = 20;
    switch (m_lvmCur)
    {
    case LVM_LAYER0:
        {
            RECT rcLV   = {nFastToSlowW, nRowHeight, nW, nH-nStatusH};
            RECT rcFtoS = {0, nRowHeight, nFastToSlowW, nH-nStatusH};
            if (m_fVisible)
            {
                m_splitter.SetVisible(FALSE);
                m_rglv[LV_LAYER0].SetVisible(TRUE);
                m_rglv[LV_LAYER1].SetVisible(FALSE);
                m_ftosLeft.SetVisible(TRUE);
                m_ftosRight.SetVisible(FALSE);
            }
            m_rglv[LV_LAYER0].Resize(&rcLV);
            m_ftosLeft.Resize(&rcFtoS);
        }
        break;
    case LVM_LAYER1:
        {
            RECT rcLV = {0, nRowHeight, nW-nFastToSlowW, nH-nStatusH};
            RECT rcFtoS = {nW-nFastToSlowW, nRowHeight, nW, nH-nStatusH};
            if (m_fVisible)
            {
                m_splitter.SetVisible(FALSE);
                m_rglv[LV_LAYER0].SetVisible(FALSE);
                m_rglv[LV_LAYER1].SetVisible(TRUE);
                m_ftosRight.SetVisible(TRUE);
                m_ftosLeft.SetVisible(FALSE);
            }
            m_rglv[LV_LAYER1].Resize(&rcLV);
            m_ftosRight.Resize(&rcFtoS);

        }
        break;

    case LVM_BOTHLAYERS:
        {
            int nSplitterW = m_splitter.GetWidth();
            int nSplitterX = m_splitter.GetPos();
            int nSplitterPermX = m_splitter.GetPermPos();
            g_nSplitterX2 = nSplitterX;

            if (nW > nSplitterPermX+nFastToSlowW-2)
                nSplitterX = nSplitterPermX;
            else
                nSplitterX = nW - nFastToSlowW - 2;
            RECT rcLV0 = {nFastToSlowW, nRowHeight, nSplitterX, nH-nStatusH};
            RECT rcLV1 = {nSplitterX + nSplitterW,  nRowHeight, nW-nFastToSlowW, nH-nStatusH};
            RECT rcSplitter  = {nSplitterX, nRowHeight, nSplitterX + nSplitterW, nH-nStatusH};
            RECT rcFtoSRight = {nW-nFastToSlowW, nRowHeight, nW, nH-nStatusH};
            RECT rcFtoSLeft  = {0, nRowHeight, nFastToSlowW, nH-nStatusH};

            if (m_fVisible)
            {
                m_splitter.SetVisible(TRUE);
                m_rglv[LV_LAYER0].SetVisible(TRUE);
                m_rglv[LV_LAYER1].SetVisible(TRUE);
                m_ftosLeft.SetVisible(TRUE);
                m_ftosRight.SetVisible(TRUE);
            }
            m_splitter.Resize(&rcSplitter);
            m_rglv[LV_LAYER0].Resize(&rcLV0);
            m_rglv[LV_LAYER1].Resize(&rcLV1);
            m_ftosRight.Resize(&rcFtoSRight);
            m_ftosLeft.Resize(&rcFtoSLeft);
        }
        break;
    }
    if (!m_fVisible)
    {
        m_splitter.SetVisible(FALSE);
        m_rglv[LV_LAYER0].SetVisible(FALSE);
        m_rglv[LV_LAYER1].SetVisible(FALSE);
        m_ftosLeft.SetVisible(FALSE);
        m_ftosRight.SetVisible(FALSE);
    }
    
    m_nW = nW;
    m_nH = nH;
}

void CViewMode_Layer::HandleEvent(eEvent event, CFileObject *pfo)
{
}

void CViewMode_Layer::UpdateIfDirty()
{
 //   m_listview.UpdateIfDirty();
}

void CViewMode_Layer::Clear()
{
}

void CViewMode_Layer::ViewLayer(eLayerViewMode lvm, DWORD dwCmd)
{
    m_lvmCur = lvm;
    g_tbm.CheckButton(TB_LAYER, dwCmd, TRUE);
    HMENU hmenu = GetMenu(g_hwndMain);
    CheckMenuItem(hmenu, IDM_VIEW_LAYER0,     (dwCmd == IDM_VIEW_LAYER0) ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hmenu, IDM_VIEW_LAYER1,     (dwCmd == IDM_VIEW_LAYER1) ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hmenu, IDM_VIEW_BOTHLAYERS, (dwCmd == IDM_VIEW_BOTHLAYERS) ? MF_CHECKED : MF_UNCHECKED);
    Resize();
}

void CViewMode_Layer::HandleMenuCommand(WORD command)
{
    switch(command)
    {
    case IDM_VIEW_LAYER0:
        ViewLayer(LVM_LAYER0, command);
        break;
    
    case IDM_VIEW_LAYER1:
        ViewLayer(LVM_LAYER1, command);
        break;

    case IDM_VIEW_BOTHLAYERS:
        ViewLayer(LVM_BOTHLAYERS, command);
        break;

    case IDM_VIEW_UNPLACED:
        m_unplaced.ToggleView();
        break;
    }
}
