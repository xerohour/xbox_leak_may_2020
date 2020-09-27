// XboxAddinDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XboxAddin.h"
#include "XboxAddinDlg.h"
#include "SecondaryDlg.h"
#include "PrimaryDlg.h"
#include "Audiopath.h"
#include "RightPaneDlg.h"
#include "LeftPaneDlg.h"
#include "XboxAddinComponent.h"
#include "TargetDirectoryDlg.h"
#include "TargetXboxDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
    BOOL GetFileVersion( LPTSTR szExeName, LPTSTR szFileVersion, short nFileVersionSize );
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXboxAddinDlg dialog

CXboxAddinDlg::CXboxAddinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXboxAddinDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXboxAddinDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_wSplitterPos = 0;
	m_pLeftPaneDlg = NULL;
	m_pRightPaneDlg = NULL;
}

CXboxAddinDlg::~CXboxAddinDlg()
{
	if( m_pLeftPaneDlg )
	{
		delete m_pLeftPaneDlg;
		m_pLeftPaneDlg = NULL;
	}
	if( m_pRightPaneDlg )
	{
		delete m_pRightPaneDlg;
		m_pRightPaneDlg = NULL;
	}
}

void CXboxAddinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXboxAddinDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CXboxAddinDlg, CDialog)
	//{{AFX_MSG_MAP(CXboxAddinDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_COMMAND(ID_SET_TARGET_DIRECTORY, OnSetTargetDirectory)
	ON_COMMAND(ID_CONNECT, OnConnect)
	ON_COMMAND(ID_DISCONNECT, OnDisconnect)
	ON_COMMAND(ID_SYNCHRONIZE_ALL, OnSynchronizeAll)
	ON_COMMAND(ID_SYNCHRONIZE_AUTO, OnSynchronizeAuto)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_XBOX_PANIC, OnXboxPanic)
	ON_COMMAND(ID_SET_XBOX_NAME, OnSetXboxName)
	ON_WM_MENUSELECT()
	ON_MESSAGE(WM_APP, OnApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXboxAddinDlg message handlers

BOOL CXboxAddinDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
    // Resize the panes
    UpdateListBoxPositions();

	// Disable all the dialogs, unless we're already connected
	OnConnectionStateChanged();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CXboxAddinDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch( nID & 0xFFF0 )
	{
	case IDM_ABOUTBOX:
		OnAppAbout();
		break;
	case SC_CLOSE:
		DestroyWindow();
		break;
	default:
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CXboxAddinDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CXboxAddinDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

int CXboxAddinDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create the Splitter and position it in the Editor
	// (temporary values added until coords are saved to design-time file)
	RECT rect;
	rect.left = lpCreateStruct->x;
    rect.right = rect.left + DEFAULTSPLITTERWIDTH;
    rect.top = lpCreateStruct->y;
	rect.bottom = lpCreateStruct->cy;

	m_wndVSplitter.Create( this, &rect );

	// Create the left pane dialog and position it in the First Splitter Pane
	m_pLeftPaneDlg = new CLeftPaneDlg;
	if( m_pLeftPaneDlg == NULL )
	{
		return -1;
	}

	// Point the left pane dialog back to us and at the AudioPath object
	m_pLeftPaneDlg->Create( IDD_LEFT_PANE, this );
	m_pLeftPaneDlg->ShowWindow( SW_SHOW );
	m_wndVSplitter.SetFirstPane( m_pLeftPaneDlg );

	// Create the other files dialog and position it in the Second Splitter Pane
	m_pRightPaneDlg = new CRightPaneDlg;
	if( m_pRightPaneDlg == NULL )
	{
		return -1;
	}

	// Point the other files dialog back to us and at the AudioPath object
	m_pRightPaneDlg->Create( IDD_RIGHT_PANE, this );
	m_pRightPaneDlg->ShowWindow( SW_SHOW );
	m_wndVSplitter.SetSecondPane( m_pRightPaneDlg );

	// Attach the menu 
	HMENU hMenu = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDR_XBOXADDIN_MENU) );
	::SetMenu( GetSafeHwnd(), hMenu );
	
	return 0;
}

void CXboxAddinDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

    UpdateListBoxPositions();
}

void CXboxAddinDlg::UpdateListBoxPositions( void )
{
    if( m_wndVSplitter.GetSafeHwnd() == NULL )
    {
        return;
    }

    RECT rectClient;
    GetClientRect( &rectClient );

    // get splitter position from audio path
	WORD pos = m_wSplitterPos;

	// calculate where splitter should be
	if(pos == 0)
	{
		// put it right in the middle
		pos = WORD(min( USHRT_MAX, (rectClient.right - DEFAULTSPLITTERWIDTH)/2 ));

		// save where it is
        m_wSplitterPos = pos;
	}

    // Update splitter position, if it's non-zero
	if( pos != 0 )
	{
		// splitter is right side of chord dialog and left side
		// of signpost dialog
		RECT rect = rectClient;
		rect.right = max( DEFAULTSPLITTERWIDTH, pos );
		rect.left = rect.right - DEFAULTSPLITTERWIDTH;
		m_wndVSplitter.SetTracker(rect);
		m_wndVSplitter.MoveWindow( &rect, TRUE );
		m_pLeftPaneDlg->MoveWindow(0, 0, rect.left, rectClient.bottom);
		m_pRightPaneDlg->MoveWindow(rect.right, 0, rectClient.right - rect.right, rectClient.bottom);
	}
}

void CXboxAddinDlg::EndTrack( long lNewPos )
{
    m_wSplitterPos = (WORD)lNewPos;
}

void CXboxAddinDlg::OnDestroy() 
{
	// Save window placement
	ASSERT( theApp.m_pXboxAddinComponent->m_pXboxAddinDlg != NULL );
	if( theApp.m_pXboxAddinComponent->m_pXboxAddinDlgWP == NULL )
	{
		theApp.m_pXboxAddinComponent->m_pXboxAddinDlgWP = new WINDOWPLACEMENT;
	}
	if( theApp.m_pXboxAddinComponent->m_pXboxAddinDlgWP )
	{
		theApp.m_pXboxAddinComponent->m_pXboxAddinDlgWP->length = sizeof(WINDOWPLACEMENT);
		if( GetWindowPlacement( theApp.m_pXboxAddinComponent->m_pXboxAddinDlgWP ) == 0 )
		{
			delete theApp.m_pXboxAddinComponent->m_pXboxAddinDlgWP;
			theApp.m_pXboxAddinComponent->m_pXboxAddinDlgWP = NULL;
		}
	}

	CDialog::OnDestroy();
}

void CXboxAddinDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CXboxAddinDlg::PostNcDestroy() 
{
	// Free resources
	ASSERT( theApp.m_pXboxAddinComponent->m_pXboxAddinDlg != NULL );
	if( theApp.m_pXboxAddinComponent->m_pXboxAddinDlg )
	{
		delete theApp.m_pXboxAddinComponent->m_pXboxAddinDlg;
		theApp.m_pXboxAddinComponent->m_pXboxAddinDlg = NULL;
	}

	CDialog::PostNcDestroy();
}

void CXboxAddinDlg::OnFileClose() 
{
	DestroyWindow();
}

void CXboxAddinDlg::OnSetTargetDirectory() 
{
	CTargetDirectoryDlg targetDirectoryDlg;
	if( IDC_SYNCHRONIZE == targetDirectoryDlg.DoModal() )
	{
		OnSynchronizeAll();
	}
}

void CXboxAddinDlg::OnConnect() 
{
	theApp.ConnectToXbox();

	OnConnectionStateChanged();

	OnSynchronizeAll();
}

void CXboxAddinDlg::OnDisconnect() 
{
	theApp.DisconnectFromXbox();

	OnConnectionStateChanged();
}

