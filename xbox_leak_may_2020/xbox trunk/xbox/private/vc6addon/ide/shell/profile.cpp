#include "stdafx.h"

#include "tools.h"
#include "shell.h"
#include "find.h"
#include "toolexpt.h"

BOOL GetRegIntCore(UINT & nValue, HKEY hkey, LPCSTR szSection, LPCSTR szKey, UINT nDefault);
BOOL GetRegDataCore(HGLOBAL & hglobRet, HKEY hkey, LPCSTR szSection, LPCSTR szKey, HGLOBAL hglobDefault = NULL);
LONG GetRegStringCore(LPSTR szRet, DWORD & nSize, DWORD & nType, HKEY hkey, LPCSTR szSection, LPCSTR szKey);

BOOL WriteRegDataCore(HKEY hkey, LPCSTR szSection, LPCSTR szKey, HGLOBAL hglobValue);

IMPLEMENT_DYNAMIC(CInitFile, CMemFile);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
//	Reg DB stuff

// BASED_CODE definitions for the profile string values
// I don't think INI files are ever localized... (bc)

// [General] section...
//
static char BASED_CODE szGeneral[] = "General";
static char BASED_CODE szSetup[] = "Setup";
static char BASED_CODE szMainWndPos[] = "AppWindow";
static char BASED_CODE szMainWndSize[] = "AppWindowSize";
static char BASED_CODE szPropPagePos[] = "PropertiesWindow";
static char BASED_CODE szMaxApp[] = "MaximizeApp";
static char BASED_CODE szMaxDoc[] = "MaximizeDoc";
static char BASED_CODE szToolTips[] = "ToolTips";
static char BASED_CODE szToolTipKeys[] = "ToolTipKeys";
static char BASED_CODE szLargeToolBars[] = "LargeToolBars";
static char BASED_CODE szWindowMenuItems[] = "WindowMenuItems";
static char BASED_CODE szWindowMenuSorted[] = "WindowMenuSorted";
static char BASED_CODE szPushPin[] = "PropertiesPushPin";
static char BASED_CODE szFileSection[] = "Recent File List";
static char BASED_CODE szProjectCount[] = "ProjectCount";
static char BASED_CODE szProjectEntry[] = "Project%d";
static char BASED_CODE szFileCount[] = "FileCount";
static char BASED_CODE szFileMnemonics[] = "FileMnemonics";
static char BASED_CODE szFileEntry[] = "File%d";
static char BASED_CODE szFileRTCEntry[] = "TypeInfo%d";
static char BASED_CODE szWorkspaceAuto[] = "WorkspaceAuto";
static char BASED_CODE szWindowsMenu[] = "TraditionalMenu";
static char BASED_CODE szMRUOnFile[] = "MRUListsOnFileMenu";
#ifndef NO_VBA
static char BASED_CODE szOleAutomation[] = "SupportOleAutomation";
#endif	// NO_VBA


static char BASED_CODE szPinGoToDlg[] = "PinGoToDlg";
static char BASED_CODE szDlgEdDblClickEditCode[] = "DlgEdDblClickEditCode";

#ifdef ENABLE_FILE_AUTOSAVES
static char BASED_CODE szAutosaveEnabled[] = "AutosaveEnabled";
static char BASED_CODE szAutosaveInterval[] = "AutosaveInterval";
#endif	// ENABLE_FILE_AUTOSAVES

char BASED_CODE szSearch[] = "Search";
char BASED_CODE szSearchFind[] = "Find%d";
char BASED_CODE szSearchReplace[] = "Replace%d";


// General purpose strings...
//
char BASED_CODE szEmpty[] = "";
static char BASED_CODE szCoordFmt[] = "%d,%d";
static char BASED_CODE szColorFmt[] = "%u,%u,%u";

// Stolen from afx.h before they were removed...
//
#define _countof(array)	(sizeof(array)/sizeof(array[0]))

// MBS <-> WCS conversion with truncation
#define _wcstombsz(lpszDest, lpszSrc, nMaxBytes) \
	wcstombs(lpszDest, lpszSrc, nMaxBytes), lpszDest[nMaxBytes-1] = 0

LPSTR GetRegistryKeyName()
{
	ASSERT(theApp.m_pszRegKeyName != NULL);
	return (char*) theApp.m_pszRegKeyName;
}

static BOOL ParseCoord(const char* szCoord, int* px, int* py)
{
	int n;
	const char* pch = szCoord;
	char* pch2 = (char*)pch;

	n = (int)_tcstol(pch, &pch2, 10);
	if (pch2 == pch)
		return FALSE;
	*px = n;
	pch = pch2;

	if (*pch == ',')
		pch += 1;

	pch2 = (char*)pch;
	n = (int)_tcstol(pch, &pch2, 10);
	if (pch2 == pch)
		return FALSE;
	*py = n;

	return TRUE;
}

BOOL ParseRect(const char* szRect, CRect& rect)
{
	int n;
	const char* pch = szRect;
	char* pch2 = (char*)pch;

	n = (int)_tcstol(pch, &pch2, 10);
	if (pch2 == pch)
		return FALSE;
	rect.left = n;
	pch = pch2;

	if (*pch == ',')
		pch += 1;

	pch2 = (char*)pch;
	n = (int)_tcstol(pch, &pch2, 10);
	if (pch2 == pch)
		return FALSE;
	rect.top = n;
	pch = pch2;

	if (*pch == ',')
		pch += 1;

	pch2 = (char*)pch;
	n = (int)_tcstol(pch, &pch2, 10);
	if (pch2 == pch)
		return FALSE;
	rect.right = n;
	pch = pch2;

	if (*pch == ',')
		pch += 1;

	pch2 = (char*)pch;
	n = (int)_tcstol(pch, &pch2, 10);
	if (pch2 == pch)
		return FALSE;
	rect.bottom = n;

	return TRUE;
}

BOOL GetRegRect(const char* szSection, const char* szEntry, CRect& rect)
{
	return ParseRect(GetRegString(szSection, szEntry), rect);
}

void WriteRegRect(const char* szSection, const char* szEntry, const CRect& rect)
{
	char szBuf [64];
	sprintf(szBuf, "%d,%d,%d,%d", rect.left, rect.top, rect.right, rect.bottom);
	WriteRegString(szSection, szEntry, szBuf);
}

//
// Read a profile string consisting of two numbers separated by a space
// or comma and use it as a position for a window.  The size of the
// window is passed in so this function can adjust the position if
// necessary to keep the window visible on the screen.  If the function
// is successful, TRUE is returned and rect contains a window rectangle.
// Otherwise, FALSE is returned and rect is unmodified.
//
static BOOL GetProfileWndPos(const char* szSection, const char* szEntry,
	CRect& rect, const CSize& size)
{
	CString str = GetRegString(szSection, szEntry);

	if (str.IsEmpty())
		return FALSE;

	CPoint topLeft;
	if (!ParseCoord(str, (int*)&topLeft.x, (int*)&topLeft.y))
		return FALSE;

	rect = CRect(topLeft, size);
	ForceRectOnScreen(rect);

	return TRUE;
}


static void WriteWndProfile(const char* szSection, const char* szEntry,
	CWnd* pWnd)
{
	CRect rect;
	char szBuf [20];

	ASSERT(pWnd != NULL);
	ASSERT(pWnd->m_hWnd != NULL);

	pWnd->GetWindowRect(&rect);
	sprintf(szBuf, szCoordFmt, rect.top, rect.left);
	WriteRegString(szSection, szEntry, szBuf);
}


