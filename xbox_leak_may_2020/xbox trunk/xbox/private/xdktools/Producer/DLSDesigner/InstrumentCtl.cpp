// InstrumentCtl.cpp : Implementation of the CInstrumentCtrl ActiveX Control class.

#include "stdafx.h"
#include "DLSDesignerDLL.h"
#include "Instrument.h"
#include "Articulation.h"
#include "Region.h"
#include "InstrumentFVEditor.h"
#include "InstrumentCtl.h"
#include "DLSLoadSaveUtils.h"
#include "DlsDefsPlus.h"
#include "UndoMan.h"
#include "AboutDlg.h"
#include "Collection.h"
#include "InstrumentPropPgMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int CInstrumentCtrl::m_nInstrumentCtrlRefCount = 0;
HMENU CInstrumentCtrl::m_hMenuInPlace = NULL;


IMPLEMENT_DYNCREATE(CInstrumentCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CInstrumentCtrl, COleControl)
	//{{AFX_MSG_MAP(CInstrumentCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CHILDACTIVATE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_WM_CHAR()
	ON_COMMAND(IDM_HELP_FINDER, OnHelpFinder)
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CInstrumentCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CInstrumentCtrl)
	// NOTE - ClassWizard will add and remove dispatch map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CInstrumentCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CInstrumentCtrl, COleControl)
	//{{AFX_EVENT_MAP(CInstrumentCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl Interface map

BEGIN_INTERFACE_MAP(CInstrumentCtrl, COleControl)
	INTERFACE_PART(CInstrumentCtrl, IID_IOleInPlaceActiveObject, MyOleInPlaceActiveObject)
	INTERFACE_PART(CInstrumentCtrl, IID_IDMUSProdEditor, Editor)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
