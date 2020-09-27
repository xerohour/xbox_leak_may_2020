// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "JazzApp.h"

#include "MainFrm.h"
#include "ComponentCntrItem.h"
#include "Splash.h"

#include "ProjectPropTabGeneral.h"
#include "ProjectPropTabFolders.h"
#include <initguid.h>
#include <PrivateTransport.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CJzWaitCursor Class 

CJzWaitCursor::CJzWaitCursor( CFrameWnd* pMainFrame, CWnd* pWndCapture )
{
	ASSERT( pMainFrame != NULL );
	ASSERT( pWndCapture != NULL );

    m_dwRef = 0;
	m_pMainFrame = pMainFrame;
	m_pWndCapture = pWndCapture;
}

CJzWaitCursor::~CJzWaitCursor()
{
}

ULONG CJzWaitCursor::StartWait()
{
	if( m_dwRef == 0 )
	{
		m_pWndCapture->SetCapture();
		m_pMainFrame->BeginWaitCursor();
	}

	return ++m_dwRef;
}

ULONG CJzWaitCursor::EndWait()
{
    ASSERT( m_dwRef != 0 );

    --m_dwRef;

    if( m_dwRef == 0 )
    {
		MSG msg;

		// eat all mouse messages in our queue
		while( ::PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST,
				PM_REMOVE|PM_NOYIELD) );

		::ReleaseCapture();
		m_pMainFrame->EndWaitCursor();
        delete this;
        return 0;
    }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CJzStatusBarPane Class 

CJzStatusBarPane::CJzStatusBarPane( HANDLE hKey, short nLifeSpan, short nIndex )
{
    m_hKey = hKey;
	m_nLifeSpan = nLifeSpan;
	m_nStatusBarIndex = 0;
	m_nIndex = nIndex;
    m_nWidth = 0;
	m_nStyle = SBPS_NOBORDERS;
}

CJzStatusBarPane::~CJzStatusBarPane()
{
}


/////////////////////////////////////////////////////////////////////////////
// CMyMDIClient Class 

CMyMDIClient::CMyMDIClient()
{
	m_pbmpWallpaper = NULL;
}

