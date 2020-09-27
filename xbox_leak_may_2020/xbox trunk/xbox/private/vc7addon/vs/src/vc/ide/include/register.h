//--------------------------------------------------------------------------
// Copyright (c) 1997,1998 Microsoft Corporation
//                     All Rights Reserved
// Information Contained Herein Is Proprietary and Confidential.
//--------------------------------------------------------------------------
//////////////////////////////////////////////
// Registration helper for packages
//
#pragma once
// The current user LCID
//
#define REGKEY_LANG_DESC	      "C/C++"
#define REGVALUE_NCB_CPP_EXTENSIONS "NCB Default C/C++ Extensions"
#define REGVALUE_NCB_IDL_EXTENSIONS "NCB Default IDL Extensions"

LCID GetUILocaleLCID(LPCTSTR szAltKey);
void UtilGetSatelliteDllName(CComBSTR& strSatDllDir, CComBSTR& strSatDllName, HINSTANCE hInstMain, LPCTSTR pszUIDllName, LPCTSTR pszAltKey);
void UtilGetSatelliteDllNameW(CComBSTR& strSatDllDir, CComBSTR& strSatDllName, HINSTANCE hInstMain, LPCOLESTR pszUIDllName, LPCOLESTR pszAltKey);

// PACKAGES
BOOL RegisterPackage(REFIID clsid, LPCTSTR szPackageName, 
					 LPCTSTR szAboutText, BOOL fPreload, LPCTSTR szAltKey);
BOOL RegisterPackageEx(REFIID clsid,
					 LPCTSTR pszPackageName, 
					 LPCTSTR pszAboutText,
					 LPCTSTR pszAltKey,
					 DWORD dwAutoCount,
					 LPCTSTR szAutomationNames[],
					 LPCTSTR pszSatDir,
					 LPCTSTR pszSatFile);
BOOL UnregisterPackage(REFIID clsid, LPCTSTR szAltKey);

// SERVICES
BOOL RegisterService(REFIID clsidService, REFIID clsidPackage, 
					 LPCTSTR szServiceName, LPCTSTR szAltKey);
BOOL UnregisterService(REFIID clsidService, LPCTSTR szAltKey);

// PROJECT TYPES
BOOL RegisterProjectType(REFIID clsidProject, REFIID clsidPackage,
						 LPCTSTR pszProjectKey,
						 LPCTSTR pszProjectItemKey,
						 LPCTSTR pszProjectName,
						 LPCTSTR pszProjectItemName,
						 DWORD	 dwProjectPriority,
						 DWORD	 dwItemPriority,
						 LPCTSTR pszDispFileExt,
						 LPCTSTR pszPossProjExt,
						 LPCTSTR pszTmplDir,
						 LPCTSTR pszNewFilesDir,
						 LPCTSTR pszItmTmplDir,
						 LPCTSTR pszDefProjExt,
						 REFGUID rguidPseudoFolder,
						 LPCTSTR szAltKey,
						 DWORD	 dwPkgID);

BOOL UnregisterProjectType(
						   
		REFIID clsidProject, 
		REFIID clsidPackage, 
		REFGUID rguidPseudoFolder,
		LPCTSTR szAltKey);

LONG CreateBaseKey(LPCTSTR szAltKey,
				   CRegKey *pKey,
				   HKEY hKeyParent = HKEY_LOCAL_MACHINE,
				   LPTSTR lpszClass = REG_NONE,
				   DWORD dwOptions = REG_OPTION_NON_VOLATILE,
				   REGSAM samDesired = KEY_READ | KEY_WRITE,
				   LPSECURITY_ATTRIBUTES lpSecAttr = NULL,
				   LPDWORD lpdwDisposition = NULL);

struct EditorExt
{
	LPCTSTR szExt;
	ULONG lRanking;
};
// EDITORS
// To register editor extensions, pass in an array of EditorExt as follow:
// EditorExt pMyEditorExt[] = { {_T("cpp"), 50}, {_T("hxx"), 25}, {NULL,0}};
BOOL RegisterEditor(REFIID clsidEditor, REFIID clsidPackage,
						 LPCTSTR szEditorName, 
						 EditorExt pExtensions[],
						 LPCTSTR szAltKey);
