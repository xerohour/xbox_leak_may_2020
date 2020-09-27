//-----------------------------------------------------------------------------
// File: SplitterWnd.cpp
//
// Desc: MFC class for implementing a splitter window.
//
// Hist: 03.01.00 - New for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "MakeXBG.h"


extern CMakeXBGApp g_App;




//-----------------------------------------------------------------------------
// Name: class CSplitterFrame
// Desc: The splitter frame class
//-----------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CSplitterFrame, CMDIChildWnd)

CSplitterFrame::CSplitterFrame()
{
}

CSplitterFrame::~CSplitterFrame()
{
}




//-----------------------------------------------------------------------------
// Name: OnCreateClient()
// Desc: Creates a split-view for the document. View 0 is the tree view for the
//       geometry model, and view 1 is the d3d rendering view for the geometry
//       model.
//-----------------------------------------------------------------------------
BOOL CSplitterFrame::OnCreateClient( LPCREATESTRUCT, CCreateContext* pContext )
{
    // Create a splitter with 1 row, 2 columns
    if( FALSE == m_wndSplitter.CreateStatic( this, 1, 2 ) )
        return FALSE;

    // Add the first splitter pane - the default view in column 0
    if( FALSE == m_wndSplitter.CreateView( 0, 0, RUNTIME_CLASS(CModelTreeView), 
                                           CSize(200, 50), pContext ) )
        return FALSE;

    // Add the second splitter pane - an input view in column 1
    if( FALSE == m_wndSplitter.CreateView( 0, 1, pContext->m_pNewViewClass, 
                                           CSize(0, 0), pContext ) )
        return FALSE;

    // Gain access to the views
    m_pTreeView      = (CModelTreeView*)m_wndSplitter.GetPane(0,0);
    m_pD3DRenderView = (CModelRenderView*)m_wndSplitter.GetPane(0,1);

    // Activate the render view
    SetActiveView( m_pTreeView );

    return TRUE;
}




BEGIN_MESSAGE_MAP(CSplitterFrame, CMDIChildWnd)
    //{{AFX_MSG_MAP(CSplitterFrame)
    ON_WM_SIZE()
    ON_WM_MDIACTIVATE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()




//-----------------------------------------------------------------------------
// Name: class CMainSplitterWnd
// Desc: The splitter window class
//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CMainSplitterWnd, CSplitterWnd)

CMainSplitterWnd::CMainSplitterWnd()
{
}

CMainSplitterWnd::~CMainSplitterWnd()
{
}




//-----------------------------------------------------------------------------
// Name: GetActivePane()
// Desc: Retreives the active pane of the main splitter window
//-----------------------------------------------------------------------------
CWnd* CMainSplitterWnd::GetActivePane( int* pRow, int* pCol )
{
    // Attempt to use active view of frame window
    CFrameWnd* pFrameWnd = GetParentFrame();
    CWnd*      pView     = pFrameWnd->GetActiveView();

    // Failing that, use the current focus
    if( NULL == pView  )
        pView = GetFocus();

    return pView;
}




void CSplitterFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
    CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

    // Set menu item states
    CMenu* pMenu = g_App.m_pMainWnd->GetMenu();
    if( bActivate )
    {
        pMenu->EnableMenuItem( ID_FILE_CLOSE, MF_ENABLED );
        pMenu->EnableMenuItem( ID_FILE_SAVE_AS, MF_ENABLED );

        if( m_pTreeView->GetDocument()->m_pSelectedFrame )
        {
            if( m_pTreeView->GetDocument()->m_pSelectedFrame->m_dwMeshFVF )
            {
                pMenu->EnableMenuItem( IDM_SETMESHFVF, MF_ENABLED );
                pMenu->EnableMenuItem( IDM_STRIPIFY, MF_ENABLED );
            }
        }
    }
    else
    {
        pMenu->EnableMenuItem( ID_FILE_CLOSE,  MF_GRAYED );
        pMenu->EnableMenuItem( ID_FILE_SAVE_AS,   MF_GRAYED );
        pMenu->EnableMenuItem( IDM_SETMESHFVF, MF_GRAYED );
        pMenu->EnableMenuItem( IDM_STRIPIFY, MF_GRAYED );
    }
}

