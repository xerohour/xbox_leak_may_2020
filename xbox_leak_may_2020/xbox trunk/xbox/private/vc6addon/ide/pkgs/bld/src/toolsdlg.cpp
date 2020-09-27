 //
// CToolsDlg

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "toolsdlg.h"	// our local header
#include "bldslob.h"	// our local header

/*
R.I.P. for v4.0 with VBA?
#include <dlgbase.h>	// CDirChooser

IMPLEMENT_DYNAMIC(CToolsDlg, CDialog)
IMPLEMENT_DYNAMIC(CEditToolDlg, CDialog)
*/

IMPLEMENT_DYNAMIC(CCustomBuildPage, CSlobPageTab)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static BOOL m_bCustomToolPageIgnoreChange = FALSE;

extern void GetStrFromGrid(CStringListGridWnd* pGrid, CString& value); // projpage.cpp
extern void SetStrFromGrid(CStringListGridWnd* pGrid, const CString& value); // projpage.cpp
extern void EnableControl(CWnd* pWnd, GPT gpt);		// implemented in proppage.cpp

/*
R.I.P. for v4.0 with VBA?
BEGIN_POPUP_MENU(ToolDirMacro)
	MENU_ITEM(IDM_BLDTOOL_DIRMACRO1)
	MENU_ITEM(IDM_BLDTOOL_DIRMACRO2)
END_POPUP_MENU()

CDirMacroMenu::CDirMacroMenu()
{
	// FUTURE: when SetPopup is public: then don't use a class!
	SetPopup(MENU_CONTEXT_POPUP(ToolDirMacro));
}

BEGIN_POPUP_MENU(ToolFileMacro)
	MENU_ITEM(IDM_BLDTOOL_FILEMACRO1)
	MENU_ITEM(IDM_BLDTOOL_FILEMACRO2)
END_POPUP_MENU()


CFileMacroMenu::CFileMacroMenu()
{
	// FUTURE: when SetPopup is public: then don't use a class!
	SetPopup(MENU_CONTEXT_POPUP(ToolFileMacro));
}

BEGIN_MESSAGE_MAP (CEditToolDlg, CDialog)
	//{{AFX_MSG_MAP (CEditToolDlg)
	ON_BN_CLICKED(IDC_EDITTOOL_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP	()

CEditToolDlg::CEditToolDlg(CCustomTool * pTool, CWnd * pwndParent)
	: C3dDialog (IDD_EDIT_TOOL, pwndParent)
{
	// whose data?
	m_pTool = pTool;
}

BOOL CEditToolDlg::OnInitDialog()
{
	// init. the base-class dialog
	if (!C3dDialog::OnInitDialog())
		return FALSE;	// failed to init.

	// load out menu buttons
	if (!m_btnDirMacro.SubclassDlgItem(IDC_EDITTOOL_DIRMACRO, this))
		return FALSE;	// not ok
	if (!m_btnFileMacro.SubclassDlgItem(IDC_EDITTOOL_FILEMACRO, this))
		return FALSE;	// not ok

	return TRUE;	// ok
}

// command browser filters
static const TCHAR szExeFilter[] = _TEXT("Executable (*.exe)");
static const TCHAR szBatFitler[] = _TEXT("Batch File (*.bat)");
static const TCHAR szCmdFitler[] = _TEXT("Command File (*.cmd)");

void CEditToolDlg::OnBrowse()
{
	CString strTitle, strFilter;
	VERIFY(strTitle.LoadString(IDS_BROWSEDLG_TITLE));

	CFileDialog dlg(TRUE);
	dlg.m_ofn.lpstrTitle = strTitle;
    dlg.m_ofn.Flags |= OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON;

	AppendFilterSuffix(strFilter, dlg.m_ofn, szExeFilter);
	AppendFilterSuffix(strFilter, dlg.m_ofn, NotOnNT() ? szBatFitler : szCmdFitler);
	dlg.m_ofn.lpstrFilter = strFilter;
	dlg.m_ofn.nFilterIndex = 1;

	// perform the command browser dialog
	if (dlg.DoModal() == IDOK)
	{
		// replace current selection and then set focus there
		CEdit * peditCommand = (CEdit *)GetDlgItem(IDC_EDITTOOL_EXE);
		peditCommand->ReplaceSel((LPCSTR)dlg.GetPathName());
		peditCommand->SetFocus();
	}
}

BOOL CEditToolDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// base-class process this?
	if (C3dDialog::OnCommand(wParam, lParam))
		return TRUE;	// handled by base-class


	// can we?
	if (IDM_BLDTOOL_FIRSTMACRO <= wParam && wParam <= IDM_BLDTOOL_LASTMACRO)
	{
		const TCHAR * pchName;
		if (CCustomTool::MapMacroIdToName((UINT)wParam, pchName))
		{
			// place this in the edit-box
			CString strMacro;
			strMacro += _TEXT("$(");
			strMacro += pchName;
			strMacro += _TEXT(")");

			// replace current selection and then set focus there
			CEdit * peditOutput = (CEdit *)GetDlgItem(IDC_EDITTOOL_OUTPUT);
			ASSERT(peditOutput != (CEdit *)NULL);
			peditOutput->ReplaceSel((LPCSTR)strMacro);
			peditOutput->SetFocus();

			return TRUE; // handled by us
		}
	}

	return FALSE;	// not handled
}

void CEditToolDlg::DoDataExchange(CDataExchange * pDX)
{
	C3dDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditToolDlg)
		DDX_Text(pDX, IDC_EDITTOOL_NAME, m_pTool->m_strComponentName);
		DDX_Text(pDX, IDC_EDITTOOL_EXE, m_pTool->m_strToolExeName);
		DDX_Text(pDX, IDC_EDITTOOL_MENU, m_pTool->m_strToolMenuText);
		DDX_Text(pDX, IDC_EDITTOOL_PREFIX, m_pTool->m_strToolPrefix);
		DDX_Text(pDX, IDC_EDITTOOL_INPUT, m_pTool->m_strToolInput);
		DDX_Text(pDX, IDC_EDITTOOL_OUTPUT, m_pTool->m_strToolOutput);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP (CToolsDlg, CDialog)
	//{{AFX_MSG_MAP (CToolsDlg)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_LBN_SELCHANGE(IDC_TOOLS_LIST, OnSelectTool)
	ON_BN_CLICKED(IDC_BUILDER_SPECIFIC, OnCheckGlobal)
	ON_BN_CLICKED(IDC_NEW_TOOL, OnNewTool)
	ON_BN_CLICKED(IDC_EDIT_TOOL, OnEditTool)
	ON_BN_CLICKED(IDC_DELETE_TOOL, OnDeleteTool)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP	()

int CToolsDlg::OnCreate(LPCREATESTRUCT lpcs)
{
	// make a copy of the global tools
	CCustomTool * pTool;
	g_prjcompmgr.InitCustomToolEnum();
	while (g_prjcompmgr.NextCustomTool(pTool))
	{	
		// create our local custom tools list
		CCustomTool * pToolLcl = (CCustomTool *)pTool->Clone();
		m_lstGblTools.AddTail(pToolLcl);
	}	

	// no tool has the focus
	m_pToolFocus = (CCustomTool *)NULL;

	return 0;	// ok
}

void CToolsDlg::OnDestroy()
{
	// free up our local custom tools list   
	POSITION pos = m_lstGblTools.GetHeadPosition();
	while (pos != (POSITION)NULL)
		delete (CCustomTool *)m_lstGblTools.GetNext(pos);
}

BOOL CToolsDlg::OnInitDialog()
{
	// init. the base-class dialog
	if (!CDialog::OnInitDialog())
		return FALSE;	// failed to init.

	// set the global/per-builder check state
	((CButton *)GetDlgItem(IDC_BUILDER_SPECIFIC))->SetCheck(!m_fGlobal);

	// FUTURE: enable when per-builder tools
	GetDlgItem(IDC_BUILDER_SPECIFIC)->EnableWindow(FALSE);

	// refresh the tools list
	FillToolsList();
	return TRUE;	// ok
}

void CToolsDlg::OnOK()
{
	CCustomTool * pTool;
	CCustomTool * pToolLcl;

	// tool lists we want to un-assign or assign to the project
	CPtrList lstUnAssign;
	CPtrList lstAssign;

	// de-register any?
	g_prjcompmgr.InitCustomToolEnum();
	while (g_prjcompmgr.NextCustomTool(pTool))
	{
		// in our local list?
		BOOL fFound = FALSE;
		POSITION pos = m_lstGblTools.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			pToolLcl = (CCustomTool *)m_lstGblTools.GetNext(pos);
			if (pToolLcl->CompId() == pTool->CompId())
			{
				fFound = TRUE;
				break;
			}
		}

		// found?
		if (!fFound)
		{
			// no, so de-register
			g_prjcompmgr.DeregisterCustomTool(pTool);

			lstUnAssign.AddTail(pTool);		// un-assign
		}
	}

	// register any we need to, or just copy props
	POSITION pos = m_lstGblTools.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		POSITION posCurr = pos;
		pToolLcl = (CCustomTool *)m_lstGblTools.GetNext(pos);
		
		// already registered?
		if (!pToolLcl->CompId())
		{
			// no, so register
			g_prjcompmgr.RegisterBldSysComp(pToolLcl);

			// keep
			m_lstGblTools.RemoveAt(posCurr);

			pToolLcl->Refresh();
			lstAssign.AddTail(pToolLcl);	// assign
		}
		else
		{
			// copy props
			if (g_prjcompmgr.LookupBldSysComp(pToolLcl->CompId(), (CBldSysCmp * &)pTool))
			{
				pToolLcl->CopyProps(pTool);

				// re-assign (unassign then assign)
				lstUnAssign.AddTail(pTool);		// un-assign

				pTool->Refresh();
				lstAssign.AddTail(pTool);	// assign
			}
		}
	}

	// unassign or assign any?
	if (lstUnAssign.GetCount() || lstAssign.GetCount())
	{
		// re-assign our global custom tools to the project in question
		const CPtrArray * pCfgArray = m_pProject->GetConfigArray();
		int iSize = pCfgArray->GetSize();
		CProjTempConfigChange projTempConfigChange(m_pProject);

		for (int i = 0; i < iSize; i++)
		{
			projTempConfigChange.ChangeConfig((ConfigurationRecord *)pCfgArray->GetAt(i));
			CTargetItem * pTarget = m_pProject->GetActiveTarget();

			// unassign?
			if (lstUnAssign.GetCount())
				CActionSlob::UnAssignActions(pTarget, &lstUnAssign);

			// assign?
			if (lstAssign.GetCount())
				CActionSlob::AssignActions(pTarget, &lstAssign);

		}
	}

	// pass-on
	C3dDialog::OnOK();
}

void CToolsDlg::OnNewTool()
{
	// create a new tool
	TCHAR pchName[] = "Custom Tool";
	CCustomTool * pToolLcl = new CCustomTool(pchName);

	CEditToolDlg dlg(pToolLcl);
	if (dlg.DoModal() == IDOK)
	{
		// keep, and add to list
		CPtrList * plstTools = m_fGlobal ? &m_lstGblTools : &m_lstBldTools;
		plstTools->AddTail(pToolLcl);
	}
	else
	{
		// ignore, don't add to list
		delete pToolLcl;
	}

	// refresh the tools list
	FillToolsList();
}

void CToolsDlg::OnEditTool()
{
	// edit the currently selected tool
	CEditToolDlg dlg(m_pToolFocus);
	(void) dlg.DoModal();
}

void CToolsDlg::OnDeleteTool()
{
	CPtrList * plstTools = m_fGlobal ? &m_lstGblTools : &m_lstBldTools;

	// delete the currently selected tool
	POSITION pos = plstTools->Find(m_pToolFocus);
	ASSERT(pos != (POSITION)NULL);
	plstTools->RemoveAt(pos);

	// is this one we created? if so then delete it here
	// otherwise we'll need to deregister with the comp. mgr.
	if (!m_pToolFocus->CompId())
		delete m_pToolFocus;

	// refresh the tools list
	FillToolsList();
}

void CToolsDlg::OnSelectTool()
{
	// get the listbox
	CListBox * plist = (CListBox *)GetDlgItem(IDC_TOOLS_LIST);
	ASSERT(plist != (CListBox *)NULL);

	CCustomTool * pToolFocus;

	// get the new tool focus
	int i = plist->GetCurSel();
	pToolFocus = (CCustomTool *) (i != LB_ERR ? plist->GetItemDataPtr(i) : NULL);

	// enable or disable our edit and delete tool buttons appropriately
	GetDlgItem(IDC_EDIT_TOOL)->EnableWindow(pToolFocus != (CCustomTool *)NULL);
	GetDlgItem(IDC_DELETE_TOOL)->EnableWindow(pToolFocus != (CCustomTool *)NULL);

	m_pToolFocus = pToolFocus;
}

void CToolsDlg::OnCheckGlobal()
{
	// global/per-builder check state?
	m_fGlobal = ((CButton *)GetDlgItem(IDC_BUILDER_SPECIFIC))->GetCheck() != 0;

	// refresh the tools list
	FillToolsList();
}

void CToolsDlg::FillToolsList()
{
	// get the listbox
	CListBox * plist = (CListBox *)GetDlgItem(IDC_TOOLS_LIST);
	ASSERT(plist != (CListBox *)NULL);

	// empty
	plist->ResetContent();

	// which tools list
	CPtrList * plstTools = m_fGlobal ? &m_lstGblTools : &m_lstBldTools;

	// enumerate
	POSITION pos = plstTools->GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		// this is the tool
		CBuildTool * pTool = (CBuildTool *)plstTools->GetNext(pos);
		plist->SetItemDataPtr(plist->AddString((LPCTSTR)*pTool->GetName()), (void *)pTool);
	}

	// can we retain the old tool focus?
	if (plist->GetCount() > 0)
	{
		int i;
		if (m_pToolFocus != (CCustomTool *)NULL)
		{
			i = plist->FindStringExact(0, (LPCTSTR)*m_pToolFocus->GetName());
			if (i == LB_ERR)
				i = 0;	// cannot find, default to first
		}
		else
		{
			i = 0;	// no old tool focus, default to first
		}

		// set the selection
		plist->SetCurSel(i);
	}

	// update our notion of the current tool
	OnSelectTool();
}
*/