static void ReadWndProfile(const char* szSection, const char* szEntry,
	CWnd* pWnd)
{
	CRect rect;
	pWnd->GetWindowRect(&rect);
	if (GetProfileWndPos(szSection, szEntry, rect, rect.Size()))
		pWnd->MoveWindow(&rect);
}

#ifdef _WIN32
void CTheApp::SavePrivateAutosaveSettings()
{
	// save autosave state and autosave interval [patbr]
#ifdef ENABLE_FILE_AUTOSAVES
	WriteRegInt(szGeneral, szAutosaveEnabled,  gAutosaver.IsEnabled());
	WriteRegInt(szGeneral, szAutosaveInterval, gAutosaver.GetInterval());
#endif	// ENABLE_FILE_AUTOSAVES
}
#endif

void CTheApp::SavePrivateProfileSettings()
{
	char szBuf [32];
	CRect rect;

	if(((CMainFrame*) m_pMainWnd)->m_lastWindowRect != CFrameWnd::rectDefault)
	{
		// Save the last non-min/max main window position/size
		CPoint pt = ((CMainFrame*) m_pMainWnd)->m_lastWindowRect.TopLeft();
		sprintf(szBuf, szCoordFmt, pt.x, pt.y);
		WriteRegString(szGeneral, szMainWndPos, szBuf );

		CSize size = ((CMainFrame*) m_pMainWnd)->m_lastWindowRect.Size();
		ASSERT(size.cx>0 || size.cy>0);  // Should never have negative sizes
		sprintf(szBuf, szCoordFmt, size.cx, size.cy);
		WriteRegString(szGeneral, szMainWndSize, szBuf );
	}

	// Save the position of the property page
	GetPropertyBrowser()->GetWindowRect(&rect);

	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);
	x = (rect.left < x - rect.right) ? rect.left : rect.right;
	y = (rect.top < y - rect.bottom) ? rect.top : rect.bottom;

	sprintf(szBuf, szCoordFmt, x, y);
	WriteRegString(szGeneral, szPropPagePos, szBuf );

	WriteRegInt(szGeneral, szMaxApp, m_bMaximizeApp);
	WriteRegInt(szGeneral, szMaxDoc, m_bMaximizeDoc);
	WriteRegInt(szGeneral, szToolTips, m_bToolTips);
	WriteRegInt(szGeneral, szToolTipKeys, m_bToolTipKeys);
	WriteRegInt(szGeneral, szLargeToolBars, m_bLargeToolBars);
	WriteRegInt(szGeneral, szWorkspaceAuto, m_bWorkspaceAuto);
	WriteRegInt(szGeneral, szWindowMenuItems, m_nWindowMenuItems);
	WriteRegInt(szGeneral, szWindowMenuSorted, m_bWindowMenuSorted);
	WriteRegInt(szGeneral, szMRUOnFile, m_bMRUOnFileMenu);
	WriteRegInt(szGeneral, szWindowsMenu, !Menu::IsInCmdBarMode());

	BOOL bPopProp;
	m_theAppSlob.GetIntProp(P_PoppingProperties, bPopProp);
	WriteRegInt(szGeneral, szPushPin, !bPopProp);

	WriteRegInt(szGeneral, szPinGoToDlg, m_bPinGoToDlg);
	WriteRegInt(szGeneral, szDlgEdDblClickEditCode, m_bDlgEdDblClickEditCode);

	// Save the command tools setting
	SaveToolListSetting() ;

	// Write the MRU section from scratch.
	CString strKeyMain = GetRegistryKeyName();
	RegDeleteKey(HKEY_CURRENT_USER, strKeyMain + chKeySep + szFileSection);

	if (m_pRecentProjectList != NULL)
	{
		m_pRecentProjectList->WriteList();	// Deletes contents of key!
		WriteRegInt(szFileSection, szProjectCount, m_pRecentProjectList->GetSize());
	}
	if (m_pRecentFileList != NULL)
	{
		m_pRecentFileList->WriteList();
		WriteRegInt(szFileSection, szFileCount, m_pRecentFileList->GetSize());
	}

	SavePrivateAutosaveSettings();
	SaveLastProject();
	SaveFindReplaceStrings();

	POSITION pos = m_packages.GetHeadPosition();
	while (pos != NULL)
		((CPackage*)m_packages.GetNext(pos))->SaveSettings();
}

/****************************************************************************

	FUNCTION:	SaveFindReplaceStrings

****************************************************************************/

void
SaveFindReplaceStrings()
{
	ShellPickList().SaveToRegistry();
}


#ifdef _WIN32
void CTheApp::LoadPrivateAutosaveSettings()
{
	// restore autosave state and autosave interval [patbr]
#ifdef ENABLE_FILE_AUTOSAVES
	int fAutosaveEnabled  = GetProfileInt(szGeneral, szAutosaveEnabled,  FALSE);
	int iAutosaveInterval = GetProfileInt(szGeneral, szAutosaveInterval, 15);

	if (fAutosaveEnabled)
		VERIFY(gAutosaver.Enable(iAutosaveInterval));
	else
		gAutosaver.Disable();
#endif	// ENABLE_FILE_AUTOSAVES
}
#endif

void CTheApp::LoadPrivateProfileSettings()
{
	m_bFirstInstance = (GetProfileString(NULL, szSetup, "") == "1");
	if (m_bFirstInstance)
		WriteProfileString("", szSetup, "0");

	m_bMaximizeApp = GetProfileInt(szGeneral, szMaxApp, TRUE);
	m_bMaximizeDoc = GetProfileInt(szGeneral, szMaxDoc, FALSE);
	m_bToolTips = GetProfileInt(szGeneral, szToolTips, TRUE);
	m_bToolTipKeys = GetProfileInt(szGeneral, szToolTipKeys, TRUE);
	m_bLargeToolBars = GetProfileInt(szGeneral, szLargeToolBars, FALSE);
	m_nWindowMenuItems = GetProfileInt(szGeneral, szWindowMenuItems, 10); 
	m_bWindowMenuSorted = GetProfileInt(szGeneral, szWindowMenuSorted, TRUE); 
	m_bMRUOnFileMenu = GetProfileInt(szGeneral, szMRUOnFile, FALSE); 
	m_bWorkspaceAuto = GetProfileInt(szGeneral, szWorkspaceAuto, TRUE);

	BOOL bPushPin = GetProfileInt(szGeneral, szPushPin, FALSE);
	m_theAppSlob.SetIntProp(P_PoppingProperties, !bPushPin);

	if (!(m_bInvokedCommandLine && m_bRunInvisibly)) // REVIEW
	{

	m_bPinGoToDlg = GetProfileInt(szGeneral, szPinGoToDlg, TRUE);
	m_bDlgEdDblClickEditCode = GetProfileInt(szGeneral, szDlgEdDblClickEditCode, TRUE); 


		LoadFindReplaceStrings();

	// BLOCK: get the size and position for the main window
	{
		CSize size;
		CString str = GetRegString(szGeneral, szMainWndSize);
		if (!ParseCoord(str, (int*)&size.cx, (int*)&size.cy))
			size = CFrameWnd::rectDefault.Size();

		((CMainFrame*) m_pMainWnd)->m_lastWindowRect.BottomRight() =
			((CMainFrame*) m_pMainWnd)->m_lastWindowRect.TopLeft() + size;

		GetProfileWndPos(szGeneral, szMainWndPos,
			((CMainFrame*) m_pMainWnd)->m_lastWindowRect,
			((CMainFrame*) m_pMainWnd)->m_lastWindowRect.Size());
	}

	// BLOCK: get position of the property page
	{
		CPoint pt;
		CString str = GetRegString(szGeneral, szPropPagePos);
		if (!ParseCoord(str, (int*)&m_ptPropPage.x, (int*)&m_ptPropPage.y))
			m_ptPropPage = CPoint(CSheetWnd::xDefaultPos, CSheetWnd::yDefaultPos);
	}

	// We need to determing if the user wants HMENU or CmdBars. HMENUS are used by 
	// disabled users. To allow them to get at the trad UI without having to use the
	// non-trad UI, we support a cmdline switch as well as the registry.
	BOOL bOldMenu = ScanCommandLine("WindowsMenu");	
	if (!bOldMenu)
	{
		// Ask windows if there's a screen reader in place
		BOOL bScreenReader=FALSE;
		BOOL bFoundScreenReader=SystemParametersInfo(SPI_GETSCREENREADER, 0, &bScreenReader, FALSE);
		// only notice this setting if windows supports this SPI value
		bScreenReader&=bFoundScreenReader;

		// If the command line and registry aren't specified, then the default is taken from SPI_GETSCREENREADER
		// The command line wasn't specified, so look in the registry
		bOldMenu = GetProfileInt(szGeneral, szWindowsMenu, bScreenReader);
	}

	// Set the flag indicating which the user perfers.
	Menu::UseHMENU(bOldMenu) ;
	} // cmdline
}

