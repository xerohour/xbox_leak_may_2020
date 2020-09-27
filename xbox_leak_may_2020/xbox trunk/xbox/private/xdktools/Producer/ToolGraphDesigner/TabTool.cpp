// TabTool.cpp : implementation file
//

#include "stdafx.h"
#include "ToolGraphDesignerDLL.h"
#include "Tool.h"
#include "GraphCtl.h"
#include "TabTool.h"
#include "OlePropSheet.h"
#include "OlePropPage.h"
#include <dmusicf.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabTool property page

CTabTool::CTabTool( CToolPropPageManager* pToolPropPageManager ) : CPropertyPage(CTabTool::IDD)
{
	//{{AFX_DATA_INIT(CTabTool)
	//}}AFX_DATA_INIT
	
	ASSERT( pToolPropPageManager != NULL );

	m_pTool = NULL;
	m_pPageManager = pToolPropPageManager;
	m_fNeedToDetach = FALSE;
	m_dwBitsUI = 0;
}

CTabTool::~CTabTool()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabTool::SetTool

void CTabTool::SetTool( CTool* pTool )
{
	m_pTool = pTool;
	m_dwBitsUI = 0;

	if( m_pTool
	&&  m_pTool->m_dwBitsUI & UD_MULTIPLESELECT )
	{
		m_pTool = NULL;
		m_dwBitsUI |= UD_MULTIPLESELECT;
	}

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTabTool::GetBitsUI

DWORD CTabTool::GetBitsUI( void )
{
	return m_dwBitsUI;
}


/////////////////////////////////////////////////////////////////////////////
// CTabTool::SetModifiedFlag

void CTabTool::SetModifiedFlag( void ) 
{
	ASSERT( m_pTool != NULL );

	m_pTool->SetModified( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CTabTool::EnableControls

void CTabTool::EnableControls( BOOL fEnable ) 
{
	m_editName.EnableWindow( fEnable );
	m_btnToolProperties.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CTabTool::UpdateControls

void CTabTool::UpdateControls() 
{
	// Make sure controls have been created
	if( ::IsWindow(m_editName.m_hWnd) == FALSE )
	{
		return;
	}
	
	// Update controls
	m_editName.LimitText( DMUS_MAX_NAME );

	if( m_pTool )
	{
		EnableControls( TRUE );

		// Set name
		m_editName.SetWindowText( m_pTool->m_strName );
	}
	else
	{
		if( m_dwBitsUI & UD_MULTIPLESELECT )
		{
			CString strMultiple;
			strMultiple.LoadString( IDS_MULTIPLE_TOOLS_SELECTED );
			m_editName.SetWindowText( strMultiple );
		}
		else
		{
			m_editName.SetWindowText( _T("") );
		}

		EnableControls( FALSE );
	}
}


void CTabTool::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabTool)
	DDX_Control(pDX, IDC_BUTTON_TOOL_PROPERTIES, m_btnToolProperties);
	DDX_Control(pDX, IDC_NAME, m_editName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabTool, CPropertyPage)
	//{{AFX_MSG_MAP(CTabTool)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_NAME, OnKillFocusName)
	ON_BN_CLICKED(IDC_BUTTON_TOOL_PROPERTIES, OnToolProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTabTool message handlers


/////////////////////////////////////////////////////////////////////////////
// CTabTool::OnInitDialog

BOOL CTabTool::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabTool::OnSetActive

BOOL CTabTool::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();

	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabTool::OnCreate

int CTabTool::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Attach the window to the property page structure.
	// This has been done once already in the main application
	// since the main application owns the property sheet.
	// It needs to be done here so that the window handle can
	// be found in the DLLs handle map.
	if( !FromHandlePermanent( m_hWnd ) )
	{
		HWND hWnd = m_hWnd;
		m_hWnd = NULL;
		Attach( hWnd );
		m_fNeedToDetach = TRUE;
	}

	if( CPropertyPage::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}
	
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CTabTool::OnDestroy

void CTabTool::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Detach the window from the property page structure.
	// This will be done again by the main application since
	// it owns the property sheet.  It needs o be done here
	// so that the window handle can be removed from the
	// DLLs handle map.
	if( m_fNeedToDetach && m_hWnd )
	{
		HWND hWnd = m_hWnd;
		Detach();
		m_hWnd = hWnd;
	}

	CPropertyPage::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CTabTool::OnKillFocusName

void CTabTool::OnKillFocusName() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pTool == NULL
	||  m_pTool->m_pGraph == NULL )
	{
		ASSERT( 0 );
		return;

	}

	CString strName;
	m_editName.GetWindowText( strName );

	// Strip leading and trailing spaces
	strName.TrimRight();
	strName.TrimLeft();

	if( strName.IsEmpty() )
	{
		m_editName.SetWindowText( m_pTool->m_strName );
	}
	else
	{
		if( strName.Compare( m_pTool->m_strName ) != 0 )
		{
			// Save undo state
			m_pTool->m_pGraph->m_pUndoMgr->SaveState( m_pTool->m_pGraph, theApp.m_hInstance, IDS_UNDO_CHANGE_TOOL_NAME );
			
			// Update tool's name
			m_pTool->m_strName = strName;
			m_pTool->m_pGraph->SetModified( TRUE );

			//Refresh UI
			m_pTool->m_pGraph->Refresh();
		}
	}
}


#define PROPPAGE_WIDTH	250
#define PROPPAGE_HEIGHT	150

/////////////////////////////////////////////////////////////////////////////
// CTabTool::OnToolProperties

void CTabTool::OnToolProperties() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pTool == NULL
	||  m_pTool->m_pIDMTool == NULL 
	||  m_pTool->m_pGraph == NULL 
	||  m_pTool->m_pGraph->m_pGraphCtrl == NULL )
	{
		ASSERT( 0 );
		return;

	}

	// Store the original stream of DirectMusic tool data
	IStream* pIOrigStream = m_pTool->m_pIStream;
	if( pIOrigStream )
	{
		pIOrigStream->AddRef();
	}

	// Determine title
	CString strTitle;
	strTitle= m_pTool->m_pGraph->m_strName + _T(" - ") + m_pTool->m_strName;

	IDMUSProdUpdateObjectNow *pIDMUSProdUpdateObjectNow = NULL;
	m_pTool->QueryInterface( IID_IDMUSProdUpdateObjectNow, (void**)&pIDMUSProdUpdateObjectNow );

	// Display the property sheet
	COlePropSheet* pPropSheet = new COlePropSheet( m_pTool->m_pIDMTool, strTitle, pIDMUSProdUpdateObjectNow );
	if( pPropSheet )
	{
		int nResult = pPropSheet->DoModal();

		// Get the "updated" stream
		m_pTool->RefreshStreamOfData();
		IStream* pINewStream = m_pTool->m_pIStream;
		if( pINewStream )
		{
			pINewStream->AddRef();
		}

		// Was anything changed?
		if( m_pTool->IsStreamOfDataEqual( pIOrigStream ) == false )
		{
			// Handle "OK"
			if( nResult == IDOK )
			{
				// Save undo state
				m_pTool->m_pIStream = pIOrigStream;
				m_pTool->m_pGraph->m_pUndoMgr->SaveState( m_pTool->m_pGraph, theApp.m_hInstance, IDS_UNDO_CHANGE_TOOL_PROPERTIES );
			
				m_pTool->m_pIStream = pINewStream;
				m_pTool->m_pGraph->SetModified( TRUE );
			}

			// Handle "Cancel"
			else
			{
				// Restore original data
				IPersistStream* pIPersistStream;
				if( SUCCEEDED ( m_pTool->m_pIDMTool->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream ) ) )
				{
					StreamSeek( pIOrigStream, 0, STREAM_SEEK_SET );
					pIPersistStream->Load( pIOrigStream );

					RELEASE( pIPersistStream );
				}

				// Get the "original" stream
				m_pTool->RefreshStreamOfData();
			}

			// Sync changes with DMusic DLLs
			m_pTool->m_pGraph->SyncGraphWithDirectMusic();
		}
			
		RELEASE( pINewStream );
	}

	RELEASE( pIOrigStream );
	RELEASE( pIDMUSProdUpdateObjectNow );

	m_btnToolProperties.SetFocus();
}
