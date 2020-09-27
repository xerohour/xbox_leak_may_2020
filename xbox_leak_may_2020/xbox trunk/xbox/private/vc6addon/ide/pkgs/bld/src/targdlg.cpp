//
// Implementation for CTargetDlg class
//
// History:
// Date				Who					What
// 12/14/93			ignatius			created
//////////////////////////////////////////////////////////////////

#include "stdafx.h"		// standard AFX include
#pragma hdrstop
#include "targdlg.h"
#include "resource.h"
#include "msgboxes.h"
#include "projitem.h"
#include "exttarg.h"
#include "targctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

class CProjectTarget : public CObject
{
// Attributes:
public:
	CString m_strName;
	CString m_strType;
	BOOL m_bDebug;
	BOOL m_bDeleted;
	BOOL m_bSupported;
	CProjectTarget *m_pCloneFrom;
	CProjectTarget *m_pCopyFilesFrom;
	CProjectTarget *m_pMirrorFilesFrom;
};

///////////
// CBaseTargetDlg IMPLEMENTATION
///////////
CBaseTargetDlg::~CBaseTargetDlg()
{
	// Free the target list
	POSITION pos = m_ProjectTargets.GetHeadPosition();
	CProjectTarget *pTarget;
	while (pos)
	{
		pTarget = (CProjectTarget *)m_ProjectTargets.GetNext( pos );
		ASSERT_VALID( pTarget );

		delete pTarget;
	}
	m_ProjectTargets.RemoveAll();
}

////////////////////////////////
// CBaseTargetDlg::DoDataExchange()
/////////////////////////////////
void CBaseTargetDlg::DoDataExchange(CDataExchange* pDX)
{
	C3dDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBaseTargetDlg)
		DDX_Control(pDX, IDC_TARGET_LIST, m_TargetList);
		DDX_Control(pDX, IDC_PLATFORM_TYPE, m_Platform);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP (CBaseTargetDlg, C3dDialog)
	//{{AFX_MSG_MAP (CBaseTargetDlg)
	ON_LBN_SELCHANGE(IDC_TARGET_LIST, UpdateTypeDisplay)
	ON_BN_CLICKED(IDOK, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP	()

///////////////////////////////
// BOOL CBaseTargetDlg::DoModal()
///////////////////////////////
int CBaseTargetDlg::DoModal(CString * pstrConfigName /* = NULL */)
{
	m_pProject = g_pActiveProject;
	ASSERT (m_pProject != NULL);

	CString	strCurrentConfigName;
	
	if (!pstrConfigName)
		// get the project's current configuration
		m_pProject->GetStrProp(P_ProjActiveConfiguration, strCurrentConfigName);
	else
		strCurrentConfigName = *pstrConfigName;

	// Make sure we are in the required config
	CProjTempConfigChange projTempConfigChange(m_pProject);
	projTempConfigChange.ChangeConfig(strCurrentConfigName);

	CString strProject;
	CProject * pProject = NULL;
	CProject::InitProjectEnum();
	while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, TRUE)) != NULL)
	{
		const CPtrArray * pCfgArray = pProject->GetConfigArray();
		int cCfgs = pCfgArray->GetSize();
		for (int i=0; i < cCfgs; i++)
		{
			ConfigurationRecord *pConfiguration = (ConfigurationRecord *)pCfgArray->GetAt(i);
			ASSERT_VALID(pConfiguration);
			if( !pConfiguration->IsBuildable() )
				continue;	// ignore this one, get next
			
			// This target name.
			CString strConfigName = pConfiguration->GetConfigurationName();

			if (!ShowTarget(strConfigName))
				continue;	// ignore this one, get next

			m_lstAvailableTargets.AddTail((void *)pConfiguration);
		}
	}

	if (m_lstAvailableTargets.IsEmpty())
		return IDABORT;

	return C3dDialog::DoModal();
}

///////////////////////////////
// BOOL CBaseTargetDlg::ShowTarget()
///////////////////////////////
BOOL CBaseTargetDlg::ShowTarget(LPCTSTR lpszName)
{
	// Filter out any required targets
	if (m_plstFilterTargs != NULL && m_plstFilterTargs->Find(lpszName))
		return FALSE;

	return TRUE;
}

