// TabAudioPathAudioPath.cpp : implementation file
//

#include "stdafx.h"
#include "TabAudioPathAudioPath.h"
#include "AudioPathDesignerDll.h"
#include "AudioPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathAudioPath property page

CTabAudioPathAudioPath::CTabAudioPathAudioPath( CAudioPathPropPageManager* pAudioPathPropPageManager ) : CPropertyPage(CTabAudioPathAudioPath::IDD)
{
	//{{AFX_DATA_INIT(CTabAudioPathAudioPath)
	//}}AFX_DATA_INIT
	
	ASSERT( pAudioPathPropPageManager != NULL );

	m_pAudioPath = NULL;
	m_pPageManager = pAudioPathPropPageManager;
	m_fNeedToDetach = FALSE;
}

CTabAudioPathAudioPath::~CTabAudioPathAudioPath()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathAudioPath::SetAudioPath

void CTabAudioPathAudioPath::SetAudioPath( CDirectMusicAudioPath* pAudioPath )
{
	m_pAudioPath = pAudioPath;

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathAudioPath::SetModifiedFlag

void CTabAudioPathAudioPath::SetModifiedFlag( void ) 
{
	ASSERT( m_pAudioPath != NULL );

	m_pAudioPath->SetModified( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathAudioPath::EnableControls

void CTabAudioPathAudioPath::EnableControls( BOOL fEnable ) 
{
	if( fEnable
	&&	m_pAudioPath
	&&	!m_pAudioPath->IsInSegment() )
	{
		m_editName.EnableWindow( TRUE );
	}
	else
	{
		m_editName.EnableWindow( FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathAudioPath::UpdateControls

void CTabAudioPathAudioPath::UpdateControls() 
{
	// Make sure controls have been created
	if( (m_pAudioPath == NULL)
	||	(::IsWindow(m_editName.m_hWnd) == FALSE) )
	{
		return;
	}
	
	// Update controls
	m_editName.LimitText( DMUS_MAX_NAME );

	if( m_pAudioPath )
	{
		EnableControls( TRUE );

		// Set name
		m_editName.SetWindowText( m_pAudioPath->m_strName );
	}
	else
	{
		m_editName.SetWindowText( _T("") );

		EnableControls( FALSE );
	}

	// Set the default sample rate
	/*
	int nID = IDC_22;
	switch( m_pAudioPath->m_dwSampleRate )
	{
	case 11025:
		nID = IDC_11;
		break;
	case 22050:
		// Default
		//nID = IDC_22;
		break;
	case 44100:
		nID = IDC_44;
		break;
	case 48000:
		nID = IDC_48;
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	CheckRadioButton( IDC_11, IDC_48, nID );
	*/
}


void CTabAudioPathAudioPath::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabAudioPathAudioPath)
	DDX_Control(pDX, IDC_NAME, m_editName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabAudioPathAudioPath, CPropertyPage)
	//{{AFX_MSG_MAP(CTabAudioPathAudioPath)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_NAME, OnKillFocusName)
	//ON_BN_CLICKED(IDC_11, On11)
	//ON_BN_CLICKED(IDC_22, On22)
	//ON_BN_CLICKED(IDC_44, On44)
	//ON_BN_CLICKED(IDC_48, On48)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathAudioPath message handlers


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathAudioPath::OnInitDialog

BOOL CTabAudioPathAudioPath::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathAudioPath::OnSetActive

BOOL CTabAudioPathAudioPath::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CAudioPathPropPageManager::sm_nActiveTab );

	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathAudioPath::OnCreate

int CTabAudioPathAudioPath::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CTabAudioPathAudioPath::OnDestroy

void CTabAudioPathAudioPath::OnDestroy() 
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
// CTabAudioPathAudioPath::OnKillFocusName

void CTabAudioPathAudioPath::OnKillFocusName() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pAudioPathComponent != NULL );
	ASSERT( theApp.m_pAudioPathComponent->m_pIFramework != NULL );

	if( m_pAudioPath )
	{
		CString strName;

		m_editName.GetWindowText( strName );

		// Strip leading and trailing spaces
		strName.TrimRight();
		strName.TrimLeft();

		if( strName.IsEmpty() )
		{
			m_editName.SetWindowText( m_pAudioPath->m_strName );
		}
		else
		{
			if( strName.Compare( m_pAudioPath->m_strName ) != 0 )
			{
				BSTR bstrName = strName.AllocSysString();
				m_pAudioPath->SetNodeName( bstrName );
				theApp.m_pAudioPathComponent->m_pIFramework->RefreshNode( m_pAudioPath );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathAudioPath::On11

/*
void CTabAudioPathAudioPath::On11() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPath )
	{
		m_pAudioPath->SetSampleRate( 11025 );
	}
}

void CTabAudioPathAudioPath::On22() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPath )
	{
		m_pAudioPath->SetSampleRate( 22050 );
	}
}

void CTabAudioPathAudioPath::On44() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPath )
	{
		m_pAudioPath->SetSampleRate( 44100 );
	}
}

void CTabAudioPathAudioPath::On48() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPath )
	{
		m_pAudioPath->SetSampleRate( 48000 );
	}
}
*/
