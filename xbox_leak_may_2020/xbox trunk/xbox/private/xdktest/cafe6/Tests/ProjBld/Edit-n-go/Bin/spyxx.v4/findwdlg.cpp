// findwdlg.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "findtool.h"
#include "findwdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindWindowDlg dialog

CFindWindowDlg::CFindWindowDlg(CWnd *pParentWnd) : CDialog(CFindWindowDlg::IDD, pParentWnd)
{
	//{{AFX_DATA_INIT(CFindWindowDlg)
	m_ShowProperties = TRUE;
	m_ShowMessages = FALSE;
	//}}AFX_DATA_INIT
}

CFindWindowDlg::~CFindWindowDlg()
{
}

void CFindWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindWindowDlg)
	DDX_Check(pDX, IDC_FW_SHOWPROPERTIES, m_ShowProperties);
	DDX_Check(pDX, IDC_FW_SHOWMESSAGES, m_ShowMessages);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFindWindowDlg, CDialog)
	//{{AFX_MSG_MAP(CFindWindowDlg)
	ON_BN_CLICKED(IDC_FW_HIDESPY, OnClickedHide)
	ON_EN_KILLFOCUS(IDC_FW_ENTERHWND, VerifyEnteredHwnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CFindWindowDlg message handlers

BOOL CFindWindowDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CFont* pFont = GetStdFont(font_Normal);
	ASSERT(pFont);
	HFONT hfont = (HFONT)pFont->m_hObject;

	SendDlgItemMessage(IDC_FW_ENTERHWND, WM_SETFONT, (WPARAM)hfont, FALSE);
	SendDlgItemMessage(IDC_FW_CAPTION, WM_SETFONT, (WPARAM)hfont, FALSE);
	SendDlgItemMessage(IDC_FW_CLASS, WM_SETFONT, (WPARAM)hfont, FALSE);
	SendDlgItemMessage(IDC_FW_STYLE, WM_SETFONT, (WPARAM)hfont, FALSE);
	SendDlgItemMessage(IDC_FW_RECT, WM_SETFONT, (WPARAM)hfont, FALSE);

// begin special font mucking-around
	if (_getmbcp() != 0)
	{
		pFont = GetStdFont(font_Fixed);
		ASSERT(pFont);
		hfont = (HFONT)pFont->m_hObject;

		SendDlgItemMessage(IDC_FW_CAPTION, WM_SETFONT, (WPARAM)hfont, FALSE);
		SendDlgItemMessage(IDC_FW_CLASS, WM_SETFONT, (WPARAM)hfont, FALSE);
	}
// end special font mucking-around

	m_FindToolIcon.SubclassDlgItem(IDC_FW_FINDTOOL, this);
	m_FindToolIcon.Initialize(this);

	m_hwndSelected = (HWND)NULL;

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CFindWindowDlg::OnOK()
{
	HWND hwndSelected;
	CHAR szHwnd[9];

	GetDlgItemText(IDC_FW_ENTERHWND, szHwnd, sizeof(szHwnd));

	hwndSelected = (HWND)_tcstoul(szHwnd, NULL, 16);

	if (::IsWindow(hwndSelected))
	{
		if (!IsSpyableWindow(hwndSelected))
		{
			SpyMessageBox(IDS_WND_NOT_SPYABLE);
			GotoDlgCtrl(GetDlgItem(IDC_FW_ENTERHWND));
			return;
		}

		m_hwndSelected = hwndSelected;

		// Restore the Spy app if necessary.
		if (IsDlgButtonChecked(IDC_FW_HIDESPY))
		{
			theApp.m_pMainWnd->ShowWindow(SW_SHOWNA);
			CheckDlgButton(IDC_FW_HIDESPY, 0);
		}
		CDialog::OnOK();
	}
	else
	{
		SpyMessageBox(IDS_FW_INVALIDHWND);
		GotoDlgCtrl(GetDlgItem(IDC_FW_ENTERHWND));
	}
}

void CFindWindowDlg::OnCancel()
{
	// Restore the Spy app if necessary.
	if (IsDlgButtonChecked(IDC_FW_HIDESPY))
	{
		theApp.m_pMainWnd->ShowWindow(SW_SHOWNA);
		CheckDlgButton(IDC_FW_HIDESPY, 0);
	}
	CDialog::OnCancel();
}

void CFindWindowDlg::SetSelectedWindow(HWND hwndSelected)
{
	CHAR sz[256];
	LPSTR lpszClass;
	int cch;
	RECT rc;

	if (::IsWindow(hwndSelected))
	{
		wsprintf(sz, "%08X", hwndSelected);
		SetDlgItemText(IDC_FW_ENTERHWND, sz);

		sz[0] = '"';
		cch = ::GetWindowText(hwndSelected, &sz[1], 256 - 2);
		sz[cch + 1] = '"';
		sz[cch + 2] = 0;
		SetDlgItemText(IDC_FW_CAPTION, sz);

		::GetClassName(hwndSelected, sz, 256);
		if ((lpszClass = GetExpandedClassName(sz)) != NULL)
			_tcscat(sz, lpszClass);
		SetDlgItemText(IDC_FW_CLASS, sz);

		wsprintf(sz, "%08X", ::GetWindowLong(hwndSelected, GWL_STYLE));
		SetDlgItemText(IDC_FW_STYLE, sz);

		::GetWindowRect(hwndSelected, &rc);
		wsprintf(sz, "(%d, %d)-(%d, %d) %dx%d", rc.left, rc.top, rc.right, rc.bottom, rc.right - rc.left, rc.bottom - rc.top);
		SetDlgItemText(IDC_FW_RECT, sz);
	}
	else
	{
		SetDlgItemText(IDC_FW_ENTERHWND, "");
		SetDlgItemText(IDC_FW_CAPTION, "");
		SetDlgItemText(IDC_FW_CLASS, "");
		SetDlgItemText(IDC_FW_STYLE, "");
		SetDlgItemText(IDC_FW_RECT, "");
	}
}

void CFindWindowDlg::VerifyEnteredHwnd()
{
	HWND hwndSelected;
	CHAR szHwnd[9];
	CHAR sz[256];
	LPSTR lpszClass;
	int cch;
	RECT rc;

	GetDlgItemText(IDC_FW_ENTERHWND, szHwnd, sizeof(szHwnd));

	hwndSelected = (HWND)_tcstoul(szHwnd, NULL, 16);	// assume user entered HWND in hex

	if (::IsWindow(hwndSelected))
	{
		sz[0] = '"';
		cch = ::GetWindowText(hwndSelected, &sz[1], 256 - 2);
		sz[cch + 1] = '"';
		sz[cch + 2] = 0;
		SetDlgItemText(IDC_FW_CAPTION, sz);

		::GetClassName(hwndSelected, sz, 256);
		if ((lpszClass = GetExpandedClassName(sz)) != NULL)
			_tcscat(sz, lpszClass);
		SetDlgItemText(IDC_FW_CLASS, sz);

		wsprintf(sz, "%08X", ::GetWindowLong(hwndSelected, GWL_STYLE));
		SetDlgItemText(IDC_FW_STYLE, sz);

		::GetWindowRect(hwndSelected, &rc);
		wsprintf(sz, "(%d, %d)-(%d, %d) %dx%d", rc.left, rc.top, rc.right, rc.bottom, rc.right - rc.left, rc.bottom - rc.top);
		SetDlgItemText(IDC_FW_RECT, sz);
	}
	else
	{
		SetDlgItemText(IDC_FW_CAPTION, "");
		SetDlgItemText(IDC_FW_CLASS, "");
		SetDlgItemText(IDC_FW_STYLE, "");
		SetDlgItemText(IDC_FW_RECT, "");
	}
}

void CFindWindowDlg::BeginFindWindowTracking()
{
	SetSelectedWindow((HWND)NULL);
	m_hwndFound = NULL;
}

void CFindWindowDlg::UpdateFindWindowTracking(HWND hwndFound)
{
	SetSelectedWindow(hwndFound);
	m_hwndFound = hwndFound;
}

void CFindWindowDlg::EndFindWindowTracking()
{
	if (m_hwndFound)
		m_hwndSelected = m_hwndFound;
}

void CFindWindowDlg::OnClickedHide()
{
	if (IsDlgButtonChecked(IDC_FW_HIDESPY))
	{
		theApp.m_pMainWnd->ShowWindow(SW_HIDE);
	}
	else
	{
		theApp.m_pMainWnd->ShowWindow(SW_SHOWNA);
	}
}

/////////////////////////////////////////////////////////////////////////////
