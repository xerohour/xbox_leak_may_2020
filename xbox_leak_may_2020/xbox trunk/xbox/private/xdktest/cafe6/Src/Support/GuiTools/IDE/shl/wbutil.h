///////////////////////////////////////////////////////////////////////////////
//  WBUTIL.H
//
//  Created by :            Date :
//      DavidGa                 12/2/93
//
//  Description :
//      Declaration of workbench-specific global utility functions
//

#ifndef __WBUTIL_H__
#define __WBUTIL_H__

#include "Strings.h"

#include "shlxprt.h"

///////////////////////////////////////////////////////////////////////////////
//  Global utility functions

SHL_API CString GetLocString(UINT id, int nLang = -1);

extern CString target_platform_str[];

typedef enum {PLATFORM_WIN32_X86 	= 0x00000001,
			  PLATFORM_WIN32S_X86 	= 0x00000002,
			  PLATFORM_MAC_68K   	= 0x00000004,
			  PLATFORM_MAC_PPC		= 0x00000008, 
			  PLATFORM_WIN32_MIPS	= 0x00000010,
			  PLATFORM_WIN32_ALPHA	= 0x00000020,
			  PLATFORM_WIN32_PPC	= 0x00000040,
			  // xbox-specific
			  PLATFORM_XBOX			= 0x00000080,
			  // add new platforms above this line
			  PLATFORM_CURRENT		= 0x20000000,
			  PLATFORM_USER			= 0x40000000,
			  PLATFORM_NON_USER		= 0x80000000} PLATFORM_TYPE;

SHL_API int GetUserTargetPlatforms(void);

#endif //__WBUTIL_H__
