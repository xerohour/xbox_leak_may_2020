// vcpb.cpp : Implementation of DLL Exports.

// Note: Proxy/Stub Information
//	To build a separate proxy/stub DLL, 
//	run nmake -f vcpbps.mk in the project directory.

#include "stdafx.h"
#include "VCProjectEngine.h"

#include "configuration.h"
#include "fileconfiguration.h"

#include "platform.h"
#include "PlatformXbox.h"

#include "debugsettings.h"

#include "SettingsPage.h"

#include "VCNodeFactory.h"
#include "XMLFile.h"

#include "profile.h"
#include "register.h"
#include "util2.h"

static LPOLESTR g_wszAlternateRegistryRoot = NULL;

// Declare the settings dialog pages ...

// Compiler
typedef CSettingsPage<&CLSID_ClGeneral,IDS_ClGeneral,IDS_Cl> CClGeneral;
typedef CSettingsPage<&CLSID_ClOptimization,IDS_ClOptimization,IDS_Cl> CClOptimization;
typedef CSettingsPage<&CLSID_ClPreprocessor,IDS_ClPreprocessor,IDS_Cl> CClPreprocessor;
typedef CSettingsPage<&CLSID_ClAdvanced,IDS_ClAdvanced,IDS_Cl> CClAdvanced;
typedef CSettingsPage<&CLSID_ClCodeGeneration,IDS_ClCodeGeneration,IDS_Cl> CClCodeGeneration;
typedef CSettingsPage<&CLSID_ClLanguage,IDS_ClLanguage,IDS_Cl> CClLanguage;
typedef CSettingsPage<&CLSID_ClPrecompiledHeaders,IDS_ClPrecompiledHeaders,IDS_Cl> CClPrecompiledHeaders;
typedef CSettingsPage<&CLSID_ClOutputFiles,IDS_ClOutputFiles,IDS_Cl> CClOutputFiles;
typedef CSettingsPage<&CLSID_ClBrowseInfo,IDS_ClBrowseInfo,IDS_Cl> CClBrowseInfo;
typedef CAdditionalOptionsPage<VCCLCompilerTool, &CLSID_ClAdditionalOptions,IDS_Cl, VCCLID_AdditionalOptions> CClAdditionalOptions;

// Midl
typedef CSettingsPage<&CLSID_MidlAdvanced,IDS_MidlAdvanced,IDS_Midl> CMidlAdvanced;
typedef CSettingsPage<&CLSID_MidlOutput,IDS_MidlOutput,IDS_Midl> CMidlOutput;
typedef CSettingsPage<&CLSID_MidlGeneral,IDS_MidlGeneral,IDS_Midl> CMidlGeneral;
typedef CAdditionalOptionsPage<VCMidlTool, &CLSID_MidlAdditionalOptions,IDS_Midl, VCMIDLID_AdditionalOptions> CMidlAdditionalOptions;

// RC
typedef CSettingsPage<&CLSID_RCGeneral,IDS_RCGeneral,IDS_RC> CRCGeneral;
typedef CAdditionalOptionsPage<VCResourceCompilerTool, &CLSID_RCAdditionalOptions,IDS_RC, VCRCID_AdditionalOptions> CRCAdditionalOptions;

// Build events
typedef CSettingsPage<&CLSID_VCPreBldGeneral, IDS_PreBuildEvent, IDS_BldEvents> CPreBldEventGeneral;
typedef CSettingsPage<&CLSID_VCPreLinkGeneral, IDS_PreLinkEvent, IDS_BldEvents> CPreLinkEventGeneral;
typedef CSettingsPage<&CLSID_VCPostBldGeneral, IDS_PostBuildEvent, IDS_BldEvents> CPostBldEventGeneral;
typedef CSettingsPage<&CLSID_VCCustomGeneral, IDS_CustomGeneral, IDS_CustomBuild> CCustomBuildGeneral;
// typedef CSettingsPage<&CLSID_VCCustomGeneral, IDS_CustomOutputs, IDS_CustomBuild, 0,szCustomBuildToolType> CCustomBuildOutputs;

