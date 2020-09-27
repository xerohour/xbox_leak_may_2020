// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__9F36701B_33C6_11D3_8D52_00C04F8ECDB6__INCLUDED_)
#define AFX_STDAFX_H__9F36701B_33C6_11D3_8D52_00C04F8ECDB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_FREE_THREADED

// this shuts off all ATL TRACE messages, remove this line or change it to turn on what you want
#define ATL_TRACE_CATEGORY 0x0

#include "resource.h"
#include "..\ui_dll\resdll\resource.h"
#include <prjids.h>

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <atlctl.h>
#include <atlstr.h>

#include <wininet.h>
#include <wchar.h>
#include <vcstring.h>
#include <mlang.h>

#include <vcpb2.h>
#include <VCProjectPrivateTypeLib.h>
#include <ProjBldPrivate.h>
#include "macropicker.h"

#include <win9x.h>

#define NO_MFC	// don't allow certain files to include too much (eg., utilbld.h)

#include "filechng.h"
#include "VsCoCreate.h"
#include "VsAssert.h"
#include "objext.h"

#include "VsTypeInfo.h"

#ifndef UNICODE
#ifndef _wcsdec
__inline wchar_t * __cdecl _wcsdec(const wchar_t * _cpc1, const wchar_t * _cpc2) { return (wchar_t *)((_cpc1)>=(_cpc2) ? NULL : ((_cpc2)-1)); }
#endif	// _wcsdec
#endif

#ifndef _wccmp
__inline int __cdecl _wccmp(const wchar_t *_cpc1, const wchar_t *_cpc2) { return (int) ((*_cpc1)-(*_cpc2)); }
#endif	// _wccmp
#ifndef _wclen
__inline size_t __cdecl _wclen(const wchar_t *_cpc) { return (_cpc,1); }
#endif	// _wclen


