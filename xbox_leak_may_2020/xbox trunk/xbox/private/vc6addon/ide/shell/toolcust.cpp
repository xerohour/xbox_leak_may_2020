// toolcust.cpp : implementation of the CCustomizeToolsDialog class
//

#include "stdafx.h"

#include "shell.h"
#include "toolcust.h"
#include "toolexpt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

extern HWND PASCAL _SushiGetSafeOwner(CWnd* pParent);

/////////////////////////////////////////////////////////////////////////////
// CToolsGrid

CToolsGrid::CToolsGrid() : CStringListGridWnd(GRIDLIST_NONEWSELONADD)
{
	EnableSort(FALSE);
	EnableAutoSort(FALSE);
	m_pClipboardTool = NULL;
}

CToolsGrid::~CToolsGrid()
{
	if (m_pClipboardTool != NULL)
		delete m_pClipboardTool;
}

BEGIN_MESSAGE_MAP(CToolsGrid, CStringListGridWnd)
	//{{AFX_MSG_MAP(CToolsGrid)
	ON_BN_CLICKED(ID_GRD_NEW, OnGridNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CToolsGrid::OnGridNew() 
{
	if (GetRowCount() == MAXTOOLIST)
	{
		CString strMsg;
		MsgBox(Error, MsgText(strMsg, IDS_TOOL_MAX_REACHED, MAXTOOLIST));
	}
	else
	{
		CStringListGridWnd::OnGridNew();
	}
}

void CToolsGrid::DoCut()
{
	int nCurSel = m_pDlg->m_ToolGrid.GetCurSel();
	if (m_pDlg->m_ToolList.GetTool(nCurSel) == NULL)
	{
		MessageBeep(0);
		return;
	}
	if (m_pClipboardTool != NULL)
		delete m_pClipboardTool;
	m_pClipboardTool = new CTool;
	*m_pClipboardTool = *(m_pDlg->m_ToolList.GetTool(nCurSel));
	m_pDlg->m_ToolList.DeleteTool(nCurSel);
	m_pDlg->FillGrid(nCurSel);
}

void CToolsGrid::DoCopy()
{
	int nCurSel = m_pDlg->m_ToolGrid.GetCurSel();
	if (m_pDlg->m_ToolList.GetTool(nCurSel) == NULL)
	{
		MessageBeep(0);
		return;
	}
	if (m_pClipboardTool != NULL)
		delete m_pClipboardTool;
	m_pClipboardTool = new CTool;
	*m_pClipboardTool = *(m_pDlg->m_ToolList.GetTool(nCurSel));
	m_pClipboardTool->m_nOutputWindowID = INVALID_OUTPUT_WINDOW;
}

void CToolsGrid::DoPaste()
{
	if(!m_pClipboardTool)
	{
		MessageBeep(0);
		return;
	}

	int nCurSel = m_pDlg->m_ToolGrid.GetCurSel();
	int nTools = m_pDlg->m_ToolList.NumTools();
	if (nCurSel < nTools)
	{
		// replacing an existing tool
		delete m_pDlg->m_ToolList.m_toolArray[nCurSel];
		CTool *pTool = new CTool;
		*pTool = *m_pClipboardTool;
		m_pDlg->m_ToolList.m_toolArray[nCurSel] = pTool;
		m_pDlg->FillGrid(nCurSel);
	}
	else
	{
		// adding a tool to end of list
		CTool *pTool = new CTool;
		*pTool = *m_pClipboardTool;
		m_pDlg->m_ToolList.AddTool(pTool);
		m_pDlg->FillGrid(nCurSel);
	}

	m_pClipboardTool->m_nOutputWindowID = INVALID_OUTPUT_WINDOW;
}

BOOL CToolsGrid::ProcessKeyboard(MSG* pMsg, BOOL bPreTrans /*= FALSE*/)
{
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		switch (pMsg->wParam)
		{
			case 'X':
				// Ctrl+X is cut
				if (m_pActiveElement != NULL)
					return(CStringListGridWnd::ProcessKeyboard(pMsg, bPreTrans));
				if (GetKeyState(VK_CONTROL) >= 0)
					break;
				DoCut();
				return TRUE;

			case 'C':
				// Ctrl+C is copy
				if (m_pActiveElement != NULL)
					return(CStringListGridWnd::ProcessKeyboard(pMsg, bPreTrans));
				if (GetKeyState(VK_CONTROL) >= 0)
					break;
				DoCopy();
				return TRUE;

			case 'V':
				// Ctrl+V is paste
				if (m_pActiveElement != NULL)
					return(CStringListGridWnd::ProcessKeyboard(pMsg, bPreTrans));
				if (GetKeyState(VK_CONTROL) >= 0)
					break;
				DoPaste();
				return TRUE;

			case VK_DELETE:
				// Shift+Delete is cut
				if (m_pActiveElement != NULL)
					return(CStringListGridWnd::ProcessKeyboard(pMsg, bPreTrans));
				if (GetKeyState(VK_SHIFT) >= 0)
					return(CStringListGridWnd::ProcessKeyboard(pMsg, bPreTrans));
				DoCut();
				return TRUE;

			case VK_INSERT:
				// Ctrl+Insert is copy, Shift+Insert is paste
				if (m_pActiveElement != NULL)
					return(CStringListGridWnd::ProcessKeyboard(pMsg, bPreTrans));
				if (GetKeyState(VK_CONTROL) >= 0 && GetKeyState(VK_SHIFT) >= 0)
					break;
				if (GetKeyState(VK_CONTROL) < 0)
					DoCopy();
				else if (GetKeyState(VK_SHIFT) < 0)
					DoPaste();
				return TRUE;

			case VK_UP:
			case VK_DOWN:
				return(CStringListGridWnd::ProcessKeyboard(pMsg, bPreTrans));

			case VK_ESCAPE:
				if (m_pControlWnd == NULL && !m_bDragRow)
				{
					m_pDlg->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
					return TRUE;
				}
				else
					return(CStringListGridWnd::ProcessKeyboard(pMsg, bPreTrans));

			case VK_RETURN:
				// if no active element, return should close dialog
				if (m_pActiveElement == NULL)
					return(CGridWnd::ProcessKeyboard(pMsg, bPreTrans));

				CGridControlWnd::CloseActiveElement();
				return TRUE;

			case VK_TAB:
				// control-tab is dialog tab switching
				if (GetKeyState(VK_CONTROL) < 0)
					break;

				CGridControlWnd::CloseActiveElement();
				CWnd* pParent = GetParent();
				if (pParent != NULL)
				{
					pMsg->hwnd = pParent->m_hWnd;
					return CWnd::PreTranslateMessage(pMsg);
				}
				return TRUE;
		}
	}

	return(CGridControlWnd::ProcessKeyboard(pMsg, bPreTrans));
}

