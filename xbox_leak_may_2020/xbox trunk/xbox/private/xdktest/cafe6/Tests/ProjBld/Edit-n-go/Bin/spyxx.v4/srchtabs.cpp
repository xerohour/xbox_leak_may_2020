// srchtabs.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "findtool.h"
#include "srchtabs.h"
#include "msglog.h"
#include "msgview.h"
#include "msgdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CFiltersTabbedDialog

CSearchTabbedDialog::~CSearchTabbedDialog()
{
}


BEGIN_MESSAGE_MAP(CSearchTabbedDialog, CTabbedDialog)
	//{{AFX_MSG_MAP(CFiltersTabbedDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSearchObjectDlgTab dialog

CSearchObjectDlgTab::~CSearchObjectDlgTab()
{
}

BEGIN_MESSAGE_MAP(CSearchObjectDlgTab, CDialog)
	//{{AFX_MSG_MAP(CSearchObjectDlgTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchObjectDlgTab message handlers

BOOL CSearchObjectDlgTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSearchObjectDlgTab::CommitTab()
{
	CDlgTab::CommitTab();
}

BOOL CSearchObjectDlgTab::ValidateTab()
{
	return(CDlgTab::ValidateTab());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSearchWindowDlgTab dialog

CSearchWindowDlgTab::~CSearchWindowDlgTab()
{
}

BEGIN_MESSAGE_MAP(CSearchWindowDlgTab, CDialog)
	//{{AFX_MSG_MAP(CSearchWindowDlgTab)
	ON_BN_CLICKED(IDC_SEARCH_FW_HIDESPY, OnClickedHide)
	ON_EN_KILLFOCUS(IDC_SEARCH_FW_ENTERHWND, VerifyEnteredHwnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchWindowDlgTab message handlers

BOOL CSearchWindowDlgTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	CFont* pFont = GetStdFont(font_Normal);
	ASSERT(pFont);
	HFONT hfont = (HFONT)pFont->m_hObject;

	SendDlgItemMessage(IDC_SEARCH_FW_ENTERHWND, WM_SETFONT, (WPARAM)hfont, FALSE);
	SendDlgItemMessage(IDC_SEARCH_FW_ENTERCAPTION, WM_SETFONT, (WPARAM)hfont, FALSE);
	SendDlgItemMessage(IDC_SEARCH_FW_ENTERCLASS, WM_SETFONT, (WPARAM)hfont, FALSE);

// begin special font mucking-around
	if (_getmbcp() != 0)
	{
		pFont = GetStdFont(font_Fixed);
		ASSERT(pFont);
		hfont = (HFONT)pFont->m_hObject;

		SendDlgItemMessage(IDC_SEARCH_FW_ENTERCAPTION, WM_SETFONT, (WPARAM)hfont, FALSE);
		SendDlgItemMessage(IDC_SEARCH_FW_ENTERCLASS, WM_SETFONT, (WPARAM)hfont, FALSE);
	}
// end special font mucking-around

	m_FindToolIcon.SubclassDlgItem(IDC_SEARCH_FW_FINDTOOL, this);
	m_FindToolIcon.Initialize(this);

	SetSelectedWindow(m_hwndSelected);
	SetDlgItemText(IDC_SEARCH_FW_ENTERCAPTION, m_strCaption);
	SetDlgItemText(IDC_SEARCH_FW_ENTERCLASS, m_strClass);
	m_wSearchFlags = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSearchWindowDlgTab::SetSelectedWindow(HWND hwndSelected)
{
	CHAR sz[256];
	LPSTR lpszClass;
	int cch;

	if (::IsWindow(hwndSelected))
	{
		wsprintf(sz, "%08X", hwndSelected);
		SetDlgItemText(IDC_SEARCH_FW_ENTERHWND, sz);

		sz[0] = '"';
		cch = ::GetWindowText(hwndSelected, &sz[1], 256 - 2);
		sz[cch + 1] = '"';
		sz[cch + 2] = 0;
		SetDlgItemText(IDC_SEARCH_FW_ENTERCAPTION, sz);

		::GetClassName(hwndSelected, sz, 256);
		if ((lpszClass = GetExpandedClassName(sz)) != NULL)
			_tcscat(sz, lpszClass);
		SetDlgItemText(IDC_SEARCH_FW_ENTERCLASS, sz);
	}
	else
	{
		SetDlgItemText(IDC_SEARCH_FW_ENTERHWND, "");
		SetDlgItemText(IDC_SEARCH_FW_ENTERCAPTION, "");
		SetDlgItemText(IDC_SEARCH_FW_ENTERCLASS, "");
	}
}

void CSearchWindowDlgTab::VerifyEnteredHwnd()
{
	HWND hwndSelected;
	CHAR szHwnd[9];
	CHAR sz[256];
	LPSTR lpszClass;
	int cch;

	// if user didn't clear out control, check that the contents are a valid HWND
	if (GetDlgItemText(IDC_SEARCH_FW_ENTERHWND, szHwnd, sizeof(szHwnd)) > 0)
	{
		hwndSelected = (HWND)_tcstoul(szHwnd, NULL, 16);	// assume user entered HWND in hex

		if (::IsWindow(hwndSelected))
		{
			sz[0] = '"';
			cch = ::GetWindowText(hwndSelected, &sz[1], 256 - 2);
			sz[cch + 1] = '"';
			sz[cch + 2] = 0;
			SetDlgItemText(IDC_SEARCH_FW_ENTERCAPTION, sz);

			::GetClassName(hwndSelected, sz, 256);
			if ((lpszClass = GetExpandedClassName(sz)) != NULL)
				_tcscat(sz, lpszClass);
			SetDlgItemText(IDC_SEARCH_FW_ENTERCLASS, sz);
		}
		else
		{
			SetDlgItemText(IDC_SEARCH_FW_ENTERCAPTION, "");
			SetDlgItemText(IDC_SEARCH_FW_ENTERCLASS, "");
		}
	}
}

void CSearchWindowDlgTab::BeginFindWindowTracking()
{
	SetSelectedWindow((HWND)NULL);
	m_hwndFound = NULL;
}

void CSearchWindowDlgTab::UpdateFindWindowTracking(HWND hwndFound)
{
	SetSelectedWindow(hwndFound);
	m_hwndFound = hwndFound;
}

void CSearchWindowDlgTab::EndFindWindowTracking()
{
	if (m_hwndFound)
		m_hwndSelected = m_hwndFound;
}

void CSearchWindowDlgTab::OnClickedHide()
{
	if (IsDlgButtonChecked(IDC_SEARCH_FW_HIDESPY))
	{
		theApp.m_pMainWnd->ShowWindow(SW_HIDE);
	}
	else
	{
		theApp.m_pMainWnd->ShowWindow(SW_SHOWNA);
	}
}

void CSearchWindowDlgTab::CommitTab()
{
	CDlgTab::CommitTab();
}

BOOL CSearchWindowDlgTab::ValidateTab()
{
	return(CDlgTab::ValidateTab());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSearchProcessDlgTab dialog

CSearchProcessDlgTab::~CSearchProcessDlgTab()
{
}

BEGIN_MESSAGE_MAP(CSearchProcessDlgTab, CDialog)
	//{{AFX_MSG_MAP(CSearchProcessDlgTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchProcessDlgTab message handlers

BOOL CSearchProcessDlgTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	CFont* pFont = GetStdFont(font_Normal);
	ASSERT(pFont);
	HFONT hfont = (HFONT)pFont->m_hObject;

	SendDlgItemMessage(IDC_SEARCH_FP_ENTERPROCESS, WM_SETFONT, (WPARAM)hfont, FALSE);
	SendDlgItemMessage(IDC_SEARCH_FP_ENTERMODULE, WM_SETFONT, (WPARAM)hfont, FALSE);

// begin special font mucking-around
	if (_getmbcp() != 0)
	{
		pFont = GetStdFont(font_Fixed);
		ASSERT(pFont);
		hfont = (HFONT)pFont->m_hObject;

		SendDlgItemMessage(IDC_SEARCH_FP_ENTERMODULE, WM_SETFONT, (WPARAM)hfont, FALSE);
	}
// end special font mucking-around

	if (m_dwPrcSelected != -1)
	{
		CHAR sz[10];
		wsprintf(sz, "%08X", m_dwPrcSelected);
		SetDlgItemText(IDC_SEARCH_FP_ENTERPROCESS, sz);
	}
	SetDlgItemText(IDC_SEARCH_FP_ENTERMODULE, m_strModule);
	m_wSearchFlags = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSearchProcessDlgTab::CommitTab()
{
	CDlgTab::CommitTab();
}

BOOL CSearchProcessDlgTab::ValidateTab()
{
	return(CDlgTab::ValidateTab());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSearchThreadDlgTab dialog

CSearchThreadDlgTab::~CSearchThreadDlgTab()
{
}

BEGIN_MESSAGE_MAP(CSearchThreadDlgTab, CDialog)
	//{{AFX_MSG_MAP(CSearchThreadDlgTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchThreadDlgTab message handlers

BOOL CSearchThreadDlgTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	CFont* pFont = GetStdFont(font_Normal);
	ASSERT(pFont);
	HFONT hfont = (HFONT)pFont->m_hObject;

	SendDlgItemMessage(IDC_SEARCH_FT_ENTERTHREAD, WM_SETFONT, (WPARAM)hfont, FALSE);
	SendDlgItemMessage(IDC_SEARCH_FT_ENTERMODULE, WM_SETFONT, (WPARAM)hfont, FALSE);

// begin special font mucking-around
	if (_getmbcp() != 0)
	{
		pFont = GetStdFont(font_Fixed);
		ASSERT(pFont);
		hfont = (HFONT)pFont->m_hObject;

		SendDlgItemMessage(IDC_SEARCH_FT_ENTERMODULE, WM_SETFONT, (WPARAM)hfont, FALSE);
	}
// end special font mucking-around

	if (m_dwThdSelected != -1)
	{
		CHAR sz[10];
		wsprintf(sz, "%08X", m_dwThdSelected);
		SetDlgItemText(IDC_SEARCH_FT_ENTERTHREAD, sz);
	}
	SetDlgItemText(IDC_SEARCH_FT_ENTERMODULE, m_strModule);
	m_wSearchFlags = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSearchThreadDlgTab::CommitTab()
{
	CDlgTab::CommitTab();
}

BOOL CSearchThreadDlgTab::ValidateTab()
{
	return(CDlgTab::ValidateTab());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSearchMessageDlgTab dialog

CSearchMessageDlgTab::~CSearchMessageDlgTab()
{
}

BEGIN_MESSAGE_MAP(CSearchMessageDlgTab, CDialog)
	//{{AFX_MSG_MAP(CSearchMessageDlgTab)
	ON_BN_CLICKED(IDC_SEARCH_FM_HIDESPY, OnClickedHide)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchMessageDlgTab message handlers

BOOL CSearchMessageDlgTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	CFont* pFont = GetStdFont(font_Normal);
	ASSERT(pFont);
	HFONT hfont = (HFONT)pFont->m_hObject;

	SendDlgItemMessage(IDC_SEARCH_FM_ENTERHWND, WM_SETFONT, (WPARAM)hfont, FALSE);
	SendDlgItemMessage(IDC_SEARCH_FM_SELECTMSG, WM_SETFONT, (WPARAM)hfont, FALSE);

	m_FindToolIcon.SubclassDlgItem(IDC_SEARCH_FM_FINDTOOL, this);
	m_FindToolIcon.Initialize(this);

	SetSelectedWindow(m_hwndSelected);
	m_wSearchFlags = 0;

	CComboBox *pMsgsComboBox = (CComboBox*)GetDlgItem(IDC_SEARCH_FM_SELECTMSG);
	PMSGDESC pmd;
	int i;

	for (i = 0, pmd = CMsgDoc::m_aMsgDesc; i < CMsgDoc::m_cMsgs; i++, pmd++)
	{
		int iSel = pMsgsComboBox->AddString(pmd->pszMsg);
		pMsgsComboBox->SetItemDataPtr(iSel, pmd);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSearchMessageDlgTab::SetSelectedWindow(HWND hwndSelected)
{
	CHAR sz[256];

	if (::IsWindow(hwndSelected))
	{
		wsprintf(sz, "%08X", hwndSelected);
		SetDlgItemText(IDC_SEARCH_FM_ENTERHWND, sz);
	}
	else
	{
		SetDlgItemText(IDC_SEARCH_FM_ENTERHWND, "");
	}
}

void CSearchMessageDlgTab::BeginFindWindowTracking()
{
	SetSelectedWindow((HWND)NULL);
	m_hwndTrack = NULL;
}

void CSearchMessageDlgTab::UpdateFindWindowTracking(HWND hwndFound)
{
	SetSelectedWindow(hwndFound);
	m_hwndTrack = hwndFound;
}

void CSearchMessageDlgTab::EndFindWindowTracking()
{
	if (m_hwndTrack)
		m_hwndSelected = m_hwndTrack;
}

void CSearchMessageDlgTab::OnClickedHide()
{
	if (IsDlgButtonChecked(IDC_SEARCH_FM_HIDESPY))
	{
		theApp.m_pMainWnd->ShowWindow(SW_HIDE);
	}
	else
	{
		theApp.m_pMainWnd->ShowWindow(SW_SHOWNA);
	}
}

void CSearchMessageDlgTab::CommitTab()
{
	CDlgTab::CommitTab();
}

BOOL CSearchMessageDlgTab::ValidateTab()
{
	return(CDlgTab::ValidateTab());
}

/////////////////////////////////////////////////////////////////////////////