//
// primitive custom build 'tool macros'
//

typedef struct 
{
	UINT idMacro;
	const TCHAR * pchName;
} S_ToolMacroMapEl;

static const S_ToolMacroMapEl mapToolMacros[] =
{
	{IDMACRO_INTDIR, "IntDir"},				// intermediate directory
	{IDMACRO_OUTDIR, "OutDir"},				// output directory
	{IDMACRO_WKSPDIR, "WkspDir"},			// workspace directory
	{IDMACRO_PROJDIR, "ProjDir"},			// project directory
	{IDMACRO_TARGDIR, "TargetDir"},			// target directory
	{IDMACRO_INPTDIR, "InputDir"},			// input directory
	{IDMACRO_IDEDIR, "MSDEVDIR"},			// IDE directory
	{IDMACRO_REMOTEDIR, "RemoteDir"},		// remote target directory
	{IDMACRO_WKSPBASE, "WkspName"},			// workspace file basename
	{IDMACRO_TARGFILE, "TargetPath"},		// full path of target
	{IDMACRO_TARGBASE, "TargetName"},		// target file basename
	{IDMACRO_INPTFILE, "InputPath"},		// full path of input
	{IDMACRO_INPTBASE, "InputName"},		// input file basename
	{IDMACRO_REMOTEFILE, "REMOTETARGETPATH"},	// full path of remote target
};

static BOOL rgMacroUsage[sizeof(mapToolMacros) / sizeof(S_ToolMacroMapEl)];

// map a macro id (IDM_) to the macro name
BOOL MapMacroIdToName(UINT idMacro, const TCHAR * & pchName)
{
	for (int i = sizeof(mapToolMacros) / sizeof(S_ToolMacroMapEl); i > 0; i--)
		if (mapToolMacros[i-1].idMacro == idMacro)
		{
			pchName = mapToolMacros[i-1].pchName;
			return TRUE;	// mapped
		}

	return FALSE;	// not mapped
}

// map a macro name to a macro id (IDM_)
BOOL MapMacroNameToId(const TCHAR * pchName, int & cchName, UINT & idMacro)
{
	cchName = 0;

	for (int i = sizeof(mapToolMacros) / sizeof(S_ToolMacroMapEl); i > 0; i--)
	{
		int cch = _tcslen(mapToolMacros[i-1].pchName);
		if ((cch > cchName) &&
			(_tcsnicmp(mapToolMacros[i-1].pchName, pchName, cch) == 0))
		{
			idMacro = mapToolMacros[i-1].idMacro;
			cchName = cch;	// found one match, try for bigger match....
		}
	}

	return cchName != 0;	// not mapped
}

