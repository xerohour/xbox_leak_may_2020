// TabObjectRuntime.cpp : implementation file
//

#include "stdafx.h"
#include "ContainerDesignerDLL.h"

#include "Container.h"
#include "Object.h"
#include "TabObjectRuntime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabObjectRuntime property page

CTabObjectRuntime::CTabObjectRuntime( CObjectPropPageManager* pPageManager ) : CPropertyPage(CTabObjectRuntime::IDD)
{
	//{{AFX_DATA_INIT(CTabObjectRuntime)
	//}}AFX_DATA_INIT

	ASSERT( pPageManager != NULL );

	m_pContainerObject = NULL;
	m_pPageManager = pPageManager;
	m_fNeedToDetach = FALSE;
}

CTabObjectRuntime::~CTabObjectRuntime()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabObjectRuntime::SetContainerObject

void CTabObjectRuntime::SetContainerObject( CContainerObject* pContainerObject )
{
	m_pContainerObject = pContainerObject;
}


void CTabObjectRuntime::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabObjectRuntime)
	DDX_Control(pDX, IDC_RUNTIME_FILE, m_editRuntimeFile);
	DDX_Control(pDX, IDC_RUNTIME_CREATE, m_staticRuntimeCreate);
	DDX_Control(pDX, IDC_RUNTIME_SIZE, m_staticRuntimeSize);
	DDX_Control(pDX, IDC_RUNTIME_MODIFIED, m_staticRuntimeModified);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CTabObjectRuntime::EnableControls

void CTabObjectRuntime::EnableControls( BOOL fEnable ) 
{
	// May need this routine later
}


BEGIN_MESSAGE_MAP(CTabObjectRuntime, CPropertyPage)
	//{{AFX_MSG_MAP(CTabObjectRuntime)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabObjectRuntime message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabObjectRuntime::OnSetActive

BOOL CTabObjectRuntime::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pContainerObject == NULL )
	{
		EnableControls( FALSE );
		return CPropertyPage::OnSetActive();
	}

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CObjectPropPageManager::sm_nActiveTab );

	EnableControls( TRUE );

	HINSTANCE hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( theApp.m_hInstance );

	// Set runtime file name, modified date, size
	BSTR bstrFile;
	CString strFile;
	CString strFileCreate;
	CString strFileModified;
	CString strFileSize;

	strFile.LoadString( IDS_NOT_SAVED );
	strFileCreate.LoadString( IDS_UNKNOWN );
	strFileModified.LoadString( IDS_UNKNOWN );
	strFileSize.LoadString( IDS_UNKNOWN );

	// Determine filename of last saved runtime file
	if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->GetNodeRuntimeFileName( m_pContainerObject->m_FileRef.pIDocRootNode, &bstrFile ) ) )
	{
		CString strTheFile = bstrFile;
		::SysFreeString( bstrFile );

		if( strTheFile.IsEmpty() == FALSE )
		{
			strFile = strTheFile;

			HANDLE hFile = ::CreateFile( strFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
	 									 FILE_ATTRIBUTE_NORMAL, NULL );
			if( hFile != INVALID_HANDLE_VALUE )
			{
				FILETIME ftCreate;
				FILETIME ftModified;
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

	m_editRuntimeFile.SetWindowText( strFile );
	m_staticRuntimeCreate.SetWindowText( strFileCreate );
	m_staticRuntimeModified.SetWindowText( strFileModified );
	m_staticRuntimeSize.SetWindowText( strFileSize );
	
	AfxSetResourceHandle( hInstance );
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabObjectRuntime::OnDestroy

void CTabObjectRuntime::OnDestroy() 
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
// CTabObjectRuntime::OnCreate

int CTabObjectRuntime::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
