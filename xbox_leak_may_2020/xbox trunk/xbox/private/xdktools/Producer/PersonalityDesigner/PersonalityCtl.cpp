// PersonalityCtl.cpp : Implementation of the CPersonalityCtrl ActiveX Control class.

#include "stdafx.h"
#pragma warning(disable:4201)
#include "PersonalityDesigner.h"
#include "PersonalityCtl.h"
#include "PersonalityPpg.h"
#include "chordbuilder.h"
#pragma warning(default:4201)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif





IMPLEMENT_DYNCREATE(CPersonalityCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CPersonalityCtrl, COleControl)
	//{{AFX_MSG_MAP(CPersonalityCtrl)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_APP_ABOUT, AboutBox)
	ON_WM_SETFOCUS()
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(ID_CUT, OnCut)
	ON_UPDATE_COMMAND_UI(ID_CUT, OnUpdateCut)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_UPDATE_COMMAND_UI(ID_DELETE, OnUpdateDelete)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_INSERT, OnInsert)
	ON_UPDATE_COMMAND_UI(ID_INSERT, OnUpdateInsert)
	ON_COMMAND(ID_SELECTALL, OnSelectall)
	ON_UPDATE_COMMAND_UI(ID_SELECTALL, OnUpdateSelectall)
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES, OnUpdateProperties)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_DIATONIC, OnDiatonic)
	ON_UPDATE_COMMAND_UI(ID_DIATONIC, OnUpdateDiatonic)
	ON_COMMAND(ID_DOM7THS, OnDom7ths)
	ON_UPDATE_COMMAND_UI(ID_DOM7THS, OnUpdateDom7ths)
	ON_COMMAND(ID_MAJOR7THS, OnMajor7ths)
	ON_UPDATE_COMMAND_UI(ID_MAJOR7THS, OnUpdateMajor7ths)
	ON_COMMAND(ID_MINOR7THS, OnMinor7ths)
	ON_UPDATE_COMMAND_UI(ID_MINOR7THS, OnUpdateMinor7ths)
	ON_COMMAND(ID_ALLMAJOR, OnAllmajor)
	ON_UPDATE_COMMAND_UI(ID_ALLMAJOR, OnUpdateAllmajor)
	ON_COMMAND(ID_ALLMINOR, OnAllminor)
	ON_UPDATE_COMMAND_UI(ID_ALLMINOR, OnUpdateAllminor)
	ON_COMMAND(ID_DIATONICTRIAD, OnDiatonictriad)
	ON_UPDATE_COMMAND_UI(ID_DIATONICTRIAD, OnUpdateDiatonictriad)
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_UNDO, OnUndo)
	ON_UPDATE_COMMAND_UI(ID_UNDO, OnUpdateUndo)
	ON_COMMAND(ID_REDO, OnRedo)
	ON_UPDATE_COMMAND_UI(ID_REDO, OnUpdateRedo)
	ON_WM_SIZE()
	ON_COMMAND(IDM_HELP_FINDER, OnHelpFinder)
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CPersonalityCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CPersonalityCtrl)
	// NOTE - ClassWizard will add and remove dispatch map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CPersonalityCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CPersonalityCtrl, COleControl)
	//{{AFX_EVENT_MAP(CPersonalityCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl Interface map

BEGIN_INTERFACE_MAP(CPersonalityCtrl, COleControl)
    INTERFACE_PART(CPersonalityCtrl, IID_IOleInPlaceActiveObject, MyOleInPlaceActiveObject)
    INTERFACE_PART(CPersonalityCtrl, IID_IDMUSProdEditor, Editor)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CPersonalityCtrl, 1)
	PROPPAGEID(CPersonalityPropPage::guid)
END_PROPPAGEIDS(CPersonalityCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CPersonalityCtrl, "CHORDMAPDESR.ChordMapCtrl.1",
	0xd433f95e, 0xb588, 0x11d0, 0x9e, 0xdc, 0, 0xaa, 0, 0xa2, 0x1b, 0xa9)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CPersonalityCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DPersonality =
		{ 0xd433f95c, 0xb588, 0x11d0, { 0x9e, 0xdc, 0, 0xaa, 0, 0xa2, 0x1b, 0xa9 } };
