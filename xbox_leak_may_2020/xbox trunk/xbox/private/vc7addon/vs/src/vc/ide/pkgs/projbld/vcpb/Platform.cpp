#include "stdafx.h"
#include "Platform.h"
#include "projwriter.h"
#include "regscan.h"
#include "profile.h"
// tool includes
#include "bsctool.h"
#include "bldeventtool.h"
#include "cltool.h"
#include "custombuildtool.h"
#include "DeployTool.h"
#include "libtool.h"
#include "linktool.h"
#include "midltool.h"
#include "nmaketool.h"
#include "rctool.h"
#include "sdltool.h"
#include "util2.h"

const wchar_t* const PLATFORM_NAME_WIN32 = L"Win32";
const wchar_t* const PLATFORM_NAME_WIN64 = L"Win64";

CVCStringWList CPlatformWin32::s_strToolsExcludeList[5];
CVCStringWList CPlatformWin64::s_strToolsExcludeList[5];

/////////////////////////////////////////////////////////////////////////////
//

STDMETHODIMP CPlatformWin32::Initialize( void )
{
	// create instance each of the tools for this platform
	// an add each of them to the tool collection
	HRESULT hr;

	CComBSTR bstrMainKey;
	CVCProjectEngine::GetBasePlatformInfoKey(&bstrMainKey);
	bstrMainKey.Append(L"\\");
	bstrMainKey.Append(PLATFORM_NAME_WIN32);

	CComBSTR bstrExtKey;
	bstrExtKey = bstrMainKey + L"\\ToolDefaultExtensionLists";

	CComPtr<IVCToolImpl> pTool;
	// VCBscMakeTool
	hr = CVCBscMakeTool::CreateInstance( NULL, &pTool);
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szBscMakeToolShortName);

	// VCCLCompilerTool
	hr = CVCCLCompilerTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szCLCompilerToolShortName);

	// VCCustomBuildTool
	hr = CVCCustomBuildTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szCustomBuildToolShortName);

	// VCLibrarianTool
	hr = CVCLibrarianTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szLibrarianToolShortName);

	// VCLinkerTool
	hr = CVCLinkerTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szLinkerToolShortName);

	// VCMIDLTool
	hr = CVCMidlTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szMidlToolShortName);

	// VCNMakeTool
	hr = CVCNMakeTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szNMakeToolShortName);

	// VCPostBuildEventTool
	hr = CVCPostBuildEventTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szPostBuildEventToolShortName);

	// VCPreBuildEventTool
	hr = CVCPreBuildEventTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szPreBuildEventToolShortName);

	// VCPreLinkEventTool
	hr = CVCPreLinkEventTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szPreLinkEventToolShortName);

	// VCResourceCompilerTool
	hr = CVCResourceCompilerTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szResourceCompilerToolShortName);

	// VCWebServiceProxyGeneratorTool
	hr = CVCWebServiceProxyGeneratorTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szWebToolShortName);

	// VCWebDeploymentTool
	hr = CVCWebDeploymentTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szDeployToolShortName);

	// get the environment variables for the platform parameters
	CComBSTR bstrPath;
	VCGetEnvironmentVariableW(L"PATH", &bstrPath);
	put_ExecutableDirectories(bstrPath);

	CComBSTR bstrInc;
	VCGetEnvironmentVariableW(L"INCLUDE", &bstrInc);
	put_IncludeDirectories(bstrInc);

	CComBSTR bstrRef;
	VCGetEnvironmentVariableW(L"LIBPATH", &bstrRef);
	put_ReferenceDirectories(bstrRef);

	CComBSTR bstrLib;
	VCGetEnvironmentVariableW(L"LIB", &bstrLib);
	put_LibraryDirectories(bstrLib);

	CComBSTR bstrSrc;
	VCGetEnvironmentVariableW(L"SOURCE", &bstrSrc);
	put_SourceDirectories(bstrSrc);

	// These are set up as exclusion lists so that we don't have to special case any user-defined tools later.
	// Also makes for a shorter hunt through the list for the most common cases to see if we should be adding
	// the tool or not.  :-)

	s_strToolsExcludeList[toolSetUtility].AddTail(szBscMakeToolShortName);
	s_strToolsExcludeList[toolSetUtility].AddTail(szResourceCompilerToolShortName);
	s_strToolsExcludeList[toolSetUtility].AddTail(szCLCompilerToolShortName);
	s_strToolsExcludeList[toolSetUtility].AddTail(szPreLinkEventToolShortName);
	s_strToolsExcludeList[toolSetUtility].AddTail(szLinkerToolShortName);
	s_strToolsExcludeList[toolSetUtility].AddTail(szLibrarianToolShortName);
 	s_strToolsExcludeList[toolSetUtility].AddTail(szNMakeToolShortName);
 	s_strToolsExcludeList[toolSetUtility].AddTail(szWebToolShortName);
 	s_strToolsExcludeList[toolSetUtility].AddTail(szDeployToolShortName);

	s_strToolsExcludeList[toolSetMakefile].AddTail(szBscMakeToolShortName);
	s_strToolsExcludeList[toolSetMakefile].AddTail(szMidlToolShortName);
	s_strToolsExcludeList[toolSetMakefile].AddTail(szResourceCompilerToolShortName);
	s_strToolsExcludeList[toolSetMakefile].AddTail(szCLCompilerToolShortName);
	s_strToolsExcludeList[toolSetMakefile].AddTail(szPreLinkEventToolShortName);
	s_strToolsExcludeList[toolSetMakefile].AddTail(szLinkerToolShortName);
	s_strToolsExcludeList[toolSetMakefile].AddTail(szLibrarianToolShortName);
	s_strToolsExcludeList[toolSetMakefile].AddTail(szPreBuildEventToolShortName);
	s_strToolsExcludeList[toolSetMakefile].AddTail(szPostBuildEventToolShortName);
	s_strToolsExcludeList[toolSetMakefile].AddTail(szCustomBuildToolShortName);
 	s_strToolsExcludeList[toolSetMakefile].AddTail(szWebToolShortName);
 	s_strToolsExcludeList[toolSetMakefile].AddTail(szDeployToolShortName);

	s_strToolsExcludeList[toolSetLinker].AddTail(szLibrarianToolShortName);
 	s_strToolsExcludeList[toolSetLinker].AddTail(szNMakeToolShortName);

	s_strToolsExcludeList[toolSetLibrarian].AddTail(szLinkerToolShortName);
 	s_strToolsExcludeList[toolSetLibrarian].AddTail(szNMakeToolShortName);
 	s_strToolsExcludeList[toolSetLibrarian].AddTail(szDeployToolShortName);

	// nothing whatsoever is excluded for toolSetAll

	// now, let's add any vendor tools that have been registered with us
	/*
	Registry looks something like this:
	HKEY_LOCAL_MACHINE\ ... \VC_OBJECTS_PLATFORM_INFO
		Win32 (with default prop = guid)
			directories
				<inc, exe, lib directories>
			Vendor Tools
				vendor tool short name (with default prop = guid)
					<any excluded configs>
	*/

	if( UsingRegistry() )
	{
		CComBSTR bstrToolsSubKey = bstrMainKey;
		bstrToolsSubKey += L"\\VendorTools";

		CRegistryScanner RegScannerTools;
		if (!RegScannerTools.Open(HKEY_LOCAL_MACHINE, bstrToolsSubKey))
			return S_OK;	// nothing to do if no reg key
	
	
		// Now Add In VendorTools
		CComQIPtr<IVCProjectEngineImpl> spProjEngineImpl;
		spProjEngineImpl = g_pProjectEngine;
		if (spProjEngineImpl == NULL)
			return E_UNEXPECTED;

		CStringW strToolBaseKey;
		strToolBaseKey = bstrToolsSubKey;
		strToolBaseKey += L"\\";

		CComPtr<IDispatch> spDispTools;
		CComQIPtr<IVCCollection> spVendorTools;
		hr = spProjEngineImpl->get_VendorTools(&spDispTools);
		spVendorTools = spDispTools;
		RETURN_ON_FAIL_OR_NULL2(hr, spVendorTools, E_UNEXPECTED);

		CComBSTR bstrToolName;
		while (RegScannerTools.EnumKey(&bstrToolName))

		{	
			CComPtr<IDispatch> spDispBaseTool;
			hr = spVendorTools->Item(CComVariant(bstrToolName), &spDispBaseTool);
			if (spDispBaseTool == NULL)	// not already present
			{
				CComBSTR bstrToolGUID;
				if (!RegScannerTools.GetValue(L"\0", &bstrToolGUID))
					return E_UNEXPECTED;
				hr = spProjEngineImpl->AddVendorTool(bstrToolName, bstrToolGUID, &spDispBaseTool);
			}
			RETURN_ON_NULL2(spDispBaseTool, E_UNEXPECTED);
	
			pTool = spDispBaseTool;
			ADD_TOOL2(hr, pTool, m_rgTools);
	
			CStringW strKey = strToolBaseKey;
			strKey += bstrToolName;

			s_strToolsExcludeList[toolSetMakefile].AddTail(bstrToolName);	// nobody gets to add tools here - everything excluded.

			CComBSTR bstrVal;
			hr = GetRegStringW(strKey, L"Include_toolSetUtility", &bstrVal);
			if ( hr == S_OK )	// this is an inclusion element, so element needs to be PRESENT to not be excluded
				s_strToolsExcludeList[toolSetUtility].AddTail(bstrToolName);

			bstrVal.Empty();
			hr = GetRegStringW(strKey, L"Exclude_toolSetLinker", &bstrVal);
			if ( hr == S_OK )	// this is an inclusion element, so element needs to be PRESENT to not be excluded
				s_strToolsExcludeList[toolSetLinker].AddTail(bstrToolName);

			bstrVal.Empty();
			hr = GetRegStringW(strKey, L"Exclude_toolSetLibrarian", &bstrVal);
			if ( hr == S_OK )	// this is an inclusion element, so element needs to be PRESENT to not be excluded
				s_strToolsExcludeList[toolSetLibrarian].AddTail(bstrToolName);
		}
	}
	return S_OK;
}

