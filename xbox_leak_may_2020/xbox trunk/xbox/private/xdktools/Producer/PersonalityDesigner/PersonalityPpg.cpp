// PersonalityPpg.cpp : Implementation of the CPersonalityPropPage property page class.

#include "stdafx.h"
#pragma warning(disable:4201)
#include "PersonalityDesigner.h"
#include "PersonalityPpg.h"
#pragma warning(default:4201)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CPersonalityPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CPersonalityPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CPersonalityPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Personality Editor Property Page  {03DE8B81-A76B-11d0-9EDC-00AA00A21BA9}
const CLSID CLSID_PersonalityPropPage = 
{ 0x8eaf7e00, 0xb6a1, 0x11d0, { 0x9e, 0xdc, 0x0, 0xaa, 0x0, 0xa2, 0x1b, 0xa9 } };

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CPersonalityPropPage, "CHORDMAPDESR.ChordMapPropPage.1",
	0xd433f95f, 0xb588, 0x11d0, 0x9e, 0xdc, 0, 0xaa, 0, 0xa2, 0x1b, 0xa9)


/////////////////////////////////////////////////////////////////////////////
// CPersonalityPropPage::CPersonalityPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CPersonalityPropPage

BOOL CPersonalityPropPage::CPersonalityPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_PERSONALITY_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityPropPage::CPersonalityPropPage - Constructor

CPersonalityPropPage::CPersonalityPropPage() :
	COlePropertyPage(IDD, IDS_PERSONALITY_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CPersonalityPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityPropPage::DoDataExchange - Moves data between page and properties

void CPersonalityPropPage::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	//{{AFX_DATA_MAP(CPersonalityPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityPropPage message handlers
