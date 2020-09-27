// helpdirs.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "helpdirs.h"

#include <io.h>		 // For _access

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// To compile this file, you must provide implementations of these two
// functions:
//
//  (1) CString GetRegString(const char* szKey, const char* szValue)
//
//      which is a function that does a registry lookup.  Given the
//      key and the value, it returns the requested registry information.
//      If the information cannot be found for any reason, it returns an
//      empty string.
//
//  (2) void WriteRegString(const char* szKey, const char* szValue, const char* sz)
//
//      which is a function that writes the specified sz string into the
//      registry.
//
// In addition, your .RC file (and associated resource.h file) must contain
// the following three items:
//
//  (1) the IDD_NOHELPFILE dialog template, copied from SUSHI.RC
//
//  (2) the IDS_ERROR_HELP_NOFILE string, which reads "Cannot access help
//      file '%s'.  Please correct the help directory locations shown below."
//
//  (3) the IDS_HELP_DEFAULTDIR string, which reads "\HELP"


/////////////////////////////////////////////////////////////////////////////
// Registry key strings for Help information
//
static TCHAR BASED_CODE szHelpKey[] = _T("Help");
static TCHAR BASED_CODE szLocalHelp[] = _T("LocalHelp");
static TCHAR BASED_CODE szRemoteHelp[] = _T("RemoteHelp");
static TCHAR BASED_CODE szNotFound[] = _T("Not found");


/////////////////////////////////////////////////////////////////////////////
// LocateHelpFile
//
// This function is great.  You tell it what file you are looking for.  It
// finds it, and gives you the fully qualified path.  If it can't find the
// file, the user will be prompted with a nifty dialog -- but that's all
// invisible to the caller.
//
// If bQueryUser is TRUE, there are only two possible resolutions when you
// call this function: we either found the file, OR the user cancelled out
// of the dialog which asked them to locate the file.  So if this function
// returns FALSE, you don't need to raise an error message.
//
// If bQueryUser is FALSE, this function will return FALSE if the file cannot
// be found without asking the user for a location.  So, bQueryUser should
// only be FALSE if you do not want to bother the user, and a failure to find
// the file is OK.
//
// strFName:    the name of the help file you need.
// strFullPath: if the function returns TRUE, this string will contain the
//              full path of the file you requested.
// bQueryUser:  TRUE->Query use for help directories if needed.
//				FALSE->Fail rather than ask user.
//
// return value: TRUE if the file is found, FALSE if the user cancels.
//
BOOL LocateHelpFile(const CString& strFName, CString& strFullPath, BOOL bQueryUser)
{
	int iKey = 1;
	int nDirsFound = 0;
	const char* szHelpPrefix = szLocalHelp;

	// Look for the file in all local and remote help directories.
	while (1)
	{
		CString strDir = GetRegString(szHelpKey, AppendIntegerToString(iKey++, szHelpPrefix), szNotFound);
		if (0 == strDir.Compare(szNotFound))
		{
			// Have we run out of places to look?
			if (szHelpPrefix == szRemoteHelp)
				break;

			// no more local values in the registry, try the remote values
			szHelpPrefix = szRemoteHelp;
			iKey = 1;
			continue;
		}

		nDirsFound++;

		if (IsFileThere(strDir, strFName, strFullPath))
			return TRUE;
	}

	// If there were no legal entries, the registry was probably nuked.
	// Try to regenerate some reasonable defaults, and try again.
	if (nDirsFound == 0)
	{
		GenerateDefaultHelpDirectories();
		return LocateHelpFile(strFName, strFullPath, bQueryUser);
	}

	// If we had some places to look, but still couldn't find the file,
	// we need the user's help.  Bring up the Help Directories dialog.
	if (bQueryUser && HelpDirectoriesDialog(strFName))
		return LocateHelpFile(strFName, strFullPath, TRUE);

	// We only get here if the user cancels out of the Help Dirs dialog.
	return FALSE;
}

// Utility functions
//
BOOL IsFileThere(const CString& strDir, const CString& strFName, CString& strFullPath)
{
	ASSERT(!strFName.IsEmpty());

	const TCHAR *pch = strFName;
	if ((strFName[0] != _T('\\')) &&
	    ((strFName.GetLength() < 2) || (*_tcsinc(pch) != _T(':'))))
	{
		if (strDir.IsEmpty())
			return FALSE;

		strFullPath = strDir;

		// Add a backslash between path and fname if needed
		if (strFullPath.Right(1).Compare("\\"))
			strFullPath += "\\";
	}

	strFullPath += strFName;

	if (_access(strFullPath, 04) == 0)	// check for read privs
		return TRUE;

	return FALSE;
}

