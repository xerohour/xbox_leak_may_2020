// LinkTool.cpp : Implementation of Linker Tool

#include "stdafx.h"
#include "LinkTool.h"
#include "FileRegistry.h"
#include "BldHelpers.h"
#include "BldActions.h"
#include "stylesheet.h"
#include "ProjWriter.h"

// constants
const wchar_t* const wszLinkDefaultExtensions = L"*.obj;*.res;*.lib;*.rsc";	// remember to update vcpb.rgs if you change this...
const wchar_t* const wszLinkImpLibName = L"$(OutDir)/$(TargetName).lib";
const wchar_t* const wszLinkPDBFile = L"$(OutDir)/$(ProjectName).pdb";
const wchar_t* wszLinkerDelOnRebuildExtensions = L"exe;dll;exp;map;lib;ilk;pdb;ocx;pkg";

const linkProgressOption lShowProgress = linkProgressNotSet;
const long lnTypeLibraryResourceID = 1;
const subSystemOption lSubSystem = subSystemNotSet;
const long lnHeapReserveSize = 0;
const long lnHeapCommitSize = 0;
const long lnStackReserveSize = 0;
const long lnStackCommitSize = 0;
const machineTypeOption lTargetMachine = machineNotSet;

// static initializers
CLinkerToolOptionHandler CVCLinkerTool::s_optHandler;
CComBSTR CVCLinkerTool::s_bstrBuildDescription = L"";
CComBSTR CVCLinkerTool::s_bstrToolName = L"";
CComBSTR CVCLinkerTool::s_bstrExtensions = L"";
GUID CVCLinkerTool::s_pPages[8];
BOOL CVCLinkerTool::s_bPagesInit;


////////////////////////////////////////////////////////////////////////////////
// Option Table for the Linker Tool switches

// NOTE: we're using macros for enum value ranges to help in keeping enum range checks as low maintenance as possible.
// If you add an enum to this table, make sure you use macros to define the upper and lower bounds and use CHECK_BOUNDS
// on the get/put methods associated with that property (both object model and property page).
// WARNING: if you change ANYTHING about a multi-prop here (including add/remove), be sure to update how the property
// is obtained on both the tool and page objects.
BEGIN_OPTION_TABLE(CLinkerToolOptionHandler, L"VCLinkerTool", IDS_Link, TRUE /* pre & post */, FALSE /* case sensitive */)
	// general
	OPT_ENUM(ShowProgress,		L"|VERBOSE|VERBOSE:LIB", StdEnumMin, ShowProgressMax,	L"ShowProgress", IDS_LinkGeneral, VCLINKID)
	OPT_BSTR(OutputFile,		L"OUT:%s",						L"OutputFile", single,				IDS_LinkGeneral, VCLINKID)
	OPT_BSTR_NOQUOTE(Version,	L"VERSION:%s",					L"Version",			single,			IDS_LinkGeneral, VCLINKID)
	OPT_ENUM(LinkIncremental,	L"|INCREMENTAL:NO|INCREMENTAL", StdEnumMin, LinkIncrMax, L"LinkIncremental", IDS_LinkGeneral, VCLINKID)
	OPT_BOOL(SuppressStartupBanner, L"|NOLOGO",					L"SuppressStartupBanner",			IDS_LinkGeneral, VCLINKID)
	OPT_BSTR(AdditionalLibraryDirectories, L"LIBPATH:%s",		L"AdditionalLibraryDirectories",	multipleNoCase,		IDS_LinkGeneral, VCLINKID)
	OPT_BOOL(LinkDLL,			L"|DLL",						L"LinkDLL",							SPECIAL_HYPERLINK, VCLINKID)
	// input
	OPT_BOOL(IgnoreAllDefaultLibraries, L"|NODEFAULTLIB",		L"IgnoreAllDefaultLibraries",		IDS_LinkInput, VCLINKID)
	OPT_BSTR(IgnoreDefaultLibraryNames, L"NODEFAULTLIB:%s",		L"IgnoreDefaultLibraryNames", multipleNoCase, IDS_LinkInput, VCLINKID)
	OPT_BSTR(ModuleDefinitionFile, L"DEF:%s",					L"ModuleDefinitionFile",		single, IDS_LinkInput, VCLINKID)
	OPT_BSTR(AddModuleNamesToAssembly, L"ASSEMBLYMODULE:%s",	L"AddModuleNamesToAssembly",	multipleNoCase, IDS_LinkInput, VCLINKID)
	OPT_BSTR(EmbedManagedResourceFile, L"ASSEMBLYRESOURCE:%s",		L"EmbedManagedResourceFile",		multipleNoCase, IDS_LinkInput, VCLINKID)
	OPT_BSTR(ForceSymbolReferences, L"INCLUDE:%s",				L"ForceSymbolReferences", multiple,		IDS_LinkInput, VCLINKID)
	OPT_BSTR(DelayLoadDLLs,		L"DELAYLOAD:%s",				L"DelayLoadDLLs", multipleNoCase,	IDS_LinkInput, VCLINKID)
	// MIDL
	OPT_BSTR_SPECIAL(MidlCommandFile,	L"MIDL:@%s",			L"MidlCommandFile",	single,			IDS_LinkMIDL, VCLINKID)
	OPT_BOOL(IgnoreEmbeddedIDL,	L"|IGNOREIDL",					L"IgnoreEmbeddedIDL",				IDS_LinkMIDL, VCLINKID)
	OPT_BSTR(MergedIDLBaseFileName,	L"IDLOUT:%s",				L"MergedIDLBaseFileName",	single,	IDS_LinkMIDL, VCLINKID)
	OPT_BSTR(TypeLibraryFile,	L"TLBOUT:%s",					L"TypeLibraryFile",		single,		IDS_LinkMIDL, VCLINKID)
	OPT_INT_NOZERO(TypeLibraryResourceID, L"TLBID:%d",			L"TypeLibraryResourceID",			IDS_LinkMIDL, VCLINKID)
	// debug
	OPT_BOOL(GenerateDebugInformation, L"|DEBUG",				L"GenerateDebugInformation",		IDS_LinkDebug, VCLINKID)
	OPT_BSTR(ProgramDatabaseFile, L"PDB:%s",					L"ProgramDatabaseFile",	single,		IDS_LinkDebug, VCLINKID)
	OPT_BSTR(StripPrivateSymbols, L"PDBSTRIPPED:%s",			L"StripPrivateSymbols", single,		IDS_LinkDebug, VCLINKID)
	OPT_BOOL_SET_COND(GenerateMapFile, L"|MAP",					L"GenerateMapFile", MapFileName,	IDS_LinkDebug, VCLINKID)
	OPT_BSTR_COND(MapFileName,	L":%s",					L"MapFileName", GenerateMapFile, single,	IDS_LinkDebug, VCLINKID)
	OPT_BOOL(MapExports,		L"|MAPINFO:EXPORTS",			L"MapExports",						IDS_LinkDebug, VCLINKID)
	OPT_BOOL(MapLines,			L"|MAPINFO:LINES",				L"MapLines",						IDS_LinkDebug, VCLINKID)
	// System
	OPT_ENUM(SubSystem, L"|SUBSYSTEM:CONSOLE|SUBSYSTEM:WINDOWS", StdEnumMin, LinkSubSystemMax, L"SubSystem", IDS_LinkSystem, VCLINKID)
	OPT_INT_NOZERO_SET_COND(HeapReserveSize, L"HEAP:%d",	L"HeapReserveSize", HeapCommitSize,			IDS_LinkSystem, VCLINKID)
	OPT_INT_NOZERO_COND(HeapCommitSize, L",%d",			L"HeapCommitSize", HeapReserveSize,			IDS_LinkSystem, VCLINKID)
	OPT_INT_NOZERO_SET_COND(StackReserveSize, L"STACK:%d",	L"StackReserveSize", StackCommitSize,		IDS_LinkSystem, VCLINKID)
	OPT_INT_NOZERO_COND(StackCommitSize, L",%d",			L"StackCommitSize", StackReserveSize,		IDS_LinkSystem, VCLINKID)
	OPT_ENUM(LargeAddressAware, L"|LARGEADDRESSAWARE:NO|LARGEADDRESSAWARE", StdEnumMin, LargeAddrMax, L"LargeAddressAware", IDS_LinkSystem, VCLINKID)
	OPT_ENUM(TerminalServerAware, L"|TSAWARE:NO|TSAWARE",	StdEnumMin, TSAwareMax,		L"TerminalServerAware", IDS_LinkSystem, VCLINKID)
	OPT_BOOL(SwapRunFromCD,		L"|SWAPRUN:CD",						L"SwapRunFromCD",					IDS_LinkSystem, VCLINKID)
	OPT_BOOL(SwapRunFromNet,	L"|SWAPRUN:NET",					L"SwapRunFromNet",					IDS_LinkSystem, VCLINKID)
	// optimization
	OPT_ENUM(OptimizeReferences, L"|OPT:NOREF|OPT:REF",		StdEnumMin, OptRefMax,	L"OptimizeReferences",	IDS_LinkOptimization, VCLINKID)
	OPT_ENUM(EnableCOMDATFolding, L"|OPT:NOICF|OPT:ICF",	StdEnumMin, OptFoldMax, L"EnableCOMDATFolding",		IDS_LinkOptimization, VCLINKID)
	OPT_ENUM(OptimizeForWindows98, L"|OPT:NOWIN98|OPT:WIN98",	StdEnumMin, OptWin98Max,	L"OptimizeForWindows98", IDS_LinkOptimization, VCLINKID)	 
	OPT_BSTR(FunctionOrder,		L"ORDER:%s",						L"FunctionOrder", single,			IDS_LinkOptimization, VCLINKID)
	OPT_BOOL(LinkTimeCodeGeneration,	L"|LTCG",					L"LinkTimeCodeGeneration",			SPECIAL_HYPERLINK, VCLINKID)
	// advanced
	OPT_BSTR(EntryPointSymbol,	L"ENTRY:%s",						L"EntryPointSymbol", single,		IDS_LinkAdvanced, VCLINKID)
	OPT_BOOL(ResourceOnlyDLL,	L"|NOENTRY",						L"ResourceOnlyDLL",					IDS_LinkAdvanced, VCLINKID)
	OPT_BOOL(SetChecksum,		L"|RELEASE",						L"SetChecksum",						IDS_LinkAdvanced, VCLINKID)
	OPT_BSTR(BaseAddress,		L"BASE:%s",							L"BaseAddress", single,				IDS_LinkAdvanced, VCLINKID)
	OPT_BOOL(TurnOffAssemblyGeneration, L"|NOASSEMBLY",				L"TurnOffAssemblyGeneration",		IDS_LinkAdvanced, VCLINKID)
	OPT_BOOL(SupportUnloadOfDelayLoadedDLL, L"|DELAY:UNLOAD",		L"SupportUnloadOfDelayLoadedDLL",	IDS_LinkAdvanced, VCLINKID)
	OPT_BSTR(ImportLibrary,		L"IMPLIB:%s",						L"ImportLibrary", single,			IDS_LinkAdvanced, VCLINKID)
	OPT_BSTR(MergeSections,		L"MERGE:%s",						L"MergeSections", single,			IDS_LinkAdvanced, VCLINKID)
	OPT_ENUM(TargetMachine,		L"|MACHINE:IX86",			StdEnumMin,	TargetMachineMax,	L"TargetMachine", IDS_LinkAdvanced, VCLINKID)	 
END_OPTION_TABLE()

