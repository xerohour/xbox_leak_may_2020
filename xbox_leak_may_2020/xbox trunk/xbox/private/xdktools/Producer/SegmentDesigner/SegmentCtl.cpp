// SegmentCtl.cpp : Implementation of the CSegmentCtrl ActiveX Control class.

#include "stdafx.h"
#include "SegmentDesignerDLL.h"
#include "SegmentCtl.h"
#include "SegmentAboutBox.h"
#include "SegmentPpg.h"
#include "SegmentRef.h"
#include "AddTrackDlg.h"
#include "Track.h"
#include "SegmentGuids.h"
#include "UndoMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CSegmentCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CSegmentCtrl, COleControl)
	ON_MESSAGE(WM_WNDACTIVATE, OnWndActivate)
	//{{AFX_MSG_MAP(CSegmentCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE_MERGE, OnEditPasteMerge)
	ON_COMMAND(ID_EDIT_PASTE_OVERWRITE, OnEditPasteOverwrite)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectall)
	ON_COMMAND(ID_EDIT_INSERT, OnEditInsert)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERT, OnUpdateEditInsert)
	ON_WM_ACTIVATE()
	ON_WM_KILLFOCUS()
	ON_COMMAND(ID_SEGMENT_DELETE_TRACK, OnSegmentDeleteTrack)
	ON_UPDATE_COMMAND_UI(ID_SEGMENT_DELETE_TRACK, OnUpdateSegmentDeleteTrack)
	ON_COMMAND(ID_SEGMENT_NEWTRACK, OnSegmentNewtrack)
	ON_COMMAND(IDM_HELP_FINDER, OnHelpFinder)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_MERGE, OnUpdateEditPasteMerge)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_OVERWRITE, OnUpdateEditPasteOverwrite)
	ON_WM_PARENTNOTIFY()
	ON_COMMAND(ID_EDIT_DELETE_TRACK, OnSegmentDeleteTrack)
	ON_COMMAND(ID_EDIT_ADD_TRACK, OnSegmentNewtrack)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CSegmentCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CSegmentCtrl)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CSegmentCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CSegmentCtrl, COleControl)
	//{{AFX_EVENT_MAP(CSegmentCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl Interface map

BEGIN_INTERFACE_MAP(CSegmentCtrl, COleControl)
    INTERFACE_PART(CSegmentCtrl, IID_IOleInPlaceActiveObject, MyOleInPlaceActiveObject)
    INTERFACE_PART(CSegmentCtrl, IID_IDMUSProdEditor, Editor)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CSegmentCtrl, 1)
	UNREFERENCED_PARAMETER(pIDs);
//	PROPPAGEID(CSegmentPropPage::guid)
END_PROPPAGEIDS(CSegmentCtrl) 


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CSegmentCtrl, "SEGMENTDESIGNER.SegmentCtrl.1",
	0xdfce860e, 0xa6fa, 0x11d1, 0x88, 0x81, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CSegmentCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DCSegmentCtrl =
		{ 0xdfce860c, 0xa6fa, 0x11d1, { 0x88, 0x81, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15 } };
