#include "stdafx.h"
#include "mfcappwz.h"
#include "symbols.h"
#include "lang.h"
#include "ddxddv.h"
#include "platname.h"
#include "utilbld_.h"
#include "awiface.h"
#ifndef VS_PACKAGE
#include <utilctrl.h>
#endif	// VS_PACKAGE

#include <initguid.h>
#include <bldguid.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef VS_PACKAGE
	static const TCHAR* szModuleSubdir1 = "\\";
#else
	#ifdef _DEBUG
		static const TCHAR* szModuleSubdir1 = "\\ided\\";
	#else
		static const TCHAR* szModuleSubdir1 = "\\ide\\";
	#endif //_DEBUG
#endif //VS_PACKAGE

#ifdef _DEBUG
static char BASED_CODE szCtlWiz[] = "mfctlwzd.awx";
#else
static char BASED_CODE szCtlWiz[] = "mfctlwz.awx";
#endif //_DEBUG

static const TCHAR* szModuleSubdir2 = "Template";
static char BASED_CODE szMacSubsection[] = "\\Platforms\\Macintosh";
static char BASED_CODE szMacOleKey[] = "OLE present";
static char BASED_CODE szMacODBCKey[] = "ODBC present";
static char BASED_CODE szMacWOSAKey[] = "WOSA present";

//extern char BASED_CODE szMSVCSection[];

// This maps the extension title to the DLL-name.  It's here to remember what
//  extensions AppWizard has inserted into CPromtpDlg
static CMapStringToString g_CustomAppWizards;

extern CPlatNameConvert g_PlatNameConvert;

// This function is defined in miscdlgs.cpp, and is used in OnPromptDlgOK()
BOOL ScanForLangDll();

//  Defined in mfcappwz.cpp
BOOL DoesFileExist(const char* szFile);

CAppWizIFace g_AppWizIFace;	// one 'n' only appwiz interface
//CAppWizIFace AFX_EXT_DATA* g_pAppWizIFace;	// exported pointer to 'this'

extern HINSTANCE GetResourceHandle();

extern "C" CAppWizIFace* WINAPI GetAppWizIFace()
{
	return &g_AppWizIFace;
}


BOOL IsReadOnly(const char* szFile)
{
	struct _stat st;
	if (_stat(szFile, &st) == 0)
		return (!(st.st_mode & _S_IWRITE));
	else
		return FALSE;
}

BOOL CheckIsReadOnly(const char* szFile)
{
	CString strPrompt;
	AfxFormatString1(strPrompt, IDP_FILE_IS_READONLY, szFile);
	while (IsReadOnly(szFile))
	{
		if (AfxMessageBox(strPrompt, MB_RETRYCANCEL | MB_ICONEXCLAMATION) == IDCANCEL)
			return FALSE;
	}
	return TRUE;
}


/*BOOL RegMacSectionKeyExists(TCHAR* szKey)
{
	CString strSection = szMSVCSection;
	strSection += szMacSubsection;
	HKEY hkey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, strSection, 0, KEY_READ, &hkey) != ERROR_SUCCESS)
		return FALSE;

	BOOL bReturn = (RegQueryValueEx(hkey, szKey, NULL, NULL, NULL, NULL)
						== ERROR_SUCCESS);

	RegCloseKey(hkey);
	return bReturn;
}*/


BOOL IsMacOle()
{
	/*static BOOL bFirstTime = TRUE;
	static BOOL bIsMacOle = FALSE;
	
	if (bFirstTime)
	{
		bFirstTime = FALSE;
		bIsMacOle = RegMacSectionKeyExists(szMacOleKey);
	}
	return bIsMacOle;*/
	return TRUE;
}

BOOL IsMacOcx()
{
	return TRUE;
}

BOOL IsMacODBC()
{
	/*static BOOL bFirstTime = TRUE;
	static BOOL bIsMacODBC = FALSE;
	
	if (bFirstTime)
	{
		bFirstTime = FALSE;
		bIsMacODBC = RegMacSectionKeyExists(szMacODBCKey);
	}
	return bIsMacODBC;*/
	return TRUE;
}



BOOL IsMacWOSA()
{
	/*static BOOL bFirstTime = TRUE;
	static BOOL bIsMacWOSA = FALSE;
	
	if (bFirstTime)
	{
		bFirstTime = FALSE;
		bIsMacWOSA = RegMacSectionKeyExists(szMacWOSAKey);
	}
	return bIsMacWOSA;*/
	return FALSE;
}

// If the project name is different from before, then we re-run FillDefaultNames.
void UpdateNamesIfNecessary(NAMES& names, const char* pszRoot)
{
	if (CreateSymbol("root") == pszRoot)
		return;
		
	names.m_bUpdateNames = TRUE;
	SetSymbol("ROOT", Upper(pszRoot));
	SetSymbol("root", pszRoot);
	SetSymbol("Root", pszRoot);

	// need an OEM version of root for bat files...
	CString strRootOEM = pszRoot;
	strRootOEM.AnsiToOem();
	SetSymbol("root_oem", strRootOEM);

	FillDefaultNames(names, pszRoot);
}


/////////////////////////////////////////////////////////////////////////////
// Functions to find & apply USER EXTENSIONS

// Given a filename, this function determines whether it's a valid user extension
BOOL IsUserExtension(const TCHAR* tszFileName)
{
	return TRUE;
}



