//
// Alpha (RISC) Platform
//
// [matthewt]
//
						
#ifndef _INCLUDE_PLATFORM_ALF_H
#define _INCLUDE_PLATFORM_ALF_H

#include "alpha.h"

// declare out platform
extern	PlatformInfo	g_PlatformInfoWin32ALPHA;

// declare our project types
// declare our project types (FOR ALPHA)
class CProjTypeWin32ALPHAExe : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeWin32ALPHAExe)

public:
	CProjTypeWin32ALPHAExe()
		: CProjType(szAddOnx86, BCID_ProjType_AlphaExe, /* id */
					IDS_WIN32EXE_PROJTYPE, CProjType::application, /* props */
					szAddOnx86, BCID_Platform_Alpha) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD blc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {return ImageExe | SubsystemWindows | TargetIsDebugable;}
};

class CProjTypeWin32ALPHADll : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeWin32ALPHADll)

public:
	CProjTypeWin32ALPHADll()
		: CProjType(szAddOnx86, BCID_ProjType_AlphaDll, /* id */
					IDS_WIN32DLL_PROJTYPE, CProjType::dynamiclib, /* props */
					szAddOnx86, BCID_Platform_Alpha) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD blc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {return ImageDLL | TargetIsDebugable;}
};

class CProjTypeWin32ALPHACon : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeWin32ALPHACon)

public:
	CProjTypeWin32ALPHACon()
		: CProjType(szAddOnx86, BCID_ProjType_AlphaCon, /* id */
					IDS_WIN32CON_PROJTYPE, CProjType::consoleapp, /* props */
					szAddOnx86, BCID_Platform_Alpha) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD blc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {return ImageExe | SubsystemConsole | TargetIsDebugable;}
	UINT GetUseMFCDefault() {return NoUseMFC;}
};

class CProjTypeWin32ALPHALib : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeWin32ALPHALib)

public:
	CProjTypeWin32ALPHALib()
		: CProjType(szAddOnx86, BCID_ProjType_AlphaLib, /* id */
					IDS_WIN32LIB_PROJTYPE, CProjType::staticlib, /* props */
					szAddOnx86, BCID_Platform_Alpha) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD blc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {return ImageStaticLib | TargetIsDebugable;}
};

#endif // _INCLUDE_PLATFORM_ALF_H
