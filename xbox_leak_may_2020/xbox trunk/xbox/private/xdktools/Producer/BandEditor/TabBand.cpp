// TabBand.cpp : implementation file
//

#include "stdafx.h"
#include "BandEditorDLL.h"
#include "Band.h"
#include "TabBand.h"
#include <guiddlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabBand property page

CTabBand::CTabBand( CBandPropPageManager* pBandPropPageManager ) : CPropertyPage(CTabBand::IDD)
{
	//{{AFX_DATA_INIT(CTabBand)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	ASSERT( pBandPropPageManager != NULL );

	m_pBand = NULL;
	m_pPageManager = pBandPropPageManager;
	m_fNeedToDetach = FALSE;
}

CTabBand::~CTabBand()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabBand::SetBand

void CTabBand::SetBand( CBand* pBand )
{
	m_pBand = pBand;

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBand::EnableControls

void CTabBand::EnableControls( BOOL fEnable ) 
{
	m_editName.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CTabBand::UpdateControls

void CTabBand::UpdateControls()
{
	// Make sure controls have been created
	if( ::IsWindow(m_editName.m_hWnd) == FALSE )
	{
		return;
	}
	
	// Update controls
	m_editName.LimitText( DMUS_MAX_NAME );

	if( m_pBand )
	{
		EnableControls( TRUE );

		// Set name
		m_editName.SetWindowText( m_pBand->m_csName );
	}
	else
	{
		m_editName.SetWindowText( _T("") );
		
		EnableControls( FALSE );
	}
}

void CTabBand::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabBand)
	DDX_Control(pDX, IDC_NAME, m_editName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabBand, CPropertyPage)
	//{{AFX_MSG_MAP(CTabBand)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_NAME, OnKillFocusName)
	ON_BN_CLICKED(IDC_EDIT_GUID, OnEditGuid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTabBand message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabBand::OnSetActive

BOOL CTabBand::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBand::OnCreate

int CTabBand::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
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
// CTabBand::OnDestroy

void CTabBand::OnDestroy() 
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
// CTabBand::OnKillFocusName

void CTabBand::OnKillFocusName() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pIFramework != NULL );

	if( m_pBand )
	{
		CString strName;

		m_editName.GetWindowText( strName );

		// Strip leading and trailing spaces
		strName.TrimRight();
		strName.TrimLeft();

		if( strName.IsEmpty() )
		{
			m_editName.SetWindowText( m_pBand->m_csName );
		}
		else
		{
			if( strName.Compare( m_pBand->m_csName ) != 0 )
			{
				HRESULT hr = S_OK;
				BSTR bstrName;

				if( m_pBand->m_pIDocRootNode
				&&  m_pBand->m_pIDocRootNode != m_pBand )
				{
					bstrName = strName.AllocSysString();
					hr = m_pBand->ValidateNodeName( bstrName );
				}
				
				if( hr == S_OK )
				{
					bstrName = strName.AllocSysString();
					m_pBand->SetNodeName( bstrName );
					theApp.m_pIFramework->RefreshNode( m_pBand );
				}
				else
				{
					m_editName.SetWindowText( m_pBand->m_csName );
					m_editName.SetFocus();
				}
			}
		}
	}
}

void CTabBand::OnEditGuid() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pBand )
	{
		CGuidDlg dlg;
		memcpy( &dlg.m_guid, &m_pBand->m_guidBand, sizeof(GUID) );
		if( dlg.DoModal() == IDOK )
		{
			//m_pBand->m_pUndoMgr->SaveState( m_pBand, theApp.m_hInstance, IDS_UNDO_BAND_GUID );
			memcpy( &m_pBand->m_guidBand, &dlg.m_guid, sizeof(GUID) );
			m_pBand->SetModifiedFlag( TRUE );

			// Notify connected nodes that Band GUID has changed
			theApp.m_pIFramework->NotifyNodes( m_pBand, DOCROOT_GuidChange, NULL );
		}
	}
	
}