BOOL GetExtensionDescription(CString& rstrDescription, const CString& strExtension, HICON& hIcon)
{
	UINT dwDummySize;
	LPVOID lpBuffer;
	DWORD dwHandle;

	// NT can be an idiot sometimes; versioninfo APIs won't take a const string
	CString strCopyExtension = strExtension;

	DWORD dwSize = GetFileVersionInfoSize((LPTSTR) (LPCTSTR) strCopyExtension, &dwHandle);
	if (dwSize == 0)
		return FALSE;
	CString strBuffer;
	LPTSTR szBuf = strBuffer.GetBuffer(dwSize);
	if (!GetFileVersionInfo((LPTSTR) (LPCTSTR) strCopyExtension, dwHandle, dwSize, szBuf)
		|| !VerQueryValue(szBuf, "\\VarFileInfo\\Translation", &lpBuffer, &dwDummySize)
		|| dwSize == 0 || dwDummySize == 0)
	{
		strBuffer.ReleaseBuffer();
		return FALSE;
	}

	// We've successfully read the versioninfo stuff.  Now translate the
	//  the langid into a localized string of the language name

	char szName[512];
	wsprintf(szName, "\\StringFileInfo\\%04x%04x\\ProductName",
		*((WORD*)lpBuffer), *((WORD*)lpBuffer+1));
	if (!VerQueryValue(szBuf, szName, &lpBuffer, &dwDummySize))
	{
		strBuffer.ReleaseBuffer();
		return FALSE;
	}
	
	rstrDescription = (char*) lpBuffer;
	strBuffer.ReleaseBuffer();

	hIcon = ExtractIcon(GetResourceHandle(), strCopyExtension, 0);

	return TRUE;
}

// All TARGET_ macros should be set by the time this is called.
void FillTruncatedDefaultNames(LPCTSTR pszRoot)
{
	CString filebase = pszRoot;

	// Now set all filenames that must be abbreviated
	if (DoesSymbolExist("TARGET_MAC"))
	{
		// Because hc35 doesn't accept long file names, everyone has to suffer
		//  when we target the MAC for a help app.  In particular, the .hm
		//  file can't exceed 8.3
		filebase = filebase.Left(8);
	}
	SetSymbol("HM_FILE", filebase);
	CString filebaseOEM = filebase;

	// batch files use the OEM character set.  Everything else uses the ANSI character
	// set.  If we've got upper ASCII, there is a difference between the two.
	filebaseOEM.AnsiToOem();
	SetSymbol("HM_FILE_OEM", filebaseOEM);
	if (filebase.Compare(filebaseOEM))	
		SetSymbol("HM_NOTE", "1");	// we have upper ASCII, so need a note about why all the
									// file names looks wierd in makehelp.bat
	else
		RemoveSymbol("HM_NOTE");

	filebase = filebase.Left(5);
	SetSymbol("mac_hpj", filebase + "Mac");
}

// Helper used in RecordPlatforms.  Takes longname of single platform, and updates
//  g_PlatformsSelected as it sets the appropriate symbols.
void SetAppWizSymbolsForPlatform(LPCTSTR szPlatformLong)
{			
	if (szPlatformLong == NULL)
		// This happens if an extension supports "no" platforms.  In that case,
		//  we default to supporting Win32.
		szPlatformLong = szPlatforms[WIN];

	CString strValue;
	SetSymbol(szPlatformLong, "1");	// Set symbol w/ platform name

	if (g_PlatformsSelected.Lookup(szPlatformLong, strValue))
	{
		// This platform is stored in our g_PlatformsSelected map.
		if (strValue != "1")
		{
			// There's an abbreviation stored for it ("TARGET_68KMAC",
			//  etc.) so set the abbreviation as well
			SetSymbol(strValue, "1");
		}
	}
	else
	{
		// The platform wasn't stored, so we must store it now.
		//  That way, next time around, we'll know to remove its
		//  corresponding symbol.
		g_PlatformsSelected[szPlatformLong] = "1";
	}
}

// This is called from CPromptDlg's OnOK.  It takes a look at what
//  the user checked in the platforms list, and sets the appropriate symbols.
//  g_PlatformsSelected is used & updated as appropriate
void RecordPlatforms(int* pnPlatforms)
{
	// First, remove platform symbols previously checked.  We use
	//  g_PlatformsSelected as our guide
	POSITION pos = g_PlatformsSelected.GetStartPosition();
	while (pos != NULL)
	{
		CString strKey, strValue;
		g_PlatformsSelected.GetNextAssoc(pos, strKey, strValue);
		RemoveSymbol(strKey);
		RemoveSymbol(strValue);
	}

	// Now, go through the checklist, and set the checked platforms, using
	//  g_PlatformsSelected as our guide, and updating it along the way
	int nCount = g_strlPlatformsDisplayed.GetCount();
	ASSERT (nCount <= MAX_PLATFORMS);
	if (nCount == 0)
	{
		// We have an extension that displayed no platforms
		SetAppWizSymbolsForPlatform(NULL);
	}
	else
	{
		// There exist platforms; thus we will have made sure something was selected
		POSITION pos = g_strlPlatformsDisplayed.GetHeadPosition();
		int i=0;
		while (pos != NULL)
		{
			CString strPlatform = g_strlPlatformsDisplayed.GetNext(pos);
			// We only give a hoot if the user checked the platform
			if (pnPlatforms[i++] == 1)
			{
				g_PlatNameConvert.UItoLong(strPlatform, strPlatform);
				SetAppWizSymbolsForPlatform(strPlatform);
			}
		}
	}

	// If either mac is selected, set TARGET_MAC.
	if (DoesSymbolExist("TARGET_68KMAC") || DoesSymbolExist("TARGET_POWERMAC"))
		SetSymbol("TARGET_MAC", "1");
	else
		RemoveSymbol("TARGET_MAC");

	// Set name macros which depend on target
	FillTruncatedDefaultNames(projOptions.m_strProj);
}

