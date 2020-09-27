// winfdlg.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "msglog.h"
#include "msgview.h"
#include "msgdoc.h"
#include "findtool.h"
#include "filtrdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFiltersWindowDlgTab dialog

CFiltersWindowDlgTab::~CFiltersWindowDlgTab()
{
}

BEGIN_MESSAGE_MAP(CFiltersWindowDlgTab, CDlgTab)
	//{{AFX_MSG_MAP(CFiltersWindowDlgTab)
	ON_BN_CLICKED(IDC_FWT_ALLWINDOWS, OnClickedWinFiltersAllWindows)
	ON_BN_CLICKED(IDC_FWT_HIDE, OnClickedHide)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFiltersWindowDlgTab message handlers

BOOL CFiltersWindowDlgTab::OnInitDialog()
{
	CDlgTab::OnInitDialog();

	CFont* pFont = GetStdFont(font_Normal);
	ASSERT(pFont);
	HFONT hfont = (HFONT)pFont->m_hObject;

	SendDlgItemMessage(IDC_FWT_SOBHANDLE, WM_SETFONT, (WPARAM)hfont, FALSE);
	SendDlgItemMessage(IDC_FWT_SOBTEXT, WM_SETFONT, (WPARAM)hfont, FALSE);
	SendDlgItemMessage(IDC_FWT_SOBCLASS, WM_SETFONT, (WPARAM)hfont, FALSE);
	SendDlgItemMessage(IDC_FWT_SOBSTYLE, WM_SETFONT, (WPARAM)hfont, FALSE);
	SendDlgItemMessage(IDC_FWT_SOBRECT, WM_SETFONT, (WPARAM)hfont, FALSE);
	SendDlgItemMessage(IDC_FWT_SOBTHREAD, WM_SETFONT, (WPARAM)hfont, FALSE);
	SendDlgItemMessage(IDC_FWT_SOBPROCESS, WM_SETFONT, (WPARAM)hfont, FALSE);

// begin special font mucking-around
	if (_getmbcp() != 0)
	{
		pFont = GetStdFont(font_Fixed);
		ASSERT(pFont);
		hfont = (HFONT)pFont->m_hObject;

		SendDlgItemMessage(IDC_FWT_SOBTEXT, WM_SETFONT, (WPARAM)hfont, FALSE);
		SendDlgItemMessage(IDC_FWT_SOBCLASS, WM_SETFONT, (WPARAM)hfont, FALSE);
	}
// end special font mucking-around

	m_pDoc = ((CFiltersTabbedDialog*)GetParent())->m_pDoc;

	if (m_pDoc->m_fIncludeParent)
	{
		CheckDlgButton(IDC_FWT_PARENT, 1);
	}

	if (m_pDoc->m_fIncludeChildren)
	{
		CheckDlgButton(IDC_FWT_CHILDREN, 1);
	}

	if (m_pDoc->m_fIncludeThread)
	{
		CheckDlgButton(IDC_FWT_SAMETHREAD, 1);
	}

	if (m_pDoc->m_fIncludeProcess)
	{
		CheckDlgButton(IDC_FWT_SAMEPROCESS, 1);
	}

	m_FindToolIcon.SubclassDlgItem(IDC_FWT_FINDTOOL, this);
	m_FindToolIcon.Initialize(this);

	m_nObjectType = m_pDoc->m_nObjectType;

	if (m_nObjectType == OT_WINDOW)
	{
		m_hwndSelected = m_pDoc->m_hwndToSpyOn;
	}
	else
	{
		m_hwndSelected = (HWND)NULL;
	}

	UpdateSOBFields();

	return TRUE;  // return TRUE unless you set the focus to a control
}



void CFiltersWindowDlgTab::UpdateSOBFields()
{
	BOOL fEnable = FALSE;

	ShowSOBLabels(m_nObjectType);

	switch (m_nObjectType)
	{
		case OT_WINDOW:
			SetSelectedWindow(m_hwndSelected);
			fEnable = TRUE;
			break;

		case OT_THREAD:
			SetSelectedThread(m_pDoc->m_tidToSpyOn);
			break;

		case OT_PROCESS:
			SetSelectedProcess(m_pDoc->m_pidToSpyOn);
			break;

		case OT_NONE:
			break;

		default:
			ASSERT(FALSE);
			break;
	}

	if (m_pDoc->m_fAllWindows)
	{
		CheckDlgButton(IDC_FWT_ALLWINDOWS, 1);
		GraySOB(TRUE);
		fEnable = FALSE;
	}

	EnableCheckboxes(fEnable);
}



