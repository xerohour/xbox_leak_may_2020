// ComponentView.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "MainFrm.h"
#include "ComponentCntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComponentView

IMPLEMENT_DYNCREATE(CComponentView, CView)

CComponentView::CComponentView()
{
	m_pEditorCtrl = NULL;
	m_pINode = NULL;
	m_pIProject = NULL;
	m_pIEditor = NULL;
	m_fSeed = FALSE;
}

CComponentView::~CComponentView()
{
	if( m_pINode )
	{
		m_pINode->Release();
	}

	if( m_pIProject )
	{
		m_pIProject->Release();
	}

	if( m_pIEditor )
	{
		m_pIEditor->Release();
	}
}


BEGIN_MESSAGE_MAP(CComponentView, CView)
	//{{AFX_MSG_MAP(CComponentView)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_SETUP, OnUpdateFilePrintSetup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComponentView drawing

void CComponentView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	m_pEditorCtrl = NULL;
}

void CComponentView::OnDraw(CDC* pDC)
{
	CComponentDoc* pDoc = GetDocument();
	ASSERT_VALID( pDoc );
}


/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

BOOL CComponentView::IsSelected(const CObject* pDocItem) const
{
	// The implementation below is adequate if your selection consists of
	//  only CComponentCntrItem objects.  To handle different selection
	//  mechanisms, the implementation here should be replaced.

	// TODO: implement this function that tests for a selected OLE client item

	return pDocItem == m_pEditorCtrl;
}

/////////////////////////////////////////////////////////////////////////////
// CComponentView diagnostics

#ifdef _DEBUG
void CComponentView::AssertValid() const
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	CView::AssertValid();
}

void CComponentView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


//////////////////////////////////////////////////////////////////////////////
// CComponentView::GetDocument

CComponentDoc* CComponentView::GetDocument() // non-debug version is inline
{
	ASSERT( m_pDocument->IsKindOf(RUNTIME_CLASS(CComponentDoc)) );

	return (CComponentDoc*)m_pDocument;
}


/////////////////////////////////////////////////////////////////////////////
// CComponentView message handlers

//////////////////////////////////////////////////////////////////////////////
// CComponentView::OnSize

void CComponentView::OnSize( UINT nType, int cx, int cy ) 
{
	CView::OnSize(nType, cx, cy);

	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem( this );

	if( pActiveItem != NULL )
	{
		CRect rect;

		GetClientRect( &rect );
		pActiveItem->SetItemRects( &rect );
	}
}


//////////////////////////////////////////////////////////////////////////////
// CComponentView::OnSetFocus

void CComponentView::OnSetFocus( CWnd* pOldWnd ) 
{
	CView::OnSetFocus( pOldWnd );
}


//////////////////////////////////////////////////////////////////////////////
// CComponentView::OnDestroy

void CComponentView::OnDestroy() 
{
	// Save Editor WindowPlacement
	if( m_pINode )
	{
		CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
		if( pTreeCtrl )
		{
			HTREEITEM hItem = theApp.m_pFramework->FindTreeItem( m_pINode );
			if( hItem )
			{
				CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
				if( pJzNode )
				{
					CFrameWnd* pFrame = GetParentFrame();
					if( pFrame )
					{
						pJzNode->m_wp.length = sizeof(pJzNode->m_wp);
						if( pFrame->GetWindowPlacement( &pJzNode->m_wp ) == 0 )
						{
							ZeroMemory( &pJzNode->m_wp, sizeof(pJzNode->m_wp) );
						}
					}
				}
			}
		}
	}

	CView::OnDestroy();

	CComponentDoc* pDoc = GetDocument();
	ASSERT_VALID( pDoc );

	if( m_pEditorCtrl )
	{
		m_pEditorCtrl->Release( OLECLOSE_NOSAVE );  // release OLE object
		pDoc->RemoveItem( m_pEditorCtrl );				// disconnect from document
		m_pEditorCtrl->InternalRelease();			// may 'delete m_pEditorCtrl'
	}
	
	if( m_pINode )
	{
		m_pINode->SetEditorWindow( NULL );
	}
}


//////////////////////////////////////////////////////////////////////////////
// CComponentView::OnUpdate

void CComponentView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) 
{
	if( lHint == 1 )
	{
		// PChannel name just changed
		RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN );
	}
	else
	{
		CView::OnUpdate( pSender, lHint, pHint );
	}
}


//////////////////////////////////////////////////////////////////////////////
// CComponentView::OnActivateView

void CComponentView::OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView ) 
{
	CView::OnActivateView( bActivate, pActivateView, pDeactiveView );
}


//////////////////////////////////////////////////////////////////////////////
// CComponentView::OnUpdateFileSave

