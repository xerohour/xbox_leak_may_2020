// srchdlgs.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "findtool.h"
#include "srchdlgs.h"
#include "spytreed.h"
#include "treectl.h"
#include "spytreec.h"
#include "wndnode.h"
#include "wndtreed.h"
#include "prctreed.h"
#include "thdtreed.h"
#include "msglog.h"
#include "msgview.h"
#include "msgdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchWindowDlg dialog

CSearchWindowDlg::CSearchWindowDlg(CWnd *pParentWnd) : CDialog(CSearchWindowDlg::IDD, pParentWnd)
{
	//{{AFX_DATA_INIT(CSearchWindowDlg)
	//}}AFX_DATA_INIT
}

CSearchWindowDlg::~CSearchWindowDlg()
{
}

void CSearchWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchWindowDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSearchWindowDlg, CDialog)
	//{{AFX_MSG_MAP(CSearchWindowDlg)
	ON_BN_CLICKED(IDC_SEARCH_FW_HIDESPY, OnClickedHide)
	ON_EN_KILLFOCUS(IDC_SEARCH_FW_ENTERHWND, VerifyEnteredHwnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchWindowDlg message handlers

BOOL CSearchWindowDlg::OnInitDialog()
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

	// set default direction of search
	CheckRadioButton(IDC_SEARCH_FW_UPDIRECTION, IDC_SEARCH_FW_DOWNDIRECTION,
		m_fSearchUp ? IDC_SEARCH_FW_UPDIRECTION :IDC_SEARCH_FW_DOWNDIRECTION);

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CSearchWindowDlg::OnOK()
{
	HWND hwndSelected;
	CHAR szHwnd[9];
	CHAR szTemp[256];

	if (GetDlgItemText(IDC_SEARCH_FW_ENTERHWND, szHwnd, sizeof(szHwnd)))
	{
		hwndSelected = (HWND)_tcstoul(szHwnd, NULL, 16);
		m_hwndSelected = hwndSelected;
		m_wSearchFlags = SRCHFLAG_WINDOW_USEHANDLE;

		// Restore the Spy app if necessary.
		if (IsDlgButtonChecked(IDC_SEARCH_FW_HIDESPY))
		{
			theApp.m_pMainWnd->ShowWindow(SW_SHOWNA);
			CheckDlgButton(IDC_SEARCH_FW_HIDESPY, 0);
		}

		// check direction of search
		if (IsDlgButtonChecked(IDC_SEARCH_FW_UPDIRECTION))
			m_fSearchUp = TRUE;
		else
			m_fSearchUp = FALSE;

		CDialog::OnOK();

	}
	else if (GetDlgItemText(IDC_SEARCH_FW_ENTERCAPTION, szTemp, sizeof(szTemp)))
	{
		m_strCaption = szTemp;
		m_wSearchFlags = SRCHFLAG_WINDOW_USECAPTION;

		// Restore the Spy app if necessary.
		if (IsDlgButtonChecked(IDC_SEARCH_FW_HIDESPY))
		{
			theApp.m_pMainWnd->ShowWindow(SW_SHOWNA);
			CheckDlgButton(IDC_SEARCH_FW_HIDESPY, 0);
		}

		// check direction of search
		if (IsDlgButtonChecked(IDC_SEARCH_FW_UPDIRECTION))
			m_fSearchUp = TRUE;
		else
			m_fSearchUp = FALSE;

		CDialog::OnOK();
	}
	else if (GetDlgItemText(IDC_SEARCH_FW_ENTERCLASS, szTemp, sizeof(szTemp)))
	{
		m_strClass = szTemp;
		m_wSearchFlags = SRCHFLAG_WINDOW_USECLASS;

		// Restore the Spy app if necessary.
		if (IsDlgButtonChecked(IDC_SEARCH_FW_HIDESPY))
		{
			theApp.m_pMainWnd->ShowWindow(SW_SHOWNA);
			CheckDlgButton(IDC_SEARCH_FW_HIDESPY, 0);
		}

		// check direction of search
		if (IsDlgButtonChecked(IDC_SEARCH_FW_UPDIRECTION))
			m_fSearchUp = TRUE;
		else
			m_fSearchUp = FALSE;

		CDialog::OnOK();
	}
	else
	{
		SpyMessageBox(IDS_NO_VALID_WND_SRCH);
		GotoDlgCtrl(GetDlgItem(IDC_SEARCH_FW_ENTERHWND));
	}
}


void CSearchWindowDlg::OnCancel()
{
	// Restore the Spy app if necessary.
	if (IsDlgButtonChecked(IDC_SEARCH_FW_HIDESPY))
	{
		theApp.m_pMainWnd->ShowWindow(SW_SHOWNA);
		CheckDlgButton(IDC_SEARCH_FW_HIDESPY, 0);
	}

	CDialog::OnCancel();
}


void CSearchWindowDlg::SetSelectedWindow(HWND hwndSelected)
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


void CSearchWindowDlg::VerifyEnteredHwnd()
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


void CSearchWindowDlg::BeginFindWindowTracking()
{
	SetSelectedWindow((HWND)NULL);
	m_hwndFound = NULL;
}


void CSearchWindowDlg::UpdateFindWindowTracking(HWND hwndFound)
{
	SetSelectedWindow(hwndFound);
	m_hwndFound = hwndFound;
}


void CSearchWindowDlg::EndFindWindowTracking()
{
	if (m_hwndFound)
		m_hwndSelected = m_hwndFound;
}


void CSearchWindowDlg::OnClickedHide()
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


/////////////////////////////////////////////////////////////////////////////
// CSearchProcessDlg dialog

CSearchProcessDlg::CSearchProcessDlg(CWnd *pParentWnd) : CDialog(CSearchProcessDlg::IDD, pParentWnd)
{
	//{{AFX_DATA_INIT(CSearchProcessDlg)
	//}}AFX_DATA_INIT
}

CSearchProcessDlg::~CSearchProcessDlg()
{
}

void CSearchProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchProcessDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSearchProcessDlg, CDialog)
	//{{AFX_MSG_MAP(CSearchProcessDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchProcessDlg message handlers

