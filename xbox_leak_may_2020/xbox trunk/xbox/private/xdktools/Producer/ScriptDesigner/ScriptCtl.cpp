// ScriptCtl.cpp : Implementation of the CScriptCtrl ActiveX Control class.

#include "stdafx.h"
#include "ScriptDesignerDLL.h"
#include "Script.h"
#include "ScriptCtl.h"
#include "ScriptRef.h"
#include "ScriptPpg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CScriptCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CScriptCtrl, COleControl)
	//{{AFX_MSG_MAP(CScriptCtrl)
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
	ON_COMMAND(IDM_CALL_ROUTINE, OnCallRoutine)
	ON_UPDATE_COMMAND_UI(IDM_CALL_ROUTINE, OnUpdateCallRoutine)
	ON_UPDATE_COMMAND_UI(IDM_SET_VARIABLE, OnUpdateSetVariable)
	ON_COMMAND(IDM_SET_VARIABLE, OnSetVariable)
	ON_COMMAND(IDM_HELP_SCRIPT_API, OnHelpScriptAPI)
	ON_COMMAND(IDM_HELP_SCRIPT_LANGUAGE, OnHelpScriptLanguage)
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CScriptCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CScriptCtrl)
	// NOTE - ClassWizard will add and remove dispatch map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CScriptCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CScriptCtrl, COleControl)
	//{{AFX_EVENT_MAP(CScriptCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl Interface map

BEGIN_INTERFACE_MAP(CScriptCtrl, COleControl)
    INTERFACE_PART(CScriptCtrl, IID_IOleInPlaceActiveObject, MyOleInPlaceActiveObject)
    INTERFACE_PART(CScriptCtrl, IID_IDMUSProdEditor, Editor)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CScriptCtrl, 1)
	PROPPAGEID(CScriptPropPage::guid)
END_PROPPAGEIDS(CScriptCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CScriptCtrl, "SCRIPTDESIGNER.ScriptCtrl.1",
	0xd135db58, 0x66ed, 0x11d3, 0xb4, 0x5d, 0, 0x10, 0x5a, 0x27, 0x96, 0xde)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CScriptCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DScript =
		{ 0xd135db56, 0x66ed, 0x11d3, { 0xb4, 0x5d, 0, 0x10, 0x5a, 0x27, 0x96, 0xde } };
