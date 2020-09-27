// SongCtl.cpp : Implementation of the CSongCtrl ActiveX Control class.

#include "stdafx.h"
#include "SongDesignerDLL.h"
#include "Song.h"
#include "SongCtl.h"
#include "SongRef.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CSongCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CSongCtrl, COleControl)
	//{{AFX_MSG_MAP(CSongCtrl)
	ON_WM_CREATE()
	ON_COMMAND(ID_APP_ABOUT, AboutBox)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERT, OnUpdateEditInsert)
	ON_COMMAND(ID_EDIT_INSERT, OnEditInsert)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(IDM_HELP_FINDER, OnHelpFinder)
	ON_WM_PARENTNOTIFY()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEACTIVATE()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CSongCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CSongCtrl)
	// NOTE - ClassWizard will add and remove dispatch map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CSongCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CSongCtrl, COleControl)
	//{{AFX_EVENT_MAP(CSongCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl Interface map

BEGIN_INTERFACE_MAP(CSongCtrl, COleControl)
    INTERFACE_PART(CSongCtrl, IID_IOleInPlaceActiveObject, MyOleInPlaceActiveObject)
    INTERFACE_PART(CSongCtrl, IID_IDMUSProdEditor, Editor)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CSongCtrl, "SONGDESIGNER.SongCtrl.1",
	0x35327daf, 0x7151, 0x43da, 0xaf, 0x9b, 0x57, 0x9d, 0x8a, 0x20, 0x41, 0x03)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CSongCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DSong =
		{ 0x9cb3a837, 0xb566, 0x4b17, { 0x8a, 0x9b, 0x1f, 0x9a, 0x19, 0xd9, 0x9c, 0xe2 } };
