/////////////////////////////////////////////////////////////////////////////
// CProjectConfigurationsDlg - "Project Configurations" dialog box.

#include "stdafx.h"
#pragma hdrstop
#include "bldslob.h"
#include "vwslob.h"
#include "configs.h"
#include "prjdlg.h"
#include "targdlg.h"
#include "awiface.h"
#include "targctrl.h"

#include <prjapi.h>
#include <prjguid.h>
#include "oleref.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// Subprojects dialog.

class CSubprojectCheckList : public CCheckList
{
public:
	
	struct CSubprojectCheckItem
	{
		HFILESET m_hFileSet;
		HPROJDEP m_hProjDep;
		BOOL m_bChecked : 1;
		BOOL m_bOriginalCheck : 1;
	};

	afx_msg void OnDestroy();
	
	void ResetContent();
	int AddItem(const TCHAR* szItemText, BOOL bChecked, HFILESET hFileSet, HPROJDEP hProjDep);
	
	void SetCheck(int nItem, int bCheck);
	int GetCheck(int nItem);
	
	void SetOriginalCheck(int nItem, int bCheck);
	int GetOriginalCheck(int nItem);
	
	void SetFileSet(int nItem, HFILESET hFileSet);
	HFILESET GetFileSet(int nItem);

	void SetProjDep(int nItem, HPROJDEP hProjDep);
	HPROJDEP GetProjDep(int nItem);

	int m_nMaxWidth;

	DECLARE_MESSAGE_MAP()
};

class CSubprojectsDlg : public C3dDialog
{
// Construction
public:
	CSubprojectsDlg(HFILESET hProject, CWnd* pParent = NULL);

// Dialog Data
public:
	CSubprojectCheckList m_subprojects;

// Overrides
protected:

// Implementation
protected:
	BOOL OnAccept();
	void OnSelProject();
	
	HFILESET m_hProject;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual void OnOK();
#if 0
	afx_msg void OnNew();
#endif

	void FillCurrent();
	void FillAvailable();

	DECLARE_MESSAGE_MAP()
};


void ChangeOKtoClose(CDialog* pDialog)
{
	CString str;
	VERIFY(str.LoadString(IDS_CLOSE));
	pDialog->SetDlgItemText(IDOK, str);
	pDialog->GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CProjectConfigurationsDlg - construction
CProjectConfigurationsDlg::CProjectConfigurationsDlg(CWnd* pParent /*=NULL*/)
	: C3dDialog(IDD_PROJECTS, pParent)
{
}

CProjectConfigurationsDlg::~CProjectConfigurationsDlg()
{
}

/////////////////////////////////////////////////////////////////////////////
// CProjectConfigurationsDlg - message map, data exchange.
void CProjectConfigurationsDlg::DoDataExchange(CDataExchange* pDX)
{
	C3dDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CONFIGURATIONS_TREE, m_ConfigurationsTree);
}

BEGIN_MESSAGE_MAP(CProjectConfigurationsDlg, C3dDialog)
	ON_COMMAND(IDC_ADD, OnAddConfiguration)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_NOTIFY(TVN_SELCHANGED, IDC_CONFIGURATIONS_TREE, OnSelectConfiguration)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CProjectConfigurationsDlg - initialization

BOOL CProjectConfigurationsDlg::OnInitDialog()
{
	if (!C3dDialog::OnInitDialog())
		return FALSE;

	// Fill the tree.
	FillTree();

	return TRUE;
}