_declspec(selectany) LPOLESTR szCLCompilerToolType = L"C/C++ Compiler Tool";
_declspec(selectany) LPOLESTR szCLCompilerToolPath = L"cl.exe";
_declspec(selectany) LPOLESTR szCLCompilerToolShortName = L"VCCLCompilerTool";
_declspec(selectany) LPOLESTR szLinkerToolType = L"Linker Tool";
_declspec(selectany) LPOLESTR szLinkerToolPath = L"link.exe";
_declspec(selectany) LPOLESTR szLinkerToolShortName = L"VCLinkerTool";
_declspec(selectany) LPOLESTR szLibrarianToolType = L"Librarian Tool";
_declspec(selectany) LPOLESTR szLibrarianToolPath = L"lib.exe";
_declspec(selectany) LPOLESTR szLibrarianToolShortName = L"VCLibrarianTool";
_declspec(selectany) LPOLESTR szMidlToolType = L"MIDL Tool";
_declspec(selectany) LPOLESTR szMidlToolPath = L"midl.exe";
_declspec(selectany) LPOLESTR szMidlToolShortName = L"VCMIDLTool";
_declspec(selectany) LPOLESTR szCustomBuildToolType = L"Custom Build Tool";
_declspec(selectany) LPOLESTR szCustomBuildToolPath = L"cmd.exe";
_declspec(selectany) LPOLESTR szCustomBuildToolShortName = L"VCCustomBuildTool";
_declspec(selectany) LPOLESTR szResourceCompilerToolType = L"Resource Compiler Tool";
_declspec(selectany) LPOLESTR szResourceCompilerToolPath = L"rc.exe";
_declspec(selectany) LPOLESTR szResourceCompilerToolShortName = L"VCResourceCompilerTool";
_declspec(selectany) LPOLESTR szBuildEventToolPath = L"cmd.exe";
_declspec(selectany) LPOLESTR szPreBuildEventToolShortName = L"VCPreBuildEventTool";
_declspec(selectany) LPOLESTR szPreBuildEventToolType = L"Pre-Build Event Tool";
_declspec(selectany) LPOLESTR szPreLinkEventToolShortName = L"VCPreLinkEventTool";
_declspec(selectany) LPOLESTR szPreLinkEventToolType = L"Pre-Link Event Tool";
_declspec(selectany) LPOLESTR szPostBuildEventToolShortName = L"VCPostBuildEventTool";
_declspec(selectany) LPOLESTR szPostBuildEventToolType = L"Post-Build Event Tool";
_declspec(selectany) LPOLESTR szBscMakeToolType = L"BscMake Tool";
_declspec(selectany) LPOLESTR szBscMakeToolPath = L"bscmake.exe";
_declspec(selectany) LPOLESTR szBscMakeToolShortName = L"VCBscMakeTool";
_declspec(selectany) LPOLESTR szNMakeToolType = L"NMake Tool";
_declspec(selectany) LPOLESTR szNMakeToolPath = L"nmake.exe";
_declspec(selectany) LPOLESTR szNMakeToolShortName = L"VCNMakeTool";
_declspec(selectany) LPOLESTR szWebToolType = L"Web Service Proxy Generator Tool";
_declspec(selectany) LPOLESTR szWebToolPathNative = L"sproxy.exe";
//_declspec(selectany) LPOLESTR szWebToolPathManaged = L"wsdl.exe /language:\"Microsoft.MCpp.MCppCodeGenerator,MCppCodeGenerator\"";
_declspec(selectany) LPOLESTR szWebToolPathManaged = L"wsdl.exe";
_declspec(selectany) LPOLESTR szWebToolShortName = L"VCWebServiceProxyGeneratorTool";
_declspec(selectany) LPOLESTR szDeployToolType = L"Web Deployment Tool";
_declspec(selectany) LPOLESTR szDeployToolPath = L"VCDeploy.exe";
_declspec(selectany) LPOLESTR szDeployToolShortName = L"VCWebDeploymentTool";
_declspec(selectany) LPOLESTR szXboxDeployToolType = L"Xbox Deployment Tool"; // Not used?
_declspec(selectany) LPOLESTR szXboxDeployToolPath = L"XboxDeploy.exe";
_declspec(selectany) LPOLESTR szXboxDeployToolShortName = L"XboxDeploymentTool";

_declspec(selectany) LPOLESTR szXboxImageToolType = L"Xbox Image Tool";
_declspec(selectany) LPOLESTR szXboxImageToolPath = L"imagebld.exe"; // Not used?
_declspec(selectany) LPOLESTR szXboxImageToolShortName = L"XboxImageTool";

_declspec(selectany) LPOLESTR szVcObjectsKey = L"VC_OBJECTS";
_declspec(selectany) LPOLESTR szPlatformRegKeyType = L"_PLATFORM_INFO";
_declspec(selectany) LPOLESTR szVCPlatformRegSubKey = L"\\VC\\VC_OBJECTS_PLATFORM_INFO";

extern BOOL IsRealProp(long id);
extern BOOL IsBuildEvent(bldActionTypes buildType);
extern HRESULT DoSetErrorInfo(HRESULT hrErr, long idErr, BOOL bReportError = TRUE);
extern HRESULT DoSetErrorInfo2(HRESULT hrErr, long idErr, LPCOLESTR szParam, BOOL bReportError = TRUE);
extern HRESULT DoSetErrorInfo3(HRESULT hrErr, long idErr, LPCOLESTR szParam1, LPCOLESTR szParam2, BOOL bReportError = TRUE);

#define CHECK_ZOMBIE(x, err) \
    if( (x) == NULL )\
	return DoSetErrorInfo(VCPROJ_E_ZOMBIE, err);

#define CHECK_POINTER_VALID(x) \
    if( (x) == NULL ){\
		VSASSERT(false, "NULL pointer passed to function");\
		return E_POINTER;} \
	else if (IsBadWritePtr((x), sizeof(*(x)))) {\
		VSASSERT(false, "Bad write pointer passed to function");\
		return E_POINTER; }

