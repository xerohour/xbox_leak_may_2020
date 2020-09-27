// ContainerCtl.cpp : Implementation of the CContainerCtrl ActiveX Control class.

#include "stdafx.h"
#include "ContainerDesignerDLL.h"
#include "ContainerCtl.h"
#include "ContainerPpg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CContainerCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CContainerCtrl, COleControl)
	//{{AFX_MSG_MAP(CContainerCtrl)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CContainerCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CContainerCtrl)
	// NOTE - ClassWizard will add and remove dispatch map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CContainerCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CContainerCtrl, COleControl)
	//{{AFX_EVENT_MAP(CContainerCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CContainerCtrl, 1)
	PROPPAGEID(CContainerPropPage::guid)
END_PROPPAGEIDS(CContainerCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CContainerCtrl, "CONTAINERDESIGNER.ContainerCtrl.1",
	0x778a0b8f, 0x6f81, 0x11d3, 0xb4, 0x5f, 0, 0x10, 0x5a, 0x27, 0x96, 0xde)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CContainerCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DContainer =
		{ 0x778a0b8d, 0x6f81, 0x11d3, { 0xb4, 0x5f, 0, 0x10, 0x5a, 0x27, 0x96, 0xde } };
const IID BASED_CODE IID_DContainerEvents =
		{ 0x778a0b8e, 0x6f81, 0x11d3, { 0xb4, 0x5f, 0, 0x10, 0x5a, 0x27, 0x96, 0xde } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwContainerOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CContainerCtrl, IDS_CONTAINER, _dwContainerOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CContainerCtrl::CContainerCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CContainerCtrl

BOOL CContainerCtrl::CContainerCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_CONTAINER,
			IDB_CONTAINER,
			afxRegApartmentThreading,
			_dwContainerOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CContainerCtrl::CContainerCtrl - Constructor

CContainerCtrl::CContainerCtrl()
{
/////////////////////////////////////////////////////////////////////////
// Following commented out to prevent 3 ASSERTS that occur
// because we are not building a typelib
//	InitializeIIDs(&IID_DContainer, &IID_DContainerEvents);
//
// Following code copied from InitializeIIDs (since we cannot override)
	m_piidPrimary = &IID_DContainer;
	m_piidEvents = &IID_DContainerEvents;
	EnableTypeLib();	// Needed to prevent ASSERT when closing editor
	InitStockEventMask();
	InitStockPropMask();
//////////////////////////////////////////////////////////////////////////
}


/////////////////////////////////////////////////////////////////////////////
// CContainerCtrl::~CContainerCtrl - Destructor

CContainerCtrl::~CContainerCtrl()
{
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CContainerCtrl::OnDraw - Drawing function

void CContainerCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	// TODO: Replace the following code with your own drawing code.
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	pdc->Ellipse(rcBounds);
}


/////////////////////////////////////////////////////////////////////////////
// CContainerCtrl::DoPropExchange - Persistence support

void CContainerCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CContainerCtrl::OnResetState - Reset control to default state

void CContainerCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CContainerCtrl::AboutBox - Display an "About" box to the user

void CContainerCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_CONTAINER);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CContainerCtrl message handlers