const IID BASED_CODE IID_DCSegmentCtrlEvents =
		{ 0xdfce860d, 0xa6fa, 0x11d1, { 0x88, 0x81, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwSegmentOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CSegmentCtrl, IDS_SEGMENT, _dwSegmentOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl::CSegmentCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CSegmentCtrl

BOOL CSegmentCtrl::CSegmentCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_SEGMENT,
			IDB_SEGMENT,
			afxRegApartmentThreading,
			_dwSegmentOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl::CSegmentCtrl - Constructor

CSegmentCtrl::CSegmentCtrl()
{
/////////////////////////////////////////////////////////////////////////
// Following commented out to prevent 3 ASSERTS that occur
// because we are not building a typelib
//	InitializeIIDs(&IID_DCSegmentCtrl, &IID_DCSegmentCtrlEvents);
//
// Following code copied from InitializeIIDs (since we cannot override)
	m_piidPrimary = &IID_DCSegmentCtrl;
	m_piidEvents = &IID_DCSegmentCtrlEvents;
	EnableTypeLib();	// Needed to prevent ASSERT when closing editor
	InitStockEventMask();
	InitStockPropMask();
//////////////////////////////////////////////////////////////////////////

	m_pSegmentDlg		= NULL;

	m_hWndContainer		= NULL;
	m_hMenuInPlace		= NULL;
	m_hAcceleratorTable	= NULL;
	m_hKeyStatusBar		= NULL;
	m_pSegment			= NULL;
	m_fDocWindowActive	= FALSE;
}



/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl::~CSegmentCtrl - Destructor

CSegmentCtrl::~CSegmentCtrl()
{
	ASSERT(m_pSegmentDlg);
	delete m_pSegmentDlg;
	if(m_pSegment)
	{
		m_pSegment->Release();
		m_pSegment = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl IDMUSProdEditor implementation

/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl::XEditor::AddRef

STDMETHODIMP_(ULONG) CSegmentCtrl::XEditor::AddRef()
{
	METHOD_PROLOGUE_EX_( CSegmentCtrl, Editor )

	return (ULONG)pThis->ExternalAddRef();
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl::XEditor::Release

STDMETHODIMP_(ULONG) CSegmentCtrl::XEditor::Release()
{
	METHOD_PROLOGUE_EX_( CSegmentCtrl, Editor )

	return (ULONG)pThis->ExternalRelease();
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl::XEditor::QueryInterface

STDMETHODIMP CSegmentCtrl::XEditor::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_PROLOGUE_EX_( CSegmentCtrl, Editor )

	return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl::XEditor implementation

/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl::XEditor::AttachObjects

HRESULT CSegmentCtrl::XEditor::AttachObjects( IDMUSProdNode* pINode )
{
	METHOD_MANAGE_STATE( CSegmentCtrl, Editor )

	ASSERT_VALID( pThis );
	ASSERT( pINode != NULL );

	GUID guidNode;

	pINode->GetNodeId( &guidNode );
	if( ::IsEqualGUID( guidNode, GUID_SegmentRefNode ) )
	{
		CSegmentRef* pSegmentRef = (CSegmentRef *)pINode;
		ASSERT( pSegmentRef->m_pSegment != NULL );
		pThis->m_pSegment = pSegmentRef->m_pSegment;
	}
	else
	{
		pThis->m_pSegment = (CSegment *)pINode;
	}

	ASSERT( pThis->m_pSegment != NULL );
	pThis->m_pSegment->AddRef();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl::XEditor::OnInitMenuFilePrint

HRESULT CSegmentCtrl::XEditor::OnInitMenuFilePrint( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CSegmentCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl::XEditor::OnFilePrint

HRESULT CSegmentCtrl::XEditor::OnFilePrint( void )
{
	METHOD_MANAGE_STATE( CSegmentCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl::XEditor::OnInitMenuFilePrintPreview

HRESULT CSegmentCtrl::XEditor::OnInitMenuFilePrintPreview( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CSegmentCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl::XEditor::OnFilePrintPreview

HRESULT CSegmentCtrl::XEditor::OnFilePrintPreview( void )
{
	METHOD_MANAGE_STATE( CSegmentCtrl, Editor )

	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl::XEditor::OnViewProperties

HRESULT CSegmentCtrl::XEditor::OnViewProperties()
{
	METHOD_MANAGE_STATE( CSegmentCtrl, Editor )

	IDMUSProdStrip		*pIStrip = NULL;
	IDMUSProdTimeline	*pTimeline = NULL;
	VARIANT				var;
	HRESULT				hr = E_FAIL;

	if( !pThis->m_pSegmentDlg )
	{
		goto ON_ERROR;
	}
	if(FAILED(pThis->m_pSegmentDlg->GetTimeline(&pTimeline)))
	{
		goto ON_ERROR;
	}

	// Determine which strip if any is active
	if(FAILED(pTimeline->GetTimelineProperty(TP_ACTIVESTRIP, &var)))
	{
		goto ON_ERROR;
	}

	ASSERT(var.vt == VT_UNKNOWN);
	if(V_UNKNOWN(&var) == NULL)
	{
		goto ON_ERROR;
	}

	if(FAILED(V_UNKNOWN(&var)->QueryInterface(IID_IDMUSProdStrip, (void **) &pIStrip)))
	{
		V_UNKNOWN(&var)->Release();
		goto ON_ERROR;
	}

	V_UNKNOWN(&var)->Release();
	ASSERT(pIStrip != NULL);
	hr = pIStrip->OnWMMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_PROPERTIES, 0), 0L, 0L, 0L);

ON_ERROR:
	if( pIStrip )
	{
		pIStrip->Release();
	}
	if( pTimeline )
	{
		pTimeline->Release();
	}

	if( FAILED( hr ) )
	{
		ASSERT(pThis->m_pSegment != NULL);
		pThis->m_pSegment->OnShowProperties();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl::XEditor::OnF1Help

HRESULT CSegmentCtrl::XEditor::OnF1Help( void )
{
	METHOD_MANAGE_STATE( CSegmentCtrl, Editor )
	
    // Determine name of DMUSProd.exe help file
	CString strHelpFileName;

	if( theApp.GetHelpFileName( strHelpFileName ) )
	{
		strHelpFileName += "::/htm/SegmentDesignerWindow.htm";
		::HtmlHelp( NULL, strHelpFileName, HH_DISPLAY_TOPIC, 0 );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl::XMyOleInPlaceActiveObject implementation

STDMETHODIMP_(ULONG) CSegmentCtrl::XMyOleInPlaceActiveObject::AddRef()
{
	METHOD_MANAGE_STATE( CSegmentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.AddRef();
}

STDMETHODIMP_(ULONG) CSegmentCtrl::XMyOleInPlaceActiveObject::Release()
{
	METHOD_MANAGE_STATE( CSegmentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.Release();
}

STDMETHODIMP CSegmentCtrl::XMyOleInPlaceActiveObject::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_MANAGE_STATE( CSegmentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.QueryInterface( iid, ppvObj );
}

STDMETHODIMP CSegmentCtrl::XMyOleInPlaceActiveObject::GetWindow( HWND* lphwnd )
{
	METHOD_MANAGE_STATE( CSegmentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.GetWindow( lphwnd );
}

STDMETHODIMP CSegmentCtrl::XMyOleInPlaceActiveObject::ContextSensitiveHelp( BOOL fEnterMode )
{
	METHOD_MANAGE_STATE( CSegmentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.ContextSensitiveHelp( fEnterMode );
}

STDMETHODIMP CSegmentCtrl::XMyOleInPlaceActiveObject::TranslateAccelerator( LPMSG lpmsg )
{
	METHOD_MANAGE_STATE( CSegmentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	// Translate only if the timeline has focus
	if( pThis->m_pSegmentDlg )
	{
		IDMUSProdTimeline* pITimeline;
		if( SUCCEEDED( pThis->m_pSegmentDlg->GetTimeline( &pITimeline ) ) )
		{
			HWND hwndFocus;
			hwndFocus = ::GetFocus();
			IOleWindow* pIOleWindow;

			if( SUCCEEDED( pITimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleWindow ) ) )
			{
				HWND hwnd;
				if( pIOleWindow->GetWindow(&hwnd) == S_OK )
				{
					if( hwndFocus == hwnd )
					{
						if( ::TranslateAccelerator(pThis->m_hWnd, pThis->m_hAcceleratorTable, lpmsg) )
						{
							pIOleWindow->Release();
							pITimeline->Release();
							return S_OK;
						}
					}
				}
				pIOleWindow->Release();
			}
			pITimeline->Release();
		}
	}

	return pThis->m_xOleInPlaceActiveObject.TranslateAccelerator( lpmsg );
}

STDMETHODIMP CSegmentCtrl::XMyOleInPlaceActiveObject::OnFrameWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CSegmentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	if( pThis->m_fDocWindowActive && pThis->m_pSegmentDlg )
	{
		pThis->m_pSegmentDlg->Activate( fActivate );

		// Post a message so we can change the property page
		pThis->PostMessage(WM_WNDACTIVATE, fActivate, 0);
	}

	return pThis->m_xOleInPlaceActiveObject.OnFrameWindowActivate( fActivate );
}

STDMETHODIMP CSegmentCtrl::XMyOleInPlaceActiveObject::OnDocWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CSegmentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	if( pThis->m_pSegmentDlg )
	{
		pThis->m_pSegmentDlg->Activate( fActivate );

		// Post a message so we can change the property page
		pThis->PostMessage(WM_WNDACTIVATE, fActivate, 0);
	}

	pThis->m_fDocWindowActive = fActivate;

	return pThis->m_xOleInPlaceActiveObject.OnDocWindowActivate( fActivate );
}

STDMETHODIMP CSegmentCtrl::XMyOleInPlaceActiveObject::ResizeBorder(
	LPCRECT, LPOLEINPLACEUIWINDOW, BOOL fFrameWindow )
{
	METHOD_MANAGE_STATE( CSegmentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

    if( fFrameWindow == TRUE )
	{
		pThis->OnShowToolBars();
	}

	return S_OK;
}

STDMETHODIMP CSegmentCtrl::XMyOleInPlaceActiveObject::EnableModeless( BOOL fEnable )
{
	METHOD_MANAGE_STATE( CSegmentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.EnableModeless( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl::DoPropExchange - Persistence support

void CSegmentCtrl::DoPropExchange(CPropExchange* pPX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl::AboutBox - Display an "About" box to the user

void CSegmentCtrl::AboutBox()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CSegmentAboutBox dlgAbout;
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl message handlers

int CSegmentCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(!m_pSegmentDlg);

	if (COleControl::OnCreate(lpCreateStruct) == -1)
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
	m_hMenuInPlace = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDR_SEGMENT_DESIGNER) );

	// Load control's accelerator table
	m_hAcceleratorTable = ::LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_SEGMENT_ACCELS));

	CString strOutput;
	
	// Create control's dialog
	m_pSegmentDlg = new CSegmentDlg;
	if(m_pSegmentDlg == NULL)
	{
		return -1;
	}

	m_pSegmentDlg->m_pSegmentCtrl = this;

	if(0 == m_pSegmentDlg->Create(NULL, "Segment", WS_CHILD | WS_VISIBLE, CRect(0, 0, 800, 600), this, 888, NULL))
	{
		return -1;
	}

	if( m_pSegment )
	{
		if( m_pSegment->m_pComponent )
		{
			m_pSegment->m_pComponent->m_pIConductor->SetActiveTransport( (IDMUSProdTransport *)m_pSegment, m_pSegment->GetButtonState() );
		}
	}

	m_pSegmentDlg->Activate( TRUE );
	m_fDocWindowActive = TRUE;

	return 0;
}

void CSegmentCtrl::OnSize(UINT nType, int cx, int cy) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	COleControl::OnSize(nType, cx, cy);

	ASSERT(m_pSegmentDlg);

	RECT rRect;

	GetClientRect(&rRect);
	m_pSegmentDlg->MoveWindow(rRect.top, rRect.left, rRect.right - rRect.left, rRect.bottom - rRect.top); 
}

void CSegmentCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pSegment);
	ASSERT(m_pSegment->m_pComponent);
	ASSERT(m_pSegment->m_pComponent->m_pIConductor);

	// Make the timeline regain Focus
	ASSERT(m_pSegmentDlg != NULL);
	if(m_pSegmentDlg != NULL)
	{
		IDMUSProdTimeline* pITimeline;
		HRESULT hr = m_pSegmentDlg->GetTimeline(&pITimeline);
		ASSERT(SUCCEEDED(hr) && pITimeline != NULL);
		if(SUCCEEDED(hr) && pITimeline != NULL)
		{
			IOleWindow* pIOleWindow;
			if(SUCCEEDED(pITimeline->QueryInterface(IID_IOleWindow, (void **) &pIOleWindow)))
			{
				HWND hWnd;
				pIOleWindow->GetWindow(&hWnd);
				::SetFocus(hWnd);
				pIOleWindow->Release();
			}

			pITimeline->Release();
		}
	}

	COleControl::OnSetFocus(pOldWnd);

	if( m_pSegment != NULL )
	{
		m_pSegment->m_pComponent->m_pIConductor->SetActiveTransport( (IDMUSProdTransport *)m_pSegment, m_pSegment->GetButtonState() );
	}
}

void CSegmentCtrl::OnShowToolBars() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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

	// Set up the Status Bar
	ASSERT( m_pSegment != NULL );
	ASSERT( m_pSegment->m_pComponent != NULL );

	// Create status bar panes if they don't already exist
	if( m_hKeyStatusBar == NULL )
	{
		m_pSegment->m_pComponent->m_pIFramework->SetNbrStatusBarPanes( 2, SBLS_EDITOR, &m_hKeyStatusBar );
		m_pSegment->m_pComponent->m_pIFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 0, SBS_SUNKEN, 20 );
		m_pSegment->m_pComponent->m_pIFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 1, SBS_SUNKEN, 20 );

		// Tell the timeline to update the RealTime display in the status bar
		if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
		{
			m_pSegmentDlg->m_pTimeline->SetParam( GUID_TimelineShowRealTime, 0xFFFFFFFF, 0, 0, (void *)&m_hKeyStatusBar );
		}
	}

	COleControl::OnShowToolBars();
}

void CSegmentCtrl::OnHideToolBars() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSegment != NULL );
	ASSERT( m_pSegment->m_pComponent != NULL );

	if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
	{
		HANDLE hKey = NULL;

		// hKey is NULL - this will stop RealTime display
		m_pSegmentDlg->m_pTimeline->SetParam( GUID_TimelineShowRealTime, 0xFFFFFFFF, 0, 0, (void *)&hKey );
	}

	m_pSegment->m_pComponent->m_pIFramework->RestoreStatusBar( m_hKeyStatusBar );
	m_hKeyStatusBar = NULL;

	COleControl::OnHideToolBars();
}

void CSegmentCtrl::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	COleControl::OnDestroy();
	
	if( m_hMenuInPlace )
	{
		::DestroyMenu( m_hMenuInPlace );
		m_hMenuInPlace = NULL;
	}
}

HMENU CSegmentCtrl::OnGetInPlaceMenu() 
{
	return m_hMenuInPlace;
}

void CSegmentCtrl::OnAppAbout() 
{
	AboutBox();
}

void CSegmentCtrl::OnEditCut() 
{
	SendEditToTimeline(ID_EDIT_CUT);
}

void CSegmentCtrl::OnEditCopy() 
{
	SendEditToTimeline(ID_EDIT_COPY);
}

void CSegmentCtrl::OnEditPasteMerge() 
{
	if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
	{
		m_pSegmentDlg->m_pTimeline->SetPasteType( TL_PASTE_MERGE );
	}
	SendEditToTimeline(ID_EDIT_PASTE);
}

void CSegmentCtrl::OnEditPasteOverwrite() 
{
	if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
	{
		m_pSegmentDlg->m_pTimeline->SetPasteType( TL_PASTE_OVERWRITE );
	}
	SendEditToTimeline(ID_EDIT_PASTE);
}

void CSegmentCtrl::OnEditInsert() 
{
	SendEditToTimeline(ID_EDIT_INSERT);
}

void CSegmentCtrl::OnEditDelete() 
{
	SendEditToTimeline(ID_EDIT_DELETE);
}

void CSegmentCtrl::OnEditSelectall() 
{
	SendEditToTimeline(ID_EDIT_SELECT_ALL);
}

void CSegmentCtrl::OnEditRedo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pSegment != NULL);
	if(m_pSegment != NULL)
	{
		ASSERT(m_pSegment->m_pUndoMgr != NULL);
		if(m_pSegment->m_pUndoMgr != NULL)
		{
			// Save the current timeline settings
			m_pSegment->SyncTimelineSettings();

			// Freeze undo queue
			VARIANT var;
			if( m_pSegment->m_pSegmentDlg && m_pSegment->m_pSegmentDlg->m_pTimeline )
			{
				var.vt = VT_BOOL;
				V_BOOL(&var) = TRUE;
				m_pSegment->m_pSegmentDlg->m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
			}

			m_pSegment->m_pUndoMgr->Redo(m_pSegment);

			// Re-enable undo queue
			if( m_pSegment->m_pSegmentDlg && m_pSegment->m_pSegmentDlg->m_pTimeline )
			{
				var.vt = VT_BOOL;
				V_BOOL(&var) = FALSE;
				m_pSegment->m_pSegmentDlg->m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
			}
		}
		if( m_pSegment->m_pSegmentDlg && m_pSegment->m_pSegmentDlg->m_pTimeline )
		{
			m_pSegment->m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( GUID_Segment_RecordButton, 0xFFFFFFFF, &(m_pSegment->m_fRecordPressed));
			m_pSegment->m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( GUID_Segment_WindowActive, 0xFFFFFFFF, &m_fDocWindowActive );
		}
	}
}

void CSegmentCtrl::OnEditUndo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pSegment != NULL);
	if(m_pSegment != NULL)
	{
		ASSERT(m_pSegment->m_pUndoMgr != NULL);
		if(m_pSegment->m_pUndoMgr != NULL)
		{
			// Save the current timeline settings
			m_pSegment->SyncTimelineSettings();

			// Freeze undo queue
			VARIANT var;
			if( m_pSegment->m_pSegmentDlg && m_pSegment->m_pSegmentDlg->m_pTimeline )
			{
				var.vt = VT_BOOL;
				V_BOOL(&var) = TRUE;
				m_pSegment->m_pSegmentDlg->m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
			}

			m_pSegment->m_pUndoMgr->Undo(m_pSegment, m_pSegment->m_pComponent->m_pIFramework);

			// Re-enable undo queue
			if( m_pSegment->m_pSegmentDlg && m_pSegment->m_pSegmentDlg->m_pTimeline )
			{
				var.vt = VT_BOOL;
				V_BOOL(&var) = FALSE;
				m_pSegment->m_pSegmentDlg->m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
			}
		}
		
		if( m_pSegment->m_pSegmentDlg && m_pSegment->m_pSegmentDlg->m_pTimeline )
		{
			m_pSegment->m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( GUID_Segment_RecordButton, 0xFFFFFFFF, &(m_pSegment->m_fRecordPressed));
			m_pSegment->m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( GUID_Segment_WindowActive, 0xFFFFFFFF, &m_fDocWindowActive );
		}
	}
}

void CSegmentCtrl::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSegment != NULL );

	CString strRedo;
	CString strRedoAccel;

	strRedo.LoadString( IDS_REDO );
	strRedoAccel.LoadString( IDS_REDO_ACCEL );

	BOOL fEnable = FALSE;

	if( m_pSegment )
	{
		TCHAR achText[256];

		ASSERT( m_pSegment->m_pUndoMgr != NULL );

		if( m_pSegment->m_pUndoMgr )
		{
			if( m_pSegment->m_pUndoMgr->GetRedo(achText, 256) )
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

void CSegmentCtrl::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSegment != NULL );

	CString strUndo;
	CString strUndoAccel;

	strUndo.LoadString( IDS_UNDO );
	strUndoAccel.LoadString( IDS_UNDO_ACCEL );

	BOOL fEnable = FALSE;

	if( m_pSegment )
	{
		TCHAR achText[256];

		ASSERT( m_pSegment->m_pUndoMgr != NULL );

		if( m_pSegment->m_pUndoMgr )
		{
			if( m_pSegment->m_pUndoMgr->GetUndo(achText, 256) )
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

void CSegmentCtrl::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	pCmdUI->Enable(TimelineEditAvailable(ID_EDIT_CUT));
}

void CSegmentCtrl::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	pCmdUI->Enable(TimelineEditAvailable(ID_EDIT_COPY));
}

void CSegmentCtrl::OnUpdateEditInsert(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	pCmdUI->Enable(TimelineEditAvailable(ID_EDIT_INSERT));
}

void CSegmentCtrl::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	pCmdUI->Enable(TimelineEditAvailable(ID_EDIT_DELETE));
}

void CSegmentCtrl::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	pCmdUI->Enable(TimelineEditAvailable(ID_EDIT_SELECT_ALL));
}	

void CSegmentCtrl::SendEditToTimeline(WPARAM wId)
{
	HRESULT					hr;
	BOOL					bAvailable = FALSE;
	IDMUSProdTimeline		*pITimeline;
	IDMUSProdTimelineEdit	*pITimelineEdit;

	ASSERT(m_pSegmentDlg != NULL);
	if(m_pSegmentDlg != NULL)
	{
		hr = m_pSegmentDlg->GetTimeline(&pITimeline);
		ASSERT(SUCCEEDED(hr) && pITimeline != NULL);
		if(SUCCEEDED(hr) && pITimeline != NULL)
		{
			if(SUCCEEDED(pITimeline->QueryInterface(IID_IDMUSProdTimelineEdit, (void **) &pITimelineEdit)))
			{
				bAvailable = TimelineEditAvailable(wId);
				ASSERT(bAvailable);
				if(bAvailable)
				{
					switch(wId)
					{
					case ID_EDIT_CUT:
						hr = pITimelineEdit->Cut(NULL);
						break;
					case ID_EDIT_COPY:
						hr = pITimelineEdit->Copy(NULL);
						break;
					case ID_EDIT_PASTE:
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
				}
				pITimelineEdit->Release();
			}
			pITimeline->Release();
		}

		if ( FAILED(hr) && hr != E_NOTIMPL )
		{
			TRACE( "SegmentCtl: Cut/Copy/Paste %x Failed with %x\n", wId, hr );
		}
	}
	else
	{
		hr = E_UNEXPECTED;
	}
}

BOOL CSegmentCtrl::TimelineEditAvailable(WPARAM wId)
{
	HRESULT					hr;
	IDMUSProdTimeline		*pITimeline;
	IDMUSProdTimelineEdit	*pITimelineEdit;

	ASSERT(m_pSegmentDlg != NULL);
	if(m_pSegmentDlg != NULL)
	{
		hr = m_pSegmentDlg->GetTimeline(&pITimeline);
		ASSERT(SUCCEEDED(hr) && pITimeline != NULL);
		if(SUCCEEDED(hr) && pITimeline != NULL)
		{
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
					hr = pITimelineEdit->CanSelectAll();
					break;
				default:
					hr = S_FALSE;
				}
				pITimelineEdit->Release();
			}
			pITimeline->Release();
			if(hr == S_OK)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CSegmentCtrl::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	COleControl::OnActivate(nState, pWndOther, bMinimized);
	
	TRACE("CSegmentCtrl: Activate %d\n", nState);
}

void CSegmentCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	COleControl::OnKillFocus(pNewWnd);
	
	TRACE0("CSegmentCtrl: KillFocus\n");
	
}

void CSegmentCtrl::OnSegmentDeleteTrack() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( m_pSegment );
	ASSERT( m_pSegmentDlg );
	ASSERT( m_pSegmentDlg->m_pTimeline );

	if( AfxMessageBox(IDS_WARNING_DELETE_TRACK, MB_OKCANCEL) != IDOK )
	{
		return;
	}

	// Skip the TimeStrip, since it can't be removed.
	DWORD dwEnum = 1;
	IDMUSProdStrip* pIStrip;
	IDMUSProdStripMgr* pIStripMgr;
	BOOL fStateSaved = FALSE;
	VARIANT variant;

	while( m_pSegmentDlg->m_pTimeline->EnumStrip( dwEnum, &pIStrip ) == S_OK )
	{
		ASSERT( pIStrip );
		dwEnum++;
		if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->StripGetTimelineProperty( pIStrip, STP_GUTTER_SELECTED, &variant ) ) )
		{
			VARIANT varStripMgr;
			if( (V_BOOL( &variant ) == TRUE)
			&&	SUCCEEDED( pIStrip->GetStripProperty( SP_STRIPMGR, &varStripMgr ) )
			&& (V_UNKNOWN(&varStripMgr) != NULL))
			{
				if( SUCCEEDED( V_UNKNOWN(&varStripMgr)->QueryInterface( IID_IDMUSProdStripMgr, (void **)&pIStripMgr ) ) )
				{
					m_pSegment->UpdateSavedState( fStateSaved, IDS_UNDO_DELETE_TRACK );
					m_pSegment->PrivRemoveStripMgr( pIStripMgr );

					pIStripMgr->Release();
					pIStripMgr = NULL;
					// Start over, since any number of strips may have been removed.
					dwEnum = 1;
				}
				V_UNKNOWN(&varStripMgr)->Release();
			}
		}

		pIStrip->Release();
	}

	// If no strips were removed, try and remove the active strip
	if( !fStateSaved && SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetTimelineProperty( TP_ACTIVESTRIP, &variant ) ) )
	{
		ASSERT(variant.vt == VT_UNKNOWN);
		if(V_UNKNOWN(&variant) != NULL)
		{
			if(SUCCEEDED(V_UNKNOWN(&variant)->QueryInterface(IID_IDMUSProdStrip, (void **) &pIStrip)))
			{
				V_UNKNOWN(&variant)->Release();

				if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->StripGetTimelineProperty( pIStrip, STP_POSITION, &variant ) ) )
				{
					VARIANT varStripMgr;
					if( (V_I4( &variant ) > 0)
					&&	SUCCEEDED( pIStrip->GetStripProperty( SP_STRIPMGR, &varStripMgr ) )
					&& (V_UNKNOWN(&varStripMgr) != NULL) )
					{
						if( SUCCEEDED( V_UNKNOWN(&varStripMgr)->QueryInterface( IID_IDMUSProdStripMgr, (void **)&pIStripMgr ) ) )
						{
							m_pSegment->UpdateSavedState( fStateSaved, IDS_UNDO_DELETE_TRACK );
							m_pSegment->PrivRemoveStripMgr( pIStripMgr );

							pIStripMgr->Release();
							pIStripMgr = NULL;
						}
						V_UNKNOWN(&varStripMgr)->Release();
					}
				}

				pIStrip->Release();
			}
		}
	}
}

