// ProjectPropTabGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "ProjectPropTabGeneral.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabGeneral property page

CProjectPropTabGeneral::CProjectPropTabGeneral( CProjectPropPageManager* pPageManager ) : CPropertyPage(CProjectPropTabGeneral::IDD)
{
	//{{AFX_DATA_INIT(CProjectPropTabGeneral)
	//}}AFX_DATA_INIT

	ASSERT( pPageManager != NULL );

	m_pProject = NULL;
	m_pPageManager = pPageManager;
}

CProjectPropTabGeneral::~CProjectPropTabGeneral()
{
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabGeneral::SetProject

void CProjectPropTabGeneral::SetProject( CProject* pProject )
{
	m_pProject = pProject;
}


void CProjectPropTabGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectPropTabGeneral)
	DDX_Control(pDX, IDC_LAST_MODIFIED, m_staticLastModified);
	DDX_Control(pDX, IDC_LAST_MODIFIED_BY, m_staticLastModifiedBy);
	DDX_Control(pDX, IDC_PROJECT_FILENAME, m_staticProjectFileName);
	DDX_Control(pDX, IDC_DESCRIPTION, m_editDescription);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabGeneral::EnableControls

void CProjectPropTabGeneral::EnableControls( BOOL fEnable ) 
{
	m_editDescription.EnableWindow( fEnable );
}


BEGIN_MESSAGE_MAP(CProjectPropTabGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CProjectPropTabGeneral)
	ON_EN_CHANGE(IDC_DESCRIPTION, OnChangeDescription)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabGeneral message handlers

/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabGeneral::OnSetActive

BOOL CProjectPropTabGeneral::OnSetActive() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( m_pProject == NULL )
	{
		EnableControls( FALSE );
		return CPropertyPage::OnSetActive();
	}

	// Store active tab
	IDMUSProdPropSheet* pIPropSheet = theApp.GetPropertySheet();
	if( pIPropSheet )
	{
		pIPropSheet->GetActivePage( &CProjectPropPageManager::sm_nActiveTab );
		pIPropSheet->Release();
	}

	EnableControls( TRUE );

	CString strText;
	CString strLastModified;

	if( m_pProject->m_pProjectDoc )
	{
		strText = m_pProject->m_pProjectDoc->GetPathName();
		if( strText.IsEmpty() )
		{
			strText.LoadString( IDS_NOT_SAVED );
		}
		else
		{
			// Actual Last Modified Date
			HANDLE hFile = ::CreateFile( strText, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
	 									 FILE_ATTRIBUTE_NORMAL, NULL );
			if( hFile != INVALID_HANDLE_VALUE )
			{
				FILETIME ftModified;

				if( ::GetFileTime(hFile, NULL, NULL, &ftModified) )
				{
					CTime timeFile( ftModified );
					strLastModified = timeFile.Format( "%A, %B %d, %Y %I:%M:%S %p" );
				}
				CloseHandle( hFile );
			}

		}

		m_staticProjectFileName.SetWindowText( strText );
		m_staticLastModified.SetWindowText( strLastModified );

		if( m_pProject->m_strLastModifiedBy.IsEmpty() )
		{
			CString strUnknown;

			strUnknown.LoadString( IDS_UNKNOWN );
			m_staticLastModifiedBy.SetWindowText( strUnknown );
		}
		else
		{
			m_staticLastModifiedBy.SetWindowText( m_pProject->m_strLastModifiedBy );
		}

		m_editDescription.SetWindowText( m_pProject->m_strDescription );
	}
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabGeneral::OnChangeDescription

void CProjectPropTabGeneral::OnChangeDescription() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	m_editDescription.GetWindowText( m_pProject->m_strDescription );

	// Strip leading and trailing spaces
	m_pProject->m_strDescription.TrimRight();
	m_pProject->m_strDescription.TrimLeft();
}
