// filenew.cpp : implementation file for CFileNewTabbedDialog,
//										 CNewFileDialogTab,
//										 CNewTemplateDialogTab,
//										 CNewProjectDialogTab,
//										 CNewWorkspaceDialogTab,
//										 CNewOtherDocDialogTab
//

#include "stdafx.h"
#include "afxcview.h"

#include "filenew.h"
#include "shell.h"
#include "docobtpl.h"
#include "toolexpt.h"
#include <iswsguid.h>				// IID_IWebPkgProject


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define COLUMNEXTRA 20	// extra column width for small icon and spacing

extern HWND PASCAL _SushiGetSafeOwner(CWnd* pParent);

extern void GetRegWorkspaceDir(CString& strLocation);
extern void WriteRegWorkspaceDir(const CString& strLocation);

void SetListCtrlStyle(CListCtrl *pListCtrl, LONG lNewStyle)
{
	LONG lStyle;
	lStyle = ::GetWindowLong(pListCtrl->m_hWnd, GWL_STYLE);
	lStyle &= ~LVS_TYPEMASK;
	lStyle |= lNewStyle;
	::SetWindowLong(pListCtrl->m_hWnd, GWL_STYLE, lStyle);
	pListCtrl->Arrange(LVA_SNAPTOGRID);
	pListCtrl->Invalidate();
	pListCtrl->UpdateWindow();
}

typedef enum CreateFileError
{
	cfeNoError,
	cfeFileExists,
	cfeInvalidFile,
	cfeInvalidPath,
	cfeNonexistentPath,
	cfeCannotCreatePath,
	cfeCannotCreateFile
};

extern void EnsureSlashAtEnd(CString& str);

CreateFileError CanCreateFile(const CString &strFileName, const CString &strPathName, BOOL fDirMustExist)
{
	CDir dir;
	CPath path;
	CWaitCursor wc;
	if (!dir.CreateFromStringEx(strPathName, TRUE))
		return cfeInvalidPath;
	if (!path.CreateFromDirAndFilename(dir, strFileName))
		return cfeInvalidFile;
	if (path.ExistsOnDisk())
		return cfeFileExists;
	if (fDirMustExist && !dir.ExistsOnDisk())
		return cfeNonexistentPath;
	if (!dir.CreateOnDisk())
		return cfeCannotCreatePath;
	if (!path.CanCreateOnDisk(TRUE))
		return cfeCannotCreateFile;
	return cfeNoError;
}

BOOL IsProjectNameValid(LPCTSTR pszProject)
{

	// Disallow a few reserved DOS words
	const char * szReserved[]={"nul", "con", "aux", "template" };

	for(int i=0; i<(sizeof(szReserved)/sizeof(char *)); i++)
		if(_tcsicmp(pszProject, szReserved[i])==0)
			return FALSE;

	// We need to disallow com? and lpt? where ? can be any number from 1 to 9
	if(_tcslen(pszProject)==4 && pszProject[3]>='1' && pszProject[3]<='9')
	{
		if((_tcsnicmp(pszProject, "com", 3)==0) 
			|| (_tcsnicmp(pszProject, "lpt", 3)==0))
			return FALSE;
	}

	// DevStudio96 RAID 7501 [patbr]: disallow space as first or last character in name
	if (*pszProject == _T('\0') || *pszProject == _T(' ') || *pszProject == _T('.'))
		return FALSE;

	DWORD cchLeft = 8;	// 8 as default maximum length for file name
	DWORD dwMaxLen = 8;
	DWORD dwDummy1;
	if (::GetVolumeInformation(NULL, NULL, 0, NULL, &dwMaxLen, &dwDummy1, NULL, 0))
	{
		// succesfully got info from file system -- use it.
		cchLeft = dwMaxLen;
	}

	TCHAR ch;
	while ((ch = *pszProject) != _T('\0'))
	{
		if (_istalnum(ch) || ch == _T('_') || ch == _T(' ') || IsDBCSLeadByte(ch) || (IsJapaneseSystem() && IsSBKatakana(ch)) || ch < 0)
		{
			cchLeft -= _tclen(pszProject);	// count the character
			if (cchLeft < 0)
				return FALSE;	// too long as name for project
		}
		else
		{
			return FALSE;	// illegal as name for project
		}
		pszProject = _tcsinc(pszProject);

		// disallow trailing spaces and dots.
		if( (ch == _T(' ') || ch == _T('.')) && *pszProject == _T('\0'))
			return FALSE;	// illegal as name for project
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CFileNewTabbedDialog

int CFileNewTabbedDialog::c_nSelectTab = 0;

IMPLEMENT_DYNAMIC(CFileNewTabbedDialog, CTabbedDialog);

CFileNewTabbedDialog::CFileNewTabbedDialog(BOOL bUseSavedData /*=FALSE*/, BOOL bForceAddToProject /*=FALSE*/, BOOL bForceShowProjectTab /*=FALSE*/)
	: CTabbedDialog(IDS_NEW, NULL, bUseSavedData ? c_nSelectTab: 0, commitOnOk)
{
	IProjectWorkspace *pInterface;
	pInterface = g_IdeInterface.GetProjectWorkspace();
	ASSERT(pInterface != NULL);
	CADWORD Projects;
	pInterface->GetLoadedProjects(&Projects);
	int nSize;
	if ((nSize = Projects.cElems) != 0)
	{
		COleRef<IPkgProject> pActive;
		pInterface->GetActiveProject(&pActive);
		m_pActiveProject = pActive;

		for (int nIndex = 0; nIndex < nSize; nIndex++)
		{
			IPkgProject *pProject = (IPkgProject *)Projects.pElems[nIndex];
			LPCOLESTR pszProjName, pszProjPath;
			DWORD dwPackageID, dwFlags;
			pProject->GetProjectInfo(&pszProjName, &pszProjPath, &dwPackageID, &dwFlags);
			if ((dwFlags & GPI_PROJISEXECUTABLE) == 0)
			{
				CString strProject = pszProjName;
				m_strlistProjects.AddTail(strProject);
				m_ptrlistBuilders.AddTail(pProject);
				pProject->AddRef();
			}
			pProject->Release();
			CoTaskMemFree((void *)pszProjName);
			CoTaskMemFree((void *)pszProjPath);
		}
		CoTaskMemFree((void *)Projects.pElems);
	}

	m_pTemplate = NULL;
	m_pProject = NULL;
	m_pProjectProvider = NULL;
	m_dwProjectType = 0L;
	m_fAddToProject = FALSE;

	m_strFileName.Empty();
	m_strPathName.Empty();

	m_fCommitting = FALSE;
	m_Type = CPartTemplate::newnone;

	BOOL fHaveWizards = FALSE;
	POSITION pos = theApp.GetFirstDocTemplatePosition();
	while (pos != NULL)
	{
		CPartTemplate *pTemplate = (CPartTemplate *)theApp.GetNextDocTemplate(pos);
		if (CPartTemplate::newtemplate == pTemplate->GetTabType())
		{
			fHaveWizards = TRUE;
			break;
		}
	}

	AddTab(new CNewFileDialogTab(bUseSavedData, bForceAddToProject));
	if (fHaveWizards)
		AddTab(new CNewTemplateDialogTab(bUseSavedData, bForceAddToProject));
	AddTab(new CNewProjectDialogTab(bUseSavedData, bForceAddToProject));
	if (!bForceAddToProject && !bForceShowProjectTab)
		AddTab(new CNewWorkspaceDialogTab(bUseSavedData));
	AddTab(new CNewOtherDocDialogTab(bUseSavedData, bForceAddToProject));

	if ((nSize == 0) || bForceShowProjectTab)
		SelectTab(fHaveWizards ? 2 : 1, TRUE);
}

CFileNewTabbedDialog::~CFileNewTabbedDialog()
{
	c_nSelectTab = GetCurrentTab();
	m_strlistProjects.RemoveAll();
	while(!m_ptrlistBuilders.IsEmpty())
	{
		IPkgProject *pProj = (IPkgProject *)m_ptrlistBuilders.RemoveHead();
		pProj->Release();
	}
}

void CFileNewTabbedDialog::OnOK()
{
	m_fCommitting = TRUE;
	if (m_nTabCur != -1)
	{
		CDlgTab* pTab = GetTab(m_nTabCur);
		if (!pTab->ValidateTab())
		{
		//	TRACE("Tab %s not validated!\n", (const char*)GetTab(m_nTabCur)->m_strCaption);
			m_fCommitting = FALSE;
			return;
		}

		pTab->Deactivate(this);
	}

	// At this point, all tabs must be valid.  We differ from CTabbedDialog in that
	// we commit only the active tab, and cancel all of the others.
	for (int nTab = 0; nTab < m_tabs.GetSize(); nTab++)
	{
		if (GetTab(nTab)->GetSafeHwnd() != NULL)
		{
			if (nTab == m_nTabCur)
				GetTab(nTab)->CommitTab();
			else
				GetTab(nTab)->CancelTab();
		}
	}

	m_fCommitting = FALSE;
	EndDialog(IDOK);
}

void CFileNewTabbedDialog::OnCancel()
{
	CTabbedDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// CNewFileDialogTab

int CNewFileDialogTab::m_nSelected;
BOOL CNewFileDialogTab::m_fAddToProject = TRUE;
int CNewFileDialogTab::m_nIndex;
CString CNewFileDialogTab::m_strFileName;
CString CNewFileDialogTab::m_strPathName;
int CNewFileDialogTab::m_nFocusCtrlId = -1;
#ifdef ENABLE_LISTVIEW_MODES
VIEW_MODE CNewFileDialogTab::c_ViewMode = smallicon;
#endif	// ENABLE_LISTVIEW_MODES

IMPLEMENT_DYNAMIC(CNewFileDialogTab, CDlgTab);

CNewFileDialogTab::CNewFileDialogTab(BOOL bUseSavedData /*=FALSE*/, BOOL bForceAddToProject /*=FALSE*/)
	: CDlgTab(IDDP_FILENEW_FILES, IDS_FILESTAB)
{
	if (!bUseSavedData)
	{
		m_nFocusCtrlId = -1;
		m_nSelected = 0;
		m_strFileName.Empty();
	}

	if (m_strPathName.IsEmpty())
	{
		CDir dir;
		dir.CreateFromCurrent();
		m_strPathName = (const TCHAR *)dir;
	}

	if (bForceAddToProject)
	{
		m_fAddToProjectPrev = m_fAddToProject;
		m_fAddToProject = TRUE;
	}
	m_fForceAddToProj = bForceAddToProject;

	m_fIsInitialized = FALSE;
	m_fDoubleClicked = FALSE;
}

void CNewFileDialogTab::DoDataExchange(CDataExchange* pDX)
{
	CDlgTab::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CNewFileDialogTab)
	DDX_Control(pDX, IDC_FILETYPES, m_FileTypes);
	DDX_Check(pDX, IDC_ADDFILETOPROJECT, m_fAddToProject);
	DDX_CBIndex(pDX, IDC_PROJECTFORFILE, m_nIndex);
	DDX_Text(pDX, IDC_NAMEFORFILE, m_strFileName);
	DDX_Text(pDX, IDC_LOCATIONFORFILE, m_strPathName);
	//}}AFX_DATA_MAP
}

BOOL CNewFileDialogTab::OnInitDialog()
{
	UpdateData(FALSE);

//	m_imageListNormal.Create(32, 32, ILC_COLOR, 1, 1);
//	m_imageListNormal.SetBkColor(GetSysColor(COLOR_WINDOW));
	m_imageListSmall.Create(16, 16, ILC_COLOR, 1, 1);
	m_imageListSmall.SetBkColor(GetSysColor(COLOR_WINDOW));

	POSITION pos = theApp.GetFirstDocTemplatePosition();

	// Set the generic members of the LV_ITEM stuct:
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvItem.iSubItem = 0;
	lvItem.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
	lvItem.state = 0;

	// while loop variables
	CPartTemplate *pTemplate;
	HICON hIcon;
	CString strDisplay;
	int nIndex = 0;
	int nColumnWidth = 0;
	while (pos != NULL)
	{
		pTemplate = (CPartTemplate *)theApp.GetNextDocTemplate(pos);
		ASSERT(pTemplate != NULL);
		if (CPartTemplate::newfile == pTemplate->GetTabType() && (hIcon = pTemplate->GetIcon()) &&
			pTemplate->GetDocString(strDisplay, CDocTemplate::fileNewName) && !strDisplay.IsEmpty())
		{
			hIcon = pTemplate->GetIcon();
		//	m_imageListNormal.Add(hIcon);
			m_imageListSmall.Add(hIcon);

			pTemplate->GetDocString(strDisplay, CDocTemplate::fileNewName);
			lvItem.iItem = nIndex;
			lvItem.pszText = (LPTSTR)(LPCTSTR)strDisplay;
			lvItem.iImage = nIndex;
			lvItem.lParam = (LPARAM)pTemplate;

			VERIFY(m_FileTypes.InsertItem(&lvItem) != -1);

			nColumnWidth = max(nColumnWidth, m_FileTypes.GetStringWidth(strDisplay));
			nIndex++;
		}
	}

	m_FileTypes.SetColumnWidth(-1, nColumnWidth + COLUMNEXTRA);
	m_FileTypes.SetItemState(m_nSelected, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

//	m_FileTypes.SetImageList(&m_imageListNormal, LVSIL_NORMAL);
	m_FileTypes.SetImageList(&m_imageListSmall, LVSIL_SMALL);

#ifndef ENABLE_LISTVIEW_MODES
	SetListCtrlStyle(&m_FileTypes, LVS_SMALLICON);
	SetListCtrlStyle(&m_FileTypes, LVS_LIST);
#else	// ENABLE_LISTVIEW_MODES
	switch (c_ViewMode)
	{
		case largeicon:
			::CheckRadioButton(m_hWnd, IDC_LARGEICONVIEW, IDC_LISTVIEW, IDC_LARGEICONVIEW);
			SetListCtrlStyle(&m_FileTypes, LVS_SMALLICON);
			SetListCtrlStyle(&m_FileTypes, LVS_ICON);
			break;

		case smallicon:
			::CheckRadioButton(m_hWnd, IDC_LARGEICONVIEW, IDC_LISTVIEW, IDC_SMALLICONVIEW);
			SetListCtrlStyle(&m_FileTypes, LVS_ICON);
			SetListCtrlStyle(&m_FileTypes, LVS_SMALLICON);
			break;

		case list:
			::CheckRadioButton(m_hWnd, IDC_LARGEICONVIEW, IDC_LISTVIEW, IDC_LISTVIEW);
			SetListCtrlStyle(&m_FileTypes, LVS_ICON);
			SetListCtrlStyle(&m_FileTypes, LVS_LIST);
			break;
	}
#endif	// ENABLE_LISTVIEW_MODES

	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));

	CString strProjectDir;
	pos = pParent->m_strlistProjects.GetHeadPosition();
	if (pos != NULL)
	{
		POSITION pos2 = pParent->m_ptrlistBuilders.GetHeadPosition();
		while (pos != NULL)
		{
			CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_PROJECTFORFILE);
			ASSERT(pos2 != NULL);
			CString strProject = pParent->m_strlistProjects.GetNext(pos);
			IPkgProject *pProject = (IPkgProject *)pParent->m_ptrlistBuilders.GetNext(pos2);
			int nIndex = pComboBox->InsertString(-1, strProject);
			pComboBox->SetItemData(nIndex, (DWORD)pProject);
			if (pProject == pParent->m_pActiveProject)
			{
				pComboBox->SetCurSel(nIndex);

				m_nIndex = nIndex;

				LPCOLESTR pszProjName, pszProjPath;
				DWORD dwPackageID, dwFlags;
				pProject->GetProjectInfo(&pszProjName, &pszProjPath, &dwPackageID, &dwFlags);
				CString strProjPath = pszProjPath;
				CPath path;
				path.Create(strProjPath);
				CDir dir;
				dir.CreateFromPath(path);
				strProjectDir = (const TCHAR *)dir;
				CoTaskMemFree((void *)pszProjName);
				CoTaskMemFree((void *)pszProjPath);
			}
		}
		ASSERT(pos2 == NULL);
	}
	else
	{
		m_fAddToProject = FALSE;
		((CButton *)GetDlgItem(IDC_ADDFILETOPROJECT))->SetCheck(m_fAddToProject);
		GetDlgItem(IDC_ADDFILETOPROJECT)->EnableWindow(m_fAddToProject);
	}

	GetDlgItem(IDC_PROJECTFORFILE)->EnableWindow(m_fAddToProject);
//	GetDlgItem(IDC_NAMEFORFILE)->EnableWindow(m_fAddToProject);
//	GetDlgItem(IDC_FILENAMESTATIC)->EnableWindow(m_fAddToProject);
//	GetDlgItem(IDC_LOCATIONFORFILE)->EnableWindow(m_fAddToProject);
//	GetDlgItem(IDC_FILELOCATIONSTATIC)->EnableWindow(m_fAddToProject);
//	GetDlgItem(IDC_BROWSE)->EnableWindow(m_fAddToProject);

	if (!strProjectDir.IsEmpty())
		SetDlgItemText(IDC_LOCATIONFORFILE, strProjectDir);

	// limit length of file name
	((CEdit *)GetDlgItem(IDC_NAMEFORFILE))->LimitText(_MAX_FNAME);

	m_fIsInitialized = TRUE;

	if (m_nFocusCtrlId == -1)
		return(TRUE);

	CWnd *pWnd = GetDlgItem(m_nFocusCtrlId);
	if (pWnd == NULL)
		return(TRUE);

	GotoDlgCtrl(pWnd);
	return(FALSE);
}

