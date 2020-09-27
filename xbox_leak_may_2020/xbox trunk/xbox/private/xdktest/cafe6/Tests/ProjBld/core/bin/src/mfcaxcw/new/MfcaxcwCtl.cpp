// MfcaxcwCtl.cpp : Implementation of the CMfcaxcwCtrl ActiveX Control class.

#include "stdafx.h"
#include "mfcaxcw.h"
#include "MfcaxcwCtl.h"
#include "MfcaxcwPpg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CMfcaxcwCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CMfcaxcwCtrl, COleControl)
	//{{AFX_MSG_MAP(CMfcaxcwCtrl)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CMfcaxcwCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CMfcaxcwCtrl)
	// NOTE - ClassWizard will add and remove dispatch map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CMfcaxcwCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CMfcaxcwCtrl, COleControl)
	//{{AFX_EVENT_MAP(CMfcaxcwCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CMfcaxcwCtrl, 1)
	PROPPAGEID(CMfcaxcwPropPage::guid)
END_PROPPAGEIDS(CMfcaxcwCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CMfcaxcwCtrl, "MFCAXCW.MfcaxcwCtrl.1",
	0x7efbebfb, 0xcaa8, 0x11d0, 0x84, 0xbf, 0, 0xaa, 0, 0xc0, 0x8, 0x48)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CMfcaxcwCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DMfcaxcw =
		{ 0x7efbebf9, 0xcaa8, 0x11d0, { 0x84, 0xbf, 0, 0xaa, 0, 0xc0, 0x8, 0x48 } };
const IID BASED_CODE IID_DMfcaxcwEvents =
		{ 0x7efbebfa, 0xcaa8, 0x11d0, { 0x84, 0xbf, 0, 0xaa, 0, 0xc0, 0x8, 0x48 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwMfcaxcwOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CMfcaxcwCtrl, IDS_MFCAXCW, _dwMfcaxcwOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CMfcaxcwCtrl::CMfcaxcwCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CMfcaxcwCtrl

BOOL CMfcaxcwCtrl::CMfcaxcwCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.
	int i = 6;
	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_MFCAXCW,
			IDB_MFCAXCW,
			afxRegApartmentThreading,
			_dwMfcaxcwOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CMfcaxcwCtrl::CMfcaxcwCtrl - Constructor

CMfcaxcwCtrl::CMfcaxcwCtrl()
{
	InitializeIIDs(&IID_DMfcaxcw, &IID_DMfcaxcwEvents);

	// TODO: Initialize your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CMfcaxcwCtrl::~CMfcaxcwCtrl - Destructor

CMfcaxcwCtrl::~CMfcaxcwCtrl()
{
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CMfcaxcwCtrl::OnDraw - Drawing function

void CMfcaxcwCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	// TODO: Replace the following code with your own drawing code.
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	pdc->Ellipse(rcBounds);
}


/////////////////////////////////////////////////////////////////////////////
// CMfcaxcwCtrl::DoPropExchange - Persistence support

void CMfcaxcwCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CMfcaxcwCtrl::OnResetState - Reset control to default state

void CMfcaxcwCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CMfcaxcwCtrl::AboutBox - Display an "About" box to the user

void CMfcaxcwCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_MFCAXCW);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CMfcaxcwCtrl message handlers
