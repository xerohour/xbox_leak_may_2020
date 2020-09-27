#ifndef _BUILDNT_H_
#define _BUILDNT_H_

/*
	to compile within Visual C++'s IDE, go to
	Project/Settings/C++/Preprocessor/Preprocessor Definitions
	and add the BUILD_FROM_VC define

	and add the following include paths in Tools/Options/Directories:

		include:	\xbox\public\oak\inc
		include:	\xbox\public\sdk\inc
		include:	\xbox\private\sdk\inc
		include:	\xbox\public\sdk\inc\crt [for XBox apps]
		lib:		\xbox\public\sdk\lib\i386

	Note to self: for custom entry points like xbounce, go to
	Project/Settings/Link/Category:Output/Entry-Point Symbol
	[xbounce uses wWinMainCRTStartup]

	Note: need to remove /GZ call if default libraries are disabled

*/

#ifdef BUILD_FROM_VC

/*
	A default XBox project will have the following defines:

	Debug:   WIN32,_XBOX,_DEBUG,_WINDOWS,_MBCS
	Release: WIN32,_XBOX,NDEBUG,_WINDOWS,_MBCS
*/

/* force UNICODE */

#ifdef _MBCS
#pragma message("Undefining _MBCS macro")
#undef _MBCS
#endif

#ifndef UNICODE
#pragma message("Adding UNICODE macro")
#define UNICODE
#endif

#define i386					1
#define _X86_					1
#define STD_CALL
#define CONDITION_HANDLING		1
#define NT_UP					1
#define NT_INST					0

#ifdef WIN32
#undef WIN32
#endif
#define WIN32					100

#define _NT1X_					100
#define WINNT					1
#define _WIN32_WINNT			0x0500
#define WINVER					0x0500
#define _WIN32_IE				0x0501
#define	XBOX					1
#define WIN32_LEAN_AND_MEAN		1
#define DBG						1
#define DEVL					1
#define FPO						0
#define _MT						1
#define DEVKIT


#if NDEBUG
#define FREEBUILD 1
#else
#define FREEBUILD 0
#endif

#include "warning.h"

#else
#include <xtl.h> /* Xbox app master file */
#endif /* #ifdef BUILD_FROM_VC */

#endif /* #ifndef _BUILDNT_H_ */