void CSegmentCtrl::OnUpdateSegmentDeleteTrack(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( m_pSegmentDlg );
	ASSERT( m_pSegmentDlg->m_pTimeline );

	// Skip the TimeStrip, since it can't be removed.
	DWORD dwEnum = 1;
	IDMUSProdStrip* pIStrip;
	VARIANT variant;

	// Check to see if any strips are gutter selected
	while( m_pSegmentDlg->m_pTimeline->EnumStrip( dwEnum, &pIStrip ) == S_OK )
	{
		ASSERT( pIStrip );
		dwEnum++;
		if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->StripGetTimelineProperty( pIStrip, STP_GUTTER_SELECTED, &variant ) ) )
		{
			if( V_BOOL( &variant ) == TRUE )
			{
				pCmdUI->Enable( TRUE );
				pIStrip->Release();
				return;
			}
		}
		pIStrip->Release();
	}

	// Check to see if a strip other than the time strip (top strip) is active
	if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetTimelineProperty( TP_ACTIVESTRIP, &variant ) ) )
	{
		ASSERT(variant.vt == VT_UNKNOWN);
		if(V_UNKNOWN(&variant) != NULL)
		{
			if(SUCCEEDED(V_UNKNOWN(&variant)->QueryInterface(IID_IDMUSProdStrip, (void **) &pIStrip)))
			{
				V_UNKNOWN(&variant)->Release();

				if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->StripGetTimelineProperty( pIStrip, STP_POSITION, &variant ) ) )
				{
					if( V_I4( &variant ) > 0 )
					{
						pCmdUI->Enable( TRUE );
						pIStrip->Release();
						return;
					}
				}

				pIStrip->Release();
			}
		}
	}
	pCmdUI->Enable( FALSE );
}

