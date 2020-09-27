// TreeBar.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "MainFrm.h"
#include "TreeBar.h"
#include "DeleteFileDlg.h"
#include "DeleteProjectDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTreeBar construction/destruction

CTreeBar::CTreeBar()
{

	m_dwRef = 0;
	AddRef();

	m_pIDataObject = NULL;
	m_pIDragNode = NULL;
	m_dwStartDragButton = 0;
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;
	m_pDragImage = NULL;
	m_hDropItem = NULL;
	m_dwDragRMenuEffect = DROPEFFECT_NONE;
	m_dwDragScrollTick = 0;
	m_dwDragExpandTick = 0;

	m_pIClipboardDataObject = NULL;
	m_pIClipboardNode = NULL;
	
	m_pINodeRightMenu = NULL;
	m_pEditCtrl = NULL;
	m_hAcceleratorTable = NULL;

	m_nSortType = TREE_SORTBY_NAME;

	// Now see if value for m_nSortType has been saved to registry
	{
		HKEY hKeyOpen;
		LONG lResult;

		lResult = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE,
							  	  _T("Software\\Microsoft\\DMUSProducer"), 0, KEY_READ, &hKeyOpen );
		if( lResult == ERROR_SUCCESS )
		{
			DWORD dwType;
			DWORD dwTreeSortBy;
			DWORD dwCbData = sizeof(DWORD);
			lResult = ::RegQueryValueEx( hKeyOpen, _T("TreeSortBy"), NULL,
										 &dwType, (LPBYTE)&dwTreeSortBy, &dwCbData );
			if( (lResult == ERROR_SUCCESS)
			&&  (dwType == REG_DWORD) )
			{
				ASSERT( (dwTreeSortBy == TREE_SORTBY_NAME)
					||	(dwTreeSortBy == TREE_SORTBY_TYPE)
					||	(dwTreeSortBy == TREE_SORTBY_SIZE) );

				if( (dwTreeSortBy == TREE_SORTBY_NAME)
				||	(dwTreeSortBy == TREE_SORTBY_TYPE)
				||	(dwTreeSortBy == TREE_SORTBY_SIZE) )
				{
					m_nSortType = dwTreeSortBy;
				}
			}

			::RegCloseKey( hKeyOpen );
		}
	}

}

CTreeBar::~CTreeBar()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar Create

BOOL CTreeBar::Create( CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID )
{
	if( !CDialogBar::Create(pParentWnd, nIDTemplate, nStyle, nID) )
	{
		return FALSE;
	}

	m_sizeFloating = m_sizeDefault;
	m_sizeDocked = m_sizeDefault;

	// Create the splitter bar
	m_wndSplitter.Create( this );

	// Register drop target
	HRESULT hr = ::RegisterDragDrop( GetSafeHwnd(), (IDropTarget *)this );
	ASSERT(SUCCEEDED(hr));

	// Load accelerators
	m_hAcceleratorTable = ::LoadAccelerators( theApp.m_hInstance, MAKEINTRESOURCE(IDR_PROJECTTREE_ACCELS) );
	
	return TRUE;
}