CString AppendIntegerToString(int i, const TCHAR* szInput)
{
	ASSERT(i > 0);
	ASSERT(i < 100);	// if more than two digits, increase buffer size

	CString str;
	TCHAR* sz = str.GetBufferSetLength(strlen(szInput) + 6);

	wsprintf(sz, "%s%d", szInput, i);

	str.ReleaseBuffer();
	return str;
}

BOOL HelpDirectoriesDialog(const CString& strFile)
{
	CHelpDirDlg dlg;

	// Read the local and remote paths from the registry
	dlg.m_strPath1 = GetRegString(szHelpKey, AppendIntegerToString(1, szLocalHelp));
	dlg.m_strPath2 = GetRegString(szHelpKey, AppendIntegerToString(1, szRemoteHelp));

	// Generate the dialog's error message
	CString strError;
	strError.LoadString(IDS_ERROR_HELP_NOFILE);
	wsprintf(dlg.m_strMessage.GetBufferSetLength(strError.GetLength() + strFile.GetLength() + 2), strError, strFile);
	dlg.m_strMessage.ReleaseBuffer();

	if (dlg.DoModal() != IDOK)
		return FALSE;	// return FALSE if user cancels dialog

	// Unless the user cancelled the dialog, write the new information
	// into the registry.
	WriteRegString(szHelpKey, AppendIntegerToString(1, szLocalHelp), dlg.m_strPath1);
	WriteRegString(szHelpKey, AppendIntegerToString(1, szRemoteHelp), dlg.m_strPath2);

	return TRUE;
}

void GenerateDefaultHelpDirectories()
{
	// Our best-guess default is a "help" directory which is at the same
	// level in the directory hierarchy as the current executable.
	CString strLocal;
	::GetModuleFileName(NULL, strLocal.GetBufferSetLength(_MAX_PATH), _MAX_PATH);
	strLocal.ReleaseBuffer();

	// Convert C:\FOO\BIN\MSVC.EXE to C:\FOO\HELP.
	int i = strLocal.ReverseFind('\\');
	ASSERT(i != -1);
	strLocal.GetBufferSetLength(i);
	strLocal.ReleaseBuffer();

	// Now we have C:\FOO\BIN
	i = strLocal.ReverseFind('\\');
	if (i != -1)
	{
		strLocal.GetBufferSetLength(i);
		strLocal.ReleaseBuffer();
	}

	// Now we have C:\FOO (or just "C:", if the exe was in the root).
	CString strHelp;
	strHelp.LoadString(IDS_HELP_DEFAULTDIR);	// This string is "\HELP"
	strLocal += strHelp;

	// It's essential that one of these strings be non-empty,
	// to avoid infinite recursion in LocateHelpFile.
	ASSERT(!strLocal.IsEmpty());

	WriteRegString(szHelpKey, AppendIntegerToString(1, szLocalHelp), strLocal);
	WriteRegString(szHelpKey, AppendIntegerToString(1, szRemoteHelp), "");

//REVIEW ALANE - temporary code

	// As a favor to all the dogfooders out there, this should preserve
	// their remote directory settings as we switch to the new registry
	// keys.  Coded added 4/6/94, and intended for removal in a week or two.

	CString strRemote = GetRegString("Directories", "Remote Help");
	if (!strRemote.IsEmpty())
		WriteRegString(szHelpKey, AppendIntegerToString(1, szRemoteHelp), strRemote);

//REVIEW - end of temporary code
}

/////////////////////////////////////////////////////////////////////////////
// CHelpDirDlg dialog

CHelpDirDlg::CHelpDirDlg(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CHelpDirDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHelpDirDlg)
	m_strPath1 = "";
	m_strPath2 = "";
	m_strMessage = "";
	//}}AFX_DATA_INIT
}

void CHelpDirDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHelpDirDlg)
	DDX_Control(pDX, IDC_INFO_ICON, m_icon);
	DDX_Text(pDX, IDC_HELPPATH1, m_strPath1);
	DDX_Text(pDX, IDC_HELPPATH2, m_strPath2);
	DDX_Text(pDX, IDC_ERRORTEXT, m_strMessage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHelpDirDlg, CBaseDialog)
	//{{AFX_MSG_MAP(CHelpDirDlg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelpDirDlg message handlers

LRESULT CHelpDirDlg::OnCommandHelp(WPARAM, LPARAM)
{
	// Obviously, we cannot invoke help from this dialog!
	return TRUE;
}

BOOL CHelpDirDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	// Place the exclamation-point icon into dialog.
	HICON hicon = theApp.LoadStandardIcon(IDI_EXCLAMATION);
	m_icon.SetIcon(hicon);

	return TRUE; // return TRUE  unless you set the focus to a control
}
