///////////////////////////////////////////////////////////////////////////////
// WORKSPC.CPP
//

// comment at end of file explains how to generate new layouts for MSDEV.

#include "stdafx.h"

#include "workspc.h"
#include "about.h"
#include "bardockx.h"

#include <srcapi.h>
#include <srcguid.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern void EnableClock(BOOL bEnable);
extern BOOL IsClockEnabled();

BOOL g_bReloadProject = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg dialog

//	CWorkspaceCheckList::SetCheck
//		Override CCheckList for check notification to the dialog.

void CWorkspaceCheckList::SetCheck(int nItem, BOOL bCheck)
{
	CCheckList::SetCheck(nItem, bCheck);
}

CWorkspaceDlg::CWorkspaceDlg(CDockManager* pManager, CWnd* pParent /*=NULL*/)
	: CDlgTab(CWorkspaceDlg::IDD, IDS_WORKSPACE)
{
	m_pManager = pManager;
	m_nOrder = 100;

	//{{AFX_DATA_INIT(CWorkspaceDlg)
	m_bAuto = (theApp.m_bWorkspaceAuto != 0);
	m_bStatusbar = (theApp.m_bStatusbar != 0);
	m_bAutosave = gAutosaver.IsEnabled();
	m_iInterval = gAutosaver.GetInterval();
	m_bClock = IsClockEnabled();
	m_bReloadProject = g_bReloadProject;
	m_nFiles = theApp.GetRecentFileListSize();
	m_nProjects = theApp.GetRecentProjectListSize();
	m_nWindows = theApp.m_nWindowMenuItems;
	m_bWindowsSort = theApp.m_bWindowMenuSorted;
	m_bMRUSubmenu = !theApp.m_bMRUOnFileMenu;
	m_bTraditionalMenu=!Menu::IsInCmdBarMode();
	//}}AFX_DATA_INIT
}

void CWorkspaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDlgTab::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorkspaceDlg)
	DDX_Check(pDX, IDC_AUTO_WORKSPACE, m_bAuto);
	DDX_Check(pDX, IDC_STATUSBAR, m_bStatusbar);
	DDX_Check(pDX, IDC_MENU_TRADITIONAL, m_bTraditionalMenu);
	DDX_Check(pDX, IDC_MRU_SUBMENU, m_bMRUSubmenu);
	DDX_Text(pDX, IDC_AUTOSAVE_INTERVAL, m_iInterval);
	DDX_Check(pDX, IDC_CLOCK, m_bClock);
	DDX_Check(pDX, IDC_RELOAD_PROJECT, m_bReloadProject);
	DDX_Text(pDX, IDC_FILES_COUNT, m_nFiles);
	DDV_MinMaxInt(pDX, m_nFiles, 1, 15);
	DDX_Text(pDX, IDC_PROJECTS_COUNT, m_nProjects);
	DDV_MinMaxInt(pDX, m_nProjects, 1, 15);
	DDX_Text(pDX, IDC_WINDOWS_COUNT, m_nWindows);
	DDV_MinMaxInt(pDX, m_nWindows, 1, 31);
	DDX_Check(pDX, IDC_WINDOWS_SORT, m_bWindowsSort);
	//}}AFX_DATA_MAP
#ifdef ENABLE_FILE_AUTOSAVES
	DDX_Check(pDX, IDC_AUTOSAVE_ENABLE, m_bAutosave);
#endif	// ENABLE_FILE_AUTOSAVES
}

//	CWorkspaceDlg::UpdateMDIState
//		For real time MDI switching of views in synch with checkboxes.

void CWorkspaceDlg::UpdateMDIState(int nItem)
{
	ASSERT(nItem >= 0 && nItem < m_aViewWorkers.GetSize());

	CDockWorker* pDocker = GetWorker(nItem);

	// Low overhead if check and visible same
	pDocker->SetMDIState(!m_lbViews.GetCheck(nItem));
}

//	CWorkspaceDlg::GetWorker
//  	Gets the CDockWorker for the given index from m_aViewWorkers, or for
//		the current selection if no index is given.

CDockWorker* CWorkspaceDlg::GetWorker(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_aViewWorkers.GetSize())
		return NULL;
	else
		return (CDockWorker*) m_aViewWorkers.GetAt(nIndex);
}

BEGIN_MESSAGE_MAP(CWorkspaceDlg, CDlgTab)
	//{{AFX_MSG_MAP(CWorkspaceDlg)
	//}}AFX_MSG_MAP
#ifdef ENABLE_FILE_AUTOSAVES
	ON_BN_CLICKED(IDC_AUTOSAVE_ENABLE, OnClickedAutosaveEnable)
#endif	// ENABLE_FILE_AUTOSAVES
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg message handlers

