// BandPpg.cpp : Implementation of the CBandPropPage property page class.

#include "stdafx.h"
#include "BandEditorDLL.h"
#include "BandPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CBandPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CBandPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CBandPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



// Band Editor Property Page  {3BD2BA12-46E7-11D0-89AC-00A0C9054129}
const CLSID CLSID_BandPropPage = 
{ 0x3bd2ba12, 0x46e7, 0x11d0, { 0x89, 0xac, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CBandPropPage, "STYLEDESIGNER.BandPropPage.1",
	0x3bd2ba12, 0x46e7, 0x11d0, 0x89, 0xac, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29)


/////////////////////////////////////////////////////////////////////////////
// CBandPropPage::CBandPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CBandPropPage

BOOL CBandPropPage::CBandPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_BAND_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CBandPropPage::CBandPropPage - Constructor

CBandPropPage::CBandPropPage() :
	COlePropertyPage(IDD, IDS_BAND_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CBandPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CBandPropPage::DoDataExchange - Moves data between page and properties

void CBandPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CBandPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CBandPropPage message handlers
