// GraphCtl.cpp : Implementation of the CGraphCtrl ActiveX Control class.

#include "stdafx.h"
#include "ToolGraphDesignerDLL.h"
#include "Graph.h"
#include "GraphCtl.h"
#include "GraphRef.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CGraphCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CGraphCtrl, COleControl)
	//{{AFX_MSG_MAP(CGraphCtrl)
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
	ON_COMMAND(IDM_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(IDM_HELP_FINDER, OnHelpFinder)
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CGraphCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CGraphCtrl)
	// NOTE - ClassWizard will add and remove dispatch map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CGraphCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CGraphCtrl, COleControl)
	//{{AFX_EVENT_MAP(CGraphCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl Interface map

BEGIN_INTERFACE_MAP(CGraphCtrl, COleControl)
    INTERFACE_PART(CGraphCtrl, IID_IOleInPlaceActiveObject, MyOleInPlaceActiveObject)
    INTERFACE_PART(CGraphCtrl, IID_IDMUSProdEditor, Editor)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CGraphCtrl, "TOOLGRAPHDESIGNER.GraphCtrl.1",
	0x7A6D839D, 0xE531, 0x4bbd, 0xB3, 0xBC, 0xDD, 0x16, 0xF5, 0xD3, 0x5B, 0x43)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CGraphCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DGraph =
		{ 0x7053BA5E, 0xD076, 0x486d, { 0x8B, 0xA8, 0xD8, 0x91, 0x8F, 0x22, 0x8C, 0xB8 } };
