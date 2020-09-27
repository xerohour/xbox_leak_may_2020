// Microsoft Visual C++ shell library.
// Copyright (C) 1994 Microsoft Corporation,
// All rights reserved.

#ifndef __VSHELL_H__
#define __VSHELL_H__

#pragma warning(disable:4200) // (C8) 'zero sized array in struct/union'
#pragma warning(disable:4062) // (C8) 'enumerate in switch not handled'
#pragma warning(disable:4065) // (C8) 'switch statement contains only default'
#pragma warning(disable:4165) // '__stdcall function can't take variable number of arguments'

#ifdef _DEBUG
#pragma comment(lib, "devshld.lib")
#else
#pragma comment(lib, "devshl.lib")
#endif

#define OEMRESOURCE
#include <afxwin.h>
#include <afxext.h>
#include <afxole.h>
#pragma warning(disable:4200) // (C8) 'zero sized array in struct/union'
#include <afxpriv.h>
#include <tchar.h>
#include <limits.h>

#include "oleref.h"

#include "ids.h"
#include "main.h"
#include "package.h"
#include "shldocs.h"
#include "shlmenu.h"
#include "shlsrvc.h"
#include "util.h"

/////////////////////////////////////////////////////////////////////////////

#undef AFX_DATA
#define AFX_DATA AFX_CORE_DATA



#undef AFX_DATA
#define AFX_DATA

#endif // __VSHELL_H__

/////////////////////////////////////////////////////////////////////////////
