// TabFileDesign.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "TabFileDesign.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabFileDesign property page

CTabFileDesign::CTabFileDesign( CFilePropPageManager* pPageManager ) : CPropertyPage(CTabFileDesign::IDD)
{
	//{{AFX_DATA_INIT(CTabFileDesign)
	//}}AFX_DATA_INIT

	ASSERT( pPageManager != NULL );

	m_pFileNode = NULL;
	m_pPageManager = pPageManager;
}

CTabFileDesign::~CTabFileDesign()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileDesign::SetFile

void CTabFileDesign::SetFile( CFileNode* pFileNode )
{
	m_pFileNode = pFileNode;
}


void CTabFileDesign::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabFileDesign)
	DDX_Control(pDX, IDC_DESIGN_FILENAME, m_editDesignFileName);
	DDX_Control(pDX, IDC_DESIGN_CREATE, m_staticDesignCreate);
	DDX_Control(pDX, IDC_DESIGN_SIZE, m_staticDesignSize);
	DDX_Control(pDX, IDC_DESIGN_MODIFIED, m_staticDesignModified);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileDesign::EnableControls

void CTabFileDesign::EnableControls( BOOL fEnable ) 
{
}


BEGIN_MESSAGE_MAP(CTabFileDesign, CPropertyPage)
	//{{AFX_MSG_MAP(CTabFileDesign)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabFileDesign message handlers

BOOL CTabFileDesign::OnSetActive() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( m_pFileNode == NULL )
	{
		EnableControls( FALSE );
		return CPropertyPage::OnSetActive();
	}

	// Store active tab
	IDMUSProdPropSheet* pIPropSheet = theApp.GetPropertySheet();
	if( pIPropSheet )
	{
		pIPropSheet->GetActivePage( &CFilePropPageManager::sm_nActiveTab );
		pIPropSheet->Release();
	}

	EnableControls( TRUE );

	HINSTANCE hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( theApp.m_hInstance );

	CString strFileName;
	CString strFileCreate;
	CString strFileModified;
	CString strFileSize;

	// Set design file filename
	m_pFileNode->ConstructFileName( strFileName );
	m_editDesignFileName.SetWindowText( strFileName );

	// Set design file modified date and size
	strFileCreate.Empty();
	strFileModified.Empty();
	strFileSize.Empty();
	HANDLE hFile = ::CreateFile( strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
	 							 FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile != INVALID_HANDLE_VALUE )
	{
		FILETIME ftModified;
		FILETIME ftCreate;
		DWORD dwFileSize;

		if( ::GetFileTime(hFile, &ftCreate, NULL, &ftModified) )
		{
			CTime timeFile;
			
			timeFile = ftCreate;
			strFileCreate = timeFile.Format( "%A, %B %d, %Y   %I:%M:%S %p" );

			timeFile = ftModified;
			strFileModified = timeFile.Format( "%A, %B %d, %Y   %I:%M:%S %p" );
		}

		dwFileSize = GetFileSize( hFile, NULL );

		CString strSize;
		CString strBytes;
		strSize.Format( "%u", dwFileSize );

		int i, j;
		int nLength = strSize.GetLength();

		for( i=0, j=nLength;  i < nLength ; i++ )
		{
			strBytes += strSize[i];
			j--;
			if( (j > 0)
			&& !(j % 3) )
			{
				strBytes += ',';
			}
		}
		
		if( dwFileSize < 1024 )
		{
			AfxFormatString1( strFileSize, IDS_SIZE_BYTES, strBytes );
		}
		else
		{
			CString strKB;
			
			double dblKB = dwFileSize / 1024.0;
			strKB.Format( "%.2f", dblKB );

			AfxFormatString2( strFileSize, IDS_SIZE_KB, strKB, strBytes );
		}

		CloseHandle( hFile );
	}
	else
	{
		strFileCreate.LoadString( IDS_UNKNOWN );
		strFileModified.LoadString( IDS_UNKNOWN );
		strFileSize.LoadString( IDS_UNKNOWN );
	}

	m_staticDesignCreate.SetWindowText( strFileCreate );
	m_staticDesignModified.SetWindowText( strFileModified );
	m_staticDesignSize.SetWindowText( strFileSize );

	AfxSetResourceHandle( hInstance );
	
	return CPropertyPage::OnSetActive();
}
