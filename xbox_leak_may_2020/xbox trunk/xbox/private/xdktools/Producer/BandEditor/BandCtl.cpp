// BandCtl.cpp : Implementation of the CBandCtrl ActiveX Control class.

#include "stdafx.h"
#include "BandEditorDLL.h"
#include "Band.h"
#include "BandRef.h"
#include "BandCtl.h"
#include "BandDlg.h"
#include "BandPpg.h"
#include "BandAboutBox.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CBandCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CBandCtrl, COleControl)
	//{{AFX_MSG_MAP(CBandCtrl)
	ON_WM_CREATE()
	ON_COMMAND(ID_APP_ABOUT, AboutBox)
	ON_COMMAND(ID_EDIT_PROPERTIES, OnEditProperties)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_WM_SETFOCUS()
	ON_COMMAND(IDM_HELP_FINDER, OnHelpFinder)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECTALL, OnUpdateEditSelectAll)
	ON_COMMAND(ID_EDIT_SELECTALL, OnEditSelectAll)
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CBandCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CBandCtrl)
	// NOTE - ClassWizard will add and remove dispatch map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CBandCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CBandCtrl, COleControl)
	//{{AFX_EVENT_MAP(CBandCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl Interface map

BEGIN_INTERFACE_MAP(CBandCtrl, COleControl)
    INTERFACE_PART(CBandCtrl, IID_IOleInPlaceActiveObject, MyOleInPlaceActiveObject)
    INTERFACE_PART(CBandCtrl, IID_IOleInPlaceObject, MyOleInPlaceObject)
    INTERFACE_PART(CBandCtrl, IID_IDMUSProdEditor, Editor)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CBandCtrl, 1)
	PROPPAGEID(CBandPropPage::guid)