BOOL CNewFileDialogTab::Activate(CTabbedDialog* pParentWnd, CPoint position)
{
	BOOL fRet = CDlgTab::Activate(pParentWnd, position);
	EnableButtons();
	return fRet;
}

void CNewFileDialogTab::Deactivate(CTabbedDialog* pParentWnd)
{
	CDlgTab::Deactivate(pParentWnd);
}

BOOL CNewFileDialogTab::ValidateTab()
{
	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));
	if (pParent->IsCommitting() || m_fDoubleClicked)
	{
		if (m_FileTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED) == -1)
		{
			MsgBox(Error, IDS_ERR_FILE_TYPE);
			m_FileTypes.SetFocus();
			return FALSE;
		}

		m_nSelected = m_FileTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		CPartTemplate *pTemplate = (CPartTemplate *)m_FileTypes.GetItemData(m_nSelected);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CPartTemplate)));

		((CEdit *)GetDlgItem(IDC_NAMEFORFILE))->GetWindowText(m_strFileName);

		// trim leading and trailing spaces from filename...
		m_strFileName.TrimLeft();
		m_strFileName.TrimRight();
		CString strCreate;

		if (!m_strFileName.IsEmpty())
		{
			// add default extension for name checking--it will be added on creation.
			if (m_strFileName.Find('.') == -1)
			{
				CString strExtension;
				pTemplate->GetDocString(strExtension, CDocTemplate::filterExt);
				m_strFileName += strExtension;
			}

			((CEdit *)GetDlgItem(IDC_LOCATIONFORFILE))->GetWindowText(m_strPathName);
			CreateFileError cfe = CanCreateFile(m_strFileName, m_strPathName, TRUE);
			if (cfe == cfeFileExists)
			{
				if (MsgBox(Question, IDS_EXISTING_FILEPATH, MB_YESNO) != IDYES)
					return FALSE;
			}
			else if (cfe == cfeInvalidFile || cfe == cfeCannotCreateFile)
			{
				if (m_strFileName.GetLength() + m_strPathName.GetLength() >= _MAX_PATH - 1)
				{
					CString strMsg;
					MsgBox(Error, MsgText(strMsg, IDS_TOOLONG_FILENAME, m_strFileName, m_strPathName));
				}
				else
					MsgBox(Error, IDS_INVALID_FILENAME);
				GotoDlgCtrl(GetDlgItem(IDC_NAMEFORFILE));
				return FALSE;
			}
			else if (cfe == cfeInvalidPath || cfe == cfeNonexistentPath || cfe == cfeCannotCreatePath)
			{
				MsgBox(Error, IDS_INVALID_PATHNAME);
				GotoDlgCtrl(GetDlgItem(IDC_LOCATIONFORFILE));
				return FALSE;
			}

			strCreate = m_strPathName;
			EnsureSlashAtEnd(strCreate);
			strCreate += m_strFileName;

			if (!pTemplate->CanCreateDocument(strCreate))
			{
				// pTemplate is responsible for issuing appropriate error message!
				return FALSE;
			}
		}
		if (IsDlgButtonChecked(IDC_ADDFILETOPROJECT))
		{
			if (m_strFileName.IsEmpty())
			{
				MsgBox(Error, IDS_NEED_FILENAME);
				GotoDlgCtrl(GetDlgItem(IDC_NAMEFORFILE));
				return FALSE;
			}

			USES_CONVERSION;

			CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_PROJECTFORFILE);
			IPkgProject *pProject = (IPkgProject *)pComboBox->GetItemData(m_nIndex);
			if (pProject->CanAddFile(T2W(strCreate), NULL, TRUE) != S_OK)
			{
				return FALSE;
			}
		}
	}

	return(TRUE);
}

void CNewFileDialogTab::CommitTab()
{
	CDlgTab::CommitTab();

	m_nSelected = m_FileTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	CDocTemplate *pTemplate = (CDocTemplate *)m_FileTypes.GetItemData(m_nSelected);

	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));
	pParent->SetType(CPartTemplate::newfile);
	pParent->SetTemplate(pTemplate);
	pParent->SetAddToProject(m_fAddToProject);

	// trim leading and trailing spaces from filename...
	m_strFileName.TrimLeft();
	m_strFileName.TrimRight();

	pParent->SetFileName(m_strFileName);
	pParent->SetPathName(m_strPathName);
	if (m_fAddToProject)
	{
		CString strProject;
		CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_PROJECTFORFILE);
		pComboBox->GetLBText(m_nIndex, strProject);
		DWORD dwData = pComboBox->GetItemData(m_nIndex);
		pParent->SetProject((IPkgProject *)dwData);
	}

	if (m_fForceAddToProj)
		m_fAddToProject = m_fAddToProjectPrev;
}

void CNewFileDialogTab::CancelTab()
{
	CDlgTab::CancelTab();

	if (m_fForceAddToProj)
		m_fAddToProject = m_fAddToProjectPrev;
}

void CNewFileDialogTab::OnBrowse()
{
	CDir dir;
	if (!dir.CreateFromStringEx(m_strPathName, TRUE))
		VERIFY(dir.CreateFromCurrent());

	CDirChooser dlg((const TCHAR *)dir);
	if (dlg.DoModal() == IDOK)
	{
		m_strPathName = dlg.GetPathName();
		GetDlgItem(IDC_LOCATIONFORFILE)->SetWindowText(m_strPathName);
	}
	EnableButtons();
}

#ifdef ENABLE_LISTVIEW_MODES
void CNewFileDialogTab::OnClickLargeIconView()
{
	SetListCtrlStyle(&m_FileTypes, LVS_ICON);
	c_ViewMode = largeicon;
}

void CNewFileDialogTab::OnClickSmallIconView()
{
	SetListCtrlStyle(&m_FileTypes, LVS_SMALLICON);
	c_ViewMode = smallicon;
}

void CNewFileDialogTab::OnClickListView()
{
	SetListCtrlStyle(&m_FileTypes, LVS_LIST);
	c_ViewMode = list;
}
#endif	// ENABLE_LISTVIEW_MODES

void CNewFileDialogTab::OnClickAddToProject()
{
	GetDlgItem(IDC_PROJECTFORFILE)->EnableWindow(IsDlgButtonChecked(IDC_ADDFILETOPROJECT));
//	GetDlgItem(IDC_NAMEFORFILE)->EnableWindow(IsDlgButtonChecked(IDC_ADDFILETOPROJECT));
//	GetDlgItem(IDC_LOCATIONFORFILE)->EnableWindow(IsDlgButtonChecked(IDC_ADDFILETOPROJECT));
//	GetDlgItem(IDC_FILENAMESTATIC)->EnableWindow(IsDlgButtonChecked(IDC_ADDFILETOPROJECT));
//	GetDlgItem(IDC_FILELOCATIONSTATIC)->EnableWindow(IsDlgButtonChecked(IDC_ADDFILETOPROJECT));
//	GetDlgItem(IDC_BROWSE)->EnableWindow(IsDlgButtonChecked(IDC_ADDFILETOPROJECT));
	EnableButtons();
}

void CNewFileDialogTab::OnDblClkType(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_fDoubleClicked = TRUE;
	if (ValidateTab())
	{
		CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
		ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));
		pParent->OnOK();
	}
	m_fDoubleClicked = FALSE;
}

void CNewFileDialogTab::OnSelectType(NMHDR* pNMHDR, LRESULT* pResult)
{
	EnableButtons();
}

void CNewFileDialogTab::OnChangeName()
{
	EnableButtons();
}

void CNewFileDialogTab::OnChangeLocation()
{
	EnableButtons();
}

void CNewFileDialogTab::OnProjectChange()
{
	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_PROJECTFORFILE);
	m_nIndex = pComboBox->GetCurSel();
	IPkgProject *pProject = (IPkgProject *)pComboBox->GetItemDataPtr(m_nIndex);
	LPCOLESTR pszProjName, pszProjPath;
	DWORD dwPackageID, dwFlags;
	pProject->GetProjectInfo(&pszProjName, &pszProjPath, &dwPackageID, &dwFlags);
	CString strProjPath = pszProjPath;
	CPath path;
	path.Create(strProjPath);
	CDir dir;
	dir.CreateFromPath(path);
	CString strProjectDir = (const TCHAR *)dir;
	SetDlgItemText(IDC_LOCATIONFORFILE, strProjectDir);
	CoTaskMemFree((void *)pszProjName);
	CoTaskMemFree((void *)pszProjPath);
}

void CNewFileDialogTab::EnableButtons()
{
	if (!m_fIsInitialized)
		return;

	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));

	if (pParent->m_btnOk.GetSafeHwnd() == NULL)
		return;

	((CEdit *)GetDlgItem(IDC_NAMEFORFILE))->GetWindowText(m_strFileName);
	((CEdit *)GetDlgItem(IDC_LOCATIONFORFILE))->GetWindowText(m_strPathName);

	// trim leading and trailing spaces from filename...
	m_strFileName.TrimLeft();
	m_strFileName.TrimRight();

	BOOL fHasItems = (m_FileTypes.GetItemCount() > 0);
	BOOL fSelected = (m_FileTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED) != -1);
	BOOL fAddToPrj = (IsDlgButtonChecked(IDC_ADDFILETOPROJECT));
	BOOL fFileName = (!m_strFileName.IsEmpty());
	BOOL fFilePath = (!m_strPathName.IsEmpty());

	if (fHasItems && fSelected && (!fAddToPrj || (fAddToPrj && fFileName && fFilePath)))
	{
		pParent->SetDefButtonIndex(IDOK - 1);
		pParent->m_btnOk.EnableWindow(TRUE);
	}
	else
	{
		pParent->SetDefButtonIndex(IDCANCEL - 1);
		pParent->m_btnOk.EnableWindow(FALSE);
	}
}

BEGIN_MESSAGE_MAP(CNewFileDialogTab, CDlgTab)
	//{{AFX_MSG_MAP (CNewFileDialogTab)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
#ifdef ENABLE_LISTVIEW_MODES
	ON_BN_CLICKED(IDC_LARGEICONVIEW, OnClickLargeIconView)
	ON_BN_CLICKED(IDC_SMALLICONVIEW, OnClickSmallIconView)
	ON_BN_CLICKED(IDC_LISTVIEW, OnClickListView)
