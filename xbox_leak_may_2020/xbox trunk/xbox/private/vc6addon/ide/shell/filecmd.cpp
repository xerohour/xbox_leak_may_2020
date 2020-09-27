/////////////////////////////////////////////////////////////////////////////
//
// Standard File Commands
//
// This file contains implementations for:
//
//	* File New
//	* File Open
//	* File Save All
//	* Recently opened files
//	* Recently opened projects
//
// Various related utilities are also included here.
//

#include "stdafx.h"
#include "docobtpl.h"
#include <dlgs.h>
#include "shell.h"
#include "resource.h"
#include "filenew.h"
#include "barglob.h"

#include <sys/types.h>
#include <sys/stat.h>

#include "toolexpt.h"
#include "shellrec.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////

static BOOL fForceAddToProject = FALSE;
static BOOL fForceShowProjectTab = FALSE;

void CTheApp::OnFileNew()
{
	BOOL fDialogDone = FALSE;
	BOOL fBackButton = FALSE;

	while (!fDialogDone)
	{
		CFileNewTabbedDialog dlg(fBackButton, fForceAddToProject, fForceShowProjectTab);
		if (dlg.DoModal() == IDOK)
		{
			if (dlg.GetType() == CPartTemplate::newfile || dlg.GetType() == CPartTemplate::newother)
			{
				fDialogDone = TRUE;
				CPartTemplate *pTemplate = (CPartTemplate *)dlg.GetTemplate();
				ASSERT(pTemplate != NULL);
				ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CPartTemplate)));
				CString strFileName   = dlg.GetFileName();
				CString strPathName   = dlg.GetPathName();
				IPkgProject *pProject = dlg.GetProject(); // Un-AddRefed

				if (!strFileName.IsEmpty())
				{
					if (strPathName.IsEmpty())
						strPathName = ".";

					CDir dir;
					dir.CreateFromStringEx(strPathName, TRUE);
					CPath path;
					path.CreateFromDirAndFilename(dir, (LPCTSTR)strFileName);

					// if extension not provided for file, use default from doctemplate
					if (_tcslen(path.GetExtension()) == 0)
					{
						CString strExtension;
						pTemplate->GetDocString(strExtension, CDocTemplate::filterExt);
						path.ChangeExtension(strExtension);
					}

					if (dlg.GetType() == CPartTemplate::newfile)
					{
						CDocument *pOpenDoc;
						CDocTemplate *pOpenTemplate = theApp.GetOpenTemplate((LPCTSTR)path, pOpenDoc);
						if ((pOpenTemplate != NULL) && (pOpenDoc != NULL))
						{
							ASSERT(pOpenDoc->IsKindOf(RUNTIME_CLASS(CPartDoc)));
							((CPartDoc*)pOpenDoc)->OnCloseDocument();
						}

						pTemplate->NewDocumentFile((LPCTSTR)path, pProject, NULL);
					}
					else	// DocObject file
					{
						CDocument *pOpenDoc;
						CDocTemplate *pOpenTemplate = theApp.GetOpenTemplate((LPCTSTR)path, pOpenDoc);
						if ((pOpenTemplate != NULL) && (pOpenDoc != NULL))
						{
							ASSERT(pOpenDoc->IsKindOf(RUNTIME_CLASS(CPartDoc)));
							if (((CPartDoc *)pOpenDoc)->AttemptCloseDoc())
							{
								((CPartDoc*)pOpenDoc)->OnCloseDocument();
								pTemplate->NewDocumentFile((LPCTSTR)path, pProject, NULL);
							}
						}
						else
							pTemplate->NewDocumentFile((LPCTSTR)path, pProject, NULL);
					}
				}
				else
				{
					pTemplate->NewDocumentFile((LPCTSTR)"", NULL, NULL);
				}
			}
			else if (dlg.GetType() == CPartTemplate::newtemplate)
			{
				fDialogDone = TRUE;
			}
			else if (dlg.GetType() == CPartTemplate::newproject)
			{
				USES_CONVERSION;

				fDialogDone = TRUE;
				CString strPathName = dlg.GetPathName();
				CString strFileName = dlg.GetFileName();
				CDir dir;
				dir.CreateFromStringEx(strPathName, TRUE);
				CPath path;
				CString strPath = dir;
				// If strPath's length is 3, then we have a C:\ situation. Shouldn't add \ to it
				if(strPath.GetLength() > 3)
					strPath += "\\";
				strPath += strFileName;
				BOOL fCanCreate;

				IProjectWorkspace *pInterface;
				pInterface = g_IdeInterface.GetProjectWorkspace();
				ASSERT(pInterface != NULL);

				if (SUCCEEDED(dlg.GetProjectProvider()->CanCreateProject(T2W(strPath), dlg.GetProjectType(), &fCanCreate)) && fCanCreate)
				{
					COleRef<IPkgProject> pProject;
					DWORD dwRetVal;
					if (dlg.GetCreateNew())
					{
						CDocument *pDoc;
						// workspace creation will create the directory if necessary
						if ((SUCCEEDED(pInterface->CreateEmptyWorkspace(&pDoc, strPath))) && (pDoc != NULL))
						{
							// REVIEW [patbr]: extension can be project specific!
							strFileName += _T(BUILDER_EXT);
							path.CreateFromDirAndFilename(dir, (LPCTSTR)strFileName);
							
							VERIFY(SUCCEEDED(dlg.GetProjectProvider()->CreateProject(T2W(path), dlg.GetProjectType(), &pProject, &dwRetVal)));
							if (dwRetVal == CREATE_PROJ_OK)
							{
								VERIFY(SUCCEEDED(pInterface->AddProject(pProject, TRUE)));
								theApp.NotifyPackages(PN_NEW_PROJECT, (void *)pProject);
								theApp.NotifyPackages(PN_WORKSPACE_INIT, (void *)(const TCHAR *)path);
								pInterface->SetActiveProject(pProject, TRUE);
								// force Project Workspace window visible
								DkWShowWindow(MAKEDOCKID(PACKAGE_PRJSYS, IDDW_PROJECT), TRUE);
							}
							else if (dwRetVal == CREATE_PROJ_RERUN_PROMPT)
							{
								VERIFY(SUCCEEDED(pInterface->CloseWorkspace()));
								// remove newly created MDP and OPT files from disk.
								path.ChangeExtension(WORKSPACE_EXT);
								if (_access(path, 00) == 0)
									VERIFY(path.DeleteFromDisk());
								path.ChangeExtension(WSOPTIONS_EXT);
								if (_access(path, 00) == 0)
									VERIFY(path.DeleteFromDisk());
								// attempt to delete directory from disk--it may be empty.
								dir.RemoveFromDisk();

								// remove the filename from the workspace MRU list.
								int iCount = m_pRecentProjectList->GetSize();
								for (int i = 0; i < iCount; i++)
								{
									CString strMRU;
									strMRU = (*(m_pRecentProjectList))[i];

									char szPath[_MAX_PATH], szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];
									_splitpath(path.GetFullPath(), szDrive, szDir, szFName, szExt);
									_makepath(szPath, szDrive, szDir, szFName, NULL);

									if (!(strMRU.CompareNoCase(szPath)))
									{
										m_pRecentProjectList->Remove(i);
										break;
									}
								}

								fDialogDone = FALSE;
								fBackButton = TRUE;
							}
							else if (dwRetVal == CREATE_PROJ_CANCEL)
							{
								// remove newly created MDP and OPT files from disk.
								VERIFY(SUCCEEDED(pInterface->CloseWorkspace()));
								path.ChangeExtension(WORKSPACE_EXT);
								if (_access(path, 00) == 0)
									VERIFY(path.DeleteFromDisk());
								path.ChangeExtension(WSOPTIONS_EXT);
								if (_access(path, 00) == 0)
									VERIFY(path.DeleteFromDisk());
								// attempt to delete directory from disk--it may be empty.
								dir.RemoveFromDisk();

								// remove the filename from the workspace MRU list.
								int iCount = m_pRecentProjectList->GetSize();
								for (int i = 0; i < iCount; i++)
								{
									CString strMRU;
									strMRU = (*(m_pRecentProjectList))[i];

									char szPath[_MAX_PATH], szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];
									_splitpath(path.GetFullPath(), szDrive, szDir, szFName, szExt);
									_makepath(szPath, szDrive, szDir, szFName, NULL);

									if (!(strMRU.CompareNoCase(szPath)))
									{
										m_pRecentProjectList->Remove(i);
										break;
									}
								}
							}
						}
						else
						{
							// could not create empty workspace, probably because a package
							// returned FALSE from the PN_QUERY_NEW_WORKSPACE notification.
							MsgBox(Error, IDS_FAIL_NEW_WORKSPACE);
						}
					}
					else
					{
						// create the directory for the project if necessary
						if (!dir.ExistsOnDisk())
							VERIFY(dir.CreateOnDisk());

						BOOL fWorkspaceEmpty = TRUE;
						// if there are no projects in the workspace then freeze the
						// project workspace window while project is being added...
						COleRef<IPkgProject> pActiveProject;
						pInterface->GetActiveProject(&pActiveProject);
						if (pActiveProject != NULL)
							fWorkspaceEmpty = FALSE;

						// REVIEW [patbr]: extension can be project specific!
						strFileName += _T(BUILDER_EXT);
						path.CreateFromDirAndFilename(dir, (LPCTSTR)strFileName);


						VERIFY(SUCCEEDED(dlg.GetProjectProvider()->CreateProject(T2W(path), dlg.GetProjectType(), &pProject, &dwRetVal)));
						if (dwRetVal == CREATE_PROJ_OK)
						{
							// REVIEW [patbr]: freezing workspace window causes it to not paint during wizards
							COleRef<IProjectWorkspaceWindow> pPrjWksWin;
							pPrjWksWin = NULL;
							if (fWorkspaceEmpty && SUCCEEDED(FindInterface(IID_IProjectWorkspaceWindow, (LPVOID FAR *)&pPrjWksWin)) && (pPrjWksWin != NULL))
								pPrjWksWin->FreezeContainingPane(NULL);

							VERIFY(SUCCEEDED(pInterface->AddProject(pProject, TRUE)));
							IPkgProject *pDependentProject = dlg.GetProject();
							if (pDependentProject != NULL)
							{
								HRESULT hr = pProject->SetDependentProject(pDependentProject);
								ASSERT(SUCCEEDED(hr) || hr == E_NOTIMPL);
								if (hr == E_NOTIMPL)
									MsgBox(Error, IDS_PROJECT_NOT_DEPENDENT);
							}
							theApp.NotifyPackages(PN_NEW_PROJECT, (void *)pProject);
							pInterface->SetActiveProject(pProject, TRUE);

							if (fWorkspaceEmpty && (pPrjWksWin != NULL))
								pPrjWksWin->ThawContainingPane(NULL);

							// force Project Workspace window visible
							DkWShowWindow(MAKEDOCKID(PACKAGE_PRJSYS, IDDW_PROJECT), TRUE);
						}
						else if (dwRetVal == CREATE_PROJ_RERUN_PROMPT)
						{
							// remove newly created MDP and OPT files from disk.
							path.ChangeExtension(WORKSPACE_EXT);
							if (_access(path, 00) == 0)
								VERIFY(path.DeleteFromDisk());
							path.ChangeExtension(WSOPTIONS_EXT);
							if (_access(path, 00) == 0)
								VERIFY(path.DeleteFromDisk());
							// attempt to delete directory from disk--it may be empty.
							dir.RemoveFromDisk();

							// remove the filename from the workspace MRU list.
							int iCount = m_pRecentProjectList->GetSize();
							for (int i = 0; i < iCount; i++)
							{
								CString strMRU;
								strMRU = (*(m_pRecentProjectList))[i];

								char szPath[_MAX_PATH], szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];
								_splitpath(path.GetFullPath(), szDrive, szDir, szFName, szExt);
								_makepath(szPath, szDrive, szDir, szFName, NULL);

								if (!(strMRU.CompareNoCase(szPath)))
								{
									m_pRecentProjectList->Remove(i);
									break;
								}
							}

							fDialogDone = FALSE;
							fBackButton = TRUE;
						}
					}
				}
				else
				{
					fDialogDone = FALSE;
					fBackButton = TRUE;
				}
				pInterface->ReleaseProjectTypeInfo();
			}
			else if (dlg.GetType() == CPartTemplate::newworkspace)
			{
				fDialogDone = TRUE;
				CPartTemplate *pTemplate = (CPartTemplate *)dlg.GetTemplate();
				ASSERT(pTemplate != NULL);
				ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CPartTemplate)));
				CString strFileName   = dlg.GetFileName();
				CString strPathName   = dlg.GetPathName();

				if (strPathName.IsEmpty())
					strPathName = ".";

				CDir dir;
				dir.CreateFromStringEx(strPathName, TRUE);
				CPath path;
				path.CreateFromDirAndFilename(dir, (LPCTSTR)strFileName);

				COleRef<IProjectWorkspace> pInterface;
				VERIFY(SUCCEEDED(theApp.FindInterface(IID_IProjectWorkspace, (LPVOID FAR *)&pInterface)));
				ASSERT(pInterface != NULL);

				CDocument *pDoc;
				// workspace creation will create the directory if necessary
				VERIFY(SUCCEEDED(pInterface->CreateEmptyWorkspace(&pDoc, path)));
				if (pDoc != NULL)
				{
					theApp.NotifyPackages(PN_WORKSPACE_INIT, (void *)(const TCHAR *)path);
				}
				else
				{
					// could not create empty workspace, probably because a package
					// returned FALSE from the PN_QUERY_NEW_WORKSPACE notification.
					MsgBox(Error, IDS_FAIL_NEW_WORKSPACE);
				}
			}
		}
		else
			fDialogDone = TRUE;
	}
}