// This prevents naming conflicts caused by project names such "xxxxxdoc"
//  The check is extra conservative.  We don't know what appwiz options
//  the user will choose, so we don't know what files may conflict.  So,
//  we check ALL possible conflicts.  If, instead, we waited until we DO know,
//  that would cause the warning to be displayed just before creation,
//  which is pretty late in the game and darned annoying for the user.
static BOOL InterfereWithDefaults(const char* szFile, BOOL bMacSelected, BOOL bExtwiz)
{
	CString strFile = szFile;
	strFile.MakeLower();
	int nLength = strFile.GetLength();
 	CString strThree = strFile.Right(3);
	
	return (strFile == "mainfrm"
		|| strFile == "childfrm"
		|| strFile == "ipframe"
		|| strFile == "cntritem"
		|| strFile == "srvritem"
		|| strFile == "stdafx"
		|| strFile == "resource"
		|| (bMacSelected && nLength == 8 && strThree == "mac")
		|| (bExtwiz && (strFile == "custom" || strFile == "debug" || strFile == "chooser")));
}
static BOOL InterfereWithDefaultsOCX(LPCTSTR szFile)
{
	CString strFile = szFile;
	strFile.MakeLower();
	int nLength = strFile.GetLength();
	
	return (strFile == "stdafx"
		|| strFile == "resource");
}



// GLOBAL APPWIZ APIS

#ifdef VS_PACKAGE
void CAppWizIFace::InitAppWiz(HWND hWnd, IServiceProvider *pSp)
{
	ReleasePtrs();

	ASSERT(pSp);

	m_pSp = pSp ;
	m_pSp->AddRef() ;

	VERIFY(SUCCEEDED(pSp->QueryService(SID_SBuildService, IID_IBuildWizardX, (LPVOID *) &m_pBldWizX)));
	VERIFY(SUCCEEDED(pSp->QueryService(SID_SBuildService, IID_IBuildPlatformsX, (LPVOID *) &m_pBldPlatsX)));

	theDLL.InitAppWiz(hWnd);
}
#else
void CAppWizIFace::InitAppWiz(HWND hWnd)
{
	theDLL.InitAppWiz(hWnd);
}
#endif

int CAppWizIFace::RunAppWizSteps()
{
	return theDLL.RunAppWizSteps();
}

// This is implemented in mfcappwz.cpp
void CreateProject(HBUILDER hBld);
extern AppWizTermStatus g_TermStatus;

void CAppWizIFace::AddAppWizTargets(HBUILDER hBld)
{
    pTermStatus = &g_TermStatus;
    do
    {
        CreateProject(hBld);
        pTermStatus = pTermStatus->nextAppWizTermStatus;
    } while (pTermStatus);  
}

BOOL CAppWizIFace::GetAppWizDialog( CString &strDialog )
{
	GetSymbol("APPWIZ_INITIAL_DIALOG", strDialog );
	return !strDialog.IsEmpty();
}

void CAppWizIFace::ExitAppWiz()
{
	theDLL.ExitAppWiz();
    
    // delete all the newly added stuff for multiple projects
    pTermStatus = g_TermStatus.nextAppWizTermStatus;
    while (pTermStatus) {
        AppWizTermStatus *pPrevAppWizTermStatus;
        pPrevAppWizTermStatus = pTermStatus;
        pTermStatus = pTermStatus->nextAppWizTermStatus;
        delete (pPrevAppWizTermStatus);
    }
    memset(g_TermStatus.szFullName, '\0', MAX_PATH);

#ifdef VS_PACKAGE

	ReleasePtrs() ;

#endif
}


#ifndef VS_PACKAGE
// CPROMPTDLG HOOKS

int CAppWizIFace::AddAppWizProjectTypesAtTop(CListBox* pList)
{
	CString strPTEntry;
	strPTEntry.LoadString(IDS_APPWIZ_EXE);
	pList->InsertString(0, strPTEntry);
	pList->SetItemData(0, (DWORD)LoadIcon(GetResourceHandle(), MAKEINTRESOURCE(IDR_MFCAPPWIZARD)));

	strPTEntry.LoadString(IDS_APPWIZ_DLL);
	pList->InsertString(1, strPTEntry);
	pList->SetItemData(1, (DWORD)LoadIcon(GetResourceHandle(), MAKEINTRESOURCE(IDR_MFCAPPWIZARDDLL)));

	// If we can find ControlWizard, then add the ControlWizard choice
	//  as well.
	CString strCtlWizFull = (theDLL.m_strAppwizDir + szModuleSubdir1) + szCtlWiz;

	DWORD dwCtlWizAttr = GetFileAttributes(strCtlWizFull);
	if (dwCtlWizAttr != 0xffffffff && !(dwCtlWizAttr & FILE_ATTRIBUTE_DIRECTORY))
	{
		strPTEntry.LoadString(IDS_CTLWIZ_TITLE);
		pList->InsertString(2, strPTEntry);
		//pList->SetItemData(2, (DWORD)LoadIcon(GetResourceHandle(), MAKEINTRESOURCE(IDR_OLECONTROL)));
		pList->SetItemData(2, (DWORD)ExtractIcon(GetResourceHandle(), strCtlWizFull, 0));
		return NUM_PROJECTS;
	}
	else
		return NUM_PROJECTS-1;
}

int CAppWizIFace::AddAppWizProjectTypesAtTop(CProjTypeList* pList)
{
	CString strPTEntry;
	strPTEntry.LoadString(IDS_APPWIZ_EXE);
	pList->InsertString(0, strPTEntry);
	pList->SetItemDataPtr(0, (void *)LoadIcon(GetResourceHandle(), MAKEINTRESOURCE(IDR_MFCAPPWIZARD)));

	strPTEntry.LoadString(IDS_APPWIZ_DLL);
	pList->InsertString(1, strPTEntry);
	pList->SetItemDataPtr(1, (void *)LoadIcon(GetResourceHandle(), MAKEINTRESOURCE(IDR_MFCAPPWIZARDDLL)));

	// If we can find ControlWizard, then add the ControlWizard choice
	//  as well.
	CString strCtlWizFull = (theDLL.m_strAppwizDir + szModuleSubdir1) + szCtlWiz;
	DWORD dwCtlWizAttr = GetFileAttributes(strCtlWizFull);
	if (dwCtlWizAttr != 0xffffffff && !(dwCtlWizAttr & FILE_ATTRIBUTE_DIRECTORY))
	{
		strPTEntry.LoadString(IDS_CTLWIZ_TITLE);
		pList->InsertString(2, strPTEntry);
		pList->SetItemDataPtr(2, (void *)ExtractIcon(GetResourceHandle(), strCtlWizFull, 0));
		return NUM_PROJECTS;
	}
	else
		return NUM_PROJECTS-1;
}