STDMETHODIMP CPlatformWin32::SaveObject(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent)
{
	// Name
	NodeAttribute( xml, CComBSTR( L"Name" ), CComBSTR( PLATFORM_NAME_WIN32 ) );
	
	// end of attributes, start children
	EndNodeHeader( xml, false );

	return S_OK;
}

STDMETHODIMP CPlatformWin32::MatchName(BSTR bstrNameToMatch, VARIANT_BOOL bFullOnly, VARIANT_BOOL *pbMatched)
{
	CHECK_POINTER_VALID(pbMatched);
	*pbMatched = VARIANT_FALSE;
	if( bstrNameToMatch && _wcsicmp( PLATFORM_NAME_WIN32, bstrNameToMatch ) == 0 )
	{
		*pbMatched = VARIANT_TRUE;
	}
	return S_OK;
}

STDMETHODIMP CPlatformWin32::get_Name(/*[out, retval]*/ BSTR *pVal)
{
	CComBSTR bstrName( PLATFORM_NAME_WIN32 );
	*pVal = bstrName.Detach();
	return S_OK;
}

// IVCPlatformImpl
STDMETHODIMP CPlatformWin32::IsToolInToolset(toolSetType listStyle, IVCToolImpl* pTool, VARIANT_BOOL* pbInToolset)
{
	CHECK_POINTER_NULL(pbInToolset);
	*pbInToolset = VARIANT_FALSE;

	if (!pTool)
		return S_FALSE;

	CComBSTR bstrToolName;
	if (FAILED(pTool->get_ToolShortName(&bstrToolName)))
	{
		VSASSERT(FALSE, "*All* tools are supposed to have a short name.");
		return S_FALSE;
	}

	CStringW strToolName = bstrToolName;
	if (strToolName.IsEmpty())
	{
		VSASSERT(FALSE, "*All* tools are supposed to have a short name.");
		return S_FALSE;
	}

	*pbInToolset = (!s_strToolsExcludeList[listStyle].Find(strToolName));
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//

STDMETHODIMP CPlatformWin64::Initialize( void )
{
	// create instance each of the tools for this platform
	// and add each of them to the tool collection
	HRESULT hr;
	CComPtr<IVCToolImpl> pTool;
	CComBSTR bstrMainKey;
	CVCProjectEngine::GetBasePlatformInfoKey(&bstrMainKey);
	bstrMainKey.Append(L"\\");
	bstrMainKey.Append(PLATFORM_NAME_WIN64);

	CComBSTR bstrSubKey;

	CComBSTR bstrExtKey;
	bstrExtKey = bstrMainKey + L"\\ToolDefaultExtensionLists";

	// VCBscMakeTool
	hr = CVCBscMakeTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szBscMakeToolShortName);

	// VCCLCompilerTool
	hr = CVCCLCompilerTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szCLCompilerToolShortName);

	// VCCustomBuildTool
	hr = CVCCustomBuildTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szCustomBuildToolShortName);

	// VCLibrarianTool
	hr = CVCLibrarianTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szLibrarianToolShortName);

	// VCLinkerTool
	hr = CVCLinkerTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szLinkerToolShortName);

	// VCMIDLTool
	hr = CVCMidlTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szMidlToolShortName);

	// VCNMakeTool
	hr = CVCNMakeTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szNMakeToolShortName);

	// VCPostBuildEventTool
	hr = CVCPostBuildEventTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szPostBuildEventToolShortName);

	// VCPreBuildEventTool
	hr = CVCPreBuildEventTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szPreBuildEventToolShortName);

	// VCPreLinkEventTool
	hr = CVCPreLinkEventTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szPreLinkEventToolShortName);

	// VCResourceCompilerTool
	hr = CVCResourceCompilerTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szResourceCompilerToolShortName);

	// VCWebServiceProxyGeneratorTool
	hr = CVCWebServiceProxyGeneratorTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szWebToolShortName);

	// VCWebDeploymentTool
	hr = CVCWebDeploymentTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szDeployToolShortName);

	// These are set up as exclusion lists so that we don't have to special case any user-defined tools later.
	// Also makes for a shorter hunt through the list for the most common cases to see if we should be adding
	// the tool or not.  :-)

	s_strToolsExcludeList[toolSetUtility].AddTail(szBscMakeToolShortName);
	s_strToolsExcludeList[toolSetUtility].AddTail(szResourceCompilerToolShortName);
	s_strToolsExcludeList[toolSetUtility].AddTail(szCLCompilerToolShortName);
	s_strToolsExcludeList[toolSetUtility].AddTail(szPreLinkEventToolShortName);
	s_strToolsExcludeList[toolSetUtility].AddTail(szLinkerToolShortName);
	s_strToolsExcludeList[toolSetUtility].AddTail(szLibrarianToolShortName);
 	s_strToolsExcludeList[toolSetUtility].AddTail(szNMakeToolShortName);
 	s_strToolsExcludeList[toolSetUtility].AddTail(szWebToolShortName);
 	s_strToolsExcludeList[toolSetUtility].AddTail(szDeployToolShortName);

	s_strToolsExcludeList[toolSetMakefile].AddTail(szBscMakeToolShortName);
	s_strToolsExcludeList[toolSetMakefile].AddTail(szMidlToolShortName);
	s_strToolsExcludeList[toolSetMakefile].AddTail(szResourceCompilerToolShortName);
	s_strToolsExcludeList[toolSetMakefile].AddTail(szCLCompilerToolShortName);
	s_strToolsExcludeList[toolSetMakefile].AddTail(szPreLinkEventToolShortName);
	s_strToolsExcludeList[toolSetMakefile].AddTail(szLinkerToolShortName);
	s_strToolsExcludeList[toolSetMakefile].AddTail(szLibrarianToolShortName);
	s_strToolsExcludeList[toolSetMakefile].AddTail(szPreBuildEventToolShortName);
	s_strToolsExcludeList[toolSetMakefile].AddTail(szPostBuildEventToolShortName);
	s_strToolsExcludeList[toolSetMakefile].AddTail(szCustomBuildToolShortName);
 	s_strToolsExcludeList[toolSetMakefile].AddTail(szWebToolShortName);
 	s_strToolsExcludeList[toolSetMakefile].AddTail(szDeployToolShortName);

	s_strToolsExcludeList[toolSetLinker].AddTail(szLibrarianToolShortName);
 	s_strToolsExcludeList[toolSetLinker].AddTail(szNMakeToolShortName);

	s_strToolsExcludeList[toolSetLibrarian].AddTail(szLinkerToolShortName);
 	s_strToolsExcludeList[toolSetLibrarian].AddTail(szNMakeToolShortName);
 	s_strToolsExcludeList[toolSetLibrarian].AddTail(szDeployToolShortName);

	// get the environment variables for the platform parameters
	// get the environment variables for the platform parameters
	CComBSTR bstrPath;
	VCGetEnvironmentVariableW(L"PATH", &bstrPath);
	put_ExecutableDirectories(bstrPath);

	CComBSTR bstrInc;
	VCGetEnvironmentVariableW(L"INCLUDE", &bstrInc);
	put_IncludeDirectories(bstrInc);

	CComBSTR bstrRef;
	VCGetEnvironmentVariableW(L"LIBPATH", &bstrRef);
	put_IncludeDirectories(bstrRef);

	CComBSTR bstrLib;
	VCGetEnvironmentVariableW(L"LIB", &bstrLib);
	put_LibraryDirectories(bstrLib);

	CComBSTR bstrSrc;
	VCGetEnvironmentVariableW(L"SOURCE", &bstrSrc);
	put_SourceDirectories(bstrSrc);

	// nothing whatsoever is excluded for toolSetAll

	// now, let's add any vendor tools that have been registered with us
	/*
	Registry looks something like this:
	HKEY_LOCAL_MACHINE\ ... \VC_OBJECTS_PLATFORM_INFO
		Win64 (with default prop = guid)
			directories
				<inc, exe, lib directories>
			Vendor Tools
				vendor tool short name (with default prop = guid)
					<any excluded configs>
	*/


	CComBSTR bstrToolsSubKey = bstrMainKey;
	bstrToolsSubKey.Append( L"\\VendorTools" );

	CRegistryScanner RegScannerTools;
	if (!RegScannerTools.Open(HKEY_LOCAL_MACHINE, bstrToolsSubKey))
		return S_OK;	// nothing to do if no reg key

	CComQIPtr<IVCProjectEngineImpl> spProjEngineImpl;
	spProjEngineImpl = g_pProjectEngine;
	if (spProjEngineImpl == NULL)
		return E_UNEXPECTED;

	CStringW strToolBaseKey;
	strToolBaseKey = bstrToolsSubKey;
	strToolBaseKey += L"\\";

	CComPtr<IDispatch> spDispTools;
	CComQIPtr<IVCCollection> spVendorTools;
	hr = spProjEngineImpl->get_VendorTools(&spDispTools);
	spVendorTools = spDispTools;
	RETURN_ON_FAIL_OR_NULL2(hr, spVendorTools, E_UNEXPECTED);

	CComBSTR bstrToolName;

	while (RegScannerTools.EnumKey(&bstrToolName))
	{
		CComPtr<IDispatch> spDispBaseTool;
		hr = spVendorTools->Item(CComVariant(bstrToolName), &spDispBaseTool);
		if (spDispBaseTool == NULL)	// not already present
		{
			CComBSTR bstrToolGUID;
			if (!RegScannerTools.GetValue(L"\0", &bstrToolGUID))
				return E_UNEXPECTED;
			hr = spProjEngineImpl->AddVendorTool(bstrToolName, bstrToolGUID, &spDispBaseTool);
		}
		RETURN_ON_NULL2(spDispBaseTool, E_UNEXPECTED);

		pTool = spDispBaseTool;
		ADD_TOOL2(hr, pTool, m_rgTools);

		CStringW strKey = strToolBaseKey;
		strKey += bstrToolName;

		s_strToolsExcludeList[toolSetMakefile].AddTail(bstrToolName);	// nobody gets to add tools here - everything excluded.

		CComBSTR bstrVal;
		hr = GetRegStringW(strKey, L"Include_toolSetUtility", &bstrVal);
		if ( hr == S_OK )	// this is an inclusion element, so element needs to be PRESENT to not be excluded
			s_strToolsExcludeList[toolSetUtility].AddTail(bstrToolName);

		bstrVal.Empty();
		hr = GetRegStringW(strKey, L"Exclude_toolSetLinker", &bstrVal);
		if ( hr == S_OK )	// this is an inclusion element, so element needs to be PRESENT to not be excluded
			s_strToolsExcludeList[toolSetLinker].AddTail(bstrToolName);

		bstrVal.Empty();
		hr = GetRegStringW(strKey, L"Exclude_toolSetLibrarian", &bstrVal);
		if ( hr == S_OK )	// this is an inclusion element, so element needs to be PRESENT to not be excluded
			s_strToolsExcludeList[toolSetLibrarian].AddTail(bstrToolName);
	}
	return S_OK;
}

STDMETHODIMP CPlatformWin64::SaveObject(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent)
{
	// Name
	NodeAttribute( xml, CComBSTR( L"Name" ), CComBSTR( PLATFORM_NAME_WIN64 ) );
	
	// end of attributes, start children
	EndNodeHeader( xml, false );

	return S_OK;
}

STDMETHODIMP CPlatformWin64::MatchName(BSTR bstrNameToMatch, VARIANT_BOOL bFullOnly, VARIANT_BOOL *pbMatched)
{
	CHECK_POINTER_VALID(pbMatched);
	*pbMatched = VARIANT_FALSE;
	if( bstrNameToMatch && _wcsicmp( PLATFORM_NAME_WIN64, bstrNameToMatch ) == 0 )
	{
		*pbMatched = VARIANT_TRUE;
	}
	return S_OK;
}

STDMETHODIMP CPlatformWin64::get_Name(/*[out, retval]*/ BSTR *pVal)
{
	CComBSTR bstrName( PLATFORM_NAME_WIN64 );
	*pVal = bstrName.Detach();
	return S_OK;
}

// IVCPlatformImpl
STDMETHODIMP CPlatformWin64::IsToolInToolset(toolSetType listStyle, IVCToolImpl* pTool, VARIANT_BOOL* pbInToolset)
{
	CHECK_POINTER_NULL(pbInToolset);
	*pbInToolset = VARIANT_FALSE;

	if (!pTool)
		return S_FALSE;

	CComBSTR bstrToolName;
	if (FAILED(pTool->get_ToolShortName(&bstrToolName)))
	{
		VSASSERT(FALSE, "*All* tools are supposed to have a short name.");
		return S_FALSE;
	}

	CStringW strToolName = bstrToolName;
	if (strToolName.IsEmpty())
	{
		VSASSERT(FALSE, "*All* tools are supposed to have a short name.");
		return S_FALSE;
	}

	*pbInToolset = (!s_strToolsExcludeList[listStyle].Find(strToolName));
	return S_OK;
}
