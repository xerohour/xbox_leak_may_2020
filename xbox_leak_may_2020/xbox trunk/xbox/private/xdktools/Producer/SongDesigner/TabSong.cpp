// TabSong.cpp : implementation file
//

#include "stdafx.h"
#include "TabSong.h"
#include "SongDesignerDll.h"
#include "Song.h"
#include <dmusicf.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabSong property page

CTabSong::CTabSong( CSongPropPageManager* pSongPropPageManager ) : CPropertyPage(CTabSong::IDD)
{
	//{{AFX_DATA_INIT(CTabSong)
	//}}AFX_DATA_INIT
	
	ASSERT( pSongPropPageManager != NULL );

	m_pSong = NULL;
	m_pPageManager = pSongPropPageManager;
	m_fNeedToDetach = FALSE;
}

CTabSong::~CTabSong()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabSong::SetSong

void CTabSong::SetSong( CDirectMusicSong* pSong )
{
	m_pSong = pSong;

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTabSong::EnableControls

void CTabSong::EnableControls( BOOL fEnable ) 
{
	m_editName.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CTabSong::UpdateControls

void CTabSong::UpdateControls() 
{
	// Make sure controls have been created
	if( ::IsWindow(m_editName.m_hWnd) == FALSE )
	{
		return;
	}
	
	// Update controls
	m_editName.LimitText( DMUS_MAX_NAME );

	if( m_pSong )
	{
		EnableControls( TRUE );

		// Set name
		m_editName.SetWindowText( m_pSong->m_strName );
	}
	else
	{
		m_editName.SetWindowText( _T("") );

		EnableControls( FALSE );
	}
}


void CTabSong::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabSong)
	DDX_Control(pDX, IDC_NAME, m_editName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabSong, CPropertyPage)
	//{{AFX_MSG_MAP(CTabSong)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_NAME, OnKillFocusName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTabSong message handlers


/////////////////////////////////////////////////////////////////////////////
// CTabSong::OnInitDialog

BOOL CTabSong::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabSong::OnSetActive

BOOL CTabSong::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CSongPropPageManager::sm_nActiveTab );

	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabSong::OnCreate

int CTabSong::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CTabSong::OnDestroy

void CTabSong::OnDestroy() 
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
// CTabSong::OnKillFocusName

void CTabSong::OnKillFocusName() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	if( m_pSong )
	{
		CString strName;

		m_editName.GetWindowText( strName );

		// Strip leading and trailing spaces
		strName.TrimRight();
		strName.TrimLeft();

		if( strName.IsEmpty() )
		{
			m_editName.SetWindowText( m_pSong->m_strName );
		}
		else
		{
			if( strName.Compare( m_pSong->m_strName ) != 0 )
			{
				BSTR bstrName = strName.AllocSysString();
				m_pSong->SetNodeName( bstrName );
				theApp.m_pSongComponent->m_pIFramework8->RefreshNode( m_pSong );
			}
		}
	}
}