// BSC
typedef CSettingsPage<&CLSID_VCBSCGeneral,IDS_BSCGeneral,IDS_BSC> CBSCGeneral;
typedef CAdditionalOptionsPage<VCBscMakeTool, &CLSID_BSCAdditionalOptions,IDS_BSC, VCBSCID_AdditionalOptions> CBSCAdditionalOptions;

// Lib
typedef CSettingsPage<&CLSID_LibGeneral,IDS_LibGeneral,IDS_Lib> CLibGeneral;
typedef CAdditionalOptionsPage<VCLibrarianTool, &CLSID_LibAdditionalOptions,IDS_Lib,VCLIBID_AdditionalOptions> CLibAdditionalOptions;

// Linker
typedef CSettingsPage<&CLSID_LinkGeneral,IDS_LinkGeneral,IDS_Link> CLinkGeneral;
typedef CSettingsPage<&CLSID_LinkInput,IDS_LinkInput,IDS_Link> CLinkInput;
typedef CSettingsPage<&CLSID_LinkDebug,IDS_LinkDebug,IDS_Link> CLinkDebug;
typedef CSettingsPage<&CLSID_LinkSystem,IDS_LinkSystem,IDS_Link> CLinkSystem;
typedef CSettingsPage<&CLSID_LinkOptimization,IDS_LinkOptimization,IDS_Link> CLinkOptimization;
typedef CSettingsPage<&CLSID_LinkAdvanced,IDS_LinkAdvanced,IDS_Link> CLinkAdvanced;
typedef CSettingsPage<&CLSID_LinkMIDL,IDS_LinkMIDL,IDS_Link> CLinkMIDL;
typedef CAdditionalOptionsPage<VCLinkerTool, &CLSID_LinkAdditionalOptions, IDS_Link, VCLINKID_AdditionalOptions> CLinkAdditionalOptions;

// NMake
typedef CSettingsPage<&CLSID_VCNMakeGeneral, IDS_NMake, NULL> CNMakeGeneral;

// Web Service Proxy Generator
typedef CSettingsPage<&CLSID_WebUtilityGeneral, IDS_WebGeneral, IDS_WEBTOOL> CWebUtilityGeneral;
typedef CAdditionalOptionsPage<VCWebServiceProxyGeneratorTool, &CLSID_WebAdditionalOptions,IDS_WEBTOOL, VCWEBID_AdditionalOptions> CWebAdditionalOptions;

// Web Deployment
typedef CSettingsPage<&CLSID_WebDeploymentGeneral, IDS_DeployGeneral, IDS_DEPLOYTOOL> CWebDeployGeneral;

// Xbox Deployment
typedef CSettingsPage<&CLSID_XboxDeploymentGeneral, IDS_XboxDeployGeneral, IDS_XBOXDEPLOYTOOL> CXboxDeployGeneral;

// Xbox Image
typedef CSettingsPage<&CLSID_XboxImageGeneral, IDS_XboxImageGeneral, IDS_XBOXIMAGETOOL> CXboxImageGeneral;
typedef CSettingsPage<&CLSID_XboxImageCertificate, IDS_XboxImageCertificate, IDS_XBOXIMAGETOOL> CXboxImageCertificate;
typedef CSettingsPage<&CLSID_XboxImageTitleInfo, IDS_XboxImageTitleInfo, IDS_XBOXIMAGETOOL> CXboxImageTitleInfo;

// Config General
typedef CSettingsPage<&CLSID_GeneralFileConfigSettingsPage, IDS_GeneralConfigSettings, 0> CGeneralFileConfigSettingsPage;

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_VCProjectEngineObject, CVCProjectEngine)
	OBJECT_ENTRY(CLSID_VCPlatformWin32, CPlatformWin32)
	OBJECT_ENTRY(CLSID_VCPlatformWin64, CPlatformWin64)
	OBJECT_ENTRY(CLSID_VCPlatformXbox, CPlatformXbox)
	OBJECT_ENTRY(CLSID_VCNodeFactory, CVCNodeFactory)

	OBJECT_ENTRY(CLSID__VCDebugSettings, CDebugSettings)
	OBJECT_ENTRY(CLSID_DebugSettingsPage, CDbgSettingsPage)
	OBJECT_ENTRY(CLSID_GeneralConfigSettingsPage, CGeneralConfigSettingsPage)
	OBJECT_ENTRY(CLSID_GeneralFileConfigSettingsPage, CGeneralFileConfigSettingsPage)