//	CTheApp::LoadMRUList
//		Call this after the packages are loaded, or loading the
//		runtime classes may fail.

void CTheApp::LoadMRUList()
{
	// Load the file MRU
	UINT nStart = 0;
	UINT nCount = GetProfileInt(szFileSection, szFileCount, _AFX_MRU_COUNT);
	nCount = min(nCount,  ID_FILE_MRU_LAST - ID_FILE_MRU_FIRST + 1);

	if (nCount > 0)
	{
		// create file MRU since nMaxMRU not zero
		m_pRecentFileList = new CTypedRecentFileList(nStart, szFileSection,
			szFileEntry, szFileRTCEntry, nCount);
		m_pRecentFileList->ReadList();

		nStart = nCount;	// Next menu starts where this left off.
	}

	// Load the project MRU
	nCount = GetProfileInt(szFileSection, szProjectCount, _AFX_MRU_COUNT);
	nCount = min(nCount, ID_PROJ_MRU_LAST - ID_PROJ_MRU_FIRST + 1);

	if (nCount > 0)
	{
		// Needs to be a CBRecentFileList so that it gets the correct UpdateMenu function.
		m_pRecentProjectList = new CBRecentFileList(nStart, szFileSection,
			szProjectEntry, nCount);
		m_pRecentProjectList->ReadList();
	}

	CalculateRecentMnemonics();
}

// Given the size of the lists, work out how many mnemonics should be given to each.
void CTheApp::CalculateRecentMnemonics()
{
	CBRecentFileList *pRecentFile=(CBRecentFileList*)m_pRecentFileList;
	CBRecentFileList *pRecentProject=(CBRecentFileList*)m_pRecentProjectList;

	if(theApp.m_bMRUOnFileMenu)
	{
		int nFileShortStart = GetProfileInt(szFileSection, szFileMnemonics, -1);

		if(nFileShortStart<=0)
		{
			// Now update the shortcut start values for each list
			ASSERT(m_pRecentFileList);
			ASSERT(m_pRecentProjectList);

			int nFiles=m_pRecentFileList->GetSize();
			int nProjects=m_pRecentProjectList->GetSize();
			int nEntries=nFiles+nProjects;
			int nFileShorts=-1;
			int nProjectShorts=-1;

			// If both can fit inside 10, do that.
			if(nEntries<=10)
			{
				nFileShorts=nFiles;
				nProjectShorts=nProjects;
			}
			else
			{
				// otherwise, split proportionately
				nFileShorts=10.0*(double)nFiles/(double)nEntries;
				nProjectShorts=10-nProjectShorts;
			}

			pRecentFile->SetShortCount(nFileShorts);
			pRecentProject->SetShortCount(nProjectShorts);
			pRecentProject->SetStart(nFileShorts);
		}
		else
		{
			// user expressed a preference, so use that.
			int nFileShortCount=min(10, nFileShortStart);
			pRecentFile->SetShortCount(nFileShortCount);
			pRecentProject->SetShortCount(10-nFileShortCount);
			pRecentProject->SetStart(nFileShortCount);
		}
	}
	else
	{
		pRecentFile->SetShortCount(10);
		pRecentFile->SetStart(0);
		pRecentProject->SetShortCount(10);
		pRecentProject->SetStart(0);
	}
}

/****************************************************************************

	FUNCTION:	LoadFindReplaceStrings

****************************************************************************/

void
LoadFindReplaceStrings()
{
	ShellPickList().LoadFromRegistry(&findReplace);
}


#define MAX_REG_STR 4096
CString GetRegString(LPCSTR szSection, LPCSTR szKey, LPCSTR szDefault)
{
	return theApp.GetProfileString(szSection, szKey, szDefault);
}

/*
//
// Sometimes we have to know whether or not the value was there
//
BOOL FGetRegString (LPCSTR szSection, LPCSTR szKey, CString& strReturn)
{
#ifdef _WIN32
	CString strKey = GetRegistryKeyName();
	if (szSection != NULL)
		strKey = strKey + chKeySep + szSection;

	HKEY hSectionKey = NULL;
	RegOpenKeyEx(HKEY_CURRENT_USER, strKey, 0, KEY_READ, &hSectionKey);

	DWORD nSize = 0;
	DWORD nType = REG_NONE;
	if (hSectionKey == NULL || RegQueryValueEx(hSectionKey, (char*) szKey, NULL,
			&nType, NULL, &nSize) != ERROR_SUCCESS || nSize == 0)
	{
		return FALSE;
	}
	else
	{
		ASSERT(nType == REG_SZ);
		unsigned char * sz = (unsigned char *)malloc (nSize);

		if (RegQueryValueEx(hSectionKey, (char*) szKey, NULL,
				&nType, sz, &nSize) != ERROR_SUCCESS)
		{
			free (sz);
			return FALSE;
		}
		else
		{
			strReturn = sz;
			free (sz);
		}
	}

	if (hSectionKey != NULL)
		RegCloseKey(hSectionKey);

	return TRUE;
#else
	strReturn = GetRegString (szSection, szKey, "");
	return TRUE;
#endif
}
*/

UINT GetRegInt(LPCSTR szSection, LPCSTR szKey, int nDefault)
{
	return theApp.GetProfileInt(szSection, szKey, nDefault);
}

UINT CTheApp::GetProfileInt(LPCTSTR szSection, LPCTSTR szKey, int nDefault)
{
	UINT nValue;
	if (!GetRegIntCore(nValue, HKEY_CURRENT_USER, szSection, szKey, (UINT)nDefault))
		GetRegIntCore(nValue, HKEY_LOCAL_MACHINE, szSection, szKey, (UINT)nDefault);
	return nValue;
}

