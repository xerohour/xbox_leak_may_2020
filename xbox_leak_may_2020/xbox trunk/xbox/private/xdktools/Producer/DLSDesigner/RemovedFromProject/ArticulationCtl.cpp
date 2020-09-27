// ArticulationCtl.cpp : Implementation of the CArticulationCtrl ActiveX Control class.

#include "stdafx.h"
#include "DLSDesignerDLL.h"
#include "Articulation.h"
#include "ArticulationCtl.h"
#include "ArticulationPpg.h"
#include "DLSLoadSaveUtils.h"
#include "Instrument.h"
#include "region.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CArticulationCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CArticulationCtrl, COleControl)
	//{{AFX_MSG_MAP(CArticulationCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CArticulationCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CArticulationCtrl)
	// NOTE - ClassWizard will add and remove dispatch map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CArticulationCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CArticulationCtrl, COleControl)
	//{{AFX_EVENT_MAP(CArticulationCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl Interface map

BEGIN_INTERFACE_MAP(CArticulationCtrl, COleControl)
    INTERFACE_PART(CArticulationCtrl, IID_IOleInPlaceActiveObject, MyOleInPlaceActiveObject)
    INTERFACE_PART(CArticulationCtrl, IID_IJazzEditor, Editor)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CArticulationCtrl, 1)
	PROPPAGEID(CArticulationPropPage::guid)
