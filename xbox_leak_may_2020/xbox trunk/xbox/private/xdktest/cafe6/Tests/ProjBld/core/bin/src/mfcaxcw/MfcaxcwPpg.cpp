// MfcaxcwPpg.cpp : Implementation of the CMfcaxcwPropPage property page class.

#include "stdafx.h"
#include "mfcaxcw.h"
#include "MfcaxcwPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CMfcaxcwPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CMfcaxcwPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CMfcaxcwPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CMfcaxcwPropPage, "MFCAXCW.MfcaxcwPropPage.1",
	0x7efbebfc, 0xcaa8, 0x11d0, 0x84, 0xbf, 0, 0xaa, 0, 0xc0, 0x8, 0x48)


/////////////////////////////////////////////////////////////////////////////
// CMfcaxcwPropPage::CMfcaxcwPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CMfcaxcwPropPage

BOOL CMfcaxcwPropPage::CMfcaxcwPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_MFCAXCW_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CMfcaxcwPropPage::CMfcaxcwPropPage - Constructor

CMfcaxcwPropPage::CMfcaxcwPropPage() :
	COlePropertyPage(IDD, IDS_MFCAXCW_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CMfcaxcwPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CMfcaxcwPropPage::DoDataExchange - Moves data between page and properties

void CMfcaxcwPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CMfcaxcwPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CMfcaxcwPropPage message handlers
