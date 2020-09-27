// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B21EBCD4_880D_11D4_A058_00A0C9170060__INCLUDED_)
#define AFX_STDAFX_H__B21EBCD4_880D_11D4_A058_00A0C9170060__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// The following line is needed to include winsock2.h
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0401
#endif

#ifndef wcsicmp
#define wcsicmp _wcsicmp
#endif

#ifndef stricmp
#define stricmp _stricmp
#endif

#if defined( _XBOX )
#include <xtl.h>
#else
#define _WINSOCKAPI_  // prevent default of linking with winsock (we need winsock 2)
#include <windows.h>
#endif
#include <new.h>
#include <tchar.h>
#include <stdio.h>
#include <errno.h>
#include <process.h>
#include <eh.h>
#include <time.h>
#include <malloc.h>

#ifdef _DEBUG
#define ASSERT(f) {if (!(f)) _asm { int 3 }}
#define ASSERT_HERE {_asm { int 3 }}
#define VERIFY(f) ASSERT(f)
#else
#define ASSERT(f) ((void)0)
#define ASSERT_HERE ((void)0)
#define VERIFY(f) ((void)f)
#endif

//#define INVALID_HANDLE ((HANDLE) 0xffffffff)
#define INVALID_HANDLE INVALID_HANDLE_VALUE

#pragma warning (disable:4710) // We don't care if inline suceeds

#include "timer.h"
#include "numbers.h"
#include "mid.h"
#include "xmta.h"
#include "store.h"
#include "host.h"
#include "tree.h"
#include "factory.h"
#include "except.h"
#include "error.h"
#include "event.h"
#include "dispatch.h"
#include "parser.h"
#include "command.h"
#include "loops.h"
#include "parameter.h"
#include "testobj.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B21EBCD4_880D_11D4_A058_00A0C9170060__INCLUDED_)
