
#include "stdafx.h"
#include "mfcappwz.h"
#include "seqdlg.h"
#include "codegen.h"
#include "dlgs.h"
#include "oleutil.h"
#include "ddxddv.h"
#include "projtype.h"
#include "miscdlgs.h"
#include "oledlg.h"
#include "datbdlg.h"
#include "symbols.h"
#include "extend.h"
#include "lang.h"

#ifndef VS_PACKAGE
#include "ids.h"
#endif	// VS_PACKAGE

#include "platname.h"
#include "awiface.h"

#ifndef VS_PACKAGE
#include <bldapi.h>
#include <bldguid.h>
#endif	// VS_PACKAGE

#include <limits.h>

#ifndef VS_PACKAGE
#include <slob.h>
#endif	// VS_PACKAGE

#ifndef VS_PACKAGE
#include <utilctrl.h>
#include <path.h>
#endif	// VS_PACKAGE

#include <dlgbase.h>
#include <direct.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <windef.h>
#include "dde.h"            // directly send DDEEXECUTE

#ifdef VS_PACKAGE
#include "util.h"  //from sloblib
#else
#include "version.h"        // SLM generated version file
#include <afxdllxx.h>		// our modified version of afxdllx.h
#endif	// VS_PACKAGE

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef VS_PACKAGE
CBldWiz g_CBldWiz;
#endif

AFX_EXTENSION_MODULE AppWizardDLL = { NULL, NULL };

#ifndef VS_PACKAGE
extern "C" BOOL WINAPI RawDllMain(HINSTANCE, DWORD dwReason, LPVOID);
extern "C" BOOL (WINAPI* _pRawDllMain)(HINSTANCE, DWORD, LPVOID) = &RawDllMain;

extern "C" BOOL APIENTRY RawDllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		if (!IsShellDefFileVersion(SHELL_DEFFILE_VERSION))
			return(FALSE);
	}
	return(ExtRawDllMain(hInstance, dwReason, lpReserved));
}

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		// Extension DLL one-time initialization.
		if (!AfxInitExtensionModule(AppWizardDLL, hInstance))
			return 0;

		// Wire up this DLL into the resource chain
		new CDynLinkLibrary(AppWizardDLL);

		// Don't need thread attach notifications
		VERIFY(DisableThreadLibraryCalls(hInstance));
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		// Terminate the library before destructors are called
		AfxTermExtensionModule(AppWizardDLL);
	}
	return 1;
}
#endif	// VS_PACKAGE

HINSTANCE GetResourceHandle()
{
	return AppWizardDLL.hModule;
}

// This figures out on the basis of the compiler version whether the current
// product is the learning edition (V5) or not.
BOOL IsLearningBox()
{
	// Assume Learning is false if any failure occurs.
	static BOOL bHaveChecked = FALSE;
	static BOOL bIsLearning = FALSE;

	if (bHaveChecked)
		return bIsLearning;

	bHaveChecked = TRUE;

	DWORD dw;
	CString strC2Dir;
	TCHAR szBuf [MAX_PATH];
	HKEY hKey;

	// strVCDir = "\\\\HKEYLOCALMACHINE\\microsoft\\devstudio\\products\\Microsoft Visual C++\\ProductDir"
	if ( RegOpenKeyEx (
		HKEY_LOCAL_MACHINE, _T ("Software\\Microsoft\\DevStudio\\6.0\\Products\\Microsoft Visual C++"),
		0, KEY_READ, &hKey) == ERROR_SUCCESS 
	   )
	{
		DWORD dwType, cbData = MAX_PATH;
		if (RegQueryValueEx (hKey, _T ("ProductDir"), NULL, &dwType, (LPBYTE) szBuf, &cbData) == ERROR_SUCCESS)
		{
			ASSERT (dwType == REG_SZ);
			szBuf [cbData] = 0;
			strC2Dir = szBuf;
			strC2Dir+="\\bin\\c2.dll";
		}
		RegCloseKey (hKey);
	}	

	TCHAR *szC2 = strC2Dir.GetBuffer(strC2Dir.GetLength());

	// Get version information from c2.exe.
	DWORD dwSize = GetFileVersionInfoSize( szC2, &dw );
	if (0!=dwSize)
	{
		LPVOID lpBuffer = new BYTE[dwSize];

		if (NULL==lpBuffer)
			return bIsLearning;

		if (GetFileVersionInfo( szC2, dw, dwSize, lpBuffer))
		{
			UINT uLen;
			VS_FIXEDFILEINFO* pVerInfo;
			if (0!=VerQueryValue( lpBuffer, _T("\\"), (LPVOID*)&pVerInfo, &uLen ))
			{
				// if the last digit in the version number is an 8 or a 9
				// then we have the learning edition
				UINT lower = pVerInfo->dwFileVersionMS & 0x0000FFFF;
				bIsLearning = ( lower % 10 ) >= 8 ;
			}
		}
		delete [] lpBuffer;
	}
	return bIsLearning;
}


/////////////////////////////////////////////////////////////////////////////
// mfc\src internal def
BOOL PASCAL _AfxFullPath(LPSTR lpszPathOut, LPCSTR lpszFileIn);

/////////////////////////////////////////////////////////////////////////////
// keys for Registry

//char BASED_CODE szMSVCSection[] = "Software\\Microsoft\\Visual C++ 3.0";
//char BASED_CODE szMSVCGeneralSection[] = "Software\\Microsoft\\Visual C++ 3.0\\General";
//char BASED_CODE szMFCPathKey[] = "MFCPath32Bit";
//static char BASED_CODE szHelpKey[] = "HelpFile32Bit";
//static char BASED_CODE szHelpFile[] = "MSVC20.HLP";

// Special string for user data of CMainDlg.  It identifies the window as the
//  AppWizard main window.
static LPCTSTR szMainDlgUserData = "AppWizard main window";

/////////////////////////////////////////////////////////////////////////////
// structures

struct ClassCategoryInfo
{
  int categoryID;
  const char* condition;
};

struct BaseClassInfo
{
  int categoryID;
  const char* baseClass;
  const char* condition;
};

// ClassCategoryInfo is used to determine which classes will be created
//  depending upon the symbols that are defined.  Any class with NULL
//  as a condition is always created.  A class listed more than once
//  with multiple symbols indicates an OR condition.  Multiple symbols
//  separated by (exactly) " && " indicates an AND condition (see
//  classDlgAutoProxy).
static ClassCategoryInfo classCategories[] =
{
    { classView, "PROJTYPE_NOT_DLG" },
    { classLeftView, "PROJTYPE_EXPLORER" },
    { classApp, NULL },
    { classFrame, "PROJTYPE_NOT_DLG" },
    { classFrame, "NODOCVIEW" },
    { classChildFrame, "MDICHILD" },
    { classDoc, "PROJTYPE_NOT_DLG" },
    { classDlg, "PROJTYPE_DLG"},
    { classRecset, "CRecordView" },
    { classRecset, "CDaoRecordView" },
    { classRecset, "COleDBRecordView" },
    { classSrvrItem, "MINI_SERVER" },
    { classSrvrItem, "FULL_SERVER" },
    { classSrvrItem, "CONTAINER_SERVER" },
    { classCntrItem, "CONTAINER" },
    { classCntrItem, "CONTAINER_SERVER" },
    { classIPFrame, "MINI_SERVER" },
    { classIPFrame, "FULL_SERVER" },
    { classIPFrame, "CONTAINER_SERVER" },
	{ classDlgAutoProxy, "PROJTYPE_DLG && AUTOMATION" },
    { classWndView, "NODOCVIEW" },
    { -1, NULL }
};


// BaseClassInfo is used to determine the base class of a given class.  The
//  base class is determined by finding the first entry where 1) the condition
//  is NULL or 2) the condition is TRUE.  Specific base classes depending on
//  symbol settings should therefore be placed before the generic ones where
//  the condition is NULL.
// Note: it is not necessary to qualify a class with its symbolic conditions
//  if it is always the same.  For example, classIPFrame will only be
//  created when either CONTAINER_SERVER, MINI_SERVER, or FULL_SERVER is
//  defined, but it is not necessary to qualify it as such.  That job is
//  handled by the ClassCategoryInfo above.
static BaseClassInfo baseClasses[] =
{
    { classApp, "CWinApp", NULL },
    { classFrame, "CMDIFrameWnd", "PROJTYPE_MDI" },
    { classFrame, "CFrameWnd", NULL },
    { classChildFrame, "CMDIChildWnd", NULL },
    { classRecset, "CDAORecordset", "DAO" },
    { classRecset, "CCommand", "OLEDB" },
	{ classRecset, "CRecordset", NULL },
	{ classDoc, "CRichEditDoc", "CRichEditView" },
	{ classDoc, "COleDocument", "CONTAINER" },
    { classDoc, "COleServerDoc", "FULL_SERVER" },
    { classDoc, "COleServerDoc", "MINI_SERVER" },
    { classDoc, "COleServerDoc", "CONTAINER_SERVER" },
    { classDoc, "CDocument", NULL },
    { classDlg, "CDialog", NULL },
	{ classSrvrItem, "CDocObjectServerItem", "ACTIVE_DOC_SERVER" },
    { classSrvrItem, "COleServerItem", NULL },
    { classCntrItem, "CRichEditCntrItem", "CRichEditView" },
	{ classCntrItem, "COleDocObjectItem", "ACTIVE_DOC_CONTAINER" },
	{ classCntrItem, "COleClientItem", NULL },
	{ classIPFrame, "COleDocIPFrameWnd", "ACTIVE_DOC_SERVER" },
    { classIPFrame, "COleIPFrameWnd", NULL },
	{ classDlgAutoProxy, "CCmdTarget", NULL },
	{ classLeftView, "CTreeView", NULL },
	{ classWndView, "CWnd", NULL },
    { -1, NULL, NULL }
};
// CView is a hard-coded special case, and is intentionally left out of baseClasses[].

/////////////////////////////////////////////////////////////////////////////
// Global state

// HINSTANCE hMSVCResHandle = NULL;

CWnd* pLaunchParent = NULL;     // will be non-null if launched from IDE
CStringList strlProjTypes;

const TCHAR* szPlatforms[] =
{
#if defined(_X86_)
	"Win32 (x86)",
#elif defined(_MIPS_)
	"Win32 (MIPS)",
#elif defined(_ALPHA_)
	"Win32 (ALPHA)",
#else
#error "Define TARGET for this platform"
#endif
	"Macintosh",
	"Power Macintosh",
	"Java Virtual Machine",
    "Xbox",
};
#if defined(_X86_)
#define TARGET "TARGET_INTEL"
#elif defined(_MIPS_)
#define TARGET "TARGET_MIPS"
#elif defined(_ALPHA_)
#define TARGET "TARGET_ALPHA"
#else
#error "Define TARGET for this platform"
#endif
CMapStringToString g_PlatformsSelected; // Long names of selected platforms
void InitPlatformsSelected()
{
	g_PlatformsSelected.RemoveAll();
	g_PlatformsSelected[szPlatforms[WIN]] = TARGET;
	g_PlatformsSelected[szPlatforms[MAC]] = "TARGET_68KMAC";
	g_PlatformsSelected[szPlatforms[POWERMAC]] = "TARGET_POWERMAC";
	g_PlatformsSelected[szPlatforms[JAVA]] = "TARGET_JAVA";
    g_PlatformsSelected[szPlatforms[XBOX]] = "TARGET_XBOX";
}
#undef TARGET
CPlatNameConvert g_PlatNameConvert;

// This remembers what AppWizard has last inserted into the platforms checklist,
//  so that when we get pnPlatforms back from CPromptDlg, we'll know what the hell
//  it's talking about.  (I.e., we'll know what indices of pnPlatforms correspond
//  to what platforms.)
CStringList g_strlPlatformsDisplayed;


#define WM_MYQUIT WM_USER+3
struct Message
{
    UINT messageName;
    WPARAM wparam;
    LPARAM lparam;
} msgNotification;

AppWizTermStatus g_TermStatus;
AppWizTermStatus* pTermStatus = &g_TermStatus;

void CreateDirectory(const char* szDir, BOOL bChangeTo);	// Defined in codegen.cpp
BOOL GenerateDBBindings(CString& strDecls, CString& strInits, CString& strRFX);

//static BOOL bODBCInstalled;
static BOOL bOLEInstalled;

//////////////////////////////////////////////////////////////////////////////

ProjOptions projOptions;   // Contains all user-set options, visible to all dlgs.


BOOL IsMacInstalled()
{
#ifndef VS_PACKAGE
	// Determine available platforms for a vanilla Application
	LPBUILDPLATFORMS pPlatforms;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_IBuildPlatforms, (LPVOID FAR *)&pPlatforms)));

	const CStringList* pPlatformsLong;
	pPlatforms->GetAvailablePlatforms(&pPlatformsLong, strlProjTypes.GetHead(), FALSE);
	pPlatforms->Release();

	// See if we can find the Mac platform
	POSITION pos = pPlatformsLong->GetHeadPosition();
	while (pos != NULL)
	{
		CString strLong = pPlatformsLong->GetNext(pos);
		if (strLong == szPlatforms[MAC] || strLong == szPlatforms[POWERMAC])
			return TRUE;
	}
	return FALSE;
#endif
	return FALSE ;
}

void MakeOKDefault(CMainDlg* pMainDlg)
{
	pMainDlg->SetDefID(IDOK);
	((CButton*) (pMainDlg->GetDlgItem(ID_HELP)))->SetButtonStyle(BS_PUSHBUTTON);
    ((CButton*) (pMainDlg->GetDlgItem(IDCANCEL)))->SetButtonStyle(BS_PUSHBUTTON);
    ((CButton*) (pMainDlg->GetDlgItem(IDC_BACK)))->SetButtonStyle(BS_PUSHBUTTON);
    ((CButton*) (pMainDlg->GetDlgItem(IDC_END)))->SetButtonStyle(BS_PUSHBUTTON);
	((CButton*) (pMainDlg->GetDlgItem(IDOK)))->SetButtonStyle(BS_DEFPUSHBUTTON);
}

/*BOOL IsaMainBtn(char c)
{
	return (c == 'n' || c == 'N'
		|| c == 'b' || c == 'B'
		|| c == 'f' || c == 'F'
		|| c == 'h' || c == 'H');
}*/

//////////////////////////////////////////////////////////////////////////////
// Borrowed from \cmpnt\src\profile.cpp
// Code for getting the correct help file path

BOOL BuildHelpPath(LPCSTR szHelpPath, LPCSTR szHelpFile, LPSTR szFullHelpPath)
{
    BOOL bRet = FALSE ;

    // blechh...  I hate casting away const, but it turns out that
    // this function doesn't really change the strings so I guess
    // it's okay
    LPSTR lpNextHelpPath = (LPSTR) szHelpPath;
    do
    {
        LPSTR lpCurrentHelpPath = lpNextHelpPath;
        lpNextHelpPath = strchr(lpCurrentHelpPath,';');
        if (lpNextHelpPath)
            *lpNextHelpPath = '\0';
        strcpy(szFullHelpPath,lpCurrentHelpPath) ;
        // Add a backslash if necessary
		TCHAR ch = *_tcsdec((TCHAR*) szFullHelpPath,
			((TCHAR*) szFullHelpPath) + _tcslen((TCHAR*) szFullHelpPath));
        if (ch != '\\')
            _tcscat((TCHAR*) szFullHelpPath, "\\") ;
        strcat(szFullHelpPath,szHelpFile) ;
        bRet = (_access(szFullHelpPath, 0) == 0);
        if (lpNextHelpPath)
        {
            *lpNextHelpPath = ';';
            lpNextHelpPath++;
        }
    }
    while (bRet == FALSE && lpNextHelpPath != NULL);

    return bRet;
}

/////////////////////////////////////////////////////////////////////////////


void fillBaseClasses(CString bc[])
{
    int i = 0;
    int currID = -1;
    BaseClassInfo *pbci;
    while ((pbci = &baseClasses[i])->categoryID != -1)
    {
        CString value;
        currID = pbci->categoryID;
        // Pass over undefined symbols
        for( ; (pbci->condition != NULL) &&
            !DoesSymbolExist(pbci->condition) &&
            currID == pbci->categoryID; pbci = &baseClasses[++i])
            ;
        if (currID == pbci->categoryID)    // We found a defined symbol or NULL
        {
            ASSERT(currID != -1);
            // Enter base class name and skip to next category
            bc[currID] = pbci->baseClass;
            for (; currID == pbci->categoryID; pbci = &baseClasses[++i])
                ;
        }
        // (Otherwise, we've already skipped to the next category or the end.)
    }
}

// Helper used by fillCategoryEnabled as it parses through
//  the conditions in classCategories.  This simply parses
//  out any "&&"'s between symbols, indicating an 'and' condition
static BOOL IsClassConditionTrue(LPCTSTR szCondition)
{
	const int MAX_CONDITION_SIZE = 256;
	if (_tcslen(szCondition) >= MAX_CONDITION_SIZE)
	{
		// If this assert fails, a really big condition was passed.
		//  You may need to increase MAX_CONDITION_SIZE
		ASSERT(FALSE);
		return FALSE;
	}

	// Copy to our writeable stack buffer
	TCHAR szConditionBuffer[MAX_CONDITION_SIZE];
	_tcscpy(szConditionBuffer, szCondition);
	LPTSTR szCurrCondition = szConditionBuffer;
	LPTSTR szNextCondition = szCurrCondition-4;		// Compensate for initial iteration

	while (szNextCondition != NULL)
	{
		szCurrCondition = szNextCondition + 4;	// Skip over " && "

		// Find the end of the current symbol
		szNextCondition = _tcschr(szCurrCondition, ' ');
		if (szNextCondition != NULL)
		{
			// A space must be followed by double ampersands
			//  and then the next symbol e.g.,: "SYMBOL1 && SYMBOL2"
			ASSERT (*(szNextCondition+1) == '&');
			ASSERT (*(szNextCondition+2) == '&');
			ASSERT (*(szNextCondition+3) == ' ');
			ASSERT (*(szNextCondition+4) != '\0');

			*szNextCondition = '\0';	// plug it up so we don't read past current condition
		}

		if (!DoesSymbolExist(szCurrCondition))
			return FALSE;		// If any of the symbol is FALSE, return FALSE
	}

	// We survived this long, it must be TRUE
	return TRUE;
}

void fillCategoryEnabled(BOOL bce[])
{
    // First, reset enabled's to FALSE.
    for(int i=0; i < classMax; i++)
        bce[i] = FALSE;

    i=0;
    int currID = -1;
    ClassCategoryInfo *pcci;
    while ((pcci=&classCategories[i])->categoryID != -1)
    {
        CString value;
        currID = pcci->categoryID;

        // Pass over undefined symbols
        for( ; (pcci->condition != NULL) &&
            !IsClassConditionTrue(pcci->condition) &&
            currID == pcci->categoryID; pcci = &classCategories[++i])
            ;
        if (currID == pcci->categoryID)    // We found a defined symbol or NULL
        {
            ASSERT(currID != -1);
            bce[currID] = TRUE;
            for (; currID == pcci->categoryID; pcci = &classCategories[++i])
                ;
        }
        // (Otherwise, we've already skipped to the next category or the end.)
    }
}

/////////////////////////////////////////////////////////////////////////////
// Prevent overlap of canned classes or project name

//TODO: need to regen the mfcclass.h to include new classes!

char* rgszReserved[] =
{
#include "mfcclass.h"
    NULL
};

//TODO: need to regen the mfcproj.h to include new .DLLs and other
//reserved project names


