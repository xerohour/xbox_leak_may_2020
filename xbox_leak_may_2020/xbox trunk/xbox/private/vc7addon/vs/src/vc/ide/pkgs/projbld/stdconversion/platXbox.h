//
// Xbox Platform
//
//
						
#pragma once

#include "xbox.h"

// declare out platform
extern	PlatformInfo	g_PlatformInfoXbox;

// declare our project types
class CProjTypeXboxExe : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeXboxExe)

public:
	CProjTypeXboxExe()
		: CProjType(szAddOnx86, BCID_ProjType_XboxExe, /* id */
					IDS_XBOXEXE_PROJTYPE, CProjType::application, /* props */
					szAddOnx86, BCID_Platform_Xbox) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {  return ImageExe | SubsystemWindows | TargetIsDebugable; }
};

class CProjTypeXboxLib : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeXboxLib)

public:
	CProjTypeXboxLib()
		: CProjType(szAddOnx86, BCID_ProjType_XboxLib, /* id */
					IDS_XBOXLIB_PROJTYPE, CProjType::staticlib, /* props */
  					szAddOnx86, BCID_Platform_Xbox) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	BOOL GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption);	
	int GetAttributes () {  return ImageStaticLib | TargetIsDebugable; }
};
