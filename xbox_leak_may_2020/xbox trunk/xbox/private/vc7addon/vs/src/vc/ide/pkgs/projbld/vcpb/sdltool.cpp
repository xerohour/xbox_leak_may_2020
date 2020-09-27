// SdlBaseTool.cpp : Implementation of Web Service Proxy Generator Tool

#include "stdafx.h"
#include "SdlTool.h"
#include "BuildEngine.h"
#include "scanner.h"
#include "ProjWriter.h"

// constants
const wchar_t* const wszWebDefaultExtensions = L"*.sdl;*.wsdl";	// remember to update vcpb.rgs if you change this...

// static initializers
CWebToolOptionHandler CVCWebServiceProxyGeneratorTool::s_optHandler;
CComBSTR CVCWebServiceProxyGeneratorTool::s_bstrBuildDescription = L"";
CComBSTR CVCWebServiceProxyGeneratorTool::s_bstrToolName = L"";
CComBSTR CVCWebServiceProxyGeneratorTool::s_bstrExtensions = L"";
GUID CVCWebServiceProxyGeneratorTool::s_pPages[2];
BOOL CVCWebServiceProxyGeneratorTool::s_bPagesInit;

////////////////////////////////////////////////////////////////////////////////
// Option Table for Web Service Utility switches

// NOTE: we're using macros for enum value ranges to help in keeping enum range checks as low maintenance as possible.
// If you add an enum to this table, make sure you use macros to define the upper and lower bounds and use CHECK_BOUNDS
// on the get/put methods associated with that property (both object model and property page).
// WARNING: if you change ANYTHING about a multi-prop here (including add/remove), be sure to update how the property
// is obtained on both the tool and page objects.
BEGIN_OPTION_TABLE(CWebToolOptionHandler, L"VCWebServiceProxyGeneratorTool", IDS_WEBTOOL, TRUE /* pre & post */, FALSE /* case sensitive */)
	// general
	OPT_BSTR(Output,				L"out:%s",		L"Output",		single,		IDS_WEBTOOL, VCWEBID)	// Output filename
	OPT_BOOL(SuppressStartupBanner,	L"|nologo",		L"SuppressStartupBanner",	IDS_WEBTOOL, VCWEBID)
END_OPTION_TABLE()


BOOL CWebToolOptionHandler::SetEvenIfDefault(VARIANT *pvarDefault, long idOption)
{
	CComVariant varDefault;
	BOOL bRet = FALSE;

	switch (idOption)
	{
	case VCWEBID_SuppressStartupBanner:
		varDefault.vt = VT_BOOL;
		varDefault = VARIANT_TRUE;
		bRet = TRUE;
		break;
	}
	varDefault.Detach(pvarDefault);
	return bRet;
}

BOOL CWebToolOptionHandler::SynthesizeOptionIfNeeded(IVCPropertyContainer* pPropContainer, long idOption)
{
	VSASSERT(pPropContainer != NULL, "Cannot synthesize a tool option without a property container");
	RETURN_ON_NULL2(pPropContainer, FALSE);	// can't synthesize if we don't have a property container...

	switch (idOption)
	{
	case VCWEBID_Output:
		return TRUE;
	default:
		return FALSE;
	}
}

BOOL CWebToolOptionHandler::SynthesizeOption(IVCPropertyContainer* pPropContainer, long idOption, 
	CComVariant& rvar)
{
	switch (idOption)
	{
	case VCWEBID_Output:
		{
			rvar.Clear();
			VSASSERT(pPropContainer != NULL, "Cannot create an option value without a property container");
			CComBSTR bstrVal;
			GetDefaultValue( idOption, &bstrVal, pPropContainer );
			return SetBstrInVariant(pPropContainer, bstrVal, rvar);
		}
	default:
		VSASSERT(FALSE, "Case statement mismatch between SynthesizeOptionIfNeeded and SynthesizeOption");	// shouldn't be here!!
		return FALSE;
	}
}