void CTheApp::OnFileNewInProject()
{
	fForceAddToProject = TRUE;
	OnFileNew();
	fForceAddToProject = FALSE;
}

void CTheApp::OnFileNewProject(BOOL fAddToWks)
{
	fForceAddToProject = fAddToWks;
	fForceShowProjectTab = TRUE;
	OnFileNew();
	fForceShowProjectTab = FALSE;
	fForceAddToProject = FALSE;
}

void CTheApp::OnInsertFiles()
{
	CAddFilesDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		USES_CONVERSION;

		int iMaxFile = dlg.m_saFileNames.GetSize();
		if (iMaxFile > 0)
		{
			LPOLESTR *ppszFiles = new LPOLESTR[iMaxFile];
			for (int iFile = 0; iFile < iMaxFile; iFile++)
			{
				LPOLESTR wstr = T2W(dlg.m_saFileNames[iFile]);
				ppszFiles[iFile] = (LPOLESTR)malloc((wcslen(wstr)+1) * sizeof(wchar_t));
				wcscpy(ppszFiles[iFile], wstr);
			}
			LPCOLESTR pszDefaultFolder = NULL;
			VERIFY(SUCCEEDED(dlg.GetProject()->AddFiles((LPCOLESTR *)ppszFiles, iMaxFile, NULL)));
			for (iFile = 0; iFile < iMaxFile; iFile++)
				free(ppszFiles[iFile]);
			delete[] ppszFiles;
		}
	}
}

CDocTemplate* CTheApp::GetTemplateFromEditor(UINT idsEditor, const char* szFilename)
{
	CDocTemplate* pTemplate = NULL;
	POSITION pos = m_packages.GetHeadPosition();
	while( pos != NULL )
	{
		CPackage* pPackage = (CPackage*)m_packages.GetNext(pos);
		if( (pTemplate = pPackage->GetTemplateFromEditor(idsEditor, szFilename))
			!= NULL )
		{
			break;
		}
	}
	return pTemplate;
}

extern CPartDoc* ActivateExistingDoc(LPCTSTR lpszFileName,
	CDocTemplate* pTemplate)
{
	CDocument* pOpenDoc;
	CDocTemplate* pOpenTemplate =
		theApp.GetOpenTemplate(lpszFileName, pOpenDoc);

	if (pOpenTemplate == NULL)
		return NULL;

	ASSERT(pOpenDoc->IsKindOf(RUNTIME_CLASS(CPartDoc)));

	((CPartDoc*) pOpenDoc)->ActivateDoc();

	if (pTemplate != NULL && pTemplate != pOpenTemplate)
	{
		if (((CPartDoc*) pOpenDoc)->AttemptCloseDoc())
			return NULL;
	}

	return (CPartDoc*) pOpenDoc;
}

void CTheApp::OnFileOpen()
{
	// prompt the user (with all document templates)
	CStringArray saFileNames;
//	CFileOpenReturn forUserInput;
	UINT nOpenAsEditor;
	BOOL bReadOnly = FALSE;
	if (!DoPromptFileOpen(saFileNames, AFX_IDS_OPENFILE,
		OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, NULL, &nOpenAsEditor, &bReadOnly))
		return; // open cancelled

	CString strFileName;
	for( int iFile = 0; iFile < saFileNames.GetSize(); iFile++)
	{
		strFileName = saFileNames[iFile];

		CString strFileTemp = strFileName;
		// don't do MakeUpper on the original file name--this mangles MBCS characters
		strFileTemp.MakeUpper();
		int nIndex;
		// check for indications that this is a UNC or URL path name and adjust if necessary
		if ((nIndex = strFileTemp.Find("\\\\\\")) != -1)
		{
			strFileName = strFileName.Right(strFileName.GetLength() - nIndex - 1);
		}
		else if ((nIndex = strFileTemp.Find("FILE:")) != -1)
		{
			strFileName = strFileName.Right(strFileName.GetLength() - nIndex);
		}
		else if ((nIndex = strFileTemp.Find("HTTP:")) != -1)
		{
			strFileName = strFileName.Right(strFileName.GetLength() - nIndex);
		}

		// Change the case of the file to match the actual file
		GetActualFileCase( strFileName );

		CPartDoc* pDocument = NULL;

		// Determine which doc-template to use.
		CDocTemplate* pTemplate = GetTemplateFromEditor(nOpenAsEditor, strFileName);

		if (!OpenFile(pTemplate, strFileName, bReadOnly))
		{
			// If we can't open the filename, then stop.
			break ;
		};
/*
		if (pTemplate == NULL)		// auto
			pDocument = (CPartDoc*)OpenDocumentFile(strFileName);
		else if (ActivateExistingDoc(strFileName, pTemplate) != NULL)
			continue;
		else						// open using a specific editor
			pDocument = (CPartDoc*)pTemplate->OpenDocumentFile(strFileName);

		if (bReadOnly && pDocument != NULL)
			pDocument->SetReadOnly();

		if (pDocument == NULL)
			break;	// Assume something is terribly wrong and *bail out*.
*/
	}
}

