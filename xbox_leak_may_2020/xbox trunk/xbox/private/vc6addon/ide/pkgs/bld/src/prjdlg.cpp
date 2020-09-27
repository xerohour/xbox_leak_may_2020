// prjdlg.cpp : implementation file
//

#include "stdafx.h"
#include "bldiface.h"
#include "awiface.h"
#include <dlgbase.h>
#include "prjdlg.h"
#include "prjconfg.h"
#include "resource.h"
#include "exttarg.h"
#include "prjguid.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static int 	g_nLastPT = -1;

// Defined in bldiface.cpp
CAppWizIFace* GetAppWizIFace();

extern CString g_strTarg;

#define MAX_DISP  62


// This is the registry key under which we remember which platforms were last checked
//  by the user.  By using the registry, we always remember the user's "favorite"
//  platforms from session to session.
// COLINT: Since this is no longer in AppWizard, you may want to change the key's name.
static const char szDefPltSection[] = "AppWizard\\Default Platforms";


/////////////////////////////////////////////////////////////////////////////
// Japanese-specific functions

// This tests whether we're on a Japanese system
inline BOOL IsJapaneseSystem(void)
{
	return (PRIMARYLANGID(GetSystemDefaultLangID()) == LANG_JAPANESE);
}

// This function assumes we're on a Japanese system
BOOL IsSBKatakana(unsigned char c)
{
	return c >= 0xa1 && c <= 0xdf;
}


/////////////////////////////////////////////////////////////////////////////
// Configuration name validation
//
// A valid config name is at most <filename> characters according to the 
// file system, and contains only alphanumeric, underscore, single-byte
// katakana, or double-byte characters.

BOOL IsValidConfigName(LPCTSTR psz)
{
	if (*psz == _T('\0') || *psz == _T('.'))
		return FALSE;
	
	int cchLeft = 8;                // 8 for file name
	DWORD dwMaxLen = 8;
	DWORD  dwDummy1;
	if (::GetVolumeInformation(NULL, NULL, 0, NULL, &dwMaxLen,
		&dwDummy1, NULL, 0))
	{
		// succesfully got info from file system -- use it.
		cchLeft = (int)dwMaxLen;
	}
	
	TCHAR ch;
	while ((ch = *psz) != _T('\0'))
	{
		if (_istalnum(ch) || ch == _T('_') || ch == _T(' ') || IsDBCSLeadByte(ch)
			|| (IsJapaneseSystem() && IsSBKatakana(ch)) || ch < 0)
		{
			// count the character
			cchLeft -= _tclen(psz);
			// leave 8 for file name (Win95J is one character too short for MAX_LENGTH, so adjust it to 9)
			if (cchLeft < 9)
				return FALSE;		// too long
		}
		else
		{
			// illegal
			return FALSE;
		}
		psz = _tcsinc(psz);
	}
	return TRUE;
}