// set macro name usage
void SetMacroIdUsage(BOOL fUsage /*= TRUE*/, UINT idMacro /*= (UINT)-1*/)
{
	if (idMacro == (UINT)-1)
	{
		for (int i = sizeof(mapToolMacros) / sizeof(S_ToolMacroMapEl); i > 0; i--)
			rgMacroUsage[i-1] = fUsage;
	}
	else
	{
		int i = idMacro-IDMACRO_FIRST;
		ASSERT(0 <= i && i < sizeof(mapToolMacros) / sizeof(S_ToolMacroMapEl));
		rgMacroUsage[i] = fUsage;
	}
}

// set macro name usage
BOOL GetMacroIdUsage(UINT idMacro)
{
	int i = idMacro-IDMACRO_FIRST;
	ASSERT(0 <= i && i < sizeof(mapToolMacros) / sizeof(S_ToolMacroMapEl));
	return rgMacroUsage[i];
}

static void RemoveBlankLinesFromGrid(CStringListGridWnd& grid)
{
	CString str;
	int iItem;

	iItem = 0;
	while (iItem < grid.GetCount())
	{
		// get our strings from the grid
		if (grid.GetText(iItem, str))
		{
			str.TrimLeft();
			str.TrimRight();
			if (str.IsEmpty())
			{
				// remove
				grid.DeleteString(iItem);
				continue;
			}
		}
		else
			ASSERT(FALSE);

		// next item
		iItem++;
	}

}

// our custom build dialog tab

BEGIN_POPUP_MENU(ToolDirMacro)
	MENU_ITEM(IDMACRO_INTDIR)
	MENU_ITEM(IDMACRO_OUTDIR)
	MENU_ITEM(IDMACRO_TARGDIR)
	MENU_ITEM(IDMACRO_INPTDIR)
	MENU_ITEM(IDMACRO_PROJDIR)
	MENU_ITEM(IDMACRO_WKSPDIR)
	MENU_ITEM(IDMACRO_IDEDIR)
	MENU_ITEM(IDMACRO_REMOTEDIR)
END_POPUP_MENU()

BEGIN_POPUP_MENU(ToolFileMacro)
	MENU_ITEM(IDMACRO_TARGFILE)
	MENU_ITEM(IDMACRO_TARGBASE)
	MENU_ITEM(IDMACRO_INPTFILE)
	MENU_ITEM(IDMACRO_INPTBASE)
	MENU_ITEM(IDMACRO_WKSPBASE)
	MENU_ITEM(IDMACRO_REMOTEFILE)
END_POPUP_MENU()

BEGIN_MESSAGE_MAP(CCustomBuildPage, CSlobPageTab)
	//{{AFX_MSG_MAP(CCustomBuildPage)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_USER_DEPS, OnUserDefinedDeps)
	ON_WM_DRAWITEM()
	ON_EN_SETFOCUS(IDC_CUSTOM_CMDS, OnActivateEditCmds)
	ON_EN_SETFOCUS(IDC_CUSTOM_OUTPUT, OnActivateEditOutput)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_IDE_CONTROL_MAP(CCustomBuildPage, IDD_CUSTOM_BUILD_TAB, IDS_CUSTOM_BUILD_TITLE)
	MAP_EDIT(IDC_CUSTOM_DESC, P_CustomDescription)
	MAP_EDIT(IDC_CUSTOM_CMDS, P_CustomCommand)
	MAP_EDIT(IDC_CUSTOM_OUTPUT, P_CustomOutputSpec)
//	{ IDC_CUSTOM_CMDS, IDC_CUSTOM_CMDS, P_CustomCommand, (CControlMap::CTL_TYPE)100, 0x0, NULL },
//	{ IDC_CUSTOM_OUTPUT, IDC_CUSTOM_OUTPUT, P_CustomOutputSpec, (CControlMap::CTL_TYPE)100, 0x0, NULL },
END_IDE_CONTROL_MAP()

BEGIN_MESSAGE_MAP(CUserDepsDlg, C3dDialog)
	//{{AFX_MSG_MAP(CUserDepsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
// User-defined dependencies dialog.
//
CUserDepsDlg::CUserDepsDlg( CStringArray& strArryUserDeps, CWnd* pParent ) : C3dDialog(IDD_USER_DEPS,pParent),
	m_strArryUserDeps(strArryUserDeps), m_bChanged(FALSE)
{
}

BOOL CUserDepsDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (HIWORD(wParam))
	{
	case GLN_CHANGE:
		m_bChanged = TRUE;
		break;

	case GLN_KILLFOCUS:
		m_gridUserDeps.AcceptControl();
		break;
	}
	return C3dDialog::OnCommand(wParam,lParam);
}

BOOL CUserDepsDlg::OnInitDialog()
{
	// create our grid controls	
	if (!m_gridUserDeps.ReplaceControl(this,
								   IDC_USER_DEPS_GRID, IDC_USER_DEPS_GRID,
								   WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_TABSTOP,
								   WS_EX_CLIENTEDGE, GRIDWND_TB_ALL) )
		return FALSE;	// failure

	// reset our grid
	m_gridUserDeps.ResetContent();

	// split our property string into this grid
	CString str;

	int i;

	for (i=0; i < m_strArryUserDeps.GetSize(); i++ )
	{
		CString strDep = m_strArryUserDeps[i];

		// strip first and last quotes
		int length = strDep.GetLength();
		if (3 <= length)
		{
			if ('\"' == strDep[0] && '\"' == strDep[length-1] )
			{
				strDep = strDep.Mid(1,length-2);
			}
		}
		(void)m_gridUserDeps.AddString(strDep);
	}

	m_gridUserDeps.SetFocus();

	return FALSE;	// Sets focus
}

void CUserDepsDlg::OnOK()
{
	//
	// Save
	// 
	m_gridUserDeps.AcceptControl(FALSE);
	if (m_bChanged)
	{
		int iItem = 0, cItems = m_gridUserDeps.GetCount();
		m_strArryUserDeps.RemoveAll();
		CString strDep;

		while (iItem < cItems)
		{
			// get our strings from the grid
			if (m_gridUserDeps.GetText(iItem, strDep))
			{
				// strip first and last quotes
				int length = strDep.GetLength();
				if (3 <= length)
				{
					if ('\"' == strDep[0] && '\"' == strDep[length-1] )
					{
						strDep = strDep.Mid(1,length-2);
					}
				}
				m_strArryUserDeps.Add(strDep);
			}

			// next item
			iItem++;
		}
		C3dDialog::OnOK();
	}
	else
	{
		C3dDialog::OnCancel();
	}
}

CCustomBuildPage::CCustomBuildPage() : m_bChanged(FALSE), m_bDontDeactivate(FALSE)
{
}

