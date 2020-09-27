#include "stdafx.h"
#include "PlatformXbox.h"
#include "projwriter.h"
#include "regscan.h"
#include "profile.h"
// tool includes
#include "bsctool.h"
#include "bldeventtool.h"
#include "cltool.h"
#include "custombuildtool.h"
#include "XboxImageTool.h"
#include "XboxDeployTool.h"
#include "libtool.h"
#include "linktool.h"
#include "midltool.h"
#include "nmaketool.h"
#include "rctool.h"
#include "sdltool.h"
#include "util2.h"

const wchar_t* const PLATFORM_NAME_XBOX  = L"Xbox";

CVCStringWList CPlatformXbox::s_strToolsExcludeList[5];

/////////////////////////////////////////////////////////////////////////////
//

STDMETHODIMP CPlatformXbox::Initialize( void )
{
	// create instance each of the tools for this platform
	// an add each of them to the tool collection
	HRESULT hr;

	CComBSTR bstrMainKey;
	CVCProjectEngine::GetBasePlatformInfoKey(&bstrMainKey);
	bstrMainKey.Append(L"\\");
	bstrMainKey.Append(PLATFORM_NAME_XBOX);

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

#if 0

	// VCMIDLTool
	hr = CVCMidlTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szMidlToolShortName);

#endif

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

#if 0

	// VCWebServiceProxyGeneratorTool
	hr = CVCWebServiceProxyGeneratorTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szWebToolShortName);

	// VCWebDeploymentTool
	hr = CVCWebDeploymentTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szDeployToolShortName);

#endif

	// CXboxDeploymentTool
	hr = CXboxDeploymentTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szXboxDeployToolShortName);

	// CXboxImageTool
	hr = CXboxImageTool::CreateInstance( NULL, &pTool );
	ADD_TOOL(hr, pTool, m_rgTools, bstrExtKey, szXboxImageToolShortName);

	// get the environment variables for the platform parameters

	// Why isn't this set in the rgs script? Ah well

	CComBSTR bstrXDK;
	VCGetEnvironmentVariableW(L"XDK", &bstrXDK);

	// val 'Path Dirs' = s '%XDK%\Xbox\Bin\VC7;%XDK%\Xbox\Bin;%PATH%'


	CComBSTR bstrPath;
	VCGetEnvironmentVariableW(L"PATH", &bstrPath);
	CComBSTR bstrPath2;
	bstrPath2.Append(bstrXDK);
	bstrPath2.Append("\\Xbox\\Bin\\VC7;");
	bstrPath2.Append(bstrXDK);
	bstrPath2.Append("\\Xbox\\Bin;");
	bstrPath2.Append(bstrPath);
	put_ExecutableDirectories(bstrPath2);

	// val 'Include Dirs' = s '%XDK%\Xbox\Include'

	// CComBSTR bstrInc;
	// VCGetEnvironmentVariableW(L"INCLUDE", &bstrInc);
	CComBSTR bstrInc2;
	bstrInc2.Append(bstrXDK);
	bstrInc2.Append("\\Xbox\\Include");
	put_IncludeDirectories(bstrInc2);

	CComBSTR bstrRef;
	VCGetEnvironmentVariableW(L"LIBPATH", &bstrRef);
	put_ReferenceDirectories(bstrRef);

	// val 'Library Dirs' = s '%XDK%\Xbox\Lib'

	// CComBSTR bstrLib;
	// VCGetEnvironmentVariableW(L"LIB", &bstrLib);
	CComBSTR bstrLib2;
	bstrLib2.Append(bstrXDK);
	bstrLib2.Append("\\Xbox\\Lib");
	put_LibraryDirectories(bstrLib2);

	// val 'Source Dirs' = s '%XDK%\Source'

	// CComBSTR bstrSrc;
	// VCGetEnvironmentVariableW(L"SOURCE", &bstrSrc);
	CComBSTR bstrSrc2;
	bstrSrc2.Append(bstrXDK);
	bstrSrc2.Append("\\Source");
	put_SourceDirectories(bstrSrc2);

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
		Xbox (with default prop = guid)
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

STDMETHODIMP CPlatformXbox::SaveObject(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent)
{
	// Name
	NodeAttribute( xml, CComBSTR( L"Name" ), CComBSTR( PLATFORM_NAME_XBOX ) );
	
	// end of attributes, start children
	EndNodeHeader( xml, false );

	return S_OK;
}

STDMETHODIMP CPlatformXbox::MatchName(BSTR bstrNameToMatch, VARIANT_BOOL bFullOnly, VARIANT_BOOL *pbMatched)
{
	CHECK_POINTER_VALID(pbMatched);
	*pbMatched = VARIANT_FALSE;
	if( bstrNameToMatch && _wcsicmp( PLATFORM_NAME_XBOX, bstrNameToMatch ) == 0 )
	{
		*pbMatched = VARIANT_TRUE;
	}
	return S_OK;
}

STDMETHODIMP CPlatformXbox::get_Name(/*[out, retval]*/ BSTR *pVal)
{
	CComBSTR bstrName( PLATFORM_NAME_XBOX );
	*pVal = bstrName.Detach();
	return S_OK;
}

// IVCPlatformImpl
STDMETHODIMP CPlatformXbox::IsToolInToolset(toolSetType listStyle, IVCToolImpl* pTool, VARIANT_BOOL* pbInToolset)
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
