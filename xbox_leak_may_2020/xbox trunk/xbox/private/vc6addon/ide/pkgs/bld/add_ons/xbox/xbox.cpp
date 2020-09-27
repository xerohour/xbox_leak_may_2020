//
// Xbox C/C++ AddOn
//
// Xbox Platform C/C++ Tools Component Add-On 'package'
//
// [matthewt]
//

#include "stdafx.h" 
#include <afxdllxx.h>	// our modified version of afxdllx.h

#include <initguid.h>
#include <bldapi.h>
#include <bldguid.h>

// add-on's MFC extension DLL information struct.
static AFX_EXTENSION_MODULE extensionDLL = {NULL, NULL};

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// our components
#include "platxbox.h"

#include "xbxtoolc.h"
#include "xbxtooll.h"
#include "toolxbe.h"
#include "toolxbcp.h"


class CXboxCAddOn : public CPackage
{
public:
	CXboxCAddOn();

	// register the build system components for this platform
	BOOL DoBldSysCompRegister(CProjComponentMgr * pcompmgr, DWORD blc_type, DWORD blc_id);

	virtual DWORD GetIdRange(RANGE_TYPE rt);

private:
	WORD wpkgid;
};

CXboxCAddOn::CXboxCAddOn()
	// o instance
	// o handle to window
	// o package id (* do not use *)
	// o command-table
	// o index into cmdtable
	: CPackage(extensionDLL.hModule, 0, 0)
{
}


BOOL CXboxCAddOn::DoBldSysCompRegister(CProjComponentMgr * pcompmgr, DWORD blc_type, DWORD bsc_id)
{
	wpkgid = pcompmgr->GenerateBldSysCompPackageId((TCHAR *)szAddOnXbox);

    DWORD bsc_idXboxPlatform = GenerateComponentId(wpkgid, BCID_Platform_Xbox);

	// C++ installed?
	static BOOL fGotCPP = TRUE;
#if 0	// avoid loading DEVCPP package for no reason
	if( !fGotCPP ){
		LPBUILDSYSTEM pBldSysIFace;
		VERIFY(SUCCEEDED(theApp.FindInterface(IID_IBuildSystem, (LPVOID FAR *)&pBldSysIFace)));
		fGotCPP = (pBldSysIFace->LanguageSupported(CPlusPlus) == S_OK);
		ASSERT(fGotCPP);
		pBldSysIFace->Release();
	}	
#endif
	CPlatform *pPlat;
	// what is the hook?
	switch (blc_type)
	{
		case BLC_Platform:
			pPlat = new CPlatform(&g_PlatformInfoXbox);
			pcompmgr->RegisterBldSysComp(pPlat, bsc_idXboxPlatform);
			break;

		case BLC_TargetType:
			if (bsc_id == bsc_idXboxPlatform){
				// our Xbox project types
				pcompmgr->RegisterBldSysComp(new CProjTypeXboxExe);
#ifdef XBOXDLL
				pcompmgr->RegisterBldSysComp(new CProjTypeXboxDll);
#endif
				pcompmgr->RegisterBldSysComp(new CProjTypeXboxLib);
				pcompmgr->RegisterBldSysComp(new CProjTypeXboxGeneric);
			}
			break;

		case BLC_Tool:
		{
			DWORD type = pcompmgr->BldSysCompType(bsc_id);

			// for the platform?
			if (type == BLC_Platform)
			{
				// yes
				// only for the Xbox platform!
				if (bsc_id == bsc_idXboxPlatform){
					// our Xbox tools
					if (fGotCPP)	// install C/C++ compiler only if have language
						pcompmgr->RegisterBldSysComp(new CCCompilerXboxTool,
													 GenerateComponentId(wpkgid, BCID_Tool_Compiler_Xbox));
					pcompmgr->RegisterBldSysComp(new CLinkerXboxExeTool,
												 GenerateComponentId(wpkgid, BCID_Tool_Linker_XboxExe));
#ifdef XBOXDLL
					pcompmgr->RegisterBldSysComp(new CLinkerXboxDllTool,
												 GenerateComponentId(wpkgid, BCID_Tool_Linker_XboxDll));
#endif
					pcompmgr->RegisterBldSysComp(new CXbeBuilderTool,
												 GenerateComponentId(wpkgid, BCID_Tool_XbeBuilder));
#ifdef XBCP
					pcompmgr->RegisterBldSysComp(new CXbcpTool,
												 GenerateComponentId(wpkgid, BCID_Tool_Xbcp));
#endif
				}
			break;
			}
			// for a target type?
			else if (fGotCPP && type == BLC_TargetType)	// install C/C++ compiler only if have language
			{		   
				// yes
				CBldSysCmp * pcomp;
				VERIFY(pcompmgr->LookupBldSysComp(bsc_id, pcomp));

				// only for the Xbox platform!
				if (pcomp->RelatedCompId() != bsc_idXboxPlatform)	break;

				if (((CProjType *)pcomp)->GetUniqueTypeId() == CProjType::generic)	break;

				// add our Xbox C/C++ compiler tool to these target types
				((CProjType *)pcomp)->AddTool(wpkgid, BCID_Tool_Compiler_Xbox);
			}
			break;
		}

		case BLC_OptionHdlr:
			// only for the Xbox platform!
			if (bsc_id == bsc_idXboxPlatform){
				// our Xbox option handlers
				if (fGotCPP)	// install C/C++ compiler only if have language
					pcompmgr->RegisterBldSysComp(new OPTION_HANDLER(CompilerXbox));

				pcompmgr->RegisterBldSysComp(new OPTION_HANDLER(XbeBuilder));
				pcompmgr->RegisterBldSysComp(new OPTION_HANDLER(LinkerXboxExe));
#ifdef XBOXDLL
				pcompmgr->RegisterBldSysComp(new OPTION_HANDLER(LinkerXboxDll));
#endif
#ifdef XBCP
				pcompmgr->RegisterBldSysComp(new OPTION_HANDLER(Xbcp));
#endif
			}
			break;

		case BLC_Scanner:
			break;

		default:
			break;
	}

	// nothing left to register
	return FALSE;
}

DWORD CXboxCAddOn::GetIdRange(RANGE_TYPE rt)
{
	switch (rt)
	{
		case MIN_RESOURCE:
		case MIN_STRING:
			return MIN_POWERPC_RESOURCE_ID;
			
		case MAX_RESOURCE:
		case MAX_STRING:
			return MAX_X86_RESOURCE_ID;
	}

	return CPackage::GetIdRange(rt);
}






// package entry/exit routines

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
		DisableThreadLibraryCalls(hInstance);

		// NOTE: global/static constructors have already been called!
		// Extension DLL one-time initialization - do not allocate memory here,
		//   use the TRACE or ASSERT macros or call MessageBox
		if (!AfxInitExtensionModule(extensionDLL, hInstance))
			return 0;

		// This adds our DLL to the MFC maintained list of "AFX
		// Extension DLLs" which is used by serialization functions
		// and resource loading...  This will be deleted by MFC in
		// the AfxTermExtensionModule function.
		new CDynLinkLibrary(extensionDLL);

		// rest of init now happens in InitPackage
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		// Terminate the library before destructors are called
		AfxTermExtensionModule(extensionDLL);
	}

	return TRUE;   // ok
}

extern "C" BOOL PASCAL AFX_EXPORT InitPackage(HWND hWndShell)
{
	// register this package
	theApp.RegisterPackage(new CXboxCAddOn);	  

	// other initialization
	return TRUE;	// init'ed ok
}

extern "C" void PASCAL AFX_EXPORT ExitPackage()
{
}