BOOL CCustomBuildPage::OnInitDialog()
{
	// call the base-class
	if (!CSlobPageTab::OnInitDialog())
		return FALSE;

#if 0
	// create our grid controls	
	if (!m_gridCmds.ReplaceControl(this,
								   IDC_CUSTOM_CMDS_PHOLDER, IDC_CUSTOM_CMDS,
								   WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_TABSTOP,
								   WS_EX_CLIENTEDGE, GRIDWND_TB_ALL) ||
		!m_gridOutput.ReplaceControl(this,
									 IDC_CUSTOM_OUTPUT_PHOLDER, IDC_CUSTOM_OUTPUT,
									 WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_TABSTOP,
									 WS_EX_CLIENTEDGE, GRIDWND_TB_ALL)
	   )
		return FALSE;	// failure

	m_gridCmds.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	m_gridOutput.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

#endif
	if (!m_editCmds.SubclassDlgItem(IDC_CUSTOM_CMDS, this))
		return FALSE;	// not ok

	if (!m_editOutput.SubclassDlgItem(IDC_CUSTOM_OUTPUT, this))
		return FALSE;	// not ok

	CEdit *pEditCmds = (CEdit *)GetDlgItem(IDC_CUSTOM_CMDS);
	// pEditCmds->SubclassDlgItem(IDC_CUSTOM_CMDS, this);
	CMenu *pMenu = pEditCmds->GetMenu();
	//pMenu->InsertMenu( 1, MF_BYPOSITION | MF_POPUP, (MENU_CONTEXT_POPUP(ToolDirMacro))->strID );
	//pMenu->InsertMenu( 2, MF_BYPOSITION | MF_POPUP, MENU_CONTEXT_POPUP(ToolDirMacro) );

	// load out menu buttons
	if (!m_btnDirMacro.SubclassDlgItem(IDC_DIR_MACRO, this))
		return FALSE;	// not ok

	if (!m_btnFileMacro.SubclassDlgItem(IDC_FILE_MACRO, this))
		return FALSE;	// not ok

	m_btnDirMacro.SetPopup(MENU_CONTEXT_POPUP(ToolDirMacro));
	m_btnFileMacro.SetPopup(MENU_CONTEXT_POPUP(ToolFileMacro));

	//
	// Do not use auto enable since there are no update command ui handlers.
	//
	m_btnDirMacro.m_pNewMenu->SetAutoEnable(FALSE);
	m_btnFileMacro.m_pNewMenu->SetAutoEnable(FALSE);

	// enable the directories/files buttons
	m_btnDirMacro.EnableWindow(TRUE);
	m_btnFileMacro.EnableWindow(TRUE);

	m_pEditLastFocus = &m_editCmds; // default
	m_pEditLastFocus->SetFocus();
	m_editOutput.SetModify(FALSE);
	m_bChanged = FALSE;
	m_bDontDeactivate = FALSE;

	return TRUE;	// ok
}

int CCustomBuildPage::OnCreate(LPCREATESTRUCT lpcs)
{
	return 0;	// ok
}

void CCustomBuildPage::OnUserDefinedDeps()
{
	CUserDepsDlg dlg(m_strArryUserDeps);

	if (IDOK==dlg.DoModal())
	{
		CString strDeps;
		int iItem = 0;
		int cItems = m_strArryUserDeps.GetSize();
		strDeps.Empty();

		while (iItem < cItems)
		{
			if (!m_strArryUserDeps[iItem].IsEmpty())
			{
				strDeps += "\"";
				strDeps += m_strArryUserDeps[iItem];
				strDeps += "\"\t";
			}
			// next item
			iItem++;
		}

		// set the prop.
		m_pSlob->SetStrProp(P_UserDefinedDeps, strDeps);
	}
}

void CCustomBuildPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if ((nIDCtl == IDC_FILE_MACRO || nIDCtl == IDC_DIR_MACRO) && 
		ODA_FOCUS==lpDrawItemStruct->itemAction && ODS_FOCUS==lpDrawItemStruct->itemState)
	{
		m_bDontDeactivate = TRUE;
	}
	CWnd::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CCustomBuildPage::OnActivateEditCmds()
{
	m_pEditLastFocus = &m_editCmds;

	// enable the directories/files buttons
	m_btnDirMacro.EnableWindow(TRUE);
	m_btnFileMacro.EnableWindow(TRUE);
}

void CCustomBuildPage::OnActivateEditOutput()
{
	m_pEditLastFocus = &m_editOutput;

	// enable the directories/files buttons
	m_btnDirMacro.EnableWindow(TRUE);
	m_btnFileMacro.EnableWindow(TRUE);
}

BOOL CCustomBuildPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
#if 1
	// split params
	HWND hwndCtl = (HWND)lParam;
	UINT idCtl = LOWORD(wParam);
	UINT nNotify = HIWORD(wParam);

	// killing focus from a grid control?
	if (idCtl == IDC_CUSTOM_CMDS || idCtl == IDC_CUSTOM_OUTPUT)
	{
		switch (nNotify) {
		case GLN_KILLFOCUS:
			{
				UINT idProp = (idCtl == IDC_CUSTOM_CMDS) ? P_CustomCommand : P_CustomOutputSpec;

				// get the contents of the grid list into the approp. prop
				// UNDONE:
				// (void) GetStrPropFromGrid(idProp, idCtl);
			}
			break;
		case GLN_ACCEPT_CTL:
			{
				// if (!m_pgridLastFocus || !m_pgridLastFocus->m_pActiveElement)
#if 0
				if (!m_pEditLastFocus /* || !m_pEditLastFocus->m_pActiveElement */)
				{
					m_btnDirMacro.EnableWindow(FALSE);
					m_btnFileMacro.EnableWindow(FALSE);
				}
#endif

				CEdit *pNew = (CEdit *)GetDlgItem(idCtl);
				if (pNew != m_pEditLastFocus)
				{
					// m_pgridLastFocus->AcceptControl();
					if (pNew == &m_editCmds || pNew == &m_editOutput)
					{
						m_pEditLastFocus = pNew;
						pNew->SetFocus();

						// enable the directories/files buttons
						m_btnDirMacro.EnableWindow(TRUE);
						m_btnFileMacro.EnableWindow(TRUE);
					}
				}
			}
			break;
		case GLN_ACTIVATE_CTL:
			{
#if 0
				CStringListGridWnd *pNew = (CStringListGridWnd *)GetDlgItem(idCtl);
				if (m_pgridLastFocus && pNew != m_pgridLastFocus)
					m_pgridLastFocus->AcceptControl();

				m_btnDirMacro.EnableWindow(TRUE);
				m_btnFileMacro.EnableWindow(TRUE);

				// remember this last grid focus
				m_pgridLastFocus = (CStringListGridWnd *)GetDlgItem(idCtl);
				ASSERT(m_pgridLastFocus != (CStringListGridWnd *)NULL);
#else
				CEdit *pNew = (CEdit *)GetDlgItem(idCtl);
				if (pNew != m_pEditLastFocus)
				{
					// m_pgridLastFocus->AcceptControl();
					if (pNew == &m_editCmds || pNew == &m_editOutput)
					{
						m_pEditLastFocus = pNew;
						pNew->SetFocus();

						// enable the directories/files buttons
						m_btnDirMacro.EnableWindow(TRUE);
						m_btnFileMacro.EnableWindow(TRUE);
					}
				}

				m_btnDirMacro.EnableWindow(TRUE);
				m_btnFileMacro.EnableWindow(TRUE);

				// remember this last grid focus
				// m_pgridLastFocus = (CStringListGridWnd *)GetDlgItem(idCtl);
				// ASSERT(m_pgridLastFocus != (CStringListGridWnd *)NULL);
#endif
			}
			break;
		}
	}

	BOOL bEnable = ((CEdit *)GetDlgItem(IDC_CUSTOM_CMDS))->IsWindowEnabled() || ((CEdit *)GetDlgItem(IDC_CUSTOM_OUTPUT))->IsWindowEnabled();

	// macro insertion?
	if (IDM_BLDTOOL_FIRSTMACRO <= wParam && wParam <= IDM_BLDTOOL_LASTMACRO)
	{
		const TCHAR * pchName;
		if (MapMacroIdToName((UINT)wParam, pchName))
		{
			// place this in the edit-box
			CString strMacro;
			strMacro += _TEXT("$(");
			strMacro += pchName;
			strMacro += _TEXT(")");

			// replace current selection and then set focus there
			//CStringListGridWnd * pGrid = m_pgridLastFocus != (CStringListGridWnd *)NULL ? m_pgridLastFocus : (CStringListGridWnd *)GetDlgItem(IDC_CUSTOM_CMDS);

			CEdit * pEdit = m_pEditLastFocus;

			// set the macro text in the grid?
			if (pEdit != (CEdit *)NULL)
			{
				pEdit->ReplaceSel((LPCSTR)strMacro);
				pEdit->SetFocus();
			}

			return TRUE; // handled by us
		}
	}

	return CSlobPageTab::OnCommand(wParam, lParam);	// not handled by us