// default value handlers
// string props
void CLinkerToolOptionHandler::GetDefaultValue( long id, BSTR *pVal, IVCPropertyContainer *pPropContainer )
{
	switch( id )
	{
	case VCLINKID_ImportLibrary:
	{
		if (pPropContainer == NULL)
			GetDefaultString( pVal );
		else
		{
			*pVal = NULL;
			VARIANT_BOOL bDLL = VARIANT_FALSE;
			if (pPropContainer->GetBoolProperty(VCLINKID_LinkDLL, &bDLL) != S_OK || bDLL != VARIANT_TRUE)
				GetDefaultString( pVal );
			else
			{
				VARIANT_BOOL bIgnoreExp = VARIANT_FALSE;
				if (pPropContainer->GetBoolProperty(VCLINKID_IgnoreImportLibrary, &bIgnoreExp) == S_OK && bIgnoreExp == VARIANT_TRUE)
					GetDefaultString( pVal );
				else
					*pVal = SysAllocString( wszLinkImpLibName );
			}
		}
		break;
	}
	case VCLINKID_OutputFile:
	{
		if (pPropContainer == NULL)
			GetDefaultString( pVal );
		else
		{
			*pVal = NULL;
			CComBSTR bstrFile = L"$(OutDir)/$(ProjectName).";
			VARIANT_BOOL bDll = VARIANT_FALSE;
			if (pPropContainer->GetBoolProperty(VCLINKID_LinkDLL, &bDll) == S_OK && bDll == VARIANT_TRUE)
				bstrFile += L"dll";
			else
				bstrFile += L"exe";
			*pVal = bstrFile.Detach();
		}
		break;
	}
	case VCLINKID_ProgramDatabaseFile:
	{
		if (pPropContainer == NULL)
			GetDefaultString( pVal );
		else
		{
			VARIANT_BOOL bGenDebug = VARIANT_FALSE;
			HRESULT hr = pPropContainer->GetBoolProperty(VCLINKID_GenerateDebugInformation, &bGenDebug);
			if (hr != S_OK || bGenDebug == VARIANT_FALSE)
				GetDefaultString( pVal );
			else
				*pVal = SysAllocString( wszLinkPDBFile );
		}
		break;
	}
 	case VCLINKID_EntryPointSymbol:
		{
			if (pPropContainer == NULL)
			{
				GetDefaultString(pVal);
				break;
			}

			VARIANT_BOOL bDLL;
			if (pPropContainer->GetBoolProperty(VCLINKID_LinkDLL, &bDLL) == S_OK && bDLL)
			{
				GetDefaultString(pVal);
				break;	// no need to set the entry point for DLLs
			}

			charSet charSetType = charSetNotSet;
			if (pPropContainer->GetIntProperty(VCCFGID_CharacterSet, (long *)&charSetType) != S_OK ||
				charSetType != charSetUnicode)
			{
				GetDefaultString(pVal);
				break;	// no need to set entry point for non-Unicode code
			}

			compileAsManagedOptions compileManaged;
			if (pPropContainer->GetIntProperty(VCCLID_CompileAsManaged, (long *)&compileManaged) == S_OK &&
				compileManaged != managedNotSet)
			{
				GetDefaultString(pVal);
				break;	// no need to set entry point for managed code
			}

			subSystemOption subSystemType = subSystemNotSet;
			if (pPropContainer->GetIntProperty(VCLINKID_SubSystem, (long *)&subSystemType) == S_OK &&
				subSystemType == subSystemConsole)
			{
				GetDefaultString(pVal);
				break;	// no need to set entry point for console apps
			}

			// Now, let's see if we're linking to MFC.  If the user chose to hand-add libraries, they're on their own.
			useOfMfc useMFC = useMfcStdWin;
			if (pPropContainer->GetIntProperty(VCCFGID_UseOfMFC, (long *)&useMFC) != S_OK || useMFC == useMfcStdWin)
			{
				GetDefaultString(pVal);
				break;	// no need to set entry point for non-MFC apps
			}

			// what we're left with is something that links to MFC and needs an explicit entry point
			CComBSTR bstrEntryPoint = L"wWinMainCRTStartup";
			*pVal = bstrEntryPoint.Detach();
			break;
		}
// 	case VCLINKID_AdditionalLibraryDirectories:
// 	case VCLINKID_DelayLoadDLLs:
// 	case VCLINKID_FunctionOrder:
// 	case VCLINKID_MapFileName:
// 	case VCLINKID_StripPrivateSymbols:
// 	case VCLINKID_MergedIDLBaseFileName:
// 	case VCLINKID_TypeLibraryFile:
// 	case VCLINKID_IgnoreDefaultLibraryNames:
// 	case VCLINKID_ModuleDefinitionFile:
// 	case VCLINKID_AddModuleNamesToAssembly:
// 	case VCLINKID_EmbedManagedResourceFile:
// 	case VCLINKID_MidlCommandFile:
// 	case VCLINKID_AdditionalDependencies:
// 	case VCLINKID_AdditionalOptions:
// 	case VCLINKID_Version:
// 	case VCLINKID_ForceSymbolReferences:
// 	case VCLINKID_BaseAddress:
// 	case VCLINKID_MergeSections:
	default:
		GetDefaultString( pVal );
		break;
	}
}

// integer props
void CLinkerToolOptionHandler::GetDefaultValue( long id, long *pVal, IVCPropertyContainer *pPropCnt )
{
	switch( id )
	{
	case VCLINKID_ShowProgress:
		*pVal = lShowProgress;
		break;
	case VCLINKID_LinkIncremental:
		*pVal = linkIncrementalDefault;
		break;
	case VCLINKID_TypeLibraryResourceID:
		*pVal = lnTypeLibraryResourceID;
		break;
	case VCLINKID_TerminalServerAware:
		*pVal = termSvrAwareDefault;
		break;
	case VCLINKID_SubSystem:
		*pVal = lSubSystem;
		break;
	case VCLINKID_HeapReserveSize:
		*pVal = lnHeapReserveSize;
		break;
	case VCLINKID_HeapCommitSize:
		*pVal = lnHeapCommitSize;
		break;
	case VCLINKID_StackReserveSize:
		*pVal = lnStackReserveSize;
		break;
	case VCLINKID_StackCommitSize:
		*pVal = lnStackCommitSize;
		break;
	case VCLINKID_LargeAddressAware:
		*pVal = addrAwareDefault;
		break;
	case VCLINKID_OptimizeReferences:
		*pVal = optReferencesDefault;
		break;
	case VCLINKID_EnableCOMDATFolding:
		*pVal = optFoldingDefault;
		break;
	case VCLINKID_OptimizeForWindows98:
		*pVal = optWin98Default;
		break;
	case VCLINKID_TargetMachine:
		*pVal = lTargetMachine;
		break;
	default:
		*pVal = 0;
		break;
	}
}

// boolean props
void CLinkerToolOptionHandler::GetDefaultValue( long id, VARIANT_BOOL *pVal, IVCPropertyContainer *pPropCnt )
{
// 	switch( id )
// 	{
// 	case VCLINKID_SuppressStartupBanner:
// 	case VCLINKID_IgnoreImportLibrary:
// 	case VCLINKID_RegisterOutput:
// 	case VCLINKID_IgnoreAllDefaultLibraries:
// 	case VCLINKID_IgnoreEmbeddedIDL:
// 	case VCLINKID_LinkDLL:
// 	case VCLINKID_SwapRunFromCD:
// 	case VCLINKID_SwapRunFromNet:
// 	case VCLINKID_GenerateDebugInformation:
// 	case VCLINKID_GenerateMapFile:
// 	case VCLINKID_MapExports:
// 	case VCLINKID_MapLines:
// 	case VCLINKID_LinkTimeCodeGeneration:
// 	case VCLINKID_SupportUnloadOfDelayLoadedDLL:
// 	case VCLINKID_ResourceOnlyDLL:
// 	case VCLINKID_SetChecksum:
// 	case VCLINKID_TurnOffAssemblyGeneration:
// 	default:
		GetValueFalse( pVal );
// 		break;
// 	}
}

BOOL CLinkerToolOptionHandler::SetEvenIfDefault(VARIANT *pvarDefault, long idOption)
{
	CComVariant varDefault;
	BOOL bRet = FALSE;

	switch (idOption)
	{
	case VCLINKID_SuppressStartupBanner:
		varDefault.vt = VT_BOOL;
		varDefault = VARIANT_TRUE;
		bRet = TRUE;
		break;
	}
	varDefault.Detach(pvarDefault);
	return bRet;
}

BOOL CLinkerToolOptionHandler::SynthesizeOptionIfNeeded(IVCPropertyContainer* pPropContainer, long idOption)
{
	switch (idOption)
	{
	case VCLINKID_OutputFile:
		return TRUE;
	case VCLINKID_ProgramDatabaseFile:
		return TRUE;
	case VCLINKID_EntryPointSymbol:
		return TRUE;
	default:
		return FALSE;
	}
}

BOOL CLinkerToolOptionHandler::SynthesizeOption(IVCPropertyContainer* pPropContainer, long idOption, 
	CComVariant& rvar)
{
	CComBSTR bstrVal;
	switch (idOption)
	{
	case VCLINKID_OutputFile:
		CreateOutputName(pPropContainer, &bstrVal, FALSE);
		break;
	case VCLINKID_ProgramDatabaseFile:
		CreatePDBName(pPropContainer, &bstrVal, FALSE);
		break;
	case VCLINKID_EntryPointSymbol:
		GetDefaultValue(VCLINKID_EntryPointSymbol, &bstrVal, pPropContainer);
		break;
	default:
		VSASSERT(FALSE, "Case statement mismatch between SynthesizeOptionIfNeeded and SynthesizeOption");	// shouldn't be here!!
		return FALSE;
	}

	rvar = bstrVal;
	return TRUE;
}

static const wchar_t* szScriptItemCL = L"\r\n<a title=\"%s | %s\" onclick=\"onHelp('VC.Project.VCConfiguration.%s')\" href=\"settingspage.htm\">%s</a>";

void CLinkerToolOptionHandler::FormatScriptItem(const wchar_t* szOptionName, long idOption, long idOptionPage, CStringW& rstrOption)
{
	if (idOption != SPECIAL_HYPERLINK)
	{
		COptionHandlerBase::FormatScriptItem(szOptionName, idOption, idOptionPage, rstrOption);
		return;
	}

	VSASSERT(idOption == VCLINKID_LinkDLL || idOption == VCLINKID_LinkTimeCodeGeneration, "Hey, unexpected special handling option for linker!");

	CStringW strProp;
	CStringW strPropBase;
	if (idOption == VCLINKID_LinkDLL)
	{
		strProp.LoadString(VCCFGID_ConfigurationType);
		strPropBase = L"ConfigurationType";
	}
	else
	{
		strProp.LoadString(VCCFGID_WholeProgramOptimization);
		strPropBase = L"WholeProgramOptimization";
	}
	long nColon = strProp.Find(L": ");
	if (nColon)
		strProp = strProp.Left(nColon-1);
	strProp.TrimLeft();
	strProp.TrimRight();

	CStringW strGen;
	strGen.LoadString(IDS_GeneralConfigSettings);
	CStringW strTmp;
	strTmp.Format(szScriptItemCL, strGen, strProp, strPropBase, rstrOption);
	rstrOption = strTmp;
}

void CLinkerToolOptionHandler::EvaluateSpecialString(CStringW& rstrVal, BOOL bIsMultiple, LPCOLESTR szOption, long idOption,
	LPCOLESTR szOptionName, long idOptionPage, IVCPropertyContainer* pPropContainer, BOOL bIncludeSlash, 
	BOOL bForDisplay, commandLineOptionStyle fStyle, CStringW& rstrTrailing, CStringW& rstrSwitch) 
{
	if (idOption != VCLINKID_MidlCommandFile)
	{
		EvaluateString(rstrVal, bIsMultiple, szOption, szOptionName, idOption, idOptionPage, bIncludeSlash, TRUE /* quote it */,
			bForDisplay, rstrSwitch);
		return;
	}

	rstrSwitch.Empty();
	if (rstrVal.IsEmpty())	// don't generate anything on empty strings
		return;

	if (pPropContainer == NULL)		// not a lot we can do with no property container
		return;

	CPathW pathMidlFile;
	if (fStyle == cmdLineForRefresh || 
		(CVCToolImpl::CreatePathRelativeToProject(pPropContainer, rstrVal, pathMidlFile) && pathMidlFile.ExistsOnDisk()))
	{	// we were given an actual file to work with
		EvaluateString(rstrVal, bIsMultiple, szOption, szOptionName, idOption, idOptionPage, bIncludeSlash, TRUE /* quote it */,
			bForDisplay, rstrSwitch);
		return;
	}

	if (fStyle == cmdLineForBuild)
	{
		CComPtr<IVCPropertyContainer> spActualContainer;
		CVCProjectEngine::ResolvePropertyContainer(pPropContainer, &spActualContainer);
		CComQIPtr<IVCBuildableItem> spBuildableItem = spActualContainer;
		if (spBuildableItem == NULL)	// not a config or file config, not much we can do
			return;

		CComPtr<IVCBuildEngine> spBuildEngine;
		spBuildableItem->get_ExistingBuildEngine(&spBuildEngine);
		if (spBuildEngine == NULL)	// not doing a build, not much we can do
			return;

		CComBSTR bstrCommentLine;
		bstrCommentLine.LoadString(IDS_CREATING_LINK_MIDL_RSP);
		CComBSTR bstrCommandIn = rstrVal;
		CComBSTR bstrCommandOut;
		spBuildEngine->FormTempFile(bstrCommandIn, bstrCommentLine, &bstrCommandOut);
		rstrVal = bstrCommandOut;
	}
	else	// cmdLineForDisplay
	{
		CStringW strComment;
		strComment.Format(IDS_USE_LINK_MIDL_RSP, rstrVal);
		rstrTrailing += strComment;
		rstrVal.LoadString(IDS_LINK_MIDL_RSP_NAME);
	}

	EvaluateString(rstrVal, bIsMultiple, szOption, szOptionName, idOption, idOptionPage, bIncludeSlash, TRUE /* quote it */,
		bForDisplay, rstrSwitch);
}

BOOL CLinkerToolOptionHandler::CreateOutputName(IVCPropertyContainer* pPropContainer, BSTR *pbstrVal,
	BOOL bCheckForExistence /* = TRUE */)
{
	VSASSERT(pPropContainer != NULL, "Cannot create output name without a property container");
	RETURN_ON_NULL2(pPropContainer, FALSE);

	if (bCheckForExistence &&
		pPropContainer->GetEvaluatedStrProperty(VCLINKID_OutputFile, pbstrVal) == S_OK)	// found it directly set, so use it
		return TRUE;

	CVCLinkerTool::s_optHandler.GetDefaultValue( VCLINKID_OutputFile, pbstrVal, pPropContainer );
	HRESULT hr = pPropContainer->Evaluate(*pbstrVal, pbstrVal);

	return (SUCCEEDED(hr));
}

BOOL CLinkerToolOptionHandler::CreatePDBName(IVCPropertyContainer* pPropContainer, BSTR *pbstrVal,
	BOOL bCheckForExistence /* = TRUE */)
{
	VSASSERT(pPropContainer != NULL, "Cannot create output name without a property container");
	RETURN_ON_NULL2(pPropContainer, FALSE);

	if (bCheckForExistence &&
		pPropContainer->GetEvaluatedStrProperty(VCLINKID_ProgramDatabaseFile, pbstrVal) == S_OK)	// found it directly set, so use it
		return TRUE;

	CVCLinkerTool::s_optHandler.GetDefaultValue( VCLINKID_ProgramDatabaseFile, pbstrVal, pPropContainer );
	HRESULT hr = pPropContainer->Evaluate(*pbstrVal, pbstrVal);

	return (SUCCEEDED(hr));
}

