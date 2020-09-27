// thdptabs.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#pragma message("Remove local copy of TLHELP32.H when official version available.")

#include "hotlinkc.h"
#include "proptab.h"
#include "thdptabs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CThreadGeneralPropTab

VALUETABLE CThreadGeneralPropTab::m_astThreadState[] =
{
	0, NULL,
	1, NULL,
	2, NULL,
	3, NULL,
	4, NULL,
	5, NULL,
	6, NULL,
	7, NULL,
	0, NULL
};

VALUETABLE CThreadGeneralPropTab::m_astWaitReason[] =
{
	0, NULL,
	1, NULL,
	2, NULL,
	3, NULL,
	4, NULL,
	5, NULL,
	6, NULL,
	7, NULL,
	8, NULL,
	9, NULL,
	10, NULL,
	11, NULL,
	12, NULL,
	13, NULL,
	14, NULL,
	15, NULL,
	16, NULL,
	17, NULL,
	18, NULL,
	19, NULL,
	0, NULL
};



CThreadGeneralPropTab::~CThreadGeneralPropTab()
{
}


INT CThreadGeneralPropTab::m_aControlIDs[] =
{
	IDC_TGT_THREADID,
	IDC_TGT_PROCESSID,
	IDC_TGT_EXECUTABLE,
	IDC_TGT_CURRENTPRIORITY,
	IDC_TGT_BASEPRIORITY,
	IDC_TGT_THREADSTATE,
	IDC_TGT_WAITREASON,
	IDC_TGT_STARTADDRESS,
//	IDC_TGT_USERPC,
	IDC_TGT_CPUTIME,
	IDC_TGT_USERTIME,
	IDC_TGT_PRIVILEGEDTIME,
	IDC_TGT_ELAPSEDTIME,
	IDC_TGT_CONTEXTSWITCHES,
	IDC_TGT_USERPC,
	0
};


BEGIN_MESSAGE_MAP(CThreadGeneralPropTab, CPropertyTab)
	//{{AFX_MSG_MAP(CThreadGeneralPropTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CThreadGeneralPropTab message handlers