#ifdef AARDVARK
	OBJECT_ENTRY(CLSID_GeneralStyleSheetSettings, CGeneralStyleSheetSettings)
#endif	// AARDVARK

	// Compiler
	OBJECT_ENTRY(CLSID_ClGeneral, CClGeneral)
	OBJECT_ENTRY(CLSID_ClOptimization, CClOptimization)
	OBJECT_ENTRY(CLSID_ClPreprocessor, CClPreprocessor)
	OBJECT_ENTRY(CLSID_ClAdvanced, CClAdvanced)
	OBJECT_ENTRY(CLSID_ClCodeGeneration, CClCodeGeneration)
	OBJECT_ENTRY(CLSID_ClLanguage, CClLanguage)
	OBJECT_ENTRY(CLSID_ClPrecompiledHeaders, CClPrecompiledHeaders)
	OBJECT_ENTRY(CLSID_ClOutputFiles, CClOutputFiles)
	OBJECT_ENTRY(CLSID_ClBrowseInfo, CClBrowseInfo)
	OBJECT_ENTRY(CLSID_ClAdditionalOptions, CClAdditionalOptions)

	// MIDL
	OBJECT_ENTRY(CLSID_MidlAdvanced, CMidlAdvanced)
	OBJECT_ENTRY(CLSID_MidlOutput, CMidlOutput)
	OBJECT_ENTRY(CLSID_MidlGeneral, CMidlGeneral)
	OBJECT_ENTRY(CLSID_MidlAdditionalOptions, CMidlAdditionalOptions)

	// RC
	OBJECT_ENTRY(CLSID_RCGeneral, CRCGeneral)
	OBJECT_ENTRY(CLSID_RCAdditionalOptions, CRCAdditionalOptions)

	// BSC
	OBJECT_ENTRY(CLSID_VCBSCGeneral, CBSCGeneral)
	OBJECT_ENTRY(CLSID_BSCAdditionalOptions, CBSCAdditionalOptions)

	// Lib
	OBJECT_ENTRY(CLSID_LibGeneral, CLibGeneral)
	OBJECT_ENTRY(CLSID_LibAdditionalOptions, CLibAdditionalOptions)

	// Link
	OBJECT_ENTRY(CLSID_LinkMIDL, CLinkMIDL)
	OBJECT_ENTRY(CLSID_LinkGeneral, CLinkGeneral)
	OBJECT_ENTRY(CLSID_LinkInput, CLinkInput)
	OBJECT_ENTRY(CLSID_LinkDebug, CLinkDebug)
	OBJECT_ENTRY(CLSID_LinkSystem, CLinkSystem)
	OBJECT_ENTRY(CLSID_LinkOptimization, CLinkOptimization)
	OBJECT_ENTRY(CLSID_LinkAdvanced, CLinkAdvanced)
	OBJECT_ENTRY(CLSID_LinkAdditionalOptions, CLinkAdditionalOptions)
	
	// Custom build
	OBJECT_ENTRY(CLSID_VCCustomGeneral, CCustomBuildGeneral)
	
	// Build events
	OBJECT_ENTRY(CLSID_VCPreBldGeneral, CPreBldEventGeneral)
	OBJECT_ENTRY(CLSID_VCPreLinkGeneral, CPreLinkEventGeneral)
	OBJECT_ENTRY(CLSID_VCPostBldGeneral, CPostBldEventGeneral)

	// NMake 
	OBJECT_ENTRY(CLSID_VCNMakeGeneral, CNMakeGeneral)

	// Web Service Proxy Generator
	OBJECT_ENTRY(CLSID_WebUtilityGeneral, CWebUtilityGeneral)
	OBJECT_ENTRY(CLSID_WebAdditionalOptions, CWebAdditionalOptions)

	// Web Deployment
	OBJECT_ENTRY(CLSID_WebDeploymentGeneral, CWebDeployGeneral)

	// Xbox Deployment
	OBJECT_ENTRY(CLSID_XboxDeploymentGeneral, CXboxDeployGeneral)

	// Xbox Image
	OBJECT_ENTRY(CLSID_XboxImageGeneral, CXboxImageGeneral)
	OBJECT_ENTRY(CLSID_XboxImageCertificate, CXboxImageCertificate)
	OBJECT_ENTRY(CLSID_XboxImageTitleInfo, CXboxImageTitleInfo)