BOOL CLinkerToolOptionHandler::OverrideOptionSet(IVCPropertyContainer* pPropContainer, long idOption)
{
	RETURN_ON_NULL2(g_pProjectEngine, FALSE);

	// Are we an Xbox or not?

	CComBSTR bstrPlatName;
	pPropContainer->GetStrProperty(VCPLATID_Name, &bstrPlatName);
	if (lstrcmpW(bstrPlatName, L"Xbox") != 0)
	{
		return FALSE;	// Not Xbox, don't override any options
	}

	switch (idOption)
	{
	case VCLINKID_SubSystem:	// Override SubSystem option
		return TRUE;
	default:
		return FALSE;
	}
}

HRESULT CLinkerToolOptionHandler::GenerateCommandLine(IVCPropertyContainer* pPropContainer, BOOL bForDisplay, commandLineOptionStyle fStyle, 
		CStringW& rstrCmdLine)
{
	// Are we an Xbox or not?

	CComBSTR bstrPlatName;
	pPropContainer->GetStrProperty(VCPLATID_Name, &bstrPlatName);
	bool bIsXbox = 0 == lstrcmpW(bstrPlatName, L"Xbox");
	if(!bIsXbox){
		return COptionHandlerBase::GenerateCommandLine(pPropContainer, bForDisplay,  fStyle, rstrCmdLine);
	}
	else {
		HRESULT hr = COptionHandlerBase::GenerateCommandLine(pPropContainer, bForDisplay,  fStyle, rstrCmdLine);
		if(SUCCEEDED(hr)){
			// Add /subSystem:Xbox. We do it here to keep from having to add Xbox to the subsystem enumeration.
			rstrCmdLine += L" /subsystem:xbox";
		}
		return hr;
	}
}


/////////////////////////////////////////////////////////////////////////////
HRESULT CVCLinkerTool::CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppLinkTool)
{
	CHECK_POINTER_NULL(ppLinkTool);
	*ppLinkTool = NULL;

	CComObject<CVCLinkerTool> *pObj;
	HRESULT hr = CComObject<CVCLinkerTool>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		if( pPropContainer )
			pObj->Initialize(pPropContainer);
		CVCLinkerTool *pVar = pObj;
		pVar->AddRef();
		*ppLinkTool = pVar;
	}
	return hr;
}

STDMETHODIMP CVCLinkerTool::IsDeleteOnRebuildFile(LPCOLESTR szFile, VARIANT_BOOL* pbDelOnRebuild)
{
	CHECK_POINTER_NULL(pbDelOnRebuild);

	BOOL fDel = BldFileNameMatchesExtension(szFile, wszLinkerDelOnRebuildExtensions);
	*pbDelOnRebuild = fDel ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CVCLinkerTool::get_DefaultExtensions(BSTR* pVal)
{
	return DoGetDefaultExtensions(s_bstrExtensions, wszLinkDefaultExtensions, pVal);
}

STDMETHODIMP CVCLinkerTool::put_DefaultExtensions(BSTR newVal)
{
	s_bstrExtensions = newVal;
	return S_OK;
}

STDMETHODIMP CVCLinkerTool::GetAdditionalOptionsInternal(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, 
	BSTR* pbstrAdditionalOptions)
{ 
	return GetProcessedAdditionalOptions(pItem, VCLINKID_AdditionalOptions, bForBuild, bSkipLocal, pbstrAdditionalOptions);
}

STDMETHODIMP CVCLinkerTool::GetBuildDescription(IVCBuildAction* pAction, BSTR* pbstrBuildDescription)
{
	if (s_bstrBuildDescription.Length() == 0)
		s_bstrBuildDescription.LoadString(IDS_DESC_LINKING);
	return s_bstrBuildDescription.CopyTo(pbstrBuildDescription);
}

STDMETHODIMP CVCLinkerTool::GetAdditionalIncludeDirectoriesInternal(IVCPropertyContainer* pPropContainer, BSTR* pbstrIncDirs)
{
	CHECK_POINTER_NULL(pbstrIncDirs);
	CHECK_READ_POINTER_NULL(pPropContainer);
	CComBSTR bstrTmp;
	HRESULT hr = pPropContainer->GetMultiProp(VCLINKID_AdditionalLibraryDirectories, L";", VARIANT_FALSE, &bstrTmp);
	if (SUCCEEDED(hr))
		pPropContainer->Evaluate(bstrTmp, pbstrIncDirs);
	else
		*pbstrIncDirs = bstrTmp.Detach();
	return hr;
}

STDMETHODIMP CVCLinkerTool::get_ToolPathInternal(BSTR* pbstrToolPath)
{ 
	return get_ToolPath(pbstrToolPath);
}

STDMETHODIMP CVCLinkerTool::get_Bucket(long *pVal)
{
	CHECK_POINTER_NULL( pVal );
	*pVal = BUCKET_LINK; 
	return S_OK;
}

STDMETHODIMP CVCLinkerTool::GenerateOutput(long type, IVCBuildActionList* plstActions, IVCBuildErrorContext* pEC)
{
	return DoGenerateOutput(type, plstActions, pEC, TRUE, this);
}

///////////////////////////////////////////////////////////////////////////////
// Affects our output?
STDMETHODIMP CVCLinkerTool::AffectsOutput(long nPropID, VARIANT_BOOL* pbAffectsOutput)
{
	CHECK_POINTER_NULL(pbAffectsOutput);
	*pbAffectsOutput = VARIANT_FALSE;

	// change our outputs?
	switch (nPropID)
	{
	case VCLINKID_LinkDLL:				// enables *.dll
	case VCLINKID_LinkIncremental:		// enables *.ilk
	case VCLINKID_ProgramDatabaseFile:	// *.pdb
	case VCLINKID_GenerateMapFile:		// enables *.map
	case VCLINKID_MapFileName:			// *.map
	case VCLINKID_OutputFile:			// *.exe, *.dll etc.
	case VCLINKID_ModuleDefinitionFile:	// *.def
	case VCLINKID_AdditionalDependencies:	// extra stuff to add to link line
	case VCLINKID_DependentInputs:		// inputs we pick up from dependent projects
	case VCLINKID_IgnoreImportLibrary:	// ignore the import .lib
	case VCLINKID_OutputsDirty:			// anything we care about dirty
	case VCLINKID_RegisterOutput:		// register output of build
		*pbAffectsOutput = VARIANT_TRUE;
		break;
	default:
		*pbAffectsOutput = VARIANT_FALSE;
		break;
	}

	return S_OK;
}

GUID* CVCLinkerTool::GetPageIDs()
{ 
	if (s_bPagesInit == FALSE)
	{
		s_pPages[0] = __uuidof(LinkGeneral);
		s_pPages[1] = __uuidof(LinkInput);
		s_pPages[2] = __uuidof(LinkDebug);
		s_pPages[3] = __uuidof(LinkSystem);
		s_pPages[4] = __uuidof(LinkOptimization);
		s_pPages[5] = __uuidof(LinkMIDL);
		s_pPages[6] = __uuidof(LinkAdvanced);
		s_pPages[7] = __uuidof(LinkAdditionalOptions);
		s_bPagesInit = TRUE;
	}
	return s_pPages; 
}

STDMETHODIMP CVCLinkerTool::IsSpecialConsumable(LPCOLESTR szPath, VARIANT_BOOL* pbSpecial)
{
	CHECK_POINTER_NULL(pbSpecial);

	if (BldFileNameMatchesExtension(szPath, L"def"))
		*pbSpecial = VARIANT_TRUE;
	else
		*pbSpecial = VARIANT_FALSE;
	return S_OK;
}

HRESULT CVCLinkerTool::GetPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, CPathW& rpath)
{
	return DoGetPrimaryOutputFromTool(pItem, rpath, TRUE);
}

STDMETHODIMP CVCLinkerTool::HasPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, VARIANT_BOOL* pbHasPrimaryOutput)
{
	return COptionHandlerBase::GetValueTrue( pbHasPrimaryOutput );
}

STDMETHODIMP CVCLinkerTool::GetPrimaryOutputIDFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, long* pnOutputID)
{
	CHECK_POINTER_NULL(pnOutputID);
	*pnOutputID = VCLINKID_OutputFile;
	return S_OK;
}

STDMETHODIMP CVCLinkerTool::GetImportLibraryFromTool(IUnknown* pItem, BSTR* pbstrPath)
{
	CHECK_READ_POINTER_NULL(pItem);
	CHECK_POINTER_NULL(pbstrPath);
	*pbstrPath = NULL;

	CComPtr<VCConfiguration> spConfig;
	GetProjectCfgForItem(pItem, &spConfig);
	RETURN_ON_NULL2(spConfig, S_FALSE);

	CComQIPtr<IVCPropertyContainer> spPropContainer = spConfig;
	RETURN_ON_NULL2(spPropContainer, E_UNEXPECTED);

	ConfigurationTypes configType = typeUnknown;
	if (spConfig->get_ConfigurationType(&configType) != S_OK)
	{
		VSASSERT(false, "Hey, how did we get a config without a non-default config type?");
		return E_UNEXPECTED;
	}
	else if (configType != typeDynamicLibrary)
		return S_FALSE;

	VARIANT_BOOL fLib;	// does this dll have an export lib ?
	HRESULT hr = spPropContainer->GetBoolProperty(VCLINKID_IgnoreImportLibrary, &fLib);
	RETURN_ON_FAIL(hr);

	if (fLib == VARIANT_TRUE)
		return S_FALSE;

	CComBSTR bstrImportLib;
	hr = spPropContainer->GetEvaluatedStrProperty(VCLINKID_ImportLibrary, &bstrImportLib);
	RETURN_ON_FAIL(hr);

	CStringW strImportLib = bstrImportLib;
	strImportLib.TrimLeft();
	if (strImportLib.IsEmpty())
	{
		s_optHandler.GetDefaultValue( VCLINKID_ImportLibrary, &bstrImportLib );
		hr = spPropContainer->Evaluate(bstrImportLib, &bstrImportLib);
		RETURN_ON_FAIL(hr);
		strImportLib = bstrImportLib;
	}

	CPathW pathName;
	if (!CreatePathRelativeToProject(spPropContainer, strImportLib, pathName))
		return VCPROJ_E_BAD_PATH;

	pathName.GetFullPath(strImportLib);
	bstrImportLib = strImportLib;
	*pbstrPath = bstrImportLib.Detach();
	return S_OK;
}

STDMETHODIMP CVCLinkerTool::GetProgramDatabaseFromTool(IVCPropertyContainer* pPropContainer, VARIANT_BOOL bSchmoozeOnly, 
	BSTR* pbstrPDB)
{
	CHECK_POINTER_NULL(pbstrPDB);
	*pbstrPDB = NULL;

	VARIANT_BOOL bGenDebug = VARIANT_FALSE;
	HRESULT hr = pPropContainer->GetBoolProperty(VCLINKID_GenerateDebugInformation, &bGenDebug);
	if (hr != S_OK || bGenDebug == VARIANT_FALSE)
		return S_FALSE;

	CComBSTR bstrPDBFile;
	if (!s_optHandler.CreatePDBName(pPropContainer, &bstrPDBFile))
		return S_FALSE;
	CStringW strPDBFile = bstrPDBFile;

	CPathW pathName;
	if (!CreatePathRelativeToProject(pPropContainer, strPDBFile, pathName))
		return VCPROJ_E_BAD_PATH;

	pathName.GetFullPath(strPDBFile);
	bstrPDBFile = strPDBFile;
	*pbstrPDB = bstrPDBFile.Detach();
	return S_OK;
}

STDMETHODIMP CVCLinkerTool::HasDependencies(IVCBuildAction* pAction, VARIANT_BOOL* pbHasDependencies)
{
	return DoHasDependencies(pAction, TRUE, pbHasDependencies);
}

STDMETHODIMP CVCLinkerTool::IsTargetTool(IVCBuildAction* pAction, VARIANT_BOOL* pbTargetTool)
{ // tool always operates on target, not on file
	return COptionHandlerBase::GetValueTrue(pbTargetTool);
}

void CVCLinkerTool::SaveSpecialProps(IStream *xml, IVCPropertyContainer *pPropContainer)
{
	// Register
	CComVariant varRegister;
	if (pPropContainer->GetLocalProp(VCLINKID_RegisterOutput, &varRegister) == S_OK && 
		(varRegister.vt == VT_BOOL || varRegister.vt == VT_I2))
		NodeAttribute( xml, L"RegisterOutput", varRegister.boolVal == VARIANT_TRUE ? L"TRUE" : L"FALSE" );

	// Ignore export lib
	CComVariant varIgnore;
	if (pPropContainer->GetLocalProp(VCLINKID_IgnoreImportLibrary, &varIgnore) == S_OK && 
		(varIgnore.vt == VT_BOOL || varIgnore.vt == VT_I2))
		NodeAttribute( xml, L"IgnoreImportLibrary", varIgnore.boolVal == VARIANT_TRUE ? L"TRUE" : L"FALSE" );
}