END_PROPPAGEIDS(CBandCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CBandCtrl, "STYLEDESIGNER.BandCtrl.1",
	0x3bd2ba11, 0x46e7, 0x11d0, 0x89, 0xac, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CBandCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DBand =
		{ 0x3bd2ba0f, 0x46e7, 0x11d0, { 0x89, 0xac, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };
const IID BASED_CODE IID_DBandEvents =
		{ 0x3bd2ba10, 0x46e7, 0x11d0, { 0x89, 0xac, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwBandOleMisc =
	OLEMISC_SIMPLEFRAME |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CBandCtrl, IDS_BAND, _dwBandOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::CBandCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CBandCtrl

BOOL CBandCtrl::CBandCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_BAND,
			IDB_BAND,
			afxRegApartmentThreading,
			_dwBandOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::CBandCtrl - Constructor

CBandCtrl::CBandCtrl()
{
	m_pComponent = NULL;

/////////////////////////////////////////////////////////////////////////
// Following commented out to prevent 3 ASSERTS that occur
// because we are not building a typelib
//	InitializeIIDs(&IID_DBand, &IID_DBandEvents);
//
// Following code copied from InitializeIIDs (since we cannot override)
	m_piidPrimary = &IID_DBand;
	m_piidEvents = &IID_DBandEvents;
	EnableTypeLib();	// Needed to prevent ASSERT when closing editor
	InitStockEventMask();
	InitStockPropMask();
//////////////////////////////////////////////////////////////////////////

	EnableSimpleFrame();

	m_pBandDlg = NULL;
	m_hWndContainer = NULL;
	m_hMenuInPlace = NULL;
	m_hKeyStatusBar = NULL;
	m_pToolBar = NULL;
	m_pBand = NULL;
	m_hLastFocusWnd = NULL;
	m_fFrameActive = false;
	m_fDocWindowActive = false;
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::~CBandCtrl - Destructor

CBandCtrl::~CBandCtrl()
{

	// If we are in a Property Page Manager we need to remove ourselves before we go away
	IDMUSProdPropSheet *pIPropSheet;
	if(SUCCEEDED(m_pBand->m_pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**) &pIPropSheet)))
	{
		pIPropSheet->RemovePageManagerByObject(&(CInstrumentListItem::m_CommonPropertyObject));
		pIPropSheet->Release();
	}


	if( m_pBand )
	{
		m_pBand->Release();
	}
	if (m_pComponent)
	{
		m_pComponent->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl IDMUSProdEditor implementation

/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::XEditor::AddRef

STDMETHODIMP_(ULONG) CBandCtrl::XEditor::AddRef()
{
	METHOD_PROLOGUE_EX_( CBandCtrl, Editor )

	return (ULONG)pThis->ExternalAddRef();
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::XEditor::Release

STDMETHODIMP_(ULONG) CBandCtrl::XEditor::Release()
{
	METHOD_PROLOGUE_EX_( CBandCtrl, Editor )

	return (ULONG)pThis->ExternalRelease();
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::XEditor::QueryInterface

STDMETHODIMP CBandCtrl::XEditor::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_PROLOGUE_EX_( CBandCtrl, Editor )

	return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::XEditor implementation

/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::XEditor::AttachObjects

HRESULT CBandCtrl::XEditor::AttachObjects( IDMUSProdNode* pINode )
{
	METHOD_MANAGE_STATE( CBandCtrl, Editor )

	ASSERT_VALID( pThis );
	ASSERT( pINode != NULL );

	GUID guidNode;

	pINode->GetNodeId( &guidNode );
	if( ::IsEqualGUID( guidNode, GUID_BandRefNode ) )
	{
		CBandRef* pBandRef = (CBandRef *)pINode;
		ASSERT( pBandRef->m_pBand != NULL );
		pThis->m_pBand = pBandRef->m_pBand;
	}
	else
	{
		pThis->m_pBand = (CBand *)pINode;
	}
	
	pThis->m_pBand->AddRef();
	pThis->m_pComponent = (CBandComponent*)pThis->m_pBand->m_pComponent;
	pThis->m_pComponent->AddRef();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::XEditor::OnInitMenuFilePrint

HRESULT CBandCtrl::XEditor::OnInitMenuFilePrint( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CBandCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::XEditor::OnFilePrint

HRESULT CBandCtrl::XEditor::OnFilePrint( void )
{
	METHOD_MANAGE_STATE( CBandCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::XEditor::OnInitMenuFilePrintPreview

HRESULT CBandCtrl::XEditor::OnInitMenuFilePrintPreview( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CBandCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::XEditor::OnFilePrintPreview

HRESULT CBandCtrl::XEditor::OnFilePrintPreview( void )
{
	METHOD_MANAGE_STATE( CBandCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::XEditor::OnViewProperties

HRESULT CBandCtrl::XEditor::OnViewProperties( void )
{
	METHOD_MANAGE_STATE( CBandCtrl, Editor )
	ASSERT( theApp.m_pIFramework != NULL );
	HRESULT hr = E_FAIL;

	if(pThis->m_pBand)
	{
		// Show the pchannel property page if a pchannel is selected
		// Otherwise show the band's property page
		if(pThis->m_pBandDlg != NULL && pThis->m_pBand->GetNumberOfSelectedInstruments() > 0)
		{
			hr = S_OK;
			pThis->m_pBandDlg->RefreshPChannelPropertyPage();
		}
		else
		{
			hr = S_OK;
			pThis->m_pBand->OnViewProperties();
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::XEditor::OnF1Help

HRESULT CBandCtrl::XEditor::OnF1Help( void )
{
	METHOD_MANAGE_STATE( CBandCtrl, Editor )

    // Determine name of DMUSProd.exe help file
	CString strHelpFileName;

	if( theApp.GetHelpFileName( strHelpFileName ) )
	{
		strHelpFileName += "::/htm/BandEditorWindow.htm";
		::HtmlHelp( NULL, strHelpFileName, HH_DISPLAY_TOPIC, 0 );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::XMyOleInPlaceActiveObject implementation

STDMETHODIMP_(ULONG) CBandCtrl::XMyOleInPlaceActiveObject::AddRef()
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.AddRef();
}

STDMETHODIMP_(ULONG) CBandCtrl::XMyOleInPlaceActiveObject::Release()
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.Release();
}

STDMETHODIMP CBandCtrl::XMyOleInPlaceActiveObject::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.QueryInterface( iid, ppvObj );
}

STDMETHODIMP CBandCtrl::XMyOleInPlaceActiveObject::GetWindow( HWND* lphwnd )
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.GetWindow( lphwnd );
}

STDMETHODIMP CBandCtrl::XMyOleInPlaceActiveObject::ContextSensitiveHelp( BOOL fEnterMode )
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.ContextSensitiveHelp( fEnterMode );
}

STDMETHODIMP CBandCtrl::XMyOleInPlaceActiveObject::TranslateAccelerator( LPMSG lpmsg )
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.TranslateAccelerator( lpmsg );
}

STDMETHODIMP CBandCtrl::XMyOleInPlaceActiveObject::OnFrameWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	if( fActivate
	&&	pThis->m_fDocWindowActive )
	{
		pThis->m_pBand->Activate();
	}
	
	if(pThis->m_pBandDlg)
	{
		if(fActivate)
		{
			pThis->m_pBandDlg->m_PChannelList.SetFocus();
			if(pThis->m_pBandDlg->IsMidiRegistered() == false)
			{
				pThis->m_pBandDlg->RegisterMidi();
			}
		}
		else
		{
			pThis->m_pBandDlg->UnRegisterMidi();
		}
	}

	pThis->m_fFrameActive = fActivate ? true : false;

	return pThis->m_xOleInPlaceActiveObject.OnFrameWindowActivate( fActivate );
}

STDMETHODIMP CBandCtrl::XMyOleInPlaceActiveObject::OnDocWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	if( fActivate
	&&	pThis->m_fFrameActive )
	{
		pThis->m_pBand->Activate();
	}

	if(pThis->m_pBandDlg)
	{
		if( fActivate )
		{
			CInstrumentListItem::m_CommonPropertyObject.SetBandDialog(pThis->m_pBandDlg);
			pThis->m_pBandDlg->m_PChannelList.SetFocus();
			pThis->m_pBandDlg->RegisterMidi();
		}
		else
		{
			CInstrumentListItem::m_CommonPropertyObject.SetBandDialog(NULL);
			pThis->m_pBandDlg->UnRegisterMidi();
		}
	}

	if( fActivate )
	{
		pThis->m_pBand->OnViewProperties();
	}

	pThis->m_fDocWindowActive = fActivate ? true : false;

	return pThis->m_xOleInPlaceActiveObject.OnDocWindowActivate( fActivate );
}

STDMETHODIMP CBandCtrl::XMyOleInPlaceActiveObject::ResizeBorder(
	LPCRECT, LPOLEINPLACEUIWINDOW, BOOL fFrameWindow )
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

    if( fFrameWindow == TRUE )
	{
		pThis->OnShowToolBars();
	}

	return S_OK;
}

STDMETHODIMP CBandCtrl::XMyOleInPlaceActiveObject::EnableModeless( BOOL fEnable )
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.EnableModeless( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::XMyOleInPlaceObject implementation

STDMETHODIMP_(ULONG) CBandCtrl::XMyOleInPlaceObject::AddRef()
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceObject.AddRef();
}

STDMETHODIMP_(ULONG) CBandCtrl::XMyOleInPlaceObject::Release()
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceObject.Release();
}

STDMETHODIMP CBandCtrl::XMyOleInPlaceObject::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceObject.QueryInterface( iid, ppvObj );
}

STDMETHODIMP CBandCtrl::XMyOleInPlaceObject::GetWindow( HWND* lphwnd )
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceObject.GetWindow( lphwnd );
}

STDMETHODIMP CBandCtrl::XMyOleInPlaceObject::ContextSensitiveHelp( BOOL fEnterMode )
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceObject.ContextSensitiveHelp( fEnterMode );
}

STDMETHODIMP CBandCtrl::XMyOleInPlaceObject::InPlaceDeactivate()
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceObject )
	ASSERT_VALID( pThis );

	// Need to do this here instead of in OnDestroy, because OnDocWindowActivate() is called for
	// the newly active window (which may be another Band control) before our OnDestroy
	// method is called.
	if(pThis->m_pBandDlg)
	{
		CInstrumentListItem::m_CommonPropertyObject.SetBandDialog(NULL);
	}

	return pThis->m_xOleInPlaceObject.InPlaceDeactivate();
}

STDMETHODIMP CBandCtrl::XMyOleInPlaceObject::UIDeactivate()
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceObject.UIDeactivate();
}

STDMETHODIMP CBandCtrl::XMyOleInPlaceObject::SetObjectRects( LPCRECT lprcPosRect, LPCRECT lprcClipRect )
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceObject.SetObjectRects( lprcPosRect, lprcClipRect );
}

STDMETHODIMP CBandCtrl::XMyOleInPlaceObject::ReactivateAndUndo()
{
	METHOD_MANAGE_STATE( CBandCtrl, MyOleInPlaceObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceObject.ReactivateAndUndo();
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::OnDraw - Drawing function

void CBandCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// TODO: Replace the following code with your own drawing code.
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::DoPropExchange - Persistence support

void CBandCtrl::DoPropExchange(CPropExchange* pPX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::OnResetState - Reset control to default state

void CBandCtrl::OnResetState()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::AboutBox - Display an "About" box to the user

void CBandCtrl::AboutBox()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CBandAboutBox dlgAbout;
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl message handlers

/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::OnCreate

int CBandCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pBand != NULL );

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

	// Cut down on flicker when redrawing Band editor
	ModifyStyle( 0, WS_CLIPCHILDREN, 0 );
	CWnd* pWndParent = GetParent();
	if( pWndParent )
	{
		pWndParent->ModifyStyle( 0, WS_CLIPCHILDREN, 0 );
	}

	// Load control's in-place menu
	m_hMenuInPlace = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDR_BAND_EDITOR) );

	// Create control's toolbar
//	CWnd* pWndParent = CWnd::FromHandle( m_hWndContainer );
//	if( pWndParent )
//	{
//		m_pToolBar = new CToolBar;
//		m_pToolBar->Create( pWndParent );
//		m_pToolBar->LoadToolBar( IDR_BAND_EDITOR );
//		m_pToolBar->SetBarStyle( m_pToolBar->GetBarStyle() | CBRS_SIZE_DYNAMIC );
//		m_pToolBar->SetOwner( this );
//	}
	
	// Create control's dialog
	m_pBandDlg = new CBandDlg();
	if( m_pBandDlg == NULL )
	{
		return -1;
	}

	m_pBandDlg->m_pBandCtrl = this;

	if( 0 == m_pBandDlg->Create( NULL, "WindowName", WS_CHILD | WS_VISIBLE, CRect( 0, 0, 800, 400 ), this, 888, NULL ) )
	{
		return -1;
	}

	m_pBand->m_pBandCtrl = this;
	m_pBandDlg->OnInitialUpdate();

	m_fFrameActive = true;
	m_fDocWindowActive = true;
	m_pBand->Activate();
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::OnDestroy

void CBandCtrl::OnDestroy() 
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

	ASSERT( m_pBand != NULL );

	m_pBand->m_pBandCtrl = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::OnGetInPlaceMenu

HMENU CBandCtrl::OnGetInPlaceMenu() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return m_hMenuInPlace;
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::OnEditProperties

void CBandCtrl::OnEditProperties() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CRect rectControl;
	GetRectInContainer( rectControl );

	MSG msg;
	ZeroMemory( &msg, sizeof(MSG) );

	msg.hwnd = this->GetSafeHwnd();
	msg.message = WM_MYPROPVERB;

	m_xOleObject.DoVerb( OLEIVERB_PROPERTIES,
						 &msg, m_pClientSite, 0L, m_hWndContainer, rectControl );
}


/////////////////////////////////////////////////////////////////////////////
// CBandCtrl::OnHideToolBars

void CBandCtrl::OnHideToolBars() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pBand != NULL );
	ASSERT( m_pBand->m_pComponent != NULL );

	m_pBand->m_pComponent->m_pIFramework->RestoreStatusBar( m_hKeyStatusBar );
	m_hKeyStatusBar = NULL;

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
// CBandCtrl::OnShowToolBars

