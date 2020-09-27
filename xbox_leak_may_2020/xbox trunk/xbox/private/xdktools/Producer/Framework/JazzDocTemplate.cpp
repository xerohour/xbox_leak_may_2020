// JazzDocTemplate.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJazzDocTemplate

IMPLEMENT_DYNAMIC(CJazzDocTemplate, CMultiDocTemplate)


/////////////////////////////////////////////////////////////////////////////
// CJazzDocTemplate::CJazzDocTemplate

CJazzDocTemplate::CJazzDocTemplate( IDMUSProdDocType* pIDocType,
								    HINSTANCE hInstance,
								    UINT nIDResource,
								    CRuntimeClass* pDocClass,
								    CRuntimeClass* pFrameClass,
									CRuntimeClass* pViewClass )
			   : CMultiDocTemplate( nIDResource,
									pDocClass,
									pFrameClass,
									pViewClass )
{
	m_pIDocType = pIDocType;
//	m_pIDocType->AddRef();		intentionally missing

	m_hInstance = hInstance;

	// Determine the tree image index for this DocType
	m_nTreeImageIndex = -1;
	if( theApp.m_pFramework )
	{
		CImageList lstImages;
		lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
		lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );
		HICON hIcon = ::LoadIcon( m_hInstance, MAKEINTRESOURCE(nIDResource) );
		lstImages.Add( hIcon );
		::DestroyIcon( hIcon );
		hIcon = ::LoadIcon( m_hInstance, MAKEINTRESOURCE(nIDResource) );
		lstImages.Add( hIcon );
		::DestroyIcon( hIcon );
		if( FAILED (theApp.m_pFramework->AddNodeImageList( lstImages.Detach(), &m_nTreeImageIndex ) ) )
		{
			m_nTreeImageIndex = -1;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDocTemplate::~CJazzDocTemplate

CJazzDocTemplate::~CJazzDocTemplate()
{
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDocTemplate::GetInstance

HINSTANCE CJazzDocTemplate::GetInstance()
{
	return m_hInstance;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDocTemplate::GetTreeImageIndex

short CJazzDocTemplate::GetTreeImageIndex()
{
	return m_nTreeImageIndex;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDocTemplate::LoadTemplate

void CJazzDocTemplate::LoadTemplate()
{
	CMultiDocTemplate::LoadTemplate();

	if( m_hMenuShared == NULL )	// Use Jazz menu if none supplied
	{
		m_hMenuShared = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDR_JAZZTYPE) );
	}

//	if( m_hAccelTable == NULL )	// Use Jazz accel if none supplied
//	{
//		m_hAccelTable = ::LoadAccelerators( theApp.m_hInstance, MAKEINTRESOURCE(IDR_JAZZTYPE) );
//	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDocTemplate::IsEqualDocType

BOOL CJazzDocTemplate::IsEqualDocType( IDMUSProdDocType* pIDocType )
{
	if( pIDocType
	&& (pIDocType == m_pIDocType) )
	{
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDocTemplate::IsProjectDocType

BOOL CJazzDocTemplate::IsProjectDocType( void )
{
	if( m_pIDocType == NULL )
	{
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDocTemplate::GetDocString

BOOL CJazzDocTemplate::GetDocString( CString& rString, enum DocStringIndex i ) const
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( m_hInstance );

	CString strTemp;
	BOOL fReturn = AfxExtractSubString( strTemp, m_strDocStrings, (int)i );

	if( i == CDocTemplate::filterExt )
	{
		if( fReturn == TRUE )
		{
			CString strLeft;
			CString strRight;
			int nFindPos;

			strRight = strTemp;

			nFindPos = strRight.Find( _T(";") );
			while( nFindPos != -1 )
			{
				strLeft = strLeft + strRight.Left( nFindPos + 1 );
				strRight = _T("*") + strRight.Right( strRight.GetLength() - nFindPos - 1 ); 
				strTemp = strLeft + strRight;

				nFindPos = strRight.Find( _T(";") );
			}
		}
	}

	rString = strTemp;

	AfxSetResourceHandle( hInstance );

	return fReturn;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDocTemplate::OpenDocumentFile

CDocument* CJazzDocTemplate::OpenDocumentFile( LPCTSTR szPathName, BOOL bMakeVisible )
{
	if( IsProjectDocType() == FALSE )
	{
		if( theApp.m_nFileTarget != TGT_PROJECT )
		{
			IDMUSProdProject* pIProject;
			
			if( FAILED ( theApp.GetFirstProject( &pIProject ) ) )
			{
				pIProject = NULL;
			}

			if( pIProject )
			{
				pIProject->Release();
			}
			else
			{
				if( !theApp.CreateNewProject() )
				{
					return NULL;
				}
			}
		}
	}

	HINSTANCE hInstance;
	CDocument* pDocument;

	hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( m_hInstance );

	// don't let MFC open the window
	bMakeVisible = FALSE;

	theApp.m_fSendFileNameChangeNotification = FALSE;
	pDocument = CMultiDocTemplate::OpenDocumentFile( szPathName, bMakeVisible );
	theApp.m_fSendFileNameChangeNotification = TRUE;

	// and flag this "hidden" view as the seed view
	// and keep it around to prevent the document from
	// being deleted for zero views
	if( pDocument
	&&  pDocument->IsKindOf( RUNTIME_CLASS(CComponentDoc) ) )
	{
		CComponentDoc *pDoc;
		CComponentView* pView;

		pDoc = (CComponentDoc *)pDocument;

		POSITION pos = pDoc->GetFirstViewPosition();
		while( pos )
		{
			pView = (CComponentView*)pDoc->GetNextView( pos );
			ASSERT_VALID( pView );

			pView->m_fSeed = TRUE;
			if( pView->m_pINode )
			{
				HWND hWndEditor;
				if( SUCCEEDED ( pView->m_pINode->GetEditorWindow(&hWndEditor) ) )
				{
					if( hWndEditor )
					{
						pView->m_fSeed = FALSE;
					}
				}
			}
		}
	}

	AfxSetResourceHandle( hInstance );

	return pDocument;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDocTemplate::MatchDocType

CDocTemplate::Confidence CJazzDocTemplate::MatchDocType( LPCTSTR lpszPathName, CDocument*& rpDocMatch )
{
	ASSERT( lpszPathName != NULL );
	rpDocMatch = NULL;

	// go through all documents
	POSITION pos = GetFirstDocPosition();
	while( pos != NULL )
	{
		CDocument* pDoc = GetNextDoc( pos );
		if( AfxComparePath(pDoc->GetPathName(), lpszPathName) )
		{
			// already open
			rpDocMatch = pDoc;
			return yesAlreadyOpen;
		}
	}

	// see if it matches a suffix
	CString strFilterExt;
	if( GetDocString(strFilterExt, CDocTemplate::filterExt)
	&& !strFilterExt.IsEmpty() )
	{
		// see if extension matches
		ASSERT( strFilterExt[0] == '.' );

		LPCTSTR lpszFileExt = _tcsrchr( lpszPathName, '.' );
		if( lpszFileExt )
		{
			BOOL fContinue = TRUE;
			CString strExt;
			int nFindPos;

			nFindPos = strFilterExt.Find( _T("*") );
			while( fContinue )
			{
				if( nFindPos == -1 )
				{
					fContinue = FALSE;

					nFindPos = strFilterExt.Find( _T(".") );
					if( nFindPos != 0 )
					{
						break;
					}
					strExt = strFilterExt;
				}
				else
				{
					strExt = strFilterExt.Left( nFindPos - 1 );
					strFilterExt = strFilterExt.Right( strFilterExt.GetLength() - (nFindPos + 1) ); 
				}

				if( _tcsicmp(lpszFileExt, strExt) == 0 )
				{
					// extension matches, looks like ours
					return yesAttemptNative; 
				}

				nFindPos = strFilterExt.Find( _T("*") );
			}
		}
	}

	// unknown document type
	return yesAttemptForeign;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDocTemplate Message Map

BEGIN_MESSAGE_MAP(CJazzDocTemplate, CMultiDocTemplate)
	//{{AFX_MSG_MAP(CJazzDocTemplate)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