BOOL CThreadGeneralPropTab::OnInitDialog()
{
	CPropertyTab::OnInitDialog();

	m_astThreadState[0].pszValue = ids(IDS_THDP_INITIALIZED);
	m_astThreadState[1].pszValue = ids(IDS_THDP_READY);
	m_astThreadState[2].pszValue = ids(IDS_THDP_RUNNING);
	m_astThreadState[3].pszValue = ids(IDS_THDP_STANDBY);
	m_astThreadState[4].pszValue = ids(IDS_THDP_TERMINATED);
	m_astThreadState[5].pszValue = ids(IDS_THDP_WAIT);
	m_astThreadState[6].pszValue = ids(IDS_THDP_TRANSITION);
	m_astThreadState[7].pszValue = ids(IDS_THDP_UNKNOWN);

	m_astWaitReason[0].pszValue  = ids(IDS_THDP_EXECUTIVE);
	m_astWaitReason[1].pszValue  = ids(IDS_THDP_FREE_PAGE);
	m_astWaitReason[2].pszValue  = ids(IDS_THDP_PAGE_IN);
	m_astWaitReason[3].pszValue  = ids(IDS_THDP_POOL_ALLOCATION);
	m_astWaitReason[4].pszValue  = ids(IDS_THDP_EXECUTION_DELAY);
	m_astWaitReason[5].pszValue  = ids(IDS_THDP_SUSPENDED);
	m_astWaitReason[6].pszValue  = ids(IDS_THDP_USER_REQUEST);
	m_astWaitReason[7].pszValue  = ids(IDS_THDP_EXECUTIVE);
	m_astWaitReason[8].pszValue  = ids(IDS_THDP_FREE_PAGE);
	m_astWaitReason[9].pszValue  = ids(IDS_THDP_PAGE_IN);
	m_astWaitReason[10].pszValue = ids(IDS_THDP_POOL_ALLOCATION);
	m_astWaitReason[11].pszValue = ids(IDS_THDP_EXECUTION_DELAY);
	m_astWaitReason[12].pszValue = ids(IDS_THDP_SUSPENDED);
	m_astWaitReason[13].pszValue = ids(IDS_THDP_USER_REQUEST);
	m_astWaitReason[14].pszValue = ids(IDS_THDP_EVENT_PAIR_HIGH);
	m_astWaitReason[15].pszValue = ids(IDS_THDP_EVENT_PAIR_LOW);
	m_astWaitReason[16].pszValue = ids(IDS_THDP_LPC_RECEIVE);
	m_astWaitReason[17].pszValue = ids(IDS_THDP_LPC_REPLY);
	m_astWaitReason[18].pszValue = ids(IDS_THDP_VIRTUAL_MEMORY);
	m_astWaitReason[19].pszValue = ids(IDS_THDP_PAGE_OUT);

// begin special font mucking-around
	if (_getmbcp() != 0)
	{
		CFont* pFont = GetStdFont(font_Fixed);
		ASSERT(pFont);
		HFONT hfont = (HFONT)pFont->m_hObject;

		SendDlgItemMessage(IDC_TGT_EXECUTABLE, WM_SETFONT, (WPARAM)hfont, FALSE);
	}
// end special font mucking-around

	m_HotLinkProcessID.SubclassDlgItem(IDC_TGT_PROCESSID, this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}



void CThreadGeneralPropTab::UpdateFields()
{
	CPropertyInspector* pTabDlg = (CPropertyInspector*)GetParent();

	if (!theApp.IsChicago())
	{
		// Win32
		SetDlgItemText(IDC_TGT_THREADID, FormatHex((DWORD)pTabDlg->m_ThreadInfo.dwThreadId));
		m_HotLinkProcessID.SetJumpObject((DWORD)pTabDlg->m_ThreadInfo.dwProcessId);
		SetDlgItemText(IDC_TGT_EXECUTABLE, pTabDlg->m_strExecutable);
		SetDlgItemText(IDC_TGT_CURRENTPRIORITY, FormatULong(pTabDlg->m_ThreadInfo.dwPriorityCrnt));
		SetDlgItemText(IDC_TGT_BASEPRIORITY, FormatULong(pTabDlg->m_ThreadInfo.dwPriorityBase));
		SetDlgItemText(IDC_TGT_THREADSTATE, FormatULongFromTable(pTabDlg->m_ThreadInfo.dwThreadState, m_astThreadState));
		SetDlgItemText(IDC_TGT_WAITREASON, FormatULongFromTable(pTabDlg->m_ThreadInfo.dwThreadWaitReason, m_astWaitReason));
		SetDlgItemText(IDC_TGT_STARTADDRESS, FormatHex((DWORD)pTabDlg->m_ThreadInfo.dwStartAddr));
		SetDlgItemText(IDC_TGT_CPUTIME, FormatTime(pTabDlg->m_ThreadInfo.liPctCPUTime));
		SetDlgItemText(IDC_TGT_USERTIME, FormatTime(pTabDlg->m_ThreadInfo.liPctUserTime));
		SetDlgItemText(IDC_TGT_PRIVILEGEDTIME, FormatTime(pTabDlg->m_ThreadInfo.liPctPrivTime));
		SetDlgItemText(IDC_TGT_ELAPSEDTIME, FormatTimeDiff(pTabDlg->m_ThreadInfo.liElapsedTime, pTabDlg->m_ThreadInfo.PerfTime));
		SetDlgItemText(IDC_TGT_CONTEXTSWITCHES, FormatULong(pTabDlg->m_ThreadInfo.dwCntxSwtchPerSec));
		if (pTabDlg->m_ThreadDetInfo.dwUserPC)
			SetDlgItemText(IDC_TGT_USERPC, FormatHex((DWORD)pTabDlg->m_ThreadDetInfo.dwUserPC));
		else
			SetDlgItemText(IDC_TGT_USERPC, ids(IDS_UNAVAILABLE));
	}
	else
	{
		// Chicago
		SetDlgItemText(IDC_TGT_THREADID, FormatHex((DWORD)pTabDlg->m_ThreadEntry.th32ThreadID));
		m_HotLinkProcessID.SetJumpObject((DWORD)pTabDlg->m_ThreadEntry.th32OwnerProcessID);
		SetDlgItemText(IDC_TGT_EXECUTABLE, pTabDlg->m_strExecutable);
		SetDlgItemText(IDC_TGT_CURRENTPRIORITY, FormatULong(pTabDlg->m_ThreadEntry.tpBasePri + pTabDlg->m_ThreadEntry.tpDeltaPri));
		SetDlgItemText(IDC_TGT_BASEPRIORITY, FormatULong(pTabDlg->m_ThreadEntry.tpBasePri));
	//	SetDlgItemText(IDC_TGT_THREADSTATE, ids(IDS_UNAVAILABLE));
	//	SetDlgItemText(IDC_TGT_WAITREASON, ids(IDS_UNAVAILABLE));
	//	SetDlgItemText(IDC_TGT_STARTADDRESS, ids(IDS_UNAVAILABLE));
	//	SetDlgItemText(IDC_TGT_CPUTIME, FormatTimeAdd(pTabDlg->m_ftThdKernelTime, pTabDlg->m_ftThdUserTime));
	//	SetDlgItemText(IDC_TGT_USERTIME, FormatTime(pTabDlg->m_ftThdUserTime));
	//	SetDlgItemText(IDC_TGT_PRIVILEGEDTIME, FormatTime(pTabDlg->m_ftThdKernelTime));
	//	SetDlgItemText(IDC_TGT_ELAPSEDTIME, FormatTimeDiff(pTabDlg->m_stThdSnapshotTime, pTabDlg->m_ftThdCreationTime));
	//	SetDlgItemText(IDC_TGT_CONTEXTSWITCHES, ids(IDS_UNAVAILABLE));
	//	SetDlgItemText(IDC_TGT_USERPC, ids(IDS_UNAVAILABLE));
		GetDlgItem(IDC_TGT_THREADSTATELABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TGT_THREADSTATE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TGT_WAITREASONLABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TGT_WAITREASON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TGT_STARTADDRESSLABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TGT_STARTADDRESS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TGT_CPUTIMELABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TGT_CPUTIME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TGT_USERTIMELABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TGT_USERTIME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TGT_PRIVILEGEDTIMELABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TGT_PRIVILEGEDTIME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TGT_ELAPSEDTIMELABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TGT_ELAPSEDTIME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TGT_CONTEXTSWITCHESLABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TGT_CONTEXTSWITCHES)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TGT_USERPCLABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TGT_USERPC)->ShowWindow(SW_HIDE);
	}
}