#else
	CControlMap* pCtlMap = FindControl(LOWORD(wParam));
	CString strBuf;
	BOOL bString = FALSE;
	BOOL bValidateReq = FALSE;

	// macro insertion?
	if (IDM_BLDTOOL_FIRSTMACRO <= wParam && wParam <= IDM_BLDTOOL_LASTMACRO)
	{
		const TCHAR * pchName;
		if (MapMacroIdToName((UINT)wParam, pchName))
		{
			// place this in the edit-box
			CString strMacro;
			strMacro += _TEXT("$(");
			strMacro += pchName;
			strMacro += _TEXT(")");

			// replace current selection and then set focus there
			// CStringListGridWnd * pGrid = m_pgridLastFocus != (CStringListGridWnd *)NULL ? m_pgridLastFocus : (CStringListGridWnd *)GetDlgItem(IDC_CUSTOM_CMDS);

			CEdit * pEdit = m_pEditLastFocus;

			// set the macro text in the grid?
			if (pEdit != (CEdit *)NULL)
			{
				ASSERT(pEdit != NULL);
				pEdit->ReplaceSel((LPCSTR)strMacro);
				pEdit->SetFocus();
			}

			return TRUE; // handled by us
		}
	}

	if (m_pSlob == NULL || pCtlMap == NULL ||
		(m_nValidateID && LOWORD(wParam) != ((UINT) m_nValidateID) &&
		pCtlMap->m_nCtlType != CControlMap::check &&
		pCtlMap->m_nCtlType != CControlMap::radio))
	{
		// If the control is not found in the map, just pass this
		// message on for default processing...
		// Or if we have a validate ID already, this message is from
		// another control.
		return CSlobPageTab::OnCommand(wParam, lParam);	// not handled by us
	}

	UINT nNotify = HIWORD(wParam);

	ASSERT(m_pSlob != NULL);

	if (IDC_CUSTOM_CMDS==pCtlMap->m_nCtlID || IDC_CUSTOM_OUTPUT==pCtlMap->m_nCtlID)
	{

		if (nNotify == EN_CHANGE
			&& !m_bCustomToolPageIgnoreChange )
		{
			m_nValidateID = pCtlMap->m_nCtlID;
			m_bChanged = TRUE;
		}
#if 1
		else if (nNotify == GLN_KILLFOCUS)
		{
#if 0
#if 1
			if (!m_bDontDeactivate)
				((CStringListGridWnd *)GetDlgItem(pCtlMap->m_nCtlID))->AcceptControl(!m_bDontDeactivate);
#else
			((CStringListGridWnd *)GetDlgItem(pCtlMap->m_nCtlID))->AcceptControl(!m_bDontDeactivate);
#endif
#else
			// UNDONE:
#endif
			m_bDontDeactivate = FALSE;
//			bValidateReq = TRUE;
		}
		else if (nNotify == GLN_ACCEPT_CTL)
		{
			// if (!m_pgridLastFocus || !m_pgridLastFocus->m_pActiveElement)
#if 0
			if (!m_pEditLastFocus /* || !m_pEditLastFocus->m_pActiveElement */)
			{
				m_btnDirMacro.EnableWindow(FALSE);
				m_btnFileMacro.EnableWindow(FALSE);
			}
#endif
			bValidateReq = TRUE;
		}
		else if (nNotify == GLN_ACTIVATE_CTL)
		{
#if 0
			CStringListGridWnd *pNew = (CStringListGridWnd *)GetDlgItem(pCtlMap->m_nCtlID);
			if (m_pgridLastFocus && pNew != m_pgridLastFocus)
				m_pgridLastFocus->AcceptControl();
#else
			CEdit *pNew = (CEdit *)GetDlgItem(pCtlMap->m_nCtlID);
			if (pNew != m_pEditLastFocus)
			{
				if (pNew == &m_editCmds || pNew == &m_editOutput)
				{
					m_pEditLastFocus = pNew;
					pNew->SetFocus();

					// enable the directories/files buttons
					m_btnDirMacro.EnableWindow(TRUE);
					m_btnFileMacro.EnableWindow(TRUE);
				}
			}

#endif

			m_btnDirMacro.EnableWindow(TRUE);
			m_btnFileMacro.EnableWindow(TRUE);

			// remember this last grid focus
			// m_pgridLastFocus = (CStringListGridWnd *)GetDlgItem(pCtlMap->m_nCtlID);
			// ASSERT(m_pgridLastFocus != (CStringListGridWnd *)NULL);
			// ASSERT(m_pEditLastFocus != (CEdit *)NULL);
		}
#endif
		else
		{
			return CSlobPageTab::OnCommand(wParam, lParam);
		}

	}
	else
	{
		return CSlobPageTab::OnCommand(wParam, lParam);
	}

	if (bValidateReq)
	{
		PostMessage(WM_USER_VALIDATEREQ);
		return CDlgTab::OnCommand(wParam, lParam);
	}

	return TRUE;
#endif
}

void CCustomBuildPage::InitPage()
{
	// call base-class
	CSlobPageTab::InitPage();

	m_bChanged = FALSE;

	CString strInput;

	BOOL bOnlyFileItems = FALSE;

	// set up out input file text
	if (m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)))
	{
		CProxySlob * pProxySlob = (CProxySlob *)m_pSlob;
		// single selection?

		bOnlyFileItems = pProxySlob->IsSortOf(RUNTIME_CLASS(CFileItem));

		if (pProxySlob->IsSingle())
		{
			OptTreeNode * pNode = pProxySlob->GetSingle();
			CProjItem * pItem = pNode->pItem;
			
			CProjTempConfigChange projTempConfigChange(pItem->GetProject());

			projTempConfigChange.ChangeConfig((ConfigurationRecord *)pNode->pcr->m_pBaseRecord);

			const CPath * pPath = NULL;

			// single target?
			if (pProxySlob->IsSortOf(RUNTIME_CLASS(CProject)))
			{	
				// try looking right at our actions
				// we may not have a dependency graph so enumerating it will not work
				CActionSlobList * pActions = pItem->GetProject()->GetActiveConfig()->GetActionList();
				POSITION pos = pActions->GetHeadPosition();
				ASSERT(NULL!=pos);

				while (pos != (POSITION)NULL)
				{
					CActionSlob * pAction = (CActionSlob *)pActions->GetNext(pos);

					// primary output tool?
					if (!pAction->m_pTool->HasPrimaryOutput())
						continue;	// no

					int idOldBag = pAction->Item()->UsePropertyBag(CloneBag);
					pAction->DirtyOutput();
					CFileRegSet * pSet = pAction->GetOutput();
					pAction->Item()->UsePropertyBag(idOldBag);					 

					if (pSet->GetCount())
					{
						FileRegHandle frh = pSet->GetFirstFrh();
						pPath = g_FileRegistry.GetRegEntry(frh)->GetFilePath();
						ASSERT(pPath != (const CPath *)NULL);
						frh->ReleaseFRHRef();
						break;
					}
				}
			}
			// single file?
			else if (pProxySlob->IsSortOf(RUNTIME_CLASS(CFileItem)))
			{
				pPath = pItem->GetFilePath();
				ASSERT(pPath != (const CPath *)NULL);
			}
			else
			{
				//
				// Expecting one or the other.
				//
				ASSERT(FALSE);
			}

			ASSERT(pPath != (const CPath *)NULL);

			if (NULL!=pPath)
			{
				// get filename relative to project directory
				CDir dirProj = pItem->GetProject()->GetWorkspaceDir();
				if (!pPath->GetRelativeName(dirProj, strInput))
					// just use absolute
					strInput = (const TCHAR *)*pPath;
			}
		}
		else
		{
			VERIFY(strInput.LoadString(IDS_MULTI_SELECTION));
		}
	}
	else
	{
		ASSERT(FALSE);	// not supported
	}

	GetDlgItem(IDC_CUSTOM_INPUT)->SetWindowText(strInput);

#if 0
	// set up the contents of the build command(s) list from the props
	(void) SetStrPropToGrid(P_CustomCommand, IDC_CUSTOM_CMDS);

	// set up the contents of the output file(s) list from the props
	(void) SetStrPropToGrid(P_CustomOutputSpec, IDC_CUSTOM_OUTPUT);
#endif

	//
	// User-defined dependencies
	//
	if (bOnlyFileItems)
	{
		CString str;
		m_strArryUserDeps.RemoveAll();

		// get the prop.
		OptBehaviour optbeh = ((CProjItem *)m_pSlob)->GetOptBehaviour();
		
		// no inheritance!
		(void) ((CProjItem *)m_pSlob)->SetOptBehaviour(optbeh & ~OBInherit);

		BOOL fOk = m_pSlob->GetStrProp(P_UserDefinedDeps, str) == valid;

		// reset
		(void) ((CProjItem *)m_pSlob)->SetOptBehaviour(optbeh);

		// got prop. ok?
		if (fOk)
		{
			// split up our string
			COptionList optlst(_T('\t'), FALSE,TRUE);
			optlst.SetString(str);

			POSITION pos = optlst.GetHeadPosition();
			while (pos != (POSITION)NULL)
			{
				CString strDep = optlst.GetNext(pos);

				// strip first and last quotes
				int length = strDep.GetLength();
				if (3 <= length)
				{
					if ('\"' == strDep[0] && '\"' == strDep[length-1] )
					{
						strDep = strDep.Mid(1,length-2);
					}
				}
				(void)m_strArryUserDeps.Add(strDep);
			}
		}
	}
	else
	{
		// Hide dependencies button.
		GetDlgItem(IDC_USER_DEPS)->ShowWindow(SW_HIDE);
	}