CString CTheApp::GetProfileString( LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szDefault /* = NULL*/)
{
	char sz[MAX_REG_STR];
	sz[0] = 0;
	DWORD nType = REG_SZ;
	DWORD nSize = MAX_REG_STR;
	if (ERROR_SUCCESS != GetRegStringCore(sz, nSize, nType, HKEY_CURRENT_USER, szSection, szKey))
	{
		nType = REG_SZ;
		nSize = MAX_REG_STR;
		if (ERROR_SUCCESS != GetRegStringCore(sz, nSize, nType, HKEY_LOCAL_MACHINE, szSection, szKey))
			return szDefault ? szDefault : _TEXT("");
	}
	return sz;
}

BOOL GetRegIntCore(UINT & nValue, HKEY hkey, LPCSTR szSection, LPCSTR szKey, UINT nDefault)
{
	BOOL bRet = TRUE; // optimism
	CString strKey = GetRegistryKeyName();
	if (szSection != NULL)
		strKey = strKey + chKeySep + szSection;

	HKEY hSectionKey = NULL;
	RegOpenKeyEx(hkey, strKey, 0, KEY_READ, &hSectionKey);

	DWORD nSize = sizeof UINT;
	DWORD nType = REG_NONE;
	if (hSectionKey == NULL || RegQueryValueEx(hSectionKey, (LPTSTR) szKey, NULL,
			&nType, (LPBYTE)&nValue, &nSize) != ERROR_SUCCESS || nSize == 0)
	{
		// copy the default value
		nValue = nDefault;
		bRet = FALSE;
	}
	else
	{
		ASSERT(nType == REG_DWORD);
	}

	if (hSectionKey != NULL)
    	RegCloseKey(hSectionKey);

	return bRet;
}

BOOL GetRegDataCore(HGLOBAL & hglobRet, HKEY hkey, LPCSTR szSection, LPCSTR szKey, HGLOBAL hglobDefault)
{
	BOOL bRet = TRUE; // optimism
	CString strKey = GetRegistryKeyName();
	if (szSection != NULL)
		strKey = strKey + chKeySep + szSection;

	HKEY hSectionKey = NULL;
	RegOpenKeyEx(hkey, strKey, 0, KEY_READ, &hSectionKey);

	LPBYTE lpData;
	DWORD nSize = 0;
	DWORD nType = REG_NONE;
	if (hSectionKey == NULL || RegQueryValueEx(hSectionKey, (LPTSTR)szKey, NULL,
			&nType, NULL, &nSize) != ERROR_SUCCESS || nSize == 0)
	{
		// copy the default value
		hglobRet = hglobDefault;
		bRet = FALSE;
	}
	else
	{
		ASSERT(nType == REG_BINARY);
		hglobRet = ::GlobalAlloc(GMEM_MOVEABLE, nSize);
		lpData = (LPBYTE) ::GlobalLock(hglobRet);

		if (RegQueryValueEx(hSectionKey, (LPTSTR)szKey, NULL,
				&nType, lpData, &nSize) != ERROR_SUCCESS)
		{
			::GlobalUnlock(hglobRet);
			::GlobalFree(hglobRet);
			hglobRet = hglobDefault;
			bRet = FALSE;
		}
		else
		{
			::GlobalUnlock(hglobRet);
		}
	}

	if (hSectionKey != NULL)
		RegCloseKey(hSectionKey);

	return bRet;
}

LONG GetRegStringCore( LPSTR szRet, DWORD & nSize, DWORD & nType, HKEY hkey, LPCSTR szSection, LPCSTR szKey)
{
	LONG lRet;
	CString strKey = GetRegistryKeyName();
	if (szSection != NULL)
		strKey = strKey + chKeySep + szSection;

	HKEY hSectionKey = NULL;
	lRet = RegOpenKeyEx(hkey, strKey, 0, KEY_READ, &hSectionKey);
	if (hSectionKey != NULL)
		lRet = RegQueryValueEx(hSectionKey, (LPTSTR)szKey, NULL, &nType, (LPBYTE)szRet, &nSize);

	if (hSectionKey != NULL)
    	RegCloseKey(hSectionKey);

	return lRet;
}

static IStream *GetRegIStreamCore(HKEY hKey, LPCTSTR szSection, LPCTSTR szKey)
{
	IStream *pIStreamResult = NULL;

	CString strKey = GetRegistryKeyName();
	if (szSection != NULL)
		strKey = strKey + chKeySep + szSection;

	HKEY hSectionKey = NULL;
	RegOpenKeyEx(hKey, strKey, 0, KEY_READ, &hSectionKey);

	LPBYTE lpData;
	DWORD nSize = 0;
	DWORD nType = REG_NONE;
	if ((hSectionKey != NULL) &&
		(RegQueryValueEx(hSectionKey, szKey, NULL, &nType, NULL, &nSize) == ERROR_SUCCESS) &&
		(nSize != 0))
	{
		ASSERT(nType == REG_BINARY);
		HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, nSize);
		lpData = (LPBYTE) ::GlobalLock(hGlobal);

		if (::RegQueryValueEx(hSectionKey, szKey, NULL, &nType, lpData, &nSize) != ERROR_SUCCESS)
		{
			::GlobalUnlock(hGlobal);
			::GlobalFree(hGlobal);
		}
		else
		{
			::GlobalUnlock(hGlobal);

			HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pIStreamResult);

			if (FAILED(hr))
				::GlobalFree(hGlobal);
		}
	}

	if (hSectionKey != NULL)
		RegCloseKey(hSectionKey);

	return pIStreamResult;
}

IStream *GetRegIStream(LPCTSTR szSection, LPCTSTR szKey)
{
	IStream *pIStreamResult = GetRegIStreamCore(HKEY_CURRENT_USER, szSection, szKey);
	if (pIStreamResult == NULL)
		pIStreamResult = GetRegIStreamCore(HKEY_LOCAL_MACHINE, szSection, szKey);
	return pIStreamResult;
}

HGLOBAL GetRegData(LPCSTR szSection, LPCSTR szKey, HGLOBAL hglobDefault)
{
	HGLOBAL hglob = NULL; 
	if (!GetRegDataCore(hglob, HKEY_CURRENT_USER, szSection, szKey, hglobDefault))
		GetRegDataCore(hglob, HKEY_LOCAL_MACHINE, szSection, szKey, hglobDefault);
	return hglob;
}

BOOL WriteRegString(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szVal)
{
	return theApp.WriteProfileString(szSection, szKey, szVal);
}

BOOL WriteRegInt(LPCSTR szSection, LPCSTR szKey, int nValue)
{
	return theApp.WriteProfileInt(szSection, szKey, nValue);
}

BOOL WriteRegData(LPCSTR szSection, LPCSTR szKey, HGLOBAL hglobValue)
{
	return WriteRegDataCore(HKEY_CURRENT_USER, szSection, szKey, hglobValue);
}