// default value handlers
// string props
void CWebToolOptionHandler::GetDefaultValue( long id, BSTR *pVal, IVCPropertyContainer *pPropCnt )
{
	if (id == VCWEBID_Output)
	{
		BOOL bIsManaged = FALSE;
		genProxyLanguage genProxy = genProxyNative;
		if( pPropCnt )
		{
		    HRESULT hr = pPropCnt->GetIntProperty(VCWEBID_GeneratedProxyLanguage, (long *)&genProxy);
		    if( hr == S_OK && genProxy == genProxyManaged )
		    	bIsManaged = TRUE;
		}

		CComBSTR bstrOut;
		if( bIsManaged )
			bstrOut = L"$(InputName).dll";
		else
			bstrOut = L"$(InputName).h";

		*pVal = bstrOut.Detach();
	}
	else
		GetDefaultString( pVal );
}

// integer props
void CWebToolOptionHandler::GetDefaultValue( long id, long *pVal, IVCPropertyContainer *pPropCnt )
{
	*pVal = 0;
}

// boolean props
void CWebToolOptionHandler::GetDefaultValue( long id, VARIANT_BOOL *pVal, IVCPropertyContainer *pPropCnt )
{
	if ( id == VCWEBID_SuppressStartupBanner )
		GetValueTrue( pVal );
	else
		GetValueFalse( pVal );
}