BOOL CToolsGrid::BeginDrag(UINT nFlags, const CPoint& point)
{
	// don't allow dragging of the new row!
	CGridElement* pElement = ElementFromPoint(point);
	CGridRow* pLastRow = GetRowAt(m_rows.GetTailPosition());

	if ((pElement == pLastRow) && (m_pNewRow != NULL))
	{
		m_bDragSelecting = FALSE;
		m_bDragRow = FALSE;
		m_pDropRow = NULL;
		return TRUE;
	}
	else
	{
		return(CGridWnd::BeginDrag(nFlags, point));
	}
}

void CToolsGrid::AddNewRow(BOOL bSelect /*= TRUE*/)
{
	if (GetRowCount() == MAXTOOLIST)
		m_pNewRow = NULL;
	else
		CStringListGridWnd::AddNewRow(bSelect);
}

void CToolsGrid::DoMove(int nToIndex)
{
	// virtually the same as CStringListGridWnd::DoMove(), but handling 
	// of reinsertion of moved rows differs when the new row is NULL.
	ASSERT(!IsFlag(GRIDLIST_NOORDER));

	if ((nToIndex > GetCount()) || ((nToIndex == GetCount()) && (m_pNewRow != NULL)))
		return;

	if (nToIndex < 0)
		return;

	ASSERT(m_selection.GetCount() == 1);

	// Notify the grid owner of these moves
	SORTABLEROW* pSelRows;
	pSelRows = new SORTABLEROW[m_selection.GetCount()];
	int cMove = 0;

	POSITION posSel = GetHeadSelPosition();
	int nDest = nToIndex;
	while (posSel != NULL)
	{
		CStringListGridRow* pSelRow = (CStringListGridRow*) GetNextSel(posSel);
		if (pSelRow->IsNewRow())
		{
			delete [] pSelRows;
			return;
		}
		else
		{
			int nSrc = GetRowIndex(pSelRow);

			if (!OnMove(nSrc, nDest++))
			{
				Deselect(pSelRow);
				nDest--;
			}
			else
			{
				pSelRows[cMove].m_pRow = pSelRow;
				pSelRows[cMove++].m_nRow = nSrc;
			}
		}
	}

	// Remove the select rows
	posSel = GetHeadSelPosition();
	while (posSel != NULL)
	{
		CStringListGridRow* pSelRow = (CStringListGridRow*) GetNextSel(posSel);

		// Remove the row from the grid
		POSITION pos = m_rows.Find(pSelRow);
		ASSERT(pos != NULL);

		// Update m_posTopVisibleRow if needed
		if (m_posTopVisibleRow == pos)
		{
			GetPrevRow(m_posTopVisibleRow);
			if (m_posTopVisibleRow != NULL && (GetRowAt(m_posTopVisibleRow) == GetCaptionRow()))
			{
				m_posTopVisibleRow = NULL;
				m_cyScroll = 0;
			}
		}
		m_rows.RemoveAt(pos);
	}

	// Sort the row list
	qsort(pSelRows, cMove, sizeof(SORTABLEROW), CompareRowIDs);

	// Now move each row from our temp list into the grid's list of rows
	POSITION posTarget = NULL;
	for (int iCur = 0; iCur < cMove; iCur++)
	{
		// Where should we insert the row?
		if (posTarget == NULL)
		{
			if (nToIndex >= GetCount())
			{
				if (m_pNewRow != NULL)
					posTarget = m_rows.InsertBefore(m_rows.Find(m_pNewRow), pSelRows[iCur].m_pRow);
				else
					posTarget = m_rows.InsertAfter(m_rows.FindIndex(nToIndex), pSelRows[iCur].m_pRow);
			}
			else if (nToIndex <= 0)
			{
				posTarget = m_rows.InsertAfter(m_rows.GetHeadPosition(), pSelRows[iCur].m_pRow);
			}
			else
			{
				posTarget = m_rows.InsertAfter(m_rows.FindIndex(nToIndex), pSelRows[iCur].m_pRow);
			}
		}
		else
		{
			posTarget = m_rows.InsertAfter(posTarget, pSelRows[iCur].m_pRow);
		}
	}
	delete [] pSelRows;

	AdjustTopVisible();
	ResetSize(); // INEFFICIENT
	Invalidate(FALSE); // INEFFICIENT
	ResetScrollBars();

	// Make sure the primary selection is still visible
	if (!m_selection.IsEmpty())
	{
		CGridElement* pSelElement = (CGridElement*)m_selection.GetHead();
		if (pSelElement != NULL)
		{
			CGridRow* pRow = pSelElement->GetRow();
			ASSERT(pRow != NULL);

			ScrollIntoView(pRow);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCustomizeToolsDialog

IMPLEMENT_DYNAMIC(CCustomizeToolsDialog, CDlgTab);

CCustomizeToolsDialog::CCustomizeToolsDialog() : CDlgTab(IDDP_CUSTOMIZE_TOOLSMENU, IDS_TOOLS)
{
	m_bOutputWindowAvailable = FALSE;
 	m_ToolList = toolList;
	m_nDeletedOutputWindows = 0;
}

BOOL CCustomizeToolsDialog::OnInitDialog()
{
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_TM_COMMAND);
	pEdit->LimitText(MAXPATHNAME);
	pEdit = (CEdit *)GetDlgItem(IDC_TM_INITDIR);
	pEdit->LimitText(MAXINITDIR);
	pEdit = (CEdit *) GetDlgItem(IDC_TM_ARGUMENTS);
	pEdit->LimitText(MAXARGUMENT);

	// FUTURE: temporary workaround [patbr]
	// if SRC package not available, disable the 'redirect to output window'
	// checkbox, because the output window still lives in the SRC package.
	if (theApp.GetPackage(PACKAGE_VCPP))
		m_bOutputWindowAvailable = TRUE;

	// create the tools grid.
	VERIFY(m_ToolGrid.ReplaceControl(this, IDC_TM_PLACEHOLDER, IDC_TM_TOOLSGRID,
		WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_TABSTOP, WS_EX_CLIENTEDGE, GRIDWND_TB_ALL));
	m_ToolGrid.m_pDlg = this;
	FillGrid();
	m_ToolGrid.SetFocus();

	m_mbtnArgMacros.SubclassDlgItem(IDC_TM_ARGMACROS, this);
	m_mbtnDirMacros.SubclassDlgItem(IDC_TM_DIRMACROS, this);
	VERIFY(m_mbtnArgMacros.LoadMenu(IDR_TOOL_MACRO));
	VERIFY(m_mbtnDirMacros.LoadMenu(IDR_TOOL_MACRO));
	m_mbtnDirMacros.SetPos(1); // Use second menu for dirs
	return TRUE;
}

void CCustomizeToolsDialog::FillGrid(int nSelectRow)
{
	m_ToolGrid.ResetContent();

	for (int i = 0; i < m_ToolList.NumTools(); i++)
	{
		CTool *pTool = m_ToolList.GetTool(i);
		ASSERT(pTool != NULL);
		m_ToolGrid.AddString((const TCHAR *)pTool->m_strMenuName);

		if (m_ToolGrid.GetCount() == MAXTOOLIST)
		{
			POSITION pos = m_ToolGrid.m_rows.Find(m_ToolGrid.m_pNewRow);
			if (pos != NULL)
			{
				delete m_ToolGrid.m_pNewRow;
				m_ToolGrid.m_pNewRow = NULL;
			}
		}
	}

	// set the current selection in the tools list.
	nSelectRow = min(nSelectRow, m_ToolGrid.GetCount());  // OK to select new item.
	m_ToolGrid.SetCurSel(nSelectRow);
	m_ToolList.SetCurTool(nSelectRow);
	ShowToolInfo();

	EnableControls();
}

void CCustomizeToolsDialog::OnMacroItem(UINT nId)
{
	if (IDS_TOOLMACRO_First <= nId && nId <= IDS_TOOLMACRO_Last)
	{
		CString strMacro;
		VERIFY(strMacro.LoadString(nId));
		CEdit *pwnd = (CEdit *)GetDlgItem(IDC_TM_ARGUMENTS);
		pwnd->ReplaceSel(strMacro);
		pwnd->SetFocus();
	}
	else if (IDS_TOOLMACRODIR_First <= nId && nId <= IDS_TOOLMACRODIR_Last)
	{
		CString strMacro;
		int nStrId;
		switch (nId)
		{
			case IDS_TOOLMACRODIR_FileDir:
				nStrId = IDS_TOOLMACRO_FileDir; break;
			case IDS_TOOLMACRODIR_FileName:
				nStrId = IDS_TOOLMACRO_FileName; break;
			case IDS_TOOLMACRODIR_Dir:
				nStrId = IDS_TOOLMACRO_Dir; break;
			case IDS_TOOLMACRODIR_TargetDir:
				nStrId = IDS_TOOLMACRO_TargetDir; break;
			case IDS_TOOLMACRODIR_TargetName:
				nStrId = IDS_TOOLMACRO_TargetName; break;
			case IDS_TOOLMACRODIR_WkspDir:
				nStrId = IDS_TOOLMACRO_WkspDir; break;
			case IDS_TOOLMACRODIR_WkspName:
				nStrId = IDS_TOOLMACRO_WkspName; break;
		}
		VERIFY(strMacro.LoadString(nStrId));
		CEdit *pwnd = (CEdit *)GetDlgItem(IDC_TM_INITDIR);
		pwnd->ReplaceSel(strMacro);
		pwnd->SetFocus();
	}
	else
		ASSERT(0);
}

BOOL CCustomizeToolsDialog::ValidateTab()
{
	int nCurSel = m_ToolGrid.GetCurSel();
	int nTools = m_ToolList.NumTools();
	if ((nCurSel < nTools) && !GetToolInfo())
		return FALSE;

	CVshellPackage *pPackage = (CVshellPackage *)theApp.GetPackage(PACKAGE_SUSHI);
	LPOUTPUTWINDOW pInterface = g_IdeInterface.GetOutputWindow();

	int nID;
	for (nID = IDM_USERTOOLS_BASE; nID <= IDM_USERTOOLS_LAST; nID++)
	{
		CTool *pCurTool;
		if ((pCurTool = m_ToolList.GetTool(nID-IDM_USERTOOLS_BASE)) != NULL)
		{
			pPackage->SetVisibleMenuItem(nID, TRUE);

			if (pCurTool->m_strPathName.Find(" /") != -1)
			{
				MsgBox(Error, IDS_NO_ARGS_IN_PATH);
				goto HandleError;
			}

			CPath pathTemp;

			// now validate the paths entered by the user...
			if (!pathTemp.Create(pCurTool->m_strPathName))
			{
				MsgBox(Error, IDS_FEXCEP_BADPATH);
				goto HandleError;
			}

			TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR];
			_splitpath((const TCHAR *)pCurTool->m_strPathName, szDrive, szDir, NULL, NULL);

			if (*szDrive || *szDir)
			{
				// if path entered by user contains drive or dir, treat it as full path.
				if (!pathTemp.ExistsOnDisk())  
				{
					CString strMsg;

					if (MsgBox(Question, MsgText(strMsg, IDS_TOOL_PATH_INVALID, pathTemp.GetFullPath(), pCurTool->m_strMenuName), MB_YESNO) != IDNO)
						goto HandleError;
					else
					{
						pCurTool->m_strPathName = pathTemp.GetFullPath();
						pCurTool->m_fUseSearchPath = FALSE;
						pCurTool->m_fGUITool = FALSE;
					}
				}
				else
				{
					pCurTool->m_strPathName = pathTemp.GetFullPath();
					pCurTool->m_fUseSearchPath = FALSE;
					pCurTool->m_fGUITool = IsFileGUIExe((LPSTR)(const TCHAR *)pathTemp);
				}
			}
			else
			{
				// otherwise, user simply entered the filename--check along the path for it.
				TCHAR szFullPath[_MAX_PATH], *psz;
				if ((SearchPath(NULL, pCurTool->m_strPathName, ".EXE", _MAX_PATH, szFullPath, &psz)) ||
					(SearchPath(NULL, pCurTool->m_strPathName, ".COM", _MAX_PATH, szFullPath, &psz)) ||
					(SearchPath(NULL, pCurTool->m_strPathName, ".BAT", _MAX_PATH, szFullPath, &psz)) ||
					(SearchPath(NULL, pCurTool->m_strPathName, ".CMD", _MAX_PATH, szFullPath, &psz)))
				{
					pCurTool->m_fUseSearchPath = TRUE;
					pCurTool->m_strPathName = psz;
					pCurTool->m_fGUITool = IsFileGUIExe((LPSTR)szFullPath);
				}
				else
				{
					CString strMsg;

					if (MsgBox(Question, MsgText(strMsg, IDS_TOOL_PATH_INVALID, pCurTool->m_strPathName, pCurTool->m_strMenuName), MB_YESNO) != IDNO)
						goto HandleError;
					else
					{
						pCurTool->m_fUseSearchPath = TRUE;
						pCurTool->m_fGUITool = FALSE;
					}
				}
			}

			if (pInterface != NULL)
			{
				if (pCurTool->m_nOutputWindowID != INVALID_OUTPUT_WINDOW)
				{
					// user tools should only have output window tabs if either 
					// 'Ask for Arguments' or 'Redirect to Output Window' checked.
					if (pCurTool->m_fGUITool || (!pCurTool->m_fAskArguments && !pCurTool->m_fOutputRedirectDefault))
					{
						m_rgnDeletedOutputWindows[m_nDeletedOutputWindows++] = pCurTool->m_nOutputWindowID;
						pCurTool->m_nOutputWindowID = INVALID_OUTPUT_WINDOW;
					}
					else
					{
						pInterface->OutputWindowModifyVwin(pCurTool->m_nOutputWindowID, (LPSTR)(const TCHAR *)pCurTool->m_strMenuName, (UINT *)&(pCurTool->m_nOutputWindowID));
					}
				}
				else
				{
					// user tools should only have output window tabs if either 
					// 'Ask for Arguments' or 'Redirect to Output Window' checked.
					if (!pCurTool->m_fGUITool && (pCurTool->m_fAskArguments || pCurTool->m_fOutputRedirectDefault))
					{
						pInterface->OutputWindowAddVwin((LPSTR)(const TCHAR *)pCurTool->m_strMenuName, FALSE, (UINT *)&(pCurTool->m_nOutputWindowID));
					}		  
				}
			}
		}
		else
			pPackage->SetVisibleMenuItem(nID, FALSE);
	}

	if (pInterface != NULL)
	{
		for (int i = 0; i < m_nDeletedOutputWindows; i++) 
			pInterface->OutputWindowDeleteVwin(m_rgnDeletedOutputWindows[i]);
	}
	// set count back to zero
	m_nDeletedOutputWindows = 0;

	toolList = m_ToolList;
	return TRUE;

HandleError:
	m_ToolGrid.SetCurSel(nID - IDM_USERTOOLS_BASE);
	m_ToolList.SetCurTool(nID - IDM_USERTOOLS_BASE);
	ShowToolInfo();
	EnableControls();
	((CEdit *)GetDlgItem(IDC_TM_COMMAND))->SetFocus();
	((CEdit *)GetDlgItem(IDC_TM_COMMAND))->SetSel((DWORD)0xFFFF0000, TRUE);
	return(FALSE);
}

void CCustomizeToolsDialog::CancelTab()
{
	CDlgTab::CancelTab();
}

BEGIN_MESSAGE_MAP (CCustomizeToolsDialog, CDlgTab)
	//{{AFX_MSG_MAP (CCustomizeToolsDialog)
	ON_EN_KILLFOCUS(IDC_TM_COMMAND, OnPathNameKillFocus)
	ON_BN_CLICKED(IDC_TM_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_TM_REDIRECTOUTPUT, OnRedirect)
	ON_SELCHANGE(IDC_TM_TOOLSGRID, OnGridSelChange)
	ON_ADDSTRING(IDC_TM_TOOLSGRID, OnGridAddString)
	ON_DELETESTRING(IDC_TM_TOOLSGRID, OnGridDeleteString)
	ON_SETFOCUS(IDC_TM_TOOLSGRID, OnGridSetFocus)
	ON_KILLFOCUS(IDC_TM_TOOLSGRID, OnGridKillFocus)
	ON_ACTIVATE_CTL(IDC_TM_TOOLSGRID, OnGridActivate)
	ON_ACCEPT_CTL(IDC_TM_TOOLSGRID, OnGridAccept)
	ON_COMMAND_RANGE(IDS_TOOLMACRO_First, IDS_TOOLMACRODIR_Last, OnMacroItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CCustomizeToolsDialog::OnPathNameKillFocus()
{
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_TM_COMMAND);
	CTool *pTool = m_ToolList.GetTool(m_ToolGrid.GetCurSel());
	CString pathName;

	if (pTool == NULL) return;

	pEdit->GetWindowText(pathName);

	// create path, but don't verify at this point.
	pTool->m_strPathName = pathName;

	TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR];
	_splitpath((const TCHAR *)pathName, szDrive, szDir, NULL, NULL);

	if (*szDrive || *szDir)
	{
		// if path entered by user contains drive or dir, treat it as full path.
		CPath pathVerify;
		if (pathVerify.Create(pathName))
		{
			if (pathVerify.ExistsOnDisk())  
			{
				// check for GUI executable.
				pTool->m_fGUITool = IsFileGUIExe((LPSTR)(const TCHAR *)pathVerify);
				EnableRedirection();
			}
		}
	}
	else
	{
		// otherwise, user simply entered the filename--check along the path for it.
		TCHAR szFullPath[_MAX_PATH], *psz;
		if ((SearchPath(NULL, pathName, ".EXE", _MAX_PATH, szFullPath, &psz)) ||
			(SearchPath(NULL, pathName, ".COM", _MAX_PATH, szFullPath, &psz)) ||
			(SearchPath(NULL, pathName, ".BAT", _MAX_PATH, szFullPath, &psz)) ||
			(SearchPath(NULL, pathName, ".CMD", _MAX_PATH, szFullPath, &psz)))
		{
			// check for GUI executable.
			pTool->m_fGUITool = IsFileGUIExe((LPSTR)szFullPath);
			EnableRedirection();
		}
	}
}