BOOL WriteRegDataCore(HKEY hkey, LPCSTR szSection, LPCSTR szKey, HGLOBAL hglobValue)
{
#ifdef _WIN32
	CString strKey = GetRegistryKeyName();
	if (szSection != NULL)
		strKey = strKey + chKeySep + szSection;

	DWORD dwDisp;
	HKEY hSectionKey = NULL;
	RegCreateKeyEx(hkey, strKey, 0, "", REG_OPTION_NON_VOLATILE,
		KEY_WRITE, NULL, &hSectionKey, &dwDisp);

	long lResult;

	if (hglobValue == NULL)
	{
		lResult = RegDeleteValue(hSectionKey, (char*) szKey);
	}
	else
	{
		LPBYTE lpData = (LPBYTE) ::GlobalLock(hglobValue);
		WORD* pwSize = (WORD*) lpData;
		int nSize = *pwSize + sizeof(WORD);	// First word is data size.
		ASSERT(nSize > 0);	// Hey! no data.

		lResult = RegSetValueEx(hSectionKey, (char*) szKey, NULL,
			REG_BINARY,	lpData, nSize);
		::GlobalUnlock(hglobValue);
	}

	RegCloseKey(hSectionKey);

	return (lResult == ERROR_SUCCESS);
#else
	return TRUE;	// Do nothing.
#endif
}

BOOL DeleteRegData(LPCSTR szSection, LPCSTR szKey)
{
	return WriteRegDataCore(HKEY_CURRENT_USER, szSection, szKey, NULL);
}

static BOOL WriteRegIStreamCore(HKEY hkey, LPCTSTR szSection, LPCTSTR szKey, IStream *pIStream)
{
#ifdef _WIN32
	CString strKey = GetRegistryKeyName();
	if (szSection != NULL)
		strKey = strKey + chKeySep + szSection;

	DWORD dwDisp;
	HKEY hSectionKey = NULL;
	RegCreateKeyEx(hkey, strKey, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSectionKey, &dwDisp);

	long lResult;

	if (pIStream == NULL)
	{
		lResult = RegDeleteValue(hSectionKey, (char*) szKey);
	}
	else
	{
		HGLOBAL hGlobal = NULL;
		lResult = ::GetHGlobalFromStream(pIStream, &hGlobal);

		// If you get this assert, it's probably because you didn't create
		// the stream with CreateStreamOnHGlobal().  While we might be able
		// to do cooler stuff w.r.t. copying non-HGLOBAL streams, it doesn't
		// seem worth it at this moment.  -mgrier 6/19/96
		ASSERT(SUCCEEDED(lResult));

		LPBYTE lpData = (LPBYTE) ::GlobalLock(hGlobal);

		ULARGE_INTEGER cbStream;

		LARGE_INTEGER li;
		li.QuadPart = 0;
		// See where the end pointer is...
		pIStream->Seek(li, STREAM_SEEK_END, &cbStream);

		// No streams larger than 2^32 bytes for us!!!
		ASSERT(cbStream.HighPart == 0);
		lResult = RegSetValueEx(hSectionKey, szKey, NULL, REG_BINARY, lpData, cbStream.LowPart);
		::GlobalUnlock(hGlobal);
	}

	RegCloseKey(hSectionKey);

	return (lResult == ERROR_SUCCESS);
#else
	return TRUE;	// Do nothing.
#endif
}

BOOL WriteRegIStream(LPCTSTR szSection, LPCTSTR szKey, IStream *pIStream)
{
	return WriteRegIStreamCore(HKEY_CURRENT_USER, szSection, szKey, pIStream);
}

///////////////////////////////////////////////////////////////////////////////
//	Control Panel Registry stuff

CCPLReg::CCPLReg()
{
	m_bEnglishMeasure = TRUE;
	m_strDecimal = _TEXT(".");

	// Get pointers to the date/time formatting functions in kernel32.dll

	m_hKernel = LoadLibrary("KERNEL32.DLL");
	ASSERT(m_hKernel);

	m_lpfnGetTimeFormatW = (int (FAR WINAPI *)(LCID, DWORD, CONST SYSTEMTIME *, LPCWSTR, LPWSTR, int))GetProcAddress(m_hKernel, "GetTimeFormatW");
	m_lpfnGetTimeFormatA = (int (FAR WINAPI *)(LCID, DWORD, CONST SYSTEMTIME *, LPCSTR, LPSTR, int))GetProcAddress(m_hKernel, "GetTimeFormatA");

	m_lpfnGetDateFormatW = (int (FAR WINAPI *)(LCID, DWORD, CONST SYSTEMTIME *, LPCWSTR, LPWSTR, int))GetProcAddress(m_hKernel, "GetDateFormatW");
	m_lpfnGetDateFormatA = (int (FAR WINAPI *)(LCID, DWORD, CONST SYSTEMTIME *, LPCSTR, LPSTR, int))GetProcAddress(m_hKernel, "GetDateFormatA");

	// Read the registry entries - it's ok to do this now,
	// as these entries should always exist!
	Update();
}

CCPLReg::~CCPLReg()
{
	if (m_hKernel)
		FreeLibrary(m_hKernel);
}

const CString CCPLReg::Format(CTime& time, FORMAT_TYPE fType, DWORD dwFlags /*= DATE_SHORTDATE*/)
{
	CString strResult;

	// The time functions will crash past Jan 19, 2038 since GetLocalTm() will return NULL
	// and things like GetYear() dereference it unconditionaly.  Prevent that from happening
	// here and just return an empty string in the future...
	if (time.GetLocalTm(NULL) == NULL)
		return strResult;

	// Convert the time to a SYSTEMTIME structure that the os
	// date/time formatting functions use

	SYSTEMTIME TempTime;
	TempTime.wYear = (WORD)time.GetYear();
	TempTime.wMonth = (WORD)time.GetMonth();
	TempTime.wDayOfWeek = (WORD)time.GetDayOfWeek();
	TempTime.wDay = (WORD)time.GetDay();
	TempTime.wHour = (WORD)time.GetHour();
	TempTime.wMinute = (WORD)time.GetMinute();
	TempTime.wSecond = (WORD)time.GetSecond();
	TempTime.wMilliseconds = 0;

	// Do the formatting

	TCHAR *pch = strResult.GetBuffer(_MAX_PATH);
	switch (fType)
	{
		case TIME_ALL:
		case TIME_NO_SECONDS:
			{
  				DWORD timeflags = 0;

				if (fType == TIME_NO_SECONDS)
					timeflags = TIME_NOSECONDS;

				// If we have GetTimeFormatA then use it,
				// otherwise we must use GetTimeFormatW and
				// do a wide char to multi byte conversion.

				if (m_lpfnGetTimeFormatA)
				{
					(*(m_lpfnGetTimeFormatA))(GetUserDefaultLCID(), timeflags, &TempTime, NULL, pch, _MAX_PATH);
				}
				else
				{
					WCHAR szTemp[_MAX_PATH];

					(*(m_lpfnGetTimeFormatW))(GetUserDefaultLCID(), timeflags, &TempTime, NULL, szTemp, _countof(szTemp));
					_wcstombsz(pch, szTemp, _countof(szTemp));
				}
				break;
			}

		case DATE_ALL:
			{
				// If we have GetDateFormatA then use it,
				// otherwise we must use GetDateFormatW and
				// do a wide char to multi byte conversion.

 				if (m_lpfnGetDateFormatA)
				{
					(*(m_lpfnGetDateFormatA))(GetUserDefaultLCID(), dwFlags, &TempTime, NULL, pch, _MAX_PATH);
				}
				else
				{
					WCHAR szTemp[_MAX_PATH];

					(*(m_lpfnGetDateFormatW))(GetUserDefaultLCID(), dwFlags, &TempTime, NULL, szTemp, _countof(szTemp));
					_wcstombsz(pch, szTemp, _countof(szTemp));
				}
				break;

			}

		default:
			ASSERT(FALSE);
	}

	// Release the buffer and return the formatted string
	strResult.ReleaseBuffer();
 	return strResult;
}