void CBandCtrl::OnShowToolBars() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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

	ASSERT( m_pBand != NULL );
	ASSERT( m_pBand->m_pComponent != NULL );

	// Create status bar panes if they don't already exist
	if( m_hKeyStatusBar == NULL )
	{
		m_pBand->m_pComponent->m_pIFramework->SetNbrStatusBarPanes( 1, SBLS_EDITOR, &m_hKeyStatusBar );
		m_pBand->m_pComponent->m_pIFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 0, SBS_SUNKEN, 15 );
	}

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
// CBandCtrl::OnSize

void CBandCtrl::OnSize(UINT nType, int cx, int cy) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	COleControl::OnSize(nType, cx, cy);
	
	m_pBandDlg->MoveWindow( 0, 0, cx, cy );
}

void CBandCtrl::OnEditUndo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	IPersistStream *pIPStream;

	m_pBand->QueryInterface( IID_IPersistStream, (void **) &pIPStream );
	if( pIPStream )
	{
		m_pBand->m_bInUndoRedo = true;
		m_pBandDlg->m_pBandDo->Undo( pIPStream );
		m_pBand->m_bInUndoRedo = false;
		pIPStream->Release();
	}

	// Update display
	m_pBandDlg->RefreshDisplay();

	// Notify DocRoot that Band has changed
	IDMUSProdNode* pIDocRootNode;

	if( SUCCEEDED ( m_pBand->GetDocRootNode(&pIDocRootNode) ) )
	{
		if( pIDocRootNode != (IDMUSProdNode *)m_pBand )
		{
			IDMUSProdNotifySink* pINotifySink;

			if( SUCCEEDED ( pIDocRootNode->QueryInterface(IID_IDMUSProdNotifySink, (void**)&pINotifySink) ) )
			{
				// Ask DocRoot if change should be sent
				if( pINotifySink->OnUpdate( m_pBand, GUID_BAND_ChangeNotifyMsg, NULL ) == S_OK )
				{
					m_pBand->SendBand();
				}

				pINotifySink->Release();
			}
		}

		pIDocRootNode->Release();
	}

    // Saving clears this, but we are still dirty
    m_pBand->SetModifiedFlag( TRUE );

	if(m_pBandDlg)
		m_pBandDlg->SetFocus();
}