void CCustomizeToolsDialog::EnableControls()
{
	CTool *pTool = m_ToolList.GetTool(m_ToolGrid.GetCurSel());
	if (pTool != NULL)
	{
		GetDlgItem(IDC_TM_COMMAND)->EnableWindow(TRUE);
		GetDlgItem(IDC_TM_BROWSE)->EnableWindow(TRUE);
		GetDlgItem(IDC_TM_ARGUMENTS)->EnableWindow(TRUE);
		GetDlgItem(IDC_TM_INITDIR)->EnableWindow(TRUE);
		GetDlgItem(IDC_TM_COMMAND_LABEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_TM_ARGUMENTS_LABEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_TM_INITDIR_LABEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_TM_PROMPTFORARGS)->EnableWindow(TRUE);
		GetDlgItem(IDC_TM_ARGMACROS)->EnableWindow(TRUE);
		GetDlgItem(IDC_TM_DIRMACROS)->EnableWindow(TRUE);
		EnableRedirection();
	}
	else
	{
		GetDlgItem(IDC_TM_COMMAND)->EnableWindow(FALSE);
		GetDlgItem(IDC_TM_BROWSE)->EnableWindow(FALSE);
		GetDlgItem(IDC_TM_ARGUMENTS)->EnableWindow(FALSE);
		GetDlgItem(IDC_TM_INITDIR)->EnableWindow(FALSE);
		GetDlgItem(IDC_TM_COMMAND_LABEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_TM_ARGUMENTS_LABEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_TM_INITDIR_LABEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_TM_PROMPTFORARGS)->EnableWindow(FALSE);
		GetDlgItem(IDC_TM_REDIRECTOUTPUT)->EnableWindow(FALSE);
		GetDlgItem(IDC_TM_CLOSEONEXIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_TM_ARGMACROS)->EnableWindow(FALSE);
		GetDlgItem(IDC_TM_DIRMACROS)->EnableWindow(FALSE);
	}
}

