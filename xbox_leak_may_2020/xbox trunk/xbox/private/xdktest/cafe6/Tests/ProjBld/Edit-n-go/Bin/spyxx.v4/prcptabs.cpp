// prcptabs.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#pragma message("Remove local copy of TLHELP32.H when official version available.")

#include "hotlinkc.h"
#include "proptab.h"
#include "prcptabs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CProcessGeneralPropTab

CProcessGeneralPropTab::~CProcessGeneralPropTab()
{
}

INT CProcessGeneralPropTab::m_aControlIDs[] =
{
	IDC_PGT_PROCESSID,
	IDC_PGT_EXECUTABLE,
	IDC_PGT_CPUTIME,
	IDC_PGT_USERTIME,
	IDC_PGT_PRIVILEGEDTIME,
	IDC_PGT_ELAPSEDTIME,
	IDC_PGT_BASEPRIORITY,
	IDC_PGT_NUMTHREADS,
	0
};

BEGIN_MESSAGE_MAP(CProcessGeneralPropTab, CPropertyTab)
	//{{AFX_MSG_MAP(CProcessGeneralPropTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessGeneralPropTab message handlers

BOOL CProcessGeneralPropTab::OnInitDialog()
{
	CPropertyTab::OnInitDialog();

// begin special font mucking-around
	if (_getmbcp() != 0)
	{
		CFont* pFont = GetStdFont(font_Fixed);
		ASSERT(pFont);
		HFONT hfont = (HFONT)pFont->m_hObject;

		SendDlgItemMessage(IDC_PGT_EXECUTABLE, WM_SETFONT, (WPARAM)hfont, FALSE);
	}
// end special font mucking-around

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CProcessGeneralPropTab::UpdateFields()
{
	CPropertyInspector* pTabDlg = (CPropertyInspector*)GetParent();

	if (!theApp.IsChicago())
	{
		// Win32
		SetDlgItemText(IDC_PGT_PROCESSID, FormatHex((DWORD)pTabDlg->m_ProcessInfo.dwProcessId));
		SetDlgItemText(IDC_PGT_EXECUTABLE, pTabDlg->m_strExecutable);
		SetDlgItemText(IDC_PGT_CPUTIME, FormatTime(pTabDlg->m_ProcessInfo.liPctCPUTime));
		SetDlgItemText(IDC_PGT_USERTIME, FormatTime(pTabDlg->m_ProcessInfo.liPctUserTime));
		SetDlgItemText(IDC_PGT_PRIVILEGEDTIME, FormatTime(pTabDlg->m_ProcessInfo.liPctPrivTime));
		SetDlgItemText(IDC_PGT_ELAPSEDTIME, FormatTimeDiff(pTabDlg->m_ProcessInfo.liElapsedTime, pTabDlg->m_ProcessInfo.PerfTime));
		SetDlgItemText(IDC_PGT_BASEPRIORITY, FormatULong(pTabDlg->m_ProcessInfo.dwPriorityBase));
		SetDlgItemText(IDC_PGT_NUMTHREADS, FormatULong(pTabDlg->m_ProcessInfo.dwThrdCnt));
	}
	else
	{
		// Chicago
		SetDlgItemText(IDC_PGT_PROCESSID, FormatHex((DWORD)pTabDlg->m_ProcessEntry.th32ProcessID));
		SetDlgItemText(IDC_PGT_EXECUTABLE, pTabDlg->m_strExecutable);
	//	SetDlgItemText(IDC_PGT_CPUTIME, FormatTimeAdd(pTabDlg->m_ftPrcKernelTime, pTabDlg->m_ftPrcUserTime));
	//	SetDlgItemText(IDC_PGT_USERTIME, FormatTime(pTabDlg->m_ftPrcUserTime));
	//	SetDlgItemText(IDC_PGT_PRIVILEGEDTIME, FormatTime(pTabDlg->m_ftPrcKernelTime));
	//	SetDlgItemText(IDC_PGT_ELAPSEDTIME, FormatTimeDiff(pTabDlg->m_stPrcSnapshotTime, pTabDlg->m_ftPrcCreationTime));
		GetDlgItem(IDC_PGT_CPUTIMELABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PGT_CPUTIME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PGT_USERTIMELABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PGT_USERTIME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PGT_PRIVILEGEDTIMELABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PGT_PRIVILEGEDTIME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PGT_ELAPSEDTIMELABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PGT_ELAPSEDTIME)->ShowWindow(SW_HIDE);
		SetDlgItemText(IDC_PGT_BASEPRIORITY, FormatULong(pTabDlg->m_ProcessEntry.pcPriClassBase));
		SetDlgItemText(IDC_PGT_NUMTHREADS, FormatULong(pTabDlg->m_ProcessEntry.cntThreads));
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CProcessMemoryPropTab

CProcessMemoryPropTab::~CProcessMemoryPropTab()
{
}

INT CProcessMemoryPropTab::m_aControlIDs[] =
{
	IDC_PMT_VIRTUAL,
	IDC_PMT_PEAKVIRTUAL,
	IDC_PMT_WORKINGSET,
	IDC_PMT_PEAKWORKINGSET,
	IDC_PMT_PAGEDPOOL,
	IDC_PMT_NONPAGEDPOOL,
	IDC_PMT_PRIVATEBYTES,
	IDC_PMT_FREEBYTES,
	IDC_PMT_RESERVEDBYTES,
	IDC_PMT_FREEIMAGEBYTES,
	IDC_PMT_RESERVEDIMAGEBYTES,
	0
};

BEGIN_MESSAGE_MAP(CProcessMemoryPropTab, CPropertyTab)
	//{{AFX_MSG_MAP(CProcessMemoryPropTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessMemoryPropTab message handlers

void CProcessMemoryPropTab::UpdateFields()
{
	CPropertyInspector* pTabDlg = (CPropertyInspector*)GetParent();

	if (!theApp.IsChicago())
	{
		// Win32
		SetDlgItemText(IDC_PMT_VIRTUAL, FormatKb(pTabDlg->m_ProcessInfo.dwVirtBytes));
		SetDlgItemText(IDC_PMT_PEAKVIRTUAL, FormatKb(pTabDlg->m_ProcessInfo.dwVirtBytesPeak));
		SetDlgItemText(IDC_PMT_WORKINGSET, FormatKb(pTabDlg->m_ProcessInfo.dwWorkingSet));
		SetDlgItemText(IDC_PMT_PEAKWORKINGSET, FormatKb(pTabDlg->m_ProcessInfo.dwWorkingSetPeak));
		SetDlgItemText(IDC_PMT_PAGEDPOOL, FormatKb(pTabDlg->m_ProcessInfo.dwPoolPagedBytes));
		SetDlgItemText(IDC_PMT_NONPAGEDPOOL, FormatKb(pTabDlg->m_ProcessInfo.dwPoolNonPagedBytes));
		SetDlgItemText(IDC_PMT_PRIVATEBYTES, FormatKb(pTabDlg->m_ProcessInfo.dwPrivateBytes));
		SetDlgItemText(IDC_PMT_FREEBYTES, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwBytesFree));
		SetDlgItemText(IDC_PMT_RESERVEDBYTES, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwBytesReserved));
		SetDlgItemText(IDC_PMT_FREEIMAGEBYTES, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwBytesImgFree));
		SetDlgItemText(IDC_PMT_RESERVEDIMAGEBYTES, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwBytesImgReserved));
	}
	else
	{
		// Chicago
		SetDlgItemText(IDC_PMT_VIRTUAL, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PMT_PEAKVIRTUAL, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PMT_WORKINGSET, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PMT_PEAKWORKINGSET, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PMT_PAGEDPOOL, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PMT_NONPAGEDPOOL, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PMT_PRIVATEBYTES, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PMT_FREEBYTES, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PMT_RESERVEDBYTES, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PMT_FREEIMAGEBYTES, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PMT_RESERVEDIMAGEBYTES, ids(IDS_UNAVAILABLE));
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CProcessPageFilePropTab

CProcessPageFilePropTab::~CProcessPageFilePropTab()
{
}

INT CProcessPageFilePropTab::m_aControlIDs[] =
{
	IDC_PPT_PAGEFILE,
	IDC_PPT_PEAKPAGEFILE,
	IDC_PPT_PAGEFAULTS,
	0
};

BEGIN_MESSAGE_MAP(CProcessPageFilePropTab, CPropertyTab)
	//{{AFX_MSG_MAP(CProcessPageFilePropTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessPageFilePropTab message handlers

void CProcessPageFilePropTab::UpdateFields()
{
	CPropertyInspector* pTabDlg = (CPropertyInspector*)GetParent();

	if (!theApp.IsChicago())
	{
		// Win32
		SetDlgItemText(IDC_PPT_PAGEFILE, FormatKb(pTabDlg->m_ProcessInfo.dwPageFilesBytes));
		SetDlgItemText(IDC_PPT_PEAKPAGEFILE, FormatKb(pTabDlg->m_ProcessInfo.dwPageFilesBytesPeak));
		SetDlgItemText(IDC_PPT_PAGEFAULTS, FormatULong(pTabDlg->m_ProcessInfo.dwPageFltsPerSec));
	}
	else
	{
		// Chicago
		SetDlgItemText(IDC_PPT_PAGEFILE, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PPT_PEAKPAGEFILE, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PPT_PAGEFAULTS, ids(IDS_UNAVAILABLE));
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CProcessFileIOPropTab

CProcessFileIOPropTab::~CProcessFileIOPropTab()
{
}

INT CProcessFileIOPropTab::m_aControlIDs[] =
{
	IDC_PFT_CONTROLBYTES,
	IDC_PFT_CONTROLOPS,
	IDC_PFT_READBYTES,
	IDC_PFT_READOPS,
	IDC_PFT_WRITEBYTES,
	IDC_PFT_WRITEOPS,
	0
};

BEGIN_MESSAGE_MAP(CProcessFileIOPropTab, CPropertyTab)
	//{{AFX_MSG_MAP(CProcessFileIOPropTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessFileIOPropTab message handlers

void CProcessFileIOPropTab::UpdateFields()
{
	CPropertyInspector* pTabDlg = (CPropertyInspector*)GetParent();

	if (!theApp.IsChicago())
	{
		// Win32
		SetDlgItemText(IDC_PFT_CONTROLBYTES, FormatKb(pTabDlg->m_ProcessInfo.liFileCtrlBytesPerSec));
		SetDlgItemText(IDC_PFT_CONTROLOPS, FormatULong(pTabDlg->m_ProcessInfo.dwFileCtrlOpsPerSec));
		SetDlgItemText(IDC_PFT_READBYTES, FormatKb(pTabDlg->m_ProcessInfo.liFileReadBytesPerSec));
		SetDlgItemText(IDC_PFT_READOPS, FormatULong(pTabDlg->m_ProcessInfo.dwFileReadOpsPerSec));
		SetDlgItemText(IDC_PFT_WRITEBYTES, FormatKb(pTabDlg->m_ProcessInfo.liFileWriteBytesPerSec));
		SetDlgItemText(IDC_PFT_WRITEOPS, FormatULong(pTabDlg->m_ProcessInfo.dwFileWriteOpsPerSec));
	}
	else
	{
		// Chicago
		SetDlgItemText(IDC_PFT_CONTROLBYTES, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PFT_CONTROLOPS, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PFT_READBYTES, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PFT_READOPS, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PFT_WRITEBYTES, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PFT_WRITEOPS, ids(IDS_UNAVAILABLE));
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CProcessSpacePropTab

CProcessSpacePropTab::~CProcessSpacePropTab()
{
}

INT CProcessSpacePropTab::m_aControlIDs[] =
{
	IDC_PST_SELECTSPACETYPE,
	IDC_PST_BYTES_EXEC,
	IDC_PST_BYTES_EXECRO,
	IDC_PST_BYTES_EXECRW,
	IDC_PST_BYTES_EXECWC,
	IDC_PST_BYTES_NA,
	IDC_PST_BYTES_RO,
	IDC_PST_BYTES_RW,
	IDC_PST_BYTES_WC,
	0
};

BEGIN_MESSAGE_MAP(CProcessSpacePropTab, CPropertyTab)
	//{{AFX_MSG_MAP(CProcessSpacePropTab)
	ON_CBN_SELCHANGE(IDC_PST_SELECTSPACETYPE, OnSpaceTypeChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessSpacePropTab message handlers

void CProcessSpacePropTab::UpdateFields()
{
	CPropertyInspector* pTabDlg = (CPropertyInspector*)GetParent();

	CComboBox *pCombo = (CComboBox *)GetDlgItem(IDC_PST_SELECTSPACETYPE);

	pCombo->ResetContent();
	pCombo->AddString(ids(IDS_IMAGE));
	pCombo->AddString(ids(IDS_MAPPED));
	pCombo->AddString(ids(IDS_RESERVED));
	pCombo->AddString(ids(IDS_UNASSIGNED));

	pCombo->SetCurSel(0);

	if (!theApp.IsChicago())
	{
		// Win32
		SetDlgItemText(IDC_PST_BYTES_EXEC,   FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwImgSpE));
		SetDlgItemText(IDC_PST_BYTES_EXECRO, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwImgSpER));
		SetDlgItemText(IDC_PST_BYTES_EXECRW, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwImgSpERW));
		SetDlgItemText(IDC_PST_BYTES_EXECWC, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwImgSpEWC));
		SetDlgItemText(IDC_PST_BYTES_NA,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwImgSpNA));
		SetDlgItemText(IDC_PST_BYTES_RO,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwImgSpR));
		SetDlgItemText(IDC_PST_BYTES_RW,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwImgSpRW));
		SetDlgItemText(IDC_PST_BYTES_WC,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwImgSpWC));
	}
	else
	{
		// Chicago
		SetDlgItemText(IDC_PST_BYTES_EXEC,   ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PST_BYTES_EXECRO, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PST_BYTES_EXECRW, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PST_BYTES_EXECWC, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PST_BYTES_NA,     ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PST_BYTES_RO,     ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PST_BYTES_RW,     ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PST_BYTES_WC,     ids(IDS_UNAVAILABLE));
	}
}