//
// Called by Documents.Open and CTheApp.OnFileOpen to open a file.
//
CDocument* CTheApp::OpenFile(	CDocTemplate* pTemplate, // Pointer to the template which opens the file. Can be Null.
								CString strFileName,	 // Filename to open.
								BOOL bReadOnly)		 // True if the file should be marked readonly
{
	CPartDoc* pDocument = NULL ;

	if (pTemplate == NULL)
	{
		// Open using the Auto type.
		pDocument = (CPartDoc*)OpenDocumentFile(strFileName);
	}
	else 
	{
		CPartDoc* pExistingDoc = ActivateExistingDoc(strFileName, pTemplate) ;
		if ( pExistingDoc == NULL)
		{
			// Open using a specific editor.
			pDocument = (CPartDoc*)pTemplate->OpenDocumentFile(strFileName);
		}
		else 
		{
			// Notice this function does not return null if the file already exitst.
			return pExistingDoc ;
		}
	}

	if (bReadOnly && pDocument != NULL)
	{
		pDocument->SetReadOnly();
	}

	return pDocument ;
}

CDocument* CTheApp::OpenDocumentFile(LPCSTR lpszFileName)
{
	CDocument* pDocument = ActivateExistingDoc(lpszFileName, NULL);
	if (pDocument == NULL)
		pDocument = CWinApp::OpenDocumentFile(lpszFileName);

	// FUTURE: Bug 1031 needs a message box here when file doesn't exist...

	// FUTURE: Maybe remove binary editor from MatchDocType loop and
	// lookup file associations before trying it here...

	return pDocument;
}

/////////////////////////////////////////////////////////////////////////////
//
// Class for handling IDIS compatible file dialogs.
//
class CIdisFileDialog : public C3dFileDialog
{
public:
	CIdisFileDialog(BOOL bOpenFileDialog,
		CDocument *pCurrentDoc = NULL,
		LPCSTR lpszDefExt = NULL,
		LPCSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_SHOWHELP,
		LPCSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL,
		UINT nHelpID = 0);

	virtual BOOL OnInitDialog();
 	virtual BOOL OnFileNameOK();
	afx_msg void OnSelChangeFilter();
	afx_msg void OnTypeChange(UINT idCtl, NMHDR* pNMHDR, LRESULT* pResult);

	CPtrArray* m_ppaEditors;
	UINT m_nOpenAsEditor;
	CString m_strCurrentExt;
	CDocument *m_pCurrentDoc;

protected:
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CIdisFileDialog, C3dFileDialog)
	ON_CBN_SELCHANGE(cmb1, OnSelChangeFilter)
	ON_NOTIFY_RANGE(CDN_TYPECHANGE, 0, 0xffff, OnTypeChange)
END_MESSAGE_MAP()


CIdisFileDialog::CIdisFileDialog(BOOL bOpenFileDialog, CDocument *pCurrentDoc, LPCSTR lpszDefExt,
	LPCSTR lpszFileName, DWORD dwFlags, LPCSTR lpszFilter, CWnd* pParentWnd,
	UINT nHelpID)
: C3dFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName,
	dwFlags, lpszFilter, pParentWnd,
	(nHelpID != 0 ? nHelpID : (bOpenFileDialog ? IDD_CHICAGO_FILEOPEN : IDD_FILE_SAVE_AS)))
{
	m_ofn.hInstance = GetResourceHandle();
	m_pCurrentDoc = pCurrentDoc;

	if( bOpenFileDialog )
	{
		m_iddWin95 = IDD_CHICAGO_FILEOPEN;
		m_iddWinNT = IDD_CHICAGO_FILEOPEN;
		m_ofn.Flags &= ~OFN_HIDEREADONLY;
	} 
	else
	{
		ASSERT(m_pCurrentDoc != NULL);
		m_iddWin95 = NULL;
		m_iddWinNT = NULL;
		m_ofn.Flags |= OFN_HIDEREADONLY;
	}
}

BOOL CIdisFileDialog::OnInitDialog()
{
	if (m_bOpenFileDialog)
	{
		CComboBox* pOpenAs = (CComboBox*)GetDlgItem(IDC_EDITOR);
		ASSERT( pOpenAs != NULL );

		if( !IsExplorer() )
		{
			// non-Chicago dialog has read-only checkbox hidden
			CWnd* pReadOnly = GetDlgItem(chx1);
			pReadOnly->ShowWindow(SW_SHOWNOACTIVATE);
			pReadOnly->EnableWindow(TRUE);

			// same goes for OpenAs Edit Combo
			pOpenAs->ShowWindow(SW_SHOWNOACTIVATE);
			pOpenAs->EnableWindow(TRUE);
			CWnd * pOpenAsCap = GetDlgItem(IDC_EDITOR_CAP);
			pOpenAsCap->ShowWindow(SW_SHOWNOACTIVATE);
			pOpenAsCap->EnableWindow(TRUE);
		}
		else
		{
			// resize the commdlg controls slightly for long filter strings, and
			// position the Open As combo directly below the Filters combo
			CRect rcFileName, rcOpenAs, rcFilters, rcOpenAsTxt, rcFilterTxt;
			pOpenAs->GetWindowRect(&rcOpenAs);

			CWnd *pFileName = GetParent()->GetDlgItem(edt1);
			ASSERT(pFileName != NULL);			// File name edit
			pFileName->GetWindowRect(&rcFileName);
			rcFileName.right += 5;
			pFileName->SetWindowPos(NULL, rcFileName.left, rcFileName.top, rcFileName.right - rcFileName.left,
				rcFileName.bottom - rcFileName.top, SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER | SWP_SHOWWINDOW);

			CWnd* pFilters = GetParent()->GetDlgItem(cmb1);		// List files of type combo
			ASSERT( pFilters != NULL );
			pFilters->GetWindowRect(&rcFilters);
			rcFilters.right += 5;
			pFilters->SetWindowPos(NULL, rcFilters.left, rcFilters.top, rcFilters.right - rcFilters.left,
				rcFilters.bottom - rcFilters.top, SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER | SWP_SHOWWINDOW);

			CWnd* pFilterTxt = GetParent()->GetDlgItem(stc2);	// List files of type caption
			ASSERT( pFilterTxt != NULL );
			pFilterTxt->GetWindowRect(&rcFilterTxt);

			CWnd* pOpenAsTxt = GetDlgItem(IDC_EDITOR_CAP);	// Open As caption
			ASSERT( pOpenAsTxt != NULL );
			pOpenAsTxt->GetWindowRect(&rcOpenAsTxt);

			rcOpenAsTxt.left = rcFilterTxt.left;
			rcOpenAsTxt.right = rcFilterTxt.right;
			ScreenToClient(&rcOpenAsTxt);
			pOpenAsTxt->MoveWindow(&rcOpenAsTxt);

			rcOpenAs.left = rcFilters.left;
			rcOpenAs.right = rcFilters.right;
			ScreenToClient(&rcOpenAs);
			pOpenAs->MoveWindow(&rcOpenAs);
		}

		for( int nEd = 0; nEd < m_ppaEditors->GetSize(); nEd++ )
		{
			SRankedStringID* prsi = (SRankedStringID*)m_ppaEditors->GetAt(nEd);
			CString strEd;
			strEd.LoadString(prsi->ids);
			int nIndex = pOpenAs->AddString(strEd);
			pOpenAs->SetItemData(nIndex, prsi->ids);
		}

		pOpenAs->SetCurSel(0);
	}
	else
	{
		if( !IsExplorer() )
		{
			GetDlgItem(IDC_EDITOR_CAP)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_EDITOR)->ShowWindow(SW_HIDE);
			GetDlgItem(chx1)->ShowWindow(SW_HIDE);  // Read-only checkbox.
			
			CString strSaveType;
			strSaveType.LoadString(IDS_SAVE_FILE_AS_TYPE);
			GetDlgItem(stc2)->SetWindowText(strSaveType);
		}
	}

	return C3dFileDialog::OnInitDialog();
}

static IsInFilterList(CDocTemplate* pTemplate, const CPtrArray& paEditors)
{
	int nEditors = paEditors.GetSize();
	for (int i = 0; i < nEditors; i++)
	{
		SRankedStringID* prsi = (SRankedStringID *)	paEditors[i];
		if (pTemplate == theApp.GetTemplateFromEditor(prsi->ids, NULL))
			return TRUE;
	}

	return FALSE;
}

