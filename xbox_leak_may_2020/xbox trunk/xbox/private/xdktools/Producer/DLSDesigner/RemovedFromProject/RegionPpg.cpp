// RegionPpg.cpp : Implementation of the CRegionPropPage property page class.

#include "stdafx.h"
#include "DLSDesignerDLL.h"
#include "RegionPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CRegionPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CRegionPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CRegionPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CRegionPropPage, "DLSDESIGNER.RegionPropPage.1",
	0xbc964e97, 0x96f7, 0x11d0, 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29)


/////////////////////////////////////////////////////////////////////////////
// CRegionPropPage::CRegionPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CRegionPropPage

BOOL CRegionPropPage::CRegionPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_REGION_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CRegionPropPage::CRegionPropPage - Constructor

CRegionPropPage::CRegionPropPage() :
	COlePropertyPage(IDD, IDS_REGION_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CRegionPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CRegionPropPage::DoDataExchange - Moves data between page and properties

void CRegionPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CRegionPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CRegionPropPage message handlers
