//
// Implementation for CBatchBldDlg class
//
// History:
// Date				Who			What
// 01/17/94			colint			created
//////////////////////////////////////////////////////////////////

#include "stdafx.h"		// standard AFX include
#pragma hdrstop
#include "batchdlg.h"
#include "resource.h"
#include "msgboxes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


CMapStringToPtr CBatchBldDlg::m_mapSelected;
BOOL CBatchBldDlg::m_bSelectionOnly = FALSE;

///////////
// CBatchBldDlg IMPLEMENTATION
///////////
CBatchBldDlg::CBatchBldDlg ( CWnd * pParent /* = NULL */)
 			: C3dDialog (CBatchBldDlg::IDD, pParent) 
{
	m_bRebuild = FALSE;
	m_bClean = FALSE;
}

BEGIN_MESSAGE_MAP (CBatchBldDlg, C3dDialog)
	//{{AFX_MSG_MAP (CBatchBldDlg)
		ON_BN_CLICKED(IDC_REBUILD_BATCH, OnRebuild)
		ON_BN_CLICKED(IDC_CLEAN_BATCH, OnClean)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP	()

void CBatchBldDlg::DoDataExchange(CDataExchange* pDX)
{
	C3dDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBatchBldDlg)
	DDX_Check(pDX, IDC_SELECT_BATCH, m_bSelectionOnly);
	//}}AFX_DATA_MAP
}

///////////////////////////////
// BOOL CBatchBldDlg::OnInitDialog()
///////////////////////////////
BOOL CBatchBldDlg::OnInitDialog()
{
	CString	strCurrentConfigName ;
	CString strProject;
	CListBox * plbTarget = (CListBox *)GetDlgItem (IDC_TARGET_LIST) ;
	int i, iCurSel, nConfigs;
	const ConfigurationRecord * pcr;
	CPlatform * pPlatform;
	ASSERT (g_pActiveProject != NULL);

	// Subclass the check listbox
	VERIFY(m_lbConfigs.SubclassDlgItem(IDC_TARGET_LIST, this));
	m_lbConfigs.SetRedraw(FALSE);

	// call base class OnInitDialog
	C3dDialog::OnInitDialog() ;
	// get the project's current configuration
	g_pActiveProject->GetStrProp(P_ProjActiveConfiguration, strCurrentConfigName);

	m_lbConfigs.ResetContent();
	// First, enumerate all possible configurations and put the supported
	// ones in the list box
	CProject::InitProjectEnum();
	CProject * pProject;
	CClientDC dc(&m_lbConfigs);
	int width, maxwidth = 0;

	while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, TRUE)) != NULL)
	{
		const CPtrArray & ppcr = *pProject->GetConfigArray();
		nConfigs = ppcr.GetSize();
		for (i = 0; i < nConfigs; i++)
		{
			pcr = (ConfigurationRecord *)ppcr[i];
		
			// Only works for internal projects
			if( pProject->m_bProjIsExe )
				continue;

	 		// If the project is an internal makefile then we must get the
			// platform from the projtype of ppcr[i]
			CProjType * pprojtype;
			VERIFY(g_prjcompmgr.LookupProjTypeByName(pcr->GetOriginalTypeName(), pprojtype));
			pPlatform = pprojtype->GetPlatform();
 
			if (pPlatform->IsSupported() && (pPlatform->GetBuildable()==TRUE) )
			{
				CString strConfig = pcr->GetConfigurationName();
				m_lbConfigs.AddString(strConfig);
				width = dc.GetTextExtent(strConfig, strConfig.GetLength()).cx;
				maxwidth = __max(maxwidth, width);
			}
		}
	}
		
	// might need horz scrollbars
	m_lbConfigs.SetHorizontalExtent(maxwidth + 14); // add width of checkbox

	// update # of configs actually supported
	nConfigs = m_lbConfigs.GetCount();

	iCurSel = plbTarget->FindStringExact (-1, (const TCHAR *)strCurrentConfigName) ;
	if (iCurSel!=LB_ERR) plbTarget->SetCurSel(iCurSel);

	for (i = 0; i < nConfigs; i++){
		if( m_mapSelected.GetCount() ){
			CString strConfigName;
			m_lbConfigs.GetText(i, strConfigName);
			void *pDummy;
			if( m_mapSelected.Lookup(strConfigName, pDummy) ){
				m_lbConfigs.SetCheck(i, TRUE);
			}
		} else {
			m_lbConfigs.SetCheck(i, TRUE);
		}
	}

	m_lbConfigs.SetRedraw(TRUE);

	return TRUE ;
}