void CSegmentCtrl::OnSegmentNewtrack() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CAddTrackDlg dlg;
	dlg.m_strSegmentName = m_pSegment->m_PPGSegment.strSegmentName;
	UINT result = dlg.DoModal();
	HRESULT hr;
	if(result == IDOK)
	{
		hr = E_NOTIMPL;
		if( dlg.m_dwCount )
		{
			ASSERT( dlg.m_pGuid );

			CTrack* pTrack;
			DWORD dwCount;
			BOOL fStateSaved = FALSE;

			// Freeze undo queue
			VARIANT var;

			for( dwCount = 0; dwCount < dlg.m_dwCount; dwCount++ )
			{
				pTrack = new CTrack;
				ASSERT( pTrack != NULL );
				if ( pTrack == NULL )
				{
					hr = E_OUTOFMEMORY;
				}
				else
				{
					BOOL fOldStateSaved = fStateSaved;
					m_pSegment->UpdateSavedState( fStateSaved, IDS_UNDO_ADD_TRACK );

					// After we've saved the state the first time, set TP_FREEZE_UNDO to true
					if( !fOldStateSaved && m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
					{
						var.vt = VT_BOOL;
						V_BOOL(&var) = TRUE;
						m_pSegmentDlg->m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
					}

					pTrack->m_dwGroupBits = 1;
					memcpy( &pTrack->m_guidClassID, &(dlg.m_pGuid[dwCount]), sizeof(GUID) );
					hr = m_pSegment->AddTrack( pTrack );

					// Send a CreateTrack message to the stripmgr
					IDMUSProdStripMgr *pStripMgr = NULL;
					pTrack->GetStripMgr( &pStripMgr );
					if( pStripMgr )
					{
						// Undo state is already frozen, so any collateral changes this makes
						// won't make it into the Undo queue
						pStripMgr->OnUpdate( GUID_Segment_CreateTrack, pTrack->m_dwGroupBits, NULL );

						// Notify all StripMgrs that this track was created
						if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
						{
							m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( pTrack->m_guidClassID, pTrack->m_dwGroupBits, pStripMgr );
						}

						// Make sure segment has final version of track
						IUnknown* pIUnknown;
						if( SUCCEEDED ( pStripMgr->QueryInterface( IID_IUnknown, (void **)&pIUnknown ) ) )
						{
							m_pSegment->OnDataChanged( pIUnknown );
							pIUnknown->Release();
						}

						pStripMgr->Release();
					}
				}
			}

			if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
			{
				// Re-enable undo queue
				var.vt = VT_BOOL;
				V_BOOL(&var) = FALSE;
				m_pSegmentDlg->m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
			}

		}
		if(FAILED(hr))
		{
			AfxMessageBox(IDS_FAILED_ADD_TRACK);
		}
	}
}