BOOL UnregisterEditor(REFIID clsidEditor, LPCTSTR szAltKey);

// PERSISTENCE KEYS
BOOL RegisterPersistenceKey(LPCTSTR szKeyName, LPCTSTR szAltKey, REFIID clsid);
BOOL UnregisterPersistenceKey(LPCTSTR szKeyName, LPCTSTR szAltKey);

// COMMAND TABLE
BOOL RegisterCommandTable(REFIID clsid, LPCTSTR szCommandTableFilename,
						UINT nIDCommandTable,
						UINT nVersion,
						LPCTSTR szAltKey =NULL,
						BOOL fUsingSatellite = FALSE);

BOOL UnregisterCommandTable(REFIID clsid, LPCTSTR szAltKey =NULL);


// OUTPUT WINDOW
BOOL RegisterOutputWindow(REFIID guid,
                          LPCTSTR szOutputWindowNameName, 
                          BOOL fInitiallyVisible,
                          LPCTSTR szAltKey);

BOOL UnregisterOutputWindow(REFIID guid, LPCTSTR szAltKey);

// OPTIONS PAGES
BOOL RegisterOptionsPage( LPCTSTR szGroupName, 
						  LPCTSTR szPageName, 
						  CLSID guidPackage,
						  REFIID guidPage);
BOOL UnregisterOptionsPage(LPCTSTR szPageName, LPCTSTR szAltKey);


// LANGUAGE MANAGER
BOOL RegisterLanguageManager(REFIID guid,
							 REFIID guidPkg,
							 LPCTSTR pExtensions[],
							 LPCTSTR szAltKey);

BOOL UnregisterLanguageManager(LPCTSTR pExtensions[],
							 LPCTSTR szAltKey);

BOOL RegisterToolWindow
(
 REFIID guidToolWindow,
 REFIID clsidPackage,
 LPCTSTR szToolWindowName /* = NULL */,
 LPCTSTR szAltKey /* = NULL */
 );

BOOL UnregisterToolWindow
(
 REFIID guidToolWindow, 
 LPCTSTR szAltKey /* = NULL */
 );

//The splash screen and the about box will QI the package for IVsInstalledProduct
//(and IVsMicrosoftInstalledProduct) and get all the information it needs from there.
BOOL RegisterProduct(LPCTSTR szProdKeyName, REFIID clsidPackage, LPCTSTR szAltKey);

BOOL UnregisterProduct(LPCTSTR szProdKeyName,
					 LPCTSTR szAltKey);

BOOL RegisterLibMgr(REFIID clsid, LPCTSTR szLibMgrName, 
					 REFIID pkgid, LPCTSTR szAltKey);
BOOL UnregisterLibMgr(REFIID clsid, LPCTSTR pszAltKey);

BOOL RegisterAutomationExtender(BSTR bstrExtenderCATID, BSTR bstrExtenderName, REFCLSID clsidExtensionProvider, LPCTSTR pszAltKey);
BOOL UnregisterAutomationExtender(BSTR bstrExtenderCATID, BSTR bstrExtenderName, LPCTSTR pszAltKey);

BOOL RegisterExtensionlessFile(BSTR bstrExtensionlessFile, const GUID & sidLanguageService, BSTR bstrAltKey);

void UtilGetVCInstallFolder(CStringA& strDir, BOOL bAddTrailingSlash = TRUE);
void UtilGetVCInstallFolder(CStringW& strDir, BOOL bAddTrailingSlash = TRUE);
void UtilGetVSInstallFolder(CStringA& strDir, BOOL bAddTrailingSlash = TRUE);
void UtilGetVSInstallFolder(CStringW& strDir, BOOL bAddTrailingSlash = TRUE);
void UtilGetDevenvFolder(CStringA& strDir, BOOL bAddTrailingSlash = TRUE);
void UtilGetDevenvFolder(CStringW& strDir, BOOL bAddTrailingSlash = TRUE);
void UtilGetCommon7Folder(CStringA& strDir, BOOL bAddTrailingSlash = TRUE);
void UtilGetCommon7Folder(CStringW& strDir, BOOL bAddTrailingSlash = TRUE);

HRESULT GetRegRootStrings(LPCOLESTR pszRegistrationRoot, BSTR * pbstrRootBegin, BSTR * pbstrRootEnd);
