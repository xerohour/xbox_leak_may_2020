// vcprojcnvt.cpp : Implementation of DLL Exports.

// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f vcprojcnvtps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include "vcprojcnvt2.h"
#include "VCProjConvert.h"
#include "project.h"
#include "register.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_VCProjConvert, CVCProjConvert)
END_OBJECT_MAP()

class CVcprojcnvtApp : public CWinApp
{
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CVcprojcnvtApp)
    public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    //}}AFX_VIRTUAL

};

CVcprojcnvtApp theApp;

void __cdecl dummyDumper(void * pvData, size_t nBytes)
{
}
									


BOOL CVcprojcnvtApp::InitInstance()
{
	if (FAILED(_Module.Init(ObjectMap, m_hInstance, &LIBID_VCPROJCNVTLib)))
        return FALSE;
	_CrtSetDumpClient(dummyDumper);

	return TRUE;
}

int CVcprojcnvtApp::ExitInstance()
{
	if (g_pPrjcompmgr)
	{
		g_pPrjcompmgr->Term();
		delete g_pPrjcompmgr;
		g_pPrjcompmgr = NULL;
	}

	if (g_pPrjoptengine)
	{
		delete g_pPrjoptengine;
		g_pPrjoptengine = NULL;
	}

	g_pActiveProject = NULL;
	g_nIDOptHdlrUnknown = 0;
	//	g_prjoptengine
	_CrtSetDumpClient(dummyDumper);
	_CrtSetReportMode(0,0);
    _Module.Term();
    return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE
extern "C"
STDAPI DllCanUnloadNow(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type
extern "C"
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.DllGetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// VSDllRegisterServer - Adds entries to the system registry

static LPOLESTR g_wszAlternateRegistryRoot = NULL;

_ATL_REGMAP_ENTRY g_rm[] = {
	{L"REGROOTBEGIN",NULL },
	{L"REGROOTEND", NULL },
	{0,0}
};

STDAPI VSDllRegisterServer(LPOLESTR wszAlternateRegistryRoot)
{
	if( wszAlternateRegistryRoot == NULL ) // Dovii NoProvii
		wszAlternateRegistryRoot = L"Software\\Microsoft\\VisualStudio\\7.0";

    // registers object, typelib and all interfaces in typelib
	g_wszAlternateRegistryRoot = wszAlternateRegistryRoot;

	CComBSTR bstrRootBegin;
	CComBSTR bstrRootEnd;
	HRESULT hr = GetRegRootStrings( g_wszAlternateRegistryRoot, &bstrRootBegin, &bstrRootEnd );
	g_rm[0].szData = bstrRootBegin.m_str;
	g_rm[1].szData = bstrRootEnd.m_str;
    hr = _Module.DllRegisterServer();
	g_wszAlternateRegistryRoot = NULL;
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
	HRESULT hr= VSDllRegisterServer(L"Software\\Microsoft\\VisualStudio\\7.0");
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// VSDllUnregisterServer - Removes entries from the system registry

STDAPI VSDllUnregisterServer(LPOLESTR wszAlternateRegistryRoot)
{
	if( wszAlternateRegistryRoot == NULL ) // Dovii NoProvii
		wszAlternateRegistryRoot = L"Software\\Microsoft\\VisualStudio\\7.0";

    // registers object, typelib and all interfaces in typelib
	g_wszAlternateRegistryRoot = wszAlternateRegistryRoot;

	CComBSTR bstrRootBegin;
	CComBSTR bstrRootEnd;
	HRESULT hr = GetRegRootStrings( g_wszAlternateRegistryRoot, &bstrRootBegin, &bstrRootEnd );
	g_rm[0].szData = bstrRootBegin.m_str;
	g_rm[1].szData = bstrRootEnd.m_str;
    hr = _Module.DllUnregisterServer();
	g_wszAlternateRegistryRoot = NULL;
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr= VSDllUnregisterServer(L"Software\\Microsoft\\VisualStudio\\7.0");
    return hr;
}
