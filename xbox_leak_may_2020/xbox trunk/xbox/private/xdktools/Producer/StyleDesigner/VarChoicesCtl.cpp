// VarChoicesCtl.cpp : Implementation of the CVarChoicesCtrl ActiveX Control class.

#include "stdafx.h"
#include "StyleDesignerDLL.h"
#include "Style.h"
#include "VarChoices.h"
#include "VarChoicesCtl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVarChoicesCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVarChoicesCtrl, COleControl)
	//{{AFX_MSG_MAP(CVarChoicesCtrl)
	ON_WM_CREATE()
	ON_COMMAND(ID_APP_ABOUT, AboutBox)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
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

BEGIN_DISPATCH_MAP(CVarChoicesCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CVarChoicesCtrl)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CVarChoicesCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CVarChoicesCtrl, COleControl)
	//{{AFX_EVENT_MAP(CVarChoicesCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl Interface map

BEGIN_INTERFACE_MAP(CVarChoicesCtrl, COleControl)
    INTERFACE_PART(CVarChoicesCtrl, IID_IOleInPlaceActiveObject, MyOleInPlaceActiveObject)
    INTERFACE_PART(CVarChoicesCtrl, IID_IDMUSProdEditor, Editor)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVarChoicesCtrl, "STYLEDESIGNER.VarChoicesCtrl.1",
	0x3bd2ba15, 0x46e7, 0x11d0, 0x89, 0xac, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CVarChoicesCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DVarChoices =
		{ 0x3bd2ba13, 0x46e7, 0x11d0, { 0x89, 0xac, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };
const IID BASED_CODE IID_DVarChoicesEvents =
		{ 0x3bd2ba14, 0x46e7, 0x11d0, { 0x89, 0xac, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwVarChoicesOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVarChoicesCtrl, IDS_VARCHOICES, _dwVarChoicesOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::CVarChoicesCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CVarChoicesCtrl

BOOL CVarChoicesCtrl::CVarChoicesCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_VARCHOICES,
			IDB_VARCHOICES,
			afxRegApartmentThreading,
			_dwVarChoicesOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::CVarChoicesCtrl - Constructor

CVarChoicesCtrl::CVarChoicesCtrl()
{
/////////////////////////////////////////////////////////////////////////
// Following commented out to prevent 3 ASSERTS that occur
// because we are not building a typelib
//	InitializeIIDs(&IID_DVarChoices, &IID_DVarChoicesEvents);
//
// Following code copied from InitializeIIDs (since we cannot override)
	m_piidPrimary = &IID_DVarChoices;
	m_piidEvents = &IID_DVarChoicesEvents;
	EnableTypeLib();	// Needed to prevent ASSERT when closing editor
	InitStockEventMask();
	InitStockPropMask();
//////////////////////////////////////////////////////////////////////////

	m_pVarChoicesDlg = NULL;
	m_hWndContainer = NULL;
	m_hMenuInPlace = NULL;
	m_pVarChoices = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::~CVarChoicesCtrl - Destructor

CVarChoicesCtrl::~CVarChoicesCtrl()
{
	RELEASE( m_pVarChoices );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl IDMUSProdEditor implementation

/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::XEditor::AddRef

STDMETHODIMP_(ULONG) CVarChoicesCtrl::XEditor::AddRef()
{
	METHOD_PROLOGUE_EX_( CVarChoicesCtrl, Editor )

	return (ULONG)pThis->ExternalAddRef();
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::XEditor::Release

STDMETHODIMP_(ULONG) CVarChoicesCtrl::XEditor::Release()
{
	METHOD_PROLOGUE_EX_( CVarChoicesCtrl, Editor )

	return (ULONG)pThis->ExternalRelease();
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::XEditor::QueryInterface

STDMETHODIMP CVarChoicesCtrl::XEditor::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_PROLOGUE_EX_( CVarChoicesCtrl, Editor )

	return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::XEditor::AttachObjects implementation

HRESULT CVarChoicesCtrl::XEditor::AttachObjects( IDMUSProdNode* pINode )
{
	METHOD_MANAGE_STATE( CVarChoicesCtrl, Editor )

	ASSERT_VALID( pThis );
	ASSERT( pINode != NULL );

	pThis->m_pVarChoices = (CVarChoices *)pINode;

	ASSERT( pThis->m_pVarChoices != NULL );
	pThis->m_pVarChoices->AddRef();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::XEditor::OnInitMenuFilePrint

HRESULT CVarChoicesCtrl::XEditor::OnInitMenuFilePrint( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CVarChoicesCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::XEditor::OnFilePrint

HRESULT CVarChoicesCtrl::XEditor::OnFilePrint( void )
{
	METHOD_MANAGE_STATE( CVarChoicesCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::XEditor::OnInitMenuFilePrintPreview

HRESULT CVarChoicesCtrl::XEditor::OnInitMenuFilePrintPreview( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CVarChoicesCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::XEditor::OnFilePrintPreview

HRESULT CVarChoicesCtrl::XEditor::OnFilePrintPreview( void )
{
	METHOD_MANAGE_STATE( CVarChoicesCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::XEditor::OnViewProperties

HRESULT CVarChoicesCtrl::XEditor::OnViewProperties( void )
{
	METHOD_MANAGE_STATE( CVarChoicesCtrl, Editor )

	HRESULT hr = E_FAIL;

	if( pThis->m_pVarChoicesDlg )
	{
		if( pThis->m_pVarChoicesDlg->OnViewProperties() )
		{
			hr = S_OK;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::XEditor::OnF1Help

HRESULT CVarChoicesCtrl::XEditor::OnF1Help( void )
{
	METHOD_MANAGE_STATE( CVarChoicesCtrl, Editor )
	
    // Determine name of DMUSProd.exe help file
	CString strHelpFileName;

	if( theApp.GetHelpFileName( strHelpFileName ) )
	{
		strHelpFileName += "::/htm/VariationChoicesWindow.htm";
		::HtmlHelp( NULL, strHelpFileName, HH_DISPLAY_TOPIC, 0 );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::XMyOleInPlaceActiveObject implementation

STDMETHODIMP_(ULONG) CVarChoicesCtrl::XMyOleInPlaceActiveObject::AddRef()
{
	METHOD_MANAGE_STATE( CVarChoicesCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.AddRef();
}

STDMETHODIMP_(ULONG) CVarChoicesCtrl::XMyOleInPlaceActiveObject::Release()
{
	METHOD_MANAGE_STATE( CVarChoicesCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.Release();
}

STDMETHODIMP CVarChoicesCtrl::XMyOleInPlaceActiveObject::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_MANAGE_STATE( CVarChoicesCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.QueryInterface( iid, ppvObj );
}

STDMETHODIMP CVarChoicesCtrl::XMyOleInPlaceActiveObject::GetWindow( HWND* lphwnd )
{
	METHOD_MANAGE_STATE( CVarChoicesCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.GetWindow( lphwnd );
}

STDMETHODIMP CVarChoicesCtrl::XMyOleInPlaceActiveObject::ContextSensitiveHelp( BOOL fEnterMode )
{
	METHOD_MANAGE_STATE( CVarChoicesCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.ContextSensitiveHelp( fEnterMode );
}

STDMETHODIMP CVarChoicesCtrl::XMyOleInPlaceActiveObject::TranslateAccelerator( LPMSG lpmsg )
{
	METHOD_MANAGE_STATE( CVarChoicesCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );
	
//	OLEINPLACEFRAMEINFO frameInfo = pThis->m_frameInfo;
//	if( ::OleTranslateAccelerator(pThis->m_pInPlaceFrame, &frameInfo, lpmsg) == S_OK )
//	{
//		return S_OK;
//	}

	return pThis->m_xOleInPlaceActiveObject.TranslateAccelerator( lpmsg );
}

STDMETHODIMP CVarChoicesCtrl::XMyOleInPlaceActiveObject::OnFrameWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CVarChoicesCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnFrameWindowActivate( fActivate );
}

STDMETHODIMP CVarChoicesCtrl::XMyOleInPlaceActiveObject::OnDocWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CVarChoicesCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnDocWindowActivate( fActivate );
}

STDMETHODIMP CVarChoicesCtrl::XMyOleInPlaceActiveObject::ResizeBorder(
	LPCRECT, LPOLEINPLACEUIWINDOW, BOOL fFrameWindow )
{
	METHOD_MANAGE_STATE( CVarChoicesCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

    if( fFrameWindow == TRUE )
	{
		pThis->OnShowToolBars();
	}

	return S_OK;
}

STDMETHODIMP CVarChoicesCtrl::XMyOleInPlaceActiveObject::EnableModeless( BOOL fEnable )
{
	METHOD_MANAGE_STATE( CVarChoicesCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.EnableModeless( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::OnDraw - Drawing function

void CVarChoicesCtrl::OnDraw( CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid )
{
	UNREFERENCED_PARAMETER(pdc);
	UNREFERENCED_PARAMETER(rcBounds);
	UNREFERENCED_PARAMETER(rcInvalid);
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::DoPropExchange - Persistence support

void CVarChoicesCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CVarChoicesAbout dialog used for VarChoices Editor About Box

class CVarChoicesAbout : public CDialog
{
public:
	CVarChoicesAbout();

// Dialog Data
	//{{AFX_DATA(CVarChoicesAbout)
	enum { IDD = IDD_ABOUTBOX_VARCHOICES };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVarChoicesAbout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CVarChoicesAbout)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CVarChoicesAbout::CVarChoicesAbout() : CDialog(CVarChoicesAbout::IDD)
{
	//{{AFX_DATA_INIT(CVarChoicesAbout)
	//}}AFX_DATA_INIT
}

void CVarChoicesAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVarChoicesAbout)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVarChoicesAbout, CDialog)
	//{{AFX_MSG_MAP(CVarChoicesAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CVarChoicesAbout::OnInitDialog() 
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

			AfxFormatString1( strFileVersion, IDS_VARCHOICES_VERSION_TEXT, achFileVersion );
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
// CVarChoicesCtrl::AboutBox - Display an "About" box to the user

void CVarChoicesCtrl::AboutBox()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CVarChoicesAbout dlgAbout;
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl message handlers

/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::OnCreate

int CVarChoicesCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
	m_hMenuInPlace = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDR_VARCHOICES_EDITOR) );


	// Create control's dialog
	{
		CDC* pDC = GetDC();
		if( pDC )
		{
			TEXTMETRIC tm;

			pDC->GetTextMetrics( &tm );

			if( tm.tmAveCharWidth < 8 )	// Small Fonts
			{
				HRSRC hRes = ::FindResource( theApp.m_hInstance, MAKEINTRESOURCE(IDD_DLG_VARCHOICES), RT_DIALOG );
				if( hRes )
				{
					HGLOBAL hTemplate = ::LoadResource( theApp.m_hInstance, hRes );
					if( hTemplate )
					{
						DLGTEMPLATE* pTemplate = (DLGTEMPLATE *)::LockResource( hTemplate );
						if( pTemplate )
						{
							pTemplate->cx = 530;
							pTemplate->cy = 368;

							UnlockResource( hTemplate );
						}

						::FreeResource( hTemplate );
					}
				}
			}

			ReleaseDC( pDC );
		}

		m_pVarChoicesDlg = new CVarChoicesDlg;
		if( m_pVarChoicesDlg == NULL )
		{
			return -1;
		}

		m_pVarChoices->m_pVarChoicesCtrl = this;

		m_pVarChoicesDlg->m_pVarChoices = m_pVarChoices;
		m_pVarChoicesDlg->m_pVarChoicesCtrl = this;
		m_pVarChoicesDlg->Create( NULL, "WindowName", WS_CHILD | WS_VISIBLE, CRect( 0, 0, 800, 400 ), this, 888, NULL );
		m_pVarChoicesDlg->OnInitialUpdate();
	}
	
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::OnDestroy

void CVarChoicesCtrl::OnDestroy() 
{
	COleControl::OnDestroy();

	if( m_hMenuInPlace )
	{
		::DestroyMenu( m_hMenuInPlace );
		m_hMenuInPlace = NULL;
	}

	ASSERT( m_pVarChoices != NULL );

	// Get Frame's WINDOWPLACEMENT information
	CWnd* pWndParent = GetParent();
	if( pWndParent )
	{
		pWndParent = pWndParent->GetParent();
		if( pWndParent )
		{
			pWndParent = pWndParent->GetParent();
			if( pWndParent )
			{
				m_pVarChoices->m_wp.length = sizeof(m_pVarChoices->m_wp);
				if( pWndParent->GetWindowPlacement( &m_pVarChoices->m_wp ) == FALSE )
				{
					memset( &m_pVarChoices->m_wp, 0, sizeof(WINDOWPLACEMENT) );
				}
			}
		}
	}

	m_pVarChoices->m_pVarChoicesCtrl = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::OnGetInPlaceMenu

HMENU CVarChoicesCtrl::OnGetInPlaceMenu() 
{
	return m_hMenuInPlace;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::OnHideToolBars

void CVarChoicesCtrl::OnHideToolBars() 
{
	COleControl::OnHideToolBars();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::OnShowToolBars

void CVarChoicesCtrl::OnShowToolBars() 
{
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
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::OnSize

void CVarChoicesCtrl::OnSize( UINT nType, int cx, int cy ) 
{
	COleControl::OnSize( nType, cx, cy );

	m_pVarChoicesDlg->MoveWindow( 0, 0, cx, cy );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::OnUpdateEditUndo

void CVarChoicesCtrl::OnUpdateEditUndo( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	/*
	ASSERT( m_pVarChoices != NULL );

	CString strUndo;
	CString strUndoAccel;

	strUndo.LoadString( IDS_UNDO );
	strUndoAccel.LoadString( IDS_UNDO_ACCEL );

	BOOL fEnable = FALSE;

	if( m_pVarChoices
	&&  m_pVarChoices->m_pPattern )
	{
		TCHAR achText[MAX_BUFFER];

		ASSERT( m_pVarChoices->m_pPattern->m_pUndoMgr != NULL );

		if( m_pVarChoices->m_pPattern->m_pUndoMgr )
		{
			if( m_pVarChoices->m_pPattern->m_pUndoMgr->GetUndo(achText, MAX_BUFFER) )
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
	*/
	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::OnEditUndo

void CVarChoicesCtrl::OnEditUndo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	/*
	ASSERT( m_pVarChoices != NULL );

	if( m_pVarChoices
	&&  m_pVarChoices->m_pPattern )
	{
		ASSERT( m_pVarChoices->m_pPattern->m_pUndoMgr != NULL );

		if( m_pVarChoices->m_pPattern->m_pUndoMgr )
		{
			TCHAR achText[MAX_BUFFER];

			if( m_pVarChoices->m_pPattern->m_pUndoMgr->GetUndo(achText, MAX_BUFFER) )
			{
				m_pVarChoices->m_pPattern->m_pUndoMgr->Undo( m_pVarChoices->m_pPattern );
				m_pVarChoices->m_pPattern->SetModified( TRUE );
			}
		}
	}
	*/
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::OnUpdateEditRedo

void CVarChoicesCtrl::OnUpdateEditRedo( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	/*
	ASSERT( m_pVarChoices != NULL );

	CString strRedo;
	CString strRedoAccel;

	strRedo.LoadString( IDS_REDO );
	strRedoAccel.LoadString( IDS_REDO_ACCEL );

	BOOL fEnable = FALSE;

	if( m_pVarChoices
	&&  m_pVarChoices->m_pPattern )
	{
		TCHAR achText[MAX_BUFFER];

		ASSERT( m_pVarChoices->m_pPattern->m_pUndoMgr != NULL );

		if( m_pVarChoices->m_pPattern->m_pUndoMgr )
		{
			if( m_pVarChoices->m_pPattern->m_pUndoMgr->GetRedo(achText, MAX_BUFFER) )
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
	*/
	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::OnEditRedo

void CVarChoicesCtrl::OnEditRedo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	/*
	ASSERT( m_pVarChoices != NULL );

	if( m_pVarChoices
	&&  m_pVarChoices->m_pPattern )
	{
		ASSERT( m_pVarChoices->m_pPattern->m_pUndoMgr != NULL );

		if( m_pVarChoices->m_pPattern->m_pUndoMgr )
		{
			TCHAR achText[MAX_BUFFER];

			if( m_pVarChoices->m_pPattern->m_pUndoMgr->GetRedo(achText, MAX_BUFFER) )
			{
				m_pVarChoices->m_pPattern->m_pUndoMgr->Redo( m_pVarChoices->m_pPattern );
				m_pVarChoices->m_pPattern->SetModified( TRUE );
			}
		}
	}
	*/
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::OnUpdateEditCopy

void CVarChoicesCtrl::OnUpdateEditCopy( CCmdUI* pCmdUI ) 
{
	if( m_pVarChoicesDlg )
	{
		m_pVarChoicesDlg->OnUpdateEditCopy( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::OnEditCopy

void CVarChoicesCtrl::OnEditCopy() 
{
	if( m_pVarChoicesDlg )
	{
		m_pVarChoicesDlg->OnEditCopy();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::OnUpdateEditPaste

void CVarChoicesCtrl::OnUpdateEditPaste( CCmdUI* pCmdUI ) 
{
	if( m_pVarChoicesDlg )
	{
		m_pVarChoicesDlg->OnUpdateEditPaste( pCmdUI );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::OnEditPaste

void CVarChoicesCtrl::OnEditPaste() 
{
	if( m_pVarChoicesDlg )
	{
		m_pVarChoicesDlg->OnEditPaste();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::PreTranslateMessage

BOOL CVarChoicesCtrl::PreTranslateMessage( MSG* pMsg ) 
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
					if( m_pVarChoicesDlg )
					{
						if( m_pVarChoicesDlg->HandleKeyDown( pMsg ) )
						{
							return TRUE;
						}
					}
					break;
			}
			break;
	}
	
	return COleControl::PreTranslateMessage( pMsg );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl::OnHelpFinder

void CVarChoicesCtrl::OnHelpFinder() 
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
