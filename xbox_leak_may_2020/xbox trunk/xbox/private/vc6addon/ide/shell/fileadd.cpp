// fileadd.cpp : implementation file for CAddFilesDialog

#include "stdafx.h"		// the standard AFX include
#pragma hdrstop
#include "resource.h"
#include "dlgs.h"		// for commdlg control IDs
#include "toolexpt.h"

#include <cderr.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddFilesDialog

//  COMMDLG's special file validation message:
const UINT CAddFilesDialog::msgValidateFileMessage = ::RegisterWindowMessage(FILEOKSTRING);

CAddFilesDialog::CAddFilesDialog()
	: C3dFileDialog(TRUE, NULL, NULL, OFN_ENABLETEMPLATE | OFN_SHOWHELP | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT,
		NULL, AfxGetApp()->m_pMainWnd, NULL, IDD_CHICAGO_ADDFILES)
{
	m_pProject = NULL;
	m_strFolderName.Empty();
}

CAddFilesDialog::CAddFilesDialog(IPkgProject *pProject, const CString &strFolderName)
	: C3dFileDialog(TRUE, NULL, NULL, OFN_ENABLETEMPLATE | OFN_SHOWHELP | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT,
		NULL, AfxGetApp()->m_pMainWnd, NULL, IDD_CHICAGO_ADDFILES)
{
	m_pProject = pProject;
	m_strFolderName = strFolderName;
}

CAddFilesDialog::~CAddFilesDialog()
{
}

BEGIN_MESSAGE_MAP(CAddFilesDialog, C3dFileDialog)
	//{{AFX_MSG_MAP(CAddFilesDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CDir CAddFilesDialog::c_dirInitial;

/////////////////////////////////////////////////////////////////////////////

void CAddFilesDialog::ResetCurrentDir()
{
	c_dirInitial.CreateFromCurrent();
}

int CAddFilesDialog::DoModal()
{
	CString strDialogTitle;
	VERIFY(strDialogTitle.LoadString(IDS_INSERTFILESDLG));
	m_ofn.lpstrTitle = strDialogTitle;
        CString *pszType;
	CString strFilter;
        IPkgProject *pPkgProject;
        BOOL fMustRelease = FALSE;
        
	theApp.BuildFilterList(strFilter, NULL, CPackage::GODI_ADDFILE, m_pProject);
        
        // Make sure we have a valid CPkgProject
        pPkgProject = m_pProject;
	if (pPkgProject == NULL)
	{
            IProjectWorkspace *pInterface;
            
            pInterface = g_IdeInterface.GetProjectWorkspace();
            if(pInterface)
            {
                pInterface->GetActiveProject(&pPkgProject);

            }
            ASSERT(pPkgProject);
            fMustRelease = TRUE;
	}
        
        // Set the default filter based on project type
        HRESULT hr = pPkgProject->GetProjectTypeName(&pszType);
        if (SUCCEEDED(hr) && *pszType != _T("Generic Project"))
        {
            m_ofn.nFilterIndex = 1;
        }
        else
        {
            int    nCount;
            LPTSTR szLastFilter;
            
            // Count the number of filters and choose the last one
            for (nCount = 0, szLastFilter = strFilter.GetBuffer(2);
                 *szLastFilter; nCount++)
            {
                while (*szLastFilter)
                {
                    szLastFilter++;
                }
                szLastFilter++;
            }
            m_ofn.nFilterIndex = nCount / 2;
        }
	m_ofn.lpstrFilter = strFilter;

        // Release the PkgProject if necessary
        if (fMustRelease)
        {
            pPkgProject->Release();
        }

	// no read-only checkbox
	m_ofn.Flags |= OFN_HIDEREADONLY;

	// set up our initial directory
	if (c_dirInitial.GetLength() == 0)
		c_dirInitial.CreateFromCurrent();
	m_ofn.lpstrInitialDir = (LPCSTR)(const TCHAR *)c_dirInitial;

	TCHAR szFileBuf[20480];
	lstrcpy(szFileBuf, _T("\0"));
	m_ofn.lpstrFile = szFileBuf;
	m_ofn.nMaxFile = 20480;

	m_fSuccess = FALSE;	// Hook will set this to true if OK was pressed.  
						// Return value of GetOpenFileName means nothing.

	// Do the base class thing
	C3dFileDialog::DoModal();

	// Check for errors
	if (m_fSuccess && (CommDlgExtendedError() != 0L))
	{
		m_fSuccess = FALSE;
		TRACE("In CAddFilesDialog::DoModal() CommDlgExtendedError returned (%lx)\n",CommDlgExtendedError());
		if (CommDlgExtendedError() == FNERR_BUFFERTOOSMALL)
		{
			AfxMessageBox(IDS_TOOMANY_FILES);
		}
	}

	c_dirInitial.CreateFromCurrent();
	return(m_fSuccess ? IDOK : IDCANCEL);
}