#define CHECK_POINTER_NULL(x) \
    if( (x) == NULL ){\
		VSASSERT(false, "NULL pointer passed to function");\
		return E_POINTER;}

#define CHECK_READ_POINTER_VALID(x) \
	if ( (x) == NULL ){\
		VSASSERT(false, "NULL read pointer passed to function"); \
		return E_INVALIDARG; } \
	else if (IsBadReadPtr((x), sizeof (*(x)))) {\
		VSASSERT(false, "Bad read pointer passed to function"); \
		return E_INVALIDARG; }

#define CHECK_READ_POINTER_NULL(x) \
	if ( (x) == NULL ){\
		VSASSERT(false, "NULL read pointer passed to function"); \
		return E_INVALIDARG; } 

#define CHECK_VARIANTBOOL(x) \
    if( (x) != VARIANT_TRUE && (x)!= VARIANT_FALSE )\
        return E_INVALIDARG;

#define CHECK_BOUNDS(a,b,x) \
    if( (x)<(a) || (x)>(b) )\
        return E_INVALIDARG;

#define CHECK_POSITIVE(x) \
	if ((x) < 0)\
		return E_INVALIDARG;

// prop pages can have inherit (-2) or edit(-3)
#define CHECK_POSITIVE_FOR_PROP_PAGES(x) \
	if (((x) < 0) && ((x) != -2) && ((x) != -3))\
		return E_INVALIDARG;

#define ASSERT_AND_CONTINUE_ON_NULL(ptr) \
	if ((ptr) == NULL) \
	{ \
		VSASSERT(FALSE, "NULL pointer part of collection."); \
		continue; \
	}

#define BREAK_ON_DONE(hr) \
	if (FAILED(hr) || hr == S_FALSE) \
		break;

#define ASSERT_AND_CONTINUE_ON_FAIL_OR_NULL(hr, ptr) \
	if (FAILED(hr) || (ptr) == NULL) \
	{ \
		VSASSERT(FALSE, "Collection enumeration either failed or returned NULL pointer"); \
		continue; \
	}

#define ASSERT_AND_CONTINUE_ON_FAIL(hr) \
	if (FAILED(hr)) \
	{ \
		VSASSERT(FALSE, "Collection enumeration either failed or returned NULL pointer"); \
		continue; \
	}

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

#define RETURN_INVALID_ON_NULL(pObj) \
	if ((pObj) == NULL) \
	{ \
		/* VSASSERT(FALSE, "Returning E_INVALIDARG!"); */ \
		return E_INVALIDARG; \
	}

#define RETURN_INVALID() \
	{ \
		/* VSASSERT(FALSE, "Hey, returning E_INVALIDARG"); */ \
		return E_INVALIDARG; \
	}

#define VCASSERTME(hr) \
	/* VSASSERT(hr != E_INVALIDARG, "Hey, got an error code I was watching for!"); */

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

#define PROJECT_IS_LOADED() \
	(!g_bInProjLoad && !g_bInProjClose)

#define STYLESHEET_IS_LOADED() \
	(!g_bInStyleLoad && !g_bInStyleClose)

#define CHECK_OK_TO_DIRTY(idProp) \
	if (PROJECT_IS_LOADED() && STYLESHEET_IS_LOADED() && IsRealProp(idProp)) \
	{ \
		HRESULT hr = SetDirty(VARIANT_TRUE); \
		if (hr == VCPROJ_E_PROJ_RELOADED) \
			return hr;	/* already reported error info */ \
		else if (hr != S_OK) \
			return DoSetErrorInfo(hr, IDS_ERR_PROJ_READ_ONLY); \
	}

#define CHECK_OK_TO_DIRTY2(idProp, bSetErrorInfo) \
	if (PROJECT_IS_LOADED() && STYLESHEET_IS_LOADED() && IsRealProp(idProp)) \
	{ \
		HRESULT hr = SetDirty(VARIANT_TRUE); \
		if (hr == VCPROJ_E_PROJ_RELOADED) \
			return hr;	/* already reported error info */ \
		else if (hr != S_OK) \
			return DoSetErrorInfo(hr, IDS_ERR_PROJ_READ_ONLY, bSetErrorInfo); \
	}