BOOL CSearchProcessDlg::OnInitDialog()
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

	// set default direction of search
	CheckRadioButton(IDC_SEARCH_FP_UPDIRECTION, IDC_SEARCH_FP_DOWNDIRECTION,
		m_fSearchUp ? IDC_SEARCH_FP_UPDIRECTION :IDC_SEARCH_FP_DOWNDIRECTION);

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CSearchProcessDlg::OnOK()
{
	CHAR szTemp[256];

	if (GetDlgItemText(IDC_SEARCH_FP_ENTERPROCESS, szTemp, sizeof(szTemp)))
	{
		m_wSearchFlags = SRCHFLAG_PROCESS_USEPROCESS;
		m_dwPrcSelected = (DWORD)_tcstoul(szTemp, NULL, 16);
	}
	else if (GetDlgItemText(IDC_SEARCH_FP_ENTERMODULE, szTemp, sizeof(szTemp)))
	{
		m_wSearchFlags = SRCHFLAG_PROCESS_USEMODULE;
		m_strModule = szTemp;
	}
	else
	{
		SpyMessageBox(IDS_NO_VALID_PRC_SRCH);
		GotoDlgCtrl(GetDlgItem(IDC_SEARCH_FP_ENTERPROCESS));
	}

	// check direction of search
	if (IsDlgButtonChecked(IDC_SEARCH_FP_UPDIRECTION))
		m_fSearchUp = TRUE;
	else
		m_fSearchUp = FALSE;

	CDialog::OnOK();
}


void CSearchProcessDlg::OnCancel()
{
	CDialog::OnCancel();
}


/////////////////////////////////////////////////////////////////////////////
// CSearchThreadDlg dialog

CSearchThreadDlg::CSearchThreadDlg(CWnd *pParentWnd) : CDialog(CSearchThreadDlg::IDD, pParentWnd)
{
	//{{AFX_DATA_INIT(CSearchThreadDlg)
	//}}AFX_DATA_INIT
}

CSearchThreadDlg::~CSearchThreadDlg()
{
}

void CSearchThreadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchThreadDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSearchThreadDlg, CDialog)
	//{{AFX_MSG_MAP(CSearchThreadDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchThreadDlg message handlers

BOOL CSearchThreadDlg::OnInitDialog()
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

	// set default direction of search
	CheckRadioButton(IDC_SEARCH_FT_UPDIRECTION, IDC_SEARCH_FT_DOWNDIRECTION,
		m_fSearchUp ? IDC_SEARCH_FT_UPDIRECTION :IDC_SEARCH_FT_DOWNDIRECTION);

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CSearchThreadDlg::OnOK()
{
	CHAR szTemp[256];

	if (GetDlgItemText(IDC_SEARCH_FT_ENTERTHREAD, szTemp, sizeof(szTemp)))
	{
		m_wSearchFlags = SRCHFLAG_THREAD_USETHREAD;
		m_dwThdSelected = (DWORD)_tcstoul(szTemp, NULL, 16);
	}
	else if (GetDlgItemText(IDC_SEARCH_FT_ENTERMODULE, szTemp, sizeof(szTemp)))
	{
		m_wSearchFlags = SRCHFLAG_THREAD_USEMODULE;
		m_strModule = szTemp;
	}
	else
	{
		SpyMessageBox(IDS_NO_VALID_THD_SRCH);
		GotoDlgCtrl(GetDlgItem(IDC_SEARCH_FT_ENTERTHREAD));
	}

	// check direction of search
	if (IsDlgButtonChecked(IDC_SEARCH_FT_UPDIRECTION))
		m_fSearchUp = TRUE;
	else
		m_fSearchUp = FALSE;

	CDialog::OnOK();
}