///////////////////////////////
// BOOL CBaseTargetDlg::OnInitDialog()
///////////////////////////////
BOOL CBaseTargetDlg::OnInitDialog()
{
	CString	strCurrentConfigName;
	CFont	*pFont = GetStdFont(font_Normal); // get the thin font
	int iCurSel;

	if( m_pProject == NULL || m_pProject->IsExeProject() )
		return FALSE;

	// call base class OnInitDialog
	C3dDialog::OnInitDialog();

	CProjType * pprojtype;
 
 	// initialization
	m_Platform.SetFont (pFont) ;
	// get the project's current configuration
	m_pProject->GetStrProp(P_ProjActiveConfiguration, strCurrentConfigName);

	// Build up the project target list
	ASSERT(m_ProjectTargets.IsEmpty());
	m_TargetList.ResetContent();

	CProjectTarget *pTarget;
	CClientDC dc(&m_TargetList);
	int width, maxwidth = 0;
	POSITION pos = m_lstAvailableTargets.GetHeadPosition();
	while (pos != NULL)
	{
   		ConfigurationRecord *pConfiguration = (ConfigurationRecord *) m_lstAvailableTargets.GetNext(pos);
		ASSERT_VALID(pConfiguration);

		// This target name.
		CString strConfigName = pConfiguration->GetConfigurationName();

		// Create a new CProjectTarget
		pTarget = new CProjectTarget;
		pTarget->m_strName = strConfigName;

		VERIFY(g_prjcompmgr.LookupProjTypeByName(LPCTSTR(pConfiguration->GetOriginalTypeName()), pprojtype));
		pTarget->m_strType = *pprojtype->GetUIDescription();
		pTarget->m_bDebug = IsDebug(pConfiguration);
		pTarget->m_pCloneFrom = NULL;
		pTarget->m_pCopyFilesFrom = NULL;
		pTarget->m_pMirrorFilesFrom = NULL;
		pTarget->m_bDeleted = FALSE;
		pTarget->m_bSupported = pprojtype->IsSupported();

		// Add it to the target list
		m_ProjectTargets.AddTail(pTarget);

		// Add it to the list box
		int iEntry = m_TargetList.AddString(pTarget->m_strName);
		ASSERT(iEntry != LB_ERR);
		if (iEntry != LB_ERR)
		{
			width = dc.GetTextExtent(pTarget->m_strName, pTarget->m_strName.GetLength()).cx;
			maxwidth = __max(maxwidth, width);
			m_TargetList.SetItemDataPtr(iEntry, (void *) pTarget);
		}
	}

	m_TargetList.SetHorizontalExtent(maxwidth);  // might need horz scrollbars

	iCurSel = m_TargetList.FindStringExact (-1, (const TCHAR *)strCurrentConfigName);
	if (iCurSel == LB_ERR && m_TargetList.GetCount())
		iCurSel = 0;	// select first if current not found

	if (iCurSel != LB_ERR)
	{
		// set the project type name 
		m_TargetList.SetCurSel(iCurSel);

		UpdateTypeDisplay();
	}

	return TRUE;
}

//////////////////////////////////////
// CBaseTargetDlg::OnOK()
///////////////////////////////////////
void CBaseTargetDlg::OnOK()
{
	C3dDialog::OnOK();
}

//////////////////////////////////////
// CBaseTargetDlg::UpdateTypeDisplay()
///////////////////////////////////////
void CBaseTargetDlg::UpdateTypeDisplay()
{
	int	iCurSel = m_TargetList.GetCurSel();

	// change the project's active configuration 
	if (iCurSel != LB_ERR)
	{
		// setting the window text
		m_pTarget = (CProjectTarget *)m_TargetList.GetItemDataPtr(iCurSel);
		ASSERT_VALID (m_pTarget);

		CString strAppend;
		// do we want to append the text '(Not Supported)' for unknown targets?
		if (!m_pTarget->m_bSupported)
			VERIFY(strAppend.LoadString(IDS_UNSUPPORTED));

		m_Platform.SetWindowText(m_pTarget-> m_strType + strAppend);
	}
}

//////////////////////////////////////
// CBaseTargetDlg::TargetFound()
///////////////////////////////////////
BOOL CBaseTargetDlg::TargetFound(LPCTSTR lpszName)
{
	POSITION pos = m_ProjectTargets.GetHeadPosition();
	CProjectTarget *pTarget;

	while (pos)
	{
		pTarget = (CProjectTarget *)m_ProjectTargets.GetNext(pos);
		if (!pTarget->m_strName.CompareNoCase(lpszName) && !pTarget->m_bDeleted)
			return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////
// CBaseTargetDlg::IsDebug()
///////////////////////////////////////
BOOL CBaseTargetDlg::IsDebug(ConfigurationRecord *pConfig)
{
	int bDebug;

	CProject * pProject = (CProject *)(pConfig->m_pBaseRecord->m_pOwner);
	pProject->SetManualBagSearchConfig(pConfig);
	pProject->GetIntProp(P_UseDebugLibs, bDebug);
	pProject->ResetManualBagSearchConfig();

	return bDebug != 0;
}

///////////
// CSelectTargetDlg IMPLEMENTATION
///////////
CSelectTargetDlg::~CSelectTargetDlg()
{
}

//////////////////////////////////////
// CSelectTargetDlg::OnInitDialog()
///////////////////////////////////////
BOOL CSelectTargetDlg::OnInitDialog()
{
	// do the base-class thing
	if (!CBaseTargetDlg::OnInitDialog())
		return FALSE;

	return TRUE;
}

BEGIN_MESSAGE_MAP (CSelectTargetDlg, CBaseTargetDlg)
	//{{AFX_MSG_MAP (CSelectTargetDlg)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_LBN_DBLCLK(IDC_TARGET_LIST, OnOK)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP	()

//////////////////////////////////////
// CSelectTargetDlg::OnOK()
///////////////////////////////////////
void CSelectTargetDlg::OnOK()
{
	if( m_pTarget == NULL )
		return;

	// remember the name of the last selected target
	m_strSelectedTarget = m_pTarget->m_strName;

	// do the base-class thing
	CBaseTargetDlg::OnOK();
}

void CSelectTargetDlg::OnCancel()
{
	// no recently selected target
	m_strSelectedTarget = _TEXT("");

	// do the base-class thing
	CBaseTargetDlg::OnCancel();
}

BOOL CSelectTargetDlg::ShowTarget(LPCTSTR lpszName)
{
	// Do the base class thing
	BOOL bShowTarget = CBaseTargetDlg::ShowTarget(lpszName);

	return bShowTarget;
}

