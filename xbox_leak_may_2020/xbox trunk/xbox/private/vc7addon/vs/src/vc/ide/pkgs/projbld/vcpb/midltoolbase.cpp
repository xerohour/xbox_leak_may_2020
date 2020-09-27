// MidlBaseTool.cpp : Implementation of MIDL Tool

#include "stdafx.h"
#include "MidlTool.h"
#include "BuildEngine.h"
#include "scanner.h"

// constants
const wchar_t* const wszMidlDefaultExtensions = L"*.idl;*.odl";	// remember to update vcpb.rgs if you change this...
const wchar_t* const wszMidlDefaultHeader = L"$(InputName)_h.h";
const wchar_t* const wszMidlDefaultTLBFile = L"$(IntDir)/$(ProjectName).tlb";
const wchar_t* const wszMidlDLLDataFileName = L"dlldata.c";
const midlWarningLevelOption lmidlWarnLevel = midlWarningLevel_1;
const midlCharOption lDefaultCharType = midlCharUnsigned;
const midlTargetEnvironment lTargetEnvironment = midlTargetWin32;
const midlErrorCheckOption lEnableErrorChecks = midlEnableCustom;
const midlStructMemberAlignOption lStructMemberAlignment = midlAlignNotSet;

// static initializers
CMidlToolOptionHandler CVCMidlTool::s_optHandler;
GUID CVCMidlTool::s_pPages[4];
BOOL CVCMidlTool::s_bPagesInit = FALSE;
CComBSTR CVCMidlTool::s_bstrBuildDescription = L"";
CComBSTR CVCMidlTool::s_bstrToolName = L"";
CComBSTR CVCMidlTool::s_bstrExtensions = L"";

////////////////////////////////////////////////////////////////////////////////
// Option Table for MIDL switches

// NOTE: we're using macros for enum value ranges to help in keeping enum range checks as low maintenance as possible.
// If you add an enum to this table, make sure you use macros to define the upper and lower bounds and use CHECK_BOUNDS
// on the get/put methods associated with that property (both object model and property page).
// WARNING: if you change ANYTHING about a multi-prop here (including add/remove), be sure to update how the property
// is obtained on both the tool and page objects.
BEGIN_OPTION_TABLE(CMidlToolOptionHandler, L"VCMidlTool", IDS_Midl, TRUE /* pre & post */, TRUE /* case sensitive */)
	// general
	OPT_BSTR(PreprocessorDefinitions,	L"D %s",						L"PreprocessorDefinitions",		multiple,	IDS_MidlGeneral, VCMIDLID)
	OPT_BSTR(AdditionalIncludeDirectories, L"I %s",						L"AdditionalIncludeDirectories", multipleNoCase,	IDS_MidlGeneral, VCMIDLID)
	OPT_BOOL(IgnoreStandardIncludePath,	L"|no_def_idir",				L"IgnoreStandardIncludePath",	IDS_MidlGeneral, VCMIDLID)
	OPT_BOOL(MkTypLibCompatible,		L"|mktyplib203",				L"MkTypLibCompatible",			IDS_MidlGeneral, VCMIDLID)
	OPT_ENUM(WarningLevel,				L"W0|W1|W2|W3|W4",		StdEnumMin, midlWarnMax,	L"WarningLevel", IDS_MidlGeneral, VCMIDLID)
	OPT_BOOL(WarnAsError,				L"|WX",							L"WarnAsError",					IDS_MidlGeneral, VCMIDLID)
	OPT_BOOL(SuppressStartupBanner,		L"|nologo",						L"SuppressStartupBanner",		IDS_MidlGeneral, VCMIDLID)
	OPT_ENUM(DefaultCharType, L"char unsigned|char signed|char ascii7", StdEnumMin, charTypeMax, L"DefaultCharType", IDS_MidlGeneral, VCMIDLID)
	OPT_ENUM(TargetEnvironment,			L"|env win32|env win64",	 StdEnumMin, envMax,	L"TargetEnvironment", IDS_MidlGeneral, VCMIDLID)
	OPT_BOOL(GenerateStublessProxies,	L"|Oicf",						L"GenerateStublessProxies",		IDS_MidlGeneral, VCMIDLID)
	// output 
	OPT_BOOL_SET_COND(GenerateTypeLibrary, L"notlb|", /* newtlb */,		L"GenerateTypeLibrary", TypeLibraryName, IDS_MidlOutput, VCMIDLID)
	OPT_BSTR_COND(TypeLibraryName,		L" /tlb %s",					L"TypeLibraryName",	GenerateTypeLibrary, single, IDS_MidlOutput, VCMIDLID)
	OPT_BSTR(OutputDirectory,			L"out %s",						L"OutputDirectory",	single,		IDS_MidlOutput, VCMIDLID)
	OPT_BSTR(HeaderFileName,			L"h %s",						L"HeaderFileName",	single,		IDS_MidlOutput, VCMIDLID)
	OPT_BSTR(DLLDataFileName,			L"dlldata %s",					L"DLLDataFileName",	single,		IDS_MidlOutput, VCMIDLID)
	OPT_BSTR(InterfaceIdentifierFileName, L"iid %s",					L"InterfaceIdentifierFileName", single, IDS_MidlOutput, VCMIDLID)
	OPT_BSTR(ProxyFileName,				L"proxy %s",					L"ProxyFileName",	single,		IDS_MidlOutput, VCMIDLID)
	// advanced
	OPT_ENUM(EnableErrorChecks,			L"|error none|error all", StdEnumMin, errCheckMax, L"EnableErrorChecks", IDS_MidlAdvanced, VCMIDLID)
	OPT_BOOL(ErrorCheckAllocations,		L"|error allocation",			L"ErrorCheckAllocations",		IDS_MidlAdvanced, VCMIDLID)
	OPT_BOOL(ErrorCheckBounds,			L"|error bounds_check",			L"ErrorCheckBounds",			IDS_MidlAdvanced, VCMIDLID)
	OPT_BOOL(ErrorCheckEnumRange,		L"|error enum",					L"ErrorCheckEnumRange",			IDS_MidlAdvanced, VCMIDLID)
	OPT_BOOL(ErrorCheckRefPointers,		L"|error ref",					L"ErrorCheckRefPointers",		IDS_MidlAdvanced, VCMIDLID)
	OPT_BOOL(ErrorCheckStubData,		L"|error stub_data",			L"ErrorCheckStubData",			IDS_MidlAdvanced, VCMIDLID)
	OPT_BOOL(ValidateParameters,		L"|robust",						L"ValidateParameters",			IDS_MidlAdvanced, VCMIDLID)
	OPT_ENUM(StructMemberAlignment,		L"|Zp1|Zp2|Zp4|Zp8",		StdEnumMin, midlStructAlignMax,	L"StructMemberAlignment", IDS_MidlAdvanced, VCMIDLID)
	OPT_BSTR(RedirectOutputAndErrors,	L"o %s",						L"RedirectOutputAndErrors", single, IDS_MidlAdvanced, VCMIDLID)
	OPT_BSTR(CPreprocessOptions,		L"cpp_opt%s",					L"CPreprocessOptions",		multiple,	IDS_MidlAdvanced, VCMIDLID)
	OPT_BSTR(UndefinePreprocessorDefinitions, L"U %s",					L"UndefinePreprocessorDefinitions",		multiple,	IDS_MidlAdvanced, VCMIDLID)
END_OPTION_TABLE()

// default value handlers
// string props
void CMidlToolOptionHandler::GetDefaultValue( long id, BSTR *pVal, IVCPropertyContainer *pPropCnt )
{
	switch( id )
	{
	case VCMIDLID_HeaderFileName:
		*pVal = SysAllocString( wszMidlDefaultHeader );
		break;
	case VCMIDLID_TypeLibraryName:
		*pVal = SysAllocString( wszMidlDefaultTLBFile );
		break;
	case VCMIDLID_DLLDataFileName:
		*pVal = SysAllocString( wszMidlDLLDataFileName );
		break;
// 	case VCMIDLID_AdditionalOptions:
// 	case VCMIDLID_PreprocessorDefinitions:
// 	case VCMIDLID_AdditionalIncludeDirectories:
// 	case VCMIDLID_OutputDirectory:
// 	case VCMIDLID_InterfaceIdentifierFileName:
// 	case VCMIDLID_ProxyFileName:
// 	case VCMIDLID_RedirectOutputAndErrors:
// 	case VCMIDLID_CPreprocessOptions:
// 	case VCMIDLID_UndefinePreprocessorDefinitions:
	default:
 		GetDefaultString( pVal );
		break;
	}
}

// integer props
void CMidlToolOptionHandler::GetDefaultValue( long id, long *pVal, IVCPropertyContainer *pPropCnt )
{
	switch( id )
	{
	case VCMIDLID_WarningLevel:
		*pVal = lmidlWarnLevel;
		break;
	case VCMIDLID_DefaultCharType:
		*pVal = lDefaultCharType;
		break;
	case VCMIDLID_TargetEnvironment:
		*pVal = lTargetEnvironment;
		break;
	case VCMIDLID_EnableErrorChecks:
		*pVal = lEnableErrorChecks;
		break;
	case VCMIDLID_StructMemberAlignment:
		*pVal = lStructMemberAlignment;
		break;
	default:
		*pVal = 0;
		break;
	}
}

// boolean props
void CMidlToolOptionHandler::GetDefaultValue( long id, VARIANT_BOOL *pVal, IVCPropertyContainer *pPropCnt )
{
	switch( id )
	{
	case VCMIDLID_SuppressStartupBanner:
	case VCMIDLID_GenerateTypeLibrary:
		GetValueTrue( pVal );
		break;
// 	case VCMIDLID_IgnoreStandardIncludePath:
// 	case VCMIDLID_MkTypLibCompatible:
// 	case VCMIDLID_WarnAsError:
//	case VCMIDLID_GenerateStublessProxies:
// 	case VCMIDLID_ErrorCheckAllocations:
// 	case VCMIDLID_ErrorCheckBounds:
// 	case VCMIDLID_ErrorCheckEnumRange:
// 	case VCMIDLID_ErrorCheckRefPointers:
// 	case VCMIDLID_ErrorCheckStubData:
// 	case VCMIDLID_ValidateParameters:
	default:
 		GetValueFalse( pVal );
		break;
	}
}


BOOL CMidlToolOptionHandler::SetEvenIfDefault(VARIANT *pvarDefault, long idOption)
{
	CComVariant varDefault;
	BOOL bRet = FALSE;

	switch (idOption)
	{
	case VCMIDLID_SuppressStartupBanner:
	case VCMIDLID_GenerateTypeLibrary:
		varDefault.vt = VT_BOOL;
		varDefault = VARIANT_TRUE;
		bRet = TRUE;
		break;
	case VCMIDLID_TargetEnvironment:
		varDefault = lTargetEnvironment;
		bRet = TRUE;
		break;
	}
	varDefault.Detach(pvarDefault);
	return bRet;
}

BOOL CMidlToolOptionHandler::SynthesizeOptionIfNeeded(IVCPropertyContainer* pPropContainer, long idOption)
{
	switch (idOption)
	{
	case VCMIDLID_TypeLibraryName:
		return CreatesTypeLib(pPropContainer);
	case VCMIDLID_HeaderFileName:
		return TRUE;
	default:
		return FALSE;
	}
}

BOOL CMidlToolOptionHandler::SynthesizeOption(IVCPropertyContainer* pPropContainer, long idOption, CComVariant& rvar)
{
	switch (idOption)
	{
	case VCMIDLID_TypeLibraryName:
		{
			CComBSTR bstrTlb;
			GetDefaultValue( idOption, &bstrTlb );
			rvar = bstrTlb;
			return TRUE;
		}
	case VCMIDLID_HeaderFileName:
		{
			CComBSTR bstrHeader;
			GetDefaultValue( idOption, &bstrHeader );
			rvar = bstrHeader;
			return TRUE;
		}
	default:
		VSASSERT(FALSE, "Mismatch between SynthesizeOptionIfNeeded and SynthesizeOption for MIDL tool");
		return FALSE;
	}
}

BOOL CMidlToolOptionHandler::CreatesTypeLib(IVCPropertyContainer* pPropContainer)
{
	VARIANT_BOOL bGen = VARIANT_TRUE;
	HRESULT hr = pPropContainer->GetBoolProperty(VCMIDLID_GenerateTypeLibrary, &bGen);
	if (hr != S_OK)
		bGen = VARIANT_TRUE;
	return (bGen == VARIANT_TRUE);
}

///////////////////////////////////////////////////////////////////////////////
// Midl Tool
HRESULT CVCMidlTool::CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppMidlTool)
{
	CHECK_POINTER_NULL(ppMidlTool);
	*ppMidlTool = NULL;

	CComObject<CVCMidlTool> *pObj;
	HRESULT hr = CComObject<CVCMidlTool>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		if( pPropContainer )
			pObj->Initialize(pPropContainer);
		CVCMidlTool *pVar = pObj;
		pVar->AddRef();
		*ppMidlTool = pVar;
	}
	return hr;
}

LPCOLESTR CVCMidlTool::GetToolFriendlyName()
{
	InitToolName();
	return s_bstrToolName;
}

void CVCMidlTool::InitToolName()
{
	if (s_bstrToolName.Length() == 0)
	{
		if (!s_bstrToolName.LoadString(IDS_MIDL_TOOLNAME))
			s_bstrToolName = szMidlToolType;
	}
}