CMyMDIClient::~CMyMDIClient()
{
	if( m_pbmpWallpaper )
	{
		m_pbmpWallpaper->DeleteObject();
		delete m_pbmpWallpaper;
		m_pbmpWallpaper = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMyMDIClient::LoadWallpaper

void CMyMDIClient::LoadWallpaper( void )
{
	// Cleanup previous bitmap
	if( m_pbmpWallpaper )
	{
		m_pbmpWallpaper->DeleteObject();
		delete m_pbmpWallpaper;
		m_pbmpWallpaper = NULL;
	}

	// Load new bitmap
	CString strWallpaper = theApp.GetProfileString( _T("Custom"), _T("Wallpaper"), _T("") );
	if( strWallpaper.IsEmpty() == FALSE )
	{
		TCHAR achDrive[_MAX_DRIVE];
		TCHAR achDir[_MAX_DIR];
		TCHAR achFNameWallpaper[_MAX_FNAME];
		TCHAR achExtWallpaper[_MAX_EXT];

		_tsplitpath( strWallpaper, achDrive, achDir, achFNameWallpaper, achExtWallpaper );

		if( achDrive[0] == (TCHAR)'\0' )
		{
			// Not a full path so look in same directory as DMUSProd.exe
			TCHAR achFileName[FILENAME_MAX];

			if( GetModuleFileName(NULL, achFileName, FILENAME_MAX) > 0 )
			{
				_tsplitpath( achFileName, achDrive, achDir, NULL, NULL );
				_tmakepath( achFileName, achDrive, achDir, achFNameWallpaper, achExtWallpaper );

				strWallpaper = achFileName;
			}
		}
		
		HGDIOBJ hBitmap = ::LoadImage( NULL, strWallpaper, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
		if( hBitmap )
		{
			m_pbmpWallpaper = new CBitmap;
			if( m_pbmpWallpaper )
			{
				m_pbmpWallpaper->Attach( hBitmap );
			}
		}
	}
}


BEGIN_MESSAGE_MAP(CMyMDIClient, CWnd)
	//{{AFX_MSG_MAP(CMyMDIClient)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMyMDIClient::OnEraseBkgnd

BOOL CMyMDIClient::OnEraseBkgnd( CDC* pDC ) 
{
	if( m_pbmpWallpaper )
	{
		// Create memory DC	
		CDC dcMem;
		if( dcMem.CreateCompatibleDC( pDC ) )
		{
			// Get clipbox rectangle
			CRect rect;
			pDC->GetClipBox( &rect );

			// Get bitmap info
			BITMAP bm;
			m_pbmpWallpaper->GetBitmap( &bm );

			// Init fields used to tile bitmap
			int nStartCol = rect.left / bm.bmWidth;
			int nEndCol = (rect.right + bm.bmWidth + 1) / bm.bmWidth;
			int nStartRow = rect.top / bm.bmHeight;
			int nEndRow = (rect.bottom + bm.bmHeight + 1) / bm.bmHeight;

			// Tile the bitmap
			CBitmap* pbmpOld = dcMem.SelectObject( m_pbmpWallpaper );
			for( int i = nStartCol ;  i < nEndCol ;  i++ )
			{
				for( int j = nStartRow ;  j < nEndRow ;  j++ )
				{
					int x = i * bm.bmWidth;
					int y = j * bm.bmHeight;
					pDC->BitBlt( x, y, bm.bmWidth, bm.bmHeight, &dcMem, 0, 0, SRCCOPY );
				}
			}
			dcMem.SelectObject( pbmpOld );
			return FALSE;
		}
	}
	
	return CWnd::OnEraseBkgnd( pDC );
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_PROJECTTREE, OnViewProjectTree)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROJECTTREE, OnUpdateViewProjectTree)
	ON_WM_CLOSE()
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_VIEW_STATUS_BAR, OnViewStatusBar)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	ON_COMMAND(ID_VIEW_PROPERTIES, OnViewProperties)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIES, OnUpdateViewProperties)
	ON_UPDATE_COMMAND_UI(IDM_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(IDM_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(IDM_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(IDM_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(IDM_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(IDM_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(IDM_EDIT_DELETE, OnUpdateEditDelete)
	ON_COMMAND(IDM_EDIT_DELETE, OnEditDelete)
	ON_WM_DROPFILES()
	ON_WM_QUERYENDSESSION()
	ON_WM_ENDSESSION()
	ON_COMMAND(ID_VIEW_TOGGLE, OnViewToggle)
	ON_WM_ACTIVATEAPP()
	ON_COMMAND(ID_VIEW_BOOKMARKS, OnViewBookmarks)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BOOKMARKS, OnUpdateViewBookmarks)
	ON_COMMAND(ID_TRANS_PLAY, OnTransPlay)
	ON_COMMAND(ID_TRANS_PLAY_FROM_START, OnTransPlayFromStart)
	ON_COMMAND(ID_TRANS_RECORD, OnTransRecord)
	ON_WM_DESTROY()
	ON_COMMAND(ID_TRANS_TRANSITION, OnTransTransition)
	ON_COMMAND(ID_TRANS_SPACE, OnTransSpace)
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, OnHelp)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, OnHelp)
	ON_COMMAND(ID_DEFAULT_HELP, OnHelp)
	ON_MESSAGE(WM_APP, OnApp)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

/////////////////////////////////////////////////////////////////////////////
// CMainFrame::CMainFrame

CMainFrame::CMainFrame()
{
	m_pWaitCursor = NULL;
	m_pProgressCtrl = NULL;
	m_hProgressCtrl = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::~CMainFrame

CMainFrame::~CMainFrame()
{
	CJzStatusBarPane* pJzStatusBarPane;

	while( !m_lstStatusBarPanes.IsEmpty() )
	{
		pJzStatusBarPane = static_cast<CJzStatusBarPane*>( m_lstStatusBarPanes.RemoveHead() );
		delete pJzStatusBarPane;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnDropFiles

void CMainFrame::OnDropFiles( HDROP hDropInfo ) 
{
	theApp.m_nFileTarget = TGT_SELECTEDNODE;

	// Activate us first!
	SetActiveWindow();   

	// Get number of filenames
	UINT nFiles = ::DragQueryFile( hDropInfo, (UINT)-1, NULL, 0 );

	CJzFileName* pJzFileName;
	IDMUSProdDocType* pIDocType;
		
	// Get the filenames
	for( UINT iFile = 0; iFile < nFiles; iFile++ )
	{
		TCHAR achFileName[_MAX_PATH];
		::DragQueryFile( hDropInfo, iFile, achFileName, _MAX_PATH );

		pIDocType = theApp.GetDocType( achFileName );
		if( pIDocType
		|| 	theApp.IsValidProjectFile( achFileName, FALSE ) )
		{
			pJzFileName = new CJzFileName( achFileName );
			if( pJzFileName )
			{
				theApp.m_lstFileNamesToOpen.AddTail( pJzFileName );
			}
			else
			{
				theApp.OpenDocumentFile( achFileName );
			}

			if( pIDocType )
			{
				pIDocType->Release();
			}
		}
		else
		{
			// Non-Producer file so copy it into the Project
			theApp.InsertForeignFile( achFileName );
		}
	}
	::DragFinish( hDropInfo );

	// Open the files
	POSITION pos;
	POSITION posRemove;
	short nPriority = 1;

	while( theApp.m_lstFileNamesToOpen.IsEmpty() == FALSE )
	{
		pos = theApp.m_lstFileNamesToOpen.GetHeadPosition();
		while( pos )
		{
			posRemove = pos;
			pJzFileName = theApp.m_lstFileNamesToOpen.GetNext( pos );

			if( pJzFileName->m_nPriority == nPriority
			||  pJzFileName->m_nPriority < 1
			||  pJzFileName->m_nPriority > MAX_FILE_OPEN_PRIORITY )
			{
				if( pJzFileName->m_fBeingLoaded == false )
				{
					pJzFileName->m_fBeingLoaded = true;
					theApp.OpenDocumentFile( pJzFileName->m_strFileName );
				}

				theApp.m_lstFileNamesToOpen.RemoveAt( posRemove );
				delete pJzFileName;
			}
		}

		nPriority++;
	}
	
	theApp.m_nFileTarget = TGT_PROJECT;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::DropFiles

void CMainFrame::DropFiles( HDROP hDropInfo ) 
{
	OnDropFiles( hDropInfo );
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::GetProjectTreeCtrl

CTreeCtrl* CMainFrame::GetProjectTreeCtrl( void )
{
	CTreeCtrl* pTreeCtrl = (CTreeCtrl *)m_wndTreeBar.GetDlgItem( IDC_TREE );

	return pTreeCtrl;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::EndTrack

void CMainFrame::EndTrack( int nWidth )
{
	m_wndTreeBar.m_sizeFloating.cx = nWidth;
	m_wndTreeBar.m_sizeDocked.cx = nWidth;

	RecalcLayout( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnCreate

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( CMDIFrameWnd::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}

	if( !CreateStatusBar() )
	{
		return -1;
	}

	EnableDocking( CBRS_ALIGN_ANY );

	if( !CreateToolBar() 
	||  !CreateBookmarkBar() 
	||  !CreateTreeBar()
	||  !CreatePropertiesBar() )
	{
		return -1;
	}

	// Subclass MDIClient window
	m_wndMDIClient.SubclassWindow( m_hWndMDIClient );
	m_wndMDIClient.LoadWallpaper();

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::PreCreateWindow

BOOL CMainFrame::PreCreateWindow( CREATESTRUCT& cs )
{
    // Use the specific class name we established earlier
	cs.lpszClass = lpszUniqueClass;

	return CMDIFrameWnd::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::CreateToolBar

BOOL CMainFrame::CreateToolBar( void )
{
	CString strTitle;

	if( !m_wndToolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP |
			CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) )
	{
		TRACE0( "Failed to create toolbar\n" );
		return FALSE;      // fail to create
	}
	
	if( !m_wndToolBar.LoadToolBar(IDR_MAINFRAME) )
	{
		TRACE0( "Failed to create toolbar\n" );
		return FALSE;      // fail to create
	}

	m_wndToolBar.ModifyStyle( 0, (WS_CLIPCHILDREN | TBSTYLE_FLAT), 0 );
	m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() ^ CBRS_HIDE_INPLACE );

	if( strTitle.LoadString(IDS_TBAR_MAIN) )
	{
		m_wndToolBar.SetWindowText( strTitle );
	}

	m_wndToolBar.EnableDocking( CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM );
	DockControlBar( &m_wndToolBar );

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::CreateBookmarkBar

BOOL CMainFrame::CreateBookmarkBar( void )
{
	CString strTitle;

	if( !m_wndBookmarkToolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP |
			CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_BOOKMARK_TOOLBAR) )
	{
		TRACE0( "Failed to create bookmark toolbar\n" );
		return FALSE;      // fail to create
	}

	m_wndBookmarkToolBar.ModifyStyle( 0, (WS_CLIPCHILDREN | TBSTYLE_FLAT), 0 );
	m_wndBookmarkToolBar.SetBarStyle( m_wndBookmarkToolBar.GetBarStyle() ^ CBRS_HIDE_INPLACE );

	if( strTitle.LoadString(IDS_TBAR_BOOKMARK) )
	{
		m_wndBookmarkToolBar.SetWindowText( strTitle );
	}

	m_wndBookmarkToolBar.EnableDocking( CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM );
	DockControlBar( &m_wndBookmarkToolBar );

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::CreateStatusBar

BOOL CMainFrame::CreateStatusBar( void )
{
	if( !m_wndStatusBar.Create(this)
	||  !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)) )
	{
		TRACE0( "Failed to create status bar\n" );
		return FALSE;      // fail to create
	}
	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::CreateTreeBar

BOOL CMainFrame::CreateTreeBar( void )
{
	CString strTitle;

	if( !m_wndTreeBar.Create(this, IDD_TREEBAR, CBRS_LEFT, IDD_TREEBAR) )
	{
		TRACE0( "Failed to create Project Tree toolbar\n" );
		return FALSE;      // fail to create
	}

	m_wndTreeBar.ModifyStyle( 0, WS_CLIPCHILDREN, 0 );
	m_wndTreeBar.SetBarStyle( m_wndTreeBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );

	if( strTitle.LoadString(IDS_TBAR_PROJECT_TREE) )
	{
		m_wndTreeBar.SetWindowText( strTitle );
	}

	m_wndTreeBar.EnableDocking( CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT );
	DockControlBar( &m_wndTreeBar );

	return TRUE;
}


// Extended dialog template (from atlhost.h)
#pragma pack(push, 1)
struct DLGTEMPLATEEX
{
	WORD dlgVer;
	WORD signature;
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	WORD cDlgItems;
	short x;
	short y;
	short cx;
	short cy;

	// Everything else in this structure is variable length,
	// and therefore must be determined dynamically

	// sz_Or_Ord menu;			// name or ordinal of a menu resource
	// sz_Or_Ord windowClass;	// name or ordinal of a window class
	// WCHAR title[titleLen];	// title string of the dialog box
	// short pointsize;			// only if DS_SETFONT is set
	// short weight;			// only if DS_SETFONT is set
	// short bItalic;			// only if DS_SETFONT is set
	// WCHAR font[fontLen];		// typeface name, if DS_SETFONT is set
};
#pragma pack(pop)

//*********************************************************************
// GetPropSheetFont
//*********************************************************************

#define IDD_PROPSHEET	1006

static HFONT GetPropSheetFont()
{
	// Create a LOGFONTA structure
    LOGFONTA logfont;

	// Initialize it
    memset(&logfont, 0, sizeof(LOGFONTA));
    logfont.lfHeight = 8;
    logfont.lfWeight = FW_NORMAL;
    logfont.lfCharSet = DEFAULT_CHARSET;
    strcpy(logfont.lfFaceName, "MS Sans Serif");

	// Try and load ComCtrl32.dll
    HINSTANCE hInst = LoadLibraryA("COMCTL32");
    if (hInst != NULL)
	{
		// Find the standard property sheet template
		HRSRC hResource = ::FindResource(hInst, MAKEINTRESOURCE(IDD_PROPSHEET), RT_DIALOG);

		// Load the standard property sheet template
		HGLOBAL hTemplate = LoadResource(hInst, hResource);
		DLGTEMPLATE* pTemplate = (DLGTEMPLATE*)LockResource(hTemplate);
		if (pTemplate != NULL)
		{
			// Check if this is an extended dialogm template
			bool bDialogEx = ((pTemplate->style & 0xFFFF0000) == 0xFFFF0000);

			// A pointer used to iterate through the dialog template
			WORD* pw;

			// Skip over the dialog template
			if (bDialogEx)
			{
				pw = (WORD*)((DLGTEMPLATEEX*)pTemplate + 1);
			}
			else
			{
				pw = (WORD*)(pTemplate + 1);
			}

			if (*pw == (WORD)-1)		// Skip menu name string or ordinal
				pw += 2; // WORDs
			else
				while(*pw++);
				
			if (*pw == (WORD)-1)		// Skip class name string or ordinal
				pw += 2; // WORDs
			else
				while(*pw++);

			while (*pw++);			// Skip caption string

			// Get the default device context
			HDC hdc = ::GetDC(NULL);

			// Convert *pw into 1/72ths of an inch
			logfont.lfHeight = -MulDiv(*pw, GetDeviceCaps(hdc, LOGPIXELSY), 72);

			// Release the default device context
			::ReleaseDC(NULL, hdc);

			pw += (bDialogEx ? 3 : 1);

			// Convert the font name from a WCHAR string to an ASCII string
			WideCharToMultiByte(CP_ACP, 0, pw, -1, logfont.lfFaceName, sizeof(logfont.lfFaceName), NULL, NULL);
		}

		// Release our lock on the ComCtl32.dll library
		FreeLibrary(hInst);
    }

	// Try and create the font
    return CreateFontIndirectA(&logfont);
}

// This function converts a size (in pixels) to dialog units using the current
// system font, then converts back to pixels using the MS Sans Serif font.
// This ensures that our property pages display with the correct size on non-english OSs.
void ConvertSystemSizeToSansSerifSize(SIZE& size)
{
	// Variables to store the size of characters in the system font
	UINT cxSysChar = 0;
	UINT cySysChar = 0;

	// Get the default device context
	HDC hdc = ::GetDC(NULL);

	// Get the property sheet font
	HFONT hfontSys;
	if ((hfontSys = GetPropSheetFont()) != NULL)
	{
		// Select the property sheet font
		HFONT hfontOld = (HFONT)SelectObject(hdc, hfontSys);

		// Get the metrics for this font
		TEXTMETRIC tm;
		GetTextMetrics(hdc, &tm);

		// Get the size of the entire alphabet, both upper and lower cases
		SIZE sizeText;
		::GetTextExtentPointA(hdc, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", 52, &sizeText);

		// Compute the average width of a character (rounding to the nearest value)
		cxSysChar = (sizeText.cx + 26) / 52;

		// compute the height of the characters, including the white space between rows
		cySysChar = tm.tmHeight + tm.tmExternalLeading;

		// Select the old font
		SelectObject(hdc, hfontOld);

		// Delete the property sheet font
		DeleteObject((HGDIOBJ)hfontSys);
	}
	else
	{
		// Could not create the font so just use the system's values
		LONG l = GetDialogBaseUnits();
		cxSysChar = LOWORD(l);
		cySysChar = HIWORD(l);
	}

	// Release the default device context
	::ReleaseDC(NULL, hdc);

	// Ensure cxSysChar and cySysChar are at least 1
	cxSysChar = max( cxSysChar, 1 );
	cySysChar = max( cySysChar, 1 );

	// Translate pixels to dialog units
	size.cx = short(MulDiv(size.cx, 4, cxSysChar));
	size.cy = short(MulDiv(size.cy, 8, cySysChar));

	// Create a LOGFONTA structure
    LOGFONTA logfont;

	// Initialize it
    memset(&logfont, 0, sizeof(LOGFONTA));
    logfont.lfHeight = 8;
    logfont.lfWeight = FW_NORMAL;
    logfont.lfCharSet = DEFAULT_CHARSET;
    strcpy(logfont.lfFaceName, "MS Sans Serif");

	// Try and create the font
	HFONT hfontSansSerif = CreateFontIndirectA(&logfont);
	if( hfontSansSerif )
	{
		// Get the default device context
		hdc = ::GetDC(NULL);

		// Select the property sheet font
		HFONT hfontOld = (HFONT)SelectObject(hdc, hfontSansSerif);

		// Get the metrics for this font
		TEXTMETRIC tm;
		GetTextMetrics(hdc, &tm);

		// Get the size of the entire alphabet, both upper and lower cases
		SIZE sizeText;
		::GetTextExtentPointA(hdc, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", 52, &sizeText);

		// Compute the average width of a character (rounding to the nearest value)
		cxSysChar = (sizeText.cx + 26) / 52;

		// compute the height of the characters, including the white space between rows
		cySysChar = tm.tmHeight + tm.tmExternalLeading;

		// Select the old font
		SelectObject(hdc, hfontOld);

		// Delete the property sheet font
		DeleteObject((HGDIOBJ)hfontSansSerif);

		// Release the default device context
		::ReleaseDC(NULL, hdc);

		// Ensure cxSysChar and cySysChar are at least 1
		cxSysChar = max( cxSysChar, 1 );
		cySysChar = max( cySysChar, 1 );

		// Translate dialog units to pixels
		size.cx = short(MulDiv(size.cx, cxSysChar, 4));
		size.cy = short(MulDiv(size.cy, cySysChar, 8));
	}
}



/////////////////////////////////////////////////////////////////////////////
// CMainFrame::CreatePropertiesBar

BOOL CMainFrame::CreatePropertiesBar( void )
{
	CString strTitle;

	if( !m_wndProperties.Create(this, IDD_PROPERTIES, 0, IDD_PROPERTIES) )
	{
		TRACE0( "Failed to create Properties dialog bar\n" );
		return FALSE;      // fail to create
	}

	m_wndProperties.SetBarStyle( m_wndProperties.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_FIXED );

	SIZE size = m_wndProperties.m_sizeDefault;
	ConvertSystemSizeToSansSerifSize( size );
	m_wndProperties.m_sizeDefault = size;

	if( strTitle.LoadString(IDS_PROPERTIES) )
	{
		m_wndProperties.SetWindowText( strTitle );
	}

	m_wndProperties.EnableDocking( 0 );
	m_wndProperties.ShowWindow( SW_HIDE );

	CRect rect;
	GetWindowRect( &rect );
	FloatControlBar( &m_wndProperties, rect.CenterPoint() );

	BOOL fSuccess = FALSE;

	CWnd* pWnd = m_wndProperties.GetDlgItem( IDC_NOTABS );
	if( pWnd )
	{
		m_wndProperties.m_pPropertySheet = new CPropertySheetCtrl;
		if( m_wndProperties.m_pPropertySheet )
		{
			m_wndProperties.m_pPropertySheet->InsertDummyPage();
			if( m_wndProperties.m_pPropertySheet->Create(pWnd, WS_CHILD, 0) )
			{
				m_wndProperties.m_pPropertySheet->ShowWindow( SW_HIDE );
				fSuccess = TRUE;
			}
		}
	}

	if( fSuccess == FALSE )
	{
		TRACE0( "Failed to create Properties dialog bar\n" );

		if( m_wndProperties.m_pPropertySheet )
		{
			delete m_wndProperties.m_pPropertySheet;
			m_wndProperties.m_pPropertySheet = NULL;
		}
	}
	
	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Overrides

/////////////////////////////////////////////////////////////////////////////
// CMainFrame::GetMessageString

void CMainFrame::GetMessageString( UINT nID, CString& rMessage ) const 
{
	CMDIFrameWnd::GetMessageString( nID, rMessage );

	if( rMessage.IsEmpty() )
	{
		theApp.m_pFramework->GetAddInsMenuHelpText( nID, rMessage );
	}

	if( rMessage.IsEmpty() )
	{
		theApp.m_pFramework->GetViewMenuHelpText( nID, rMessage );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnUpdateFrameMenu

void CMainFrame::OnUpdateFrameMenu( HMENU hMenuAlt )
{
	CMDIFrameWnd::OnUpdateFrameMenu( hMenuAlt );

	theApp.m_pFramework->FixAddInsMenu();
	theApp.m_pFramework->FixViewMenu();
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::GetFirstMDIChild

CWnd* CMainFrame::GetFirstMDIChild()
{
	CWnd* pWnd = NULL;

	if( m_hWndMDIClient )
	{
		pWnd = CWnd::FromHandle( m_hWndMDIClient );
		if( pWnd )
		{
			pWnd = pWnd->GetWindow( GW_CHILD );
		}
	}

	return pWnd;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::ActivateMDIChild

BOOL CMainFrame::ActivateMDIChild( int nPos )
{
	// Activate the corresponding MDI child window
	CWnd* pWnd = GetFirstMDIChild();

	if( pWnd )
	{
		int nMDIPos = 0;

		while( pWnd )
		{
			if( pWnd->IsWindowVisible() )
			{
				if( nMDIPos == nPos )
				{
					MDIActivate( pWnd );
					return TRUE;
				}

				nMDIPos++;
			}

			pWnd = pWnd->GetNextWindow();
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::GetStatusBarPane

HRESULT CMainFrame::GetStatusBarPane( HANDLE hKey, int nIndex, CJzStatusBarPane** ppJzStatusBarPane )
{
	if( ppJzStatusBarPane == NULL )
	{
		return E_POINTER;
	}

	*ppJzStatusBarPane = NULL;

	if( hKey == NULL )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = E_ACCESSDENIED;

	CJzStatusBarPane* pJzStatusBarPane;
    POSITION pos = m_lstStatusBarPanes.GetHeadPosition();
    while( pos )
    {
        pJzStatusBarPane = m_lstStatusBarPanes.GetNext( pos );

		if( pJzStatusBarPane->m_hKey == hKey )
		{
			// hKey is valid so we do not want to return E_ACCESSDENIED 
			hr = E_INVALIDARG;

			// Now look for a matching index
			if( pJzStatusBarPane->m_nIndex == nIndex )
			{
				*ppJzStatusBarPane = pJzStatusBarPane;
				return S_OK;
			}
		}
    }

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::SyncStatusBarPanes

HRESULT CMainFrame::SyncStatusBarPanes( void )
{
	HRESULT hr =  E_FAIL;
	CJzStatusBarPane* pJzStatusBarPane;

	UINT nIndicators[MAX_PANES + NBR_FRAMEWORK_PANES] =
	{
		ID_SEPARATOR,	// Framework Pane
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR
	};

	m_wndStatusBar.SetRedraw( FALSE );

	// Set number of panes
	int nNbrPanes = m_lstStatusBarPanes.GetCount();
	ASSERT( nNbrPanes <= MAX_PANES );
	if( m_wndStatusBar.SetIndicators((const UINT *)&nIndicators, nNbrPanes + NBR_FRAMEWORK_PANES) )
	{
		int nIndex = 0;

		POSITION pos = m_lstStatusBarPanes.GetHeadPosition();
		while( pos )
		{
			pJzStatusBarPane = m_lstStatusBarPanes.GetNext( pos );

			// Set status bar index value
			pJzStatusBarPane->m_nStatusBarIndex = nIndex++;

			// Set width and style of pane
			m_wndStatusBar.SetPaneInfo( pJzStatusBarPane->m_nStatusBarIndex + NBR_FRAMEWORK_PANES,
										ID_SEPARATOR,
										pJzStatusBarPane->m_nStyle,
										pJzStatusBarPane->m_nWidth );

			if( pJzStatusBarPane->m_strText.IsEmpty() == FALSE )
			{
				// Set text of pane
				m_wndStatusBar.SetPaneText( pJzStatusBarPane->m_nStatusBarIndex + NBR_FRAMEWORK_PANES,
											pJzStatusBarPane->m_strText,
											TRUE );
			}
		}

		hr = S_OK;
	}
	else
	{
		// Make sure there are no Component panes in status bar
		UINT nIndicator = ID_SEPARATOR;
		m_wndStatusBar.SetIndicators( &nIndicator, NBR_FRAMEWORK_PANES );

		// Clear list of panes
		while( !m_lstStatusBarPanes.IsEmpty() )
		{
			pJzStatusBarPane = static_cast<CJzStatusBarPane*>( m_lstStatusBarPanes.RemoveHead() );
			delete pJzStatusBarPane;
		}
	}

	m_wndStatusBar.SetRedraw( TRUE );
	m_wndStatusBar.Invalidate();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::InsertStatusBarPane

void CMainFrame::InsertStatusBarPane( CJzStatusBarPane* pJzStatusBarPane )
{
	CJzStatusBarPane* pJzStatusBarPaneListItem;
	POSITION pos2;

    POSITION pos = m_lstStatusBarPanes.GetHeadPosition();
    while( pos )
    {
		pos2 = pos;
        pJzStatusBarPaneListItem = m_lstStatusBarPanes.GetNext( pos );

		if( pJzStatusBarPaneListItem->m_nLifeSpan <= pJzStatusBarPane->m_nLifeSpan )
		{
			if( pJzStatusBarPaneListItem->m_hKey != pJzStatusBarPane->m_hKey )
			{
				m_lstStatusBarPanes.InsertBefore( pos2, pJzStatusBarPane );
				return;
			}
		}
    }

	m_lstStatusBarPanes.AddTail( pJzStatusBarPane );
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnViewStatusBar

void CMainFrame::OnViewStatusBar() 
{
	OnBarCheck( ID_VIEW_STATUS_BAR );
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnViewToolbar

void CMainFrame::OnViewToolbar() 
{
	OnBarCheck( ID_VIEW_TOOLBAR );
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnViewBookmarks

void CMainFrame::OnViewBookmarks() 
{
	OnBarCheck( ID_BOOKMARK_TOOLBAR );
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnUpdateViewBookmarks

void CMainFrame::OnUpdateViewBookmarks( CCmdUI* pCmdUI ) 
{
	CControlBar* pBar = GetControlBar( ID_BOOKMARK_TOOLBAR );
	if( pBar != NULL )
	{
		pCmdUI->SetCheck( (pBar->GetStyle() & WS_VISIBLE) != 0 );
		return;
	}
	pCmdUI->ContinueRouting();
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnViewProjectTree

void CMainFrame::OnViewProjectTree() 
{
	OnBarCheck( IDD_TREEBAR );
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnUpdateViewProjectTree

void CMainFrame::OnUpdateViewProjectTree(CCmdUI* pCmdUI) 
{
	CControlBar* pBar = GetControlBar( IDD_TREEBAR );
	if( pBar != NULL )
	{
		pCmdUI->SetCheck( (pBar->GetStyle() & WS_VISIBLE) != 0 );
		return;
	}
	pCmdUI->ContinueRouting();
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnViewProperties

void CMainFrame::OnViewProperties() 
{
	BOOL fTreeHasFocus = FALSE;

	// Check to see if tree has focus before Properties window steals focus
	CTreeCtrl* pTreeCtrl = GetProjectTreeCtrl();
	if( pTreeCtrl
	&&  pTreeCtrl == CWnd::GetFocus() )
	{
		fTreeHasFocus = TRUE;
	}

	OnBarCheck( IDD_PROPERTIES );

	// If the Properties window is showing
	CControlBar* pBar = GetControlBar( IDD_PROPERTIES );
	if( pBar 
	&&  pBar->GetStyle() & WS_VISIBLE )
	{
		// and the Project Tree has the focus
		if( fTreeHasFocus )
		{
			// Display properties for the node
			m_wndTreeBar.DisplayNodeProperties();
		}
		else
		{
			// Get the active MDI child window.
			CMDIChildWnd *pMDIChild = (CMDIChildWnd *)GetActiveFrame();
			if( pMDIChild )
			{
				// Get the MDI child window's active view
				CComponentView *pView = (CComponentView *)pMDIChild->GetActiveView();
				if( pView
				&&  pView->IsKindOf( RUNTIME_CLASS(CComponentView) ) )
				{
					ASSERT( pView->m_pIEditor != NULL );
					
					if( pView->m_pIEditor )
					{
						CWnd* pWnd = CWnd::GetFocus();

						while( pWnd )
						{
							if( pWnd == pMDIChild )
							{
								if( FAILED ( pView->m_pIEditor->OnViewProperties() ) )
								{
									if( m_wndProperties.m_pPropertySheet )
									{
										m_wndProperties.m_pPropertySheet->RemoveCurrentPageManager();
									}
								}
								break;
							}

							pWnd = pWnd->GetParent();
						}
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnUpdateViewProperties

void CMainFrame::OnUpdateViewProperties(CCmdUI* pCmdUI) 
{
	CControlBar* pBar = GetControlBar( IDD_PROPERTIES );
	if( pBar != NULL )
	{
		pCmdUI->SetCheck( (pBar->GetStyle() & WS_VISIBLE) != 0 );
		return;
	}
	pCmdUI->ContinueRouting();
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnClose

void CMainFrame::OnClose() 
{
	theApp.m_fShutDown = TRUE;
	theApp.m_nSavePromptAction = 0;

	theApp.SaveAppState();

	// Hide Property Sheet
	BOOL fPropSheetWasShowing = FALSE;

	if( m_wndProperties.m_pPropertySheet )
	{
		if( m_wndProperties.m_pPropertySheet->IsShowing() == S_OK )
		{
			fPropSheetWasShowing = TRUE;
		}
		m_wndProperties.m_pPropertySheet->Show( FALSE );
	}

	CMDIFrameWnd::OnClose();

	if( theApp.GetNbrDocuments() > 0 )
	{
		theApp.m_nSavePromptAction = 0;
		theApp.m_fShutDown = FALSE;
		if( fPropSheetWasShowing )
		{
			m_wndProperties.m_pPropertySheet->Show( TRUE );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnDestroy

void CMainFrame::OnDestroy() 
{
	// Cleanup Components while we still have window
	if( theApp.m_pFramework )
	{
		theApp.m_pFramework->CleanUp();
	}

	CMDIFrameWnd::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::RecalcLayout

void CMainFrame::RecalcLayout( BOOL bNotify ) 
{
	CControlBar* pBar;

	// Recalc frame layout (before Project Tree is resized)
	CMDIFrameWnd::RecalcLayout( FALSE );
	
	// Resize Project Tree
	pBar = GetControlBar( IDD_TREEBAR );
	if( (pBar != NULL)
	&&  (pBar->IsWindowVisible())
	&&  (pBar->IsIconic() == FALSE) )
	{	
		CRect rectDockBar;
		CRect rectTreeBar;

		// Stretch control bar to fit in frame
		if( !(pBar->m_dwStyle & CBRS_FLOATING) )
		{
			CWnd* pDockBar = NULL;

			if( pBar->m_dwStyle & CBRS_ALIGN_LEFT )
			{
				pDockBar = GetDlgItem( AFX_IDW_DOCKBAR_LEFT );
			}
			else if( pBar->m_dwStyle & CBRS_ALIGN_RIGHT )
			{
				pDockBar = GetDlgItem( AFX_IDW_DOCKBAR_RIGHT );
			}
			if( pDockBar )
			{
				pDockBar->GetClientRect( &rectDockBar );
				pBar->CalcDynamicLayout( rectDockBar.Height() + 4, LM_VERTDOCK|LM_STRETCH );
			}
		}

		// make sure tree list fits in control bar
		pBar->GetClientRect( &rectTreeBar );
		pBar->SendMessage( WM_SIZE, SIZE_RESTORED,
						   MAKELONG(rectTreeBar.Width(), rectTreeBar.Height()) );
	}

	// Recalc frame layout (after Project Tree is resized)
	CMDIFrameWnd::RecalcLayout( bNotify );
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnNotify

BOOL CMainFrame::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult ) 
{
	switch( wParam )
	{
		case IDC_TREE:
		{
			NMHDR* pNMHdr = (NMHDR*)lParam;
			CTreeCtrl* pTreeCtrl = GetProjectTreeCtrl();

			if( pTreeCtrl == NULL )
			{
				break;
			}

			switch( pNMHdr->code )
			{
				case NM_CLICK:
						m_wndTreeBar.DisplayNodeProperties();
						break;

				case NM_DBLCLK:
						m_wndTreeBar.OnDoubleClick( pNMHdr, pTreeCtrl, FALSE, pResult );
						return TRUE;

				case NM_KILLFOCUS:
						break;

				case NM_OUTOFMEMORY:
						break;

				case NM_RCLICK:
					{
						POINT pt;

						::GetCursorPos( &pt );
						m_wndTreeBar.OnRightClick( pNMHdr, pTreeCtrl, pt, pResult );
						return TRUE;
					}

				case NM_RDBLCLK:
						break;

				case NM_RETURN:
						break;

				case NM_SETFOCUS:
						m_wndTreeBar.OnSetFocus( pNMHdr, pTreeCtrl, pResult );
						return TRUE;

				case TVN_BEGINDRAG:
						m_wndTreeBar.OnBeginDrag( (NM_TREEVIEW*)lParam, pTreeCtrl, pResult );
						return TRUE;

				case TVN_BEGINRDRAG:
						m_wndTreeBar.OnBeginRDrag( (NM_TREEVIEW*)lParam, pTreeCtrl, pResult );
						return TRUE;

				case TVN_BEGINLABELEDIT:
						m_wndTreeBar.OnBeginLabelEdit( (TV_DISPINFO FAR*)lParam, pTreeCtrl, pResult );
						return TRUE;

				case TVN_DELETEITEM:
						m_wndTreeBar.OnDeleteItem( (NM_TREEVIEW*)lParam, pTreeCtrl, pResult );
						return TRUE;

				case TVN_ENDLABELEDIT:
						m_wndTreeBar.OnEndLabelEdit( (TV_DISPINFO FAR*)lParam, pTreeCtrl, pResult );
						return TRUE;

				case TVN_ITEMEXPANDED:
						m_wndTreeBar.OnItemExpanded( (NM_TREEVIEW*)lParam, pTreeCtrl, pResult );
						return TRUE;

				case TVN_ITEMEXPANDING:
						break;

				case TVN_SELCHANGED:
						m_wndTreeBar.OnSelChanged( (NM_TREEVIEW*)lParam, pTreeCtrl, pResult );
						return TRUE;

				case TVN_SELCHANGING:
						break;

				case TVN_KEYDOWN:
						break;
			}

			break;
		}
	}
	
	return CMDIFrameWnd::OnNotify( wParam, lParam, pResult );
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::InitWindowMenu

void CMainFrame::InitWindowMenu( CMenu* pWindowMenu )
{
	// Remove crusty MDI child window menu items
	UINT nMenuID;

	for( int i = pWindowMenu->GetMenuItemCount()-1 ;  i >= 0 ;  i-- )
	{
		nMenuID = pWindowMenu->GetMenuItemID( i );

		if( nMenuID == AFX_IDM_FIRST_MDICHILD )
		{
			pWindowMenu->RemoveMenu( i, MF_BYPOSITION );

			// Remove separator before the first MDI child menu item.
			pWindowMenu->RemoveMenu( i-1, MF_BYPOSITION );
			break;
		}
		else if( nMenuID > AFX_IDM_FIRST_MDICHILD )
		{
			pWindowMenu->RemoveMenu( i, MF_BYPOSITION );
		}
	}

	CString strWindowTitle;
	CString strMenuText;

	nMenuID = AFX_IDM_FIRST_MDICHILD;
	BOOL fChecked = FALSE;

	CWnd* pWndActive = MDIGetActive();

	CWnd* pWnd = GetFirstMDIChild();
	while( pWnd )
	{
		if( pWnd->IsWindowVisible() )
		{
			if( nMenuID > IDM_LAST_MDICHILD )
			{
				// Add the Windows... menu item
				strMenuText.LoadString( IDS_MORE_WINDOWS );

				pWindowMenu->AppendMenu( MF_STRING, nMenuID, strMenuText );
				if( fChecked == FALSE )
				{
					pWindowMenu->CheckMenuItem( nMenuID, (MF_BYCOMMAND | MF_CHECKED) );
				}
				break;
			}

			if( nMenuID == AFX_IDM_FIRST_MDICHILD )
			{
				// Add separator
				pWindowMenu->AppendMenu( MF_SEPARATOR, 0 );
			}

			pWnd->GetWindowText( strWindowTitle );
			strMenuText.Format( "&%d %s", (nMenuID - AFX_IDM_FIRST_MDICHILD) + 1, strWindowTitle );

			pWindowMenu->AppendMenu( MF_STRING, nMenuID, strMenuText );
			if( pWnd == pWndActive )
			{
				pWindowMenu->CheckMenuItem( nMenuID, (MF_BYCOMMAND | MF_CHECKED) );
				fChecked = TRUE;
			}

			nMenuID++;
		}

		pWnd = pWnd->GetNextWindow();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnInitMenuPopup

void CMainFrame::OnInitMenuPopup( CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu ) 
{
	CString strMenuText;
	TCHAR achFileText[MID_BUFFER];
	TCHAR achViewText[MID_BUFFER];
	TCHAR achAddInsText[MID_BUFFER];
	TCHAR achWindowText[MID_BUFFER];

	CMenu* pMainMenu = GetMenu();
	if( pMainMenu )
	{
		pMainMenu->GetMenuString( nIndex, strMenuText, MF_BYPOSITION );
	}

	::LoadString( theApp.m_hInstance, IDS_FILE_MENU_TEXT, achFileText, MID_BUFFER );
	::LoadString( theApp.m_hInstance, IDS_VIEW_MENU_TEXT, achViewText, MID_BUFFER );
	::LoadString( theApp.m_hInstance, IDS_ADDINS_MENU_TEXT, achAddInsText, MID_BUFFER );
	::LoadString( theApp.m_hInstance, IDS_WINDOW_MENU_TEXT, achWindowText, MID_BUFFER );

	if( _tcscmp( strMenuText, achWindowText ) == 0 )
	{
		InitWindowMenu( pPopupMenu );
		return;
	}

	if( _tcscmp( strMenuText, achFileText ) == 0 
	||  _tcscmp( strMenuText, achViewText ) == 0 
	||  _tcscmp( strMenuText, achWindowText ) == 0 )
	{
		// File, View, Window menu handled here...
		CMDIFrameWnd::OnInitMenuPopup( pPopupMenu, nIndex, bSysMenu );
	}
	else
	{
		BOOL fHandled = FALSE;

		// Get the active MDI child window.
		CMDIChildWnd *pMDIChild = (CMDIChildWnd *)GetActiveFrame();
		if( pMDIChild )
		{
			// Get the MDI child window's active view
			CComponentView *pView = (CComponentView *)pMDIChild->GetActiveView();

			if( pView
			&&  pView->IsKindOf( RUNTIME_CLASS(CComponentView) ) )
			{
				CWnd* pWnd = pView->GetNextWindow( GW_CHILD );
				if( pWnd )
				{
					pWnd->SendMessage( WM_INITMENUPOPUP, (WPARAM)pMainMenu->GetSafeHmenu(), MAKELPARAM(nIndex,bSysMenu) );
					fHandled = TRUE;
				}
			}
		}

		if( fHandled == FALSE )
		{
			CMDIFrameWnd::OnInitMenuPopup( pPopupMenu, nIndex, bSysMenu );
		}
	}

	if( _tcscmp( strMenuText, achAddInsText ) == 0 )
	{
		theApp.m_pFramework->OnInitAddInsMenu( pPopupMenu );
		return;
	}

	if( _tcscmp( strMenuText, achViewText ) == 0 )
	{
		theApp.m_pFramework->OnInitViewMenu( pPopupMenu );
		return;
	}
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CMoreWindowsDlg dialog used for "Windows..." menu item

class CMoreWindowsDlg : public CDialog
{
public:
	CMoreWindowsDlg();

// Dialog Data
	//{{AFX_DATA(CMoreWindowsDlg)
	enum { IDD = IDD_MORE_WINDOWS };
	CButton	m_btnOK;
	CListBox	m_lstbxWindows;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMoreWindowsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CMoreWindowsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeWindows();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CMoreWindowsDlg::CMoreWindowsDlg() : CDialog(CMoreWindowsDlg::IDD)
{
	//{{AFX_DATA_INIT(CMoreWindowsDlg)
	//}}AFX_DATA_INIT
}

void CMoreWindowsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMoreWindowsDlg)
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_WINDOWS, m_lstbxWindows);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMoreWindowsDlg, CDialog)
	//{{AFX_MSG_MAP(CMoreWindowsDlg)
	ON_LBN_SELCHANGE(IDC_WINDOWS, OnSelChangeWindows)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CMoreWindowsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	// Fill Windows list box
	CString strText;
	CSize sizeText;
	CDC* pDC;
	int nCurExtent;
	int nPos;
	int nIndex = 0;

	CWnd* pWndActive = pMainFrame->MDIGetActive();

	CWnd* pWnd = pMainFrame->GetFirstMDIChild();
	while( pWnd )
	{
		if( pWnd->IsWindowVisible() )
		{
			pWnd->GetWindowText( strText );

			// Add text to list box
			nPos = m_lstbxWindows.AddString( strText );
			if( nPos >= 0 )
			{
				// Set the item's index
				m_lstbxWindows.SetItemData( nPos, nIndex++ );

				// Select the active MDI child window
				if( pWnd == pWndActive )
				{
					m_lstbxWindows.SetCurSel( nPos );
					m_btnOK.EnableWindow( TRUE );
				}

				// Set horizontal extent
				nCurExtent = m_lstbxWindows.GetHorizontalExtent();

				pDC = m_lstbxWindows.GetDC();
				if( pDC )
				{
					sizeText = pDC->GetTextExtent( strText );

					if( sizeText.cx > nCurExtent )
					{
						m_lstbxWindows.SetHorizontalExtent( sizeText.cx );
					}

					m_lstbxWindows.ReleaseDC( pDC );
				}
			}
		}

		pWnd = pWnd->GetNextWindow();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMoreWindowsDlg::OnSelChangeWindows() 
{
	m_btnOK.EnableWindow( TRUE );
}

void CMoreWindowsDlg::OnOK() 
{
	// Get the selected item
	int nPos = m_lstbxWindows.GetCurSel();
	if( nPos != LB_ERR )
	{
		// Get the selected item's MDI child window index
		int nIndex = m_lstbxWindows.GetItemData( nPos );
		if( nIndex != LB_ERR )
		{
			CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
			ASSERT( pMainFrame != NULL );

			// Activate the corresponding MDI child window
			pMainFrame->ActivateMDIChild( nIndex );
		}
	}
	
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnSelectWindowMenu

BOOL CMainFrame::OnSelectWindowMenu( UINT nCommandID )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( nCommandID < AFX_IDM_FIRST_MDICHILD
	||  nCommandID > (IDM_LAST_MDICHILD + 1) )
	{
		return FALSE;
	}

	if( nCommandID == (IDM_LAST_MDICHILD + 1) )
	{
		// Windows... menu item was selected
		CMoreWindowsDlg winDlg;
		winDlg.DoModal();
	}
	else
	{
		// Activate the corresponding MDI child window
		ActivateMDIChild( nCommandID - AFX_IDM_FIRST_MDICHILD );
	}
	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnCommand

BOOL CMainFrame::OnCommand( WPARAM wParam, LPARAM lParam ) 
{
	if( HIWORD(wParam) == 0 )	// from menu
	{
		if( theApp.m_pFramework->OnSelectAddInsMenu( LOWORD(wParam) ) )
		{
			return TRUE;
		}
		if( theApp.m_pFramework->OnSelectViewMenu( LOWORD(wParam) ) )
		{
			return TRUE;
		}
		if( OnSelectWindowMenu( LOWORD(wParam) ) )
		{
			return TRUE;
		}

		// Get the active MDI child window.
		CMDIChildWnd *pMDIChild = (CMDIChildWnd *)GetActiveFrame();
		if( pMDIChild )
		{
			// Get the MDI child window's active view
			CComponentView *pView = (CComponentView *)pMDIChild->GetActiveView();

			if( pView
			&&  pView->IsKindOf( RUNTIME_CLASS(CComponentView) ) )
			{
				CWnd* pWnd = pView->GetNextWindow( GW_CHILD );
				if( pWnd )
				{
					pWnd = pWnd->GetNextWindow( GW_CHILD );
					if( pWnd )
					{
						if( ::SendMessage( pWnd->GetSafeHwnd(), WM_COMMAND, wParam, lParam ) )
						{
							return TRUE;
						}
					}
				}
			}
		}
	}
	
	return CMDIFrameWnd::OnCommand( wParam, lParam );
} 


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::DestroyWindow

BOOL CMainFrame::DestroyWindow() 
{
	if( m_wndProperties.m_pPropertySheet )
	{
		m_wndProperties.m_pPropertySheet->RemoveCurrentPageManager();
	}
	
	return CMDIFrameWnd::DestroyWindow();
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnUpdateEditCut

void CMainFrame::OnUpdateEditCut( CCmdUI* pCmdUI ) 
{
	m_wndTreeBar.OnUpdateEditCut( pCmdUI );
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnEditCut

void CMainFrame::OnEditCut( void ) 
{
	m_wndTreeBar.OnEditCut();
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnUpdateEditCopy

void CMainFrame::OnUpdateEditCopy( CCmdUI* pCmdUI ) 
{
	m_wndTreeBar.OnUpdateEditCopy( pCmdUI );
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnEditCopy

void CMainFrame::OnEditCopy( void ) 
{
	m_wndTreeBar.OnEditCopy();
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnUpdateEditPaste

void CMainFrame::OnUpdateEditPaste( CCmdUI* pCmdUI ) 
{
	m_wndTreeBar.OnUpdateEditPaste( pCmdUI );
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnEditPaste

void CMainFrame::OnEditPaste( void ) 
{
	m_wndTreeBar.OnEditPaste();
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnUpdateEditDelete

void CMainFrame::OnUpdateEditDelete( CCmdUI* pCmdUI ) 
{
	m_wndTreeBar.OnUpdateEditDelete( pCmdUI );
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnEditDelete

void CMainFrame::OnEditDelete( void ) 
{
	m_wndTreeBar.OnEditDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnQueryEndSession

BOOL CMainFrame::OnQueryEndSession() 
{
	theApp.m_fShutDown = TRUE;
	theApp.m_nSavePromptAction = 0;

	theApp.SaveAppState();

	// Hide Property Sheet
//	if( m_wndProperties.m_pPropertySheet )
//	{
//		m_wndProperties.m_pPropertySheet->Show( FALSE );
//	}

	// Save all Projects
	BOOL fContinue = theApp.SaveAllProjects();

	theApp.m_fShutDown = FALSE;
	theApp.m_nSavePromptAction = 0;
	
	return fContinue;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnEndSession

void CMainFrame::OnEndSession( BOOL fEnding ) 
{
	// Close all Projects
	if( fEnding )
	{
		theApp.CloseAllProjects();
		DestroyWindow();
	}

	CMDIFrameWnd::OnEndSession( fEnding );
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnViewToggle

void CMainFrame::OnViewToggle() 
{
	CTreeCtrl* pTreeCtrl = GetProjectTreeCtrl();
	if( pTreeCtrl == NULL )
	{
		return;
	}

	CComponentView* pActiveView = NULL;
	BOOL fPropSheetShowing = FALSE;
	BOOL fTreeShowing = FALSE;

	CMDIChildWnd* pFrame = MDIGetActive();
	if( pFrame )
	{
		pActiveView = (CComponentView *)pFrame->GetActiveView();
	}

	if( m_wndProperties.m_pPropertySheet->IsShowing() == S_OK )
	{
		fPropSheetShowing = TRUE;
	}

	if( m_wndTreeBar.GetStyle() & WS_VISIBLE )
	{
		fTreeShowing = TRUE;
	}

	CWnd* pWndHasFocus = CWnd::GetFocus();
	CWnd* pWndChild;

	// Project Tree toggles to active editor
	if( pTreeCtrl == pWndHasFocus )
	{
		if( pActiveView )
		{
			pWndChild = pActiveView->GetNextWindow( GW_CHILD );
			if( pWndChild )
			{
				pWndChild = pWndChild->GetNextWindow( GW_CHILD );
				if( pWndChild )
				{
					pWndChild = pWndChild->GetNextDlgTabItem( pWndChild );
					if( pWndChild )
					{
						pWndChild->SetFocus();
					}
				}
			}
		}
		else if( fPropSheetShowing )
		{
			m_wndProperties.m_pPropertySheet->SetFocus();
		}
		return;
	}

	// Property Sheet toggles to Project Tree
	CWnd* pWnd = pWndHasFocus;
	while( pWnd )
	{
		if( pWnd == m_wndProperties.m_pPropertySheet )
		{
			if( fTreeShowing )
			{
				pTreeCtrl->SetFocus();
			}
			else if( pActiveView )
			{
				pWndChild = pActiveView->GetNextWindow( GW_CHILD );
				if( pWndChild )
				{
					pWndChild = pWndChild->GetNextWindow( GW_CHILD );
					if( pWndChild )
					{
						pWndChild = pWndChild->GetNextDlgTabItem( pWndChild );
						if( pWndChild )
						{
							pWndChild->SetFocus();
						}
					}
				}
			}
			return;
		}

		pWnd = pWnd->GetParent();
	}

	// Active editor (or unknown) toggles to Property Sheet
	if( fPropSheetShowing )
	{
		m_wndProperties.m_pPropertySheet->SetFocus();
	}
	else if( fTreeShowing )
	{
		pTreeCtrl->SetFocus();
	}
	else if( pActiveView )
	{
		pWndChild = pActiveView->GetNextWindow( GW_CHILD );
		if( pWndChild )
		{
			pWndChild = pWndChild->GetNextWindow( GW_CHILD );
			if( pWndChild )
			{
				pWndChild = pWndChild->GetNextDlgTabItem( pWndChild );
				if( pWndChild )
				{
					pWndChild->SetFocus();
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnActivateApp

void CMainFrame::OnActivateApp( BOOL bActive, HTASK hTask ) 
{
	CMDIFrameWnd::OnActivateApp( bActive, hTask );

	if( theApp.m_pFramework )
	{
		theApp.m_pFramework->OnActivateApp( bActive );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnTransPlay

void CMainFrame::OnTransPlay() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Get IPrivateTransport interface pointer
	IDMUSProdComponent* pIComponent = NULL;
	IPrivateTransport *pITransport = NULL;
	if( SUCCEEDED ( theApp.m_pFramework->FindComponent( CLSID_CConductor,  &pIComponent ) )
	&&  SUCCEEDED ( pIComponent->QueryInterface( IID_IPrivateTransport, (void**)&pITransport ) ) )
	{
		pITransport->PlayFromCursor();
		pITransport->Release();
	}
	if( pIComponent )
	{
		pIComponent->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnTransPlayFromStart

void CMainFrame::OnTransPlayFromStart() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Get IPrivateTransport interface pointer
	IDMUSProdComponent* pIComponent = NULL;
	IPrivateTransport *pITransport = NULL;
	if( SUCCEEDED ( theApp.m_pFramework->FindComponent( CLSID_CConductor,  &pIComponent ) )
	&&  SUCCEEDED ( pIComponent->QueryInterface( IID_IPrivateTransport, (void**)&pITransport ) ) )
	{
		pITransport->PlayFromStart();
		pITransport->Release();
	}
	if( pIComponent )
	{
		pIComponent->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnTransRecord

void CMainFrame::OnTransRecord() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Get IPrivateTransport interface pointer
	IDMUSProdComponent* pIComponent = NULL;
	IPrivateTransport *pITransport = NULL;
	if( SUCCEEDED ( theApp.m_pFramework->FindComponent( CLSID_CConductor,  &pIComponent ) )
	&&  SUCCEEDED ( pIComponent->QueryInterface( IID_IPrivateTransport, (void**)&pITransport ) ) )
	{
		pITransport->Record();
		pITransport->Release();
	}
	if( pIComponent )
	{
		pIComponent->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnTransTransition

void CMainFrame::OnTransTransition() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Get IPrivateTransport interface pointer
	IDMUSProdComponent* pIComponent = NULL;
	IPrivateTransport *pITransport = NULL;
	if( SUCCEEDED ( theApp.m_pFramework->FindComponent( CLSID_CConductor,  &pIComponent ) )
	&&  SUCCEEDED ( pIComponent->QueryInterface( IID_IPrivateTransport, (void**)&pITransport ) ) )
	{
		pITransport->Transition();
		pITransport->Release();
	}
	if( pIComponent )
	{
		pIComponent->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnTransSpace

void CMainFrame::OnTransSpace() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Get IPrivateTransport interface pointer
	IDMUSProdComponent* pIComponent = NULL;
	IPrivateTransport *pITransport = NULL;
	if( SUCCEEDED ( theApp.m_pFramework->FindComponent( CLSID_CConductor,  &pIComponent ) )
	&&  SUCCEEDED ( pIComponent->QueryInterface( IID_IPrivateTransport, (void**)&pITransport ) ) )
	{
		pITransport->SpaceBarPress();
		pITransport->Release();
	}
	if( pIComponent )
	{
		pIComponent->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnHelp

void CMainFrame::OnHelp() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CTreeCtrl* pTreeCtrl = GetProjectTreeCtrl();
	CWnd* pWndHasFocus = CWnd::GetFocus();

	if( pTreeCtrl == pWndHasFocus )
	{
		// Determine name of DMUSProd.exe help file
		CString strHelpFileName;

		if( theApp.GetHelpFileName( strHelpFileName ) )
		{
			strHelpFileName += "::/htm/ProjectTree.htm";
			::HtmlHelp( NULL, strHelpFileName, HH_DISPLAY_TOPIC, 0 );
		}
		return;
	}
	else
	{
		CMDIChildWnd* pFrame = MDIGetActive();
		if( pFrame )
		{
			CComponentView* pActiveView = (CComponentView *)pFrame->GetActiveView();
			if( pActiveView
			&&  pActiveView->m_pIEditor )
			{
				if( SUCCEEDED ( pActiveView->m_pIEditor->OnF1Help() ) )
				{
					return;
				}
			}
		}
	}

	// Default to table of contents
	CString strHelpFileName;

	if( theApp.GetHelpFileName( strHelpFileName ) )
	{
		strHelpFileName += "::/htm/directmusicproducer.htm";
		::HtmlHelp( NULL, strHelpFileName, HH_DISPLAY_TOPIC, 0 );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnApp

LRESULT CMainFrame::OnApp( WPARAM wParam, LPARAM lParam )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// This allows the Transport accelerators to continue working while the DMO
	// or Tool property page is open
	if( wParam )
	{
		MSG *pMsg = (MSG *)wParam;
		if( theApp.PreTranslateMessage( pMsg ) )
		{
			::SendMessage( m_hWnd, pMsg->message, pMsg->wParam, pMsg->lParam );
		}
	}

	return FALSE;
}