static BOOL IsReservedClassName(const char* psz)
{
    for (char **ppsz = rgszReserved; *ppsz != NULL; ppsz++)
    {
        if (strcmp(psz, *ppsz) == 0)
            return TRUE;
    }
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Name verification dialog

void MakeDefRegID(const TCHAR* pch, CString& rRegID)
{
	rRegID.Empty();
	for( ; *pch != '\0'; pch = _tcsinc(pch))
	{
		if (*pch != '_')
		{
			rRegID += *pch;
		}
	}
	
	if (rRegID.IsEmpty() || _istdigit(rRegID[0]))
	   	rRegID += "My";

	rRegID += ".Document";
}

BOOL IsLower(const TCHAR* sz)
{
	CString str = sz;
	str.MakeLower();
	return (str == sz);
}

void GetTag(CString& tag, LPCTSTR szRoot /* = NULL */)
{
	if (szRoot == NULL)
		szRoot = projOptions.m_strProj;
	tag.Empty();
	
	const TCHAR* pch = szRoot;
	for( ; *pch != '\0'; pch = _tcsinc(pch))
	{
		if(_istalnum(*pch)	// Strip characters that cannot be in class names
		   || *pch == '_')  // valid chars are 'A'-'Z', 'a'-'z', '0'-'9', '_'
		{
			tag += *pch;
		}
    }
    if(tag.IsEmpty())		// An empty tag would produce: CApp, CDoc, and CView
    	tag = "My";			// CView would become CMyView, so just make tag="My"

	if (_istdigit(tag[0]))	// Can't begin with number
		tag = "My" + tag;

	if (!tag.Compare(_T("Custom")))		// probably only important for custom AppWiz...
		tag = _T("My") + tag;

	tag.SetAt(0, (TCHAR)_totupper(tag[0]));		// make 1st character upper

	// Limit length of tag so generated class names don't exceed _MAX_SYMBOL
	#define MAX_ADDED_TO_TAG 10
	if (tag.GetLength() + MAX_ADDED_TO_TAG > _MAX_SYMBOL)
		tag = tag.Left(_MAX_SYMBOL - MAX_ADDED_TO_TAG);
}

// NOTE: Adding additional classes (and consequently more default naming schemes)
//  requires updating InterfereWithDefaults() below.
// Also, no symbols are guaranteed to be set when this is called (like TARGET_MAC)
//  If a name depends on symbols, use FillTruncatedDefaultNames
void FillDefaultNames(NAMES& names, const char* pszRoot)
{
	ASSERT(*pszRoot != 0);	// empty Root is invalid

	CString tag;
	GetTag(tag, pszRoot);

	projOptions.m_strProjTag = tag;

    CString filebase = pszRoot;

    // Fill in names of base classes
    fillBaseClasses(names.strBaseClass);

    // App (file names are read-only)
    names.strClass[classApp] = "C" + tag + "App";
	if (IsReservedClassName(names.strClass[classApp]))
		names.strClass[classApp] = "CMy" + tag + "App";
	            //eg: when tag=="Win", CWinApp -> CMyWinApp

    // Frame (base class is read-only)
    names.strClass[classFrame] = "CMainFrame";
    names.strHFile[classFrame] = "MainFrm.h";
    names.strIFile[classFrame] = "MainFrm.cpp";

    // Child frame (base class is read-only)
    names.strClass[classChildFrame] = "CChildFrame";
    names.strHFile[classChildFrame] = "ChildFrm.h";
    names.strIFile[classChildFrame] = "ChildFrm.cpp";

    // Inplace frame (base class is read-only)
    names.strClass[classIPFrame] = "CInPlaceFrame";
    names.strHFile[classIPFrame] = "IpFrame.h";
    names.strIFile[classIPFrame] = "IpFrame.cpp";

    // Left View of Explorer (base class is read-only)
    names.strClass[classLeftView] = "CLeftView";
    names.strHFile[classLeftView] = "LeftView.h";
    names.strIFile[classLeftView] = "LeftView.cpp";

    // Left View of Explorer (base class is read-only)
    names.strClass[classWndView] = "CChildView";
    names.strHFile[classWndView] = "ChildView.h";
    names.strIFile[classWndView] = "ChildView.cpp";

    // Document
    names.strClass[classDoc] = "C" + tag + "Doc";
    if (IsReservedClassName(names.strClass[classDoc]))
        names.strClass[classDoc] = "CMy" + tag + "Doc";
    CString filename = filebase + "Doc";
    names.strHFile[classDoc] = filename + ".h";
    names.strIFile[classDoc] = filename + ".cpp";
	projOptions.m_bUpdateRegID = TRUE;

    /*names.strDocTag = tag;
    if (names.strDocTag.GetLength() > MAX_TAG)
        names.strDocTag = names.strDocTag.Left(MAX_TAG);
	names.strDocFilter = "";
	names.strDocFileNew = names.strDocTag;
	names.strDocRegName = names.strDocTag + " Document";
	projOptions.m_bUpdateFilter
		= projOptions.m_bUpdateFileNew
		= projOptions.m_bUpdateRegName = TRUE;*/

	// TODO: Use langdll
/*	if (projOptions.m_bFilesAfter)
		names.strDocFilter = names.strDocFileType + " "
			+ projOptions.m_strFiles;
	else
		names.strDocFilter = projOptions.m_strFiles + " "
			+ names.strDocFileType;*/
	// TODO: Should keep this Mixed?
	//names.strTitle = Mixed(pszRoot);
	names.strTitle = pszRoot;



	MakeDefRegID(tag, names.strDocRegID);
	names.strDocFileExt = "";

	// file type & creator must have exactly 4 characters.  Pad on right
	//  with ' ' if necessary
	names.strDocFileCreator = tag.Left(4);
	int nLen = names.strDocFileCreator.GetLength();
	if (nLen < 4)
	{
		CString strPad(' ', 4 - nLen);
		names.strDocFileCreator += strPad;
	}
	names.strDocFileCreator.MakeUpper();
	names.strDocFileType = names.strDocFileCreator;
    // View
    names.strClass[classView] = "C" + tag + "View";
    if (IsReservedClassName(names.strClass[classView]))
        names.strClass[classView] = "CMy" + tag + "View";
                //eg: CEditView -> CMyEditView
    if (names.strBaseClass[classView].IsEmpty())
        names.strBaseClass[classView] = "CView";
    filename = filebase + "View";   // don't abbreviate
    names.strHFile[classView] = filename + ".h";
    names.strIFile[classView] = filename + ".cpp";

    // Record set (base class is read-only)
    names.strClass[classRecset] = "C" + tag + "Set";
    if (IsReservedClassName(names.strClass[classRecset]))
        names.strClass[classRecset] = "CMy" + tag + "Set";
	switch(projOptions.m_nDataSource) {
		case dbOdbc:
			names.strBaseClass[classRecset] = "CRecordset";
			break;
		case dbDao:	
			names.strBaseClass[classRecset] = "CDAORecordset";
			break;
		case dbOledb:	
			names.strBaseClass[classRecset] = "CCommand";
			break;
		default:
			break;
	}
	
    filename = filebase + "Set";    // don't abbreviate
    names.strHFile[classRecset] = filename + ".h";
    names.strIFile[classRecset] = filename + ".cpp";

    // Dialog (for dialog-based app)
    names.strClass[classDlg] = "C" + tag + "Dlg";
    if (IsReservedClassName(names.strClass[classDlg]))
        names.strClass[classDlg] = "CMy" + tag + "Dlg";
    filename = filebase + "Dlg";    // don't abbreviate
    names.strHFile[classDlg] = filename + ".h";
    names.strIFile[classDlg] = filename + ".cpp";

	// Dialog's automation proxy (for dlg-based app w/ automation)
	names.strClass[classDlgAutoProxy] = names.strClass[classDlg] + "AutoProxy";
	names.strHFile[classDlgAutoProxy] = "DlgProxy.h";
	names.strIFile[classDlgAutoProxy] = "DlgProxy.cpp";

    // Container Item
    names.strClass[classCntrItem] = "C" + tag + "CntrItem";
    if (IsReservedClassName(names.strClass[classCntrItem]))
		names.strClass[classCntrItem] = "CMy" + tag + "CntrItem";
    names.strHFile[classCntrItem] = "CntrItem.h";
    names.strIFile[classCntrItem] = "CntrItem.cpp";
        // Note: not based on project name because too hard to come
        //  up with meaningful 4 and 2 character abbreviations
        //  for ServerItem and ContainerItem.

    // Server Item
    names.strClass[classSrvrItem] = "C" + tag + "SrvrItem";
    ASSERT(!IsReservedClassName(names.strClass[classSrvrItem]));
    names.strHFile[classSrvrItem] = "SrvrItem.h";
    names.strIFile[classSrvrItem] = "SrvrItem.cpp";
}

// these are read-only
void UpdateTrackingNames(NAMES& names, const char* pszRoot)
{
    CString filebase = pszRoot;

    // classApp names are always based on the app name
    names.strHFile[classApp] = filebase + ".h";
    names.strIFile[classApp] = filebase + ".cpp";

    // update base class names
    fillBaseClasses(names.strBaseClass);
}

// If we're run by the IDE, we need to tell it that we're exiting when
//  the user hits Cancel.
void CancelAppWiz()
{
    if (pLaunchParent == NULL) return;

    pLaunchParent->EnableWindow(TRUE);
	pLaunchParent->SetActiveWindow();
    ASSERT(pLaunchParent->IsWindowEnabled());
	pTermStatus->nTermType = APPWIZTERMTYPE_CANCEL;
}


/////////////////////////////////////////////////////////////////////////////
// Intro dialog

// OLE type equates
extern char* oleOptions[5];

class CNamesDlg;

class CConfirmDlg : public C3dDialog
{
// Construction
public:
    CConfirmDlg(CWnd* pParent = NULL, BOOL bHasCancel = TRUE);  // standard constructor

// Dialog Data
    CString m_strDir;
    //{{AFX_DATA(CConfirmDlg)
    enum { IDD = IDD_CONFIRM };
    CEdit m_stcOutput;
    CStatic m_stcDir;
    //}}AFX_DATA

// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // Generated message map functions
    //{{AFX_MSG(CConfirmDlg)
    virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
    virtual void OnOK();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
    //afx_msg void OnClickedBack();
    DECLARE_MESSAGE_MAP()
	BOOL m_bHasCancel;

private:
    void SetSymbols();
};




extern char* rgszOptions[];
extern char* rgszProjOptions[];


extern char* rgszFrameStyles[];
extern char* rgszChildFrameStyles[];

extern char* szDefaultViews[];
extern const char* szProjTypeString[];

/*
/////////////////////////////////////////////////////////////////////////////
// file path helpers

// stolen from ClassWizard->
#define wInvalidName    0
#define wIsFile         1
#define wIsDir          2

static int QualifyFileName(const char* pszFileName, char* pszQualified)
    // grunge for getting the currently selected file name
    // and for telling whether it is likely to exist when we need it
{
    if (_fullpath(pszQualified, pszFileName, _MAX_PATH) == NULL)
        return wInvalidName;
    _tcslwr((TCHAR*) pszQualified);
	// NOTE: We use GetFileAttributes instead of _stat to allow for UNC names.
	DWORD nAttributes = GetFileAttributes(pszQualified);
	if (nAttributes == 0xffffffff)
		return wInvalidName;
	if (nAttributes & FILE_ATTRIBUTE_DIRECTORY)
        return wIsDir;  // directory or something strange happened
    return wIsFile;
}
*/

/////////////////////////////////////////////////////////////////////////////
// CStatusDlg dialog

class CStatusDlg : public C3dDialog
{
    enum { IDD = IDD_PROGRESS };

// Construction
public:
    CStatusDlg(CWnd* pParent)   // modeless constructor
	: C3dDialog(CStatusDlg::IDD, pParent)
    {
        Create(MAKEINTRESOURCE(CStatusDlg::IDD), pParent);
    }
    BOOL OnInitDialog()
    {
        CenterWindow();
		//ShowWindow(SW_SHOW);
        return C3dDialog::OnInitDialog();     // does DDX
   }
};


int GetDefaultFrameStyles()
{
	return 1<<(IDC_THICKFRAME - FRAME_STYLES_START)
		| 1<<(IDC_SYSMENU - FRAME_STYLES_START)
		| 1<<(IDC_MINIMIZEBOX - FRAME_STYLES_START)
		| 1<<(IDC_MAXIMIZEBOX - FRAME_STYLES_START);
}

int GetDefaultChildFrameStyles()
{
	return 1<<(IDC_CHILD_THICKFRAME - CHILD_FRAME_STYLES_START)
		| 1<<(IDC_CHILD_MINIMIZEBOX - CHILD_FRAME_STYLES_START)
		| 1<<(IDC_CHILD_MAXIMIZEBOX - CHILD_FRAME_STYLES_START);
}

/////////////////////////////////////////////////////////////////////////////
// Names dialog

class CNamesDlg : public CSeqDlg
{
public:
    CNamesDlg(CWnd* pParent = NULL);
	virtual void WriteValues();
    //{{AFX_DATA(CNamesDlg)
    enum { IDD = IDD_NAMES };
    //}}AFX_DATA
    CListBox    m_classList;
    virtual BOOL CanDestroy();
	virtual BOOL ShouldShowPicture() { return TRUE; }
	virtual UINT ChooseBitmap() { return IDB_FLAG; }
	virtual int GetBitmapSizeIndex() { return BMPSIZE_FLAG; }
	
// Implementation
    BOOL OnInitDialog();
    int m_iSelOld;  // old selection index (not index into class list)

    virtual void DoDataExchange(CDataExchange* pDX);

    //{{AFX_MSG(CNamesDlg)
    afx_msg void OnSelectClass();
    afx_msg void OnSelchangeBaseClass();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CNamesDlg::CNamesDlg(CWnd* pParent)
    : CSeqDlg(CNamesDlg::IDD, pParent)
{
}

BEGIN_MESSAGE_MAP(CNamesDlg, CSeqDlg)
    //{{AFX_MSG_MAP(CNamesDlg)
    ON_LBN_SELCHANGE(IDC_CLASS_LIST, OnSelectClass)
    ON_CBN_SELCHANGE(IDC_BASE_CLASS_COMBO, OnSelchangeBaseClass)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define OPT_DB                  1

// Item data to determine whether CRecordView is selected in combo box.
/*#define VIEW_OTHER   1
#define VIEW_RECORD  2*/

BOOL CNamesDlg::OnInitDialog()
{
    // If class names not yet filled in, use default names.
    if (projOptions.m_names.strClass[0].IsEmpty())
        FillDefaultNames(projOptions.m_names, projOptions.m_strProj);   // just use defaults

	// We shouldn't need this set here, since it's set after first dialog.
	//SetProjectTypeSymbol();

    // First, set view base class symbols.
    SetBaseViewSymbols();

	// Be sure the other base classes are set correctly (may depend on "CRichEditView" being
	//  set, which we did above in SetBaseViewSymbols()).
	fillBaseClasses(projOptions.m_names.strBaseClass);

    VERIFY(m_classList.SubclassDlgItem(IDC_CLASS_LIST, this));

    // Set relevant symbols so the correct classes appear in the list

    // DB_SIMPLE
    if (projOptions.m_bDBSimple)
		SetSymbol("DB_NO_FILE", "1");
    else
		RemoveSymbol("DB_NO_FILE");
    if (projOptions.m_nProjType == PROJTYPE_DLG)
    {
		// KEEP!!! the NOT used for classes dialog
        SetSymbol("PROJTYPE_DLG", "1");
        RemoveSymbol("PROJTYPE_NOT_DLG");
    }
    else
    {
        RemoveSymbol("PROJTYPE_DLG");
		if( projOptions.m_bNoDocView ){
			RemoveSymbol("PROJTYPE_NOT_DLG");
		} else {
			SetSymbol("PROJTYPE_NOT_DLG", "1");
		}
    }
    if (projOptions.m_nProjType != PROJTYPE_DLG && !projOptions.m_bDBSimple)
    {
        SetSymbol("NOT_DLG_OR_SIMPLE", "1");
    }
    else
    {
        RemoveSymbol("NOT_DLG_OR_SIMPLE");
    }
	
	// Third, CMDIChildWnd
	SetChildFrameSymbols();
	SetSplitterSymbols();

    // Fill with class list - non-sorted so we can use index for 'iClass'.
    //  Only enter those classes that are enabled
    ASSERT(!(m_classList.GetStyle() & LBS_SORT));
    fillCategoryEnabled(projOptions.m_names.bCategoryEnabled);
    for (int iClass = 0; iClass < classMax; iClass++)
    {
        if (projOptions.m_names.bCategoryEnabled[iClass])
        {
            int i = m_classList.AddString(projOptions.m_names.strClass[iClass]);
            TRACE2("Added class '%s'@%d\n",
                (const char*)projOptions.m_names.strClass[iClass], i);
            m_classList.SetItemData(i, iClass);
        }
    }
    m_classList.SetCurSel(m_iSelOld = 0);

    // Fill base-class-for-view combo box
    CComboBox* comboBox = (CComboBox*)GetDlgItem(IDC_BASE_CLASS_COMBO);
    ASSERT(comboBox);
    comboBox->ResetContent();
    for (int i=0; i < DEFAULT_VIEWS_COUNT; i++)  // First, add default views
    {
        int j = comboBox->AddString(szDefaultViews[i]);
        //comboBox->SetItemData(j, VIEW_OTHER);
        TRACE1("Added view '%s'\n", (const char*)szDefaultViews[i]);
    }
    // Add database view if appropriate
    // We're now allowing CRecordView in the combo iff DB support is
    //  chosen and a data source was selected.
    CString tmp;
    if (projOptions.m_bDBHeader &&
        DoesSymbolExist("DB_SOURCE"))
    {
        int j;
		switch(projOptions.m_nDataSource) {
			case dbOdbc:
				j = comboBox->AddString("CRecordView");
				break;
			case dbDao:	
				j = comboBox->AddString("CDaoRecordView");
				break;
			case dbOledb:	
				j = comboBox->AddString("COleDBRecordView");
				break;
			default:
				break;
		}
        //comboBox->SetItemData(j, VIEW_RECORD);
    }
	// Don't add editview if we're a type of OLE container, or we're using splitters
    if (projOptions.m_nOleType != OLE_CONTAINER && projOptions.m_nOleType != OLE_CSERVER
		&& (!DoesSymbolExist("SPLITTER")))
    {
        int j = comboBox->AddString("CEditView");
        //comboBox->SetItemData(j, VIEW_OTHER);
    }

	// Don't add htmlview if we're a type of OLE container
    if (projOptions.m_nOleType == OLE_NO)
    {
        int j = comboBox->AddString("CHtmlView");
        //comboBox->SetItemData(j, VIEW_OTHER);
    }

    CDialog::OnInitDialog();        // does DDX
    CenterWindow();
    return TRUE;
}

void CNamesDlg::OnSelectClass()
{
    // get current selection
    int iSel = m_classList.GetCurSel();
    ASSERT(iSel >= 0);
    if (m_iSelOld == iSel)
        return;

    // get class ID associated with that item
    int iClass = (int)m_classList.GetItemData(iSel);
    if (!UpdateData(TRUE))      // save old class
    {
        // restore to old selection
        m_classList.SetCurSel(m_iSelOld);
        return;
    }

    // class name may have changed
    CString oldName;
    int iClassOld = (int)m_classList.GetItemData(m_iSelOld);
    m_classList.GetText(m_iSelOld, oldName);
    if (projOptions.m_names.strClass[iClassOld] != oldName)
    {
        m_classList.DeleteString(m_iSelOld);
        m_classList.InsertString(m_iSelOld, projOptions.m_names.strClass[iClassOld]);
        m_classList.SetItemData(m_iSelOld, iClassOld);
        m_classList.SetCurSel(iSel);
    }

    m_iSelOld = iSel;
    UpdateData(FALSE);      // re-fill data with new class
}

void CNamesDlg::DoDataExchange(CDataExchange* pDX)
{
    int iClass = (int)m_classList.GetItemData(m_iSelOld);
    ASSERT(iClass >= 0 && iClass < classMax);

    DDX_Text(pDX, IDC_CLASS_NAME, projOptions.m_names.strClass[iClass]);
	projOptions.m_names.strClass[iClass].ReleaseBuffer();
    DDV_ClassName(pDX, projOptions.m_names.strClass[iClass]);
    if (iClass != classView)
	{
        DDX_Text(pDX, IDC_BASE_CLASS, projOptions.m_names.strBaseClass[iClass]);
		projOptions.m_names.strBaseClass[iClass].ReleaseBuffer();
	}
    DDV_ClassName(pDX, projOptions.m_names.strBaseClass[iClass]);
    if (iClass == classView)
	{
        DDX_CBString(pDX, IDC_BASE_CLASS_COMBO, projOptions.m_names.strBaseClass[iClass]);
		projOptions.m_names.strBaseClass[iClass].ReleaseBuffer();
	}
    DDV_ClassName(pDX, projOptions.m_names.strBaseClass[iClass]);
    DDX_Text(pDX, IDC_HFILE, projOptions.m_names.strHFile[iClass]);
 	projOptions.m_names.strHFile[iClass].ReleaseBuffer();
 	DDV_FileName(pDX, projOptions.m_names.strHFile[iClass], ".h");
    DDX_Text(pDX, IDC_IFILE, projOptions.m_names.strIFile[iClass]);
 	projOptions.m_names.strIFile[iClass].ReleaseBuffer();
    DDV_FileName(pDX, projOptions.m_names.strIFile[iClass], ".cpp");

    BOOL bEnableExtras = FALSE;
    BOOL bEnableBaseEdit = FALSE;       // never enabled today
    BOOL bEnableFileEdit = TRUE;
    BOOL bEnableCombo = FALSE;
    // special cases
    switch (iClass)
    {
    case classDoc:
        break;

    case classApp:
        bEnableFileEdit = FALSE;    // can't change it !
        break;

    case classView:
        bEnableCombo = TRUE;
        break;
    }

    GetDlgItem(IDC_HFILE)->EnableWindow(bEnableFileEdit);
	CString strLabel;
	strLabel.LoadString(bEnableFileEdit ? IDS_HEADERF_ACC : IDS_HEADERF_NOACC);
    GetDlgItem(IDC_STATIC_HEADER)->SetWindowText(strLabel);
    GetDlgItem(IDC_IFILE)->EnableWindow(bEnableFileEdit);
	strLabel.LoadString(bEnableFileEdit ? IDS_IMPLF_ACC : IDS_IMPLF_NOACC);
    GetDlgItem(IDC_STATIC_IMP)->SetWindowText(strLabel);
    GetDlgItem(IDC_BASE_CLASS)->EnableWindow(bEnableBaseEdit);
	strLabel.LoadString(bEnableCombo ? IDS_BASEC_ACC : IDS_BASEC_NOACC);
	GetDlgItem(IDC_BASE_STATIC)->SetWindowText(strLabel);

    GetDlgItem(IDC_BASE_CLASS_COMBO)->ShowWindow(bEnableCombo);
    GetDlgItem(IDC_BASE_CLASS_COMBO)->EnableWindow(bEnableCombo);
    GetDlgItem(IDC_BASE_CLASS)->ShowWindow(!bEnableCombo);

    // check for unique names (classes and files)
    if (pDX->m_bSaveAndValidate)
    {
        // check for reserved class names
        if (IsReservedClassName(projOptions.m_names.strClass[iClass]))
        {
            AfxMessageBox(IDP_RESERVED_CLASS, MB_OK, 0);
            pDX->PrepareEditCtrl(IDC_CLASS_NAME);
            pDX->Fail();
        }

        for (int iOther = 0; iOther < classMax; iOther++)
        {
            if (iOther == iClass)
                continue;
			if (!projOptions.m_names.bCategoryEnabled[iOther])
				continue;
            if (projOptions.m_names.strClass[iClass] == projOptions.m_names.strClass[iOther])
            {
                AfxMessageBox(IDP_DUPLICATE_CLASS, MB_OK, 0);
                pDX->PrepareEditCtrl(IDC_CLASS_NAME);
                pDX->Fail();
            }
            if (!projOptions.m_names.strHFile[iClass].CompareNoCase(projOptions.m_names.strHFile[iOther]))
            {
                AfxMessageBox(IDP_DUPLICATE_FILE, MB_OK, 0);
                pDX->PrepareEditCtrl(IDC_HFILE);
                pDX->Fail();
            }
            if (!projOptions.m_names.strIFile[iClass].CompareNoCase(projOptions.m_names.strIFile[iOther]))
            {
                AfxMessageBox(IDP_DUPLICATE_FILE, MB_OK, 0);
                pDX->PrepareEditCtrl(IDC_IFILE);
                pDX->Fail();
            }
        }
    }
}

void CNamesDlg::OnSelchangeBaseClass()
{
    CComboBox *viewCombo = (CComboBox*) GetDlgItem(IDC_BASE_CLASS_COMBO);
	
	// Set symbol for new base class so we can refill the classes
	//  listbox if necessary (e.g., if new base class is CRecordView,
	//  we must add CRecordset to listbox).
	viewCombo->GetLBText
    	(viewCombo->GetCurSel(), projOptions.m_names.strBaseClass[classView]);
	SetBaseViewSymbols();
	fillBaseClasses(projOptions.m_names.strBaseClass);
    m_classList.ResetContent();
    int nViewClass = 0;
    // fill with class list - non-sorted so we can use index for 'iClass'
    // Only enter those classes that are enabled
    ASSERT(!(m_classList.GetStyle() & LBS_SORT));
    fillCategoryEnabled(projOptions.m_names.bCategoryEnabled);
    for (int iClass = 0; iClass < classMax; iClass++)
    {
        if (projOptions.m_names.bCategoryEnabled[iClass])
        {
            int i = m_classList.AddString(projOptions.m_names.strClass[iClass]);
            TRACE2("Added class '%s'@%d\n",
                (const char*)projOptions.m_names.strClass[iClass], i);
            m_classList.SetItemData(i, iClass);
            if (iClass == classView) nViewClass = i;
        }
    }
    m_classList.SetCurSel(nViewClass);
}

BOOL CNamesDlg::CanDestroy()
{
	if (!UpdateData(TRUE))
		return FALSE;

	CString strViewBaseClass = projOptions.m_names.strBaseClass[classView];

	// CRichEditView requires a container
	if (strViewBaseClass == "CRichEditView"
		&& projOptions.m_nOleType != OLE_CONTAINER && projOptions.m_nOleType != OLE_CSERVER)
	{
		// User chose richedit w/out container support.  Do they want container support?
		if (AfxMessageBox(IDP_NOOLE_THEN_RICHEDIT, MB_OKCANCEL) == IDCANCEL)
			// ...no
			return FALSE;

		// ...yes.  Add container support for them.
		if (projOptions.m_nOleType == OLE_NO)
			projOptions.m_nOleType = OLE_CONTAINER;
		else
			projOptions.m_nOleType = OLE_CSERVER;
		SetOleSymbols();
	}

	if (projOptions.m_bSplitter)
	{
		if (!CanViewClassBeUsedWithSplitter(strViewBaseClass))
		{
			INT iResult = ::AfxMessageBox(IDP_CANT_USE_SPLITTER, MB_OKCANCEL);

			if (iResult == IDCANCEL)
				return FALSE;

			projOptions.m_bSplitter = FALSE;

			SetSplitterSymbols();
		}
	}

	return TRUE;
}

void CNamesDlg::WriteValues()
{
	UpdateData(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CMainDlg dialog

static BOOL fKnownWizardDeterminedAlready;
static BOOL fIsKnownWizard;

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
    : C3dDialog(CMainDlg::IDD, pParent)
{
	m_nChildDlgLeft = -1;
    InitializeOptions();
	m_nMaxUserSteps = -1;
	m_nHasHelpButton = -1;		// any value is OK as long as it isn't valid...
	fKnownWizardDeterminedAlready = fIsKnownWizard = FALSE;
    InitDialogs();
    //{{AFX_DATA_INIT(CMainDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
    C3dDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CMainDlg)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMainDlg, C3dDialog)
    //{{AFX_MSG_MAP(CMainDlg)
    ON_BN_CLICKED(IDC_BACK, OnClickedBack)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_END, OnClickedEnd)
    //ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_WM_PAINT()
    ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_HELP, OnHelp)
    ON_WM_ACTIVATE()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainDlg message handlers

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
/* #ifndef PRIVATE
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
        if (pMsg->wParam == VK_TAB &&
            (SendMessage(WM_GETDLGCODE) &
             (DLGC_WANTALLKEYS | DLGC_WANTMESSAGE | DLGC_WANTTAB)) == 0)
        {
            CWnd* pCtl = GetFocus();
            if (IsChild(pCtl))
            {
                do
                {
                    int nKeyState = GetKeyState(VK_SHIFT);
                    pCtl = pCtl->GetWindow(nKeyState < 0 ? GW_HWNDPREV : GW_HWNDNEXT);

                    if (pCtl == NULL)
                    {
                        CDialog* pDlg = GetInnerDlg();
                        pDlg->SetFocus();
                        pCtl = GetFocus();
                        pCtl = pCtl->GetWindow(nKeyState < 0 ? GW_HWNDLAST : GW_HWNDFIRST);
                        if ((pCtl->GetStyle() & (WS_DISABLED | WS_TABSTOP)) != WS_TABSTOP)
                            pCtl = pDlg->GetNextDlgTabItem(pCtl, nKeyState < 0);
                        MakeOKDefault(this);
						pCtl->SetFocus();
						
						// If pCtl is an edit control, select its contents.
						char szClassName[256];
						::GetClassName(pCtl->GetSafeHwnd(), szClassName, 256);
						if (!strcmp(szClassName, "Edit"))
						{
							((CEdit*) pCtl)->SetSel(DWORD(MAKELONG(0, -1)));
						}
                        return TRUE;
                    }

                }
                while ((pCtl->GetStyle() & (WS_DISABLED | WS_TABSTOP)) != WS_TABSTOP);
            }
        }
		break;

	case WM_SYSCHAR:
	case WM_CHAR:
		if (IsaMainBtn((char) pMsg->wParam))
#ifdef VS_PACKAGE
			return CDialog::PreTranslateMessage(pMsg);
#else
			return C3dDialog::PreTranslateMessage(pMsg);
#endif	// VS_PACKAGE
		// If the user tries a kbd equivalent of a control in CSeqDlg, pass
		//  message to CSeqDlg.  (Since focus is in CMainDlg, focus must be
		//  on a button.  Thus, we pass WM_CHAR and WM_SYSCHAR messages.)
		HWND origHwnd = pMsg->hwnd;
		CDialog* pDlg = GetInnerDlg();
		pMsg->hwnd = pDlg->GetWindow(GW_CHILD)->GetSafeHwnd();
		if (pDlg->IsDialogMessage(pMsg))
		{
			return TRUE;
		}
		else
		{
			pMsg->hwnd = origHwnd;
#ifdef VS_PACKAGE
			return CDialog::PreTranslateMessage(pMsg);
#else
			return C3dDialog::PreTranslateMessage(pMsg);
#endif	// VS_PACKAGE
		}
	}
#endif //!PRIVATE
*/
	if (pMsg->message == WM_HELP && !IsWindowEnabled())
	{
		OnHelp();
		return TRUE;
	}
	return C3dDialog::PreTranslateMessage(pMsg);
}