const IID BASED_CODE IID_DSongEvents =
		{ 0x958130e0, 0x9d27, 0x4ecf, { 0x9d, 0xe2, 0x6a, 0x38, 0xed, 0x58, 0xae, 0xdd } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwSongOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CSongCtrl, IDS_SONG, _dwSongOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::CSongCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CSongCtrl

BOOL CSongCtrl::CSongCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_SONG,
			IDB_SONG,
			afxRegApartmentThreading,
			_dwSongOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::CSongCtrl - Constructor

CSongCtrl::CSongCtrl()
{
/////////////////////////////////////////////////////////////////////////
// Following commented out to prevent 3 ASSERTS that occur
// because we are not building a typelib
//	InitializeIIDs(&IID_DSong, &IID_DSongEvents);
//
// Following code copied from InitializeIIDs (since we cannot override)
	m_piidPrimary = &IID_DSong;
	m_piidEvents = &IID_DSongEvents;
	EnableTypeLib();	// Needed to prevent ASSERT when closing editor
	InitStockEventMask();
	InitStockPropMask();
//////////////////////////////////////////////////////////////////////////

	m_pSongDlg = NULL;
	m_hWndContainer = NULL;
	m_hMenuInPlace = NULL;
	m_hAcceleratorTable	= NULL;
	m_pSong = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::~CSongCtrl - Destructor

CSongCtrl::~CSongCtrl()
{
	RELEASE( m_pSong );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSongCtrl IDMUSProdEditor implementation

/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::XEditor::AddRef

STDMETHODIMP_(ULONG) CSongCtrl::XEditor::AddRef()
{
	METHOD_PROLOGUE_EX_( CSongCtrl, Editor )

	return (ULONG)pThis->ExternalAddRef();
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::XEditor::Release

STDMETHODIMP_(ULONG) CSongCtrl::XEditor::Release()
{
	METHOD_PROLOGUE_EX_( CSongCtrl, Editor )

	return (ULONG)pThis->ExternalRelease();
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::XEditor::QueryInterface

STDMETHODIMP CSongCtrl::XEditor::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_PROLOGUE_EX_( CSongCtrl, Editor )

	return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::XEditor::AttachObjects implementation

HRESULT CSongCtrl::XEditor::AttachObjects( IDMUSProdNode* pINode )
{
	METHOD_MANAGE_STATE( CSongCtrl, Editor )

	ASSERT_VALID( pThis );
	ASSERT( pINode != NULL );

	GUID guidNode;

	pINode->GetNodeId( &guidNode );
	if( ::IsEqualGUID( guidNode, GUID_SongRefNode ) )
	{
		CSongRef* pSongRef = (CSongRef *)pINode;
		ASSERT( pSongRef->m_pSong != NULL );
		pThis->m_pSong = pSongRef->m_pSong;
	}
	else
	{
		pThis->m_pSong = (CDirectMusicSong *)pINode;
	}

	ASSERT( pThis->m_pSong != NULL );
	pThis->m_pSong->AddRef();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::XEditor::OnInitMenuFilePrint

HRESULT CSongCtrl::XEditor::OnInitMenuFilePrint( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CSongCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::XEditor::OnFilePrint

HRESULT CSongCtrl::XEditor::OnFilePrint( void )
{
	METHOD_MANAGE_STATE( CSongCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::XEditor::OnInitMenuFilePrintPreview

HRESULT CSongCtrl::XEditor::OnInitMenuFilePrintPreview( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CSongCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::XEditor::OnFilePrintPreview

HRESULT CSongCtrl::XEditor::OnFilePrintPreview( void )
{
	METHOD_MANAGE_STATE( CSongCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::XEditor::OnViewProperties

HRESULT CSongCtrl::XEditor::OnViewProperties( void )
{
	METHOD_MANAGE_STATE( CSongCtrl, Editor )

	HRESULT hr = E_FAIL;

	if( pThis->m_pSongDlg )
	{
		if( pThis->m_pSongDlg->OnViewProperties() )
		{
			hr = S_OK;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::XEditor::OnF1Help

HRESULT CSongCtrl::XEditor::OnF1Help( void )
{
	METHOD_MANAGE_STATE( CSongCtrl, Editor )
	
    // Determine name of DMUSProd.exe help file
	CString strHelpFileName;

	if( theApp.GetHelpFileName( strHelpFileName ) )
	{
		strHelpFileName += "::/htm/SongDesignerWindow.htm";
		::HtmlHelp( NULL, strHelpFileName, HH_DISPLAY_TOPIC, 0 );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::XMyOleInPlaceActiveObject implementation

STDMETHODIMP_(ULONG) CSongCtrl::XMyOleInPlaceActiveObject::AddRef()
{
	METHOD_MANAGE_STATE( CSongCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.AddRef();
}

STDMETHODIMP_(ULONG) CSongCtrl::XMyOleInPlaceActiveObject::Release()
{
	METHOD_MANAGE_STATE( CSongCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.Release();
}

STDMETHODIMP CSongCtrl::XMyOleInPlaceActiveObject::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_MANAGE_STATE( CSongCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.QueryInterface( iid, ppvObj );
}

STDMETHODIMP CSongCtrl::XMyOleInPlaceActiveObject::GetWindow( HWND* lphwnd )
{
	METHOD_MANAGE_STATE( CSongCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.GetWindow( lphwnd );
}

STDMETHODIMP CSongCtrl::XMyOleInPlaceActiveObject::ContextSensitiveHelp( BOOL fEnterMode )
{
	METHOD_MANAGE_STATE( CSongCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.ContextSensitiveHelp( fEnterMode );
}

STDMETHODIMP CSongCtrl::XMyOleInPlaceActiveObject::TranslateAccelerator( LPMSG lpmsg )
{
	METHOD_MANAGE_STATE( CSongCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	if( pThis->m_pSongDlg )
	{
		if( (lpmsg->message == WM_KEYDOWN && lpmsg->wParam == 0x59) 
		||  (lpmsg->message == WM_KEYDOWN && lpmsg->wParam == 0x5A) )  
		{
			// Translate CTRL + Y when any control in the editor has focus
			// Translate CTRL + Z when any control in the editor has focus
			CWnd* pWnd = pThis->GetFocus();
			if( pThis == pWnd
			||  pThis->IsChild(pWnd) ) 
			{
				if( ::TranslateAccelerator(pThis->m_hWnd, pThis->m_hAcceleratorTable, lpmsg) )
				{
					return S_OK;
				}
			}
		}

		if( lpmsg->message == WM_KEYDOWN && lpmsg->wParam == 0xDE ) 
		{
			// Translate CTRL + SHIFT + " when any control in the editor has focus
			if( (GetKeyState(VK_CONTROL) & 0x8000)
			&&	(GetKeyState(VK_SHIFT) & 0x8000) )
			{
				CWnd* pWnd = pThis->GetFocus();
				if( pThis == pWnd
				||  pThis->IsChild(pWnd) ) 
				{
					if( pThis->m_pSong )
					{
						pThis->m_pSong->TransitionWithinSong();
						return S_OK;
					}
				}
			}
		}

		// Everything else
		else
		{
			// Translate everything else only when one of the listbox controls has focus
			if( ::IsWindow(pThis->m_pSongDlg->m_lstbxSegments.m_hWnd) )
			{
				HWND hwndFocus = ::GetFocus();
				if( pThis->m_pSongDlg->m_lstbxSegments.m_hWnd == hwndFocus 
				||  pThis->m_pSongDlg->m_lstbxTracks.m_hWnd == hwndFocus 
				||  pThis->m_pSongDlg->m_lstbxTransitions.m_hWnd == hwndFocus )
				{
					if( ::TranslateAccelerator(pThis->m_hWnd, pThis->m_hAcceleratorTable, lpmsg) )
					{
						return S_OK;
					}
				}
			}
		}
	}

//	OLEINPLACEFRAMEINFO frameInfo = pThis->m_frameInfo;
//	if( ::OleTranslateAccelerator(pThis->m_pInPlaceFrame, &frameInfo, lpmsg) == S_OK )
//	{
//		return S_OK;
//	}

	return pThis->m_xOleInPlaceActiveObject.TranslateAccelerator( lpmsg );
}

STDMETHODIMP CSongCtrl::XMyOleInPlaceActiveObject::OnFrameWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CSongCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnFrameWindowActivate( fActivate );
}

STDMETHODIMP CSongCtrl::XMyOleInPlaceActiveObject::OnDocWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CSongCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnDocWindowActivate( fActivate );
}

STDMETHODIMP CSongCtrl::XMyOleInPlaceActiveObject::ResizeBorder(
	LPCRECT, LPOLEINPLACEUIWINDOW, BOOL fFrameWindow )
{
	METHOD_MANAGE_STATE( CSongCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

    if( fFrameWindow == TRUE )
	{
		pThis->OnShowToolBars();
	}

	return S_OK;
}

STDMETHODIMP CSongCtrl::XMyOleInPlaceActiveObject::EnableModeless( BOOL fEnable )
{
	METHOD_MANAGE_STATE( CSongCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.EnableModeless( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnDraw - Drawing function

void CSongCtrl::OnDraw( CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid )
{
	UNREFERENCED_PARAMETER(pdc);
	UNREFERENCED_PARAMETER(rcBounds);
	UNREFERENCED_PARAMETER(rcInvalid);
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::DoPropExchange - Persistence support

void CSongCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnResetState - Reset control to default state

void CSongCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSongAbout dialog used for Song Editor About Box

class CSongAbout : public CDialog
{
public:
	CSongAbout();

// Dialog Data
	//{{AFX_DATA(CSongAbout)
	enum { IDD = IDD_ABOUTBOX_SONG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSongAbout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSongAbout)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CSongAbout::CSongAbout() : CDialog(CSongAbout::IDD)
{
	//{{AFX_DATA_INIT(CSongAbout)
	//}}AFX_DATA_INIT
}

void CSongAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSongAbout)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSongAbout, CDialog)
	//{{AFX_MSG_MAP(CSongAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CSongAbout::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Get version information
	TCHAR achExeName[FILENAME_MAX + 1];
	TCHAR achFileVersion[MID_BUFFER];

	if( GetModuleFileName ( theApp.m_hInstance, achExeName, FILENAME_MAX ) )
	{
		if( theApp.GetFileVersion( achExeName, achFileVersion, MID_BUFFER ) )
		{
			CString strFileVersion;

			AfxFormatString1( strFileVersion, IDS_SONG_VERSION_TEXT, achFileVersion );
			SetDlgItemText( IDC_FILE_VERSION, strFileVersion );
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::AboutBox - Display an "About" box to the user

void CSongCtrl::AboutBox()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CSongAbout dlgAbout;
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl message handlers

/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnHideToolBars

void CSongCtrl::OnHideToolBars() 
{
	COleControl::OnHideToolBars();
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnShowToolBars

void CSongCtrl::OnShowToolBars() 
{
	if( m_pInPlaceFrame == NULL )
	{
		return;
	}

	m_pInPlaceFrame->SetActiveObject( &m_xMyOleInPlaceActiveObject, NULL );

	if( m_pInPlaceDoc != NULL )
	{
		m_pInPlaceDoc->SetActiveObject( &m_xMyOleInPlaceActiveObject, NULL );
	}

	COleControl::OnShowToolBars();
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnGetInPlaceMenu

HMENU CSongCtrl::OnGetInPlaceMenu() 
{
	return m_hMenuInPlace;
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnCreate

int CSongCtrl::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( COleControl::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}

	if( m_pInPlaceFrame )
	{
		m_pInPlaceFrame->GetWindow( &m_hWndContainer );
	}
	else
	{
		// out of place active?
		if( m_bOpen )
		{
			m_hWndContainer = GetParent()->GetSafeHwnd();
		}
	}

	// Load control's in-place menu
	m_hMenuInPlace = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDR_SONG_EDITOR) );

	// Load control's accelerator table
	m_hAcceleratorTable = ::LoadAccelerators( theApp.m_hInstance, MAKEINTRESOURCE(IDR_SONG_ACCELS) );

	// Create control's dialog
	m_pSongDlg = new CSongDlg;
	if( m_pSongDlg == NULL )
	{
		return -1;
	}

	m_pSong->m_pSongCtrl = this;

	m_pSongDlg->m_pSong = m_pSong;
	m_pSongDlg->m_pSongCtrl = this;
	m_pSongDlg->Create( NULL, "WindowName", WS_CHILD | WS_VISIBLE, CRect( 0, 0, 800, 400 ), this, 888, NULL );
	m_pSongDlg->OnInitialUpdate();
	
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnDestroy

void CSongCtrl::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	COleControl::OnDestroy();

	if( m_hMenuInPlace )
	{
		::DestroyMenu( m_hMenuInPlace );
		m_hMenuInPlace = NULL;
	}

	if( m_hAcceleratorTable )
	{
		::DestroyAcceleratorTable( m_hAcceleratorTable );
		m_hAcceleratorTable = NULL;
	}

	ASSERT( m_pSong != NULL );

	m_pSong->m_pSongCtrl = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnSize

void CSongCtrl::OnSize( UINT nType, int cx, int cy ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	COleControl::OnSize( nType, cx, cy );

	m_pSongDlg->MoveWindow( 0, 0, cx, cy );
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnMouseActivate

int CSongCtrl::OnMouseActivate( CWnd* pDesktopWnd, UINT nHitTest, UINT message ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSong )
	{
		m_pSong->SetActiveTransport();
	}

	return COleControl::OnMouseActivate( pDesktopWnd, nHitTest, message );
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnSetFocus

void CSongCtrl::OnSetFocus( CWnd* pOldWnd ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	COleControl::OnSetFocus( pOldWnd );

	if( m_pSong )
	{
		m_pSong->SetActiveTransport();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnUpdateEditUndo

void CSongCtrl::OnUpdateEditUndo( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

	CString strUndo;
	CString strUndoAccel;

	strUndo.LoadString( IDS_UNDO );
	strUndoAccel.LoadString( IDS_UNDO_ACCEL );

	BOOL fEnable = FALSE;

	if( m_pSong )
	{
		TCHAR achText[MAX_BUFFER];

		ASSERT( m_pSong->m_pUndoMgr != NULL );

		if( m_pSong->m_pUndoMgr )
		{
			if( m_pSong->m_pUndoMgr->GetUndo(achText, MAX_BUFFER) )
			{
				if( *achText )
				{
					strUndo += " ";
					strUndo += achText;
				}

				fEnable = TRUE;
			}
		}
	}

	strUndo += strUndoAccel;
	pCmdUI->SetText( strUndo );
	pCmdUI->Enable( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnEditUndo

void CSongCtrl::OnEditUndo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

	if( m_pSong )
	{
		ASSERT( m_pSong->m_pUndoMgr != NULL );

		if( m_pSong->m_pUndoMgr )
		{
			TCHAR achText[MAX_BUFFER];

			if( m_pSong->m_pUndoMgr->GetUndo(achText, MAX_BUFFER) )
			{
				m_pSong->m_pUndoMgr->Undo( m_pSong );
				m_pSong->SetModified( TRUE );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnUpdateEditRedo

void CSongCtrl::OnUpdateEditRedo( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

	CString strRedo;
	CString strRedoAccel;

	strRedo.LoadString( IDS_REDO );
	strRedoAccel.LoadString( IDS_REDO_ACCEL );

	BOOL fEnable = FALSE;

	if( m_pSong )
	{
		TCHAR achText[MAX_BUFFER];

		ASSERT( m_pSong->m_pUndoMgr != NULL );

		if( m_pSong->m_pUndoMgr )
		{
			if( m_pSong->m_pUndoMgr->GetRedo(achText, MAX_BUFFER) )
			{
				if( *achText )
				{
					strRedo += " ";
					strRedo += achText;
				}

				fEnable = TRUE;
			}
		}
	}

	strRedo += strRedoAccel;
	pCmdUI->SetText( strRedo );
	pCmdUI->Enable( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnEditRedo

void CSongCtrl::OnEditRedo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

	if( m_pSong )
	{
		ASSERT( m_pSong->m_pUndoMgr != NULL );

		if( m_pSong->m_pUndoMgr )
		{
			TCHAR achText[MAX_BUFFER];

			if( m_pSong->m_pUndoMgr->GetRedo(achText, MAX_BUFFER) )
			{
				m_pSong->m_pUndoMgr->Redo( m_pSong );
				m_pSong->SetModified( TRUE );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnUpdateEditCut

void CSongCtrl::OnUpdateEditCut( CCmdUI* pCmdUI ) 
{
	if( m_pSongDlg )
	{
		m_pSongDlg->OnUpdateEditCut( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnEditCut

void CSongCtrl::OnEditCut() 
{
	if( m_pSongDlg )
	{
		m_pSongDlg->OnEditCut();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnUpdateEditCopy

void CSongCtrl::OnUpdateEditCopy( CCmdUI* pCmdUI ) 
{
	if( m_pSongDlg )
	{
		m_pSongDlg->OnUpdateEditCopy( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnEditCopy

void CSongCtrl::OnEditCopy() 
{
	if( m_pSongDlg )
	{
		m_pSongDlg->OnEditCopy();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnUpdateEditPaste

void CSongCtrl::OnUpdateEditPaste( CCmdUI* pCmdUI ) 
{
	if( m_pSongDlg )
	{
		m_pSongDlg->OnUpdateEditPaste( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnEditPaste

void CSongCtrl::OnEditPaste() 
{
	if( m_pSongDlg )
	{
		m_pSongDlg->OnEditPaste();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnUpdateEditInsert

void CSongCtrl::OnUpdateEditInsert( CCmdUI* pCmdUI ) 
{
	if( m_pSongDlg )
	{
		m_pSongDlg->OnUpdateEditInsert( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnEditInsert

void CSongCtrl::OnEditInsert() 
{
	if( m_pSongDlg )
	{
		m_pSongDlg->OnEditInsert();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnEditSelectAll

void CSongCtrl::OnEditSelectAll() 
{
	if( m_pSongDlg )
	{
		m_pSongDlg->OnEditSelectAll();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnUpdateEditDelete

void CSongCtrl::OnUpdateEditDelete( CCmdUI* pCmdUI ) 
{
	if( m_pSongDlg )
	{
		m_pSongDlg->OnUpdateEditDelete( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnEditDelete

void CSongCtrl::OnEditDelete() 
{
	if( m_pSongDlg )
	{
		m_pSongDlg->OnEditDelete();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::OnHelpFinder

void CSongCtrl::OnHelpFinder() 
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

   // Determine name of DMUSProd.exe help file
	CString strHelpFileName;

	if( theApp.GetHelpFileName( strHelpFileName ) )
	{
		strHelpFileName += "::/htm/directmusicproducer.htm";
		::HtmlHelp( NULL, strHelpFileName, HH_DISPLAY_TOPIC, 0 );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongCtrl::PreTranslateMessage

BOOL CSongCtrl::PreTranslateMessage( MSG* pMsg ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( IsDialogMessage( pMsg ) )
	{
		return TRUE;
	}
	
	return COleControl::PreTranslateMessage(pMsg);
}
