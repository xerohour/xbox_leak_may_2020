// TabSegmentDesign.cpp : implementation file
//

#include "stdafx.h"
#include "SongDesignerDLL.h"

#include "Song.h"
#include "SourceSegment.h"
#include "TabSegmentDesign.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabSegmentDesign property page

CTabSegmentDesign::CTabSegmentDesign( CSegmentPropPageManager* pPageManager ) : CPropertyPage(CTabSegmentDesign::IDD)
{
	//{{AFX_DATA_INIT(CTabSegmentDesign)
	//}}AFX_DATA_INIT

	ASSERT( pPageManager != NULL );

	m_pSourceSegment = NULL;
	m_pPageManager = pPageManager;
	m_fNeedToDetach = FALSE;
}

CTabSegmentDesign::~CTabSegmentDesign()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabSegmentDesign::SetSourceSegment

void CTabSegmentDesign::SetSourceSegment( CSourceSegment* pSourceSegment )
{
	m_pSourceSegment = pSourceSegment;
}


void CTabSegmentDesign::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabSegmentDesign)
	DDX_Control(pDX, IDC_DESIGN_FILENAME, m_editDesignFileName);
	DDX_Control(pDX, IDC_DESIGN_CREATE, m_staticDesignCreate);
	DDX_Control(pDX, IDC_DESIGN_SIZE, m_staticDesignSize);
	DDX_Control(pDX, IDC_DESIGN_MODIFIED, m_staticDesignModified);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CTabSegmentDesign::EnableControls

void CTabSegmentDesign::EnableControls( BOOL fEnable ) 
{
	// May need this method at a later time
}


BEGIN_MESSAGE_MAP(CTabSegmentDesign, CPropertyPage)
	//{{AFX_MSG_MAP(CTabSegmentDesign)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabSegmentDesign message handlers

BOOL CTabSegmentDesign::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSourceSegment == NULL )
	{
		EnableControls( FALSE );
		return CPropertyPage::OnSetActive();
	}

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CSegmentPropPageManager::sm_nActiveTab );

	EnableControls( TRUE );

	HINSTANCE hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( theApp.m_hInstance );

	BSTR bstrFileName;
	CString strFileName;
	CString strFileCreate;
	CString strFileModified;
	CString strFileSize;

	strFileName.LoadString( IDS_UNKNOWN );
	strFileCreate.LoadString( IDS_UNKNOWN );
	strFileModified.LoadString( IDS_UNKNOWN );
	strFileSize.LoadString( IDS_UNKNOWN );

	// Determine design file filename
	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->GetNodeFileName( m_pSourceSegment->m_FileRef.pIDocRootNode, &bstrFileName ) ) )
	{
		CString strTheFileName = bstrFileName;
		::SysFreeString( bstrFileName );

		if( strTheFileName.IsEmpty() == FALSE )
		{
			strFileName = strTheFileName;

			// Set design file modified date and size
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
		}
	}

	m_editDesignFileName.SetWindowText( strFileName );
	m_staticDesignCreate.SetWindowText( strFileCreate );
	m_staticDesignModified.SetWindowText( strFileModified );
	m_staticDesignSize.SetWindowText( strFileSize );

	AfxSetResourceHandle( hInstance );
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabSegmentDesign::OnDestroy

void CTabSegmentDesign::OnDestroy() 
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
// CTabSegmentDesign::OnCreate

int CTabSegmentDesign::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