BOOL CWorkspaceDlg::OnInitDialog()
{
	// Initialize the listbox and corresponding worker array.
	m_pManager->ArrayOfType(dtEdit, &m_aViewWorkers);

	VERIFY(m_lbViews.SubclassDlgItem(IDC_VIEWS, this));
	m_lbViews.SetRedraw(FALSE);

	CString str;
	int nViews = m_aViewWorkers.GetSize();
	CDockWorker* pDocker;

	for (int i = 0; i < nViews; i++)
	{
		pDocker = GetWorker(i);

		pDocker->GetText(str);
		m_lbViews.AddString(str);
		m_lbViews.SetCheck(i, pDocker->GetDock() != dpMDI);
	}

	m_lbViews.SetRedraw(TRUE);

#ifdef ENABLE_FILE_AUTOSAVES
	((CStatic *)GetDlgItem(IDC_AUTOSAVETEXT1))->EnableWindow(m_bAutosave);
	((CEdit *)GetDlgItem(IDC_AUTOSAVE_INTERVAL))->EnableWindow(m_bAutosave);
	((CStatic *)GetDlgItem(IDC_AUTOSAVETEXT2))->EnableWindow(m_bAutosave);
#endif

	CEdit *pFiles=(CEdit *)GetDlgItem(IDC_FILES_COUNT);
	CEdit *pProjects=(CEdit *)GetDlgItem(IDC_PROJECTS_COUNT);
	CEdit *pWindows=(CEdit *)GetDlgItem(IDC_WINDOWS_COUNT);

	ASSERT(pFiles);
	ASSERT(pProjects);
	ASSERT(pWindows);
	pFiles->SetLimitText(2);
	pProjects->SetLimitText(2);
	pWindows->SetLimitText(2);

	// Initialize listbox first to avoid flashing.
	CDlgTab::OnInitDialog();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CWorkspaceDlg::ValidateTab()
{
	if(UpdateData(TRUE))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CWorkspaceDlg::CommitTab()
{
	CDlgTab::CommitTab();

	theApp.m_bWorkspaceAuto = (m_bAuto != 0);
	theApp.m_bStatusbar = m_bStatusbar;
	EnableClock(m_bClock);
	g_bReloadProject = m_bReloadProject;
	ShowStatusBar(m_bStatusbar);

	m_pManager->m_pFrame->RecalcLayout();

	// HACK: Yes locking, and redrawing the window is ugly, but it is the
	//       easiest way to batch these commands into one paint, so we can
	//		 allow cancel.

	CDockWorker* pDocker;
	BOOL bDocking, bChecked;
	BOOL bUpdate = FALSE;

	int nViews = m_aViewWorkers.GetSize();
	for (int i = 0; i < nViews; i++)
	{
		pDocker = GetWorker(i);
		bDocking = (pDocker->GetDock() != dpMDI);
		bChecked = m_lbViews.GetCheck(i);
		if (pDocker->IsVisible() &&
			((bChecked && !bDocking) || (!bChecked && bDocking)))
		{
			// We only need to do this hacky update, if we are changing
			// the state of a visible window.
			bUpdate = TRUE;
			break;
		}
	}

	CWnd* pMainWnd = AfxGetApp()->m_pMainWnd;
	if (bUpdate)
		pMainWnd->SetRedraw(FALSE);

	for (i = 0; i < nViews; i++)
		UpdateMDIState(i);

	if(m_bTraditionalMenu==Menu::IsInCmdBarMode())
	{
		Menu::UseHMENU(m_bTraditionalMenu); // make it permanent
		Menu::UpdateMode(FALSE); // and update the mode next time we're idle
	}

	if (bUpdate)
	{
		pMainWnd->SetRedraw(TRUE);
	    pMainWnd->RedrawWindow(NULL, NULL,
	        RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);
	}

	// set recently used list sizes
	theApp.SetRecentFileListSize(m_nFiles);
	theApp.SetRecentProjectListSize(m_nProjects);

	if(theApp.m_bMRUOnFileMenu==m_bMRUSubmenu)
	{
		// state is changing, so update menus. This will alone handle traditional menus
		theApp.m_bMenuDirty=TRUE;

		// Set up state so that any rebuild required will be right
		theApp.m_bMRUOnFileMenu=!m_bMRUSubmenu;

		CBMenuPopup *pMenuFile=theCmdCache.GetMenu(IDM_MENU_FILE);

		CMainFrame *pFrame=(CMainFrame *)AfxGetMainWnd();

		if(pMenuFile)
		{
			if(!pMenuFile->IsDirty())
			{
				// just reset the file menu
				pFrame->ResetMenu(IDM_MENU_FILE);
			}
			else
			{
				// scan the menu, to find what is already there
				int nFileMRUIndex=-1;
				int nProjMRUIndex=-1;
				int nFileMenuIndex=-1;
				int nProjMenuIndex=-1;
				int nExitIndex=-1;

				for(int i=0; i<pMenuFile->GetMenuItemCount(); ++i)
				{
					CBMenuItem *pItem=pMenuFile->GetMenuItem(i);
					if(pItem)
					{
						int nId=pItem->GetCmdID();
						if(	nId>= ID_FILE_MRU_FIRST &&
							nId<= ID_FILE_MRU_LAST &&
							nFileMRUIndex==-1)
						{
							// find the first one of these on the menu
							nFileMRUIndex=i;
						}

						if(	nId>= ID_PROJ_MRU_FIRST &&
							nId<= ID_PROJ_MRU_LAST &&
							nProjMRUIndex==-1)
						{
							// find the first one of these on the menu
							nProjMRUIndex=i;
						}

						if( nId==IDM_MENU_FILEMRU)
						{
							// but find the last one of these
							nFileMenuIndex=i;
						}

						if( nId==IDM_MENU_WKSMRU)
						{
							// but find the last one of these
							nProjMenuIndex=i;
						}

						if(nId==ID_APP_EXIT)
						{
							// and obviously the last one of these
							nExitIndex=i;
						}
					}
				}

				// calculate insertion points
				// insert file mru submenu
				int nFileInsertIndex=-1;

				// there's not already one on the file menu
				if(nFileMRUIndex!=-1)
				{
					nFileInsertIndex=nFileMRUIndex;
				}
				else if(nFileMenuIndex!=-1)
				{
					nFileInsertIndex=nFileMenuIndex;
				}
				else if(nExitIndex!=-1)
				{
					nFileInsertIndex=nExitIndex;
				}
				else if(nProjMRUIndex!=-1)
				{
					nFileInsertIndex=nProjMRUIndex;
				}
				else if(nProjMenuIndex!=-1)
				{
					nFileInsertIndex=nProjMenuIndex;
				}
				else 
				{
					// this is ok, since -1 is a valid insert point
				}

				int nProjInsertIndex=-1;

				// there's not already one on the file menu
				if(nProjMRUIndex!=-1)
				{
					nProjInsertIndex=nProjMRUIndex;
				}
				else if(nProjMenuIndex!=-1)
				{
					nProjInsertIndex=nProjMenuIndex;
				}
				else if(nExitIndex!=-1)
				{
					nProjInsertIndex=nExitIndex;
				}
				else if(nFileMRUIndex!=-1)
				{
					nProjInsertIndex=nFileMRUIndex;
				}
				else if(nFileMenuIndex!=-1)
				{
					nProjInsertIndex=nFileMenuIndex;
				}
				else 
				{
					// this is ok, since -1 is a valid insert point
				}

				// now do the insertions and deletions
				if(m_bMRUSubmenu)
				{
					if(	nProjMenuIndex==-1)
					{
						// insert the file submenu
						pMenuFile->InsertMenu(nProjInsertIndex, MF_BYPOSITION|MF_POPUP, IDM_MENU_WKSMRU);
					}

					if(	nFileMenuIndex==-1)
					{
						// insert the file submenu
						pMenuFile->InsertMenu(nFileInsertIndex, MF_BYPOSITION|MF_POPUP, IDM_MENU_FILEMRU);
					}

					BOOL bDeleteSeparators=FALSE;

					// Now iterate deleting all the standard MRU items, and any separators after file
					for(int i=0; i<pMenuFile->GetMenuItemCount();)
					{
						CBMenuItem *pItem=pMenuFile->GetMenuItem(i);
						if(pItem)
						{
							int nId=pItem->GetCmdID();
							if(	nId>= ID_FILE_MRU_FIRST &&
								nId<= ID_FILE_MRU_LAST)
							{
								pMenuFile->DeleteMenu(i, MF_BYPOSITION);
								bDeleteSeparators=TRUE;

							} 
							else if(	nId>= ID_PROJ_MRU_FIRST &&
										nId<= ID_PROJ_MRU_LAST)
							{
								pMenuFile->DeleteMenu(i, MF_BYPOSITION);
								bDeleteSeparators=FALSE;
							} 
							else if(pItem->GetItemType()==CBMenuItem::MIT_Separator)
							{
								if(bDeleteSeparators)
								{
									pMenuFile->DeleteMenu(i, MF_BYPOSITION);
								}
								else
								{
									++i;
								}
							}
							else
							{
								bDeleteSeparators=FALSE;
								++i;
							}
						}
					}
				}
				else
				{
					if(	nProjMRUIndex==-1)
					{
						// insert the wks item
						pMenuFile->InsertMenu(nProjInsertIndex, MF_BYPOSITION, ID_PROJ_MRU_FIRST);
					}

					if(	nFileMRUIndex==-1)
					{
						// insert the file item
						pMenuFile->InsertMenu(nFileInsertIndex, MF_BYPOSITION|MF_SEPARATOR);
						pMenuFile->InsertMenu(nFileInsertIndex, MF_BYPOSITION, ID_FILE_MRU_FIRST);
						pMenuFile->InsertMenu(nFileInsertIndex, MF_BYPOSITION|MF_SEPARATOR);
					}

					// Now iterate deleting all the submenu items
					for(int i=0; i<pMenuFile->GetMenuItemCount();)
					{
						CBMenuItem *pItem=pMenuFile->GetMenuItem(i);
						if(pItem)
						{
							int nId=pItem->GetCmdID();
							if(	nId== IDM_MENU_FILEMRU)
							{
								pMenuFile->DeleteMenu(i, MF_BYPOSITION);
							} 
							else if(nId== IDM_MENU_WKSMRU)
							{
								pMenuFile->DeleteMenu(i, MF_BYPOSITION);
							} 
							else
							{
								++i;
							}
						}
					}
				}
			}
		}
	}

	// update the number of mnemonics
	theApp.CalculateRecentMnemonics();

	theApp.m_nWindowMenuItems=m_nWindows;
	theApp.m_bWindowMenuSorted=m_bWindowsSort;

#ifdef ENABLE_FILE_AUTOSAVES
	if (m_bAutosave)
		gAutosaver.Enable(m_iInterval);
	else
		gAutosaver.Disable();
#endif
}

#ifdef ENABLE_FILE_AUTOSAVES
void CWorkspaceDlg::OnClickedAutosaveEnable()
{
	int iCheckState;

	iCheckState = ((CButton *)GetDlgItem(IDC_AUTOSAVE_ENABLE))->GetCheck();
	((CStatic *)GetDlgItem(IDC_AUTOSAVETEXT1))->EnableWindow(iCheckState);
	((CEdit *)GetDlgItem(IDC_AUTOSAVE_INTERVAL))->EnableWindow(iCheckState);
	((CStatic *)GetDlgItem(IDC_AUTOSAVETEXT2))->EnableWindow(iCheckState);

	// dolphin 4640 [patbr]
	// MFC will validate entry field even if it is disabled, so put value in
	// entry field as we are disabling it so that UpdateData() will succeed.
	//
	// if we are enabling the field because the accompanying check box has
	// been checked, move the focus to the entry field for user convenience.
	if (iCheckState)
		((CEdit *)GetDlgItem(IDC_AUTOSAVE_INTERVAL))->SetFocus();
	else
		((CEdit *)GetDlgItem(IDC_AUTOSAVE_INTERVAL))->SetWindowText("15");
}
#endif	// ENABLE_FILE_AUTOSAVES

///////////////////////////////////////////////////////////////////////////////
//	CMainFrame
//		Workspace handling members.

static char BASED_CODE szLayoutVersion[] = "Version";
static char BASED_CODE szLayoutKey[] = "Layout%d";
static char BASED_CODE szStatusbar[] = "Statusbar";
static char BASED_CODE szClock[] = "Clock";
static char BASED_CODE szReloadProject[] = "ReloadProject";
static char BASED_CODE szDisableFileDep[] = "DisableFileDep";
static char BASED_CODE szWorkspaceDocs[] = "Documents";

#define RT_LAYOUT MAKEINTRESOURCE(IDRT_LAYOUT)

void CMainFrame::LoadLayout()
{
#ifdef _WIN32
	int nVer = GetRegInt(CDockWorker::s_lpszLayoutSection, szLayoutVersion, 0);
	if (nVer != LAYOUT_VERSION)
	{
		CString strKeyMain = GetRegistryKeyName();
		RegDeleteKey(HKEY_CURRENT_USER, strKeyMain + chKeySep + CDockWorker::s_lpszLayoutSection);
		// olympus 947 [patbr] removed return statement here which
		// was preventing us from executing any of the code below.
	}

	CString str;

	for (int i = 0; i < MANAGER_STATES; i++)
	{
		wsprintf(str.GetBuffer(_MAX_PATH + 1), szLayoutKey, i);
		str.ReleaseBuffer();

		m_ahManagerStates[i] = GetRegData(CDockWorker::s_lpszLayoutSection, str, NULL);
	}

	theApp.m_bStatusbar = GetRegInt(CDockWorker::s_lpszLayoutSection, szStatusbar, TRUE);
	EnableClock(GetRegInt(CDockWorker::s_lpszLayoutSection, szClock, !theApp.m_bWin4));

	// setting the file dependency scanning here. We need to do it here, instead
	// of in one of the packages, since later on we will write it out in the 
	// SaveLayout();
	theApp.m_nDisableFileDep = GetRegInt (CDockWorker::s_lpszLayoutSection, szDisableFileDep, FILEDEP_DISABLE_IFUPTODATE);
	TRACE ("CMainFrame::LoadLayout: DisableFileDep = %d\n", theApp.m_nDisableFileDep);

	// olympus 13617 [patbr]
	// if emulation is set to VC20 and no 'reload project' setting found,
	// then set 'Reload last project on startup' to TRUE (i.e., like VC20).

	BOOL bEmulationIsVC20 = FALSE;
	LPSOURCEQUERY pInterface;
	if (SUCCEEDED(theApp.FindInterface(IID_ISourceQuery, (LPVOID FAR *)&pInterface)))
	{
		ASSERT(pInterface != NULL);
		bEmulationIsVC20 = (pInterface->IsEmulationVC20() == S_OK);
		pInterface->Release();
	}

	g_bReloadProject = GetRegInt(CDockWorker::s_lpszLayoutSection, szReloadProject, bEmulationIsVC20);

	m_FS.LoadFromReg (CDockWorker::s_lpszLayoutSection);
#endif
}

void CMainFrame::SaveLayout()
{
// REVIEW: Why is this in #ifdef _WIN32? What are we worried about? WIN64? ;)
#ifdef _WIN32
	CString str;

	CString strKeyMain = GetRegistryKeyName();
	WriteRegInt(CDockWorker::s_lpszLayoutSection, szLayoutVersion, LAYOUT_VERSION);

	// Save current manager to a layout buffer.
	SaveManager();
	m_pManager->SaveWorkers();

	// Save the layout buffers.
	for (int i = 0; i < MANAGER_STATES; i++)
	{
		wsprintf(str.GetBuffer(_MAX_PATH + 1), szLayoutKey, i);
		str.ReleaseBuffer();

		if (m_ahManagerStates[i] != NULL)
			WriteRegData(CDockWorker::s_lpszLayoutSection, str, m_ahManagerStates[i]);
	}

	if (m_nManagerState < MANAGER_STATES && m_ahManagerStates[m_nManagerState] != NULL)
	{
		::GlobalFree(m_ahManagerStates[m_nManagerState]);
		m_ahManagerStates[m_nManagerState] = NULL;
	}

	WriteRegInt(CDockWorker::s_lpszLayoutSection, szStatusbar, theApp.m_bStatusbar);

	WriteRegInt(CDockWorker::s_lpszLayoutSection, szClock, IsClockEnabled());
	WriteRegInt(CDockWorker::s_lpszLayoutSection, szReloadProject, g_bReloadProject);
	WriteRegInt (CDockWorker::s_lpszLayoutSection, szDisableFileDep, theApp.m_nDisableFileDep);
	m_FS.SaveToReg (CDockWorker::s_lpszLayoutSection);

	theCmdCache.SaveMenus();
#endif
}

BOOL CMainFrame::LoadManager()
{
	CDockManager* pManagerOld = m_pManager;
	BOOL bSuccess = FALSE;
	int nMaxFails = 2;

#ifdef _EXPORT_LAYOUT
	// If we are building new resource files then we don't want to
	// read from the old ones.
	//
	if (theApp.m_bExportLayout)
		nMaxFails = 1;
#endif

	CInitFile fileInit;

	for (int nFailure = 0; nFailure < nMaxFails && !bSuccess; nFailure++)
	{
		if (nFailure == 0)
		{
			if (m_ahManagerStates[m_nManagerState] == NULL)
				continue;

			fileInit.SetBuffer(m_ahManagerStates[m_nManagerState]);
			m_ahManagerStates[m_nManagerState] = NULL;	// CInitFile is delete responsible.
		}
		else
		{
			if (!fileInit.Open(RT_LAYOUT,
					MAKEINTRESOURCE(m_nManagerState + 1),
					CFile::modeRead, CInitFile::dataResource))
				continue;
		}

		CArchive loadArchive(&fileInit,
			CArchive::load | CArchive::bNoFlushOnDelete);
		TRY
		{
			// Need to load into m_pManager for Create() to work.
			//
			loadArchive >> m_pManager;
			loadArchive.Close();
			fileInit.Close();

		    if (!m_pManager->Create(this, m_pWndMDIClient, pManagerOld))
		    	delete m_pManager;
		    else
		    {
				if (pManagerOld != NULL)
		    		delete pManagerOld;

				MapDockingCmds();
		    
				bSuccess = TRUE;
		    }
		}
		CATCH_ALL(e)
		{
			fileInit.Abort(); // will not throw an exception

			// Don't delete m_pManager, as it may be trash in this case,
			// causing the destructor to crash us.
		}
		END_CATCH_ALL
	}

	if (!bSuccess)
	{
		m_pManager = pManagerOld;
		if (m_pManager != NULL)
			m_pManager->SetAvailableWndDirty();
	}

	return bSuccess;
}

BOOL CMainFrame::SaveManager()
{
	// If we are in DocObject mode
	// we don't want to save or load the DocObject mode state.
	if(m_pManager->IsInDocObjectMode())
		return TRUE;

	CInitFile fileInit;
	fileInit.Open((LPCSTR) NULL, (LPCSTR) NULL, CFile::modeWrite);

	CArchive saveArchive(&fileInit,
		CArchive::store | CArchive::bNoFlushOnDelete);
	TRY
	{
		saveArchive << m_pManager;
		saveArchive.Close();	// flush the archive

		ASSERT(m_ahManagerStates[m_nManagerState] == NULL);	// Otherwise we leak.
		m_ahManagerStates[m_nManagerState] = fileInit.GetInitData();
		fileInit.Close();
	}
	CATCH_ALL(e)
	{
		fileInit.Abort(); // will not throw an exception

		return FALSE;
	}
	END_CATCH_ALL

#ifdef _EXPORT_LAYOUT
	if (theApp.m_bExportLayout)
	{
		CString str;
		wsprintf(str.GetBuffer(_MAX_PATH + 1), theApp.m_pszLayoutFilePath,
			m_nManagerState);
		str.ReleaseBuffer();

		CFile fileExport;
		CFileException fe;

		if (!fileExport.Open(str, CFile::modeCreate | CFile::modeReadWrite |
			CFile::shareExclusive, &fe))
			return FALSE;
		else
		{
			CArchive saveArchive(&fileExport, CArchive::store |
				CArchive::bNoFlushOnDelete);
			TRY
			{
				saveArchive << m_pManager;
				saveArchive.Close();
				fileExport.Close();
			}
			CATCH_ALL(e)
			{
				fileExport.Abort(); // will not throw an exception

				return FALSE;
			}
			END_CATCH_ALL
		}
	}
#endif

	return TRUE;
}

BOOL CMainFrame::SerializeWorkspaceLayout(CStateSaver& stateSave)
{
	BOOL bReturn = TRUE;
	BOOL bSave = stateSave.IsStoring();

	CInitFile fileInit;
	if (!stateSave.OpenStream(fileInit, szWorkspaceDocs))
	{
		if (bSave) 
			SetFileError(CFileException::generic);
		return FALSE;
	}

	UINT nMode = CArchive::bNoFlushOnDelete |
		(bSave ? CArchive::store : CArchive::load);
	CArchive ar(&fileInit, nMode);
	TRY
	{
		if (bSave)
			bReturn = SaveWorkspace(ar);
		else
			bReturn = LoadWorkspace(ar);

		ar.Close();	// flush the archive
		fileInit.Close();
	}
	CATCH_ALL(e)
	{
		if (bSave)
		{
			if (e->IsKindOf(RUNTIME_CLASS(CFileException)))
				SetFileError(((CFileException*) e)->m_cause);
			else
				SetFileError(CFileException::generic);
		}
		
		fileInit.Abort(); // will not throw an exception
		bReturn = FALSE;
	}
	END_CATCH_ALL

	return bReturn;
}

BOOL CMainFrame::LoadWorkspace(CArchive& ar)
{
	ASSERT(ar.IsLoading());

	BOOL bFailed = FALSE, bCancel = FALSE, bLock = FALSE;
	int i, iProj = -1;

	WORD wCount = 0;
	CWnd** aWnd = NULL;
	CDocument** aDoc = NULL;
	UINT* anShow = NULL;

	TRY
	{
		// If it's the wrong version, just blow out.
		WORD wVer;
		ar >> wVer;
		if (wVer != WORKSPACE_VERSION)
			return FALSE;

		WORD wZTemp;
		int iZ;

		WORD wMaximizeDoc;
		ar >> wMaximizeDoc >> wCount;

		if (wCount == 0)
		{
			theApp.m_bMaximizeDoc = (wMaximizeDoc != 0);
			return TRUE;		         
		}

		aWnd = new CWnd*[(int) wCount];
		aDoc = new CDocument*[(int) wCount];
		anShow = new UINT[(int) wCount];

		// NULL out window pointers.  If an exception is thrown reading
		// the workspace, we want to destroy all the windows created.
		memset(aWnd, '\0', wCount * sizeof(CWnd*));

		// Check to make sure the screen size has not shrunk since we wrote
		// the workspace.  If it has, do not place the window; use default
		// cascade.
		BOOL bPlace = TRUE;
		WORD wMetric;
		ar >> wMetric;
		if (wMetric > GetSystemMetrics(SM_CXSCREEN))
			bPlace = FALSE;
		ar >> wMetric;
		if (wMetric > GetSystemMetrics(SM_CYSCREEN))
			bPlace = FALSE;

		bLock = !CPartFrame::IsWorkspaceLocked();	// Don't lock if we already are.
		if (bLock)
			CPartFrame::LockWorkspace(TRUE);

		CString strComment;
		VERIFY( strComment.LoadString( IDS_LOADINGWRKSPC ) );
		StatusBeginPercentDone( strComment );

		theApp.m_bMaximizeDoc = FALSE;

		// Don't load windows if escape is down.  This call to GetAsyncKeyState
		// also primes the pump for future calls which will check the least
		// significant bit (set if the user has pressed Esc since last call).
		//
		if ((GetAsyncKeyState(VK_ESCAPE) & ~1) != 0)
			bCancel = TRUE;
		else
		{
			// Create and place the document windows.
			for (i = 0; i < (int) wCount; i++)
			{
				// Read z-order and store it for a later DeferWindowPos.
				ar >> wZTemp;
				iZ = (int) wZTemp;

				ASSERT(iZ >= 0 && iZ < wCount);

				aWnd[iZ] = LoadWorkspaceWindow(ar, bPlace, &aDoc[iZ], &anShow[iZ]);
										    
				StatusPercentDone(((i + 1) * 100) / (wCount + 1));

				// If user pressed escape, stop loading windows.
				if ((GetAsyncKeyState(VK_ESCAPE) & 1) != 0)
				{
					bCancel = TRUE;
					break;
				}
			}
		}

		if (wMaximizeDoc != 0)
		{
			theApp.m_bMaximizeDoc = TRUE;

			for (i = 0; i < (int) wCount && aWnd[i] == NULL; i++)
				;

			if (i < wCount)
				anShow[i] = SW_SHOWMAXIMIZED;
		}

		StatusPercentDone(100);	// Let the user see this.
		ShowSplashScreen(FALSE);

		if (bLock)
			CPartFrame::LockWorkspace(FALSE);

		m_pWndMDIClient->ShowWindow(SW_HIDE);

		// Z-order, and show the windows.
		HWND hwndCurrent, hwndAfter = HWND_TOP;
		UINT dwFlags = 0;
		HDWP hdwp = ::BeginDeferWindowPos((int) wCount);
		ASSERT(hdwp != NULL);

		for (i = 0; i < (int) wCount; i++)
		{
			if (aWnd[i] == NULL)
				continue;

			aWnd[i]->ShowWindow(anShow[i]);

			// Make sure the window is in the right z-order.
			hwndCurrent = ((CWnd*) aWnd[i])->m_hWnd;
			ASSERT(hwndCurrent != NULL);

			hdwp = ::DeferWindowPos(hdwp, hwndCurrent,
				hwndAfter, 0, 0, 0, 0,
				SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | dwFlags);

			hwndAfter = hwndCurrent;
			dwFlags = SWP_NOACTIVATE;	// Only activate first window.
		}

		::EndDeferWindowPos(hdwp);

		m_pWndMDIClient->ShowWindow(SW_SHOWNA);

		// Now that all the windows are visible, we need to update the
		// document frame counts, and titles.
		for (i = 0; i < (int) wCount; i++)
		{
			if (aWnd[i] == NULL)
				bFailed = TRUE;
			else
			{
				ASSERT(aDoc[i] != NULL);
				aDoc[i]->UpdateFrameCounts();
				ASSERT(aWnd[i]->IsKindOf(RUNTIME_CLASS(CPartFrame)));
				((CPartFrame*) aWnd[i])->OnUpdateFrameTitle(TRUE);
			}
		}

		m_pWndMDIClient->UpdateWindow();
	}
	CATCH_ALL(e)
	{
		bFailed = TRUE;

		for (i = 0; i < (int) wCount; i++)
		{
			ASSERT(aWnd != NULL);

			if (i != iProj && aWnd[i] != NULL)
			{
				aWnd[i]->DestroyWindow();
				delete aWnd[i];
				aWnd[i] = NULL;
			}
		}

		if (bLock)
			CPartFrame::LockWorkspace(FALSE);

		StatusEndPercentDone();

		delete [] aWnd;
		delete [] aDoc;
		delete [] anShow;

		THROW_LAST();
	}
	END_CATCH_ALL

	delete [] aWnd;
	delete [] aDoc;
	delete [] anShow;

	StatusEndPercentDone();

	FlushKeys();

	if (bFailed && !bCancel)
		InformationBox(ERR_Workspace_Window);

	return TRUE;
}

CPartFrame* CMainFrame::LoadWorkspaceWindow(CArchive& ar,
	BOOL bPlace /*=TRUE*/, CDocument** hDoc /*=NULL*/,	
    UINT* pnCmdShow /*=NULL*/)
{
	BOOL bSingle = !CPartFrame::IsWorkspaceLocked();
	if (bSingle)
		CPartFrame::LockWorkspace(TRUE);

	ASSERT(!bSingle || (pnCmdShow != NULL && hDoc != NULL));

	// Read view init data, and store it in CPartFrame static to
	// allow efficient initialization during OpenDocumentFile().
	WORD wSize;
	ar >> wSize;

	if (wSize == 0)
		CPartFrame::SetInitData(NULL);
	else
	{
		LPBYTE lpData = new BYTE[wSize + sizeof(WORD)];
		LPWORD lpwSize = (LPWORD) lpData;

		*lpwSize = wSize;
		ar.Read(lpwSize + 1, (int) wSize);

		CPartFrame::SetInitData(lpData);
	}

	// Read file open moniker.
	CString strMoniker;
	ar >> strMoniker;

	// Some kinds of monikers append to the path some details of how to open themselves.
	// For example, dialogs are saved  as .\project.rc<newline>Dialog IDD_DIALOG1 [English [US]]
	// We need to get the full path, but on Windows 95, the full path code won't allow an embedded
	// newline in the path, despite what the documentation says.
	// martynl 16Dec96

	{ // context to hide pathMoniker from everyone else, because it isn't a full representation

		// first cut off the internal part of the moniker
		int nMonikerIndex=strMoniker.Find(chMonikerSep);
		CString strInternal;

		if(nMonikerIndex!=-1)
		{
			// cut off the internal piece
			strInternal=strMoniker.Right(strMoniker.GetLength()-nMonikerIndex);
			strMoniker=strMoniker.Left(nMonikerIndex);
		}

		// determine the full path to the external piece
		CPath pathMoniker;
		pathMoniker.Create(strMoniker);

		// give strMoniker the full path, and re-add the internal piece, if it exists
		strMoniker = pathMoniker.GetFullPath()+strInternal;
	}

	// Read DocTemplate runtime class, and create the document window.
	CPartFrame* pFrame = NULL;
	CDocTemplate* pTemplate = NULL;
	CDocument* pDoc = NULL;

	//BLOCK:
	{
		USES_CONVERSION;

		CString strClsid;
		ar >> strClsid;

		CLSID clsid;
		if (CLSIDFromString(A2W(strClsid), &clsid) == S_OK)
			pTemplate = theApp.GetTemplate(clsid);
	}

	// Read the saved value of CFrameWnd::m_nWindow.
	WORD wWindow;
	ar >> wWindow;

	// If we don't have that class any more, then don't try to
	// open the file.
	if (pTemplate != NULL)
	{
		// If opening multiple doc's don't try to open the file if it
		// doesn't exist.  We want to minimize the number of error messages
		// the user might get.
		//
		BOOL bTry = FALSE;
		if (bSingle || strMoniker.Find(chMonikerSep) != -1 ||
			FileExists(strMoniker))
		{
			bTry = TRUE;
		}

		if (bTry)
		{
			CDocTemplate* pOpenTemplate;
			pFrame = theApp.GetOpenFrame(strMoniker, pOpenTemplate, pDoc);

			if (pFrame == NULL)
			{
				pDoc = pTemplate->OpenDocumentFile(strMoniker);
				if (pDoc != NULL)	// GetWorkspaceWnd invalid if we failed.
					pFrame = CPartFrame::GetWorkspaceWnd();
			}
			else
			{
				if (pTemplate != pOpenTemplate)
					CPartFrame::SetInitData(NULL);
				else if (wWindow != 0)
				{
					// This was a multiple frame window before, so it is okay
					// to create a new frame now.
					pFrame = (CPartFrame*)
						pTemplate->CreateNewFrame(pDoc, pFrame);
				}

				pTemplate = pOpenTemplate;

				if (pFrame != NULL)
				{
					ASSERT(pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)));
					pTemplate->InitialUpdateFrame(pFrame, pDoc);
				}
			}
		}
	}

	if (hDoc != NULL)
		*hDoc = pDoc;

	if (bSingle)
		CPartFrame::LockWorkspace(FALSE);


	// Read and set window placement.  Save  the showCmd for later, since
	// we want the windows to stay invisible during creation.
	WINDOWPLACEMENT place;
	ar.Read(&place, sizeof(WINDOWPLACEMENT));
	place.length = sizeof(WINDOWPLACEMENT);	// Fix for NT bug.

	if (!bPlace || pFrame == NULL)
	{
		if (!bSingle)
			*pnCmdShow = SW_SHOW;
	}
	else
	{
		if (!bSingle)
		{
			*pnCmdShow = place.showCmd;
			place.showCmd = SW_HIDE;
		}

		pFrame->SetWindowPlacement(&place);
	}

	if (pFrame != NULL)
	{
		// This is usually done in CPartTemplate::InitialUpdateFrame,
		// but with workspaces we want the window to be sized before
		// it is required to initialize itself.

		pFrame->SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE);

		// These handlers should use the data from CPartFrame::GetCurData().
		pFrame->OnLoadWorkspace();
	}

	// Delete the initialization data.
	CPartFrame::SetInitData(NULL);

	return pFrame;
}