BOOL CAddFilesDialog::OnInitDialog()
{
	C3dFileDialog::OnInitDialog();

	// Use appropriate OK button
	SetOkButtonText(IDS_INSERTFILESDLG_OK);

	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_PROJECTTOADDTO);
	
	if (m_strFolderName.IsEmpty())
	{
		// resize the commdlg controls slightly for long filter strings, and also
		// position the Insert Into combo directly below the Filters combo, same size
		CRect rcFileName, rcInsertInto, rcFilters, rcInsertIntoTxt, rcFilterTxt;
		pComboBox->GetWindowRect(&rcInsertInto);

		CWnd *pFileName = GetParent()->GetDlgItem(edt1);
		ASSERT(pFileName != NULL);			// File name edit
		pFileName->GetWindowRect(&rcFileName);
		rcFileName.right += 5;
		pFileName->SetWindowPos(NULL, rcFileName.left, rcFileName.top, rcFileName.right - rcFileName.left,
			rcFileName.bottom - rcFileName.top, SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER | SWP_SHOWWINDOW);

		CWnd *pFilters = GetParent()->GetDlgItem(cmb1);
		ASSERT(pFilters != NULL);			// List files of type combo
		pFilters->GetWindowRect(&rcFilters);
		rcFilters.right += 5;
		pFilters->SetWindowPos(NULL, rcFilters.left, rcFilters.top, rcFilters.right - rcFilters.left,
			rcFilters.bottom - rcFilters.top, SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER | SWP_SHOWWINDOW);

		CWnd *pFilterTxt = GetParent()->GetDlgItem(stc2);
		ASSERT(pFilterTxt != NULL);			// List files of type caption
		pFilterTxt->GetWindowRect(&rcFilterTxt);

		CWnd *pInsertIntoTxt = GetDlgItem(IDC_PROJECTTOADDTO_LABEL);
		ASSERT(pInsertIntoTxt != NULL);		// Insert Into caption
		pInsertIntoTxt->GetWindowRect(&rcInsertIntoTxt);

		rcInsertIntoTxt.left = rcFilterTxt.left;
		rcInsertIntoTxt.right = rcFilterTxt.right;
		ScreenToClient(&rcInsertIntoTxt);
		pInsertIntoTxt->MoveWindow(&rcInsertIntoTxt);

		rcInsertInto.left = rcFilters.left;
		rcInsertInto.right = rcFilters.right;
		ScreenToClient(&rcInsertInto);
		pComboBox->MoveWindow(&rcInsertInto);
	}

	CString strProjectName;
	LPPROJECTWORKSPACE pInterface = g_IdeInterface.GetProjectWorkspace();
	if (pInterface != NULL)
	{
		CADWORD Projects;
		pInterface->GetLoadedProjects(&Projects);
		int nSize;
		if ((nSize = Projects.cElems) != 0)
		{
			IPkgProject *pActive;
			if (m_pProject == NULL)
			{
				COleRef<IPkgProject> pProject;
				pInterface->GetActiveProject(&pProject);
				pActive = pProject;
			}
			else
				pActive = m_pProject;

			for (int nIndex = 0; nIndex < nSize; nIndex++)
			{
				IPkgProject *pProject = (IPkgProject *)Projects.pElems[nIndex];
				LPCOLESTR pszProjName, pszProjPath;
				DWORD dwPackageID, dwFlags;
				pProject->GetProjectInfo(&pszProjName, &pszProjPath, &dwPackageID, &dwFlags);
				CString strProject = pszProjName;
				CoTaskMemFree((void *)pszProjName);
				CoTaskMemFree((void *)pszProjPath);
				
				VERIFY(pComboBox->InsertString(-1, strProject) == nIndex);
				pComboBox->SetItemData(nIndex, (DWORD)pProject);
				if (pProject == pActive)
				{
					pComboBox->SetCurSel(nIndex);
					strProjectName = strProject;
				}
				// The projects are still referenced by the workspace,
				// so we now they will be good for the duration of this dialog.
				pProject->Release();
			}
		}
		CoTaskMemFree((void *)Projects.pElems);
	}

	if (!m_strFolderName.IsEmpty())
	{
		// resize the commdlg controls slightly for long filter strings, and also
		// position the Insert Into text directly below the Filters combo, same size
		CRect rcFileName, rcCancel, rcInsertIntoTxt, rcFilterTxt, rcFilters, rcTextFrame;

		CWnd *pFileName = GetParent()->GetDlgItem(edt1);
		ASSERT(pFileName != NULL);			// File name edit
		pFileName->GetWindowRect(&rcFileName);
		rcFileName.right += 5;
		pFileName->SetWindowPos(NULL, rcFileName.left, rcFileName.top, rcFileName.right - rcFileName.left,
			rcFileName.bottom - rcFileName.top, SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER | SWP_SHOWWINDOW);

		CWnd *pFilters = GetParent()->GetDlgItem(cmb1);
		ASSERT(pFilters != NULL);			// List files of type combo
		pFilters->GetWindowRect(&rcFilters);
		rcFilters.right += 5;
		pFilters->SetWindowPos(NULL, rcFilters.left, rcFilters.top, rcFilters.right - rcFilters.left,
			rcFilters.bottom - rcFilters.top, SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER | SWP_SHOWWINDOW);

		CWnd *pCancel = GetParent()->GetDlgItem(IDCANCEL);
		ASSERT(pCancel != NULL);			// Cancel button
		pCancel->GetWindowRect(&rcCancel);

		CWnd *pFilterTxt = GetParent()->GetDlgItem(stc2);
		ASSERT(pFilterTxt != NULL);			// List files of type caption
		pFilterTxt->GetWindowRect(&rcFilterTxt);

		pComboBox->ShowWindow(SW_HIDE);
		CWnd *pLabel = GetDlgItem(IDC_PROJECTTOADDTO_LABEL);
		pLabel->ShowWindow(SW_HIDE);

		CWnd *pInsertIntoTxt = GetDlgItem(IDC_FOLDERPROJECT);
		ASSERT(pInsertIntoTxt != NULL);		// Insert Into caption
		CString strMsg;
		pInsertIntoTxt->SetWindowText(MsgText(strMsg, IDS_INSERTING_FILES, m_strFolderName, strProjectName));
		pInsertIntoTxt->GetWindowRect(&rcInsertIntoTxt);
		rcInsertIntoTxt.left = rcFilterTxt.left;
		rcInsertIntoTxt.right = rcCancel.right;
		ScreenToClient(&rcInsertIntoTxt);
		pInsertIntoTxt->MoveWindow(&rcInsertIntoTxt);
		pInsertIntoTxt->ShowWindow(SW_SHOW);
	}

	return TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAddFilesDialog::OnFileNameOK()
{
	BOOL bRet = CFileDialog::OnFileNameOK();

	// workaround for Windows bug [patbr]
	// allocate enough space for the filenames selected/entered by the user
	CEdit *pEdit = (CEdit *)GetParent()->GetDlgItem(edt1);
	int nLength = pEdit->GetWindowTextLength();
	CDir dir;
	VERIFY(dir.CreateFromCurrent());
	nLength += dir.GetLength() + _MAX_EXT + 3;	// add room for extension, backslash, trailing double NULL

	LPSTR lpstrFileT = m_ofn.lpstrFile;
	int nMaxFileT = m_ofn.nMaxFile;

	LPSTR lpstrFile = (LPSTR)malloc(nLength);
	memset(lpstrFile, 0, nLength);
	m_ofn.lpstrFile = lpstrFile;
	m_ofn.nMaxFile = (DWORD)nLength;

	CString strText;
	pEdit->GetWindowText(strText);
	if (strText.Find("\"") >= 0)
	{
		// we may have multiple filenames selected or entered by the user
		// also, commdlg doesn't handle quotes properly
		_tcscpy(m_ofn.lpstrFile, (const TCHAR *)dir);
		int nOffset = dir.GetLength() + 1;
		m_ofn.lpstrFileTitle = m_ofn.lpstrFile + nOffset;
		m_ofn.nFileOffset = (WORD)nOffset;
		int nStart, nEnd;
		while ((nStart = strText.Find("\"")) != -1)
		{
			strText = strText.Right(strText.GetLength() - nStart - 1);
			nEnd = strText.Find("\"");
			CString strName = strText.Left(nEnd);
			_tcscpy(m_ofn.lpstrFile + nOffset, strName);
			strText = strText.Right(strText.GetLength() - nEnd - 1);
			nOffset += nEnd + 1;
		}
	}
	else
	{
		// single filename selected or entered--commdlg has
		// the right path and we don't have to do anything to it
		_tcscpy(m_ofn.lpstrFile, lpstrFileT);
		m_ofn.nMaxFile = nMaxFileT;
	}

	m_saFileNames.RemoveAll();
	POSITION pos = GetStartPosition();
	BOOL fFileFound;
	while (pos != (POSITION)NULL)
	{
		fFileFound = FALSE;

		CString strFileName = GetNextPathName(pos);
		CPath path;
		path.Create(strFileName);
		if (path.IsInit() && path.ExistsOnDisk())
		{
#ifdef NOTDEF
			CString strFile = strFileName;
			strFile.MakeUpper();
			CString strPrj = BUILDER_EXT;
			strPrj.MakeUpper();
			CString strWks = WORKSPACE_EXT;
			strWks.MakeUpper();
			if (strFile.Find(strPrj) != -1)
			{
				CString strMsg;
				MsgBox(Error, MsgText(strMsg, IDS_ERR_CANNOT_ADD_PRJ, strFileName));
			}
			else if (strFile.Find(strWks) != -1)
			{
				CString strMsg;
				MsgBox(Error, MsgText(strMsg, IDS_ERR_CANNOT_ADD_WKS, strFileName));
			}
			else
#endif	// NOTDEF
			{
				m_saFileNames.Add(strFileName);
				fFileFound = TRUE;
			}
		}
		else
		{
			if(!path.IsInit())
			{
				ErrorBox(ERR_File_Not_Found, strFileName);
				bRet = TRUE;
				goto exit;
			}

			// first, try to give the file an extension if it hasn't got one
			CString strExt = GetExtension(strFileName);
			if (strExt.IsEmpty())
			{
				CComboBox* pComboBox = (CComboBox*)GetParent()->GetDlgItem(cmb1);

				ASSERT (pComboBox != NULL);
				int nSel = pComboBox->GetCurSel();
				if (nSel >= 0)
				{
					LPCSTR lpsz = m_ofn.lpstrFilter;
					while (lpsz != NULL && nSel--)
					{
						lpsz += lstrlen(lpsz)+1;		// skip past display text
						if (lpsz != NULL)
							lpsz += lstrlen(lpsz)+1;    // skip past *.ext info
					}

					if (lpsz != NULL)
					{
						lpsz += lstrlen(lpsz)+1;    // skip past display text
						ASSERT(lpsz[0] == '*');
						ASSERT(lpsz[1] == '.');
						ASSERT(lpsz[2] != '\0');
						do
						{
							CString strTestName = strFileName;
							_TCHAR ext[_MAX_EXT];
							int i = 0;

							while (*++lpsz != '\0' && *lpsz != ';')
								ext[i++] = *lpsz;
							ext[i] = '\0';

							strTestName += ext;

							// Check for existence.
							if (_access(strTestName, 00) == 0)
							{
					    		// Update the file name
								m_saFileNames.Add(strTestName);
								fFileFound = TRUE;
							}

						} while ((*lpsz++ != '\0') && (!fFileFound));
					}
				}
			}

			if (!fFileFound)
			{
				CString strMsg;
				int nReturn;
				if (strFileName.Find(" ") > 0)
				{
					CString strDir = strFileName.Left(strFileName.ReverseFind('\\'));
					CString strFile = strFileName.Right(strFileName.GetLength() - strFileName.ReverseFind('\\') - 1);
					nReturn = MsgBox(Question, MsgText(strMsg, IDS_ADD_NONEXISTENT_FILE2, strFileName, strFile, strDir), MB_YESNOCANCEL);
				}
				else
					nReturn = MsgBox(Question, MsgText(strMsg, IDS_ADD_NONEXISTENT_FILE, strFileName), MB_YESNOCANCEL);

				if (nReturn == IDYES)
				{
					m_saFileNames.Add(strFileName);
				}
				else if (nReturn == IDCANCEL)
				{
					bRet = TRUE;
					goto exit;
				}
			}
		}
	}

	{ // scope
		int nIndex = ((CComboBox *)GetDlgItem(IDC_PROJECTTOADDTO))->GetCurSel();
		m_pProject = (IPkgProject *)(((CComboBox *)GetDlgItem(IDC_PROJECTTOADDTO))->GetItemDataPtr(nIndex));
		// we don't allow wildcards in the filenames...
		for (int iFile = 0; iFile < m_saFileNames.GetSize(); iFile++)
		{
			if (m_saFileNames[iFile].FindOneOf("*?") != -1)
			{
				bRet = TRUE;
				goto exit;
			}
		}
	}

	m_fSuccess = TRUE;
exit:
	free(lpstrFile);
	m_ofn.lpstrFile = lpstrFileT;
	m_ofn.nMaxFile = nMaxFileT;
	return(bRet);
}

/////////////////////////////////////////////////////////////////////////////