#endif	// ENABLE_LISTVIEW_MODES
	ON_BN_CLICKED(IDC_ADDFILETOPROJECT, OnClickAddToProject)
	ON_NOTIFY(NM_DBLCLK, IDC_FILETYPES, OnDblClkType)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILETYPES, OnSelectType)
	ON_EN_CHANGE(IDC_NAMEFORFILE, OnChangeName)
	ON_EN_CHANGE(IDC_LOCATIONFORFILE, OnChangeLocation)
	ON_CBN_SELCHANGE(IDC_PROJECTFORFILE, OnProjectChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewTemplateDialogTab

int CNewTemplateDialogTab::m_nSelected;
BOOL CNewTemplateDialogTab::m_fAddToProject = TRUE;
int CNewTemplateDialogTab::m_nIndex;
CString CNewTemplateDialogTab::m_strFileName;
CString CNewTemplateDialogTab::m_strPathName;
int CNewTemplateDialogTab::m_nFocusCtrlId = -1;
#ifdef ENABLE_LISTVIEW_MODES
VIEW_MODE CNewTemplateDialogTab::c_ViewMode = smallicon;
#endif	// ENABLE_LISTVIEW_MODES

IMPLEMENT_DYNAMIC(CNewTemplateDialogTab, CDlgTab);

CNewTemplateDialogTab::CNewTemplateDialogTab(BOOL bUseSavedData /*=FALSE*/, BOOL bForceAddToProject /*=FALSE*/)
	: CDlgTab(IDDP_FILENEW_TEMPLATES, IDS_TEMPLATESTAB)
{
	if (!bUseSavedData)
	{
		m_nFocusCtrlId = -1;
		m_nSelected = 0;
		m_strFileName.Empty();
	}

	if (m_strPathName.IsEmpty())
	{
		CDir dir;
		dir.CreateFromCurrent();
		m_strPathName = (const TCHAR *)dir;
	}

	if (bForceAddToProject)
	{
		m_fAddToProjectPrev = m_fAddToProject;
		m_fAddToProject = TRUE;
	}
	m_fForceAddToProj = bForceAddToProject;

	m_fIsInitialized = FALSE;
	m_fDoubleClicked = FALSE;
}

void CNewTemplateDialogTab::DoDataExchange(CDataExchange* pDX)
{
	CDlgTab::DoDataExchange(pDX);
	
	//{{AFX_DATA_MAP(CNewTemplateDialogTab)
	DDX_Control(pDX, IDC_TEMPLATETYPES, m_TemplateTypes);
	DDX_Check(pDX, IDC_ADDTEMPLATETOPROJECT, m_fAddToProject);
	DDX_CBIndex(pDX, IDC_PROJECTFORTEMPLATE, m_nIndex);
	DDX_Text(pDX, IDC_NAMEFORTEMPLATE, m_strFileName);
	DDX_Text(pDX, IDC_LOCATIONFORTEMPLATE, m_strPathName);
	//}}AFX_DATA_MAP
}

BOOL CNewTemplateDialogTab::OnInitDialog()
{
	UpdateData(FALSE);

//	m_imageListNormal.Create(32, 32, ILC_COLOR, 1, 1);
//	m_imageListNormal.SetBkColor(GetSysColor(COLOR_WINDOW));
	m_imageListSmall.Create(16, 16, ILC_COLOR, 1, 1);
	m_imageListSmall.SetBkColor(GetSysColor(COLOR_WINDOW));

	POSITION pos = theApp.GetFirstDocTemplatePosition();

	// Set the generic members of the LV_ITEM stuct:
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvItem.iSubItem = 0;
	lvItem.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
	lvItem.state = 0;

	// while loop variables
	CPartTemplate *pTemplate;
	HICON hIcon;
	CString strDisplay;
	int nIndex = 0;
	int nColumnWidth = 0;
	while (pos != NULL)
	{
		pTemplate = (CPartTemplate *)theApp.GetNextDocTemplate(pos);
		ASSERT(pTemplate != NULL);
		if (CPartTemplate::newtemplate == pTemplate->GetTabType() && (hIcon = pTemplate->GetIcon()) &&
			pTemplate->GetDocString(strDisplay, CDocTemplate::fileNewName) && !strDisplay.IsEmpty())
		{
			hIcon = pTemplate->GetIcon();
		//	m_imageListNormal.Add(hIcon);
			m_imageListSmall.Add(hIcon);

			pTemplate->GetDocString(strDisplay, CDocTemplate::fileNewName);
			lvItem.iItem = nIndex;
			lvItem.pszText = (LPTSTR)(LPCTSTR)strDisplay;
			lvItem.iImage = nIndex;
			lvItem.lParam = (LPARAM)pTemplate;

			VERIFY(m_TemplateTypes.InsertItem(&lvItem) != -1);

			nColumnWidth = max(nColumnWidth, m_TemplateTypes.GetStringWidth(strDisplay));
			nIndex++;
		}
	}

	m_TemplateTypes.SetColumnWidth(-1, nColumnWidth + COLUMNEXTRA);
	m_TemplateTypes.SetItemState(m_nSelected, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

//	m_TemplateTypes.SetImageList(&m_imageListNormal, LVSIL_NORMAL);
	m_TemplateTypes.SetImageList(&m_imageListSmall, LVSIL_SMALL);

#ifndef ENABLE_LISTVIEW_MODES
	SetListCtrlStyle(&m_TemplateTypes, LVS_SMALLICON);
	SetListCtrlStyle(&m_TemplateTypes, LVS_LIST);
#else	// ENABLE_LISTVIEW_MODES
	switch (c_ViewMode)
	{
		case largeicon:
			::CheckRadioButton(m_hWnd, IDC_LARGEICONVIEW, IDC_LISTVIEW, IDC_LARGEICONVIEW);
			SetListCtrlStyle(&m_TemplateTypes, LVS_SMALLICON);
			SetListCtrlStyle(&m_TemplateTypes, LVS_ICON);
			break;

		case smallicon:
			::CheckRadioButton(m_hWnd, IDC_LARGEICONVIEW, IDC_LISTVIEW, IDC_SMALLICONVIEW);
			SetListCtrlStyle(&m_TemplateTypes, LVS_ICON);
			SetListCtrlStyle(&m_TemplateTypes, LVS_SMALLICON);
			break;

		case list:
			::CheckRadioButton(m_hWnd, IDC_LARGEICONVIEW, IDC_LISTVIEW, IDC_LISTVIEW);
			SetListCtrlStyle(&m_TemplateTypes, LVS_ICON);
			SetListCtrlStyle(&m_TemplateTypes, LVS_LIST);
			break;
	}
#endif	// ENABLE_LISTVIEW_MODES

	OnSelectType(NULL, NULL);

	// limit length of template name
	((CEdit *)GetDlgItem(IDC_NAMEFORTEMPLATE))->LimitText(_MAX_FNAME);

	m_fIsInitialized = TRUE;

	if (m_nFocusCtrlId == -1)
		return(TRUE);

	CWnd *pWnd = GetDlgItem(m_nFocusCtrlId);
	if (pWnd == NULL)
		return(TRUE);

	GotoDlgCtrl(pWnd);
	return(FALSE);
}

BOOL CNewTemplateDialogTab::Activate(CTabbedDialog* pParentWnd, CPoint position)
{
	BOOL fRet = CDlgTab::Activate(pParentWnd, position);
	EnableButtons();
	return fRet;
}

void CNewTemplateDialogTab::Deactivate(CTabbedDialog* pParentWnd)
{
	CDlgTab::Deactivate(pParentWnd);
}

BOOL CNewTemplateDialogTab::ValidateTab()
{
	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));
	if (pParent->IsCommitting() || m_fDoubleClicked)
	{
		if (m_TemplateTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED) == -1)
		{
			MsgBox(Error, IDS_ERR_WIZARD_TYPE);
			m_TemplateTypes.SetFocus();
			return FALSE;
		}

		m_nSelected = m_TemplateTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		CPartTemplate *pTemplate = (CPartTemplate *)m_TemplateTypes.GetItemData(m_nSelected);

		if (IsDlgButtonChecked(IDC_ADDTEMPLATETOPROJECT))
		{
			((CEdit *)GetDlgItem(IDC_NAMEFORTEMPLATE))->GetWindowText(m_strFileName);

			// trim leading and trailing spaces from filename...
			m_strFileName.TrimLeft();
			m_strFileName.TrimRight();

			if (m_strFileName.IsEmpty())
			{
				MsgBox(Error, IDS_NEED_FILENAME);
				GotoDlgCtrl(GetDlgItem(IDC_NAMEFORTEMPLATE));
				return FALSE;
			}

			((CEdit *)GetDlgItem(IDC_LOCATIONFORTEMPLATE))->GetWindowText(m_strPathName);
			CString strCreate;
			strCreate = m_strPathName;
			EnsureSlashAtEnd(strCreate);
			strCreate += m_strFileName;

			ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CPartTemplate)));
			CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_PROJECTFORTEMPLATE);
			ASSERT(m_nIndex >= 0);
			IPkgProject *pProject = (IPkgProject *)pComboBox->GetItemData(m_nIndex);

			if (!pTemplate->CanBeAddedToProject(pProject))
			{
				MsgBox(Error, IDS_ERR_WIZARD_PROJ);
				return FALSE;
			}

			if (!pTemplate->CanCreateDocument(strCreate))
			{
				// pTemplate is responsible for issuing appropriate error message!
				return FALSE;
			}

			USES_CONVERSION;
			if (pProject->CanAddFile(T2W(strCreate), NULL, TRUE) != S_OK)
			{
				return FALSE;
			}

			CreateFileError cfe = CanCreateFile(m_strFileName, m_strPathName, TRUE);
			if (cfe == cfeFileExists)
			{
				return(MsgBox(Question, IDS_EXISTING_FILEPATH, MB_YESNO) == IDYES);
			}
			else if (cfe == cfeInvalidFile || cfe == cfeCannotCreateFile)
			{
				if (m_strFileName.GetLength() + m_strPathName.GetLength() >= _MAX_PATH - 1)
				{
					CString strMsg;
					MsgBox(Error, MsgText(strMsg, IDS_TOOLONG_FILENAME, m_strFileName, m_strPathName));
				}
				else
					MsgBox(Error, IDS_INVALID_FILENAME);
				GotoDlgCtrl(GetDlgItem(IDC_NAMEFORTEMPLATE));
				return FALSE;
			}
			else if (cfe == cfeInvalidPath || cfe == cfeNonexistentPath || cfe == cfeCannotCreatePath)
			{
				MsgBox(Error, IDS_INVALID_PATHNAME);
				GotoDlgCtrl(GetDlgItem(IDC_LOCATIONFORTEMPLATE));
				return FALSE;
			}
		}
		else if (!pTemplate->CanBeAddedToProject(NULL))
		{
			MsgBox(Error, IDS_ERR_WIZARD_NO_PROJ);
			return FALSE;
		}
		else
		{
			return(MsgBox(Question, IDS_WIZARD_ORPHANS));
		}
	}

	return(TRUE);
}

void CNewTemplateDialogTab::CommitTab()
{
	CDlgTab::CommitTab();

	m_nSelected = m_TemplateTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	CDocTemplate *pTemplate = (CDocTemplate *)m_TemplateTypes.GetItemData(m_nSelected);

	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));
	pParent->SetType(CPartTemplate::newfile);
	pParent->SetTemplate(pTemplate);
	pParent->SetAddToProject(m_fAddToProject);

	if (m_fAddToProject)
	{
		// trim leading and trailing spaces from filename...
		m_strFileName.TrimLeft();
		m_strFileName.TrimRight();

		pParent->SetFileName(m_strFileName);
		pParent->SetPathName(m_strPathName);
		CString strProject;
		//CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_PROJECTFORFILE);
		CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_PROJECTFORTEMPLATE);

		pComboBox->GetLBText(m_nIndex, strProject);
		DWORD dwData = pComboBox->GetItemData(m_nIndex);
		pParent->SetProject((IPkgProject *)dwData);
	}

	if (m_fForceAddToProj)
		m_fAddToProject = m_fAddToProjectPrev;
}

void CNewTemplateDialogTab::CancelTab()
{
	CDlgTab::CancelTab();

	if (m_fForceAddToProj)
		m_fAddToProject = m_fAddToProjectPrev;
}

void CNewTemplateDialogTab::OnBrowse()
{
	CDir dir;
	if (!dir.CreateFromStringEx(m_strPathName, TRUE))
		VERIFY(dir.CreateFromCurrent());

	CDirChooser dlg((const TCHAR *)dir);
	if (dlg.DoModal() == IDOK)
	{
		m_strPathName = dlg.GetPathName();
		GetDlgItem(IDC_LOCATIONFORTEMPLATE)->SetWindowText(m_strPathName);
	}
	EnableButtons();
}

#ifdef ENABLE_LISTVIEW_MODES
void CNewTemplateDialogTab::OnClickLargeIconView()
{
	SetListCtrlStyle(&m_TemplateTypes, LVS_ICON);
	c_ViewMode = largeicon;
}

void CNewTemplateDialogTab::OnClickSmallIconView()
{
	SetListCtrlStyle(&m_TemplateTypes, LVS_SMALLICON);
	c_ViewMode = smallicon;
}

void CNewTemplateDialogTab::OnClickListView()
{
	SetListCtrlStyle(&m_TemplateTypes, LVS_LIST);
	c_ViewMode = list;
}
#endif	// ENABLE_LISTVIEW_MODES

void CNewTemplateDialogTab::OnClickAddToProject()
{
	EnableButtons();
}

void CNewTemplateDialogTab::OnDblClkType(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_fDoubleClicked = TRUE;
	if (ValidateTab())
	{
		CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
		ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));
		pParent->OnOK();
	}
	m_fDoubleClicked = FALSE;
}