/*
BEGIN_PROPPAGEIDS(CInstrumentCtrl, 1)
	//PROPPAGEID(CInstrumentPropPg::guid)
END_PROPPAGEIDS(CInstrumentCtrl)
*/

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CInstrumentCtrl, "DLS Designer.InstrumentCtrl.1",
	0xbc964e8a, 0x96f7, 0x11d0, 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CInstrumentCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DInstrument =
		{ 0xbc964e88, 0x96f7, 0x11d0, { 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };
const IID BASED_CODE IID_DInstrumentEvents =
		{ 0xbc964e89, 0x96f7, 0x11d0, { 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwInstrumentOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_SIMPLEFRAME;

IMPLEMENT_OLECTLTYPE(CInstrumentCtrl, IDS_INSTRUMENT, _dwInstrumentOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::CInstrumentCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CInstrumentCtrl

BOOL CInstrumentCtrl::CInstrumentCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_INSTRUMENT,
			IDB_INSTRUMENT,
			afxRegApartmentThreading,
			_dwInstrumentOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::CInstrumentCtrl - Constructor

CInstrumentCtrl::CInstrumentCtrl() : 
m_pInstrument(NULL),
m_pAttachedNode(NULL)
{
/////////////////////////////////////////////////////////////////////////
// Following commented out to prevent 3 ASSERTS that occur
// because we are not building a typelib
//	InitializeIIDs(&IID_DInstrument, &IID_DInstrumentEvents);
//
// Following code copied from InitializeIIDs (since we cannot override)
	m_piidPrimary = &IID_DInstrument;
	m_piidEvents = &IID_DInstrumentEvents;
	EnableTypeLib();	// Needed to prevent ASSERT when closing editor
	InitStockEventMask();
	InitStockPropMask();
//////////////////////////////////////////////////////////////////////////

	EnableSimpleFrame();

	m_nInstrumentCtrlRefCount++;
	
	// J3 we need to do this better maybe a global class that contains m_nMIDINoteOns

	for(int i = 0; i < 128; i++)
	{
		m_nMIDINoteOns[i] = 0;
	}

	m_nLastSpacebarNote = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::~CInstrumentCtrl - Destructor

CInstrumentCtrl::~CInstrumentCtrl()
{
	m_nInstrumentCtrlRefCount--;
}


BOOL CInstrumentCtrl::PreTranslateMessage( MSG* pMsg )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// This is to make sure menu shortcuts (<Alt-key>) get to the menu.
	if (  WM_SYSCHAR == pMsg->message )
	{
		return FALSE;
	}

	// If control key is down we have to exit or IsDialogMessage() will eat the frames accelerators
	if ( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
	{
		return FALSE;
	}

	// Change Enter key into Tab so we can use the Enter key to go to the next control	
	if ((WM_KEYDOWN == pMsg->message) && (VK_RETURN == pMsg->wParam))
		pMsg->wParam = VK_TAB;
	
	// Respond to spacebar with audition
	HWND hEditorHWnd;
	m_pInstrument->GetEditorWindow(&hEditorHWnd);
	CWnd* pParentWnd = CWnd::FromHandle(hEditorHWnd);
	if((WM_KEYDOWN == pMsg->message) && (VK_SPACE == pMsg->wParam) && pParentWnd->IsChild(CWnd::GetFocus())) 
	{
		OnKeyDown(VK_SPACE,0,0);
		return TRUE;
	}
	if((WM_KEYUP == pMsg->message) && (VK_SPACE == pMsg->wParam) && pParentWnd->IsChild(CWnd::GetFocus()))
	{
		OnKeyUp(VK_SPACE,0,0);
		return TRUE;
	}
	// This insures that the tabs are processed in the dialog template
	if(VK_TAB == pMsg->wParam)
	{
		if(IsDialogMessage(pMsg))
			return TRUE;
		else
			return FALSE;
	}
	else
		return COleControl::PreTranslateMessage(pMsg);
} 

/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl IDMUSProdEditor implementation

/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::XEditor::AddRef

STDMETHODIMP_(ULONG) CInstrumentCtrl::XEditor::AddRef()
{
	METHOD_PROLOGUE_EX_( CInstrumentCtrl, Editor )

	return (ULONG)pThis->ExternalAddRef();
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::XEditor::Release

STDMETHODIMP_(ULONG) CInstrumentCtrl::XEditor::Release()
{
	METHOD_PROLOGUE_EX_( CInstrumentCtrl, Editor )

	return (ULONG)pThis->ExternalRelease();
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::XEditor::QueryInterface

STDMETHODIMP CInstrumentCtrl::XEditor::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_PROLOGUE_EX_( CInstrumentCtrl, Editor )

	return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::XEditor::AttachObjects implementation

HRESULT CInstrumentCtrl::XEditor::AttachObjects(IDMUSProdNode* pNode)
{
	METHOD_MANAGE_STATE( CInstrumentCtrl, Editor )

	ASSERT_VALID( pThis );
	ASSERT( pNode != NULL );

	HRESULT hr = S_OK;
	GUID guid;

	pNode->GetNodeId(&guid);
	
	pThis->m_pAttachedNode = pNode;
	if(guid == GUID_InstrumentNode)
	{
		pThis->m_pInstrument = (CInstrument *)pNode;
		(pThis->m_pInstrument)->AddRef();
		(pThis->m_pInstrument)->m_pInstrumentCtrl = pThis;
	}

	else if(guid == GUID_RegionNode)
	{
		CRegion* pRegion = (CRegion *)pNode;
		
		CInstrument* pInstrument = pRegion->m_pInstrument;
		
		pThis->m_pInstrument = pInstrument;
		(pThis->m_pInstrument)->AddRef();
		(pThis->m_pInstrument)->m_pInstrumentCtrl = pThis;
	}

	else if(guid == GUID_ArticulationNode)
	{
		CArticulation* pArt = (CArticulation *)pNode;
		
		// Determines that the Articulation has a valid parent
		// either an instrument or a region
#ifdef _DEBUG
		pArt->ValidateParent();
#endif 
		CInstrument* pInstrument = pArt->GetInstrument();
		if(pInstrument)
		{
			pThis->m_pInstrument = pInstrument;
			(pThis->m_pInstrument)->AddRef();
			(pThis->m_pInstrument)->m_pInstrumentCtrl = pThis;
		}
	}
	else
	{
		ASSERT(FALSE);			
		hr = E_FAIL;	
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::XEditor::OnViewProperties implementation

HRESULT CInstrumentCtrl::XEditor::OnViewProperties()
{
	METHOD_MANAGE_STATE( CInstrumentCtrl, Editor )
	ASSERT_VALID( pThis );
	
	return pThis->m_pInstrument->OnViewProperties();
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::XEditor::OnInitMenuFilePrint

HRESULT CInstrumentCtrl::XEditor::OnInitMenuFilePrint( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CInstrumentCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::XEditor::OnFilePrint

HRESULT CInstrumentCtrl::XEditor::OnFilePrint( void )
{
	METHOD_MANAGE_STATE( CInstrumentCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::XEditor::OnInitMenuFilePrintPreview

HRESULT CInstrumentCtrl::XEditor::OnInitMenuFilePrintPreview( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CInstrumentCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::XEditor::OnFilePrintPreview

HRESULT CInstrumentCtrl::XEditor::OnFilePrintPreview( void )
{
	METHOD_MANAGE_STATE( CInstrumentCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::XEditor::OnF1Help implementation

HRESULT CInstrumentCtrl::XEditor::OnF1Help()
{
	METHOD_MANAGE_STATE( CInstrumentCtrl, Editor )
	ASSERT_VALID( pThis );
	
    // Determine name of DMUSProd.exe help file
	CString strHelpFileName;

	if( theApp.GetHelpFileName( strHelpFileName ) )
	{
		strHelpFileName += "::/htm/DLSDesignerWindow.htm";
		::HtmlHelp( NULL, strHelpFileName, HH_DISPLAY_TOPIC, 0 );
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::XMyOleInPlaceActiveObject implementation

STDMETHODIMP_(ULONG) CInstrumentCtrl::XMyOleInPlaceActiveObject::AddRef()
{
	METHOD_MANAGE_STATE( CInstrumentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CInstrumentCtrl::XMyOleInPlaceActiveObject::Release()
{
	METHOD_MANAGE_STATE( CInstrumentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CInstrumentCtrl::XMyOleInPlaceActiveObject::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_MANAGE_STATE( CInstrumentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return (HRESULT) pThis->ExternalQueryInterface( &iid, ppvObj );
}

STDMETHODIMP CInstrumentCtrl::XMyOleInPlaceActiveObject::GetWindow( HWND* lphwnd )
{
	METHOD_MANAGE_STATE( CInstrumentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.GetWindow( lphwnd );
}

STDMETHODIMP CInstrumentCtrl::XMyOleInPlaceActiveObject::ContextSensitiveHelp( BOOL fEnterMode )
{
	METHOD_MANAGE_STATE( CInstrumentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.ContextSensitiveHelp( fEnterMode );
}

STDMETHODIMP CInstrumentCtrl::XMyOleInPlaceActiveObject::TranslateAccelerator( LPMSG lpmsg )
{
	METHOD_MANAGE_STATE( CInstrumentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	// Translate only if Instrument EDitor has the focus
	if(pThis->m_pInstrumentEditor)
	{
		CWnd* pWnd = pThis->GetFocus();
		if(pWnd == pThis->m_pInstrumentEditor || pThis->m_pInstrumentEditor->IsChild(pWnd))
		{
			if( ::TranslateAccelerator(pThis->m_hWnd, pThis->m_hAcceleratorTable, lpmsg) )
			{
				return S_OK;
			}
			else if (pThis->PreTranslateMessage(lpmsg))
			{
				return S_OK;
			}
		}
	}
	return S_FALSE;
}

STDMETHODIMP CInstrumentCtrl::XMyOleInPlaceActiveObject::OnFrameWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CInstrumentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );
	TRACE("Frame Window Activate\n");
	if ( fActivate )
	{
		pThis->SetTransportName();
		pThis->m_pInstrument->m_pComponent->m_pIConductor->SetActiveTransport(pThis->m_pInstrument, BS_PLAY_ENABLED | BS_NO_AUTO_UPDATE);

		if(pThis->m_pInstrumentEditor->IsMidiRegistered() == false)
			pThis->m_pInstrumentEditor->RegisterMidi();
	}
	else
	{
		pThis->m_pInstrument->m_pComponent->m_pIConductor->TransportStopped(pThis->m_pInstrument);
		pThis->m_pInstrumentEditor->UnRegisterMidi();
		pThis->m_pInstrumentEditor->TurnOffMidiNotes();

		pThis->m_pInstrument->Stop(true);
	}
	return pThis->m_xOleInPlaceActiveObject.OnFrameWindowActivate( fActivate );
}

STDMETHODIMP CInstrumentCtrl::XMyOleInPlaceActiveObject::OnDocWindowActivate( BOOL fActivate )
{
    METHOD_MANAGE_STATE( CInstrumentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );
	TRACE("Doc Window Activate\n");
	if ( fActivate )
	{
		pThis->SetTransportName();
		pThis->m_pInstrument->m_pComponent->m_pIConductor->SetActiveTransport(pThis->m_pInstrument, BS_PLAY_ENABLED | BS_NO_AUTO_UPDATE);
		pThis->m_pInstrumentEditor->RegisterMidi();
		pThis->m_pInstrument->UpdatePatch();
		pThis->m_pInstrument->OnViewProperties();
		pThis->SetFocus();
	}
	else
	{
		pThis->m_pInstrument->m_pComponent->m_pIConductor->TransportStopped(pThis->m_pInstrument);
		pThis->m_pInstrumentEditor->UnRegisterMidi();
		pThis->m_pInstrumentEditor->TurnOffMidiNotes();
		pThis->m_pInstrument->Stop(true);
	}
	return pThis->m_xOleInPlaceActiveObject.OnDocWindowActivate( fActivate );
}

STDMETHODIMP CInstrumentCtrl::XMyOleInPlaceActiveObject::ResizeBorder(
	LPCRECT, LPOLEINPLACEUIWINDOW, BOOL fFrameWindow )
{
	METHOD_MANAGE_STATE( CInstrumentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

    if( fFrameWindow == TRUE )
	{
		pThis->OnShowToolBars();
	}
	return S_OK;
}

STDMETHODIMP CInstrumentCtrl::XMyOleInPlaceActiveObject::EnableModeless( BOOL fEnable )
{
	METHOD_MANAGE_STATE( CInstrumentCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.EnableModeless( fEnable );
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::OnDraw - Drawing function

void CInstrumentCtrl::OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::DoPropExchange - Persistence support

void CInstrumentCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::OnResetState - Reset control to default state

void CInstrumentCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::AboutBox - Display an "About" box to the user

void CInstrumentCtrl::AboutBox()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl message handlers

int CInstrumentCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Load control's in-place menu
	if(m_hMenuInPlace == NULL)
		m_hMenuInPlace = ::LoadMenu(theApp.m_hInstance, MAKEINTRESOURCE(IDM_DLS_DESIGNER));

	// Create Collection's view
	m_pInstrumentEditor = new CInstrumentFVEditor(this);

	if( m_pInstrumentEditor == NULL )
	{
		return -1;
	}

	if(m_pInstrumentEditor->Create(NULL, 
								   "WindowName", 
								   WS_CHILD | WS_VISIBLE, 
								   CRect( 0, 0, 800, 400 ), 
								   this, 
								   888, 
								   NULL) == 0)
	{
		return -1;
	}

	// Set the transport name
	SetTransportName();

	m_pInstrumentEditor->OnInitialUpdate(); 
	m_hAcceleratorTable = LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_DLS_ACCELERATOR));

	// Collect the static controls in the editor
	m_pInstrumentEditor->CollectStatics();

	return 0;
}

void CInstrumentCtrl::OnSize(UINT nType, int cx, int cy) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	COleControl::OnSize(nType, cx, cy);
	
	m_pInstrumentEditor->MoveWindow( 0, 0, cx, cy );
}

void CInstrumentCtrl::OnChildActivate() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	COleControl::OnChildActivate();	
}

void CInstrumentCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Give the control on the editor a chance to 
	// consume this if it wants to (like the drums checkbox) 
	if(m_pInstrumentEditor)
	{
		if(m_pInstrumentEditor->SendKeyToChildControl(nChar, nRepCnt, nFlags) == TRUE)
			return;
	}
	switch (nChar)
    {
		case 32 :  // Space bar
		{	
			if(m_nMIDINoteOns[m_nLastSpacebarNote] > 0)
				break;

			// Figure out which note we should play
			m_nLastSpacebarNote = GetSpacebarNote();

			m_pInstrument->UpdatePatch();
			m_pInstrument->m_pComponent->PlayMIDIEvent(MIDI_NOTEON,
													  BYTE(m_nLastSpacebarNote),
													  DEFAULT_VELOCITY,
													  5, // Give enough time for the UpdatePatch to apply
													  m_pInstrument->IsDrum());

			m_nMIDINoteOns[m_nLastSpacebarNote]++;
			break;
		}
			
		default:
		{
			COleControl::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}
	
	
}

void CInstrumentCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	switch (nChar)
    {
		case 32 :  // Space bar
			while (m_nMIDINoteOns[m_nLastSpacebarNote] > 0)
			{
				m_pInstrument->m_pComponent->PlayMIDIEvent(MIDI_NOTEOFF,
														  BYTE(m_nLastSpacebarNote),
														  DEFAULT_VELOCITY,
														  0,
														  m_pInstrument->IsDrum());
				m_nMIDINoteOns[m_nLastSpacebarNote]--;
			}
			break;
		default:
			COleControl::OnKeyUp(nChar, nRepCnt, nFlags);
	}	
}

void CInstrumentCtrl::OnDestroy() 
{
	if(m_pInstrument)
	{
		ASSERT(m_pInstrument->m_pInstrumentCtrl == this);

		m_pInstrument->m_pInstrumentCtrl = NULL;		
		m_pInstrument->Release();
	}

	COleControl::OnDestroy();

	if(m_hMenuInPlace && m_nInstrumentCtrlRefCount <= 1)
	{
		::DestroyMenu(m_hMenuInPlace);
		m_hMenuInPlace = NULL;
	}
}

HMENU CInstrumentCtrl::OnGetInPlaceMenu() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return m_hMenuInPlace;
}

void CInstrumentCtrl::OnEditRedo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Remember if the instrument property page was up
	bool bInstrumentPageIsUp = m_pInstrument->IsPropertyPageShowing();

	// Turn off all the notes to prevent hanging notes
	m_pInstrumentEditor->TurnOffMidiNotes();


	CUndoMan* pUndoMgr = m_pInstrument->m_pUndoMgr;
	
	ASSERT(pUndoMgr);
	pUndoMgr->Redo(m_pInstrument);
	m_pInstrumentEditor->SetupInstrument();
	m_pInstrumentEditor->RefreshRegion();
	m_pInstrument->UpdateInstrument();
	m_pInstrument->RefreshUI(false);

	if(bInstrumentPageIsUp)
		m_pInstrument->OnShowProperties();
}

void CInstrumentCtrl::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	char actionStr[BUFFER_64];
	CString		csMenuText,csShortCut;

	AFX_MANAGE_STATE(_afxModuleAddrThis);

	csMenuText.LoadString(IDS_REDO_MENU_TEXT);
	csShortCut.LoadString(IDS_REDO_SHORTCUT_TEXT);

	if((m_pInstrument->m_pUndoMgr)->GetRedo(actionStr, sizeof(actionStr)))
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText(csMenuText + _T(" ") + actionStr + csShortCut);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetText(csMenuText + csShortCut);
	}
}

void CInstrumentCtrl::OnEditUndo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Remember if the instrument property page was up
	bool bInstrumentPageIsUp = m_pInstrument->IsPropertyPageShowing();

	// Turn off all the notes to prevent hanging notes
	m_pInstrumentEditor->TurnOffMidiNotes();

	CUndoMan* pUndoMgr = m_pInstrument->m_pUndoMgr;
	
	ASSERT(pUndoMgr);
	IPersistStream *pIPStream;

	m_pInstrument->QueryInterface( IID_IPersistStream, (void **) &pIPStream );
	if( pIPStream )
	{
		pUndoMgr->Undo(m_pInstrument);
		pIPStream->Release();
	}

	m_pInstrumentEditor->SetupInstrument();
	m_pInstrumentEditor->RefreshRegion();
	m_pInstrument->UpdateInstrument();
	
	CWnd* pWnd = m_pInstrumentEditor->GetFocus();
	if(m_pInstrument)
		m_pInstrument->RefreshUI(false);

	if(bInstrumentPageIsUp)
		m_pInstrument->OnShowProperties();

	if(pWnd)
		pWnd->SetFocus();
	
}

void CInstrumentCtrl::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	char actionStr[BUFFER_64];
	CString		csMenuText,csShortCut;

	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	csMenuText.LoadString(IDS_UNDO_MENU_TEXT);
	csShortCut.LoadString(IDS_UNDO_SHORTCUT_TEXT);

	if((m_pInstrument->m_pUndoMgr)->GetUndo(actionStr, sizeof(actionStr)))
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText(csMenuText + _T(" ") + actionStr + csShortCut);
	}
	else 
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetText(csMenuText + csShortCut);
	}
}

void CInstrumentCtrl::OnAppAbout() 
{
	AboutBox();
}

void CInstrumentCtrl::OnShowToolBars() 
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
	COleControl::OnShowToolBars();
}

void CInstrumentCtrl::TurnOffMidiNotes()
{
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
		return;

	HRESULT hr;
	BYTE i;
	for (i = 0; i < 128; i++)
	{
		if ( m_nMIDINoteOns[i] )
		{
  			while(m_nMIDINoteOns[i] >= 0)
			{
				hr = m_pInstrument->m_pComponent->PlayMIDIEvent(MIDI_NOTEOFF,
																i,
																DEFAULT_VELOCITY,
             													0,
																m_pInstrument->IsDrum());
				m_nMIDINoteOns[i]--;
				if ( FAILED(hr) )
					break;
			}
		}
	}
}

void CInstrumentCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
		COleControl::OnChar(nChar, nRepCnt, nFlags);
}

int CInstrumentCtrl::GetSpacebarNote()
{
	// return the root note if its in a region or return the closest note to
	// it that is in a region.  This is the note that should be played
	// when the spacebar is pressed.
	ASSERT(m_pInstrumentEditor != NULL);
	ASSERT(m_pInstrument != NULL);
	
	int nNote = m_pInstrument->m_nMIDINote;

	// get begin and end notes in current region
	int nFirstNote = m_pInstrumentEditor->m_pCurRegion->m_rRgnHeader.RangeKey.usLow;
	int nLastNote = m_pInstrumentEditor->m_pCurRegion->m_rRgnHeader.RangeKey.usHigh;

	if (nNote < nFirstNote || nNote > nLastNote) {
		
		// the note is not in the current region, so find the closest note
		// to the root note that IS in the current region
		if (abs(nNote - nFirstNote) > abs(nNote - nLastNote)) {

			nNote = nLastNote;
		}
		else {

			nNote = nFirstNote;
		}
	}

	return nNote;
}

void CInstrumentCtrl::OnHelpFinder() 
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

void CInstrumentCtrl::SetTransportName()
{
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
		return;
	ASSERT(m_pInstrument->m_pComponent);
	if(m_pInstrument->m_pComponent == NULL)
		return;
	if(m_pInstrument->m_pComponent->m_pIConductor == NULL)
		return;

	// The actual transport name is collection:wavename
	m_pInstrument->GetName();
	CString sWaveName = m_pInstrument->m_csName;
	if(sWaveName.IsEmpty())
		sWaveName.LoadString(IDS_INSTRUMENT_TEXT);

	CString sCollectionName = m_pInstrument->m_pCollection->m_Info.m_csName;
	CString sTransportName = sCollectionName + ":" + sWaveName;

	// Set the name for the transport
	BSTR bstrWaveName = sTransportName.AllocSysString();
	m_pInstrument->m_pComponent->m_pIConductor->SetTransportName(m_pInstrument, bstrWaveName);
}