///////////////////////////////
// CBatchBldDlg::OnOK()
///////////////////////////////
VOID CBatchBldDlg::OnOK()
{
	GetConfigs();
	m_bRebuild = FALSE;
	m_bClean = FALSE;
	C3dDialog::OnOK();
}

///////////////////////////////
// CBatchBldDlg::OnRebuild()
///////////////////////////////
VOID CBatchBldDlg::OnRebuild()
{
	GetConfigs();
	m_bRebuild = TRUE;
	m_bClean = FALSE;
	C3dDialog::OnOK();
}

///////////////////////////////
// CBatchBldDlg::OnRebuild()
///////////////////////////////
VOID CBatchBldDlg::OnClean()
{
	GetConfigs();
	m_bClean = TRUE;
	m_bRebuild = FALSE;
	C3dDialog::OnOK();
}

///////////////////////////////
// CBatchBldDlg::GetConfigs()
///////////////////////////////
void CBatchBldDlg::GetConfigs()
{
	// Construct the list of configs to build
	int nItem, nConfigs;

	// Construct a CStringList of the config names
	// that we are going to build
	m_pBuildConfigs->RemoveAll();
	nConfigs = m_lbConfigs.GetCount();
	m_mapSelected.RemoveAll();
	for (nItem = 0; nItem < nConfigs; nItem++)
	{
		if (m_lbConfigs.GetCheck(nItem))
		{	
			CString strConfigName;

			m_lbConfigs.GetText(nItem, strConfigName);
			m_pBuildConfigs->AddTail(strConfigName);
			m_mapSelected.SetAt(strConfigName,NULL);
		}
	}
}

///////////////////////////////
// CBatchBldDlg::OnCancel()
///////////////////////////////
VOID CBatchBldDlg::OnCancel()
{
	// Cancel, so no build is done

	C3dDialog::OnCancel();
}


BOOL CBatchBldDlg::LoadBatchBldOpt(CArchive & archive)
{
	TRY
	{

		// Construct the list of configs to build
		int nItem, nConfigs;

		archive >> nConfigs;
		if (nConfigs == -1)
		{
			// -1 indicates this is really a version flag
			// that we should read m_bSelectionOnly
			archive >> (int)m_bSelectionOnly;

			// now read the real nConfigs
			archive >> nConfigs;
		}
		
		m_mapSelected.RemoveAll();
		for (nItem = 0; nItem < nConfigs; nItem++)
		{
			CString strConfigName;

			archive >> strConfigName;
			m_mapSelected.SetAt(strConfigName,NULL);
		}
	}
	CATCH_ALL (e)
	{
		// failed
		return FALSE;
	}
 	END_CATCH_ALL
	
	return TRUE;	// succeed
}


BOOL CBatchBldDlg::SaveBatchBldOpt(CArchive & archive)
{
	TRY
	{
		
		// Construct the list of configs to build
		int nItem, nConfigs;

		nConfigs = m_mapSelected.GetCount();
		POSITION pos;

		ASSERT(nConfigs != -1);
		archive << (-1); // flag new format using m_bSelectionStored
		archive << (int)m_bSelectionOnly;

		archive << (nConfigs);

		pos = m_mapSelected.GetStartPosition();

		for (pos = 	m_mapSelected.GetStartPosition (); pos != NULL;)
		{
			CString strConfigName;
			void *pv;
			m_mapSelected.GetNextAssoc ( pos, strConfigName, pv );

			archive << (strConfigName);
		}

	}
	CATCH_ALL (e)
	{
		// failed
		return FALSE;
	}
	END_CATCH_ALL

 	return TRUE;
}