void CComponentView::OnUpdateFileSave( CCmdUI* pCmdUI ) 
{
	CString strMenuText;
	CString strMenuHotKey;

	strMenuText.LoadString( IDS_SAVE_MENU_TEXT );
	strMenuHotKey.LoadString( IDS_SAVE_MENU_HOTKEY );

	if( m_pINode )
	{
		CString strFileName;
		
		CComponentDoc* pDoc = (CComponentDoc* )GetDocument();
		strFileName = pDoc->GetPathName();
		
		if( strFileName.IsEmpty() == FALSE )
		{
			int nFindPos = strFileName.ReverseFind( (TCHAR)'\\' );
			if( nFindPos != -1 )
			{
				strFileName = strFileName.Right( strFileName.GetLength() - nFindPos - 1 );
			}
			strMenuText += _T(" ");
			strMenuText += strFileName;
			strMenuText += strMenuHotKey;
			pCmdUI->SetText( strMenuText );
			pCmdUI->Enable( TRUE );
			return;
		}
	}

	strMenuText += strMenuHotKey;
	pCmdUI->SetText( strMenuText );
	pCmdUI->Enable( FALSE );
}


//////////////////////////////////////////////////////////////////////////////
// CComponentView::OnUpdateFileSaveAs

void CComponentView::OnUpdateFileSaveAs( CCmdUI* pCmdUI ) 
{
	CString strMenuText;

	if( m_pINode )
	{
		CString strFileName;
		
		CComponentDoc* pDoc = (CComponentDoc* )GetDocument();
		strFileName = pDoc->GetPathName();
		
		if( strFileName.IsEmpty() == FALSE )
		{
			int nFindPos = strFileName.ReverseFind( (TCHAR)'\\' );
			if( nFindPos != -1 )
			{
				strFileName = strFileName.Right( strFileName.GetLength() - nFindPos - 1 );
			}

			AfxFormatString1( strMenuText, IDS_SAVEAS_MENU_TEXT, strFileName );
			pCmdUI->SetText( strMenuText );
			pCmdUI->Enable( TRUE );
			return;
		}
	}

	AfxFormatString1( strMenuText, IDS_SAVEAS_MENU_TEXT, _T("") );
	pCmdUI->SetText( strMenuText );
	pCmdUI->Enable( FALSE );
}


//////////////////////////////////////////////////////////////////////////////
// CComponentView::OnUpdateFilePrint

void CComponentView::OnUpdateFilePrint( CCmdUI* pCmdUI ) 
{
	ASSERT( m_pIEditor != NULL );
	
	UINT nFlags = MF_DISABLED;

	if( m_pIEditor )
	{
		if( pCmdUI->m_pMenu )
		{
			m_pIEditor->OnInitMenuFilePrint( pCmdUI->m_pMenu->GetSafeHmenu(), ID_FILE_PRINT );
			nFlags = pCmdUI->m_pMenu->GetMenuState( ID_FILE_PRINT, MF_BYCOMMAND );
		}
	}

	// Following done to prevent MFC from overriding values set in OnInitMenuFilePrint()
	if( nFlags & MF_DISABLED
	||  nFlags & MF_GRAYED )
	{
		pCmdUI->Enable( FALSE );
	}
	else
	{
		pCmdUI->Enable( TRUE );
	}
}


//////////////////////////////////////////////////////////////////////////////
// CComponentView::OnFilePrint

void CComponentView::OnFilePrint() 
{
	ASSERT( m_pIEditor != NULL );
	
	if( m_pIEditor )
	{
		m_pIEditor->OnFilePrint();
	}
}


//////////////////////////////////////////////////////////////////////////////
// CComponentView::OnUpdateFilePrintPreview

void CComponentView::OnUpdateFilePrintPreview( CCmdUI* pCmdUI ) 
{
	ASSERT( m_pIEditor != NULL );
	
	UINT nFlags = MF_DISABLED;

	if( m_pIEditor )
	{
		if( pCmdUI->m_pMenu )
		{
			m_pIEditor->OnInitMenuFilePrintPreview( pCmdUI->m_pMenu->GetSafeHmenu(), ID_FILE_PRINT_PREVIEW );
			nFlags = pCmdUI->m_pMenu->GetMenuState( ID_FILE_PRINT_PREVIEW, MF_BYCOMMAND );
		}
	}

	// Following done to prevent MFC from overriding values set in OnInitMenuFilePrint()
	if( nFlags & MF_DISABLED
	||  nFlags & MF_GRAYED )
	{
		pCmdUI->Enable( FALSE );
	}
	else
	{
		pCmdUI->Enable( TRUE );
	}
}


//////////////////////////////////////////////////////////////////////////////
// CComponentView::OnFilePrintPreview

void CComponentView::OnFilePrintPreview() 
{
	ASSERT( m_pIEditor != NULL );
	
	if( m_pIEditor )
	{
		m_pIEditor->OnFilePrintPreview();
	}
}


//////////////////////////////////////////////////////////////////////////////
// CComponentView::OnUpdateFilePrintSetup

void CComponentView::OnUpdateFilePrintSetup( CCmdUI* pCmdUI ) 
{
	// Only enable 'Print Setup' if editor can print something
	OnUpdateFilePrint( pCmdUI );
}
