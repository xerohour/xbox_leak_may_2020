//
// MIPS (RISC) Platform
//
// [matthewt]
//
						
#ifndef _INCLUDE_PLATFORM_MIPS_H
#define _INCLUDE_PLATFORM_MIPS_H

// declare out platform
extern	PlatformInfo	g_PlatformInfoWin32MIPS;

// declare our project types
class CProjTypeWin32MIPSExe : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeWin32MIPSExe)

public:
	CProjTypeWin32MIPSExe()
		: CProjType(szAddOnx86, BCID_ProjType_MipsExe, /* id */
					IDS_WIN32EXE_PROJTYPE, CProjType::application, /* props */
					szAddOnx86, BCID_Platform_Mips) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD blc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {return ImageExe | SubsystemWindows | TargetIsDebugable;}
};

class CProjTypeWin32MIPSDll : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeWin32MIPSDll)

public:
	CProjTypeWin32MIPSDll()
		: CProjType(szAddOnx86, BCID_ProjType_MipsDll, /* id */
					IDS_WIN32DLL_PROJTYPE, CProjType::dynamiclib, /* props */
					szAddOnx86, BCID_Platform_Mips) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD blc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {return ImageDLL | TargetIsDebugable;}
};

class CProjTypeWin32MIPSCon : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeWin32MIPSCon)

public:
	CProjTypeWin32MIPSCon()
		: CProjType(szAddOnx86, BCID_ProjType_MipsCon, /* id */
					IDS_WIN32CON_PROJTYPE, CProjType::consoleapp, /* props */
					szAddOnx86, BCID_Platform_Mips) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD blc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {return ImageExe | SubsystemConsole | TargetIsDebugable;}
	UINT GetUseMFCDefault() {return NoUseMFC;}
};

class CProjTypeWin32MIPSLib : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeWin32MIPSLib)

public:
	CProjTypeWin32MIPSLib()
		: CProjType(szAddOnx86, BCID_ProjType_MipsLib, /* id */
					IDS_WIN32LIB_PROJTYPE, CProjType::staticlib, /* props */
					szAddOnx86, BCID_Platform_Mips) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD blc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {return ImageStaticLib | TargetIsDebugable;}
};

#endif // _INCLUDE_PLATFORM_MIPS_H
