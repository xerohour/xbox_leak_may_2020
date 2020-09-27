//
// Xbox Platform
//
// [matthewt]
//
						
#ifndef _INCLUDE_PLATFORM_XBOX_H
#define _INCLUDE_PLATFORM_XBOX_H

#include "stdafx.h"

// declare out platform
extern	PlatformInfo	g_PlatformInfoXbox;

// declare our project types
class CProjTypeXboxExe : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeXboxExe)

public:
	CProjTypeXboxExe()
		: CProjType(szAddOnXbox, BCID_ProjType_XboxExe, /* id */
					IDS_XBOXEXE_PROJTYPE, CProjType::application, /* props */
					szAddOnXbox, BCID_Platform_Xbox) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {  return ImageExe | ImageXbe | SubsystemWindows | TargetIsDebugable; }
};

#ifdef XBOXDLL
class CProjTypeXboxDll : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeXboxDll)

public:
	CProjTypeXboxDll()
		: CProjType(szAddOnXbox, BCID_ProjType_XboxDll, /* id */
					IDS_XBOXDLL_PROJTYPE, CProjType::dynamiclib, /* props */
					szAddOnXbox, BCID_Platform_Xbox) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {  return ImageExe | SubsystemWindows | TargetIsDebugable; }
};
#endif

class CProjTypeXboxLib : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeXboxLib)

public:
	CProjTypeXboxLib()
		: CProjType(szAddOnXbox, BCID_ProjType_XboxLib, /* id */
					IDS_XBOXLIB_PROJTYPE, CProjType::staticlib, /* props */
					szAddOnXbox, BCID_Platform_Xbox) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {  return ImageExe | SubsystemWindows | TargetIsDebugable; }
};

class CProjTypeXboxGeneric : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeXboxGeneric)

public:
	CProjTypeXboxGeneric()
		: CProjType(szAddOnXbox, BCID_ProjType_XboxGeneric, /* id */
					IDS_GENERIC_PROJTYPE, CProjType::generic, /* props */
					szAddOnXbox, BCID_Platform_Xbox) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();
	int GetAttributes () {  return SubsystemWindows | TargetIsDebugable; }
	BOOL GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption);	

};

#endif // _INCLUDE_PLATFORM_XBOX_H