const IID BASED_CODE IID_DPersonalityEvents =
		{ 0xd433f95d, 0xb588, 0x11d0, { 0x9e, 0xdc, 0, 0xaa, 0, 0xa2, 0x1b, 0xa9 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwPersonalityOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CPersonalityCtrl, IDS_PERSONALITY, _dwPersonalityOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl::CPersonalityCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CPersonalityCtrl

BOOL CPersonalityCtrl::CPersonalityCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_PERSONALITY,
			IDB_PERSONALITY,
			afxRegApartmentThreading,
			_dwPersonalityOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl::CPersonalityCtrl - Constructor

CPersonalityCtrl::CPersonalityCtrl()
{
/////////////////////////////////////////////////////////////////////////
// Following commented out to prevent 3 ASSERTS that occur
// because we are not building a typelib
//	InitializeIIDs(&IID_DPersonality, &IID_DPersonalityEvents);
//
// Following code copied from InitializeIIDs (since we cannot override)
	m_piidPrimary = &IID_DPersonality;
	m_piidEvents = &IID_DPersonalityEvents;
	EnableTypeLib();	// Needed to prevent ASSERT when closing editor
	InitStockEventMask();
	InitStockPropMask();
//////////////////////////////////////////////////////////////////////////

	m_hWndContainer = NULL;
	m_hMenuInPlace = NULL;
	m_pToolBar = NULL;
	m_pdlgLastFocus = NULL;

	m_OnSizeCount = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl::~CPersonalityCtrl - Destructor

CPersonalityCtrl::~CPersonalityCtrl()
{
	TRACE( "CPersonalityCtrl destroying...\n" );

	RELEASE( m_pPersonality );
	if( m_pToolBar )
	{
		delete m_pToolBar;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl IDMUSProdEditor implementation

/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl::XEditor::AddRef

STDMETHODIMP_(ULONG) CPersonalityCtrl::XEditor::AddRef()
{
	METHOD_PROLOGUE_EX_( CPersonalityCtrl, Editor )

	return (ULONG)pThis->ExternalAddRef();
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl::XEditor::Release

STDMETHODIMP_(ULONG) CPersonalityCtrl::XEditor::Release()
{
	METHOD_PROLOGUE_EX_( CPersonalityCtrl, Editor )

	return (ULONG)pThis->ExternalRelease();
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl::XEditor::QueryInterface

STDMETHODIMP CPersonalityCtrl::XEditor::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_PROLOGUE_EX_( CPersonalityCtrl, Editor )

	return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl::XEditor implementation

/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl::XEditor::AttachObjects

HRESULT CPersonalityCtrl::XEditor::AttachObjects( IDMUSProdNode* pNode )
{
	METHOD_MANAGE_STATE( CPersonalityCtrl, Editor )

	ASSERT_VALID( pThis );
	ASSERT( pNode != NULL );

	GUID guidNode;

	pNode->GetNodeId( &guidNode );
	if( ::IsEqualGUID( guidNode, GUID_PersonalityRefNode ) )
	{
		CPersonalityRef* pPersonalityRef = (CPersonalityRef *)pNode;
		ASSERT( pPersonalityRef->m_pPersonality != NULL );
		pThis->m_pPersonality = pPersonalityRef->m_pPersonality;
	}
	else
	{
		pThis->m_pPersonality = (CPersonality *)pNode;
	}

	ASSERT( pThis->m_pPersonality != NULL );
	pThis->m_pPersonality->AddRef();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl::XEditor::OnInitMenuFilePrint

HRESULT CPersonalityCtrl::XEditor::OnInitMenuFilePrint( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CPersonalityCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl::XEditor::OnFilePrint

HRESULT CPersonalityCtrl::XEditor::OnFilePrint( void )
{
	METHOD_MANAGE_STATE( CPersonalityCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl::XEditor::OnInitMenuFilePrintPreview

HRESULT CPersonalityCtrl::XEditor::OnInitMenuFilePrintPreview( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CPersonalityCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl::XEditor::OnFilePrintPreview

HRESULT CPersonalityCtrl::XEditor::OnFilePrintPreview( void )
{
	METHOD_MANAGE_STATE( CPersonalityCtrl, Editor )

	return E_NOTIMPL;
}


HRESULT CPersonalityCtrl::XEditor::OnViewProperties( )
{
	METHOD_MANAGE_STATE( CPersonalityCtrl, Editor )

	HRESULT hr = E_FAIL;

	if( pThis->m_pPersonality )
	{
		ASSERT(pThis->m_pPersonality->m_pComponent != NULL);
		ASSERT( pThis->m_pPersonality->m_pComponent->m_pIFramework != NULL );
		IDMUSProdNode* pINode = (IDMUSProdNode *)pThis->m_pPersonality;
		IDMUSProdPropSheet* pIPropSheet;

		if( SUCCEEDED ( pThis->m_pPersonality->m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			if( pIPropSheet->IsShowing() == S_OK )
			{
				IDMUSProdPropPageObject* pIPageObject;

				if( SUCCEEDED ( pINode->QueryInterface( IID_IDMUSProdPropPageObject, (void **)&pIPageObject ) ) )
				{
					if( SUCCEEDED ( pIPageObject->OnShowProperties() ) )
					{
						hr = S_OK;
					}
					pIPageObject->Release();
				}
			}
			else
			{
				hr = S_OK;
			}

			pIPropSheet->Release();
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl::XEditor::OnF1Help

HRESULT CPersonalityCtrl::XEditor::OnF1Help( void )
{
	METHOD_MANAGE_STATE( CPersonalityCtrl, Editor )
	
    // Determine name of DMUSProd.exe help file
	CString strHelpFileName;

	if( theApp.GetHelpFileName( strHelpFileName ) )
	{
		strHelpFileName += "::/htm/ChordmapDesignerWindow.htm";
		::HtmlHelp( NULL, strHelpFileName, HH_DISPLAY_TOPIC, 0 );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl::XMyOleInPlaceActiveObject implementation

STDMETHODIMP_(ULONG) CPersonalityCtrl::XMyOleInPlaceActiveObject::AddRef()
{
	METHOD_MANAGE_STATE( CPersonalityCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.AddRef();
}

STDMETHODIMP_(ULONG) CPersonalityCtrl::XMyOleInPlaceActiveObject::Release()
{
	METHOD_MANAGE_STATE( CPersonalityCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.Release();
}

STDMETHODIMP CPersonalityCtrl::XMyOleInPlaceActiveObject::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_MANAGE_STATE( CPersonalityCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.QueryInterface( iid, ppvObj );
}

STDMETHODIMP CPersonalityCtrl::XMyOleInPlaceActiveObject::GetWindow( HWND* lphwnd )
{
	METHOD_MANAGE_STATE( CPersonalityCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.GetWindow( lphwnd );
}

STDMETHODIMP CPersonalityCtrl::XMyOleInPlaceActiveObject::ContextSensitiveHelp( BOOL fEnterMode )
{
	METHOD_MANAGE_STATE( CPersonalityCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.ContextSensitiveHelp( fEnterMode );
}

STDMETHODIMP CPersonalityCtrl::XMyOleInPlaceActiveObject::TranslateAccelerator( LPMSG lpmsg )
{
	METHOD_MANAGE_STATE( CPersonalityCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );
	
	return pThis->m_xOleInPlaceActiveObject.TranslateAccelerator( lpmsg );
}

STDMETHODIMP CPersonalityCtrl::XMyOleInPlaceActiveObject::OnFrameWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CPersonalityCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnFrameWindowActivate( fActivate );
}

STDMETHODIMP CPersonalityCtrl::XMyOleInPlaceActiveObject::OnDocWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CPersonalityCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnDocWindowActivate( fActivate );
}

STDMETHODIMP CPersonalityCtrl::XMyOleInPlaceActiveObject::ResizeBorder(
	LPCRECT, LPOLEINPLACEUIWINDOW, BOOL fFrameWindow )
{
	METHOD_MANAGE_STATE( CPersonalityCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

    if( fFrameWindow == TRUE )
	{
		pThis->OnShowToolBars();
	}

	return S_OK;
}

STDMETHODIMP CPersonalityCtrl::XMyOleInPlaceActiveObject::EnableModeless( BOOL fEnable )
{
	METHOD_MANAGE_STATE( CPersonalityCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.EnableModeless( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl::OnDraw - Drawing function

void CPersonalityCtrl::OnDraw(
			CDC* /*pdc*/, const CRect& /*rcBounds*/, const CRect& /*rcInvalid*/)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl::DoPropExchange - Persistence support

void CPersonalityCtrl::DoPropExchange(CPropExchange* pPX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl::OnResetState - Reset control to default state

void CPersonalityCtrl::OnResetState()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl::AboutBox - Display an "About" box to the user

#include "PersonalityAbout.h"
void CPersonalityCtrl::AboutBox()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPersonalityAbout dlgAbout;
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl message handlers

int CPersonalityCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
	m_hMenuInPlace = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDR_PERSONALITY_OPTIONS) );

	// Load control's accelerator table
	m_hAcceleratorTable = ::LoadAccelerators( theApp.m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1) );

	// Create control's toolbar
	CWnd* pWndParent = CWnd::FromHandle( m_hWndContainer );
	if( pWndParent )
	{
		/* to display toolbar, uncomment this code
		m_pToolBar = new CToolBar;
		m_pToolBar->Create( pWndParent );
		m_pToolBar->LoadToolBar( IDR_PERSONALITY_OPTIONS );
		m_pToolBar->SetBarStyle( m_pToolBar->GetBarStyle() | CBRS_SIZE_DYNAMIC );
		m_pToolBar->SetOwner( this );
		*/
	}

	// Create the Splitter and position it in the Editor
	// (temporary values added until coords are saved to design-time file)
	RECT rect;
	GetClientRect(&rect);
//	rect.left = MinSplitterXPos*2;
//	rect.right = rect.left + 6;
	rect.left = lpCreateStruct->cx;
	rect.right = rect.left + DefaultSplitterWidth;
	rect.bottom = lpCreateStruct->cy;
	rect.top = lpCreateStruct->y;

	m_wndSplitter.Create( (CWnd*)this, &rect );

	// Create the ChordWindow and position it in the First Splitter Pane.
	m_ChordDialog.InitializeDialog( m_pPersonality, this );
	m_ChordDialog.Create( IDD_CHORD_DIALOG, this );
	m_ChordDialog.ShowWindow( SW_SHOW );
	m_wndSplitter.SetFirstPane( &m_ChordDialog );

	// Initialize the SignPostDialog's SignPost Grid.
	m_SignPostDialog.SetSignPostListPtr( m_pPersonality->GetSignPostListPtr() );

	// Create the SignPost Window and position it in the Second Splitter Pane.
	m_SignPostDialog.InitializeDialog( &m_ChordDialog, m_pPersonality, this );
	m_SignPostDialog.Create( IDD_SIGNPOST_DIALOG, this );
	m_SignPostDialog.ShowWindow( SW_SHOW );
	m_wndSplitter.SetSecondPane( &m_SignPostDialog );

	// set up chord map to personality's length and timesig
	m_pPersonality->ChangeChordSignPostTimeSig();
	m_pPersonality->ChangeChordMapLength();


	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame::EndTrack

void CPersonalityCtrl::EndTrack( int nWidth )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	RECT rect;

	// tell personality the new position
	m_pPersonality->SetSplitterPos(nWidth);

	GetWindowRect( &rect );
	CalcWindowRect( &rect );


//	RecalcLayout();
}

BOOL CPersonalityCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return (pWnd == this)?CWnd::OnSetCursor(pWnd, nHitTest, message):FALSE;
}



HMENU CPersonalityCtrl::OnGetInPlaceMenu() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return m_hMenuInPlace;
}


////////////////////////////////////////////////
void CPersonalityCtrl::OnHideToolBars() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pPersonality != NULL );
	ASSERT( m_pPersonality->m_pComponent != NULL );

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

void CPersonalityCtrl::OnShowToolBars() 
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

void CPersonalityCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	COleControl::OnSetFocus(pOldWnd);

	IDMUSProdNode* pNode;
	IDMUSProdConductor* pConductor;

	if(SUCCEEDED(m_pPersonality->GetDocRootNode(&pNode)))
	{
		if(SUCCEEDED(m_pPersonality->m_pComponent->GetConductor(&pConductor)))
		{
			IDMUSProdTransport* pTransport;
			if(SUCCEEDED(pNode->QueryInterface(IID_IDMUSProdTransport, (void**)&pTransport)))
			{
				pConductor->SetActiveTransport(pTransport , NULL );
				pTransport->Release();
			}
			pConductor->Release();
		}
		else
		{
			ASSERT(FALSE);
		}
		pNode->Release();
	}
	else
	{
		ASSERT(FALSE);
	}
	
	if(m_pdlgLastFocus == NULL || m_pdlgLastFocus == &m_ChordDialog)
	{
		// Set the focus to the timeline
		IOleInPlaceObjectWindowless* pIOleInPlaceObjectWindowless;
		m_ChordDialog.m_pITimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleInPlaceObjectWindowless );
		if (pIOleInPlaceObjectWindowless)
		{
			HWND hwnd;
			if (pIOleInPlaceObjectWindowless->GetWindow(&hwnd) == S_OK)
			{
				::SetFocus(hwnd);
			}
			pIOleInPlaceObjectWindowless->Release();
		}
	}
	else
	{
		::SetFocus(m_SignPostDialog.m_hWnd);
	}
	// if null and a style has been added, this should set it.
	m_pPersonality->InitializeStyleReference();
}


void CPersonalityCtrl::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	
	COleControl::OnGetMinMaxInfo(lpMMI);
}


void CPersonalityCtrl::OnCut() 
{
	SendEdit(ID_CUT);	
}

void CPersonalityCtrl::OnUpdateCut(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	pCmdUI->Enable(EditAvailable(ID_CUT));	
	
}

void CPersonalityCtrl::OnDelete() 
{
	SendEdit(ID_DELETE);
}

void CPersonalityCtrl::OnUpdateDelete(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	pCmdUI->Enable(EditAvailable(ID_DELETE));
	
}

void CPersonalityCtrl::OnEditCopy() 
{
	SendEdit(ID_EDIT_COPY);	
}

void CPersonalityCtrl::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	pCmdUI->Enable(EditAvailable(ID_EDIT_COPY));
	
}

void CPersonalityCtrl::OnEditPaste() 
{
	SendEdit(ID_EDIT_PASTE);	
}

void CPersonalityCtrl::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	pCmdUI->Enable(EditAvailable(ID_EDIT_PASTE));
	
}



void CPersonalityCtrl::OnInsert() 
{
	SendEdit(ID_INSERT);
}

void CPersonalityCtrl::OnUpdateInsert(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	pCmdUI->Enable(EditAvailable(ID_INSERT));
	
}

void CPersonalityCtrl::OnSelectall() 
{
	SendEdit(ID_SELECTALL);	
}

void CPersonalityCtrl::OnUpdateSelectall(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	pCmdUI->Enable(EditAvailable(ID_SELECTALL));	
}

void CPersonalityCtrl::OnProperties() 
{
	m_xEditor.OnViewProperties();	
}

void CPersonalityCtrl::OnUpdateProperties(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	pCmdUI->Enable(TRUE);
		
}

BOOL CPersonalityCtrl::EditAvailable(WPARAM wId)
{
	if(m_pdlgLastFocus == &m_ChordDialog)
	{
		return TimelineEditAvailable(wId);
	}
	else if(m_pdlgLastFocus == &m_SignPostDialog)
	{
		return SignPostEditAvailable(wId);
	}
	else
	{
		m_pdlgLastFocus = &m_ChordDialog;
		return TimelineEditAvailable(wId);
	}
}

void CPersonalityCtrl::SendEdit(WPARAM wId)
{
	if(m_pdlgLastFocus == &m_ChordDialog)
	{
		SendEditToTimeline(wId);
	}
	else if(m_pdlgLastFocus == &m_SignPostDialog)
	{
		SendEditToSignPost(wId);
	}
	else
	{
		m_pdlgLastFocus = &m_ChordDialog;
		SendEditToTimeline(wId);
	}
}

void CPersonalityCtrl::SendEditToTimeline(WPARAM wId)
{
	HRESULT			hr = S_OK;
	BOOL			bAvailable = FALSE;
	IDMUSProdTimelineEdit	*pITimelineEdit;
	if(m_ChordDialog.m_pITimeline != NULL)
	{
		if(SUCCEEDED(m_ChordDialog.m_pITimeline->QueryInterface(IID_IDMUSProdTimelineEdit, (void **) &pITimelineEdit)))
		{
			bAvailable = TimelineEditAvailable(wId);
			ASSERT(bAvailable);
			if(bAvailable)
			{
				switch(wId)
				{
				case ID_CUT:
				case ID_EDIT_CUT:
					hr = pITimelineEdit->Cut(NULL);
					break;
				case ID_EDIT_COPY:
					hr = pITimelineEdit->Copy(NULL);
					break;
				case ID_EDIT_PASTE:
					hr = pITimelineEdit->Paste(NULL);
					break;
				case ID_INSERT:
					hr = pITimelineEdit->Insert();
					break;
				case ID_DELETE:
					hr = pITimelineEdit->Delete();
					break;
				case ID_SELECTALL:
				case ID_EDIT_SELECT_ALL:
					hr = pITimelineEdit->SelectAll();
					break;
				default:
					break;
				}
			}
			pITimelineEdit->Release();
		}
	}

//	ASSERT(SUCCEEDED(hr) || hr == E_NOTIMPL || hr == E_UNEXPECTED);
}

BOOL CPersonalityCtrl::TimelineEditAvailable(WPARAM wId)
{
	HRESULT					hr = S_OK;
	IDMUSProdTimelineEdit	*pITimelineEdit;

	if(m_ChordDialog.m_pITimeline != NULL)
	{
		if(SUCCEEDED(m_ChordDialog.m_pITimeline->QueryInterface(IID_IDMUSProdTimelineEdit, (void **) &pITimelineEdit)))
		{
			switch(wId)
			{
			case ID_CUT:
			case ID_EDIT_CUT:
				hr = pITimelineEdit->CanCut();
				break;
			case ID_EDIT_COPY:
				hr = pITimelineEdit->CanCopy();
				break;
			case ID_EDIT_PASTE:
				hr = pITimelineEdit->CanPaste(NULL);
//				hr = S_FALSE;	// because we don't know where the mouse is				
				break;
			case ID_INSERT:
				hr = pITimelineEdit->CanInsert();
//				hr = S_FALSE;	// because we don't know where the mouse is				
				break;
			case ID_DELETE:
				hr = pITimelineEdit->CanDelete();
				break;
			case ID_SELECTALL:
			case ID_EDIT_SELECT_ALL:
				hr = pITimelineEdit->CanSelectAll();
				break;
			default:
				hr = S_FALSE;
			}
			pITimelineEdit->Release();
		}
		if(hr == S_OK)
		{
			return TRUE;
		}
	}
	return FALSE;
}


void CPersonalityCtrl::SendEditToSignPost(WPARAM wId)
{
	HRESULT			hr = S_OK;
	BOOL			bAvailable = FALSE;
	bAvailable = SignPostEditAvailable(wId);
	ASSERT(bAvailable);
	if(bAvailable)
	{
		switch(wId)
		{
		case ID_CUT:
			hr = m_SignPostDialog.Cut(NULL);
			break;
		case ID_EDIT_COPY:
			hr = m_SignPostDialog.Copy(NULL);
			break;
		case ID_EDIT_PASTE:
			hr = m_SignPostDialog.Paste(NULL);
			break;
		case ID_INSERT:
			hr = m_SignPostDialog.Insert(NULL);
			break;
		case ID_DELETE:
			hr = m_SignPostDialog.Delete();
			break;
		case ID_SELECTALL:
			hr = m_SignPostDialog.SelectAll();
			break;
		default:
			break;
		}
	}

	ASSERT(SUCCEEDED(hr) || hr == E_NOTIMPL);
}

BOOL CPersonalityCtrl::SignPostEditAvailable(WPARAM wId)
{
	HRESULT			hr;

	switch(wId)
	{
	case ID_CUT:
		hr = m_SignPostDialog.CanCut();
		break;
	case ID_EDIT_COPY:
		hr = m_SignPostDialog.CanCopy();
		break;
	case ID_EDIT_PASTE:
		hr = m_SignPostDialog.CanPaste();
		break;
	case ID_INSERT:
		hr = m_SignPostDialog.CanInsert();
		break;
	case ID_DELETE:
		hr = m_SignPostDialog.CanDelete();
		break;
	case ID_SELECTALL:
		hr = S_FALSE;	// this assumes knowledge of signpost
		break;
	default:
		hr = S_FALSE;
	}
	if(hr == S_OK)
	{
		return TRUE;
	}
	return FALSE;
}




BOOL CPersonalityCtrl::OnEraseBkgnd(CDC* pDC) 
{
	UNREFERENCED_PARAMETER(pDC);
	return FALSE;
}

void CPersonalityCtrl::OnDiatonic() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	Personality* pPersonality = m_pPersonality->GetPersonality();
	long lScalePattern = Rotate24( pPersonality->m_scalepattern, m_pPersonality->GetAbsKey() );
	ChordBuilder cb(lScalePattern, ChordBuilder::Seventh, m_pPersonality->GetAbsKey());
	for(int i = 0; i < 24; i++)
	{
		strcpy(pPersonality->m_chordpalette.m_chords[i].Name(), cb.ChordName(i));
		for(int j = 0; j < DMPolyChord::MAX_POLY; j++)
		{
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ChordPattern() = cb.ChordPattern(i);
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ScalePattern() = pPersonality->m_scalepattern;
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ScaleRoot() = (BYTE)m_pPersonality->GetAbsKey();
		}
	}
	m_pPersonality->PaletteChordType() = ChordBuilder::Seventh;
	m_ChordDialog.m_pITimeline->Refresh();
	m_pPersonality->Modified() = TRUE;
	m_pPersonality->SyncPersonalityWithEngine(CPersonality::syncPersonality);	
}

void CPersonalityCtrl::OnUpdateDiatonic(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	pCmdUI->Enable(TRUE);
	
}

void CPersonalityCtrl::OnDom7ths() 
{
	Personality* pPersonality = m_pPersonality->GetPersonality();
	ChordBuilder cb(0x000491, "7");
	for(int i = 0; i < 24; i++)
	{
		strcpy(pPersonality->m_chordpalette.m_chords[i].Name(), cb.ChordName(i));
		for(int j = 0; j < DMPolyChord::MAX_POLY; j++)
		{
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ChordPattern() = cb.ChordPattern(i);
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ScalePattern() = pPersonality->m_scalepattern;
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ScaleRoot() = (BYTE)m_pPersonality->GetAbsKey();
		}
	}
	m_pPersonality->PaletteChordType() = ChordBuilder::AllDom7;
	m_ChordDialog.m_pITimeline->Refresh();	
	m_pPersonality->Modified() = TRUE;
	m_pPersonality->SyncPersonalityWithEngine(CPersonality::syncPersonality);	
}

void CPersonalityCtrl::OnUpdateDom7ths(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	pCmdUI->Enable(TRUE);
	
}

void CPersonalityCtrl::OnMajor7ths() 
{
	Personality* pPersonality = m_pPersonality->GetPersonality();
	ChordBuilder cb(0x000891, "maj7");
	for(int i = 0; i < 24; i++)
	{
		strcpy(pPersonality->m_chordpalette.m_chords[i].Name(), cb.ChordName(i));
		for(int j = 0; j < DMPolyChord::MAX_POLY; j++)
		{
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ChordPattern() = cb.ChordPattern(i);
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ScalePattern() = pPersonality->m_scalepattern;
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ScaleRoot() = (BYTE)m_pPersonality->GetAbsKey();
		}
	}
	m_pPersonality->PaletteChordType() = ChordBuilder::AllMaj7;
	m_ChordDialog.m_pITimeline->Refresh();	
	m_pPersonality->Modified() = TRUE;
	m_pPersonality->SyncPersonalityWithEngine(CPersonality::syncPersonality);		
}

void CPersonalityCtrl::OnUpdateMajor7ths(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	pCmdUI->Enable(TRUE);
	
}

void CPersonalityCtrl::OnMinor7ths() 
{
	Personality* pPersonality = m_pPersonality->GetPersonality();
	ChordBuilder cb(0x000489, "min7");
	for(int i = 0; i < 24; i++)
	{
		strcpy(pPersonality->m_chordpalette.m_chords[i].Name(), cb.ChordName(i));
		for(int j = 0; j < DMPolyChord::MAX_POLY; j++)
		{
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ChordPattern() = cb.ChordPattern(i);
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ScalePattern() = pPersonality->m_scalepattern;
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ScaleRoot() = (BYTE)m_pPersonality->GetAbsKey();
		}
	}
	m_pPersonality->PaletteChordType() = ChordBuilder::AllMin7;
	m_ChordDialog.m_pITimeline->Refresh();	
	m_pPersonality->Modified() = TRUE;
	m_pPersonality->SyncPersonalityWithEngine(CPersonality::syncPersonality);	
}

void CPersonalityCtrl::OnUpdateMinor7ths(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	pCmdUI->Enable(TRUE);
	
}

void CPersonalityCtrl::OnAllmajor() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	Personality* pPersonality = m_pPersonality->GetPersonality();
	ChordBuilder cb(0x000091, "maj");
	for(int i = 0; i < 24; i++)
	{
		strcpy(pPersonality->m_chordpalette.m_chords[i].Name(), cb.ChordName(i));
		for(int j = 0; j < DMPolyChord::MAX_POLY; j++)
		{
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ChordPattern() = cb.ChordPattern(i);
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ScalePattern() = pPersonality->m_scalepattern;
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ScaleRoot() = (BYTE)m_pPersonality->GetAbsKey();
		}
	}
	m_pPersonality->PaletteChordType() = ChordBuilder::AllMaj;
	m_ChordDialog.m_pITimeline->Refresh();	
	m_pPersonality->Modified() = TRUE;
	m_pPersonality->SyncPersonalityWithEngine(CPersonality::syncPersonality);
}

void CPersonalityCtrl::OnUpdateAllmajor(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	pCmdUI->Enable(TRUE);
	
}

void CPersonalityCtrl::OnAllminor() 
{
	Personality* pPersonality = m_pPersonality->GetPersonality();
	ChordBuilder cb(0x000089, "min");
	for(int i = 0; i < 24; i++)
	{
		strcpy(pPersonality->m_chordpalette.m_chords[i].Name(), cb.ChordName(i));
		for(int j = 0; j < DMPolyChord::MAX_POLY; j++)
		{
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ChordPattern() = cb.ChordPattern(i);
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ScalePattern() = pPersonality->m_scalepattern;
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ScaleRoot() = (BYTE)m_pPersonality->GetAbsKey();
		}
	}
	m_pPersonality->PaletteChordType() = ChordBuilder::AllMin;
	m_ChordDialog.m_pITimeline->Refresh();	
	m_pPersonality->Modified() = TRUE;
	m_pPersonality->SyncPersonalityWithEngine(CPersonality::syncPersonality);	
}

void CPersonalityCtrl::OnUpdateAllminor(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	pCmdUI->Enable(TRUE);
	
}


void CPersonalityCtrl::OnDiatonictriad() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	Personality* pPersonality = m_pPersonality->GetPersonality();
	long lScalePattern = Rotate24( pPersonality->m_scalepattern, m_pPersonality->GetAbsKey() );
	ChordBuilder cb(lScalePattern, ChordBuilder::Triad, m_pPersonality->GetAbsKey());
	for(int i = 0; i < 24; i++)
	{
		strcpy(pPersonality->m_chordpalette.m_chords[i].Name(), cb.ChordName(i));
		for(int j = 0; j < DMPolyChord::MAX_POLY; j++)
		{
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ChordPattern() = cb.ChordPattern(i);
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ScalePattern() = pPersonality->m_scalepattern;
			pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ScaleRoot() = (BYTE)m_pPersonality->GetAbsKey();
		}
	}
	m_pPersonality->PaletteChordType() = ChordBuilder::Triad;	
	m_ChordDialog.m_pITimeline->Refresh();
	m_pPersonality->Modified() = TRUE;
	m_pPersonality->SyncPersonalityWithEngine(CPersonality::syncPersonality);
}

void CPersonalityCtrl::OnUpdateDiatonictriad(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	pCmdUI->Enable(TRUE);
}

void CPersonalityCtrl::OnDestroy() 
{
	COleControl::OnDestroy();
	
	if( m_hMenuInPlace )
	{
		::DestroyMenu(m_hMenuInPlace);
		m_hMenuInPlace = NULL;
	}

	if( m_hAcceleratorTable )
	{
		::DestroyAcceleratorTable( m_hAcceleratorTable );
		m_hAcceleratorTable = NULL;
	}
	
}

BOOL CPersonalityCtrl::PreTranslateMessage(MSG* pMsg) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	::TranslateAccelerator(m_hWnd, m_hAcceleratorTable, pMsg);
	
	return COleControl::PreTranslateMessage(pMsg);
}

void CPersonalityCtrl::OnEditCut() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(EditAvailable(ID_EDIT_CUT))
		SendEdit(ID_EDIT_CUT);	
}

void CPersonalityCtrl::OnEditSelectAll() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(EditAvailable(ID_EDIT_SELECT_ALL))
		SendEdit(ID_EDIT_SELECT_ALL);	
	
}

void CPersonalityCtrl::OnUndo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pPersonality != NULL);
	TCHAR tbuf[256];
	if(m_pPersonality != NULL)
	{
		ASSERT(m_pPersonality->m_pUndoMgr != NULL);
		if(m_pPersonality->m_pUndoMgr != NULL && m_pPersonality->m_pUndoMgr->GetUndo(tbuf, 256))
		{
			HWND hwnd = ::GetFocus();

			CString person, chord, signpost, ins, del, cut, paste, str, length, timesig;
			person.LoadString(IDS_PERSONALITYUNDO_HEADER);
			chord.LoadString(IDS_CHORDMAPUNDO_HEADER);
			signpost.LoadString(IDS_SIGNPOSTUNDO_HEADER);
			ins.LoadString(IDS_INSERT_UNDO);
			del.LoadString(IDS_DELETE_UNDO);
			cut.LoadString(IDS_CUT_UNDO);
			paste.LoadString(IDS_PASTE_UNDO);
			length.LoadString(IDS_UNDO_CHORDMAPLENGTH);
			timesig.LoadString(IDS_UNDO_TIMESIG);
			str = tbuf;
			
			m_pPersonality->m_pUndoMgr->Undo(m_pPersonality);
			
			m_pPersonality->RefreshProperties();

			if(!_strnicmp(chord, tbuf, chord.GetLength()) && 
				( (str.Find(ins) > -1) || (str.Find(cut) > -1) || (str.Find(del) > -1) || (str.Find(paste) > -1))) 
			{
				m_ChordDialog.Refresh(true);
			}
			else if(str.Find(timesig) > -1)
			{
				m_pPersonality->ChangeChordSignPostTimeSig();
				m_pPersonality->ComputeChordMap();
//				m_ChordDialog.Refresh(true);
			}
			else if(str.Find(length) > -1)
			{
				m_pPersonality->ChangeChordMapLength();
				m_pPersonality->m_pComponent->m_pIFramework->RefreshNode( (IDMUSProdNode*)m_pPersonality );
//				m_ChordDialog.Refresh(false);
//				m_pPersonality->Modified() = TRUE;
//				return;
			}
			else
			{
				m_ChordDialog.Refresh(false);
			}

			if((str.Find(signpost) > -1) && 
				( (str.Find(ins) > -1) || (str.Find(del) > -1) || (str.Find(cut) > -1) || (str.Find(paste) > -1) ) )
			{
				m_pPersonality->LinkAll(true);
				m_SignPostDialog.Refresh(true);
			}
			else if(str.Find(signpost) > -1)
			{
				m_SignPostDialog.Refresh(false);
			}
			else
			{
				m_SignPostDialog.Refresh(false);
			}

			m_pPersonality->OnNameChange();
			m_pPersonality->m_pComponent->m_pIFramework->RefreshNode( (IDMUSProdNode*)m_pPersonality );
			m_pPersonality->Modified() = TRUE;
			::SetFocus(hwnd);
		}
	}	
}

void CPersonalityCtrl::OnUpdateUndo(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pPersonality != NULL);
	TCHAR tbuf[256];
	
	if(m_pPersonality != NULL)
	{
		ASSERT(m_pPersonality->m_pUndoMgr != NULL);
		if(m_pPersonality->m_pUndoMgr != NULL)
		{
			if(m_pPersonality->m_pUndoMgr->GetUndo(tbuf, 256))
			{
				CString str,str1;
				str.LoadString(IDS_UNDO);
				str1.LoadString(IDS_UNDOACCEL);
				if(*tbuf)
				{
					str += " ";
					str += tbuf;
				}
				str += str1;
				pCmdUI->SetText(str);
				pCmdUI->Enable(TRUE);
				return;
			}
		}
	}
	pCmdUI->Enable(FALSE);
}

void CPersonalityCtrl::OnRedo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pPersonality != NULL);
	TCHAR tbuf[256];
	if(m_pPersonality != NULL)
	{
		ASSERT(m_pPersonality->m_pUndoMgr != NULL);
		if(m_pPersonality->m_pUndoMgr != NULL && m_pPersonality->m_pUndoMgr->GetRedo(tbuf, 256))
		{
			HWND hwnd = ::GetFocus();
			CString person, chord, signpost, ins, del, cut, paste, str, length, timesig;
			person.LoadString(IDS_PERSONALITYUNDO_HEADER);
			chord.LoadString(IDS_CHORDMAPUNDO_HEADER);
			signpost.LoadString(IDS_SIGNPOSTUNDO_HEADER);
			ins.LoadString(IDS_INSERT_UNDO);
			del.LoadString(IDS_DELETE_UNDO);
			cut.LoadString(IDS_CUT_UNDO);
			paste.LoadString(IDS_PASTE_UNDO);
			length.LoadString(IDS_UNDO_CHORDMAPLENGTH);
			timesig.LoadString(IDS_UNDO_TIMESIG);
			str = tbuf;

			m_pPersonality->m_pUndoMgr->Redo(m_pPersonality);

			m_pPersonality->RefreshProperties();

			if(!_strnicmp(chord, tbuf, chord.GetLength()) && 
				( (str.Find(ins) > -1) || (str.Find(cut) > -1) || (str.Find(del) > -1) || (str.Find(paste) > -1))) 
			{
				m_ChordDialog.Refresh(true);
			}
			else if(str.Find(timesig) > -1)
			{
				m_pPersonality->ChangeChordSignPostTimeSig();
				m_pPersonality->ComputeChordMap();
//				m_ChordDialog.Refresh(true);
			}
			else if(str.Find(length) > -1)
			{
				m_pPersonality->ChangeChordMapLength();
//				m_ChordDialog.Refresh(true);
			}
			else
			{
				m_ChordDialog.Refresh(false);
			}

			if((str.Find(signpost) > -1) && 
				( (str.Find(ins) > -1) || (str.Find(del) > -1) || (str.Find(cut) > -1) || (str.Find(paste) > -1) ) )
			{
				m_pPersonality->LinkAll(true);
				m_SignPostDialog.Refresh(true);
			}
			else if(str.Find(signpost) > -1)
			{
				m_SignPostDialog.Refresh(false);
			}
			else
			{
				m_SignPostDialog.Refresh(false);
			}

		// Update the Project Node.
			m_pPersonality->OnNameChange();
			m_pPersonality->m_pComponent->m_pIFramework->RefreshNode( (IDMUSProdNode*)m_pPersonality );
			m_pPersonality->Modified() = TRUE;
			::SetFocus(hwnd);
		}
	}	
}

void CPersonalityCtrl::OnUpdateRedo(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pPersonality != NULL);
	TCHAR tbuf[256];
	if(m_pPersonality != NULL)
	{
		ASSERT(m_pPersonality->m_pUndoMgr != NULL);
		if(m_pPersonality->m_pUndoMgr != NULL)
		{
			if(m_pPersonality->m_pUndoMgr->GetRedo(tbuf, 256))
			{
				CString str,str1;
				str.LoadString(IDS_REDO);
				str1.LoadString(IDS_REDOACCEL);
				if(*tbuf)
				{
					str += " ";
					str += tbuf;
				}
				str += str1;
				pCmdUI->SetText(str);
				pCmdUI->Enable(TRUE);
				return;
			}
		}
	}
	pCmdUI->Enable(FALSE);
}

void CPersonalityCtrl::OnSize(UINT nType, int cx, int cy) 
{
	COleControl::OnSize(nType, cx, cy);
	if(m_OnSizeCount == 1)
	{
		// get splitter position from personality
		WORD pos = m_pPersonality->GetSplitterPos();
		// calculate where splitter should be
		if(pos == 0)
		{
			// put it right in the middle
			pos = (cx - DefaultSplitterWidth)/2;
			// tell personality where it is so it can save it
			m_pPersonality->SetSplitterPos(pos, false);
		}
		// splitter is right side of chord dialog and left side
		// of signpost dialog
		RECT rect;
		GetClientRect(&rect);
		MoveWindow(0,0,cx,cy);
		rect.left = pos;
		rect.right = rect.left + DefaultSplitterWidth;
		m_wndSplitter.SetTracker(rect);
		m_ChordDialog.MoveWindow(0,0,pos, rect.bottom);
		m_SignPostDialog.MoveWindow(rect.right, 0, cx - rect.right, rect.bottom);
//		m_ChordDialog.OnSize(nType, pos, rect.bottom);
//		m_SignPostDialog.OnSize(nType, rect.right - (pos + DefaultSplitterWidth), rect.bottom);
	}
	else if(m_OnSizeCount == 0)
	{
		++m_OnSizeCount;
	}
}

void CPersonalityCtrl::OnHelpFinder() 
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