BOOL CIdisFileDialog::OnFileNameOK()
{
	// tack on default extension, if no extension present
	if (!m_bOpenFileDialog && !m_strCurrentExt.IsEmpty())
	{
		// make sure save filename is of legal length
		if (_tcslen(m_ofn.lpstrFile) + m_strCurrentExt.GetLength() >= _MAX_PATH-1)
		{
			CString strFileName = m_ofn.lpstrFile;
			strFileName += m_strCurrentExt;
			ErrorBox(ERR_File_Invalid_Length, strFileName);
			return TRUE; // Don't let the dialog go away!
		}

		char acExt[_MAX_EXT];
		_splitpath(m_ofn.lpstrFile, NULL, NULL, NULL, acExt);
		if ((*acExt == '\0') || ((*acExt == '.') && (_tcslen(acExt) == 1)))
		{
			LPSTR pchEnd = m_ofn.lpstrFile + strlen(m_ofn.lpstrFile);
			strcpy(pchEnd, (LPCSTR)m_strCurrentExt);
		}
	}

	BOOL bError = C3dFileDialog::OnFileNameOK();

	// we don't get OnOK anymore, so do this stuff here.
	if( m_bOpenFileDialog )
	{
		CComboBox* pOpenEditor = (CComboBox*)GetDlgItem(IDC_EDITOR);
		ASSERT( pOpenEditor != NULL );
		int nIndex = pOpenEditor->GetCurSel();
		m_nOpenAsEditor = pOpenEditor->GetItemData(nIndex);

		// Check to see if it is a workspace document type, and only
		// allow single selection if it is.
		int nFiles = (m_psaFileNames == NULL) ?
			1 : m_psaFileNames->GetSize();

		if (nFiles > 1)
		{
			// Do not allow the user to open more than one workspace file.

			CPtrArray paWorkspaceEditors;
			CDocTemplate* pTemplate;
			BOOL bOnlyOne = FALSE;
			BOOL bProject = FALSE;
			CString strFileName;

			CString strFilter;
			theApp.BuildFilterList(strFilter, &paWorkspaceEditors,
				CPackage::GODI_WORKSPACEOPEN);

			pTemplate = theApp.GetTemplateFromEditor(m_nOpenAsEditor, NULL);
			bOnlyOne = (nIndex != 0) &&
				(pTemplate == theApp.GetTemplate(CLSID_WorkspaceDocument) ||
				IsInFilterList(pTemplate, paWorkspaceEditors));

			for( int iFile = 0; iFile < nFiles; iFile++ )
			{
				strFileName = m_psaFileNames->GetAt(iFile);

				bError = bOnlyOne && iFile > 0;

				if (nIndex == 0)
				{
					// Will 'auto' open this as a project?
					CDocTemplate* pBestTemplate = NULL;
					CDocTemplate::Confidence bestMatch = CDocTemplate::noAttempt;
					CDocument* pOpenDocument;

					POSITION pos = theApp.GetFirstDocTemplatePosition();
					while (pos)
					{
						pTemplate = theApp.GetNextDocTemplate(pos);
						ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CDocTemplate)));

						CDocTemplate::Confidence match;
						match = pTemplate->MatchDocType(strFileName, pOpenDocument);
						if (match > bestMatch)
						{
							bestMatch = match;
							pBestTemplate = pTemplate;
						}
						if (match == CDocTemplate::yesAlreadyOpen)
							break;      // stop here
					}
					
					if (bestMatch != CDocTemplate::noAttempt &&
						(pBestTemplate == theApp.GetTemplate(CLSID_WorkspaceDocument) ||
						IsInFilterList(pBestTemplate, paWorkspaceEditors)))
					{
						bError = bProject;
						bProject = TRUE;
					}
				}

				if (bError)
				{
					ErrorBox(ERR_MultipleMakes, strFileName);
	   				return TRUE;    // don't allow dialog to close
				}
			}
		}
	}

	if (m_bOpenFileDialog || bError)
		return bError;

	// Here we do the work that OFN_NOREADONLYRETURN | OFN_OVERWRITEPROMPT
	// would normally be used for as well as make sure the user doesn't
	// try to overwrite an open file.  We can't use those flags because
	// in the case where the user is trying to save over some other open
	// file, they would get two warnings...

	CDocument* pOpenDoc = NULL;
	CDocTemplate* pOpenTemplate = theApp.GetOpenTemplate(m_ofn.lpstrFile, pOpenDoc);

	// Note: No longer assumes we are performing save as on the active document. [olympus 14704]
	if (pOpenDoc != NULL &&
		pOpenDoc != m_pCurrentDoc)
	{
		// "File already open"
		ErrorBox(ERR_File_Already_Open, m_ofn.lpstrFile);
		return TRUE; // Don't let the dialog go away!
	}

	// On NT, CommDlg will have already checked to see if the file is writable
	// by creating a zero-sized archive file, that it doesn't clean up.
    CFileStatus fs;
	BOOL bExists = CFile::GetStatus(m_ofn.lpstrFile, fs) && (IsExplorer()
		|| (fs.m_size != 0) || !(fs.m_attribute == CFile::archive));
	if( bExists )
	{
		if ((fs.m_attribute & CFile::readOnly) != 0)
		{
			// "File is read-only."
			ErrorBox(ERR_File_Read_Only, m_ofn.lpstrFile);
			return TRUE; // Don't let the dialog go away!
		}

		// "Replace existing file?"
		if (QuestionBox(ERR_File_Exists,
			MB_YESNO | MB_DEFBUTTON2,
			m_ofn.lpstrFile) != IDYES)
		{
			return TRUE; // Don't let the dialog go away!
		}
	}

	return FALSE;
}

void CIdisFileDialog::OnSelChangeFilter()
{
	if( m_bOpenFileDialog )
	{
		Default();				// the Open dialog hasn't changed
		return;
	}

	// get new filter index
	CComboBox* pCombo = (CComboBox*)(IsExplorer() ? 
		GetParent()->GetDlgItem(cmb1) : GetDlgItem(cmb1));
	ASSERT( pCombo != NULL );
	int nSel = pCombo->GetCurSel();
	// get extension from filter string array
	CString strFilter = GetFilterExtension(nSel);

	// extract extension (use first if more than one)
	CStringArray saFilterExt;
	LPCTSTR pch = strFilter;
	while( *pch != '\0' )
	{
		// find beginning of extension
		LPCTSTR pchDot = _tcschr(pch, '.');
		if( pchDot == NULL )
			break;

		// find end of extension (semicolon, space, or EOL)
		LPCTSTR pchSemi = pchDot;
		while( (*pchSemi != '\0') && (*pchSemi != ';') && !isspace(*pchSemi) )
			pchSemi = _tcsinc(pchSemi);

		// add the extension to the array, including the dot
		saFilterExt.Add(CString(pchDot, pchSemi - pchDot));
		pch = pchSemi;
	}

	static char acFilterExt[_MAX_EXT];
	if( saFilterExt.GetSize() == 0 )
		*acFilterExt = '\0';
	else
		strcpy(acFilterExt, saFilterExt[0]);
	if( *acFilterExt != '\0' )
	{
		ASSERT( *acFilterExt == '.' );
		m_ofn.lpstrDefExt = acFilterExt + 1;
		if( (_tcschr(m_ofn.lpstrDefExt, '*') != NULL) ||
			(_tcschr(m_ofn.lpstrDefExt, '?') != NULL) )
		{
			if( IsExplorer() || m_strCurrentExt.IsEmpty() )
				m_ofn.lpstrDefExt = NULL;
			else
				m_ofn.lpstrDefExt = ((LPCSTR)m_strCurrentExt) + 1;
			return;
		}
	}
	else
		m_ofn.lpstrDefExt = NULL;

	// get current filename
	CWnd* pFileName = IsExplorer() ? GetParent()->GetDlgItem(edt1)
		: GetDlgItem(edt1);
	ASSERT( pFileName != NULL );
	CString strFileName;
	pFileName->GetWindowText(strFileName);

	char acDrive[_MAX_DRIVE];
	char acDir[_MAX_DIR];
	char acName[_MAX_FNAME];
	char acExt[_MAX_EXT];
	_splitpath(strFileName,acDrive, acDir, acName, acExt);

	// if the file is already using *any* of the extensions, leave it alone
	for( int n = 0; n < saFilterExt.GetSize(); n++ )
	{
		if( _tcsicmp(saFilterExt[n], acExt) == 0 )
			return;
	}

	// otherwise coerce it to the primary (first) extension from the filter
	strFileName = acDrive;
	strFileName += acDir;
	strFileName += acName;
	strFileName += acFilterExt;

	pFileName->SetWindowText(strFileName);
}

void CIdisFileDialog::OnTypeChange(UINT idCtl, NMHDR* pNMHDR, LRESULT* pResult)
{
	OnSelChangeFilter();
}

static void AppendFilterSuffix(CString& filter, OPENFILENAME& ofn,
	CDocTemplate* pTemplate, CString* pstrDefaultExt)
{
	ASSERT_VALID(pTemplate);
	ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CDocTemplate)));

	CString strDefaultExt, strFilterName;
	if (pTemplate->GetDocString(strDefaultExt, CDocTemplate::filterExt) &&
	 !strDefaultExt.IsEmpty() &&
	 pTemplate->GetDocString(strFilterName, CDocTemplate::filterName) &&
	 !strFilterName.IsEmpty())
	{
		// a file based document template - add to filter list
		ASSERT(strDefaultExt[0] == '.');
		if (pstrDefaultExt != NULL)
		{
			// set the default extension
			*pstrDefaultExt = ((LPCTSTR)strDefaultExt) + 1;  // skip the '.'
			ofn.lpstrDefExt = (LPTSTR)(LPCTSTR)(*pstrDefaultExt);
			ofn.nFilterIndex = ofn.nMaxCustFilter + 1;  // 1 based number
		}

		// add to filter
		AppendFilterSuffix(filter, ofn, strFilterName);
	}
}


// File dialog filters may be specified in the registry by adding a key
// called "File Filters" to the "Visual C++ 2.0" key.  This key contains
// values with names of the form "Filter<n>" where <n> starts at 1.  The
// range of <n> must be contiguous.  Each value specifies one file filter
// in the form of "<class>,<filter>" where class is a number representing
// which dialogs the filter should be used in and <filter> specifies the text
// that will be seen in the "List Files of Type" dropdown in the file dialog.
//
// Each bit of <class> specifies a class of file dialog that filters will
// appear in.  Filters may appear in more than one class of dialog by
// specifying or'd class bits in the registry.  The FLT_ values defined in
// dlgbase.h describe the bit values for each class.