///////////////////////////////////////////////////////////////////////////////
// Sdl Tool
HRESULT CVCWebServiceProxyGeneratorTool::CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppSdlTool)
{
	CHECK_POINTER_NULL(ppSdlTool);
	*ppSdlTool = NULL;

	CComObject<CVCWebServiceProxyGeneratorTool> *pObj;
	HRESULT hr = CComObject<CVCWebServiceProxyGeneratorTool>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		if( pPropContainer )
			pObj->Initialize(pPropContainer);
		CVCWebServiceProxyGeneratorTool *pVar = pObj;
		pVar->AddRef();
		*ppSdlTool = pVar;
	}
	return hr;
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::GenerateOutput(long type, IVCBuildActionList* plstActions, IVCBuildErrorContext* pEC)
{
	RETURN_ON_NULL2(plstActions, S_OK);

	BOOL bFirst = TRUE;
	CComBSTR bstrProjectDirectory;

	plstActions->Reset(NULL);
	while( true )
	{
		CComPtr<IVCBuildAction> spAction;
		HRESULT hr = plstActions->Next(&spAction, NULL);
		BREAK_ON_DONE(hr);

		CComPtr<IVCBuildableItem> spItem;
		spAction->get_Item( &spItem );
		VSASSERT( spItem != NULL, "Action created without an item.  Bad initialization." );
		CComQIPtr<IVCPropertyContainer> spPropContainer = spItem;
		VSASSERT( spPropContainer != NULL, "Action's item is not a property container.  Bad initialization." );
		RETURN_INVALID_ON_NULL(spPropContainer);

		if( IsExcludedFromBuild( spPropContainer ) )
			continue;	// nothing to do for things excluded from build...

		if( bFirst )
		{
			bFirst = FALSE;
			// get the project directory
			spPropContainer->GetStrProperty( VCPROJID_ProjectDirectory, &bstrProjectDirectory );
		}

 		CDirW dirBase;
 		CreateDirFromBSTR( bstrProjectDirectory, dirBase );
 
 		// get the output filename
 		CComBSTR bstrOutput;
 		hr = spPropContainer->GetEvaluatedStrProperty( VCWEBID_Output, &bstrOutput );
		if (hr == S_FALSE)
		{
			s_optHandler.GetDefaultValue( VCWEBID_Output, &bstrOutput, spPropContainer );
			spPropContainer->Evaluate(bstrOutput, &bstrOutput);
			hr = S_OK;
		}

		// set the output
		CPathW pathOutput;
		if( pathOutput.CreateFromDirAndFilename( dirBase, CStringW( bstrOutput ) ) )
		{
			pathOutput.GetActualCase(TRUE);
			hr = spAction->AddOutputFromPath( (wchar_t*)(const wchar_t*)pathOutput, pEC, VARIANT_TRUE, VARIANT_FALSE, 
				VCWEBID_Output, this );
			// TODO: handle errors?
			VSASSERT( hr == S_OK, "Failed to add output for sproxy command line" );
		}
	}
	return S_OK; // success
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::GetCommandLineEx(IVCBuildAction*pAction,IVCBuildableItem* pBuildableItem, 
	IVCBuildEngine* pBuildEngine, IVCBuildErrorContext* pEC, BSTR *pVal)
{
	CHECK_READ_POINTER_NULL(pAction);
	CHECK_READ_POINTER_NULL(pBuildableItem);

	CHECK_POINTER_NULL(pVal);
	*pVal = NULL;

	COptionHandlerBase* pOptionHandler = GetOptionHandler();
	RETURN_ON_NULL2(pOptionHandler, E_NOTIMPL);

	CComQIPtr<IVCPropertyContainer> spPropContainer = pBuildableItem;
	RETURN_INVALID_ON_NULL(spPropContainer);

	CComBSTR bstrProp;
	HRESULT hr = DoGetToolPath(&bstrProp, spPropContainer);	
	RETURN_ON_FAIL(hr);

	CComBSTR bstrToolExe;
	spPropContainer->Evaluate(bstrProp, &bstrToolExe);

	CStringW strOption, strCmdLine, strInput;
	strOption = bstrToolExe;
	pOptionHandler->AdvanceCommandLine(strCmdLine, strOption);

	BOOL bIsManaged = FALSE;
	genProxyLanguage genProxy = genProxyNative;
	hr = spPropContainer->GetIntProperty(VCWEBID_GeneratedProxyLanguage, (long *)&genProxy);
	if( hr == S_OK && genProxy == genProxyManaged )
	    bIsManaged = TRUE;
	

	if (bIsManaged)
	{
		CComBSTR bstrProp2;
		pAction->GetRelativePathsToInputs(TRUE, FALSE, pEC, &bstrProp2);
		strInput = bstrProp2;

    	// finish putting together the wsdl command line
		pOptionHandler->AdvanceCommandLine(strCmdLine, strOption, L" ");
		pOptionHandler->AdvanceCommandLine(strCmdLine, strInput, L" ");

		// get the output filename
		CComBSTR bstrOutput;
		hr = spPropContainer->GetEvaluatedStrProperty( VCWEBID_Output, &bstrOutput );
		if (hr == S_FALSE)
		{
		   	s_optHandler.GetDefaultValue( VCWEBID_Output, &bstrOutput, spPropContainer );
		   	spPropContainer->Evaluate(bstrOutput, &bstrOutput);
			hr = S_OK;
		}
		// get the output filename
		CComBSTR bstrAdditional;
		hr = spPropContainer->GetEvaluatedStrProperty( VCWEBID_AdditionalOptions, &bstrAdditional );

		// finish put together the rest of the batch file
		CComBSTR bstrCmdLine = L"@echo off\n";

		bstrCmdLine += L"wsdl /nologo /l:cs /out:\"$(InputName).cs\" \"";
		bstrCmdLine += strInput;
		bstrCmdLine += L"\" ";
		bstrCmdLine += bstrAdditional;
		bstrCmdLine += L"\n";

		bstrCmdLine += L"csc /t:module /nologo /o+ /debug- /out:\"";
		bstrCmdLine += bstrOutput;
		bstrCmdLine += L"\" \"$(InputName).cs\" \n";

		bstrCmdLine += L"copy \"";
		bstrCmdLine += bstrOutput;
		bstrCmdLine += L"\" \"$(OutDir)\\$(InputName).dll\"\n";

		spPropContainer->Evaluate(bstrCmdLine, &bstrCmdLine);
		
		if (pBuildEngine == NULL)
			return bstrCmdLine.CopyTo(pVal);
	
		return pBuildEngine->FormBatchFile(bstrCmdLine, pEC, pVal);
	}
	else
	{
		CComBSTR bstrProp2;
		pAction->GetRelativePathsToInputs(TRUE, FALSE, pEC, &bstrProp2);
		strInput = bstrProp2;
		pOptionHandler->AdvanceCommandLine(strCmdLine, strInput, L" ");
	
		CComBSTR bstrMidLine;
		hr = GetCommandLineOptions(pBuildableItem, NULL, VARIANT_TRUE /* include additional options */, cmdLineForBuild, &bstrMidLine);
		if (FAILED(hr))
			return hr;
		strOption = bstrMidLine;
	
		pOptionHandler->AdvanceCommandLine(strCmdLine, strOption, L" ");

		CStringW strCommand;
		strCommand.Format(IDS_CREATING_COMMANDLINE, strCmdLine);
		CComBSTR bstrCommand = strCommand;
		if( pBuildEngine )
			pBuildEngine->LogTrace(eLogCommand, bstrCommand);
		*pVal = strCmdLine.AllocSysString();
	}
	return S_OK;
}

HRESULT CVCWebServiceProxyGeneratorTool::GetPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, CPathW& rPath)
{
	if (bSchmoozeOnly)
		return S_FALSE;	// not primary project output generator

	CComQIPtr<IVCPropertyContainer> spPropContainer = pItem;
	CComQIPtr<IVCBuildableItem> spBldableItem = pItem;
	if (spPropContainer == NULL || spBldableItem == NULL)
		return S_FALSE;

	CComBSTR bstrProjectDirectory;
	HRESULT hr = spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjectDirectory);	// should never have macros in it
	VSASSERT(hr == S_OK, "How did we manage to not get a project directory?!?");
	RETURN_ON_FAIL(hr);
	CStringW strProjDir = bstrProjectDirectory;
	CDirW dirProject;
	if (!dirProject.CreateFromKnown(strProjDir))
	{
		VSASSERT(FALSE, "Invalid project directory!");
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
	}

	CComBSTR bstrOutFile;
	CStringW strInt;
	hr = spPropContainer->GetStrProperty(VCWEBID_Output, &bstrOutFile);
	RETURN_ON_FAIL(hr);
	if (hr == S_FALSE)
		s_optHandler.GetDefaultValue(VCWEBID_Output, &bstrOutFile, spPropContainer);

	CComBSTR bstrOut2;
	hr = spPropContainer->Evaluate(bstrOutFile, &bstrOut2);
	RETURN_ON_FAIL(hr);
	CStringW strOutFile = bstrOut2;

	if (!rPath.CreateFromDirAndFilename(dirProject, strOutFile))
		return S_FALSE;

	return S_OK;
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::HasPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, VARIANT_BOOL* pbHasPrimaryOutput)
{ 
	CHECK_POINTER_NULL(pbHasPrimaryOutput);
	if (bSchmoozeOnly)
		*pbHasPrimaryOutput = VARIANT_FALSE;	// not primary project output generator
	else
		*pbHasPrimaryOutput = VARIANT_TRUE;	// looking for primary *file* output, so yes, we create one

	return S_OK;
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::GetPrimaryOutputIDFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, long* pnOutputID)
{
	CHECK_POINTER_NULL(pnOutputID);
	*pnOutputID = -1;

	if (bSchmoozeOnly)
		return S_FALSE;

	*pnOutputID = VCWEBID_Output;
	return S_OK;
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::GetDefaultOutputFileFormat(IUnknown* pItem, BSTR* pbstrOutMacro)
{	// macro to use for generating default output file if the original default isn't unique
	CHECK_POINTER_NULL(pbstrOutMacro);
	CComQIPtr<IVCPropertyContainer> spPropContainer = pItem;

	CComBSTR bstrOut;
	s_optHandler.GetDefaultValue(VCWEBID_Output, &bstrOut, spPropContainer);
	CStringW strOut = bstrOut;
	int idx = strOut.Find(L'.');
	if (idx >= 0)
	{
		CStringW strLeft, strRight;
		strLeft = strOut.Left(idx-1);
		strRight = strOut.Right(strOut.GetLength()-idx+1);
		strOut = strLeft;
		strOut += L"%d";
		strOut += strRight;
		bstrOut = strOut;
	}
	*pbstrOutMacro = bstrOut.Detach();

	return S_OK;
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::SetPrimaryOutputForTool(IUnknown* pItem, BSTR bstrFile)
{	// sets the primary output for a tool
	CComQIPtr<IVCPropertyContainer> spPropContainer = pItem;
	RETURN_ON_NULL2(spPropContainer, S_FALSE);		// nothing we can do

	CStringW strFileCheck = bstrFile;
	strFileCheck.TrimLeft();
	strFileCheck.TrimRight();
	if (strFileCheck.IsEmpty())
		return S_FALSE;

	// assuming OK to dirty since we shouldn't be here unless we got called by a wizard that already checked
	return spPropContainer->SetStrProperty(VCWEBID_Output, bstrFile);
}

void CVCWebServiceProxyGeneratorTool::SaveSpecialProps(IStream *xml, IVCPropertyContainer *pPropContainer)
{
	// GeneratedProxyLanguage
	CComVariant varLang;
	if (pPropContainer->GetLocalProp(VCWEBID_GeneratedProxyLanguage, &varLang) == S_OK && 
		(varLang.vt == VT_I2 || varLang.vt == VT_I4))
	{
		OLECHAR szEnumVal[10] = {0};
		NodeAttribute( xml, L"GeneratedProxyLanguage", CComBSTR( _itow( varLang.intVal, szEnumVal, 10 ) ) );
	}

	// URL
	CComVariant varURL;
	if (pPropContainer->GetLocalProp(VCWEBID_URL, &varURL) == S_OK && varURL.vt == VT_BSTR)
		NodeAttribute( xml, L"URL", varURL.bstrVal );
}


///////////////////////////////////////////////////////////////////////////////
// Affects our output?
STDMETHODIMP CVCWebServiceProxyGeneratorTool::AffectsOutput(long nPropID, VARIANT_BOOL* pbAffectsOutput)
{
	CHECK_POINTER_NULL(pbAffectsOutput);

	// change our outputs?
	switch (nPropID)
	{
	case VCWEBID_Output:
		*pbAffectsOutput = VARIANT_TRUE;
		break;
	default:
		*pbAffectsOutput = VARIANT_FALSE;
		break;
	}

	return S_OK;
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::CreatePageObject(IUnknown **ppUnk, CLSID*pCLSID, IVCPropertyContainer *pPropCntr, IVCSettingsPage *pPage )
{
	// Get the list of pages
	if( CLSID_WebUtilityGeneral == *pCLSID )
	{
		CPageObjectImpl< CVCWebServiceGeneralPage, VCWEBTOOL_MIN_DISPID, VCWEBTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else
	{
		return S_FALSE;
	}
		
	return S_OK;
}


GUID* CVCWebServiceProxyGeneratorTool::GetPageIDs()
{ 
	if (s_bPagesInit == FALSE)
	{
		s_pPages[0] = __uuidof(WebUtilityGeneral);
		s_pPages[1] = __uuidof(WebAdditionalOptions);
		s_bPagesInit = TRUE;
	}
	return s_pPages; 
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::CanScanForDependencies(VARIANT_BOOL* pbIsScannable)
{
	return COptionHandlerBase::GetValueFalse(pbIsScannable);
}


///////////////////////////////////////////////////////////////////////////////////////
// CVCWebServiceProxyGeneratorTool::CVCWebServiceProxyGeneratorTool
///////////////////////////////////////////////////////////////////////////////////////

// general 
LPCOLESTR CVCWebServiceProxyGeneratorTool::GetToolFriendlyName()
{
	InitToolName();
	return s_bstrToolName;
}

void CVCWebServiceProxyGeneratorTool::InitToolName()
{
	if (s_bstrToolName.Length() == 0)
	{
		if (!s_bstrToolName.LoadString(IDS_WEB_TOOLNAME))
			s_bstrToolName = szWebToolType;
	}
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::get_ToolName(BSTR* pbstrToolName)
{	// friendly name of tool, e.g., "C/C++ Compiler Tool"
	CHECK_POINTER_VALID( pbstrToolName );
	InitToolName();
	s_bstrToolName.CopyTo(pbstrToolName);
	return S_OK;
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::get_Output(BSTR *pbstrOutput)
{
	return ToolGetStrProperty(VCWEBID_Output, pbstrOutput);
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::put_Output(BSTR bstrOutput)
{
	CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
	return m_spPropertyContainer->SetStrProperty( VCWEBID_Output, bstrOutput );
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::get_SuppressStartupBanner(VARIANT_BOOL* suppress)
{	// /nologo
	return ToolGetBoolProperty(VCWEBID_SuppressStartupBanner, suppress);
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::put_SuppressStartupBanner(VARIANT_BOOL suppress)
{
	CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
	CHECK_VARIANTBOOL(suppress);
	return m_spPropertyContainer->SetBoolProperty(VCWEBID_SuppressStartupBanner, suppress);
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::get_GeneratedProxyLanguage(genProxyLanguage* language)	
{	// generated proxy language: native or managed C++
	return ToolGetIntProperty(VCWEBID_GeneratedProxyLanguage, (long *)language);
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::put_GeneratedProxyLanguage(genProxyLanguage language)
{
	CHECK_BOUNDS(StdEnumMin, WebProxyMax, language);
	CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
	return m_spPropertyContainer->SetIntProperty(VCWEBID_GeneratedProxyLanguage, language);
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::get_AdditionalOptions(BSTR* options)
{	// any additional options
	return ToolGetStrProperty(VCWEBID_AdditionalOptions, options);
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::put_AdditionalOptions(BSTR options)
{
	CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
	return m_spPropertyContainer->SetStrProperty( VCWEBID_AdditionalOptions, options );
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::get_URL(BSTR* url)
{	// any additional options
	return ToolGetStrProperty(VCWEBID_URL, url);
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::put_URL(BSTR url)
{
	CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
	return m_spPropertyContainer->SetStrProperty( VCWEBID_URL, url );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// CVCWebServiceProxyGeneratorTool::IVCToolImpl
/////////////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CVCWebServiceProxyGeneratorTool::get_DefaultExtensions(BSTR* pVal)
{
	return DoGetDefaultExtensions(s_bstrExtensions, wszWebDefaultExtensions, pVal);
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::put_DefaultExtensions(BSTR newVal)
{
	s_bstrExtensions = newVal;
	return S_OK;
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::GetAdditionalOptionsInternal(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, 
	BSTR* pbstrAdditionalOptions)
{ 
	return GetProcessedAdditionalOptions(pItem, VCWEBID_AdditionalOptions, bForBuild, bSkipLocal, pbstrAdditionalOptions);
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::GetBuildDescription(IVCBuildAction* pAction, BSTR* pbstrBuildDescription)
{
	if (s_bstrBuildDescription.Length() == 0)
		s_bstrBuildDescription.LoadString(IDS_DESC_WEB_COMPILING);
	return s_bstrBuildDescription.CopyTo(pbstrBuildDescription);
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::get_ToolPathInternal(BSTR* pbstrToolPath)
{ 
	return get_ToolPath(pbstrToolPath);
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::get_Bucket(long *pVal)
{
	CHECK_POINTER_NULL( pVal );
	*pVal = BUCKET_WEB;
	return S_OK;
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::CreateToolObject(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject)
{ 
	return CreateInstance(pPropContainer, ppToolObject); 
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::GetAdditionalIncludeDirectoriesInternal(IVCPropertyContainer* pPropContainer, BSTR* pbstrIncDirs)
{
	return E_NOTIMPL;
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::get_ToolShortName(BSTR* pbstrToolName)
{
	CHECK_POINTER_NULL(pbstrToolName);
	*pbstrToolName = SysAllocString( szWebToolShortName );
	return S_OK;
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::MatchName(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches)
{
	return DoMatchName(bstrName, szWebToolType, szWebToolShortName, pbMatches);
}

HRESULT CVCWebServiceProxyGeneratorTool::DoGetToolPath(BSTR* pbstrToolPath, IVCPropertyContainer* pPropContainer)
{
	CHECK_POINTER_NULL(pbstrToolPath);
	if (pPropContainer)
	{
		genProxyLanguage genProxy = genProxyNative;
		if (pPropContainer->GetIntProperty(VCWEBID_GeneratedProxyLanguage, (long *)&genProxy) == S_OK &&
			genProxy == genProxyManaged)
		{
			*pbstrToolPath = SysAllocString( szWebToolPathManaged );
			return S_OK;
		}
	}

	*pbstrToolPath = SysAllocString( szWebToolPathNative );
	return S_OK;
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::get_ToolPath(BSTR* pbstrToolPath)
{
	return DoGetToolPath(pbstrToolPath, m_spPropertyContainer);
}

STDMETHODIMP CVCWebServiceProxyGeneratorTool::get_PropertyOption(BSTR bstrProp, long dispidProp, BSTR *pVal)
{ 
	return E_NOTIMPL;
}


// CVCWebServiceGeneralPage
///////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CVCWebServiceGeneralPage::get_Output(BSTR *pbstrOutput)
{
	return ToolGetStrProperty(VCWEBID_Output, &(CVCWebServiceProxyGeneratorTool::s_optHandler), pbstrOutput);
}

STDMETHODIMP CVCWebServiceGeneralPage::put_Output(BSTR bstrOutput)
{
	return SetStrProperty( VCWEBID_Output, bstrOutput );
}

STDMETHODIMP CVCWebServiceGeneralPage::get_SuppressStartupBanner(enumSuppressStartupBannerBOOL* suppress)
{	// /nologo
	return GetEnumBoolProperty2(VCWEBID_SuppressStartupBanner, (long *)suppress);
}

STDMETHODIMP CVCWebServiceGeneralPage::put_SuppressStartupBanner(enumSuppressStartupBannerBOOL suppress)
{
	return SetBoolProperty(VCWEBID_SuppressStartupBanner, suppress);
}

STDMETHODIMP CVCWebServiceGeneralPage::get_GeneratedProxyLanguage(genProxyLanguage* language)
{	// generated proxy language: native or managed C++
	return ToolGetIntProperty(VCWEBID_GeneratedProxyLanguage, &(CVCWebServiceProxyGeneratorTool::s_optHandler), (long *)language);
}

STDMETHODIMP CVCWebServiceGeneralPage::put_GeneratedProxyLanguage(genProxyLanguage language)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, WebProxyMax, language);
	return SetIntProperty(VCWEBID_GeneratedProxyLanguage, language);
}

void CVCWebServiceGeneralPage::GetBaseDefault(long id, CComVariant& varValue)
{
	CComBSTR bstrVal;
	VARIANT_BOOL bVal;
	switch (id)
	{
	case VCWEBID_Output:
		CVCWebServiceProxyGeneratorTool::s_optHandler.GetDefaultValue( id, &bstrVal );
		varValue = bstrVal;
		break;
	case VCWEBID_SuppressStartupBanner:
		CVCWebServiceProxyGeneratorTool::s_optHandler.GetDefaultValue( id, &bVal );
		varValue = bVal;
		break;
	default:
		CBase::GetBaseDefault(id, varValue);
		return;
	}
}