void CFiltersWindowDlgTab::ShowSOBLabels(int nObjectType)
{
	switch (nObjectType)
	{
		case OT_WINDOW:
			GetDlgItem(IDC_FWT_SOBNOSELECTIONLABEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_FWT_SOBSTYLE)->ShowWindow(SW_SHOW);

			SetDlgItemText(IDC_FWT_SOBHANDLELABEL, ids(IDS_WINDOWLABEL));
			SetDlgItemText(IDC_FWT_SOBTEXTLABEL, ids(IDS_TEXTLABEL));
			SetDlgItemText(IDC_FWT_SOBCLASSLABEL, ids(IDS_CLASSLABEL));

			GetDlgItem(IDC_FWT_SOBHANDLELABEL)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_FWT_SOBTEXTLABEL)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_FWT_SOBCLASSLABEL)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_FWT_SOBSTYLELABEL)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_FWT_SOBRECTLABEL)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_FWT_SOBTHREADLABEL)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_FWT_SOBPROCESSLABEL)->ShowWindow(SW_SHOW);

			break;

		case OT_THREAD:
			GetDlgItem(IDC_FWT_SOBNOSELECTIONLABEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_FWT_SOBSTYLE)->ShowWindow(SW_SHOW);

			SetDlgItemText(IDC_FWT_SOBHANDLELABEL, ids(IDS_THREADIDLABEL));
			SetDlgItemText(IDC_FWT_SOBTEXTLABEL, ids(IDS_PROCESSIDLABEL));
			SetDlgItemText(IDC_FWT_SOBCLASSLABEL, ids(IDS_EXECUTABLELABEL));

			GetDlgItem(IDC_FWT_SOBHANDLELABEL)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_FWT_SOBTEXTLABEL)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_FWT_SOBCLASSLABEL)->ShowWindow(SW_SHOW);

			GetDlgItem(IDC_FWT_SOBSTYLELABEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_FWT_SOBRECTLABEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_FWT_SOBTHREADLABEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_FWT_SOBPROCESSLABEL)->ShowWindow(SW_HIDE);

			break;

		case OT_PROCESS:
			GetDlgItem(IDC_FWT_SOBNOSELECTIONLABEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_FWT_SOBSTYLE)->ShowWindow(SW_SHOW);

			SetDlgItemText(IDC_FWT_SOBHANDLELABEL, ids(IDS_PROCESSIDLABEL));
			SetDlgItemText(IDC_FWT_SOBTEXTLABEL, ids(IDS_EXECUTABLELABEL));

			GetDlgItem(IDC_FWT_SOBHANDLELABEL)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_FWT_SOBTEXTLABEL)->ShowWindow(SW_SHOW);

			GetDlgItem(IDC_FWT_SOBCLASSLABEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_FWT_SOBSTYLELABEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_FWT_SOBRECTLABEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_FWT_SOBTHREADLABEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_FWT_SOBPROCESSLABEL)->ShowWindow(SW_HIDE);

			break;

		case OT_NONE:
			GetDlgItem(IDC_FWT_SOBHANDLELABEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_FWT_SOBTEXTLABEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_FWT_SOBCLASSLABEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_FWT_SOBSTYLELABEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_FWT_SOBRECTLABEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_FWT_SOBTHREADLABEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_FWT_SOBPROCESSLABEL)->ShowWindow(SW_HIDE);

			GetDlgItem(IDC_FWT_SOBSTYLE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_FWT_SOBNOSELECTIONLABEL)->ShowWindow(SW_SHOW);

			break;

		default:
			ASSERT(FALSE);
			break;
	}
}



//-----------------------------------------------------------------------------
// SetSelectedWindow
//
//
//
// Arguments:
//
//
//-----------------------------------------------------------------------------