void CSearchThreadDlg::OnCancel()
{
	CDialog::OnCancel();
}


/////////////////////////////////////////////////////////////////////////////
// CSearchMessageDlg dialog

CSearchMessageDlg::CSearchMessageDlg(CWnd *pParentWnd) : CDialog(CSearchMessageDlg::IDD, pParentWnd)
{
	//{{AFX_DATA_INIT(CSearchMessageDlg)
	//}}AFX_DATA_INIT
}

CSearchMessageDlg::~CSearchMessageDlg()
{
}

void CSearchMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchMessageDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSearchMessageDlg, CDialog)
	//{{AFX_MSG_MAP(CSearchMessageDlg)
	ON_BN_CLICKED(IDC_SEARCH_FM_HIDESPY, OnClickedHide)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchMessageDlg message handlers

BOOL CSearchMessageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CFont* pFont = GetStdFont(font_Normal);
	ASSERT(pFont);
	HFONT hfont = (HFONT)pFont->m_hObject;

	SendDlgItemMessage(IDC_SEARCH_FM_ENTERHWND, WM_SETFONT, (WPARAM)hfont, FALSE);
	SendDlgItemMessage(IDC_SEARCH_FM_SELECTTYPE, WM_SETFONT, (WPARAM)hfont, FALSE);
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

		if (m_wMsgSelected && pmd->msg == m_wMsgSelected)
			pMsgsComboBox->SetCurSel(iSel);
	}

	CComboBox *pTypeComboBox = (CComboBox*)GetDlgItem(IDC_SEARCH_FM_SELECTTYPE);

	// this assumes that ordering of POSTTYPE_* constants do not change!
	for (i = IDS_POSTTYPE_POSTED; i <= IDS_POSTTYPE_RETURN; i++)
	{
		pTypeComboBox->AddString(ids(i));
	}
	pTypeComboBox->SetCurSel(m_uTypeSelected);

	// set default direction of search
	CheckRadioButton(IDC_SEARCH_FM_UPDIRECTION, IDC_SEARCH_FM_DOWNDIRECTION,
		m_fSearchUp ? IDC_SEARCH_FM_UPDIRECTION :IDC_SEARCH_FM_DOWNDIRECTION);

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CSearchMessageDlg::OnOK()
{
	HWND hwndSelected;
	CHAR szHwnd[9];
	CHAR szTmp[32];
	int iSel;

	if (GetDlgItemText(IDC_SEARCH_FM_ENTERHWND, szHwnd, sizeof(szHwnd)))
	{
		hwndSelected = (HWND)_tcstoul(szHwnd, NULL, 16);
		m_hwndSelected = hwndSelected;
		m_wSearchFlags = SRCHFLAG_MESSAGE_USEHANDLE;

		if (GetDlgItemText(IDC_SEARCH_FM_SELECTMSG, szTmp, sizeof(szTmp)))
		{
			CComboBox *pMsgsComboBox = (CComboBox*)GetDlgItem(IDC_SEARCH_FM_SELECTMSG);
			if ((iSel = pMsgsComboBox->GetCurSel()) != CB_ERR)
			{
				PMSGDESC pmd = (PMSGDESC)pMsgsComboBox->GetItemDataPtr(iSel);

				m_wMsgSelected = pmd->msg;
				m_wSearchFlags |= SRCHFLAG_MESSAGE_USEMSG;
			}
		}

		if (GetDlgItemText(IDC_SEARCH_FM_SELECTTYPE, szTmp, sizeof(szTmp)))
		{
			CComboBox *pTypeComboBox = (CComboBox*)GetDlgItem(IDC_SEARCH_FM_SELECTTYPE);
			if ((iSel = pTypeComboBox->GetCurSel()) != CB_ERR)
			{
				// this assumes that ordering of POSTTYPE_* constants do not change!
				m_uTypeSelected = iSel;
				m_wSearchFlags |= SRCHFLAG_MESSAGE_USEPOSTTYPE;
			}
		}

		// Restore the Spy app if necessary.
		if (IsDlgButtonChecked(IDC_SEARCH_FM_HIDESPY))
		{
			theApp.m_pMainWnd->ShowWindow(SW_SHOWNA);
			CheckDlgButton(IDC_SEARCH_FM_HIDESPY, 0);
		}

		// check direction of search
		if (IsDlgButtonChecked(IDC_SEARCH_FM_UPDIRECTION))
			m_fSearchUp = TRUE;
		else
			m_fSearchUp = FALSE;

		CDialog::OnOK();
	}
	else if (GetDlgItemText(IDC_SEARCH_FM_SELECTMSG, szTmp, sizeof(szTmp)))
	{
		CComboBox *pMsgsComboBox = (CComboBox*)GetDlgItem(IDC_SEARCH_FM_SELECTMSG);
		if ((iSel = pMsgsComboBox->GetCurSel()) != CB_ERR)
		{
			PMSGDESC pmd = (PMSGDESC)pMsgsComboBox->GetItemDataPtr(iSel);

			m_wMsgSelected = pmd->msg;
			m_wSearchFlags = SRCHFLAG_MESSAGE_USEMSG;

			if (GetDlgItemText(IDC_SEARCH_FM_SELECTTYPE, szTmp, sizeof(szTmp)))
			{
				CComboBox *pTypeComboBox = (CComboBox*)GetDlgItem(IDC_SEARCH_FM_SELECTTYPE);
				if ((iSel = pTypeComboBox->GetCurSel()) != CB_ERR)
				{
					// this assumes that ordering of POSTTYPE_* constants do not change!
					m_uTypeSelected = iSel;
					m_wSearchFlags |= SRCHFLAG_MESSAGE_USEPOSTTYPE;
				}
			}

			// Restore the Spy app if necessary.
			if (IsDlgButtonChecked(IDC_SEARCH_FM_HIDESPY))
			{
				theApp.m_pMainWnd->ShowWindow(SW_SHOWNA);
				CheckDlgButton(IDC_SEARCH_FM_HIDESPY, 0);
			}

			// check direction of search
			if (IsDlgButtonChecked(IDC_SEARCH_FM_UPDIRECTION))
				m_fSearchUp = TRUE;
			else
				m_fSearchUp = FALSE;

			CDialog::OnOK();
		}
		else
		{
			SpyMessageBox(IDS_NO_VALID_MSG_SRCH);
			GotoDlgCtrl(GetDlgItem(IDC_SEARCH_FM_ENTERHWND));
		}
	}
	else if (GetDlgItemText(IDC_SEARCH_FM_SELECTTYPE, szTmp, sizeof(szTmp)))
	{
		CComboBox *pTypeComboBox = (CComboBox*)GetDlgItem(IDC_SEARCH_FM_SELECTTYPE);
		if ((iSel = pTypeComboBox->GetCurSel()) != CB_ERR)
		{
			// this assumes that ordering of POSTTYPE_* constants do not change!
			m_uTypeSelected = iSel;
			m_wSearchFlags = SRCHFLAG_MESSAGE_USEPOSTTYPE;

			// Restore the Spy app if necessary.
			if (IsDlgButtonChecked(IDC_SEARCH_FM_HIDESPY))
			{
				theApp.m_pMainWnd->ShowWindow(SW_SHOWNA);
				CheckDlgButton(IDC_SEARCH_FM_HIDESPY, 0);
			}

			// check direction of search
			if (IsDlgButtonChecked(IDC_SEARCH_FM_UPDIRECTION))
				m_fSearchUp = TRUE;
			else
				m_fSearchUp = FALSE;

			CDialog::OnOK();
		}
		else
		{
			SpyMessageBox(IDS_NO_VALID_MSG_SRCH);
			GotoDlgCtrl(GetDlgItem(IDC_SEARCH_FM_ENTERHWND));
		}
	}
	else
	{
		SpyMessageBox(IDS_NO_VALID_MSG_SRCH);
		GotoDlgCtrl(GetDlgItem(IDC_SEARCH_FM_ENTERHWND));
	}
}


void CSearchMessageDlg::OnCancel()
{
	// Restore the Spy app if necessary.
	if (IsDlgButtonChecked(IDC_SEARCH_FM_HIDESPY))
	{
		theApp.m_pMainWnd->ShowWindow(SW_SHOWNA);
		CheckDlgButton(IDC_SEARCH_FM_HIDESPY, 0);
	}

	CDialog::OnCancel();
}

void CSearchMessageDlg::SetSelectedWindow(HWND hwndSelected)
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


void CSearchMessageDlg::BeginFindWindowTracking()
{
	SetSelectedWindow((HWND)NULL);
	m_hwndTrack = NULL;
}


void CSearchMessageDlg::UpdateFindWindowTracking(HWND hwndFound)
{
	SetSelectedWindow(hwndFound);
	m_hwndTrack = hwndFound;
}


void CSearchMessageDlg::EndFindWindowTracking()
{
	if (m_hwndTrack)
		m_hwndSelected = m_hwndTrack;
}


void CSearchMessageDlg::OnClickedHide()
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
