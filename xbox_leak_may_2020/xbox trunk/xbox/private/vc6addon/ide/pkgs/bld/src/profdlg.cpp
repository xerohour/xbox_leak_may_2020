//
// Implementation for CProfDlg classes
//
// History:
// Date				Who			What
// 09/03/94			WinslowF	created
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"		// standard AFX include
#pragma hdrstop
#include "profdlg.h"
#include "resource.h"
#include "msgboxes.h"
// #include "utils.h"  // neither needed nor found bobz
#include "oleref.h"
#include "bldapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


UINT g_nidOWin = 0;
CConsoleSpawner *g_pSpawner = NULL;
LPOUTPUTWINDOW gpOutputWindow = NULL;
BOOL g_fProfiling = FALSE;
const char szProfFilter[] =	"Batch (*.bat;*.cmd)\0*.bat;*.cmd\0All Files (*.*)\0*.*\0\0";
int CProfileDlg::m_btnIndex = -1;
CString CProfileDlg::m_strComboCustom = _T("");
CString CProfileDlg::m_strEditAdvanced = _T("");


///////////////////////////////////////////////////////////////////////////////
// CProfileDlg implementation
///////////////////////////////////////////////////////////////////////////////
CProfileDlg::CProfileDlg(BOOL bExecute /*= FALSE*/, CWnd * pParent /*= NULL*/)
: C3dDialog(CProfileDlg::IDD, pParent),
  m_bExecute(bExecute)
{
	//{{AFX_DATA_INIT(CProfileDlg)
	m_strPrep    = "prep";
	m_strPlist   = "plist";
	m_strProfile = "profile";
	m_strExtPbi  = "pbi";
	m_strExtMap  = "map";
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CProfileDlg, C3dDialog)
	//{{AFX_MSG_MAP(CProfileDlg)
	ON_BN_CLICKED(IDC_PROF_FnTiming, OnPROFnTiming)
	ON_BN_CLICKED(IDC_PROF_Merge, OnPROFMerge)
	ON_BN_CLICKED(IDC_PROF_FnCover, OnPROFnCover)
	ON_BN_CLICKED(IDC_PROF_LnCover, OnPROFLnCover)
	ON_BN_CLICKED(IDC_PROF_Custom, OnPROFCustom)
	ON_BN_CLICKED(IDC_PROF_Browse, OnPROFBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// CProfileDlg::DoDataExchange
void CProfileDlg::DoDataExchange(CDataExchange* pDX)
{
	C3dDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProfileDlg)
	DDX_Radio(pDX, IDC_PROF_FnTiming, m_btnIndex);
	DDX_Text(pDX, IDS_PROF_Custom, m_strComboCustom);
	DDX_Text(pDX, IDC_PROF_Advanced, m_strEditAdvanced);
	//}}AFX_DATA_MAP
}