void CCustomizeToolsDialog::EnableRedirection()
{
	CTool *pTool = m_ToolList.GetTool(m_ToolGrid.GetCurSel());
	if (m_ToolList.NumTools() == 0 || pTool == NULL)
	{
 		GetDlgItem(IDC_TM_REDIRECTOUTPUT)->EnableWindow(FALSE);		
		GetDlgItem(IDC_TM_CLOSEONEXIT)->EnableWindow(FALSE);
	}
	else if (pTool->m_fGUITool)
	{
		GetDlgItem(IDC_TM_REDIRECTOUTPUT)->EnableWindow(FALSE);		
		GetDlgItem(IDC_TM_CLOSEONEXIT)->EnableWindow(FALSE);
	}		
	else
	{
		GetDlgItem(IDC_TM_REDIRECTOUTPUT)->EnableWindow(m_bOutputWindowAvailable);		
		GetDlgItem(IDC_TM_CLOSEONEXIT)->EnableWindow(!IsDlgButtonChecked(IDC_TM_REDIRECTOUTPUT));
	}
}

void CCustomizeToolsDialog::ShowToolInfo()
{
	CTool *pTool = m_ToolList.GetCurToolObject();
	if (pTool != NULL)
	{
		SetDlgItemText(IDC_TM_COMMAND, pTool->m_strPathName);
		SetDlgItemText(IDC_TM_ARGUMENTS, pTool->m_strDefaultArgs);
		SetDlgItemText(IDC_TM_INITDIR, pTool->m_strInitialDir);
		CheckDlgButton(IDC_TM_PROMPTFORARGS, pTool->m_fAskArguments);
		CheckDlgButton(IDC_TM_REDIRECTOUTPUT, pTool->m_fOutputRedirectDefault);
		CheckDlgButton(IDC_TM_CLOSEONEXIT, pTool->m_fCloseWindow);
		EnableRedirection();
	}
	else
	{
		SetDlgItemText(IDC_TM_COMMAND, NULL);
		SetDlgItemText(IDC_TM_ARGUMENTS, NULL);
		SetDlgItemText(IDC_TM_INITDIR, NULL);
		CheckDlgButton(IDC_TM_PROMPTFORARGS, FALSE);
		CheckDlgButton(IDC_TM_REDIRECTOUTPUT, FALSE);
		CheckDlgButton(IDC_TM_CLOSEONEXIT, FALSE);
	}
}