void CMainDlg::InitializeUserVars()
{
	m_pCurrUserStepDlg = NULL;
	m_pCurrUserSeqDlg = NULL;
	m_nCurrUserStep = 0;
}

BOOL CMainDlg::OnInitDialog()
{
    C3dDialog::OnInitDialog();
    CenterWindow();
	m_strLastSubdir = "";
	
    m_nCurrIndex = -1;
	m_nRightmostBtn = IDC_END;
	m_pConfirmDlg = NULL;

	InitializeUserVars();

	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG) szMainDlgUserData);

    return TRUE;
}

void CMainDlg::FakeShellExecute(HWND hWnd, LPCSTR szVerb, LPCSTR szFile)
{
    // open the project file
    //  (don't use ShellExecute since VB may steal the .MAK suffix)
    //  (don't pass in an HWND since that may wedge the IDE)
    char szFullPath[_MAX_PATH];

    // Add info to the projects created by $$NEWPROJ directive as well
    pTermStatus = &g_TermStatus;
    ASSERT (*(pTermStatus->szFullName) == '\0');
    strcpy(pTermStatus->szFullName, szFile);
    while (pTermStatus) {
        if (!_fullpath(szFullPath, pTermStatus->szFullName, _MAX_PATH))
        {
            TRACE("FakeShellExecute failed - _AfxFullPath failed\n");
            return;
        }		
        strncpy(pTermStatus->szFullName, szFullPath, MAX_PATH);
        pTermStatus->nTermType = APPWIZTERMTYPE_APPWIZPROJ;
        pTermStatus = pTermStatus->nextAppWizTermStatus;
    }
    pTermStatus = &g_TermStatus;

    TRACE("FakeShellExecute worked!\n");
}

/////////////////////////////////////////////////////////////////////////////
// CTheDLL

CTheDLL theDLL;

CTheDLL::CTheDLL()
{
	m_bFirstTime = TRUE;
	m_pMainDlg = NULL;
}

BOOL CTheDLL::InitInstance()
{
    /* DWORD dwDisp;
    VERIFY(RegCreateKeyEx(HKEY_CURRENT_USER, (const char*) szMSVCSection,
        0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL, &m_hKey, &dwDisp)
        == ERROR_SUCCESS);*/

    return TRUE;
}



// This should be called immediately after LoadLibrarying AppWizard in the build system's
//  handler for File.New.Project.
void CTheDLL::InitAppWiz(HWND hWnd)
{
	OleInitialize(NULL);
	// Set pLaunchParent to window handle passed to us
	pLaunchParent = CWnd::FromHandlePermanent(hWnd);
	ASSERT(pLaunchParent != NULL);

	pTermStatus = &g_TermStatus;
	pTermStatus->strlDepFiles.RemoveAll();
	pTermStatus->strlNonPrjFiles.RemoveAll();
	pTermStatus->strlNonBldFiles.RemoveAll();
	pTermStatus->strlResFiles.RemoveAll();
	pTermStatus->strlHelpFiles.RemoveAll();
	pTermStatus->strlTemplFiles.RemoveAll();
	if (m_bFirstTime)
	{
		InitInstance();
		m_bFirstTime = FALSE;
	}
	// Initialize variables
	pTermStatus->nTermType = APPWIZTERMTYPE_CANCEL;
	pTermStatus->strDefFile.Empty();
	//pTermStatus->strClwFile.Empty();
	m_bQuit = FALSE;
	strlProjTypes.RemoveAll();
	projOptions.m_strProj = "";
	InitPlatformsSelected();
	g_PlatNameConvert.Init();
	extern BOOL g_bLoadFromAppwiz;			// See top of codegen.cpp
	g_bLoadFromAppwiz = FALSE;

	// Erase any leftover names from previous invocation
	for (int i=0; i < classMax; i++)
	{
		projOptions.m_names.strClass[i].Empty();
		projOptions.m_names.strBaseClass[i].Empty();
		projOptions.m_names.strHFile[i].Empty();
		projOptions.m_names.strIFile[i].Empty();
	}
	projOptions.m_names.strDocFileExt.Empty();
	//projOptions.m_names.strDocFilter.Empty();

	// Get appWizard's module's directory (eg. \bin\)
	DWORD nSize = GetModuleFileName(NULL, m_strAppwizDir.GetBuffer(MAX_PATH), MAX_PATH);
	m_strAppwizDir.ReleaseBuffer();
	if (nSize == 0 || nSize > MAX_PATH)
		m_strAppwizDir = "";
	else
		m_strAppwizDir = m_strAppwizDir.Left(m_strAppwizDir.ReverseFind('\\'));

	strlProjTypes.RemoveAll();

#ifdef VS_PACKAGE
	IBuildWizard *pBldWizIFace = &g_CBldWiz;
#else
	LPBUILDWIZARD pBldWizIFace;
	theApp.FindInterface(IID_IBuildWizard, (LPVOID FAR *)&pBldWizIFace);
#endif	
	
	const CStringList* pList;

	pBldWizIFace->GetTargetTypeNames(&pList, NULL, TRUE);
	pBldWizIFace->Release();
	POSITION pos = pList->GetHeadPosition();
    while (pos != NULL)
		strlProjTypes.AddTail(pList->GetNext(pos));

	g_strlPlatformsDisplayed.RemoveAll();

	// If this ASSERT fires, there's probably a memory leak due to successive calls
	//  to InitAppWiz without an intervening call to ExitAppWiz.
	ASSERT(m_pMainDlg == NULL);

	m_pMainDlg = new CMainDlg(pLaunchParent);
}

#ifndef VS_PACKAGE
int CTheDLL::RunAppWiz(HWND hWnd, HBUILDER* phBld)
{
	ASSERT(FALSE);
	return 0;
}
#endif	// VS_PACKAGE

BOOL DoesFileExist(const char* szFile)
{
    struct _stat st;
    return (_stat(szFile, &st) == 0);
}

BOOL DestroyFile(const char* szFile)
{
	if (!DoesFileExist(szFile))
		return TRUE;
	CString strPrompt;
	AfxFormatString1(strPrompt, IDP_CANT_DELETE_FILE, szFile);
	while (::remove(szFile) == -1)
	{
		if (AfxMessageBox(strPrompt, MB_RETRYCANCEL | MB_ICONEXCLAMATION) == IDCANCEL)
			return FALSE;
	}
	return TRUE;
}

#ifndef VS_PACKAGE
// Registers AppWizard's main window with the main frame, so that
//  F1 help is diverted to AppWizard.
class CRegisterWithFrame
{
public:
	CRegisterWithFrame(CWnd* pwndAppWiz)
	{
//		m_pMainWnd = AfxGetMainWnd();

		ASSERT (pwndAppWiz->GetSafeHwnd() != NULL);
		theApp.SetAppWizHwnd(pwndAppWiz->GetSafeHwnd());
	}
	~CRegisterWithFrame()
	{
		theApp.SetAppWizHwnd(NULL);
	}
protected:
//	CWnd* m_pMainWnd;
};
#endif	// VS_PACKAGE

int CTheDLL::RunAppWizSteps()
{
	// If these ASSERTs	fire, you didn't call InitAppWiz before calling this!
	ASSERT(pLaunchParent != NULL);
	ASSERT(m_pMainDlg != NULL);

	pLaunchParent->EnableWindow(FALSE);
	if (!m_pMainDlg->GetSafeHwnd())
		m_pMainDlg->Create(pLaunchParent);
	m_bQuit = FALSE;

	{
#ifndef VS_PACKAGE
		// Register our existence with the main frame
		CRegisterWithFrame rwf(m_pMainDlg);
#endif	// VS_PACKAGE
		
		if (m_pMainDlg->StartAppWizSteps())
		{
			// The steps have started, so pump messages until it's
			//  time to say goodbye.
			while (!m_bQuit)
				AfxGetApp()->PumpMessage();
		}
		else
		{
			// We didn't even get a chance to start the steps, before
			//  it was determined that we have to rerun CPromptDlg
			pTermStatus->nTermType = APPWIZTERMTYPE_RERUNPROMPTDLG;
		}
	}

	pLaunchParent->EnableWindow(TRUE);
	ExitInstance();

	// If we're leaving for good, get rid of CMainDlg.  Otherwise, leave it invisible,
	//  and we'll delete it in ExitAppWiz.
	if (m_pMainDlg->GetSafeHwnd())
		m_pMainDlg->ShowWindow(SW_HIDE);

	return pTermStatus->nTermType;
}

void CTheDLL::ExitAppWiz()
{
	projOptions.m_UserExtension.Detach();
	if (m_pMainDlg != NULL)
	{
		if (m_pMainDlg->GetSafeHwnd())
			m_pMainDlg->DestroyWindow();
		delete m_pMainDlg;
		m_pMainDlg = NULL;
	}
	OleUninitialize();
}

void CMainDlg::InitializeOptions()
{
	SetCurrentTrack();

	langDlls.Init();

	projOptions.m_bMayBeAppWizTrack = FALSE;

	projOptions.m_names.m_bUpdateNames = TRUE;
	SetLanguageSymbols();

    // Database options
    projOptions.m_bDBHeader = projOptions.m_bDBSimple
	     = projOptions.m_bAutoDetect = FALSE;
    projOptions.m_nDataSource = dbOdbc;

	SetDBSymbols();

    // OLE options
    projOptions.m_nOleType = OLE_NO;
    projOptions.m_bOleAutomation = FALSE;
	projOptions.m_bOleCompFile = FALSE;
	projOptions.m_bOcx = TRUE;	// Default to true per DevStudio '96 Bug 5100
	projOptions.m_bActiveDocServer = FALSE;
	projOptions.m_bActiveDocContainer = FALSE;
	SetOleSymbols();

	// Application options
    projOptions.m_options = (1<<(IDCD_POTOOLBAR-IDCD_POBASE))
		| (1<<(IDCD_POSTATUSBAR	-	IDCD_POBASE))
        | (1<<(IDCD_POPRINT		-	IDCD_POBASE))
		| (1<<(IDCD_PO3D		-	IDCD_POBASE))
        | (1<<(IDCD_POABOUT		-	IDCD_POBASE));

    if( projOptions.m_bNoDocView )
		 projOptions.m_options &= ~(1<<(IDCD_POPRINT - IDCD_POBASE));

	SetAppOptionsSymbols();

	projOptions.m_nSizeMRU = 4;
	SetSizeMRUSymbols(projOptions.m_nSizeMRU);

	// CDocStringsDlg tracking variables
/*	projOptions.m_bUpdateFilter
		= projOptions.m_bUpdateFileNew
		= projOptions.m_bUpdateRegID
		= projOptions.m_bUpdateRegName = TRUE;*/

	projOptions.m_nFrameStyles = GetDefaultFrameStyles();
	SetFrameSymbols();
	projOptions.m_nMenuStyle = 0;
	SetMenuSymbols();

	projOptions.m_bSplitter = FALSE;
	SetSplitterSymbols();

	projOptions.m_nChildFrameStyles = GetDefaultChildFrameStyles();
	SetChildFrameSymbols();

	// Project options
	projOptions.m_projOptions = (1<<(IDCD_POVERBOSE-IDCD_POPROJBASE))
		| (1<<(IDCD_PODLL		-	IDCD_POPROJBASE));
	projOptions.m_bExtdll = FALSE;
	projOptions.m_bATLSupport = FALSE;
	projOptions.m_nProjStyle = PS_MFCNORMAL;
	SetDllSymbol();
	SetProjOptionsSymbols();
}

void CMainDlg::GetAllSymbols()
{
	GetProjectTypeSymbol();
	GetFrameSymbols();
	GetChildFrameSymbols();
	GetSizeMRUSymbols();

	GetSplitterSymbols();
	GetLanguageSymbols();

	GetAppOptionsSymbols();

	GetProjOptionsSymbols();

	GetOleSymbols();

	GetClassesSymbols();
	GetDBSymbols();

	GetDocumentSymbols();
	
	// MAC-specific
	if (DoesSymbolExist("TARGET_MAC"))
	{
		GetSymbol("FILE_TYPE", projOptions.m_names.strDocFileType);
		GetSymbol("CREATOR", projOptions.m_names.strDocFileCreator);
		GetSymbol("MAC_FILTER", projOptions.m_names.strMacFilter);
	}
}


static int nAppTrack[] = {0, 1, 2, 3, 4, 5, 6, 9};
#define APP_LAST_INDEX 7

static int nDlgTrack[] = {0, 1, 7, 5, 6, 9};
#define DLG_LAST_INDEX 5

static int nDLLTrack[] = {0, 8, 9};
#define DLL_LAST_INDEX 2

#define DBDLG 2

// Pages through the various dialogs.
void CMainDlg::InitDialogs()
{
    CSeqDlg* dlg1 = (CSeqDlg*) new CProjDlg(this);
    CSeqDlg* dlg2 = (CSeqDlg*) new CDBDlg(this);
    CSeqDlg* dlg3 = (CSeqDlg*) new COleDlg(this);
    CSeqDlg* dlg4 = (CSeqDlg*) new CDocAppOptionsDlg(this);
    CSeqDlg* dlg5 = (CSeqDlg*) new CProjOptionsDlg(this);
    CSeqDlg* dlg6 = (CSeqDlg*) new CNamesDlg(this);
    CSeqDlg* dlg7 = (CSeqDlg*) new CDlgAppOptionsDlg(this);
	CSeqDlg* dlg8 = (CSeqDlg*) new CDllProjOptionsDlg(this);

    m_pDialogClasses[0] = NULL;            // Placeholder for where CPromptDlg goes
    m_pDialogClasses[1] = dlg1;
    m_pDialogClasses[2] = dlg2;
    m_pDialogClasses[3] = dlg3;
    m_pDialogClasses[4] = dlg4;
    m_pDialogClasses[5] = dlg5;
    m_pDialogClasses[6] = dlg6;
    m_pDialogClasses[7] = dlg7;
	m_pDialogClasses[8] = dlg8;
	m_pDialogClasses[9] = NULL;

	// This is defined in pagedlg.cpp.  When we set this, we have
	//  permission to call CAppWizStepDlg's constructor with 0.
	void AppWizCallingStructor(BOOL bAppWizCallingStructor);
	AppWizCallingStructor(TRUE);

	for (int i=0; i < NUM_DLG_CLASSES; i++)
	{
		m_pExtensionKeys[i] = new CAppWizStepDlg(0);
		m_DlgMap[m_pExtensionKeys[i]] = m_pDialogClasses[i];
	}

	AppWizCallingStructor(FALSE);
}

CAppWizStepDlg* CMainDlg::ExtKeyFromID(AppWizDlgID nID)
{
	if (1 > nID || nID > NUM_DLG_CLASSES-2)
	{
		CString strNum;
		strNum.Format("%d", nID);
		ReportErrorExt(IDS_GETDLG_BAD_RANGE, strNum);
	}
	return m_pExtensionKeys[nID];
}