///////////////////////////////////////////////////////////////////////////////
// CProfileDlg::OnInitDialog
BOOL CProfileDlg::OnInitDialog()
{

	// call the base class
	C3dDialog::OnInitDialog();
	if (m_btnIndex == -1)
	{
		CButton* cb = (CButton *) GetDlgItem(IDC_PROF_FnTiming);
		cb->SetCheck(1);
	}
	switch (m_btnIndex)
	{
	case MERGE:
		OnPROFMerge();
		break;
	case CUSTOM:
		OnPROFCustom();
		break;
	default:
		break;
	}
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// CProfileDlg::OnOK
void CProfileDlg::OnOK()
{
	UpdateData(TRUE);

	CDialog::OnOK();

	SetupPathFileName();
	RunProfiler();
}

/////////////////////////////////////////////////////////////////////////////
// CProfileDlg commands

void CProfileDlg::OnPROFnTiming() 
{
	// TODO: Add your control notification handler code here
	CButton* cbAdvanced = (CButton *) GetDlgItem(IDC_PROF_Advanced);
	CButton* cbBrowse = (CButton *) GetDlgItem(IDC_PROF_Browse);
	CButton* cbCustom = (CButton *) GetDlgItem(IDS_PROF_Custom);
	if(!cbAdvanced->IsWindowEnabled())
		cbAdvanced->EnableWindow(TRUE);
	if(cbBrowse->IsWindowEnabled())
		cbBrowse->EnableWindow(FALSE);
	if(cbCustom->IsWindowEnabled())
		cbCustom->EnableWindow(FALSE);
}

void CProfileDlg::OnPROFnCover() 
{
	// TODO: Add your control notification handler code here
	CButton* cbAdvanced = (CButton *) GetDlgItem(IDC_PROF_Advanced);
	CButton* cbBrowse = (CButton *) GetDlgItem(IDC_PROF_Browse);
	CButton* cbCustom = (CButton *) GetDlgItem(IDS_PROF_Custom);
	if(!cbAdvanced->IsWindowEnabled())
		cbAdvanced->EnableWindow(TRUE);
	if(cbBrowse->IsWindowEnabled())
		cbBrowse->EnableWindow(FALSE);
	if(cbCustom->IsWindowEnabled())
		cbCustom->EnableWindow(FALSE);
}

void CProfileDlg::OnPROFLnCover() 
{
	// TODO: Add your control notification handler code here
	CButton* cbAdvanced = (CButton *) GetDlgItem(IDC_PROF_Advanced);
	CButton* cbBrowse = (CButton *) GetDlgItem(IDC_PROF_Browse);
	CButton* cbCustom = (CButton *) GetDlgItem(IDS_PROF_Custom);
	if(!cbAdvanced->IsWindowEnabled())
		cbAdvanced->EnableWindow(TRUE);
	if(cbBrowse->IsWindowEnabled())
		cbBrowse->EnableWindow(FALSE);
	if(cbCustom->IsWindowEnabled())
		cbCustom->EnableWindow(FALSE);
}

void CProfileDlg::OnPROFMerge() 
{
	// TODO: Add your control notification handler code here
	CButton* cbAdvanced = (CButton *) GetDlgItem(IDC_PROF_Advanced);
	CButton* cbBrowse = (CButton *) GetDlgItem(IDC_PROF_Browse);
	CButton* cbCustom = (CButton *) GetDlgItem(IDS_PROF_Custom);
	if(cbAdvanced->IsWindowEnabled())
		cbAdvanced->EnableWindow(FALSE);
	if(cbBrowse->IsWindowEnabled())
		cbBrowse->EnableWindow(FALSE);
	if(cbCustom->IsWindowEnabled())
		cbCustom->EnableWindow(FALSE);
}

void CProfileDlg::OnPROFCustom() 
{
	// TODO: Add your control notification handler code here
	CButton* cbAdvanced = (CButton *) GetDlgItem(IDC_PROF_Advanced);
	CButton* cbBrowse = (CButton *) GetDlgItem(IDC_PROF_Browse);
	CButton* cbCustom = (CButton *) GetDlgItem(IDS_PROF_Custom);
	if(cbAdvanced->IsWindowEnabled())
		cbAdvanced->EnableWindow(FALSE);
	if(!cbBrowse->IsWindowEnabled())
		cbBrowse->EnableWindow(TRUE);
	if(!cbCustom->IsWindowEnabled())
		cbCustom->EnableWindow(TRUE);

}

void CProfileDlg::OnPROFBrowse() 
{
	// TODO: Add your control notification handler code here
	CBrowseDlg fDlg;

	if(fDlg.DoModal() == IDOK)
	{
		CComboBox* cbBrowse = (CComboBox *) GetDlgItem(IDS_PROF_Custom);
		cbBrowse->SetCurSel(cbBrowse->AddString(fDlg.GetFileName()));
	}
}

////////////////////////////////////////////////////////////////////////////
//
//	CProfileDlg::GetDebuggeeCommandLine
//
//	PURPOSE:	Gets the command line
//
////////////////////////////////////////////////////////////////////////////
void CProfileDlg::GetDebuggeeCommandLine(PSTR CommandLine, UINT size)
{
	PSTR pszCommandLine;
	UINT NumToCopy;
	TCHAR szGetBuf[_MAX_PATH];
	CString str;
	COleRef<IBuildSystem> pBldSysIFace;
	theApp.FindInterface(IID_IBuildSystem, (void**) pBldSysIFace.InOut());
// crappy to use FindInterface on ourselves, but I'm least fixing the leak

	// get the command-line from the project
	if (pBldSysIFace != NULL && (pBldSysIFace->IsActiveBuilderValid() == S_OK))
	{
		pBldSysIFace->GetProgramRunArguments(ACTIVE_BUILDER, str);
		_tcscpy(szGetBuf, (LPCTSTR)str);
	}
	else
	{
		ASSERT(theApp.m_jit.GetActive());
		szGetBuf[0] = _T('\0');
	}

	pszCommandLine	= szGetBuf;

	// OK, copy the command line across
	*CommandLine = '\0';
	NumToCopy = min(_ftcslen(pszCommandLine)+1, size-1);
	_ftcsncpy(CommandLine, pszCommandLine, NumToCopy-1);
	CommandLine[NumToCopy-1] = '\0';
}


void CProfileDlg::SetupPathFileName()
{
	CHAR	szExe[_MAX_PATH], _szExe[_MAX_PATH + 2], _szDir[_MAX_DIR], szFName[_MAX_FNAME], szDrive[_MAX_DRIVE], szDir[_MAX_DIR], _szCmdLine[256];

	if (!g_VPROJIdeInterface.Initialize())
	{
		ASSERT(FALSE);
		return;
	}

	EXEFROM exefrom;
	VERIFY(SUCCEEDED(g_VPROJIdeInterface.GetEditDebugStatus()->GetExecutableFilename(szExe, sizeof(szExe), &exefrom)));

	
	//GetExecutableFilename(szExe, sizeof(szExe));

	_splitpath(szExe, szDrive, szDir, szFName, NULL);

	// surround _szExe with quotes, since that is how it is always used
	_szExe[0] = _T('\"');
	_makepath(&(_szExe[1]), szDrive, szDir, szFName, NULL);
	int cb = strlen(_szExe);
	_szExe[cb] = _T('\"');
	_szExe[cb + 1] = '\0';

	_makepath(_szDir, szDrive, szDir, NULL, NULL);

	GetDebuggeeCommandLine(_szCmdLine, sizeof(_szCmdLine));
	
	m_strExe = _szExe;
	m_strDir = _szDir;
	m_strCmdLine = _szCmdLine;
}

CString CProfileDlg::strArgPrep1()
{
	CString strArg = "/nologo ";
	switch (m_btnIndex)
	{
	case FNTIMING:
		strArg += "/om /ft";
		break;
	case FNSAMPLING:
		strArg += "/fs";
		break;
	case FNCOVERAGE:
		strArg += "/om /fv";
		break;
	case LNCOVERAGE:
		strArg += "/lv";
		break;
	default:
		ASSERT(FALSE);
	}

	return strArg + " " + m_strEditAdvanced + " " +  m_strExe;
}

CString CProfileDlg::strArgPrep2()
{
	CString strArg = "/nologo /m ";

	return strArg + m_strExe;
}

CString CProfileDlg::strArgProfile()
{
	CString strArg = "/nologo";

	switch (m_btnIndex)
	{
	case FNSAMPLING:
		strArg += " /s 10000 /nc ";
		break;
	default:
		strArg += " /nc ";
	}

	return	strArg + m_strExe + " " + m_strCmdLine;
}

CString CProfileDlg::strArgPlist()
{
	CString strArg = "/nologo ";

	return strArg + m_strExe;
}

CString CProfileDlg::strArgCustom()
{
	CString strArg = m_strExe;

    return strArg + " " + m_strCmdLine;
}


void CProfileDlg::RunProfiler()
{
	//	must init only once
	if ( (gpOutputWindow == NULL) && SUCCEEDED(theApp.FindInterface(IID_IOutputWindow, (LPVOID *)&gpOutputWindow)))
	{
		// we have access to an output window
		CString strName;
		strName.LoadString(OWIN_PROFILE);
		TCHAR* szName = (TCHAR*)(const TCHAR*)strName;
		gpOutputWindow->OutputWindowAddVwin(szName, FALSE, &g_nidOWin);
	}
	
	CString strCmd = "";
	CStringList slSpawnCmds;

	switch (m_btnIndex)
	{
	case FNTIMING: 
	case FNSAMPLING:
	case FNCOVERAGE:
	case LNCOVERAGE:
		strCmd =	m_strPrep + " " + strArgPrep1();
		slSpawnCmds.AddTail(strCmd);
		strCmd = m_strProfile + " " + strArgProfile();
		slSpawnCmds.AddTail(strCmd);
		strCmd = m_strPrep + " " + strArgPrep2();
		slSpawnCmds.AddTail(strCmd);
		strCmd	= m_strPlist + " " + strArgPlist();
		slSpawnCmds.AddTail(strCmd);

		break;
	case MERGE:
		strCmd =	m_strProfile + " " + strArgProfile();
		slSpawnCmds.AddTail(strCmd);
		strCmd =	m_strPrep + " " + strArgPrep2();
		slSpawnCmds.AddTail(strCmd);
		strCmd =	m_strPlist + " " + strArgPlist();
		slSpawnCmds.AddTail(strCmd);
		break;
	case CUSTOM:
		strCmd =	m_strComboCustom + " " + strArgCustom();
		slSpawnCmds.AddTail(strCmd);
		break;
	default:
		ASSERT(FALSE);
	}
	
	Spawn(&slSpawnCmds);

}

UINT CProfileDlg::Spawn(CStringList *slSpawnCmds)
{
	UINT nRet;
	
	// get the environment from the directories manager
	CEnvironmentVariableList bldenviron;
	CDirMgr * pDirMgr = GetDirMgr();
	CString str;
	pDirMgr->GetDirListString(str, DIRLIST_PATH);
	bldenviron.AddVariable ("path", str);
	bldenviron.SetVariables();				// set it

	g_fProfiling = TRUE;
	g_pSpawner = new CConsoleSpawner(&g_pSpawner);
	g_pSpawner->m_fCloseWindow = TRUE;
	g_pSpawner->m_fGUITool = FALSE;
	g_pSpawner->m_fReloadNoPrompt = TRUE;
	g_pSpawner->InitializeSpawn((LPTSTR)LPCTSTR(m_strDir), g_nidOWin, TRUE);
	g_pSpawner->m_hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	// m_pSpawner is self-deleting! no need to delete it!

	if(nRet = g_pSpawner->PerformSpawnMultiple(slSpawnCmds))
	{
		theApp.IdleAndPump(g_pSpawner->m_hNotifyEvent);
		if (WaitForSingleObject(g_pSpawner->m_hCancelEvent, 0) == WAIT_OBJECT_0)
		{
			CString strError;
			strError.LoadString(IDS_TERMINATE_USER_REQ);
			gpOutputWindow->WriteStringToOutputWindow(strError, FALSE, TRUE);
		}

	}
	bldenviron.ResetVariables();
	g_fProfiling = FALSE;

	if(g_pSpawner != NULL)
	{
		delete g_pSpawner;
		g_pSpawner = NULL;
	}
	return(nRet);
}


BOOL CProfileDlg::LoadProfOptStore(CArchive & archive)
{

	TRY
	{

		archive >> ((WORD &)m_btnIndex);
		archive >> m_strComboCustom;
		archive >> m_strEditAdvanced;

	}
	CATCH_ALL (e)
	{
		// failed
		return FALSE;
	}
 	END_CATCH_ALL

	return TRUE;	// succeed
}

BOOL CProfileDlg::SaveProfOptStore(CArchive & archive)
{
	TRY
	{
		
		archive << ((WORD)m_btnIndex);
		archive << (m_strComboCustom);
		archive << (m_strEditAdvanced);

	}
	CATCH_ALL (e)
	{
		// failed
		return FALSE;
	}
	END_CATCH_ALL

	return TRUE;	// succeed
}


BOOL FProfiling()
{
	return g_fProfiling;
}


////////////////////////////////////////////////////////////////////
// Browse dialog class
// CBrowseDlg
////////////////////////////////////////////////////////////////////


CBrowseDlg::CBrowseDlg() 
	: C3dFileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_SHOWHELP, NULL, 
		NULL, 0, 0)
{
	 m_szFileBuff[0] = _T('\0');
}

CBrowseDlg::~CBrowseDlg()
{
}

CString CBrowseDlg::GetFileName()
{
	CString strFileName = m_szFileBuff;
	return strFileName;
}

int CBrowseDlg::DoModal()
{
	CString	strTitle;

	strTitle.LoadString(IDS_PROF_BROWSETITLE);

	m_ofn.lpstrFilter = szProfFilter;
	m_ofn.nMaxCustFilter = sizeof(szProfFilter);
	m_ofn.lpstrTitle = strTitle;

	m_ofn.lpstrFile = m_szFileBuff;
	m_ofn.nMaxFile = MAX_PATH-1;
	m_ofn.lpstrFileTitle = NULL;
	m_ofn.nMaxFileTitle = 0;	 
	m_ofn.nFileOffset = 0;
	m_ofn.nFileExtension = 0;
	m_ofn.lpstrDefExt = NULL;

	return C3dFileDialog::DoModal(); 
}