const char szFileFiltersKey [] = _TEXT("File Filters");

BOOL AddRegistryFilters(CString& strFilter, int nDesiredType, OPENFILENAME& ofn)
{
	BOOL bAddedFilter = FALSE;
	CString strTemp;
	for (int i = 1; ; i += 1)
	{
		TCHAR szName [40];
		wsprintf(szName, _TEXT("Filter%d"), i);
		strTemp = GetRegString(szFileFiltersKey, szName);
		if (strTemp.IsEmpty())
			break;

		char* szFilter = NULL;
		long nType = _tcstol(strTemp, &szFilter, 0);
		if (szFilter == NULL || *szFilter != ',' || (nDesiredType & nType) == 0)
			continue; // ignore illegal or undesirable entries

		szFilter += 1; // skip comma
		while (*szFilter == ' ')
			szFilter += 1;

		AppendFilterSuffix(strFilter, ofn, szFilter);
		bAddedFilter = TRUE;
	}

	return bAddedFilter;
}

// Append "All Files (*.*)" to a file filter list.
//
void AddWildFilter(CString& strFilter, OPENFILENAME& ofn)
{
	CString str;
	VERIFY(str.LoadString(AFX_IDS_ALLFILTER));
	AppendFilterSuffix(strFilter, ofn, str);

	ofn.lpstrFilter = strFilter;
}


// Fill out a file filter list with information from the registry, or some
// preset default filters.
//
// strFilter and ofn contain the filter list when done
// nDesiredType is one of the FLT_ values and specifies which dialog this is for
// rgidDefaults and nDefaults specify the array of default filter ids
// szCommon is the name of a reg entry that specifies a "Common Files" filter
// If szCommon is NULL, the filter list has no "Common Files" entry
//
void SetFileFilters(CString& strFilter, OPENFILENAME& ofn, int nDesiredType,
	UINT* rgidDefaults, int nDefaults, const TCHAR* szCommon /* = NULL */)
{
	CString str;
	int nDefault = 0;

	// Start with the "Common Files" entry, if any...
	if (szCommon != NULL)
	{
		str = GetRegString(szFileFiltersKey, szCommon);
		if (str.IsEmpty())
		{
			// Use first default if no registry entry
			if (nDefaults)
			{
				VERIFY(str.LoadString(rgidDefaults[nDefault++]));
			}
		}

		if (!str.IsEmpty())
		{
			AppendFilterSuffix(strFilter, ofn, str);
		}
	}

	// Now add the more specific filters...
	if (!AddRegistryFilters(strFilter, nDesiredType, ofn))
	{
		// No filters in the registry, use defaults...
		for (int i = nDefault; i < nDefaults; i++)
		{
			VERIFY(str.LoadString(rgidDefaults[i]));
			AppendFilterSuffix(strFilter, ofn, str);
		}
	}

	// Finish off the list with *.*
	AddWildFilter(strFilter, ofn);

	ofn.lpstrFilter = strFilter;
}


BOOL CTheApp::DoPromptFileSave(CString& fileName, UINT nIDSTitle,
	DWORD lFlags, CPartDoc* pDocument, UINT* pnSaveType)
{
	CIdisFileDialog dlgFile(FALSE, pDocument);		// Create a File Save dialog

	CString strTitle;
	VERIFY(strTitle.LoadString(nIDSTitle));
	dlgFile.m_ofn.lpstrTitle = strTitle;
	dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);
	dlgFile.m_ofn.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();

	dlgFile.m_ofn.Flags |= lFlags;

	CString strFilter;
	CString strDefault;
	UINT* pIDSFilters = NULL;
	UINT nSaveFilter = 0;

	ASSERT( pDocument != NULL );
	if( !pDocument->GetValidFilters(&pIDSFilters, &nSaveFilter) )
	{		// lacking anything better, use the template's doc info string
		CDocTemplate* pTemplate = pDocument->GetDocTemplate();
		ASSERT_VALID(pTemplate);
		AppendFilterSuffix(strFilter, dlgFile.m_ofn, pTemplate, &strDefault);
	}
	else
	{
		ASSERT( pIDSFilters != NULL );
		CString str;
		for( int n = 0; pIDSFilters[n] != 0; n++ )
		{
			str.LoadString(pIDSFilters[n]);
			if( pIDSFilters[n] == nSaveFilter )
			{
				AppendFilterSuffix(strFilter, dlgFile.m_ofn, str, TRUE);
				dlgFile.m_ofn.nFilterIndex = n + 1;		// default filter in combo
			}
			else
				AppendFilterSuffix(strFilter, dlgFile.m_ofn, str, FALSE);
		}
	}

	// Note current extension, and make that the default.  Can't actually 
	// use m_ofn.lpstrDefExt, because that gets tacked on to EVERYTHING!
	if( dlgFile.m_ofn.lpstrDefExt == NULL )
	{
		char acExt[_MAX_EXT];
		_splitpath(dlgFile.m_ofn.lpstrFile, NULL, NULL, NULL, acExt);
		ASSERT( (acExt[0] == '.') || (acExt[0] == '\0') );
		dlgFile.m_strCurrentExt = acExt;
		if( !dlgFile.IsExplorer() )
			dlgFile.m_ofn.lpstrDefExt = ((LPCSTR)dlgFile.m_strCurrentExt) + 1;
	}
	else
	{
		dlgFile.m_strCurrentExt.Empty();
		if (*(dlgFile.m_ofn.lpstrDefExt) != '.')
			dlgFile.m_strCurrentExt += ".";
		dlgFile.m_strCurrentExt += dlgFile.m_ofn.lpstrDefExt;
	}

	dlgFile.m_ofn.lpstrFilter = strFilter;

	BOOL bRet = (dlgFile.DoModal() == IDOK) ? TRUE : FALSE;

	if( bRet && pIDSFilters != NULL )
	{
		ASSERT( dlgFile.m_ofn.nFilterIndex > 0 );
		*pnSaveType = pIDSFilters[dlgFile.m_ofn.nFilterIndex - 1];
	}
	else
		*pnSaveType = 0;

	fileName.ReleaseBuffer();
	fileName.TrimLeft();
	fileName.TrimRight();

	return bRet;
}

SRankedStringID rsiOpenAsAuto =
{
	IDS_AUTO,	0
};

CString GetFilterDescription(LPCTSTR szFilter)
{
	LPCTSTR pchParen = _tcschr(szFilter, '(');
	if( pchParen == NULL )
		return szFilter;
	else
		return CString(szFilter, pchParen - szFilter);
}

void ParseFilters(LPCTSTR szFilter, CStringArray& rsa)
{
	// add any filters we find
	LPCTSTR pch = _tcschr(szFilter, '(');
	if( pch == NULL )
		return;
	pch = _tcsinc(pch);
	while( _istspace(*pch) || (*pch == ';') || (*pch == ')') )
		pch = _tcsinc(pch);

	while( *pch != '\0' )
	{
		LPCTSTR pchStart = pch;
		while( (*pch != '\0') && !_istspace(*pch) && (*pch != ';') && (*pch != ')') )
			pch = _tcsinc(pch);
		rsa.Add(CString(pchStart, pch - pchStart));
		while( _istspace(*pch) || (*pch == ';') || (*pch == ')') )
			pch = _tcsinc(pch);
	}
}

CString MergeFilters(LPCTSTR szFilter1, LPCTSTR szFilter2)
{
	CString strMerge = GetFilterDescription(szFilter1);
	ASSERT( strMerge == GetFilterDescription(szFilter2) );
	CStringArray sa;
	ParseFilters(szFilter1, sa);
	ParseFilters(szFilter2, sa);

	strMerge += '(';
	for( int n = 0; n < sa.GetSize(); n++ )
	{
		LPCTSTR szExt = sa[n];
		BOOL bUnique = TRUE;
		for( int nComp = 0; nComp < n; nComp++ )
		{
			if( _tcsicmp(szExt, sa[nComp]) == 0 )
			{
				bUnique = FALSE;
				break;
			}
		}
		if( bUnique )
		{
			if( n > 0 )
				strMerge += ";";
			strMerge += szExt;
		}
	}
	strMerge += ')';
	return strMerge;
}

void CTheApp::BuildFilterList(CString& rstrFilter, CPtrArray* ppaEditors, 
	int nOpenDialog, IPkgProject *pContextProject /* = NULL */)
{
	CString strOpenFilters;
	CPtrArray paFilters;
	if( ppaEditors != NULL && nOpenDialog != CPackage::GODI_WORKSPACEOPEN)
		ppaEditors->Add(&rsiOpenAsAuto);

	CLSID clsidActiveProject = CLSID_NULL;
	if (pContextProject == NULL)
	{
		IProjectWorkspace *pInterface;
		pInterface = g_IdeInterface.GetProjectWorkspace();
		if(pInterface)
		{
			IPkgProject *pProject;
			pInterface->GetActiveProject(&pProject);
			if (pProject != NULL)
			{
				VERIFY(SUCCEEDED(pProject->GetClassID(&clsidActiveProject)) && clsidActiveProject != CLSID_NULL);
				pProject->Release();
			}
		}
	}
	else
	{
		VERIFY(SUCCEEDED(pContextProject->GetClassID(&clsidActiveProject)) && clsidActiveProject != CLSID_NULL);
	}

	POSITION pos = m_packages.GetHeadPosition();
	while( pos != NULL )
	{
		CPackage* pPackage = (CPackage*)m_packages.GetNext(pos);
		ASSERT( pPackage != NULL );

		SRankedStringID* prsiFilters = NULL;
		SRankedStringID* prsiEditors = NULL;
		pPackage->GetOpenDialogInfo(&prsiFilters, &prsiEditors, nOpenDialog);

		// insert the filters into their list, in rank order
		if (prsiFilters != NULL)
		{
			for (; prsiFilters->ids != 0; prsiFilters++)		// the array must be NULL-terminated
			{
				int nInsert = 0;
				for (; nInsert < paFilters.GetSize(); nInsert++)
				{
					int nRank = ((SRankedStringID*)paFilters[nInsert])->nRank;
					const CLSID *pclsid = ((SRankedStringID*)paFilters[nInsert])->pclsid;
					if ((nRank > prsiFilters->nRank) || 
						((nRank == prsiFilters->nRank) && (clsidActiveProject != *pclsid) && (clsidActiveProject == *(prsiFilters->pclsid))))
					{
						break;
					}
				}
				paFilters.InsertAt(nInsert, prsiFilters);
			}
		}

		// insert the editors into their list, in rank order
		if ((ppaEditors != NULL) && (prsiEditors != NULL))
		{
			for (; prsiEditors->ids != 0; prsiEditors++)		// the array must be NULL-terminated
			{
				int nInsert = 0;
				for (; nInsert < ppaEditors->GetSize(); nInsert++)
				{
					int nRank = ((SRankedStringID*)ppaEditors->GetAt(nInsert))->nRank;
					const CLSID *pclsid = ((SRankedStringID*)paFilters[nInsert])->pclsid;
					if ((nRank > prsiEditors->nRank) || 
						((nRank == prsiEditors->nRank) && (clsidActiveProject != *pclsid) && (clsidActiveProject == *(prsiFilters->pclsid))))
					{
						break;
					}
				}
				ppaEditors->InsertAt(nInsert, prsiEditors);
			}
		}
	}

	// load strings
	CStringArray saFilters;
	for( int nFilter = 0; nFilter < paFilters.GetSize(); nFilter++ )
	{
		SRankedStringID* prsi = (SRankedStringID*)paFilters[nFilter];
		CString str;
		if (prsi->bIsID)
	 		VERIFY(str.LoadString(prsi->ids));
		else
			str = (LPTSTR)prsi->ids;
		saFilters.Add(str);
	}

	// eliminate duplicate strings
	for (nFilter = 0; nFilter < saFilters.GetSize(); nFilter++)
	{
		CString strNew = saFilters[nFilter];
		for (int nOld = 0; nOld < nFilter; nOld++)
		{
			CString strOld = saFilters[nOld];
			if (strNew == strOld)
			{	
				saFilters.RemoveAt(nFilter--);	// remove later one
				break;
			}
		}
	}

#if 0
	// merge duplicates
	for( nFilter = 0; nFilter < saFilters.GetSize(); nFilter++ )
	{
		CString strNew = saFilters[nFilter];
		CString strDesc = GetFilterDescription(strNew);
		for( int nOld = 0; nOld < nFilter; nOld++ )
		{
			CString strOld = saFilters[nOld];
			if( strDesc == GetFilterDescription(strOld) )
			{		// if filter categories are the same, merge them
				CString strMerge = MergeFilters(strOld, strNew);
				saFilters.SetAt(nOld, strMerge);		// update existing one
				saFilters.RemoveAt(nFilter--);			// remove later one
				break;
			}
		}
	}
#endif

	// build filter string
	OPENFILENAME ofnTemp;
	for( nFilter = 0; nFilter < saFilters.GetSize(); nFilter++ )
	{
		AppendFilterSuffix(strOpenFilters, ofnTemp, saFilters[nFilter]);
	}
	AddWildFilter(strOpenFilters, ofnTemp);		// Finish off the list with *.*

	// now that the list is built, add any user filters from the registry
	// FUTURE: check the registry
	rstrFilter = strOpenFilters;
}

BOOL CTheApp::DoPromptFileOpen(CStringArray& saFileNames, UINT nIDSTitle,
	DWORD lFlags, CDocTemplate* pTemplate,
	UINT* pnOpenAsEditor, BOOL* pbOpenReadOnly)
{
	static UINT iOpenFilter = 1;

	CIdisFileDialog dlgFile(TRUE);	// Create a File Open dialog

	CString title;
	VERIFY(title.LoadString(nIDSTitle));

	dlgFile.m_ofn.Flags |= lFlags;

	CString strFilter;
	CString strDefault;
	CPtrArray paFileOpenEditors;

	BuildFilterList(strFilter, &paFileOpenEditors, CPackage::GODI_FILEOPEN);
	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ppaEditors = &paFileOpenEditors;

	dlgFile.m_ofn.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
	dlgFile.m_ofn.lpstrTitle = title;
	if (dlgFile.m_ofn.Flags & OFN_ALLOWMULTISELECT)
			dlgFile.m_ofn.nMaxFile = 20480;	// See PSS Q99338
	char acFileNameBuffer[20480] = "";
	dlgFile.m_ofn.lpstrFile = acFileNameBuffer;
	dlgFile.m_psaFileNames = &saFileNames;

	dlgFile.m_ofn.nFilterIndex = (DWORD) iOpenFilter;

	BOOL bRet = (dlgFile.DoModal() == IDOK) ? TRUE : FALSE;

	if (pnOpenAsEditor != NULL)
		*pnOpenAsEditor = dlgFile.m_nOpenAsEditor;

	if (pbOpenReadOnly != NULL)
		*pbOpenReadOnly = dlgFile.GetReadOnlyPref();

	if (bRet)
		iOpenFilter = (UINT) dlgFile.m_ofn.nFilterIndex;

	return bRet;
}

void CTheApp::DestroyAll(UINT nFlags /*= 0*/)
{
	// Always use the mainframe's close all to destroy all windows.
	((CMainFrame *)m_pMainWnd)->CloseWindows(nFlags);
}

BOOL CTheApp::CanCloseAll(UINT nFlags /*= 0*/)
{
   // First see if all windows can close.
	CMDIClientValidator* m_pWndMDIClient = ((CMainFrame *)m_pMainWnd)->m_pWndMDIClient;
	CPartFrame* pMDIChild = MDITypedGetActive(nFlags);

	while (pMDIChild != NULL)
	{
		CPartView* pView = (CPartView*)pMDIChild->GetActiveView();
		if (pView != NULL)
		{
			ASSERT(pView->IsKindOf(RUNTIME_CLASS(CPartView)));
			if (!pView->CanClose())
				return FALSE;
		}

		pMDIChild = MDITypedGetNext(pMDIChild, nFlags);
	}

	return TRUE;
}

extern CShellRecorder g_theShellRecorder;

BOOL CTheApp::SaveAll(BOOL bQuery /*= FALSE*/, UINT nFlags /*= 0*/)
{
	POSITION pos = GetFirstDocTemplatePosition();
	BOOL bRetval = TRUE;
	// Save the active MDI child in case one of the templates activates one
	// of its views:
	CWnd *pWnd = ((CMDIFrameWnd *) theApp.m_pMainWnd)->MDIGetActive ();

	// DS96 #13311 [CFlaat]: record a Documents.SaveAll operation here
	g_theShellRecorder.EmitMethodCall(seSaveAll, CShellConstArg(scTrue));

	CWaitCursor waitCursor;
	CHaltRecording HaltRec; // DS96 #13311 [CFlaat]: don't record individual find operations hereafter

	while (pos)
	{
		CPartTemplate* pTemplate = (CPartTemplate*)GetNextDocTemplate(pos);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CPartTemplate)));
		if (!pTemplate->SaveAll(bQuery, nFlags))
		{
			bRetval = FALSE;
			break;
		}
	}
	if (((CMDIFrameWnd *) theApp.m_pMainWnd)->MDIGetActive () != pWnd)
	{
		((CMDIFrameWnd *) theApp.m_pMainWnd)->MDIActivate (pWnd);
	}
	return bRetval;
}

BOOL CTheApp::HasOpenDocument(UINT nFlags /*= 0*/)
{
	return (MDITypedGetActive(nFlags) != NULL);
}

///////////////////////////////////////////////////////////////////////////////
//      CTypedRecentFileList
//              Extended recent file list helper class implementation.

// DER - 5 June 96 CTypedRecentFileList now inherits from CBRecentFileList.
CTypedRecentFileList::CTypedRecentFileList(UINT nStart, LPCTSTR lpszSection,
		LPCTSTR lpszEntryFormat, LPCTSTR lpszEntryTypeFormat, int nSize,
		int nMaxDispLen /*=AFX_ABBREV_FILENAME_LEN*/, int nShortCount)
	: CBRecentFileList(nStart, lpszSection, lpszEntryFormat, nSize, nMaxDispLen, nShortCount)
{
	m_nLastSize=m_nSize;
	m_arrTypes = new CLSID[nSize];
	m_strEntryTypeFormat = lpszEntryTypeFormat;
}

CTypedRecentFileList::~CTypedRecentFileList()
{
	delete [] m_arrTypes;
}

