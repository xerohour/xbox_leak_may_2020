// ContainerPpg.cpp : Implementation of the CContainerPropPage property page class.

#include "stdafx.h"
#include "ContainerDesignerDLL.h"
#include "ContainerPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CContainerPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CContainerPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CContainerPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CContainerPropPage, "CONTAINERDESIGNER.ContainerPropPage.1",
	0x778a0b90, 0x6f81, 0x11d3, 0xb4, 0x5f, 0, 0x10, 0x5a, 0x27, 0x96, 0xde)


/////////////////////////////////////////////////////////////////////////////
// CContainerPropPage::CContainerPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CContainerPropPage

BOOL CContainerPropPage::CContainerPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_CONTAINER_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CContainerPropPage::CContainerPropPage - Constructor

CContainerPropPage::CContainerPropPage() :
	COlePropertyPage(IDD, IDS_CONTAINER_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CContainerPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CContainerPropPage::DoDataExchange - Moves data between page and properties

void CContainerPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CContainerPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CContainerPropPage message handlers