void CFiltersWindowDlgTab::SetSelectedWindow(HWND hwndSelected)
{
	CHAR sz[256];
	LPSTR lpszClass;
	int cch;
	RECT rc;

	if (::IsWindow(hwndSelected))
	{
		wsprintf(sz, "%08X", hwndSelected);
		SetDlgItemText(IDC_FWT_SOBHANDLE, sz);

		sz[0] = '"';
		cch = ::GetWindowText(hwndSelected, &sz[1], 256 - 2);
		sz[cch + 1] = '"';
		sz[cch + 2] = 0;
		SetDlgItemText(IDC_FWT_SOBTEXT, sz);

		::GetClassName(hwndSelected, sz, 256);
		if ((lpszClass = GetExpandedClassName(sz)) != NULL)
			_tcscat(sz, lpszClass);
		SetDlgItemText(IDC_FWT_SOBCLASS, sz);

		wsprintf(sz, "%08X", ::GetWindowLong(hwndSelected, GWL_STYLE));
		SetDlgItemText(IDC_FWT_SOBSTYLE, sz);

		::GetWindowRect(hwndSelected, &rc);
		wsprintf(sz, "(%d, %d)-(%d, %d) %dx%d", rc.left, rc.top, rc.right, rc.bottom, rc.right - rc.left, rc.bottom - rc.top);
		SetDlgItemText(IDC_FWT_SOBRECT, sz);

		DWORD pid;
		DWORD tid = GetWindowThreadProcessId(hwndSelected, &pid);
		wsprintf(sz, "%08X", tid);
		SetDlgItemText(IDC_FWT_SOBTHREAD, sz);

		wsprintf(sz, "%08X", pid);
		SetDlgItemText(IDC_FWT_SOBPROCESS, sz);
	}
	else
	{
		//
		// If the handle is NULL, do not even print it out.  If it
		// is not NULL, then it is an hwnd for a destroyed window
		// that the user selected at one time, so we need to display
		// it's value even though it is invalid.
		//
		if (hwndSelected == NULL)
		{
			SetDlgItemText(IDC_FWT_SOBHANDLE, "");
		}
		else
		{
			wsprintf(sz, ids(IDS_INVALID_PARAM), hwndSelected);
			SetDlgItemText(IDC_FWT_SOBHANDLE, sz);
		}

		SetDlgItemText(IDC_FWT_SOBTEXT, "");
		SetDlgItemText(IDC_FWT_SOBCLASS, "");
		SetDlgItemText(IDC_FWT_SOBSTYLE, "");
		SetDlgItemText(IDC_FWT_SOBRECT, "");
		SetDlgItemText(IDC_FWT_SOBTHREAD, "");
		SetDlgItemText(IDC_FWT_SOBPROCESS, "");
	}
}



//-----------------------------------------------------------------------------
// SetSelectedThread
//
//
//
// Arguments:
//
//
//-----------------------------------------------------------------------------

