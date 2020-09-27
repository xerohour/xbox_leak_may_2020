///////////////////////////////////////////////////////////////////////////////
//	WBUTIL.CPP
//
//	Created by :			Date :
//		DavidGa					12/2/93
//
//	Description :
//		Implementation of workbench-specific global utility functions
//

#include "stdafx.h"
#include "wbutil.h"
#include "..\..\testutil.h"
#include "guiv1.h"
#include "coconnec.h"
#include "uwbframe.h"
#include "Strings.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

///////////////////////////////////////////////////////////////////////////////
//	Global utility functions
 
/*	GetLocString
 *		Global version of the WBDriver function - so anyone can call it.
 */
// BEGIN_HELP_COMMENT
// Function: CString GetLocString(UINT id, int nLang /*=-1*/)
// Description: Get a pointer to a localized string with the given id and language.
// Return: A CString that contains the localized string.
// Param: id An integer that contains the id of the string to retrieve. The list of valid ids is in wbus.rc.
// Param: nLang An integer that contains the language of the string. -1 for the current language; CAFE_LANG_ENGLISH for English; CAFE_LANG_JAPANESE for Japanese. (The default value is -1.)
// END_HELP_COMMENT
SHL_API CString GetLocString(UINT id, int nLang /*=-1*/)
{
	CString cstrLocalized;
	LoadString(GetLangDllHandle(), id, cstrLocalized.GetBufferSetLength(1024), 1024);
	cstrLocalized.ReleaseBuffer();
	return cstrLocalized;
}

// emmang 11/20/2000 - added xbox target platform
// looks like this is just for the cafe driver ui
CString target_platform_str[] = {"win32_x86", "win32s_x86",	//todo: localize
								 "mac_68k", "mac_ppc",
								 "win32_mips", "win32_alpha", "win32_ppc",
								 "xbox"};	


// BEGIN_HELP_COMMENT
// Function: int GetUserTargetPlatforms(void)	
// Description: Get the current platform.
// Return: An value indicating the current platform: PLATFORM_WIN32_X86, PLATFORM_WIN32S_X86, PLATFORM_MAC_68K, PLATFORM_MAC_PPC, PLATFORM_WIN32_MIPS, PLATFORM_WIN32_ALPHA, PLATFORM_WIN32_PPC.
// END_HELP_COMMENT
SHL_API int GetUserTargetPlatforms(void)	
	
	{
	gplatformStr.MakeLower();

	if(gplatformStr == "win32_x86")
		return PLATFORM_WIN32_X86;
	if(gplatformStr == "win32s_x86")
		return PLATFORM_WIN32S_X86;
	if(gplatformStr == "mac_68k")
		return PLATFORM_MAC_68K;
	if(gplatformStr == "mac_ppc")
		return PLATFORM_MAC_PPC;
	if(gplatformStr == "win32_mips")
		return PLATFORM_WIN32_MIPS;
	if(gplatformStr == "win32_alpha")
		return PLATFORM_WIN32_ALPHA;
	if(gplatformStr == "win32_ppc")
		return PLATFORM_WIN32_PPC;
	if(gplatformStr == "xbox")
		return PLATFORM_XBOX;

	//TODO: need some error message here (EXPECT isn't supported anymore).
	return 0;
	}