void CNewTemplateDialogTab::OnSelectType(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_nIndex = -1;
	m_nSelected = m_TemplateTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (m_nSelected == -1)
		return;
	CPartTemplate *pTemplate = (CPartTemplate *)m_TemplateTypes.GetItemData(m_nSelected);

	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));

	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_PROJECTFORTEMPLATE);
	pComboBox->ResetContent();

	BOOL fHaveProjects = FALSE;
	CString strProjectDir;
	POSITION pos = pParent->m_strlistProjects.GetHeadPosition();
	if (pos != NULL)
	{
		POSITION pos2 = pParent->m_ptrlistBuilders.GetHeadPosition();
		while (pos != NULL)
		{
			ASSERT(pos2 != NULL);
			CString strProject = pParent->m_strlistProjects.GetNext(pos);
			IPkgProject *pProject = (IPkgProject *)pParent->m_ptrlistBuilders.GetNext(pos2);

			// project list only contains projects that the wizard can be added to...
			if (pTemplate->CanBeAddedToProject(pProject))
			{
				int nIndex = pComboBox->InsertString(-1, strProject);
				pComboBox->SetItemData(nIndex, (DWORD)pProject);
				if (pProject == pParent->m_pActiveProject)
				{
					pComboBox->SetCurSel(nIndex);

					m_nIndex = nIndex;

					LPCOLESTR pszProjName, pszProjPath;
					DWORD dwPackageID, dwFlags;
					pProject->GetProjectInfo(&pszProjName, &pszProjPath, &dwPackageID, &dwFlags);
					CString strProjPath = pszProjPath;
					CPath path;
					path.Create(strProjPath);
					CDir dir;
					dir.CreateFromPath(path);
					strProjectDir = (const TCHAR *)dir;
					CoTaskMemFree((void *)pszProjName);
					CoTaskMemFree((void *)pszProjPath);
				}
				fHaveProjects = TRUE;
			}
		}
		ASSERT(pos2 == NULL);
	}

	if (fHaveProjects && (m_nIndex == -1))
	{
		m_nIndex = 0;
		pComboBox->SetCurSel(m_nIndex);
	}

	if (fHaveProjects && !pTemplate->CanBeAddedToProject(NULL))
	{
		((CButton *)GetDlgItem(IDC_ADDTEMPLATETOPROJECT))->SetCheck(TRUE);
		GetDlgItem(IDC_ADDTEMPLATETOPROJECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_PROJECTFORTEMPLATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EXPLANATION)->ShowWindow(SW_HIDE);
	}
	else if (!fHaveProjects)
	{
		((CButton *)GetDlgItem(IDC_ADDTEMPLATETOPROJECT))->SetCheck(FALSE);
		GetDlgItem(IDC_ADDTEMPLATETOPROJECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_PROJECTFORTEMPLATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EXPLANATION)->ShowWindow(SW_SHOW);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_ADDTEMPLATETOPROJECT))->SetCheck(m_fAddToProject);
		GetDlgItem(IDC_ADDTEMPLATETOPROJECT)->EnableWindow(TRUE);
		GetDlgItem(IDC_PROJECTFORTEMPLATE)->EnableWindow(m_fAddToProject);
		GetDlgItem(IDC_EXPLANATION)->ShowWindow(SW_HIDE);
	}

	if (!strProjectDir.IsEmpty())
		SetDlgItemText(IDC_LOCATIONFORTEMPLATE, strProjectDir);

	EnableButtons();
}

void CNewTemplateDialogTab::OnChangeName()
{
	EnableButtons();
}

void CNewTemplateDialogTab::OnChangeLocation()
{
	EnableButtons();
}

void CNewTemplateDialogTab::OnProjectChange()
{
	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_PROJECTFORTEMPLATE);
	m_nIndex = pComboBox->GetCurSel();
	ASSERT(m_nIndex >= 0);
	IPkgProject *pProject = (IPkgProject *)pComboBox->GetItemDataPtr(m_nIndex);
	LPCOLESTR pszProjName, pszProjPath;
	DWORD dwPackageID, dwFlags;
	pProject->GetProjectInfo(&pszProjName, &pszProjPath, &dwPackageID, &dwFlags);
	CString strProjPath = pszProjPath;
	CPath path;
	path.Create(strProjPath);
	CDir dir;
	dir.CreateFromPath(path);
	CString strProjectDir = (const TCHAR *)dir;
	SetDlgItemText(IDC_LOCATIONFORTEMPLATE, strProjectDir);
	CoTaskMemFree((void *)pszProjName);
	CoTaskMemFree((void *)pszProjPath);
}

void CNewTemplateDialogTab::EnableButtons()
{
	if (!m_fIsInitialized)
		return;

	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));

	if (pParent->m_btnOk.GetSafeHwnd() == NULL)
		return;

	((CEdit *)GetDlgItem(IDC_NAMEFORTEMPLATE))->GetWindowText(m_strFileName);
	((CEdit *)GetDlgItem(IDC_LOCATIONFORTEMPLATE))->GetWindowText(m_strPathName);

	// trim leading and trailing spaces from filename...
	m_strFileName.TrimLeft();
	m_strFileName.TrimRight();

	BOOL fHasItems = (m_TemplateTypes.GetItemCount() > 0);
	BOOL fSelected = (m_TemplateTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED) != -1);
	BOOL fAddToPrj = (IsDlgButtonChecked(IDC_ADDTEMPLATETOPROJECT));
	BOOL fFileName = (!m_strFileName.IsEmpty());
	BOOL fFilePath = (!m_strPathName.IsEmpty());

	if (fHasItems && fSelected && (!fAddToPrj || (fAddToPrj && fFileName && fFilePath)))
	{
		pParent->SetDefButtonIndex(IDOK - 1);
		pParent->m_btnOk.EnableWindow(TRUE);
	}
	else
	{
		pParent->SetDefButtonIndex(IDCANCEL - 1);
		pParent->m_btnOk.EnableWindow(FALSE);
	}
}

BEGIN_MESSAGE_MAP(CNewTemplateDialogTab, CDlgTab)
	//{{AFX_MSG_MAP (CNewTemplateDialogTab)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
#ifdef ENABLE_LISTVIEW_MODES
	ON_BN_CLICKED(IDC_LARGEICONVIEW, OnClickLargeIconView)
	ON_BN_CLICKED(IDC_SMALLICONVIEW, OnClickSmallIconView)
	ON_BN_CLICKED(IDC_LISTVIEW, OnClickListView)
#endif	// ENABLE_LISTVIEW_MODES
	ON_BN_CLICKED(IDC_ADDTEMPLATETOPROJECT, OnClickAddToProject)
	ON_NOTIFY(NM_DBLCLK, IDC_TEMPLATETYPES, OnDblClkType)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TEMPLATETYPES, OnSelectType)
	ON_EN_CHANGE(IDC_NAMEFORTEMPLATE, OnChangeName)
	ON_EN_CHANGE(IDC_LOCATIONFORTEMPLATE, OnChangeLocation)
	ON_CBN_SELCHANGE(IDC_PROJECTFORTEMPLATE, OnProjectChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewProjectDialogTab

int CNewProjectDialogTab::m_nSelected;
BOOL CNewProjectDialogTab::m_fCreateNew = TRUE;
BOOL CNewProjectDialogTab::m_fDependency;
int CNewProjectDialogTab::m_nIndexProject;
int CNewProjectDialogTab::m_nIndexPlatform;
CString CNewProjectDialogTab::m_strFileName;
CString CNewProjectDialogTab::m_strPathName;
CString CNewProjectDialogTab::m_strPrevName;
int CNewProjectDialogTab::m_nFocusCtrlId = -1;
int CNewProjectDialogTab::m_nIndexSave = -1;
#ifdef ENABLE_LISTVIEW_MODES
VIEW_MODE CNewProjectDialogTab::c_ViewMode = smallicon;
#endif	// ENABLE_LISTVIEW_MODES

IMPLEMENT_DYNAMIC(CNewProjectDialogTab, CDlgTab);

CNewProjectDialogTab::CNewProjectDialogTab(BOOL bUseSavedData /*=FALSE*/, BOOL bForceAddToProject /*=FALSE*/)
	: CDlgTab(IDDP_FILENEW_PROJECTS, IDS_PROJECTSTAB)
{
	if (!bUseSavedData)
	{
		m_nFocusCtrlId = -1;
		m_nSelected = 0;
		m_strFileName.Empty();
		m_strPrevName.Empty();

		IProjectWorkspace *pInterface;
		pInterface = g_IdeInterface.GetProjectWorkspace();
		ASSERT(pInterface != NULL);

		BOOL fWorkspaceInit = (pInterface->IsWorkspaceInitialised() == S_OK);
		if (!fWorkspaceInit)
			m_fCreateNew = TRUE;

		m_fDependency = FALSE;
	}

	m_nIndexSave = -1;

	if (bForceAddToProject)
	{
		m_fCreateNewPrev = m_fCreateNew;
		m_fCreateNew = FALSE;
	}
	m_fForceAddToWks = bForceAddToProject;

	m_fIsInitialized = FALSE;
	m_fDoubleClicked = FALSE;
}

void CNewProjectDialogTab::DoDataExchange(CDataExchange* pDX)
{
	CDlgTab::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CNewProjectDialogTab)
	DDX_Control(pDX, IDC_PROJECTTYPES, m_ProjectTypes);
	DDX_Check(pDX, IDC_CREATENEWWORKSPACE, m_fCreateNew);
	DDX_Check(pDX, IDC_DEPENDENCYOF, m_fDependency);
	DDX_CBIndex(pDX, IDC_DEPENDENCYPROJECT, m_nIndexProject);
	DDX_LBIndex(pDX, IDC_PLATFORMS, m_nIndexPlatform);
	DDX_Text(pDX, IDC_NAMEFORPROJECT, m_strFileName);
	DDX_Text(pDX, IDC_LOCATIONFORPROJECT, m_strPathName);
	//}}AFX_DATA_MAP
}

BOOL CNewProjectDialogTab::OnInitDialog()
{
	CWaitCursor wc;
	UpdateData(FALSE);

	m_fUserChangedLocation = FALSE;

	// Subclass the check listbox
	VERIFY(m_Platforms.SubclassDlgItem(IDC_PLATFORMS, this));
	
	CPtrArray paProjects;
//	paProjects.RemoveAll();

	IProjectWorkspace *pInterface;
	pInterface = g_IdeInterface.GetProjectWorkspace();
	ASSERT(pInterface != NULL);
	pInterface->GetProjectTypes(&paProjects);

//	m_imageListNormal.Create(32, 32, ILC_COLOR, 1, 1);
//	m_imageListNormal.SetBkColor(GetSysColor(COLOR_WINDOW));
	m_imageListSmall.Create(16, 16, ILC_COLOR, 1, 1);
	m_imageListSmall.SetBkColor(GetSysColor(COLOR_WINDOW));

	// Set the generic members of the LV_ITEM stuct:
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvItem.iSubItem = 0;
	lvItem.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
	lvItem.state = 0;

	BOOL fSupportsDeps = FALSE;
	int nSize;
	int nColumnWidth = 0;
	if ((nSize = paProjects.GetSize()) != 0)
	{
		for (int nIndex = 0; nIndex < nSize; nIndex++)
		{
			CProjTypeItem *pItem = (CProjTypeItem *)paProjects.GetAt(nIndex);

			ASSERT(pItem->m_hIcon != NULL);
		//	m_imageListNormal.Add(pItem->m_hIcon);
			m_imageListSmall.Add(pItem->m_hIcon);

			lvItem.iItem = nIndex;
			lvItem.pszText = (LPTSTR)(LPCTSTR)pItem->m_strName;
			lvItem.iImage = nIndex;
			lvItem.lParam = (LPARAM)pItem;

			VERIFY(m_ProjectTypes.InsertItem(&lvItem) != -1);

			if ((nIndex == m_nSelected) && (pItem->m_bSupportDependency))
				fSupportsDeps = TRUE;

			nColumnWidth = max(nColumnWidth, m_ProjectTypes.GetStringWidth(pItem->m_strName));
		}
	}

	m_ProjectTypes.SetColumnWidth(-1, nColumnWidth + COLUMNEXTRA);
	m_ProjectTypes.SetItemState(m_nSelected, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

//	m_ProjectTypes.SetImageList(&m_imageListNormal, LVSIL_NORMAL);
	m_ProjectTypes.SetImageList(&m_imageListSmall, LVSIL_SMALL);

#ifndef ENABLE_LISTVIEW_MODES
	SetListCtrlStyle(&m_ProjectTypes, LVS_SMALLICON);
	SetListCtrlStyle(&m_ProjectTypes, LVS_LIST);
#else	// ENABLE_LISTVIEW_MODES
	switch (c_ViewMode)
	{
		case largeicon:
			::CheckRadioButton(m_hWnd, IDC_LARGEICONVIEW, IDC_LISTVIEW, IDC_LARGEICONVIEW);
			SetListCtrlStyle(&m_ProjectTypes, LVS_SMALLICON);
			SetListCtrlStyle(&m_ProjectTypes, LVS_ICON);
			break;

		case smallicon:
			::CheckRadioButton(m_hWnd, IDC_LARGEICONVIEW, IDC_LISTVIEW, IDC_SMALLICONVIEW);
			SetListCtrlStyle(&m_ProjectTypes, LVS_ICON);
			SetListCtrlStyle(&m_ProjectTypes, LVS_SMALLICON);
			break;

		case list:
			::CheckRadioButton(m_hWnd, IDC_LARGEICONVIEW, IDC_LISTVIEW, IDC_LISTVIEW);
			SetListCtrlStyle(&m_ProjectTypes, LVS_ICON);
			SetListCtrlStyle(&m_ProjectTypes, LVS_LIST);
			break;
	}
#endif	// ENABLE_LISTVIEW_MODES

	BOOL fWorkspaceInit = (pInterface->IsWorkspaceInitialised() == S_OK);
	BOOL fWorkspaceTemp = (pInterface->IsWorkspaceTemporary() == S_OK);
	if (!fWorkspaceInit || fWorkspaceTemp)
		m_fCreateNew = TRUE;
	GetDlgItem(IDC_ADDTOCURRENTWORKSPACE)->EnableWindow(fWorkspaceInit && !fWorkspaceTemp);

	((CButton *)GetDlgItem(IDC_CREATENEWWORKSPACE))->SetCheck(m_fCreateNew);
	((CButton *)GetDlgItem(IDC_ADDTOCURRENTWORKSPACE))->SetCheck(!m_fCreateNew);

	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));

	m_nIndexProject = 0;
	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_DEPENDENCYPROJECT);
	BOOL fComboEmpty = TRUE;
	POSITION pos = pParent->m_strlistProjects.GetHeadPosition();
	if (pos != NULL)
	{
		POSITION pos2 = pParent->m_ptrlistBuilders.GetHeadPosition();
		while (pos != NULL)
		{
			ASSERT(pos2 != NULL);
			CString strProject = pParent->m_strlistProjects.GetNext(pos);
			IPkgProject *pProject = (IPkgProject *)pParent->m_ptrlistBuilders.GetNext(pos2);
			LPCOLESTR lpszProjectName, lpszProjectPath;
			DWORD dwPackageID, dwFlags;
			// REVIEW [patbr]: should be able to pass NULL in for arguments we are not interested in!
			VERIFY(SUCCEEDED(pProject->GetProjectInfo(&lpszProjectName, &lpszProjectPath, &dwPackageID, &dwFlags)));
			if (dwFlags & GPI_PROJSUPPORTSDEPS)
			{
				int nIndex = pComboBox->InsertString(-1, strProject);
				pComboBox->SetItemData(nIndex, (DWORD)pProject);
				if (pProject == pParent->m_pActiveProject)
					m_nIndexProject = nIndex;
				fComboEmpty = FALSE;
			}
		}
		ASSERT(pos2 == NULL);
	}

	pComboBox->SetCurSel(m_nIndexProject);

	GetDlgItem(IDC_DEPENDENCYOF)->EnableWindow(fSupportsDeps && !m_fCreateNew && !fComboEmpty);
	GetDlgItem(IDC_DEPENDENCYPROJECT)->EnableWindow(fSupportsDeps && !m_fCreateNew && !fComboEmpty && m_fDependency);

	m_nIndexSave = -1;
	OnSelectType(NULL, NULL);

	// limit length of project name
	((CEdit *)GetDlgItem(IDC_NAMEFORPROJECT))->LimitText(_MAX_FNAME);

	m_fIsInitialized = TRUE;

	if (m_nFocusCtrlId == -1)
		return(TRUE);

	CWnd *pWnd = GetDlgItem(m_nFocusCtrlId);
	if (pWnd == NULL)
		return(TRUE);

	GotoDlgCtrl(pWnd);
	return(FALSE);
}

