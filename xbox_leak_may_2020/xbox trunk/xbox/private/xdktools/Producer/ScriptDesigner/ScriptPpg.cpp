// ScriptPpg.cpp : Implementation of the CScriptPropPage property page class.

#include "stdafx.h"
#include "ScriptDesignerDLL.h"
#include "ScriptPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CScriptPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CScriptPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CScriptPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CScriptPropPage, "SCRIPTDESIGNER.ScriptPropPage.1",
	0xd135db59, 0x66ed, 0x11d3, 0xb4, 0x5d, 0, 0x10, 0x5a, 0x27, 0x96, 0xde)


/////////////////////////////////////////////////////////////////////////////
// CScriptPropPage::CScriptPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CScriptPropPage

BOOL CScriptPropPage::CScriptPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_SCRIPT_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CScriptPropPage::CScriptPropPage - Constructor

CScriptPropPage::CScriptPropPage() :
	COlePropertyPage(IDD, IDS_SCRIPT_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CScriptPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CScriptPropPage::DoDataExchange - Moves data between page and properties

void CScriptPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CScriptPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CScriptPropPage message handlers