void CCPLReg::Update()
{

	// Numbers
	m_bEnglishMeasure = (GetCPLInt(LOCALE_IMEASURE, m_bEnglishMeasure) == 1);
	m_strDecimal = GetCPLString (LOCALE_SDECIMAL, m_strDecimal);
}

CString CCPLReg::GetCPLString(LCTYPE lcType, LPCSTR szDefault)
{
	WCHAR szTemp[255];
	char szTempmb[255];
	LPCTSTR szRet;
	HINSTANCE hKernel;
	int (FAR WINAPI *lpfnGetLocaleInfoA)(LCID, LCTYPE, LPSTR, int);
	int (FAR WINAPI *lpfnGetLocaleInfoW)(LCID, LCTYPE, LPWSTR, int);

	hKernel = LoadLibrary( "KERNEL32.DLL" );
	ASSERT(hKernel);

	lpfnGetLocaleInfoA = (int (FAR WINAPI *)(LCID, LCTYPE, LPSTR, int))GetProcAddress(hKernel, "GetLocaleInfoA");
	lpfnGetLocaleInfoW = (int (FAR WINAPI *)(LCID, LCTYPE, LPWSTR, int))GetProcAddress(hKernel, "GetLocaleInfoW");

	// If we have GetLocaleInfoA then use it. The only o.s. which doesn't
	// have this is NT 511, so we just protect ourselves here by making
	// sure that we don't die even on NT 511 since we will just use
	// GetLocaleInfoW and do the multibyte conversion ourselves.
	if (lpfnGetLocaleInfoA)
	{
		if ((*(lpfnGetLocaleInfoA))(GetUserDefaultLCID(), lcType, szTempmb, sizeof(szTempmb)) != 0)
			szRet = szTempmb;
		else
			szRet = szDefault;
	}
	else
	{
		if ((*(lpfnGetLocaleInfoW))(GetUserDefaultLCID(), lcType, szTemp, _countof(szTemp)) != 0)
		{
			// GetLocaleInfoA not available on all platforms so we convert

			_wcstombsz(szTempmb, szTemp, _countof(szTempmb));
			szRet = szTempmb;
		}
		else
			szRet = szDefault;
	}

	if (hKernel)
		FreeLibrary(hKernel);

	return szRet;
}

UINT CCPLReg::GetCPLInt(LCTYPE lcType, int nDefault)
{
	CString strRegEntry;
	UINT nInt = nDefault;

	strRegEntry = GetCPLString(lcType, "DEFAULT");
	if (strRegEntry.Compare("DEFAULT") != 0)
	{
		const TCHAR* pch = strRegEntry;
		nInt = _ttoi(pch);
	}

	return nInt;
}

///////////////////////////////////////////////////////////////////////////////
//	Project saving stuff

static char BASED_CODE szLastProject[] = "LastProject";
extern BOOL g_bReloadProject;

BOOL CTheApp::LoadLastProject()
{
	BOOL bRet = FALSE;

	CString strPath;
	
	if (g_bReloadProject)
		strPath = GetRegString(szWorkspaceSection, szLastProject);

	if (!strPath.IsEmpty())
	{
		CDocTemplate* pTemplate = GetTemplate(CLSID_WorkspaceDocument);
		if (pTemplate != NULL)
		{
			theApp.m_bOpenedFromLastLoad = TRUE;
			bRet = (pTemplate->OpenDocumentFile(strPath) != NULL);
			theApp.m_bOpenedFromLastLoad = FALSE;
		}
	}

	// Make sure next instance does not just open the same project.
	CString strKeyMain = GetRegistryKeyName();
	RegDeleteKey(HKEY_CURRENT_USER, strKeyMain + chKeySep + szWorkspaceSection);

	return bRet;
}

BOOL CTheApp::SaveLastProject()
{
	CString strPath;

	IProjectWorkspace *lpWorkspaceWnd;
	lpWorkspaceWnd = g_IdeInterface.GetProjectWorkspace();
	if (lpWorkspaceWnd != NULL)
	{
		LPCSTR pszPath;
		VERIFY(SUCCEEDED(lpWorkspaceWnd->GetWorkspaceDocPathName(&pszPath)));

		CPath path;
		path.Create(pszPath);
		strPath = path.GetFullPath();
	}
	WriteRegString(szWorkspaceSection, szLastProject, strPath);

	return TRUE;
}

BOOL CTheApp::SerializeAllWorkspaceSettings(const CPath *pPath, DWORD dwFlags, BOOL bSave)
{
	// reset global file error
	SetFileError(CFileException::none);
	BOOL fRet = TRUE;

	if (bSave && m_bInvokedCommandLine)
	{
		return TRUE;	// must never touch .opt for cmd line build
	}			// because not all packages are loaded

	CStateSaver stateSave(bSave);
	CStateSaver *pSS = &stateSave;

	BOOL bConvert =	((pPath != NULL) && (!_tcsicmp(pPath->GetExtension(), ".mdp") || !_tcsicmp(pPath->GetExtension(), ".vcp")));

	if (pPath != NULL)
	{
		if (bConvert && !bSave)
		{
			ASSERT(!bSave);
			fRet = pSS->OpenMSF(*pPath, TRUE);
		}
		else
		{
			fRet = pSS->OpenStorage(*pPath);
		}
	}

	if (bSave || ((dwFlags & OPT_WORKSPACE_ONLY) != 0))
	{
		IProjectWorkspace *lpWorkspaceWnd;
		lpWorkspaceWnd = g_IdeInterface.GetProjectWorkspace();
		if (lpWorkspaceWnd != NULL)
		VERIFY(SUCCEEDED(lpWorkspaceWnd->SerializeWorkspaceState(pSS)));
		// ignore errors
	}

	if ((dwFlags & (OPT_DOCUMENTS_ONLY|OPT_WORKSPACE_ONLY)) == 0)
	{
		POSITION pos = m_packages.GetHeadPosition();
		while (pos != NULL)
		{
			CPackage* pPackage = (CPackage*)m_packages.GetNext(pos);
			if ((pPackage->m_flags & PKS_SERIALIZE) != 0)
			{
				TRY
				{
					// if we are loading from an old-style (binary) MDP file 
					// then give packages an opportunity to convert settings
					if (bConvert)
						pPackage->SerializeWkspcConvSettings(*pSS, dwFlags);
					else
						pPackage->SerializeWorkspaceSettings(*pSS, dwFlags);
				}
				CATCH_ALL(e)
				{
					// One of the packages blew its exception handling.  Catch the
					// exception here but continue the loop so that other packages
					// can serialize their settings.
					ASSERT(FALSE);
				}
				END_CATCH_ALL
			}
		}
	}

	// VShell OPT serializing.  Documents loaded alone, at end of project load.
	if ((bSave || ((dwFlags & OPT_DOCUMENTS_ONLY) != 0)) &&
		(!theApp.m_bInvokedCommandLine)) // optimize for cmdline bld
	{
		BOOL success;
		success = ((CMainFrame *)theApp.m_pMainWnd)->SerializeWorkspaceLayout(*pSS);
		if (!success && GetFileError())
			fRet = FALSE;
	}
	else if (GetFileError())
		fRet = FALSE;

	if ((pPath != NULL) && (bSave || !bConvert))
		pSS->CloseStorage();

	// Finish is only interesting for a save, and will happen in the CStateSaver
	// destructor if not done here.  Also, if we are loading, then we may have
	// gotten the CStateSaver from the project.
	if (bSave && !pSS->Finish())
		fRet = FALSE;

	// reset global file error
	SetFileError(CFileException::none);

	return fRet;
}