BOOL CNewProjectDialogTab::Activate(CTabbedDialog* pParentWnd, CPoint position)
{
	BOOL fRet = CDlgTab::Activate(pParentWnd, position);
	EnableButtons();
	return fRet;
}

void CNewProjectDialogTab::Deactivate(CTabbedDialog* pParentWnd)
{
	CDlgTab::Deactivate(pParentWnd);
}

BOOL CNewProjectDialogTab::ValidateTab()
{
	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));

	if (pParent->IsCommitting() || m_fDoubleClicked)
	{
		if (m_ProjectTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED) == -1)
		{
			MsgBox(Error, IDS_ERR_PROJECT_TYPE);
			m_ProjectTypes.SetFocus();
			return FALSE;
		}

		if (m_Platforms.GetCount() > 0)
		{
			BOOL fPlatformChecked = FALSE;
			for (int i = 0; i < m_Platforms.GetCount(); i++)
			{
				if (m_Platforms.GetCheck(i))
					fPlatformChecked = TRUE;
			}

			if (!fPlatformChecked)
			{
				MsgBox(Error, IDS_ERR_NO_PLATFORMS);
				m_Platforms.SetFocus();
				return(FALSE);
			}
		}

		((CEdit *)GetDlgItem(IDC_NAMEFORPROJECT))->GetWindowText(m_strFileName);
		((CEdit *)GetDlgItem(IDC_LOCATIONFORPROJECT))->GetWindowText(m_strPathName);

		// trim leading and trailing spaces from filename...
		m_strFileName.TrimLeft();
		m_strFileName.TrimRight();

		if (!IsProjectNameValid(m_strFileName))
		{
			MsgBox(Error, IDS_INVALID_PROJECTNAME);
			GotoDlgCtrl(GetDlgItem(IDC_NAMEFORPROJECT));
			return FALSE;
		}

		if (IsDlgButtonChecked(IDC_ADDTOCURRENTWORKSPACE))
		{
			if(g_IdeInterface.GetProjectWorkspace()->CanAddProject(m_strFileName)==S_FALSE)
			{
				MsgBox(Error, IDS_EXISTING_PROJECT2);
				GotoDlgCtrl(GetDlgItem(IDC_NAMEFORPROJECT));
				return FALSE;
			}
		}

		int nIndex;
		if ((nIndex = m_strPathName.ReverseFind('\\')) == (m_strPathName.GetLength() - 1))
			m_strPathName = m_strPathName.Left(nIndex);
		
		CString strTestFile;
		CreateFileError cfe;

		if (IsDlgButtonChecked(IDC_CREATENEWWORKSPACE))
		{
			strTestFile = m_strFileName;
			strTestFile += WORKSPACE_EXT;
			cfe = CanCreateFile(strTestFile, m_strPathName, FALSE);
			if (cfe == cfeFileExists)
			{
				MsgBox(Error, IDS_EXISTING_WORKSPACE);
				GotoDlgCtrl(GetDlgItem(IDC_NAMEFORPROJECT));
				return FALSE;
			}
			else if (cfe == cfeInvalidFile || cfe == cfeCannotCreateFile)
			{
				if (strTestFile.GetLength() + m_strPathName.GetLength() >= _MAX_PATH - 1)
				{
					CString strMsg;
					MsgBox(Error, MsgText(strMsg, IDS_TOOLONG_FILENAME, strTestFile, m_strPathName));
				}
				else
					MsgBox(Error, IDS_INVALID_FILENAME);
				GotoDlgCtrl(GetDlgItem(IDC_NAMEFORPROJECT));
				return FALSE;
			}
			else if (cfe == cfeInvalidPath || cfe == cfeCannotCreatePath)
			{
				MsgBox(Error, IDS_INVALID_PATHNAME);
				GotoDlgCtrl(GetDlgItem(IDC_LOCATIONFORPROJECT));
				return FALSE;
			}
		}

		strTestFile = m_strFileName;
		strTestFile += BUILDER_EXT;
		cfe = CanCreateFile(strTestFile, m_strPathName, FALSE);
		if (cfe == cfeFileExists)
		{
			MsgBox(Error, IDS_EXISTING_PROJECT);
			GotoDlgCtrl(GetDlgItem(IDC_NAMEFORPROJECT));
			return FALSE;
		}
		else if (cfe == cfeInvalidFile || cfe == cfeCannotCreateFile)
		{
			if (strTestFile.GetLength() + m_strPathName.GetLength() >= _MAX_PATH - 1)
			{
				CString strMsg;
				MsgBox(Error, MsgText(strMsg, IDS_TOOLONG_FILENAME, strTestFile, m_strPathName));
			}
			else
				MsgBox(Error, IDS_INVALID_FILENAME);
			GotoDlgCtrl(GetDlgItem(IDC_NAMEFORPROJECT));
			return FALSE;
		}
		else if (cfe == cfeInvalidPath || cfe == cfeCannotCreatePath)
		{
			MsgBox(Error, IDS_INVALID_PATHNAME);
			GotoDlgCtrl(GetDlgItem(IDC_LOCATIONFORPROJECT));
			return FALSE;
		}
	}

	return(TRUE);
}

void CNewProjectDialogTab::CommitTab()
{
	CDlgTab::CommitTab();

	m_nSelected = m_ProjectTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	CProjTypeItem *pItem = (CProjTypeItem *)m_ProjectTypes.GetItemData(m_nSelected);

	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));
	pParent->SetType(CPartTemplate::newproject);
	pParent->SetProjectProvider(pItem->m_pProjectProvider);
	pParent->SetProjectType(pItem->m_dwProject_ID);

	// trim leading and trailing spaces from filename...
	m_strFileName.TrimLeft();
	m_strFileName.TrimRight();

	pParent->SetFileName(m_strFileName);
	pParent->SetPathName(m_strPathName);
	pParent->SetCreateNew(IsDlgButtonChecked(IDC_CREATENEWWORKSPACE));

	if (!m_fCreateNew && m_fDependency)
	{
		CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_DEPENDENCYPROJECT);
		int nItem = pComboBox->GetCurSel();
		IPkgProject *pProject = (IPkgProject *)pComboBox->GetItemDataPtr(nItem);
		pParent->SetProject(pProject);
	}
	else
		pParent->SetProject(NULL);

	// user may have changed default platform selection, so notify provider
	for (int i = 0; i < m_Platforms.GetCount(); i++)
		pItem->m_pProjectProvider->ChangePlatformSelection(i, m_Platforms.GetCheck(i));

	int nIndex = m_ProjectTypes.GetNextItem(-1, LVNI_ALL);
	while (nIndex != -1)
	{
		CProjTypeItem *pItem = (CProjTypeItem *)m_ProjectTypes.GetItemData(nIndex);
		delete pItem;
		nIndex = m_ProjectTypes.GetNextItem(nIndex, LVNI_ALL);
	}

	// update the default workspace/project directory
	if (m_fCreateNew)
	{
		CDir dir;
		dir.CreateFromStringEx(m_strPathName, TRUE);
		if (!dir.IsRootDir())
			dir.RemoveLastSubdirName();
		WriteRegWorkspaceDir((const TCHAR *)dir);
	}

	if (m_fForceAddToWks)
		m_fCreateNew = m_fCreateNewPrev;
}

void CNewProjectDialogTab::CancelTab()
{
	CDlgTab::CancelTab();

	int nIndex = m_ProjectTypes.GetNextItem(-1, LVNI_ALL);
	while (nIndex != -1)
	{
		CProjTypeItem *pItem = (CProjTypeItem *)m_ProjectTypes.GetItemData(nIndex);
		delete pItem;
		nIndex = m_ProjectTypes.GetNextItem(nIndex, LVNI_ALL);
	}
	IProjectWorkspace *pInterface;
	pInterface = g_IdeInterface.GetProjectWorkspace();
	ASSERT(pInterface != NULL);
	pInterface->ReleaseProjectTypeInfo();

	if (m_fForceAddToWks)
		m_fCreateNew = m_fCreateNewPrev;
}

void CNewProjectDialogTab::OnBrowse()
{
	CDir dir;
	if (!dir.CreateFromStringEx(m_strPathName, TRUE))
	{
		CString strWorkspaceDir;
		GetRegWorkspaceDir(strWorkspaceDir);
		VERIFY(dir.CreateFromStringEx(strWorkspaceDir, TRUE));
	}

	CDirChooser dlg((const TCHAR *)dir);
	if (dlg.DoModal() == IDOK)
	{
		if (m_strPathName.Compare(dlg.GetPathName()) != 0)
		{
			m_strPathName = dlg.GetPathName();
			((CEdit *)GetDlgItem(IDC_NAMEFORPROJECT))->GetWindowText(m_strFileName);
			EnsureSlashAtEnd(m_strPathName);
			m_strPathName += m_strFileName;
			GetDlgItem(IDC_LOCATIONFORPROJECT)->SetWindowText(m_strPathName);
			m_fUserChangedLocation = TRUE;
		}
	}
	EnableButtons();
}

#ifdef ENABLE_LISTVIEW_MODES
void CNewProjectDialogTab::OnClickLargeIconView()
{
	SetListCtrlStyle(&m_ProjectTypes, LVS_ICON);
	c_ViewMode = largeicon;
}

void CNewProjectDialogTab::OnClickSmallIconView()
{
	SetListCtrlStyle(&m_ProjectTypes, LVS_SMALLICON);
	c_ViewMode = smallicon;
}

void CNewProjectDialogTab::OnClickListView()
{
	SetListCtrlStyle(&m_ProjectTypes, LVS_LIST);
	c_ViewMode = list;
}
#endif	// ENABLE_LISTVIEW_MODES

void CNewProjectDialogTab::OnClickCreateNew()
{
	GetDlgItem(IDC_DEPENDENCYOF)->EnableWindow(FALSE);
	GetDlgItem(IDC_DEPENDENCYPROJECT)->EnableWindow(FALSE);

	SetAppropriateLocation();
}

void CNewProjectDialogTab::OnClickAddToCurrent()
{
	m_nSelected = m_ProjectTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	if (m_nSelected == -1)
	{
		GetDlgItem(IDC_DEPENDENCYOF)->EnableWindow(FALSE);
		GetDlgItem(IDC_DEPENDENCYPROJECT)->EnableWindow(FALSE);
	}
	else
	{
		CProjTypeItem *pItem = (CProjTypeItem *)m_ProjectTypes.GetItemData(m_nSelected);

		if (pItem->m_bSupportDependency && ((CComboBox *)GetDlgItem(IDC_DEPENDENCYPROJECT))->GetCount() >= 1)
		{
			GetDlgItem(IDC_DEPENDENCYOF)->EnableWindow(TRUE);
			GetDlgItem(IDC_DEPENDENCYPROJECT)->EnableWindow(IsDlgButtonChecked(IDC_DEPENDENCYOF));
		}
		else
		{
			GetDlgItem(IDC_DEPENDENCYOF)->EnableWindow(FALSE);
			GetDlgItem(IDC_DEPENDENCYPROJECT)->EnableWindow(FALSE);
		}
	}

	SetAppropriateLocation();
}

void CNewProjectDialogTab::OnClickDependencyOf()
{
	GetDlgItem(IDC_DEPENDENCYPROJECT)->EnableWindow(IsDlgButtonChecked(IDC_DEPENDENCYOF));
}

void CNewProjectDialogTab::OnDblClkType(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_fDoubleClicked = TRUE;
	if (ValidateTab())
	{
		CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
		ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));
		pParent->OnOK();
	}
	m_fDoubleClicked = FALSE;
}

void CNewProjectDialogTab::OnSelectType(NMHDR* pNMHDR, LRESULT* pResult)
{
	int nIndex = m_ProjectTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	m_Platforms.ResetContent();
	if (nIndex != -1)
	{
		CProjTypeItem *pItem = (CProjTypeItem *)m_ProjectTypes.GetItemData(nIndex);

		if (pItem->m_bSupportDependency && IsDlgButtonChecked(IDC_ADDTOCURRENTWORKSPACE) && 
			((CComboBox *)GetDlgItem(IDC_DEPENDENCYPROJECT))->GetCount() >= 1)
		{
			GetDlgItem(IDC_DEPENDENCYOF)->EnableWindow(TRUE);
			GetDlgItem(IDC_DEPENDENCYPROJECT)->EnableWindow(IsDlgButtonChecked(IDC_DEPENDENCYOF));
		}
		else
		{
			GetDlgItem(IDC_DEPENDENCYOF)->EnableWindow(FALSE);
			GetDlgItem(IDC_DEPENDENCYPROJECT)->EnableWindow(FALSE);
		}

		if (nIndex != m_nIndexSave)
		{
			LPCOLESTR *ppszPlatforms;
			BOOL *pbDefSelections;
			ULONG ulCount;
			pItem->m_pProjectProvider->GetPlatforms(pItem->m_dwProject_ID, &ppszPlatforms, &pbDefSelections, &ulCount);

		//	GetDlgItem(IDC_PLATSTATIC)->EnableWindow(ulCount != 0L);
		//	m_Platforms.EnableWindow(ulCount != 0L);
			GetDlgItem(IDC_PLATSTATIC)->ShowWindow(ulCount != 0L ? SW_SHOW : SW_HIDE);
			GetDlgItem(IDC_PLATFORMS)->ShowWindow(ulCount != 0L ? SW_SHOW : SW_HIDE);

			CString strEntry;
			for (int i = 0; i < (int)ulCount; i++)
			{
				strEntry = ppszPlatforms[i];
				CoTaskMemFree((void *)ppszPlatforms[i]);
				VERIFY(m_Platforms.InsertString(-1, strEntry) == i);
				m_Platforms.SetCheck(i, pbDefSelections[i]);
			}
			CoTaskMemFree((void *)ppszPlatforms);

			m_nIndexSave = nIndex;

			// user may have changed default platform selection, so notify provider
			for (i = 0; i < m_Platforms.GetCount(); i++)
				pItem->m_pProjectProvider->ChangePlatformSelection(i, m_Platforms.GetCheck(i));
		}
	}
	else
	{
		GetDlgItem(IDC_PLATSTATIC)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PLATFORMS)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_DEPENDENCYOF)->EnableWindow(FALSE);
		GetDlgItem(IDC_DEPENDENCYPROJECT)->EnableWindow(FALSE);
	}
	m_nIndexSave = nIndex;
	SetAppropriateLocation();
	EnableButtons();
}

void CNewProjectDialogTab::OnChangeName()
{
	// When the name of the project changes, we update the location field to indicate the
	// directory that will become the root of the project (that contains the .BLD file).
	//
	// To do this, we just grab what used to be displayed, strip off the old project 
	// name (if it's there) and append the new project name.

	GetDlgItemText(IDC_NAMEFORPROJECT, m_strFileName);
	GetDlgItemText(IDC_LOCATIONFORPROJECT, m_strPathName);

	// trim leading and trailing spaces from filename...
	m_strFileName.TrimLeft();
	m_strFileName.TrimRight();

	// Remove the old project name
	if (m_strPathName.GetLength() >= m_strPrevName.GetLength() && 
		m_strPathName.Right(m_strPrevName.GetLength()).CompareNoCase(m_strPrevName) == 0)
	{
		m_strPathName = m_strPathName.Left(m_strPathName.GetLength() - m_strPrevName.GetLength());
	}

	// Add the new project name
	EnsureSlashAtEnd(m_strPathName);
	m_strPathName += m_strFileName;

	// Update the location control...
	SetDlgItemText(IDC_LOCATIONFORPROJECT, m_strPathName);

	// Keep the string to remove next time...
	m_strPrevName = m_strFileName;
	EnableButtons();
}

void CNewProjectDialogTab::OnChangeLocation()
{
	if (GetFocus() == GetDlgItem(IDC_LOCATIONFORPROJECT))
		m_fUserChangedLocation = TRUE;
	EnableButtons();
}

void CNewProjectDialogTab::EnableButtons()
{
	if (!m_fIsInitialized)
		return;

	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));

	if (pParent->m_btnOk.GetSafeHwnd() == NULL)
		return;

	((CEdit *)GetDlgItem(IDC_NAMEFORPROJECT))->GetWindowText(m_strFileName);
	((CEdit *)GetDlgItem(IDC_LOCATIONFORPROJECT))->GetWindowText(m_strPathName);

	// trim leading and trailing spaces from filename...
	m_strFileName.TrimLeft();
	m_strFileName.TrimRight();

	BOOL fHasItems = (m_ProjectTypes.GetItemCount() > 0);
	BOOL fSelected = (m_ProjectTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED) != -1);
	BOOL fFileName = (!m_strFileName.IsEmpty());
	BOOL fFilePath = (!m_strPathName.IsEmpty());

	if (fHasItems && fSelected && fFileName && fFilePath)
	{
		pParent->SetDefButtonIndex(IDOK - 1);
		pParent->m_btnOk.EnableWindow(TRUE);
	}
	else
	{
		pParent->SetDefButtonIndex(IDCANCEL - 1);
		pParent->m_btnOk.EnableWindow(FALSE);
	}
}

void CNewProjectDialogTab::SetAppropriateLocation()
{
	if (m_fUserChangedLocation)
		return;

	BOOL fUseWorkspaceSubdir = FALSE;

	int nIndex = m_ProjectTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (nIndex != -1)
	{
		CProjTypeItem *pItem = (CProjTypeItem *)m_ProjectTypes.GetItemData(nIndex);
		if (pItem->m_bUseSubDirectories && IsDlgButtonChecked(IDC_ADDTOCURRENTWORKSPACE))
			fUseWorkspaceSubdir = TRUE;
	}

	GetDlgItemText(IDC_NAMEFORPROJECT, m_strFileName);

	if (fUseWorkspaceSubdir)
	{
		// Get the current workspace directory.
		IProjectWorkspace *pInterface;
		pInterface = g_IdeInterface.GetProjectWorkspace();
		ASSERT(pInterface != NULL);
		LPCTSTR pszWorkspacePath;
		pInterface->GetWorkspaceDocPathName(&pszWorkspacePath);
		CString strWorkspacePath = pszWorkspacePath;
		CDir dir;
		dir.CreateFromPath(strWorkspacePath);
		m_strPathName = (const TCHAR *)dir;
	}
	else
	{
		// Get the user's workspace root directory.
		CString strWorkspaceDir;
		GetRegWorkspaceDir(strWorkspaceDir);
		CDir dir;
		dir.CreateFromStringEx(strWorkspaceDir, TRUE);
		m_strPathName = (const TCHAR *)dir;
	}

	// Add the new project name
	EnsureSlashAtEnd(m_strPathName);
	m_strPathName += m_strFileName;

	// Update the location control...
	SetDlgItemText(IDC_LOCATIONFORPROJECT, m_strPathName);
}

BEGIN_MESSAGE_MAP(CNewProjectDialogTab, CDlgTab)
	//{{AFX_MSG_MAP (CNewProjectDialogTab)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
#ifdef ENABLE_LISTVIEW_MODES
	ON_BN_CLICKED(IDC_LARGEICONVIEW, OnClickLargeIconView)
	ON_BN_CLICKED(IDC_SMALLICONVIEW, OnClickSmallIconView)
	ON_BN_CLICKED(IDC_LISTVIEW, OnClickListView)
#endif	// ENABLE_LISTVIEW_MODES
	ON_BN_CLICKED(IDC_CREATENEWWORKSPACE, OnClickCreateNew)
	ON_BN_CLICKED(IDC_ADDTOCURRENTWORKSPACE, OnClickAddToCurrent)
	ON_BN_CLICKED(IDC_DEPENDENCYOF, OnClickDependencyOf)
	ON_NOTIFY(NM_DBLCLK, IDC_PROJECTTYPES, OnDblClkType)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PROJECTTYPES, OnSelectType)
	ON_EN_CHANGE(IDC_NAMEFORPROJECT, OnChangeName)
	ON_EN_CHANGE(IDC_LOCATIONFORPROJECT, OnChangeLocation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewWorkspaceDialogTab

int CNewWorkspaceDialogTab::m_nSelected;
CString CNewWorkspaceDialogTab::m_strFileName;
CString CNewWorkspaceDialogTab::m_strPathName;
CString CNewWorkspaceDialogTab::m_strPrevName;
int CNewWorkspaceDialogTab::m_nFocusCtrlId = -1;
#ifdef ENABLE_LISTVIEW_MODES
VIEW_MODE CNewWorkspaceDialogTab::c_ViewMode = smallicon;
#endif	// ENABLE_LISTVIEW_MODES

IMPLEMENT_DYNAMIC(CNewWorkspaceDialogTab, CDlgTab);

CNewWorkspaceDialogTab::CNewWorkspaceDialogTab(BOOL bUseSavedData /*=FALSE*/)
	: CDlgTab(IDDP_FILENEW_WORKSPACES, IDS_WORKSPACESTAB)
{
	if (!bUseSavedData)
	{
		m_nFocusCtrlId = -1;
		m_nSelected = 0;
		m_strFileName.Empty();
		m_strPrevName.Empty();

		// Get the user's workspace root directory.
		CString strWorkspaceDir;
		GetRegWorkspaceDir(strWorkspaceDir);
		CDir dir;
		dir.CreateFromStringEx(strWorkspaceDir, TRUE);
		m_strPathName = (const TCHAR *)dir;
	}
	m_fIsInitialized = FALSE;
	m_fDoubleClicked = FALSE;
}

void CNewWorkspaceDialogTab::DoDataExchange(CDataExchange* pDX)
{
	CDlgTab::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CNewWorkspaceDialogTab)
	DDX_Control(pDX, IDC_WORKSPACETYPES, m_WorkspaceTypes);
	DDX_Text(pDX, IDC_NAMEFORWORKSPACE, m_strFileName);
	DDX_Text(pDX, IDC_LOCATIONFORWORKSPACE, m_strPathName);
	//}}AFX_DATA_MAP
}

BOOL CNewWorkspaceDialogTab::OnInitDialog()
{
	UpdateData(FALSE);

//	m_imageListNormal.Create(32, 32, ILC_COLOR, 1, 1);
//	m_imageListNormal.SetBkColor(GetSysColor(COLOR_WINDOW));
	m_imageListSmall.Create(16, 16, ILC_COLOR, 1, 1);
	m_imageListSmall.SetBkColor(GetSysColor(COLOR_WINDOW));

	POSITION pos = theApp.GetFirstDocTemplatePosition();

	// Set the generic members of the LV_ITEM stuct:
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvItem.iSubItem = 0;
	lvItem.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
	lvItem.state = 0;

	// while loop variables
	CPartTemplate *pTemplate;
	HICON hIcon;
	CString strTemplate;
	int nIndex = 0;
	int nColumnWidth = 0;
	while (pos != NULL)
	{
		pTemplate = (CPartTemplate *)theApp.GetNextDocTemplate(pos);
		ASSERT(pTemplate != NULL);
		if (CPartTemplate::newworkspace == pTemplate->GetTabType() && (hIcon = pTemplate->GetIcon()) &&
			pTemplate->GetDocString(strTemplate, CDocTemplate::fileNewName) && !strTemplate.IsEmpty())
		{
			hIcon = pTemplate->GetIcon();
		//	m_imageListNormal.Add(hIcon);
			m_imageListSmall.Add(hIcon);

			pTemplate->GetDocString(strTemplate, CDocTemplate::fileNewName);
			CString strDisplay;
			strDisplay.FormatMessage(IDS_BLANKWKS, strTemplate);
			lvItem.iItem = nIndex;
			lvItem.pszText = (LPTSTR)(LPCTSTR)strDisplay;
			lvItem.iImage = nIndex;
			lvItem.lParam = (LPARAM)pTemplate;

			VERIFY(m_WorkspaceTypes.InsertItem(&lvItem) != -1);

			nColumnWidth = max(nColumnWidth, m_WorkspaceTypes.GetStringWidth(strDisplay));
			nIndex++;
		}
	}

	m_WorkspaceTypes.SetColumnWidth(-1, nColumnWidth + COLUMNEXTRA);
	m_WorkspaceTypes.SetItemState(m_nSelected, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

//	m_WorkspaceTypes.SetImageList(&m_imageListNormal, LVSIL_NORMAL);
	m_WorkspaceTypes.SetImageList(&m_imageListSmall, LVSIL_SMALL);

#ifndef ENABLE_LISTVIEW_MODES
	SetListCtrlStyle(&m_WorkspaceTypes, LVS_SMALLICON);
	SetListCtrlStyle(&m_WorkspaceTypes, LVS_LIST);
#else	// ENABLE_LISTVIEW_MODES
	switch (c_ViewMode)
	{
		case largeicon:
			::CheckRadioButton(m_hWnd, IDC_LARGEICONVIEW, IDC_LISTVIEW, IDC_LARGEICONVIEW);
			SetListCtrlStyle(&m_WorkspaceTypes, LVS_SMALLICON);
			SetListCtrlStyle(&m_WorkspaceTypes, LVS_ICON);
			break;

		case smallicon:
			::CheckRadioButton(m_hWnd, IDC_LARGEICONVIEW, IDC_LISTVIEW, IDC_SMALLICONVIEW);
			SetListCtrlStyle(&m_WorkspaceTypes, LVS_ICON);
			SetListCtrlStyle(&m_WorkspaceTypes, LVS_SMALLICON);
			break;

		case list:
			::CheckRadioButton(m_hWnd, IDC_LARGEICONVIEW, IDC_LISTVIEW, IDC_LISTVIEW);
			SetListCtrlStyle(&m_WorkspaceTypes, LVS_ICON);
			SetListCtrlStyle(&m_WorkspaceTypes, LVS_LIST);
			break;
	}
#endif	// ENABLE_LISTVIEW_MODES

	// limit length of workspace name
	((CEdit *)GetDlgItem(IDC_NAMEFORWORKSPACE))->LimitText(_MAX_FNAME);

	m_fIsInitialized = TRUE;

	if (m_nFocusCtrlId == -1)
		return(TRUE);

	CWnd *pWnd = GetDlgItem(m_nFocusCtrlId);
	if (pWnd == NULL)
		return(TRUE);

	GotoDlgCtrl(pWnd);
	return(FALSE);
}

BOOL CNewWorkspaceDialogTab::Activate(CTabbedDialog* pParentWnd, CPoint position)
{
	BOOL fRet = CDlgTab::Activate(pParentWnd, position);
	EnableButtons();
	return fRet;
}

void CNewWorkspaceDialogTab::Deactivate(CTabbedDialog* pParentWnd)
{
	CDlgTab::Deactivate(pParentWnd);
}

BOOL CNewWorkspaceDialogTab::ValidateTab()
{
	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));

	if (pParent->IsCommitting() || m_fDoubleClicked)
	{
		if (m_WorkspaceTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED) == -1)
		{
			MsgBox(Error, IDS_ERR_WORKSPACE_TYPE);
			m_WorkspaceTypes.SetFocus();
			return FALSE;
		}

		((CEdit *)GetDlgItem(IDC_NAMEFORWORKSPACE))->GetWindowText(m_strFileName);
		((CEdit *)GetDlgItem(IDC_LOCATIONFORWORKSPACE))->GetWindowText(m_strPathName);

		// trim leading and trailing spaces from filename...
		m_strFileName.TrimLeft();
		m_strFileName.TrimRight();

		if (!IsProjectNameValid(m_strFileName))
		{
			MsgBox(Error, IDS_INVALID_PROJECTNAME);
			GotoDlgCtrl(GetDlgItem(IDC_NAMEFORWORKSPACE));
			return FALSE;
		}

		int nIndex;
		if ((nIndex = m_strPathName.ReverseFind('\\')) == (m_strPathName.GetLength() - 1))
			m_strPathName = m_strPathName.Left(nIndex);
		
		CString strTestFile;
		strTestFile = m_strFileName;
		strTestFile += WORKSPACE_EXT;
		CreateFileError cfe = CanCreateFile(strTestFile, m_strPathName, FALSE);
		if (cfe == cfeFileExists)
		{
			MsgBox(Error, IDS_EXISTING_WORKSPACE);
			GotoDlgCtrl(GetDlgItem(IDC_NAMEFORWORKSPACE));
			return FALSE;
		}
		else if (cfe == cfeInvalidFile || cfe == cfeCannotCreateFile)
		{
			if (strTestFile.GetLength() + m_strPathName.GetLength() >= _MAX_PATH - 1)
			{
				CString strMsg;
				MsgBox(Error, MsgText(strMsg, IDS_TOOLONG_FILENAME, strTestFile, m_strPathName));
			}
			else
				MsgBox(Error, IDS_INVALID_FILENAME);
			GotoDlgCtrl(GetDlgItem(IDC_NAMEFORWORKSPACE));
			return FALSE;
		}
		else if (cfe == cfeInvalidPath || cfe == cfeCannotCreatePath)
		{
			MsgBox(Error, IDS_INVALID_PATHNAME);
			GotoDlgCtrl(GetDlgItem(IDC_LOCATIONFORWORKSPACE));
			return FALSE;
		}

		m_nSelected = m_WorkspaceTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		CPartTemplate *pTemplate = (CPartTemplate *)m_WorkspaceTypes.GetItemData(m_nSelected);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CPartTemplate)));
		CString strCreate;
		strCreate = m_strPathName;
		EnsureSlashAtEnd(strCreate);
		strCreate += m_strFileName;
		if (!pTemplate->CanCreateDocument(strCreate))
		{
			// pTemplate is responsible for issuing appropriate error message!
			return FALSE;
		}
	}

	return(TRUE);
}

