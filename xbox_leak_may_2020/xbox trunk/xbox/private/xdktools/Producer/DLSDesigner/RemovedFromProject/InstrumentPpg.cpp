// InstrumentPpg.cpp : Implementation of the CInstrumentPropPage property page class.

#include "stdafx.h"
#include "DLSDesignerDLL.h"
#include "InstrumentPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CInstrumentPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CInstrumentPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CInstrumentPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CInstrumentPropPage, "DLSDESIGNER.InstrumentPropPage.1",
	0xbc964e8b, 0x96f7, 0x11d0, 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29)


/////////////////////////////////////////////////////////////////////////////
// CInstrumentPropPage::CInstrumentPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CInstrumentPropPage

BOOL CInstrumentPropPage::CInstrumentPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_INSTRUMENT_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentPropPage::CInstrumentPropPage - Constructor

CInstrumentPropPage::CInstrumentPropPage() :
	COlePropertyPage(IDD, IDS_INSTRUMENT_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CInstrumentPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentPropPage::DoDataExchange - Moves data between page and properties

void CInstrumentPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CInstrumentPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CInstrumentPropPage message handlers