void DDV_ProjectName(CDataExchange* pDX, const CString & strProjName)
{
	if (!pDX->m_bSaveAndValidate)
		return;
	if (!IsValidConfigName(strProjName))
	{
		AfxMessageBox( IDS_INVALID_CONFIGNAME, MB_OK, 0);
		pDX->Fail();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNewProjectCopyDlg dialog

CNewProjectCopyDlg::CNewProjectCopyDlg(LPCTSTR pszProject, CWnd* pParent /*=NULL*/)
	: C3dDialog(CNewProjectCopyDlg::IDD, pParent)
{
	m_strProject = pszProject;
	m_bTrackBasedOn = TRUE;
}

CNewProjectCopyDlg::~CNewProjectCopyDlg()
{
	// Free the target list
	POSITION pos = m_ProjectTargets.GetHeadPosition();
	CProjectTargetRec *pTarget;
	while (pos)
	{
		pTarget = (CProjectTargetRec *)m_ProjectTargets.GetNext( pos );
		ASSERT_VALID( pTarget );

		delete pTarget;
	}
	m_ProjectTargets.RemoveAll();
}

void CNewProjectCopyDlg::DoDataExchange(CDataExchange* pDX)
{
	C3dDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewProjectCopyDlg)
		DDX_Control(pDX, IDC_TARGET_LISTCOMBO, m_TargetList);
		DDX_Control(pDX, IDC_TARGET_PLATFORM, m_Platforms);
		DDX_Text(pDX, IDC_PROJ_NAME, m_strProjName);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDV_ProjectName(pDX, m_strProjName);
}

BEGIN_MESSAGE_MAP(CNewProjectCopyDlg, C3dDialog)
	//{{AFX_MSG_MAP(CNewProjectCopyDlg)
		ON_LBN_SELCHANGE(IDC_TARGET_LISTCOMBO, OnSelchangeCopyFrom)
		ON_LBN_SELCHANGE(IDC_TARGET_PLATFORM, OnSelchangePlatform)
		ON_EN_CHANGE(IDC_PROJ_NAME, OnChangeProjName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewProjectCopyDlg message handlers

BOOL CNewProjectCopyDlg::OnInitDialog()
{
	m_pTarget = NULL;

	C3dDialog::OnInitDialog();

	m_pProject = g_pActiveProject;
	ASSERT (m_pProject != NULL);

	m_TargetList.ResetContent();

	CString	strCurrentConfigName;
	HFILESET hFileset = NULL;
	if (m_strProject.IsEmpty())
	{
		m_pProject->GetStrProp(P_ProjActiveConfiguration, strCurrentConfigName);
	}
	else
	{
		hFileset = g_BldSysIFace.GetFileSetFromTargetName(m_strProject, ACTIVE_BUILDER);
		// make sure we have the right project for this fileset!
		m_pProject = (CProject *)g_BldSysIFace.GetBuilderFromFileSet(hFileset);
		g_BldSysIFace.GetTargetNameFromFileSet(hFileset, strCurrentConfigName, (HBUILDER)m_pProject);
	}

	// Make sure we are in the required config
	CProjTempConfigChange projTempConfigChange(m_pProject);
	projTempConfigChange.ChangeConfig(strCurrentConfigName);
	CProjectTargetRec *pTarget;
	CProjType * pprojtype;

	// UNDONE: allow configs from other projects!
	const CPtrArray * pCfgArray;
	if (hFileset != NULL)
		pCfgArray = g_BldSysIFace.CnvHFileSet(NO_BUILDER, hFileset)->GetConfigArray();
	else
		pCfgArray = m_pProject->GetConfigArray();
	int cCfgs = pCfgArray->GetSize();
	for (int i = 0; i < cCfgs; i++)
	{
		ConfigurationRecord *pConfiguration = (ConfigurationRecord *)pCfgArray->GetAt(i);
		ASSERT_VALID(pConfiguration);

		// Is it valid?
		if (hFileset != NULL && !pConfiguration->IsValid())
			continue;

		// This target name.
		CString strConfigName = pConfiguration->GetConfigurationName();

		// Create a new CProjectTargetRec
		pTarget = new CProjectTargetRec;
		pTarget->m_strName = strConfigName;

		VERIFY(g_prjcompmgr.LookupProjTypeByName(LPCTSTR(pConfiguration->GetOriginalTypeName()), pprojtype));
		pTarget->m_strType = *pprojtype->GetTypeUIDescription();
		pTarget->m_strUIType = *pprojtype->GetUIDescription();
		pTarget->m_strPlatform = *pprojtype->GetPlatformUIDescription();
		pTarget->m_bDebug = IsDebug(pConfiguration);
		pTarget->m_bSupported = pprojtype->IsSupported();

		// Add it to the target list
		m_ProjectTargets.AddTail(pTarget);

		// Add it to the list box
		int iEntry = m_TargetList.AddString(pTarget->m_strName);
		ASSERT(iEntry != LB_ERR);
		m_TargetList.SetItemDataPtr(iEntry, (void *) pTarget);
	}

	int iCurSel = m_TargetList.FindStringExact(-1, strCurrentConfigName);
	if (iCurSel == LB_ERR && m_TargetList.GetCount() > 0)
		iCurSel = 0;	// select first if current not found

	if (iCurSel != LB_ERR)
	{
		// set the project type name 
		m_TargetList.SetCurSel(iCurSel);

		m_bTrackBasedOn = TRUE;
		OnSelchangeCopyFrom();
		((CEdit *)GetDlgItem(IDC_PROJ_NAME))->SetSel(0, -1);

		m_pTarget = (CProjectTargetRec *)m_TargetList.GetItemDataPtr(iCurSel);
	}

	UpdateTargetNameText();

	return FALSE;
}

void CNewProjectCopyDlg::OnOK()
{
	if (!UpdateData(TRUE))
		return;
	if (!AddSelectedTarget())
	{
		CProjType * pProjType;
		VERIFY(g_prjcompmgr.LookupProjTypeByUIDescription(m_pTarget->m_strUIType, pProjType));
		ASSERT_VALID(pProjType);
		if (pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)))
		{
			AfxMessageBox(IDS_CANNOT_ADD_EXTCONFIG, MB_OK | MB_ICONEXCLAMATION);
		}
		return;
	}

	C3dDialog::OnOK();
}