void CNewWorkspaceDialogTab::CommitTab()
{
	CDlgTab::CommitTab();

	m_nSelected = m_WorkspaceTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	CDocTemplate *pTemplate = (CDocTemplate *)m_WorkspaceTypes.GetItemData(m_nSelected);

	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));
	pParent->SetType(CPartTemplate::newworkspace);
	pParent->SetTemplate(pTemplate);

	// trim leading and trailing spaces from filename...
	m_strFileName.TrimLeft();
	m_strFileName.TrimRight();

	pParent->SetFileName(m_strFileName);
	pParent->SetPathName(m_strPathName);

	// update the default workspace/project directory
	CDir dir;
	dir.CreateFromStringEx(m_strPathName, TRUE);
	if (!dir.IsRootDir())
		dir.RemoveLastSubdirName();
	WriteRegWorkspaceDir((const TCHAR *)dir);
}

void CNewWorkspaceDialogTab::CancelTab()
{
	CDlgTab::CancelTab();
}

void CNewWorkspaceDialogTab::OnBrowse()
{
	CDir dir;
	if (!dir.CreateFromStringEx(m_strPathName, TRUE))
	{
		CString strWorkspaceDir;
		GetRegWorkspaceDir(strWorkspaceDir);
		VERIFY(dir.CreateFromStringEx(strWorkspaceDir, TRUE));
	}

	CDirChooser dlg((const TCHAR *)dir);
	if (dlg.DoModal() == IDOK)
	{
		if (m_strPathName.Compare(dlg.GetPathName()) != 0)
		{
			m_strPathName = dlg.GetPathName();
			((CEdit *)GetDlgItem(IDC_NAMEFORWORKSPACE))->GetWindowText(m_strFileName);
			EnsureSlashAtEnd(m_strPathName);
			m_strPathName += m_strFileName;
			GetDlgItem(IDC_LOCATIONFORWORKSPACE)->SetWindowText(m_strPathName);
		}
	}
	EnableButtons();
}

#ifdef ENABLE_LISTVIEW_MODES
void CNewWorkspaceDialogTab::OnClickLargeIconView()
{
	SetListCtrlStyle(&m_WorkspaceTypes, LVS_ICON);
	c_ViewMode = largeicon;
}

void CNewWorkspaceDialogTab::OnClickSmallIconView()
{
	SetListCtrlStyle(&m_WorkspaceTypes, LVS_SMALLICON);
	c_ViewMode = smallicon;
}

void CNewWorkspaceDialogTab::OnClickListView()
{
	SetListCtrlStyle(&m_WorkspaceTypes, LVS_LIST);
	c_ViewMode = list;
}
#endif	// ENABLE_LISTVIEW_MODES

void CNewWorkspaceDialogTab::OnDblClkType(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_fDoubleClicked = TRUE;
	if (ValidateTab())
	{
		CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
		ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));
		pParent->OnOK();
	}
	m_fDoubleClicked = FALSE;
}

void CNewWorkspaceDialogTab::OnSelectType(NMHDR* pNMHDR, LRESULT* pResult)
{
	EnableButtons();
}

void CNewWorkspaceDialogTab::OnChangeName()
{
	// When the name of the project changes, we update the location field to indicate the
	// directory that will become the root of the project (that contains the .BLD file).
	//
	// To do this, we just grab what used to be displayed, strip off the old project 
	// name (if it's there) and append the new project name.

	GetDlgItemText(IDC_NAMEFORWORKSPACE, m_strFileName);
	GetDlgItemText(IDC_LOCATIONFORWORKSPACE, m_strPathName);

	// trim leading and trailing spaces from filename...
	m_strFileName.TrimLeft();
	m_strFileName.TrimRight();

	// Remove the old project name
	if (m_strPathName.GetLength() >= m_strPrevName.GetLength() && 
		m_strPathName.Right(m_strPrevName.GetLength()).CompareNoCase(m_strPrevName) == 0)
	{
		m_strPathName = m_strPathName.Left(m_strPathName.GetLength() - m_strPrevName.GetLength());
	}

	// Add the new project name
	EnsureSlashAtEnd(m_strPathName);
	m_strPathName += m_strFileName;

	// Update the location control...
	SetDlgItemText(IDC_LOCATIONFORWORKSPACE, m_strPathName);

	// Keep the string to remove next time...
	m_strPrevName = m_strFileName;
	EnableButtons();
}

void CNewWorkspaceDialogTab::OnChangeLocation()
{
	EnableButtons();
}

void CNewWorkspaceDialogTab::EnableButtons()
{
	if (!m_fIsInitialized)
		return;

	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));

	if (pParent->m_btnOk.GetSafeHwnd() == NULL)
		return;

	((CEdit *)GetDlgItem(IDC_NAMEFORWORKSPACE))->GetWindowText(m_strFileName);
	((CEdit *)GetDlgItem(IDC_LOCATIONFORWORKSPACE))->GetWindowText(m_strPathName);

	// trim leading and trailing spaces from filename...
	m_strFileName.TrimLeft();
	m_strFileName.TrimRight();

	BOOL fHasItems = (m_WorkspaceTypes.GetItemCount() > 0);
	BOOL fSelected = (m_WorkspaceTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED) != -1);
	BOOL fFileName = (!m_strFileName.IsEmpty());
	BOOL fFilePath = (!m_strPathName.IsEmpty());

	if (fHasItems && fSelected && fFileName && fFilePath)
	{
		pParent->SetDefButtonIndex(IDOK - 1);
		pParent->m_btnOk.EnableWindow(TRUE);
	}
	else
	{
		pParent->SetDefButtonIndex(IDCANCEL - 1);
		pParent->m_btnOk.EnableWindow(FALSE);
	}
}

BEGIN_MESSAGE_MAP(CNewWorkspaceDialogTab, CDlgTab)
	//{{AFX_MSG_MAP (CNewWorkspaceDialogTab)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
#ifdef ENABLE_LISTVIEW_MODES
	ON_BN_CLICKED(IDC_LARGEICONVIEW, OnClickLargeIconView)
	ON_BN_CLICKED(IDC_SMALLICONVIEW, OnClickSmallIconView)
	ON_BN_CLICKED(IDC_LISTVIEW, OnClickListView)
#endif	// ENABLE_LISTVIEW_MODES
	ON_NOTIFY(NM_DBLCLK, IDC_WORKSPACETYPES, OnDblClkType)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_WORKSPACETYPES, OnSelectType)
	ON_EN_CHANGE(IDC_NAMEFORWORKSPACE, OnChangeName)
	ON_EN_CHANGE(IDC_LOCATIONFORWORKSPACE, OnChangeLocation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewOtherDocDialogTab

int CNewOtherDocDialogTab::m_nSelected;
BOOL CNewOtherDocDialogTab::m_fAddToProject = TRUE;
int CNewOtherDocDialogTab::m_nIndex;
CString CNewOtherDocDialogTab::m_strFileName;
CString CNewOtherDocDialogTab::m_strPathName;
int CNewOtherDocDialogTab::m_nFocusCtrlId = -1;
#ifdef ENABLE_LISTVIEW_MODES
VIEW_MODE CNewOtherDocDialogTab::c_ViewMode = smallicon;
#endif	// ENABLE_LISTVIEW_MODES

IMPLEMENT_DYNAMIC(CNewOtherDocDialogTab, CDlgTab);

CNewOtherDocDialogTab::CNewOtherDocDialogTab(BOOL bUseSavedData /*=FALSE*/, BOOL bForceAddToProject /*=FALSE*/)
	: CDlgTab(IDDP_FILENEW_OTHERDOCS, IDS_OTHERDOCSTAB)
{
	if (!bUseSavedData)
	{
		m_nFocusCtrlId = -1;
		m_nSelected = 0;
		m_strFileName.Empty();
	}

	if (m_strPathName.IsEmpty())
	{
		CDir dir;
		dir.CreateFromCurrent();
		m_strPathName = (const TCHAR *)dir;
	}

	if (bForceAddToProject)
	{
		m_fAddToProjectPrev = m_fAddToProject;
		m_fAddToProject = TRUE;
	}
	m_fForceAddToProj = bForceAddToProject;

	m_fIsInitialized = FALSE;
	m_fDoubleClicked = FALSE;
}

void CNewOtherDocDialogTab::DoDataExchange(CDataExchange* pDX)
{
	CDlgTab::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CNewOtherDocDialogTab)
	DDX_Control(pDX, IDC_OTHERDOCTYPES, m_OtherDocTypes);
	DDX_Check(pDX, IDC_ADDOTHERFILETOPROJECT, m_fAddToProject);
	DDX_CBIndex(pDX, IDC_PROJECTFOROTHERFILE, m_nIndex);
	DDX_Text(pDX, IDC_NAMEFOROTHERFILE, m_strFileName);
	DDX_Text(pDX, IDC_LOCATIONFOROTHERFILE, m_strPathName);
	//}}AFX_DATA_MAP
}

BOOL CNewOtherDocDialogTab::OnInitDialog()
{
	UpdateData(FALSE);

//	m_imageListNormal.Create(32, 32, ILC_COLOR, 1, 1);
//	m_imageListNormal.SetBkColor(GetSysColor(COLOR_WINDOW));
	m_imageListSmall.Create(16, 16, ILC_COLOR, 1, 1);
	m_imageListSmall.SetBkColor(GetSysColor(COLOR_WINDOW));

	POSITION pos = theApp.GetFirstDocTemplatePosition();

	// Set the generic members of the LV_ITEM stuct:
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvItem.iSubItem = 0;
	lvItem.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
	lvItem.state = 0;

	// while loop variables
	CPartTemplate *pTemplate;
	HICON hIcon;
	CString strDisplay;
	int nIndex = 0;
	int nColumnWidth = 0;
	while (pos != NULL)
	{
		pTemplate = (CPartTemplate *)theApp.GetNextDocTemplate(pos);
		ASSERT(pTemplate != NULL);
		if (CPartTemplate::newother == pTemplate->GetTabType() && (hIcon = pTemplate->GetIcon()) &&
			pTemplate->GetDocString(strDisplay, CDocTemplate::fileNewName) && !strDisplay.IsEmpty())
		{
			hIcon = pTemplate->GetIcon();
		//	m_imageListNormal.Add(hIcon);
			m_imageListSmall.Add(hIcon);

			pTemplate->GetDocString(strDisplay, CDocTemplate::fileNewName);
			lvItem.iItem = nIndex;
			lvItem.pszText = (LPTSTR)(LPCTSTR)strDisplay;
			lvItem.iImage = nIndex;
			lvItem.lParam = (LPARAM)pTemplate;

			VERIFY(m_OtherDocTypes.InsertItem(&lvItem) != -1);

			nColumnWidth = max(nColumnWidth, m_OtherDocTypes.GetStringWidth(strDisplay));
			nIndex++;
		}
	}

	m_OtherDocTypes.SetColumnWidth(-1, nColumnWidth + COLUMNEXTRA);
	m_OtherDocTypes.SetItemState(m_nSelected, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

//	m_OtherDocTypes.SetImageList(&m_imageListNormal, LVSIL_NORMAL);
	m_OtherDocTypes.SetImageList(&m_imageListSmall, LVSIL_SMALL);

#ifndef ENABLE_LISTVIEW_MODES
	SetListCtrlStyle(&m_OtherDocTypes, LVS_SMALLICON);
	SetListCtrlStyle(&m_OtherDocTypes, LVS_LIST);
#else	// ENABLE_LISTVIEW_MODES
	switch (c_ViewMode)
	{
		case largeicon:
			::CheckRadioButton(m_hWnd, IDC_LARGEICONVIEW, IDC_LISTVIEW, IDC_LARGEICONVIEW);
			SetListCtrlStyle(&m_OtherDocTypes, LVS_SMALLICON);
			SetListCtrlStyle(&m_OtherDocTypes, LVS_ICON);
			break;

		case smallicon:
			::CheckRadioButton(m_hWnd, IDC_LARGEICONVIEW, IDC_LISTVIEW, IDC_SMALLICONVIEW);
			SetListCtrlStyle(&m_OtherDocTypes, LVS_ICON);
			SetListCtrlStyle(&m_OtherDocTypes, LVS_SMALLICON);
			break;

		case list:
			::CheckRadioButton(m_hWnd, IDC_LARGEICONVIEW, IDC_LISTVIEW, IDC_LISTVIEW);
			SetListCtrlStyle(&m_OtherDocTypes, LVS_ICON);
			SetListCtrlStyle(&m_OtherDocTypes, LVS_LIST);
			break;
	}
#endif	// ENABLE_LISTVIEW_MODES

	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));

	CString strProjectDir;
	pos = pParent->m_strlistProjects.GetHeadPosition();
	if (pos != NULL)
	{
		POSITION pos2 = pParent->m_ptrlistBuilders.GetHeadPosition();
		while (pos != NULL)
		{
			CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_PROJECTFOROTHERFILE);
			ASSERT(pos2 != NULL);
			CString strProject = pParent->m_strlistProjects.GetNext(pos);
			IPkgProject *pProject = (IPkgProject *)pParent->m_ptrlistBuilders.GetNext(pos2);
			int nIndex = pComboBox->InsertString(-1, strProject);
			pComboBox->SetItemData(nIndex, (DWORD)pProject);
			if (pProject == pParent->m_pActiveProject)
			{
				pComboBox->SetCurSel(nIndex);

				m_nIndex = nIndex;
				LPCOLESTR pszProjName, pszProjPath;
				DWORD dwPackageID, dwFlags;
				pProject->GetProjectInfo(&pszProjName, &pszProjPath, &dwPackageID, &dwFlags);
				CString strProjPath = pszProjPath;
				CPath path;
				path.Create(strProjPath);
				CDir dir;
				dir.CreateFromPath(path);
				strProjectDir = (const TCHAR *)dir;
				CoTaskMemFree((void *)pszProjName);
				CoTaskMemFree((void *)pszProjPath);
			}
		}
		ASSERT(pos2 == NULL);
	}
	else
	{
		m_fAddToProject = FALSE;
		((CButton *)GetDlgItem(IDC_ADDOTHERFILETOPROJECT))->SetCheck(m_fAddToProject);
		GetDlgItem(IDC_ADDOTHERFILETOPROJECT)->EnableWindow(m_fAddToProject);
	}

	GetDlgItem(IDC_PROJECTFOROTHERFILE)->EnableWindow(m_fAddToProject);