#define ADD_TOOL_INTERNAL(hr, pTool, rgTools) \
	{ \
		if (FAILED(hr)) \
			return hr; \
		if (!pTool) \
			return E_FAIL; \
		rgTools.Add(pTool); \
	}

#define ADD_TOOL(hr, pTool, rgTools, strPlatformKey, szToolShortName) \
	{ \
		ADD_TOOL_INTERNAL(hr, pTool, rgTools); \
		DoSetToolExtensions(pTool, strPlatformKey, szToolShortName); \
		pTool = NULL; \
	}

#define ADD_TOOL2(hr, pTool, rgTools) \
	{ \
		ADD_TOOL_INTERNAL(hr, pTool, rgTools); \
		pTool = NULL; \
	}


extern _ATL_REGMAP_ENTRY g_rm[];

#define DECLARE_VS_REGISTRY_RESOURCEID(x)\
	static HRESULT WINAPI UpdateRegistry(BOOL bRegister) {\
		return _Module.UpdateRegistryFromResource( x, bRegister, g_rm );\
	}

const int  BUCKET_MINIMUM			= 1;
const int  BUCKET_POSTBUILDEVT		= 1;
const int  BUCKET_DEPLOY			= 2;
const int  BUCKET_XBOXDEPLOY		= 2;
const int  BUCKET_POSTBUILD         = 3;
const int  BUCKET_BSC				= 4;
const int  BUCKET_XBOXIMAGE			= 5;
const int  BUCKET_LINK				= 6;
const int  BUCKET_LIB				= 6;
const int  BUCKET_PRELINK			= 7;
const int  BUCKET_RC				= 8;
const int  BUCKET_CPP				= 9;
const int  BUCKET_MIDL				= 10;
const int  BUCKET_WEB				= 11;
const int  BUCKET_CUSTOMBUILD		= 12;
const int  BUCKET_NMAKE				= 13;
const int  BUCKET_PREBUILD			= 14;
const int  BUCKET_MAXIMUM			= 14;

// order of tool display in the settings dlg
const int TOOL_DISPLAY_INDEX_CPP = 0;
const int TOOL_DISPLAY_INDEX_LINK = 1;
const int TOOL_DISPLAY_INDEX_LIB = 2;
const int TOOL_DISPLAY_INDEX_RC = 3;
const int TOOL_DISPLAY_INDEX_MIDL = 4;
const int TOOL_DISPLAY_INDEX_BSC = 5;
const int TOOL_DISPLAY_INDEX_PREBLD = 6;
const int TOOL_DISPLAY_INDEX_PRELINK = 7;
const int TOOL_DISPLAY_INDEX_POSTBLD = 8;
const int TOOL_DISPLAY_INDEX_CUSTBLD = 9;
const int TOOL_DISPLAY_INDEX_NMAKE = 10;
const int TOOL_DISPLAY_INDEX_WEB = 11;
const int TOOL_DISPLAY_INDEX_DEPLOY = 12;
const int TOOL_DISPLAY_INDEX_XBOXIMAGE = 12; // Never used at the same time as DEPLOY
const int TOOL_DISPLAY_INDEX_XBOXDEPLOY = 13;
#ifdef AARDVARK
const int TOOL_DISPLAY_INDEX_CSHARP = 13;
const int TOOL_DISPLAY_INDEX_VB = 14;
#endif // AARDVARK

#define StdEnumMin 0

#define PublicProjBuildTypeLibNumber 1
#define PrivateProjBuildTypeLibNumber 2

//#ifdef _DEBUG
//#define DIANEME_DEP_CHECK_DIAGNOSTICS 1		// this turns on diagnostic trace output
//#endif	// _DEBUG

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9F36701B_33C6_11D3_8D52_00C04F8ECDB6__INCLUDED)