#if 1
	if (m_pEditLastFocus == NULL) // REVIEW
	{
		m_pEditLastFocus = &m_editCmds; // default
	}
	m_pEditLastFocus->SetFocus();

	// enable the directories/files buttons
	m_btnDirMacro.EnableWindow(TRUE);
	m_btnFileMacro.EnableWindow(TRUE);
#else
	m_pEditLastFocus = NULL;

	// disable the directories/files buttons
	m_btnDirMacro.EnableWindow(FALSE);
	m_btnFileMacro.EnableWindow(FALSE);
#endif
}

BOOL CCustomBuildPage::OnPropChange(UINT nProp)
{
	BOOL bChanged = TRUE;
	CString str;

	if ( nProp != P_CustomCommand && nProp != P_CustomOutputSpec )
	{
		return CSlobPageTab::OnPropChange(nProp);
	} else {
			// get the tool string prop, don't use inheritance!
		OptBehaviour optbeh = ((CProjItem *)m_pSlob)->GetOptBehaviour();
		
		// no inheritance!
		(void) ((CProjItem *)m_pSlob)->SetOptBehaviour(optbeh & ~OBInherit);
	
		BOOL b = CSlobPageTab::OnPropChange(nProp);
	
		// reset
		(void) ((CProjItem *)m_pSlob)->SetOptBehaviour(optbeh);

		CControlMap* pCtlMap = FindProp(nProp);
		CWnd* pWndCtl = GetDlgItem(pCtlMap->m_nCtlID);
		CWnd* pPromptWnd = GetDlgItem(pCtlMap->m_nCtlID - 1);
		EnableControl(pWndCtl,valid);
		EnableControl(pPromptWnd,valid);

		return b;
	}

#if 0

	CControlMap* pCtlMap = FindProp(nProp);
	CWnd* pWndCtl = GetDlgItem(pCtlMap->m_nCtlID);

	GPT gpt = m_pSlob->GetStrProp(pCtlMap->m_nProp, str);

	CString strWindow;
	GetStrFromGrid((CStringListGridWnd*)pWndCtl,strWindow);

	if (gpt == invalid || gpt == ambiguous)
		str = "";	// invalid or indeterminate

	if (str != strWindow)
	{
		m_bCustomToolPageIgnoreChange = TRUE;

		SetStrFromGrid((CStringListGridWnd*)pWndCtl,str);

		m_bCustomToolPageIgnoreChange = FALSE;
	}
	else
	{
		bChanged = FALSE;
	}
	EnableControl(pWndCtl, valid);
//	EnableControl(pPromptWnd, gpt);

#endif

	return bChanged;
}

BOOL CCustomBuildPage::Validate()
{

	if (m_nValidateID == NULL)
		return TRUE;

	ASSERT(m_pSlob != NULL);

	CControlMap* pCtlMap = FindControl(m_nValidateID);
	ASSERT(pCtlMap != NULL);

	// Handle are special controls.
	switch (pCtlMap->m_nCtlID)
	{
	case IDC_CUSTOM_OUTPUT:
	case IDC_CUSTOM_CMDS:
		{
			BOOL bValid = TRUE;
			CWnd* pCtl = GetDlgItem(pCtlMap->m_nCtlID);
			ASSERT(pCtl != NULL);

			CString strBuf;
#if 0
			if (!((CStringListGridWnd*)pCtl)->AcceptControl(FALSE))
				return FALSE;
	
			RemoveBlankLinesFromGrid(*(CStringListGridWnd*)pCtl);
			GetStrFromGrid((CStringListGridWnd*)pCtl,strBuf);
#else
			pCtl->GetWindowText(strBuf);
#endif

			CString strCurVal;
			if (m_pSlob->GetStrProp(pCtlMap->m_nProp, strCurVal) == valid)
			{
 				if (strCurVal == strBuf)
				{
					m_nValidateID = NULL;
					return TRUE;
				}
			}

			bValid = m_pSlob->SetStrProp(pCtlMap->m_nProp, strBuf);

			OnPropChange(pCtlMap->m_nProp);

			if (bValid)
				m_nValidateID = NULL;
			else
				pCtl->SetFocus();

			return bValid;
		}
		break;

	default:
		return CSlobPageTab::Validate();
		break;
	}
	ASSERT(0);
	return FALSE;
}

BOOL CCustomBuildPage::ValidateTab()
{
	//
	// Get changes in active control.
	//
#if 0
	m_gridCmds.AcceptControl(FALSE);
	m_gridOutput.AcceptControl(FALSE);

	if (m_bChanged)
	{
		//
		// Remove blank lines.
		//
		RemoveBlankLinesFromGrid(m_gridCmds);
		RemoveBlankLinesFromGrid(m_gridOutput);

		// If we have a tool make sur we have some output
		if ( m_gridCmds.GetCount() && !m_gridOutput.GetCount() ) {
			AfxMessageBox( IDS_ERR_CUSTOM_OUTPUT, MB_OK | MB_ICONEXCLAMATION );

			//
			// Set focus on output files.
			//
			m_gridOutput.ActivateNextControl();
			return FALSE;
		}
	}
#else
	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
	CProxySlob * pProxySlob = (CProxySlob *)m_pSlob;

	// can only validate output not blank if not-multiselect or
	// if it changed
	if (m_editOutput.GetModify() || (pProxySlob->IsSingle()))
	{
		CString strCmds, strOutput;
		m_editCmds.GetWindowText(strCmds);
		m_editOutput.GetWindowText(strOutput);
		strCmds.TrimLeft();
		strOutput.TrimLeft();
		if ((!strCmds.IsEmpty()) && (strOutput.IsEmpty()))
		{
			AfxMessageBox( IDS_ERR_CUSTOM_OUTPUT, MB_OK | MB_ICONEXCLAMATION );
			m_pEditLastFocus = &m_editOutput;
			m_editOutput.SetFocus();

			// enable the directories/files buttons
			m_btnDirMacro.EnableWindow(TRUE);
			m_btnFileMacro.EnableWindow(TRUE);
			return FALSE;
		}
	}
#endif
    return CSlobPageTab::ValidateTab();
}

#if 0
void CCustomBuildPage::CommitTab()
{ 
	// Make sure we save the most recent grid info
	GetStrPropFromGrid(P_CustomCommand, IDC_CUSTOM_CMDS);
	GetStrPropFromGrid(P_CustomOutputSpec, IDC_CUSTOM_OUTPUT);


	// set up out input file text
	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
	CProxySlob * pProxySlob = (CProxySlob *)m_pSlob;
	BOOL bOnlyFileItems = pProxySlob->IsSortOf(RUNTIME_CLASS(CFileItem));

	//
	// Commit user-defined dependencies
	//
#if 0
	//
	// Code moved to CCustomBuildPage::OnUserDefinedDeps().  May need to move it
	// back here when #5410 is fixed...Currently overwriting intersection of propreties with value
	// of intersection even though the user has made no changes.  Worse case, user has multiple selection
	// with blank intersection, selects OK and propreties are overwritten with blank.
	//
	if (bOnlyFileItems)
	{
		CString strDeps;
		int iItem = 0;
		int cItems = m_strArryUserDeps.GetSize();
		strDeps.Empty();

		while (iItem < cItems)
		{
			if (!m_strArryUserDeps[iItem].IsEmpty())
			{
				strDeps += "\"";
				strDeps += m_strArryUserDeps[iItem];
				strDeps += "\"\t";
			}
			// next item
			iItem++;
		}

		// set the prop.
		m_pSlob->SetStrProp(P_UserDefinedDeps, strDeps);
	}
#endif

	// call base-class
	CSlobPageTab::CommitTab();
}
#endif

