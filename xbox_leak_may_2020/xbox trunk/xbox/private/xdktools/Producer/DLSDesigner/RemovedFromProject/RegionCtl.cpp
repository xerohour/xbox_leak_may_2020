// RegionCtl.cpp : Implementation of the CRegionCtrl ActiveX Control class.

#include "stdafx.h"
#include "DLSDesignerDLL.h"
#include "Region.h"
#include "RegionCtl.h"
#include "RegionPpg.h"
#include "DLSLoadSaveUtils.h" // J3 may no longer need

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CRegionCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CRegionCtrl, COleControl)
	//{{AFX_MSG_MAP(CRegionCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CRegionCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CRegionCtrl)
	// NOTE - ClassWizard will add and remove dispatch map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CRegionCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CRegionCtrl, COleControl)
	//{{AFX_EVENT_MAP(CRegionCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl Interface map

BEGIN_INTERFACE_MAP(CRegionCtrl, COleControl)
    INTERFACE_PART(CRegionCtrl, IID_IOleInPlaceActiveObject, MyOleInPlaceActiveObject)
    INTERFACE_PART(CRegionCtrl, IID_IJazzEditor, Editor)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CRegionCtrl, 1)
	PROPPAGEID(CRegionPropPage::guid)
END_PROPPAGEIDS(CRegionCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CRegionCtrl, "DLSDESIGNER.RegionCtrl.1",
	0xbc964e96, 0x96f7, 0x11d0, 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CRegionCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DRegion =
		{ 0xbc964e94, 0x96f7, 0x11d0, { 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };
const IID BASED_CODE IID_DRegionEvents =
		{ 0xbc964e95, 0x96f7, 0x11d0, { 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwRegionOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CRegionCtrl, IDS_REGION, _dwRegionOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl::CRegionCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CRegionCtrl

BOOL CRegionCtrl::CRegionCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_REGION,
			IDB_REGION,
			afxRegApartmentThreading,
			_dwRegionOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl::CRegionCtrl - Constructor

CRegionCtrl::CRegionCtrl() : m_pRegionEditor(NULL), m_pRegion(NULL)
{
	InitializeIIDs(&IID_DRegion, &IID_DRegionEvents);

// J3 we need to do this better maybe a global class that contains m_nMIDINoteOns

	for(int i = 0; i < 128; i++)
	{
		m_nMIDINoteOns[i] = 0;
	}

	// TODO: Initialize your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl::~CRegionCtrl - Destructor

CRegionCtrl::~CRegionCtrl()
{
	if(m_pRegion)
	{
		m_pRegion->Release();
		m_pRegion = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl IJazzEditor implementation

/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl::XEditor::AddRef

STDMETHODIMP_(ULONG) CRegionCtrl::XEditor::AddRef()
{
	METHOD_PROLOGUE_EX_( CRegionCtrl, Editor )

	return (ULONG)pThis->ExternalAddRef();
}


/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl::XEditor::Release

STDMETHODIMP_(ULONG) CRegionCtrl::XEditor::Release()
{
	METHOD_PROLOGUE_EX_( CRegionCtrl, Editor )

	return (ULONG)pThis->ExternalRelease();
}


/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl::XEditor::QueryInterface

STDMETHODIMP CRegionCtrl::XEditor::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_PROLOGUE_EX_( CRegionCtrl, Editor )

	return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}


/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl::XEditor::AttachObjects implementation

HRESULT CRegionCtrl::XEditor::AttachObjects( IJazzNode* pNode, IOleUndoManager* pUndoManager )
{
	METHOD_MANAGE_STATE( CRegionCtrl, Editor )

	ASSERT_VALID( pThis );
	ASSERT( pNode != NULL );

	pThis->m_pRegion = (CRegion *)pNode;
	pThis->m_pRegion->AddRef();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl::XEditor::OnInitMenuFilePrint

HRESULT CRegionCtrl::XEditor::OnInitMenuFilePrint( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CRegionCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl::XEditor::OnFilePrint

HRESULT CRegionCtrl::XEditor::OnFilePrint( void )
{
	METHOD_MANAGE_STATE( CRegionCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl::XEditor::OnInitMenuFilePrintPreview

HRESULT CRegionCtrl::XEditor::OnInitMenuFilePrintPreview( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CRegionCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl::XEditor::OnFilePrintPreview

HRESULT CRegionCtrl::XEditor::OnFilePrintPreview( void )
{
	METHOD_MANAGE_STATE( CRegionCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl::XMyOleInPlaceActiveObject implementation

STDMETHODIMP_(ULONG) CRegionCtrl::XMyOleInPlaceActiveObject::AddRef()
{
	METHOD_MANAGE_STATE( CRegionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.AddRef();
}

STDMETHODIMP_(ULONG) CRegionCtrl::XMyOleInPlaceActiveObject::Release()
{
	METHOD_MANAGE_STATE( CRegionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.Release();
}

STDMETHODIMP CRegionCtrl::XMyOleInPlaceActiveObject::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_MANAGE_STATE( CRegionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.QueryInterface( iid, ppvObj );
}

STDMETHODIMP CRegionCtrl::XMyOleInPlaceActiveObject::GetWindow( HWND* lphwnd )
{
	METHOD_MANAGE_STATE( CRegionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.GetWindow( lphwnd );
}

STDMETHODIMP CRegionCtrl::XMyOleInPlaceActiveObject::ContextSensitiveHelp( BOOL fEnterMode )
{
	METHOD_MANAGE_STATE( CRegionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.ContextSensitiveHelp( fEnterMode );
}

STDMETHODIMP CRegionCtrl::XMyOleInPlaceActiveObject::TranslateAccelerator( LPMSG lpmsg )
{
	METHOD_MANAGE_STATE( CRegionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.TranslateAccelerator( lpmsg );
}

STDMETHODIMP CRegionCtrl::XMyOleInPlaceActiveObject::OnFrameWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CRegionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnFrameWindowActivate( fActivate );
}

STDMETHODIMP CRegionCtrl::XMyOleInPlaceActiveObject::OnDocWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CRegionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnDocWindowActivate( fActivate );
}

STDMETHODIMP CRegionCtrl::XMyOleInPlaceActiveObject::ResizeBorder(
	LPCRECT, LPOLEINPLACEUIWINDOW, BOOL fFrameWindow )
{
	METHOD_MANAGE_STATE( CRegionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

    if( fFrameWindow == TRUE )
	{
		pThis->OnShowToolBars();
	}

	return S_OK;
}

STDMETHODIMP CRegionCtrl::XMyOleInPlaceActiveObject::EnableModeless( BOOL fEnable )
{
	METHOD_MANAGE_STATE( CRegionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.EnableModeless( fEnable );
}

/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl::OnDraw - Drawing function

void CRegionCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
}


/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl::DoPropExchange - Persistence support

void CRegionCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl::OnResetState - Reset control to default state

void CRegionCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl::AboutBox - Display an "About" box to the user

void CRegionCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_REGION);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CRegionCtrl message handlers

int CRegionCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
		// Create Collection's view
	m_pRegionEditor = new CRegionEditor(this);

	if( m_pRegionEditor == NULL )
	{
		return -1;
	}

	if(m_pRegionEditor->Create(NULL, 
							   "WindowName", 
							   WS_CHILD | WS_VISIBLE, 
							   CRect( 0, 0, 800, 400 ), 
							   this, 
							   888, 
							   NULL) == 0)
	{
		return -1;
	}


	m_pRegionEditor->OnInitialUpdate(); 
	return 0;
}

void CRegionCtrl::OnSize(UINT nType, int cx, int cy) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	COleControl::OnSize(nType, cx, cy);
	
	m_pRegionEditor->MoveWindow( 0, 0, cx, cy );
}

void CRegionCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	m_pRegion->UpdatePatch();
	switch (nChar)
    {
		case 32 :  // Space bar
			
   			if(m_nMIDINoteOns[m_pRegion->m_nMIDINote] > 0)
			{
				break;
			}
			
			m_pRegion->m_nMIDINote = (m_pRegion->m_rRgnHeader.RangeKey.usLow +
					       			  m_pRegion->m_rRgnHeader.RangeKey.usHigh + 1) >> 1;

			((m_pRegion->m_pComponent)->m_pIConductor)->PlayMIDIEvent(MIDI_NOTEON,
			   													      m_pRegion->m_nMIDINote,
																	  110,
																	  0);
			// J3 Do we need to do this                
			m_nMIDINoteOns[m_pRegion->m_nMIDINote]++;
		break;
	}	

	// TODO: Add your message handler code here and/or call default
	
//	COleControl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CRegionCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
    {
		case 32 :  // Space bar
// J3 Do we need to do this
		while (m_nMIDINoteOns[m_pRegion->m_nMIDINote] > 0)
		{
			((m_pRegion->m_pComponent)->m_pIConductor)->PlayMIDIEvent(MIDI_NOTEOFF,
													   m_pRegion->m_nMIDINote,
													   110,
													   0); 

			m_nMIDINoteOns[m_pRegion->m_nMIDINote]--;
		}
        break;
	}	
	

	// TODO: Add your message handler code here and/or call default
	
//	COleControl::OnKeyUp(nChar, nRepCnt, nFlags);
}