CDocTemplate* CTypedRecentFileList::GetTypeAt(int nIndex)
{
	return theApp.GetTemplate(m_arrTypes[nIndex]);
}

void CTypedRecentFileList::Add(LPCTSTR lpszPathName, CDocTemplate* pTemplate)
{
//      ASSERT_VALID(this); CRecentFileList no longer is a CObject
	ASSERT(lpszPathName != NULL);
	ASSERT(AfxIsValidString(lpszPathName));

	// fully qualify the path name
	CPath path;
	if (!path.Create(lpszPathName))
		return;

	LPCTSTR lpszFullPath = path.GetFullPath();

	// update the MRU list
	int iMRU;
	// if an existing MRU string matches file name
	int nSize = GetSize();
	for (iMRU = 0; iMRU < nSize - 1; iMRU++)
	{
		if (lstrcmpi(m_arrNames[iMRU], lpszFullPath) == 0)
			break;      // iMRU will point to matching entry
	}

	// move the filenames down the list, removes the last filename
	//      or the matching filename (in iMRU).
	int istr;
	for (istr = iMRU; istr > 0; istr--)
	{
		m_arrNames[istr] = m_arrNames[istr - 1];
		m_arrTypes[istr] = m_arrTypes[istr - 1];
	}

	m_arrNames[0] = lpszFullPath; //put new items at beginning
	m_arrTypes[0] = (pTemplate == NULL) ?
		GUID_NULL : ((CPartTemplate*)pTemplate)->GetTemplateClsid();
}

void CTypedRecentFileList::ReadList()
{
	USES_CONVERSION;

	ASSERT(!m_strSectionName.IsEmpty());
	ASSERT(!m_strEntryFormat.IsEmpty());
	ASSERT(!m_strEntryTypeFormat.IsEmpty());

	int nSize = GetSize();
	LPTSTR pszEntry = new TCHAR[m_strEntryFormat.GetLength()+5];
	for (int iMRU = 0; iMRU < nSize; iMRU++)
	{
		wsprintf(pszEntry, m_strEntryFormat, iMRU + 1);
		m_arrNames[iMRU] = GetRegString(m_strSectionName, pszEntry);
		
		wsprintf(pszEntry, m_strEntryTypeFormat, iMRU + 1);
		CString str = GetRegString(m_strSectionName, pszEntry);
		if (str.IsEmpty() || (CLSIDFromString(A2W(str), &m_arrTypes[iMRU]) != S_OK))
			m_arrTypes[iMRU] = GUID_NULL;
	}
	delete [] pszEntry;
}

void CTypedRecentFileList::WriteList()
{
	USES_CONVERSION;

	ASSERT(!m_strSectionName.IsEmpty());
	ASSERT(!m_strEntryFormat.IsEmpty());

	LPWSTR lpwsz;
	
	int nSize = GetSize();
	LPTSTR pszEntry = new TCHAR[m_strEntryFormat.GetLength()+5];
	for (int iMRU = 0; iMRU < nSize; iMRU++)
	{
		if (m_arrNames[iMRU].IsEmpty())
			break;  // all done

		wsprintf(pszEntry, m_strEntryFormat, iMRU + 1);
		WriteRegString(m_strSectionName, pszEntry, m_arrNames[iMRU]);

		wsprintf(pszEntry, m_strEntryTypeFormat, iMRU + 1);
		if (StringFromCLSID(m_arrTypes[iMRU], &lpwsz) == NOERROR)
		{
			WriteRegString(m_strSectionName, pszEntry, W2A(lpwsz));
			AfxFreeTaskMem(lpwsz);
		}
	}

	delete[] pszEntry;
}

void CTypedRecentFileList::SetSize(int nSize)
{
	// resize the name array to the new size. Forced to do a manual copy - no realloc in c++
	CLSID *arrNewTypes=new CLSID[nSize];

	ASSERT(arrNewTypes);

	int nCopy=min(nSize,GetSize());

	for(int i=0;i<nCopy; ++i)
	{
		arrNewTypes[i]=m_arrTypes[i];
	}

	delete [] m_arrTypes;

	m_arrTypes=arrNewTypes;

	CBRecentFileList::SetSize(nSize);
}

void CBRecentFileList::SetStart(int nStart)
{
	m_nStart=nStart;
}

void CBRecentFileList::SetShortCount(int nShortCount)
{
	m_nShortCount=nShortCount;
}

// This code stolen from MFC sources, and modified to only show a limited number of accelerators, plus to
// work with either menus or command bars
void CBRecentFileList::UpdateMenu(CCmdUI* pCmdUI)
{
	ASSERT(m_arrNames != NULL);

	CMenu*			pWinMenu = pCmdUI->m_pMenu;
	CBMenuPopup *	pCBMenu= (CBMenuPopup*) pCmdUI->m_pOther;

	// do nothing for the  top level call
	if(pWinMenu && pCmdUI->m_pSubMenu)
	{
		return;
	}

	// This is used to extract the disable/empty string the first time we are called. 
	if (m_strOriginal.IsEmpty())
	{
		if(pWinMenu != NULL)
		{
			pWinMenu->GetMenuString(pCmdUI->m_nID, m_strOriginal, MF_BYCOMMAND);
		} 
		else
		{
			if(pCBMenu != NULL)
			{
				pCBMenu->GetMenuString(pCmdUI->m_nID, m_strOriginal, MF_BYCOMMAND);
			}
		}
	}

	if (m_arrNames[0].IsEmpty())
	{
		// no MRU files
		if (!m_strOriginal.IsEmpty())
			pCmdUI->SetText(m_strOriginal);
		pCmdUI->Enable(FALSE);
		return;
	}

	// No menu, no need to update, really.
	if (pWinMenu==NULL && pCBMenu==NULL)
		return;

	for (int iMRU = 0; iMRU < m_nLastSize; iMRU++)
	{
		if(pWinMenu)
		{
			pWinMenu->DeleteMenu(pCmdUI->m_nID + iMRU, MF_BYCOMMAND);
		}
		else if(pCBMenu)
		{
			pCBMenu->DeleteMenu(pCmdUI->m_nID + iMRU, MF_BYCOMMAND);
		}
	}

#ifndef _MAC
	TCHAR szCurDir[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH, szCurDir);
	int nCurDir = lstrlen(szCurDir);
	ASSERT(nCurDir >= 0);
	szCurDir[nCurDir] = '\\';
	szCurDir[++nCurDir] = '\0';
#endif

	int cxScreen = ::GetSystemMetrics(SM_CXSCREEN);
	TEXTMETRIC tm;

	// determine maximum size of menu item we're willing to create
	{
		CDC dc;               
		dc.CreateCompatibleDC(NULL);

		// select the menu font
		CFont *pOldFont=dc.SelectObject(globalData.GetMenuFont(FALSE));

		dc.GetTextMetrics(&tm);

		// ensure that zero wasn't returned for some reason
		if(tm.tmAveCharWidth==0)
		{
			// arbitrary
			tm.tmAveCharWidth=15;
		}

		// clean up context
		dc.SelectObject(pOldFont);
	}

	// menu should be allowed to hold enough characters to fill half the screen
	m_nMaxDisplayLength=(cxScreen/2)/tm.tmAveCharWidth;

	// small max len implies large fonts/small screen. Allow item to almost fill screen (90% fill)
	if(m_nMaxDisplayLength<10)
	{
		m_nMaxDisplayLength=(((double)cxScreen)*0.9)/(double)tm.tmAveCharWidth;

		ASSERT(m_nMaxDisplayLength>1);
	}

	CString strName;
	CString strTemp;
	int nMnemonics=m_nShortCount;
	for (iMRU = 0; iMRU < m_nSize; iMRU++)
	{
#ifndef _MAC
		if (!GetDisplayName(strName, iMRU, szCurDir, nCurDir))
			break;
#else
		if (!GetDisplayName(strName, iMRU, NULL, 0))
			break;
#endif

		// if the name is longer than the display length, then the result will just be the base name. We'll truncate that, and add an ellipsis
		if(strName.GetLength() > m_nMaxDisplayLength)
		{
			strName=strName.Left(m_nMaxDisplayLength-4)+" ...";
		}

		// double up any '&' characters so they are not underlined
		LPCTSTR lpszSrc = strName;
		LPTSTR lpszDest = strTemp.GetBuffer(strName.GetLength()*2);
		while (*lpszSrc != 0)
		{
			if (*lpszSrc == '&')
				*lpszDest++ = '&';
			if (_istlead(*lpszSrc))
				*lpszDest++ = *lpszSrc++;
			*lpszDest++ = *lpszSrc++;
		}
		*lpszDest = 0;
		strTemp.ReleaseBuffer();

		// insert mnemonic + the file name
		CString strMnemonic;
		int nMnemonic=iMRU+1+m_nStart;
		if(nMnemonics>0 && (nMnemonic)<11)
		{
			// insert mnemonic string
			if(nMnemonic<10)
			{
				wsprintf(strMnemonic.GetBuffer(10), _T("&%d "), nMnemonic);
			}
			else
			{
				wsprintf(strMnemonic.GetBuffer(10), _T("1&0 "));
			}

			strMnemonic.ReleaseBuffer();
			--nMnemonics;
		}
		if(pWinMenu)
		{
			pWinMenu->InsertMenu(pCmdUI->m_nIndex++,MF_STRING | MF_BYPOSITION, pCmdUI->m_nID++,	strMnemonic + strTemp);
		}
		else if(pCBMenu)
		{
			pCBMenu->InsertMenu(pCmdUI->m_nIndex++,MF_STRING | MF_BYPOSITION, pCmdUI->m_nID++, strMnemonic + strTemp);
		}
	}

	// remember how many items we added this time, so that we can delete the right number next time.
	m_nLastSize=m_nSize;

	// update end menu count
	pCmdUI->m_nIndex--; // point to last menu added
	if(pWinMenu)
	{
		pCmdUI->m_nIndexMax = pWinMenu->GetMenuItemCount();
	}
	else if(pCBMenu)
	{
		pCmdUI->m_nIndexMax = pCBMenu->GetMenuItemCount();
	}

	pCmdUI->m_bEnableChanged = TRUE;    // all the added items are enabled
}