void CXboxAddinDlg::OnSynchronizeAll() 
{
	// Display a wait cursor - this may take a while
	CWaitCursor waitCursor;

	// Stop everything
	OnXboxPanic();

	// Unload everything
	m_pLeftPaneDlg->UnloadAll();
	// Can't load non-segment files, so don't need to unload them
	//m_pRightPaneDlg->UnloadAll();

	// Delete everything
	m_pLeftPaneDlg->DeleteAll();
	m_pRightPaneDlg->DeleteAll();

	// Re-copy and download everything
	m_pLeftPaneDlg->ReCopyAll();
	// Don't specifically copy files in the right pane until scripts are enabled
	//m_pRightPaneDlg->ReCopyAll();
}

void CXboxAddinDlg::OnSynchronizeAuto() 
{
	// TODO: Add your command handler code here
}

void CXboxAddinDlg::OnMenuSelect( UINT nItemID, UINT nFlags, HMENU hSysMenu )
{
	if( (0xFFFF != nFlags) && (nFlags & MF_POPUP) )
	{
		HMENU hSubMenu = ::GetSubMenu( hSysMenu, nItemID );
		switch( nItemID )
		{
		case 0: // File menu
			break;
		case 1: // Synchronize menu
			::EnableMenuItem( hSubMenu, ID_SYNCHRONIZE_ALL, theApp.m_fConnected ? MF_BYCOMMAND | MF_ENABLED : MF_BYCOMMAND | MF_GRAYED );
			//::EnableMenuItem( hSubMenu, ID_SYNCHRONIZE_AUTO, theApp.m_fConnected ? MF_BYCOMMAND | MF_ENABLED : MF_BYCOMMAND | MF_GRAYED );
			break;
		case 2: // Xbox menu
			::EnableMenuItem( hSubMenu, ID_CONNECT, theApp.m_fConnected ? MF_BYCOMMAND | MF_GRAYED : MF_BYCOMMAND | MF_ENABLED );
			::EnableMenuItem( hSubMenu, ID_DISCONNECT, theApp.m_fConnected ? MF_BYCOMMAND | MF_ENABLED : MF_BYCOMMAND | MF_GRAYED );
			::EnableMenuItem( hSubMenu, ID_XBOX_PANIC, theApp.m_fConnected ? MF_BYCOMMAND | MF_ENABLED : MF_BYCOMMAND | MF_GRAYED );
			break;
		case 3: // Help menu
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
}

void CXboxAddinDlg::OnAppAbout() 
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CXboxAddinDlg::OnConnectionStateChanged( void )
{
	m_pLeftPaneDlg->OnConnectionStateChanged();
	m_pRightPaneDlg->OnConnectionStateChanged();
}

bool CXboxAddinDlg::IsNodeDisplayed( const IDMUSProdNode *pIDMUSProdNode )
{
	return m_pLeftPaneDlg->IsNodeDisplayed( pIDMUSProdNode ) || m_pRightPaneDlg->IsNodeDisplayed( pIDMUSProdNode );
}

HRESULT CXboxAddinDlg::AddNodeToDisplay( IDMUSProdNode *pIDMUSProdNode )
{
	// Get Node's GUID
	GUID guidID;
	HRESULT hr = pIDMUSProdNode->GetNodeId( &guidID );

	if( SUCCEEDED(hr) )
	{
		if( GUID_SegmentNode == guidID )
		{
			return m_pLeftPaneDlg->AddNodeToDisplay( pIDMUSProdNode );
		}
		else
		{
			return m_pRightPaneDlg->AddNodeToDisplay( pIDMUSProdNode );
		}
	}
	return hr;
}

void CXboxAddinDlg::OnXboxPanic() 
{
    char    szResp[MAX_PATH];
    DWORD   cchResp = MAX_PATH;
	DmSendCommand(theApp.m_pdmConnection, TEXT(CMD_PREFIX "!panic"), szResp, &cchResp);

	// Wait for the panic command to process
	theApp.WaitForNotification(	NOTIFICATION_PANIC_RESULT );

	m_pLeftPaneDlg->OnPanic();
}

void CXboxAddinDlg::HandleNotification( NOTIFICATION_TYPE notificationType, DWORD dwSegmentID, DWORD dwData1 )
{
	m_pLeftPaneDlg->HandleNotification( notificationType, dwSegmentID, dwData1 );
	//m_pRightPaneDlg->HandleNotification( notificationType, dwSegmentID, dwData1 );
}

LRESULT CXboxAddinDlg::OnApp( WPARAM wParam, LPARAM lParam )
{
	if( wParam == 0 && lParam == 0 )
	{
		Notification_Struct *pNotification_Struct;
		
		do
		{
			// Find a segment notification
			pNotification_Struct = theApp.GetNotification( NOTIFICATION_SEGMENT );

			// If we found a segment notification
			if( pNotification_Struct )
			{
				HandleNotification( pNotification_Struct->notificationType, pNotification_Struct->dwSegmentID, pNotification_Struct->dwData1 );
				delete pNotification_Struct;
			}
		}
		while( pNotification_Struct );
	}

	return 0;
}

void CXboxAddinDlg::CleanUpDisplay( void )
{
	m_pRightPaneDlg->CleanUpDisplay();
}

bool CXboxAddinDlg::IsFileInUse( CFileItem *pFileItem )
{
	return m_pLeftPaneDlg->IsFileInUse( pFileItem ) || m_pRightPaneDlg->IsFileInUse( pFileItem );
}

void CXboxAddinDlg::OnSetXboxName() 
{
	CTargetXboxDlg targetXboxDlg;
	if( IDOK == targetXboxDlg.DoModal()
	&&	theApp.m_fConnected )
	{
		// Disconnect and reconnect
		theApp.DisconnectFromXbox();
		theApp.ConnectToXbox();
	}
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Get version information
	TCHAR achJazzExeName[FILENAME_MAX + 1];
	TCHAR achFileVersion[FILENAME_MAX];

	if( GetModuleFileName ( theApp.m_hInstance, achJazzExeName, FILENAME_MAX ) )
	{
		if( GetFileVersion( achJazzExeName, achFileVersion, FILENAME_MAX ) )
		{
			CString strFileVersion;

			AfxFormatString1( strFileVersion, IDS_XBOX_ADDIN_VERSION, achFileVersion );
			SetDlgItemText( IDC_FILE_VERSION, strFileVersion );
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAboutDlg::GetFileVersion( LPTSTR szExeName, LPTSTR szFileVersion, short nFileVersionSize )
{
	DWORD dwBufferSize;
	DWORD dwReserved;
	
	dwBufferSize = GetFileVersionInfoSize( szExeName, &dwReserved );
	if( dwBufferSize > 0 )
	{
		void* pBuffer;

		pBuffer = (void *)malloc( dwBufferSize );
		if( pBuffer )
		{
			CString strTheFileVersion;
			VS_FIXEDFILEINFO* pFixedInfo;
			UINT nInfoSize;

			GetFileVersionInfo( szExeName, dwReserved, dwBufferSize, pBuffer );
			VerQueryValue( pBuffer, _T("\\"), (void **)&pFixedInfo, &nInfoSize );

			WORD wVer1 = HIWORD( pFixedInfo->dwFileVersionMS );
			WORD wVer2 = LOWORD( pFixedInfo->dwFileVersionMS );
			WORD wVer3 = HIWORD( pFixedInfo->dwFileVersionLS );
			WORD wVer4 = LOWORD( pFixedInfo->dwFileVersionLS );

			strTheFileVersion.Format( _T("%u.%u.%u.%u"), wVer1, wVer2, wVer3, wVer4 );

			_tcsncpy( szFileVersion, strTheFileVersion, nFileVersionSize );

			free( pBuffer );
			return TRUE;
		}
	}

	return FALSE;
}
