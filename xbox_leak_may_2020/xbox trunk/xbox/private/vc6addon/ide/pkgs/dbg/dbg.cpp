////////////////////////////////////////////////////////////////////////////
//
// A super-simple sample package that shows off most of the shell interface.
//

#include "stdafx.h"
#pragma hdrstop

#include "afxdllxx.h"	// our modified version of afxdllx.h
#ifdef DEBUGGER_AUTOMATION
#include <utilauto.h>
#endif // DEBUGGER_AUTOMATION

////////////////////////////////////////////////////////////////////////////
// This is our entry for the AFX Extension DLL chain.  Do not change!
//
static AFX_EXTENSION_MODULE extensionDLL;

extern "C" BOOL WINAPI RawDllMain(HINSTANCE, DWORD dwReason, LPVOID);
extern "C" BOOL (WINAPI* _pRawDllMain)(HINSTANCE, DWORD, LPVOID) = &RawDllMain;

PSRCUTIL            gpISrc          = NULL;
LPBUILDSYSTEM       gpIBldSys       = NULL;
LPBUILDPLATFORMS    gpIBldPlatforms = NULL;
LPSOURCEQUERY       gpISrcQuery     = NULL;
LPOUTPUTWINDOW		gpIOutputWindow = NULL;
IProjectWorkspace	*gpIProjectWorkspace=NULL;
IPkgProject			*gpActiveIPkgProject=NULL;
IDBGProj			*gpActiveIDBGProj=NULL;

CDebug              foo;
CDebug              *pDebugCurr = &foo;

////////////////////////////////////////////////////////////////////////////
// In the CPackage constructor, we must setup a bunch of information about
// our DLL and the commands it provides...
//
CDebugPackage::CDebugPackage() : CPackage(hInst, PACKAGE_DEBUG, PKS,
                                          MAKEINTRESOURCE(IDCT_DBGPACKAGE),
                                          MAKEINTRESOURCE(IDB_MAINTOOLS),
                                          MAKEINTRESOURCE(NULL),
                                          MAKEINTRESOURCE(IDB_MAINLARGETOOLS))
{
}

////////////////////////////////////////////////////////////////////////////
// This function is proved by each package to provide access to the local
// resource handle (the hInstance for the package's DLL).  It is used by the
// CLocalResource class which is a handy helper for temporarily changing the
// module MFC uses to load resources from by default.
//
HINSTANCE GetResourceHandle()
{
	return extensionDLL.hModule;
}

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
        hInst = hInstance;

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

//void CDebugPackage::LoadGlobalAccelerators(void)
//{
//    AddGlobalAcceleratorTable(MAKEINTRESOURCE(IDR_GLOBAL_DEBUGGER_KEYS));
//}

DWORD CDebugPackage::GetIdRange(RANGE_TYPE rt)
{
	switch (rt)
	{
		case MIN_RESOURCE:
			return MIN_DEBUG_RESOURCE_ID;

		case MAX_RESOURCE:
			return MAX_DEBUG_RESOURCE_ID;

		case MIN_COMMAND:
			return MIN_SOURCE_COMMAND_ID;

		case MAX_COMMAND:
			return MAX_SOURCE_COMMAND_ID;

		case MIN_WINDOW:
			return MIN_SOURCE_WINDOW_ID;

		case MAX_WINDOW:
			return MAX_SOURCE_WINDOW_ID;
	}

	return( CPackage::GetIdRange(rt) );
}


#define CLSID_Debugger_INITIALIZER \
	{ 0x34C63003L,0xAE64,0x11CF, {0xAB,0x59,0x00,0xAA,0x00,0xC0,0x91,0xA1} }

int CDebugPackage::GetApplicationExtensions(SApplicationExtension** ppAppExts)
{
//	ASSERT(FALSE);
	static SApplicationExtension PkgAppExt =
	{
		"Debugger",
		CLSID_Debugger_INITIALIZER
	};

	*ppAppExts = &PkgAppExt;
	return 1;
}

void CDebugPackage::SerializeWorkspaceSettings(CStateSaver &pStateSave, DWORD dwFlags)
{
	if (pDebugCurr)
		pDebugCurr->Init(FALSE);					// needed for loading database projects.

	if (dwFlags & OPT_UNLOAD)
	{
		// clear all breakpoints at CV level and IDE level
		ClearCV400Breakpoints();
		ClearBreakpointNodeList();

		// CAVIAR 4650: Update doc [mikeho]
		gpISrc->ClearAllDocStatus(BRKPOINT_LINE);

		// Destroy the debugger file alias list so we don't add dupes!
		ClearPathMappings();
		
		// Reset this flag.
		g_fPromptNoSymbolInfo = TRUE;

	} else {
		// write out information whether there is an associated project or not
		// in this way we are unlike VPROJ
		if (pStateSave.IsStoring())
		{
			SaveIntoOPTFile(pStateSave);
		}
		else
		{
			LoadFromOPTFile(pStateSave);
		}
	}
}