void CSegmentCtrl::OnHelpFinder() 
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

void CSegmentCtrl::OnUpdateEditPasteMerge(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	BOOL fEnable = TimelineEditAvailable(ID_EDIT_PASTE);
	pCmdUI->Enable( fEnable );

	// Enable/Disable the main Paste menu item
	if( pCmdUI->m_pMenu )
	{
		pCmdUI->m_pMenu->EnableMenuItem( 5, fEnable ? MF_BYPOSITION : MF_BYPOSITION | MF_GRAYED );
	}
}

void CSegmentCtrl::OnUpdateEditPasteOverwrite(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	pCmdUI->Enable(TimelineEditAvailable(ID_EDIT_PASTE));
}

void CSegmentCtrl::OnWndActivate(WPARAM wParam, LPARAM lParam)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(lParam);

	if((BOOL)wParam == TRUE)
	{
		// Check if property sheet is visible
		IDMUSProdPropSheet* pIPropSheet;

		if( SUCCEEDED ( m_pSegment->m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			//  If the property sheet is visible
			if( pIPropSheet->IsShowing() == S_OK )
			{
				BOOL fSetPropPage = FALSE;
				VARIANT var;
				if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetTimelineProperty( TP_ACTIVESTRIP, &var ) ) )
				{
					fSetPropPage = TRUE;

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
						else
						{
							IDMUSProdStrip *pIDMUSProdStrip;
							if( SUCCEEDED( punkStrip->QueryInterface( IID_IDMUSProdStrip, (void**) &pIDMUSProdStrip ) ) )
							{
								VARIANT varStripMgr;
								if( SUCCEEDED( pIDMUSProdStrip->GetStripProperty( SP_STRIPMGR, &varStripMgr ) ) )
								{
									if( SUCCEEDED( V_UNKNOWN(&varStripMgr)->QueryInterface( IID_IDMUSProdPropPageObject, (void**) &pIDMUSProdPropPageObject ) ) )
									{
										if( SUCCEEDED( pIDMUSProdPropPageObject->OnShowProperties() ) )
										{
											fSetPropPage = TRUE;
										}
										pIDMUSProdPropPageObject->Release();
									}
									V_UNKNOWN(&varStripMgr)->Release();
								}
								pIDMUSProdStrip->Release();
							}
						}
						punkStrip->Release();
					}
				}
				if( !fSetPropPage )
				{
					m_pSegment->OnShowProperties();
				}
			}
			pIPropSheet->Release();
		}
	}

}

void CSegmentCtrl::OnParentNotify(UINT message, LPARAM lParam) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(lParam);

	COleControl::OnParentNotify(message, lParam);
	
	// If the user pressed the left or right mouse buttons in our control,
	// change the active transport to this segment
	if( (message == WM_LBUTTONDOWN) ||
		(message == WM_RBUTTONDOWN) )
	{
		if( m_pSegment && m_pSegment->m_pComponent && m_pSegment->m_pComponent->m_pIConductor )
		{
			m_pSegment->m_pComponent->m_pIConductor->SetActiveTransport( (IDMUSProdTransport *)m_pSegment, m_pSegment->GetButtonState() );
		}
	}
}