void CBandCtrl::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strUndo;
	CString strUndoAccel;

	strUndo.LoadString( IDS_UNDO );
	strUndoAccel.LoadString( IDS_UNDO_ACCEL );

	BOOL fEnable = FALSE;
	
	TCHAR achText[256];

	ASSERT( m_pBandDlg->m_pBandDo != NULL );

	if( m_pBandDlg->m_pBandDo )
	{
		if( m_pBandDlg->m_pBandDo->GetUndo(achText, 256) )
		{
			if( *achText )
			{
				strUndo += " ";
				strUndo += achText;
			}

			fEnable = TRUE;
		}
	}

	strUndo += strUndoAccel;
	pCmdUI->SetText( strUndo );
	pCmdUI->Enable( fEnable );
}

void CBandCtrl::OnEditRedo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	IPersistStream *pIPStream;

	m_pBand->QueryInterface( IID_IPersistStream, (void **) &pIPStream );
	if( pIPStream )
	{
		m_pBand->m_bInUndoRedo = true;
		m_pBandDlg->m_pBandDo->Redo( pIPStream );
		m_pBand->m_bInUndoRedo = false;
		pIPStream->Release();
	}

	// Update display
	m_pBandDlg->RefreshDisplay();

	// Notify DocRoot that Band has changed
	IDMUSProdNode* pIDocRootNode;

	if( SUCCEEDED ( m_pBand->GetDocRootNode(&pIDocRootNode) ) )
	{
		if( pIDocRootNode != (IDMUSProdNode *)m_pBand )
		{
			IDMUSProdNotifySink* pINotifySink;

			if( SUCCEEDED ( pIDocRootNode->QueryInterface(IID_IDMUSProdNotifySink, (void**)&pINotifySink) ) )
			{
				// Ask DocRoot if change should be sent
				if( pINotifySink->OnUpdate( m_pBand, GUID_BAND_ChangeNotifyMsg, NULL ) == S_OK )
				{
					m_pBand->SendBand();
				}

				pINotifySink->Release();
			}
		}

		pIDocRootNode->Release();
	}

    // Saving clears this, but we are still dirty
    m_pBand->SetModifiedFlag( TRUE );

	if(m_pBandDlg)
		m_pBandDlg->SetFocus();
}

