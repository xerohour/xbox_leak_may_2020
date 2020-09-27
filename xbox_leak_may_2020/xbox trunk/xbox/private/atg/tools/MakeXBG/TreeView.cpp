//-----------------------------------------------------------------------------
// File: TreeView.cpp
//
// Desc: MFC tree view class for displaying model information.
//
// Hist: 03.01.00 - New for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "MakeXBG.h"


extern CMakeXBGApp g_App;




//-----------------------------------------------------------------------------
// Name: class CModelTreeView
// Desc: Tree view class to visualize mesh data.
//-----------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CModelTreeView, CTreeView)

BEGIN_MESSAGE_MAP(CModelTreeView, CTreeView)
    //{{AFX_MSG_MAP(CModelTreeView)
    ON_WM_MOUSEACTIVATE()
    ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CModelTreeView::CModelTreeView()
{
}

CModelTreeView::~CModelTreeView()
{
}




//-----------------------------------------------------------------------------
// Name: BuildTreeCB()
// Desc: Callback to build the tree view with detailed mesh information.
//-----------------------------------------------------------------------------
BOOL BuildTreeCB( CD3DFrame* pFrame, VOID* pData )
{
    CTreeCtrl* pTree = (CTreeCtrl*)pData;
    HTREEITEM  hParentItem;
    TCHAR      strBuffer[512];

    if( pFrame->m_pParent )
        hParentItem = pFrame->m_pParent->m_TreeData;
    else
        hParentItem = TVI_ROOT;

    TV_INSERTSTRUCT tvis;
    tvis.hInsertAfter   = TVI_LAST;
    tvis.item.mask      = TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
    tvis.item.stateMask = TVIS_USERMASK;
    tvis.item.state     = 0x8000;

    if( pFrame->m_dwNumMeshSubsets > 0 )
    {
        // Fill tree info for mesh frames

        if( pFrame->m_strFrameName[0] )
            _stprintf( strBuffer, _T("Mesh: \"%s\""), pFrame->m_strFrameName );
        else
            _stprintf( strBuffer, _T("Mesh") );
        
        tvis.hParent        = hParentItem;
        tvis.item.pszText   = strBuffer;
        tvis.item.lParam    = (LONG)pFrame;
        hParentItem = pTree->InsertItem(&tvis);
        tvis.item.mask      = TVIF_TEXT | TVIF_PARAM;

        if( pFrame->m_pMeshVB )
        {
            _stprintf( strBuffer, _T("Num Polygons = %ld"), pFrame->m_dwNumMeshPolygons );
            tvis.hParent      = hParentItem;
            tvis.item.pszText = strBuffer;
            tvis.item.lParam  = 0;
            pTree->InsertItem(&tvis);

            _stprintf( strBuffer, _T("Num Vertices = %ld"), pFrame->m_dwNumMeshVertices );
            tvis.hParent      = hParentItem;
            tvis.item.pszText = strBuffer;
            tvis.item.lParam  = 0;
            pTree->InsertItem(&tvis);

            _stprintf( strBuffer, _T("Num Indices = %ld"), pFrame->m_dwNumMeshIndices );
            tvis.hParent      = hParentItem;
            tvis.item.pszText = strBuffer;
            tvis.item.lParam  = 0;
            pTree->InsertItem(&tvis);

            _stprintf( strBuffer, _T("FVF = 0x%08lx"), pFrame->m_dwMeshFVF );
            tvis.hParent      = hParentItem;
            tvis.item.pszText = strBuffer;
            tvis.item.lParam  = 2;
            pFrame->m_hFVFTreeItem = pTree->InsertItem(&tvis);
        }

        // Print subset info
        for( DWORD i=0; i < pFrame->m_dwNumMeshSubsets; i++ )
        {
            _stprintf( strBuffer, _T("Subset %d"), i );
            tvis.hParent        = hParentItem;
            tvis.item.mask      = TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
            tvis.item.stateMask = TVIS_USERMASK;
            tvis.item.state     = 0x4000;
            tvis.item.pszText   = strBuffer;
            tvis.item.lParam    = i;
            HTREEITEM hSubsetSubTree = pTree->InsertItem(&tvis);
            tvis.item.mask      = TVIF_TEXT | TVIF_PARAM;

            {
                _stprintf( strBuffer, _T("Material") );
                tvis.hParent      = hSubsetSubTree;
                tvis.item.pszText = strBuffer;
                tvis.item.lParam  = 1;
                HTREEITEM hMaterialSubTree = pTree->InsertItem(&tvis);

                {
                    _stprintf( strBuffer, _T("Diffuse = (%f,%f,%f,%f)"), pFrame->m_pMeshSubsets[i].mtrl.Diffuse.r, pFrame->m_pMeshSubsets[i].mtrl.Diffuse.g, pFrame->m_pMeshSubsets[i].mtrl.Diffuse.b, pFrame->m_pMeshSubsets[i].mtrl.Diffuse.a );
                    tvis.hParent      = hMaterialSubTree;
                    tvis.item.pszText = strBuffer;
                    tvis.item.lParam  = 0;
                    pTree->InsertItem(&tvis);

                    _stprintf( strBuffer, _T("Ambient = (%f,%f,%f,%f)"), pFrame->m_pMeshSubsets[i].mtrl.Ambient.r, pFrame->m_pMeshSubsets[i].mtrl.Ambient.g, pFrame->m_pMeshSubsets[i].mtrl.Ambient.b, pFrame->m_pMeshSubsets[i].mtrl.Ambient.a );
                    tvis.item.pszText = strBuffer;
                    tvis.item.lParam  = 1;
                    pTree->InsertItem(&tvis);

                    _stprintf( strBuffer, _T("Specular = (%f,%f,%f,%f)"), pFrame->m_pMeshSubsets[i].mtrl.Specular.r, pFrame->m_pMeshSubsets[i].mtrl.Specular.g, pFrame->m_pMeshSubsets[i].mtrl.Specular.b, pFrame->m_pMeshSubsets[i].mtrl.Specular.a );
                    tvis.item.pszText = strBuffer;
                    tvis.item.lParam  = 2;
                    pTree->InsertItem(&tvis);

                    _stprintf( strBuffer, _T("Emissive = (%f,%f,%f,%f)"), pFrame->m_pMeshSubsets[i].mtrl.Emissive.r, pFrame->m_pMeshSubsets[i].mtrl.Emissive.g, pFrame->m_pMeshSubsets[i].mtrl.Emissive.b, pFrame->m_pMeshSubsets[i].mtrl.Emissive.a );
                    tvis.item.pszText = strBuffer;
                    tvis.item.lParam  = 3;
                    pTree->InsertItem(&tvis);

                    _stprintf( strBuffer, _T("Power = %f"), pFrame->m_pMeshSubsets[i].mtrl.Power );
                    tvis.item.pszText = strBuffer;
                    tvis.item.lParam  = 4;
                    pTree->InsertItem(&tvis);

                    if( pFrame->m_pMeshSubsets[i].strTexture[0] )
                        _stprintf( strBuffer, _T("Texture = \"%s\""), pFrame->m_pMeshSubsets[i].strTexture );
                    else
                        _stprintf( strBuffer, _T("Texture = NULL") );
                    tvis.item.pszText = strBuffer;
                    tvis.item.lParam  = 5;
                    pTree->InsertItem(&tvis);
                }

                _stprintf( strBuffer, _T("Vertex Start = %ld"), pFrame->m_pMeshSubsets[i].dwVertexStart );
                tvis.hParent      = hSubsetSubTree;
                tvis.item.pszText = strBuffer;
                tvis.item.lParam  = 0;
                pTree->InsertItem(&tvis);

                _stprintf( strBuffer, _T("Vertex Count = %ld"), pFrame->m_pMeshSubsets[i].dwVertexCount );
                tvis.item.pszText = strBuffer;
                tvis.item.lParam  = 1;
                pTree->InsertItem(&tvis);

                _stprintf( strBuffer, _T("Index Start = %ld"), pFrame->m_pMeshSubsets[i].dwIndexStart );
                tvis.item.pszText = strBuffer;
                tvis.item.lParam  = 2;
                pTree->InsertItem(&tvis);

                _stprintf( strBuffer, _T("Index Count = %ld"), pFrame->m_pMeshSubsets[i].dwIndexCount );
                tvis.item.pszText = strBuffer;
                tvis.item.lParam  = 3;
                pTree->InsertItem(&tvis);
            }
        }
    }
    else
    {
        if( pFrame->m_pParent == NULL )
        {
            // Fill tree info for the root frame

            _stprintf( strBuffer, _T("File: \"%s\""), pFrame->m_strFrameName );

            tvis.hParent        = hParentItem;
            tvis.item.pszText   = strBuffer;
            tvis.item.lParam    = (LONG)pFrame;
            hParentItem = pTree->InsertItem(&tvis);
            tvis.item.mask      = TVIF_TEXT | TVIF_PARAM;

            CD3DFile* pRoot = (CD3DFile*)pFrame;

            _stprintf( strBuffer, _T("Num Polygons = %ld"), pRoot->m_dwNumPolygons );
            tvis.hParent      = hParentItem;
            tvis.item.pszText = strBuffer;
            tvis.item.lParam  = 0;
            pTree->InsertItem(&tvis);

            _stprintf( strBuffer, _T("Num Vertices = %ld"), pRoot->m_dwNumVertices );
            tvis.hParent      = hParentItem;
            tvis.item.pszText = strBuffer;
            tvis.item.lParam  = 0;
            pTree->InsertItem(&tvis);

            _stprintf( strBuffer, _T("Num Indices = %ld"), pRoot->m_dwNumIndices );
            tvis.hParent      = hParentItem;
            tvis.item.pszText = strBuffer;
            tvis.item.lParam  = 0;
            pTree->InsertItem(&tvis);
        }
        else
        {
            // Fill tree info for transformation frames
            
            if( pFrame->m_strFrameName[0] )
                _stprintf( strBuffer, _T("Frame: \"%s\""), pFrame->m_strFrameName );
            else
                _stprintf( strBuffer, _T("Frame") );

            tvis.hParent        = hParentItem;
            tvis.item.pszText   = strBuffer;
            tvis.item.lParam    = (LONG)pFrame;
            hParentItem = pTree->InsertItem(&tvis);
            tvis.item.mask      = TVIF_TEXT | TVIF_PARAM;

            // Insert the matrix info   
            if( ( pFrame->m_pParent != NULL ) )
            {
                _stprintf( strBuffer, _T("Matrix") );
                tvis.hParent      = hParentItem;
                tvis.item.pszText = strBuffer;
                tvis.item.lParam  = 0;
                HTREEITEM hMatrixSubTree = pTree->InsertItem(&tvis);

                for( DWORD r = 0; r < 4; r++ )
                {
                    _stprintf( strBuffer, _T("(%f,%f,%f,%f)"), ((FLOAT*)pFrame->m_matTransform)[r*4+0], 
                                                               ((FLOAT*)pFrame->m_matTransform)[r*4+1], 
                                                               ((FLOAT*)pFrame->m_matTransform)[r*4+2], 
                                                               ((FLOAT*)pFrame->m_matTransform)[r*4+3] );
                    tvis.hParent      = hMatrixSubTree;
                    tvis.item.pszText = strBuffer;
                    tvis.item.lParam  = r;
                    pTree->InsertItem(&tvis);
                }
            }
        }
    }

    // Set the HTREEITEM for this frame
    pFrame->m_TreeData = hParentItem;

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: SelectFrameCB()
// Desc: Callback to tag certain frames as selected.
//-----------------------------------------------------------------------------
BOOL SelectFrameCB( CD3DFrame* pFrame, VOID* pData )
{
    CD3DFrame* pSelectedFrame = (CD3DFrame*)pData;
    CD3DFrame* pSeekFrame     = pFrame;

    while( pSeekFrame )
    {
        if( pSeekFrame == pSelectedFrame )
        {
            pFrame->m_bVisible = TRUE;
            return TRUE;
        }
        pSeekFrame = pSeekFrame->m_pParent;
    }
    pFrame->m_bVisible = FALSE;

    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: ClearSubsetSelectionCB()
// Desc: Simple callback to clear all the mesh subset selections.
//-----------------------------------------------------------------------------
BOOL ClearSubsetSelectionCB( CD3DFrame* pFrame, VOID* pData )
{
    pFrame->m_lSelectedSubset = -1;
    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: OnUpdate()
// Desc: Builds the tree view. All the work is done in the callback, which adds
//       detailed mesh info into the tree view.
//-----------------------------------------------------------------------------
void CModelTreeView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
{
    CTreeCtrl&    tree  = GetTreeCtrl();
    CXBGDocument* pDoc  = GetDocument();
    CD3DFile*     pMesh = pDoc->m_pD3DFile;

    if( pMesh )
    {
        // Empty the tree
        tree.DeleteAllItems();

        // Set a base style for the tree control
        tree.ModifyStyle( 0L, TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | 
                              TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS );

        // Add items to the control
        pMesh->EnumFrames( BuildTreeCB, &tree );

        // Expand the root tree item
        tree.Expand( tree.GetFirstVisibleItem(), TVE_EXPAND );
        tree.SelectItem(tree.GetFirstVisibleItem());
    }
}




//-----------------------------------------------------------------------------
// Name: OnSelchanged()
// Desc: Handle the tree view's selection changes.
//-----------------------------------------------------------------------------
VOID CModelTreeView::OnSelchanged( NMHDR*, LRESULT* pResult ) 
{
    // Select a subset if one is chosen on the tree ctrl
    CXBGDocument* pDoc = GetDocument();

    CTreeCtrl& tree  = GetTreeCtrl();
    HTREEITEM  hItem = tree.GetSelectedItem();

    pDoc->m_pD3DFile->EnumFrames( ClearSubsetSelectionCB, NULL );
    pDoc->m_pSelectedFrame  = NULL;
    pDoc->m_lSelectedSubset = -1;

    while( hItem )
    {
        // If this was tagged as a mesh, select it
        if( ( TVIS_USERMASK & tree.GetItemState(hItem, TVIS_USERMASK) ) == 0x00004000 )
        {
            pDoc->m_lSelectedSubset = tree.GetItemData(hItem);
        }

        // If this was tagged as a frame, select it
        if( ( TVIS_USERMASK & tree.GetItemState(hItem, TVIS_USERMASK) ) == 0x00008000 )
        {
            pDoc->m_pSelectedFrame = (CD3DFrame*)tree.GetItemData(hItem);
            pDoc->m_pD3DFile->EnumFrames( SelectFrameCB, pDoc->m_pSelectedFrame );
            pDoc->m_pSelectedFrame->m_lSelectedSubset = pDoc->m_lSelectedSubset;

            pDoc->m_pTreeCtrl = &tree;

            if( pDoc->m_pSelectedFrame->m_dwMeshFVF )
            {
                CMenu* pMenu = g_App.m_pMainWnd->GetMenu();
                pMenu->EnableMenuItem( IDM_SETMESHFVF, MF_ENABLED );
                pMenu->EnableMenuItem( IDM_STRIPIFY, MF_ENABLED );
            }
            else
            {
                CMenu* pMenu = g_App.m_pMainWnd->GetMenu();
                pMenu->EnableMenuItem( IDM_SETMESHFVF, MF_GRAYED|MF_DISABLED );
                pMenu->EnableMenuItem( IDM_STRIPIFY, MF_GRAYED|MF_DISABLED );
            }

            break;
        }

        hItem = tree.GetParentItem( hItem );
    }

    // Render the scene with the changes
    ((CSplitterFrame*)GetParentFrame())->m_pD3DRenderView->OnDraw( NULL );

    if( pResult )
        (*pResult) = 0;
}




