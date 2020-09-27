// ArticulationPpg.cpp : Implementation of the CArticulationPropPage property page class.

#include "stdafx.h"
#include "DLSDesignerDLL.h"
#include "ArticulationPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CArticulationPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CArticulationPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CArticulationPropPage)
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CArticulationPropPage, "DLSDESIGNER.ArticulationPropPage.1",
	0xbc964e93, 0x96f7, 0x11d0, 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29)


/////////////////////////////////////////////////////////////////////////////
// CArticulationPropPage::CArticulationPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CArticulationPropPage

BOOL CArticulationPropPage::CArticulationPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_ARTICULATION_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CArticulationPropPage::CArticulationPropPage - Constructor

CArticulationPropPage::CArticulationPropPage() :
	COlePropertyPage(IDD, IDS_ARTICULATION_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CArticulationPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CArticulationPropPage::DoDataExchange - Moves data between page and properties

void CArticulationPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CArticulationPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CArticulationPropPage message handlers

void CArticulationPropPage::OnCancelMode() 
{
	COlePropertyPage::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}