CMapStringToPtr CProjectsDlg::m_mapSelectedProjects;

///////////
// CProjectsDlg IMPLEMENTATION
///////////
CProjectsDlg::CProjectsDlg ( CWnd * pParent /* = NULL */)
 			: C3dDialog (CProjectsDlg::IDD, pParent) 
{
	m_bExport = FALSE;
}

BEGIN_MESSAGE_MAP (CProjectsDlg, C3dDialog)
	//{{AFX_MSG_MAP (CProjectsDlg)
		ON_BN_CLICKED(IDC_SELECT_ALL, OnSelectAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP	()

///////////////////////////////
// BOOL CProjectsDlg::OnInitDialog()
///////////////////////////////
BOOL CProjectsDlg::OnInitDialog()
{
	CString strProject;
	int i;
	CPlatform * pPlatform;
	if (!m_bExport)
	{
		// this also works for write project
		CString strCaption;
		strCaption.LoadString(IDS_WRITE_PROJECTS);
		SetWindowText(strCaption);

		GetDlgItem(IDC_EXPORT_DEPS)->ShowWindow(SW_HIDE);
	}

	// Get proper workspace dir
	ASSERT(g_pProjWksIFace);
	LPCSTR pszPath;
	VERIFY(SUCCEEDED(g_pProjWksIFace->GetWorkspaceDocPathName(&pszPath)));
	CPath pathWkspc;
	ASSERT((pszPath!=NULL) && (*pszPath));
	VERIFY(pathWkspc.Create(pszPath));
	CDir dirWkspc;
	dirWkspc.CreateFromPath(pathWkspc);

	// Subclass the check listbox
	VERIFY(m_lbProjects.SubclassDlgItem(IDC_TARGET_LIST, this));
	m_lbProjects.SetRedraw(FALSE);

	// call base class OnInitDialog
	C3dDialog::OnInitDialog() ;

	m_lbProjects.ResetContent();
	// First, enumerate all possible projects put them in the list box
	CProject::InitProjectEnum();
	CProject * pProject;
	CProject * pCmpProject;
	CClientDC dc(&m_lbProjects);
	int width, maxwidth = 0;
	CString strMakePath, strProjectName;

	CPath MakPath;
	MakPath.SetAlwaysRelative();
	while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, TRUE)) != NULL)
	{
		// Only works for internal projects
		if( pProject->m_bProjIsExe )
		{
			ASSERT(0);
			continue;
		}

		strProjectName = strProject;
		MakPath = *pProject->GetFilePath();
		if (m_bExport)
			MakPath.ChangeExtension(_T(".mak"));

		MakPath.GetRelativeName(dirWkspc, strMakePath);
		strProjectName = strProject + _T("  (") + strMakePath + _T(')');

		i = m_lbProjects.AddString(strProjectName);
		if (i < 0)
		{
			ASSERT(0);
			continue;
		}
		pCmpProject = NULL;
		if ((m_mapSelectedProjects.IsEmpty()) || (m_mapSelectedProjects.Lookup(strProject, (void * &)pCmpProject)))
		{
			ASSERT(pCmpProject==NULL || pCmpProject==pProject);
			m_lbProjects.SetCheck(i, TRUE);

			// REVIEW: select also
			// m_lbProjects.SetSel(i, TRUE);
		}

		m_mapProjects.SetAt(strProjectName, (void *)pProject);
		width = dc.GetTextExtent(strProjectName, strProjectName.GetLength()).cx;
		maxwidth = __max(maxwidth, width);
	}
		
	// might need horz scrollbars
	m_lbProjects.SetHorizontalExtent(maxwidth + 14); // add width of checkbox

	m_lbProjects.SetRedraw(TRUE);

	return TRUE ;
}