#if 0
BOOL CCustomBuildPage::GetStrPropFromGrid(UINT idProp, UINT idGridCtl)
{
	// get the grid control
	CStringListGridWnd * pGrid = (CStringListGridWnd *)GetDlgItem(idGridCtl);
	ASSERT(pGrid != (CStringListGridWnd *)NULL);

	// accept the active control
	// no activation and no move
	CStringListGridRow::c_bIsMove = TRUE;
	(void)pGrid->AcceptControl(FALSE);
	CStringListGridRow::c_bIsMove = FALSE;

	// get the string from the grid and collate into property string
	COptionList optlst(_T('\t'), FALSE);
	CString str;

	int iItem = 0, cItems = pGrid->GetCount();
	while (iItem < cItems)
	{
		// get our strings from the grid
		if (pGrid->GetText(iItem, str))
			optlst.Append((const TCHAR *)str, str.GetLength());

		// next item
		iItem++;
	}

	// set the prop.
	optlst.GetString(str);
	return m_pSlob->SetStrProp(idProp, str);
}
#endif

BOOL CCustomBuildPage::SetStrPropToGrid(UINT idProp, UINT idGridCtl)
{
	BOOL fRet = FALSE;

	// get the grid control
	CStringListGridWnd * pGrid = (CStringListGridWnd *)GetDlgItem(idGridCtl);
	ASSERT(pGrid != (CStringListGridWnd *)NULL);

	// reset our grid
	pGrid->ResetContent();

	// split our property string into this grid
	CString str;

	// get the prop.
	OptBehaviour optbeh = ((CProjItem *)m_pSlob)->GetOptBehaviour();
	
	// no inheritance!
	(void) ((CProjItem *)m_pSlob)->SetOptBehaviour(optbeh & ~OBInherit);

	BOOL fOk = m_pSlob->GetStrProp(idProp, str) == valid;

	// reset
	(void) ((CProjItem *)m_pSlob)->SetOptBehaviour(optbeh);

	// got prop. ok?
	if (fOk)
	{
#if 0
		// split up our string
		COptionList optlst(_T('\t'), FALSE);
		optlst.SetString(str);

		POSITION pos = optlst.GetHeadPosition();
		while (pos != (POSITION)NULL)
			(void)pGrid->AddString(optlst.GetNext(pos));

		fRet = TRUE;
#else
		SetStrFromGrid(pGrid,str);
#endif
	}

	return fRet;
}

// get the value of a macro
BOOL GetMacroValue
(
	UINT idMacro,
	CString & strMacroValue,
	CActionSlob * pAction
)
{
	CProjItem * pItem = pAction->Item();
	CString strTemp;

	// empty macro-value
	strMacroValue.Empty();

	// is this a directory macro?
	BOOL fIsDir = FALSE;

	switch (idMacro)
	{
		// intermediate directory
		case IDMACRO_INTDIR:

		// output directory
		case IDMACRO_OUTDIR:
		{
			fIsDir = TRUE;

			// directory prop?
			UINT idOutDirProp = idMacro == IDMACRO_INTDIR ?
								 P_OutDirs_Intermediate : P_OutDirs_Target;

			GPT gpt = pItem->GetStrProp(idOutDirProp, strMacroValue);
			while (gpt != valid)
			{
				pItem = (CProjItem *)pItem->GetContainer();
				ASSERT(pItem != (CSlob *)NULL);
				gpt = pItem->GetStrProp(idOutDirProp, strMacroValue);
            }

			if (strMacroValue.IsEmpty())
			{
				strMacroValue = _T('.');
			}
			else
			{
				const TCHAR * pch = (const TCHAR *)strMacroValue;

				// prepend ".\\" if not absolute or UNC path
				if ((pch[0] != _T('\\')) && (pch[0] != _T('/')) &&
					(!(pch[0] == _T('.') && pch[1] == _T('\\'))) &&
					(!IsDBCSLeadByte (pch[0]) && (pch[1] != _T(':'))) || (IsDBCSLeadByte (pch[0])) )
					strMacroValue = _T(".\\") + strMacroValue;

				//StripTrailingSlash(strMacroValue);
			}
			break;
		}

		// IDE directory
		case IDMACRO_IDEDIR:
		{
			fIsDir = TRUE;

			// get the path of the shell
			TCHAR szPath[_MAX_PATH];
			GetModuleFileName(theApp.m_hInstance, szPath, _MAX_PATH);

			// get the directory of the shell (this is the IDE dir.)
			CPath path;
			VERIFY(path.Create(szPath));

			CDir dir;
			if (dir.CreateFromPath(path))
			{
				// path to the binaries, ie. \ide\bin
				strMacroValue = (const TCHAR *)dir;

				// remove \bin
				TCHAR * pLastSlash = _tcsrchr(strMacroValue, _T('\\'));
				if (pLastSlash != (TCHAR *)NULL)
					// truncate
					strMacroValue.GetBufferSetLength(pLastSlash - (const TCHAR *)strMacroValue);
			}

			break;
		}

		// workspace directory
		case IDMACRO_WKSPDIR:
		{
			fIsDir = TRUE;

			// fall thru'
		}

		// workspace file basename
		case IDMACRO_WKSPBASE:
		{	
			CPath pathWks;

			// Get proper workspace dir
			ASSERT(g_pProjWksIFace);
			LPCSTR pszPath;
			VERIFY(SUCCEEDED(g_pProjWksIFace->GetWorkspaceDocPathName(&pszPath)));
			if((pszPath==NULL) || !(*pszPath))
				pszPath = ".";

			VERIFY(pathWks.Create(pszPath));

			if (idMacro == IDMACRO_WKSPDIR)
			{
				CDir dir;
				if (dir.CreateFromPath(pathWks))
					strMacroValue = (const TCHAR *)dir;
			}
			else
			{
				ASSERT(idMacro == IDMACRO_WKSPBASE);
				pathWks.GetBaseNameString(strMacroValue);
			}
			break;
		}

		// directory of the remote target
		case IDMACRO_REMOTEDIR:
		{
			fIsDir = TRUE;

			// fall thru'
		}

		// full path of remote target
		case IDMACRO_REMOTEFILE:
		{
			CProject * pProject = pItem->GetProject();

			if (!pProject->GetStrProp(P_RemoteTarget, strMacroValue))
				strMacroValue = _TEXT("");

			// directory?
			if (idMacro == IDMACRO_REMOTEDIR && !strMacroValue.IsEmpty())
			{
				const CPlatform * pPlatform = pProject->GetProjType()->GetPlatform();
				UINT uPlatform = pPlatform->GetUniqueId();

				// is this a UNC directory, ie. not Macintosh?
				BOOL fRemoteIsUNC = uPlatform != mac68k &&
                    uPlatform != macppc && uPlatform != xbox;
				if (fRemoteIsUNC)
				{
					// extract directory from UNC name
					CPath path;
					if (path.Create(strMacroValue))
					{
						CDir dir;
						if (dir.CreateFromPath(path))
							strMacroValue = (const TCHAR *)dir;
					}
				}
				else
				{
					// extract directory from non-UNC name
					// where is the last ':'
					const TCHAR * pchMacroValue = (const TCHAR *)strMacroValue;
					TCHAR * pchLastColon;
                    if(uPlatform == xbox && pchMacroValue[0] != 'x' &&
                        pchMacroValue[0] != 'X')
                    {
                        /* Need to prepend the 'x' to keep the file tools
                         * happy */
                        strMacroValue = "x" + strMacroValue;
                        pchMacroValue = (const TCHAR *)strMacroValue;
                    }
                    if(uPlatform == xbox)
                        pchLastColon = _tcsrchr(pchMacroValue, _T('\\'));
                    else
                        pchLastColon = _tcsrchr(pchMacroValue, _T(':'));
					if (pchLastColon != (TCHAR *)NULL)
						strMacroValue.ReleaseBuffer(pchLastColon - pchMacroValue);
				}
			}
			break;
		}

		// project directory
		case IDMACRO_PROJDIR:
		{
			fIsDir = TRUE;

			strMacroValue = (const TCHAR *)pItem->GetProject()->GetProjDir();
			CPath path;
			if (path.Create(strMacroValue))
			{
				CDir * pWorkspaceDir = &pItem->GetProject()->GetWorkspaceDir();

				if (_tcsicmp(strMacroValue, (const TCHAR *)*pWorkspaceDir)==0)
				{
					strMacroValue = _TEXT(".");
				}
				else if (path.GetRelativeName(*pWorkspaceDir, strTemp, FALSE, FALSE))
				{
					strMacroValue = strTemp;
				}
			}
			break;
		}

		// target directory
		case IDMACRO_TARGDIR:
		{
			fIsDir = TRUE;

			// fall thru'
		}

		// full path of target
		case IDMACRO_TARGFILE:

		// target file basename
		case IDMACRO_TARGBASE:
		{
			CPath * pPath = pItem->GetProject()->GetTargetFileName();
			if (pPath == (const CPath *)NULL)
				break;

			if (idMacro == IDMACRO_TARGDIR)
			{
				CDir dir;
				if (dir.CreateFromPath(*pPath))
				{
					strMacroValue = (const TCHAR *)dir;
					CPath path;
					if (path.Create(strMacroValue))
					{
						CDir * pProjDir = &pItem->GetProject()->GetWorkspaceDir();

						if (path.GetRelativeName(*pProjDir, strTemp, FALSE, FALSE))
						{
							strMacroValue = strTemp;
						}
					}
				}
			}
			else
			if (idMacro == IDMACRO_TARGFILE)
			{
				if (!pPath->GetRelativeName(pItem->GetProject()->GetWorkspaceDir(), strMacroValue))
					strMacroValue = (const TCHAR *)*pPath;
			}
			else
			{
				ASSERT(idMacro == IDMACRO_TARGBASE);
				pPath->GetBaseNameString(strMacroValue);
			}
			delete pPath;
			break;
		}

		// input directory
		case IDMACRO_INPTDIR:
		{

			fIsDir = TRUE;

			// fall thru'
		}

		// full path of input
		case IDMACRO_INPTFILE:

		// input file basename
		case IDMACRO_INPTBASE:
		{
			// first file in input is the 'input'
			CFileRegSet * pregset = pAction->GetInput();
			pregset->InitFrhEnum();

			FileRegHandle frh = pregset->NextFrh();
			if (frh == (FileRegHandle)NULL)
				return FALSE;

			const CPath * pPath = g_FileRegistry.GetRegEntry(frh)->GetFilePath();
#ifdef REFCOUNT_WORK
			frh->ReleaseFRHRef();
#endif

			if (idMacro == IDMACRO_INPTDIR)
			{
				CDir dir;
				if (dir.CreateFromPath(*pPath))
				{
					strMacroValue = (const TCHAR *)dir;
					CPath path;
					if (path.Create(strMacroValue))
					{
						CDir * pProjDir = &pItem->GetProject()->GetWorkspaceDir();

						if (_tcsicmp(strMacroValue, (const TCHAR *)*pProjDir)==0)
						{
							strMacroValue = _TEXT(".");
						}
						else if (path.GetRelativeName(*pProjDir, strTemp, FALSE, FALSE))
						{
							strMacroValue = strTemp;
						}
					}
				}
			}
			else
			if (idMacro == IDMACRO_INPTFILE)
			{
				if ((pItem->GetStrProp(P_ProjItemOrgPath, strMacroValue) == valid) && (!strMacroValue.IsEmpty()))
				{
					// great: use persist-as value for InputPath
				}
				else if (!pPath->GetRelativeName(pItem->GetProject()->GetWorkspaceDir(), strMacroValue))
				{
					strMacroValue = (const TCHAR *)*pPath;
				}
			}
			else
			{
				ASSERT(idMacro == IDMACRO_INPTBASE);
				pPath->GetBaseNameString(strMacroValue);
			}
			break;
		}

		// didn't deal with this
		default:
			return FALSE;	// invalid
	}

	// directory or file?
#if 0
	if (!strMacroValue.IsEmpty())
	{
		// quote the macro if it isn't already
		if( strMacroValue.Find('"') < 0 ) {
			// strMacroValue = _T('"') + strMacroValue + _T('"');
		}
	}
#endif

	return TRUE;	// ok
}

