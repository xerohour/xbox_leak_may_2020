// TabGraph.cpp : implementation file
//

#include "stdafx.h"
#include "ToolGraphDesignerDLL.h"
#include "Graph.h"
#include "TabGraph.h"
#include <dmusicf.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabGraph property page

CTabGraph::CTabGraph( CGraphPropPageManager* pGraphPropPageManager ) : CPropertyPage(CTabGraph::IDD)
{
	//{{AFX_DATA_INIT(CTabGraph)
	//}}AFX_DATA_INIT
	
	ASSERT( pGraphPropPageManager != NULL );

	m_pGraph = NULL;
	m_pPageManager = pGraphPropPageManager;
	m_fNeedToDetach = FALSE;
}

CTabGraph::~CTabGraph()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabGraph::SetGraph

void CTabGraph::SetGraph( CDirectMusicGraph* pGraph )
{
	m_pGraph = pGraph;

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTabGraph::SetModifiedFlag

void CTabGraph::SetModifiedFlag( void ) 
{
	ASSERT( m_pGraph != NULL );

	m_pGraph->SetModified( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CTabGraph::EnableControls

void CTabGraph::EnableControls( BOOL fEnable ) 
{
	m_editName.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CTabGraph::UpdateControls

void CTabGraph::UpdateControls() 
{
	// Make sure controls have been created
	if( ::IsWindow(m_editName.m_hWnd) == FALSE )
	{
		return;
	}
	
	// Update controls
	m_editName.LimitText( DMUS_MAX_NAME );

	if( m_pGraph )
	{
		EnableControls( TRUE );

		// Set name
		m_editName.SetWindowText( m_pGraph->m_strName );
	}
	else
	{
		m_editName.SetWindowText( _T("") );

		EnableControls( FALSE );
	}
}


void CTabGraph::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabGraph)
	DDX_Control(pDX, IDC_NAME, m_editName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabGraph, CPropertyPage)
	//{{AFX_MSG_MAP(CTabGraph)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_NAME, OnKillFocusName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTabGraph message handlers


/////////////////////////////////////////////////////////////////////////////
// CTabGraph::OnInitDialog

BOOL CTabGraph::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabGraph::OnSetActive

BOOL CTabGraph::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CGraphPropPageManager::sm_nActiveTab );

	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabGraph::OnCreate

int CTabGraph::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CTabGraph::OnDestroy

void CTabGraph::OnDestroy() 
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
// CTabGraph::OnKillFocusName

void CTabGraph::OnKillFocusName() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pGraphComponent != NULL );
	ASSERT( theApp.m_pGraphComponent->m_pIFramework != NULL );

	if( m_pGraph )
	{
		CString strName;

		m_editName.GetWindowText( strName );

		// Strip leading and trailing spaces
		strName.TrimRight();
		strName.TrimLeft();

		if( strName.IsEmpty() )
		{
			m_editName.SetWindowText( m_pGraph->m_strName );
		}
		else
		{
			if( strName.Compare( m_pGraph->m_strName ) != 0 )
			{
				BSTR bstrName = strName.AllocSysString();
				m_pGraph->SetNodeName( bstrName );
				theApp.m_pGraphComponent->m_pIFramework->RefreshNode( m_pGraph );
			}
		}
	}
}
