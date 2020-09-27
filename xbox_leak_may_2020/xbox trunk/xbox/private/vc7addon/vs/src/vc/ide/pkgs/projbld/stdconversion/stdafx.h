// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__D43DAED5_3FCF_11D3_8D52_00C04F8ECDB6__INCLUDED_)
#define AFX_STDAFX_H__D43DAED5_3FCF_11D3_8D52_00C04F8ECDB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#include <afxwin.h>
#include <afxdisp.h>

#include "resource.h"
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <afxole.h> // mfc's ole stuff

#include <vcpb2.h> // for new build project stuff
#include <VCProjectPrivateTypelib.h>

#include "vsassert.h"

#include <vccoll.h>
#include <vccolls.h>
#include <path.h> // vc\ide\include\path.h
#include <slob.h>

// this has to be included here because projprop.h #defines one of the member
// names and hoses us if we include it afterwards
#include "dte.h"

#include "projbld2.h"
#include "projprop.h"
#include "util.h"
#include "prjconfg.h"
#include "bldiface.h"

// type of dependency
#define DEP_UserDefined	0x7		// Never scanned or modified except by user.

extern "C" {
#include "limits.h"
};
				    
#include "resource.h"
#include "projtool.h"
#include "intelx86.h"

extern CComModule _Module;

// transport layer information flags
#define TFL_NIL		0x0000			// Nil value
#define TFL_LOCAL	0x0001			// Local TL (else remote)
#define TFL_CFGABLE	0x0002			// Configurable (else not)

#define CHECK_POINTER_NULL(pObj) \
	if ((pObj) == NULL) \
		return E_POINTER;

#define RETURN_ON_FAIL_OR_NULL(hr, pObj) \
	if (FAILED(hr)) \
		return hr; \
	else if ((pObj) == NULL) \
		return E_FAIL;

#define RETURN_ON_FAIL_OR_NULL2(hr1, pObj, hr2) \
	if (FAILED(hr1)) \
		return hr2; \
	else if ((pObj) == NULL) \
		return hr2;

#define RETURN_ON_FAIL(hr) \
	if (FAILED(hr)) \
		return hr;

#define RETURN_INVALID_ON_NULL(pObj) \
	if ((pObj) == NULL) \
	{ \
		/* VSASSERT(FALSE, "Returning E_INVALIDARG!"); */ \
		return E_INVALIDARG; \
	}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__D43DAED5_3FCF_11D3_8D52_00C04F8ECDB6__INCLUDED)