END_PROPPAGEIDS(CArticulationCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CArticulationCtrl, "DLSDESIGNER.ArticulationCtrl.1",
	0xbc964e92, 0x96f7, 0x11d0, 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CArticulationCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DArticulation =
		{ 0xbc964e90, 0x96f7, 0x11d0, { 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };
const IID BASED_CODE IID_DArticulationEvents =
		{ 0xbc964e91, 0x96f7, 0x11d0, { 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwArticulationOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_SIMPLEFRAME;

IMPLEMENT_OLECTLTYPE(CArticulationCtrl, IDS_ARTICULATION, _dwArticulationOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl::CArticulationCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CArticulationCtrl

BOOL CArticulationCtrl::CArticulationCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_ARTICULATION,
			IDB_ARTICULATION,
			afxRegApartmentThreading,
			_dwArticulationOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);

}


/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl::CArticulationCtrl - Constructor

CArticulationCtrl::CArticulationCtrl() : m_pArticulationEditor(NULL), m_pArticulation(NULL)
{
	InitializeIIDs(&IID_DArticulation, &IID_DArticulationEvents);
	
	TRACE0("Entering constructor of CArticulation");

	EnableSimpleFrame();

	// J3 we need to do this better maybe a global class that contains m_nMIDINoteOns

	for(int i = 0; i < 128; i++)
	{
		m_nMIDINoteOns[i] = 0;
	}

	// TODO: Initialize your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl::~CArticulationCtrl - Destructor

CArticulationCtrl::~CArticulationCtrl()
{
	if(m_pArticulation)
	{
		m_pArticulation->Release();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl IJazzEditor implementation

/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl::XEditor::AddRef

STDMETHODIMP_(ULONG) CArticulationCtrl::XEditor::AddRef()
{
	METHOD_PROLOGUE_EX_( CArticulationCtrl, Editor )

	return (ULONG)pThis->ExternalAddRef();
}


/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl::XEditor::Release

STDMETHODIMP_(ULONG) CArticulationCtrl::XEditor::Release()
{
	METHOD_PROLOGUE_EX_( CArticulationCtrl, Editor )

	return (ULONG)pThis->ExternalRelease();
}


/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl::XEditor::QueryInterface

STDMETHODIMP CArticulationCtrl::XEditor::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_PROLOGUE_EX_( CArticulationCtrl, Editor )

	return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}


/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl::XEditor::AttachObjects implementation

HRESULT CArticulationCtrl::XEditor::AttachObjects( IJazzNode* pNode, IOleUndoManager* pUndoManager )
{
	METHOD_MANAGE_STATE( CArticulationCtrl, Editor )

	ASSERT_VALID( pThis );
	ASSERT( pNode != NULL );

	pThis->m_pArticulation = (CArticulation *)pNode;
	pThis->m_pArticulation->AddRef();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl::XEditor::OnInitMenuFilePrint

HRESULT CArticulationCtrl::XEditor::OnInitMenuFilePrint( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CArticulationCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl::XEditor::OnFilePrint

HRESULT CArticulationCtrl::XEditor::OnFilePrint( void )
{
	METHOD_MANAGE_STATE( CArticulationCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl::XEditor::OnInitMenuFilePrintPreview

HRESULT CArticulationCtrl::XEditor::OnInitMenuFilePrintPreview( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CArticulationCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl::XEditor::OnFilePrintPreview

HRESULT CArticulationCtrl::XEditor::OnFilePrintPreview( void )
{
	METHOD_MANAGE_STATE( CArticulationCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl::XMyOleInPlaceActiveObject implementation

STDMETHODIMP_(ULONG) CArticulationCtrl::XMyOleInPlaceActiveObject::AddRef()
{
	METHOD_MANAGE_STATE( CArticulationCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.AddRef();
}

STDMETHODIMP_(ULONG) CArticulationCtrl::XMyOleInPlaceActiveObject::Release()
{
	METHOD_MANAGE_STATE( CArticulationCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.Release();
}

STDMETHODIMP CArticulationCtrl::XMyOleInPlaceActiveObject::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_MANAGE_STATE( CArticulationCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.QueryInterface( iid, ppvObj );
}

STDMETHODIMP CArticulationCtrl::XMyOleInPlaceActiveObject::GetWindow( HWND* lphwnd )
{
	METHOD_MANAGE_STATE( CArticulationCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.GetWindow( lphwnd );
}

STDMETHODIMP CArticulationCtrl::XMyOleInPlaceActiveObject::ContextSensitiveHelp( BOOL fEnterMode )
{
	METHOD_MANAGE_STATE( CArticulationCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.ContextSensitiveHelp( fEnterMode );
}

STDMETHODIMP CArticulationCtrl::XMyOleInPlaceActiveObject::TranslateAccelerator( LPMSG lpmsg )
{
	METHOD_MANAGE_STATE( CArticulationCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.TranslateAccelerator( lpmsg );
}

STDMETHODIMP CArticulationCtrl::XMyOleInPlaceActiveObject::OnFrameWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CArticulationCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnFrameWindowActivate( fActivate );
}

STDMETHODIMP CArticulationCtrl::XMyOleInPlaceActiveObject::OnDocWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CArticulationCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnDocWindowActivate( fActivate );
}

STDMETHODIMP CArticulationCtrl::XMyOleInPlaceActiveObject::ResizeBorder(
	LPCRECT, LPOLEINPLACEUIWINDOW, BOOL fFrameWindow )
{
	METHOD_MANAGE_STATE( CArticulationCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

    if( fFrameWindow == TRUE )
	{
		pThis->OnShowToolBars();
	}

	return S_OK;
}

STDMETHODIMP CArticulationCtrl::XMyOleInPlaceActiveObject::EnableModeless( BOOL fEnable )
{
	METHOD_MANAGE_STATE( CArticulationCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.EnableModeless( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl::OnDraw - Drawing function

void CArticulationCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
}


/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl::DoPropExchange - Persistence support

void CArticulationCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl::OnResetState - Reset control to default state

void CArticulationCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl::AboutBox - Display an "About" box to the user

void CArticulationCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_ARTICULATION);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl message handlers

int CArticulationCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
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

		// Create Collection's view
	m_pArticulationEditor = new CArticulationEditor(this);

	if( m_pArticulationEditor == NULL )
	{
		return -1;
	}

	if(m_pArticulationEditor->Create(NULL, 
									 "WindowName", 
									 WS_CHILD | WS_VISIBLE, 
									 CRect( 0, 0, 800, 400 ), 
									 this, 
									 999, 
									 NULL) == 0)
	{
		return -1;
	}

	m_pArticulationEditor->OnInitialUpdate();

	return 0;
}

void CArticulationCtrl::OnSize(UINT nType, int cx, int cy) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	COleControl::OnSize(nType, cx, cy);
	
	m_pArticulationEditor->MoveWindow( 0, 0, cx, cy );
}

void CArticulationCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	TRACE0("Inside CArticulationCtrl::OnKeyDown");
	m_pArticulation->UpdatePatch();	 	


// J3 we want to use Validate Parent here and any where else.
	if(m_pArticulation->m_pInstrument)
	{
		ASSERT(m_pArticulation->m_pRegion == NULL);
		
		m_pArticulation->m_nMIDINote = (m_pArticulation->m_pInstrument)->m_nMIDINote;

	}
	else if(m_pArticulation->m_pRegion)
	{
		ASSERT(m_pArticulation->m_pInstrument == NULL);
		m_pArticulation->m_pRegion->m_nMIDINote = ((m_pArticulation->m_pRegion)->m_rRgnHeader.RangeKey.usLow +
			    		   (m_pArticulation->m_pRegion)->m_rRgnHeader.RangeKey.usHigh + 1) >> 1;

		m_pArticulation->m_nMIDINote = m_pArticulation->m_pRegion->m_nMIDINote;
		
	}
	else
	{
		// Problem; we should never get here if we do 
		// Articulation does not have a parent
		ASSERT(FALSE);
	}

	switch (nChar)
    {
		case 32 :  // Space bar
			if(m_nMIDINoteOns[m_pArticulation->m_nMIDINote] > 0)
			{
				break;
			}

			// J3 Do we need to do this
			((m_pArticulation->m_pComponent)->m_pIConductor)->PlayMIDIEvent(MIDI_NOTEON,
													   m_pArticulation->m_nMIDINote,
													   110,
													   0);
		m_nMIDINoteOns[m_pArticulation->m_nMIDINote]++;
        break;
	}
}

void CArticulationCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
    {
		case 32 :  // Space bar
// J3 Do we need to do this
		while (m_nMIDINoteOns[m_pArticulation->m_nMIDINote] > 0)
        {
			((m_pArticulation->m_pComponent)->m_pIConductor)->PlayMIDIEvent(MIDI_NOTEOFF,
													   m_pArticulation->m_nMIDINote,
													   110,
													   0);

			m_nMIDINoteOns[m_pArticulation->m_nMIDINote]--;
		}
        break;
	}
//	COleControl::OnKeyUp(nChar, nRepCnt, nFlags);
}
