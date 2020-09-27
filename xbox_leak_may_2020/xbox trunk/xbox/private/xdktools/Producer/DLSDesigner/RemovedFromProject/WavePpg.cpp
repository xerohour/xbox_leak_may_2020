// WavePpg.cpp : Implementation of the CWavePropPage property page class.

#include "stdafx.h"
#include "DLSDesignerDLL.h"
#include "WavePpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CWavePropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CWavePropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CWavePropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CWavePropPage, "DLSDESIGNER.WavePropPage.1",
	0xbc964e8f, 0x96f7, 0x11d0, 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29)


/////////////////////////////////////////////////////////////////////////////
// CWavePropPage::CWavePropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CWavePropPage

BOOL CWavePropPage::CWavePropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_WAVE_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CWavePropPage::CWavePropPage - Constructor

CWavePropPage::CWavePropPage() :
	COlePropertyPage(IDD, IDS_WAVE_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CWavePropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CWavePropPage::DoDataExchange - Moves data between page and properties

void CWavePropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CWavePropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CWavePropPage message handlers