BOOL CNewProjectCopyDlg::AddSelectedTarget()
{
	TrgCreateOp trgop = TrgMirror; SettingOp setop = SettingsDefault;
	HBLDTARGET hTarg = NO_TARGET, hSettingsTarg = NO_TARGET;
	HBUILDER hBld = g_BldSysIFace.GetActiveBuilder(); // default

	ASSERT_VALID (m_pTarget);

	// target to copy
	hTarg = g_BldSysIFace.GetTarget(m_pTarget->m_strName, hBld);
	hBld = g_BldSysIFace.GetBuilder(hTarg); // don't assume it's the active project

	int iUseMFC;
	g_BldSysIFace.GetTargetProp(hTarg, Prop_UseOfMFC, iUseMFC);

	// copy settings?
	hSettingsTarg = hTarg;
	setop = SettingsClone;

	// get our platform and target type name
	CProjType * pProjType;
	VERIFY(g_prjcompmgr.LookupProjTypeByUIDescription(m_pTarget->m_strUIType, pProjType));
	ASSERT_VALID(pProjType);

	CString strUIPlatformName = GetPlatformName();
	CString strTargetName = ConstructTargetName(strUIPlatformName);

	g_strTarg = (g_BldSysIFace.CnvHTarget(hBld, hTarg))->GetTargDir();

	// Make sure this name doen't already exist
	HBUILDER hTBld;
	CString strProject, strTarget;
	g_BldSysIFace.InitBuilderEnum();
	while ((hTBld = g_BldSysIFace.GetNextBuilder(strProject)) != NO_BUILDER)
	{
		// Prepare to enumerate the targets
		g_BldSysIFace.InitTargetEnum(hTBld);

		// Enumerate all the targets
		HBLDTARGET hTarget = g_BldSysIFace.GetNextTarget(strTarget, hTBld);
		while (hTarget != NO_TARGET)
		{
			if (strTarget.CompareNoCase(strTargetName) == 0)
			{
				// target already exist
				AfxMessageBox(IDS_PROJNAME_EXISTS, MB_OK | MB_ICONEXCLAMATION);
				return FALSE;
			}
			hTarget = g_BldSysIFace.GetNextTarget(strTarget, hTBld);
		}
	}

	// perform this operation using the build interface
	return g_BldSysIFace.AddTarget
	(
		// name of this target
		(LPCTSTR)strTargetName,

		(LPCTSTR)strUIPlatformName,						// UI name of this target's platform
		(const TCHAR *)*pProjType->GetTypeUIDescription(),	// UI name of this target type
		TRUE,										// we're using 'UI' names.

		m_pTarget->m_bDebug,						// debug or release settings? default=debug

		trgop,										// clone or mirror another target? default=no
		hTarg,										// target to clone or mirror

		setop,										// copy or default settings?
		hSettingsTarg,								// target to copy settings from
		 
		FALSE,										// show any warnings/errors during creation? default=yes
		TRUE,										// create a new set of output directories
		(iUseMFC != NoUseMFC),						// using MFC?
		hBld										// add to current builder
	) != NO_TARGET;

}