BOOL CCustomizeToolsDialog::GetToolInfo()
{
	if (m_ToolList.NumTools() == 0)
		return TRUE;

	CEdit *pEdit;
	CTool *pTool = m_ToolList.GetCurToolObject();
	if (pTool == NULL)
		return FALSE;

	CString pathName;
	pEdit = (CEdit *)GetDlgItem(IDC_TM_COMMAND);
	pEdit->GetWindowText(pathName);
	pTool->m_strPathName = (const TCHAR *)pathName;
	// don't validate paths here--do at ValidateTab() instead.

	CString strInitDir;
	pEdit = (CEdit *)GetDlgItem(IDC_TM_INITDIR);
	pEdit->GetWindowText(strInitDir);
	pTool->m_strInitialDir = (const TCHAR *)strInitDir;
	// FUTURE: check for existence of directory
	
	CString strArguments;
	pEdit = (CEdit *)GetDlgItem(IDC_TM_ARGUMENTS);
	pEdit->GetWindowText(strArguments);
	pTool->m_strDefaultArgs = (const TCHAR *)strArguments;

	// Check buttons
	pTool->m_fAskArguments = IsDlgButtonChecked(IDC_TM_PROMPTFORARGS);
	pTool->m_fOutputRedirectDefault = IsDlgButtonChecked(IDC_TM_REDIRECTOUTPUT);
	pTool->m_fCloseWindow = IsDlgButtonChecked(IDC_TM_CLOSEONEXIT);

	return TRUE;
}

