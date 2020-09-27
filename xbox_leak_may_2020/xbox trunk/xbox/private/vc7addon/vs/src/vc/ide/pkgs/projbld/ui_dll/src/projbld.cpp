// projbld.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f projbldps.mk in the project directory.

#include "stdafx.h"
#include "utils.h"
#include "localeinfo.h"
#include "bldguid.h"
#include "..\resdll\resource.h"
#include "bldpkg.h"
#include "register.h"
#include <path2.h>

//#include "projbld_i.c"
#ifdef _DEBUG
	#define PBDLLNAME "projbldd.dll"
#else
	#define PBDLLNAME "projbld.dll"
#endif

static LPOLESTR g_wszAlternateRegistryRoot = NULL;
static char g_szAlternateRegistryRoot[_MAX_PATH];
static char *g_pszAlternateRegistryRoot = NULL;

#define COMMANDTABLE_VERSION 1
#define PACKAGE_ID	4

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_BuildPackage, CBuildPackage)
//	OBJECT_ENTRY(CLSID_DebugProjectOpen, CDebugProjectOpen) // TODO: Put this back in
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    return _Module.DllMain(hInstance, dwReason, lpReserved, ObjectMap, &LIBID_VCProjectLibrary); 
} 

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    return _Module.DllCanUnloadNow();
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.DllGetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

_ATL_REGMAP_ENTRY g_rm[] = {
	{L"VCDIR", NULL },
	{L"COMMONIDE", NULL },
	{L"REGROOTBEGIN",NULL },
	{L"REGROOTEND", NULL },
	{0,0}
};

wchar_t szProductDir[MAX_PATH+1] = {0};
wchar_t szVsProductDir[MAX_PATH+1] = {0};
CComBSTR bstrRootBegin;
CComBSTR bstrRootEnd;

void SetupMap(void)
{
	if( g_wszAlternateRegistryRoot == NULL )	// Dovii NoProvii
		g_wszAlternateRegistryRoot = L"Software\\Microsoft\\VisualStudio\\7.0";

	WideCharToMultiByte(CP_ACP, 0, g_wszAlternateRegistryRoot, -1, g_szAlternateRegistryRoot, sizeof(g_szAlternateRegistryRoot), NULL, NULL);
	g_pszAlternateRegistryRoot = g_szAlternateRegistryRoot;

	// setup paths in registry
	LONG lRet;
	DWORD nType = REG_SZ;
	DWORD nSize = MAX_PATH;
 	HKEY hSectionKey = NULL;

	// get the VC product directory
	CStringW strKey = g_wszAlternateRegistryRoot;
	strKey += L"\\Setup\\VC";
	lRet = RegOpenKeyExW( HKEY_LOCAL_MACHINE, strKey, 0, KEY_READ, &hSectionKey );
	if( hSectionKey )
		lRet = RegQueryValueExW( hSectionKey, L"ProductDir", NULL, &nType, (LPBYTE)szProductDir, &nSize );
	int len = lstrlenW(szProductDir);
	if( len && szProductDir[len-1] != '\\' )
	{
		szProductDir[len] = '\\';
		szProductDir[len+1] = NULL;
	}
	if( hSectionKey )
    	RegCloseKey( hSectionKey );
	g_rm[0].szData = szProductDir;

	// Get The Vs product Dir
	strKey = g_wszAlternateRegistryRoot;
	strKey += L"\\Setup\\VS";
	lRet = RegOpenKeyExW( HKEY_LOCAL_MACHINE, strKey, 0, KEY_READ, &hSectionKey );
	if( hSectionKey )
		lRet = RegQueryValueExW( hSectionKey, L"ProductDir", NULL, &nType, (LPBYTE)szVsProductDir, &nSize );
	len = lstrlenW(szVsProductDir);
	if( len && szVsProductDir[len-1] != '\\' )
	{
		szVsProductDir[len] = '\\';
		szVsProductDir[len+1] = NULL;
	}
	if( hSectionKey )
    	RegCloseKey( hSectionKey );
	g_rm[1].szData = szVsProductDir;
	
	HRESULT hr = GetRegRootStrings( g_wszAlternateRegistryRoot, &bstrRootBegin, &bstrRootEnd );
	if( SUCCEEDED(hr) )
	{
		g_rm[2].szData = bstrRootBegin.m_str;
		g_rm[3].szData = bstrRootEnd.m_str;
	} else {
		g_rm[2].szKey = NULL;
	}
}

STDAPI DllRegisterServer(void)
{
	SetupMap();
	HRESULT hr = _Module.DllRegisterServer();
	_Module.RegisterTypeLib(_T("\\2"));
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	SetupMap();
	return _Module.UnregisterServer(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// VSDllRegisterServer - Adds entries to the system registry

STDAPI VSDllRegisterServer(LPOLESTR wszAlternateRegistryRoot)
{
	g_wszAlternateRegistryRoot = wszAlternateRegistryRoot;
	HRESULT hr= DllRegisterServer();
	g_wszAlternateRegistryRoot = NULL;
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// VSDllUnregisterServer - Removes entries from the system registry

STDAPI VSDllUnregisterServer(LPOLESTR wszAlternateRegistryRoot)
{
	g_wszAlternateRegistryRoot = wszAlternateRegistryRoot;
	HRESULT hr= DllUnregisterServer();
	g_wszAlternateRegistryRoot = NULL;
	return hr;
}

extern "C" HRESULT __declspec(dllexport) DLLGetDocumentation( ITypeLib *ptlib, ITypeInfo *ptinfo, LCID lcid, DWORD dwHelpStringContext, BSTR *pbstrHelpString)
{
	CComBSTR bstrHelp;
	bstrHelp.LoadString(dwHelpStringContext);
	*pbstrHelpString = bstrHelp.Detach();
	return S_OK;
}