//	GetDlgItem(IDC_NAMEFOROTHERFILE)->EnableWindow(m_fAddToProject);
//	GetDlgItem(IDC_LOCATIONFOROTHERFILE)->EnableWindow(m_fAddToProject);
//	GetDlgItem(IDC_OTHERFILENAMESTATIC)->EnableWindow(m_fAddToProject);
//	GetDlgItem(IDC_OTHERFILELOCATIONSTATIC)->EnableWindow(m_fAddToProject);
//	GetDlgItem(IDC_BROWSE)->EnableWindow(m_fAddToProject);

	if (!strProjectDir.IsEmpty())
		SetDlgItemText(IDC_LOCATIONFOROTHERFILE, strProjectDir);

	// limit length of file name
	((CEdit *)GetDlgItem(IDC_NAMEFOROTHERFILE))->LimitText(_MAX_FNAME);

	m_fIsInitialized = TRUE;

	if (m_nFocusCtrlId == -1)
		return(TRUE);

	CWnd *pWnd = GetDlgItem(m_nFocusCtrlId);
	if (pWnd == NULL)
		return(TRUE);

	GotoDlgCtrl(pWnd);
	return(FALSE);
}

BOOL CNewOtherDocDialogTab::Activate(CTabbedDialog* pParentWnd, CPoint position)
{
	BOOL fRet = CDlgTab::Activate(pParentWnd, position);
	EnableButtons();
	return fRet;
}

void CNewOtherDocDialogTab::Deactivate(CTabbedDialog* pParentWnd)
{
	CDlgTab::Deactivate(pParentWnd);
}

BOOL CNewOtherDocDialogTab::ValidateTab()
{
	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));
	if (pParent->IsCommitting() || m_fDoubleClicked)
	{
		if (m_OtherDocTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED) == -1)
		{
			MsgBox(Error, IDS_ERR_DOCUMENT_TYPE);
			m_OtherDocTypes.SetFocus();
			return FALSE;
		}

		m_nSelected = m_OtherDocTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		CPartTemplate *pTemplate = (CPartTemplate *)m_OtherDocTypes.GetItemData(m_nSelected);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CPartTemplate)));

		((CEdit *)GetDlgItem(IDC_NAMEFOROTHERFILE))->GetWindowText(m_strFileName);

		// trim leading and trailing spaces from filename...
		m_strFileName.TrimLeft();
		m_strFileName.TrimRight();
		CString strCreate;

		if (!m_strFileName.IsEmpty())
		{
			// add default extension for name checking--it will be added on creation.
			if (m_strFileName.Find('.') == -1)
			{
				CString strExtension;
				pTemplate->GetDocString(strExtension, CDocTemplate::filterExt);
				m_strFileName += strExtension;
			}

			((CEdit *)GetDlgItem(IDC_LOCATIONFOROTHERFILE))->GetWindowText(m_strPathName);
			CreateFileError cfe = CanCreateFile(m_strFileName, m_strPathName, TRUE);
			if (cfe == cfeFileExists)
			{
				if (MsgBox(Question, IDS_EXISTING_FILEPATH, MB_YESNO) != IDYES)
					return FALSE;
			}
			else if (cfe == cfeInvalidFile || cfe == cfeCannotCreateFile)
			{
				if (m_strFileName.GetLength() + m_strPathName.GetLength() >= _MAX_PATH - 1)
				{
					CString strMsg;
					MsgBox(Error, MsgText(strMsg, IDS_TOOLONG_FILENAME, m_strFileName, m_strPathName));
				}
				else
					MsgBox(Error, IDS_INVALID_FILENAME);
				GotoDlgCtrl(GetDlgItem(IDC_NAMEFOROTHERFILE));
				return FALSE;
			}
			else if (cfe == cfeInvalidPath || cfe == cfeNonexistentPath || cfe == cfeCannotCreatePath)
			{
				MsgBox(Error, IDS_INVALID_PATHNAME);
				GotoDlgCtrl(GetDlgItem(IDC_LOCATIONFOROTHERFILE));
				return FALSE;
			}

			strCreate = m_strPathName;
			EnsureSlashAtEnd(strCreate);
			strCreate += m_strFileName;

			if (!pTemplate->CanCreateDocument(strCreate))
			{
				// pTemplate is responsible for issuing appropriate error message!
				return FALSE;
			}
		}
		if (IsDlgButtonChecked(IDC_ADDOTHERFILETOPROJECT))
		{
			if (m_strFileName.IsEmpty())
			{
				MsgBox(Error, IDS_NEED_FILENAME);
				GotoDlgCtrl(GetDlgItem(IDC_NAMEFOROTHERFILE));
				return FALSE;
			}

			USES_CONVERSION;

			CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_PROJECTFOROTHERFILE);
			IPkgProject *pProject = (IPkgProject *)pComboBox->GetItemData(m_nIndex);
			if (pProject->CanAddFile(T2W(strCreate), NULL, TRUE) != S_OK)
			{
				return FALSE;
			}
		}
	}

	return(TRUE);
}

void CNewOtherDocDialogTab::CommitTab()
{
	CDlgTab::CommitTab();

	m_nSelected = m_OtherDocTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	CDocTemplate *pTemplate = (CDocTemplate *)m_OtherDocTypes.GetItemData(m_nSelected);

	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));
	pParent->SetType(CPartTemplate::newother);
	pParent->SetTemplate(pTemplate);
	pParent->SetAddToProject(m_fAddToProject);

	// trim leading and trailing spaces from filename...
	m_strFileName.TrimLeft();
	m_strFileName.TrimRight();

	pParent->SetFileName(m_strFileName);
	pParent->SetPathName(m_strPathName);
	if (m_fAddToProject)
	{
		CString strProject;
		CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_PROJECTFOROTHERFILE);
		pComboBox->GetLBText(m_nIndex, strProject);
		DWORD dwData = pComboBox->GetItemData(m_nIndex);
		pParent->SetProject((IPkgProject *)dwData);
	}

	if (m_fForceAddToProj)
		m_fAddToProject = m_fAddToProjectPrev;
}

void CNewOtherDocDialogTab::CancelTab()
{
	CDlgTab::CancelTab();

	if (m_fForceAddToProj)
		m_fAddToProject = m_fAddToProjectPrev;
}

void CNewOtherDocDialogTab::OnBrowse()
{
	CDir dir;
	if (!dir.CreateFromStringEx(m_strPathName, TRUE))
		VERIFY(dir.CreateFromCurrent());

	CDirChooser dlg((const TCHAR *)dir);
	if (dlg.DoModal() == IDOK)
	{
		m_strPathName = dlg.GetPathName();
		GetDlgItem(IDC_LOCATIONFOROTHERFILE)->SetWindowText(m_strPathName);
	}
	EnableButtons();
}

#ifdef ENABLE_LISTVIEW_MODES
void CNewOtherDocDialogTab::OnClickLargeIconView()
{
	SetListCtrlStyle(&m_OtherDocTypes, LVS_ICON);
	c_ViewMode = largeicon;
}

void CNewOtherDocDialogTab::OnClickSmallIconView()
{
	SetListCtrlStyle(&m_OtherDocTypes, LVS_SMALLICON);
	c_ViewMode = smallicon;
}

void CNewOtherDocDialogTab::OnClickListView()
{
	SetListCtrlStyle(&m_OtherDocTypes, LVS_LIST);
	c_ViewMode = list;
}
#endif	// ENABLE_LISTVIEW_MODES

void CNewOtherDocDialogTab::OnClickAddToProject()
{
	GetDlgItem(IDC_PROJECTFOROTHERFILE)->EnableWindow(IsDlgButtonChecked(IDC_ADDOTHERFILETOPROJECT));
//	GetDlgItem(IDC_NAMEFOROTHERFILE)->EnableWindow(IsDlgButtonChecked(IDC_ADDOTHERFILETOPROJECT));
//	GetDlgItem(IDC_LOCATIONFOROTHERFILE)->EnableWindow(IsDlgButtonChecked(IDC_ADDOTHERFILETOPROJECT));
//	GetDlgItem(IDC_OTHERFILENAMESTATIC)->EnableWindow(IsDlgButtonChecked(IDC_ADDOTHERFILETOPROJECT));
//	GetDlgItem(IDC_OTHERFILELOCATIONSTATIC)->EnableWindow(IsDlgButtonChecked(IDC_ADDOTHERFILETOPROJECT));
//	GetDlgItem(IDC_BROWSE)->EnableWindow(IsDlgButtonChecked(IDC_ADDOTHERFILETOPROJECT));
	EnableButtons();
}

void CNewOtherDocDialogTab::OnDblClkType(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_fDoubleClicked = TRUE;
	if (ValidateTab())
	{
		CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
		ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));
		pParent->OnOK();
	}
	m_fDoubleClicked = FALSE;
}

void CNewOtherDocDialogTab::OnSelectType(NMHDR* pNMHDR, LRESULT* pResult)
{
	EnableButtons();
}

void CNewOtherDocDialogTab::OnChangeName()
{
	EnableButtons();
}

void CNewOtherDocDialogTab::OnChangeLocation()
{
	EnableButtons();
}

void CNewOtherDocDialogTab::OnProjectChange()
{
	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_PROJECTFOROTHERFILE);
	m_nIndex = pComboBox->GetCurSel();
	IPkgProject *pProject = (IPkgProject *)pComboBox->GetItemDataPtr(m_nIndex);
	LPCOLESTR pszProjName, pszProjPath;
	DWORD dwPackageID, dwFlags;
	pProject->GetProjectInfo(&pszProjName, &pszProjPath, &dwPackageID, &dwFlags);
	CString strProjPath = pszProjPath;
	CPath path;
	path.Create(strProjPath);
	CDir dir;
	dir.CreateFromPath(path);
	CString strProjectDir = (const TCHAR *)dir;
	SetDlgItemText(IDC_LOCATIONFOROTHERFILE, strProjectDir);
	CoTaskMemFree((void *)pszProjName);
	CoTaskMemFree((void *)pszProjPath);
}

void CNewOtherDocDialogTab::EnableButtons()
{
	if (!m_fIsInitialized)
		return;

	CFileNewTabbedDialog *pParent = (CFileNewTabbedDialog *)GetParent();
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CFileNewTabbedDialog)));

	if (pParent->m_btnOk.GetSafeHwnd() == NULL)
		return;

	((CEdit *)GetDlgItem(IDC_NAMEFOROTHERFILE))->GetWindowText(m_strFileName);
	((CEdit *)GetDlgItem(IDC_LOCATIONFOROTHERFILE))->GetWindowText(m_strPathName);

	// trim leading and trailing spaces from filename...
	m_strFileName.TrimLeft();
	m_strFileName.TrimRight();

	BOOL fHasItems = (m_OtherDocTypes.GetItemCount() > 0);
	BOOL fSelected = (m_OtherDocTypes.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED) != -1);
	BOOL fAddToPrj = (IsDlgButtonChecked(IDC_ADDOTHERFILETOPROJECT));
	BOOL fFileName = (!m_strFileName.IsEmpty());
	BOOL fFilePath = (!m_strPathName.IsEmpty());

	if (fHasItems && fSelected && (!fAddToPrj || (fAddToPrj && fFileName && fFilePath)))
	{
		pParent->SetDefButtonIndex(IDOK - 1);
		pParent->m_btnOk.EnableWindow(TRUE);
	}
	else
	{
		pParent->SetDefButtonIndex(IDCANCEL - 1);
		pParent->m_btnOk.EnableWindow(FALSE);
	}
}

BEGIN_MESSAGE_MAP(CNewOtherDocDialogTab, CDlgTab)
	//{{AFX_MSG_MAP (CNewOtherDocDialogTab)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
#ifdef ENABLE_LISTVIEW_MODES
	ON_BN_CLICKED(IDC_LARGEICONVIEW, OnClickLargeIconView)
	ON_BN_CLICKED(IDC_SMALLICONVIEW, OnClickSmallIconView)
	ON_BN_CLICKED(IDC_LISTVIEW, OnClickListView)
#endif	// ENABLE_LISTVIEW_MODES
	ON_BN_CLICKED(IDC_ADDOTHERFILETOPROJECT, OnClickAddToProject)
	ON_NOTIFY(NM_DBLCLK, IDC_OTHERDOCTYPES, OnDblClkType)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_OTHERDOCTYPES, OnSelectType)
	ON_EN_CHANGE(IDC_NAMEFOROTHERFILE, OnChangeName)
	ON_EN_CHANGE(IDC_LOCATIONFOROTHERFILE, OnChangeLocation)
	ON_CBN_SELCHANGE(IDC_PROJECTFOROTHERFILE, OnProjectChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
