// wndptabs.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "hotlinkc.h"
#include "proptab.h"
#include "wndptabs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CWindowGeneralPropTab

CWindowGeneralPropTab::~CWindowGeneralPropTab()
{
}


INT CWindowGeneralPropTab::m_aControlIDs[] =
{
	IDC_WGT_TEXT,
	IDC_WGT_WINDOWHANDLE,
	IDC_WPT_WINDOWPROC,
	IDC_WGT_RECTANGLE,
	IDC_WGT_RESTOREDRECT,
	IDC_WGT_CLIENTRECT,
	IDC_WPT_INSTANCEHANDLE,
	IDC_WGT_MENUHANDLE,
	IDC_WGT_USERDATA,
	IDC_WGT_EXTRABYTES,
	0
};


BEGIN_MESSAGE_MAP(CWindowGeneralPropTab, CPropertyTab)
	//{{AFX_MSG_MAP(CWindowGeneralPropTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWindowGeneralPropTab message handlers

BOOL CWindowGeneralPropTab::OnInitDialog()
{
	CPropertyTab::OnInitDialog();

// begin special font mucking-around
	if (_getmbcp() != 0)
	{
		CFont* pFont = GetStdFont(font_Fixed);
		ASSERT(pFont);
		HFONT hfont = (HFONT)pFont->m_hObject;

		SendDlgItemMessage(IDC_WGT_TEXT, WM_SETFONT, (WPARAM)hfont, FALSE);
	}
// end special font mucking-around

	return TRUE;  // return TRUE  unless you set the focus to a control
}



void CWindowGeneralPropTab::UpdateFields()
{
	CPropertyInspector* pTabDlg = (CPropertyInspector*)GetParent();

	if (pTabDlg->m_flStyles & WS_CHILD)
		SetDlgItemText(IDC_WGT_MENUHANDLELABEL, ids(IDS_CONTROLID));
	else
		SetDlgItemText(IDC_WGT_MENUHANDLELABEL, ids(IDS_MENUHANDLE));

	SetDlgItemText(IDC_WGT_TEXT, pTabDlg->m_strWindowCaption);
	SetDlgItemText(IDC_WGT_WINDOWHANDLE, FormatHex((DWORD)pTabDlg->m_hwnd));
	SetDlgItemText(IDC_WPT_WINDOWPROC, pTabDlg->m_strWndProc);
	SetDlgItemText(IDC_WGT_RECTANGLE, pTabDlg->m_strRectangle);
	SetDlgItemText(IDC_WGT_RESTOREDRECT, pTabDlg->m_strRestoredRect);
	SetDlgItemText(IDC_WGT_CLIENTRECT, pTabDlg->m_strClientRect);
	SetDlgItemText(IDC_WPT_INSTANCEHANDLE, FormatHex((DWORD)pTabDlg->m_hInstance));
	SetDlgItemText(IDC_WGT_MENUHANDLE, FormatHex(pTabDlg->m_dwID));
	SetDlgItemText(IDC_WGT_USERDATA, FormatHex(pTabDlg->m_dwUserData));
	FillCBFromStrList((CComboBox*)GetDlgItem(IDC_WGT_EXTRABYTES), &pTabDlg->m_strListExtraBytes);
}


/////////////////////////////////////////////////////////////////////////////
// CWindowStylesPropTab

CWindowStylesPropTab::~CWindowStylesPropTab()
{
}


INT CWindowStylesPropTab::m_aControlIDs[] =
{
	IDC_WST_STYLES,
	IDC_WST_STYLESLIST,
	IDC_WST_EXTSTYLES,
	IDC_WST_EXTSTYLESLIST,
	0
};


BEGIN_MESSAGE_MAP(CWindowStylesPropTab, CPropertyTab)
	//{{AFX_MSG_MAP(CWindowStylesPropTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWindowStylesPropTab message handlers


void CWindowStylesPropTab::UpdateFields()
{
	CPropertyInspector* pTabDlg = (CPropertyInspector*)GetParent();

	SetDlgItemText(IDC_WST_STYLES, FormatHex(pTabDlg->m_flStyles));

	FillLBFromStrList((CListBox*)GetDlgItem(IDC_WST_STYLESLIST), &pTabDlg->m_strListStyles);

	SetDlgItemText(IDC_WST_EXTSTYLES, FormatHex(pTabDlg->m_flExtStyles));

	FillLBFromStrList((CListBox*)GetDlgItem(IDC_WST_EXTSTYLESLIST), &pTabDlg->m_strListExtStyles);
}



/////////////////////////////////////////////////////////////////////////////
// CWindowWindowsPropTab

CWindowWindowsPropTab::~CWindowWindowsPropTab()
{
}


INT CWindowWindowsPropTab::m_aControlIDs[] =
{
	IDC_WWT_NEXTWINDOW,
	IDC_WWT_PREVWINDOW,
	IDC_WWT_PARENTWINDOW,
	IDC_WWT_FIRSTCHILD,
	IDC_WWT_OWNERWINDOW,
	0
};


BEGIN_MESSAGE_MAP(CWindowWindowsPropTab, CPropertyTab)
	//{{AFX_MSG_MAP(CWindowWindowsPropTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWindowWindowsPropTab message handlers