void CNewProjectCopyDlg::OnSelchangeCopyFrom()
{
	// change the project's active configuration 
	int	iCurSel = m_TargetList.GetCurSel();

	if (iCurSel == LB_ERR)
		return;

	// setting the window text
	m_pTarget = (CProjectTargetRec *)m_TargetList.GetItemDataPtr(iCurSel);
	ASSERT_VALID (m_pTarget);

	const CStringList* pPlatformList = g_prjcompmgr.GetListOfPlatforms(m_pTarget->m_strType, TRUE);
	CString strUI;

	// Clear the platforms listbox and add in the supported platform
	// names.
	m_Platforms.SetRedraw(FALSE);
	m_Platforms.ResetContent();
	POSITION pos = pPlatformList->GetHeadPosition();
	while (pos != NULL)
	{
		strUI = pPlatformList->GetNext(pos);
		int iEntry = m_Platforms.AddString(strUI);
		ASSERT(iEntry != LB_ERR);
		CPlatform * pPlatform;
		g_prjcompmgr.LookupPlatformByUIDescription(strUI, pPlatform);
		m_Platforms.SetItemDataPtr(iEntry, (void *)pPlatform );
	}

	m_Platforms.SetRedraw(TRUE);

	// Try to select the platform that was originally selected if it is
	// still available. Otherwise we select the first platform.
	if (m_Platforms.SelectString(-1, m_pTarget->m_strPlatform) == LB_ERR)
		m_Platforms.SetCurSel(0);	

	if (m_bTrackBasedOn)
	{
		VERIFY(g_BldSysIFace.GetFlavourFromConfigName(m_pTarget->m_strName, m_strProjName));
 		SetDlgItemText(IDC_PROJ_NAME, m_strProjName);
		m_bTrackBasedOn = TRUE;
	}

	UpdateTargetNameText();
}

void CNewProjectCopyDlg::OnChangeProjName()
{
	m_bTrackBasedOn = FALSE;

	GetDlgItemText(IDC_PROJ_NAME, m_strProjName);
	UpdateTargetNameText();
}

void CNewProjectCopyDlg::OnSelchangePlatform()
{
	UpdateTargetNameText();
}

void CNewProjectCopyDlg::UpdateTargetNameText()
{
	if (m_pTarget == NULL)
		return;

	SetDlgItemText(IDC_STATIC_CONFIG_NAME, 
		ConstructTargetName(GetPlatformName()));
}

CString CNewProjectCopyDlg::GetPlatformName ()
{
	ASSERT_VALID (m_pTarget);

	CString strPlatform;
	int iCurSel = m_Platforms.GetCurSel();	
	if (iCurSel != LB_ERR)
	{
		m_Platforms.GetWindowText(strPlatform);
		CPlatform * pPlatform = (CPlatform *)m_Platforms.GetItemDataPtr(iCurSel);
		strPlatform = *pPlatform->GetUIName();
	}
	else
		strPlatform = m_pTarget->m_strPlatform;

	return strPlatform;
}

CString CNewProjectCopyDlg::ConstructTargetName(const CString &strPlatform)
{
	ASSERT_VALID(m_pTarget);

	CString strName;
	if (!IsValidConfigName(m_strProjName))
	{
		strName.LoadString(IDS_INVALID);
		return strName;
	}
	int ich = m_pTarget->m_strName.Find('-');
	if (ich != -1)
		strName = m_pTarget->m_strName.Left(ich);
	else
		strName = m_pTarget->m_strName;

	return (strName + _TEXT("- ") + strPlatform + _TEXT(" ") + m_strProjName);
}

BOOL CNewProjectCopyDlg::IsDebug(ConfigurationRecord *pConfig)
{
	int bDebug;

	m_pProject->SetManualBagSearchConfig(pConfig);
	m_pProject->GetIntProp(P_UseDebugLibs, bDebug);
	m_pProject->ResetManualBagSearchConfig();

	return bDebug != 0;
}