////////////////////////////////////////////////////////////
// Button-clicked handlers

void CProjectsDlg::DoDataExchange(CDataExchange* pDX)
{
	C3dDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectsDlg)
	DDX_Check(pDX, IDC_EXPORT_DEPS, m_bExportDeps);
	//}}AFX_DATA_MAP
}

///////////////////////////////
// CProjectsDlg::OnOK()
///////////////////////////////
VOID CProjectsDlg::OnOK()
{
	GetProjects();
	if (m_mapSelectedProjects.IsEmpty())
	{
		MsgBox(Error, IDS_NO_PROJ_SEL);
		m_lbProjects.SetFocus();
		return;
	}
	C3dDialog::OnOK();
}

///////////////////////////////
// CProjectsDlg::OnSelectAll()
///////////////////////////////
VOID CProjectsDlg::OnSelectAll()
{
	m_lbProjects.SetSel(-1, TRUE);
	m_lbProjects.SetFocus();
}

///////////////////////////////
// CProjectsDlg::GetProjects()
///////////////////////////////
void CProjectsDlg::GetProjects()
{
	m_mapSelectedProjects.RemoveAll();  // review
	
	CProject * pProject;
	int nItem, nProjects = m_lbProjects.GetCount();
	CString strProjectName, strProject;

	for (nItem = 0; nItem < nProjects; nItem++)
	{
		m_lbProjects.GetText(nItem, strProjectName);
		if (!m_mapProjects.Lookup(strProjectName, (void * &) pProject))
		{
			ASSERT(0);
			continue;
		}
		strProject = pProject->GetTargetName();
		if (m_lbProjects.GetCheck(nItem))
		{	
			// update selection
			m_mapSelectedProjects.SetAt(strProject, pProject);
		}
		else
		{
			// make sure it is not selected
			m_mapSelectedProjects.RemoveKey(strProject);
		}
	}
}

///////////////////////////////
// CProjectsDlg::OnCancel()
///////////////////////////////
VOID CProjectsDlg::OnCancel()
{
	// Cancel, so no export is done

	C3dDialog::OnCancel();
}

BOOL CProjectsDlg::LoadProjectsSelOpt(CArchive & archive)
{
	TRY
	{

		// Construct the list of projects to export
		int nItem, nProjects;
		void * pv;

		archive >> nProjects;
		
		m_mapSelectedProjects.RemoveAll();
		for (nItem = 0; nItem < nProjects; nItem++)
		{
			CString strProject;

			archive >> strProject;
			m_mapSelectedProjects.SetAt(strProject,NULL);
		}

		CString strProject;
		CProject::InitProjectEnum();
		CProject * pProject;
		while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, TRUE)) != NULL)
		{
			if (m_mapSelectedProjects.Lookup(strProject, pv))
			{
				m_mapSelectedProjects.SetAt(strProject, pProject);
			}
		}
	}
	CATCH_ALL (e)
	{
		// failed
		return FALSE;
	}
 	END_CATCH_ALL
	
	return TRUE;	// succeed
}


BOOL CProjectsDlg::SaveProjectsSelOpt(CArchive & archive)
{
	TRY
	{
		
		// Construct the list of projects to export
		int nItem, nProjects;

		nProjects = m_mapSelectedProjects.GetCount();
		POSITION pos;

		archive << (nProjects);

		pos = m_mapSelectedProjects.GetStartPosition();

		while (pos != NULL)
		{
			CString strProject;
			void *pv;
			m_mapSelectedProjects.GetNextAssoc ( pos, strProject, pv );

			archive << (strProject);
		}

	}
	CATCH_ALL (e)
	{
		// failed
		return FALSE;
	}
	END_CATCH_ALL

 	return TRUE;
}