BOOL CMainFrame::SaveWorkspace(CArchive& ar)
{
	ASSERT(ar.IsStoring());

	CPartFrame* pFrame;
	WORD wCount = 0, wMaximizeDoc = (WORD)theApp.m_bMaximizeDoc;

	WORD wZOrderProj = WORD(-1);

	// Count the number of windows we will be saving.
	for (CWnd* pWndNext = MDIGetActive(); pWndNext != NULL;
		pWndNext = pWndNext->GetWindow(GW_HWNDNEXT))
	{
		if(!IsValidMDICycleMember(pWndNext))
			continue;

		pFrame = (CPartFrame*) pWndNext;
		ASSERT(pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)));
		if (!pFrame->IsInWorkspace())
			continue;

		wCount++;
	}

	ar << (WORD) WORKSPACE_VERSION << wMaximizeDoc << wCount;

    if (wCount == 0)
        return TRUE;

	ar << (WORD) GetSystemMetrics(SM_CXSCREEN)
		<< (WORD) GetSystemMetrics(SM_CYSCREEN);

	BOOL bSavedDocWnds = FALSE, bSavedDependents = FALSE;

	while (!bSavedDocWnds || !bSavedDependents)
	{
		pWndNext = MDIGetActive();
		WORD wZOrder = wCount;

		// Write out the per window data in reverse Z-order, so that we can
		// load them in the right order.
		for (pWndNext = pWndNext->GetWindow(GW_HWNDLAST); pWndNext != NULL;
			pWndNext = pWndNext->GetWindow(GW_HWNDPREV))
		{
			if(!IsValidMDICycleMember(pWndNext))
				continue;

			pFrame = (CPartFrame*) pWndNext;
			ASSERT(pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)));
			if (!pFrame->IsInWorkspace())
				continue;

			wZOrder--;

			// Save non-dependents in first loop, and dependents
			// the second.
			if (bSavedDocWnds ? !pFrame->IsDependent() : pFrame->IsDependent())
				continue;

			// Write ZOrder.
			ar << wZOrder;
			SaveWorkspaceWindow(ar, pFrame, TRUE);
		}

		ASSERT(wZOrder == 0);	// Missing some windows?

		bSavedDependents = bSavedDocWnds;
		bSavedDocWnds = TRUE;
	}

	return TRUE;
}