END_OBJECT_MAP()





/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
#ifdef DEBUG
#endif
	return _Module.DllMain(hInstance, dwReason, lpReserved, ObjectMap, &LIBID_VCProjectEngineLibrary); 
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
// VSDllRegisterServer - Adds entries to the system registry

_ATL_REGMAP_ENTRY g_rm[] = {
	{L"EXEPATH", NULL },
	{L"LIB", NULL },
	{L"INCLUDE", NULL },
	{L"SOURCE", NULL },
	{L"HHPATH", NULL },
	{L"PATHENV", NULL },
	{L"REGROOTBEGIN",NULL },
	{L"REGROOTEND", NULL },
	{L"XDK", NULL },
	{0,0}
};

wchar_t szVsProductDir[MAX_PATH+1] = {0};
CComBSTR bstrPath;
CComBSTR bstrWin;
CComBSTR bstrHhcDir;

CComBSTR bstrRootBegin;
CComBSTR bstrRootEnd;
CComBSTR bstrExePath;
CComBSTR bstrLib;
CComBSTR bstrInclude;
CComBSTR bstrSource;
CComBSTR bstrXDK;

static void SetupMap()
{
	// setup paths in registry
	LONG lRet;
	DWORD nType = REG_SZ;
	DWORD nSize = MAX_PATH;
	HKEY hSectionKey = NULL;

	// get the VS product directory
	CStringW strKey = g_wszAlternateRegistryRoot;
	strKey += L"\\Setup\\VS";
	lRet = RegOpenKeyExW( HKEY_LOCAL_MACHINE, strKey, 0, KEY_READ, &hSectionKey );
	if( hSectionKey )
	{
		lRet = RegQueryValueExW( hSectionKey, L"ProductDir", NULL, &nType, (LPBYTE)szVsProductDir, &nSize );
		RegCloseKey( hSectionKey );
	}

	// figure out where the help compiler is
	bstrHhcDir = szVsProductDir;
	int nLen = bstrHhcDir.Length();
	if (nLen > 0 && bstrHhcDir[nLen-1] != L'\\')
		bstrHhcDir += L'\\';
	bstrHhcDir += L"Visual Studio SDKs\\HTML Help 1.3 SDK\\workshop";

	// append the path for this process
	VCGetEnvironmentVariableW(L"Path", &bstrPath);

	// get the windows directory
	char szWin[2048];
	int rval = GetWindowsDirectoryA(szWin, 2047);
	if( rval )
	{
	    bstrWin = szWin;
	    bstrWin += L"\\";
	}

	// The VC_RealSetup Key should be set only if you are trying to reg the project system after the fact on a
	// real install. Otherwise its not going to be set.
	CComBSTR bstrRealSetup;
	HRESULT hr = ::VCGetEnvironmentVariableW(L"VC_REALSETUP", &bstrRealSetup);
	if( hr != S_OK ) // Batch setup.
	{
	    bstrExePath = L"$(VCInstallDir)bin;$(VSInstallDir)Common7\\Tools;$(VSInstallDir);";
		bstrExePath += L"$(FrameworkDir)$(FrameworkVersion);$(FrameworkSDKDir);";
// 	    bstrExePath += bstrPath;
    
		bstrLib = L"$(VCInstallDir)lib;$(VCInstallDir)atlmfc\\lib;$(FrameworkSDKDir)lib";

		bstrInclude = L"$(VCInstallDir)include;$(VCInstallDir)atlmfc\\include;$(FrameworkSDKDir)include";

		bstrSource = L"$(VCInstallDir)atlmfc\\src\\mfc;$(VCInstallDir)atlmfc\\src\\atl;$(VCInstallDir)crt\\src";
	}
	else  // REAL SETUP USES MANY DIFFERENT PATHS.
	{
		bstrExePath = L"$(VCInstallDir)bin;$(VSInstallDir)Common7\\Tools\\bin\\prerelease;";
		bstrExePath += L"$(VSInstallDir)Common7\\Tools\\bin;$(VSInstallDir)Common7\\Tools;";
		bstrExePath += L"$(VSInstallDir)Common7\\ide;";
	    bstrExePath += bstrHhcDir;
	    bstrExePath +=";$(FrameworkSDKDir)bin;$(FrameworkDir)$(FrameworkVersion);";
// 	    bstrExePath += bstrPath;

		bstrLib = L"$(VCInstallDir)lib;$(VCInstallDir)atlmfc\\lib;$(VCInstallDir)PlatformSDK\\lib\\prerelease;";
	    bstrLib += L"$(VCInstallDir)PlatformSDK\\lib;$(FrameworkSDKDir)lib";

		bstrInclude = L"$(VCInstallDir)include;$(VCInstallDir)atlmfc\\include;";
		bstrInclude += L"$(VCInstallDir)PlatformSDK\\include\\prerelease;$(VCInstallDir)PlatformSDK\\include;";
	    bstrInclude += L"$(FrameworkSDKDir)include";
	    
		bstrSource = L"$(VCInstallDir)atlmfc\\src\\mfc;$(VCInstallDir)atlmfc\\src\\atl;$(VCInstallDir)crt\\src";
	}

	// Get The registry strings for the VS registry root.
	HRESULT chr = GetRegRootStrings( g_wszAlternateRegistryRoot, &bstrRootBegin, &bstrRootEnd );

	// Get the root of the XDK
	VCGetEnvironmentVariableW(L"XDK", &bstrXDK);

	// now fill up everything.
	g_rm[0].szData = bstrExePath.m_str;
	g_rm[1].szData = bstrLib.m_str;
	g_rm[2].szData = bstrInclude.m_str;
	g_rm[3].szData = bstrSource.m_str;
	g_rm[4].szData = bstrHhcDir.m_str;
	g_rm[5].szData = bstrPath.m_str;
	g_rm[6].szData = bstrRootBegin.m_str;
	g_rm[7].szData = bstrRootEnd.m_str;
	g_rm[8].szData = bstrXDK.m_str;
}