void CProjectConfigurationsDlg::OnOK()
{
	// Flush the undo buffer
	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
	VERIFY (SUCCEEDED (pProjSysIFace->FlushUndo ()));

	CObList * plstCombos;
	POSITION pos;
	plstCombos = CConfigCombo::GetCombos();
	pos = plstCombos->GetHeadPosition();
	while (pos != NULL)
	{
		CConfigCombo * pTargetCombo = (CConfigCombo *)plstCombos->GetNext(pos);
		pTargetCombo->RefreshTargets();
	}
	
	C3dDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CProjectConfigurationsDlg - message handlers
void CProjectConfigurationsDlg::OnAddConfiguration()
{
	// Get the projects name..
	HTREEITEM hItem = m_ConfigurationsTree.GetSelectedItem();
	if (IsConfig(hItem))
		hItem = m_ConfigurationsTree.GetParentItem(hItem);

	CString	strProject = GetProjectName(hItem);

	// Let the user create a new configuration.
	CNewProjectCopyDlg dlg(strProject, this);

	if (dlg.DoModal() == IDOK)
	{
		// don't do FillTree() here, because it will fully expand trees

		// Enumerate the children of this project tree item, and delete each one.
		HTREEITEM hConfigItem = m_ConfigurationsTree.GetChildItem(hItem);
		while (hConfigItem != NULL)
		{
			m_ConfigurationsTree.DeleteItem(hConfigItem);

			// Get the next item in the tree.
			hConfigItem = m_ConfigurationsTree.GetChildItem(hItem);
		}

		HFILESET hFileSet = (HFILESET)m_ConfigurationsTree.GetItemData(hItem);
		HBUILDER hBld = g_BldSysIFace.GetBuilderFromFileSet(hFileSet);

		// Get the fileset name.
		CString strFileset;

		if (!g_BldSysIFace.GetFileSetName(hFileSet, strFileset, hBld))
			return;

		// Now get the configurations for this project.
		CStringList slConfigurations;
		if (!g_BldSysIFace.GetFileSetTargets(hFileSet, slConfigurations))
			return;

		HTREEITEM hTreeConfiguration;

		// And add them to the tree.
		while (!slConfigurations.IsEmpty())
		{
			CString strConfiguration = slConfigurations.RemoveHead();

			// Get the HBLDTARGET for this configuration.
			HBLDTARGET hConfiguration = g_BldSysIFace.GetTarget(strConfiguration, NO_BUILDER);
			ASSERT(hConfiguration != NULL);

			// Remove the fileset name from the configuration name.
			ASSERT(strConfiguration.GetLength() > strFileset.GetLength() + 3);
			LPCTSTR pszConfiguration = (LPCTSTR)strConfiguration + strFileset.GetLength() + 3;

			// Add the configuration to the tree.
			hTreeConfiguration = m_ConfigurationsTree.InsertItem(pszConfiguration, hItem);
			VERIFY(m_ConfigurationsTree.SetItemData(hTreeConfiguration, (ULONG)hConfiguration));
		}

		// And expand the fileset.
		m_ConfigurationsTree.Expand(hItem, TVE_EXPAND);
	}
}

void CProjectConfigurationsDlg::OnRemove()
{
	// Get the selected item.
	HTREEITEM hItem = m_ConfigurationsTree.GetSelectedItem();
	BOOL bConfig = IsConfig(hItem);
	LPCTSTR pszItemName = bConfig ? GetConfigName(hItem) : GetProjectName(hItem);
	UINT nMsg = bConfig ? IDS_DEL_CONFIG : IDS_DEL_PROJECT;

	// Confirm the action.
	CString strMsg;
	AfxFormatString1(strMsg, nMsg, pszItemName);
	if (AfxMessageBox(strMsg, MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		CWaitCursor curWait;

		if (bConfig)
		{
			// Get the HBLDTARGET.
			HBLDTARGET hConfig = GetConfig(hItem);
			HBUILDER hBld = g_BldSysIFace.GetBuilder(hConfig);

			// Delete the target.
			g_BldSysIFace.DeleteTarget(hConfig, hBld);
			CProject *pProject = g_BldSysIFace.CnvHBuilder(hBld);
			pProject->InformDependants(SN_ALL);
		}
		else
		{
			// Enumerate the children of this project node, and delete each one.
			HTREEITEM hConfigItem = m_ConfigurationsTree.GetChildItem(hItem);
			HBLDTARGET hConfig = GetConfig(hConfigItem);
			HBUILDER hBld = g_BldSysIFace.GetBuilder(hConfig);
			CProject *pProject = g_BldSysIFace.CnvHBuilder(hBld);
			IBSProject *pBSProj = pProject->GetInterface();
			COleRef<IPkgProject> pPkgProj;
			pBSProj->QueryInterface(IID_IPkgProject, (void **)&pPkgProj);
			pBSProj->Release();
			BOOL retVal =SUCCEEDED(g_pProjWksIFace->DeleteProject(pPkgProj, FALSE));

			while (hConfigItem != NULL)
			{

				// Delete the tree item (so we don't have to do FillTree(), which fully expands trees)
				m_ConfigurationsTree.DeleteItem(hConfigItem);

				// Get the next item in the tree.
				hConfigItem = m_ConfigurationsTree.GetChildItem(hItem);
			}
			m_nFileset--;
		}

		// Delete the tree item (so we don't have to do FillTree(), which fully expands trees)
		m_ConfigurationsTree.DeleteItem(hItem);
	}
}

/*
void CProjectConfigurationsDlg::OnDependencies()
{
	// Get the selected project.
	HTREEITEM hItem = m_ConfigurationsTree.GetSelectedItem();
	if (IsConfig(hItem))
		hItem = m_ConfigurationsTree.GetParentItem(hItem);

	CSubprojectsDlg dlg(GetProject(hItem), this);
	dlg.DoModal();
}
*/

void CProjectConfigurationsDlg::OnSelectConfiguration(NMHDR* pNMHDR, LRESULT* pResult)
{
	EnableButtons();
}

/////////////////////////////////////////////////////////////////////////////
// CProjectConfigurationsDlg - helpers
void CProjectConfigurationsDlg::EnableButtons()
{
	
	HTREEITEM hItem = m_ConfigurationsTree.GetSelectedItem();
	BOOL bConfig = IsConfig(hItem);
	BOOL bEnableRemove;
	HBUILDER hBld;

	if ( bConfig )
	{
		// Enable if there is atleast one more config let after we delete this one.
		bEnableRemove = m_ConfigurationsTree.GetNextSiblingItem(hItem) != NULL 
			|| m_ConfigurationsTree.GetPrevSiblingItem(hItem) != NULL;
		hBld = g_BldSysIFace.GetBuilder(GetConfig(hItem));

	}	
	else
	{
		// If this is a target it shouldn't be the last one.
		bEnableRemove = m_nFileset > 1;
		hBld = g_BldSysIFace.GetBuilderFromFileSet(GetProject(hItem));
	}

	// Can't remove the last configuration, nor a project.
	GetDlgItem(IDC_REMOVE)->EnableWindow(bEnableRemove);

	// Can't add target to projects with unsupported project types.
	CProject * pProject = g_BldSysIFace.CnvHBuilder(hBld);
	CProjType * pProjType = pProject->GetProjType();
	GetDlgItem(IDC_ADD)->EnableWindow(pProjType->IsSupported());

	// The rules for dependencies are so "magic," we'll leave
	// button enabled, and display a message box if we can't do it.
}

void CProjectConfigurationsDlg::FillTree()
{
	DWORD dwSelected = 0;
	// BLOCK: Get the selected item.
	{
		HTREEITEM hItem = m_ConfigurationsTree.GetSelectedItem();
		if (hItem != NULL)
			dwSelected = m_ConfigurationsTree.GetItemData(hItem);
		else
			dwSelected = (DWORD)g_BldSysIFace.GetActiveTarget();
	}

	// Clear the tree.
	m_ConfigurationsTree.DeleteAllItems();
	m_nFileset = 0;
	
	// Select this item.
	HTREEITEM hSelItem = NULL;

	// Fill the tree with project configurations.
	HBUILDER hBld;
	HFILESET hFileSet;
	CString strProject;
	// iterate over all projects
	g_BldSysIFace.InitBuilderEnum();
	while ((hBld = g_BldSysIFace.GetNextBuilder(strProject, FALSE /* REVIEW */)) != NO_BUILDER)
	{
		g_BldSysIFace.InitFileSetEnum(hBld);
		while ((hFileSet = g_BldSysIFace.GetNextFileSet(hBld)) != NULL)
		{
			// Get the fileset name.
			CString strFileset;
			if (!g_BldSysIFace.GetFileSetName(hFileSet, strFileset, hBld))
				continue;

			// Insert the item.
			HTREEITEM hTreeFileset = m_ConfigurationsTree.InsertItem(strFileset);
			VERIFY(m_ConfigurationsTree.SetItemData(hTreeFileset, (ULONG)hFileSet));

			// Select this item?
			if (dwSelected == (DWORD)hFileSet)
				hSelItem = hTreeFileset;

			// Now get the configurations for this project.
			CStringList slConfigurations;
			if (!g_BldSysIFace.GetFileSetTargets(hFileSet, slConfigurations))
				continue;

			HTREEITEM hTreeConfiguration = NULL;

			// And add them to the tree.
			while (!slConfigurations.IsEmpty())
			{
				CString strConfiguration = slConfigurations.RemoveHead();

				// Get the HBLDTARGET for this configuration.
				HBLDTARGET hConfiguration = g_BldSysIFace.GetTarget(strConfiguration, hBld);
				ASSERT(hConfiguration != NULL);

				// Remove the fileset name from the configuration name.
				ASSERT(strConfiguration.GetLength() > strFileset.GetLength() + 3);
				LPCTSTR pszConfiguration = (LPCTSTR)strConfiguration + strFileset.GetLength() + 3;

				// Add the configuration to the tree.
				hTreeConfiguration = m_ConfigurationsTree.InsertItem(pszConfiguration, hTreeFileset);
				VERIFY(m_ConfigurationsTree.SetItemData(hTreeConfiguration, (ULONG)hConfiguration));

				// Select this item?
				if (dwSelected == (DWORD)hConfiguration)
					hSelItem = hTreeConfiguration;
			}

			// And expand the fileset.
			m_ConfigurationsTree.Expand(hTreeFileset, TVE_EXPAND);
			m_nFileset++;

			// make a selection in any case
			if (hSelItem == NULL)
				hSelItem = hTreeConfiguration;
		}
	}

	// Select an item?
	if (hSelItem != NULL)
	{
		// Select the new item.
		m_ConfigurationsTree.Select(hSelItem, TVGN_CARET);
		m_ConfigurationsTree.EnsureVisible(hSelItem);
	}

	// Enable/Disable the buttons.
	EnableButtons();
}

void CProjectConfigurationsDlg::CalculateInvalidDependencies(HBLDTARGET hConfig, CStringList& rslConfigs)
{
	// Get the configurations name.
	CString strConfigName;
	HBUILDER hBld = g_BldSysIFace.GetBuilder(hConfig);
	VERIFY(g_BldSysIFace.GetTargetName(hConfig, strConfigName, hBld));

	// Get the active project.
	CProject* pProject = g_BldSysIFace.CnvHBuilder(hBld);

	// BLOCK:
	{
		CTargetItem *pTarget = pProject->GetTarget(strConfigName);
		// Get the target items config array
		const CPtrArray* pConfigArray = pTarget->GetConfigArray();
		int nSize = pConfigArray->GetSize();

		// Look for valid configs for this target item and add to filter list
		ConfigurationRecord * pcr = NULL;
		for (int i = 0; i < nSize; i++)
		{
			pcr = (ConfigurationRecord*)pConfigArray->GetAt(i);
			if (pcr->IsValid())
			{
				CString str = pcr->GetConfigurationName();
				// add to filter list
				if (!rslConfigs.Find(str))
					rslConfigs.AddTail(str);
			}
		}
	}

	const CPtrArray & ppcr = *pProject->GetConfigArray();
	ConfigurationRecord * pcr;
	int ccr = ppcr.GetSize();

	// BLOCK:
	{
		for (int icr = 0; icr < ccr; icr++)
		{
			pcr = (ConfigurationRecord *)ppcr[icr];

			CProjTempConfigChange projTempConfigChange(pProject);
			projTempConfigChange.ChangeConfig(pcr);

//			if (pProject->TargetReachable(strConfigName))
//				rslConfigs.AddTail(pcr->GetConfigurationName());
 		}
	}

	// BLOCK:
	{
		CProjTempConfigChange projTempConfigChange(pProject);
		projTempConfigChange.ChangeConfig(strConfigName);

		CObList ol;
		pProject->FlattenSubtree(ol, CProjItem::flt_Normal | CProjItem::flt_ExcludeGroups | CProjItem::flt_RespectItemExclude | 
						CProjItem::flt_ExcludeDependencies | CProjItem::flt_RespectTargetExclude);
		POSITION pos = ol.GetHeadPosition();
		while (pos != NULL)
		{
			CProjItem * pItem = (CProjItem *)ol.GetNext(pos);
			
			if (pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
			{
				/* 
				CString strTarget;

				((CTargetReference*)pItem)->GetStrProp(P_TargetRefName, strTarget);
				rslConfigs.AddTail(strTarget);
				*/
			}
		}
	}
}

BOOL CProjectConfigurationsDlg::IsConfig(HTREEITEM hItem)
{
	return !m_ConfigurationsTree.ItemHasChildren(hItem);
}

HBLDTARGET CProjectConfigurationsDlg::GetConfig(HTREEITEM hItem)
{
	ASSERT(IsConfig(hItem));

	// Get the HBLDTARGET from the item.
	return (HBLDTARGET) m_ConfigurationsTree.GetItemData(hItem);
}

LPCTSTR CProjectConfigurationsDlg::GetConfigName(HTREEITEM hItem)
{
	static CString strConfiguration;
	ASSERT(IsConfig(hItem));

	// Get the HBLDTARGET.
	HBLDTARGET hConfig = GetConfig(hItem);
	HBUILDER hBld = g_BldSysIFace.GetBuilder(hConfig);

	// Get the name from the build system.
	if (g_BldSysIFace.GetTargetName(hConfig, strConfiguration, hBld))
		return strConfiguration;
	else
		return NULL;
}

HFILESET CProjectConfigurationsDlg::GetProject(HTREEITEM hItem)
{
	ASSERT(!IsConfig(hItem));

	// Get the HFILESET from the item.
	return (HFILESET) m_ConfigurationsTree.GetItemData(hItem);
}

LPCTSTR CProjectConfigurationsDlg::GetProjectName(HTREEITEM hItem)
{
	static CString strFileset;
	ASSERT(!IsConfig(hItem));

	// Get the HFILESET.
	HFILESET hFileset = GetProject(hItem);
	HBUILDER hBld = g_BldSysIFace.GetBuilderFromFileSet(hFileset);

	// Get the name from the build system.
	if (g_BldSysIFace.GetFileSetName(hFileset, strFileset, hBld))
		return strFileset;
	else
		return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CSubprojectsCheckList

BEGIN_MESSAGE_MAP(CSubprojectCheckList, CCheckList)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CSubprojectCheckList::OnDestroy()
{
	ResetContent();
	CCheckList::OnDestroy();
}

void CSubprojectCheckList::ResetContent()
{
	m_nMaxWidth = 0;
	int nItemCount = GetCount();
	for (int i = 0; i < nItemCount; i += 1)
		delete (CSubprojectCheckItem*)CCheckList::GetItemDataPtr(i);
	
	if (m_hWnd != NULL)
		CCheckList::ResetContent();
}

int CSubprojectCheckList::AddItem(const TCHAR* szItemText, BOOL bChecked, HFILESET hFileSet, HPROJDEP hProjDep)
{
	CSubprojectCheckItem* pItem = new CSubprojectCheckItem;
	pItem->m_hFileSet = hFileSet;
	pItem->m_hProjDep = hProjDep;
	pItem->m_bChecked = bChecked;
	pItem->m_bOriginalCheck = bChecked;

	CClientDC dc(this);
	int nItem = CCheckList::AddString(szItemText);

	int width = dc.GetTextExtent(szItemText, _tcslen(szItemText)).cx;
	if (width > m_nMaxWidth)
	{
		// might need horz scrollbars
		m_nMaxWidth = width;
		SetHorizontalExtent(width + 14); // add width of checkbox
	}

	CCheckList::SetItemDataPtr(nItem, pItem);

	return nItem;
}

void CSubprojectCheckList::SetCheck(int nItem, int bCheck /*=TRUE*/)
{
	CSubprojectCheckItem* pItem = (CSubprojectCheckItem*)CCheckList::GetItemDataPtr(nItem);
	ASSERT(pItem != NULL);
	pItem->m_bChecked = bCheck;
	InvalidateCheck(nItem);
}

int CSubprojectCheckList::GetCheck(int nItem)
{
	CSubprojectCheckItem* pItem = (CSubprojectCheckItem*)CCheckList::GetItemDataPtr(nItem);
	ASSERT(pItem != NULL);
	return pItem->m_bChecked ? 1 : 0;
}

void CSubprojectCheckList::SetOriginalCheck(int nItem, int bCheck /*=TRUE*/)
{
	CSubprojectCheckItem* pItem = (CSubprojectCheckItem*)CCheckList::GetItemDataPtr(nItem);
	ASSERT(pItem != NULL);
	pItem->m_bOriginalCheck = bCheck;
}

int CSubprojectCheckList::GetOriginalCheck(int nItem)
{
	CSubprojectCheckItem* pItem = (CSubprojectCheckItem*)CCheckList::GetItemDataPtr(nItem);
	return pItem->m_bOriginalCheck ? 1 : 0;
}

void CSubprojectCheckList::SetFileSet(int nItem, HFILESET hFileSet)
{
	CSubprojectCheckItem* pItem = (CSubprojectCheckItem*)CCheckList::GetItemDataPtr(nItem);
	ASSERT(pItem != NULL);
	pItem->m_hFileSet = hFileSet;
}

HFILESET CSubprojectCheckList::GetFileSet(int nItem)
{
	CSubprojectCheckItem* pItem = (CSubprojectCheckItem*)CCheckList::GetItemDataPtr(nItem);
	return pItem->m_hFileSet;
}

void CSubprojectCheckList::SetProjDep(int nItem, HPROJDEP hProjDep)
{
	CSubprojectCheckItem* pItem = (CSubprojectCheckItem*)CCheckList::GetItemDataPtr(nItem);
	ASSERT(pItem != NULL);
	pItem->m_hProjDep = hProjDep;
}

HPROJDEP CSubprojectCheckList::GetProjDep(int nItem)
{
	CSubprojectCheckItem* pItem = (CSubprojectCheckItem*)CCheckList::GetItemDataPtr(nItem);
	return pItem->m_hProjDep;
}


/////////////////////////////////////////////////////////////////////////////
// CSubprojectsDlg dialog

CSubprojectsDlg::CSubprojectsDlg(HFILESET hProject, CWnd* pParent /*=NULL*/)
	: C3dDialog(IDD_SUBPROJECTS, pParent), m_hProject(hProject)
{
}

BEGIN_MESSAGE_MAP(CSubprojectsDlg, C3dDialog)
	ON_CBN_SELCHANGE(IDC_PROJECT, OnSelProject)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSubprojectsDlg message handlers

void FillProjectCombo(CComboBox* pCombo, HFILESET hFilesetSelect)
{
	pCombo->ResetContent();

	HBUILDER hBld;
	HFILESET hFileSet;
	CString strSelect, strProject;
	// iterate over all projects
	g_BldSysIFace.InitBuilderEnum();
	while ((hBld = g_BldSysIFace.GetNextBuilder(strProject, FALSE /* REVIEW */)) != NO_BUILDER)
	{
		// Fill the tree with project configurations.
		g_BldSysIFace.InitFileSetEnum(hBld);
		while (hFileSet = g_BldSysIFace.GetNextFileSet(hBld))
		{
			// Get the fileset name.
			CString strFileset;
			if (!g_BldSysIFace.GetFileSetName(hFileSet, strFileset, hBld))
				continue;

			int nItem = pCombo->AddString(strFileset);
			pCombo->SetItemData(nItem, (ULONG)hFileSet);
		
			if (hFilesetSelect == hFileSet)
				strSelect = strFileset;
		}
	}
	pCombo->SelectString(-1,strSelect);
}

BOOL CSubprojectsDlg::OnInitDialog()
{
	C3dDialog::OnInitDialog();

	m_subprojects.SubclassDlgItem(IDC_SUBPROJECTS, this);

	FillProjectCombo((CComboBox*)GetDlgItem(IDC_PROJECT), m_hProject);
	
	m_subprojects.ResetContent();
	FillCurrent();
	FillAvailable();

	return TRUE;
}

BOOL CSubprojectsDlg::OnAccept()
{
	BOOL bAnyChanges = FALSE;
	int nItemCount = m_subprojects.GetCount();
	HBUILDER  hProjBld = g_BldSysIFace.GetBuilderFromFileSet(m_hProject);
	CProject * pTopProject = g_BldSysIFace.CnvHBuilder(hProjBld);
	for (int i = 0; i < nItemCount; i += 1)
	{
		BOOL bChecked = m_subprojects.GetCheck(i);
		BOOL bOriginalCheck = m_subprojects.GetOriginalCheck(i);
		if (bChecked == bOriginalCheck)
			continue;
		
		bAnyChanges = TRUE;

		HFILESET hFileSet = m_subprojects.GetFileSet(i);
		HBUILDER hBld = g_BldSysIFace.GetBuilderFromFileSet(hFileSet);
		CProject * pSubProject = g_BldSysIFace.CnvHBuilder(hBld);
		if (bChecked)
		{

 			if (g_BldSysIFace.AddProjectDependency(m_hProject, hFileSet, hProjBld, hBld) == NULL)
			{
  				AfxMessageBox(IDS_ERR_NEWDEP, MB_ICONEXCLAMATION);
				
				m_subprojects.SetCurSel(i);
				m_subprojects.SetFocus();
				return FALSE;
   			}

		}
		else
		{
			CString strSub;
			CProjectDependency * pProjDep = g_BldSysIFace.CnvHProjDep(m_hProject, m_subprojects.GetProjDep(i));
			pProjDep->GetStrProp(P_ProjItemName,strSub);

			if (!g_BldSysIFace.RemoveProjectDependency(m_subprojects.GetProjDep(i), m_hProject, hProjBld))
			{
				MessageBeep(0); // no message would make any more sense here...
				m_subprojects.SetCurSel(i);
				m_subprojects.SetFocus();
				return FALSE;
			}
			// search list for the dep
			CString strTop; pTopProject->GetName(strTop);
			::RemoveDep( strTop, strSub );
		}
		SetWorkspaceDocDirty();
	}
	
	if (bAnyChanges)
		ChangeOKtoClose(this);

	return TRUE;
}

void CSubprojectsDlg::OnOK()
{
	if (!OnAccept())
		return;

	// Flush the undo buffer
	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
	VERIFY (SUCCEEDED (pProjSysIFace->FlushUndo ()));
	
	C3dDialog::OnOK();
}

void CSubprojectsDlg::OnSelProject()
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_PROJECT);

	if (!OnAccept())
	{
		int nItemCount = pCombo->GetCount();
		for (int i = 0; i < nItemCount; i += 1)
		{
			if (m_hProject == (HFILESET)pCombo->GetItemData(i))
			{
				pCombo->SetCurSel(i);
				break;
			}
		}
		return;
	}
	
	int nItem = pCombo->GetCurSel();
	ASSERT(nItem >= 0);
	HFILESET hFileSet = (HFILESET)pCombo->GetItemData(nItem);
	ASSERT(hFileSet != NULL);
	
	m_hProject = hFileSet;
	m_subprojects.ResetContent();
	FillCurrent();
	FillAvailable();
}

extern 	BOOL SelectFilesetInCombo(CComboBox* pCombo, HFILESET hFileset);

void CSubprojectsDlg::FillCurrent()
{
	g_BldSysIFace.InitProjectDepEnum(m_hProject);
	for (;;)
	{
		HPROJDEP hProjDep = g_BldSysIFace.GetNextProjectDep(m_hProject);
		if (hProjDep == NULL)
			break;

		CString strSubproject;
		HFILESET hFileSet = g_BldSysIFace.GetDependantProject(hProjDep, m_hProject, ACTIVE_BUILDER);
		if (hFileSet)
		{
			g_BldSysIFace.GetFileSetName(hFileSet, strSubproject, ACTIVE_BUILDER);
			m_subprojects.AddItem(strSubproject, TRUE, NULL, hProjDep);
		}
	}
}

void CSubprojectsDlg::FillAvailable()
{
	CStringList slInvalid;

	HBUILDER hBld = g_BldSysIFace.GetActiveBuilder();

	CString strFileSet;
 	CTargetItem * pSelectedTarget = g_BldSysIFace.CnvHFileSet(hBld, m_hProject);

	HFILESET hFileSet;
	CString strProject;
	// iterate over all projects
	g_BldSysIFace.InitBuilderEnum();
	while ((hBld = g_BldSysIFace.GetNextBuilder(strProject, TRUE /* REVIEW */)) != NO_BUILDER)
	{
		g_BldSysIFace.InitFileSetEnum(hBld);
		for (;;)
		{
			hFileSet = g_BldSysIFace.GetNextFileSet(hBld);
			if (hFileSet == NULL)
				break;

			if (hFileSet == m_hProject)
				continue;

	 		CTargetItem * pTarget = g_BldSysIFace.CnvHFileSet(hBld, hFileSet);

			if (pTarget->IsTargetReachable(pSelectedTarget))
				continue;

			if (pSelectedTarget->ContainsTarget(pTarget))
				continue;

			g_BldSysIFace.GetFileSetName(hFileSet, strFileSet, hBld);

			m_subprojects.AddItem(strFileSet, FALSE, hFileSet, NULL);
		}
	}
}

////////////////////////////////////////////////////////////////////////////

void CProject::OnSubprojects()
{
	// Is project window active?
	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
	BOOL bActive = pProjSysIFace->IsWindowActive() == S_OK;

	HBUILDER hBld = ACTIVE_BUILDER; // default

	// use selection if possible
	if	(bActive)
	{
		CMultiSlob *	pMultiSlob = NULL;
		
		// Find the current selection.
		pProjSysIFace->GetSelectedSlob(&pMultiSlob);

		if( (pMultiSlob != NULL) && (pMultiSlob->GetContentList()->GetCount() > 0) )
		{
			
			CSlob * pSlob = pMultiSlob->GetDominant();
			if (pSlob->IsKindOf(RUNTIME_CLASS(CBuildViewSlob)))
				pSlob = ((CBuildViewSlob *)pSlob)->GetBuildSlob();
			else if (!pSlob->IsKindOf(RUNTIME_CLASS(CBuildSlob)))
				pSlob = NULL;
			
			if (pSlob != NULL)
			{
				hBld = (HBUILDER)((CBuildSlob *)pSlob)->GetProjItem()->GetProject();
			}
		}
	}

	// Show the dialog.
	CSubprojectsDlg dlg(g_BldSysIFace.GetFileSet(hBld, ACTIVE_TARGET));
	dlg.DoModal();
}
