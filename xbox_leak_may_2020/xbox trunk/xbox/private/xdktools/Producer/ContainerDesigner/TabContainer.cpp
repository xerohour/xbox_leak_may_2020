// TabContainer.cpp : implementation file
//

#include "stdafx.h"
#include "TabContainer.h"
#include "ContainerDesignerDll.h"
#include "Container.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabContainer property page

CTabContainer::CTabContainer( CContainerPropPageManager* pContainerPropPageManager ) : CPropertyPage(CTabContainer::IDD)
{
	//{{AFX_DATA_INIT(CTabContainer)
	//}}AFX_DATA_INIT
	
	ASSERT( pContainerPropPageManager != NULL );

	m_pContainer = NULL;
	m_pPageManager = pContainerPropPageManager;
	m_fNeedToDetach = FALSE;
}

CTabContainer::~CTabContainer()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabContainer::SetContainer

void CTabContainer::SetContainer( CDirectMusicContainer* pContainer )
{
	m_pContainer = pContainer;

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTabContainer::SetModifiedFlag

void CTabContainer::SetModifiedFlag( void ) 
{
	ASSERT( m_pContainer != NULL );

	m_pContainer->SetModified( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CTabContainer::EnableControls

void CTabContainer::EnableControls( BOOL fEnable ) 
{
	if( m_pContainer
	&&  m_pContainer->IsInScript() )
	{
		m_editName.EnableWindow( FALSE );
	}
	else
	{
		m_editName.EnableWindow( fEnable );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabContainer::UpdateControls

void CTabContainer::UpdateControls() 
{
	// Make sure controls have been created
	if( ::IsWindow(m_editName.m_hWnd) == FALSE )
	{
		return;
	}
	
	// Update controls
	m_editName.LimitText( DMUS_MAX_NAME );

	if( m_pContainer )
	{
		EnableControls( TRUE );

		// Set name
		m_editName.SetWindowText( m_pContainer->m_strName );
	}
	else
	{
		m_editName.SetWindowText( _T("") );

		EnableControls( FALSE );
	}
}


void CTabContainer::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabContainer)
	DDX_Control(pDX, IDC_BUTTON_CONTENTS, m_btnContents);
	DDX_Control(pDX, IDC_NAME, m_editName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabContainer, CPropertyPage)
	//{{AFX_MSG_MAP(CTabContainer)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_NAME, OnKillFocusName)
	ON_BN_CLICKED(IDC_BUTTON_CONTENTS, OnButtonContents)
	ON_BN_DOUBLECLICKED(IDC_BUTTON_CONTENTS, OnDoubleClickedButtonContents)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTabContainer message handlers


/////////////////////////////////////////////////////////////////////////////
// CTabContainer::OnInitDialog

BOOL CTabContainer::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabContainer::OnSetActive

BOOL CTabContainer::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CContainerPropPageManager::sm_nActiveTab );

	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabContainer::OnCreate

int CTabContainer::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CTabContainer::OnDestroy

void CTabContainer::OnDestroy() 
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
// CTabContainer::OnKillFocusName

void CTabContainer::OnKillFocusName() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pContainerComponent != NULL );
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );

	if( m_pContainer )
	{
		CString strName;

		m_editName.GetWindowText( strName );

		// Strip leading and trailing spaces
		strName.TrimRight();
		strName.TrimLeft();

		if( strName.IsEmpty() )
		{
			m_editName.SetWindowText( m_pContainer->m_strName );
		}
		else
		{
			if( strName.Compare( m_pContainer->m_strName ) != 0 )
			{
				BSTR bstrName = strName.AllocSysString();
				m_pContainer->SetNodeName( bstrName );
				theApp.m_pContainerComponent->m_pIFramework8->RefreshNode( m_pContainer );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabContainer::OnButtonContents

void CTabContainer::OnButtonContents( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pContainer == NULL )
	{
		return;
	}

	m_pContainer->OnAddRemoveFiles();
}


/////////////////////////////////////////////////////////////////////////////
// CTabContainer::OnDoubleClickedButtonContents

void CTabContainer::OnDoubleClickedButtonContents( void ) 
{
	OnButtonContents();
}
