// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//		are changed infrequently
//

#if _MSC_VER >= 800
#pragma warning(disable:4200) // (C8) 'zero sized array in struct/union'
#pragma warning(disable:4062) // (C8) 'enumerate in switch not handled'
#pragma warning(disable:4065) // (C8) 'switch statement contains only default'
#endif

#pragma warning(disable:4100) // 'unreferenced formal parameter'
#pragma warning(disable:4244) // conversion from 'int' to 'short'

#ifdef _NTWIN
#pragma warning(disable:4165) // '__stdcall function can't take variable number of arguments'
#endif

#ifdef _WIN32
#define _SUSHI_PROJECT
#else
#define _NOCTL3D
#endif

#include <guiddef.h>

// turn off min rebuild info for sure on mfc/windows stuff
#pragma component(minrebuild,off)
#define OEMRESOURCE // to get oem bitmap id's
#pragma warning(disable:4665) // 'mfc using bad template stuff'
#pragma warning(disable:4664) // 'mfc using bad template stuff'
#include <afxwin.h>
#include <afxext.h>
#include <afxole.h>
#include <afxpriv.h>
#ifdef _WIN32
#include <tchar.h>
#else
#include <ctype.h>
#endif
#pragma warning(4:4664) // 'mfc using bad template stuff: turn it back on now.'
#pragma warning(4:4665) // 'mfc using bad template stuff: turn it back on now.'

// Include the NT image file defines
#include <macimage.h>
#include <ppcimage.h>

extern "C" {
#include "limits.h"
};

#if 0
#define VB_MAKEFILES
#endif

#include "ids.h"
#include "main.h"
#include "shlbar.h"
#include "shldocs.h"
#include "shlmenu.h"
#include "shlsrvc.h"
#include "util.h"

#ifdef _DEBUG
#include "perftick.h"
#endif

// turn on min rebuild info for rest of the bld sys stuff in the pch
#pragma component(minrebuild,on)

// build system interface
#pragma message("\tPrecompiling \"bldiface.h\"...")
#include "bldiface.h"

// project configuration/component management
#pragma message("\tPrecompiling \"prjconfg.h\"...")
#include "prjconfg.h"

// tool option handlers
#pragma message("\tPrecompiling \"prjoptn.h\"...")
#include "prjoptn.h"

#pragma message("\tPrecompiling \"projpage.h\"...")
#include "projpage.h"

#pragma message("\tPrecompiling \"projitem.h\"...")
#include "projitem.h"

//#pragma message("\tPrecompiling \"projtool.h\"...")
//#include "projtool.h"

#pragma message("\tPrecompiling \"project.h\"...")
#include "project.h"

#pragma message("\tPrecompiling \"schmztl.h\"...")
#include "schmztl.h"

#pragma message("\tPrecompiling \"targitem.h\"...")
#include "targitem.h"

#pragma message("\tPrecompiling \"projdep.h\"...")
#include "projdep.h"

#include "dirmgr.h"
