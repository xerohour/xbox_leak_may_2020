// CollectionPpg.cpp : Implementation of the CCollectionPropPage property page class.

#include "stdafx.h"
#include "DLSDesignerDLL.h"
#include "CollectionPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCollectionPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CCollectionPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CCollectionPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CCollectionPropPage, "DLSDESIGNER.CollectionPropPage.1",
	0xbc964e87, 0x96f7, 0x11d0, 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29)


/////////////////////////////////////////////////////////////////////////////
// CCollectionPropPage::CCollectionPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CCollectionPropPage

BOOL CCollectionPropPage::CCollectionPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_COLLECTION_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionPropPage::CCollectionPropPage - Constructor

CCollectionPropPage::CCollectionPropPage() :
	COlePropertyPage(IDD, IDS_COLLECTION_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CCollectionPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionPropPage::DoDataExchange - Moves data between page and properties

void CCollectionPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CCollectionPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionPropPage message handlers