void CMainFrame::SaveWorkspaceWindow(CArchive& ar, CPartFrame* pFrame,
	BOOL bRelative /*=FALSE*/)
{
	ASSERT_VALID(pFrame);

	// Write frame initialization data.
	LPBYTE lpData = pFrame->GetInitData();
	if (lpData == NULL)
		ar << (WORD) 0;
	else
	{
		LPWORD lpwSize = (LPWORD) lpData;
		int nSize = *lpwSize + sizeof(WORD);	// First word is data size.
		ASSERT(nSize > 0);	// Hey! no data.

		ar.Write(lpData, nSize);

		delete [] lpData;
	}

	// Write the file open moniker.
	CString strMoniker = pFrame->GetWorkspaceMoniker(bRelative);
	ASSERT(!strMoniker.IsEmpty());
	ar << strMoniker;

	// Write template CLSID.
	CDocument* pDoc = pFrame->GetActiveDocument();
	ASSERT_VALID(pDoc);
	CPartTemplate* pTemplate = (CPartTemplate*) pDoc->GetDocTemplate();
	ASSERT(pTemplate != NULL && pTemplate->IsKindOf(RUNTIME_CLASS(CPartTemplate)));

	CString strClsid;

	LPWSTR lpwsz;
	if (StringFromCLSID(pTemplate->GetTemplateClsid(), &lpwsz) == NOERROR)
	{
		USES_CONVERSION;

		strClsid = W2A(lpwsz);
		AfxFreeTaskMem(lpwsz);
	}

	ar << strClsid;

	// Write window number.
	ar << (WORD) pFrame->m_nWindow;

	// Write window placement.
	WINDOWPLACEMENT place;
	place.length = sizeof(WINDOWPLACEMENT);
	pFrame->GetWindowPlacement(&place);
	ar.Write(&place, sizeof(WINDOWPLACEMENT));
}

/*
Generating new layouts for MSDEVD.EXE:
======================================
Follow directions in dev\ide\exes\dev\layouts.doc.  Make changes if necessary.
*/
