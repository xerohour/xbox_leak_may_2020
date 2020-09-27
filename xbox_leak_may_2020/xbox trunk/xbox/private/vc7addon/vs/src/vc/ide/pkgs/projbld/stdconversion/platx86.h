//
// Intelx86 Platform
//
// [matthewt]
//
						
#ifndef _INCLUDE_PLATFORM_X86_H
#define _INCLUDE_PLATFORM_X86_H

// declare out platform
extern	PlatformInfo	g_PlatformInfoWin32x86;

// declare our project types
class CProjTypeWin32Exe : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeWin32Exe)

public:
	CProjTypeWin32Exe()
		: CProjType(szAddOnx86, BCID_ProjType_x86Exe, /* id */
					IDS_WIN32EXE_PROJTYPE, CProjType::application, /* props */
					szAddOnx86, BCID_Platform_x86) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {  return ImageExe | SubsystemWindows | TargetIsDebugable; }
};

// declare our project types
class CProjTypeGeneric : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeGeneric)

public:
	CProjTypeGeneric()
		: CProjType(szAddOnx86, BCID_ProjType_Generic, /* id */
					IDS_GENERIC_PROJTYPE, CProjType::generic, /* props */
					szAddOnx86, BCID_Platform_x86) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();
	int GetAttributes () {  return SubsystemWindows | TargetIsDebugable; }
	BOOL GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption);	

};

class CProjTypeWin32Dll : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeWin32Dll)

public:
	CProjTypeWin32Dll()
		: CProjType(szAddOnx86, BCID_ProjType_x86Dll, /* id */
					IDS_WIN32DLL_PROJTYPE, CProjType::dynamiclib, /* props */
 					szAddOnx86, BCID_Platform_x86) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {  return ImageDLL | TargetIsDebugable; }
};

class CProjTypeWin32Con : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeWin32Con)

public:
	CProjTypeWin32Con()
 		: CProjType(szAddOnx86, BCID_ProjType_x86Con, /* id */
					IDS_WIN32CON_PROJTYPE, CProjType::consoleapp, /* props */
 					szAddOnx86, BCID_Platform_x86) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {  return ImageExe | SubsystemConsole | TargetIsDebugable; }
	UINT GetUseMFCDefault() { return NoUseMFC; }
};

class CProjTypeWin32Lib : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeWin32Lib)

public:
	CProjTypeWin32Lib()
		: CProjType(szAddOnx86, BCID_ProjType_x86Lib, /* id */
					IDS_WIN32LIB_PROJTYPE, CProjType::staticlib, /* props */
  					szAddOnx86, BCID_Platform_x86) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {  return ImageStaticLib | TargetIsDebugable; }
};

#endif // _INCLUDE_PLATFORM_X86_H