CSeqDlg* CMainDlg::ExtSeqDlgFromKey(CAppWizStepDlg* pStepDlg)
{
	CSeqDlg* pSeqDlg;
	VERIFY (m_DlgMap.Lookup(pStepDlg, pSeqDlg));
	ASSERT_VALID(pSeqDlg);
	return pSeqDlg;
}

struct AltHelpIDEntry
{
	int nDlg;
	int nFirstHelpID;
};

// This structure and GetAltHelpID exist so that unambiguous help IDs
//  are sent.  Some dialogs appear on more than one track (e.g.,
//  Classes dialog)-- and thus the dialog will have a different
//  "step#" depending on where it appears in each track.  In order for
//  the documentation to correctly present the dialog's title (which
//  includes the step number), we pass distinct help IDs for the same
//  dialog on different tracks.
// AltHelpIDs's entries work like this:  The first field is the index
//  into m_pDialogClasses of the dialog.  (Not the index into a
//  track.)  The last field is an alternate ID for the dialog, which
//  is defined to be distinct from all other alternate IDs, all other
//  dialog IDs, and all other offsets from other alternate IDs.  The
//  latter is necessary, since, for example, the project options
//  dialog appears on three tracks: thus we use the original
//  dialog ID for its occurrence on nAppTrack, IDD_PROJOPTIONS_ALT for
//  its occurrence on nDlgTrack, and an offset IDD_PROJOPTIONS_ALT+1
//  for its occurrence on nDLLTrack.  The last entry in AltHelpIDs
//  is marked by a "-1" in the first field.
// The _ALT symbols are defined in resource.h.
// NOTE: All this help ID hokey-dokeying occurs before
//  CTheDLL::WinHelp adds the final offset to avoid collisions with
//  the rest of msvc.

static AltHelpIDEntry AltHelpIDs[] =
{
    { 5, IDD_PROJOPTIONS_ALT },
    { 6, IDD_NAMES_ALT },
    { -1, -1 },
};

// This takes an index into m_pDialogClasses and a track.  If it finds
//  the dialog in AltHelpIDs, it returns a suitable alternate ID to be
//  passed as the LPARAM to the Help Command.  Otherwise, it returns
//  0, which is the default LPARAM to the Help Command.
LPARAM GetAltHelpID(int nDlg, int* pnTrack)
{
	int i=0;
	int iDlg;
	while ((iDlg = AltHelpIDs[i].nDlg) != -1)
	{
		if (nDlg == iDlg)
		{
			// If we're on the regular app track, keep the ID the same
			if (pnTrack == (int*) nAppTrack)
				return (LPARAM) 0;

			// Generate helpID based on info in AltHelpIDs.  Add
			//  HID_BASE_RESOURCE, since CDialog::OnCommandHelp won't
			//  do this unless we send it 0 as the LPARAM.
			LPARAM nRetval = HID_BASE_RESOURCE + AltHelpIDs[i].nFirstHelpID;
			if (pnTrack == (int*) nDlgTrack)
				return nRetval;
			else
				return nRetval+1;
		}
		i++;
	}
	return (LPARAM) 0;
}

int CMainDlg::GetSeqDlgIndex(CSeqDlg* pSeqDlg)
{
	ASSERT (pSeqDlg != NULL);
	for (int i=0; i < NUM_DLG_CLASSES; i++)
	{
		if (pSeqDlg == m_pDialogClasses[i])
			return i;
	}
	ASSERT(FALSE);
	return 1;
}

#ifdef _DEBUG
#define szSuffix "D"
#else
#define szSuffix ""
#endif

// Table of the names of custom app-wizards whose control-help is included in the DevStudio
// help file.  For other app-wizards, we use the wizard's own .HLP file.
static LPCTSTR rgszKnownAppWizNames[] =
{
	"VJAPPWIZ" szSuffix,
	"INETAWZ" szSuffix,
	"MFCTLWZ" szSuffix,
	"ATLWIZ" szSuffix,
	"ADDINWZ" szSuffix,
	"FORMBAS" szSuffix,
	"CONWZ" szSuffix,
	"DLLWZ" szSuffix,
	"EXEWZ" szSuffix,
	"GENWZ" szSuffix,
	"LIBWZ" szSuffix,
	"MAKWZ" szSuffix,
	"XPWIZ" szSuffix,
	NULL
};

static BOOL IsKnownWizard()
{
	if (fKnownWizardDeterminedAlready)
		return fIsKnownWizard;

	fKnownWizardDeterminedAlready = TRUE;	// we're just about to determine it
	fIsKnownWizard = FALSE;

    if (IsUserAWX())
    {
		CString strAwxName = projOptions.m_UserExtension.GetAWXName();
		_TCHAR szFname[_MAX_FNAME];
		_tsplitpath(strAwxName, NULL, NULL, szFname, NULL);

		for (LPCTSTR *psz = rgszKnownAppWizNames; *psz != NULL && !fIsKnownWizard; psz++)
		{
			if (_tcsicmp(szFname, *psz) == 0)
			{
				fIsKnownWizard = TRUE;
			}
		}
    }

	return fIsKnownWizard;
}

static BOOL IsDefinitelyKnownWizard()	// slightly broader definition than IsKnownWizard
{
	return (!IsUserAWX() || projOptions.m_UserExtension.m_bCtlWiz || IsKnownWizard());
}

#ifdef _DEBUG
#define VJAPPWIZ "VJAPPWIZD.AWX"
#else
#define VJAPPWIZ "VJAPPWIZ.AWX"
#endif

// Black and white determination of help file name.
// Returns wrong result for standard AppWiz step called
// from within a custom AppWizard.  (Gray area case)
void GetHelpFile(CString& strHelpFile)
{
	if (IsDefinitelyKnownWizard())	// Known Wiz
	{
		strHelpFile = AfxGetApp()->m_pszHelpFilePath;
		if (!_access(strHelpFile, 04) == 0)		// not where it is expected.  Try again in help dir
		{
			_TCHAR szFile[_MAX_EXT];
			_splitpath(strHelpFile, NULL, NULL, szFile, NULL);
			CString strHelpDir;
			ASSERT(FALSE);
			//Fix this
			//::GetHelpDir(strHelpDir);
			strHelpFile.Format("%s\\%s.hlp", (LPCTSTR)strHelpDir, szFile);
		}
	}
	else
	{
		// custom appwiz showing custom step
		// Get extension's help file path
		//CString strHelpFile = projOptions.m_UserExtension.GetAWXName();
	    strHelpFile = projOptions.m_UserExtension.GetAWXName();
		int nLength = strHelpFile.GetLength();
		if (nLength < 3)
		{
			ASSERT(FALSE);
			return;
		}
		strHelpFile = strHelpFile.Left(nLength-3) + "hlp";
	}
}

void CMainDlg::OnHelp()
{
	// custom appwiz showing custom step
	// Get extension's help file path
    CString strHelpFile;
    BOOL    fUserAWX = IsUserAWX();
	BOOL	fKnownWizard = IsKnownWizard();

	if (fUserAWX)
	{
		strHelpFile = projOptions.m_UserExtension.GetAWXName();
		_TCHAR szFname[_MAX_FNAME];
		_tsplitpath(strHelpFile, NULL, NULL, szFname, NULL);
	}

	if (IsDefinitelyKnownWizard())
	{
		// No custom AppWizard
		if (!IsWindowEnabled())
		{
			// appwiz isn't on top.  Some other modal dialog is instead (like
			//  pick data source or confirmation dialog).  Let that dialog
			//  supply its own help ID.
			C3dDialog::OnHelp();
			return;
		}

	    if (!(0 < m_nCurrIndex && m_nCurrIndex < m_nCurrLastIndex))
	    {
			ASSERT(fKnownWizard || projOptions.m_UserExtension.m_bCtlWiz);	// all of IsDefinitelyKnownWizard except !IsUserAWX
			CDialog* pDlg = GetInnerDlg();
			// pDlg->SendMessage(WM_COMMANDHELP, 0, 0);
	        return;
	    }

		HWND hWnd = GetInnerDlg()->GetSafeHwnd();
	    ASSERT(hWnd != NULL);
		::SendMessage(hWnd, WM_COMMANDHELP, 0,
			GetAltHelpID(m_nCurrTrack[m_nCurrIndex], m_nCurrTrack));
	}
	else if (m_pCurrUserSeqDlg != NULL)
	{
		// custom appwiz showing standard appwiz step
		if (!IsWindowEnabled())
		{
			// appwiz isn't on top.  Some other modal dialog is instead (like
			//  pick data source or confirmation dialog).  Let that dialog
			//  supply its own help ID.
			C3dDialog::OnHelp();
			return;
		}
		int nDlg = GetSeqDlgIndex(m_pCurrUserSeqDlg);
		HWND hWnd = GetInnerDlg()->GetSafeHwnd();
	    ASSERT(hWnd != NULL);
		::SendMessage(hWnd, WM_COMMANDHELP, 0,
			GetAltHelpID(nDlg, m_nCurrTrack));
	}
	else
	{
		// custom appwiz showing custom step
		// Get context
		DWORD dwContext;
		CWnd* pAppMainWnd = AfxGetApp()->GetMainWnd();
		CWnd* pPopup = pAppMainWnd->GetLastActivePopup();
		if (pPopup == pAppMainWnd || pPopup == this)
		{
			// There's nothing on top of the custom appwiz step dialog
			dwContext = ((CAppWizStepDlg*) GetInnerDlg())->m_nIDTemplate + HID_BASE_RESOURCE;
		}
		else
		{
			// There's a modal dialog or something on top of the custom appwiz
			//  step dialog.  Use that modal guy's help context
			dwContext = pPopup->SendMessage(WM_HELPHITTEST, 0, 0);
		}

		// Directly call WinHelp
		::GetHelpFile(strHelpFile);
		::WinHelp(AfxGetApp()->GetMainWnd()->GetSafeHwnd(),
			strHelpFile, HELP_CONTEXT, dwContext);
	}
}

// This gets called when the user clicks "Next", or clicks "Finish" from
//  the last step (since, in that case, Finish == IDOK).
void CMainDlg::OnOK()
{
	if (IsUserAWX())
	{
		// Handle IDOK when a user dialog is present
		if (m_nCurrUserStep == m_nMaxUserSteps
			&& m_nMaxUserSteps != -1)
		{
			// Here we're the last step, so Finish == IDOK.
			OnClickedEnd();
			return;
		}
	    // This isn't the last dialog, so bring on the next one.
		ActivateUserDlg(TRUE);
	}
	else
	{
		// Handle IDOK when one of ours is present
		ASSERT(m_nCurrIndex != m_nCurrLastIndex);
		if (m_nCurrIndex == m_nCurrLastIndex - 1)
		{
			// Here we're the last step, so Finish == IDOK.
			OnClickedEnd();
			return;
		}
	    // This isn't the last dialog, so bring on the next one.
	    ActivateDlg(m_nCurrIndex + 1);
	}
}

void CMainDlg::OnClickedBack()
{
	if (IsUserAWX())
	{
		ActivateUserDlg(FALSE);
	}
	else
	    ActivateDlg(m_nCurrIndex - 1);
}

void CMainDlg::OnClickedEnd()
{
	CDialog* pDlg = GetInnerDlg();
	ASSERT(pDlg != NULL);

	if (!IsUserDlg())
	{
		// Hitting finish from one of our own dialogs
		if (!(((CSeqDlg*) pDlg)->CanDestroy()))
			return;
		((CSeqDlg*) pDlg)->WriteValues();
	}
	else
	{
		// It's a user's extension dialog
		if (!(((CAppWizStepDlg*) pDlg)->OnDismiss()))
			return;
	}
	ActivateConfirmDlg();
}

void CMainDlg::OnCancel()
{
    // Destroy current dialog before destroying main dialog
    CDialog* pCurrDlg = GetInnerDlg();
    ASSERT(pCurrDlg);
    VERIFY(pCurrDlg->DestroyWindow());
    CancelAppWiz();
    VERIFY(DestroyWindow());
}


// Hack: The last step has Next disabled, so Finish should be the
//  default. this becomes a problem when the user tabs around, since
//  the tabbing code is hard-coded in this file so the inner-dialog's
//  controls get put into the tab order.  In order that the proper
//  button stays the default even when the user tabs into the inner
//  dialog, I'm only letting IDOK be the default button.  Thus, for
//  Finish to "look" like it's the default, I interchange IDOK and
//  IDC_END, plus their captions.  Thus, the "Finish button" is really
//  IDOK on the classes dialog.
void CMainDlg::InterchangeEndAndOK()
{
	CString strOK, strEnd;
	CRect rectOK, rectEnd;
	CButton* pOK = (CButton*) GetDlgItem(IDOK);
	CButton* pEnd = (CButton*) GetDlgItem(IDC_END);
	pOK->GetWindowRect(&rectOK);
	pEnd->GetWindowRect(&rectEnd);
	ScreenToClient(&rectOK);
	ScreenToClient(&rectEnd);
	pOK->GetWindowText(strOK);
	pEnd->GetWindowText(strEnd);
	
	pOK->MoveWindow(&rectEnd);
	pEnd->MoveWindow(&rectOK);
	pOK->SetWindowText(strEnd);
	pEnd->SetWindowText(strOK);

	m_nRightmostBtn = (m_nRightmostBtn == IDOK) ? IDC_END : IDOK;
}

// This decides whether to call InterchangeEndAndOK, and how to
//  enable/disable the buttons.
void CMainDlg::AdjustEndAndOK(BOOL bIsLastDlg)
{
    if (bIsLastDlg)
    {
		// We're at the last dialog, so make "Finish" the default, and
		//  disable "Next".
		if (m_nRightmostBtn != IDOK)
			InterchangeEndAndOK();
		GetDlgItem(IDC_END)->EnableWindow(FALSE);
    }
    else
    {
        // We're not at the last dialog, so make sure "Next" is
		//  enabled and is the default
		if (m_nRightmostBtn != IDC_END)
			InterchangeEndAndOK();
		// only enable "Finish" if the in-progress app is not an ActiveX doc server or it has a file extension
        GetDlgItem(IDC_END)->EnableWindow(!(projOptions.m_bActiveDocServer && projOptions.m_names.strDocFileExt.IsEmpty()));
    }
}


void CMainDlg::SetCurrentTrack()
{
	switch(projOptions.m_nProjType)
	{
	case PROJTYPE_DLG:
		m_nCurrTrack = (int*) nDlgTrack;
		m_nCurrLastIndex = DLG_LAST_INDEX;
		break;
	case PROJTYPE_MDI:
	case PROJTYPE_SDI:
		m_nCurrTrack = (int*) nAppTrack;
		m_nCurrLastIndex = APP_LAST_INDEX;
		break;
	case PROJTYPE_DLL:
		m_nCurrTrack = (int*) nDLLTrack;
		m_nCurrLastIndex = DLL_LAST_INDEX;
		break;
	}
}

// Size of standard bitmap
#define BMP_LEFT 16
#define BMP_TOP 16
#define BMP_WIDTH 195
#define BMP_HEIGHT 147

// Size of flag bitmap
#define BMP_LEFT1 0
#define BMP_TOP1 0
#define BMP_WIDTH1 163
#define BMP_HEIGHT1 225

// Size of 3D checkbox bitmap
#define BMP2_LEFT (RIGHT_YELLOW-155)
#define BMP2_TOP (2*BMP_TOP + BMP_HEIGHT + 1)
#define BMP2_WIDTH 181
#define BMP2_HEIGHT 71

// Other values used for drawing around the bitmap
#define RIGHT_YELLOW (163+BMP_LEFT)
#define BOTTOM_YELLOW_DLGU 197
#define RIGHT_GRAY_DLGU 125


void GetBitmapRect(int nSize, RECT* rect)
{
	switch(nSize)
	{
	case BMPSIZE_STANDARD:   // The usual, regular bitmaps on most of the steps
		rect->left = BMP_LEFT;
		rect->top = BMP_TOP;
		rect->right = rect->left + BMP_WIDTH;
		rect->bottom = rect->top + BMP_HEIGHT;
		break;
	case BMPSIZE_FLAG:  	 // Checkered flag bitmap on last step
		rect->left = BMP_LEFT1;
		rect->top = BMP_TOP1;
		rect->right = rect->left + BMP_WIDTH1;
		rect->bottom = rect->top + BMP_HEIGHT1;
		break;
	case BMPSIZE_3D:		// Small dialog-cut picture for 3D checkbox
		rect->left = BMP2_LEFT;
		rect->top = BMP2_TOP;
		rect->right = rect->left + BMP2_WIDTH;
		rect->bottom = rect->top + BMP2_HEIGHT;
		break;
	case BMPSIZE_SUM:		// Union of sizes used for invalidation
		rect->left = rect->top = 0;
		rect->right = BMP_WIDTH + BMP_LEFT;
		rect->bottom = BMP_HEIGHT1 + BMP_TOP;
		break;
	}
}

void CMainDlg::CalculateDimensions()
{
	if (m_nChildDlgLeft != -1)
		return;

	const int nPadding = 5;
	CRect rect;
	CSeqDlg* pDlg = m_pDialogClasses[2];    // Just pick any CSeqDlg to check its size
	pDlg->Create(pDlg->m_nTemplate, this);
	pDlg->GetClientRect(&rect);
	VERIFY(pDlg->DestroyWindow());
	int nWidthLeftRegion = max(BMP2_WIDTH+BMP2_LEFT, max(BMP_WIDTH1+BMP_LEFT1, BMP_WIDTH+BMP_LEFT));
	int nWidthChildDlg = rect.right;
	GetWindowRect(&rect);
	int nMinWidthMainDlg = rect.Width();
	int nWidthMainDlg = nWidthLeftRegion + nWidthChildDlg + nPadding;
	if (nWidthMainDlg < nMinWidthMainDlg)
		nWidthMainDlg = nMinWidthMainDlg;
	VERIFY(SetWindowPos(NULL, 0, 0, nWidthMainDlg, rect.Height(), SWP_NOZORDER | SWP_NOMOVE));
	m_nChildDlgLeft = nWidthLeftRegion + nPadding;
}

COLORREF GetDlgBackground()
{
	static BOOL bFirstTime = TRUE;
	static DWORD dwDlgBk;

	if (bFirstTime)
	{
		dwDlgBk = GetSysColor(COLOR_BTNFACE);
		bFirstTime = FALSE;
	}

	return dwDlgBk;
}

void CMainDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (IsUserDlg())
		return;                // Don't draw pictures for extensions.

	CSeqDlg* pDlg = (CSeqDlg*) GetInnerDlg();
	if (pDlg == NULL)
	{
		ASSERT(IsUserAWX());
		return;
	}

	if (!pDlg->ShouldShowPicture())
		return;

	BOOL bTwoBitmaps = pDlg->ShowTwoBitmaps();
	
	DrawBitmap(&dc, pDlg, 1);
	if (bTwoBitmaps)
		DrawBitmap(&dc, pDlg, 2);
	
	CBitmap pattern;
	pattern.LoadBitmap(IDB_YELLOW_PATTERN);
	CBrush brush(&pattern);
	RECT rectBmp;
	GetBitmapRect(pDlg->GetBitmapSizeIndex(), &rectBmp);
	BOOL bBmpIsFlag = (pDlg->GetBitmapSizeIndex() == BMPSIZE_FLAG);
	CPen penBogus(PS_SOLID, 0, COLORREF(0));
	CPen* pOldPen = dc.SelectObject(&penBogus);  // Remember current pen.

	// Draw yellow ABOVE bitmap
	RECT rect = {0, 0, 0, 0};
	rect.bottom = rectBmp.top;
	rect.right = RIGHT_YELLOW+1;
	dc.DPtoLP(&rect);
	dc.FillRect(&rect, &brush);

	// Draw yellow to LEFT of bitmap
	rect.left = 0;
	rect.top = rectBmp.top;
	rect.right = rectBmp.left;
	rect.bottom = rectBmp.bottom+1;
	dc.DPtoLP(&rect);
	dc.FillRect(&rect, &brush);

	// Draw yellow to RIGHT of bitmap
	//  (Only necessary for bitmaps not crossing right line.)
	rect.left = rectBmp.right;
	rect.top = rectBmp.top;
	rect.right = RIGHT_YELLOW+1;
	rect.bottom = rectBmp.bottom+1;
	if (rect.left < rect.right)
	{
		dc.DPtoLP(&rect);
		dc.FillRect(&rect, &brush);

		// Fill in gray to right of yellow.
		GetClientRect(&rect);
		rect.left = RIGHT_YELLOW+3;
		rect.top = rectBmp.top;
		rect.bottom = rectBmp.bottom+1;
		dc.DPtoLP(&rect);
		CBrush brushGray(GetDlgBackground());
		dc.FillRect(&rect, &brushGray);
	}

	// If we're showing a second bitmap, take care of its background
	if (bTwoBitmaps)
	{
		DrawLowerBackground(&dc, &brush, pOldPen);
		return;
	}

	// Draw yellow BELOW bitmap
	rect.left = 0;
	rect.top = 0;
	rect.right = 1;
	rect.bottom = BOTTOM_YELLOW_DLGU;
	MapDialogRect(&rect);
	rect.right = RIGHT_YELLOW+1;
	rect.top = rectBmp.bottom;
	dc.DPtoLP(&rect);
	dc.FillRect(&rect, &brush);

	// Draw right border lines
	CPen penGray(PS_SOLID, 0, 0x00808080);
	dc.SelectObject(&penGray);
	dc.MoveTo(rect.right, 0);
	POINT point;
	point.x = rect.right;
	point.y = rectBmp.top;
	dc.DPtoLP(&point);
	if (!bBmpIsFlag)
		dc.LineTo(point.x, point.y);
	point.x = rect.right;
	point.y = rectBmp.bottom;
	dc.DPtoLP(&point);
	if (!bBmpIsFlag)
		dc.MoveTo(point.x, point.y);
	dc.LineTo(point.x, rect.bottom);

	CPen penWhite(PS_SOLID, 0, 0x00ffffff);
	dc.SelectObject(&penWhite);
	dc.MoveTo(rect.right+1, 0);
	point.x = rect.right+1;
	point.y = rectBmp.top;
	dc.DPtoLP(&point);
	if (!bBmpIsFlag)
		dc.LineTo(point.x, point.y);
	point.x = rect.right+1;
	point.y = rectBmp.bottom;
	dc.DPtoLP(&point);
	if (!bBmpIsFlag)
		dc.MoveTo(point.x, point.y);
	dc.LineTo(point.x, rect.bottom);

	// Fill gray rectangle to right of nonexistent
	//  second bitmap
	GetBitmapRect(BMPSIZE_3D, &rectBmp);
	CBrush brushGray(GetDlgBackground());
	rect.left = RIGHT_YELLOW+3;
	rect.top = rectBmp.top;
	rect.right = rectBmp.right;
	rect.bottom = rectBmp.bottom;
	dc.DPtoLP(&rect);
	dc.FillRect(&rect, &brushGray);

	// Restore original pen
	dc.SelectObject(pOldPen);
}