static int AddCAWsFromHandle(CListBox* pList, HANDLE hSearch, WIN32_FIND_DATA& ffd,
	CString& strDir)
{
	int nReturn = 0;
	ASSERT(hSearch != INVALID_HANDLE_VALUE);
	do
	{
		CString strDescription;
		CString strDllName = strDir + ffd.cFileName;
		if (ffd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY
			&& IsUserExtension(strDllName))
		{
			HICON hIcon = NULL;
			if (GetExtensionDescription(strDescription, strDllName, hIcon))
			{
				// TODO: Take this if out once mfcctlwz.dll is dropped into bin
				if (_tcsicmp(ffd.cFileName, szCtlWiz))
				{
					int nItem = pList->AddString(strDescription);
					pList->SetItemData(nItem, (DWORD)hIcon);

					nReturn++;
					g_CustomAppWizards[strDescription] = strDllName;
				}
				else
					g_CustomAppWizards[szCtlWiz] = strDllName;
			}
			else
			{
				int nItem = pList->AddString(ffd.cFileName);
				pList->SetItemData(nItem, (DWORD)hIcon);
				nReturn++;
				g_CustomAppWizards[ffd.cFileName] = strDllName;
			}
		}
	}
	while (::FindNextFile(hSearch, &ffd));
	return nReturn;
}

static int AddCAWsFromHandle(CProjTypeList* pList, HANDLE hSearch, WIN32_FIND_DATA& ffd,
	CString& strDir)
{
	int nReturn = 0;
	ASSERT(hSearch != INVALID_HANDLE_VALUE);
	do
	{
		CString strDescription;
		CString strDllName = strDir + ffd.cFileName;
		if (ffd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY
			&& IsUserExtension(strDllName))
		{
			HICON hIcon = NULL;
			if (GetExtensionDescription(strDescription, strDllName, hIcon))
			{
				// TODO: Take this if out once mfcctlwz.dll is dropped into bin
				if (_tcsicmp(ffd.cFileName, szCtlWiz))
				{
					int nItem = pList->AddString(strDescription);
					pList->SetItemDataPtr(nItem, (void *)hIcon);

					nReturn++;
					g_CustomAppWizards[strDescription] = strDllName;
				}
				else
					g_CustomAppWizards[szCtlWiz] = strDllName;
			}
			else
			{
				int nItem = pList->AddString(ffd.cFileName);
				pList->SetItemDataPtr(nItem, (void *)hIcon);
				nReturn++;
				g_CustomAppWizards[ffd.cFileName] = strDllName;
			}
		}
	}
	while (::FindNextFile(hSearch, &ffd));
	return nReturn;
}

// This takes the projtype combo box, and fills it with any custom appwiz's it finds.
//  It returns whether it found any custom AppWiz's.
int CAppWizIFace::AddCustomAppWizProjectTypesAtBottom(CListBox* pList)
{
	int nReturn = 0;			// Counts number of custom appwizzes we put in the combo
	g_CustomAppWizards.RemoveAll();
	WIN32_FIND_DATA ffd;
	CString strDir;

	// Search \msvc30\bin\ide(d) dir for setup-installed CAWs
	strDir = theDLL.m_strAppwizDir + szModuleSubdir1;
	HANDLE hSearch = ::FindFirstFile(strDir + "*.AWX", &ffd);
	if (hSearch	!= INVALID_HANDLE_VALUE)
		nReturn += AddCAWsFromHandle(pList, hSearch, ffd, strDir);
	::FindClose(hSearch);

	// Search \msvc30\Template dir for user-created CAWs
	CDir dir;
	dir.CreateFromString(theDLL.m_strAppwizDir);		// \msvc30\bin
	dir.RemoveLastSubdirName();							// \msvc30
	dir.AppendSubdirName(szModuleSubdir2);				// \msvc30\Template
	strDir = (LPCTSTR) dir;
	if (*_tcsdec((LPCTSTR) strDir, (LPCTSTR) strDir + strDir.GetLength()) != '\\')
		strDir += '\\';
	hSearch = ::FindFirstFile(strDir + "*.AWX", &ffd);
	if (hSearch	!= INVALID_HANDLE_VALUE)
		nReturn += AddCAWsFromHandle(pList, hSearch, ffd, strDir);
	::FindClose(hSearch);
	
	return nReturn;
}


// Same but using a CProjTypeIconList* 
int CAppWizIFace::AddCustomAppWizProjectTypesAtBottom(CProjTypeList* pList)
{
	int nReturn = 0;			// Counts number of custom appwizzes we put in the combo
	g_CustomAppWizards.RemoveAll();
	WIN32_FIND_DATA ffd;
	CString strDir;

	// Search \msvc30\bin\ide(d) dir for setup-installed CAWs
	strDir = theDLL.m_strAppwizDir + szModuleSubdir1;
	HANDLE hSearch = ::FindFirstFile(strDir + "*.AWX", &ffd);
	if (hSearch	!= INVALID_HANDLE_VALUE)
		nReturn += AddCAWsFromHandle(pList, hSearch, ffd, strDir);
	::FindClose(hSearch);

	// Search \msvc30\Template dir for user-created CAWs
	CDir dir;
	dir.CreateFromString(theDLL.m_strAppwizDir);		// \msvc30\bin
	dir.RemoveLastSubdirName();							// \msvc30
	dir.AppendSubdirName(szModuleSubdir2);				// \msvc30\Template
	strDir = (LPCTSTR) dir;
	if (*_tcsdec((LPCTSTR) strDir, (LPCTSTR) strDir + strDir.GetLength()) != '\\')
		strDir += '\\';
	hSearch = ::FindFirstFile(strDir + "*.AWX", &ffd);
	if (hSearch	!= INVALID_HANDLE_VALUE)
		nReturn += AddCAWsFromHandle(pList, hSearch, ffd, strDir);
	::FindClose(hSearch);
	
	return nReturn;
}