STDMETHODIMP CVCLinkerTool::GetCommandLinesForBuild(IVCBuildActionList* pActions, bldAttributes attrib, 
	IVCBuildEngine* pBldEngine, IVCBuildErrorContext* pErrorContext, IVCCommandLineList** ppCmds)
{
	HRESULT hr = CVCToolImpl::GetCommandLinesForBuild(pActions, attrib, pBldEngine, pErrorContext, ppCmds);
	if (hr != S_OK)
		return hr;

	// now need to check if we need to add a command line for registration
	pActions->Reset(NULL);
	while (hr == S_OK)
	{
		CComPtr<IVCBuildAction> spAction;
		hr = pActions->Next(&spAction, NULL);
		BREAK_ON_DONE(hr);
		ASSERT_AND_CONTINUE_ON_NULL(spAction);

		CComPtr<VCConfiguration> spProjCfg;
		HRESULT hr1 = spAction->get_ProjectConfiguration(&spProjCfg);
		ASSERT_AND_CONTINUE_ON_FAIL_OR_NULL(hr1, spProjCfg);

		VARIANT_BOOL bRegister;
		hr1 = spProjCfg->get_RegisterOutput(&bRegister);
		ASSERT_AND_CONTINUE_ON_FAIL(hr1);
		if (bRegister == VARIANT_FALSE)	// nothing more to do if not registering
			continue;

		CComQIPtr<IVCPropertyContainer> spPropContainer = spProjCfg;
		ASSERT_AND_CONTINUE_ON_NULL(spPropContainer);

		CComBSTR bstrTargetPath = L"$(TargetPath)";
		hr1 = spPropContainer->Evaluate(bstrTargetPath, &bstrTargetPath);
		ASSERT_AND_CONTINUE_ON_FAIL(hr1);
		CStringW strTargetTmp = bstrTargetPath;
		CStringW strTargetDir, strTargetDrive;
		VSASSERT(!strTargetTmp.IsEmpty(), "Hey, no target path!");
		if (strTargetTmp.IsEmpty())
			continue;

		CPathW pathTarget;
		if (!pathTarget.Create(strTargetTmp))
			ASSERT_AND_CONTINUE_ON_FAIL(E_FAIL);
		pathTarget.SetAlwaysRelative();

		CStringW strTargetFile = pathTarget.GetFileName();
		CComBSTR bstrProjDir;
		hr1 = spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjDir);
		ASSERT_AND_CONTINUE_ON_FAIL(hr1);
		CDirW dirProj;
		dirProj.CreateFromKnown(bstrProjDir);
		pathTarget.GetRelativeName(dirProj, strTargetDir);
		int nLastForwardSlash = strTargetDir.ReverseFind(L'/');
		int nLastBackSlash = strTargetDir.ReverseFind(L'\\');
		if (nLastForwardSlash > nLastBackSlash)
			strTargetDir = strTargetDir.Left(nLastForwardSlash);
		else if (nLastBackSlash > nLastForwardSlash)
			strTargetDir = strTargetDir.Left(nLastBackSlash);
		else	// can only be the same if neither was found
			strTargetDir.Empty();
		int nLen = strTargetDir.GetLength();
		if (nLen <= 3)		// this is minimum size to be absolute path
		{}	// do nothing
		else if ((strTargetDir[0] == L'\\' || strTargetDir[0] == L'/') &&
			(strTargetDir[1] == L'\\' || strTargetDir[1] == L'/'))	// in format \\foo\bar\junk
		{
			int cSlashes = 1;
			int idx = 1;
			while (idx < nLen && cSlashes < 4)
			{
				if (strTargetDir[idx] == L'/' || strTargetDir[idx] == L'\\')
					cSlashes++;
				idx++;
			}
			if (idx < nLen)
			{
				strTargetDrive = strTargetDir.Left(idx-1);
				strTargetDir = strTargetDir.Right(nLen-idx);
			}
		}
		else if (strTargetDir[1] == L':' && (strTargetDir[2] == L'/' || strTargetDir[2] == L'\\')) // in format 'c:\foo\bar'
		{
			strTargetDrive = strTargetDir.Left(2);	// want the 'c:'
			if (strTargetDir.GetLength() <= 2)
				strTargetDir.Empty();
			else
				strTargetDir = strTargetDir.Right(nLen-2);	// take off the 'c:'
		}
		else
		{
			if (strTargetDir[0] != L'.' && strTargetDir[0] != L'/' && strTargetDir[0] != L'\\')
				strTargetDir = L".\\" + strTargetDir;
		}

		CComBSTR bstrCmd = L"@echo off\r\n";
		if (!strTargetDrive.IsEmpty())
		{
			bstrCmd += strTargetDir;
			bstrCmd += L"\r\n";
		}
		if (!strTargetDir.IsEmpty())
		{
			bstrCmd += L"cd ";
			bstrCmd += strTargetDir;
			bstrCmd += L"\r\n";
		}
		bstrCmd += L"regsvr32 /s \"";
		bstrCmd += strTargetFile;
		bstrCmd += L"\"";
		CComBSTR bstrCmdForBuild;
		hr1 = pBldEngine->FormBatchFile(bstrCmd, pErrorContext, &bstrCmdForBuild);

		CComBSTR bstrDescription;
		bstrDescription.LoadString(IDS_REGISTER_OUTPUT);

		if (*ppCmds == NULL)
		{
			hr1 = CVCCommandLineList::CreateInstance(ppCmds);
			ASSERT_AND_CONTINUE_ON_FAIL_OR_NULL(hr1, *ppCmds);
		}
		CComPtr<IVCCommandLine> spCmdLine;
		hr1 = CVCCommandLine::CreateInstance(&spCmdLine);
		ASSERT_AND_CONTINUE_ON_FAIL_OR_NULL(hr, spCmdLine);

		spCmdLine->put_CommandLineContents(bstrCmdForBuild);
		spCmdLine->put_Description(bstrDescription);
		spCmdLine->put_NumberOfProcessors(1);

		(*ppCmds)->Add(spCmdLine, FALSE);
	}

	return S_OK;
}

HRESULT CVCLinkerTool::GetAdditionalDependenciesInternal(IVCPropertyContainer* pItem, IVCBuildAction* pAction, BOOL bForSave, 
	BSTR* pbstrInputs)
{		// additional things to add to command line
	BOOL bAdd = FALSE;
	if (!bForSave)
	{
		CComQIPtr<IVCPropertyContainer> spPropContainer = pItem;
		CComBSTR bstrDelayLoad;
		bAdd = (spPropContainer && spPropContainer->GetStrProperty(VCLINKID_DelayLoadDLLs, &bstrDelayLoad) == S_OK 
			&& bstrDelayLoad.Length() > 0);
	}
	return DoGetAdditionalDependenciesInternal(pItem, pAction, bForSave, pbstrInputs, GetOptionHandler(), 
		VCLINKID_AdditionalDependencies, VCLINKID_DependentInputs, bAdd ? L"DelayImp.lib" : L"");
}

STDMETHODIMP CVCLinkerTool::get_ToolShortName(BSTR* pbstrToolName)
{
	*pbstrToolName = SysAllocString( szLinkerToolShortName );
	return S_OK;
}

STDMETHODIMP CVCLinkerTool::MatchName(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches)
{
	return DoMatchName(bstrName, szLinkerToolType, szLinkerToolShortName, pbMatches);
}

STDMETHODIMP CVCLinkerTool::CreateToolObject(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject)
{ 
	return CreateInstance(pPropContainer, ppToolObject); 
}

STDMETHODIMP CVCLinkerTool::CanScanForDependencies(VARIANT_BOOL* pbIsScannable)
{
	return COptionHandlerBase::GetValueTrue(pbIsScannable);
}

STDMETHODIMP CVCLinkerTool::GetDependencies(IVCBuildAction* pAction, IVCBuildStringCollection** ppStrings, 
	VARIANT_BOOL* pbUpToDate)
{
	CHECK_READ_POINTER_NULL(pAction);
	return DoGetDependencies(pAction, ppStrings, pbUpToDate, TRUE);
}

BOOL CVCLinkerTool::CreateOutputName(IVCPropertyContainer* pPropContainer, BSTR* pbstrVal)
{
	return s_optHandler.CreateOutputName(pPropContainer, pbstrVal, FALSE);
}

STDMETHODIMP CVCLinkerTool::CreatePageObject(IUnknown **ppUnk, CLSID*pCLSID, IVCPropertyContainer *pPropCntr, IVCSettingsPage *pPage )
{
	// Get the list of pages
	if( CLSID_LinkGeneral == *pCLSID )
	{
		CPageObjectImpl< CVCLinkerGeneralPage, VCLINKERTOOL_MIN_DISPID, VCLINKERTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else if( CLSID_LinkInput == *pCLSID )
	{
		CPageObjectImpl< CVCLinkerInputPage, VCLINKERTOOL_MIN_DISPID, VCLINKERTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else if( CLSID_LinkDebug == *pCLSID )
	{
		CPageObjectImpl< CVCLinkerDebugPage, VCLINKERTOOL_MIN_DISPID, VCLINKERTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else if( CLSID_LinkSystem == *pCLSID )
	{
		CPageObjectImpl< CVCLinkerSystemPage, VCLINKERTOOL_MIN_DISPID, VCLINKERTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else if( CLSID_LinkOptimization == *pCLSID )
	{
		CPageObjectImpl< CVCLinkerOptimizationPage, VCLINKERTOOL_MIN_DISPID, VCLINKERTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else if( CLSID_LinkAdvanced == *pCLSID )
	{
		CPageObjectImpl< CVCLinkerAdvancedPage, VCLINKERTOOL_MIN_DISPID, VCLINKERTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else if( CLSID_LinkMIDL == *pCLSID )
	{
		CPageObjectImpl< CVCLinkerMIDLPage, VCLINKERTOOL_MIN_DISPID, VCLINKERTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else
	{
		return S_FALSE;
	}
		
	return S_OK;
}

LPCOLESTR CVCLinkerTool::GetToolFriendlyName()
{
	InitToolName();
	return s_bstrToolName;
}

void CVCLinkerTool::InitToolName()
{
	if (s_bstrToolName.Length() == 0)
	{
		if (!s_bstrToolName.LoadString(IDS_LINK_TOOLNAME))
			s_bstrToolName = szLinkerToolType;
	}
}

// general
STDMETHODIMP CVCLinkerTool::get_ToolName(BSTR* pbstrToolName)
{	// friendly name of tool, e.g., "C/C++ Compiler Tool"
	CHECK_POINTER_VALID( pbstrToolName );
	InitToolName();
	s_bstrToolName.CopyTo(pbstrToolName);
	return S_OK;
}

STDMETHODIMP CVCLinkerTool::get_AdditionalOptions(BSTR* pbstrAdditionalOptions)
{	// anything we don't know what to do with, given verbatim, in same order as entered, parent contributions first, goes after all other options
	return ToolGetStrProperty(VCLINKID_AdditionalOptions, pbstrAdditionalOptions);
}

STDMETHODIMP CVCLinkerTool::put_AdditionalOptions(BSTR bstrAdditionalOptions)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_AdditionalOptions, bstrAdditionalOptions);
}

STDMETHODIMP CVCLinkerTool::get_OutputFile(BSTR* pbstrOut)
{	// (/OUT:[file]) change the output file name (default is based on 1st lib or obj name on cmd line)
	return ToolGetStrProperty(VCLINKID_OutputFile, pbstrOut);
}

STDMETHODIMP CVCLinkerTool::put_OutputFile(BSTR bstrOut)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_OutputFile, bstrOut);
}

STDMETHODIMP CVCLinkerTool::get_ShowProgress(linkProgressOption* poptSetting)
{	// see enum above (/VERBOSE)
	return ToolGetIntProperty(VCLINKID_ShowProgress, (long *)poptSetting);
}

STDMETHODIMP CVCLinkerTool::put_ShowProgress(linkProgressOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, ShowProgressMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCLINKID_ShowProgress, optSetting);
}

STDMETHODIMP CVCLinkerTool::get_Version(BSTR* pbstrVersion)
{	// (/VERSION:major[.minor]) put this version number into header of created image
	return ToolGetStrProperty(VCLINKID_Version, pbstrVersion);
}

STDMETHODIMP CVCLinkerTool::put_Version(BSTR bstrVersion)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_Version, bstrVersion);
}

STDMETHODIMP CVCLinkerTool::get_LinkIncremental(linkIncrementalType* poptSetting)
{	// (/INCREMENTAL:YES, /INCREMENTAL:NO, not set)
	return ToolGetIntProperty(VCLINKID_LinkIncremental, (long *)poptSetting);
}

STDMETHODIMP CVCLinkerTool::put_LinkIncremental(linkIncrementalType optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, LinkIncrMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCLINKID_LinkIncremental, optSetting);
}

STDMETHODIMP CVCLinkerTool::get_SuppressStartupBanner(VARIANT_BOOL* pbNoLogo)
{	// (/NOLOGO) enable suppression of copyright message (no explicit off)
	return ToolGetBoolProperty(VCLINKID_SuppressStartupBanner, pbNoLogo);
}

STDMETHODIMP CVCLinkerTool::put_SuppressStartupBanner(VARIANT_BOOL bNoLogo)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bNoLogo );
	return m_spPropertyContainer->SetBoolProperty(VCLINKID_SuppressStartupBanner, bNoLogo);
}

STDMETHODIMP CVCLinkerTool::get_IgnoreImportLibrary(VARIANT_BOOL* pbIgnore)
{	// ignore any export .lib created
	return ToolGetBoolProperty(VCLINKID_IgnoreImportLibrary, pbIgnore);
}

STDMETHODIMP CVCLinkerTool::put_IgnoreImportLibrary(VARIANT_BOOL bIgnore)
{
	CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
	CHECK_VARIANTBOOL(bIgnore);
	return m_spPropertyContainer->SetBoolProperty(VCLINKID_IgnoreImportLibrary, bIgnore);
}

