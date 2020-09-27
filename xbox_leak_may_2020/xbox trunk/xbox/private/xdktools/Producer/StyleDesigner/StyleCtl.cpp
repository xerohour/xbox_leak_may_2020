// StyleCtl.cpp : Implementation of the CStyleCtrl ActiveX Control class.

#include "stdafx.h"
#include "StyleDesignerDLL.h"
#include "Style.h"
#include "StyleCtl.h"
#include "StyleRef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CStyleCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CStyleCtrl, COleControl)
	//{{AFX_MSG_MAP(CStyleCtrl)
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
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(IDM_HELP_FINDER, OnHelpFinder)
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_MESSAGE(WM_SYNC_STYLEEDITOR, OnSyncStyleEditor)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CStyleCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CStyleCtrl)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CStyleCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CStyleCtrl, COleControl)
	//{{AFX_EVENT_MAP(CStyleCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl Interface map

BEGIN_INTERFACE_MAP(CStyleCtrl, COleControl)
    INTERFACE_PART(CStyleCtrl, IID_IOleInPlaceActiveObject, MyOleInPlaceActiveObject)
    INTERFACE_PART(CStyleCtrl, IID_IDMUSProdEditor, Editor)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CStyleCtrl, "STYLEDESIGNER.StyleCtrl.1",
	0x3bd2ba05, 0x46e7, 0x11d0, 0x89, 0xac, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CStyleCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DStyle =
		{ 0x3bd2ba03, 0x46e7, 0x11d0, { 0x89, 0xac, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };
const IID BASED_CODE IID_DStyleEvents =
		{ 0x3bd2ba04, 0x46e7, 0x11d0, { 0x89, 0xac, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwStyleOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CStyleCtrl, IDS_STYLE, _dwStyleOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::CStyleCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CStyleCtrl

BOOL CStyleCtrl::CStyleCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_STYLE,
			IDB_STYLE,
			afxRegApartmentThreading,
			_dwStyleOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::CStyleCtrl - Constructor

CStyleCtrl::CStyleCtrl()
{
/////////////////////////////////////////////////////////////////////////
// Following commented out to prevent 3 ASSERTS that occur
// because we are not building a typelib
//	InitializeIIDs(&IID_DStyle, &IID_DStyleEvents);
//
// Following code copied from InitializeIIDs (since we cannot override)
	m_piidPrimary = &IID_DStyle;
	m_piidEvents = &IID_DStyleEvents;
	EnableTypeLib();	// Needed to prevent ASSERT when closing editor
	InitStockEventMask();
	InitStockPropMask();
//////////////////////////////////////////////////////////////////////////

	m_pStyleDlg = NULL;
	m_hWndContainer = NULL;
	m_hMenuInPlace = NULL;
	m_pToolBar = NULL;
	m_pStyle = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::~CStyleCtrl - Destructor

CStyleCtrl::~CStyleCtrl()
{
	RELEASE( m_pStyle );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl IDMUSProdEditor implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::XEditor::AddRef

STDMETHODIMP_(ULONG) CStyleCtrl::XEditor::AddRef()
{
	METHOD_PROLOGUE_EX_( CStyleCtrl, Editor )

	return (ULONG)pThis->ExternalAddRef();
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::XEditor::Release

STDMETHODIMP_(ULONG) CStyleCtrl::XEditor::Release()
{
	METHOD_PROLOGUE_EX_( CStyleCtrl, Editor )

	return (ULONG)pThis->ExternalRelease();
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::XEditor::QueryInterface

STDMETHODIMP CStyleCtrl::XEditor::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_PROLOGUE_EX_( CStyleCtrl, Editor )

	return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::XEditor::AttachObjects implementation

HRESULT CStyleCtrl::XEditor::AttachObjects( IDMUSProdNode* pINode )
{
	METHOD_MANAGE_STATE( CStyleCtrl, Editor )

	ASSERT_VALID( pThis );
	ASSERT( pINode != NULL );

	GUID guidNode;

	pINode->GetNodeId( &guidNode );
	if( ::IsEqualGUID( guidNode, GUID_StyleRefNode ) )
	{
		CStyleRef* pStyleRef = (CStyleRef *)pINode;
		ASSERT( pStyleRef->m_pStyle != NULL );
		pThis->m_pStyle = pStyleRef->m_pStyle;
	}
	else
	{
		pThis->m_pStyle = (CDirectMusicStyle *)pINode;
	}

	ASSERT( pThis->m_pStyle != NULL );
	pThis->m_pStyle->AddRef();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::XEditor::OnInitMenuFilePrint

HRESULT CStyleCtrl::XEditor::OnInitMenuFilePrint( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CStyleCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::XEditor::OnFilePrint

HRESULT CStyleCtrl::XEditor::OnFilePrint( void )
{
	METHOD_MANAGE_STATE( CStyleCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::XEditor::OnInitMenuFilePrintPreview

HRESULT CStyleCtrl::XEditor::OnInitMenuFilePrintPreview( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CStyleCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::XEditor::OnFilePrintPreview

HRESULT CStyleCtrl::XEditor::OnFilePrintPreview( void )
{
	METHOD_MANAGE_STATE( CStyleCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::XEditor::OnViewProperties

HRESULT CStyleCtrl::XEditor::OnViewProperties( void )
{
	METHOD_MANAGE_STATE( CStyleCtrl, Editor )

	HRESULT hr = E_FAIL;

	if( pThis->m_pStyleDlg )
	{
		if( pThis->m_pStyleDlg->OnViewProperties() )
		{
			hr = S_OK;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::XEditor::OnF1Help

HRESULT CStyleCtrl::XEditor::OnF1Help( void )
{
	METHOD_MANAGE_STATE( CStyleCtrl, Editor )
	
    // Determine name of DMUSProd.exe help file
	CString strHelpFileName;

	if( theApp.GetHelpFileName( strHelpFileName ) )
	{
		strHelpFileName += "::/htm/StyleDesignerWindow.htm";
		::HtmlHelp( NULL, strHelpFileName, HH_DISPLAY_TOPIC, 0 );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::XMyOleInPlaceActiveObject implementation

STDMETHODIMP_(ULONG) CStyleCtrl::XMyOleInPlaceActiveObject::AddRef()
{
	METHOD_MANAGE_STATE( CStyleCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.AddRef();
}

STDMETHODIMP_(ULONG) CStyleCtrl::XMyOleInPlaceActiveObject::Release()
{
	METHOD_MANAGE_STATE( CStyleCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.Release();
}

STDMETHODIMP CStyleCtrl::XMyOleInPlaceActiveObject::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_MANAGE_STATE( CStyleCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.QueryInterface( iid, ppvObj );
}

STDMETHODIMP CStyleCtrl::XMyOleInPlaceActiveObject::GetWindow( HWND* lphwnd )
{
	METHOD_MANAGE_STATE( CStyleCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.GetWindow( lphwnd );
}

STDMETHODIMP CStyleCtrl::XMyOleInPlaceActiveObject::ContextSensitiveHelp( BOOL fEnterMode )
{
	METHOD_MANAGE_STATE( CStyleCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.ContextSensitiveHelp( fEnterMode );
}

STDMETHODIMP CStyleCtrl::XMyOleInPlaceActiveObject::TranslateAccelerator( LPMSG lpmsg )
{
	METHOD_MANAGE_STATE( CStyleCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );
	
//	OLEINPLACEFRAMEINFO frameInfo = pThis->m_frameInfo;
//	if( ::OleTranslateAccelerator(pThis->m_pInPlaceFrame, &frameInfo, lpmsg) == S_OK )
//	{
//		return S_OK;
//	}

	return pThis->m_xOleInPlaceActiveObject.TranslateAccelerator( lpmsg );
}

STDMETHODIMP CStyleCtrl::XMyOleInPlaceActiveObject::OnFrameWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CStyleCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnFrameWindowActivate( fActivate );
}

STDMETHODIMP CStyleCtrl::XMyOleInPlaceActiveObject::OnDocWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CStyleCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnDocWindowActivate( fActivate );
}

STDMETHODIMP CStyleCtrl::XMyOleInPlaceActiveObject::ResizeBorder(
	LPCRECT, LPOLEINPLACEUIWINDOW, BOOL fFrameWindow )
{
	METHOD_MANAGE_STATE( CStyleCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

    if( fFrameWindow == TRUE )
	{
		pThis->OnShowToolBars();
	}

	return S_OK;
}

STDMETHODIMP CStyleCtrl::XMyOleInPlaceActiveObject::EnableModeless( BOOL fEnable )
{
	METHOD_MANAGE_STATE( CStyleCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.EnableModeless( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnDraw - Drawing function

void CStyleCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	UNREFERENCED_PARAMETER(pdc);
	UNREFERENCED_PARAMETER(rcBounds);
	UNREFERENCED_PARAMETER(rcInvalid);
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::DoPropExchange - Persistence support

void CStyleCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CStyleAbout dialog used for Style Editor About Box

class CStyleAbout : public CDialog
{
public:
	CStyleAbout();

// Dialog Data
	//{{AFX_DATA(CStyleAbout)
	enum { IDD = IDD_ABOUTBOX_STYLE };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStyleAbout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CStyleAbout)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CStyleAbout::CStyleAbout() : CDialog(CStyleAbout::IDD)
{
	//{{AFX_DATA_INIT(CStyleAbout)
	//}}AFX_DATA_INIT
}

void CStyleAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStyleAbout)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStyleAbout, CDialog)
	//{{AFX_MSG_MAP(CStyleAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CStyleAbout::OnInitDialog() 
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

			AfxFormatString1( strFileVersion, IDS_STYLE_VERSION_TEXT, achFileVersion );
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
// CStyleCtrl::AboutBox - Display an "About" box to the user

void CStyleCtrl::AboutBox()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CStyleAbout dlgAbout;
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl message handlers

/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnCreate

int CStyleCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if( COleControl::OnCreate(lpCreateStruct) == -1 )
		return -1;

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
	m_hMenuInPlace = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDR_STYLE_EDITOR) );

	// Create control's toolbar
//	CWnd* pWndParent = CWnd::FromHandle( m_hWndContainer );
//	if( pWndParent )
//	{
//		m_pToolBar = new CToolBar;
//		m_pToolBar->Create( pWndParent );
//		m_pToolBar->LoadToolBar( IDR_STYLE_EDITOR );
//		m_pToolBar->SetBarStyle( m_pToolBar->GetBarStyle() | CBRS_SIZE_DYNAMIC );
//		m_pToolBar->SetOwner( this );
//	}

	// Create control's dialog
	m_pStyleDlg = new CStyleDlg;
	if( m_pStyleDlg == NULL )
	{
		return -1;
	}

	m_pStyle->m_pStyleCtrl = this;

	m_pStyleDlg->m_pStyle = m_pStyle;
	m_pStyleDlg->m_pStyleCtrl = this;
	m_pStyleDlg->Create( NULL, "WindowName", WS_CHILD | WS_VISIBLE, CRect( 0, 0, 800, 400 ), this, 888, NULL );
	m_pStyleDlg->OnInitialUpdate();
	
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnDestroy

void CStyleCtrl::OnDestroy() 
{
	COleControl::OnDestroy();

	if( m_hMenuInPlace )
	{
		::DestroyMenu( m_hMenuInPlace );
		m_hMenuInPlace = NULL;
	}

	if( m_pToolBar )
	{
		delete m_pToolBar;	
		m_pToolBar = NULL;
	}

	ASSERT( m_pStyle != NULL );

	m_pStyle->m_pStyleCtrl = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnGetInPlaceMenu

HMENU CStyleCtrl::OnGetInPlaceMenu() 
{
	return m_hMenuInPlace;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnHideToolBars

void CStyleCtrl::OnHideToolBars() 
{
	COleControl::OnHideToolBars();
	return;

	if( m_pToolBar )
	{
		BORDERWIDTHS border;

		border.top = 0;
		border.left = 0;
		border.right = 0;
		border.bottom = 0;

		if( SUCCEEDED ( m_pInPlaceFrame->RequestBorderSpace(&border) ) )
		{
			m_pInPlaceFrame->SetBorderSpace( &border );
			m_pToolBar->ShowWindow( SW_HIDE );
			return;
		}
	}

	m_pInPlaceFrame->SetBorderSpace( NULL );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnShowToolBars

void CStyleCtrl::OnShowToolBars() 
{
	BORDERWIDTHS border;
	CRect rectBorder;

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
	return;

	// Show the ToolBar
	if( m_pToolBar )
	{
		if( SUCCEEDED ( m_pInPlaceFrame->GetBorder(&rectBorder) ) )
		{
			border.top = TOOLBAR_HEIGHT;
			border.left = 0;
			border.right = 0;
			border.bottom = 0;

			if( SUCCEEDED ( m_pInPlaceFrame->RequestBorderSpace(&border) ) )
			{
				m_pInPlaceFrame->SetBorderSpace( &border );

				m_pToolBar->MoveWindow( rectBorder.left,
										rectBorder.top,
										rectBorder.Width(),
										TOOLBAR_HEIGHT );
				m_pToolBar->ShowWindow( SW_SHOWNORMAL );
				return;
			}
		}
	}

	m_pInPlaceFrame->SetBorderSpace( NULL );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnSize

void CStyleCtrl::OnSize( UINT nType, int cx, int cy ) 
{
	COleControl::OnSize( nType, cx, cy );

	m_pStyleDlg->MoveWindow( 0, 0, cx, cy );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnUpdateEditUndo

void CStyleCtrl::OnUpdateEditUndo( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	CString strUndo;
	CString strUndoAccel;

	strUndo.LoadString( IDS_UNDO );
	strUndoAccel.LoadString( IDS_UNDO_ACCEL );

	BOOL fEnable = FALSE;

	if( m_pStyle )
	{
		TCHAR achText[MAX_BUFFER];

		ASSERT( m_pStyle->m_pUndoMgr != NULL );

		if( m_pStyle->m_pUndoMgr )
		{
			if( m_pStyle->m_pUndoMgr->GetUndo(achText, MAX_BUFFER) )
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
// CStyleCtrl::OnEditUndo

void CStyleCtrl::OnEditUndo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	if( m_pStyle )
	{
		ASSERT( m_pStyle->m_pUndoMgr != NULL );

		if( m_pStyle->m_pUndoMgr )
		{
			TCHAR achText[MAX_BUFFER];

			if( m_pStyle->m_pUndoMgr->GetUndo(achText, MAX_BUFFER) )
			{
				m_pStyle->m_pUndoMgr->Undo( m_pStyle );
				m_pStyle->SetModified( TRUE );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnUpdateEditRedo

void CStyleCtrl::OnUpdateEditRedo( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	CString strRedo;
	CString strRedoAccel;

	strRedo.LoadString( IDS_REDO );
	strRedoAccel.LoadString( IDS_REDO_ACCEL );

	BOOL fEnable = FALSE;

	if( m_pStyle )
	{
		TCHAR achText[MAX_BUFFER];

		ASSERT( m_pStyle->m_pUndoMgr != NULL );

		if( m_pStyle->m_pUndoMgr )
		{
			if( m_pStyle->m_pUndoMgr->GetRedo(achText, MAX_BUFFER) )
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
// CStyleCtrl::OnEditRedo

void CStyleCtrl::OnEditRedo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	if( m_pStyle )
	{
		ASSERT( m_pStyle->m_pUndoMgr != NULL );

		if( m_pStyle->m_pUndoMgr )
		{
			TCHAR achText[MAX_BUFFER];

			if( m_pStyle->m_pUndoMgr->GetRedo(achText, MAX_BUFFER) )
			{
				m_pStyle->m_pUndoMgr->Redo( m_pStyle );
				m_pStyle->SetModified( TRUE );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnUpdateEditCut

void CStyleCtrl::OnUpdateEditCut( CCmdUI* pCmdUI ) 
{
	if( m_pStyleDlg )
	{
		m_pStyleDlg->OnUpdateEditCut( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnEditCut

void CStyleCtrl::OnEditCut() 
{
	if( m_pStyleDlg )
	{
		m_pStyleDlg->OnEditCut();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnUpdateEditCopy

void CStyleCtrl::OnUpdateEditCopy( CCmdUI* pCmdUI ) 
{
	if( m_pStyleDlg )
	{
		m_pStyleDlg->OnUpdateEditCopy( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnEditCopy

void CStyleCtrl::OnEditCopy() 
{
	if( m_pStyleDlg )
	{
		m_pStyleDlg->OnEditCopy();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnUpdateEditPaste

void CStyleCtrl::OnUpdateEditPaste( CCmdUI* pCmdUI ) 
{
	if( m_pStyleDlg )
	{
		m_pStyleDlg->OnUpdateEditPaste( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnEditPaste

void CStyleCtrl::OnEditPaste() 
{
	if( m_pStyleDlg )
	{
		m_pStyleDlg->OnEditPaste();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnUpdateEditInsert

void CStyleCtrl::OnUpdateEditInsert( CCmdUI* pCmdUI ) 
{
	if( m_pStyleDlg )
	{
		m_pStyleDlg->OnUpdateEditInsert( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnEditInsert

void CStyleCtrl::OnEditInsert() 
{
	if( m_pStyleDlg )
	{
		m_pStyleDlg->OnEditInsert();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnUpdateEditDelete

void CStyleCtrl::OnUpdateEditDelete( CCmdUI* pCmdUI ) 
{
	if( m_pStyleDlg )
	{
		m_pStyleDlg->OnUpdateEditDelete( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnEditDelete

void CStyleCtrl::OnEditDelete() 
{
	if( m_pStyleDlg )
	{
		m_pStyleDlg->OnEditDelete();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnSyncStyleEditor

LRESULT CStyleCtrl::OnSyncStyleEditor( WPARAM wParam, LPARAM lParam )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(lParam);

	ASSERT( m_pStyle != NULL );

	m_pStyle->SyncStyleEditor( wParam );

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::PreTranslateMessage

BOOL CStyleCtrl::PreTranslateMessage( MSG* pMsg ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( pMsg->message )
	{
		case WM_KEYDOWN:
			if( pMsg->lParam & 0x40000000 )
			{
				break;
			}

			switch( pMsg->wParam )
			{
				case 0x59:	// VK_Y		(Redo)
					if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
					{
						OnEditRedo();
						return TRUE;
					}
					break;

				case 0x5A:	// VK_Z		(Undo)
					if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
					{
						OnEditUndo();
						return TRUE;
					}
					break;

				default:
					if( m_pStyleDlg )
					{
						if( m_pStyleDlg->HandleKeyDown( pMsg ) )
						{
							return TRUE;
						}
					}
					break;
			}
			break;
	}

	if( IsDialogMessage( pMsg ) )
	{
		return TRUE;
	}
	
	return COleControl::PreTranslateMessage( pMsg );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl::OnHelpFinder

void CStyleCtrl::OnHelpFinder() 
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