BOOL CTreeBar::Create( CWnd* pParentWnd, LPCTSTR szTemplate, UINT nStyle, UINT nID )
{
	if( !CDialogBar::Create( pParentWnd, szTemplate, nStyle, nID) )
	{
		return FALSE;
	}

	m_sizeFloating = m_sizeDefault;
	m_sizeDocked = m_sizeDefault;

	// Create the splitter bar
	m_wndSplitter.Create( this );
	
	// Register drop target
	HRESULT hr = ::RegisterDragDrop( GetSafeHwnd(), (IDropTarget *)this );
	ASSERT(SUCCEEDED(hr));

	// Load accelerators
	m_hAcceleratorTable = ::LoadAccelerators( theApp.m_hInstance, MAKEINTRESOURCE(IDR_PROJECTTREE_ACCELS) );

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// IUknown CTreeBar::QueryInterface

HRESULT CTreeBar::QueryInterface( REFIID riid, LPVOID *ppvObj )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    if( ::IsEqualIID(riid, IID_IDropSource)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDropSource *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDropTarget) )
    {
        AddRef();
        *ppvObj = (IDropTarget *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}


/////////////////////////////////////////////////////////////////////////////
// IUnknown CTreeBar::AddRef

ULONG CTreeBar::AddRef( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return ++m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// IUnknown CTreeBar::Release

ULONG CTreeBar::Release( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    ASSERT( m_dwRef != 0 );

    --m_dwRef;

    ASSERT( m_dwRef != 0 );	// m_dwRef should never get to zero.

//  if( m_dwRef == 0 )		   CTreeBar should be deleted when		
//  {						   control is destroyed. 						
//		delete this;		
//		return 0;
//  }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar IDropSource implementation

/////////////////////////////////////////////////////////////////////////////
// IDropSource CTreeBar::QueryContinueDrag

HRESULT CTreeBar::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
 
	if( fEscapePressed )
	{
        return DRAGDROP_S_CANCEL;
	}

	if( m_dwStartDragButton == MK_LBUTTON )
	{
		if( grfKeyState & MK_RBUTTON )
		{
			return DRAGDROP_S_CANCEL;
		}

		if( !(grfKeyState & MK_LBUTTON) )
		{
			return DRAGDROP_S_DROP;
		}
	}

	if( m_dwStartDragButton == MK_RBUTTON )
	{
		if( grfKeyState & MK_LBUTTON )
		{
			return DRAGDROP_S_CANCEL;
		}
		
		if( !(grfKeyState & MK_RBUTTON) )
		{
			return DRAGDROP_S_DROP;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IDropSource CTreeBar::GiveFeedback

HRESULT CTreeBar::GiveFeedback( DWORD dwEffect )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( m_pDragImage )
	{
		CPoint pt;

		GetCursorPos( &pt );

		// Move the drag image
		m_pDragImage->DragMove( pt );
	}

	return DRAGDROP_S_USEDEFAULTCURSORS;
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar IDropTarget implementation

/////////////////////////////////////////////////////////////////////////////
// IDropTarget CTreeBar::DragEnter

HRESULT CTreeBar::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	ASSERT( pIDataObject != NULL );
	ASSERT( m_pIDataObject == NULL );

	// Store IDataObject associated with current drag-drop operation
	m_pIDataObject = pIDataObject;
	m_pIDataObject->AddRef();

	// Reset timer used to scroll Project Tree
	m_dwDragScrollTick = 0;

	// Reset timer used to expand the Project Tree 
	m_dwDragExpandTick = 0;

	if( m_pDragImage )
	{
		CPoint point( pt.x, pt.y );

		// Show the feedback image
		m_pDragImage->DragEnter( GetDesktopWindow (), point );
	}

	// Determine effect of drop
	return DragOver( grfKeyState, pt, pdwEffect );
}


#define SCROLL_INSET	12
#define DO_HSCROLL		1
#define DO_VSCROLL		2

/////////////////////////////////////////////////////////////////////////////
// IDropTarget CTreeBar::DragOver

HRESULT CTreeBar::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	ASSERT( m_pIDataObject != NULL );

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	ASSERT( pTreeCtrl != NULL );

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragShowNolock( FALSE );
	}

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	CPoint point( pt.x, pt.y );

	HTREEITEM hItem = GetItemFromPoint( point );
	if( hItem )
	{
		if( hItem != m_hDropItem )
		{
			// Unhilite the old drop target
			pTreeCtrl->SelectDropTarget( NULL );

			// Hilite the new drop target
			m_hDropItem = hItem;
			pTreeCtrl->SelectDropTarget( m_hDropItem );

			// Reset expand timer
			m_dwDragExpandTick = 0;
		}

		// Now determine the effect of the drop
		CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
		if( pJzNode )
		{
			IDMUSProdNode* pINode = pJzNode->m_pINode;
			ASSERT( pINode != NULL );

			if( pINode )
			{
				BOOL fWillSetReference;

				if( pINode->CanPasteFromData( m_pIDataObject, &fWillSetReference ) == S_OK )
				{
					if( fWillSetReference )
					{
						dwEffect = DROPEFFECT_COPY;
					}
					else if( grfKeyState & MK_RBUTTON )
					{
						dwEffect = *pdwEffect;
					}
					else
					{
						if( grfKeyState & MK_CONTROL )
						{
							dwEffect = DROPEFFECT_COPY;
						}
						else
						{
							if( *pdwEffect & DROPEFFECT_COPY
							&&  *pdwEffect & DROPEFFECT_MOVE )
							{
								dwEffect = DROPEFFECT_MOVE;
							}
							else
							{
								dwEffect = *pdwEffect;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		// Unhilite the old drop target
		pTreeCtrl->SelectDropTarget( NULL );
		m_hDropItem = NULL;
	}

	// Set temp drag over fields
	if( grfKeyState & (MK_RBUTTON | MK_LBUTTON) )
	{
		m_dwOverDragButton = grfKeyState & (MK_RBUTTON | MK_LBUTTON);
		m_dwOverDragEffect = dwEffect;
	}

	// Check to see if we need to expand the Project Tree
	if( m_hDropItem )
	{
		UINT nState = pTreeCtrl->GetItemState( m_hDropItem, TVIS_EXPANDED );
		if( !(nState & TVIS_EXPANDED) )
		{
			if( pTreeCtrl->ItemHasChildren(m_hDropItem) )
			{
				DWORD dwExpandTick = m_dwDragExpandTick;
				m_dwDragExpandTick = 0;

				DWORD dwTick = GetTickCount();

				if( dwExpandTick )
				{
					if( dwTick >= dwExpandTick )
					{
						// Time to expand
						pTreeCtrl->Expand( hItem, TVE_EXPAND );
						m_dwDragExpandTick = 0;
						
						// Postpone scrolling
						m_dwDragScrollTick  = 0;
					}
					else
					{
						m_dwDragExpandTick = dwExpandTick;	// still waiting...
					}
				}
				else
				{
					m_dwDragExpandTick = dwTick + 1000;		// wait 1000 ms
				}
			}
		}
	}

	// Check to see if we need to scroll the Project Tree
	DWORD dwScrollTick = m_dwDragScrollTick;
	m_dwDragScrollTick = 0;

	if( hItem )
	{
		CRect rect;
		UINT nAction = -1;
		short nDirection = -1;

		pTreeCtrl->GetClientRect( &rect );
		pTreeCtrl->ClientToScreen( &rect );

		int nScrollSpeed = (rect.right - rect.left) >> 2;

		if( rect.PtInRect( point ) )
		{
			int nScrollMin;
			int nScrollMax;
			int nScrollPos;

			if( point.y <= (rect.top + SCROLL_INSET) )
			{
				pTreeCtrl->GetScrollRange( SB_VERT, &nScrollMin, &nScrollMax );
				nScrollPos = pTreeCtrl->GetScrollPos( SB_VERT );

				if( nScrollPos > nScrollMin )
				{
					nDirection = DO_VSCROLL;

					if( point.x <= (rect.left + nScrollSpeed) )
					{
						nAction = SB_PAGEUP;
					}
					else
					{
						nAction = SB_LINEUP;
					}
				}
			}
			else if( point.y >= (rect.bottom - SCROLL_INSET) )
			{
				pTreeCtrl->GetScrollRange( SB_VERT, &nScrollMin, &nScrollMax );
				nScrollPos = pTreeCtrl->GetScrollPos( SB_VERT );

				if( nScrollPos < nScrollMax )
				{
					nDirection = DO_VSCROLL;

					if( point.x <= (rect.left + nScrollSpeed) )
					{
						nAction = SB_PAGEDOWN;
					}
					else
					{
						nAction = SB_LINEDOWN;
					}
				}
			}
			else if( point.x <= (rect.left + SCROLL_INSET) )
			{
				pTreeCtrl->GetScrollRange( SB_HORZ, &nScrollMin, &nScrollMax );
				nScrollPos = pTreeCtrl->GetScrollPos( SB_HORZ );

				if( nScrollPos > nScrollMin )
				{
					nDirection = DO_HSCROLL;
					nAction = SB_LINELEFT;
				}
			}
			else if( point.x >= (rect.right - SCROLL_INSET) )
			{
				pTreeCtrl->GetScrollRange( SB_HORZ, &nScrollMin, &nScrollMax );
				nScrollPos = pTreeCtrl->GetScrollPos( SB_HORZ );

				if( nScrollPos < nScrollMax )
				{
					nDirection = DO_HSCROLL;
					nAction = SB_LINERIGHT;
				}
			}
		}

		if( nAction != -1 )
		{
			DWORD dwTick = GetTickCount();

			if( dwScrollTick )
			{
				if( dwTick >= dwScrollTick )
				{
					// Time to scroll
					if( nDirection == DO_VSCROLL )
					{
						pTreeCtrl->SendMessage( WM_VSCROLL, nAction, 0L );
					}
					else
					{
						pTreeCtrl->SendMessage( WM_HSCROLL, nAction, 0L );
					}

					m_dwDragScrollTick = dwTick + 100;	// wait 100 ms
					if( nAction == SB_PAGEUP
					||  nAction == SB_PAGEDOWN )
					{
						m_dwDragScrollTick += 400;		// wait another 400 ms 
					}									// when scrolling pages
				}
				else
				{
					m_dwDragScrollTick = dwScrollTick;	// still waiting...
				}
			}
			else
			{
				m_dwDragScrollTick = dwTick + 400;		// wait 400 ms
			}

			dwEffect |= DROPEFFECT_SCROLL;
		}
	}

	if( m_pDragImage )
	{
		// Show the feedback image
		m_pDragImage->DragShowNolock( TRUE );
	}

	// Return the effect
	*pdwEffect = dwEffect;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IDropTarget CTreeBar::DragLeave

HRESULT CTreeBar::DragLeave( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Release IDataObject
	if( m_pIDataObject )
	{
		m_pIDataObject->Release();
		m_pIDataObject = NULL;
	}

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragLeave( GetDesktopWindow () );
	}

	// Unhilite the drop target
	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	ASSERT( pTreeCtrl != NULL );
	if( m_hDropItem )
	{
		pTreeCtrl->SelectDropTarget( NULL );
		m_hDropItem = NULL;
	}

	//Reset temp drag over fields
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;

	// Reset timer used to scroll Project Tree
	m_dwDragScrollTick = 0;

	// Reset timer used to expand the Project Tree
	m_dwDragExpandTick = 0;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IDropTarget CTreeBar::Drop

HRESULT CTreeBar::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	ASSERT( m_pIDataObject != NULL );
	ASSERT( m_pIDataObject == pIDataObject );

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragLeave( GetDesktopWindow () );
	}

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	ASSERT( pTreeCtrl != NULL );

	CPoint point( pt.x, pt.y );

	// Set default values
	HRESULT hr = S_OK;
	*pdwEffect = DROPEFFECT_NONE;

	// Choose effect when right mouse drag - Move, Copy, or Cancel?
	if( m_dwOverDragButton & MK_RBUTTON )
	{
		HMENU hMenu;
		HMENU hMenuPopup;

		hMenu = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDM_DRAG_RMENU) );
		if( hMenu )
		{
			m_dwDragRMenuEffect = DROPEFFECT_NONE;

			// Track right context menu for drag-drop via TrackPopupMenu
			hMenuPopup = ::GetSubMenu( hMenu, 0 );

			// Init state of menu items
			if( !(m_dwOverDragEffect & DROPEFFECT_MOVE) )
			{
				::EnableMenuItem( hMenuPopup, IDM_DRAG_MOVE, (MF_GRAYED | MF_BYCOMMAND) );
			}

			// Display and track menu
			::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
							  point.x, point.y, 0, GetSafeHwnd(), NULL );
			DestroyMenu( hMenu );

			// Need to process WM_COMMAND from TrackPopupMenu
			MSG msg;
			while( ::PeekMessage( &msg, GetSafeHwnd(), NULL, NULL, PM_REMOVE) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}

			// WM_COMMAND from TrackPopupMenu will have set m_dwDragRMenuEffect
			m_dwOverDragEffect = m_dwDragRMenuEffect;
			m_dwDragRMenuEffect = DROPEFFECT_NONE;
		}
	}

	// Paste data
	if( m_dwOverDragEffect != DROPEFFECT_NONE )
	{
		HTREEITEM hItem = GetItemFromPoint( point );
		if( hItem )
		{
			pTreeCtrl->SelectItem( hItem );

			CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
			if( pJzNode )
			{
				IDMUSProdNode* pINode = pJzNode->m_pINode;
				ASSERT( pINode != NULL );

				if( pINode )
				{
					if( pINode == m_pIDragNode )
					{
						// No point in drag-drop to same node
						hr = E_FAIL;
					}
					else
					{
						HTREEITEM hTargetItemParent = NULL;
						HTREEITEM hTargetItem = theApp.m_pFramework->FindTreeItem( pINode );
						if( hTargetItem )
						{
							hTargetItemParent = pTreeCtrl->GetNextItem( hTargetItem, TVGN_PARENT );
						}
						
						HTREEITEM hDocRootParentFolder = NULL;
						HTREEITEM hDragItemParent = NULL;
						HTREEITEM hDragItem = NULL; 

						if( m_pIDragNode )
						{
							hDragItem = theApp.m_pFramework->FindTreeItem( m_pIDragNode );
							if( hDragItem )
							{
								hDragItemParent = pTreeCtrl->GetNextItem( hDragItem, TVGN_PARENT );
							}

							if( theApp.m_pFramework->IsDocRootNode( m_pIDragNode ) )
							{
								if( hDragItemParent )
								{
									hDocRootParentFolder = pTreeCtrl->GetNextItem( hDragItemParent, TVGN_PARENT );
								}
							}
						}

						BOOL fTargetUseOpenCloseImages;
						pINode->UseOpenCloseImages( &fTargetUseOpenCloseImages);

						GUID guidTargetItemNodeId;
						pINode->GetNodeId( &guidTargetItemNodeId );

						GUID guidDragItemNodeId;
						if( m_pIDragNode )
						{
							m_pIDragNode->GetNodeId( &guidDragItemNodeId );
						}
						else
						{
							guidDragItemNodeId = GUID_AllZeros;
						}

						if( m_dwOverDragEffect & DROPEFFECT_MOVE 
						&&  hDragItem
						&&  hTargetItem
						&& ( (hTargetItem == hDragItemParent) ||
						     (hTargetItem == hDocRootParentFolder) ||
							 ((fTargetUseOpenCloseImages == FALSE) && (hTargetItemParent == hDragItemParent) && (::IsEqualGUID(guidTargetItemNodeId, guidDragItemNodeId))) ) )
						{
							// No point in moving node to same folder
							hr = E_FAIL;
						}
						else
						{
							theApp.m_fInPasteFromData = TRUE;
							hr = pINode->PasteFromData( pIDataObject );
							theApp.m_fInPasteFromData = FALSE;

							if( SUCCEEDED ( hr ) )
							{
								*pdwEffect = m_dwOverDragEffect;
							}
						}
					}
				}
			}
		}
	}

	// Cleanup
	DragLeave();

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::CalcDynamicLayout

CSize CTreeBar::CalcDynamicLayout( int nLength, DWORD nMode )
{
	// Show/hide splitter bar
	if( IsFloating() )
	{
		m_wndSplitter.ShowWindow( SW_HIDE );
	}
	else
	{
		m_wndSplitter.ShowWindow( SW_SHOWNA );
	}

	// Return default if it is being docked or floated
	if( (nMode & LM_VERTDOCK) 
	||  (nMode & LM_HORZDOCK) )
	{
		if( nMode & LM_STRETCH )	// stretch to fit
		{
			if( nMode & LM_HORZ )
			{
				nLength = min( 30, nLength );
				return CSize( m_sizeDocked.cx = m_sizeFloating.cx = nLength, m_sizeDocked.cy );
			}
			else
			{
				return CSize( m_sizeDocked.cx, m_sizeDocked.cy = nLength );	
			}
		}
		else
		{
			return m_sizeDocked;
		}
	}

	if( nMode & LM_MRUWIDTH )
	{
		return m_sizeFloating;
	}

	//In all other cases, accept the dynamic length
	if( nMode & LM_LENGTHY )
	{ 
		nLength = max( GetSystemMetrics(SM_CYSMCAPTION), nLength );
		return CSize( m_sizeFloating.cx, m_sizeFloating.cy = nLength );	
	}
	else
	{
		nLength = max( 60, nLength );
		return CSize( m_sizeFloating.cx = m_sizeDocked.cx = nLength, m_sizeFloating.cy );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnDoubleClick

void CTreeBar::OnDoubleClick( NMHDR* pNMHdr, CTreeCtrl* pTreeCtrl, BOOL fEnterKey, LRESULT* pResult ) 
{
	*pResult = 0;

	HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
	if( hItem )
	{
		// Determine the cursor position
		POINT pt;
		::GetCursorPos( &pt );
		::ScreenToClient( pTreeCtrl->GetSafeHwnd(), &pt );

		if( fEnterKey == FALSE )
		{
			// Where is the cursor in the respect to the tree control?
			UINT uFlags;
			HTREEITEM hCursorItem = pTreeCtrl->HitTest( pt, &uFlags );

			if( (hItem != hCursorItem)
			||  (!(uFlags & TVHT_ONITEMLABEL)  &&  !(uFlags & TVHT_ONITEMICON)) )
			{
				// Cursor not over label of the selected item so just return
				*pResult = 1;
				return;
			}
		}

		CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
		ASSERT( pJzNode != NULL );

		IDMUSProdNode* pINode = pJzNode->m_pINode;
		ASSERT( pINode != NULL );

		if( pINode )
		{
			GUID guidNodeId;

			if( SUCCEEDED ( pINode->GetNodeId( &guidNodeId ) ) )
			{
				if( IsEqualGUID ( guidNodeId, GUID_ProjectFolderNode )
				||  IsEqualGUID ( guidNodeId, GUID_DirectoryNode ) )
				{
					;
				}
				else if( IsEqualGUID ( guidNodeId, GUID_FileNode ) )
				{
					CFileNode* pFileNode = (CFileNode *)pINode;

					CString strFileName;
					pFileNode->ConstructFileName( strFileName );

					if( pFileNode->IsProducerFile() )
					{
						HTREEITEM hChildItem = pTreeCtrl->GetNextItem( hItem, TVGN_CHILD );
						if( hChildItem == NULL )
						{
							// DirectMusic Producer file needs to be opened
							theApp.OpenTheFile( strFileName, TGT_FILENODE );
							if( pFileNode->m_pIChildNode )
							{
								CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pFileNode->m_pIChildNode );
								if( pComponentDoc )
								{
									pComponentDoc->OpenEditorWindow( pFileNode->m_pIChildNode );
								}
							}
							*pResult = 1;
						}
						else
						{
							UINT nState = pTreeCtrl->GetItemState( hItem, TVIS_EXPANDED );
							if( !(nState & TVIS_EXPANDED) )
							{
								pTreeCtrl->SelectItem( hChildItem );
								*pResult = 1;
							}
						}
					}
					else
					{
						// Run the "foreign" application to edit the non-Producer file
						CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
						ASSERT( pMainFrame != NULL );

						CString strWorkingDir;

						int nFindPos = strFileName.ReverseFind( (TCHAR)'\\' );
						if( nFindPos != -1 )
						{
							strWorkingDir = strFileName.Left( nFindPos );
						}

						::ShellExecute( pMainFrame->GetSafeHwnd(), _T("open"), strFileName, NULL, strWorkingDir, SW_SHOWNORMAL );
					}
				}
				else
				{
					// If reference node, use document of referenced file
					IDMUSProdReferenceNode* pIReferenceNode;
					if( SUCCEEDED ( pINode->QueryInterface( IID_IDMUSProdReferenceNode, (void **)&pIReferenceNode ) ) )
					{
						IDMUSProdNode* pIDocRootNode;
						if( SUCCEEDED ( pIReferenceNode->GetReferencedFile( &pIDocRootNode ) ) )
						{
							if( pIDocRootNode )
							{
								CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pIDocRootNode );
								if( pComponentDoc )
								{
									pComponentDoc->OpenEditorWindow( pIDocRootNode );

									// If node has editor don't expand/collapse node
									CLSID clsidEditor;
									if( SUCCEEDED( pIDocRootNode->GetEditorClsId(&clsidEditor) ) )
									{
										*pResult = 1;
									}
								}

								pIDocRootNode->Release();
							}
						}

						pIReferenceNode->Release();
					}
					else
					{
						CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pINode );
						if( pComponentDoc )
						{
							pComponentDoc->OpenEditorWindow( pINode );

							// If node has editor don't expand/collapse node
							CLSID clsidEditor;
							if( SUCCEEDED( pINode->GetEditorClsId(&clsidEditor) ) )
							{
								*pResult = 1;
							}
						}
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnBeginLabelEdit

void CTreeBar::OnBeginLabelEdit( TV_DISPINFO FAR* pTVDispInfo, CTreeCtrl* pTreeCtrl, LRESULT* pResult ) 
{
	CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( pTVDispInfo->item.hItem );
	ASSERT( pJzNode != NULL );

	IDMUSProdNode* pINode = pJzNode->m_pINode;
	ASSERT( pINode != NULL );

	CEdit* pEdit = pTreeCtrl->GetEditControl();

	if( pINode
	&&  pEdit )
	{
		// Subclass the edit control
		m_pEditCtrl = new CEditLabel( pINode );
		if( m_pEditCtrl )
		{
			m_pEditCtrl->SubclassWindow( pEdit->GetSafeHwnd() );
		}

		// Limit length of text
		short nMaxLength;

		if( SUCCEEDED ( pINode->GetNodeNameMaxLength(&nMaxLength) ) )
		{
			if( nMaxLength > 0 )
			{
				pEdit->LimitText( nMaxLength );
				*pResult = 0;
				return;
			}
		}
	}
	
	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::CheckEditControl
		
void CTreeBar::CheckEditControl()
{
	// For some reason we periodically miss TVN_ENDLABELEDIT notifications
	// when drag is halfway started on double-click.
	if( m_pEditCtrl )
	{
		CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
		ASSERT( pTreeCtrl != NULL );

		CEdit* pEdit = pTreeCtrl->GetEditControl();

		if( pEdit == NULL )
		{
			delete m_pEditCtrl;
			m_pEditCtrl = NULL;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnEndLabelEdit

void CTreeBar::OnEndLabelEdit( TV_DISPINFO FAR* pTVDispInfo, CTreeCtrl* pTreeCtrl, LRESULT* pResult ) 
{
	if( m_pEditCtrl )
	{
		m_pEditCtrl->UnsubclassWindow();
		delete m_pEditCtrl;
		m_pEditCtrl = NULL;
	}

	CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( pTVDispInfo->item.hItem );
	if( pJzNode )
	{
		IDMUSProdNode* pINode = pJzNode->m_pINode;
		ASSERT( pINode != NULL );

		if( pINode )
		{
			if( pTVDispInfo->item.pszText )
			{
				CString strName;
				BSTR bstrName;

				strName = pTVDispInfo->item.pszText;

				if( !strName.IsEmpty() )
				{
					strName.TrimLeft();
					strName.TrimRight();
					bstrName = strName.AllocSysString();

					if( pINode->ValidateNodeName( bstrName ) == S_OK )
					{
						bstrName = strName.AllocSysString();

						theApp.m_fUserChangedNodeName = TRUE;
						HRESULT hr = pINode->SetNodeName( bstrName );
						theApp.m_fUserChangedNodeName = FALSE;

						if( SUCCEEDED ( hr ) )
						{
							theApp.m_pFramework->SyncNodeName( pINode, TRUE );
							return;
						}
					}
				}
			}
		}
	}
	
	pTreeCtrl->SetFocus();

	*pResult = 0;	// Let the tree control think we have cancelled the edit.
					// This is necessary because we want to alter new text by
					//		stripping leading and trailing spaces.
					// If text passes IDMUSProdNode edits, we take on the responsibility 
					//		of calling SetItemText() to update the label.
}
		

/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnItemExpanded

void CTreeBar::OnItemExpanded( NM_TREEVIEW* pNMTreeView, CTreeCtrl* pTreeCtrl, LRESULT* pResult ) 
{
	// Deal with node images
	CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( pNMTreeView->itemNew.hItem );
	if( pJzNode )
	{
		IDMUSProdNode* pINode = pJzNode->m_pINode;
		ASSERT( pINode != NULL );

		if( pINode )
		{
			BOOL fUseOpenCloseImages;
			int nImage;
			int nImageSel;

			pINode->UseOpenCloseImages( &fUseOpenCloseImages );
			if( fUseOpenCloseImages )
			{
				short nTheImage;

				pINode->GetNodeImageIndex( &nTheImage );
				pTreeCtrl->GetItemImage( pNMTreeView->itemNew.hItem, nImage, nImageSel );

				if( pNMTreeView->action == TVE_EXPAND )
				{
					if( nImage == nTheImage )
					{
						nImage = pNMTreeView->itemNew.iImage + 1;
						nImageSel = pNMTreeView->itemNew.iSelectedImage + 1;
					}
				}
				else
				{
					if( nImage == (nTheImage + 1) )
					{
						nImage = pNMTreeView->itemNew.iImage - 1;
						nImageSel = pNMTreeView->itemNew.iSelectedImage - 1;
					}
				}
				pTreeCtrl->SetItemImage( pNMTreeView->itemNew.hItem, nImage, nImageSel );
			}
		}
	}

	// Sort child nodes
	if( pNMTreeView->action == TVE_EXPAND )
	{
		SortChildren( pNMTreeView->itemNew.hItem );
	}
	
	*pResult = 0;
}

		
/////////////////////////////////////////////////////////////////////////////
// CompareTreeItems

int CALLBACK CompareTreeItems( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
	CJzNode* pJzNode1 = (CJzNode *)lParam1;
	CJzNode* pJzNode2 = (CJzNode *)lParam2;
	IDMUSProdNode* pINode1 = pJzNode1->m_pINode;
	IDMUSProdNode* pINode2 = pJzNode2->m_pINode;
	CString strName1;
	CString strName2;
	BSTR bstrName1; 
	BSTR bstrName2; 
	GUID guidNodeId1;
	GUID guidNodeId2;

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	if( SUCCEEDED ( pINode1->GetNodeId( &guidNodeId1 ) )
	&&  SUCCEEDED ( pINode2->GetNodeId( &guidNodeId2 ) ) )
	{
		// Directory nodes should appear at the top
		if( IsEqualGUID( guidNodeId1, GUID_DirectoryNode ) )
		{
			if( !IsEqualGUID( guidNodeId2, GUID_DirectoryNode ) )
			{
				return -1;
			}
		}
		else if( IsEqualGUID( guidNodeId2, GUID_DirectoryNode ) ) 
		{
			if( !IsEqualGUID( guidNodeId1, GUID_DirectoryNode ) )
			{
				return 1;
			}
		}

		// File nodes should appear in m_nSortType order
		if( IsEqualGUID( guidNodeId1, GUID_FileNode ) 
		&&  IsEqualGUID( guidNodeId2, GUID_FileNode ) )
		{
			if( SUCCEEDED ( theApp.m_pFramework->GetNodeFileName( pINode1, &bstrName1 ) ) )
			{
				strName1 = bstrName1;
				::SysFreeString( bstrName1 );
			}

			if( SUCCEEDED ( theApp.m_pFramework->GetNodeFileName( pINode2, &bstrName2 ) ) )
			{
				strName2 = bstrName2;
				::SysFreeString( bstrName2 );
			}

			switch( pMainFrame->m_wndTreeBar.GetSortType() )
			{
				case TREE_SORTBY_NAME:
					break;

				case TREE_SORTBY_TYPE:
				{
					CString strExt1;
					int nFindPos = strName1.ReverseFind( (TCHAR)'.' );
					if( nFindPos != -1 )
					{
						strExt1 = strName1.Right( strName1.GetLength() - nFindPos );
					}

					CString strExt2;
					nFindPos = strName2.ReverseFind( (TCHAR)'.' );
					if( nFindPos != -1 )
					{
						strExt2 = strName2.Right( strName2.GetLength() - nFindPos );
					}

					if( strExt1.CompareNoCase(strExt2) != 0 )
					{
						return strExt1.CompareNoCase( strExt2 );
					}
					break;	// Sort alphabetically
				}

				case TREE_SORTBY_SIZE:
				{
					DWORD dwFileSize1 = 0;
					HANDLE hFile = ::CreateFile( strName1, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
	 											 FILE_ATTRIBUTE_NORMAL, NULL );
					if( hFile != INVALID_HANDLE_VALUE )
					{
						dwFileSize1 = GetFileSize( hFile, NULL );
						CloseHandle( hFile );
					}

					DWORD dwFileSize2 = 0;
					hFile = ::CreateFile( strName2, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
	 									  FILE_ATTRIBUTE_NORMAL, NULL );
					if( hFile != INVALID_HANDLE_VALUE )
					{
						dwFileSize2 = GetFileSize( hFile, NULL );
						CloseHandle( hFile );
					}

					if( dwFileSize1 < dwFileSize2 )
					{
						return 1;
					}
					if( dwFileSize1 > dwFileSize2 )
					{
						return -1;
					}
					break;	// Sort alphabetically

				}
			}
		}
	}

	// Use custom sort algorithm (if available)
	IDMUSProdSortNode* pISortNode;
	IDMUSProdNode* pIParentNode;

	if( SUCCEEDED ( pINode1->GetParentNode( &pIParentNode ) ) )
	{
		if( pIParentNode )
		{
			HRESULT hr = E_FAIL; 
			int nResult;

			if( SUCCEEDED ( pIParentNode->QueryInterface( IID_IDMUSProdSortNode, (void **)&pISortNode ) ) )
			{
				hr = pISortNode->CompareNodes( pINode1, pINode2, &nResult );
				pISortNode->Release();
			}
			pIParentNode->Release();

			if( SUCCEEDED ( hr ) )
			{
				return nResult;
			}
		}
	}

	// Sort by node name
	if( SUCCEEDED ( pINode1->GetNodeName( &bstrName1 ) ) )
	{
		strName1 = bstrName1;
		::SysFreeString( bstrName1 );
	}

	if( SUCCEEDED ( pINode2->GetNodeName( &bstrName2 ) ) )
	{
		strName2 = bstrName2;
		::SysFreeString( bstrName2 );
	}

	return strName1.CompareNoCase( strName2 );
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnDeleteItem

void CTreeBar::OnDeleteItem( NM_TREEVIEW* pNMTreeView, CTreeCtrl* pTreeCtrl, LRESULT* pResult ) 
{
	CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( pNMTreeView->itemOld.hItem );
	if( pJzNode )
	{
		IDMUSProdNode* pINode = pJzNode->m_pINode;
		ASSERT( pINode != NULL );

		if( pINode )
		{
			// Clear stored hItem
			IDMUSProdNode* pIDocRootNode;
			GUID guidNodeId;

			pINode->GetNodeId( &guidNodeId );

			if( ::IsEqualGUID( guidNodeId, GUID_ProjectFolderNode )
			||  ::IsEqualGUID( guidNodeId, GUID_DirectoryNode ) )
			{
				CDirectoryNode* pDirNode = (CDirectoryNode *)pINode;

				pDirNode->m_hItem = NULL;
			}
			else if( ::IsEqualGUID( guidNodeId, GUID_FileNode ) )
			{
				CFileNode* pFileNode = (CFileNode *)pINode;

				pFileNode->m_hItem = NULL;
			}
			else if( SUCCEEDED ( pINode->GetDocRootNode( &pIDocRootNode ) ) )
			{
				if( pIDocRootNode )
				{
					if( pIDocRootNode == pINode )
					{
						CFileNode* pFileNode = theApp.GetFileByDocRootNode( pIDocRootNode );
						if( pFileNode )
						{
							pFileNode->m_hChildItem = NULL;
							pFileNode->Release();
						}
					}

					pIDocRootNode->Release();
				}
			}

			// Close associated editor window
			HWND hWndEditor;
			CFrameWnd* pWndEditor;

			if( SUCCEEDED ( pINode->GetEditorWindow(&hWndEditor) ) )
			{
				if( hWndEditor )
				{
					pWndEditor = (CFrameWnd *)CWnd::FromHandlePermanent( hWndEditor );
					if( pWndEditor )
					{
						CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pINode );
						if( pComponentDoc )
						{
							pWndEditor->ActivateFrame();
							pComponentDoc->PreCloseFrame( pWndEditor );
							pWndEditor->SendMessage( WM_CLOSE );
						}
					}
					else
					{
						::DestroyWindow( hWndEditor );
					}
				}
			}

			// Clean up clipboard IDataObject
			if( pINode == m_pIClipboardNode )
			{
				ASSERT( m_pIClipboardDataObject != NULL );

				if( ::OleIsCurrentClipboard( m_pIClipboardDataObject ) == S_OK )
				{
					OleFlushClipboard();
				}

				m_pIClipboardDataObject->Release();
				m_pIClipboardDataObject = NULL;
				m_pIClipboardNode = NULL;
			}

		}
	}
 
	if( pJzNode )
	{
		delete pJzNode;
		pTreeCtrl->SetItemData( pNMTreeView->itemOld.hItem, NULL );
	}

	*pResult = 0;	
}
		

/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnSelChanged

void CTreeBar::OnSelChanged( NM_TREEVIEW* pNMTreeView, CTreeCtrl* pTreeCtrl, LRESULT* pResult ) 
{
	IDMUSProdNode* pINode;

	if( pNMTreeView->itemOld.hItem )
	{
		CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( pNMTreeView->itemOld.hItem );
		if( pJzNode )
		{
			pINode = pJzNode->m_pINode;
			ASSERT( pINode != NULL );

			if( pINode )
			{
				pINode->OnNodeSelChanged( FALSE );
			}
		}
	}

	if( pNMTreeView->itemNew.hItem )
	{
		CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( pNMTreeView->itemNew.hItem );
		if( pJzNode )
		{
			pINode = pJzNode->m_pINode;
			ASSERT( pINode != NULL );

			if( pINode )
			{
				// Set the active Project to the Project containing this Node
				IDMUSProdProject* pIProject;

				if( SUCCEEDED ( theApp.m_pFramework->FindProject( pINode, &pIProject ) ) )
				{
					theApp.SetActiveProject( (CProject *)pIProject );
					pIProject->Release();
				}

				// Notify that node that it has been selected
				DisplayNodeProperties();
				pINode->OnNodeSelChanged( TRUE );
			}
		}
	}
	
	*pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::DoDrag

void CTreeBar::DoDrag( NM_TREEVIEW* pNMTreeView, CTreeCtrl* pTreeCtrl,
					   DWORD dwStartDragButton, LRESULT* pResult )
{
	*pResult = 1;

	CPoint pt( pNMTreeView->ptDrag );
	pTreeCtrl->ClientToScreen( &pt );

	HTREEITEM hItem = GetItemFromPoint( pt );
	if( hItem )
	{
		pTreeCtrl->SelectItem( hItem );

		// If already dragging, just return
		if( m_pDragImage )
		{
			return;
		}

		CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
		if( pJzNode )
		{
			IDMUSProdNode* pINode = pJzNode->m_pINode;
			ASSERT( pINode != NULL );

			if( pINode )
			{
				IDropSource* pIDropSource;
				IDataObject* pIDataObject;
				DWORD dwEffect;
				HRESULT hr;

				if( SUCCEEDED ( QueryInterface(IID_IDropSource, (void **)&pIDropSource ) ) )
				{
					if( SUCCEEDED ( pINode->CreateDataObject( &pIDataObject ) ) )
					{
						// Need to keep this around throughout drag/drop operation
						pINode->AddRef();

						// Create image used for drag-drop feedback
						HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
						if( hItem )
						{
							ASSERT( m_pDragImage == NULL );
							m_pDragImage = pTreeCtrl->CreateDragImage( hItem );
							ASSERT( m_pDragImage != NULL );

							if( m_pDragImage )
							{
								m_pDragImage->BeginDrag( 0, CPoint(8, 12) );
							}
						}

						// Start drag-drop operation
						DWORD dwOKDragEffects = DROPEFFECT_COPY;
						if( pINode->CanDelete() == S_OK )
						{
							dwOKDragEffects |= DROPEFFECT_MOVE;
						}

						m_dwStartDragButton = dwStartDragButton;
						m_pIDragNode = pINode;
						
						hr = ::DoDragDrop( pIDataObject, pIDropSource, dwOKDragEffects, &dwEffect );
	
						m_dwStartDragButton = 0;
						m_pIDragNode = NULL;

						// Delete image used for drag-drop feedback
						if( m_pDragImage )
						{
							m_pDragImage->EndDrag();

							delete m_pDragImage;
							m_pDragImage = NULL;
						}

						switch( hr )
						{
							case DRAGDROP_S_DROP:
								if( dwEffect & DROPEFFECT_MOVE )
								{
									DeleteTreeNode( pINode, 0 );
								}
								break;
						}

						pIDataObject->Release();
						pINode->Release();
						*pResult = 0;
					}

					pIDropSource->Release();
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnBeginDrag

void CTreeBar::OnBeginDrag( NM_TREEVIEW* pNMTreeView, CTreeCtrl* pTreeCtrl, LRESULT* pResult )
{
	DoDrag( pNMTreeView, pTreeCtrl, MK_LBUTTON, pResult ); 
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnBeginRDrag

void CTreeBar::OnBeginRDrag( NM_TREEVIEW* pNMTreeView, CTreeCtrl* pTreeCtrl, LRESULT* pResult )
{
	DoDrag( pNMTreeView, pTreeCtrl, MK_RBUTTON, pResult ); 
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::DeleteTreeNode

BOOL CTreeBar::DeleteTreeNode( IDMUSProdNode* pINode, WORD wFlags )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CString strMsg;
	GUID guidNodeId;
	int nAnswer;

	ASSERT( pINode != NULL );

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	ASSERT( pTreeCtrl != NULL );

	CWnd* pWndHadFocus = CWnd::GetFocus();

	// If node represents document, we want to delete its FileNode
	CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pINode );
	if( pComponentDoc )
	{
		if( pComponentDoc->m_pIDocRootNode == pINode )
		{
			pINode = pComponentDoc->m_pFileNode;
			ASSERT( pINode != NULL );
		}
	}

	if( pINode->CanDelete() != S_OK )
	{
		return FALSE;
	}

	if( FAILED ( pINode->GetNodeId( &guidNodeId ) ) )
	{
		return FALSE;
	}

	BOOL fReturn = FALSE;

	// Handle Projects
	if( IsEqualGUID ( guidNodeId, GUID_ProjectFolderNode ) )
	{
		CProject* pProject = (CProject *)pINode;

		short nWhichFiles = DPF_NO_FILES;
		BOOL fRuntimePrompt;

		// Are you sure?
		nAnswer = IDOK;

		if( wFlags & DTN_PROMPT_DELETE )
		{
			CDeleteProjectDlg DeleteProjectDlg;
			DeleteProjectDlg.m_pProject = pProject;

			nAnswer = DeleteProjectDlg.DoModal();

			nWhichFiles = DeleteProjectDlg.m_nWhichFiles;
			fRuntimePrompt = DeleteProjectDlg.m_fRuntimePrompt;
		}

		if( nAnswer == IDOK )
		{
			ASSERT( nWhichFiles != DPF_NO_FILES );

			// Delete Project files from Project Tree and hard drive
			fReturn = theApp.DeleteProject( pProject, nWhichFiles, fRuntimePrompt );
		}
	}

	// Handle directories
	else if( IsEqualGUID ( guidNodeId, GUID_DirectoryNode ) )
	{
		CDirectoryNode* pDirNode = (CDirectoryNode *)pINode;

		CString strPath;
		pDirNode->ConstructPath( strPath );

		CProject* pProject = theApp.GetProjectByProjectDir( strPath );
		if( pProject )
		{
			// Directory also resides in Project Tree as a Project
			// so delete the Project
			short nWhichFiles = DPF_NO_FILES;
			BOOL fRuntimePrompt;

			// Are you sure?
			nAnswer = IDOK;

			if( wFlags & DTN_PROMPT_DELETE )
			{
				CDeleteProjectDlg DeleteProjectDlg;
				DeleteProjectDlg.m_pProject = pProject;

				nAnswer = DeleteProjectDlg.DoModal();

				nWhichFiles = DeleteProjectDlg.m_nWhichFiles;
				fRuntimePrompt = DeleteProjectDlg.m_fRuntimePrompt;
			}

			if( nAnswer == IDOK )
			{
				ASSERT( nWhichFiles != DPF_NO_FILES );

				// Delete Project files from Project Tree and hard drive
				fReturn = theApp.DeleteProject( pProject, nWhichFiles, fRuntimePrompt );
			}

			pProject->Release();
		}
		else
		{
			// Are you sure?
			nAnswer = IDOK;

			if( wFlags & DTN_PROMPT_DELETE )
			{
				AfxFormatString1( strMsg, IDS_DELETE_DIR, pDirNode->m_strName );
				nAnswer = AfxMessageBox( strMsg, MB_OKCANCEL );
			}

			if( nAnswer == IDOK )
			{
				// Delete directory from Project Tree and hard drive
				pDirNode->AddRef();
				if( pDirNode->DeleteDirectoryToRecycleBin() )
				{
					fReturn = TRUE;
				}
				pDirNode->Release();
			}
		}
	}

	// Handle files
	else if( IsEqualGUID ( guidNodeId, GUID_FileNode ) )
	{
		CFileNode* pFileNode = (CFileNode *)pINode;

		// Are you sure?
		nAnswer = IDYES;

		if( wFlags & DTN_PROMPT_USER )
		{
			CJzNotifyNode* pJzNotifyNode;
			CFileNode* pTheFileNode;
			BOOL fReferenced = FALSE;

			// Set fReferenced flag
			POSITION pos = pFileNode->m_lstNotifyNodes.GetHeadPosition();
			while( pos )
			{
				pJzNotifyNode = static_cast<CJzNotifyNode*>( pFileNode->m_lstNotifyNodes.GetNext(pos) );
				
				pTheFileNode = theApp.GetFileByGUID( pJzNotifyNode->m_guidFile );
				if( pTheFileNode )
				{
					fReferenced = TRUE;
					pTheFileNode->Release();
					break;
				}
			}

			if( fReferenced
			&&  wFlags & DTN_PROMPT_REFERENCES )
			{
				if( pFileNode->m_hItem )
				{
					if( pTreeCtrl )
					{
						pTreeCtrl->EnsureVisible( pFileNode->m_hItem );
						pTreeCtrl->SelectItem( pFileNode->m_hItem );
					}
				}

				CDeleteFileDlg deleteDlg;
				deleteDlg.m_pFileNode = pFileNode;
				nAnswer = deleteDlg.DoModal();
			}
			else if( wFlags & DTN_PROMPT_DELETE )
			{
				CString strFileName;

				pFileNode->ConstructFileName( strFileName );
				if( strFileName.IsEmpty() )
				{
					AfxFormatString1( strMsg, IDS_DELETE_FILE, pFileNode->m_strName );
				}
				else
				{
					AfxFormatString1( strMsg, IDS_DELETE_FILE, strFileName );
				}
				nAnswer = AfxMessageBox( strMsg, MB_OKCANCEL );
			}
		}

		if( nAnswer == IDOK
		||  nAnswer == IDYES )
		{
			// Store filename
			CString strFileName;
			pFileNode->ConstructFileName( strFileName );

			// Delete node
			theApp.m_fDeleteFromTree = TRUE;
			if( SUCCEEDED ( pINode->DeleteNode( FALSE ) ) )
			{
				// Delete file from hard drive
				if( SUCCEEDED ( theApp.DeleteFileToRecycleBin( strFileName ) ) )
				{
					fReturn = TRUE;
				}
				else
				{
					// Put file back in the Project Tree
					BSTR bstrFileName = strFileName.AllocSysString();
					theApp.m_pFramework->ShowFile( bstrFileName );
				}
			}
			theApp.m_fDeleteFromTree = FALSE;
		}
	}

	// Handle all other nodes
	else
	{
		BOOL fPromptUser = (wFlags & DTN_PROMPT_DELETE) ? TRUE : FALSE;

		if( SUCCEEDED ( pINode->DeleteNode( fPromptUser ) ) )
		{
			fReturn = TRUE;
		}
	}

	if( pWndHadFocus == pTreeCtrl
	&&  pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return fReturn;
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnRightClick

void CTreeBar::OnRightClick( NMHDR* pNMHdr, CTreeCtrl* pTreeCtrl, CPoint pt, LRESULT* pResult ) 
{
	HINSTANCE hInstance;
	UINT nResourceId;

	HTREEITEM hItem = GetItemFromPoint( pt );
	if( hItem )
	{
		pTreeCtrl->SelectItem( hItem );

		CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
		if( pJzNode )
		{
			IDMUSProdNode* pINode = pJzNode->m_pINode;
			ASSERT( pINode != NULL );

			if( pINode )
			{
				if( SUCCEEDED ( pINode->GetRightClickMenuId(&hInstance, &nResourceId) ) )
				{
					HMENU hMenu;
					HMENU hMenuPopup;

					hMenu = ::LoadMenu( hInstance, MAKEINTRESOURCE(nResourceId) );

					if( hMenu )
					{
						hMenuPopup = ::GetSubMenu( hMenu, 0 );
						pINode->OnRightClickMenuInit( hMenuPopup );

						m_pINodeRightMenu = pINode;
						::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
										  pt.x, pt.y, 0, GetSafeHwnd(), NULL );

						DestroyMenu( hMenu );
					}
				}
			}
		}
	}

	pTreeCtrl->SetFocus();
	
	*pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnSetFocus

void CTreeBar::OnSetFocus( NMHDR* pNMHdr, CTreeCtrl* pTreeCtrl, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Set the active Project to the Project containing this Node
	HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
	if( hItem )
	{
		CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
		if( pJzNode )
		{
			IDMUSProdNode* pINode = pJzNode->m_pINode;
			ASSERT( pINode != NULL );

			if( pINode )
			{
				IDMUSProdProject* pIProject;

				if( SUCCEEDED ( theApp.m_pFramework->FindProject( pINode, &pIProject ) ) )
				{
					theApp.SetActiveProject( (CProject *)pIProject );
					pIProject->Release();
				}
			}
		}
	}

	*pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::RightClickMenu

void CTreeBar::RightClickMenu( IDMUSProdNode* pINode, CPoint pt ) 
{
	ASSERT( pINode != NULL );

	if( pINode )
	{
		HINSTANCE hInstance;
		UINT nResourceId;

		if( SUCCEEDED ( pINode->GetRightClickMenuId(&hInstance, &nResourceId) ) )
		{
			HMENU hMenu;
			HMENU hMenuPopup;

			hMenu = ::LoadMenu( hInstance, MAKEINTRESOURCE(nResourceId) );

			if( hMenu )
			{
				hMenuPopup = ::GetSubMenu( hMenu, 0 );
				pINode->OnRightClickMenuInit( hMenuPopup );

				m_pINodeRightMenu = pINode;
				::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
								  pt.x, pt.y, 0, GetSafeHwnd(), NULL );

				DestroyMenu( hMenu );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::GetItemFromPoint

HTREEITEM CTreeBar::GetItemFromPoint( CPoint pt )
{
	HTREEITEM hItem = NULL;

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl )
	{
		UINT uFlags;

		// Convert to pTreeCtrl coordinates
		::ScreenToClient( pTreeCtrl->GetSafeHwnd(), &pt );

		// Get HTREEITEM at the current cursor position
		hItem = pTreeCtrl->HitTest( pt, &uFlags );
	}

	return hItem;
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::SortChildren

void CTreeBar::SortChildren( HTREEITEM hParentItem )
{
	ASSERT( hParentItem != NULL );

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl == NULL )
	{
		return;
	}

	BOOL fSort = FALSE;

	if( hParentItem == TVI_ROOT )
	{
		fSort = TRUE;
	}
	else
	{
		CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hParentItem );
		if( pJzNode )
		{
			IDMUSProdNode* pINode = pJzNode->m_pINode;
			ASSERT( pINode != NULL );

			if( pINode )
			{
				GUID guidNodeId;

				if( SUCCEEDED ( pINode->GetNodeId( &guidNodeId ) ) )
				{
					if( IsEqualGUID ( guidNodeId, GUID_ProjectFolderNode )
					||  IsEqualGUID ( guidNodeId, GUID_DirectoryNode ) )
					{
						fSort = TRUE;
					}
				}
			}
		}
	}

	// For now force everything with children to sort
	fSort = TRUE;

	if( fSort )
	{
		TV_SORTCB sort;
		sort.hParent = hParentItem;
		sort.lpfnCompare = CompareTreeItems;
		sort.lParam = 0;

		pTreeCtrl->SortChildrenCB( &sort );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::DisplayNodeProperties

void CTreeBar::DisplayNodeProperties( void )
{
	if( theApp.m_nShowNodeInTree != SHOW_NODE_IN_TREE )
	{
		// Don't bother updating properties if we are opening a file
		// and we don't even want to show it in the Project Tree
		return;
	}

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl == NULL )
	{
		return;
	}

	CWnd* pWndHadFocus = CWnd::GetFocus();

	IDMUSProdPropSheet* pIPropSheet = theApp.GetPropertySheet();
	if( pIPropSheet )
	{
		if( pIPropSheet->IsShowing() == S_OK )
		{
			HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
			if( hItem )
			{
				CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
				if( pJzNode )
				{
					IDMUSProdNode* pINode = pJzNode->m_pINode;
					ASSERT( pINode != NULL );

					BOOL fHaveProperties = FALSE;

					if( pINode )
					{
						// Get properties for node
						IDMUSProdPropPageObject* pIPageObject;
						if( SUCCEEDED ( pINode->QueryInterface( IID_IDMUSProdPropPageObject, (void **)&pIPageObject ) ) )
						{
							fHaveProperties = TRUE;
							if( pIPropSheet->IsEqualPageManagerObject(pIPageObject) != S_OK )
							{
								pIPageObject->OnShowProperties();
							}
							pIPageObject->Release();
						}
						else
						{
							// Get properties for referenced file
							IDMUSProdReferenceNode* pIReferenceNode;
							if( SUCCEEDED ( pINode->QueryInterface( IID_IDMUSProdReferenceNode, (void **)&pIReferenceNode ) ) )
							{
								IDMUSProdNode* pIDocRootNode;
								if( SUCCEEDED ( pIReferenceNode->GetReferencedFile( &pIDocRootNode ) ) )
								{
									IDMUSProdPropPageObject* pIPageObject;
									if( SUCCEEDED ( pIDocRootNode->QueryInterface( IID_IDMUSProdPropPageObject, (void **)&pIPageObject ) ) )
									{
										fHaveProperties = TRUE;
										if( pIPropSheet->IsEqualPageManagerObject(pIPageObject) != S_OK )
										{
											pIPageObject->OnShowProperties();
										}
										pIPageObject->Release();
									}

									pIDocRootNode->Release();
								}

								pIReferenceNode->Release();
							}
						}

						if( fHaveProperties == FALSE )
						{
							// Display 'No Properties'
							CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
							if( pMainFrame )
							{
								if( pMainFrame->m_wndProperties.m_pPropertySheet )
								{
									pMainFrame->m_wndProperties.m_pPropertySheet->RemoveCurrentPageManager();
								}
							}
						}
					}
				}
			}
		}

		pIPropSheet->Release();
	}

	if( pWndHadFocus == pTreeCtrl
	&&  pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar message handlers

BEGIN_MESSAGE_MAP(CTreeBar, CDialogBar)
	//{{AFX_MSG_MAP(CTreeBar)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_COMMAND(IDM_DRAG_MOVE, OnDragRMenuMove)
	ON_COMMAND(IDM_DRAG_COPY, OnDragRMenuCopy)
	ON_COMMAND(IDM_DRAG_CANCEL, OnDragRMenuCancel)
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


#define TREE_BORDER	5

/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnSize

void CTreeBar::OnSize(UINT nType, int cx, int cy) 
{
    if( nType == SIZE_MINIMIZED )
	{
        return;
	}

	// Exit if we are not fully created yet
	if( !::IsWindow(m_wndSplitter) )
	{
		return;
	}

	// Move the splitter bar
	if( !(m_dwStyle & CBRS_FLOATING) )
	{
		if( m_dwStyle & CBRS_ALIGN_LEFT )
		{
			m_wndSplitter.SetWindowPos( NULL, cx - 6, 1, 5, cy - 2,
										SWP_NOACTIVATE | SWP_NOZORDER );
		}
		else if( m_dwStyle & CBRS_ALIGN_RIGHT )
		{
			m_wndSplitter.SetWindowPos( NULL, 1, 1, 5, cy - 2,
										SWP_NOACTIVATE | SWP_NOZORDER );
		}
	}

	// find tree list control
	CWnd* pTreeCtrl = GetDlgItem( IDC_TREE );
	if( pTreeCtrl == NULL )
	{
		return;
	}

	// recalc size of tree list control to fit in control bar
	int nX  = TREE_BORDER + 1;
	int nY  = TREE_BORDER;
	int nCX = cx - (TREE_BORDER << 1);
	int nCY = cy - (TREE_BORDER << 1);

	if( !(m_dwStyle & CBRS_FLOATING) )
	{
		if( m_dwStyle & CBRS_ALIGN_LEFT )
		{
			nCX = cx - ((TREE_BORDER << 1) + 6);
		}
		else if( m_dwStyle & CBRS_ALIGN_RIGHT )
		{
			nX  = (TREE_BORDER << 1);
			nCX = cx - ((TREE_BORDER << 1) + 6);
		}
	}
    pTreeCtrl->MoveWindow( nX, nY, nCX, nCY, TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnCommand

BOOL CTreeBar::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if( HIWORD(wParam) == 0 )	// menu command
	{
		if( m_pINodeRightMenu )	// can receive menu commands when m_pINodeRightMenu is NULL
		{
			CWnd* pWndHadFocus = CWnd::GetFocus();
			m_pINodeRightMenu->OnRightClickMenuSelect( LOWORD(wParam) );
			m_pINodeRightMenu = NULL ;

			if( m_pEditCtrl == NULL )
			{
				CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
				if( pTreeCtrl )
				{
					if( pWndHadFocus == pTreeCtrl
					&&  pWndHadFocus != CWnd::GetFocus() )
					{
						pWndHadFocus->SetFocus();
					}
				}
			}
			return TRUE;
		}
	}
	
	return CDialogBar::OnCommand( wParam, lParam );
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::PreTranslateMessage

BOOL CTreeBar::PreTranslateMessage( MSG* pMsg ) 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	if( m_pEditCtrl )
	{
		CheckEditControl();	// May set m_pEditCtrl to NULL
	}

	if( m_pEditCtrl == NULL )
	{
		if( ::TranslateAccelerator(pMainFrame->m_hWnd, m_hAcceleratorTable, pMsg) )
		{
			return TRUE;
		}
	}

	if( pMsg->message == WM_KEYDOWN )
	{
		if( !(pMsg->lParam & 0x40000000) )
		{
			switch( pMsg->wParam )
			{
				case VK_RETURN:
				{
					CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
					if( pTreeCtrl )
					{
						HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
						if( hItem )
						{
							CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
							if( pJzNode )
							{
								IDMUSProdNode* pINode = pJzNode->m_pINode;
								ASSERT( pINode != NULL );

								if( pINode )
								{
									LRESULT lResult;

									CWnd* pWndHadFocus = CWnd::GetFocus();
									OnDoubleClick( NULL, pTreeCtrl, TRUE, &lResult );
									if( pWndHadFocus == pTreeCtrl
									&&  pWndHadFocus != CWnd::GetFocus() )
									{
										pWndHadFocus->SetFocus();
									}
								}
							}
						}
					}
					return TRUE;
				}

				case VK_F2:
					if( m_pEditCtrl == NULL )
					{
						CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
						if( pTreeCtrl )
						{
							HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
							if( hItem )
							{
								CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
								if( pJzNode )
								{
									IDMUSProdNode* pINode = pJzNode->m_pINode;
									ASSERT( pINode != NULL );

									if( pINode )
									{
										theApp.m_pFramework->EditNodeLabel( pINode );
									}
								}
							}
						}
						return TRUE;
					}
					break;
			}
		}
	}
	
	return CDialogBar::PreTranslateMessage( pMsg );
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnDestroy

void CTreeBar::OnDestroy() 
{
	if( m_pEditCtrl )
	{
		delete m_pEditCtrl;
		m_pEditCtrl = NULL;
	}
	
	HRESULT hr = ::RevokeDragDrop( GetSafeHwnd() );
	ASSERT(SUCCEEDED(hr));

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl )
	{
		CImageList* pImageList = pTreeCtrl->SetImageList( NULL, TVSIL_NORMAL );
		if( pImageList )
		{
			pImageList->DeleteImageList();
		}
	}

	CDialogBar::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnDragRMenuMove

void CTreeBar::OnDragRMenuMove() 
{
	m_dwDragRMenuEffect = DROPEFFECT_MOVE;	
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnDragRMenuCopy

void CTreeBar::OnDragRMenuCopy() 
{
	m_dwDragRMenuEffect = DROPEFFECT_COPY;	
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnDragRMenuCancel

void CTreeBar::OnDragRMenuCancel() 
{
	m_dwDragRMenuEffect = DROPEFFECT_NONE;	
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnDropFiles

void CTreeBar::OnDropFiles( HDROP hDropInfo ) 
{
	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	ASSERT( pTreeCtrl != NULL );

	// Activate Project Tree node under cursor
	if( pTreeCtrl )
	{
		CPoint pt;

		GetCursorPos( &pt );
		HTREEITEM hItem = GetItemFromPoint( pt );
		if( hItem )
		{
			pTreeCtrl->SelectItem( hItem );

			// Give the Node a chance to deal with the file
			CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
			if( pJzNode )
			{
				IDMUSProdNode* pINode = pJzNode->m_pINode;
				ASSERT( pINode != NULL );

				if( pINode )
				{
					IDMUSProdNodeDrop* pINodeDrop;

					if( SUCCEEDED ( pINode->QueryInterface( IID_IDMUSProdNodeDrop, (void **)&pINodeDrop ) ) )
					{
						HRESULT hr = pINodeDrop->OnDropFiles( hDropInfo );

						pINodeDrop->Release();
						if( hr == S_OK )
						{
							// Node took care of drop so just return
							return;
						}
					}
				}
			}
		}
	}

	// Attempt to open the file
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );
	
	pMainFrame->DropFiles( hDropInfo );
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnUpdateEditCut

void CTreeBar::OnUpdateEditCut( CCmdUI* pCmdUI ) 
{
	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl
	&&  pTreeCtrl == CWnd::GetFocus() )
	{
		HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
		if( hItem )
		{
			CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
			if( pJzNode )
			{
				IDMUSProdNode* pINode = pJzNode->m_pINode;
				ASSERT( pINode != NULL );
				
				if( pINode )
				{
					if( pINode->CanCut() == S_OK )
					{
						pCmdUI->Enable( TRUE );
						return;
					}
				}
			}
		}
	}
	
	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnEditCut

void CTreeBar::OnEditCut( void ) 
{
	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl
	&&  pTreeCtrl == CWnd::GetFocus() )
	{
		HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
		if( hItem )
		{
			CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
			if( pJzNode )
			{
				IDMUSProdNode* pINode = pJzNode->m_pINode;
				ASSERT( pINode != NULL );
				
				if( pINode )
				{
					if( pINode->CanCut() == S_OK )
					{
						if( CopyToClipboard( pINode ) )
						{
							DeleteTreeNode( pINode, 0 );
						}
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnUpdateEditCopy

void CTreeBar::OnUpdateEditCopy( CCmdUI* pCmdUI ) 
{
	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl
	&&  pTreeCtrl == CWnd::GetFocus() )
	{
		HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
		if( hItem )
		{
			CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
			if( pJzNode )
			{
				IDMUSProdNode* pINode = pJzNode->m_pINode;
				ASSERT( pINode != NULL );
				
				if( pINode )
				{
					if( pINode->CanCopy() == S_OK )
					{
						pCmdUI->Enable( TRUE );
						return;
					}
				}
			}
		}
	}
	
	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::CopyToClipboard

BOOL CTreeBar::CopyToClipboard( IDMUSProdNode* pINode ) 
{
	ASSERT( pINode != NULL );

	BOOL fReturn = FALSE;

	if( pINode->CanCopy() == S_OK )
	{
		IDataObject* pIDataObject;

		if( SUCCEEDED ( pINode->CreateDataObject( &pIDataObject ) ) )
		{
			if( ::OleSetClipboard( pIDataObject ) == S_OK )
			{
				if( m_pIClipboardDataObject )
				{
					m_pIClipboardDataObject->Release();
				}
				m_pIClipboardDataObject = pIDataObject;
				m_pIClipboardDataObject->AddRef();
				m_pIClipboardNode = pINode;

				fReturn = TRUE;
			}

			pIDataObject->Release();
		}
	}

	return fReturn;
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnEditCopy

void CTreeBar::OnEditCopy( void ) 
{
	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl
	&&  pTreeCtrl == CWnd::GetFocus() )
	{
		HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
		if( hItem )
		{
			CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
			if( pJzNode )
			{
				IDMUSProdNode* pINode = pJzNode->m_pINode;
				ASSERT( pINode != NULL );
				
				if( pINode )
				{
					if( pINode->CanCopy() == S_OK )
					{
						CopyToClipboard( pINode );
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnUpdateEditPaste

void CTreeBar::OnUpdateEditPaste( CCmdUI* pCmdUI ) 
{
	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl
	&&  pTreeCtrl == CWnd::GetFocus() )
	{
		HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
		if( hItem )
		{
			CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
			if( pJzNode )
			{
				IDMUSProdNode* pINode = pJzNode->m_pINode;
				ASSERT( pINode != NULL );
				
				if( pINode )
				{
					IDataObject* pIDataObject;

					// Get the IDataObject
					if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
					{
						BOOL fWillSetReference;

						// Determine if node can paste this object
						HRESULT hr = pINode->CanPasteFromData( pIDataObject, &fWillSetReference );
						
						pIDataObject->Release();
						pIDataObject = NULL;
						
						if( hr == S_OK )
						{
							pCmdUI->Enable( TRUE );
							return;
						}
					}
				}
			}
		}
	}
	
	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnEditPaste

void CTreeBar::OnEditPaste( void ) 
{
	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl
	&&  pTreeCtrl == CWnd::GetFocus() )
	{
		HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
		if( hItem )
		{
			CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
			if( pJzNode )
			{
				IDMUSProdNode* pINode = pJzNode->m_pINode;
				ASSERT( pINode != NULL );
				
				if( pINode )
				{
					IDataObject* pIDataObject;

					// Get the IDataObject
					if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
					{
						// Let node handle paste
						theApp.m_fInPasteFromData = TRUE;
						pINode->PasteFromData( pIDataObject );
						theApp.m_fInPasteFromData = FALSE;
						
						pIDataObject->Release();
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnUpdateEditDelete

void CTreeBar::OnUpdateEditDelete( CCmdUI* pCmdUI ) 
{
	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl
	&&  pTreeCtrl == CWnd::GetFocus() )
	{
		HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
		if( hItem )
		{
			CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
			if( pJzNode )
			{
				IDMUSProdNode* pINode = pJzNode->m_pINode;
				ASSERT( pINode != NULL );
				
				if( pINode )
				{
					if( pINode->CanDelete() == S_OK )
					{
						pCmdUI->Enable( TRUE );
						return;
					}
				}
			}
		}
	}
	
	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar::OnEditDelete

void CTreeBar::OnEditDelete( void ) 
{
	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl
	&&  pTreeCtrl == CWnd::GetFocus() )
	{
		HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
		if( hItem )
		{
			CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
			if( pJzNode )
			{
				IDMUSProdNode* pINode = pJzNode->m_pINode;
				ASSERT( pINode != NULL );
				
				if( pINode )
				{
					DeleteTreeNode( pINode, DTN_PROMPT_USER );
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar GetSortType

int CTreeBar::GetSortType( void )
{
	return m_nSortType;
}


/////////////////////////////////////////////////////////////////////////////
// CTreeBar SetSortType

void CTreeBar::SetSortType( int nNewSortType )
{
	if( m_nSortType != nNewSortType )
	{
		ASSERT( (nNewSortType == TREE_SORTBY_NAME)
			||	(nNewSortType == TREE_SORTBY_TYPE)
			||	(nNewSortType == TREE_SORTBY_SIZE) );

		if( (nNewSortType == TREE_SORTBY_NAME)
		||	(nNewSortType == TREE_SORTBY_TYPE)
		||	(nNewSortType == TREE_SORTBY_SIZE) )
		{
			// Set the new sort type
			m_nSortType = nNewSortType;

			// Update the registry
			{
				LONG  lResult;
				HKEY  hKeyOpen;
				DWORD dwDisposition;

				lResult = RegCreateKeyEx( HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\DMUSProducer"),
										  0, 0, REG_OPTION_NON_VOLATILE,
										  KEY_ALL_ACCESS, NULL, &hKeyOpen, &dwDisposition );
				if( lResult == ERROR_SUCCESS )
				{
					DWORD dwCbData = sizeof( DWORD );
					::RegSetValueEx( hKeyOpen, _T("TreeSortBy"), 0, REG_DWORD, (LPBYTE)&m_nSortType, dwCbData);

					RegCloseKey( hKeyOpen );
				}
			}

			// Now resort File nodes in all Projects in the Project Tree
			IDMUSProdProject* pINextProject;
			HRESULT hr = theApp.GetFirstProject( &pINextProject );
			while( SUCCEEDED( hr )  &&  pINextProject )
			{
				IDMUSProdProject* pIProject = pINextProject;
				CProject* pProject = (CProject *)pIProject;

				pProject->SortTree();

				hr = theApp.GetNextProject( pIProject, &pINextProject );
				pIProject->Release();
			}
		}
	}
}


//*****************************************************************************
//***** CSplitter
//*****************************************************************************

BEGIN_MESSAGE_MAP(CSplitter, CWnd)
	//{{AFX_MSG_MAP(CSplitter)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()

//*****************************************************************************
CSplitter::CSplitter() :
	m_rcTrack(0, 0, 0, 0),
	m_fTracking(FALSE),
	m_fVisible(FALSE)
{
}

//*****************************************************************************
CSplitter::~CSplitter()
{
}

//*****************************************************************************
BOOL CSplitter::Create( CWnd *pParent )
{

	// Create our splitter class
	WNDCLASS wc;

	ZeroMemory( &wc, sizeof(wc) );
	wc.lpszClassName = "DittoSplitterClass";
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = ::DefWindowProc;
	wc.hInstance     = AfxGetInstanceHandle();
	wc.hCursor       = theApp.LoadCursor( MAKEINTRESOURCE(IDC_SPLITTER) );
	wc.hbrBackground = (HBRUSH)( COLOR_BTNFACE + 1 );

	// Register our splitter class
	AfxRegisterClass( &wc );

	// Create our splitter
	return CWnd::Create( wc.lpszClassName, "", WS_VISIBLE | WS_CHILD, 
						 CRect(0, 0, 0, 0), pParent, IDC_SPLITTER );
}

//*****************************************************************************
BOOL CSplitter::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}

//*****************************************************************************
void CSplitter::OnPaint()
{
	CPaintDC dc( this );
	CRect rc;
	CBrush brush;

	GetClientRect( &rc );
	dc.Draw3dRect( &rc, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW) );

	if( brush.CreateSolidBrush( GetSysColor(COLOR_BTNFACE) ) )
	{
		CBrush* pOldBrush = dc.SelectObject( &brush );
		rc.InflateRect( -1, -1 );
		dc.FillRect( &rc, &brush );
		dc.SelectObject( pOldBrush );
	}		
}

//*****************************************************************************
void CSplitter::OnLButtonDown( UINT nFlags, CPoint point )
{
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	// Capture the mouse
	SetCapture();

	// Set our initial splitter position in our main frame's client coordinates
	GetWindowRect( &m_rcTrack );
	pMainFrame->ScreenToClient( &m_rcTrack );
	m_rcTrack.top++; m_rcTrack.bottom--; m_rcTrack.right--;

	// Set our tracking flag
	m_fTracking = TRUE;

	// Draw our initial tracker
	InvertTracker();
}

//*****************************************************************************
void CSplitter::OnLButtonUp(UINT nFlags, CPoint point)
{
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	BOOL fWasTracking = m_fTracking;

	OnEndCapture();

	if( fWasTracking )
	{
		CDialogBar* pTreeBar = (CDialogBar *)GetParent();

		if( pTreeBar )
		{
			if( !(pTreeBar->m_dwStyle & CBRS_FLOATING) )
			{
				if( pTreeBar->m_dwStyle & CBRS_ALIGN_LEFT )
				{
					pMainFrame->EndTrack( m_rcTrack.left + 4 );
				}
				else if( pTreeBar->m_dwStyle & CBRS_ALIGN_RIGHT )
				{
					CRect rect;

					pMainFrame->GetClientRect( &rect );
					pMainFrame->EndTrack( rect.Width() - m_rcTrack.left );
				}
			}
		}
	}
}

//*****************************************************************************
void CSplitter::OnMouseMove( UINT nFlags, CPoint point )
{

	if( m_fTracking )
	{
		CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
		ASSERT( pMainFrame != NULL );

		// Convert out point to client coordinates of our frame
		ClientToScreen( &point );
		pMainFrame->ScreenToClient( &point );

		// Get the client are of our frame
		CRect rcClient;
		pMainFrame->GetClientRect( &rcClient );

		// Force the point to be in our client area
		if( (point.x + 34) > rcClient.right )
		{
			point.x = rcClient.right - 34;
		}
		if( (point.x - 31) < rcClient.left )
		{
			point.x = rcClient.left + 31;
		}

		// If the point has changed since the last mouse move, then update change
		if( m_rcTrack.left != (point.x - 1) )
		{
			InvertTracker();
			m_rcTrack.left  = point.x - 1;
			m_rcTrack.right = point.x + 3;
			InvertTracker();
		}
	}
}

//*****************************************************************************
void CSplitter::OnCancelMode()
{
	OnEndCapture();
	CWnd::OnCancelMode();
}

//*****************************************************************************
void CSplitter::OnCaptureChanged( CWnd *pWnd )
{
	OnEndCapture();
	CWnd::OnCaptureChanged( pWnd );
}

//*****************************************************************************
void CSplitter::OnEndCapture()
{
	if( m_fVisible )
	{
		InvertTracker();
	}

	if( m_fTracking )
	{
		ReleaseCapture();
		m_fTracking = FALSE;
	}
}

//*****************************************************************************
void CSplitter::InvertTracker()
{
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	// Get the DC of our main frame
	CDC *pDC = pMainFrame->GetDC();

	// Create our inverted brush pattern (looks just like frame window sizing)
	CBrush *pBrush = CDC::GetHalftoneBrush();

	// Select the brush into our DC  
	HBRUSH hBrushStock = NULL;
	if( pBrush != NULL )
	{
		hBrushStock = (HBRUSH)SelectObject( pDC->m_hDC, pBrush->m_hObject );
	}

	// Draw our tracking line
	pDC->PatBlt( m_rcTrack.left, m_rcTrack.top, m_rcTrack.Width(), m_rcTrack.Height(), PATINVERT );

	// Free our brush and DC
	if (hBrushStock != NULL)
	{
		SelectObject( pDC->m_hDC, hBrushStock );
	}
	ReleaseDC( pDC );

	// Toggle visible flag
	m_fVisible = !m_fVisible;
}