void CMainDlg::DrawLowerBackground(CDC* pdc, CBrush* pBrush, CPen* pOldPen)
{
	RECT rectBmp, rectBmp2;
	GetBitmapRect(BMPSIZE_STANDARD, &rectBmp);
	GetBitmapRect(BMPSIZE_3D, &rectBmp2);
	
	// Draw yellow ABOVE bitmap
	RECT rect = {0, 0, 0, 0};
	rect.top = rectBmp.bottom;
	rect.bottom = rectBmp2.top;
	rect.right = RIGHT_YELLOW+1;
	pdc->DPtoLP(&rect);
	pdc->FillRect(&rect, pBrush);

	// Draw yellow to LEFT of bitmap
	rect.left = 0;
	rect.top = rectBmp2.top;
	rect.right = rectBmp2.left;
	rect.bottom = rectBmp2.bottom+1;
	pdc->DPtoLP(&rect);
	pdc->FillRect(&rect, pBrush);

	// Draw yellow BELOW bitmap
	rect.left = 0;
	rect.top = 0;
	rect.right = 1;
	rect.bottom = BOTTOM_YELLOW_DLGU;
	MapDialogRect(&rect);
	rect.right = RIGHT_YELLOW+1;
	rect.top = rectBmp2.bottom;
	pdc->DPtoLP(&rect);
	pdc->FillRect(&rect, pBrush);

	// Draw right border lines
	CPen penGray(PS_SOLID, 0, 0x00808080);
	pdc->SelectObject(&penGray);
	pdc->MoveTo(rect.right, 0);
	POINT point;
	point.x = rect.right;
	point.y = rectBmp.top;
	pdc->DPtoLP(&point);
	pdc->LineTo(point.x, point.y);
	point.x = rect.right;
	point.y = rectBmp.bottom;
	pdc->DPtoLP(&point);
	pdc->MoveTo(point.x, point.y);
	point.x = rect.right;
	point.y = rectBmp2.top;
	pdc->DPtoLP(&point);
	pdc->LineTo(point.x, point.y);
	point.x = rect.right;
	point.y = rectBmp2.bottom;
	pdc->DPtoLP(&point);
	pdc->MoveTo(point.x, point.y);
	pdc->LineTo(point.x, rect.bottom);

	CPen penWhite(PS_SOLID, 0, 0x00ffffff);
	pdc->SelectObject(&penWhite);
	pdc->MoveTo(rect.right+1, 0);
	point.x = rect.right+1;
	point.y = rectBmp.top;
	pdc->DPtoLP(&point);
	pdc->LineTo(point.x, point.y);
	point.x = rect.right+1;
	point.y = rectBmp.bottom;
	pdc->DPtoLP(&point);
	pdc->MoveTo(point.x, point.y);
	point.x = rect.right+1;
	point.y = rectBmp2.top;
	pdc->DPtoLP(&point);
	pdc->LineTo(point.x, point.y);
	point.x = rect.right+1;
	point.y = rectBmp2.bottom;
	pdc->DPtoLP(&point);
	pdc->MoveTo(point.x, point.y);
	pdc->LineTo(point.x, rect.bottom);

	// Restore original pen
	pdc->SelectObject(pOldPen);
}


void CMainDlg::DrawBitmap(CDC* pdc, CSeqDlg* pDlg, int nBmp)
{
	UINT nBmpID;
	if (nBmp == 1)
		nBmpID = pDlg->ChooseBitmap();
	else
		nBmpID = pDlg->ChooseBitmap2();
	if (nBmpID == 0)
		return;

	RECT rect;
	GetBitmapRect(nBmp == 1 ? pDlg->GetBitmapSizeIndex() : BMPSIZE_3D, &rect);
	int nWidth = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;

	CDC dcMem;
	if (!dcMem.CreateCompatibleDC(pdc))
		return;

	if (nBmp == 1)
	{
		CBitmap picture;
		picture.LoadBitmap(nBmpID);
		BITMAP bitmap;
		picture.GetObject(sizeof (BITMAP), &bitmap);

		ASSERT(nWidth == bitmap.bmWidth);
		ASSERT(nHeight == bitmap.bmHeight);

		CBitmap* pOldBitmap = dcMem.SelectObject(&picture);
		pdc->BitBlt(rect.left, rect.top, nWidth, nHeight, &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pOldBitmap);
	}
	else
	{
		CImageWell imageWell;
		CSize size(nWidth, nHeight);
		VERIFY(imageWell.Load(nBmpID, size));
		VERIFY(imageWell.Open());
		CBitmap picture;
		picture.CreateCompatibleBitmap(pdc, nWidth, nHeight);
		CBitmap* pOldBitmap = dcMem.SelectObject(&picture);
		dcMem.BitBlt(0, 0, nWidth, nHeight, pdc, rect.left, rect.top, SRCCOPY);

		CPoint destPoint(0, 0);
		VERIFY(imageWell.DrawImage(&dcMem, destPoint, 0));
		pdc->BitBlt(rect.left, rect.top, nWidth, nHeight, &dcMem, 0, 0, SRCCOPY);
		imageWell.Close();

		dcMem.SelectObject(pOldBitmap);
	}
	
}