void CCustomizeToolsDialog::OnBrowse()
{
	CString strDefExt, strFilter, strTitle;

	// instantiating a CCurDir here preserves the current directory across
	// the browse dialog call, so browse will not cause dir to be changed.
	CCurDir cd;

	strDefExt = "exe";
	strFilter.LoadString(IDS_FILE_TYPE_DLG);
	strTitle.LoadString(IDS_ADDTOOL_DLG_TITLE);

	C3dFileDialog dlgBrowse(TRUE, strDefExt, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strFilter, this, IDD_ADDTOOL_FILE_OPEN);
	dlgBrowse.m_iddWin95 = dlgBrowse.m_iddWinNT = 0x0;
	dlgBrowse.m_ofn.hwndOwner = _SushiGetSafeOwner(NULL);
	dlgBrowse.m_ofn.lpstrTitle = strTitle;

	if (dlgBrowse.DoModal() == IDOK)
	{
		CString strPath = dlgBrowse.GetPathName();
		((CEdit *)GetDlgItem(IDC_TM_COMMAND))->SetWindowText(strPath);
	}
}

void CCustomizeToolsDialog::OnRedirect()
{
	GetDlgItem(IDC_TM_CLOSEONEXIT)->EnableWindow(!IsDlgButtonChecked(IDC_TM_REDIRECTOUTPUT));
}

