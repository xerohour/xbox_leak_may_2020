// PatternCtl.cpp : Implementation of the CPatternCtrl ActiveX Control class.

#include "stdafx.h"
#include "StyleDesignerDLL.h"
#include "Style.h"
#include "Pattern.h"
#include "PatternCtl.h"
#include "MIDIStripMgr.h"
#include <SegmentGUIDS.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CPatternCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CPatternCtrl, COleControl)
	//{{AFX_MSG_MAP(CPatternCtrl)
	ON_WM_CREATE()
	ON_COMMAND(ID_APP_ABOUT, AboutBox)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_PATTERN_NEWSTRIP, OnPatternNewstrip)
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_PASTE_INSERT, OnEditPasteInsert)
	ON_COMMAND(ID_EDIT_PASTE_MERGE, OnEditPasteMerge)
	ON_COMMAND(ID_EDIT_PASTE_OVERWRITE, OnEditPasteOverwrite)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectall)
	ON_COMMAND(ID_EDIT_INSERT, OnEditInsert)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERT, OnUpdateEditInsert)
	ON_COMMAND(ID_EDIT_MERGE_VARIATIONS, OnEditMergeVariations)
	ON_UPDATE_COMMAND_UI(ID_EDIT_MERGE_VARIATIONS, OnUpdateEditMergeVariations)
	ON_COMMAND(IDM_HELP_FINDER, OnHelpFinder)
	ON_COMMAND(ID_EDIT_DELETE_PART, OnEditDeletePart)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE_PART, OnUpdateEditDeletePart)
	ON_WM_PARENTNOTIFY()
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_INSERT, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_MERGE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_OVERWRITE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_QUANTIZE, OnEditQuantize)
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_MESSAGE(WM_OPEN_VARCHOICES, OnOpenVarChoicesEditor)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CPatternCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CPatternCtrl)
	// NOTE - ClassWizard will add and remove dispatch map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CPatternCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CPatternCtrl, COleControl)
	//{{AFX_EVENT_MAP(CPatternCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl Interface map