STDMETHODIMP CVCMidlTool::GenerateOutput(long type, IVCBuildActionList* plstActions, IVCBuildErrorContext* pEC)
{
	RETURN_ON_NULL2(plstActions, S_OK);

	BOOL bFirst = TRUE;
	CComBSTR bstrProjectDirectory;

	plstActions->Reset(NULL);
	while (TRUE)
	{
		CComPtr<IVCBuildAction> spAction;
		HRESULT hr = plstActions->Next(&spAction, NULL);
		BREAK_ON_DONE(hr);

		CComPtr<IVCBuildableItem> spItem;
		spAction->get_Item(&spItem);
		VSASSERT(spItem != NULL, "Action created without an item.  Bad initialization.");
		CComQIPtr<IVCPropertyContainer> spPropContainer = spItem;
		VSASSERT(spPropContainer != NULL, "Action's item is not a property container.  Bad initialization.");
		RETURN_INVALID_ON_NULL(spPropContainer);

		if (IsExcludedFromBuild(spPropContainer))
			continue;	// nothing to do for things excluded from build...

		if (bFirst)
		{
			bFirst = FALSE;
			spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjectDirectory);	// default if not specified; should never have macros in it
		}

		CStringW strDirName;
		CDirW dirOutput;

		CComBSTR bstrDirName;
		CDirW dirBase;
		CreateDirFromBSTR(bstrProjectDirectory, dirBase);
		if (spPropContainer->GetEvaluatedStrProperty(VCMIDLID_OutputDirectory, &bstrDirName) == S_OK)
		{
			strDirName = bstrDirName;
			if (!strDirName.IsEmpty())
			{
				int i = strDirName.GetLength();
				const wchar_t* pszTmp = _wcsdec((const wchar_t*)strDirName, (const wchar_t*)(strDirName)+i);
				// add trailing slash if necessary
				if (*pszTmp != L'\\' && *pszTmp != L'/')
					strDirName += L'\\';

				strDirName += L'a';
				CPathW tmpPath;
				if (tmpPath.CreateFromDirAndFilename(dirBase, strDirName))
				{
					tmpPath.GetActualCase(TRUE);
					if (dirOutput.CreateFromPath(tmpPath))
					{
						dirBase = dirOutput;
						bstrProjectDirectory.Empty();
						strDirName = dirBase;
						bstrProjectDirectory = strDirName;
					}
				}
			}
		}

		CComPtr<VCFileConfiguration> spFileCfg;
		if (spAction->get_FileConfiguration(&spFileCfg) != S_OK || spFileCfg == NULL)
		{
			CComPtr<VCConfiguration> spProjCfg;
			HRESULT hrT = spAction->get_ProjectConfiguration(&spProjCfg);
			RETURN_ON_FAIL(hrT);
			CHECK_ZOMBIE(spProjCfg, IDS_ERR_TOOL_ZOMBIE);

		 	// assemble all the .tlb files from the configuration
			CComPtr<IEnumVARIANT> spFiles;
			HRESULT hr = CBldAction::GetFileCollection(spProjCfg, &spFiles);
			RETURN_ON_FAIL_OR_NULL(hr, spFiles);

			while (TRUE)
			{
				CComVariant var;
				hr = spFiles->Next(1, &var, NULL);
				BREAK_ON_DONE(hr);
				CComPtr<VCFileConfiguration> spFileCfg;
				if (!CBldAction::GetFileCfgForProjCfg(var, spProjCfg, &spFileCfg))
					continue;
				CComQIPtr<IVCBuildableItem> spBuildableFileCfg = spFileCfg;
				if (spBuildableFileCfg == NULL)
				{
					VSASSERT(FALSE, "File configs must support IVCBuildableItem interface.");
					continue;
				}
				CComPtr<IVCBuildActionList> spFileActions;
				if (FAILED(spBuildableFileCfg->get_ActionList(&spFileActions)))
					continue;
				spFileActions->Reset(NULL);

				while (TRUE)
				{
					CComPtr<IVCBuildAction> spFileAction;
					HRESULT hr2 = spFileActions->Next(&spFileAction, NULL);
					BREAK_ON_DONE(hr2);
					BOOL fFound = FALSE;
					spFileAction->MatchesOldTool(this, &fFound);
					if (fFound)
						GenerateTypeLibDep(spAction, spPropContainer, pEC, dirBase);
				}
			}
		}
		else
		{
			GenerateTypeLibDep(spAction, spPropContainer, pEC, dirBase);

			// now we want to check if "/h filename" is in the option table or not, if it is
			// then we need to add it to the outputs list as well. 
			CComBSTR bstrHdr;
			if (spPropContainer->GetEvaluatedStrProperty(VCMIDLID_HeaderFileName, &bstrHdr) == S_FALSE)
			{	// pick up the default instead, then
				s_optHandler.GetDefaultValue( VCMIDLID_HeaderFileName, &bstrHdr );
				spPropContainer->Evaluate(bstrHdr, &bstrHdr);
			}
			CStringW strHdr = bstrHdr;
			strHdr.TrimLeft();
			if (!strHdr.IsEmpty())
			{
				// ignore failures to add an output (due to invalid value
				// provided by the user) and continue on
				spAction->AddOutputFromFileName(bstrHdr, pEC, L".h", (wchar_t *)(const wchar_t *)dirBase, VARIANT_TRUE,
					VCMIDLID_HeaderFileName, this);
			}

			// now we want to check if "/iid filename" is in the option table or not, if it is
			// then we need to add it to the outputs as well. 
			CComBSTR bstrIid;
			if (spPropContainer->GetEvaluatedStrProperty(VCMIDLID_InterfaceIdentifierFileName, &bstrIid) == S_OK)
			{
				CStringW strIid = bstrIid;
				if (!strIid.IsEmpty())
				{
					// ignore failures to add an output (due to invalid value
					// provided by the user) and continue on
					spAction->AddOutputFromFileName(bstrIid, pEC, L".c", (wchar_t *)(const wchar_t *)dirBase, VARIANT_TRUE,
						VCMIDLID_InterfaceIdentifierFileName, this);
				}
			}

			ConfigurationTypes configType = typeUnknown;
			spPropContainer->GetIntProperty(VCCFGID_ConfigurationType, (long *)&configType);
			BOOL fDll = (configType == typeDynamicLibrary);
			if (fDll)
			{
				CComBSTR bstrDllDataFile;
				CStringW strDllDataFile;
				if (spPropContainer->GetEvaluatedStrProperty(VCMIDLID_DLLDataFileName, &bstrDllDataFile) == S_OK)
				{
					strDllDataFile = bstrDllDataFile;
					if (strDllDataFile.IsEmpty())
						strDllDataFile = L"dlldata.c";
				}
				else
					strDllDataFile = L"dlldata.c";
				CPathW path1;
				path1.CreateFromDirAndFilename(dirBase, strDllDataFile);
				path1.GetActualCase(TRUE);
				spAction->AddOutputFromPath((wchar_t *)(const wchar_t *)path1, pEC, VARIANT_FALSE, VARIANT_TRUE, 
					VCMIDLID_DLLDataFileName, this);
			}

			BOOL bGenDefaultStubName = TRUE;
			CComBSTR bstrProxyStub;
			if (spPropContainer->GetStrProperty(VCMIDLID_ProxyFileName, &bstrProxyStub) == S_OK && bstrProxyStub.Length() > 0)
			{
				if (SUCCEEDED(spPropContainer->Evaluate(bstrProxyStub, &bstrProxyStub)))
				{
					CPathW pathStub;
					if (pathStub.CreateFromDirAndFilename(dirBase, bstrProxyStub))
					{
						bGenDefaultStubName = FALSE;
						pathStub.GetActualCase(TRUE);
						spAction->AddOutputFromPath((wchar_t *)(const wchar_t *)pathStub, pEC, VARIANT_FALSE, VARIANT_TRUE,
							VCMIDLID_ProxyFileName, this);
					}
				}
			}
			if (fDll && bGenDefaultStubName)
			{
				CComQIPtr<IVCPropertyContainer> spFileCfgPropContainer = spFileCfg;
				CComBSTR bstrFileName;
				if (spFileCfgPropContainer != NULL)
				{
					spFileCfgPropContainer->GetEvaluatedStrProperty(VCFILEID_Name, &bstrFileName);
				}
				CStringW strFileName = bstrFileName;
				if (!strFileName.IsEmpty())
				{
					CPathW path2;
					path2.CreateFromDirAndFilename(dirBase, strFileName);
					CStringW strFile = path2.GetFileName();
					CStringW strExt;
					int nExt = strFile.ReverseFind(L'.');
					if (nExt >= 0)
						strFile = strFile.Left(nExt);
					strFile += L"_p.c";
					path2.ChangeFileName(strFile);
					path2.GetActualCase(TRUE);
					spAction->AddOutputFromPath((wchar_t *)(const wchar_t *)path2, pEC, VARIANT_FALSE, VARIANT_TRUE, -1,
						this);
				}
			}
		}
	}

	return S_OK; // success
}

void CVCMidlTool::GenerateTypeLibDep(IVCBuildAction* pAction, IVCPropertyContainer* pPropContainer, IVCBuildErrorContext* pEC, 
	CDirW& dirBase)
{
	// .tlb file is not always generated. check if we're supposed to be generating one
	if (s_optHandler.CreatesTypeLib(pPropContainer))
	{
		CComBSTR bstrTlb;
		if (pPropContainer->GetEvaluatedStrProperty(VCMIDLID_TypeLibraryName, &bstrTlb) == S_FALSE)
		{	// pick up the default instead, then
			s_optHandler.GetDefaultValue( VCMIDLID_TypeLibraryName, &bstrTlb );
			pPropContainer->Evaluate(bstrTlb, &bstrTlb);
		}
		CStringW strTlb = bstrTlb;
		strTlb.TrimLeft();
		if (!strTlb.IsEmpty())
		{
			pAction->AddOutputFromFileName(bstrTlb, pEC, L".tlb", (wchar_t *)(const wchar_t *)dirBase, VARIANT_TRUE,
				VCMIDLID_TypeLibraryName, this);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Affects our output?
STDMETHODIMP CVCMidlTool::AffectsOutput(long nPropID, VARIANT_BOOL* pbAffectsOutput)
{
	CHECK_POINTER_NULL(pbAffectsOutput);
	*pbAffectsOutput = VARIANT_FALSE;

	// change our outputs?
	switch (nPropID)
	{
	case VCMIDLID_InterfaceIdentifierFileName:	// *.c
	case VCMIDLID_TypeLibraryName:				// *.tlb
	case VCMIDLID_OutputDirectory:				// ALL
	case VCMIDLID_HeaderFileName:				// *.h
	case VCMIDLID_OutputsDirty:					// regenerate outptus
	case VCFCFGID_ExcludedFromBuild:			// included/excluded from build
	case VCMIDLID_GenerateTypeLibrary:			// do we generate a .tlb?
		*pbAffectsOutput = VARIANT_TRUE;
		break;
	default:
		*pbAffectsOutput = VARIANT_FALSE;
		break;
	}

	return S_OK;
}

STDMETHODIMP CVCMidlTool::CreatePageObject(IUnknown **ppUnk, CLSID*pCLSID, IVCPropertyContainer *pPropCntr, IVCSettingsPage *pPage )
{
	// Get the list of pages
	if( CLSID_MidlGeneral == *pCLSID )
	{
		CPageObjectImpl< CVCMidlGeneralPage,VCMIDLTOOL_MIN_DISPID,VCMIDLTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else if( CLSID_MidlOutput == *pCLSID )
	{
		CPageObjectImpl< CVCMidlOutputPage,VCMIDLTOOL_MIN_DISPID,VCMIDLTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else if( CLSID_MidlAdvanced == *pCLSID )
	{
		CPageObjectImpl< CVCMidlAdvancedPage,VCMIDLTOOL_MIN_DISPID,VCMIDLTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else
	{
		return S_FALSE;
	}
		
	return S_OK;
}

GUID* CVCMidlTool::GetPageIDs()
{ 
	if (s_bPagesInit == FALSE)
	{
		s_pPages[0] = __uuidof(MidlGeneral);
		s_pPages[1] = __uuidof(MidlOutput);
		s_pPages[2] = __uuidof(MidlAdvanced);
		s_pPages[3] = __uuidof(MidlAdditionalOptions);
		s_bPagesInit = TRUE;
	}
	return s_pPages; 
}

STDMETHODIMP CVCMidlTool::CanScanForDependencies(VARIANT_BOOL* pbIsScannable)
{
	return s_optHandler.GetValueTrue(pbIsScannable);
}

////////////////////////////////////////////////////////////////////
//	Scans files for includes.
STDMETHODIMP CVCMidlTool::GenerateDependencies(IVCBuildAction* pAction, void* pfrhFileCookie, IVCBuildErrorContext* pEC)
{
    	// WARNING (KiP) - Generate Dependencies works on ANSI Files
	// Many of the calls n this function will need to be _mbs calls instead of unicode
	// TCHAR characters and _tcs funtions should not be used because when UNICODE is defined,
	// TCHAR becomes UNICODE and we explicitly want _mbs not matter what.
	// yes, ANSI
	
	BldFileRegHandle	frhFile = (BldFileRegHandle)pfrhFileCookie;

	const char*		pFileNameBegin;
	BOOL			bSkipping = FALSE;
	int			iEntryType;
	char			cDelim;

	HANDLE hMem;
	DWORD dwFileSize;

	const char * pcFileMap;
	const CPathW* pPath = g_FileRegistry.GetRegEntry(frhFile)->GetFilePath();

	if (!g_StaticBuildEngine.OpenMemFile(frhFile, hMem, pcFileMap, dwFileSize, pEC, pAction))
		return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_NOT_FOUND, IDS_ERR_FILE_NOT_FOUND, pPath->GetFullPath());	// couldn't open, continue with next

	const char* pMax;
	int nLine;

	if (pcFileMap == (const char*)NULL)
		goto CloseMemFile;	// no need to open

	pMax = pcFileMap + dwFileSize;
	nLine = 0;

	//	Check for afx hack.  If the file begins with "//{{NO_DEP", don't scan
	if ((dwFileSize > 10) && _mbsncmp((const unsigned char *)pcFileMap, (const unsigned char *)"//{{NO_DEP", 10) == 0)
	{
		pAction->SetDependencyState(depNone);
		goto CloseMemFile;
	}

	do
	{
		nLine++;	// start of a new line.

		// Skip leading blanks
		while (pcFileMap < pMax && _ismbcspace( (const unsigned char )*pcFileMap))
		{
			if (*pcFileMap == '\r')
				nLine++;
			pcFileMap++;
		}

		if (pcFileMap >= pMax)
			goto SkipToEnd;

		 // Check this first so we ignore comments.
		if (*pcFileMap == '/')
		{
			if (bSkipping)
			{
				if (pMax - pcFileMap >= 17 && _mbsncmp((const unsigned char *)pcFileMap, (const unsigned char *)"//}}START_DEPSCAN", 17) == 0)
					bSkipping = FALSE;
			}
			else
			{
				if (pMax - pcFileMap >= 16 && _mbsncmp((const unsigned char *)pcFileMap, (const unsigned char *)"//{{STOP_DEPSCAN", 16) == 0)
					bSkipping = TRUE;
			}
		}
		else if (*pcFileMap == '#' || *pcFileMap == 'i')
		{
			if (bSkipping) 
				goto SkipToEnd;

			// Go past #
			if( *pcFileMap == '#' )
				if (++pcFileMap >= pMax)
					goto SkipToEnd;

			// Skip white space after #
			while (pcFileMap < pMax && _ismbcspace(*pcFileMap))
				pcFileMap++;

			// Look for "include"
			if (pMax - pcFileMap <= 7)
				goto SkipToEnd;

			if (_mbsncmp((const unsigned char *)pcFileMap, (const unsigned char *)"include", 7) == 0)
				pcFileMap += 7;
			else if (_mbsncmp((const unsigned char *)pcFileMap, (const unsigned char *)"import", 6) == 0)
				pcFileMap += 6;
			else
				goto SkipToEnd;

			// Skip white space after include
			while (pcFileMap < pMax && _ismbcspace( *pcFileMap))
				pcFileMap++;

			if (pcFileMap >= pMax)
				goto SkipToEnd;

			// Look for the delimiter either " or <
			if (*pcFileMap == '"')
			{
				iEntryType = IncTypeCheckParentDir |  IncTypeCheckIncludePath | IncTypeCheckOriginalDir;
				cDelim = '"';
			}
			else if (*pcFileMap == '<')
			{
				iEntryType = IncTypeCheckIncludePath;
				cDelim = '>';
			}
			else 
				goto SkipToEnd;

			pcFileMap++;
			if (pcFileMap >= pMax)
				goto SkipToEnd;

			pFileNameBegin = pcFileMap;

			// Look for	other delimeter, or a return if the user screwed up
			while (pcFileMap < pMax && *pcFileMap != cDelim && *pcFileMap != '\r')
				pcFileMap = (const char *)_mbsinc((const unsigned char *)pcFileMap);

			if ( pcFileMap >= pMax || *pcFileMap == '\r')
				goto SkipToEnd;

			// File name now starts at pFileNameBegin and ends at pcFileMap-1.
			int nNameLength =  (int)(pcFileMap - pFileNameBegin);

			CStringA strFileA;	// yes, ANSI -- 'cause IDL files have ANSI contents
			char *pbuf = strFileA.GetBuffer(nNameLength + 1);

			_mbsncpy((unsigned char*)pbuf, (unsigned char*)pFileNameBegin, nNameLength);
			pbuf[nNameLength] = '\0';
			strFileA.ReleaseBuffer();


			// OK Now switch to unicode and add to list
			CStringW strFile;
			strFile = strFileA;
			CVCStringWList strlstIncs;
			CVCStringWList strlstPaths;
			strlstIncs.AddHead(L"\"" + strFile + L"\"");
			if (!ResolveIncludeDirectivesToPath(*pPath, pAction, strlstIncs, strlstPaths))
			{
				pAction->MarkMissingDependencies(TRUE);
				goto CloseMemFile;
			}
			if (strlstPaths.GetCount())
			{
				CStringW strInc = strlstPaths.GetHead();
				CComBSTR bstrInc = strInc;
				HRESULT hr = pAction->AddScannedDependencyOfType(iEntryType, bstrInc, nLine, TRUE, pEC);
				VSASSERT(hr == S_OK, "pAction->AddScannedDependencyOfType returned unexpected value!");
			}
		}

SkipToEnd:

		while (pcFileMap < pMax && *pcFileMap != '\r')
			pcFileMap = (const char *)_mbsinc((const unsigned char *)pcFileMap);

		if (pcFileMap < pMax && *pcFileMap == '\r')
			pcFileMap++;

	} while (pcFileMap < pMax);

CloseMemFile:

	g_StaticBuildEngine.CloseMemFile(hMem, pEC);

	return S_OK;	 		
}		

///////////////////////////////////////////////////////////////////////////////////////
// CVCMidlTool::VCMidlTool
///////////////////////////////////////////////////////////////////////////////////////

// general (/ms_ext, /c_ext left out because they're archaic); @response deliberately left out
STDMETHODIMP CVCMidlTool::get_ToolName(BSTR* pbstrToolName)
{	// friendly name of tool, e.g., "C/C++ Compiler Tool"
	CHECK_POINTER_VALID( pbstrToolName );
	InitToolName();
	s_bstrToolName.CopyTo(pbstrToolName);
	return S_OK;
}

STDMETHODIMP CVCMidlTool::get_AdditionalOptions(BSTR* pbstrAdditionalOptions)
{	// anything we don't know what to do with, given verbatim, in same order as entered, parent contributions first, goes after all other options
	return ToolGetStrProperty(VCMIDLID_AdditionalOptions, pbstrAdditionalOptions);
}

STDMETHODIMP CVCMidlTool::put_AdditionalOptions(BSTR bstrAdditionalOptions)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCMIDLID_AdditionalOptions, bstrAdditionalOptions);
}

STDMETHODIMP CVCMidlTool::get_PreprocessorDefinitions(BSTR* pbstrDefines)
{	// (/D![macro]) define macro for MIDL.  can have multiple.
	return ToolGetStrProperty(VCMIDLID_PreprocessorDefinitions, pbstrDefines, true /* local only */);
}

STDMETHODIMP CVCMidlTool::put_PreprocessorDefinitions(BSTR bstrDefines)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCMIDLID_PreprocessorDefinitions, bstrDefines);
}

STDMETHODIMP CVCMidlTool::get_AdditionalIncludeDirectories(BSTR* pbstrIncludePath)
{	// (/I![path]) directory to add to include path, may have multiple
	return ToolGetStrProperty(VCMIDLID_AdditionalIncludeDirectories, pbstrIncludePath, true /* local only */);
}

STDMETHODIMP CVCMidlTool::put_AdditionalIncludeDirectories(BSTR bstrIncludePath)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCMIDLID_AdditionalIncludeDirectories, bstrIncludePath);
}

STDMETHODIMP CVCMidlTool::get_IgnoreStandardIncludePath(VARIANT_BOOL* pbIgnore)
{	// (/no_def_idir) ignore current and INCLUDE path
	return ToolGetBoolProperty(VCMIDLID_IgnoreStandardIncludePath, pbIgnore);
}

STDMETHODIMP CVCMidlTool::put_IgnoreStandardIncludePath(VARIANT_BOOL bIgnore)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bIgnore );
	return m_spPropertyContainer->SetBoolProperty(VCMIDLID_IgnoreStandardIncludePath, bIgnore);
}

STDMETHODIMP CVCMidlTool::get_MkTypLibCompatible(VARIANT_BOOL* pbCompatible)
{	// (/mktyplib203) forces compatibility with mktyplib.exe version 2.03
	return ToolGetBoolProperty(VCMIDLID_MkTypLibCompatible, pbCompatible);
}

STDMETHODIMP CVCMidlTool::put_MkTypLibCompatible(VARIANT_BOOL bCompatible)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bCompatible );
	return m_spPropertyContainer->SetBoolProperty(VCMIDLID_MkTypLibCompatible, bCompatible);
}

STDMETHODIMP CVCMidlTool::get_WarningLevel(midlWarningLevelOption* poptSetting)
{	// see enum above (/W0 - /W4)
	return ToolGetIntProperty(VCMIDLID_WarningLevel, (long *)poptSetting);
}

STDMETHODIMP CVCMidlTool::put_WarningLevel(midlWarningLevelOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, midlWarnMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCMIDLID_WarningLevel, optSetting);
}

STDMETHODIMP CVCMidlTool::get_WarnAsError(VARIANT_BOOL* pbWarnAsError)
{	// (/WX) treat warnings as errors
	return ToolGetBoolProperty(VCMIDLID_WarnAsError, pbWarnAsError);
}

STDMETHODIMP CVCMidlTool::put_WarnAsError(VARIANT_BOOL bWarnAsError)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bWarnAsError );
	return m_spPropertyContainer->SetBoolProperty(VCMIDLID_WarnAsError, bWarnAsError);
}

STDMETHODIMP CVCMidlTool::get_SuppressStartupBanner(VARIANT_BOOL* pbNoLogo)
{	// (/nologo) enable suppression of copyright message
	return ToolGetBoolProperty(VCMIDLID_SuppressStartupBanner, pbNoLogo);
}

STDMETHODIMP CVCMidlTool::put_SuppressStartupBanner(VARIANT_BOOL bNoLogo)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bNoLogo );
	return m_spPropertyContainer->SetBoolProperty(VCMIDLID_SuppressStartupBanner, bNoLogo);
}

STDMETHODIMP CVCMidlTool::get_DefaultCharType(midlCharOption* poptSetting)
{	// see enum above (/char)
	return ToolGetIntProperty(VCMIDLID_DefaultCharType, (long *)poptSetting);
}

STDMETHODIMP CVCMidlTool::put_DefaultCharType(midlCharOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, charTypeMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCMIDLID_DefaultCharType, optSetting);
}

STDMETHODIMP CVCMidlTool::get_TargetEnvironment(midlTargetEnvironment* poptSetting)
{	// (/env win64 vs. /env win32) target Win64 (or Win32) environment
	return ToolGetIntProperty(VCMIDLID_TargetEnvironment, (long *)poptSetting);
}

STDMETHODIMP CVCMidlTool::put_TargetEnvironment(midlTargetEnvironment optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, envMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCMIDLID_TargetEnvironment, optSetting);
}

STDMETHODIMP CVCMidlTool::get_GenerateStublessProxies(VARIANT_BOOL* poptSetting)
{	// (/Oicf)
	return ToolGetBoolProperty(VCMIDLID_GenerateStublessProxies, poptSetting);
}

STDMETHODIMP CVCMidlTool::put_GenerateStublessProxies(VARIANT_BOOL optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( optSetting );
	return m_spPropertyContainer->SetBoolProperty(VCMIDLID_GenerateStublessProxies, optSetting);
}

// output 
STDMETHODIMP CVCMidlTool::get_GenerateTypeLibrary(VARIANT_BOOL* poptSetting)
{	// (/notlb for no) generate a type library?; default: yes
	return ToolGetBoolProperty(VCMIDLID_GenerateTypeLibrary, poptSetting);
}

STDMETHODIMP CVCMidlTool::put_GenerateTypeLibrary(VARIANT_BOOL optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL(optSetting);
	return m_spPropertyContainer->SetBoolProperty(VCMIDLID_GenerateTypeLibrary, optSetting);
}

STDMETHODIMP CVCMidlTool::get_OutputDirectory(BSTR* pbstrOut)
{	// (/out dir) directory to place output files in; default: project directory
	return ToolGetStrProperty(VCMIDLID_OutputDirectory, pbstrOut);
}

STDMETHODIMP CVCMidlTool::put_OutputDirectory(BSTR bstrOut)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCMIDLID_OutputDirectory, bstrOut);
}

STDMETHODIMP CVCMidlTool::get_HeaderFileName(BSTR* pbstrHeaderFile)
{	// (/h[eader] filename) specify name of header file generated; default: <idlfile>.h
	return ToolGetStrProperty(VCMIDLID_HeaderFileName, pbstrHeaderFile);
}

STDMETHODIMP CVCMidlTool::put_HeaderFileName(BSTR bstrHeaderFile)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCMIDLID_HeaderFileName, bstrHeaderFile);
}

STDMETHODIMP CVCMidlTool::get_DLLDataFileName(BSTR* pbstrDLLData)
{	// (/dlldata filename) specify name of DLLDATA file generated; default: dlldata.c
	return ToolGetStrProperty(VCMIDLID_DLLDataFileName, pbstrDLLData);
}

STDMETHODIMP CVCMidlTool::put_DLLDataFileName(BSTR bstrDLLData)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCMIDLID_DLLDataFileName, bstrDLLData);
}

STDMETHODIMP CVCMidlTool::get_InterfaceIdentifierFileName(BSTR* pbstrIID)
{	// (/iid filename) specify name of IID file; default: <idlfile>_i.c
	return ToolGetStrProperty(VCMIDLID_InterfaceIdentifierFileName, pbstrIID);
}

STDMETHODIMP CVCMidlTool::put_InterfaceIdentifierFileName(BSTR bstrIID)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCMIDLID_InterfaceIdentifierFileName, bstrIID);
}

STDMETHODIMP CVCMidlTool::get_ProxyFileName(BSTR* pbstrProxyFile)
{	// (/proxy filename) specify name of proxy file; default: <idlfile>_p.c
	return ToolGetStrProperty(VCMIDLID_ProxyFileName, pbstrProxyFile);
}

STDMETHODIMP CVCMidlTool::put_ProxyFileName(BSTR bstrProxyFile)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCMIDLID_ProxyFileName, bstrProxyFile);
}

STDMETHODIMP CVCMidlTool::get_TypeLibraryName(BSTR* pbstrTLBFile)
{	// (/tlb filename) specify name of TLB file; default: <idlfile>.tlb
	return ToolGetStrProperty(VCMIDLID_TypeLibraryName, pbstrTLBFile);
}

STDMETHODIMP CVCMidlTool::put_TypeLibraryName(BSTR bstrTLBFile)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCMIDLID_TypeLibraryName, bstrTLBFile);
}

// Advanced
STDMETHODIMP CVCMidlTool::get_EnableErrorChecks(midlErrorCheckOption* poptSetting)
{	// see enum above (/error {none|all}) 
	return ToolGetIntProperty(VCMIDLID_EnableErrorChecks, (long *)poptSetting);
}

STDMETHODIMP CVCMidlTool::put_EnableErrorChecks(midlErrorCheckOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, errCheckMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCMIDLID_EnableErrorChecks, optSetting);
}

STDMETHODIMP CVCMidlTool::get_ErrorCheckAllocations(VARIANT_BOOL* pbErrorCheck)
{	// (/error allocation) check for out of memory errors
	return ToolGetBoolProperty(VCMIDLID_ErrorCheckAllocations, pbErrorCheck);
}

STDMETHODIMP CVCMidlTool::put_ErrorCheckAllocations(VARIANT_BOOL bErrorCheck)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bErrorCheck );
	return m_spPropertyContainer->SetBoolProperty(VCMIDLID_ErrorCheckAllocations, bErrorCheck);
}

STDMETHODIMP CVCMidlTool::get_ErrorCheckBounds(VARIANT_BOOL* pbErrorCheck)
{	// (/error bounds_check) check size	vs. transmission length specifications
	return ToolGetBoolProperty(VCMIDLID_ErrorCheckBounds, pbErrorCheck);
}

STDMETHODIMP CVCMidlTool::put_ErrorCheckBounds(VARIANT_BOOL bErrorCheck)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bErrorCheck );
	return m_spPropertyContainer->SetBoolProperty(VCMIDLID_ErrorCheckBounds, bErrorCheck);
}

STDMETHODIMP CVCMidlTool::get_ErrorCheckEnumRange(VARIANT_BOOL* pbErrorCheck)
{	// (/error enum) check enum values to be in allowable range not in docs
	return ToolGetBoolProperty(VCMIDLID_ErrorCheckEnumRange, pbErrorCheck);
}

STDMETHODIMP CVCMidlTool::put_ErrorCheckEnumRange(VARIANT_BOOL bErrorCheck)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bErrorCheck );
	return m_spPropertyContainer->SetBoolProperty(VCMIDLID_ErrorCheckEnumRange, bErrorCheck);
}

STDMETHODIMP CVCMidlTool::get_ErrorCheckRefPointers(VARIANT_BOOL* pbErrorCheck)
{	// (/error ref) check ref pointers to be non-NULL
	return ToolGetBoolProperty(VCMIDLID_ErrorCheckRefPointers, pbErrorCheck);
}

STDMETHODIMP CVCMidlTool::put_ErrorCheckRefPointers(VARIANT_BOOL bErrorCheck)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bErrorCheck );
	return m_spPropertyContainer->SetBoolProperty(VCMIDLID_ErrorCheckRefPointers, bErrorCheck);
}

STDMETHODIMP CVCMidlTool::get_ErrorCheckStubData(VARIANT_BOOL* pbErrorCheck)
{	// (/error stub_data) emit additional check for server side data stub validity
	return ToolGetBoolProperty(VCMIDLID_ErrorCheckStubData, pbErrorCheck);
}

STDMETHODIMP CVCMidlTool::put_ErrorCheckStubData(VARIANT_BOOL bErrorCheck)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bErrorCheck );
	return m_spPropertyContainer->SetBoolProperty(VCMIDLID_ErrorCheckStubData, bErrorCheck);
}

STDMETHODIMP CVCMidlTool::get_ValidateParameters(VARIANT_BOOL* pbValidate)
{	// (/robust) generate additional information to validate parameters not in docs
	return ToolGetBoolProperty(VCMIDLID_ValidateParameters, pbValidate);
}

STDMETHODIMP CVCMidlTool::put_ValidateParameters(VARIANT_BOOL bValidate)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bValidate );
	return m_spPropertyContainer->SetBoolProperty(VCMIDLID_ValidateParameters, bValidate);
}

STDMETHODIMP CVCMidlTool::get_StructMemberAlignment(midlStructMemberAlignOption* poptSetting)
{	// see enum above (/Zp[num])
	return ToolGetIntProperty(VCMIDLID_StructMemberAlignment, (long *)poptSetting);
}

STDMETHODIMP CVCMidlTool::put_StructMemberAlignment(midlStructMemberAlignOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, midlStructAlignMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCMIDLID_StructMemberAlignment, optSetting);
}

STDMETHODIMP CVCMidlTool::get_RedirectOutputAndErrors(BSTR* pbstrOutput)
{	// (/o file) redirect output from screen to a file
	return ToolGetStrProperty(VCMIDLID_RedirectOutputAndErrors, pbstrOutput);
}

STDMETHODIMP CVCMidlTool::put_RedirectOutputAndErrors(BSTR bstrOutput)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCMIDLID_RedirectOutputAndErrors, bstrOutput);
}

STDMETHODIMP CVCMidlTool::get_CPreprocessOptions(BSTR* pbstrOpt)
{	// (/cpp_opt"[option]") C-compiler preprocessor option to pass to MIDL.  must be in exact format for desired switches
	return ToolGetStrProperty(VCMIDLID_CPreprocessOptions, pbstrOpt, true /* local only */);
}

STDMETHODIMP CVCMidlTool::put_CPreprocessOptions(BSTR bstrOpt)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCMIDLID_CPreprocessOptions, bstrOpt);
}

STDMETHODIMP CVCMidlTool::get_UndefinePreprocessorDefinitions(BSTR* pbstrUndefines)
{	// (/U![macro]) remove any previous definition of symbol.  can have multiple.
	return ToolGetStrProperty(VCMIDLID_UndefinePreprocessorDefinitions, pbstrUndefines, true /* local only */);
}

STDMETHODIMP CVCMidlTool::put_UndefinePreprocessorDefinitions(BSTR bstrUndefines)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCMIDLID_UndefinePreprocessorDefinitions, bstrUndefines);
}

// Automation properties
STDMETHODIMP CVCMidlTool::get_ToolPath(BSTR *pVal)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	*pVal = SysAllocString( szMidlToolPath );
	return S_OK;
}

STDMETHODIMP CVCMidlTool::get_PropertyOption(BSTR bstrProp, long dispidProp, BSTR *pVal)
{ 
	return DoGetPropertyOption(bstrProp, dispidProp, pVal); 
}

