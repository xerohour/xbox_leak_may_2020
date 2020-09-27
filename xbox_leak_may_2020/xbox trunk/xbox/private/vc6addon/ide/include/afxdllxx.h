// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1995 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

/////////////////////////////////////////////////////////////////////////////
// AFXDLLX.H: Extra header for building an MFC Extension DLL
//
// This file is really a source file that you should include in the
// main source file of your DLL.  It must only be included once, and
// not multiple times (you will get linker errors if it is included
// multiple times).  If you do not use _AFXEXT, it is not required
// but you may want the feature it provides.
//
// Previous versions of 32-bit MFC did not require this file.  This version
// requires this file to support dynamic loading of extension DLLs.  In
// other words, if your application does LoadLibrary on any extension
// DLL (instead of binding to the DLL at link time), this file is
// required.

#ifndef VERSION_CHECK_ONLY
#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif
#ifndef _AFX_FULLTYPEINFO
#pragma component(mintypeinfo, on)
#endif

// The following symbol used to force inclusion of this module for _AFXEXT
#if defined(_X86_) || defined(_MAC)
extern "C" { int _afxForceEXTDLL; }
#else
extern "C" { int __afxForceEXTDLL; }
#endif

/////////////////////////////////////////////////////////////////////////////
// RawDllMain that saves current app class list and factory list

extern "C" BOOL WINAPI ExtRawDllMain(HINSTANCE, DWORD dwReason, LPVOID);

// modification from original: below line commented out so package RawDllMain called
// extern "C" BOOL (WINAPI* _pRawDllMain)(HINSTANCE, DWORD, LPVOID) = &ExtRawDllMain;

extern "C"
BOOL WINAPI ExtRawDllMain(HINSTANCE, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		// save critical data pointers before running the constructors
		AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
		pModuleState->m_pClassInit = pModuleState->m_classList;
		pModuleState->m_pFactoryInit = pModuleState->m_factoryList;
		pModuleState->m_classList.m_pHead = NULL;
		pModuleState->m_factoryList.m_pHead = NULL;
	}
	return TRUE;    // ok
}

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif
#ifndef _AFX_FULLTYPEINFO
#pragma component(mintypeinfo, off)
#endif
#endif // !VERSION_CHECK_ONLY

/////////////////////////////////////////////////////////////////////////////
// Package version checking

#ifdef _M_IX86
#ifdef _DEBUG
#define PACKVFILENAME "i386\pkgverd.h"      // Generated at shell compile-time
#else
#define PACKVFILENAME "i386\pkgver.h"       // Generated at shell compile-time
#endif
#elif defined(_M_MRX000)
#ifdef _DEBUG
#define PACKVFILENAME "mips\pkgverd.h"
#else
#define PACKVFILENAME "mips\pkgver.h"
#endif
#elif defined(_M_PPC)
#ifdef _DEBUG
#define PACKVFILENAME "ppc\pkgverd.h"
#else
#define PACKVFILENAME "ppc\pkgver.h"
#endif
#elif defined(_M_ALPHA)
#ifdef _DEBUG
#define PACKVFILENAME "alpha\pkgverd.h"
#else
#define PACKVFILENAME "alpha\pkgver.h"
#endif
#else
#error Unsupported Platform
#endif

#include PACKVFILENAME

/////////////////////////////////////////////////////////////////////////////