void CAppWizIFace::FillPlatformsListFromCustomAppWizType(LPCTSTR szExtName, CCheckList* pChklstPlatforms)
{
	// Update the list with what the extension allows us to.  First,
	//  load the extension.
	
	// Start by converting description (szExtName) to the DLL name
	CString strDllName;
	VERIFY(g_CustomAppWizards.Lookup(szExtName, strDllName));

	// Then load 'er up!
	if (!projOptions.m_UserExtension.Attach(strDllName))
		AfxThrowUserException();
	// Special case: if ControlWizard, use "MFC AppWizard" as title; otherwise, use
	//  what the awx gave us.
	if (!_tcsicmp(szExtName, szCtlWiz))
	{
		projOptions.m_UserExtension.m_strExtensionTitle.LoadString(IDS_CTLWIZ_TITLE);
		projOptions.m_UserExtension.m_bCtlWiz = TRUE;
	}
	else
		projOptions.m_UserExtension.m_strExtensionTitle = szExtName;

	// Next, get all available platforms
	LPBUILDPLATFORMS pPlatforms;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_IBuildPlatforms, (LPVOID FAR *)&pPlatforms)));
	const CStringList* pPlatformsLong;
	pPlatforms->GetAvailablePlatforms(&pPlatformsLong, NULL, FALSE);
	pPlatforms->Release();
	// Copy to a new list the extension can write to.  If this extension makes
	//  a DLL, be sure not to include the 68K Mac platform.
	BOOL bIsDLL = (DoesSymbolExist("PROJTYPE_DLL") || DoesSymbolExist("PROJTYPE_OCX"));
    BOOL bIsXbox = DoesSymbolExist("PROJTYPE_XBOX");
#if 0
    BOOL fIsJava = DoesSymbolExist("PROJTYPE_JAVA");
#else
	BOOL fIsJava = FALSE;
#endif
	CStringList PlatformsLongCopy;
    {
	    POSITION pos = pPlatformsLong->GetHeadPosition();
	    while (pos != NULL)
	    {
		    CString strPlatLong = pPlatformsLong->GetNext(pos);
		    if (bIsDLL && strPlatLong == szPlatforms[MAC])
			    continue;

            if (!fIsJava && strPlatLong == szPlatforms[JAVA])
                continue;

            if (!bIsXbox && strPlatLong == szPlatforms[XBOX])
                continue;

		    PlatformsLongCopy.AddTail(strPlatLong);
	    }
    }

	// Consult the extension
	GetAWX()->GetPlatforms(PlatformsLongCopy);

	// Now fill in checklist with what's left in PlatformsLongCopy
	//  (converted to UI names if possible).  If this extension makes
	//  a DLL, be sure not to include the 68K Mac platform.
	g_strlPlatformsDisplayed.RemoveAll();
	POSITION pos = PlatformsLongCopy.GetHeadPosition();
	while (pos != NULL)
	{
		CString strPlatLong, strPlatUI;
		strPlatLong = PlatformsLongCopy.GetNext(pos);

		// Don't allow 68K Mac with a DLL
		if (bIsDLL && strPlatLong == szPlatforms[MAC])
			continue;

		g_PlatNameConvert.LongToUI(strPlatLong, strPlatUI);
		int i = pChklstPlatforms->InsertString(-1, strPlatUI);
		g_strlPlatformsDisplayed.AddTail(strPlatUI);	// Remember what we store for later
	}
}

void CAppWizIFace::FillPlatformsListFromAppWizType(int nProjType, CCheckList* pChklstPlatforms)
{
	// Special case: project type 2 == ocx, which is really a custom appwiz.
	if (nProjType == 2)
	{
		FillPlatformsListFromCustomAppWizType(szCtlWiz, pChklstPlatforms);
		return;
	}

	// This is an appwiz project, so we'll fill the platforms
	//  checklist with what the IDE would have given us, had
	//  the project type been "Application" or "Dynamic Link Library".
	// This is done so that we check the correct msvc key for whether
	//  Mac is installed.  If appwiz just went and checked on its own,
	//  it wouldn't know what key to use (i.e., whether we're launched
	//  by a sniff test).  Also, this way, all platform strings are gotten
	//  from the IDE rather than being hard coded here.
	//  This is also necessary so that we can get the localized
	//  string for the platforms, rather than hardcoding our own
	//  English version here.
	// This requires knowledge of the project types passed to us, by the
	//  IDE.  If the order or names ever change, we must update
	//  these hard-coded numbers.

	// First, be sure we're using our own symbol table.  (Soon symbols like
	//  "TARGET_POWERMAC" will be set, and we need to make sure they're set
	//  in AppWizard's own dictionary, rather than some extension we're
	//  going to detach later on.)
	projOptions.m_UserExtension.Detach();

	// Remember target type for when we create it later on
	pTermStatus->nProjType = nProjType;


	g_strlPlatformsDisplayed.RemoveAll();

	LPBUILDPLATFORMS pPlatforms;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_IBuildPlatforms, (LPVOID FAR *)&pPlatforms)));
	const CStringList* pPlatformsUI;
	pPlatforms->GetAvailablePlatforms(&pPlatformsUI, strlProjTypes.GetAt(strlProjTypes.FindIndex(nProjType)), TRUE);
	const CStringList* pPlatformsLong;
	pPlatforms->GetAvailablePlatforms(&pPlatformsLong, strlProjTypes.GetAt(strlProjTypes.FindIndex(nProjType)), FALSE);
	pPlatforms->Release();

	POSITION posUI = pPlatformsUI->GetHeadPosition();
	POSITION posLong = pPlatformsLong->GetHeadPosition();
	while (posUI != NULL && posLong != NULL)
	{
		CString strUI = pPlatformsUI->GetNext(posUI);
		CString strLong = pPlatformsLong->GetNext(posLong);

		if (strLong == szPlatforms[WIN]
				// This is the Win32 platform-- used for exe or dll
			|| (strLong == szPlatforms[MAC] && nProjType == 0)
				// This is the Macintosh platform-- used only for exe
			|| (strLong == szPlatforms[POWERMAC]))
				// This is the PowerMac platform-- used for exe or dll
		{
			pChklstPlatforms->InsertString(-1, strUI);
			g_strlPlatformsDisplayed.AddTail(strUI);	// Remember what we store for later
		}
	}
}