#define CCH_ALLOC_EXPAND	512

BOOL ExpandMacros
(
	CString * pstrOut,
	const TCHAR * pchIn,
	CActionSlob * pAction,
	BOOL	bNoEnvVars
)
{
	// just test the usage
	BOOL fJustTestUsage = pstrOut == (CString *)NULL;

	ASSERT(pchIn != (TCHAR *)NULL);

	TCHAR * pchSpec = (TCHAR *)pchIn;

	int cchOut, cchAlloc;
	TCHAR * pchOut;

	if (!fJustTestUsage)
	{
		cchAlloc = cchOut = CCH_ALLOC_EXPAND;
		pchOut = (TCHAR *)malloc(cchAlloc * sizeof(TCHAR));
		*pchOut = _T('\0'); cchOut -= sizeof(TCHAR);
	}

	CString strMacroValue;

	// copy everything verbatim unless we come across a macro
	while (*pchSpec != _T('\0'))
	{
		// macro?
		TCHAR * pch = pchSpec;
		while (!(*pch == _T('$') && *(pch + sizeof(TCHAR)) == _T('(')) && *pch != _T('\0'))
			pch = _tcsinc(pch);

		int cchReq = 0; 
		const TCHAR * pchBlk = (const TCHAR *)NULL;

		// do block append?
		if (pch != pchSpec)
		{
			// yes
			cchReq = pch - pchSpec;

			// get block start
			pchBlk = pchSpec;

			// advance
			pchSpec = pch;
		}
		else
		{
			UINT idMacro; int cchMacro;

			// macro-name exact match?
			// skip '$('
			pch += sizeof(TCHAR) * 2;

			if (MapMacroNameToId(pch , cchMacro, idMacro) &&
				*(pch + cchMacro) == _T(')'))
			{
				// advance and skip macro
				pchSpec = pch + cchMacro;

				// skip trailing ')'
				pchSpec++;

				// this macro is now used
				SetMacroIdUsage(TRUE, idMacro);

				// get the macro value
				if ( (!fJustTestUsage) && GetMacroValue(idMacro, strMacroValue, pAction))
				{
					// get the char. block
					cchReq = strMacroValue.GetLength();
					pchBlk = (const TCHAR *)strMacroValue;
				}
			}
			else if( !bNoEnvVars )
			{
				TCHAR macro[1024];
				TCHAR value[1024];
				TCHAR *pchStart;

				// save the start
				pchStart = pch;

				// Find last char of Macro
				while (!(*pch == _T(')')) && *pch != _T('\0') )
					pch = _tcsinc(pch);

				if( *pch == _T('\0') ){
					// stand alone "$(" with no trailing ")"
					strMacroValue = "$(";
					pchBlk = (const TCHAR *)strMacroValue;
					cchReq = strMacroValue.GetLength();
					pchSpec = pchStart;
				} else {

					cchMacro = pch - pchStart;
					_tcsncpy( macro, pchStart, cchMacro );
					macro[cchMacro] = _T('\0');
	
					if( GetEnvironmentVariable( macro, value, 1023 ) ){
						strMacroValue = value;
						pchBlk = (const TCHAR *)strMacroValue;
						cchReq = strMacroValue.GetLength();
					}
					else {
						PBUILD_TRACE("warning: unresolved Environment Variable  \"$(%s)\"\n", (const char *)macro );
						CBuildIt::m_mapMissingEnvVars.SetAt(macro, NULL);
					}
	
					if( *pch == _T(')') )
						pch = _tcsinc(pch);

					// advance and skip macro
					pchSpec = pch;
				}
			}
			else {
				strMacroValue = "$(";
				pchBlk = (const TCHAR *)strMacroValue;
				cchReq = strMacroValue.GetLength();
				pchSpec = pch;
			}
		} 

		// not just testing usage and chars. req'd and char blk to copy?
		if (!fJustTestUsage && cchReq && pchBlk)
		{
			if (cchReq > cchOut)
			{
				int cchIncAlloc = max(CCH_ALLOC_EXPAND, cchReq);

				cchAlloc += cchIncAlloc; cchOut += cchIncAlloc;
				pchOut = (TCHAR *)realloc(pchOut, cchAlloc * sizeof(TCHAR));
			}
						
			// copy block
			_tcsncat(pchOut, pchBlk, cchReq); cchOut -= cchReq;
		}
	}

	// copy into CString
	if (!fJustTestUsage)
	{
		(*pstrOut) = pchOut;

		// free up our local buffer
		free(pchOut);
	}

	return TRUE;
}