void CMainDlg::RedrawBitmap(int nBmpSize)
{
	RECT rect;
	GetBitmapRect(nBmpSize, &rect);
	RedrawWindow(&rect, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

CDialog* CMainDlg::GetInnerDlg()
{
	if (IsUserAWX())
	{
		if (m_pCurrUserSeqDlg != NULL)
			return (CDialog*) m_pCurrUserSeqDlg;
		else if (m_pCurrUserStepDlg != NULL)
			return (CDialog*) m_pCurrUserStepDlg;
		else
			return NULL;
	}
	else
		return (CDialog*) m_pDialogClasses[m_nCurrTrack[m_nCurrIndex]];
}

BOOL CMainDlg::IsUserDlg()
{
	return (m_pCurrUserSeqDlg == NULL && m_pCurrUserStepDlg != NULL);
}

void CMainDlg::SetNumberOfSteps(int nSteps)
{
	m_nMaxUserSteps = nSteps;
	UpdateStepDisplay();
}


void CMainDlg::SetFocusOnStep(CDialog* pDlg)
{
	HWND hwnd = pDlg->GetSafeHwnd();
	ASSERT(hwnd != NULL);
	HWND hCtl = ::GetNextDlgTabItem(hwnd, NULL, FALSE);
	if (hCtl != NULL && ::IsWindow(hCtl) && ::IsWindowEnabled(hCtl))
	{
		CWnd* pCtl = CWnd::FromHandle(hCtl);
		ASSERT (pCtl != NULL);
		pDlg->GotoDlgCtrl(pCtl);
	}
	else
		GetDlgItem(IDOK)->SetFocus();
}

void CMainDlg::ActivateUserDlg(BOOL bDirIsNext)
{
	// Gross special case... if this is called immediately after appwiz is invoked,
	//  we just want to put up cpromptdlg.
	if (!bDirIsNext && m_pCurrUserStepDlg == NULL)
	{
		m_pCurrUserSeqDlg = NULL;
		theDLL.m_bQuit = TRUE;
		pTermStatus->nTermType = APPWIZTERMTYPE_RERUNPROMPTDLG;
		return;
	}

	if (m_pCurrUserStepDlg != NULL && !DismissCurrentDlg(FALSE))
		return;

	CDialog* pPrevDlg = GetInnerDlg();	// Remember old dialog
	
	BOOL bWasSeqDlg = (m_pCurrUserSeqDlg != NULL);

	if (bDirIsNext)
	{
		// User hit Next
		CAppWizStepDlg* pNewUserDlg = m_pCurrUserStepDlg;
		pNewUserDlg = GetAWX()->Next(pNewUserDlg);

		// If we just called Next(NULL), they might have updated their symbols,
		//  so load them in again
		if (m_pCurrUserStepDlg == NULL)
			GetAllSymbols();

		if (pNewUserDlg == NULL)
		{
			// NULL implies we're at the last step, so Finish == IDOK.
			m_pCurrUserSeqDlg = NULL;
			ActivateConfirmDlg();
			return;		// return without setting m_pCurrUserStepDlg to NULL
		}
		m_pCurrUserStepDlg = pNewUserDlg;
		m_nCurrUserStep++;
	}
	else
	{
		// User hit Back
		m_pCurrUserStepDlg = GetAWX()->Back(m_pCurrUserStepDlg);
		m_nCurrUserStep--;
		if (m_pCurrUserStepDlg == NULL)
		{
			// NULL implies to show CPromptDlg
			if (bWasSeqDlg)
				VERIFY(pPrevDlg->DestroyWindow());
			else
			{
				VERIFY(pPrevDlg->ShowWindow(SW_HIDE));
				m_UserDlgsToDestroy[pPrevDlg] = (void*) 1;
			}
			m_pCurrUserSeqDlg = NULL;
			theDLL.m_bQuit = TRUE;
			pTermStatus->nTermType = APPWIZTERMTYPE_RERUNPROMPTDLG;
			return;
		}
	}

	// We're still here.  So that means that the current dialog was
	//  successfully validated, and we will bring up another
	//  user dialog.  First, see if the "user" dialog is actually one
	//  of our own.
	if (!m_DlgMap.Lookup(m_pCurrUserStepDlg, m_pCurrUserSeqDlg))
		// It's not an appwiz dialog
		m_pCurrUserSeqDlg = NULL;
	else
		// It IS an appwiz dialog, so make sure the symbol-setting flag is set
		projOptions.m_bMayBeAppWizTrack = TRUE;

	// may need to add or remove the help button
	ArrangeButtons((IsDefinitelyKnownWizard() || m_pCurrUserSeqDlg != NULL) ? 0 : 1);
	
	// Since we're not activating confirm or prompt dlgs, we now know
	//  we can destroy the inner dialog.
	if (pPrevDlg != NULL)
	{
		if (bWasSeqDlg)
			VERIFY(pPrevDlg->DestroyWindow());
		else
		{
			VERIFY(pPrevDlg->ShowWindow(SW_HIDE));
			m_UserDlgsToDestroy[pPrevDlg] = (void*) 1;
		}
	}

    MakeOKDefault(this);

	// Here, we're given a valid new extension dlg (ours or theirs) to put up.
	CDialog* pNewDlg;
	if (m_pCurrUserSeqDlg != NULL)
	{
	    VERIFY(m_pCurrUserSeqDlg->Create(m_pCurrUserSeqDlg->m_nTemplate, this));
		pNewDlg = (CDialog*) m_pCurrUserSeqDlg;
	}
	else
	{
		if (m_pCurrUserStepDlg->GetSafeHwnd())
		{
			m_pCurrUserStepDlg->ShowWindow(SW_SHOW);
			m_pCurrUserStepDlg->SetFocus();
		}
		else
			VERIFY(m_pCurrUserStepDlg->Create(m_pCurrUserStepDlg->m_nIDTemplate, this));
		pNewDlg = (CDialog*) m_pCurrUserStepDlg;
	}

	// Placement of dialog depends on whether it's one of ours (in which case,
	//  save space on the left for picture.
	int nLeft = 0;						// Default == place dlg flush left.
	if (m_pCurrUserSeqDlg != NULL)
		nLeft = m_nChildDlgLeft;		// Save space on left for picture

    pNewDlg->SetWindowPos(NULL, nLeft, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);

    pNewDlg->ShowWindow(SW_SHOWNOACTIVATE);
    pNewDlg->EnableWindow(TRUE);
	EnableWindow(TRUE);
	SetFocusOnStep(pNewDlg);
#ifndef VS_PACKAGE
	imeEnableIME(0, TRUE);
#endif	// VS_PACKAGE
	UpdateStepDisplay();

	if (bWasSeqDlg != (m_pCurrUserSeqDlg != NULL))
	{
		// If we're going from a seq to non-seq dlg, or going from a
		//  non-seq to a seq dlg, we need to invalidate the picture area
		RECT rect = {0, 0, 0, 0};
		rect.bottom = BOTTOM_YELLOW_DLGU;
		MapDialogRect(&rect);
		int nBottom = rect.bottom;

		GetClientRect(&rect);
		rect.bottom = nBottom;
		
		CPaintDC dc(this);
		dc.DPtoLP(&rect);

		RedrawWindow(&rect, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}
	else if (m_pCurrUserSeqDlg != NULL)
	{
		// In any other case where we're about to put up a seqdlg, invalidate
		//  the bitmap areas.
		RedrawBitmap(BMPSIZE_SUM);
		RedrawBitmap(BMPSIZE_3D);
	}
}	

// Record current data, and reset track if necessary
BOOL CMainDlg::DismissCurrentDlg(BOOL bShouldDestroy /* = TRUE */)
{
	// If current dialog was CPromptDlg, it was already dismissed.
	if (!IsUserAWX() && m_nCurrIndex == 0)
		return TRUE;

    CDialog* pCurrDlg = NULL;
    pCurrDlg = GetInnerDlg();

	ASSERT(pCurrDlg != NULL);      // This isn't called if CPromptDlg or CConfirmDlg is active

	// Are we allowed to destroy the current dialog?
	if (!IsUserDlg())
	{
		// It's one of our own dialogs
		if (!(((CSeqDlg*) pCurrDlg)->CanDestroy()))
       		return FALSE;

		// We're still here, so we have permission to dismiss.
		if (bShouldDestroy)
			VERIFY(pCurrDlg->DestroyWindow());
	}
	else
	{
		// It's a user's extensions dialog
		if (!(((CAppWizStepDlg*) pCurrDlg)->OnDismiss()))
			return FALSE;

		// We're still here, so we have permission to dismiss.
		if (bShouldDestroy)
		{
			VERIFY(pCurrDlg->ShowWindow(SW_HIDE));
			m_UserDlgsToDestroy[pCurrDlg] = (void*) 1;
		}
	}

    // Decide if we need to change tracks.
    if (!IsUserAWX() && m_nCurrIndex == 1 && projOptions.m_nProjType != PROJTYPE_DLL)
    {
        // We're leaving the project type dialog, so ensure proper track
		SetCurrentTrack();
    }
	return TRUE;
}

void CMainDlg::DestroyUserDlgs()
{
	if (!GetSafeHwnd())
		return;

	POSITION pos = m_UserDlgsToDestroy.GetStartPosition();
	while (pos != NULL)
	{
		CAppWizStepDlg* pDlg;
		void* pTemp;
		m_UserDlgsToDestroy.GetNextAssoc(pos, pDlg, pTemp);
		VERIFY(pDlg->DestroyWindow());
	}
	m_UserDlgsToDestroy.RemoveAll();
}


void CMainDlg::ActivateDlg(int nDlg)
{
	if (m_nCurrIndex == nDlg)
        return;
    ASSERT(nDlg != -1);  // (Used to mean "End" was pushed).

	// Record current data, and reset track if necessary
	if (nDlg != -2 && !DismissCurrentDlg())
		return;

	// Make sure the proper button is default
    MakeOKDefault(this);

	// Special case for first (CPromptDlg) dialog
    if (nDlg == 0 || nDlg == -2)
    {
		theDLL.m_bQuit = TRUE;
		pTermStatus->nTermType = APPWIZTERMTYPE_RERUNPROMPTDLG;
        return;
    }

	// Display new dialog
    CSeqDlg* pDlg = m_pDialogClasses[m_nCurrTrack[nDlg]];
	int nHelpID = GetAltHelpID(m_nCurrTrack[nDlg], m_nCurrTrack);
	if (nHelpID == 0)
		nHelpID = pDlg->m_nTemplate;
	else
		nHelpID -= HID_BASE_RESOURCE;
	SetHelpID(nHelpID);
    m_nCurrIndex = nDlg;
    VERIFY(pDlg->Create(pDlg->m_nTemplate, this));
	int nLeft = m_nChildDlgLeft;
	if (!pDlg->ShouldShowPicture())
		nLeft = 0;
    pDlg->SetWindowPos(NULL, nLeft, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
    pDlg->ShowWindow(SW_SHOWNOACTIVATE);
    pDlg->EnableWindow(TRUE);
	EnableWindow(TRUE);
	SetFocusOnStep(pDlg);
#ifndef VS_PACKAGE
	imeEnableIME(0, TRUE);
#endif	// VS_PACKAGE

	UpdateStepDisplay();

	RedrawBitmap(BMPSIZE_SUM);
	RedrawBitmap(BMPSIZE_3D);
}

void CMainDlg::UpdateStepDisplay()
{
	// Determine current and maximum steps
	int nCurrentStep, nMaxStep;
	CString strWizardTitle;
	if (IsUserAWX())
	{
		// Currently an extension is in charge, so get the
		//  step numbers & title from it.
		nCurrentStep = m_nCurrUserStep;
		nMaxStep = m_nMaxUserSteps;
		strWizardTitle = projOptions.m_UserExtension.m_strExtensionTitle;
	}
	else
	{
		// Currently we're in charge, so we know the step numbers & title
		nCurrentStep = m_nCurrIndex;
		nMaxStep =
			(m_nCurrIndex <= 1 && projOptions.m_nProjType != PROJTYPE_DLL)
			? -1 : m_nCurrLastIndex-1;
		strWizardTitle.LoadString(IDS_APPWIZ_TITLE);
	}

	// Now construct the title message

	// Bail if we're not created yet (e.g., we just loaded a custom appwiz which
	//  set the number of steps, all before CMainDlg has been created).
	if (!GetSafeHwnd())
		return;

	CString strBuffer;
	LPTSTR szBuffer = strBuffer.GetBuffer(strWizardTitle.GetLength() + 32);
    if (nMaxStep == -1)
    {
        // We don't know the total number of steps
		CString strFullString;
		strFullString.LoadString(IDS_TITLE_NOMAXSTEP);
        sprintf(szBuffer, (LPCTSTR) strFullString, (LPCTSTR) strWizardTitle, nCurrentStep);
	    SetWindowText(szBuffer);
    }
    else if (nMaxStep > 0)
    {
        // We DO know the total number of steps
		CString strFullString;
		strFullString.LoadString(IDS_TITLE_MAXSTEP);
        sprintf(szBuffer, (LPCTSTR) strFullString, (LPCTSTR) strWizardTitle, nCurrentStep, nMaxStep);
	    SetWindowText(szBuffer);
    }
	strBuffer.ReleaseBuffer();

    // Make "Next" or "Finish" the default
	AdjustEndAndOK(nCurrentStep == nMaxStep && nMaxStep != -1);
}


// nAddHelpButton == 1 (add help button) or 0 (don't add help button)
void CMainDlg::ArrangeButtons(int nAddHelpButton)
{
	// We need to dynamically add/remove the help button, which means
	//  shifting all the other buttons to the left (or right) as
	//  appropriate, because the buttons must be right-aligned and
	//  the help button (when visible) must be on the right.
	// Thus, we store the dialog-unit coordinates here, and move the
	//  buttons to the appropriate coordinates, which we convert from
	//  dialog units to client coordinates.

	const int BACK[2]	= {  83,  23 };
	const int NEXT[2]   = { 133,  73 };
	const int FINISH[2] = { 193, 133 };
	const int CANCEL[2] = { 253, 193 };
	static int y = -1;

	ASSERT (nAddHelpButton == 1 || nAddHelpButton == 0);

	if (m_nHasHelpButton == nAddHelpButton)
		return;		// same state we were in when we came in here, so nothing to do...

	m_nHasHelpButton = nAddHelpButton;

	if (y == -1)
	{
		RECT rect;
		GetDlgItem(IDC_BACK)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		y = rect.top;
	}

	RECT rect = {0,0,0,0};
	
	rect.left = BACK[nAddHelpButton];
	MapDialogRect(&rect);
	GetDlgItem(IDC_BACK)->SetWindowPos(NULL, rect.left, y, 0, 0, SWP_NOREDRAW | SWP_NOSIZE | SWP_NOZORDER);

	rect.left = NEXT[nAddHelpButton];
	MapDialogRect(&rect);
	GetDlgItem(IDOK)->SetWindowPos(NULL, rect.left, y, 0, 0, SWP_NOREDRAW | SWP_NOSIZE | SWP_NOZORDER);

	rect.left = FINISH[nAddHelpButton];
	MapDialogRect(&rect);
	GetDlgItem(IDC_END)->SetWindowPos(NULL, rect.left, y, 0, 0, SWP_NOREDRAW | SWP_NOSIZE | SWP_NOZORDER);

	rect.left = CANCEL[nAddHelpButton];
	MapDialogRect(&rect);
	GetDlgItem(IDCANCEL)->SetWindowPos(NULL, rect.left, y, 0, 0, SWP_NOREDRAW | SWP_NOSIZE | SWP_NOZORDER);

	GetDlgItem(ID_HELP)->ShowWindow(nAddHelpButton ? SW_SHOW : SW_HIDE);

	// Finally, add/remove the "?" button in the titlebar
	LONG nExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	nExStyle = nAddHelpButton ?
		nExStyle & ~WS_EX_CONTEXTHELP :
		nExStyle | WS_EX_CONTEXTHELP;
	SetWindowLong(m_hWnd, GWL_EXSTYLE, nExStyle);

	Invalidate();
    DrawMenuBar();
}

// Returns TRUE if everything went according to plan and steps are being brought up
//  as normal (in which case appwiz will relinquish control when CTheDLL::m_bQuit
//  is set to TRUE).  Returns FALSE if we need to immediately bring back CPromptDlg.
//  See CTheDLL::RunAppWizSteps() to show how this is called.
BOOL CMainDlg::StartAppWizSteps()
{
    m_nCurrIndex = 0;

	// Bug 2032 in the DevStudio '96 database.  This symbol is left lingering around
	// if the previous appwizard had changed the view's type to CRichEditView.  I'm
	// really not confident that this is the right place to clear this, but it seems
	// as good as any I've found.  -mgrier 5/15/96
	RemoveSymbol("CRichEditView");

	// If this fires, be sure InitAppWiz() was called first!
	ASSERT(pLaunchParent != NULL);
	if (pLaunchParent)
		pLaunchParent->EnableWindow(FALSE);

	VERIFY(GetCurrentDirectory(_MAX_PATH, m_strCWDProjGen.GetBuffer(_MAX_PATH)) != 0);
	m_strCWDProjGen.ReleaseBuffer();

	InitializeUserVars();

	// A change in the CPromptDlg's project type may cause a track change.
	SetCurrentTrack();
	
	if (IsUserAWX())
	{
		// Launch the custom AppWizard steps
		GetAllSymbols();
		if (m_nMaxUserSteps == 0)
		{
			// We have a zero-step extension
			if (GetAWX()->Next(NULL) != NULL)	// Let 'em set symbols
			{
				// They didn't return NULL when they were supposed to!
				ReportErrorExt(IDP_BAD_0STEP_AWX);
				return FALSE;
			}

			if (ActivateConfirmDlg())
				return TRUE;	// If they created project, outta here.
			return FALSE;		// Otherwise they canceled CConfirmDlg; bring back promptdlg
		}
		CalculateDimensions();
		ArrangeButtons((IsDefinitelyKnownWizard() || m_pCurrUserSeqDlg != NULL) ? 0 : 1);
        ShowWindow(SW_SHOW);
		ActivateUserDlg(TRUE);
	}
	else
	{
		// Launch the standard AppWizard steps
		CalculateDimensions();
		ArrangeButtons(0);
        ShowWindow(SW_SHOW);
        ActivateDlg(1);
	}

    return TRUE;
}

BOOL CMainDlg::ActivateConfirmDlg()
{
    CConfirmDlg dlg(this, (!IsUserAWX() || m_nMaxUserSteps != 0));

	// VERIFY(SetCurrentDirectory(m_strCWDProjGen));
	m_pConfirmDlg = &dlg;
    int nRetValue = dlg.DoModal();
	m_pConfirmDlg = NULL;
    switch(nRetValue)
    {
    case IDOK:
        if (pLaunchParent != NULL)
        {
            pLaunchParent->EnableWindow(TRUE);
			ASSERT(pLaunchParent->IsWindowEnabled());
        }
        VERIFY(DestroyWindow());
		return TRUE;
    case IDCANCEL:
		return FALSE;
    }
	return TRUE;
}

void MyPostQuitMessage()
{
	::PostMessage(NULL, WM_MYQUIT, 0, 0);
}

void CMainDlg::OnDestroy()
{
	if (pLaunchParent)
		pLaunchParent->EnableWindow(TRUE);
    ::WinHelp(m_hWnd, NULL, HELP_QUIT, 0L);
    C3dDialog::OnDestroy();
}

void CMainDlg::PostNcDestroy()
{
	theDLL.m_bQuit = TRUE;
}

CMainDlg::~CMainDlg()
{
	DeleteDialogs();
}

void CMainDlg::UserDeletedStandardStep(CAppWizStepDlg* pDlg)
{
	for (int i=0; i < NUM_DLG_CLASSES; i++)
		if (m_pExtensionKeys[i] == pDlg)
			m_pExtensionKeys[i] = NULL;
}

// Delete dialogs from the heap
void CMainDlg::DeleteDialogs()
{
	// This is defined in pagedlg.cpp.  When we set this, we have
	//  permission to call CAppWizStepDlg's destructor
	void AppWizCallingStructor(BOOL bAppWizCallingStructor);
	AppWizCallingStructor(TRUE);

    for (int i=0; i < NUM_DLG_CLASSES; i++)
    {
        if (m_pDialogClasses[i] != NULL)
            delete m_pDialogClasses[i];
		if (m_pExtensionKeys[i] != NULL)
			delete m_pExtensionKeys[i];
    }
	
	AppWizCallingStructor(FALSE);
}

BOOL CMainDlg::FinalProcessing()
{
	// First, get rid of .pjx, .mdp & .opt files if they exist.
	CPath PjxFile, MdpFile, OptFile;
	PjxFile.CreateFromDirAndFilename(projOptions.m_ProjDir, projOptions.m_strProj + BUILDER_EXT);
	MdpFile.CreateFromDirAndFilename(projOptions.m_ProjDir, projOptions.m_strProj + ".mdp");
	OptFile.CreateFromDirAndFilename(projOptions.m_ProjDir, projOptions.m_strProj + ".opt");

	if (!DestroyFile(PjxFile.GetFullPath()))
		return FALSE;
	if (!DestroyFile(MdpFile.GetFullPath()))
		return FALSE;
	if (!DestroyFile(OptFile.GetFullPath()))
		return FALSE;
	
    // create a status dialog
	ASSERT(m_pConfirmDlg);
    CStatusDlg statDlg(m_pConfirmDlg);

    if (Generate(statDlg.GetDlgItem(IDC_OUTPROJ), statDlg.GetDlgItem(IDC_OUTFILE)))
    {
        statDlg.DestroyWindow();    // avoids uglyness on completion!
        return TRUE;
    }
    else
    {
        statDlg.DestroyWindow();    // avoids warning in destructor
        return FALSE;
    }

    // else don't cancel the dialog
}

// Return index into szPlatforms (MAC, POWERMAC, WIN) that the given long
//  platform name corresponds to.  If it's not in the array, return -1.
int PlatformLongToEnum(const CString& strPlatform)
{
	// Determine which platform this really is
	if (strPlatform == szPlatforms[MAC])
		return MAC;
	else if (strPlatform == szPlatforms[POWERMAC])
		return POWERMAC;
	else if (strPlatform == szPlatforms[WIN])
		return WIN;
	else if (strPlatform == szPlatforms[JAVA])
		return JAVA;
    else if (strPlatform == szPlatforms[XBOX])
        return XBOX;
	else
		return -1;
}

void AddDefaultTargetsWithPseudoDebug(const CString& strPlatform,
	const CString& strProjType, HBLDTARGET& hDebugTarg, HBLDTARGET& hReleaseTarg, HBUILDER hBld)
{
	TCHAR szFileName[_MAX_FNAME];	// our project name
	CString strTargName;			// our target name
	CString strCfgName;				// our config. name
	
	// First, get the project's name
	_tsplitpath(pTermStatus->szFullName, NULL, NULL, szFileName, NULL);
	
	// Add retail target first.

	strTargName = szFileName;
	VERIFY(strCfgName.LoadString(IDS_RELEASE));
	strTargName += strCfgName;

#ifdef VS_PACKAGE
	IBuildWizard *pBldWizIFace = &g_CBldWiz;
#else
	LPBUILDWIZARD pBldWizIFace;
	theApp.FindInterface(IID_IBuildWizard, (LPVOID FAR *)&pBldWizIFace);
#endif

	pBldWizIFace->AddTarget(
		&hReleaseTarg,
		strTargName,				// Default target name
		strPlatform,				// Platform name
		strProjType,				// Target type
		FALSE,						// Using non-UI ("official") names
		FALSE,						// Retail settings
		TrgDefault,					// don't clone or mirror
		NO_TARGET,					// don't clone or mirror
		SettingsDefault,			// default settings
		NO_TARGET,					// no target to copy settings from
		FALSE, TRUE, TRUE,			// other random stuff
		hBld);						// builder that contains target

	ASSERT (hReleaseTarg != NO_TARGET);

	// The targets have AWX as their extension.
	CString strAWX = "awx";
	//pBldWizIFace->SetTargetDefExt(hDebugTarg, strAWX, hBld);
	pBldWizIFace->SetTargetDefExt(hReleaseTarg, strAWX, hBld);

	// Add custom build step to copy .AWX & .PDB files to msvc30\Template
	CString strDescription;
	strDescription.LoadString(IDS_COPY_AWX);
	pBldWizIFace->AssignCustomBuildStep(
		"if not exist \"$(MSDEVDIR)\\Template\\nul\" md \"$(MSDEVDIR)\\Template\"\t\
copy \"$(TargetPath)\" \"$(MSDEVDIR)\\Template\"\t\
if exist \"$(OutDir)\\$(TargetName).pdb\" copy \"$(OutDir)\\$(TargetName).pdb\" \"$(MSDEVDIR)\\Template\"",
		"$(MSDEVDIR)\\Template\\$(TargetName).awx",
		strDescription,
		hReleaseTarg,
		NO_FILE,
		hBld);

	// Add the pseudo-debug target as mirroring & copying hReleaseTarget.
	
	strTargName = szFileName;
	VERIFY(strCfgName.LoadString(IDS_PSEUDO_DEBUG));
	strTargName += strCfgName;

	//  And create that target.  Start its options off as retail options.
	pBldWizIFace->AddTarget(
		&hDebugTarg,
		strTargName,
		strPlatform,				// Platform name
		strProjType,				// Target type
		FALSE,						// Using non-UI ("official") names
		FALSE,						// Retail settings
		TrgMirror,					// We're mirroring...
		hReleaseTarg,				// ...the retail target we just added
		SettingsClone,				// We want to copy settings from...
		hReleaseTarg,				// ...release target
		FALSE, TRUE, TRUE,			// other random stuff
		hBld);						// builder that contains target

	ASSERT (hDebugTarg != NO_TARGET);

	// Now the fun begins.  Let's play with the options of hDebugTarget, so that
	//  it makes a retail build w/ symbols & no optimization

	// First, the compiler.  Add /Od /Zi /D "_PSEUDO_DEBUG"
	pBldWizIFace->SetToolSettings(
		hDebugTarg,					// We're partying on the "debug" target
		"/Od /Zi /D ""_PSEUDO_DEBUG""",
		"cl.exe",					// Compiler options
		TRUE, FALSE, ACTIVE_BUILDER);

	// Next, the linker.  Add /debug & /incremental:yes
	pBldWizIFace->SetToolSettings(
		hDebugTarg,					// We're partying on the "debug" target
		"/debug /incremental:yes",					
		"link.exe",					// Linker options
		TRUE, FALSE, ACTIVE_BUILDER);

	// The resource compiler.  Add /D "_PSEUDO_DEBUG"
	pBldWizIFace->SetToolSettings(
		hDebugTarg,					// We're partying on the "debug" target
		"/D ""_PSEUDO_DEBUG""",
		"rc.exe",					// Resource compiler options
		TRUE, FALSE, ACTIVE_BUILDER);

	// Set the output & intermediate directories.
	CString strWinDebug = "Debug";
	pBldWizIFace->SetTargetOutDir(
		hDebugTarg,					// We're partying on the "debug" target
		strWinDebug,				// Set the output directory...to Debug
		ACTIVE_BUILDER);
	pBldWizIFace->SetTargetIntDir(
		hDebugTarg,					// We're partying on the "debug" target
		strWinDebug,				// Set the intermediate directory...to Debug
		ACTIVE_BUILDER);

/*	pBldWizIFace->SetActiveTarget(hDebugTarg, hBld);
	pBldWizIFace->SetDefaultTarget(hDebugTarg, hBld);*/
	pBldWizIFace->Release();

}

void AddUnicodeTargets(HBLDTARGET hDebug, HBLDTARGET hRelease, LPCTSTR szProjType, HBUILDER hBld)
{
	// The win32 ansi targets have already been created (they were passed here), so
	//  clone their settings and mirror their filesets with new UNICODE targets

	// First generate their names
	// Need the project name
	TCHAR szFileName[_MAX_FNAME];
	_tsplitpath(pTermStatus->szFullName, NULL, NULL, szFileName, NULL);
	CString strNameDebug, strNameRelease, strCfgName;
	strNameDebug = szFileName;
	VERIFY(strCfgName.LoadString(IDS_DEBUG_UNICODE));
	strNameDebug += strCfgName;
	strNameRelease = szFileName;
	VERIFY(strCfgName.LoadString(IDS_RELEASE_UNICODE));
	strNameRelease += strCfgName;

	// Second, clone the given targets

#ifdef VS_PACKAGE
	IBuildWizard *pBldWizIFace = &g_CBldWiz;
#else
	LPBUILDWIZARD pBldWizIFace;
	theApp.FindInterface(IID_IBuildWizard, (LPVOID FAR *)&pBldWizIFace);
#endif

	HBLDTARGET hDebugUnicode;
	pBldWizIFace->AddTarget(
		&hDebugUnicode,
		strNameDebug,		// name of this target
		szPlatforms[WIN],	// 'official' name of this target's platform
		szProjType,			// 'official' name of this target type
		FALSE,				// using 'official' names
		TRUE,				// debug settings?  Ignored
		TrgMirror,			// mirror ansi target
		hDebug,				// target to mirror
		SettingsClone,		// copy settings
		hDebug,				// target to copy settings from
		FALSE, TRUE, TRUE,	// Other random stuff
		hBld);				// The builder
	ASSERT (hDebugUnicode != NULL);
		
	HBLDTARGET hReleaseUnicode;
	pBldWizIFace->AddTarget(
		&hReleaseUnicode,
		strNameRelease,		// name of this target
		szPlatforms[WIN],	// 'official' name of this target's platform
		szProjType,			// 'official' name of this target type
		FALSE,				// using 'official' names
		TRUE,				// debug settings?  Ignored
		TrgMirror,			// mirror ansi target
		hRelease,			// target to mirror
		SettingsClone,		// copy settings
		hRelease,			// target to copy settings from
		FALSE, TRUE, TRUE,	// Other random stuff
		hBld);				// The builder
	ASSERT (hReleaseUnicode != NULL);

	// Third, change the output directories (append a "u")
	VERIFY(pBldWizIFace->GetTargetOutDir(hDebug, strNameDebug, hBld) == S_OK);
	VERIFY(pBldWizIFace->GetTargetOutDir(hRelease, strNameRelease, hBld) == S_OK);
	strNameDebug += 'U';
	strNameRelease += 'U';
	VERIFY(pBldWizIFace->SetTargetOutDir(hDebugUnicode, strNameDebug, hBld) == S_OK);
	VERIFY(pBldWizIFace->SetTargetOutDir(hReleaseUnicode, strNameRelease, hBld) == S_OK);
	VERIFY(pBldWizIFace->SetTargetIntDir(hDebugUnicode, strNameDebug, hBld) == S_OK);
	VERIFY(pBldWizIFace->SetTargetIntDir(hReleaseUnicode, strNameRelease, hBld) == S_OK);

	// Penultimately, have the unicode targets define UNICODE
	pBldWizIFace->SetToolSettings(hDebugUnicode, "/D \"_MBCS\"",
			"cl.exe", FALSE, FALSE, hBld);
	pBldWizIFace->SetToolSettings(hReleaseUnicode, "/D \"_MBCS\"",
			"cl.exe", FALSE, FALSE, hBld);
	pBldWizIFace->SetToolSettings(hDebugUnicode, "/D \"_UNICODE\"",
			"cl.exe", TRUE, FALSE, hBld);
	pBldWizIFace->SetToolSettings(hReleaseUnicode, "/D \"_UNICODE\"",
			"cl.exe", TRUE, FALSE, hBld);

/*	pBldWizIFace->SetActiveTarget(hDebug, hBld);
	pBldWizIFace->SetDefaultTarget(hDebug, hBld);*/
	
	// TODO: Verify we don't actually need to do this
	// Finally, add /entry:wWinMainCRTStartup
/*	pBldWizIFace->SetToolSettings(hDebugUnicode, "/entry:wWinMainCRTStartup",
			"link.exe", TRUE, FALSE, hBld);
	pBldWizIFace->SetToolSettings(hReleaseUnicode, "/entry:wWinMainCRTStartup",
			"link.exe", TRUE, FALSE, hBld);*/
	pBldWizIFace->Release();
}

void AddCopyLicenseStep(HBLDTARGET hTarg, HBLDFILE hLicense, HBUILDER hBld)
{
	// Adds a custom build step to copy the .lic file in a controlwiz project
	CString strDescription;
	strDescription.LoadString(IDS_COPY_LIC);

#ifdef VS_PACKAGE
	IBuildWizard *pBldWizIFace = &g_CBldWiz;
#else
	LPBUILDWIZARD pBldWizIFace;
	theApp.FindInterface(IID_IBuildWizard, (LPVOID FAR *)&pBldWizIFace);
#endif

	pBldWizIFace->AssignCustomBuildStep(
		"copy \"$(ProjDir)\\$(TargetName).lic\" \"$(OutDir)\"",
		"$(OutDir)\\$(TargetName).lic",
		strDescription,
		hTarg,
		hLicense,
		hBld);
	pBldWizIFace->Release();
}

void AddRegSvrStep(HBLDTARGET hTarg, HBUILDER hBld)
{
	// Adds a custom build step to register the control
	CString strDescription;
	strDescription.LoadString(IDS_REGISTER_CONTROL);

#ifdef VS_PACKAGE
	IBuildWizard *pBldWizIFace = &g_CBldWiz;
#else
	LPBUILDWIZARD pBldWizIFace;
	theApp.FindInterface(IID_IBuildWizard, (LPVOID FAR *)&pBldWizIFace);
#endif

	pBldWizIFace->AssignCustomBuildStep(
		"regsvr32 /s /c \"$(TargetPath)\"\t\
echo regsvr32 exec. time > \"$(OutDir)\\regsvr32.trg\"",
		"$(OutDir)\\regsvr32.trg",
		strDescription,
		hTarg,
		NO_FILE,
		hBld);
	pBldWizIFace->Release();
}

void AddMakehelpStep(HBLDTARGET hTarg, HBLDFILE hHpjFile, HBLDFILE hResource, HBLDFILE hCnt, HBUILDER hBld, BOOL bMac = FALSE)
{

#ifdef VS_PACKAGE
	IBuildWizard *pBldWizIFace = &g_CBldWiz;
#else
	LPBUILDWIZARD pBldWizIFace;
	theApp.FindInterface(IID_IBuildWizard, (LPVOID FAR *)&pBldWizIFace);
#endif


	CString strDescription;
	CString strOutput;
	CString strCommand;

	// put this step on the hpj
	strDescription.LoadString(IDS_MAKEHELP);

	strOutput = "\"$(OutDir)\\$(InputName).hlp\"";

	strCommand = "start /wait hcw /C /E /M \"hlp\\$(InputName).hpj\"\t" \
		"if errorlevel 1 goto :Error\t" \
		"if not exist \"hlp\\$(InputName).hlp\" goto :Error\t" \
		"copy \"hlp\\$(InputName).hlp\" $(OutDir)\t";

	if (DoesSymbolExist(_T("PROJTYPE_CUSTOMAW")))
	{
		strCommand += "copy \"$(OutDir)\\$(TargetName).hlp\" \"$(MSDEVDIR)\\Template\"\t";
		strOutput += "\t\"$(MSDEVDIR)\\Template\"";
	}

	CString strTmp, strTmp2;
	if (!strTmp2.LoadString(IDS_ERR_BLD_HELP))
		strTmp2 = _T("Problem encountered creating help file");
	strTmp.Format("goto :done\t"\
		":Error\t"\
		"echo hlp\\$(InputName).hpj(1) : error: %s\t"\
		"type \"hlp\\$(InputName).log\"\t"\
		":done", strTmp);
	strCommand += strTmp;

	if( hHpjFile != NO_FILE ){
		pBldWizIFace->AssignCustomBuildStep(
			strCommand,
			strOutput,
			strDescription,
			hTarg,
			hHpjFile,
			hBld);
	
		// Add dependencies (not needed for dialog-based apps or custom appwizards)
		if( !DoesSymbolExist( _T("PROJTYPE_DLG") ) )
		{
			CString strDep = "hlp\\AfxCore.rtf";
			if (DoesSymbolExist(_T("PRINT")))
				strDep += "\nhlp\\AfxPrint.rtf";
			if( hResource != NO_FILE )
				strDep += "\nhlp\\$(TargetName).hm";
	
			pBldWizIFace->SetUserDefinedDependencies(
				strDep,
				hTarg,
				hHpjFile,
				hBld);
		}
	}

	strDescription.LoadString(IDS_MAKEHM);

	strOutput = "hlp\\$(TargetName).hm";

	strCommand = "echo. >\"hlp\\$(TargetName).hm\"\t"\
		"echo // Commands (ID_* and IDM_*) >>\"hlp\\$(TargetName).hm\"\t"\
		"makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>\"hlp\\$(TargetName).hm\"\t"\
		"echo. >>\"hlp\\$(TargetName).hm\"\t"\
		"echo // Prompts (IDP_*) >>\"hlp\\$(TargetName).hm\"\t"\
		"makehm IDP_,HIDP_,0x30000 resource.h >>\"hlp\\$(TargetName).hm\"\t"\
		"echo. >>\"hlp\\$(TargetName).hm\"\t"\
		"echo // Resources (IDR_*) >>\"hlp\\$(TargetName).hm\"\t"\
		"makehm IDR_,HIDR_,0x20000 resource.h >>\"hlp\\$(TargetName).hm\"\t"\
		"echo. >>\"hlp\\$(TargetName).hm\"\t"\
		"echo // Dialogs (IDD_*) >>\"hlp\\$(TargetName).hm\"\t"\
		"makehm IDD_,HIDD_,0x20000 resource.h >>\"hlp\\$(TargetName).hm\"\t"\
		"echo. >>\"hlp\\$(TargetName).hm\"\t"\
		"echo // Frame Controls (IDW_*) >>\"hlp\\$(TargetName).hm\"\t"\
		"makehm IDW_,HIDW_,0x50000 resource.h >>\"hlp\\$(TargetName).hm\"";

	if( hResource != NO_FILE ){
		pBldWizIFace->AssignCustomBuildStep(
			strCommand,
			strOutput,
			strDescription,
			hTarg,
			hResource,
			hBld);
	}
	
	strDescription.LoadString(IDS_MAKECNT);

	strOutput = "$(OutDir)\\$(InputName).cnt";

	strCommand = "copy \"hlp\\$(InputName).cnt\" $(OutDir)";


	if( hCnt != NO_FILE ){
		pBldWizIFace->AssignCustomBuildStep(
			strCommand,
			strOutput,
			strDescription,
			hTarg,
			hCnt,
			hBld);
	}

	pBldWizIFace->Release();
}

enum { AFXDLL, USRDLL, AFXEXT };

void AddDllOptions(HBLDTARGET hTarg, HBUILDER hBld, int nSymbol)
{
	ASSERT (0 <= nSymbol && nSymbol <= 2);
	static LPCTSTR szSymbols[3] =
	{
		"/D \"_USRDLL\" /D \"_AFXDLL\"",
		"/D \"_USRDLL\"",
		"/D \"_AFXEXT\"",
	};

#ifdef VS_PACKAGE
	IBuildWizard *pBldWizIFace = &g_CBldWiz;
#else
	LPBUILDWIZARD pBldWizIFace;
	theApp.FindInterface(IID_IBuildWizard, (LPVOID FAR *)&pBldWizIFace);
#endif

	// Reset...
	pBldWizIFace->SetToolSettings(hTarg, szSymbols[0], "cl.exe", FALSE, FALSE, hBld);
	pBldWizIFace->SetToolSettings(hTarg, szSymbols[2], "cl.exe", FALSE, FALSE, hBld);

	// Set the option corresponding to nSymbol
	pBldWizIFace->SetToolSettings(hTarg, szSymbols[nSymbol], "cl.exe", TRUE, FALSE, hBld);
	pBldWizIFace->Release();
}

int CalcDllSymbol()
{
	int nReturn = -1;
	if (DoesSymbolExist("MFCDLL"))
	{
		if (DoesSymbolExist("EXTDLL"))
			// MFC Extension DLL
			nReturn = AFXEXT;
		else
			// Regular DLL, dynamically linked
			nReturn = AFXDLL;
	}
	else
	{
		// Regular DLL, statically linked
		ASSERT (!DoesSymbolExist("EXTDLL"));
		nReturn = USRDLL;
	}
	ASSERT (nReturn != -1);
	return nReturn;
}

void AddFilesFromList(HFILESET hFileSet, CString & strProjDir, const CStringList * pstrlFiles, HBUILDER hBld, HFOLDER hFolder = NO_FOLDER, int fSettings = 0)
{
	if (pstrlFiles==NULL || pstrlFiles->IsEmpty())
		return;  // nothing to do

	HBLDFILE hFile;
	CString strFile;
	CPath path;

#ifdef VS_PACKAGE
	IBuildWizard *pBldWizIFace = &g_CBldWiz;
#else
	LPBUILDWIZARD pBldWizIFace;
	theApp.FindInterface(IID_IBuildWizard, (LPVOID FAR *)&pBldWizIFace);
#endif

	POSITION pos = pstrlFiles->GetHeadPosition();
	while (pos != NULL)
	{
		strFile = pstrlFiles->GetNext(pos);

		// Make full pathname of file
		strFile = strProjDir + strFile;

		// Make CPath from the full pathname of file
		VERIFY(path.Create(strFile));

		// Call add dep file API
		VERIFY(pBldWizIFace->AddFile(&hFile, hFileSet, &path, fSettings, hBld, hFolder) == S_OK);
	}
	pBldWizIFace->Release();
}

void CreateProject(HBUILDER hBld)
{
	// This should only be called if the project was created by AppWizard
	ASSERT(pTermStatus->nTermType == APPWIZTERMTYPE_APPWIZPROJ);

	// Determine the target-type
	
	int nTarget = 0;				// Default is .exe
	BOOL bUsePseudoDebug = FALSE;	// Default is make default targets
    BOOL fSkipIt = FALSE;
	CString strProjType;

	if (IsUserAWX())
	{
		// The project generated by an extension gets its target type
		//  from the appropriate symbol.
		if (DoesSymbolExist("PROJTYPE_DLL") || DoesSymbolExist("PROJTYPE_OCX"))
			nTarget = 1;			// DLL
#if 0
		else if (DoesSymbolExist("PROJTYPE_JAVA"))
        {
			// REVIEW UNDONE HACK ALERT...
			// jimsc 2/21/96
			// We know this is Java, just set the string.
			fSkipIt = TRUE;
            strProjType = "Java Project";
        }
#endif	// 0
		else if (DoesSymbolExist("PROJTYPE_LIB"))
        {
			fSkipIt = TRUE;
            strProjType = "Static Library";
        }
		else if (DoesSymbolExist("PROJTYPE_GEN"))
        {
			fSkipIt = TRUE;
            strProjType = "Generic Project";
        }
		else if (DoesSymbolExist("PROJTYPE_MAK"))
        {
			fSkipIt = TRUE;
            strProjType = "External Target";
        }
		else if (DoesSymbolExist("PROJTYPE_CON"))
        {
			fSkipIt = TRUE;
            strProjType = "Console Application";
        }
		else if (DoesSymbolExist("PROJTYPE_QWA"))
        {
			fSkipIt = TRUE;
            strProjType = "QuickWin Application";
        }
		else if (DoesSymbolExist("PROJTYPE_GRA"))
        {
			fSkipIt = TRUE;
            strProjType = "Standard Graphics Application";
        }
		else if (DoesSymbolExist("PROJTYPE_CUSTOMAW"))
		{
			// This option is generally used only by ExtensionWizard to create an extension project
			nTarget = 1;			// DLL
			bUsePseudoDebug = TRUE;	// Make Pseudo-debug target
			SetSymbol("EXTDLL", "1");
		}
		else
			nTarget = 0;			// EXE
	}
	else
	{
		// Here, AppWizard is creating the project, so our answer
		//  was recorded in pTermStatus
		nTarget = pTermStatus->nProjType;
	}

    if (!fSkipIt)
        strProjType = strlProjTypes.GetAt(strlProjTypes.FindIndex(nTarget));

	// Determine platforms

	// Add default targets based on target-type & platforms
	HBLDTARGET hDebugTarg = NO_TARGET, hReleaseTarg = NO_TARGET;
	HFILESET hFileSet;

	enum {DEBUG_TGT, RELEASE_TGT};
	HBLDTARGET htgts[PLATFORM_MAX][2];
	for (int i=0; i < PLATFORM_MAX; i++)
		for (int j=0; j < 2; j++)
			htgts[i][j] = NO_TARGET;


	// Determine project name and directory
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szProjName[_MAX_FNAME];
	TCHAR szExt[_MAX_FNAME];
	_tsplitpath(pTermStatus->szFullName, szDrive, szDir, szProjName, szExt );
	CString strProjName = szProjName;
	strProjName += szExt;
	CString strPath = szDrive;
	strPath += szDir;

    if (pTermStatus != &g_TermStatus)
    {
        // This is not the first project, but some additional
        // project being added because of $$NEWPROJ directive
        // So the path name should contain the subdirectory
        strPath += strProjName;
        strPath += "\\";
    }

	// Go through each of the installed platforms.  For each, check
	//  whether the user chose it; if so, add the appropriate targets

#ifdef VS_PACKAGE
	IBuildWizard *pBldWizIFace = &g_CBldWiz;
#else
	LPBUILDWIZARD pBldWizIFace;
	theApp.FindInterface(IID_IBuildWizard, (LPVOID FAR *)&pBldWizIFace);
#endif

	POSITION pos = g_PlatformsSelected.GetStartPosition();
	while (pos != NULL)
	{
		CString strPlatform, tmp;
		g_PlatformsSelected.GetNextAssoc(pos, strPlatform, tmp);
		if (!DoesSymbolExist(strPlatform))
			continue;

		int nPlatform;
		if ((nPlatform = PlatformLongToEnum(strPlatform)) != -1)
		{
			// If this platform is a known one (i.e., not contrived
			//  by an extension), then add the dbg & rel targets.  If
			//  previous ones were added have these targets mirror them.
			if (bUsePseudoDebug)
			{
				// Projects that build custom AppWizards use pseudo-debug
				//  rather than debug...
				AddDefaultTargetsWithPseudoDebug
					(strPlatform, strProjType, hDebugTarg, hReleaseTarg, hBld);
			}
			else
				pBldWizIFace->AddDefaultTargets
					(strPlatform, strProjType, hDebugTarg, hReleaseTarg,
					FALSE, TRUE, TRUE, hDebugTarg, hBld, strProjName);

			// did it work? (this only fails if platform or projtype is bad)
			if (hDebugTarg == NO_TARGET)
				break;

			if (DoesSymbolExist("PROJTYPE_OCX"))
			{
				// OLE Controls have OCX as their extension
				CString strOCX = "ocx";
				pBldWizIFace->SetTargetDefExt(hDebugTarg, strOCX, hBld);
				pBldWizIFace->SetTargetDefExt(hReleaseTarg, strOCX, hBld);
			}


			if (!IsUserAWX())
			{
				pBldWizIFace->SetTargetIsAppWiz(hDebugTarg, 1, hBld);
				pBldWizIFace->SetTargetIsAppWiz(hReleaseTarg, 1, hBld);
			}

			// Set the appropriate target handles
			htgts[nPlatform][DEBUG_TGT] = hDebugTarg;
			htgts[nPlatform][RELEASE_TGT] = hReleaseTarg;
		}
	}

	if (hDebugTarg == NO_TARGET)
	{
		// If no "real" targets were selected (possible with an extension), return.
		ASSERT(hReleaseTarg == NO_TARGET);
		pBldWizIFace->Release();
		return;
	}

	ASSERT(hDebugTarg != NO_TARGET);
	ASSERT(hReleaseTarg != NO_TARGET);

	// We now take the list of files and add them to the project: a three
	//  step process

	// STEP 1/3: Create CPtrList of CPath pointers from the filelist
	
	CStringList* pstrlFiles = &(pTermStatus->strlDepFiles); // For easy list reference
	int nNumFiles = pstrlFiles->GetCount();
	CPath* pPaths = NULL;
	if (nNumFiles > 0)
		pPaths = new CPath[nNumFiles];

	CPtrList ptrlPaths;
	pos = pstrlFiles->GetHeadPosition();
	for (i=0; pos != NULL; i++)
	{
		ASSERT(i < nNumFiles);

		CString strFile = pstrlFiles->GetNext(pos);

		// Make full pathname of file
		strFile = strPath + strFile;

		// Make CPath from the full pathname of file
		pPaths[i].Create(strFile);

		// Add entry in ptrlist for this path
		ptrlPaths.AddTail(&(pPaths[i]));
	}

	// STEP 2/3: Call the bldiface API with the new ptrlist

#ifndef VS_PACKAGE
	theApp.NotifyPackages(PN_BEGIN_MULTIOP);
#endif

	pBldWizIFace->GetFileSet(hBld, hDebugTarg, &hFileSet);	// use debug which a target of fileset

	// Add default groups
	HFOLDER hSources = NO_FOLDER;
	HFOLDER hHeaders = NO_FOLDER;
	HFOLDER hResources = NO_FOLDER;
	HFOLDER hHelpFiles = NO_FOLDER;
	HFOLDER hTemplates = NO_FOLDER;

	// if this is a java project, then we set file type correctly and don't create the "Header files" folder.
	CString strT;
#if 0
	if (DoesSymbolExist("PROJTYPE_JAVA"))
	{
		strT.LoadString(IDS_SOURCE_FILES);
		pBldWizIFace->AddFolder(&hSources, hFileSet, strT, hBld, NO_FOLDER, "java;html");
		ASSERT(hSources != NO_FOLDER);
	}
	else if (!DoesSymbolExist("PROJTYPE_GEN") )
#else
	if (!DoesSymbolExist("PROJTYPE_GEN") )
#endif
	{
		CString strFilter;
		if (DoesSymbolExist("SOURCE_FILTER"))
		{
			strT.LoadString(IDS_SOURCE_FILES);
			GetSymbol("SOURCE_FILTER", strFilter );
			pBldWizIFace->AddFolder(&hSources, hFileSet, strT, hBld, NO_FOLDER, strFilter);
		}
		if (DoesSymbolExist("INCLUDE_FILTER"))
		{
			strT.LoadString(IDS_HEADER_FILES);
			GetSymbol("INCLUDE_FILTER", strFilter );
			pBldWizIFace->AddFolder(&hHeaders, hFileSet, strT, hBld, NO_FOLDER, strFilter);
		}
		ASSERT(hSources != NO_FOLDER);
	}

//	if (!pTermStatus->strlResFiles.IsEmpty())
	if (!DoesSymbolExist("PROJTYPE_LIB") && !DoesSymbolExist("PROJTYPE_GEN") )
	{
		strT.LoadString(IDS_RESOURCE_FILES);
		pBldWizIFace->AddFolder(&hResources, hFileSet, strT, hBld, NO_FOLDER, "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"); // UNDONE: more files to filter
	}

	// Optionally add folder for Help files
	if (!pTermStatus->strlHelpFiles.IsEmpty())
	{
		strT.LoadString(IDS_HELP_FILES);
		pBldWizIFace->AddFolder(&hHelpFiles, hFileSet, strT, hBld, NO_FOLDER, "cnt;rtf");
	}

	// Optionally add folder for Template files
	if (!pTermStatus->strlTemplFiles.IsEmpty())
	{
		strT.LoadString(IDS_TEMPLATE_FILES);
		pBldWizIFace->AddFolder(&hTemplates, hFileSet, strT, hBld, NO_FOLDER, "<templates>");
	}

	CPtrList ptrlhFiles;
	pBldWizIFace->AddMultiFiles(hFileSet, &ptrlPaths, &ptrlhFiles, FALSE, hBld, NO_FOLDER);

	// Optionally add any other resource files as well
	if (!pTermStatus->strlResFiles.IsEmpty())
		AddFilesFromList(hFileSet, strPath, &(pTermStatus->strlResFiles), hBld, hResources);

	// Optionally add any help files as well
	if (!pTermStatus->strlHelpFiles.IsEmpty())
		AddFilesFromList(hFileSet, strPath, &(pTermStatus->strlHelpFiles), hBld, hHelpFiles);

	// Optionally add any exclude from build files
	if (!pTermStatus->strlNonBldFiles.IsEmpty())
		AddFilesFromList(hFileSet, strPath, &(pTermStatus->strlNonBldFiles), hBld, NO_FOLDER /* use defaults */, ADB_EXCLUDE_FROM_BUILD /* must exclude from build */);

	// Optionally add (and exclude) any template files as well
	if (!pTermStatus->strlTemplFiles.IsEmpty())
		AddFilesFromList(hFileSet, strPath, &(pTermStatus->strlTemplFiles), hBld, hTemplates, (ADB_EXCLUDE_FROM_BUILD | ADB_EXCLUDE_FROM_SCAN) /* must exclude from build & scan */);

#ifndef VS_PACKAGE
	theApp.NotifyPackages(PN_END_MULTIOP);
#endif

	// STEP 3/3: Iterate through fileset to find special files

	pos = ptrlPaths.GetHeadPosition();
	POSITION pos2 = ptrlhFiles.GetHeadPosition();
	HBLDFILE hStdafx = NO_FILE, hLicense = NO_FILE, hResource = NO_FILE,
		hHpjFile = NO_FILE, hCntFile = NO_FILE, hMacHpjFile = NO_FILE, hRFile = NO_FILE, hOdl = NO_FILE;
	CString strOdlFname;

	while (pos != NULL)
	{
		ASSERT(pos2 != NULL);

		// Get HBLDFILE corresponding to this file that was given to us by AddMultiFiles
		HBLDFILE hFile = (HBLDFILE) ptrlhFiles.GetNext(pos2);

		// Determine what type of file this is and set the appropriate HBLDFILE
		CString strFile = ((CPath*)ptrlPaths.GetNext(pos))->GetFileName();
		if (!strFile.CompareNoCase("stdafx.cpp"))
			hStdafx = hFile;	// Remember whether to use precompiled headers for later...
		else if (!strFile.CompareNoCase("resource.h"))
			hResource = hFile;	// Remember whether to use precompiled headers for later...
		else
		{
			LPCTSTR szExt = _tcschr(strFile, '.');
			if (szExt == NULL)
			{}		// no extension?  doesn't belong in any of the buckets below...
			else if (!_tcsicmp(szExt, ".r") && DoesSymbolExist("TARGET_MAC"))
				hRFile = hFile;
			else if (!_tcsicmp(szExt, ".lic")
				&& DoesSymbolExist("PROJTYPE_OCX") && DoesSymbolExist("LICENSED"))
			{
				hLicense = hFile;
			}
			else if( !_tcsicmp(szExt, ".hpj") )
			{
				// LATER: With multiple languages, we'll need to distinguish
				//  between all the languages' .hpj files.
				// Is this the mac or the win32 .hpj file?
				CString strMacHpjFilename;
				GetSymbol("mac_hpj", strMacHpjFilename);
				if (!strFile.CompareNoCase(strMacHpjFilename + ".hpj"))
				{
					hMacHpjFile = hFile;
				}
				else
					hHpjFile = hFile;
			}
			else if( !_tcsicmp(szExt, ".cnt") )
			{
				hCntFile = hFile;
			}
			else if (!_tcsicmp(szExt, ".odl"))
			{
				ASSERT(hOdl == NO_FILE);	// don't expect more than one .odl
				hOdl = hFile;

				_TCHAR szFname[_MAX_FNAME];
				_splitpath(LPCTSTR(strFile), NULL, NULL, szFname, NULL);
				strOdlFname = szFname;
			}
		}
	}

	// We're done with adding the files to the project.  Let's clean up our mess.
	if (nNumFiles > 0)
		delete [nNumFiles] pPaths;

	// Exclude .r file and mac .hpj file from Win32 builds
	if (hRFile != NO_FILE && htgts[WIN][DEBUG_TGT] != NO_TARGET)
	{
		ASSERT(htgts[WIN][RELEASE_TGT] != NO_TARGET);
		for (int j=0; j < 2; j++)
		{
			pBldWizIFace->SetBuildExclude(htgts[WIN][j], hRFile, TRUE);
			if (hMacHpjFile != NO_FILE)
				pBldWizIFace->SetBuildExclude(htgts[WIN][j], hMacHpjFile, TRUE);
		}
	}

	// Exclude win32 .hpj file from Mac builds
	if (hHpjFile != NO_FILE)
	{
		for (int i=0; i < PLATFORM_MAX; i++)
		{
			if (i != MAC && i != POWERMAC)
				continue;

			if (htgts[i][DEBUG_TGT] != NO_TARGET)
			{
				ASSERT (htgts[i][RELEASE_TGT] != NO_TARGET);
				for (int j=0; j < 2; j++)
					pBldWizIFace->SetBuildExclude(htgts[i][j], hHpjFile, TRUE);
			}
		}
	}

	// Ok, all the targets have been added, and the files have been added
	//  too, so nothing left to do, right?  Wrong.  Now let's set the
	//  target & file options to use precompiled headers if there's a stdafx.cpp
	if (hStdafx != NO_FILE)
	{
		// For each target, replace /YX with /Yu"stdafx.h", and
		//  add /Yc"stdafx.h" for stdafx.cpp.
		for (i=0; i < PLATFORM_MAX; i++)
		{
			if (htgts[i][DEBUG_TGT] == NO_TARGET)
				continue;
			ASSERT(htgts[i][RELEASE_TGT] != NO_TARGET);

			for (int j=0; j < 2; j++)
			{
				pBldWizIFace->SetToolSettings(htgts[i][j], "/YX",
					"cl.exe", FALSE, FALSE, hBld);
				pBldWizIFace->SetToolSettings(htgts[i][j], "/Yu\"stdafx.h\"",
					"cl.exe", TRUE, FALSE, hBld);
				pBldWizIFace->SetToolSettings(htgts[i][j], hStdafx, "/Yc\"stdafx.h\"",
					"cl.exe", TRUE, FALSE);
			}
		}
	}

	// Other options changes: change options to link MFC statically,
	//  use .def file, mac creator, invoke makehelp.bat.
	const int MFC_STATIC = 1;
	const int MFC_DYNAMIC = 2;
	int nMFCProp = DoesSymbolExist("MFCDLL") ? MFC_DYNAMIC : MFC_STATIC;
	int nDllSymbol = (DoesSymbolExist("PROJTYPE_DLL") || DoesSymbolExist("PROJTYPE_CUSTOMAW"))
		? CalcDllSymbol() : -1;
 	CString strCreator;
	GetSymbol("CREATOR", strCreator);
	for (i=0; i < PLATFORM_MAX; i++)
	{
		if (htgts[i][DEBUG_TGT] == NO_TARGET)
			continue;
		ASSERT(htgts[i][RELEASE_TGT] != NO_TARGET);

		for (int j=0; j < 2; j++)
		{
			// Link MFC
			if (i == MAC)	// Special case:  68K mac must link statically
				pBldWizIFace->SetTargetUseOfMFC(htgts[i][j], MFC_STATIC, hBld);
			else if (i == JAVA)
				pBldWizIFace->SetTargetUseOfMFC(htgts[i][j], 0, hBld);
			else
				pBldWizIFace->SetTargetUseOfMFC(htgts[i][j], nMFCProp, hBld);

			// Everybody should be _MBCS
			pBldWizIFace->SetToolSettings(htgts[i][j], "/D _MBCS",
					"cl.exe", TRUE, FALSE, hBld);

			// Special symbols for DLLs
			if (nDllSymbol != -1)
				AddDllOptions(htgts[i][j], hBld, nDllSymbol);

			/*if (!pTermStatus->strClwFile.IsEmpty())
			{
				// Set the name of the clw file if we have one

				// Make full pathname of file
				CString strClwFile = strPath + pTermStatus->strClwFile;
				pBldWizIFace->SetTargetProp(htgts[i][j], Prop_ClsWzdName, strClwFile, hBld);
			}*/

			// Mac creator
			if (i == MAC || i == POWERMAC)
			{
				pBldWizIFace->SetToolSettings(htgts[i][j], "/MAC:creator=\"" +  strCreator + "\"",
					"link.exe", TRUE, FALSE, hBld);
			}

			// Help support
			if (hHpjFile != NO_FILE && i == WIN)		// For win32
				AddMakehelpStep(htgts[i][j], hHpjFile,hResource,hCntFile, hBld);
			else if (hMacHpjFile != NO_FILE && (i == MAC || i == POWERMAC))
				AddMakehelpStep(htgts[i][j], hMacHpjFile, hResource, hCntFile, hBld, TRUE);

		}
	}

	if (hOdl != NO_FILE && DoesSymbolExist("HAS_ODL_HEADER"))
	{
		for (i=0; i < PLATFORM_MAX; i++)
		{
			if (htgts[i][DEBUG_TGT] == NO_TARGET)
				continue;
			ASSERT(htgts[i][RELEASE_TGT] != NO_TARGET);

			for (int j=0; j < 2; j++)
			{
				_TCHAR szSettings[_MAX_FNAME + 10];
				_stprintf(szSettings, "/h \"I%s.h\"", LPCTSTR(strOdlFname));

				pBldWizIFace->SetToolSettings(htgts[i][j], hOdl, szSettings,
					"mktyplib.exe", TRUE, FALSE);
			}
		}
	}

	if (DoesSymbolExist("PROJTYPE_OCX"))
	{
		ASSERT(htgts[WIN][DEBUG_TGT] != NULL);
		ASSERT(htgts[WIN][RELEASE_TGT] != NULL);
		if (hLicense != NO_FILE)
		{
			ASSERT(DoesSymbolExist("LICENSED"));
	
			// If the control is licensed, the project makefile must know
			//  to copy the license to the output dirs via a custom build tool
			AddCopyLicenseStep(htgts[WIN][DEBUG_TGT], hLicense, hBld);
			AddCopyLicenseStep(htgts[WIN][RELEASE_TGT], hLicense, hBld);

		}
		AddRegSvrStep(htgts[WIN][DEBUG_TGT], hBld);
		AddRegSvrStep(htgts[WIN][RELEASE_TGT], hBld);
		AddUnicodeTargets(htgts[WIN][DEBUG_TGT], htgts[WIN][RELEASE_TGT], strProjType, hBld);
	}
	else if (DoesSymbolExist("HAS_REGISTRATION"))
	{
		AddRegSvrStep(htgts[WIN][DEBUG_TGT], hBld);
		AddRegSvrStep(htgts[WIN][RELEASE_TGT], hBld);
	}

	if (DoesSymbolExist("_ATL_WIZ_UNICODE_") && !DoesSymbolExist("PROJTYPE_OCX"))
	{
		ASSERT(htgts[WIN][DEBUG_TGT] != NULL);
		ASSERT(htgts[WIN][RELEASE_TGT] != NULL);
		AddUnicodeTargets(htgts[WIN][DEBUG_TGT], htgts[WIN][RELEASE_TGT], strProjType, hBld);
	}

	if (htgts[WIN][DEBUG_TGT] != NO_TARGET)
	{
		// This will make the win32 ansi debug (for ctlwiz projects) or
		//  the pseudo debug (for customwz projects) or
		//  the Win32, non-mac, debug (for appwiz projects)
		//  target the active (currently selected in UI) & default (when
		//  you nmake the .mak file from command-line) target.
		pBldWizIFace->SetActiveTarget(htgts[WIN][DEBUG_TGT], hBld);
		pBldWizIFace->SetDefaultTarget(htgts[WIN][DEBUG_TGT], hBld);
	}

	{
		// These lines should always be at the BOTTOM of CreateProject.  This
		//  calls into the custom AppWizard and gives it the opportunity
		//  to customize the project.  This happens AFTER all of the default
		//  aspects of the project have been set up.

		IBuildProject* pProject = NULL;
		pBldWizIFace->GetAutoProject(hBld, &pProject);
		if (pProject != NULL)
		{
			// Second parameter is documented as reserved.  Internal consumers
			//  may use it as IBuildWizard or QI for IBuildSystem.  They
			//  must AddRef it if they want it to hang around.
			GetAWX()->CustomizeProject(pProject);
			pProject->Release();
		}
#ifndef VS_PACKAGE
		else
			ASSERT (FALSE);		// GetAutoProject failed?!
#endif
	}

	pBldWizIFace->Release();
	pTermStatus->strlDepFiles.RemoveAll();
	pTermStatus->strlResFiles.RemoveAll();
	pTermStatus->strlNonPrjFiles.RemoveAll();
	pTermStatus->strlNonBldFiles.RemoveAll();
	pTermStatus->strlHelpFiles.RemoveAll();
	pTermStatus->strlTemplFiles.RemoveAll();
}

// Performs cleanup done after EACH TIME appwiz is exited.  NOT when
//  the IDE unloads it (as it was before).
int CTheDLL::ExitInstance()
{
//	UnloadOleEntryPoints();
	langDlls.FreeLibs();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// Routine that glues it all together

void MakeProjectDirectory(const char* szSubDir)
{
    // first make sure '.' is a valid directory
    if (_chdir(".") != 0)
    {
        AfxMessageBox(IDP_CURDIR_GONE, MB_OK, 0);
        AfxThrowUserException();
    }

    if (szSubDir[0] != '\0')
        CreateDirectory(szSubDir, TRUE);    // relative path
}

CMainDlg::Generate(CWnd* pProjStatus, CWnd* pFileStatus)
{
    // save original directory
    char szOrigDir[_MAX_DIR];
    if (_getcwd(szOrigDir, _MAX_DIR) == NULL)
        szOrigDir[0] = '\0';

    BOOL bOK = TRUE;
    BeginWaitCursor();
    TRY
    {
        MakeProjectDirectory((LPCTSTR) projOptions.m_ProjDir);

		// Initialize file list
		pTermStatus->strlDepFiles.RemoveAll();
		pTermStatus->strlResFiles.RemoveAll();
		pTermStatus->strlHelpFiles.RemoveAll();
		pTermStatus->strlNonPrjFiles.RemoveAll();
		pTermStatus->strlNonBldFiles.RemoveAll();
		pTermStatus->strlTemplFiles.RemoveAll();

		// Generate project
		CProjGen projGen;
		projGen.Go(pProjStatus, pFileStatus);

		CString strFileToOpen = projOptions.m_strProj;
        if (pLaunchParent != NULL && !strFileToOpen.IsEmpty())
        {
            // ugly if we are visible while IDE is scanning dependencies!
            ASSERT(pProjStatus->GetParent() != NULL);
            pProjStatus->GetParent()->SetWindowPos(NULL, 0, 0, 0, 0,
                SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_HIDEWINDOW);
            SetWindowPos(NULL, 0, 0, 0, 0,
                SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_HIDEWINDOW);
			// Commented these out when made appwz a DLL
            pLaunchParent->EnableWindow(TRUE);
            ASSERT(pLaunchParent->IsWindowEnabled());
			
			pLaunchParent->SetActiveWindow();
//          pLaunchParent->UpdateWindow();

            // inherits the current directory from where we are now !
            FakeShellExecute(pLaunchParent->m_hWnd, "open", strFileToOpen);
        }
    }
    CATCH (CUserException, e)
    {
        bOK = FALSE;
    }
    AND_CATCH (CException, e)
    {
		TCHAR szError[1024];
		e->GetErrorMessage(szError, sizeof(szError) / sizeof(szError[0]));

		CString strBaseMessage;
		strBaseMessage.LoadString(IDS_GENERATE_FAILURE);
		CString strFormattedMessage;
		strFormattedMessage.Format(strBaseMessage, szError);

        AfxMessageBox(strFormattedMessage, MB_OK, 0);
        bOK = FALSE;
    }
    END_CATCH

/*    // Free handle to DLL.
	if (projOptions.m_hLocalizedRsc != NULL)
	{
   		FreeLibrary(projOptions.m_hLocalizedRsc);
		projOptions.m_hLocalizedRsc = NULL;
	}*/

	if (bOK)
		langDlls.FreeLibs();

    EndWaitCursor();
    // restore to original directory
    if (szOrigDir[0] != '\0')
        _chdir(szOrigDir);

    return bOK;
}

/////////////////////////////////////////////////////////////////////////////
// CConfirmDlg dialog


CConfirmDlg::CConfirmDlg(CWnd* pParent /*=NULL*/, BOOL bHasCancel /*=TRUE*/)
    : C3dDialog(CConfirmDlg::IDD, pParent)
{
	m_bHasCancel = bHasCancel;
	SetHelpID(CConfirmDlg::IDD);
}

void CConfirmDlg::OnHelp()
{
	SendMessage(WM_COMMANDHELP, 0, 0);
}

void CConfirmDlg::DoDataExchange(CDataExchange* pDX)
{
    C3dDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CConfirmDlg)
    DDX_Control(pDX, IDC_OUTPUT, m_stcOutput);
    DDX_Control(pDX, IDC_CONFIRM_DIR, m_stcDir);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConfirmDlg, C3dDialog)
    //{{AFX_MSG_MAP(CConfirmDlg)
    ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
    //ON_BN_CLICKED(IDC_BACK, OnClickedBack)
END_MESSAGE_MAP()

void SpaceToUnderscore(CString& str)
{
	const TCHAR* pch = str;
	for (int i=0; *pch != '\0'; i += _tclen(pch), pch = _tcsinc(pch))
	{
		if (str[i] == _T(' '))
			str.SetAt(i, _T('_'));
	}
}

void CConfirmDlg::SetSymbols()
{
    SetMFCPathSymbol();
	SetYearSymbol();

	CString strInsertionComment;
	strInsertionComment.LoadString(IDS_INSERT_LOCATION_COMMENT);
	SetSymbol(_T("INSERT_LOCATION_COMMENT"), strInsertionComment);

    // set the current directory and to be generated directory
	m_strDir = (LPCTSTR) projOptions.m_ProjDir;
	SetSymbol("FullProjDir", m_strDir);

	if (IsUserAWX() && !langDlls.IsAnythingLoaded())
	{
		// If we're an extension, the extension may want to have the localized
		//  DLLs loaded.  Thus, we may want to load them all now.
		CString strUseDefault;
		GetSymbol(_T("USE_DEFAULT_LANGUAGE"), strUseDefault);

		if (strUseDefault.IsEmpty())
			langDlls.LoadLibsByName();
	}

	if (!langDlls.IsAnythingLoaded())
	{
		// We haven't had the occasion to load any langdlls yet
		//  (this can occur if we're generating a DLL), so look for the right
		//  language to use. [speed:4169]

		langDlls.m_AvailableLangDlls.ScanForAvailableLanguagesIfNecessary();

		CString strLibs;
		LPCTSTR szLibs = "appwzenu.dll\n0x040904E4";	// default guess
		if (!langDlls.m_AvailableLangDlls.m_strDefaultDllName.IsEmpty())
		{
			strLibs.Format("%s\n0x%x", (LPCTSTR)langDlls.m_AvailableLangDlls.m_strDefaultDllName, 
				langDlls.m_AvailableLangDlls.m_dwDefaultTranslation);
			szLibs = strLibs;
		}
		VERIFY(langDlls.LoadLibsByName(szLibs));
	}

	// TODO: This doesn't work: 0-step zapped extensions get no symbols set.
	/*if (IsUserAWX() && !projOptions.m_bMayBeAppWizTrack)
		// Don't set these symbols if we're a non-appwiz-track extension
		return;*/

	SetMenuSymbols();
	SetFrameSymbols();
	SetChildFrameSymbols();
	SetSizeMRUSymbols(projOptions.m_nSizeMRU);

	SetSplitterSymbols();
	SetLanguageSymbols();

    // get the strings for the two main edit items
	// TODO: DELETE!!! We don't need these cuz of MFC #pragmas, right?
//    SetSymbol("EXTRA_DEBUG_LIBRARIES", "");
 //   SetSymbol("EXTRA_RELEASE_LIBRARIES", "");

    // Project type
	// We shouldn't need this set here, since it's set after first dialog.
	//SetProjectTypeSymbol();

    // app options
	SetAppOptionsSymbols();

	// project options (common to both doc & dlg app)
	// For custom wizards we do not update symbols here since that would write over
	// any change that the custom wizard made.
	// For v5 we should *never* do this here (the custom test is just to make the
	// minimal change in v4.1 which is post-beta).
	if (!IsUserAWX())
	{
		SetProjOptionsSymbols();
	}

	SetOleSymbols();

    // fill in the symbols for classes
    CString tmp;
    if (projOptions.m_names.strClass[0].IsEmpty())
        FillDefaultNames(projOptions.m_names, projOptions.m_strProj);   // just use defaults
    UpdateTrackingNames(projOptions.m_names, projOptions.m_strProj);
	tmp = projOptions.m_strProjTag;   // non-DBCS root string
	SetSymbol("Safe_root", tmp);
	SetSymbol("safe_root", tmp);
	tmp.MakeUpper();
	SetSymbol("SAFE_ROOT", tmp);
	SetClassesSymbols();
	SetDBSymbols();

    // Set DLG-related symbols
    if (projOptions.m_nProjType == PROJTYPE_DLG)
    {
		// KEEP!!! the NOT used for classes dialog
        SetSymbol("PROJTYPE_DLG", "1");
        RemoveSymbol("PROJTYPE_NOT_DLG");
		CString strDialog;
		strDialog.Format("IDD_%s_DIALOG",tmp); // name of dialog resource to open
		SetSymbol( _T("APPWIZ_INITIAL_DIALOG"), strDialog );
    }
    else
    {
        RemoveSymbol("PROJTYPE_DLG");
		if( !projOptions.m_bNoDocView ) {
			SetSymbol("PROJTYPE_NOT_DLG", "1");
		}
    }
    if (projOptions.m_nProjType != PROJTYPE_DLG && !projOptions.m_bDBSimple)
    {
        SetSymbol("NOT_DLG_OR_SIMPLE", "1");
    }
    else
    {
        RemoveSymbol("NOT_DLG_OR_SIMPLE");
    }

	SetDocumentSymbols();
	
	// MAC-specific
	if (DoesSymbolExist("TARGET_MAC"))
	{
		SetSymbol("FILE_TYPE", projOptions.m_names.strDocFileType);
		SetSymbol("CREATOR", projOptions.m_names.strDocFileCreator);
		SetSymbol("MAC_FILTER", projOptions.m_names.strMacFilter);
		
		CString strRfile = projOptions.m_strProj + "Mac";
		SetSymbol("R_FILE", strRfile);
		SetSymbol("r_file", strRfile);
	}

	SetBaseViewSymbols();
	
	RemoveInapplicableSymbols();
}

void CConfirmDlg::OnOK()
{
    TRY
    {
		if(projOptions.m_nProjType == 3)
		{
			projOptions.m_nStartRes = GetRegInt("AppWizard","StartRes",0);
			projOptions.m_nStartRes = projOptions.m_nStartRes%30000;
			projOptions.m_nStartRes+=1000;
			WriteRegInt("AppWizard","StartRes",projOptions.m_nStartRes );
		}
        if (!((CMainDlg*) m_pParentWnd)->FinalProcessing())
            return;
    }
    CATCH (CException, e)
    {
        return;
    }
    END_CATCH
    C3dDialog::OnOK();
}

BOOL CConfirmDlg::OnInitDialog()
{
    C3dDialog::OnInitDialog();
    CenterWindow();

	// Should Cancel button say "< Back"?
	if (!m_bHasCancel)
	{
		CString strBack;
		strBack.LoadString(IDS_BACK);
		GetDlgItem(IDCANCEL)->SetWindowText(strBack);
	}

	// Print static creation message ("AppWizard will create...") at top
	CString strCreation, strWizardName;
	if (!IsUserAWX() || DoesSymbolExist("PROJTYPE_CUSTOMAW"))
		strWizardName.LoadString(IDS_APPWIZ_TITLE_CREATION);
	else
		strWizardName = projOptions.m_UserExtension.m_strExtensionTitle;
	strCreation.Format(IDS_CREATION, (LPCTSTR) strWizardName);
	SetDlgItemText(IDC_STATIC_CREATION, strCreation);

	// Fill edit ctrl with parsed contents of confirm.inf
	ListStream descriptionList;
    TRY
    {
        SetSymbols();          // Set options into symbol table
	    ASSERT(m_stcOutput.m_hWnd != NULL);
	    ASSERT(m_stcDir.m_hWnd != NULL);

	    // set directory name
	    //m_stcDir.SetWindowText(FixPath(m_strDir));
		// With FixPath moved to bld system, I wonder... why was I using
		//  it here in the first place?!
		m_stcDir.SetWindowText(m_strDir);

	    // fill with canned message
		CProjGen projGen;		// For storage in case confirm.inf loops & includes
		g_IncludeStack.Clear();
		LoadAndProcessTemplate("confirm.inf", &descriptionList);
		g_IncludeStack.Clear();

    }
    CATCH(CException, e)
    {
		OnCancel();
        return TRUE;
    }
    END_CATCH


    // turn into a long string
    CString strOut;
    POSITION pos = descriptionList.list.GetHeadPosition();
    while (pos)
    {
        const CString& s = descriptionList.list.GetNext(pos);
        if (!strOut.IsEmpty())
        {
            strOut += '\r';
            strOut += '\n';
        }
        strOut += s;
    }
	
	// Figure out whether we need a scrollbar.
	
	// Count # lines in output text
	const TCHAR* pch = (const TCHAR*) strOut;
	int nTextLines = 1;
	while (pch != NULL)
	{
		pch = _tcschr(pch, _T('\n'));
		if (pch == NULL)
			break;
		pch = _tcsinc(pch);
		nTextLines++;
	}

	CWindowDC dc(&m_stcOutput);
	CFont* pFont = m_stcOutput.GetFont();
	CFont *pOldFont = dc.SelectObject(pFont);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	dc.SelectObject(pOldFont);

	LONG fHeight = tm.tmHeight * nTextLines;
	RECT rect;
	m_stcOutput.GetClientRect(&rect);
	m_stcOutput.SetWindowText(strOut);
	if (fHeight > rect.bottom)
		m_stcOutput.ShowScrollBar(SB_VERT, TRUE);
	else
		m_stcOutput.ShowScrollBar(SB_VERT, FALSE);

	// If a modal dialog is brought up before OnInitDialog is finished (e.g.,
	//  if a server's guid is already registered, and appwiz asks the user
	//  whether to use the old guid), somehow, for some reason pLaunchParent
	//  seems to become enabled, and just disabling pLaunchParent makes
	//  CConfirmDlg disabled (though its titlebar appears active).  Enabling
	//  CConfirmDlg here seems to fix all the trouble.
	EnableWindow(TRUE);

    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// Little helper which will clear a radio button (i.e., set the other in its pair)
//  or clear a checkbox.
void MyResetCheck(CButton* pBtn)
{
    if (pBtn->GetButtonStyle() == BS_AUTORADIOBUTTON)
    {
        ((CButton*) pBtn->GetParent()->GetNextDlgGroupItem(pBtn))->SetCheck(1);
    }
    pBtn->SetCheck(0);
}

// Miscellaneous...

BOOL IsMDI()
{
	return projOptions.m_nProjType == PROJTYPE_MDI;
}

void SetMDI(BOOL bMDI)
{
	projOptions.m_nProjType = (bMDI ? PROJTYPE_MDI : PROJTYPE_SDI);
	SetProjectTypeSymbol();
}

BOOL CConfirmDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	//OnHelp();
	return TRUE;
}

BOOL CMainDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	if (GetDlgItem(ID_HELP)->IsWindowVisible())
	{
		// Do regular winhelp stuff if a help button is around
		OnHelp();
		return TRUE;
	}

	return DoHelpInfo(pHelpInfo);
}

BOOL CMainDlg::DoHelpInfo(HELPINFO* pInfo)
{
	if (pInfo->iContextType == HELPINFO_WINDOW)
	{
		HWND hwndCtrl = (HWND)pInfo->hItemHandle;
		DWORD hid;
		HWND hwndHelp;
#ifdef VS_PACKAGE
		return TRUE;
#else
		if (!GetHID(m_hWnd, hwndCtrl, &hwndHelp, &hid))
		{
			// default F1 to dialog help in main help window
//			CWnd::OnHelp();
			return TRUE;
		}
#endif	// VS_PACKAGE

        CString strHelp;
        GetHelpFile(strHelp);
        if(strHelp.IsEmpty())
			return TRUE;
   		::WinHelp(hwndHelp, strHelp, HELP_WM_HELP, hid);
	}
	return TRUE;
}

BOOL CanViewClassBeUsedWithSplitter(const CString &rstrViewClassName)
{
	BOOL fResult = TRUE;
	ULONG i;

	static LPCTSTR rgszClassesThatCantUseSplitters[] =
	{
		_T("CListView"),
		_T("CTreeView"),
		_T("CEditView"),
		_T("CRichEditView"),
		_T("CFormView"),
	};

	for (i=0; i<(sizeof(rgszClassesThatCantUseSplitters) / sizeof(rgszClassesThatCantUseSplitters[0])); i++)
	{
		if (rstrViewClassName == rgszClassesThatCantUseSplitters[i])
		{
			fResult = FALSE;
			break;
		}
	}

	return fResult;
}

