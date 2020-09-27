// GridPpg.cpp : Implementation of the CGridPropPage property page class.

#include "stdafx.h"
#include "Grid.h"
#include "GridPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CGridPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CGridPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CGridPropPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CGridPropPage, "GRID.GridPropPage.1",
	0xeab0cd47, 0x9459, 0x11d0, 0x8c, 0x10, 0, 0xa0, 0xc9, 0x2e, 0x1c, 0xac)


/////////////////////////////////////////////////////////////////////////////
// CGridPropPage::CGridPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CGridPropPage

BOOL CGridPropPage::CGridPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_GRID_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CGridPropPage::CGridPropPage - Constructor

CGridPropPage::CGridPropPage() :
	COlePropertyPage(IDD, IDS_GRID_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CGridPropPage)
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CGridPropPage::DoDataExchange - Moves data between page and properties

void CGridPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CGridPropPage)
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CGridPropPage message handlers