void CFiltersWindowDlgTab::SetSelectedThread(DWORD tid)
{
	CHAR sz[256];
	LPWSTR lpwsz;
	int iIndex = -1;

	if (!theApp.IsChicago())
	{
		// Win32
//		enum CPROCDB::PROCDBERR error;
		WORD wQueryType = PROCDBQ_THREAD;
		CPROCDB ProcessDatabase(wQueryType/*, &error*/);
		CPROCDB::PRFCNTRTHRD PrfCntrThrd;

		if ((iIndex = ProcessDatabase.GetThrdIndexFromId(tid)) != -1)
		{
			ProcessDatabase.GetPrfCntrThrd(iIndex, &PrfCntrThrd);

			wsprintf(sz, "%08X", tid);
			SetDlgItemText(IDC_FWT_SOBHANDLE, sz);

			wsprintf(sz, "%08X", PrfCntrThrd.dwProcessId);
			SetDlgItemText(IDC_FWT_SOBTEXT, sz);

			lpwsz = (LPWSTR)ProcessDatabase.GetThreadName(iIndex);
			::SetDlgItemTextW(m_hWnd, IDC_FWT_SOBCLASS, lpwsz);
		}
		else
		{
			wsprintf(sz, ids(IDS_INVALID_PARAM), tid);
			SetDlgItemText(IDC_FWT_SOBHANDLE, sz);

			SetDlgItemText(IDC_FWT_SOBTEXT, "");
			SetDlgItemText(IDC_FWT_SOBCLASS, "");
		}
	}
	else
	{
		// Chicago
		HANDLE hThreadList;
		THREADENTRY32 te32;
		BOOL fFound = FALSE;

		te32.dwSize = sizeof(THREADENTRY32);

		if ((hThreadList = (*theApp.pfnCreateToolhelp32Snapshot)(TH32CS_SNAPTHREAD, 0)) != NULL)
		{
			if ((*theApp.pfnThread32First)(hThreadList, &te32))
			{
				if (te32.th32ThreadID == tid)
				{
					fFound = TRUE;
				}
				else
				{
					while ((*theApp.pfnThread32Next)(hThreadList, &te32))
					{
						if (te32.th32ThreadID == tid)
						{
							fFound = TRUE;
							break;
						}
					}
				}

				if (fFound)
				{
					wsprintf(sz, "%08X", tid);
					SetDlgItemText(IDC_FWT_SOBHANDLE, sz);

					wsprintf(sz, "%08X", te32.th32OwnerProcessID);
					SetDlgItemText(IDC_FWT_SOBTEXT, sz);

					HANDLE hProcessList;
					PROCESSENTRY32 pe32;

					pe32.dwSize = sizeof(PROCESSENTRY32);

					if ((hProcessList = (*theApp.pfnCreateToolhelp32Snapshot)(TH32CS_SNAPPROCESS, 0)) != NULL)
					{
						if ((*theApp.pfnProcess32First)(hProcessList, &pe32))
						{
							if (pe32.th32ProcessID == te32.th32OwnerProcessID)
							{
								if (pe32.dwSize == sizeof(PROCESSENTRY32))
								{
									char szFileName[_MAX_FNAME];
									_splitpath(pe32.szExeFile, NULL, NULL, szFileName, NULL);
									SetDlgItemText(IDC_FWT_SOBCLASS, szFileName);
								}
								else
									SetDlgItemText(IDC_FWT_SOBCLASS, "");
							}
							else
							{
								while ((*theApp.pfnProcess32Next)(hProcessList, &pe32))
								{
									if (pe32.th32ProcessID == te32.th32OwnerProcessID)
									{
										if (pe32.dwSize == sizeof(PROCESSENTRY32))
										{
											char szFileName[_MAX_FNAME];
											_splitpath(pe32.szExeFile, NULL, NULL, szFileName, NULL);
											SetDlgItemText(IDC_FWT_SOBCLASS, szFileName);
										}
										else
											SetDlgItemText(IDC_FWT_SOBCLASS, "");
										break;
									}
								}
							}
						}
					}
				}
				else
				{
					wsprintf(sz, ids(IDS_INVALID_PARAM), tid);
					SetDlgItemText(IDC_FWT_SOBHANDLE, sz);

					SetDlgItemText(IDC_FWT_SOBTEXT, "");
					SetDlgItemText(IDC_FWT_SOBCLASS, "");
				}
			}

			CloseHandle(hThreadList);
		}
	}

	SetDlgItemText(IDC_FWT_SOBSTYLE, "");
	SetDlgItemText(IDC_FWT_SOBRECT, "");
	SetDlgItemText(IDC_FWT_SOBTHREAD, "");
	SetDlgItemText(IDC_FWT_SOBPROCESS, "");
}



//-----------------------------------------------------------------------------
// SetSelectedProcess
//
//
//
// Arguments:
//
//
//-----------------------------------------------------------------------------

