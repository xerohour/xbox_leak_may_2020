// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//		are changed infrequently
//

#ifndef _WIN32
#define AFX_EXT_DATA	 __based(__segname("_DATA"))
#define AFX_EXT_DATADEF AFX_APP_DATA
#endif

#if _MSC_VER >= 800
#pragma warning(disable:4200) // (C8) 'zero sized array in struct/union'
#pragma warning(disable:4062) // (C8) 'enumerate in switch not handled'
#pragma warning(disable:4065) // (C8) 'switch statement contains only default'
#endif

#ifdef _NTWIN
#pragma warning(disable:4165) // '__stdcall function can't take variable number of arguments'
#endif

#include <guiddef.h>
#define OEMRESOURCE // to get oem bitmap id's
#include <afxwin.h>
#include <afxext.h>
#include <afxole.h>
#include <afxpriv.h>
#include <afxctl.h>
#ifdef _WIN32
#include <tchar.h>
#else
#include <ctype.h>
#endif

extern "C" {
#include "limits.h"
};

#include <cmpmgr.h>
#include <docobj.h>
#include <oleipc.h>
#include "oleimpl2.h"	// where COleFrameHook is implemented, need to negotiate w/ the MFC group to move this 
						// class to Afxole.h.  InPlace Component implementation CIPCompFrameHook needs to 
						// derive from this class.
#include "oleref.h"

#include <html_old.h>

#include "main.h"
#include "ids.h"
#include "slob.h"
#include "shlmenu.h"
#include "shlbar.h"
#include "shldocs.h"
#include "shlfutr_.h"
#include "shlsrvc.h"
#include "cmdtable.h"
#include "util.h"
#include "utilbld_.h"
#include "utilctrl.h"
#include "package.h"
#include "path.h"

#include "mainfrm.h"
#include "cmdcache.h"
#include "dockman.h"
#include "dockwnds.h"
#include "barcust.h"
#include "sheet.h"
#include "proppage.h"
#include "dlgbase.h"

#pragma warning(default:4242)

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA
