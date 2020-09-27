// LibTool.cpp : Implementation of Librarian Tool

#include "stdafx.h"
#include "LibTool.h"
#include "FileRegistry.h"
#include "BldHelpers.h"

// constants
const wchar_t* const wszLibDefaultExtensions = L"*.obj;*.res;*.rsc;*.lib";	// remember to update vcpb.rgs if you change this...
const wchar_t* const wszLibDefaultName = L"$(OutDir)/$(ProjectName).lib";
const wchar_t* const wszLibDelOnRebuildExtensions = L"lib;map";


// static initializers
CComBSTR CVCLibrarianTool::s_bstrBuildDescription = L"";
CComBSTR CVCLibrarianTool::s_bstrToolName = L"";
CComBSTR CVCLibrarianTool::s_bstrExtensions = L"";
CLibrarianToolOptionHandler CVCLibrarianTool::s_optHandler;
GUID CVCLibrarianTool::s_pPages[2];
BOOL CVCLibrarianTool::s_bPagesInit = FALSE;


////////////////////////////////////////////////////////////////////////////////
// Option Table for the Librarian Tool switches

// NOTE: we're using macros for enum value ranges to help in keeping enum range checks as low maintenance as possible.
// If you add an enum to this table, make sure you use macros to define the upper and lower bounds and use CHECK_BOUNDS
// on the get/put methods associated with that property (both object model and property page).
// WARNING: if you change ANYTHING about a multi-prop here (including add/remove), be sure to update how the property
// is obtained on both the tool and page objects.
BEGIN_OPTION_TABLE(CLibrarianToolOptionHandler, L"VCLibrarianTool", IDS_Lib, TRUE /* pre & post */, FALSE /* case sensitive */)
	OPT_BSTR(OutputFile,				L"OUT:%s",				L"OutputFile",			single,		IDS_LibGeneral, VCLIBID)
	OPT_BSTR(AdditionalLibraryDirectories, L"LIBPATH:%s",		L"AdditionalLibraryDirectories",	multipleNoCase,	IDS_LibGeneral, VCLIBID)
	OPT_BOOL(SuppressStartupBanner,		L"|NOLOGO",				L"SuppressStartupBanner",			IDS_LibGeneral, VCLIBID)
	OPT_BSTR(ModuleDefinitionFile,		L"DEF:%s",				L"ModuleDefinitionFile", single,	IDS_LibGeneral, VCLIBID)
	OPT_BOOL(IgnoreAllDefaultLibraries,	L"|NODEFAULTLIB",		L"IgnoreAllDefaultLibraries",		IDS_LibGeneral, VCLIBID)
	OPT_BSTR(IgnoreDefaultLibraryNames,	L"NODEFAULTLIB:%s",		L"IgnoreDefaultLibraryNames", multipleNoCase, IDS_LibGeneral, VCLIBID)
	OPT_BSTR(ExportNamedFunctions,		L"EXPORT:%s",			L"ExportNamedFunctions", multiple,	IDS_LibGeneral, VCLIBID)
	OPT_BSTR(ForceSymbolReferences,		L"INCLUDE:%s",			L"ForceSymbolReferences", multiple,	IDS_LibGeneral, VCLIBID)
END_OPTION_TABLE()

// default value handlers
// string props
void CLibrarianToolOptionHandler::GetDefaultValue( long id, BSTR *pVal, IVCPropertyContainer *pPropContainer )
{
	switch( id )
	{
	case VCLIBID_OutputFile:
	{
		if( !pPropContainer )
			GetDefaultString( pVal );
		else
			*pVal = SysAllocString( wszLibDefaultName );
		break;
	}
// 	case VCLIBID_AdditionalOptions:
// 	case VCLIBID_AdditionalDependencies:
// 	case VCLIBID_AdditionalLibraryDirectories:
// 	case VCLIBID_ForceSymbolReferences:
// 	case VCLIBID_IgnoreAllDefaultLibraries:
// 	case VCLIBID_ExportNamedFunctions:
// 	case VCLIBID_ModuleDefinitionFile:
	default:
		COptionHandlerBase::GetDefaultString( pVal );
		break;
	}
}

// integer props
void CLibrarianToolOptionHandler::GetDefaultValue( long id, long *pVal, IVCPropertyContainer *pPropCnt )
{
	*pVal = 0;
}

// boolean props
void CLibrarianToolOptionHandler::GetDefaultValue( long id, VARIANT_BOOL *pVal, IVCPropertyContainer *pPropCnt )
{
	switch( id )
	{
	case VCLIBID_SuppressStartupBanner:
		GetValueTrue( pVal );
		break;
// 	case VCLIBID_IgnoreAllDefaultLibraries:
	default:
		COptionHandlerBase::GetValueFalse( pVal );
		break;
	}
}

BOOL CLibrarianToolOptionHandler::SetEvenIfDefault(VARIANT *pvarDefault, long idOption)
{
	CComVariant varDefault;
	BOOL bRet = FALSE;

	switch (idOption)
	{
	case VCLIBID_SuppressStartupBanner:
		varDefault = VARIANT_TRUE;
		varDefault.vt = VT_BOOL;
		bRet = TRUE;
		break;
	}
	varDefault.Detach(pvarDefault);
	return bRet;
}

BOOL CLibrarianToolOptionHandler::SynthesizeOptionIfNeeded(IVCPropertyContainer* pPropContainer, long idOption)
{
	switch (idOption)
	{
	case VCLIBID_OutputFile:
		return TRUE;
	default:
		return FALSE;
	}
}

BOOL CLibrarianToolOptionHandler::SynthesizeOption(IVCPropertyContainer* pPropContainer, long idOption, 
	CComVariant& rvar)
{
	switch (idOption)
	{
	case VCLIBID_OutputFile:
		{
			CComBSTR bstrOutputFile;
			if (CreateOutputName(pPropContainer, &bstrOutputFile, FALSE))
			{
				rvar.Clear();
				rvar = bstrOutputFile;
				return TRUE;
			}
			return FALSE;
		}
	default:
		VSASSERT(FALSE, "Case statement mismatch between SynthesizeOptionIfNeeded and SynthesizeOption");	// shouldn't be here!!
		return FALSE;
	}
}

BOOL CLibrarianToolOptionHandler::CreateOutputName(IVCPropertyContainer* pPropContainer, BSTR* pbstrVal,
	BOOL bCheckForExistence /* = TRUE */)
{
	if (bCheckForExistence &&
		pPropContainer->GetEvaluatedStrProperty(VCLIBID_OutputFile, pbstrVal) == S_OK)	// found it directly set, so use it
		return TRUE;

	CVCLibrarianTool::s_optHandler.GetDefaultValue( VCLINKID_OutputFile, pbstrVal, pPropContainer );
	HRESULT hr = pPropContainer->Evaluate(*pbstrVal, pbstrVal);

	return (SUCCEEDED(hr));
}

///////////////////////////////////////////////////////////////////////////////
// Librarian Tool
HRESULT CVCLibrarianTool::CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppLibTool)
{
	CHECK_POINTER_NULL(ppLibTool);
	*ppLibTool = NULL;

	CComObject<CVCLibrarianTool> *pObj;
	HRESULT hr = CComObject<CVCLibrarianTool>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		if( pPropContainer )
			pObj->Initialize(pPropContainer);
		CVCLibrarianTool *pVar = pObj;
		pVar->AddRef();
		*ppLibTool = pVar;
	}
	return hr;
}

STDMETHODIMP CVCLibrarianTool::IsDeleteOnRebuildFile(LPCOLESTR szFile, VARIANT_BOOL* pbDelOnRebuild)
{
	CHECK_POINTER_NULL(pbDelOnRebuild);
	BOOL fDel = BldFileNameMatchesExtension(szFile, wszLibDelOnRebuildExtensions);
	*pbDelOnRebuild = fDel ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

LPCOLESTR CVCLibrarianTool::GetToolFriendlyName()
{
	InitToolName();
	return s_bstrToolName;
}

void CVCLibrarianTool::InitToolName()
{
	if (s_bstrToolName.Length() == 0)
	{
		if (!s_bstrToolName.LoadString(IDS_LIB_TOOLNAME))
			s_bstrToolName = szLibrarianToolType;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Affects our output?
STDMETHODIMP CVCLibrarianTool::AffectsOutput(long nPropID, VARIANT_BOOL* pbAffectsOutput)
{
	CHECK_POINTER_NULL(pbAffectsOutput);
	*pbAffectsOutput = VARIANT_FALSE;

	// change our outputs?
	switch (nPropID)
	{
	case VCLIBID_OutputFile:			// *.lib
	case VCLIBID_ModuleDefinitionFile:	// *.def
	case VCLIBID_AdditionalDependencies:		// extra stuff to add to link line
	case VCLIBID_OutputsDirty:			// anything we care about dirty
	case VCLIBID_DependentInputs:		// inputs we pick up from dependent projects
		*pbAffectsOutput = VARIANT_TRUE;
		break;
	default:
		*pbAffectsOutput = VARIANT_FALSE;
		break;
	}

	return S_OK;
}

STDMETHODIMP CVCLibrarianTool::CreatePageObject(IUnknown **ppUnk, CLSID*pCLSID, IVCPropertyContainer *pPropCntr, IVCSettingsPage *pPage )
{
	// Get the list of pages
	if( CLSID_LibGeneral == *pCLSID )
	{
		CPageObjectImpl< CVCLibrarianPage,VCLIBRARIANTOOL_MIN_DISPID,VCLIBRARIANTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else
	{
		return S_FALSE;
	}
		
	return S_OK;
}


GUID* CVCLibrarianTool::GetPageIDs()
{ 
	if (s_bPagesInit == FALSE)
	{
		s_pPages[0] = __uuidof(LibGeneral);
		s_pPages[1] = __uuidof(LibAdditionalOptions);
		s_bPagesInit = TRUE;
	}
	return s_pPages; 
}

STDMETHODIMP CVCLibrarianTool::GenerateOutput(long type, IVCBuildActionList* plstActions, IVCBuildErrorContext* pEC)
{
	return DoGenerateOutput(type, plstActions, pEC, FALSE, this);
}

HRESULT CVCLibrarianTool::GetPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, CPathW& rpath)
{
	return DoGetPrimaryOutputFromTool(pItem, rpath, FALSE);
}

STDMETHODIMP CVCLibrarianTool::HasPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, VARIANT_BOOL* pbHasPrimaryOutput)
{
	return COptionHandlerBase::GetValueTrue( pbHasPrimaryOutput );
}

STDMETHODIMP CVCLibrarianTool::GetPrimaryOutputIDFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, long* pnOutputID)
{
	CHECK_POINTER_NULL(pnOutputID);
	*pnOutputID = VCLIBID_OutputFile;
	return S_OK;
}

STDMETHODIMP CVCLibrarianTool::HasDependencies(IVCBuildAction* pAction, VARIANT_BOOL* pbHasDependencies)
{
	return DoHasDependencies(pAction, FALSE, pbHasDependencies);
}

STDMETHODIMP CVCLibrarianTool::GetDependencies(IVCBuildAction* pAction, IVCBuildStringCollection** ppStrings, 
	VARIANT_BOOL* pbUpToDate)
{
	return DoGetDependencies(pAction, ppStrings, pbUpToDate, FALSE);
}

BOOL CVCLibrarianTool::CreateOutputName(IVCPropertyContainer* pPropContainer, BSTR* pbstrVal)
{
	return s_optHandler.CreateOutputName(pPropContainer, pbstrVal, FALSE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VCLibrarianTool
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CVCLibrarianTool::get_ToolName(BSTR* pbstrToolName)
{	// friendly name of tool, e.g., "Librarian Tool"
	CHECK_POINTER_VALID(pbstrToolName);
	InitToolName();
	s_bstrToolName.CopyTo(pbstrToolName);
	return S_OK;
}

STDMETHODIMP CVCLibrarianTool::get_AdditionalOptions(BSTR* pbstrAdditionalOptions)
{	// anything we don't know what to do with, given verbatim, in same order as entered, parent contributions first, goes after all other options
	return ToolGetStrProperty(VCLIBID_AdditionalOptions, pbstrAdditionalOptions);
}

STDMETHODIMP CVCLibrarianTool::put_AdditionalOptions(BSTR bstrAdditionalOptions)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLIBID_AdditionalOptions, bstrAdditionalOptions);
}

STDMETHODIMP CVCLibrarianTool::get_SuppressStartupBanner(VARIANT_BOOL* pbNoLogo)
{	// (/NOLOGO) enable suppression of copyright message (no explicit off)
	return ToolGetBoolProperty(VCLIBID_SuppressStartupBanner, pbNoLogo);
}

STDMETHODIMP CVCLibrarianTool::put_SuppressStartupBanner(VARIANT_BOOL bNoLogo)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bNoLogo );
	return m_spPropertyContainer->SetBoolProperty(VCLIBID_SuppressStartupBanner, bNoLogo);
}

STDMETHODIMP CVCLibrarianTool::get_ModuleDefinitionFile(BSTR* pbstrDefFile)
{	// (/DEF:file)
	return ToolGetStrProperty(VCLIBID_ModuleDefinitionFile, pbstrDefFile);
}

STDMETHODIMP CVCLibrarianTool::put_ModuleDefinitionFile(BSTR bstrDefFile)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLIBID_ModuleDefinitionFile, bstrDefFile);
}

STDMETHODIMP CVCLibrarianTool::get_ExportNamedFunctions(BSTR* pbstrSymbols)
{	// (/EXPORT:[symbol]) export function, can have multiple
	return ToolGetStrProperty(VCLIBID_ExportNamedFunctions, pbstrSymbols, true /* local only */);
}

STDMETHODIMP CVCLibrarianTool::put_ExportNamedFunctions(BSTR bstrSymbols)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLIBID_ExportNamedFunctions, bstrSymbols);
}

STDMETHODIMP CVCLibrarianTool::get_AdditionalLibraryDirectories(BSTR* pbstrLibPath)
{	// (/LIBPATH:[dir]) specify path to search for libraries on, can have multiple
	return ToolGetStrProperty(VCLIBID_AdditionalLibraryDirectories, pbstrLibPath, true /* local only */);
}

STDMETHODIMP CVCLibrarianTool::put_AdditionalLibraryDirectories(BSTR bstrLibPath)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLIBID_AdditionalLibraryDirectories, bstrLibPath);
}

STDMETHODIMP CVCLibrarianTool::get_IgnoreAllDefaultLibraries(VARIANT_BOOL* pbNoDefault)
{	// (/NODEFAULTLIB) no default libraries not in docs
	return ToolGetBoolProperty(VCLIBID_IgnoreAllDefaultLibraries, pbNoDefault);
}

STDMETHODIMP CVCLibrarianTool::put_IgnoreAllDefaultLibraries(VARIANT_BOOL bNoDefault)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bNoDefault );
	return m_spPropertyContainer->SetBoolProperty(VCLIBID_IgnoreAllDefaultLibraries, bNoDefault);
}

STDMETHODIMP CVCLibrarianTool::get_IgnoreDefaultLibraryNames(BSTR* pbstrLib)
{	// (/NODEFAULTLIB:[name]) ignore particular default library can have multiple not in docs
	return ToolGetStrProperty(VCLIBID_IgnoreDefaultLibraryNames, pbstrLib, true /* local only */);
}

STDMETHODIMP CVCLibrarianTool::put_IgnoreDefaultLibraryNames(BSTR bstrLib)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLIBID_IgnoreDefaultLibraryNames, bstrLib);
}

STDMETHODIMP CVCLibrarianTool::get_OutputFile(BSTR* pbstrOut)
{	// (/OUT:[file]) change the output file name (default is based on 1st lib or obj name on cmd line)
	return ToolGetStrProperty(VCLIBID_OutputFile, pbstrOut);
}

STDMETHODIMP CVCLibrarianTool::put_OutputFile(BSTR bstrOut)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLIBID_OutputFile, bstrOut);
}

STDMETHODIMP CVCLibrarianTool::get_AdditionalDependencies(BSTR* pbstrInputs)
{	// additional inputs to the librarian tool such as extra libraries
	return ToolGetStrProperty(VCLIBID_AdditionalDependencies, pbstrInputs, true /* local only */);
}

STDMETHODIMP CVCLibrarianTool::put_AdditionalDependencies(BSTR bstrInputs)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLIBID_AdditionalDependencies, bstrInputs);
}

STDMETHODIMP CVCLibrarianTool::get_ForceSymbolReferences(BSTR* pbstrSymbol)
{	// (/INCLUDE:[symbol]) force symbol reference, can have multiple
	return ToolGetStrProperty(VCLIBID_ForceSymbolReferences, pbstrSymbol, true /* local only */);
}

STDMETHODIMP CVCLibrarianTool::put_ForceSymbolReferences(BSTR bstrSymbol)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLIBID_ForceSymbolReferences, bstrSymbol);
}

// Automation properties
STDMETHODIMP CVCLibrarianTool::get_ToolPath(BSTR *pVal)
{
	CHECK_POINTER_VALID( pVal );
	*pVal = SysAllocString( szLibrarianToolPath );
	return S_OK;
}

STDMETHODIMP CVCLibrarianTool::get_PropertyOption(BSTR bstrProp, long dispidProp, BSTR *pVal)
{ 
	return DoGetPropertyOption(bstrProp, dispidProp, pVal); 
}

// IVCToolImpl
STDMETHODIMP CVCLibrarianTool::get_DefaultExtensions(BSTR* pVal)
{
	return DoGetDefaultExtensions(s_bstrExtensions, wszLibDefaultExtensions, pVal);
}

STDMETHODIMP CVCLibrarianTool::put_DefaultExtensions(BSTR newVal)
{
	s_bstrExtensions = newVal;
	return S_OK;
}

STDMETHODIMP CVCLibrarianTool::GetAdditionalOptionsInternal(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, 
	BSTR* pbstrAdditionalOptions)
{ 
	return GetProcessedAdditionalOptions(pItem, VCLIBID_AdditionalOptions, bForBuild, bSkipLocal, pbstrAdditionalOptions);
}

STDMETHODIMP CVCLibrarianTool::GetBuildDescription(IVCBuildAction* pAction, BSTR* pbstrBuildDescription)
{
	if (s_bstrBuildDescription.Length() == 0)
		s_bstrBuildDescription.LoadString(IDS_DESC_LIBING);
	return s_bstrBuildDescription.CopyTo(pbstrBuildDescription);
}

STDMETHODIMP CVCLibrarianTool::GetAdditionalIncludeDirectoriesInternal(IVCPropertyContainer* pPropContainer, BSTR* pbstrIncDirs)
{
	CHECK_POINTER_NULL(pbstrIncDirs);
	CHECK_READ_POINTER_NULL(pPropContainer);
	CComBSTR bstrTmp;
	HRESULT hr = pPropContainer->GetMultiProp(VCLIBID_AdditionalLibraryDirectories, L";", VARIANT_FALSE, &bstrTmp);
	if (SUCCEEDED(hr))
		pPropContainer->Evaluate(bstrTmp, pbstrIncDirs);
	else
		*pbstrIncDirs = bstrTmp.Detach();
	return hr;
}

STDMETHODIMP CVCLibrarianTool::get_ToolPathInternal(BSTR* pbstrToolPath)
{ 
	return get_ToolPath(pbstrToolPath);
}

STDMETHODIMP CVCLibrarianTool::get_Bucket(long *pVal)
{
	CHECK_POINTER_NULL( pVal );
	*pVal = BUCKET_LIB; 
	return S_OK;
}

STDMETHODIMP CVCLibrarianTool::IsTargetTool(IVCBuildAction* pAction, VARIANT_BOOL* pbTargetTool)
{ // tool always operates on target, not on file
	return COptionHandlerBase::GetValueTrue( pbTargetTool );
}

HRESULT CVCLibrarianTool::GetAdditionalDependenciesInternal(IVCPropertyContainer* pItem, IVCBuildAction* pAction, 
	BOOL bForSave, BSTR* pbstrInputs)
{		// additional things to add to command line
	return DoGetAdditionalDependenciesInternal(pItem, pAction, bForSave, pbstrInputs, GetOptionHandler(), 
		VCLIBID_AdditionalDependencies, VCLIBID_DependentInputs);
}

STDMETHODIMP CVCLibrarianTool::get_ToolShortName(BSTR* pbstrToolName)
{
	CHECK_POINTER_NULL(pbstrToolName);
	*pbstrToolName = SysAllocString( szLibrarianToolShortName );
	return S_OK;
}

STDMETHODIMP CVCLibrarianTool::MatchName(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches)
{
	return DoMatchName(bstrName, szLibrarianToolType, szLibrarianToolShortName, pbMatches);
}

STDMETHODIMP CVCLibrarianTool::CreateToolObject(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject)
{ 
	return CreateInstance(pPropContainer, ppToolObject); 
}

STDMETHODIMP CVCLibrarianTool::CanScanForDependencies(VARIANT_BOOL* pbIsScannable)
{
	return COptionHandlerBase::GetValueTrue(pbIsScannable);
}

STDMETHODIMP CVCLibrarianPage::get_SuppressStartupBanner(enumSuppressStartupBannerUpBOOL* pbNoLogo)
{	// (/NOLOGO) enable suppression of copyright message (no explicit off)
	return GetEnumBoolProperty2(VCLIBID_SuppressStartupBanner, (long *)pbNoLogo);
}

STDMETHODIMP CVCLibrarianPage::put_SuppressStartupBanner(enumSuppressStartupBannerUpBOOL bNoLogo)
{
	return SetBoolProperty(VCLIBID_SuppressStartupBanner, bNoLogo);
}

STDMETHODIMP CVCLibrarianPage::get_ModuleDefinitionFile(BSTR* pbstrDefFile)
{	// (/DEF:file)
	return ToolGetStrProperty(VCLIBID_ModuleDefinitionFile, &(CVCLibrarianTool::s_optHandler), pbstrDefFile);
}

STDMETHODIMP CVCLibrarianPage::put_ModuleDefinitionFile(BSTR bstrDefFile)
{
	return SetStrProperty(VCLIBID_ModuleDefinitionFile, bstrDefFile);
}

STDMETHODIMP CVCLibrarianPage::get_ExportNamedFunctions(BSTR* pbstrSymbols)
{	// (/EXPORT:[symbol]) export function, can have multiple
	return ToolGetStrProperty(VCLIBID_ExportNamedFunctions, &(CVCLibrarianTool::s_optHandler), pbstrSymbols, true /* local only */);
}

STDMETHODIMP CVCLibrarianPage::put_ExportNamedFunctions(BSTR bstrSymbols)
{
	return SetStrProperty(VCLIBID_ExportNamedFunctions, bstrSymbols);
}

STDMETHODIMP CVCLibrarianPage::get_AdditionalLibraryDirectories(BSTR* pbstrLibPath)
{	// (/LIBPATH:[dir]) specify path to search for libraries on, can have multiple
	return ToolGetStrProperty(VCLIBID_AdditionalLibraryDirectories, &(CVCLibrarianTool::s_optHandler), pbstrLibPath, true /* local only */);
}

STDMETHODIMP CVCLibrarianPage::put_AdditionalLibraryDirectories(BSTR bstrLibPath)
{
	return SetStrProperty(VCLIBID_AdditionalLibraryDirectories, bstrLibPath);
}

// input and output
STDMETHODIMP CVCLibrarianPage::get_IgnoreAllDefaultLibraries(enumIgnoreAllDefaultLibrariesBOOL* pbNoDefault)
{	// (/NODEFAULTLIB) no default libraries not in docs
	return GetEnumBoolProperty2(VCLIBID_IgnoreAllDefaultLibraries, (long *)pbNoDefault);
}

STDMETHODIMP CVCLibrarianPage::put_IgnoreAllDefaultLibraries(enumIgnoreAllDefaultLibrariesBOOL bNoDefault)
{
	return SetBoolProperty(VCLIBID_IgnoreAllDefaultLibraries, bNoDefault);
}

STDMETHODIMP CVCLibrarianPage::get_IgnoreDefaultLibraryNames(BSTR* pbstrLib)
{	// (/NODEFAULTLIB:[name]) ignore particular default library can have multiple not in docs
	return ToolGetStrProperty(VCLIBID_IgnoreDefaultLibraryNames, &(CVCLibrarianTool::s_optHandler), pbstrLib, true /* local only */);
}

STDMETHODIMP CVCLibrarianPage::put_IgnoreDefaultLibraryNames(BSTR bstrLib)
{
	return SetStrProperty(VCLIBID_IgnoreDefaultLibraryNames, bstrLib);
}

STDMETHODIMP CVCLibrarianPage::get_OutputFile(BSTR* pbstrOut)
{	// (/OUT:[file]) change the output file name (default is based on 1st lib or obj name on cmd line)
	return ToolGetStrProperty(VCLIBID_OutputFile, &(CVCLibrarianTool::s_optHandler), pbstrOut);
}

STDMETHODIMP CVCLibrarianPage::put_OutputFile(BSTR bstrOut)
{
	return SetStrProperty(VCLIBID_OutputFile, bstrOut);
}

STDMETHODIMP CVCLibrarianPage::get_AdditionalDependencies(BSTR* pbstrInputs)
{	// additional inputs to the librarian tool such as extra libraries
	return ToolGetStrProperty(VCLIBID_AdditionalDependencies, &(CVCLibrarianTool::s_optHandler), pbstrInputs, true /* local only */);
}

STDMETHODIMP CVCLibrarianPage::put_AdditionalDependencies(BSTR bstrInputs)
{
	return SetStrProperty(VCLIBID_AdditionalDependencies, bstrInputs);
}

STDMETHODIMP CVCLibrarianPage::get_ForceSymbolReferences(BSTR* pbstrSymbol)
{	// (/INCLUDE:[symbol]) force symbol reference, can have multiple
	return ToolGetStrProperty(VCLIBID_ForceSymbolReferences, &(CVCLibrarianTool::s_optHandler), pbstrSymbol, true /* local only */);
}

STDMETHODIMP CVCLibrarianPage::put_ForceSymbolReferences(BSTR bstrSymbol)
{
	return SetStrProperty(VCLIBID_ForceSymbolReferences, bstrSymbol);
}

void CVCLibrarianPage::GetBaseDefault(long id, CComVariant& varValue)
{
	if (id == VCLIBID_OutputFile)
	{
		CComBSTR bstrVal;
		CVCLibrarianTool::s_optHandler.GetDefaultValue( VCLIBID_OutputFile, &bstrVal );
		varValue = bstrVal;
	}
	else if (id == VCLIBID_SuppressStartupBanner)
	{
		VARIANT_BOOL bVal;
		CVCLibrarianTool::s_optHandler.GetDefaultValue( VCLIBID_SuppressStartupBanner, &bVal );
		varValue = bVal;
	}

	else
		CBase::GetBaseDefault(id, varValue);
}

