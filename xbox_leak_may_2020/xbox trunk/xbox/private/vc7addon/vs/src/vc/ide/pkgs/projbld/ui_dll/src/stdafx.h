// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__93607FA2_EEBE_11D2_8D27_00C04F8EEA30__INCLUDED_)
#define AFX_STDAFX_H__93607FA2_EEBE_11D2_8D27_00C04F8EEA30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_FREE_THREADED

#include "resource.h"
#include "../resdll/resource.h"
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

#include "vsassert.h"
#undef ASSERT
#define ASSERT(x) VSASSERT (x, "vc proj bld")

extern HRESULT DoSetErrorInfo(HRESULT hrErr, long idErr, BOOL bSetErrorInfo = TRUE);
extern HRESULT DoSetErrorInfo2(HRESULT hrErr, long idErr, LPCOLESTR szParam, BOOL bSetErrorInfo = TRUE);
extern HRESULT HandleSetErrorInfo(HRESULT hrErr, LPCOLESTR szErr);

#define CHECK_ZOMBIE(x, err) \
	if( (x) == NULL )\
		return DoSetErrorInfo(VCPROJ_E_ZOMBIE, err);

#define CHECK_POINTER_VALID(x) \
	if( (x) == NULL ){\
		return E_POINTER;} \
	else if (IsBadWritePtr((x), sizeof(*(x)))) {\
		return E_POINTER; }

#define CHECK_POINTER_NULL(pObj) \
	if ((pObj) == NULL) \
		return E_POINTER;

#define CHECK_READ_POINTER_VALID(x) \
	if ( (x) == NULL ){\
		return E_INVALIDARG; } \
	else if (IsBadReadPtr((x), sizeof (*(x)))) {\
		return E_INVALIDARG; }

#define CHECK_READ_POINTER_NULL(x) \
	if ( (x) == NULL ){\
		return E_INVALIDARG; } 

#define RETURN_ON_FAIL_OR_NULL(hr, pObj) \
	if (FAILED(hr)) \
		return hr; \
	else if ((pObj) == NULL) \
		return E_FAIL;

#define RETURN_ON_FAIL_OR_NULL2(hr1, pObj, hr2) \
	if (FAILED(hr1)) \
		return hr1; \
	else if ((pObj) == NULL) \
		return hr2;

#define RETURN_SPECIFIC_ON_FAIL_OR_NULL(hr1, pObj, hr2) \
	if (FAILED(hr1) || (pObj) == NULL) \
		return hr2;

#define RETURN_ON_FAIL(hr) \
	if (FAILED(hr)) \
		return hr;

#define RETURN_ON_FAIL2(hr1, hr2) \
	if (FAILED(hr1)) \
		return hr2;

#define RETURN_ON_NULL(pObj) \
	if ((pObj) == NULL) \
		return E_FAIL;

#define RETURN_ON_NULL2(pObj, hr) \
	if ((pObj) == NULL) \
		return hr;

#define RETURN_INVALID_ON_NULL(pObj) \
	if ((pObj) == NULL) \
	{ \
		/* VSASSERT(FALSE, "Returning E_INVALIDARG!"); */\
		return E_INVALIDARG; \
	}

#define RETURN_INVALID() \
	{ \
		/* VSASSERT(FALSE, "Hey, returning E_INVALIDARG"); */\
		return E_INVALIDARG; \
	}

#define CONTINUE_ON_FAIL_OR_NULL(hr, pObj) \
	if (FAILED(hr)) \
		continue; \
	else if ((pObj) == NULL) \
		continue;

#define CONTINUE_ON_FAIL(hr) \
	if (FAILED(hr)) \
		continue;

#ifndef DROPEFFECT
typedef  DWORD DROPEFFECT;
#endif	// DROPEFFECT

#include <atlstr.h>
#include "vcstring.h"

#include <oleauto.h>

//#undef CVCString
//#define CVCString CStringW

#include "vccolls.h"
#include "vcmap.h"
#include <vcpb2.h>
#include <VCProjectPrivateTypeLib.h>
#include <ProjBld2.h>
#include <ProjBldPrivate.h>

#include <SHLOBJ.H>
// #include "vcpbeng.h"

#include "vsshell.h"
#include "dte.h"

#define NO_MFC	// don't allow certain files to include too much (eg., utilbld.h)

#include "vstypeinfo.h"

#define PublicProjBuildTypeLibNumber 1
#define PrivateProjBuildTypeLibNumber 2

extern HINSTANCE g_hInstLocale; // hinstance of localized resources

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__93607FA2_EEBE_11D2_8D27_00C04F8EEA30__INCLUDED)
 