STDMETHODIMP CVCLinkerTool::get_RegisterOutput(VARIANT_BOOL* pbRegister)
{	// register the output if it's a DLL
	return ToolGetBoolProperty(VCLINKID_RegisterOutput, pbRegister);
}

STDMETHODIMP CVCLinkerTool::put_RegisterOutput(VARIANT_BOOL bRegister)
{
	CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
	CHECK_VARIANTBOOL(bRegister);
	return m_spPropertyContainer->SetBoolProperty(VCLINKID_RegisterOutput, bRegister);
}

STDMETHODIMP CVCLinkerTool::get_AdditionalLibraryDirectories(BSTR* pbstrLibPath)
{	// (/LIBPATH:[dir]) specify path to search for libraries on, can have multiple
	return ToolGetStrProperty(VCLINKID_AdditionalLibraryDirectories, pbstrLibPath, true /* local only */);
}

STDMETHODIMP CVCLinkerTool::put_AdditionalLibraryDirectories(BSTR bstrLibPath)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_AdditionalLibraryDirectories, bstrLibPath);
}

// input
STDMETHODIMP CVCLinkerTool::get_AdditionalDependencies(BSTR* pbstrDependencies)
{	// additional inputs to the link line (comdlg32.lib, etc.)
	return ToolGetStrProperty(VCLINKID_AdditionalDependencies, pbstrDependencies, true /* local only */);
}

STDMETHODIMP CVCLinkerTool::put_AdditionalDependencies(BSTR bstrDependencies)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_AdditionalDependencies, bstrDependencies);
}

STDMETHODIMP CVCLinkerTool::get_IgnoreAllDefaultLibraries(VARIANT_BOOL* pbNoDefaults)
{	// (/NODEFAULTLIB) ignore all default libraries
	return ToolGetBoolProperty(VCLINKID_IgnoreAllDefaultLibraries, pbNoDefaults);
}

STDMETHODIMP CVCLinkerTool::put_IgnoreAllDefaultLibraries(VARIANT_BOOL bNoDefaults)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bNoDefaults );
	return m_spPropertyContainer->SetBoolProperty(VCLINKID_IgnoreAllDefaultLibraries, bNoDefaults);
}

STDMETHODIMP CVCLinkerTool::get_IgnoreDefaultLibraryNames(BSTR* pbstrLib)
{	// (/NODEFAULTLIB:[name]) ignore particular default library, can have multiple
	return ToolGetStrProperty(VCLINKID_IgnoreDefaultLibraryNames, pbstrLib, true /* local only */);
}

STDMETHODIMP CVCLinkerTool::put_IgnoreDefaultLibraryNames(BSTR bstrLib)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_IgnoreDefaultLibraryNames, bstrLib);
}

STDMETHODIMP CVCLinkerTool::get_ModuleDefinitionFile(BSTR* pbstrDefFile)
{	// (/DEF:file) use/specify module definition file
	return ToolGetStrProperty(VCLINKID_ModuleDefinitionFile, pbstrDefFile);
}

STDMETHODIMP CVCLinkerTool::put_ModuleDefinitionFile(BSTR bstrDefFile)
{
	CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
	return m_spPropertyContainer->SetStrProperty(VCLINKID_ModuleDefinitionFile, bstrDefFile);
}

STDMETHODIMP CVCLinkerTool::get_AddModuleNamesToAssembly(BSTR* pbstrNonAssy)
{	// (/ASSEMBLYMODULE:file) imports a non-assembly file
	return ToolGetStrProperty(VCLINKID_AddModuleNamesToAssembly, pbstrNonAssy, true /* local only */);
}

STDMETHODIMP CVCLinkerTool::put_AddModuleNamesToAssembly(BSTR bstrNonAssy)
{
	CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
	return m_spPropertyContainer->SetStrProperty(VCLINKID_AddModuleNamesToAssembly, bstrNonAssy);
}

STDMETHODIMP CVCLinkerTool::get_EmbedManagedResourceFile(BSTR* pbstrRes)
{	// (/ASSEMBLYRESOURCE:file) embed an assembly resource file
	return ToolGetStrProperty(VCLINKID_EmbedManagedResourceFile, pbstrRes, true /* local only */);
}

STDMETHODIMP CVCLinkerTool::put_EmbedManagedResourceFile(BSTR bstrRes)
{
	CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
	return m_spPropertyContainer->SetStrProperty(VCLINKID_EmbedManagedResourceFile, bstrRes);
}

STDMETHODIMP CVCLinkerTool::get_ForceSymbolReferences(BSTR* pbstrSymbol)
{	// (/INCLUDE:[symbol]) force symbol reference, can have multiple
	return ToolGetStrProperty(VCLINKID_ForceSymbolReferences, pbstrSymbol, true /* local only */);
}

STDMETHODIMP CVCLinkerTool::put_ForceSymbolReferences(BSTR bstrSymbol)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_ForceSymbolReferences, bstrSymbol);
}

STDMETHODIMP CVCLinkerTool::get_DelayLoadDLLs(BSTR* pbstrDLLName)
{	// (/DELAYLOAD:[dll_name]) delay load specified DLL, can have multiple
	return ToolGetStrProperty(VCLINKID_DelayLoadDLLs, pbstrDLLName, true /* local only */);
}

STDMETHODIMP CVCLinkerTool::put_DelayLoadDLLs(BSTR bstrDLLName)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_DelayLoadDLLs, bstrDLLName);
}

// MIDL
STDMETHODIMP CVCLinkerTool::get_MidlCommandFile(BSTR* pbstrMidlCmdFile)
{	// (/midl:<@midl cmd file>) specify response file for MIDL commands to use
	return ToolGetStrProperty(VCLINKID_MidlCommandFile, pbstrMidlCmdFile);
}

STDMETHODIMP CVCLinkerTool::put_MidlCommandFile(BSTR bstrMidlCmdFile)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_MidlCommandFile, bstrMidlCmdFile);
}

STDMETHODIMP CVCLinkerTool::get_IgnoreEmbeddedIDL(VARIANT_BOOL* pbIgnoreIDL)
{ 	// (/ignoreidl) ignore .idlsym sections of .obj files
	return ToolGetBoolProperty(VCLINKID_IgnoreEmbeddedIDL, pbIgnoreIDL);
}

STDMETHODIMP CVCLinkerTool::put_IgnoreEmbeddedIDL(VARIANT_BOOL bIgnoreIDL)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bIgnoreIDL );
	return m_spPropertyContainer->SetBoolProperty(VCLINKID_IgnoreEmbeddedIDL, bIgnoreIDL);
}

STDMETHODIMP CVCLinkerTool::get_MergedIDLBaseFileName(BSTR* pbstrIDLFile)
{	// (/idlout:<filename>) name intermediate IDL output file
	return ToolGetStrProperty(VCLINKID_MergedIDLBaseFileName, pbstrIDLFile);
}

STDMETHODIMP CVCLinkerTool::put_MergedIDLBaseFileName(BSTR bstrIDLFile)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_MergedIDLBaseFileName, bstrIDLFile);
}

STDMETHODIMP CVCLinkerTool::get_TypeLibraryFile(BSTR* pbstrTLBFile)
{	// (/tlbout:<filename>) name intermediate typelib output file
	return ToolGetStrProperty(VCLINKID_TypeLibraryFile, pbstrTLBFile);
}

STDMETHODIMP CVCLinkerTool::put_TypeLibraryFile(BSTR bstrTLBFile)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_TypeLibraryFile, bstrTLBFile);
}

STDMETHODIMP CVCLinkerTool::get_TypeLibraryResourceID(long* pnResID)
{	// (/tlbid:<id>) specify resource ID for generated .tlb file
	return ToolGetIntProperty(VCLINKID_TypeLibraryResourceID, pnResID);
}

STDMETHODIMP CVCLinkerTool::put_TypeLibraryResourceID(long nResID)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_POSITIVE(nResID);
	return m_spPropertyContainer->SetIntProperty(VCLINKID_TypeLibraryResourceID, nResID);
}

// output
STDMETHODIMP CVCLinkerTool::get_LinkDLL(VARIANT_BOOL* pbDLL)
{	// (/DLL) build a DLL as the main output
	return ToolGetBoolProperty(VCLINKID_LinkDLL, pbDLL);
}

STDMETHODIMP CVCLinkerTool::put_LinkDLL(VARIANT_BOOL bDLL)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bDLL );
	return m_spPropertyContainer->SetBoolProperty(VCLINKID_LinkDLL, bDLL);
}

// debug
STDMETHODIMP CVCLinkerTool::get_GenerateDebugInformation(VARIANT_BOOL* pbDebug)
{	// (/DEBUG) generate debug info
	return ToolGetBoolProperty(VCLINKID_GenerateDebugInformation, pbDebug);
}

STDMETHODIMP CVCLinkerTool::put_GenerateDebugInformation(VARIANT_BOOL bDebug)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bDebug );
	return m_spPropertyContainer->SetBoolProperty(VCLINKID_GenerateDebugInformation, bDebug);
}

STDMETHODIMP CVCLinkerTool::get_ProgramDatabaseFile(BSTR* pbstrFile)
{	// (/PDB:file) use program database
	return ToolGetStrProperty(VCLINKID_ProgramDatabaseFile, pbstrFile);
}

STDMETHODIMP CVCLinkerTool::put_ProgramDatabaseFile(BSTR bstrFile)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_ProgramDatabaseFile, bstrFile);
}

STDMETHODIMP CVCLinkerTool::get_StripPrivateSymbols(BSTR* pbstrStrippedPDB)
{	// (/PDBSTRIPPED:file) strip private symbols from PDB
	return ToolGetStrProperty(VCLINKID_StripPrivateSymbols, pbstrStrippedPDB);
}

STDMETHODIMP CVCLinkerTool::put_StripPrivateSymbols(BSTR bstrStrippedPDB)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_StripPrivateSymbols, bstrStrippedPDB);
}

STDMETHODIMP CVCLinkerTool::get_GenerateMapFile(VARIANT_BOOL* pbMap)
{	// (/MAP[:file]) generate map file during linking
	return ToolGetBoolProperty(VCLINKID_GenerateMapFile, pbMap);
}

STDMETHODIMP CVCLinkerTool::put_GenerateMapFile(VARIANT_BOOL bMap)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bMap );
	return m_spPropertyContainer->SetBoolProperty(VCLINKID_GenerateMapFile, bMap);
}

STDMETHODIMP CVCLinkerTool::get_MapFileName(BSTR* pbstrMapFile)
{	// optional argument to GenerateMapFile property
	return ToolGetStrProperty(VCLINKID_MapFileName, pbstrMapFile);
}

STDMETHODIMP CVCLinkerTool::put_MapFileName(BSTR bstrMapFile)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_MapFileName, bstrMapFile);
}

STDMETHODIMP CVCLinkerTool::get_MapExports(VARIANT_BOOL* pbExports)
{	// (/MAPINFO:EXPORTS) include exported functions in map info
	return ToolGetBoolProperty(VCLINKID_MapExports, pbExports);
}

STDMETHODIMP CVCLinkerTool::put_MapExports(VARIANT_BOOL bExports)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bExports );
	return m_spPropertyContainer->SetBoolProperty(VCLINKID_MapExports, bExports);
}

STDMETHODIMP CVCLinkerTool::get_MapLines(VARIANT_BOOL* pbLines)
{	// (/MAPINFO:LINES) include line number info in map info
	return ToolGetBoolProperty(VCLINKID_MapLines, pbLines);
}

STDMETHODIMP CVCLinkerTool::put_MapLines(VARIANT_BOOL bLines)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bLines );
	return m_spPropertyContainer->SetBoolProperty(VCLINKID_MapLines, bLines);
}

// System
STDMETHODIMP CVCLinkerTool::get_SubSystem(subSystemOption* poptSetting)
{	// see enum above (/SUBSYSTEM)
	return ToolGetIntProperty(VCLINKID_SubSystem, (long *)poptSetting);
}

STDMETHODIMP CVCLinkerTool::put_SubSystem(subSystemOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, LinkSubSystemMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCLINKID_SubSystem, optSetting);
}

STDMETHODIMP CVCLinkerTool::get_HeapReserveSize(long* pnReserveSize)
{	// (/HEAP:reserve[,commit]) total heap allocation size in virtual memory
	return ToolGetIntProperty(VCLINKID_HeapReserveSize, pnReserveSize);
}

STDMETHODIMP CVCLinkerTool::put_HeapReserveSize(long nReserveSize)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_POSITIVE(nReserveSize);
	return m_spPropertyContainer->SetIntProperty(VCLINKID_HeapReserveSize, nReserveSize);
}

STDMETHODIMP CVCLinkerTool::get_HeapCommitSize(long* pnCommitSize)
{	// (/HEAP:reserve[,commit]) total heap allocation size in physical memory
	return ToolGetIntProperty(VCLINKID_HeapCommitSize, pnCommitSize);
}

STDMETHODIMP CVCLinkerTool::put_HeapCommitSize(long nCommitSize)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_POSITIVE(nCommitSize);
	return m_spPropertyContainer->SetIntProperty(VCLINKID_HeapCommitSize, nCommitSize);
}

STDMETHODIMP CVCLinkerTool::get_StackReserveSize(long* pnReserveSize)
{	// (/STACK:reserve[,commit]) total stack allocation size in virtual memory
	return ToolGetIntProperty(VCLINKID_StackReserveSize, pnReserveSize);
}