void CProcessSpacePropTab::OnSpaceTypeChange()
{
	CPropertyInspector* pTabDlg = (CPropertyInspector*)GetParent();

	CComboBox *pCombo = (CComboBox *)GetDlgItem(IDC_PST_SELECTSPACETYPE);

	int iSel = pCombo->GetCurSel();

	if (!theApp.IsChicago())
	{
		// Win32
		switch (iSel)
		{
			case (IDS_IMAGE - IDS_IMAGE):
				SetDlgItemText(IDC_PST_BYTES_EXEC,   FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwImgSpE));
				SetDlgItemText(IDC_PST_BYTES_EXECRO, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwImgSpER));
				SetDlgItemText(IDC_PST_BYTES_EXECRW, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwImgSpERW));
				SetDlgItemText(IDC_PST_BYTES_EXECWC, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwImgSpEWC));
				SetDlgItemText(IDC_PST_BYTES_NA,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwImgSpNA));
				SetDlgItemText(IDC_PST_BYTES_RO,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwImgSpR));
				SetDlgItemText(IDC_PST_BYTES_RW,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwImgSpRW));
				SetDlgItemText(IDC_PST_BYTES_WC,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwImgSpWC));
				break;

			case (IDS_MAPPED - IDS_IMAGE):
				SetDlgItemText(IDC_PST_BYTES_EXEC,   FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwMapSpE));
				SetDlgItemText(IDC_PST_BYTES_EXECRO, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwMapSpER));
				SetDlgItemText(IDC_PST_BYTES_EXECRW, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwMapSpERW));
				SetDlgItemText(IDC_PST_BYTES_EXECWC, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwMapSpEWC));
				SetDlgItemText(IDC_PST_BYTES_NA,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwMapSpNA));
				SetDlgItemText(IDC_PST_BYTES_RO,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwMapSpR));
				SetDlgItemText(IDC_PST_BYTES_RW,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwMapSpRW));
				SetDlgItemText(IDC_PST_BYTES_WC,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwMapSpWC));
				break;

			case (IDS_RESERVED - IDS_IMAGE):
				SetDlgItemText(IDC_PST_BYTES_EXEC,   FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwResSpE));
				SetDlgItemText(IDC_PST_BYTES_EXECRO, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwResSpER));
				SetDlgItemText(IDC_PST_BYTES_EXECRW, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwResSpERW));
				SetDlgItemText(IDC_PST_BYTES_EXECWC, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwResSpEWC));
				SetDlgItemText(IDC_PST_BYTES_NA,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwResSpNA));
				SetDlgItemText(IDC_PST_BYTES_RO,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwResSpR));
				SetDlgItemText(IDC_PST_BYTES_RW,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwResSpRW));
				SetDlgItemText(IDC_PST_BYTES_WC,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwResSpWC));
				break;

			case (IDS_UNASSIGNED - IDS_IMAGE):
				SetDlgItemText(IDC_PST_BYTES_EXEC,   FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwUnassSpE));
				SetDlgItemText(IDC_PST_BYTES_EXECRO, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwUnassSpER));
				SetDlgItemText(IDC_PST_BYTES_EXECRW, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwUnassSpERW));
				SetDlgItemText(IDC_PST_BYTES_EXECWC, FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwUnassSpEWC));
				SetDlgItemText(IDC_PST_BYTES_NA,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwUnassSpNA));
				SetDlgItemText(IDC_PST_BYTES_RO,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwUnassSpR));
				SetDlgItemText(IDC_PST_BYTES_RW,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwUnassSpRW));
				SetDlgItemText(IDC_PST_BYTES_WC,     FormatKb(pTabDlg->m_ProcessAddrSpcInfo.dwUnassSpWC));
				break;
		}
	}
	else
	{
		// Chicago
		SetDlgItemText(IDC_PST_BYTES_EXEC,   ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PST_BYTES_EXECRO, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PST_BYTES_EXECRW, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PST_BYTES_EXECWC, ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PST_BYTES_NA,     ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PST_BYTES_RO,     ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PST_BYTES_RW,     ids(IDS_UNAVAILABLE));
		SetDlgItemText(IDC_PST_BYTES_WC,     ids(IDS_UNAVAILABLE));
	}
}

/////////////////////////////////////////////////////////////////////////////
