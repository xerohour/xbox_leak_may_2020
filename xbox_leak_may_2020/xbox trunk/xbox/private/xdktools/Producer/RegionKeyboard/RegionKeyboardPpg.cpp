// RegionKeyboardPpg.cpp : Implementation of the CRegionKeyboardPropPage property page class.

#include "stdafx.h"
#include "RegionKeyboard.h"
#include "RegionKeyboardPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CRegionKeyboardPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CRegionKeyboardPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CRegionKeyboardPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CRegionKeyboardPropPage, "REGIONKEYBOARD.RegionKeyboardPropPage.1",
	0x36cd3188, 0xee61, 0x11d0, 0x87, 0x6a, 0, 0xaa, 0, 0xc0, 0x81, 0x46)


/////////////////////////////////////////////////////////////////////////////
// CRegionKeyboardPropPage::CRegionKeyboardPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CRegionKeyboardPropPage

BOOL CRegionKeyboardPropPage::CRegionKeyboardPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_REGIONKEYBOARD_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CRegionKeyboardPropPage::CRegionKeyboardPropPage - Constructor

CRegionKeyboardPropPage::CRegionKeyboardPropPage() :
	COlePropertyPage(IDD, IDS_REGIONKEYBOARD_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CRegionKeyboardPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CRegionKeyboardPropPage::DoDataExchange - Moves data between page and properties

void CRegionKeyboardPropPage::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	
	//{{AFX_DATA_MAP(CRegionKeyboardPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CRegionKeyboardPropPage message handlers