///////////////////////////////////////////////////////////////////////////////
//	CInitFile
//

CInitFile::CInitFile() : CMemFile()
{
	m_type = dataNil;
	m_bWrite = FALSE;

	m_pmsf = NULL;
	m_sn = 0;

	m_lpszSection = NULL;
	m_lpszKey = NULL;

	m_hglobUserMem = NULL;
	m_lpStream = NULL;
}

CInitFile::~CInitFile()
{
	// Do this here, since the v-table will not get to our code
	// from CMemFile.
	if (m_lpBuffer)
		Close();
	ASSERT(m_lpBuffer == NULL);
}

BOOL CInitFile::Open(LPCSTR szSection, LPCSTR szKey, UINT nOpenFlags,
	INIT_DATATYPE type /*=dataReg*/)
{
	ASSERT(nOpenFlags == modeRead || nOpenFlags == modeWrite);
	ASSERT(type == dataReg || type == dataResource);
	ASSERT(type == dataReg || nOpenFlags != modeWrite);	// No writing to resource.

	m_type = type;
	m_bWrite = (nOpenFlags == modeWrite);

	m_lpszSection = szSection;
	m_lpszKey = szKey;

	if (!m_bWrite)
		return LoadFromSource();

	return TRUE;
}

DWORD LimitHash(const char *str)
{
	//a standard hash function
	const char *sz;
	DWORD val = 0, g;
	for(sz = str; *sz != '\0'; sz++){
		val = (val << 4) + (*sz);
		if (g = val&0xf000000) {
			val = val ^ (g >> 24);
			val = val ^ g;
		}
	}
	return val;
}

BOOL CInitFile::Open(LPSTORAGE pStorage, CString strStream, UINT nOpenFlags)
{
	USES_CONVERSION;

	ASSERT(pStorage != NULL);
	ASSERT(nOpenFlags == modeRead || nOpenFlags == modeWrite);

	m_type = dataStorage;
	m_strStream = strStream;

	int nLength = m_strStream.GetLength();
	if (nLength > 31)
	{
		// OLE stream name length limit is 32 chars, so if the stream name requested
		// is longer than this limit, truncate the name and append name length to get
		// a "unique" stream name.
		// REVIEW [patbr]: it is possible to get a non-unique stream name if the stream
		// names requested are identical through 28 chars and of the same length.

		// SteveJS: changed length to result of a hash function to provide a better chance of a unique id
		// - also lowered limit to 31 char's as OpenStream and CreateStream seem to 
		// fail if the NULL is not within 32 chars.
		CString strLength;
		strLength.Format("%X", LimitHash(m_strStream));
		m_strStream = m_strStream.Left(31-strLength.GetLength());
		m_strStream += strLength;
	}

	HRESULT hr;
	if (nOpenFlags == modeRead)
		hr = pStorage->OpenStream(T2W(m_strStream), 0, STGM_DIRECT | STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &m_lpStream);
	else
		hr = pStorage->CreateStream(T2W(m_strStream), STGM_DIRECT | STGM_WRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE, 0, 0, &m_lpStream);
	if (FAILED(hr) || (m_lpStream == NULL))
		return FALSE;

	m_bWrite = (nOpenFlags == modeWrite);

	if (!m_bWrite)
		return LoadFromSource();

	return TRUE;
}

BOOL CInitFile::Open(MSF *pmsf, WORD sn, UINT nOpenFlags)
{
	ASSERT(pmsf != NULL);
	ASSERT(nOpenFlags == modeRead || nOpenFlags == modeWrite);

	m_type = dataMSF;
	m_bWrite = (nOpenFlags == modeWrite);

	m_pmsf = pmsf;
	m_sn = sn;

	if (!m_bWrite)
		return LoadFromSource();

	return TRUE;
}

// Using this function gives delete responsibility for the data to
// the file.
void CInitFile::SetBuffer(HGLOBAL hglob, UINT nCountBytes /*=-1*/,
	INIT_DATATYPE type /*=dataReg*/)
{
	ASSERT(m_nFileSize + m_nBufferSize == 0 && m_lpBuffer == NULL);
	ASSERT(m_type == type || m_type == dataNil);

	if (type == dataResource)
		m_lpBuffer = (BYTE FAR*) ::LockResource(hglob);
	else
		m_lpBuffer = (BYTE FAR*) ::GlobalLock(hglob);

	if (nCountBytes == -1)
	{
		ASSERT(type == dataReg);

		WORD FAR* lpw = (WORD FAR*) m_lpBuffer;
		nCountBytes = (UINT) *lpw + sizeof(WORD);
		ASSERT(nCountBytes != 0);
	}

	m_type = type;
	m_hglobUserMem = hglob;
	m_nFileSize = nCountBytes;
	m_nBufferSize = nCountBytes;

	if (type == dataReg)
		Seek(sizeof(WORD), begin);
}

HGLOBAL CInitFile::GetInitData()
{
	ASSERT(m_hglobUserMem == NULL);	// User already has this. Close.

	// This is equivalent to a write, so turn off the write flag
	// to keep the file from writing when we close.
	m_bWrite = FALSE;

	HGLOBAL hglob = ::GlobalAlloc(GMEM_MOVEABLE, m_nFileSize + sizeof(WORD));
	WORD FAR* lpw = (WORD FAR*) ::GlobalLock(hglob);
	*lpw++ = (WORD) m_nFileSize;
	Memcpy((BYTE FAR*)lpw, m_lpBuffer, m_nFileSize);
	::GlobalUnlock(hglob);

	return hglob;
}

void CInitFile::Close()
{
	if (m_bWrite && !WriteToDest())
		AfxThrowFileException(CFileException::generic);

	Abort();
}

void CInitFile::Abort()
{
	if (m_hglobUserMem != NULL)
	{
		if (m_type != dataResource)
		{
			::GlobalUnlock(m_hglobUserMem);
			::GlobalFree(m_hglobUserMem);
		}
		else
		{
#ifndef _WIN32
			::UnlockResource(m_hglobUserMem);
#endif
			::FreeResource(m_hglobUserMem);
		}

		m_hglobUserMem = NULL;
		m_lpBuffer = NULL;
		m_nBufferSize = 0;
		m_nFileSize = 0;
	}

	if (m_type == dataStorage)
	{
		ASSERT(m_lpStream != NULL);
		m_lpStream->Release();
		m_lpStream = NULL;
	}

	m_type = dataNil;
	m_bWrite = FALSE;
	m_pmsf = NULL;
	m_sn = 0;
	m_lpszSection = NULL;
	m_lpszKey = NULL;

	CMemFile::Close();
}

#define LARGEINT2DOUBLE(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