STDAPI VSDllRegisterServer(LPOLESTR wszAlternateRegistryRoot)
{
	if( wszAlternateRegistryRoot == NULL ) // Dovii NoProvii
		wszAlternateRegistryRoot = L"Software\\Microsoft\\VisualStudio\\7.0";

	// registers object, typelib and all interfaces in typelib
	g_wszAlternateRegistryRoot = wszAlternateRegistryRoot;

	SetupMap();

	HRESULT hr = _Module.DllRegisterServer();
	_Module.RegisterTypeLib(_T("\\2"));
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
	g_wszAlternateRegistryRoot = wszAlternateRegistryRoot;
	if( wszAlternateRegistryRoot == NULL )	// Dovii NoProvii
		wszAlternateRegistryRoot = L"Software\\Microsoft\\VisualStudio\\7.0";

	SetupMap();

	HRESULT hr = _Module.DllUnregisterServer();
	_Module.UnRegisterTypeLib(_T("\\2"));
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

extern "C" HRESULT __declspec(dllexport) DLLGetDocumentation( ITypeLib *ptlib, ITypeInfo *ptinfo, LCID lcid, DWORD dwHelpStringContext, BSTR *pbstrHelpString)
{
	CComBSTR bstrHelp;
	HRESULT hr = bstrHelp.LoadString(dwHelpStringContext);
	*pbstrHelpString = bstrHelp.Detach();
	return S_OK;
}

HRESULT OpenTopic(IVCProjectEngineImpl *pProjEngineImpl, BSTR bstrTopic)
{
	CComPtr<Help> pHlp;
	HRESULT hr = pProjEngineImpl->GetHelp(&pHlp);
	if( pHlp )
		hr = pHlp->DisplayTopicFromF1Keyword(bstrTopic);
	return hr;
}