void CFiltersWindowDlgTab::SetSelectedProcess(DWORD pid)
{
	CHAR sz[260];
	LPWSTR lpwsz;
	int iIndex = -1;

	if (!theApp.IsChicago())
	{
		// Win32
//		enum CPROCDB::PROCDBERR error;
		WORD wQueryType = PROCDBQ_PROCESS;
		CPROCDB ProcessDatabase(wQueryType/*, &error*/);
		CPROCDB::PRFCNTRPROC PrfCntrProc;

		if ((iIndex = ProcessDatabase.GetProcIndexFromId(pid)) != -1)
		{
			ProcessDatabase.GetPrfCntrProc(iIndex, &PrfCntrProc);

			wsprintf(sz, "%08X", pid);
			SetDlgItemText(IDC_FWT_SOBHANDLE, sz);

			lpwsz = (LPWSTR)ProcessDatabase.GetProcessName(iIndex);
			::SetDlgItemTextW(m_hWnd, IDC_FWT_SOBTEXT, lpwsz);
		}
		else
		{
			wsprintf(sz, ids(IDS_INVALID_PARAM), pid);
			SetDlgItemText(IDC_FWT_SOBHANDLE, sz);

			SetDlgItemText(IDC_FWT_SOBTEXT, "");
		}
	}
	else
	{
		// Chicago
		HANDLE hProcessList;
		PROCESSENTRY32 pe32;
		BOOL fFound = FALSE;

		pe32.dwSize = sizeof(PROCESSENTRY32);

		if ((hProcessList = (*theApp.pfnCreateToolhelp32Snapshot)(TH32CS_SNAPPROCESS, 0)) != NULL)
		{
			if ((*theApp.pfnProcess32First)(hProcessList, &pe32))
			{
				if (pe32.th32ProcessID == pid)
				{
					fFound = TRUE;
				}
				else
				{
					while ((*theApp.pfnProcess32Next)(hProcessList, &pe32))
					{
						if (pe32.th32ProcessID == pid)
						{
							fFound = TRUE;
							break;
						}
					}
				}

				if (fFound)
				{
					wsprintf(sz, "%08X", pid);
					SetDlgItemText(IDC_FWT_SOBHANDLE, sz);

					if (pe32.dwSize == sizeof(PROCESSENTRY32))
					{
						char szFileName[_MAX_FNAME];
						_splitpath(pe32.szExeFile, NULL, NULL, szFileName, NULL);
						SetDlgItemText(IDC_FWT_SOBTEXT, szFileName);
					}
					else
						SetDlgItemText(IDC_FWT_SOBTEXT, "");
				}
				else
				{
					wsprintf(sz, ids(IDS_INVALID_PARAM), pid);
					SetDlgItemText(IDC_FWT_SOBHANDLE, sz);

					SetDlgItemText(IDC_FWT_SOBTEXT, "");
				}
			}

			CloseHandle(hProcessList);
		}
	}

	SetDlgItemText(IDC_FWT_SOBCLASS, "");
	SetDlgItemText(IDC_FWT_SOBSTYLE, "");
	SetDlgItemText(IDC_FWT_SOBRECT, "");
	SetDlgItemText(IDC_FWT_SOBTHREAD, "");
	SetDlgItemText(IDC_FWT_SOBPROCESS, "");
}



void CFiltersWindowDlgTab::BeginFindWindowTracking()
{
	SetSelectedWindow((HWND)NULL);
	ShowSOBLabels(OT_WINDOW);
	GraySOB(FALSE);
	CheckDlgButton(IDC_FWT_ALLWINDOWS, 0);
	EnableCheckboxes(TRUE);
	m_hwndFound = NULL;
}



void CFiltersWindowDlgTab::UpdateFindWindowTracking(HWND hwndFound)
{
	SetSelectedWindow(hwndFound);
	m_hwndFound = hwndFound;
}



void CFiltersWindowDlgTab::EndFindWindowTracking()
{
	if (m_hwndFound)
	{
		m_hwndSelected = m_hwndFound;
		m_nObjectType = OT_WINDOW;
	}
	else
	{
		UpdateSOBFields();
	}
}



void CFiltersWindowDlgTab::OnClickedWinFiltersAllWindows()
{
	GraySOB(IsDlgButtonChecked(IDC_FWT_ALLWINDOWS));

	if (!IsDlgButtonChecked(IDC_FWT_ALLWINDOWS) && m_nObjectType == OT_WINDOW)
	{
		EnableCheckboxes(TRUE);
	}
	else
	{
		EnableCheckboxes(FALSE);
	}
}



void CFiltersWindowDlgTab::EnableCheckboxes(BOOL fEnable)
{
	GetDlgItem(IDC_FWT_PARENT)->EnableWindow(fEnable);
	GetDlgItem(IDC_FWT_CHILDREN)->EnableWindow(fEnable);
	GetDlgItem(IDC_FWT_SAMETHREAD)->EnableWindow(fEnable);
	GetDlgItem(IDC_FWT_SAMEPROCESS)->EnableWindow(fEnable);
}



void CFiltersWindowDlgTab::GraySOB(BOOL fGray)
{
	GetDlgItem(IDC_FWT_SOBHANDLE)->EnableWindow(!fGray);
	GetDlgItem(IDC_FWT_SOBTEXT)->EnableWindow(!fGray);
	GetDlgItem(IDC_FWT_SOBCLASS)->EnableWindow(!fGray);
	GetDlgItem(IDC_FWT_SOBSTYLE)->EnableWindow(!fGray);
	GetDlgItem(IDC_FWT_SOBRECT)->EnableWindow(!fGray);
	GetDlgItem(IDC_FWT_SOBTHREAD)->EnableWindow(!fGray);
	GetDlgItem(IDC_FWT_SOBPROCESS)->EnableWindow(!fGray);
	GetDlgItem(IDC_FWT_SOBNOSELECTIONLABEL)->EnableWindow(!fGray);
}