BOOL CWindowWindowsPropTab::OnInitDialog()
{
	CPropertyTab::OnInitDialog();

	m_HotLinkNextWindow.SubclassDlgItem(IDC_WWT_NEXTWINDOW, this);
	m_HotLinkPrevWindow.SubclassDlgItem(IDC_WWT_PREVWINDOW, this);
	m_HotLinkParentWindow.SubclassDlgItem(IDC_WWT_PARENTWINDOW, this);
	m_HotLinkFirstChild.SubclassDlgItem(IDC_WWT_FIRSTCHILD, this);
	m_HotLinkOwnerWindow.SubclassDlgItem(IDC_WWT_OWNERWINDOW, this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}



void CWindowWindowsPropTab::UpdateFields()
{
	CPropertyInspector* pTabDlg = (CPropertyInspector*)GetParent();

	m_HotLinkNextWindow.SetJumpObject((DWORD)pTabDlg->m_hwndNext);
	m_HotLinkPrevWindow.SetJumpObject((DWORD)pTabDlg->m_hwndPrevious);
	m_HotLinkParentWindow.SetJumpObject((DWORD)pTabDlg->m_hwndParent);
	m_HotLinkFirstChild.SetJumpObject((DWORD)pTabDlg->m_hwndChild);
	m_HotLinkOwnerWindow.SetJumpObject((DWORD)pTabDlg->m_hwndOwner);
}



/////////////////////////////////////////////////////////////////////////////
// CWindowClassPropTab

CWindowClassPropTab::~CWindowClassPropTab()
{
}


INT CWindowClassPropTab::m_aControlIDs[] =
{
	IDC_WCT_CLASSNAME,
	IDC_WCT_STYLES,
	IDC_WCT_STYLESCOMBO,
	IDC_WCT_CLASSEXTRABYTES,
	IDC_WCT_CLASSEXTRABYTESCOMBO,
	IDC_WCT_CLASSATOM,
	IDC_WCT_WINDOWEXTRABYTES,
	IDC_WCT_MENUNAME,
	IDC_WCT_ICONHANDLE,
	IDC_WCT_CURSORHANDLE,
	IDC_WCT_BACKGROUNDBRUSH,
	IDC_WCT_INSTANCEHANDLE,
	IDC_WCT_WINDOWPROC,
	0
};


BEGIN_MESSAGE_MAP(CWindowClassPropTab, CPropertyTab)
	//{{AFX_MSG_MAP(CWindowClassPropTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWindowClassPropTab message handlers


BOOL CWindowClassPropTab::OnInitDialog()
{
	CPropertyTab::OnInitDialog();

// begin special font mucking-around
	if (_getmbcp() != 0)
	{
		CFont* pFont = GetStdFont(font_Fixed);
		ASSERT(pFont);
		HFONT hfont = (HFONT)pFont->m_hObject;

		SendDlgItemMessage(IDC_WCT_CLASSNAME, WM_SETFONT, (WPARAM)hfont, FALSE);
	}
// end special font mucking-around

	return TRUE;  // return TRUE  unless you set the focus to a control
}



void CWindowClassPropTab::UpdateFields()
{
	CPropertyInspector* pTabDlg = (CPropertyInspector*)GetParent();

	SetDlgItemText(IDC_WCT_CLASSNAME, pTabDlg->m_strClassName);
	SetDlgItemText(IDC_WCT_STYLES, pTabDlg->m_strClassStyles);
	FillCBFromStrList((CComboBox*)GetDlgItem(IDC_WCT_STYLESCOMBO), &pTabDlg->m_strListClassStyles);
	SetDlgItemText(IDC_WCT_CLASSEXTRABYTES, pTabDlg->m_strClassExtraBytes);
	FillCBFromStrList((CComboBox*)GetDlgItem(IDC_WCT_CLASSEXTRABYTESCOMBO), &pTabDlg->m_strListClassExtraBytes);
	SetDlgItemText(IDC_WCT_CLASSATOM, pTabDlg->m_strClassAtom);
	SetDlgItemText(IDC_WCT_WINDOWEXTRABYTES, pTabDlg->m_strClassWndExtraBytes);
	SetDlgItemText(IDC_WCT_MENUNAME, pTabDlg->m_strClassMenuName);
	SetDlgItemText(IDC_WCT_ICONHANDLE, pTabDlg->m_strClassIcon);
	SetDlgItemText(IDC_WCT_CURSORHANDLE, pTabDlg->m_strClassCursor);
	SetDlgItemText(IDC_WCT_BACKGROUNDBRUSH, pTabDlg->m_strClassBrush);
	SetDlgItemText(IDC_WCT_INSTANCEHANDLE, pTabDlg->m_strClassInstance);
	SetDlgItemText(IDC_WCT_WINDOWPROC, pTabDlg->m_strClassWndProc);
}



/////////////////////////////////////////////////////////////////////////////
// CWindowProcessPropTab

CWindowProcessPropTab::~CWindowProcessPropTab()
{
}


INT CWindowProcessPropTab::m_aControlIDs[] =
{
	IDC_WPT_PROCESSID,
	IDC_WPT_THREADID,
	0
};


BEGIN_MESSAGE_MAP(CWindowProcessPropTab, CPropertyTab)
	//{{AFX_MSG_MAP(CWindowProcessPropTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWindowProcessPropTab message handlers


BOOL CWindowProcessPropTab::OnInitDialog()
{
	CPropertyTab::OnInitDialog();

	m_HotLinkProcessID.SubclassDlgItem(IDC_WPT_PROCESSID, this);
	m_HotLinkThreadID.SubclassDlgItem(IDC_WPT_THREADID, this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}



void CWindowProcessPropTab::UpdateFields()
{
	CPropertyInspector* pTabDlg = (CPropertyInspector*)GetParent();

	if (pTabDlg->m_fValidProcessThreadID)
	{
		m_HotLinkProcessID.SetJumpObject(pTabDlg->m_ProcessID);
		m_HotLinkThreadID.SetJumpObject(pTabDlg->m_ThreadID);
	}
	else
	{
		m_HotLinkProcessID.MarkInvalid();
		m_HotLinkThreadID.MarkInvalid();
	}
}