BOOL CCustomizeToolsDialog::OnAddTool(int nIndex)
{
	if (m_ToolList.NumTools() == MAXTOOLIST)
	{
		CString strMsg;
		MsgBox(Error, MsgText(strMsg, IDS_TOOL_MAX_REACHED, MAXTOOLIST));
		return FALSE;
	}

	CTool *pTool = new CTool;

	VERIFY(m_ToolGrid.GetText(nIndex, pTool->m_strMenuName));
	pTool->m_strPathName.Empty();
	pTool->m_strDefaultArgs.Empty();
	pTool->m_strInitialDir.Empty();
	pTool->m_fAskArguments = FALSE;
	pTool->m_fOutputRedirectDefault = FALSE;
	pTool->m_fCloseWindow = FALSE;
	pTool->m_nOutputWindowID = INVALID_OUTPUT_WINDOW;
	pTool->m_fGUITool = FALSE;
	pTool->m_fUseSearchPath = TRUE;
	pTool->m_fVisibleOnMenu = TRUE;
	pTool->m_fReloadNoPrompt = FALSE;
	m_ToolList.SetCurTool(m_ToolList.AddTool(pTool) - 1);

	ShowToolInfo();
	EnableControls();

	return TRUE;
}

BOOL CCustomizeToolsDialog::OnDelTool(int nIndex)
{
	int nCurSel = m_ToolGrid.GetCurSel();

	CTool *pTool = m_ToolList.GetTool(nCurSel);
	if (pTool->m_nOutputWindowID != INVALID_OUTPUT_WINDOW)
		m_rgnDeletedOutputWindows[m_nDeletedOutputWindows++] = pTool->m_nOutputWindowID;
	m_ToolList.DeleteTool(nCurSel);

	ShowToolInfo();
	EnableControls();

	return TRUE;
}