void CAppWizIFace::FillPlatformsListFromCustomAppWizType(LPCTSTR szExtName, CStringArray* pChklstPlatforms)
{
	// Update the list with what the extension allows us to.  First,
	//  load the extension.
	
	// Start by converting description (szExtName) to the DLL name
	CString strDllName;
	VERIFY(g_CustomAppWizards.Lookup(szExtName, strDllName));

	// Then load 'er up!
	if (!projOptions.m_UserExtension.Attach(strDllName))
		AfxThrowUserException();
	// Special case: if ControlWizard, use "MFC AppWizard" as title; otherwise, use
	//  what the awx gave us.
	if (!_tcsicmp(szExtName, szCtlWiz))
	{
		projOptions.m_UserExtension.m_strExtensionTitle.LoadString(IDS_CTLWIZ_TITLE);
		projOptions.m_UserExtension.m_bCtlWiz = TRUE;
	}
	else
		projOptions.m_UserExtension.m_strExtensionTitle = szExtName;

	// Next, get all available platforms
	LPBUILDPLATFORMS pPlatforms;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_IBuildPlatforms, (LPVOID FAR *)&pPlatforms)));
	const CStringList* pPlatformsLong;
	pPlatforms->GetAvailablePlatforms(&pPlatformsLong, NULL, FALSE);
	pPlatforms->Release();
	// Copy to a new list the extension can write to.  If this extension makes
	//  a DLL, be sure not to include the 68K Mac platform.
	BOOL bIsDLL = (DoesSymbolExist("PROJTYPE_DLL") || DoesSymbolExist("PROJTYPE_OCX"));
    BOOL bIsXbox = DoesSymbolExist("PROJTYPE_XBOX");
#if 0
    BOOL fIsJava = DoesSymbolExist("PROJTYPE_JAVA");
#else
	BOOL fIsJava = FALSE;
#endif
	CStringList PlatformsLongCopy;
    {
	    POSITION pos = pPlatformsLong->GetHeadPosition();
	    while (pos != NULL)
	    {
		    CString strPlatLong = pPlatformsLong->GetNext(pos);
		    if (bIsDLL && strPlatLong == szPlatforms[MAC])
			    continue;

            if (!fIsJava && strPlatLong == szPlatforms[JAVA])
                continue;

            if (!bIsXbox && strPlatLong == szPlatforms[XBOX])
                continue;

		    PlatformsLongCopy.AddTail(strPlatLong);
	    }
    }

	// Consult the extension
	GetAWX()->GetPlatforms(PlatformsLongCopy);

	// Now fill in checklist with what's left in PlatformsLongCopy
	//  (converted to UI names if possible).  If this extension makes
	//  a DLL, be sure not to include the 68K Mac platform.
	g_strlPlatformsDisplayed.RemoveAll();
	POSITION pos = PlatformsLongCopy.GetHeadPosition();
	while (pos != NULL)
	{
		CString strPlatLong, strPlatUI;
		strPlatLong = PlatformsLongCopy.GetNext(pos);

		// Don't allow 68K Mac with a DLL
		if (bIsDLL && strPlatLong == szPlatforms[MAC])
			continue;

		g_PlatNameConvert.LongToUI(strPlatLong, strPlatUI);
		int i = pChklstPlatforms->Add(strPlatUI);
		g_strlPlatformsDisplayed.AddTail(strPlatUI);	// Remember what we store for later
	}
}

void CAppWizIFace::FillPlatformsListFromAppWizType(int nProjType, CStringArray* pChklstPlatforms)
{
	// Special case: project type 2 == ocx, which is really a custom appwiz.
	if (nProjType == 2)
	{
		FillPlatformsListFromCustomAppWizType(szCtlWiz, pChklstPlatforms);
		return;
	}

	// This is an appwiz project, so we'll fill the platforms
	//  checklist with what the IDE would have given us, had
	//  the project type been "Application" or "Dynamic Link Library".
	// This is done so that we check the correct msvc key for whether
	//  Mac is installed.  If appwiz just went and checked on its own,
	//  it wouldn't know what key to use (i.e., whether we're launched
	//  by a sniff test).  Also, this way, all platform strings are gotten
	//  from the IDE rather than being hard coded here.
	//  This is also necessary so that we can get the localized
	//  string for the platforms, rather than hardcoding our own
	//  English version here.
	// This requires knowledge of the project types passed to us, by the
	//  IDE.  If the order or names ever change, we must update
	//  these hard-coded numbers.

	// First, be sure we're using our own symbol table.  (Soon symbols like
	//  "TARGET_POWERMAC" will be set, and we need to make sure they're set
	//  in AppWizard's own dictionary, rather than some extension we're
	//  going to detach later on.)
	projOptions.m_UserExtension.Detach();

	// Remember target type for when we create it later on
	pTermStatus->nProjType = nProjType;


	g_strlPlatformsDisplayed.RemoveAll();

	LPBUILDPLATFORMS pPlatforms;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_IBuildPlatforms, (LPVOID FAR *)&pPlatforms)));
	const CStringList* pPlatformsUI;
	pPlatforms->GetAvailablePlatforms(&pPlatformsUI, strlProjTypes.GetAt(strlProjTypes.FindIndex(nProjType)), TRUE);
	const CStringList* pPlatformsLong;
	pPlatforms->GetAvailablePlatforms(&pPlatformsLong, strlProjTypes.GetAt(strlProjTypes.FindIndex(nProjType)), FALSE);
	pPlatforms->Release();

	POSITION posUI = pPlatformsUI->GetHeadPosition();
	POSITION posLong = pPlatformsLong->GetHeadPosition();
	while (posUI != NULL && posLong != NULL)
	{
		CString strUI = pPlatformsUI->GetNext(posUI);
		CString strLong = pPlatformsLong->GetNext(posLong);

		if (strLong == szPlatforms[WIN]
				// This is the Win32 platform-- used for exe or dll
			|| (strLong == szPlatforms[MAC] && nProjType == 0)
				// This is the Macintosh platform-- used only for exe
			|| (strLong == szPlatforms[POWERMAC]))
				// This is the PowerMac platform-- used for exe or dll
		{
			pChklstPlatforms->Add(strUI);
			g_strlPlatformsDisplayed.AddTail(strUI);	// Remember what we store for later
		}
	}
}
#endif	// VS_PACKAGE