const IID BASED_CODE IID_DScriptEvents =
		{ 0xd135db57, 0x66ed, 0x11d3, { 0xb4, 0x5d, 0, 0x10, 0x5a, 0x27, 0x96, 0xde } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwScriptOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CScriptCtrl, IDS_SCRIPT, _dwScriptOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::CScriptCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CScriptCtrl

BOOL CScriptCtrl::CScriptCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_SCRIPT,
			IDB_SCRIPT,
			afxRegApartmentThreading,
			_dwScriptOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::CScriptCtrl - Constructor

CScriptCtrl::CScriptCtrl()
{
/////////////////////////////////////////////////////////////////////////
// Following commented out to prevent 3 ASSERTS that occur
// because we are not building a typelib
//	InitializeIIDs(&IID_DScript, &IID_DScriptEvents);
//
// Following code copied from InitializeIIDs (since we cannot override)
	m_piidPrimary = &IID_DScript;
	m_piidEvents = &IID_DScriptEvents;
	EnableTypeLib();	// Needed to prevent ASSERT when closing editor
	InitStockEventMask();
	InitStockPropMask();
//////////////////////////////////////////////////////////////////////////

	m_pScriptDlg = NULL;
	m_hWndContainer = NULL;
	m_hMenuInPlace = NULL;
	m_hAcceleratorTable	= NULL;
	m_hKeyStatusBar = NULL;
	m_pScript = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::~CScriptCtrl - Destructor

CScriptCtrl::~CScriptCtrl()
{
	RELEASE( m_pScript );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl IDMUSProdEditor implementation

/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::XEditor::AddRef

STDMETHODIMP_(ULONG) CScriptCtrl::XEditor::AddRef()
{
	METHOD_PROLOGUE_EX_( CScriptCtrl, Editor )

	return (ULONG)pThis->ExternalAddRef();
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::XEditor::Release

STDMETHODIMP_(ULONG) CScriptCtrl::XEditor::Release()
{
	METHOD_PROLOGUE_EX_( CScriptCtrl, Editor )

	return (ULONG)pThis->ExternalRelease();
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::XEditor::QueryInterface

STDMETHODIMP CScriptCtrl::XEditor::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_PROLOGUE_EX_( CScriptCtrl, Editor )

	return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::XEditor::AttachObjects implementation

HRESULT CScriptCtrl::XEditor::AttachObjects( IDMUSProdNode* pINode )
{
	METHOD_MANAGE_STATE( CScriptCtrl, Editor )

	ASSERT_VALID( pThis );
	ASSERT( pINode != NULL );

	GUID guidNode;

	pINode->GetNodeId( &guidNode );
	if( ::IsEqualGUID( guidNode, GUID_ScriptRefNode ) )
	{
		CScriptRef* pScriptRef = (CScriptRef *)pINode;
		ASSERT( pScriptRef->m_pScript != NULL );
		pThis->m_pScript = pScriptRef->m_pScript;
	}
	else
	{
		pThis->m_pScript = (CDirectMusicScript *)pINode;
	}

	ASSERT( pThis->m_pScript != NULL );
	pThis->m_pScript->AddRef();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::XEditor::OnInitMenuFilePrint

HRESULT CScriptCtrl::XEditor::OnInitMenuFilePrint( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CScriptCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::XEditor::OnFilePrint

HRESULT CScriptCtrl::XEditor::OnFilePrint( void )
{
	METHOD_MANAGE_STATE( CScriptCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::XEditor::OnInitMenuFilePrintPreview

HRESULT CScriptCtrl::XEditor::OnInitMenuFilePrintPreview( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CScriptCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::XEditor::OnFilePrintPreview

HRESULT CScriptCtrl::XEditor::OnFilePrintPreview( void )
{
	METHOD_MANAGE_STATE( CScriptCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::XEditor::OnViewProperties

HRESULT CScriptCtrl::XEditor::OnViewProperties( void )
{
	METHOD_MANAGE_STATE( CScriptCtrl, Editor )

	HRESULT hr = E_FAIL;

	if( pThis->m_pScriptDlg )
	{
		if( pThis->m_pScriptDlg->OnViewProperties() )
		{
			hr = S_OK;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::XEditor::OnF1Help

HRESULT CScriptCtrl::XEditor::OnF1Help( void )
{
	METHOD_MANAGE_STATE( CScriptCtrl, Editor )
	
    // Determine name of DMUSProd.exe help file
	CString strHelpFileName;

	if( theApp.GetHelpFileName( strHelpFileName ) )
	{
		strHelpFileName += "::/htm/ScriptDesignerWindow.htm";
		::HtmlHelp( NULL, strHelpFileName, HH_DISPLAY_TOPIC, 0 );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::XMyOleInPlaceActiveObject implementation

STDMETHODIMP_(ULONG) CScriptCtrl::XMyOleInPlaceActiveObject::AddRef()
{
	METHOD_MANAGE_STATE( CScriptCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.AddRef();
}

STDMETHODIMP_(ULONG) CScriptCtrl::XMyOleInPlaceActiveObject::Release()
{
	METHOD_MANAGE_STATE( CScriptCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.Release();
}

STDMETHODIMP CScriptCtrl::XMyOleInPlaceActiveObject::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_MANAGE_STATE( CScriptCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.QueryInterface( iid, ppvObj );
}

STDMETHODIMP CScriptCtrl::XMyOleInPlaceActiveObject::GetWindow( HWND* lphwnd )
{
	METHOD_MANAGE_STATE( CScriptCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.GetWindow( lphwnd );
}

STDMETHODIMP CScriptCtrl::XMyOleInPlaceActiveObject::ContextSensitiveHelp( BOOL fEnterMode )
{
	METHOD_MANAGE_STATE( CScriptCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.ContextSensitiveHelp( fEnterMode );
}

STDMETHODIMP CScriptCtrl::XMyOleInPlaceActiveObject::TranslateAccelerator( LPMSG lpmsg )
{
	METHOD_MANAGE_STATE( CScriptCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	if( pThis->m_pScriptDlg )
	{
		// VK_R (Call Routine)
		// VK_E (Set Variable)
		if( (lpmsg->message == WM_KEYDOWN && lpmsg->wParam == 0x52)  
		||  (lpmsg->message == WM_KEYDOWN && lpmsg->wParam == 0x45) 
		||  (lpmsg->message == WM_KEYDOWN && lpmsg->wParam == 0x59) 
		||  (lpmsg->message == WM_KEYDOWN && lpmsg->wParam == 0x5A) )  
		{
			// Translate CTRL + R when any control in the editor has focus
			// Translate CTRL + E when any control in the editor has focus
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

		// Everything else
		else
		{
			// Translate everything else only when the source edit control has focus
			if( ::IsWindow(pThis->m_pScriptDlg->m_editSource.m_hWnd) )
			{
				HWND hwndFocus = ::GetFocus();
				if( pThis->m_pScriptDlg->m_editSource.m_hWnd == hwndFocus )
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

STDMETHODIMP CScriptCtrl::XMyOleInPlaceActiveObject::OnFrameWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CScriptCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnFrameWindowActivate( fActivate );
}

STDMETHODIMP CScriptCtrl::XMyOleInPlaceActiveObject::OnDocWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CScriptCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnDocWindowActivate( fActivate );
}

STDMETHODIMP CScriptCtrl::XMyOleInPlaceActiveObject::ResizeBorder(
	LPCRECT, LPOLEINPLACEUIWINDOW, BOOL fFrameWindow )
{
	METHOD_MANAGE_STATE( CScriptCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

    if( fFrameWindow == TRUE )
	{
		pThis->OnShowToolBars();
	}

	return S_OK;
}

STDMETHODIMP CScriptCtrl::XMyOleInPlaceActiveObject::EnableModeless( BOOL fEnable )
{
	METHOD_MANAGE_STATE( CScriptCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.EnableModeless( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnDraw - Drawing function

void CScriptCtrl::OnDraw( CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid )
{
	UNREFERENCED_PARAMETER(pdc);
	UNREFERENCED_PARAMETER(rcBounds);
	UNREFERENCED_PARAMETER(rcInvalid);
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::DoPropExchange - Persistence support

void CScriptCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnResetState - Reset control to default state

void CScriptCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CScriptAbout dialog used for Script Editor About Box

class CScriptAbout : public CDialog
{
public:
	CScriptAbout();

// Dialog Data
	//{{AFX_DATA(CScriptAbout)
	enum { IDD = IDD_ABOUTBOX_SCRIPT };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptAbout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CScriptAbout)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CScriptAbout::CScriptAbout() : CDialog(CScriptAbout::IDD)
{
	//{{AFX_DATA_INIT(CScriptAbout)
	//}}AFX_DATA_INIT
}

void CScriptAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScriptAbout)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScriptAbout, CDialog)
	//{{AFX_MSG_MAP(CScriptAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CScriptAbout::OnInitDialog() 
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

			AfxFormatString1( strFileVersion, IDS_SCRIPT_VERSION_TEXT, achFileVersion );
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
// CScriptCtrl::AboutBox - Display an "About" box to the user

void CScriptCtrl::AboutBox()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CScriptAbout dlgAbout;
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl message handlers

/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnHideToolBars

void CScriptCtrl::OnHideToolBars() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );

	theApp.m_pScriptComponent->m_pIFramework->RestoreStatusBar( m_hKeyStatusBar );
	m_hKeyStatusBar = NULL;

	COleControl::OnHideToolBars();
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnShowToolBars

void CScriptCtrl::OnShowToolBars() 
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

	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );

	// Create status bar panes if they don't already exist
	if( m_hKeyStatusBar == NULL )
	{
		theApp.m_pScriptComponent->m_pIFramework->SetNbrStatusBarPanes( 1, SBLS_EDITOR, &m_hKeyStatusBar );
		theApp.m_pScriptComponent->m_pIFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 0, SBS_SUNKEN, 18 );
	}

	COleControl::OnShowToolBars();
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnGetInPlaceMenu

HMENU CScriptCtrl::OnGetInPlaceMenu() 
{
	return m_hMenuInPlace;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnCreate

int CScriptCtrl::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
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

	// Cut down on flicker when redrawing Script editor
	ModifyStyle( 0, WS_CLIPCHILDREN, 0 );
	CWnd* pWndParent = GetParent();
	if( pWndParent )
	{
		pWndParent->ModifyStyle( 0, WS_CLIPCHILDREN, 0 );
	}

	// Load control's in-place menu
	m_hMenuInPlace = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDR_SCRIPT_EDITOR) );

	// Load control's accelerator table
	m_hAcceleratorTable = ::LoadAccelerators( theApp.m_hInstance, MAKEINTRESOURCE(IDR_SCRIPT_ACCELS) );

	// Create control's dialog
	m_pScriptDlg = new CScriptDlg;
	if( m_pScriptDlg == NULL )
	{
		return -1;
	}

	m_pScript->m_pScriptCtrl = this;

	m_pScriptDlg->m_pScript = m_pScript;
	m_pScriptDlg->m_pScriptCtrl = this;
	m_pScriptDlg->Create( NULL, "WindowName", WS_CHILD | WS_VISIBLE, CRect( 0, 0, 800, 400 ), this, 888, NULL );
	m_pScriptDlg->OnInitialUpdate();
	
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnDestroy

void CScriptCtrl::OnDestroy() 
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

	ASSERT( m_pScript != NULL );

	m_pScript->m_pScriptCtrl = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnSize

void CScriptCtrl::OnSize( UINT nType, int cx, int cy ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	COleControl::OnSize( nType, cx, cy );

	m_pScriptDlg->MoveWindow( 0, 0, cx, cy );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnUpdateEditUndo

void CScriptCtrl::OnUpdateEditUndo( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScript != NULL );

	// Make sure we have the latest text
	// Necessary because sync takes place in KillFocus 
	// and undo may have been initiated by Alt+E/Alt+U or Ctrl+Z
	if( m_pScriptDlg )
	{
		m_pScriptDlg->SyncSource();
	}

	CString strUndo;
	CString strUndoAccel;

	strUndo.LoadString( IDS_UNDO );
	strUndoAccel.LoadString( IDS_UNDO_ACCEL );

	BOOL fEnable = FALSE;

	if( m_pScript )
	{
		TCHAR achText[256];

		ASSERT( m_pScript->m_pUndoMgr != NULL );

		if( m_pScript->m_pUndoMgr )
		{
			if( m_pScript->m_pUndoMgr->GetUndo(achText, 256) )
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
// CScriptCtrl::OnEditUndo

void CScriptCtrl::OnEditUndo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScript != NULL );

	if( m_pScript )
	{
		ASSERT( m_pScript->m_pUndoMgr != NULL);
		
		if( m_pScript->m_pUndoMgr )
		{
			TCHAR achText[MAX_BUFFER];

			if( m_pScript->m_pUndoMgr->GetUndo(achText, MAX_BUFFER) )
			{
				m_pScript->m_pUndoMgr->Undo( m_pScript );
				m_pScript->SetModified( TRUE );
				m_pScript->SyncScriptEditor( SSE_SOURCE );

				// Following will sync routines and variables
				m_pScript->SyncScriptWithDirectMusic();	
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnUpdateEditRedo

void CScriptCtrl::OnUpdateEditRedo( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScript != NULL );

	// Make sure we have the latest text
	// Necessary because sync takes place in KillFocus 
	// and redo may have been initiated by Alt+E/Alt+R or Ctrl+Y
	if( m_pScriptDlg )
	{
		m_pScriptDlg->SyncSource();
	}

	CString strRedo;
	CString strRedoAccel;

	strRedo.LoadString( IDS_REDO );
	strRedoAccel.LoadString( IDS_REDO_ACCEL );

	BOOL fEnable = FALSE;

	if( m_pScript )
	{
		TCHAR achText[MAX_BUFFER];

		ASSERT( m_pScript->m_pUndoMgr != NULL );

		if( m_pScript->m_pUndoMgr )
		{
			if( m_pScript->m_pUndoMgr->GetRedo(achText, MAX_BUFFER) )
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
// CScriptCtrl::OnEditRedo

void CScriptCtrl::OnEditRedo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScript != NULL );

	if( m_pScript )
	{
		ASSERT( m_pScript->m_pUndoMgr != NULL );
		
		if( m_pScript->m_pUndoMgr )
		{
			TCHAR achText[MAX_BUFFER];

			if( m_pScript->m_pUndoMgr->GetRedo(achText, MAX_BUFFER) )
			{
				m_pScript->m_pUndoMgr->Redo( m_pScript );
				m_pScript->SetModified( TRUE );
				m_pScript->SyncScriptEditor( SSE_SOURCE );

				// Following will sync routines and variables
				m_pScript->SyncScriptWithDirectMusic();	
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnUpdateEditCut

void CScriptCtrl::OnUpdateEditCut( CCmdUI* pCmdUI ) 
{
	if( m_pScriptDlg )
	{
		m_pScriptDlg->OnUpdateEditCut( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnEditCut

void CScriptCtrl::OnEditCut() 
{
	if( m_pScriptDlg )
	{
		m_pScriptDlg->OnEditCut();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnUpdateEditCopy

void CScriptCtrl::OnUpdateEditCopy( CCmdUI* pCmdUI ) 
{
	if( m_pScriptDlg )
	{
		m_pScriptDlg->OnUpdateEditCopy( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnEditCopy

void CScriptCtrl::OnEditCopy() 
{
	if( m_pScriptDlg )
	{
		m_pScriptDlg->OnEditCopy();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnUpdateEditPaste

void CScriptCtrl::OnUpdateEditPaste( CCmdUI* pCmdUI ) 
{
	if( m_pScriptDlg )
	{
		m_pScriptDlg->OnUpdateEditPaste( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnEditPaste

void CScriptCtrl::OnEditPaste() 
{
	if( m_pScriptDlg )
	{
		m_pScriptDlg->OnEditPaste();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnUpdateEditInsert

void CScriptCtrl::OnUpdateEditInsert( CCmdUI* pCmdUI ) 
{
	if( m_pScriptDlg )
	{
		m_pScriptDlg->OnUpdateEditInsert( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnEditInsert

void CScriptCtrl::OnEditInsert() 
{
	if( m_pScriptDlg )
	{
		m_pScriptDlg->OnEditInsert();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnEditSelectAll

void CScriptCtrl::OnEditSelectAll() 
{
	if( m_pScriptDlg )
	{
		m_pScriptDlg->OnEditSelectAll();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnUpdateEditDelete

void CScriptCtrl::OnUpdateEditDelete( CCmdUI* pCmdUI ) 
{
	if( m_pScriptDlg )
	{
		m_pScriptDlg->OnUpdateEditDelete( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnEditDelete

void CScriptCtrl::OnEditDelete() 
{
	if( m_pScriptDlg )
	{
		m_pScriptDlg->OnEditDelete();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnUpdateCallRoutine

void CScriptCtrl::OnUpdateCallRoutine( CCmdUI* pCmdUI ) 
{
	pCmdUI->Enable( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnCallRoutine

void CScriptCtrl::OnCallRoutine() 
{
	if( m_pScriptDlg )
	{
		m_pScriptDlg->CallSelectedRoutines();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnUpdateSetVariable

void CScriptCtrl::OnUpdateSetVariable( CCmdUI* pCmdUI ) 
{
	pCmdUI->Enable( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnSetVariable

void CScriptCtrl::OnSetVariable() 
{
	if( m_pScriptDlg )
	{
		m_pScriptDlg->SetSelectedVariable();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::PreTranslateMessage

BOOL CScriptCtrl::PreTranslateMessage( MSG* pMsg ) 
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScriptDlg 
	&&  ::IsWindow(m_pScriptDlg->m_editSource.m_hWnd) )
	{
		switch( pMsg->message )
		{
			case WM_KEYDOWN:
			{
				if( pMsg->lParam & 0x40000000 )
				{
					break;
				}

				switch( pMsg->wParam )
				{
					case VK_TAB:
					{
						CWnd* pWndFocus = GetFocus();
						if( pWndFocus )
						{
							if( &m_pScriptDlg->m_editSource != pWndFocus )
							{
								CWnd* pWndNext;
								if( GetAsyncKeyState(VK_SHIFT) & 0x8000 )
								{
									pWndNext = GetNextDlgTabItem( pWndFocus, TRUE );
								}
								else
								{
									pWndNext = GetNextDlgTabItem( pWndFocus, FALSE );
								}
								if( pWndNext )
								{
									pWndNext->SetFocus();
								}
								return TRUE;
							}
						}
						break;
					}
				}
				break;
			}

			case WM_SYSKEYDOWN:
			{
				if( pMsg->lParam & 0x40000000 )
				{
					break;
				}

				switch( pMsg->wParam )
				{
					case 0x42:  // VK_B		(Variable listbox)
					case 0x62:  // VK_B		(Variable listbox)
						if( GetAsyncKeyState(VK_MENU) & 0x8000 )
						{
							m_pScriptDlg->m_lstbxVariables.SetFocus();
							return TRUE;
						}
						break;

					case 0x49:	// VK_I		(Reinitialize button)
					case 0x69:	// VK_I		(Reinitialize button)
						if( GetAsyncKeyState(VK_MENU) & 0x8000 )
						{
							m_pScriptDlg->SyncSource();  // To pick up latest changes
							m_pScriptDlg->OnInitVariables();
							return TRUE;
						}
						break;

					case 0x4f:	// VK_O		(Routine listbox)
					case 0x6f:	// VK_O		(Routine listbox)
						if( GetAsyncKeyState(VK_MENU) & 0x8000 )
						{
							m_pScriptDlg->m_lstbxRoutines.SetFocus();
							return TRUE;
						}
						break;

					case 0x52:	// VK_R		(Refresh button)
					case 0x72:	// VK_R		(Refresh button)
						if( GetAsyncKeyState(VK_MENU) & 0x8000 )
						{
							m_pScriptDlg->SyncSource();  // To pick up latest changes
							m_pScriptDlg->OnRefresh();
							return TRUE;
						}
						break;

					case 0x53:	// VK_S		(Source edit control)
					case 0x73:	// VK_S		(Source edit control)
						if( GetAsyncKeyState(VK_MENU) & 0x8000 )
						{
							m_pScriptDlg->m_editSource.SetFocus();
							return TRUE;
						}
						break;
				}
				break;
			}
		}
	}
	
	return COleControl::PreTranslateMessage( pMsg );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnHelpFinder

void CScriptCtrl::OnHelpFinder() 
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
// CScriptCtrl::OnHelpScriptAPI

void CScriptCtrl::OnHelpScriptAPI() 
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

   // Determine name of DMUSProd.exe help file
	CString strHelpFileName;

	if( theApp.GetHelpFileName( strHelpFileName ) )
	{
		strHelpFileName += "::/htm/DirectMusicScriptingReference.htm";
		::HtmlHelp( NULL, strHelpFileName, HH_DISPLAY_TOPIC, 0 );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptCtrl::OnHelpScriptLanguage

void CScriptCtrl::OnHelpScriptLanguage() 
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

   // Determine name of DMUSProd.exe help file
	CString strHelpFileName;

	if( theApp.GetHelpFileName( strHelpFileName ) )
	{
		strHelpFileName += "::/htm/AudioVBScriptLanguage.htm";
		::HtmlHelp( NULL, strHelpFileName, HH_DISPLAY_TOPIC, 0 );
	}
}
