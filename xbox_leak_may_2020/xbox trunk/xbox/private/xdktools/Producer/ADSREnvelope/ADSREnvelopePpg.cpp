// ADSREnvelopePpg.cpp : Implementation of the CADSREnvelopePropPage property page class.

#include "stdafx.h"
#include "ADSREnvelope.h"
#include "ADSREnvelopePpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CADSREnvelopePropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CADSREnvelopePropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CADSREnvelopePropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CADSREnvelopePropPage, "ADSRENVELOPE.ADSREnvelopePropPage.1",
	0x71ae3627, 0xa9bd, 0x11d0, 0xbc, 0xba, 0, 0xaa, 0, 0xc0, 0x81, 0x46)


/////////////////////////////////////////////////////////////////////////////
// CADSREnvelopePropPage::CADSREnvelopePropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CADSREnvelopePropPage

BOOL CADSREnvelopePropPage::CADSREnvelopePropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_ADSRENVELOPE_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CADSREnvelopePropPage::CADSREnvelopePropPage - Constructor

CADSREnvelopePropPage::CADSREnvelopePropPage() :
	COlePropertyPage(IDD, IDS_ADSRENVELOPE_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CADSREnvelopePropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CADSREnvelopePropPage::DoDataExchange - Moves data between page and properties

void CADSREnvelopePropPage::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	//{{AFX_DATA_MAP(CADSREnvelopePropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CADSREnvelopePropPage message handlers
