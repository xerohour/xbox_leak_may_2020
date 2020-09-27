//
// Merced (RISC) Platform
//
// [matthewt]
//
						
#ifndef _INCLUDE_PLATFORM_MERCED_H
#define _INCLUDE_PLATFORM_MERCED_H

#include "merced.h"

// declare out platform
extern	PlatformInfo	g_PlatformInfoWin64MERCED;

// declare our project types
// declare our project types (FOR MERCED)
class CProjTypeWin64MERCEDExe : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeWin64MERCEDExe)

public:
	CProjTypeWin64MERCEDExe()
		: CProjType(szAddOnx86, BCID_ProjType_MercedExe, /* id */
					IDS_WIN32EXE_PROJTYPE, CProjType::application, /* props */
					szAddOnx86, BCID_Platform_Merced) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD blc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {return ImageExe | SubsystemWindows | TargetIsDebugable;}
};

class CProjTypeWin64MERCEDDll : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeWin64MERCEDDll)

public:
	CProjTypeWin64MERCEDDll()
		: CProjType(szAddOnx86, BCID_ProjType_MercedDll, /* id */
					IDS_WIN32DLL_PROJTYPE, CProjType::dynamiclib, /* props */
					szAddOnx86, BCID_Platform_Merced) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD blc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {return ImageDLL | TargetIsDebugable;}
};

class CProjTypeWin64MERCEDCon : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeWin64MERCEDCon)

public:
	CProjTypeWin64MERCEDCon()
		: CProjType(szAddOnx86, BCID_ProjType_MercedCon, /* id */
					IDS_WIN32CON_PROJTYPE, CProjType::consoleapp, /* props */
					szAddOnx86, BCID_Platform_Merced) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD blc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {return ImageExe | SubsystemConsole | TargetIsDebugable;}
	UINT GetUseMFCDefault() {return NoUseMFC;}
};

class CProjTypeWin64MERCEDLib : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeWin64MERCEDLib)

public:
	CProjTypeWin64MERCEDLib()
		: CProjType(szAddOnx86, BCID_ProjType_MercedLib, /* id */
					IDS_WIN32LIB_PROJTYPE, CProjType::staticlib, /* props */
					szAddOnx86, BCID_Platform_Merced) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD blc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {return ImageStaticLib | TargetIsDebugable;}
};

#endif // _INCLUDE_PLATFORM_ALF_H
