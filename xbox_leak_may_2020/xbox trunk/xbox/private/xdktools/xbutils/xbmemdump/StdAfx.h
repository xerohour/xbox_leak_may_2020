// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__987EC98E_EFAB_4588_B008_88EB1090D306__INCLUDED_)
#define AFX_STDAFX_H__987EC98E_EFAB_4588_B008_88EB1090D306__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(_X86_)
#define _X86_
#endif

#ifndef _XBOX
#define _XBOX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <ntos.h>
#include <nturtl.h>
#include <mi.h>
#include <heap.h>
#include <xboxverp.h>

#include <stdio.h>
#include <stdlib.h>
#include <crtdbg.h>
#include <windows.h>
#include <xboxdbg.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__987EC98E_EFAB_4588_B008_88EB1090D306__INCLUDED_)