BOOL CCustomizeToolsDialog::OnChangeTool(int nIndex)
{
	CTool *pTool = m_ToolList.GetCurToolObject();
	VERIFY(m_ToolGrid.GetText(nIndex, pTool->m_strMenuName));
	return TRUE;
}

BOOL CCustomizeToolsDialog::OnMoveTool(int nSrcIndex, int nDestIndex)
{
	if (nSrcIndex > nDestIndex)
	{
		while (nSrcIndex > nDestIndex)
		{
			m_ToolList.SwapTool(nSrcIndex, nSrcIndex-1);
			nSrcIndex--;
		}
	}
	else
	{
		while (nSrcIndex < nDestIndex)
		{
			m_ToolList.SwapTool(nSrcIndex, nSrcIndex+1);
			nSrcIndex++;
		}
	}
	m_ToolList.SetCurTool(nDestIndex);
	return TRUE;
}

void CCustomizeToolsDialog::OnGridSelChange()
{
	// register any changes made to the current tool
	GetToolInfo();

	// select the new current tool
	m_ToolList.SetCurTool(m_ToolGrid.GetCurSel());
	ShowToolInfo();
	EnableControls();

	return;
}

void CCustomizeToolsDialog::OnGridAddString()
{
	GetDlgItem(IDC_TM_COMMAND)->SetFocus();
}

void CCustomizeToolsDialog::OnGridDeleteString()
{
	// refill the grid
	FillGrid(m_ToolGrid.GetCurSel());

	// may have deleted new row if grid was full
	if (m_ToolGrid.m_pNewRow == NULL)
		((CStringListGridWnd *)&m_ToolGrid)->AddNewRow(FALSE);
}

void CCustomizeToolsDialog::OnGridSetFocus()
{
}

void CCustomizeToolsDialog::OnGridKillFocus()
{
	if (m_ToolGrid.m_pActiveElement != NULL)
		((CGridControlWnd *)&m_ToolGrid)->CloseActiveElement();
}

void CCustomizeToolsDialog::OnGridActivate()
{
}

void CCustomizeToolsDialog::OnGridAccept()
{
}

/////////////////////////////////////////////////////////////////////////////