const IID BASED_CODE IID_DGraphEvents =
		{ 0xB4932E33, 0x8924, 0x4a3b, { 0x9D, 0x6D, 0xE7, 0x07, 0x9B, 0x8E, 0xE1, 0x3A } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwGraphOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CGraphCtrl, IDS_GRAPH, _dwGraphOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::CGraphCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CGraphCtrl

BOOL CGraphCtrl::CGraphCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_GRAPH,
			IDB_GRAPH,
			afxRegApartmentThreading,
			_dwGraphOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::CGraphCtrl - Constructor

CGraphCtrl::CGraphCtrl()
{
/////////////////////////////////////////////////////////////////////////
// Following commented out to prevent 3 ASSERTS that occur
// because we are not building a typelib
//	InitializeIIDs(&IID_DGraph, &IID_DGraphEvents);
//
// Following code copied from InitializeIIDs (since we cannot override)
	m_piidPrimary = &IID_DGraph;
	m_piidEvents = &IID_DGraphEvents;
	EnableTypeLib();	// Needed to prevent ASSERT when closing editor
	InitStockEventMask();
	InitStockPropMask();
//////////////////////////////////////////////////////////////////////////

	m_pGraphDlg = NULL;
	m_hWndContainer = NULL;
	m_hMenuInPlace = NULL;
	m_hAcceleratorTable	= NULL;
	m_pGraph = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::~CGraphCtrl - Destructor

CGraphCtrl::~CGraphCtrl()
{
	RELEASE( m_pGraph );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl IDMUSProdEditor implementation

/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::XEditor::AddRef

STDMETHODIMP_(ULONG) CGraphCtrl::XEditor::AddRef()
{
	METHOD_PROLOGUE_EX_( CGraphCtrl, Editor )

	return (ULONG)pThis->ExternalAddRef();
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::XEditor::Release

STDMETHODIMP_(ULONG) CGraphCtrl::XEditor::Release()
{
	METHOD_PROLOGUE_EX_( CGraphCtrl, Editor )

	return (ULONG)pThis->ExternalRelease();
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::XEditor::QueryInterface

STDMETHODIMP CGraphCtrl::XEditor::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_PROLOGUE_EX_( CGraphCtrl, Editor )

	return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::XEditor::AttachObjects implementation

HRESULT CGraphCtrl::XEditor::AttachObjects( IDMUSProdNode* pINode )
{
	METHOD_MANAGE_STATE( CGraphCtrl, Editor )

	ASSERT_VALID( pThis );
	ASSERT( pINode != NULL );

	GUID guidNode;

	pINode->GetNodeId( &guidNode );
	if( ::IsEqualGUID( guidNode, GUID_ToolGraphRefNode ) )
	{
		CGraphRef* pGraphRef = (CGraphRef *)pINode;
		ASSERT( pGraphRef->m_pGraph != NULL );
		pThis->m_pGraph = pGraphRef->m_pGraph;
	}
	else
	{
		pThis->m_pGraph = (CDirectMusicGraph *)pINode;
	}

	ASSERT( pThis->m_pGraph != NULL );
	pThis->m_pGraph->AddRef();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::XEditor::OnInitMenuFilePrint

HRESULT CGraphCtrl::XEditor::OnInitMenuFilePrint( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CGraphCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::XEditor::OnFilePrint

HRESULT CGraphCtrl::XEditor::OnFilePrint( void )
{
	METHOD_MANAGE_STATE( CGraphCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::XEditor::OnInitMenuFilePrintPreview

HRESULT CGraphCtrl::XEditor::OnInitMenuFilePrintPreview( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CGraphCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::XEditor::OnFilePrintPreview

HRESULT CGraphCtrl::XEditor::OnFilePrintPreview( void )
{
	METHOD_MANAGE_STATE( CGraphCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::XEditor::OnViewProperties

HRESULT CGraphCtrl::XEditor::OnViewProperties( void )
{
	METHOD_MANAGE_STATE( CGraphCtrl, Editor )

	HRESULT hr = E_FAIL;

	if( pThis->m_pGraphDlg )
	{
		if( pThis->m_pGraphDlg->OnViewProperties() )
		{
			hr = S_OK;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::XEditor::OnF1Help

HRESULT CGraphCtrl::XEditor::OnF1Help( void )
{
	METHOD_MANAGE_STATE( CGraphCtrl, Editor )
	
    // Determine name of DMUSProd.exe help file
	CString strHelpFileName;

	if( theApp.GetHelpFileName( strHelpFileName ) )
	{
		strHelpFileName += "::/htm/ToolgraphDesignerWindow.htm";
		::HtmlHelp( NULL, strHelpFileName, HH_DISPLAY_TOPIC, 0 );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::XMyOleInPlaceActiveObject implementation

STDMETHODIMP_(ULONG) CGraphCtrl::XMyOleInPlaceActiveObject::AddRef()
{
	METHOD_MANAGE_STATE( CGraphCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.AddRef();
}

STDMETHODIMP_(ULONG) CGraphCtrl::XMyOleInPlaceActiveObject::Release()
{
	METHOD_MANAGE_STATE( CGraphCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.Release();
}

STDMETHODIMP CGraphCtrl::XMyOleInPlaceActiveObject::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_MANAGE_STATE( CGraphCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.QueryInterface( iid, ppvObj );
}

STDMETHODIMP CGraphCtrl::XMyOleInPlaceActiveObject::GetWindow( HWND* lphwnd )
{
	METHOD_MANAGE_STATE( CGraphCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.GetWindow( lphwnd );
}

STDMETHODIMP CGraphCtrl::XMyOleInPlaceActiveObject::ContextSensitiveHelp( BOOL fEnterMode )
{
	METHOD_MANAGE_STATE( CGraphCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.ContextSensitiveHelp( fEnterMode );
}

STDMETHODIMP CGraphCtrl::XMyOleInPlaceActiveObject::TranslateAccelerator( LPMSG lpmsg )
{
	METHOD_MANAGE_STATE( CGraphCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	// Translate only if one of the cntrol's in this editor has focus
	if( pThis->m_pGraphDlg )
	{
		CWnd* pWnd = CWnd::GetFocus();
		while( pWnd )
		{
			if( pWnd == pThis->m_pGraphDlg )
			{
				if( ::TranslateAccelerator(pThis->m_hWnd, pThis->m_hAcceleratorTable, lpmsg) )
				{
					return S_OK;
				}
				break;
			}
			pWnd = pWnd->GetParent();
		}
	}

//	OLEINPLACEFRAMEINFO frameInfo = pThis->m_frameInfo;
//	if( ::OleTranslateAccelerator(pThis->m_pInPlaceFrame, &frameInfo, lpmsg) == S_OK )
//	{
//		return S_OK;
//	}

	return pThis->m_xOleInPlaceActiveObject.TranslateAccelerator( lpmsg );
}

STDMETHODIMP CGraphCtrl::XMyOleInPlaceActiveObject::OnFrameWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CGraphCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnFrameWindowActivate( fActivate );
}

STDMETHODIMP CGraphCtrl::XMyOleInPlaceActiveObject::OnDocWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CGraphCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnDocWindowActivate( fActivate );
}

STDMETHODIMP CGraphCtrl::XMyOleInPlaceActiveObject::ResizeBorder(
	LPCRECT, LPOLEINPLACEUIWINDOW, BOOL fFrameWindow )
{
	METHOD_MANAGE_STATE( CGraphCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

    if( fFrameWindow == TRUE )
	{
		pThis->OnShowToolBars();
	}

	return S_OK;
}

STDMETHODIMP CGraphCtrl::XMyOleInPlaceActiveObject::EnableModeless( BOOL fEnable )
{
	METHOD_MANAGE_STATE( CGraphCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.EnableModeless( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnDraw - Drawing function

void CGraphCtrl::OnDraw( CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid )
{
	UNREFERENCED_PARAMETER(pdc);
	UNREFERENCED_PARAMETER(rcBounds);
	UNREFERENCED_PARAMETER(rcInvalid);
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::DoPropExchange - Persistence support

void CGraphCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnResetState - Reset control to default state

void CGraphCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CGraphAbout dialog used for Graph Editor About Box

class CGraphAbout : public CDialog
{
public:
	CGraphAbout();

// Dialog Data
	//{{AFX_DATA(CGraphAbout)
	enum { IDD = IDD_ABOUTBOX_GRAPH };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphAbout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CGraphAbout)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CGraphAbout::CGraphAbout() : CDialog(CGraphAbout::IDD)
{
	//{{AFX_DATA_INIT(CGraphAbout)
	//}}AFX_DATA_INIT
}

void CGraphAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGraphAbout)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGraphAbout, CDialog)
	//{{AFX_MSG_MAP(CGraphAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CGraphAbout::OnInitDialog() 
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

			AfxFormatString1( strFileVersion, IDS_GRAPH_VERSION_TEXT, achFileVersion );
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
// CGraphCtrl::AboutBox - Display an "About" box to the user

void CGraphCtrl::AboutBox()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CGraphAbout dlgAbout;
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl message handlers

/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnHideToolBars

void CGraphCtrl::OnHideToolBars() 
{
	COleControl::OnHideToolBars();
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnShowToolBars

void CGraphCtrl::OnShowToolBars() 
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
// CGraphCtrl::OnGetInPlaceMenu

HMENU CGraphCtrl::OnGetInPlaceMenu() 
{
	return m_hMenuInPlace;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnCreate

int CGraphCtrl::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
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
	m_hMenuInPlace = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDR_GRAPH_EDITOR) );

	// Load control's accelerator table
	m_hAcceleratorTable = ::LoadAccelerators( theApp.m_hInstance, MAKEINTRESOURCE(IDR_GRAPH_ACCELS) );

	// Create control's dialog
	m_pGraphDlg = new CGraphDlg;
	if( m_pGraphDlg == NULL )
	{
		return -1;
	}

	m_pGraph->m_pGraphCtrl = this;

	m_pGraphDlg->m_pGraph = m_pGraph;
	m_pGraphDlg->m_pGraphCtrl = this;
	m_pGraphDlg->Create( NULL, "WindowName", WS_CHILD | WS_VISIBLE, CRect( 0, 0, 800, 400 ), this, 888, NULL );
	m_pGraphDlg->OnInitialUpdate();
	
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnDestroy

void CGraphCtrl::OnDestroy() 
{
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

	ASSERT( m_pGraph != NULL );

	m_pGraph->m_pGraphCtrl = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnSize

void CGraphCtrl::OnSize( UINT nType, int cx, int cy ) 
{
	COleControl::OnSize( nType, cx, cy );

	m_pGraphDlg->MoveWindow( 0, 0, cx, cy );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnUpdateEditUndo

void CGraphCtrl::OnUpdateEditUndo( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraph != NULL );

	CString strUndo;
	CString strUndoAccel;

	strUndo.LoadString( IDS_UNDO );
	strUndoAccel.LoadString( IDS_UNDO_ACCEL );

	BOOL fEnable = FALSE;

	if( m_pGraph )
	{
		TCHAR achText[MAX_BUFFER];

		ASSERT( m_pGraph->m_pUndoMgr != NULL );

		if( m_pGraph->m_pUndoMgr )
		{
			if( m_pGraph->m_pUndoMgr->GetUndo(achText, MAX_BUFFER) )
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
// CGraphCtrl::OnEditUndo

void CGraphCtrl::OnEditUndo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraph != NULL );

	if( m_pGraph )
	{
		ASSERT( m_pGraph->m_pUndoMgr != NULL );

		if( m_pGraph->m_pUndoMgr )
		{
			TCHAR achText[MAX_BUFFER];

			if( m_pGraph->m_pUndoMgr->GetUndo(achText, MAX_BUFFER) )
			{
				m_pGraph->m_pUndoMgr->Undo( m_pGraph );
				m_pGraph->SetModified( TRUE );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnUpdateEditRedo

void CGraphCtrl::OnUpdateEditRedo( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraph != NULL );

	CString strRedo;
	CString strRedoAccel;

	strRedo.LoadString( IDS_REDO );
	strRedoAccel.LoadString( IDS_REDO_ACCEL );

	BOOL fEnable = FALSE;

	if( m_pGraph )
	{
		TCHAR achText[MAX_BUFFER];

		ASSERT( m_pGraph->m_pUndoMgr != NULL );

		if( m_pGraph->m_pUndoMgr )
		{
			if( m_pGraph->m_pUndoMgr->GetRedo(achText, MAX_BUFFER) )
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
// CGraphCtrl::OnEditRedo

void CGraphCtrl::OnEditRedo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraph != NULL );

	if( m_pGraph )
	{
		ASSERT( m_pGraph->m_pUndoMgr != NULL );

		if( m_pGraph->m_pUndoMgr )
		{
			TCHAR achText[MAX_BUFFER];

			if( m_pGraph->m_pUndoMgr->GetRedo(achText, MAX_BUFFER) )
			{
				m_pGraph->m_pUndoMgr->Redo( m_pGraph );
				m_pGraph->SetModified( TRUE );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnUpdateEditCut

void CGraphCtrl::OnUpdateEditCut( CCmdUI* pCmdUI ) 
{
	if( m_pGraphDlg )
	{
		m_pGraphDlg->OnUpdateEditCut( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnEditCut

void CGraphCtrl::OnEditCut() 
{
	if( m_pGraphDlg )
	{
		m_pGraphDlg->OnEditCut();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnUpdateEditCopy

void CGraphCtrl::OnUpdateEditCopy( CCmdUI* pCmdUI ) 
{
	if( m_pGraphDlg )
	{
		m_pGraphDlg->OnUpdateEditCopy( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnEditCopy

void CGraphCtrl::OnEditCopy() 
{
	if( m_pGraphDlg )
	{
		m_pGraphDlg->OnEditCopy();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnUpdateEditPaste

void CGraphCtrl::OnUpdateEditPaste( CCmdUI* pCmdUI ) 
{
	if( m_pGraphDlg )
	{
		m_pGraphDlg->OnUpdateEditPaste( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnEditPaste

void CGraphCtrl::OnEditPaste() 
{
	if( m_pGraphDlg )
	{
		m_pGraphDlg->OnEditPaste();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnUpdateEditInsert

void CGraphCtrl::OnUpdateEditInsert( CCmdUI* pCmdUI ) 
{
	if( m_pGraphDlg )
	{
		m_pGraphDlg->OnUpdateEditInsert( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnEditInsert

void CGraphCtrl::OnEditInsert() 
{
	if( m_pGraphDlg )
	{
		m_pGraphDlg->OnEditInsert();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnEditSelectAll

void CGraphCtrl::OnEditSelectAll() 
{
	if( m_pGraphDlg )
	{
		m_pGraphDlg->OnEditSelectAll();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnUpdateEditDelete

void CGraphCtrl::OnUpdateEditDelete( CCmdUI* pCmdUI ) 
{
	if( m_pGraphDlg )
	{
		m_pGraphDlg->OnUpdateEditDelete( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnEditDelete

void CGraphCtrl::OnEditDelete() 
{
	if( m_pGraphDlg )
	{
		m_pGraphDlg->OnEditDelete();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl::OnHelpFinder

void CGraphCtrl::OnHelpFinder() 
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