STDMETHODIMP CVCMidlTool::get_FullIncludePath(BSTR* fullIncludePath)
{	// include path, including all inherited values, plus platform includes
	return get_FullIncludePathInternal(fullIncludePath);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// CVCMidlTool::IVCToolImpl
/////////////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CVCMidlTool::get_DefaultExtensions(BSTR* pVal)
{
	return DoGetDefaultExtensions(s_bstrExtensions, wszMidlDefaultExtensions, pVal);
}

STDMETHODIMP CVCMidlTool::put_DefaultExtensions(BSTR newVal)
{
	s_bstrExtensions = newVal;
	return S_OK;
}

STDMETHODIMP CVCMidlTool::GetAdditionalOptionsInternal(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, 
	BSTR* pbstrAdditionalOptions)
{ 
	return GetProcessedAdditionalOptions(pItem, VCMIDLID_AdditionalOptions, bForBuild, bSkipLocal, pbstrAdditionalOptions);
}

STDMETHODIMP CVCMidlTool::GetBuildDescription(IVCBuildAction* pAction, BSTR* pbstrBuildDescription)
{
	if (s_bstrBuildDescription.Length() == 0)
		s_bstrBuildDescription.LoadString(IDS_DESC_MKTYPLIB_COMPILING);
	return s_bstrBuildDescription.CopyTo(pbstrBuildDescription);
}

STDMETHODIMP CVCMidlTool::get_ToolPathInternal(BSTR* pbstrToolPath)
{ 
	*pbstrToolPath = SysAllocString( szMidlToolPath );
	return S_OK;
}

STDMETHODIMP CVCMidlTool::get_Bucket(long *pVal)
{
	CHECK_POINTER_NULL( pVal );
	*pVal = BUCKET_MIDL;
	return S_OK;
}

STDMETHODIMP CVCMidlTool::CreateToolObject(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject)
{ 
	return CreateInstance(pPropContainer, ppToolObject); 
}

STDMETHODIMP CVCMidlTool::GetAdditionalIncludeDirectoriesInternal(IVCPropertyContainer* pPropContainer, BSTR* pbstrIncDirs)
{
	CHECK_POINTER_NULL(pbstrIncDirs);
	CHECK_READ_POINTER_NULL(pPropContainer);

	CComBSTR bstrTmp;
	HRESULT hr = pPropContainer->GetMultiProp(VCMIDLID_AdditionalIncludeDirectories, L";", VARIANT_FALSE, &bstrTmp);
	if (SUCCEEDED(hr))
		pPropContainer->Evaluate(bstrTmp, pbstrIncDirs);
	else
		*pbstrIncDirs = bstrTmp.Detach();
	return hr;
}

STDMETHODIMP CVCMidlTool::get_IncludePathID(long* pnIncludeID)
{
	CHECK_POINTER_NULL(pnIncludeID);
	*pnIncludeID = VCMIDLID_AdditionalIncludeDirectories;
	return S_OK;
}

STDMETHODIMP CVCMidlTool::get_ToolShortName(BSTR* pbstrToolName)
{
	*pbstrToolName = SysAllocString( szMidlToolShortName );
	return S_OK;
}

STDMETHODIMP CVCMidlTool::MatchName(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches)
{
	return DoMatchName(bstrName, szMidlToolType, szMidlToolShortName, pbMatches);
}

// settings pages
/////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CVCMidlGeneralPage::get_PreprocessorDefinitions(BSTR* pbstrDefines)
{	// (/D![macro]) define macro for MIDL.  can have multiple.
	return ToolGetStrProperty(VCMIDLID_PreprocessorDefinitions, &(CVCMidlTool::s_optHandler), pbstrDefines, true /* local only */);
}

STDMETHODIMP CVCMidlGeneralPage::put_PreprocessorDefinitions(BSTR bstrDefines)
{
	return SetStrProperty(VCMIDLID_PreprocessorDefinitions, bstrDefines);
}

STDMETHODIMP CVCMidlGeneralPage::get_AdditionalIncludeDirectories(BSTR* pbstrIncludePath)
{	// (/I![path]) directory to add to include path, may have multiple
	return ToolGetStrProperty(VCMIDLID_AdditionalIncludeDirectories, &(CVCMidlTool::s_optHandler), pbstrIncludePath, true /* local only */);
}

STDMETHODIMP CVCMidlGeneralPage::put_AdditionalIncludeDirectories(BSTR bstrIncludePath)
{
	return SetStrProperty(VCMIDLID_AdditionalIncludeDirectories, bstrIncludePath);
}

STDMETHODIMP CVCMidlGeneralPage::get_IgnoreStandardIncludePath(enumMidlIgnoreStandardIncludePathBOOL* pbIgnore)
{	// (/no_def_idir) ignore current and INCLUDE path
	return GetEnumBoolProperty2(VCMIDLID_IgnoreStandardIncludePath, (long *)pbIgnore);
}

STDMETHODIMP CVCMidlGeneralPage::put_IgnoreStandardIncludePath(enumMidlIgnoreStandardIncludePathBOOL bIgnore)
{
	return SetBoolProperty(VCMIDLID_IgnoreStandardIncludePath, bIgnore);
}

STDMETHODIMP CVCMidlGeneralPage::get_MkTypLibCompatible(enumMkTypLibCompatibleBOOL* pbCompatible)
{	// (/mktyplib203) forces compatibility with mktyplib.exe version 2.03
	return GetEnumBoolProperty2(VCMIDLID_MkTypLibCompatible, (long *)pbCompatible);
}

STDMETHODIMP CVCMidlGeneralPage::put_MkTypLibCompatible(enumMkTypLibCompatibleBOOL bCompatible)
{
	return SetBoolProperty(VCMIDLID_MkTypLibCompatible, bCompatible);
}

STDMETHODIMP CVCMidlGeneralPage::get_WarningLevel(midlWarningLevelOption* poptSetting)
{	// see enum above (/W0 - /W4)
	return ToolGetIntProperty(VCMIDLID_WarningLevel, &(CVCMidlTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCMidlGeneralPage::put_WarningLevel(midlWarningLevelOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, midlWarnMax, optSetting);
	return SetIntProperty(VCMIDLID_WarningLevel, optSetting);
}

STDMETHODIMP CVCMidlGeneralPage::get_WarnAsError(enumWarnAsErrorBOOL* pbWarnAsError)
{	// (/WX) treat warnings as errors
	return GetEnumBoolProperty2(VCMIDLID_WarnAsError, (long *)pbWarnAsError);
}

STDMETHODIMP CVCMidlGeneralPage::put_WarnAsError(enumWarnAsErrorBOOL bWarnAsError)
{
	return SetBoolProperty(VCMIDLID_WarnAsError, bWarnAsError);
}

STDMETHODIMP CVCMidlGeneralPage::get_SuppressStartupBanner(enumSuppressStartupBannerBOOL* pbNoLogo)
{	// (/nologo) enable suppression of copyright message
	return GetEnumBoolProperty2(VCMIDLID_SuppressStartupBanner, (long *)pbNoLogo);
}

STDMETHODIMP CVCMidlGeneralPage::put_SuppressStartupBanner(enumSuppressStartupBannerBOOL bNoLogo)
{
	return SetBoolProperty(VCMIDLID_SuppressStartupBanner, bNoLogo);
}

STDMETHODIMP CVCMidlGeneralPage::get_DefaultCharType(midlCharOption* poptSetting)
{	// see enum above (/char)
	return ToolGetIntProperty(VCMIDLID_DefaultCharType, &(CVCMidlTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCMidlGeneralPage::put_DefaultCharType(midlCharOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, charTypeMax, optSetting);
	return SetIntProperty(VCMIDLID_DefaultCharType, optSetting);
}

STDMETHODIMP CVCMidlGeneralPage::get_TargetEnvironment(midlTargetEnvironment* poptSetting)
{	// (/env win64 vs. /env win32) target Win64 (or Win32) environment
	return ToolGetIntProperty(VCMIDLID_TargetEnvironment, &(CVCMidlTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCMidlGeneralPage::put_TargetEnvironment(midlTargetEnvironment optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, envMax, optSetting);
	return SetIntProperty(VCMIDLID_TargetEnvironment, optSetting);
}

STDMETHODIMP CVCMidlGeneralPage::get_GenerateStublessProxies(enumGenerateStublessProxiesBOOL* poptSetting)
{	// (/Oicf)
	return GetEnumBoolProperty2(VCMIDLID_GenerateStublessProxies, (long *)poptSetting);
}

STDMETHODIMP CVCMidlGeneralPage::put_GenerateStublessProxies(enumGenerateStublessProxiesBOOL optSetting)
{
	return SetBoolProperty(VCMIDLID_GenerateStublessProxies, optSetting);
}

void CVCMidlGeneralPage::GetBaseDefault(long id, CComVariant& varValue)
{
	if (id == VCMIDLID_SuppressStartupBanner)
	{
		VARIANT_BOOL bVal;
		CVCMidlTool::s_optHandler.GetDefaultValue( id, &bVal );
		varValue = bVal;
	}
	else
		CBase::GetBaseDefault(id, varValue);
}

// output 
STDMETHODIMP CVCMidlOutputPage::get_GenerateTypeLibrary(enumGenerateTypeLibraryBOOL* poptSetting)
{	// (/notlb) generate a type library or not; default is yes
	return GetEnumBoolProperty2(VCMIDLID_GenerateTypeLibrary, (long *)poptSetting);
}

STDMETHODIMP CVCMidlOutputPage::put_GenerateTypeLibrary(enumGenerateTypeLibraryBOOL optSetting)
{
	return SetBoolProperty(VCMIDLID_GenerateTypeLibrary, optSetting);
}

STDMETHODIMP CVCMidlOutputPage::get_OutputDirectory(BSTR* pbstrOut)
{	// (/out dir) directory to place output files in; default: project directory
	return ToolGetStrProperty(VCMIDLID_OutputDirectory, &(CVCMidlTool::s_optHandler), pbstrOut);
}

STDMETHODIMP CVCMidlOutputPage::put_OutputDirectory(BSTR bstrOut)
{
	return SetStrProperty(VCMIDLID_OutputDirectory, bstrOut);
}

STDMETHODIMP CVCMidlOutputPage::get_HeaderFileName(BSTR* pbstrHeaderFile)
{	// (/h[eader] filename) specify name of header file generated; default: <idlfile>.h
	return ToolGetStrProperty(VCMIDLID_HeaderFileName, &(CVCMidlTool::s_optHandler), pbstrHeaderFile);
}

STDMETHODIMP CVCMidlOutputPage::put_HeaderFileName(BSTR bstrHeaderFile)
{
	return SetStrProperty(VCMIDLID_HeaderFileName, bstrHeaderFile);
}

STDMETHODIMP CVCMidlOutputPage::get_DLLDataFileName(BSTR* pbstrDLLData)
{	// (/dlldata filename) specify name of DLLDATA file generated; default: dlldata.c
	return ToolGetStrProperty(VCMIDLID_DLLDataFileName, &(CVCMidlTool::s_optHandler), pbstrDLLData);
}

STDMETHODIMP CVCMidlOutputPage::put_DLLDataFileName(BSTR bstrDLLData)
{
	return SetStrProperty(VCMIDLID_DLLDataFileName, bstrDLLData);
}

STDMETHODIMP CVCMidlOutputPage::get_InterfaceIdentifierFileName(BSTR* pbstrIID)
{	// (/iid filename) specify name of IID file; default: <idlfile>_i.c
	return ToolGetStrProperty(VCMIDLID_InterfaceIdentifierFileName, &(CVCMidlTool::s_optHandler), pbstrIID);
}

STDMETHODIMP CVCMidlOutputPage::put_InterfaceIdentifierFileName(BSTR bstrIID)
{
	return SetStrProperty(VCMIDLID_InterfaceIdentifierFileName, bstrIID);
}

STDMETHODIMP CVCMidlOutputPage::get_ProxyFileName(BSTR* pbstrProxyFile)
{	// (/proxy filename) specify name of proxy file; default: <idlfile>_p.c
	return ToolGetStrProperty(VCMIDLID_ProxyFileName, &(CVCMidlTool::s_optHandler), pbstrProxyFile);
}

STDMETHODIMP CVCMidlOutputPage::put_ProxyFileName(BSTR bstrProxyFile)
{
	return SetStrProperty(VCMIDLID_ProxyFileName, bstrProxyFile);
}

STDMETHODIMP CVCMidlOutputPage::get_TypeLibraryName(BSTR* pbstrTLBFile)
{	// (/tlb filename) specify name of TLB file; default: <idlfile>.tlb
	return ToolGetStrProperty(VCMIDLID_TypeLibraryName, &(CVCMidlTool::s_optHandler), pbstrTLBFile);
}

STDMETHODIMP CVCMidlOutputPage::put_TypeLibraryName(BSTR bstrTLBFile)
{
	return SetStrProperty(VCMIDLID_TypeLibraryName, bstrTLBFile);
}

void CVCMidlOutputPage::GetBaseDefault(long id, CComVariant& varValue)
{
	CComBSTR bstrVal;
	VARIANT_BOOL bVal;
	switch (id)
	{
	case VCMIDLID_HeaderFileName:
	case VCMIDLID_DLLDataFileName:
	case VCMIDLID_TypeLibraryName:
		CVCMidlTool::s_optHandler.GetDefaultValue( id, &bstrVal );
		varValue = bstrVal;
		break;
	case VCMIDLID_GenerateTypeLibrary:
		CVCMidlTool::s_optHandler.GetDefaultValue(VCMIDLID_GenerateTypeLibrary, &bVal);
		varValue = bVal;
		break;
	default:
		CBase::GetBaseDefault(id, varValue);
		return;
	}

}

// Advanced
STDMETHODIMP CVCMidlAdvancedPage::get_EnableErrorChecks(midlErrorCheckOption* poptSetting)
{	// see enum above (/error {none|all}) 
	return ToolGetIntProperty(VCMIDLID_EnableErrorChecks, &(CVCMidlTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCMidlAdvancedPage::put_EnableErrorChecks(midlErrorCheckOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, errCheckMax, optSetting);
	return SetIntProperty(VCMIDLID_EnableErrorChecks, optSetting);
}

STDMETHODIMP CVCMidlAdvancedPage::get_ErrorCheckAllocations(enumErrorCheckAllocationsBOOL* pbErrorCheck)
{	// (/error allocation) check for out of memory errors
	return GetEnumBoolProperty2(VCMIDLID_ErrorCheckAllocations, (long *)pbErrorCheck);
}

STDMETHODIMP CVCMidlAdvancedPage::put_ErrorCheckAllocations(enumErrorCheckAllocationsBOOL bErrorCheck)
{
	return SetBoolProperty(VCMIDLID_ErrorCheckAllocations, bErrorCheck);
}

STDMETHODIMP CVCMidlAdvancedPage::get_ErrorCheckBounds(enumErrorCheckBoundsBOOL* pbErrorCheck)
{	// (/error bounds_check) check size	vs. transmission length specifications
	return GetEnumBoolProperty2(VCMIDLID_ErrorCheckBounds, (long *)pbErrorCheck);
}

STDMETHODIMP CVCMidlAdvancedPage::put_ErrorCheckBounds(enumErrorCheckBoundsBOOL bErrorCheck)
{
	return SetBoolProperty(VCMIDLID_ErrorCheckBounds, bErrorCheck);
}

STDMETHODIMP CVCMidlAdvancedPage::get_ErrorCheckEnumRange(enumErrorCheckEnumRangeBOOL* pbErrorCheck)
{	// (/error enum) check enum values to be in allowable range not in docs
	return GetEnumBoolProperty2(VCMIDLID_ErrorCheckEnumRange, (long *)pbErrorCheck);
}

STDMETHODIMP CVCMidlAdvancedPage::put_ErrorCheckEnumRange(enumErrorCheckEnumRangeBOOL bErrorCheck)
{
	return SetBoolProperty(VCMIDLID_ErrorCheckEnumRange, bErrorCheck);
}

STDMETHODIMP CVCMidlAdvancedPage::get_ErrorCheckRefPointers(enumErrorCheckRefPointersBOOL* pbErrorCheck)
{	// (/error ref) check ref pointers to be non-NULL
	return GetEnumBoolProperty2(VCMIDLID_ErrorCheckRefPointers, (long *)pbErrorCheck);
}

STDMETHODIMP CVCMidlAdvancedPage::put_ErrorCheckRefPointers(enumErrorCheckRefPointersBOOL bErrorCheck)
{
	return SetBoolProperty(VCMIDLID_ErrorCheckRefPointers, bErrorCheck);
}

STDMETHODIMP CVCMidlAdvancedPage::get_ErrorCheckStubData(enumErrorCheckStubDataBOOL* pbErrorCheck)
{	// (/error stub_data) emit additional check for server side data stub validity
	return GetEnumBoolProperty2(VCMIDLID_ErrorCheckStubData, (long *)pbErrorCheck);
}

STDMETHODIMP CVCMidlAdvancedPage::put_ErrorCheckStubData(enumErrorCheckStubDataBOOL bErrorCheck)
{
	return SetBoolProperty(VCMIDLID_ErrorCheckStubData, bErrorCheck);
}

STDMETHODIMP CVCMidlAdvancedPage::get_ValidateParameters(enumValidateParametersBOOL* pbValidate)
{	// (/robust) generate additional information to validate parameters not in docs
	return GetEnumBoolProperty2(VCMIDLID_ValidateParameters, (long *)pbValidate);
}

STDMETHODIMP CVCMidlAdvancedPage::put_ValidateParameters(enumValidateParametersBOOL bValidate)
{
	return SetBoolProperty(VCMIDLID_ValidateParameters, bValidate);
}

STDMETHODIMP CVCMidlAdvancedPage::get_StructMemberAlignment(midlStructMemberAlignOption* poptSetting)
{	// see enum above (/Zp[num])
	return ToolGetIntProperty(VCMIDLID_StructMemberAlignment, &(CVCMidlTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCMidlAdvancedPage::put_StructMemberAlignment(midlStructMemberAlignOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, midlStructAlignMax, optSetting);
	return SetIntProperty(VCMIDLID_StructMemberAlignment, optSetting);
}

STDMETHODIMP CVCMidlAdvancedPage::get_RedirectOutputAndErrors(BSTR* pbstrOutput)
{	// (/o file) redirect output from screen to a file
	return ToolGetStrProperty(VCMIDLID_RedirectOutputAndErrors, &(CVCMidlTool::s_optHandler), pbstrOutput);
}

STDMETHODIMP CVCMidlAdvancedPage::put_RedirectOutputAndErrors(BSTR bstrOutput)
{
	return SetStrProperty(VCMIDLID_RedirectOutputAndErrors, bstrOutput);
}

STDMETHODIMP CVCMidlAdvancedPage::get_CPreprocessOptions(BSTR* pbstrOpt)
{	// (/cpp_opt"[option]") C-compiler Advanced option to pass to MIDL.  must be in exact format for desired switches
	return ToolGetStrProperty(VCMIDLID_CPreprocessOptions, &(CVCMidlTool::s_optHandler), pbstrOpt, true /* local only */);
}

STDMETHODIMP CVCMidlAdvancedPage::put_CPreprocessOptions(BSTR bstrOpt)
{
	return SetStrProperty(VCMIDLID_CPreprocessOptions, bstrOpt);
}

STDMETHODIMP CVCMidlAdvancedPage::get_UndefinePreprocessorDefinitions(BSTR* pbstrUndefines)
{	// (/U![macro]) remove any previous definition of symbol.  can have multiple.
	return ToolGetStrProperty(VCMIDLID_UndefinePreprocessorDefinitions, &(CVCMidlTool::s_optHandler), pbstrUndefines, true /* local only */);
}

STDMETHODIMP CVCMidlAdvancedPage::put_UndefinePreprocessorDefinitions(BSTR bstrUndefines)
{
	return SetStrProperty(VCMIDLID_UndefinePreprocessorDefinitions, bstrUndefines);
}