STDMETHODIMP CVCLinkerTool::put_StackReserveSize(long nReserveSize)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_POSITIVE(nReserveSize);
	return m_spPropertyContainer->SetIntProperty(VCLINKID_StackReserveSize, nReserveSize);
}

STDMETHODIMP CVCLinkerTool::get_StackCommitSize(long* pnCommitSize)
{	// (/STACK:reserve[,commit]) total stack allocation size in physical memory
	return ToolGetIntProperty(VCLINKID_StackCommitSize, pnCommitSize);
}

STDMETHODIMP CVCLinkerTool::put_StackCommitSize(long nCommitSize)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_POSITIVE(nCommitSize);
	return m_spPropertyContainer->SetIntProperty(VCLINKID_StackCommitSize, nCommitSize);
}

STDMETHODIMP CVCLinkerTool::get_LargeAddressAware(addressAwarenessType* poptSetting)
{	// (/LARGEADDRESSAWARE[:NO]) tells the linker the app can handle addresses greater than 2GB
	return ToolGetIntProperty(VCLINKID_LargeAddressAware, (long *)poptSetting);
}

STDMETHODIMP CVCLinkerTool::put_LargeAddressAware(addressAwarenessType optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, LargeAddrMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCLINKID_LargeAddressAware, optSetting);
}

STDMETHODIMP CVCLinkerTool::get_TerminalServerAware(termSvrAwarenessType* poptSetting)
{	// (/TSAWARE, /TSAWARE:NO, not set) not in docs
	return ToolGetIntProperty(VCLINKID_TerminalServerAware, (long *)poptSetting);
}

STDMETHODIMP CVCLinkerTool::put_TerminalServerAware(termSvrAwarenessType optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, TSAwareMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCLINKID_TerminalServerAware, optSetting);
}

STDMETHODIMP CVCLinkerTool::get_SwapRunFromCD(VARIANT_BOOL* pbRun)
{	// swap run from CD (/SWAPRUN:CD)
	return ToolGetBoolProperty(VCLINKID_SwapRunFromCD, pbRun);
}

STDMETHODIMP CVCLinkerTool::put_SwapRunFromCD(VARIANT_BOOL bRun)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bRun );
	return m_spPropertyContainer->SetBoolProperty(VCLINKID_SwapRunFromCD, bRun);
}

STDMETHODIMP CVCLinkerTool::get_SwapRunFromNet(VARIANT_BOOL* pbRun)
{	// swap run from the net (/SWAPRUN:NET)
	return ToolGetBoolProperty(VCLINKID_SwapRunFromNet, pbRun);
}

STDMETHODIMP CVCLinkerTool::put_SwapRunFromNet(VARIANT_BOOL bRun)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bRun );
	return m_spPropertyContainer->SetBoolProperty(VCLINKID_SwapRunFromNet, bRun);
}

// optimization
STDMETHODIMP CVCLinkerTool::get_OptimizeReferences(optRefType* poptSetting)
{	// (/OPT:REF, /OPT:NOREF, not set) eliminate/keep functions & data never referenced
	return ToolGetIntProperty(VCLINKID_OptimizeReferences, (long *)poptSetting);
}

STDMETHODIMP CVCLinkerTool::put_OptimizeReferences(optRefType optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, OptRefMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCLINKID_OptimizeReferences, optSetting);
}

STDMETHODIMP CVCLinkerTool::get_EnableCOMDATFolding(optFoldingType* poptSetting)
{	// (/OPT:ICF, /OPT:NOICF, not set) eliminate/keep redundant COMDAT data (data folding)
	return ToolGetIntProperty(VCLINKID_EnableCOMDATFolding, (long *)poptSetting);
}

STDMETHODIMP CVCLinkerTool::put_EnableCOMDATFolding(optFoldingType optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, OptFoldMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCLINKID_EnableCOMDATFolding, optSetting);
}

STDMETHODIMP CVCLinkerTool::get_OptimizeForWindows98(optWin98Type* poptSetting)
{	// (/OPT:WIN98, /OPT:NOWIN98, not set) 
	return ToolGetIntProperty(VCLINKID_OptimizeForWindows98, (long *)poptSetting);
}

STDMETHODIMP CVCLinkerTool::put_OptimizeForWindows98(optWin98Type optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, OptWin98Max, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCLINKID_OptimizeForWindows98, optSetting);
}

STDMETHODIMP CVCLinkerTool::get_FunctionOrder(BSTR* pbstrOrder)
{	// (/ORDER:@[file]) place functions in order specified in file
	return ToolGetStrProperty(VCLINKID_FunctionOrder, pbstrOrder);
}

STDMETHODIMP CVCLinkerTool::put_FunctionOrder(BSTR bstrOrder)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_FunctionOrder, bstrOrder);
}

STDMETHODIMP CVCLinkerTool::get_LinkTimeCodeGeneration(VARIANT_BOOL* pbCodeGen)
{	// (/LTCG) enable link time code gen of objects compiled with /GL
	return ToolGetBoolProperty(VCLINKID_LinkTimeCodeGeneration, pbCodeGen);
}

STDMETHODIMP CVCLinkerTool::put_LinkTimeCodeGeneration(VARIANT_BOOL bCodeGen)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bCodeGen );
	return m_spPropertyContainer->SetBoolProperty(VCLINKID_LinkTimeCodeGeneration, bCodeGen);
}

// Automation properties
STDMETHODIMP CVCLinkerTool::get_ToolPath(BSTR *pVal)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_POINTER_VALID(pVal);
	*pVal = SysAllocString( szLinkerToolPath );
	return S_OK;
}

// Advanced
STDMETHODIMP CVCLinkerTool::get_EntryPointSymbol(BSTR* pbstrEntry)
{	// (/ENTRY:[symbol]) set entry point address for EXE or DLL incompatible with /NOENTRY
	return ToolGetStrProperty(VCLINKID_EntryPointSymbol, pbstrEntry);
}

STDMETHODIMP CVCLinkerTool::put_EntryPointSymbol(BSTR bstrEntry)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_EntryPointSymbol, bstrEntry);
}

STDMETHODIMP CVCLinkerTool::get_ResourceOnlyDLL(VARIANT_BOOL* pbNoEntry)
{	// (/NOENTRY) no entry point.  required for resource-only DLLs incompatible with /ENTRY
	return ToolGetBoolProperty(VCLINKID_ResourceOnlyDLL, pbNoEntry);
}

STDMETHODIMP CVCLinkerTool::put_ResourceOnlyDLL(VARIANT_BOOL bNoEntry)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bNoEntry );
	return m_spPropertyContainer->SetBoolProperty(VCLINKID_ResourceOnlyDLL, bNoEntry);
}

STDMETHODIMP CVCLinkerTool::get_SetChecksum(VARIANT_BOOL* pbRelease)
{	// (/RELEASE) set the checksum in the header of a .exe
	return ToolGetBoolProperty(VCLINKID_SetChecksum, pbRelease);
}

STDMETHODIMP CVCLinkerTool::put_SetChecksum(VARIANT_BOOL bRelease)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bRelease );
	return m_spPropertyContainer->SetBoolProperty(VCLINKID_SetChecksum, bRelease);
}

STDMETHODIMP CVCLinkerTool::get_BaseAddress(BSTR* pbstrAddress)
{	// (/BASE:{address| filename,key}) base address to place program at can be numeric or string
	return ToolGetStrProperty(VCLINKID_BaseAddress, pbstrAddress);
}

STDMETHODIMP CVCLinkerTool::put_BaseAddress(BSTR bstrAddress)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_BaseAddress, bstrAddress);
}

STDMETHODIMP CVCLinkerTool::get_TurnOffAssemblyGeneration(VARIANT_BOOL* pbTurnOffAssemblyGeneration)
{	// (/NOASSEMBLY) cause the output file to be built without an assembly
	return ToolGetBoolProperty(VCLINKID_TurnOffAssemblyGeneration, pbTurnOffAssemblyGeneration);
}

STDMETHODIMP CVCLinkerTool::put_TurnOffAssemblyGeneration(VARIANT_BOOL bTurnOffAssemblyGeneration)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bTurnOffAssemblyGeneration );
	return m_spPropertyContainer->SetBoolProperty(VCLINKID_TurnOffAssemblyGeneration, bTurnOffAssemblyGeneration);
}

STDMETHODIMP CVCLinkerTool::get_SupportUnloadOfDelayLoadedDLL(VARIANT_BOOL* pbSupportUnloadOfDelayLoadedDLL)
{	// (/DELAY:UNLOAD) use to allow explicit unloading of the DLL
	return ToolGetBoolProperty(VCLINKID_SupportUnloadOfDelayLoadedDLL, pbSupportUnloadOfDelayLoadedDLL);
}

STDMETHODIMP CVCLinkerTool::put_SupportUnloadOfDelayLoadedDLL(VARIANT_BOOL bSupportUnloadOfDelayLoadedDLL)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bSupportUnloadOfDelayLoadedDLL );
	return m_spPropertyContainer->SetBoolProperty(VCLINKID_SupportUnloadOfDelayLoadedDLL, bSupportUnloadOfDelayLoadedDLL);
}

STDMETHODIMP CVCLinkerTool::get_ImportLibrary(BSTR* pbstrImportLib)
{	// (/IMPLIB:[library]) generate specified import library
	return ToolGetStrProperty(VCLINKID_ImportLibrary, pbstrImportLib);
}

STDMETHODIMP CVCLinkerTool::put_ImportLibrary(BSTR bstrImportLib)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_ImportLibrary, bstrImportLib);
}

STDMETHODIMP CVCLinkerTool::get_MergeSections(BSTR* pbstrMerge)
{	// (/MERGE:from=to) merge section 'from' into section 'to'
	return ToolGetStrProperty(VCLINKID_MergeSections, pbstrMerge);
}

STDMETHODIMP CVCLinkerTool::put_MergeSections(BSTR bstrMerge)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCLINKID_MergeSections, bstrMerge);
}

STDMETHODIMP CVCLinkerTool::get_TargetMachine(machineTypeOption* poptSetting)
{
	return ToolGetIntProperty(VCLINKID_TargetMachine, (long *)poptSetting);
}

STDMETHODIMP CVCLinkerTool::put_TargetMachine(machineTypeOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, TargetMachineMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCLINKID_TargetMachine, optSetting);
}

/*********************************************************************/

STDMETHODIMP CVCLinkerGeneralPage::get_AdditionalOptions(BSTR* pbstrAdditionalOptions)
{	// anything we don't know what to do with, given verbatim, in same order as entered, parent contributions first, goes after all other options
	return ToolGetStrProperty(VCLINKID_AdditionalOptions, &(CVCLinkerTool::s_optHandler), pbstrAdditionalOptions);
}

STDMETHODIMP CVCLinkerGeneralPage::put_AdditionalOptions(BSTR bstrAdditionalOptions)
{
	return SetStrProperty(VCLINKID_AdditionalOptions, bstrAdditionalOptions);
}

STDMETHODIMP CVCLinkerGeneralPage::get_OutputFile(BSTR* pbstrOut)
{	// (/OUT:[file]) change the output file name (default is based on 1st lib or obj name on cmd line)
	return ToolGetStrProperty(VCLINKID_OutputFile, &(CVCLinkerTool::s_optHandler), pbstrOut);
}

STDMETHODIMP CVCLinkerGeneralPage::put_OutputFile(BSTR bstrOut)
{
	return SetStrProperty(VCLINKID_OutputFile, bstrOut);
}

