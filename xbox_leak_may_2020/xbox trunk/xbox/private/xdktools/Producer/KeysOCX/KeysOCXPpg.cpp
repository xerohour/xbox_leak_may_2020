// KeysOCXPpg.cpp : Implementation of the CKeysOCXPropPage property page class.

#include "stdafx.h"
#include "KeysOCX.h"
#include "KeysOCXPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CKeysOCXPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CKeysOCXPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CKeysOCXPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CKeysOCXPropPage, "KEYSOCX.KeysOCXPropPage.1",
	0xcdd09f87, 0xe73c, 0x11d0, 0x89, 0xab, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29)


/////////////////////////////////////////////////////////////////////////////
// CKeysOCXPropPage::CKeysOCXPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CKeysOCXPropPage

BOOL CKeysOCXPropPage::CKeysOCXPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_KEYSOCX_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CKeysOCXPropPage::CKeysOCXPropPage - Constructor

CKeysOCXPropPage::CKeysOCXPropPage() :
	COlePropertyPage(IDD, IDS_KEYSOCX_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CKeysOCXPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CKeysOCXPropPage::DoDataExchange - Moves data between page and properties

void CKeysOCXPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CKeysOCXPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CKeysOCXPropPage message handlers