void CFiltersWindowDlgTab::OnClickedHide()
{
	if (IsDlgButtonChecked(IDC_FWT_HIDE))
	{
		theApp.m_pMainWnd->ShowWindow(SW_HIDE);
	}
	else
	{
		theApp.m_pMainWnd->ShowWindow(SW_SHOWNA);
	}
}


BOOL CFiltersWindowDlgTab::ValidateTab()
{
	if (m_hwndSelected)
	{
		if (!IsDlgButtonChecked(IDC_FWT_ALLWINDOWS) && !IsSpyableWindow(m_hwndSelected))
		{
			SpyMessageBox(IDS_WND_NOT_SPYABLE);
			GotoDlgCtrl(GetDlgItem(IDC_FWT_HIDE));
			return(FALSE);
		}
	}

	return(TRUE);
}


void CFiltersWindowDlgTab::CommitTab()
{
	BOOL fSelectedSave = m_pDoc->IsSomethingSelected();

	if (m_hwndSelected)
	{
		m_pDoc->SetWindowToSpyOn(m_hwndSelected);
	}

	m_pDoc->m_fAllWindows = IsDlgButtonChecked(IDC_FWT_ALLWINDOWS);
	m_pDoc->m_fIncludeParent = IsDlgButtonChecked(IDC_FWT_PARENT);
	m_pDoc->m_fIncludeChildren = IsDlgButtonChecked(IDC_FWT_CHILDREN);
	m_pDoc->m_fIncludeThread = IsDlgButtonChecked(IDC_FWT_SAMETHREAD);
	m_pDoc->m_fIncludeProcess = IsDlgButtonChecked(IDC_FWT_SAMEPROCESS);

	m_pDoc->UpdateTitle();

	//
	// If they have selected "Save as Default", copy the
	// current settings to the default settings.
	//
	if (IsDlgButtonChecked(IDC_FWT_SAVE))
	{
		CMsgDoc::m_fIncludeParentDef = m_pDoc->m_fIncludeParent;
		CMsgDoc::m_fIncludeChildrenDef = m_pDoc->m_fIncludeChildren;
		CMsgDoc::m_fIncludeThreadDef = m_pDoc->m_fIncludeThread;
		CMsgDoc::m_fIncludeProcessDef = m_pDoc->m_fIncludeProcess;
	}

	//
	// Restore the Spy app if needed.
	//
	if (IsDlgButtonChecked(IDC_FWT_HIDE))
	{
		theApp.m_pMainWnd->ShowWindow(SW_SHOWNA);
		CheckDlgButton(IDC_FWT_HIDE, 0);
	}

	if (m_pDoc->IsSomethingSelected())
	{
		//
		// If there was nothing selected before, and they selected
		// something (this includes All Windows) then we want to turn
		// on the logging now.  If there was already something
		// selected before, and they change the selection, we just
		// leave the Logging state as it is.
		//
		if (!fSelectedSave)
		{
			if (!m_pDoc->IsLogging())
			{
				m_pDoc->OnMessagesStartStop();
			}
		}
	}
	else
	{
		//
		// Else if they have nothing selected then we want to be sure
		// logging is stopped.  This could happen if they had nothing
		// selected originally, then they turned on All Windows, then
		// they went back again and turned off All Windows.  If we don't
		// explicitly stop the logging here, then logging will still
		// be enabled with some overhead involved in the hook (although
		// nothing will match the filters and actually be displayed).
		//
		if (m_pDoc->IsLogging())
		{
			m_pDoc->OnMessagesStartStop();
		}
	}

	CDlgTab::CommitTab();
}



void CFiltersWindowDlgTab::CancelTab()
{
	//
	// Restore the Spy app if needed.
	//
	if (IsDlgButtonChecked(IDC_FWT_HIDE))
	{
		theApp.m_pMainWnd->ShowWindow(SW_SHOWNA);
		CheckDlgButton(IDC_FWT_HIDE, 0);
	}

	CDlgTab::CancelTab();
}