STDMETHODIMP CVCLinkerGeneralPage::get_ShowProgress(linkProgressOption* poptSetting)
{	// see enum above (/VERBOSE)
	return ToolGetIntProperty(VCLINKID_ShowProgress, &(CVCLinkerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCLinkerGeneralPage::put_ShowProgress(linkProgressOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, ShowProgressMax, optSetting);
	return SetIntProperty(VCLINKID_ShowProgress, optSetting);
}

STDMETHODIMP CVCLinkerGeneralPage::get_Version(BSTR* pbstrVersion)
{	// (/VERSION:vers) put this version number into header of created image
	return ToolGetStrProperty(VCLINKID_Version, &(CVCLinkerTool::s_optHandler), pbstrVersion);
}

STDMETHODIMP CVCLinkerGeneralPage::put_Version(BSTR bstrVersion)
{
	return SetStrProperty(VCLINKID_Version, bstrVersion);
}

STDMETHODIMP CVCLinkerGeneralPage::get_LinkIncremental(linkIncrementalType* poptSetting)
{	// (/INCREMENTAL:YES, /INCREMENTAL:NO, not set)
	return ToolGetIntProperty(VCLINKID_LinkIncremental, &(CVCLinkerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCLinkerGeneralPage::put_LinkIncremental(linkIncrementalType optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, LinkIncrMax, optSetting);
	return SetIntProperty(VCLINKID_LinkIncremental, optSetting);
}

STDMETHODIMP CVCLinkerGeneralPage::get_SuppressStartupBanner(enumSuppressStartupBannerUpBOOL* pbNoLogo)
{	// (/NOLOGO) enable suppression of copyright message (no explicit off)
	return GetEnumBoolProperty2(VCLINKID_SuppressStartupBanner, (long *)pbNoLogo);
}

STDMETHODIMP CVCLinkerGeneralPage::put_SuppressStartupBanner(enumSuppressStartupBannerUpBOOL bNoLogo)
{
	return SetBoolProperty(VCLINKID_SuppressStartupBanner, bNoLogo);
}

STDMETHODIMP CVCLinkerGeneralPage::get_IgnoreImportLibrary(enumBOOL* pbIgnore)
{	// ignore export lib
	return GetEnumBoolProperty(VCLINKID_IgnoreImportLibrary, pbIgnore);
}

STDMETHODIMP CVCLinkerGeneralPage::put_IgnoreImportLibrary(enumBOOL bIgnore)
{
	return SetBoolProperty(VCLINKID_IgnoreImportLibrary, bIgnore);
}

STDMETHODIMP CVCLinkerGeneralPage::get_RegisterOutput(enumBOOL* pbRegister)
{	// register the primary output of the build
	return GetEnumBoolProperty(VCLINKID_RegisterOutput, pbRegister);
}

STDMETHODIMP CVCLinkerGeneralPage::put_RegisterOutput(enumBOOL bRegister)
{
	return SetBoolProperty(VCLINKID_RegisterOutput, bRegister);
}

STDMETHODIMP CVCLinkerGeneralPage::get_AdditionalLibraryDirectories(BSTR* pbstrLibPath)
{	// (/LIBPATH:[dir]) specify path to search for libraries on, can have multiple
	return ToolGetStrProperty(VCLINKID_AdditionalLibraryDirectories, &(CVCLinkerTool::s_optHandler), pbstrLibPath, true /* local only */);
}

STDMETHODIMP CVCLinkerGeneralPage::put_AdditionalLibraryDirectories(BSTR bstrLibPath)
{
	return SetStrProperty(VCLINKID_AdditionalLibraryDirectories, bstrLibPath);
}

void CVCLinkerGeneralPage::GetBaseDefault(long id, CComVariant& varValue)
{
	switch (id)
	{
	case VCLINKID_ImportLibrary:
	case VCLINKID_OutputFile:
		{
			CComBSTR bstrVal;
			CVCLinkerTool::s_optHandler.GetDefaultValue( id, &bstrVal, m_pContainer );
			if (!bstrVal)
				bstrVal = L"";
			varValue = bstrVal;
		}
		break;
	case VCLINKID_SuppressStartupBanner:
		{
			VARIANT_BOOL bVal;
			CVCLinkerTool::s_optHandler.GetDefaultValue( id, &bVal );
			varValue = bVal;
		}
		break;
	default:
		CBase::GetBaseDefault(id, varValue);
	}
}

// input
STDMETHODIMP CVCLinkerInputPage::get_AdditionalDependencies(BSTR* pbstrInputs)
{	// additional inputs to the link line (comdlg32.lib, etc.)
	return ToolGetStrProperty(VCLINKID_AdditionalDependencies, &(CVCLinkerTool::s_optHandler), pbstrInputs, true /* local only */);
}

STDMETHODIMP CVCLinkerInputPage::put_AdditionalDependencies(BSTR bstrInputs)
{
	return SetStrProperty(VCLINKID_AdditionalDependencies, bstrInputs);
}

STDMETHODIMP CVCLinkerInputPage::get_IgnoreAllDefaultLibraries(enumIgnoreAllDefaultLibrariesBOOL* pbNoDefaults)
{	// (/NODEFAULTLIB) ignore all default libraries
	return GetEnumBoolProperty2(VCLINKID_IgnoreAllDefaultLibraries, (long *)pbNoDefaults);
}

STDMETHODIMP CVCLinkerInputPage::put_IgnoreAllDefaultLibraries(enumIgnoreAllDefaultLibrariesBOOL bNoDefaults)
{
	return SetBoolProperty(VCLINKID_IgnoreAllDefaultLibraries, bNoDefaults);
}

STDMETHODIMP CVCLinkerInputPage::get_IgnoreDefaultLibraryNames(BSTR* pbstrLib)
{	// (/NODEFAULTLIB:[name]) ignore particular default library, can have multiple
	return ToolGetStrProperty(VCLINKID_IgnoreDefaultLibraryNames, &(CVCLinkerTool::s_optHandler), pbstrLib, true /* local only */);
}

STDMETHODIMP CVCLinkerInputPage::put_IgnoreDefaultLibraryNames(BSTR bstrLib)
{
	return SetStrProperty(VCLINKID_IgnoreDefaultLibraryNames, bstrLib);
}

STDMETHODIMP CVCLinkerInputPage::get_ModuleDefinitionFile(BSTR* pbstrDefFile)
{	// (/DEF:file) use/specify module definition file
	return ToolGetStrProperty(VCLINKID_ModuleDefinitionFile, &(CVCLinkerTool::s_optHandler), pbstrDefFile);
}

STDMETHODIMP CVCLinkerInputPage::put_ModuleDefinitionFile(BSTR bstrDefFile)
{
	return SetStrProperty(VCLINKID_ModuleDefinitionFile, bstrDefFile);
}

STDMETHODIMP CVCLinkerInputPage::get_AddModuleNamesToAssembly(BSTR* pbstrNonAssy)
{	// (/ASSEMBLYMODULE:file) imports a non-assembly file
	return ToolGetStrProperty(VCLINKID_AddModuleNamesToAssembly, &(CVCLinkerTool::s_optHandler), pbstrNonAssy, true /* local only */);
}

STDMETHODIMP CVCLinkerInputPage::put_AddModuleNamesToAssembly(BSTR bstrNonAssy)
{
	return SetStrProperty(VCLINKID_AddModuleNamesToAssembly, bstrNonAssy);
}

STDMETHODIMP CVCLinkerInputPage::get_EmbedManagedResourceFile(BSTR* pbstrRes)
{	// (/ASSEMBLYRESOURCE:file) embed a managed resource file
	return ToolGetStrProperty(VCLINKID_EmbedManagedResourceFile, &(CVCLinkerTool::s_optHandler), pbstrRes, true /* local only */);
}

STDMETHODIMP CVCLinkerInputPage::put_EmbedManagedResourceFile(BSTR bstrRes)
{
	return SetStrProperty(VCLINKID_EmbedManagedResourceFile, bstrRes);
}

STDMETHODIMP CVCLinkerInputPage::get_ForceSymbolReferences(BSTR* pbstrSymbol)
{	// (/INCLUDE:[symbol]) force symbol reference, can have multiple
	return ToolGetStrProperty(VCLINKID_ForceSymbolReferences, &(CVCLinkerTool::s_optHandler), pbstrSymbol, true /* local only */);
}

STDMETHODIMP CVCLinkerInputPage::put_ForceSymbolReferences(BSTR bstrSymbol)
{
	return SetStrProperty(VCLINKID_ForceSymbolReferences, bstrSymbol);
}

STDMETHODIMP CVCLinkerInputPage::get_DelayLoadDLLs(BSTR* pbstrDLLName)
{	// (/DELAYLOAD:[dll_name]) delay load specified DLL, can have multiple
	return ToolGetStrProperty(VCLINKID_DelayLoadDLLs, &(CVCLinkerTool::s_optHandler), pbstrDLLName, true /* local only */);
}

STDMETHODIMP CVCLinkerInputPage::put_DelayLoadDLLs(BSTR bstrDLLName)
{
	return SetStrProperty(VCLINKID_DelayLoadDLLs, bstrDLLName);
}

// debug
STDMETHODIMP CVCLinkerDebugPage::get_GenerateDebugInformation(enumGenerateDebugInformationBOOL* pbDebug)
{	// (/DEBUG) generate debug info
	return GetEnumBoolProperty2(VCLINKID_GenerateDebugInformation, (long *)pbDebug);
}

STDMETHODIMP CVCLinkerDebugPage::put_GenerateDebugInformation(enumGenerateDebugInformationBOOL bDebug)
{
	return SetBoolProperty(VCLINKID_GenerateDebugInformation, bDebug);
}

STDMETHODIMP CVCLinkerDebugPage::get_ProgramDatabaseFile(BSTR* pbstrFile)
{	// (/PDB:file) use program database
	return ToolGetStrProperty(VCLINKID_ProgramDatabaseFile, &(CVCLinkerTool::s_optHandler), pbstrFile);
}

STDMETHODIMP CVCLinkerDebugPage::put_ProgramDatabaseFile(BSTR bstrFile)
{
	return SetStrProperty(VCLINKID_ProgramDatabaseFile, bstrFile);
}

STDMETHODIMP CVCLinkerDebugPage::get_StripPrivateSymbols(BSTR* pbstrStrippedPDB)
{	// (/PDBSTRIPPED:file) strip private symbols from PDB
	return ToolGetStrProperty(VCLINKID_StripPrivateSymbols, &(CVCLinkerTool::s_optHandler), pbstrStrippedPDB);
}

STDMETHODIMP CVCLinkerDebugPage::put_StripPrivateSymbols(BSTR bstrStrippedPDB)
{
	return SetStrProperty(VCLINKID_StripPrivateSymbols, bstrStrippedPDB);
}

STDMETHODIMP CVCLinkerDebugPage::get_GenerateMapFile(enumGenerateMapFileBOOL* pbMap)
{	// (/MAP[:file]) generate map file during linking
	return GetEnumBoolProperty2(VCLINKID_GenerateMapFile, (long *)pbMap);
}

STDMETHODIMP CVCLinkerDebugPage::put_GenerateMapFile(enumGenerateMapFileBOOL bMap)
{
	return SetBoolProperty(VCLINKID_GenerateMapFile, bMap);
}

STDMETHODIMP CVCLinkerDebugPage::get_MapFileName(BSTR* pbstrMapFile)
{	// optional argument to GenerateMapFile property
	return ToolGetStrProperty(VCLINKID_MapFileName, &(CVCLinkerTool::s_optHandler), pbstrMapFile);
}

STDMETHODIMP CVCLinkerDebugPage::put_MapFileName(BSTR bstrMapFile)
{
	return SetStrProperty(VCLINKID_MapFileName, bstrMapFile);
}

STDMETHODIMP CVCLinkerDebugPage::get_MapExports(enumMapExportsBOOL* pbExports)
{	// (/MAPINFO:EXPORTS) include exported functions in map info
	return GetEnumBoolProperty2(VCLINKID_MapExports, (long *)pbExports);
}

STDMETHODIMP CVCLinkerDebugPage::put_MapExports(enumMapExportsBOOL bExports)
{
	return SetBoolProperty(VCLINKID_MapExports, bExports);
}

STDMETHODIMP CVCLinkerDebugPage::get_MapLines(enumMapLinesBOOL* pbLines)
{	// (/MAPINFO:LINES) include line number info in map info
	return GetEnumBoolProperty2(VCLINKID_MapLines, (long *)pbLines);
}

STDMETHODIMP CVCLinkerDebugPage::put_MapLines(enumMapLinesBOOL bLines)
{
	return SetBoolProperty(VCLINKID_MapLines, bLines);
}

void CVCLinkerDebugPage::GetBaseDefault(long id, CComVariant& varValue)
{
	if (id == VCLINKID_ProgramDatabaseFile)
	{
		CComBSTR bstrVal;
		CVCLinkerTool::s_optHandler.GetDefaultValue( VCLINKID_ProgramDatabaseFile, &bstrVal );
		varValue = bstrVal;
	}
	else
		CBase::GetBaseDefault(id, varValue);
}

// System
STDMETHODIMP CVCLinkerSystemPage::HideProperty( DISPID dispid, BOOL *pfHide)
{
	CHECK_POINTER_NULL(pfHide);

	*pfHide = FALSE;

	if( dispid == VCLINKID_SubSystem )
	{
		CComBSTR bstrPlatName;
		m_pContainer->GetStrProperty(VCPLATID_Name, &bstrPlatName);
		if (lstrcmpW(bstrPlatName, L"Xbox") == 0)
		{
			*pfHide = TRUE; // Always /Subsystem::Xbox
		}
	}
	return S_OK;
}

STDMETHODIMP CVCLinkerSystemPage::get_SubSystem(subSystemOption* poptSetting)
{	// see enum above (/SUBSYSTEM)
	return ToolGetIntProperty(VCLINKID_SubSystem, &(CVCLinkerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCLinkerSystemPage::put_SubSystem(subSystemOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, LinkSubSystemMax, optSetting);
	return SetIntProperty(VCLINKID_SubSystem, optSetting);
}

STDMETHODIMP CVCLinkerSystemPage::get_HeapReserveSize(long* pnReserveSize)
{	// (/HEAP:reserve[,commit]) total heap allocation size in virtual memory
	return ToolGetIntProperty(VCLINKID_HeapReserveSize, &(CVCLinkerTool::s_optHandler), pnReserveSize);
}

STDMETHODIMP CVCLinkerSystemPage::put_HeapReserveSize(long nReserveSize)
{
	CHECK_POSITIVE_FOR_PROP_PAGES(nReserveSize);
	return SetIntProperty(VCLINKID_HeapReserveSize, nReserveSize);
}

STDMETHODIMP CVCLinkerSystemPage::get_HeapCommitSize(long* pnCommitSize)
{	// (/HEAP:reserve[,commit]) total heap allocation size in physical memory
	return ToolGetIntProperty(VCLINKID_HeapCommitSize, &(CVCLinkerTool::s_optHandler), pnCommitSize);
}

STDMETHODIMP CVCLinkerSystemPage::put_HeapCommitSize(long nCommitSize)
{
	CHECK_POSITIVE_FOR_PROP_PAGES(nCommitSize);
	return SetIntProperty(VCLINKID_HeapCommitSize, nCommitSize);
}

STDMETHODIMP CVCLinkerSystemPage::get_StackReserveSize(long* pnReserveSize)
{	// (/STACK:reserve[,commit]) total stack allocation size in virtual memory
	return ToolGetIntProperty(VCLINKID_StackReserveSize, &(CVCLinkerTool::s_optHandler), pnReserveSize);
}

STDMETHODIMP CVCLinkerSystemPage::put_StackReserveSize(long nReserveSize)
{
	CHECK_POSITIVE_FOR_PROP_PAGES(nReserveSize);
	return SetIntProperty(VCLINKID_StackReserveSize, nReserveSize);
}

STDMETHODIMP CVCLinkerSystemPage::get_StackCommitSize(long* pnCommitSize)
{	// (/STACK:reserve[,commit]) total stack allocation size in physical memory
	return ToolGetIntProperty(VCLINKID_StackCommitSize, &(CVCLinkerTool::s_optHandler), pnCommitSize);
}

STDMETHODIMP CVCLinkerSystemPage::put_StackCommitSize(long nCommitSize)
{
	CHECK_POSITIVE_FOR_PROP_PAGES(nCommitSize);
	return SetIntProperty(VCLINKID_StackCommitSize, nCommitSize);
}

STDMETHODIMP CVCLinkerSystemPage::get_LargeAddressAware(addressAwarenessType* poptSetting)
{	// (/LARGEADDRESSAWARE[:NO]) tells the linker the app can handle addresses greater than 2GB
	return ToolGetIntProperty(VCLINKID_LargeAddressAware, &(CVCLinkerTool::s_optHandler),(long *)poptSetting );
}

STDMETHODIMP CVCLinkerSystemPage::put_LargeAddressAware(addressAwarenessType optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, LargeAddrMax, optSetting);
	return SetIntProperty(VCLINKID_LargeAddressAware, optSetting);
}

STDMETHODIMP CVCLinkerSystemPage::get_TerminalServerAware(termSvrAwarenessType* poptSetting)
{	// (/TSAWARE, /TSAWARE:NO, not set) not in docs
	return ToolGetIntProperty(VCLINKID_TerminalServerAware, &(CVCLinkerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCLinkerSystemPage::put_TerminalServerAware(termSvrAwarenessType optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, TSAwareMax, optSetting);
	return SetIntProperty(VCLINKID_TerminalServerAware, optSetting);
}

STDMETHODIMP CVCLinkerSystemPage::get_SwapRunFromCD(enumSwapRunFromCDBOOL* pbRun)
{	// swap run from CD (/SWAPRUN:CD)
	return GetEnumBoolProperty2(VCLINKID_SwapRunFromCD, (long *)pbRun);
}

STDMETHODIMP CVCLinkerSystemPage::put_SwapRunFromCD(enumSwapRunFromCDBOOL bRun)
{
	return SetBoolProperty(VCLINKID_SwapRunFromCD, bRun);
}

STDMETHODIMP CVCLinkerSystemPage::get_SwapRunFromNet(enumSwapRunFromNetBOOL* pbRun)
{	// swap run from the net (/SWAPRUN:NET)
	return GetEnumBoolProperty2(VCLINKID_SwapRunFromNet, (long *)pbRun);
}

STDMETHODIMP CVCLinkerSystemPage::put_SwapRunFromNet(enumSwapRunFromNetBOOL bRun)
{
	return SetBoolProperty(VCLINKID_SwapRunFromNet, bRun);
}

// optimization
STDMETHODIMP CVCLinkerOptimizationPage::get_OptimizeReferences(optRefType* poptSetting)
{	// (/OPT:REF, /OPT:NOREF, not set) eliminate/keep functions & data never referenced
	return ToolGetIntProperty(VCLINKID_OptimizeReferences, &(CVCLinkerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCLinkerOptimizationPage::put_OptimizeReferences(optRefType optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, OptRefMax, optSetting);
	return SetIntProperty(VCLINKID_OptimizeReferences, optSetting);
}

STDMETHODIMP CVCLinkerOptimizationPage::get_EnableCOMDATFolding(optFoldingType* poptSetting)
{	// (/OPT:ICF, /OPT:NOICF, not set) eliminate/keep redundant COMDAT data (data folding)
	return ToolGetIntProperty(VCLINKID_EnableCOMDATFolding, &(CVCLinkerTool::s_optHandler),(long *)poptSetting );
}

STDMETHODIMP CVCLinkerOptimizationPage::put_EnableCOMDATFolding(optFoldingType optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, OptFoldMax, optSetting);
	return SetIntProperty(VCLINKID_EnableCOMDATFolding, optSetting);
}

STDMETHODIMP CVCLinkerOptimizationPage::get_OptimizeForWindows98(optWin98Type* poptSetting)
{	// (/OPT:WIN98, /OPT:NOWIN98, not set) 
	return ToolGetIntProperty(VCLINKID_OptimizeForWindows98, &(CVCLinkerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCLinkerOptimizationPage::put_OptimizeForWindows98(optWin98Type optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, OptWin98Max, optSetting);
	return SetIntProperty(VCLINKID_OptimizeForWindows98, optSetting);
}

STDMETHODIMP CVCLinkerOptimizationPage::get_FunctionOrder(BSTR* pbstrOrder)
{	// (/ORDER:@[file]) place functions in order specified in file
	return ToolGetStrProperty(VCLINKID_FunctionOrder, &(CVCLinkerTool::s_optHandler), pbstrOrder);
}

STDMETHODIMP CVCLinkerOptimizationPage::put_FunctionOrder(BSTR bstrOrder)
{
	return SetStrProperty(VCLINKID_FunctionOrder, bstrOrder);
}

// MIDL page
STDMETHODIMP CVCLinkerMIDLPage::get_MidlCommandFile(BSTR* pbstrMidlCmdFile)
{	// (/midl:<@midl cmd file>) specify response file for MIDL commands to use
	return ToolGetStrProperty(VCLINKID_MidlCommandFile, &(CVCLinkerTool::s_optHandler), pbstrMidlCmdFile);
}

STDMETHODIMP CVCLinkerMIDLPage::put_MidlCommandFile(BSTR bstrMidlCmdFile)
{
	return SetStrProperty(VCLINKID_MidlCommandFile, bstrMidlCmdFile);
}

STDMETHODIMP CVCLinkerMIDLPage::get_IgnoreEmbeddedIDL(enumIgnoreEmbeddedIDLBOOL* pbIgnoreIDL)
{ 	// (/ignoreidl) ignore .idlsym sections of .obj files
	return GetEnumBoolProperty2(VCLINKID_IgnoreEmbeddedIDL, (long *)pbIgnoreIDL);
}

STDMETHODIMP CVCLinkerMIDLPage::put_IgnoreEmbeddedIDL(enumIgnoreEmbeddedIDLBOOL bIgnoreIDL)
{
	return SetBoolProperty(VCLINKID_IgnoreEmbeddedIDL, bIgnoreIDL);
}

STDMETHODIMP CVCLinkerMIDLPage::get_MergedIDLBaseFileName(BSTR* pbstrIDLFile)
{	// (/idlout:<filename>) name intermediate IDL output file
	return ToolGetStrProperty(VCLINKID_MergedIDLBaseFileName, &(CVCLinkerTool::s_optHandler), pbstrIDLFile);
}

STDMETHODIMP CVCLinkerMIDLPage::put_MergedIDLBaseFileName(BSTR bstrIDLFile)
{
	return SetStrProperty(VCLINKID_MergedIDLBaseFileName, bstrIDLFile);
}

STDMETHODIMP CVCLinkerMIDLPage::get_TypeLibraryFile(BSTR* pbstrTLBFile)
{	// (/tlbout:<filename>) name intermediate typelib output file
	return ToolGetStrProperty(VCLINKID_TypeLibraryFile, &(CVCLinkerTool::s_optHandler), pbstrTLBFile);
}

STDMETHODIMP CVCLinkerMIDLPage::put_TypeLibraryFile(BSTR bstrTLBFile)
{
	return SetStrProperty(VCLINKID_TypeLibraryFile, bstrTLBFile);
}

STDMETHODIMP CVCLinkerMIDLPage::get_TypeLibraryResourceID(long* pnResID)
{	// (/tlbid:<id>) specify resource ID for generated .tlb file
	return ToolGetIntProperty(VCLINKID_TypeLibraryResourceID, &(CVCLinkerTool::s_optHandler), pnResID);
}

STDMETHODIMP CVCLinkerMIDLPage::put_TypeLibraryResourceID(long nResID)
{
	CHECK_POSITIVE_FOR_PROP_PAGES(nResID);
	return SetIntProperty(VCLINKID_TypeLibraryResourceID, nResID);
}

// Advanced
STDMETHODIMP CVCLinkerAdvancedPage::get_EntryPointSymbol(BSTR* pbstrEntry)
{	// (/ENTRY:[symbol]) set entry point address for EXE or DLL incompatible with /NOENTRY
	return ToolGetStrProperty(VCLINKID_EntryPointSymbol, &(CVCLinkerTool::s_optHandler), pbstrEntry);
}

STDMETHODIMP CVCLinkerAdvancedPage::put_EntryPointSymbol(BSTR bstrEntry)
{
	return SetStrProperty(VCLINKID_EntryPointSymbol, bstrEntry);
}

STDMETHODIMP CVCLinkerAdvancedPage::get_ResourceOnlyDLL(enumResourceOnlyDLLBOOL* pbNoEntry)
{	// (/NOENTRY) no entry point.  required for resource-only DLLs incompatible with /ENTRY
	return GetEnumBoolProperty2(VCLINKID_ResourceOnlyDLL, (long *)pbNoEntry);
}

STDMETHODIMP CVCLinkerAdvancedPage::put_ResourceOnlyDLL(enumResourceOnlyDLLBOOL bNoEntry)
{
	return SetBoolProperty(VCLINKID_ResourceOnlyDLL, bNoEntry);
}

STDMETHODIMP CVCLinkerAdvancedPage::get_SetChecksum(enumSetChecksumBOOL* pbRelease)
{	// (/RELEASE) set the checksum in the header of a .exe
	return GetEnumBoolProperty2(VCLINKID_SetChecksum, (long *)pbRelease);
}

STDMETHODIMP CVCLinkerAdvancedPage::put_SetChecksum(enumSetChecksumBOOL bRelease)
{
	return SetBoolProperty(VCLINKID_SetChecksum, bRelease);
}

STDMETHODIMP CVCLinkerAdvancedPage::get_BaseAddress(BSTR* pbstrAddress)
{	// (/BASE:{address| filename,key}) base address to place program at can be numeric or string
	return ToolGetStrProperty(VCLINKID_BaseAddress, &(CVCLinkerTool::s_optHandler), pbstrAddress);
}

STDMETHODIMP CVCLinkerAdvancedPage::put_BaseAddress(BSTR bstrAddress)
{
	return SetStrProperty(VCLINKID_BaseAddress, bstrAddress);
}

STDMETHODIMP CVCLinkerAdvancedPage::get_TurnOffAssemblyGeneration(enumTurnOffAssemblyGenerationBOOL* pbNoAssy)
{	// (/NOASSEMBLY) cause the output file to be built without an assembly
	return GetEnumBoolProperty2(VCLINKID_TurnOffAssemblyGeneration, (long *)pbNoAssy);
}

STDMETHODIMP CVCLinkerAdvancedPage::put_TurnOffAssemblyGeneration(enumTurnOffAssemblyGenerationBOOL bNoAssy)
{
	return SetBoolProperty(VCLINKID_TurnOffAssemblyGeneration, bNoAssy);
}

STDMETHODIMP CVCLinkerAdvancedPage::get_SupportUnloadOfDelayLoadedDLL(enumSupportUnloadOfDelayLoadedDLLBOOL* pbSupportUnloadOfDelayLoadedDLL)
{	// (/DELAY:UNLOAD) use to allow explicit unloading of the DLL
	return GetEnumBoolProperty2(VCLINKID_SupportUnloadOfDelayLoadedDLL, (long *)pbSupportUnloadOfDelayLoadedDLL);
}

STDMETHODIMP CVCLinkerAdvancedPage::put_SupportUnloadOfDelayLoadedDLL(enumSupportUnloadOfDelayLoadedDLLBOOL bDelayUnload)
{
	return SetBoolProperty(VCLINKID_SupportUnloadOfDelayLoadedDLL, bDelayUnload);
}

STDMETHODIMP CVCLinkerAdvancedPage::get_ImportLibrary(BSTR* pbstrImportLib)
{	// (/IMPLIB:[library]) generate specified import library
	return ToolGetStrProperty(VCLINKID_ImportLibrary, &(CVCLinkerTool::s_optHandler), pbstrImportLib);
}

STDMETHODIMP CVCLinkerAdvancedPage::put_ImportLibrary(BSTR bstrImportLib)
{
	return SetStrProperty(VCLINKID_ImportLibrary, bstrImportLib);
}

STDMETHODIMP CVCLinkerAdvancedPage::get_MergeSections(BSTR* pbstrMerge)
{	// (/MERGE:from=to) merge section 'from' into section 'to'
	return ToolGetStrProperty(VCLINKID_MergeSections, &(CVCLinkerTool::s_optHandler), pbstrMerge);
}

STDMETHODIMP CVCLinkerAdvancedPage::put_MergeSections(BSTR bstrMerge)
{
	return SetStrProperty(VCLINKID_MergeSections, bstrMerge);
}

STDMETHODIMP CVCLinkerAdvancedPage::get_TargetMachine(machineTypeOption* poptSetting)
{
	return ToolGetIntProperty(VCLINKID_TargetMachine, &(CVCLinkerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCLinkerAdvancedPage::put_TargetMachine(machineTypeOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, TargetMachineMax, optSetting);
	return SetIntProperty(VCLINKID_TargetMachine, optSetting);
}
