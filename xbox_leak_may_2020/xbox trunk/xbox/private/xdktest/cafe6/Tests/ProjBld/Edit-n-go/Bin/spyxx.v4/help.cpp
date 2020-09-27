// help.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "helpdirs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


static const char chKeySep = '\\';
static char szDefaultKeyName[] = "Software\\Microsoft\\Visual C++ 3.0";
static char szHelpFileName[] = "SPYXX.HLP";
static char szMsgHelpFileName[] = "API32.HLP";



#define MAX_REG_STR 4096
CString GetRegString(LPCSTR szSection, LPCSTR szKey, LPCSTR szDefault)
{
	CString strKey = szDefaultKeyName;
	if (szSection != NULL)
		strKey = strKey + chKeySep + szSection;

	HKEY hSectionKey;
	RegOpenKeyEx(HKEY_CURRENT_USER, strKey, 0, KEY_READ, &hSectionKey);

	char szT[MAX_REG_STR];
	DWORD nSize = MAX_REG_STR;
	DWORD nType = REG_NONE;
	if (hSectionKey == NULL || RegQueryValueEx(hSectionKey, (char*) szKey, NULL, &nType, (LPBYTE) szT, &nSize) != ERROR_SUCCESS)
	{
		// copy the default value
		if (szDefault != NULL)
			_tcsncpy(szT, szDefault, MAX_REG_STR);
		else
			szT[0] = '\0';
	}
	else
		ASSERT(nType == REG_EXPAND_SZ);

	if (hSectionKey != NULL)
		RegCloseKey(hSectionKey);

	return szT;
}


BOOL WriteRegString(LPCSTR szSection, LPCSTR szKey, LPCSTR szVal)
{
	CString strKey = szDefaultKeyName;
	if (szSection != NULL)
		strKey = strKey + chKeySep + szSection;

	DWORD dwDisp;
	HKEY hSectionKey;
	RegCreateKeyEx(HKEY_CURRENT_USER, strKey, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSectionKey, &dwDisp);

	long lResult = RegSetValueEx(hSectionKey, (char*) szKey, NULL, REG_SZ, (LPBYTE) szVal, _tcslen(szVal) + 1);

	RegCloseKey(hSectionKey);

	return (lResult == ERROR_SUCCESS);
}



void CSpyApp::WinHelp(DWORD dwData, UINT nCmd)
{
	if (dwData == IDD_ABOUT + HID_BASE_RESOURCE)
	{
		MessageBeep((UINT)-1);
		return;
	}

	switch (nCmd)
	{
		case HELP_CONTEXT:
			// invoke different help context for process general properties tab on NT
			if ((dwData == IDD_PROCESSGENERALTAB + HID_BASE_RESOURCE) && (!IsChicago()))
				dwData = IDD_PROCESSGENERALTAB_NT + HID_BASE_RESOURCE;

			// invoke different help context for thread general properties tab on NT
			if ((dwData == IDD_THREADGENERALTAB + HID_BASE_RESOURCE) && (!IsChicago()))
				dwData = IDD_THREADGENERALTAB_NT + HID_BASE_RESOURCE;

			InvokeWinHelp(dwData, HELPTYPE_CONTEXT);
			break;

		case HELP_HELPONHELP:
			InvokeWinHelp(dwData, HELPTYPE_HELPONHELP);
			break;

		case HELP_INDEX:
			InvokeWinHelp(dwData, HELPTYPE_INDEX);
			break;
	}
}

void CSpyApp::InvokeWinHelp(DWORD dwData, int helpType)
{
	CString strFile;
	CString strFName;
	BOOL fSuccess;

	if (helpType == HELPTYPE_MSGHELP)
	{
		strFName = szMsgHelpFileName;
	}
	else
	{
		strFName = szHelpFileName;
	}

	if (!LocateHelpFile(strFName, strFile, TRUE))
	{
		return;
	}

	BeginWaitCursor();

	switch (helpType)
	{
		case HELPTYPE_INDEX:
			fSuccess = ::WinHelp(m_pMainWnd->m_hWnd, strFile, HELP_INDEX, dwData);
			break;

		case HELPTYPE_HELPONHELP:
			fSuccess = ::WinHelp(m_pMainWnd->m_hWnd, strFile, HELP_HELPONHELP, dwData);
			break;

		case HELPTYPE_CONTEXT:
			fSuccess = ::WinHelp(m_pMainWnd->m_hWnd, strFile, HELP_CONTEXT, dwData);
			break;

		case HELPTYPE_MSGHELP:
			fSuccess = ::WinHelp(m_pMainWnd->m_hWnd, strFile, HELP_KEY, dwData);
			break;

		default:
			//
			// Unknown help type.
			//
			ASSERT(FALSE);
			fSuccess = FALSE;
			break;
	}

	EndWaitCursor();

	if (!fSuccess)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_LAUNCH_HELP);
	}
}

