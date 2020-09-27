// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//		are changed infrequently
//

#if _MSC_VER >= 800
#pragma warning(disable:4200) // (C8) 'zero sized array in struct/union'
#pragma warning(disable:4062) // (C8) 'enumerate in switch not handled'
#pragma warning(disable:4065) // (C8) 'switch statement contains only default'
#endif

#ifdef _NTWIN
#pragma warning(disable:4165) // '__stdcall function can't take variable number of arguments'
#endif

#pragma warning(disable:4251) // dll-interface warnings...
#pragma warning(disable:4275)

#define XBCP

#ifdef _WIN32
#define _SUSHI_PROJECT
#else
#define _NOCTL3D
#endif

#define OEMRESOURCE // to get oem bitmap id's
#include <afxwin.h>
#include <afxext.h>
#include <afxole.h>
#include <afxpriv.h>
#ifdef _WIN32
#include <tchar.h>
#else
#include <ctype.h>
#endif

// Include the NT image file defines
#include <macimage.h>

extern "C" {
#include "limits.h"
};
				    
#include "main.h"
#include "shlsrvc.h"
#include "package.h"
#include "resource.h"
#include "bldiface.h"
#include "prjconfg.h"
#include "projtool.h"
#include "xbox.h"
#include <bldrcvtr.h>