BEGIN_INTERFACE_MAP(CPatternCtrl, COleControl)
    INTERFACE_PART(CPatternCtrl, IID_IOleInPlaceActiveObject, MyOleInPlaceActiveObject)
    INTERFACE_PART(CPatternCtrl, IID_IDMUSProdEditor, Editor)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CPatternCtrl, "STYLEDESIGNER.PatternCtrl.1",
	0x3bd2ba09, 0x46e7, 0x11d0, 0x89, 0xac, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CPatternCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DPattern =
		{ 0x3bd2ba07, 0x46e7, 0x11d0, { 0x89, 0xac, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };
const IID BASED_CODE IID_DPatternEvents =
		{ 0x3bd2ba08, 0x46e7, 0x11d0, { 0x89, 0xac, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwPatternOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CPatternCtrl, IDS_PATTERN, _dwPatternOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::CPatternCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CPatternCtrl

BOOL CPatternCtrl::CPatternCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_PATTERN,
			IDB_PATTERN,
			afxRegApartmentThreading,
			_dwPatternOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::CPatternCtrl - Constructor

CPatternCtrl::CPatternCtrl()
{
/////////////////////////////////////////////////////////////////////////
// Following commented out to prevent 3 ASSERTS that occur
// because we are not building a typelib
//	InitializeIIDs(&IID_DPattern, &IID_DPatternEvents);
//
// Following code copied from InitializeIIDs (since we cannot override)
	m_piidPrimary = &IID_DPattern;
	m_piidEvents = &IID_DPatternEvents;
	EnableTypeLib();	// Needed to prevent ASSERT when closing editor
	InitStockEventMask();
	InitStockPropMask();
//////////////////////////////////////////////////////////////////////////

	m_pPatternDlg = NULL;
	m_hWndContainer = NULL;
	m_hMenuInPlace = NULL;
	m_hAcceleratorTable	= NULL;
	m_hKeyStatusBar = NULL;
	m_pToolBar = NULL;
	m_pPattern = NULL;
	// Initially, our doc window is inactive and the main window is active
	m_fDocWindowActive = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::~CPatternCtrl - Destructor

CPatternCtrl::~CPatternCtrl()
{
	RELEASE( m_pPattern );
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl IDMUSProdEditor implementation

/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::XEditor::AddRef

STDMETHODIMP_(ULONG) CPatternCtrl::XEditor::AddRef()
{
	METHOD_PROLOGUE_EX_( CPatternCtrl, Editor )

	return (ULONG)pThis->ExternalAddRef();
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::XEditor::Release

STDMETHODIMP_(ULONG) CPatternCtrl::XEditor::Release()
{
	METHOD_PROLOGUE_EX_( CPatternCtrl, Editor )

	return (ULONG)pThis->ExternalRelease();
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::XEditor::QueryInterface

STDMETHODIMP CPatternCtrl::XEditor::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_PROLOGUE_EX_( CPatternCtrl, Editor )

	return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::XEditor::AttachObjects implementation

HRESULT CPatternCtrl::XEditor::AttachObjects( IDMUSProdNode* pNode )
{
	METHOD_MANAGE_STATE( CPatternCtrl, Editor )

	ASSERT_VALID( pThis );
	ASSERT( pNode != NULL );

	pThis->m_pPattern = (CDirectMusicPattern *)pNode;
	pThis->m_pPattern->AddRef();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::XEditor::OnInitMenuFilePrint

HRESULT CPatternCtrl::XEditor::OnInitMenuFilePrint( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CPatternCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::XEditor::OnFilePrint

HRESULT CPatternCtrl::XEditor::OnFilePrint( void )
{
	METHOD_MANAGE_STATE( CPatternCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::XEditor::OnInitMenuFilePrintPreview

HRESULT CPatternCtrl::XEditor::OnInitMenuFilePrintPreview( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CPatternCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::XEditor::OnFilePrintPreview

HRESULT CPatternCtrl::XEditor::OnFilePrintPreview( void )
{
	METHOD_MANAGE_STATE( CPatternCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::XEditor::OnViewProperties

HRESULT CPatternCtrl::XEditor::OnViewProperties( void )
{
	METHOD_MANAGE_STATE( CPatternCtrl, Editor )

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	if( !pThis->m_pPattern )
	{
		return E_FAIL;
	}

	IDMUSProdPropSheet* pIPropSheet;
	if( FAILED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;
	if( pIPropSheet->IsShowing() == S_OK )
	{
		if( pThis->m_pPatternDlg && pThis->m_pPatternDlg->m_pIMIDIStripMgr )
		{
			BOOL fSetPropPage = FALSE;
			VARIANT var;
			if( SUCCEEDED( pThis->m_pPatternDlg->m_pITimelineCtl->GetTimelineProperty( TP_ACTIVESTRIP, &var ) ) )
			{
				IUnknown *punkStrip = V_UNKNOWN(&var);
				if( punkStrip )
				{
					IDMUSProdStrip *pIStrip;
					if( SUCCEEDED( punkStrip->QueryInterface( IID_IDMUSProdStrip, (void **) &pIStrip ) ) )
					{
						if( SUCCEEDED( pIStrip->OnWMMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_PROPERTIES, 0), 0L, 0L, 0L) ) )
						{
							hr = S_OK;
						}
						pIStrip->Release();
					}
					punkStrip->Release();
				}
			}
		}
		if( hr != S_OK )
		{
			pThis->m_pPattern->OnShowProperties();
		}
	}
	else
	{
		hr = S_OK;
	}

	RELEASE( pIPropSheet );

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::XEditor::OnF1Help

HRESULT CPatternCtrl::XEditor::OnF1Help( void )
{
	METHOD_MANAGE_STATE( CPatternCtrl, Editor )
	
    // Determine name of DMUSProd.exe help file
	CString strHelpFileName;

	if( theApp.GetHelpFileName( strHelpFileName ) )
	{
		strHelpFileName += "::/htm/PatternEditor.htm";
		::HtmlHelp( NULL, strHelpFileName, HH_DISPLAY_TOPIC, 0 );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::XMyOleInPlaceActiveObject implementation

STDMETHODIMP_(ULONG) CPatternCtrl::XMyOleInPlaceActiveObject::AddRef()
{
	METHOD_MANAGE_STATE( CPatternCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.AddRef();
}

STDMETHODIMP_(ULONG) CPatternCtrl::XMyOleInPlaceActiveObject::Release()
{
	METHOD_MANAGE_STATE( CPatternCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.Release();
}

STDMETHODIMP CPatternCtrl::XMyOleInPlaceActiveObject::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_MANAGE_STATE( CPatternCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.QueryInterface( iid, ppvObj );
}

STDMETHODIMP CPatternCtrl::XMyOleInPlaceActiveObject::GetWindow( HWND* lphwnd )
{
	METHOD_MANAGE_STATE( CPatternCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.GetWindow( lphwnd );
}

STDMETHODIMP CPatternCtrl::XMyOleInPlaceActiveObject::ContextSensitiveHelp( BOOL fEnterMode )
{
	METHOD_MANAGE_STATE( CPatternCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.ContextSensitiveHelp( fEnterMode );
}

STDMETHODIMP CPatternCtrl::XMyOleInPlaceActiveObject::TranslateAccelerator( LPMSG lpmsg )
{
	METHOD_MANAGE_STATE( CPatternCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );
	
	// Translate only if the timeline has focus
	if( pThis->m_pPatternDlg
	&&  pThis->m_pPatternDlg->m_pITimelineCtl )
	{
		HWND hwndFocus;
		hwndFocus = ::GetFocus();
		IOleInPlaceObjectWindowless* pIOleInPlaceObjectWindowless;

		pThis->m_pPatternDlg->m_pITimelineCtl->QueryInterface( IID_IOleWindow, (void**)&pIOleInPlaceObjectWindowless );
		if( pIOleInPlaceObjectWindowless )
		{
			HWND hwnd;
			if( pIOleInPlaceObjectWindowless->GetWindow(&hwnd) == S_OK )
			{
				if( hwndFocus == hwnd )
				{
					if( ::TranslateAccelerator(pThis->m_hWnd, pThis->m_hAcceleratorTable, lpmsg) )
					{
						RELEASE( pIOleInPlaceObjectWindowless );
						return S_OK;
					}
				}
			}
			RELEASE( pIOleInPlaceObjectWindowless );
		}
	}

//	OLEINPLACEFRAMEINFO frameInfo = pThis->m_frameInfo;
//	if( ::OleTranslateAccelerator(pThis->m_pInPlaceFrame, &frameInfo, lpmsg) == S_OK )
//	{
//		return S_OK;
//	}

	return pThis->m_xOleInPlaceActiveObject.TranslateAccelerator( lpmsg );
}

STDMETHODIMP CPatternCtrl::XMyOleInPlaceActiveObject::OnFrameWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CPatternCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	if( pThis->m_fDocWindowActive && pThis->m_pPattern )
	{
		pThis->m_pPattern->ActivateDialog( fActivate );
		
		if( fActivate )
		{
			// Check if property sheet is visible
			IDMUSProdPropSheet* pIPropSheet;

			if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
			{
				//  If the property sheet is visible
				if( pIPropSheet->IsShowing() == S_OK )
				{
					BOOL fSetPropPage = FALSE;
					VARIANT var;
					if( pIPropSheet->IsEqualPageManagerObject( pThis->m_pPattern ) != S_OK
					&&	SUCCEEDED( pThis->m_pPatternDlg->m_pITimelineCtl->GetTimelineProperty( TP_ACTIVESTRIP, &var ) ) )
					{
						IUnknown *punkStrip;
						punkStrip = V_UNKNOWN(&var);
						if( punkStrip )
						{
							IDMUSProdPropPageObject* pIDMUSProdPropPageObject;
							if( SUCCEEDED( punkStrip->QueryInterface( IID_IDMUSProdPropPageObject, (void**) &pIDMUSProdPropPageObject ) ) )
							{
								if( SUCCEEDED( pIDMUSProdPropPageObject->OnShowProperties() ) )
								{
									fSetPropPage = TRUE;
								}
								pIDMUSProdPropPageObject->Release();
							}
							punkStrip->Release();
						}
					}
					if( !fSetPropPage )
					{
						pThis->m_pPattern->OnShowProperties();
					}
				}
				pIPropSheet->Release();
			}
		}
	}

	//TRACE("OnFrameWindowActivate %d %d\n", pThis->m_fFrameWindowActive, pThis->m_fDocWindowActive );

	return pThis->m_xOleInPlaceActiveObject.OnFrameWindowActivate( fActivate );
}

STDMETHODIMP CPatternCtrl::XMyOleInPlaceActiveObject::OnDocWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CPatternCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	if( pThis->m_pPattern )
	{
		pThis->m_pPattern->ActivateDialog( fActivate );
		
		if( fActivate )
		{
			// Check if property sheet is visible
			IDMUSProdPropSheet* pIPropSheet;

			if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
			{
				//  If the property sheet is visible
				if( pIPropSheet->IsShowing() == S_OK )
				{
					BOOL fSetPropPage = FALSE;
					VARIANT var;
					if( pIPropSheet->IsEqualPageManagerObject( pThis->m_pPattern ) != S_OK
					&&	SUCCEEDED( pThis->m_pPatternDlg->m_pITimelineCtl->GetTimelineProperty( TP_ACTIVESTRIP, &var ) ) )
					{
						IUnknown *punkStrip;
						punkStrip = V_UNKNOWN(&var);
						if( punkStrip )
						{
							IDMUSProdPropPageObject* pIDMUSProdPropPageObject;
							if( SUCCEEDED( punkStrip->QueryInterface( IID_IDMUSProdPropPageObject, (void**) &pIDMUSProdPropPageObject ) ) )
							{
								if( SUCCEEDED( pIDMUSProdPropPageObject->OnShowProperties() ) )
								{
									fSetPropPage = TRUE;
								}
								pIDMUSProdPropPageObject->Release();
							}
							punkStrip->Release();
						}
					}
					if( !fSetPropPage )
					{
						pThis->m_pPattern->OnShowProperties();
					}
				}
				pIPropSheet->Release();
			}
		}
	}

	pThis->m_fDocWindowActive = fActivate;
	//TRACE("OnDocWindowActivate %d %d\n", pThis->m_fFrameWindowActive, pThis->m_fDocWindowActive );

	return pThis->m_xOleInPlaceActiveObject.OnDocWindowActivate( fActivate );
}

STDMETHODIMP CPatternCtrl::XMyOleInPlaceActiveObject::ResizeBorder(
	LPCRECT, LPOLEINPLACEUIWINDOW, BOOL fFrameWindow )
{
	METHOD_MANAGE_STATE( CPatternCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

    if( fFrameWindow == TRUE )
	{
		pThis->OnShowToolBars();
	}

	return S_OK;
}

STDMETHODIMP CPatternCtrl::XMyOleInPlaceActiveObject::EnableModeless( BOOL fEnable )
{
	METHOD_MANAGE_STATE( CPatternCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.EnableModeless( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::OnDraw - Drawing function

void CPatternCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	UNREFERENCED_PARAMETER(pdc);
	UNREFERENCED_PARAMETER(rcBounds);
	UNREFERENCED_PARAMETER(rcInvalid);
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::DoPropExchange - Persistence support

void CPatternCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::OnOpenVarChoicesEditor

LRESULT CPatternCtrl::OnOpenVarChoicesEditor( WPARAM wParam, LPARAM lParam )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(wParam);

	ASSERT( m_pPattern != NULL );

	CDirectMusicPartRef* pPartRef = (CDirectMusicPartRef *)lParam;

	m_pPattern->OpenVarChoicesEditor( pPartRef );

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CPatternAbout dialog used for Pattern Editor About Box

class CPatternAbout : public CDialog
{
public:
	CPatternAbout();

// Dialog Data
	//{{AFX_DATA(CPatternAbout)
	enum { IDD = IDD_ABOUTBOX_PATTERN };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatternAbout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CPatternAbout)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CPatternAbout::CPatternAbout() : CDialog(CPatternAbout::IDD)
{
	//{{AFX_DATA_INIT(CPatternAbout)
	//}}AFX_DATA_INIT
}

void CPatternAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatternAbout)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPatternAbout, CDialog)
	//{{AFX_MSG_MAP(CPatternAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CPatternAbout::OnInitDialog() 
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

			AfxFormatString1( strFileVersion, IDS_PATTERN_VERSION_TEXT, achFileVersion );
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
// CPatternCtrl::AboutBox - Display an "About" box to the user

void CPatternCtrl::AboutBox()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPatternAbout dlgAbout;
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl message handlers

/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::OnCreate

int CPatternCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
	m_hMenuInPlace = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDR_PATTERN_EDITOR) );

	// Load control's accelerator table
	m_hAcceleratorTable = ::LoadAccelerators( theApp.m_hInstance, MAKEINTRESOURCE(IDR_PATTERN_ACCELS) );

	// Create control's toolbar
//	CWnd* pWndParent = CWnd::FromHandle( m_hWndContainer );
//	if( pWndParent )
//	{
//		m_pToolBar = new CToolBar;
//		m_pToolBar->Create( pWndParent );
//		m_pToolBar->LoadToolBar( IDR_PATTERN_EDITOR );
//		m_pToolBar->SetBarStyle( m_pToolBar->GetBarStyle() | CBRS_SIZE_DYNAMIC );
//		m_pToolBar->SetOwner( this );
//	}
	
	// Create control's dialog
	m_pPatternDlg = new CPatternDlg;
	if( m_pPatternDlg == NULL )
	{
		return -1;
	}

	ASSERT( m_pPattern != NULL );
	ASSERT( m_pPattern->m_pStyle != NULL );

	m_pPattern->m_pPatternCtrl = this;

	m_pPatternDlg->m_pStyle = m_pPattern->m_pStyle;
	m_pPatternDlg->m_pPatternCtrl = this;
	if( 0 == m_pPatternDlg->Create( NULL, "WindowName", WS_CHILD | WS_VISIBLE, CRect( 0, 0, 800, 400 ), this, 888, NULL ))
	{
		return -1;
	}
	m_pPatternDlg->OnInitialUpdate();
	
	// By default, we're active when opened
	m_fDocWindowActive = TRUE;
	m_pPattern->ActivateDialog( TRUE );

	if( m_pPatternDlg
	&&  m_pPatternDlg->m_pIMIDIStripMgr )
	{
		// Make sure we have Pattern design info
		// or chord strip undo will not work
		if( m_pPattern->m_pPatternDesignData == NULL )
		{
			// Don't let GetPatternFromMIDIStripMgr() method turn on modified flag
			BOOL fOrigModified = m_pPattern->m_fModified;

			m_pPattern->m_fDontUpdateDirectMusicDuringLoad = TRUE;
			m_pPattern->GetPatternFromMIDIStripMgr();
			m_pPattern->m_fDontUpdateDirectMusicDuringLoad = FALSE;
			
			m_pPattern->SetModified( fOrigModified );
		}
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::OnDestroy

void CPatternCtrl::OnDestroy() 
{
	if( m_pPatternDlg && m_pPatternDlg->m_pIMIDIStripMgr )
	{
		m_pPatternDlg->m_pIMIDIStripMgr->OnUpdate( STYLE_PatternWindowClose, 0xFFFFFFFF, NULL );
	}

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
	
	if( m_pToolBar )
	{
		delete m_pToolBar;	
		m_pToolBar = NULL;
	}

	ASSERT( m_pPattern != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIConductor != NULL );
	if( theApp.m_pStyleComponent && theApp.m_pStyleComponent->m_pIConductor )
	{
		theApp.m_pStyleComponent->m_pIConductor->SetBtnStates( m_pPattern, 0 );	
	}

	m_pPattern->m_pPatternCtrl = NULL;

	// Deactivate the dialog
	m_pPattern->ActivateDialog( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::OnGetInPlaceMenu

HMENU CPatternCtrl::OnGetInPlaceMenu() 
{
	return m_hMenuInPlace;
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::OnHideToolBars

void CPatternCtrl::OnHideToolBars() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	if( m_pPatternDlg
	&&  m_pPatternDlg->m_pITimelineCtl )
	{
		HANDLE hKey = NULL;

		// hKey is NULL - this will stop RealTime display
		m_pPatternDlg->m_pITimelineCtl->SetParam( GUID_TimelineShowRealTime, 0xFFFFFFFF, 0, 0, (void *)&hKey );
	}

	theApp.m_pStyleComponent->m_pIFramework->RestoreStatusBar( m_hKeyStatusBar );
	m_hKeyStatusBar = NULL;

	COleControl::OnHideToolBars();
	return;
/*
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
*/
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::OnShowToolBars

void CPatternCtrl::OnShowToolBars() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pInPlaceFrame == NULL )
	{
		return;
	}

	m_pInPlaceFrame->SetActiveObject( &m_xMyOleInPlaceActiveObject, NULL );

	if( m_pInPlaceDoc != NULL )
	{
		m_pInPlaceDoc->SetActiveObject( &m_xMyOleInPlaceActiveObject, NULL );
	}

	// Set up the Status Bar
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	// Create status bar panes if they don't already exist
	if( m_hKeyStatusBar == NULL )
	{
		theApp.m_pStyleComponent->m_pIFramework->SetNbrStatusBarPanes( 2, SBLS_EDITOR, &m_hKeyStatusBar );
		theApp.m_pStyleComponent->m_pIFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 0, SBS_SUNKEN, 20 );
		theApp.m_pStyleComponent->m_pIFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 1, SBS_SUNKEN, 20 );

		// Tell the timeline to update the RealTime display in the status bar
		if( m_pPatternDlg
		&&  m_pPatternDlg->m_pITimelineCtl )
		{
			m_pPatternDlg->m_pITimelineCtl->SetParam( GUID_TimelineShowRealTime, 0xFFFFFFFF, 0, 0, (void *)&m_hKeyStatusBar );
		}
	}

	COleControl::OnShowToolBars();
	return;
/*
	// Show the ToolBar
	if( m_pToolBar )
	{
		BORDERWIDTHS border;
		CRect rectBorder;
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
*/
}

/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::OnSize

void CPatternCtrl::OnSize(UINT nType, int cx, int cy) 
{
	COleControl::OnSize(nType, cx, cy);
	m_pPatternDlg->MoveWindow( 0, 0, cx, cy, FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::OnEraseBkgnd

BOOL CPatternCtrl::OnEraseBkgnd( CDC* pDC ) 
{
	UNREFERENCED_PARAMETER(pDC);

	return FALSE;
}

void CPatternCtrl::OnPatternNewstrip() 
{
	IMIDIMgr* pIMIDIMgr;
	if( SUCCEEDED( m_pPatternDlg->m_pIMIDIStripMgr->QueryInterface( IID_IMIDIMgr, (void**)&pIMIDIMgr ) ) )
	{
		pIMIDIMgr->AddNewStrip();
		pIMIDIMgr->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl::OnSetFocus

void CPatternCtrl::OnSetFocus( CWnd* pOldWnd ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIConductor != NULL );
	ASSERT( m_pPattern != NULL );
	ASSERT( m_pPattern->m_pStyle != NULL );

	COleControl::OnSetFocus( pOldWnd );

	if( m_pPattern == m_pPattern->m_pIDocRootNode )
	{
		// if the Pattern isn't part of a Style, SetActiveTransport to NULL so
		// the Conductor knows that this isn't a playable Pattern (until
		// the music engine is updated to support Pattern w/o style playback
		theApp.m_pStyleComponent->m_pIConductor->SetActiveTransport( NULL, 0 );	
	}
	else
	{
		DWORD dwButtonState = BS_AUTO_UPDATE;
		if( m_pPatternDlg != NULL && m_pPatternDlg->m_punkMIDIStripMgr != NULL )
		{
			dwButtonState |= BS_RECORD_ENABLED;
			if( m_pPattern && m_pPattern->m_fRecordPressed )
			{
				dwButtonState |= BS_RECORD_CHECKED;
			}
		}
		theApp.m_pStyleComponent->m_pIConductor->SetActiveTransport( m_pPattern, dwButtonState );	
		theApp.m_pStyleComponent->m_pIConductor->SetTempo( m_pPattern, m_pPattern->m_pStyle->m_dblTempo, TRUE );
	}

	// Set the focus to the timeline
	if( m_pPatternDlg
	&&  m_pPatternDlg->m_pITimelineCtl )
	{
		IOleInPlaceObjectWindowless* pIOleInPlaceObjectWindowless;

		m_pPatternDlg->m_pITimelineCtl->QueryInterface( IID_IOleWindow, (void**)&pIOleInPlaceObjectWindowless );
		if( pIOleInPlaceObjectWindowless )
		{
			HWND hwnd;
			if( pIOleInPlaceObjectWindowless->GetWindow(&hwnd) == S_OK )
			{
				::SetFocus(hwnd);
			}

			RELEASE( pIOleInPlaceObjectWindowless );
		}
	}
}

void CPatternCtrl::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	pCmdUI->Enable(TimelineEditAvailable(ID_EDIT_CUT));
}

void CPatternCtrl::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	pCmdUI->Enable(TimelineEditAvailable(ID_EDIT_COPY));
}

void CPatternCtrl::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	pCmdUI->Enable(TimelineEditAvailable(ID_EDIT_PASTE));
}

void CPatternCtrl::OnUpdateEditInsert(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	pCmdUI->Enable(TimelineEditAvailable(ID_EDIT_INSERT));
}

void CPatternCtrl::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	pCmdUI->Enable(TimelineEditAvailable(ID_EDIT_DELETE));
}

BOOL CPatternCtrl::TimelineEditAvailable(WPARAM wId)
{
	HRESULT					hr;
	IDMUSProdTimeline		*pITimeline;
	IDMUSProdTimelineEdit	*pITimelineEdit;

	ASSERT(m_pPatternDlg != NULL);
	if(m_pPatternDlg != NULL)
	{
		pITimeline = m_pPatternDlg->m_pITimelineCtl;
		ASSERT(pITimeline != NULL);
		if(pITimeline != NULL)
		{
			pITimeline->AddRef();
			if(SUCCEEDED(pITimeline->QueryInterface(IID_IDMUSProdTimelineEdit, (void **) &pITimelineEdit)))
			{
				switch(wId)
				{
				case ID_EDIT_CUT:
					hr = pITimelineEdit->CanCut();
					break;
				case ID_EDIT_COPY:
					hr = pITimelineEdit->CanCopy();
					break;
//				case ID_EDIT_PASTE_INSERT:
				case ID_EDIT_PASTE_MERGE:
				case ID_EDIT_PASTE_OVERWRITE:
				case ID_EDIT_PASTE:
					hr = pITimelineEdit->CanPaste(NULL);
					break;
				case ID_EDIT_INSERT:
					hr = pITimelineEdit->CanInsert();
					break;
				case ID_EDIT_DELETE:
					hr = pITimelineEdit->CanDelete();
					break;
				case ID_EDIT_SELECT_ALL:
					hr = S_OK;
					break;
				default:
					hr = S_FALSE;
				}

				RELEASE( pITimelineEdit );
			}

			RELEASE( pITimeline );

			if( hr == S_OK )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CPatternCtrl::OnUpdateEditRedo( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pPattern != NULL );

	CString strRedo;
	CString strRedoAccel;

	strRedo.LoadString( IDS_REDO );
	strRedoAccel.LoadString( IDS_REDO_ACCEL );

	BOOL fEnable = FALSE;

	if( m_pPattern )
	{
		TCHAR achText[256];

		ASSERT( m_pPattern->m_pUndoMgr != NULL );

		if( m_pPattern->m_pUndoMgr )
		{
			if( m_pPattern->m_pUndoMgr->GetRedo(achText, 256) )
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

void CPatternCtrl::OnUpdateEditUndo( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pPattern != NULL );

	CString strUndo;
	CString strUndoAccel;

	strUndo.LoadString( IDS_UNDO );
	strUndoAccel.LoadString( IDS_UNDO_ACCEL );

	BOOL fEnable = FALSE;

	if( m_pPattern )
	{
		TCHAR achText[256];

		ASSERT( m_pPattern->m_pUndoMgr != NULL );

		if( m_pPattern->m_pUndoMgr )
		{
			if( m_pPattern->m_pUndoMgr->GetUndo(achText, 256) )
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

void CPatternCtrl::OnEditCut() 
{
	SendEditToTimeline(ID_EDIT_CUT);
}

void CPatternCtrl::OnEditCopy() 
{
	SendEditToTimeline(ID_EDIT_COPY);
}

void CPatternCtrl::OnEditPaste() 
{
	SendEditToTimeline(ID_EDIT_PASTE);
}

void CPatternCtrl::OnEditPasteInsert() 
{
	return;

//	SendEditToTimeline(ID_EDIT_PASTE_INSERT);
}

void CPatternCtrl::OnEditPasteMerge() 
{
	SendEditToTimeline( ID_EDIT_PASTE_MERGE );
}

void CPatternCtrl::OnEditPasteOverwrite() 
{
	SendEditToTimeline( ID_EDIT_PASTE_OVERWRITE );
}

void CPatternCtrl::OnEditInsert() 
{
	SendEditToTimeline(ID_EDIT_INSERT);
}

void CPatternCtrl::OnEditDelete() 
{
	SendEditToTimeline(ID_EDIT_DELETE);
}

void CPatternCtrl::OnEditSelectall() 
{
	SendEditToTimeline(ID_EDIT_SELECT_ALL);
}

void CPatternCtrl::SendEditToTimeline(WPARAM wId)
{
	HRESULT					hr;
	BOOL					bAvailable = FALSE;
	IDMUSProdTimeline		*pITimeline;
	IDMUSProdTimelineEdit	*pITimelineEdit;

	ASSERT(m_pPatternDlg != NULL);
	if(m_pPatternDlg != NULL)
	{
		pITimeline = m_pPatternDlg->m_pITimelineCtl;
		ASSERT( pITimeline != NULL);
		if( pITimeline != NULL)
		{
			pITimeline->AddRef();
			if(SUCCEEDED(pITimeline->QueryInterface(IID_IDMUSProdTimelineEdit, (void **) &pITimelineEdit)))
			{
				bAvailable = TimelineEditAvailable(wId);
				ASSERT(bAvailable);
				if(bAvailable)
				{
					if( m_pPattern )
					{
						// If the selecte time range is not empty, and there is more than one
						// gutter selected strip that can paste, set m_pPattern->m_fInGroupEdit
						long lStart = -1, lEnd = -1;
						pITimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_MS, &lStart );
						pITimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_MS, &lEnd );
						if( lStart != lEnd )
						{
							// Count number of gutter selected strips that can edit
							DWORD dwStripIndex = 0;
							long lCount = 0;
							IDMUSProdStrip *pStrip;
							while( S_OK == pITimeline->EnumStrip( dwStripIndex, &pStrip ) )
							{
								// Check for gutter selection
								VARIANT varGutter;
								if( SUCCEEDED( pITimeline->StripGetTimelineProperty( pStrip, STP_GUTTER_SELECTED, &varGutter ) )
								&&	(V_BOOL( &varGutter ) == TRUE) )
								{
									IDMUSProdTimelineEdit *pITimelineEdit;
									if( SUCCEEDED( pStrip->QueryInterface( IID_IDMUSProdTimelineEdit,
															  (void**) &pITimelineEdit ) ) )
									{
										switch( wId )
										{
										case ID_EDIT_CUT:
											if( S_OK == pITimelineEdit->CanCut() )
											{
												lCount++;
											}
											break;
											/* Don't care about copy
										case ID_EDIT_COPY:
											if( S_OK == pITimelineEdit->CanCopy() )
											{
												lCount++;
											}
											break;
											*/
						//				case ID_EDIT_PASTE_INSERT:
										case ID_EDIT_PASTE_MERGE:
										case ID_EDIT_PASTE_OVERWRITE:
										case ID_EDIT_PASTE:
											if( S_OK == pITimelineEdit->CanPaste(NULL) )
											{
												lCount++;
											}
											break;
										case ID_EDIT_INSERT:
											if( S_OK == pITimelineEdit->CanInsert() )
											{
												lCount++;
											}
											break;
										case ID_EDIT_DELETE:
											if( S_OK == pITimelineEdit->CanDelete() )
											{
												lCount++;
											}
											break;
											/* Don't care about select all
										case ID_EDIT_SELECT_ALL:
											if( S_OK == pITimelineEdit->CanPaste() )
											{
												lCount++;
											}
											hr = S_OK;
											break;
											*/
										}
										pITimelineEdit->Release();
									}
								}
								pStrip->Release();
								dwStripIndex++;
							}

							if( lCount > 1 )
							{
								m_pPattern->m_fInGroupEdit = TRUE;
							}
						}
					}

					switch(wId)
					{
					case ID_EDIT_CUT:
						hr = pITimelineEdit->Cut(NULL);
						break;
					case ID_EDIT_COPY:
						hr = pITimelineEdit->Copy(NULL);
						break;
					case ID_EDIT_PASTE:
					case ID_EDIT_PASTE_MERGE:
						pITimeline->SetPasteType(TL_PASTE_MERGE);
						hr = pITimelineEdit->Paste(NULL);
						break;
					case ID_EDIT_PASTE_OVERWRITE:
						pITimeline->SetPasteType(TL_PASTE_OVERWRITE);
						hr = pITimelineEdit->Paste(NULL);
						break;
					case ID_EDIT_INSERT:
						hr = pITimelineEdit->Insert();
						break;
					case ID_EDIT_DELETE:
						hr = pITimelineEdit->Delete();
						break;
					case ID_EDIT_SELECT_ALL:
						hr = pITimelineEdit->SelectAll();
						break;
					default:
						break;
					}

					if( m_pPattern )
					{
						m_pPattern->m_fInGroupEdit = FALSE;
					}
				}

				RELEASE( pITimelineEdit );
			}

			RELEASE( pITimeline );
		}
	}
}

void CPatternCtrl::OnEditRedo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pPattern != NULL );
	if( m_pPattern )
	{
		ASSERT( m_pPattern->m_pUndoMgr != NULL );
		if( m_pPattern->m_pUndoMgr )
		{
			// Save the current timeline settings
			m_pPattern->SyncTimelineSettings();

			ASSERT( m_pPattern->m_pStyle != NULL );
			if( m_pPattern->m_pStyle )
			{
				m_pPattern->m_pStyle->m_fPChannelChange = TRUE;
			}
			m_pPattern->m_fInRedo = TRUE;		// Use this flag for undo/redo
			m_pPattern->SetModified( TRUE );	// Call before Redo() to set m_fSendChangeNotification flag
			m_pPattern->m_pUndoMgr->Redo( m_pPattern );
			m_pPattern->m_fInRedo = FALSE;
		}
	}
}

void CPatternCtrl::OnEditUndo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pPattern != NULL );
	if( m_pPattern )
	{
		ASSERT( m_pPattern->m_pUndoMgr != NULL);
		if( m_pPattern->m_pUndoMgr )
		{
			// Save the current timeline settings
			m_pPattern->SyncTimelineSettings();

			ASSERT( m_pPattern->m_pStyle != NULL );
			if( m_pPattern->m_pStyle )
			{
				m_pPattern->m_pStyle->m_fPChannelChange = TRUE;
			}
			m_pPattern->m_fInUndo = TRUE;
			m_pPattern->SetModified( TRUE );	// Call before Undo() to set m_fSendChangeNotification flag
			m_pPattern->m_pUndoMgr->Undo( m_pPattern );
			m_pPattern->m_fInUndo = FALSE;
		}
	}
}

void CPatternCtrl::OnEditMergeVariations() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdStripMgr *pIStripMgr;
	IMIDIMgr		  *pIMIDIMgr;

	ASSERT(m_pPatternDlg != NULL);
	if(m_pPatternDlg != NULL)
	{
		pIStripMgr = m_pPatternDlg->m_pIMIDIStripMgr;
		ASSERT( pIStripMgr != NULL);
		if( pIStripMgr != NULL)
		{
			pIStripMgr->AddRef();
			if(SUCCEEDED(pIStripMgr->QueryInterface(IID_IMIDIMgr, (void **) &pIMIDIMgr)))
			{
				pIMIDIMgr->MergeVariations( TRUE );
				RELEASE( pIMIDIMgr );
			}

			RELEASE( pIStripMgr );
		}
	}
}

void CPatternCtrl::OnUpdateEditMergeVariations(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdStripMgr	*pIStripMgr;
	IMIDIMgr			*pIMIDIMgr;
	BOOL				fEnable = FALSE;

	ASSERT(m_pPatternDlg != NULL);
	if(m_pPatternDlg != NULL)
	{
		pIStripMgr = m_pPatternDlg->m_pIMIDIStripMgr;
		ASSERT( pIStripMgr != NULL);
		if( pIStripMgr != NULL)
		{
			pIStripMgr->AddRef();
			if(SUCCEEDED(pIStripMgr->QueryInterface(IID_IMIDIMgr, (void **) &pIMIDIMgr)))
			{
				if( pIMIDIMgr->MergeVariations( FALSE ) == S_OK )
				{
					fEnable = TRUE;
				}
				RELEASE( pIMIDIMgr );
			}

			RELEASE( pIStripMgr );
		}
	}
	pCmdUI->Enable( fEnable );
}

void CPatternCtrl::OnHelpFinder() 
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

void CPatternCtrl::OnEditDeletePart() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	IMIDIMgr *pIMIDIMgr;
	if ( m_pPatternDlg != NULL )
	{
		if ( m_pPatternDlg->m_pIMIDIStripMgr != NULL )
		{
			if(SUCCEEDED(m_pPatternDlg->m_pIMIDIStripMgr->QueryInterface( IID_IMIDIMgr, (void**) &pIMIDIMgr )))
			{
				pIMIDIMgr->DeleteTrack();
				RELEASE( pIMIDIMgr );
			}
		}
	}
}

void CPatternCtrl::OnUpdateEditDeletePart(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Ask the MIDIMgr if we can delete a track
	BOOL fCanDelete = FALSE;
	IMIDIMgr *pIMIDIMgr;
	if ( m_pPatternDlg != NULL )
	{
		if ( m_pPatternDlg->m_pIMIDIStripMgr != NULL )
		{
			if(SUCCEEDED(m_pPatternDlg->m_pIMIDIStripMgr->QueryInterface( IID_IMIDIMgr, (void**) &pIMIDIMgr )))
			{
				if( pIMIDIMgr->CanDeleteTrack() == S_OK )
				{
					// Yes, we can
					fCanDelete = TRUE;
				}
				RELEASE( pIMIDIMgr );
			}
		}
	}

	pCmdUI->Enable( fCanDelete );
}

void CPatternCtrl::OnParentNotify(UINT message, LPARAM lParam) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIConductor != NULL );
	ASSERT( m_pPattern != NULL );
	ASSERT( m_pPattern->m_pStyle != NULL );

	COleControl::OnParentNotify(message, lParam);
	
	// If the user pressed the left or right mouse buttons in our control,
	// change the active transport to this segment
	if( (message == WM_LBUTTONDOWN) ||
		(message == WM_RBUTTONDOWN) )
	{
		if( m_pPattern == m_pPattern->m_pIDocRootNode )
		{
			// if the Pattern isn't part of a Style, SetActiveTransport to NULL so
			// the Conductor knows that this isn't a playable Pattern (until
			// the music engine is updated to support Pattern w/o style playback
			theApp.m_pStyleComponent->m_pIConductor->SetActiveTransport( NULL, 0 );	
		}
		else
		{
			DWORD dwButtonState = BS_AUTO_UPDATE;
			if( m_pPatternDlg != NULL && m_pPatternDlg->m_punkMIDIStripMgr != NULL )
			{
				dwButtonState |= BS_RECORD_ENABLED;
				if( m_pPattern && m_pPattern->m_fRecordPressed )
				{
					dwButtonState |= BS_RECORD_CHECKED;
				}
			}
			theApp.m_pStyleComponent->m_pIConductor->SetActiveTransport( m_pPattern, dwButtonState );	
			theApp.m_pStyleComponent->m_pIConductor->SetTempo( m_pPattern, m_pPattern->m_pStyle->m_dblTempo, TRUE );
		}
	}
}

void SendCommandToTimeline( int nID, IDMUSProdTimeline *pITimelineCtl )
{
	VARIANT var;
	if( SUCCEEDED( pITimelineCtl->GetTimelineProperty( TP_ACTIVESTRIP, &var ) ) )
	{
		if( V_UNKNOWN(&var) )
		{
			IDMUSProdStrip *pIStrip;
			if( SUCCEEDED( V_UNKNOWN(&var)->QueryInterface( IID_IDMUSProdStrip, (void**) &pIStrip ) ) )
			{
				pIStrip->OnWMMessage( WM_COMMAND, nID, 0, 0, 0 );
				pIStrip->Release();
			}
			V_UNKNOWN(&var)->Release();
		}
	}
}

void CPatternCtrl::OnEditQuantize() 
{
	// ID_EDIT_QUANTIZE must have the same value as in the MIDIStripMgr project
	SendCommandToTimeline( ID_EDIT_QUANTIZE, m_pPatternDlg->m_pITimelineCtl );
}