BOOL CInitFile::LoadFromSource()
{
	ASSERT (!m_bWrite);

	switch (m_type)
	{
		case dataMSF:
			//BLOCK:
			{
				CB cb = MSFGetCbStream(m_pmsf, m_sn);
				if ( cb != -1 )
				{
					SetLength((DWORD) cb);
					ASSERT(m_lpBuffer);

					if (MSFReadStream(m_pmsf, m_sn, m_lpBuffer, cb))
						return TRUE;
				}
			}
			break;

		case dataReg:
			//BLOCK:
			{
				HGLOBAL hglob = GetRegData(m_lpszSection, m_lpszKey, NULL);

				if (hglob != NULL)
				{
					WORD FAR* lpw = (WORD FAR*) ::GlobalLock(hglob);
					UINT nSize = (UINT) *lpw;
					ASSERT(nSize != 0);
					::GlobalUnlock(hglob);

					SetBuffer(hglob, nSize + sizeof(WORD));
					Seek(sizeof(WORD), CFile::begin);

					return TRUE;
				}
			}
			break;

		case dataResource:
			//BLOCK:
			{
				// Try to make a resource based file.
				HINSTANCE hinst = AfxFindResourceHandle(m_lpszKey, m_lpszSection);
				if (hinst != NULL)
				{
					HRSRC hrsrc = ::FindResource(hinst, m_lpszKey, m_lpszSection);
					if (hrsrc != NULL)
					{
						HGLOBAL hglob;
						UINT nSize = (UINT)	::SizeofResource(hinst, hrsrc);

						if (nSize != 0 &&
							(hglob = ::LoadResource(hinst, hrsrc)) != NULL)
						{
							SetBuffer(hglob, nSize, m_type);
							return TRUE;
						}
					}
				}
			}
			break;

		case dataStorage:
			//BLOCK:
			{
				STATSTG statstg;
				m_lpStream->Stat(&statstg, STATFLAG_DEFAULT);
				ULONG cb = (ULONG)LARGEINT2DOUBLE(statstg.cbSize);
				SetLength((DWORD)cb);
				ASSERT(m_lpBuffer);

				if (SUCCEEDED(m_lpStream->Read(m_lpBuffer, m_nBufferSize, &cb)))
				{
					ASSERT(m_nBufferSize == cb);
					return TRUE;
				}
			}
			break;

		default:
			return TRUE;
	}


	Abort();
	return FALSE;
}

BOOL CInitFile::WriteToDest()
{
	// Bail if no buffer or in read mode:
	if (!m_bWrite || m_lpBuffer == NULL)
		return TRUE;

	// Set mode to read, which will prevent another Flush during
	// destruction and possible recursion if an error occurs.
	m_bWrite = FALSE;

	switch (m_type)
	{
		case dataMSF:
			return MSFWriteStream(m_pmsf, m_sn, m_lpBuffer, m_nBufferSize);

		case dataReg:
			//BLOCK:
			{
				// Write the archive data preceded by a byte count to an hglob.
				HGLOBAL hglob = GetInitData();
				WriteRegData(m_lpszSection, m_lpszKey, hglob);
				::GlobalFree(hglob);
			}
			break;

		case dataStorage:
			//BLOCK:
			{
				DWORD dwWritten;
				HRESULT hr;
				hr = m_lpStream->Write(m_lpBuffer, m_nBufferSize, &dwWritten);
				if (SUCCEEDED(hr))
				{
					ASSERT(m_nBufferSize == dwWritten);
				}
				return(SUCCEEDED(hr));
			}
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CInitFile diagonstics

#ifdef _DEBUG
void CInitFile::AssertValid() const
{
	// don't call CMemFile::AssertValid as it doesn't know about r/o memory.
	CFile::AssertValid();

	ASSERT((m_lpBuffer == NULL && m_nBufferSize == 0) ||
		AfxIsValidAddress(m_lpBuffer, (UINT)m_nBufferSize, m_bWrite));
	ASSERT(m_nFileSize <= m_nBufferSize);
	// m_nPosition might be after the end of file, so we cannot ASSERT
	// its validity
}
#endif // _DEBUG

///////////////////////////////////////////////////////////////////////////////
//	CStateSaver
//

CStateSaver::CStateSaver(BOOL bSaving)
{
	m_bSaving = bSaving;
	m_pMSF = NULL;
	m_pSaver = NULL;
	m_lpStorage = NULL;
}

CStateSaver::~CStateSaver()
{
	if (m_pMSF != NULL)
		MSFClose(m_pMSF);

	if (m_pSaver != NULL)
		delete m_pSaver;
}

BOOL CStateSaver::OpenFile(CInitFile& fileInit, SN nStream,
	LPCSTR szSection, LPCSTR szKey)
{
	ASSERT(!m_bSaving);

	UINT nMode = CFile::modeRead;

	if (m_pMSF != NULL)
		return fileInit.Open(m_pMSF, nStream, nMode);
	else if (szSection != NULL || szKey != NULL)
		return fileInit.Open(szSection, szKey, nMode);

	return FALSE;
}

//	CStateSaver::Finish()
//		Always returns true if not saving.
BOOL CStateSaver::Finish()
{
	if (m_bSaving && m_pSaver != NULL)
		return m_pSaver->Finish();

    m_strFileName.Empty();

	return TRUE;
}

BOOL CStateSaver::OpenMSF(LPCTSTR szFilePath, BOOL bForceWriteable)
{
	ASSERT(m_pMSF == NULL);

	// Start from scratch when saving.
	if (m_bSaving)
	{
		if (m_pSaver != NULL)
			delete m_pSaver;

		m_pSaver = new CFileSaver(szFilePath);

		if (!m_pSaver->CanSave())
		{
			if (bForceWriteable)
			{
				if (!m_pSaver->MakeWriteable())
				{
					SetFileError(ferrCantSaveReadOnly);
					return FALSE;
				}
			}
			else
			{
				SetFileError(ferrCantSaveReadOnly);
				return FALSE;
			}
		}

		szFilePath = m_pSaver->GetSafeName();
	}

	m_pMSF = MSFOpen((LPTSTR) szFilePath, m_bSaving);
    m_strFileName = szFilePath;
	return(m_pMSF != NULL);
}

BOOL CStateSaver::OpenStorage(LPCTSTR szFilePath)
{
	USES_CONVERSION;

	// Start from scratch when saving.
	if (m_bSaving)
	{
		if (m_pSaver != NULL)
			delete m_pSaver;
		m_pSaver = new CFileSaver(szFilePath);

		if (!m_pSaver->CanSave() && !m_pSaver->MakeWriteable())
		{
			SetFileError(ferrCantSaveReadOnly);
			return FALSE;
		}
		szFilePath = m_pSaver->GetSafeName();
	}

	HRESULT hr;
	if (m_bSaving)
		hr = ::StgCreateDocfile(T2W(szFilePath), STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE, 0, &m_lpStorage);
	else
		hr = ::StgOpenStorage(T2W(szFilePath), NULL, STGM_DIRECT | STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, 0, &m_lpStorage);
	return((SUCCEEDED(hr)) && (m_lpStorage != NULL));
}

BOOL CStateSaver::OpenStream(CInitFile& fileInit, CString strStream)
{
	UINT nMode = (m_bSaving ? CFile::modeWrite : CFile::modeRead);

	if (m_lpStorage != NULL)
		return(fileInit.Open(m_lpStorage, strStream, nMode));

	return FALSE;
}

void CStateSaver::CloseStorage()
{
	if (m_lpStorage != NULL)
		m_lpStorage->Release();
	m_lpStorage = NULL;
}