void CBandCtrl::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strRedo;
	CString strRedoAccel;

	strRedo.LoadString( IDS_REDO );
	strRedoAccel.LoadString( IDS_REDO_ACCEL );

	BOOL fEnable = FALSE;
	
	TCHAR achText[256];

	ASSERT( m_pBandDlg->m_pBandDo != NULL );

	if( m_pBandDlg->m_pBandDo )
	{
		if( m_pBandDlg->m_pBandDo->GetRedo(achText, 256) )
		{
			if( *achText )
			{
				strRedo += " ";
				strRedo += achText;
			}

			fEnable = TRUE;
		}
	}

	strRedo += strRedoAccel;
	pCmdUI->SetText( strRedo );
	pCmdUI->Enable( fEnable );
}

void CBandCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	COleControl::OnSetFocus(pOldWnd);

	IDMUSProdTransport* pITransport;

	if( SUCCEEDED ( m_pBand->m_pIDocRootNode->QueryInterface( IID_IDMUSProdTransport, (void **)&pITransport ) ) )
	{
		m_pBand->m_pComponent->m_pIConductor->SetActiveTransport( pITransport, 0 );
		//m_pBand->Send();To Do: use args

		pITransport->Release();
	}
}

BOOL CBandCtrl::PreTranslateMessage( MSG* pMsg ) 
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
			}
			break;
	}

	return COleControl::PreTranslateMessage( pMsg );
}

void CBandCtrl::OnHelpFinder() 
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

void CBandCtrl::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{
	if(m_pBandDlg == NULL)
	{
		pCmdUI->Enable( false );
	}
	else
	{
		pCmdUI->Enable( true );
	}
}

void CBandCtrl::OnEditSelectAll() 
{
	if(m_pBandDlg)
	{
		m_pBandDlg->SelectAllPChannels();
	}
}