void CBRecentFileList::SetSize(int nSize)
{
	// resize the name array to the new size. Forced to do a manual copy - no realloc in c++
	CString *arrNewNames=new CString[nSize];

	ASSERT(arrNewNames);

	int nCopy=min(nSize,GetSize());

	for(int i=0;i<nCopy; ++i)
	{
		arrNewNames[i]=m_arrNames[i];
	}

	delete [] m_arrNames;

	m_arrNames=arrNewNames;
	m_nSize=nSize;
}

/////////////////////////////////////////////////////////////////////////////
// MRU file list extended implementation

void CTheApp::OnUpdateRecentProjMenu(CCmdUI* pCmdUI)
{
	ASSERT_VALID(this);
	if (m_pRecentProjectList == NULL) // no MRU projects
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		m_pRecentProjectList->UpdateMenu(pCmdUI);
	}
}

// Before Orion 6072, this function assumed that ID_PROJ_MRU_LAST and
// ID_FILE_MRU_LAST were invalid MRU ids. They are valid, so the assertions 
// and code have been modified. martynl 12Mar96
BOOL CTheApp::OnOpenRecentFile(UINT nID)
{
	ASSERT_VALID(this);
	ASSERT((nID >= ID_FILE_MRU_FIRST && nID <= ID_FILE_MRU_LAST) ||
		(nID >= ID_PROJ_MRU_FIRST && nID <= ID_PROJ_MRU_LAST));

	BOOL bFile = TRUE;
	UINT nIndex = nID - ID_FILE_MRU_FIRST;
	if (nID >= ID_PROJ_MRU_FIRST && nID <= ID_PROJ_MRU_LAST)
	{
		bFile = FALSE;
		nIndex = nID - ID_PROJ_MRU_FIRST;
	}

	ASSERT((bFile && m_pRecentFileList != NULL) ||
		(!bFile && m_pRecentProjectList != NULL));
	ASSERT((bFile && nIndex < (UINT) m_pRecentFileList->GetSize()) ||
		(!bFile && nIndex < (UINT) m_pRecentProjectList->GetSize()));

	CString strOpen;

	if (bFile)
		strOpen = (*m_pRecentFileList)[nIndex];
	else
	{
		// executables opened as workspaces have extensions in MRU
		strOpen = (*m_pRecentProjectList)[nIndex];
		// Check first if a .dsw or mdp file exists. If not,
		// we probably have a exe open as a workspace.
		CString strOpenT = strOpen + _T(WORKSPACE_EXT);
		if (_access(strOpenT, 00) == 0)
		{
			strOpen = strOpenT;
		}
		else
		{
			CString strOpenT2 = strOpen + _T(".mdp");;
			if (_access(strOpenT2, 00) == 0)
				strOpen = strOpenT2;
		}
	}

	ASSERT(strOpen.GetLength() != 0);

	TRACE2("MRU: open file (%d) '%s'.\n", nIndex, (LPCTSTR)strOpen);

	// Try to choose the right template to open the file.
	CDocTemplate* pOpenTemplate = NULL;
	if (!bFile)
	{
		pOpenTemplate = GetTemplate(CLSID_WorkspaceDocument);
 
		// If this doesn't look like a MDP file the let the application decide
		CDocument* pOpenDoc;
		if (pOpenTemplate != NULL && !pOpenTemplate->MatchDocType(strOpen, pOpenDoc))
			pOpenTemplate = NULL;
	}
	else
	{
		pOpenTemplate = ((CTypedRecentFileList*) m_pRecentFileList)->
			GetTypeAt(nID - ID_FILE_MRU_FILE1);
	}

	// See if the file is already open.
	CDocument* pCurDoc;
	CDocTemplate* pCurTemplate = GetOpenTemplate(strOpen, pCurDoc);

	if (pCurTemplate != NULL)
	{
		ASSERT(pCurDoc->IsKindOf(RUNTIME_CLASS(CPartDoc)));

		((CPartDoc*) pCurDoc)->ActivateDoc();
		if (pOpenTemplate == NULL || pOpenTemplate == pCurTemplate ||
			!((CPartDoc*) pCurDoc)->AttemptCloseDoc())
			return TRUE;
	}

	// Set a flag so that we can find out if we were opened using the MRU or not.
	m_bOpenedFromMRU = TRUE;

	if (pOpenTemplate == NULL)
		pCurDoc = OpenDocumentFile(strOpen);
	else
		pCurDoc = pOpenTemplate->OpenDocumentFile(strOpen);

	if (pCurDoc == NULL)
	{
		// If file does not exist, remove the filename from the appropriate MRU list.
		if (_access(strOpen, 00) != 0)
		{
			CRecentFileList *pRecentList = (bFile ? m_pRecentFileList : m_pRecentProjectList);
			int iCount = pRecentList->GetSize();
			for (int i = 0; i < iCount; i++)
			{
				CString strMRU;
				strMRU = (*(pRecentList))[i];

				char szPath[_MAX_PATH], szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];
				_splitpath(strOpen, szDrive, szDir, szFName, szExt);
				if (bFile)
					_makepath(szPath, szDrive, szDir, szFName, szExt);
				else
					_makepath(szPath, szDrive, szDir, szFName, NULL);

				if (!(strMRU.CompareNoCase(szPath)))
				{
					pRecentList->Remove(i);
					break;
				}
			}
		}
	}

	// Reset the flag
	m_bOpenedFromMRU = FALSE;

	return TRUE;
}

void CTheApp::LockRecentFileList(BOOL bLock)
{
	if (bLock)
		m_nLockRecentFileList++;
	else
		m_nLockRecentFileList--;
}

void CTheApp::AddToRecentFileList(LPCTSTR lpszPathName)
	// pszPathName must be a full path in upper-case ANSI character set
{
	if (m_nLockRecentFileList < 1)
	{
		CDocument* pDoc;
		CDocTemplate* pTemplate = GetOpenTemplate(lpszPathName, pDoc);

		if (m_pRecentProjectList != NULL && pTemplate == GetTemplate(CLSID_WorkspaceDocument))
		{
			// olympus 16294 [patbr]
			// it's a workspace, so add it to the MRU workspace list, but only
			// if the file exists on disk--otherwise we'll handle this when
			// the workspace is closed and the file is written to disk.
			struct _stat stBuf;
			if (_stat(lpszPathName, &stBuf) != -1)
			{
				// executables opened as workspaces have extensions in MRU--all
				// other workspace files (DSW/MDP) appear with no extension
				TCHAR * pchDot = _tcsrchr(lpszPathName, '.');
				if (!_tcsicmp(pchDot, WORKSPACE_EXT) || !_tcsicmp(pchDot, ".mdp"))
				{
					CString strProjName(lpszPathName, pchDot - lpszPathName);
					m_pRecentProjectList->Add(strProjName);
				}
				else
				{
					CString strProjName(lpszPathName);
					m_pRecentProjectList->Add(strProjName);
				}
			}
		}
		else if (m_pRecentFileList != NULL)
		{
			// otherwise add to recent file list
			if( (pTemplate != NULL) && 
				pTemplate->IsKindOf(RUNTIME_CLASS(CDocObjectTemplate)) )
			{
				// docobject template--not necessary to remember the type
				((CTypedRecentFileList*)m_pRecentFileList)->Add(lpszPathName, NULL);
			}
			else
			{
				// remember the type file was opened as
				((CTypedRecentFileList*)m_pRecentFileList)->Add(lpszPathName, pTemplate);
			}
		}
	}
}

void CTheApp::SetRecentFileListSize(int nSize)
{ 
	((CBRecentFileList *)m_pRecentFileList)->SetSize(nSize); 
}

void CTheApp::SetRecentProjectListSize(int nSize)
{
	m_pRecentProjectList->SetSize(nSize); 
}

int CTheApp::GetRecentFileListSize()
{
	return m_pRecentFileList->GetSize();
}

int CTheApp::GetRecentProjectListSize()
{
	return m_pRecentProjectList->GetSize();
}

void CVshellPackage::OnFileNew()
{
	theApp.OnFileNew();
}

void CVshellPackage::OnFileNewInProject()
{
	theApp.OnFileNewInProject();
}

void CVshellPackage::OnFileOpen()
{
	theApp.OnFileOpen();
}

void CVshellPackage::OnFileSaveAll()
{
	theApp.SaveAll(FALSE, FILTER_DEBUG);
}

void CVshellPackage::OnInsertFiles()
{
	theApp.OnInsertFiles();
}

void CVshellPackage::OnFileSaveAllExit()
{
	theApp.SaveAll(FALSE, FILTER_DEBUG);
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0L);
}