void ResetSymbolsForDLL()
{
	projOptions.m_nProjType = PROJTYPE_DLL;
	SetProjectTypeSymbol();
	ResetOleSymbols();
	RemoveSymbol("TARGET_68KMAC");

	// Undo database selections
	projOptions.m_nDataSource = dbOdbc;
	projOptions.m_bDBHeader = projOptions.m_bDBSimple = FALSE;
	if (projOptions.m_names.strBaseClass[classView] == "CDaoRecordView"
		|| projOptions.m_names.strBaseClass[classView] == "CRecordView")
	{
	    projOptions.m_names.strBaseClass[classView] = "CView";
	}
	RemoveSymbol("DB_COLSINFO");
	RemoveSymbol("DB_VARSINFO");
	RemoveSymbol("DB_SOURCE");
	RemoveSymbol("DB_DSN");
	SetDBSymbols();
	SetBaseViewSymbols();
}

BOOL CAppWizIFace::CanRunAppWizSteps(LPCTSTR szProj, LPCTSTR szProjDir, int* pnPlatforms)
{
	projOptions.m_strProj = szProj;
	VERIFY(projOptions.m_ProjDir.CreateFromString(szProjDir));
	CString strFullDirPath = (LPCTSTR) projOptions.m_ProjDir;
	if (*_tcsdec(strFullDirPath, ((LPCTSTR) strFullDirPath) + strFullDirPath.GetLength()) != '\\')
		strFullDirPath += '\\';
	SetSymbol("FULL_DIR_PATH", strFullDirPath);
	SetSymbol("SOURCE_FILTER","cpp;c;cxx;rc;def;r;odl;idl;hpj;bat");
	SetSymbol("INCLUDE_FILTER","h;hpp;hxx;hm;inl");

	CPath MakFile, MdpFile, VcpFile;
	MakFile.CreateFromDirAndFilename(projOptions.m_ProjDir, projOptions.m_strProj + BUILDER_EXT);
	MdpFile.CreateFromDirAndFilename(projOptions.m_ProjDir, projOptions.m_strProj + ".mdp");
	VcpFile.CreateFromDirAndFilename(projOptions.m_ProjDir, projOptions.m_strProj + ".opt");

	// COMMON BLOCK 1 (From CPromptDlg::FinishProcessing)
    // document info
    // fill symbol table if ok
	UpdateNamesIfNecessary(projOptions.m_names, projOptions.m_strProj);

	// Save a copy of the project name before munging it by upper-
	// casing it.
    SetSymbol("MIXED_CASE_ROOT", szProj);

	SetSymbol("ROOT", Upper(szProj));
	SetSymbol("root", szProj);
	SetSymbol("Root", szProj);

    if (projOptions.m_names.strClass[0].IsEmpty())
        FillDefaultNames(projOptions.m_names, projOptions.m_strProj);   // just use defaults
	SetDocumentSymbols();
	CString tmp = projOptions.m_strProjTag;   // non-DBCS root string
	SetSymbol("Safe_root", tmp);
	SetSymbol("safe_root", tmp);
	tmp.MakeUpper();
	SetSymbol("SAFE_ROOT", tmp);

    SetSymbol("platform_long", szPlatforms[WIN]);
#if defined(_X86_)
	SetSymbol("cc",			"cl");
	SetSymbol("cpu",		"i386");
	SetSymbol("host_cpu",	"X86");
	SetSymbol("linker",		"link");
#elif defined(_MIPS_)
	SetSymbol("cc",			"cl");
	SetSymbol("cpu",		"MIPS");
	SetSymbol("host_cpu",	"MIPS");
	SetSymbol("linker",		"link");
#elif defined(_ALPHA_)
	SetSymbol("cc",			"cl");
	SetSymbol("cpu",		"ALPHA");
	SetSymbol("host_cpu",	"ALPHA");
	SetSymbol("linker",		"link");
#else
#error "Need to define cc/cpu for this architecture"
#endif
	
	CString strPlatformShort;
	g_PlatNameConvert.LongToUI(szPlatforms[WIN], strPlatformShort);
	SetSymbol("platform_short", strPlatformShort);

    // Transfer info to projOptions
    UpdateTrackingNames(projOptions.m_names, projOptions.m_strProj);

	if (IsUserAWX())
	{
		// Custom AppWizard
		// this was where I set projOptions.m_UserExtension.m_strExtensionTItle.
		//  now, that's done immediately after we attach.
	}
	else
	{
		// Standard AppWizard

		// APPWIZ BLOCK 1
		// ...make sure a lang dll seems to be around
		if (!ScanForLangDll())
			return FALSE;

		// ...and make sure some files don't already exist
		CString strProjFile = strFullDirPath + projOptions.m_strProj;
        if (DoesFileExist(strProjFile + ".cpp")
            || DoesFileExist(strProjFile + ".h")
            || DoesFileExist(strProjFile + ".rc")
			|| DoesFileExist(strProjFile + ".clw")
			|| DoesFileExist(strFullDirPath + "stdafx.cpp")
			|| DoesFileExist(strFullDirPath + "stdafx.h")
			|| DoesFileExist(strFullDirPath + "resource.h"))
        {
            AfxMessageBox(IDP_FILE_EXISTS);
            return FALSE;
        }
	}
	// For either AppWizard or an extension, set symbols relating to
	//  the platforms selected.

	// Must do this now, since TARGET_MAC can get set here, and
	//  it's used below.
	ASSERT(pTermStatus != NULL);
	RecordPlatforms(pnPlatforms);

	// COMMON BLOCK 2
	// ...and make sure project name won't interfere with our
	//  default naming scheme
	if (DoesSymbolExist("PROJTYPE_OCX"))
	{
		if (InterfereWithDefaultsOCX(projOptions.m_strProj))
		{
			AfxMessageBox(IDP_FILE_INTERFERE_DEFAULTS_OCX);
			return FALSE;
		}
	}
	else if (InterfereWithDefaults(projOptions.m_strProj,
			DoesSymbolExist("TARGET_MAC"), DoesSymbolExist("PROJTYPE_CUSTOMAW")))
	{
		AfxMessageBox(IDP_FILE_INTERFERE_DEFAULTS);
		return FALSE;
	}

	// TODO: Properly separate powermac from 68kmac
	if (DoesSymbolExist("TARGET_MAC"))
	{
		// Reset Win32-only options previously set by the user.

		// ODBC
		if (!IsMacODBC())
		{
			projOptions.m_bDBSimple = projOptions.m_bDBHeader = FALSE;
	    	if (projOptions.m_names.strBaseClass[classView] == "CRecordView")
	        	projOptions.m_names.strBaseClass[classView] = "CView";
			SetDBSymbols();
			SetBaseViewSymbols();
		}

		// DAO
		if (projOptions.m_nDataSource == dbDao)
		{
			// If user previously picked a DAO data source, undo that
			projOptions.m_nDataSource = dbOdbc;
	    	if (projOptions.m_names.strBaseClass[classView] == "CDaoRecordView")
	        	projOptions.m_names.strBaseClass[classView] = "CView";
			RemoveSymbol("DB_COLSINFO");
			RemoveSymbol("DB_VARSINFO");
			RemoveSymbol("DB_SOURCE");
			RemoveSymbol("DB_DSN");
			SetDBSymbols();
			SetBaseViewSymbols();
		}

		// OLE
		if (!IsMacOle())
		{
			projOptions.m_nOleType = OLE_NO;
			projOptions.m_bOleAutomation = FALSE;
			projOptions.m_bOleCompFile = FALSE;
			SetOleSymbols();
		}
		if (!IsMacOcx())
		{
			projOptions.m_bOcx = FALSE;
			SetOleSymbols();
		}
	
		// WOSA
		if (!IsMacWOSA())
		{
			projOptions.m_options &= ~(1 << IDCD_POMAPI-IDCD_POBASE);
			projOptions.m_options &= ~(1 << IDCD_POSOCKETS-IDCD_POBASE);
			RemoveSymbol("MAPI");
			RemoveSymbol("SOCKETS");
		}

		// If Mac & no Win32 or Powermac targets, don't use shared mfc30.
		if (!DoesSymbolExist(szPlatforms[WIN]) && !DoesSymbolExist("TARGET_POWERMAC"))
			projOptions.m_projOptions &= ~(1 << (IDCD_PODLL-IDCD_POPROJBASE));
		SetProjOptionsSymbols();
	}

	if (IsMacInstalled())
		SetSymbol("INSTALLED_MAC", "1");
	else
		RemoveSymbol("INSTALLED_MAC");

	// We're going to be doing some symbol-setting below, so, if we're
	//  using an extension, it's time to attach its symbol table

	if (IsUserAWX())
	{
		// CUSTOM BLOCK 2

		// Start up the extension
		ASSERT(IsUserAWX());	// It was already loaded above

		// Change defaults if the extension creates a DLL
		if (DoesSymbolExist("PROJTYPE_DLL") || DoesSymbolExist("PROJTYPE_OCX"))
		{
			ResetSymbolsForDLL();
		}
		return TRUE;
	}
	else
	{
		// Here, the user selected appwiz.

		// APPWIZ BLOCK 2

		
		// We will eventually have to delete .bld, .mdp & .vcp if they
		//  exist.  To save user's time, let's check whether they're
		//  readonly now.
		if (!CheckIsReadOnly(MakFile.GetFullPath()))
			return FALSE;
		if (!CheckIsReadOnly(MdpFile.GetFullPath()))
			return FALSE;
		if (!CheckIsReadOnly(VcpFile.GetFullPath()))
			return FALSE;

		// Get rid of a previous loading of an extension
		projOptions.m_UserExtension.Detach();

        // Start up appwiz... finally!!
		if (pTermStatus->nProjType == 0)
		{
			// Create an .exe
			projOptions.m_nProjType = PROJTYPE_MDI;
			projOptions.m_bNoDocView = FALSE;
			SetProjectTypeSymbol();
			SetOleSymbols();
			if (projOptions.m_names.strBaseClass[classView] == "CRecordView")
				SetSymbol("CRecordView", "1");
		
		}
		else
		{
			// Create a .dll
			ResetSymbolsForDLL();
		}
		return TRUE;
	}

	ASSERT(FALSE);
}
