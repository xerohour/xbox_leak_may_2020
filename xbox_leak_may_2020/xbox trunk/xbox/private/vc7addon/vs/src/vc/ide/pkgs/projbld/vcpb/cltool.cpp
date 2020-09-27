// CLTool.cpp : Implementation of CVCCLCompilerBaseTool
#include "stdafx.h"
#include "CLTool.h"
#include "FileRegistry.h"
#include "BldHelpers.h"
#include "BldActions.h"
#include "BuildEngine.h"
#include "scanner.h"
#include "MRDependencies.h"
#include "vsappid.h"

// constants
const wchar_t* const wszCLDefaultExtensions = L"*.cpp;*.cxx;*.cc;*.c";	// remember to update vcpb.rgs if you change this...
const wchar_t* const wszCLDefaultPCHUsingName = L"StdAfx.h";
const wchar_t* const wszCLDefaultPCHFileName = L"$(IntDir)/$(TargetName).pch";

const optimizeOption lOptimization = optimizeCustom;
const inlineExpansionOption lInlineFunctionExpansion = expandDisable;
const favorSizeOrSpeedOption lFavorSizeOrSpeed = favorNone;
const ProcessorOptimizeOption lOptimizeForProcessor = procOptimizeBlended;
const preprocessOption lPreprocess = preprocessNo;
const pchOption lUsePrecompiledHeader = pchNone;
const asmListingOption lAssemblerOutput = asmListingNone;
const debugOption lDebugInformation = debugDisabled;
const browseInfoOption lBrowseInformation = brInfoNone;
const CompileAsOptions lCompileAs = compileAsCPlusPlus;
const warningLevelOption lWarnLevel = warningLevel_1;
const basicRuntimeCheckOption lBasicRuntimeChecks = runtimeBasicCheckNone;
const callingConventionOption lCallingConvention = callConventionCDecl;
const structMemberAlignOption lStructMemberAlignment = alignNotSet;
const runtimeLibraryOption lRuntimeLibrary = rtSingleThreaded;
const compileAsManagedOptions lManagedExtensions = managedNotSet;

// static initializers
CCLCompilerToolOptionHandler CVCCLCompilerTool::s_optHandler;
CComBSTR CVCCLCompilerTool::s_bstrBuildDescription = L"";
CComBSTR CVCCLCompilerTool::s_bstrToolName = L"";
CComBSTR CVCCLCompilerTool::s_bstrExtensions = L"";


////////////////////////////////////////////////////////////////////////////////
// Option Table for base C/C++ compiler switches

// NOTE: we're using macros for enum value ranges to help in keeping enum range checks as low maintenance as possible.
// If you add an enum to this table, make sure you use macros to define the upper and lower bounds and use CHECK_BOUNDS
// on the get/put methods associated with that property (both object model and property page).
// WARNING: if you change ANYTHING about a multi-prop here (including add/remove), be sure to update how the property
// is obtained on both the tool and page objects.
BEGIN_OPTION_TABLE(CCLCompilerToolOptionHandler, L"VCCLCompilerTool", IDS_Cl, TRUE /* pre & post */, TRUE /* case sensitive */)
	// Optimization
	OPT_ENUM(Optimization,			L"Od|O1|O2|Ox|", StdEnumMin, OptimizeMax, L"Optimization",	IDS_ClOptimization,	VCCLID)
	OPT_BOOL(GlobalOptimizations,	L"|Og",	/* Og- */			L"GlobalOptimizations",			IDS_ClOptimization,	VCCLID)
	OPT_ENUM(InlineFunctionExpansion, L"|Ob1|Ob2",	/* Ob0 */ StdEnumMin, InlineMax, L"InlineFunctionExpansion", IDS_ClOptimization, VCCLID)
	OPT_BOOL(EnableIntrinsicFunctions, L"|Oi",	/* Oi- */		L"EnableIntrinsicFunctions",	IDS_ClOptimization, VCCLID)
	OPT_BOOL(ImproveFloatingPointConsistency, L"|Op", /* Op- */	L"ImproveFloatingPointConsistency",	IDS_ClOptimization, VCCLID)
	OPT_ENUM(FavorSizeOrSpeed,	L"|Ot|Os",	StdEnumMin,	CodeGenMax,	L"FavorSizeOrSpeed",	IDS_ClOptimization, VCCLID)
	OPT_BOOL(OmitFramePointers,		L"|Oy",	/* Oy- */			L"OmitFramePointers",		IDS_ClOptimization, VCCLID)
	OPT_BOOL(EnableFiberSafeOptimizations, L"|GT",				L"EnableFiberSafeOptimizations", IDS_ClOptimization, VCCLID)
	OPT_BOOL(WholeProgramOptimization, L"|GL",					L"WholeProgramOptimization",	SPECIAL_HYPERLINK, VCCLID)
	OPT_ENUM(OptimizeForProcessor,	L"|G5|G6", /* GB */ StdEnumMin,	OptProcMax,	L"OptimizeForProcessor", IDS_ClOptimization, VCCLID)
	OPT_BOOL(OptimizeForWindowsApplication,	L"|GA",				L"OptimizeForWindowsApplication", IDS_ClOptimization, VCCLID)
	// Preprocessor
	OPT_BSTR(AdditionalIncludeDirectories,	L"I %s",			L"AdditionalIncludeDirectories", multipleNoCase, IDS_ClGeneral, VCCLID)
	OPT_BSTR(AdditionalUsingDirectories,	L"AI %s",			L"AdditionalUsingDirectories", multipleNoCase, IDS_ClGeneral, VCCLID)
	OPT_BSTR(PreprocessorDefinitions, L"D %s",					L"PreprocessorDefinitions",		multiple, IDS_ClPreprocessor, VCCLID)
	OPT_BOOL(IgnoreStandardIncludePath,L"|X",					L"IgnoreStandardIncludePath",			IDS_ClPreprocessor, VCCLID)	
	OPT_ENUM(GeneratePreprocessedFile, L"|P|EP /P",		StdEnumMin, PreprocessMax,	L"GeneratePreprocessedFile",		IDS_ClPreprocessor, VCCLID)
	OPT_BOOL(KeepComments,			L"|C",						L"KeepComments",				IDS_ClPreprocessor, VCCLID)
	// Code Generation; /MP, /MPlowpri deliberately left out (since we require /FD or /Gm to be thrown)
	OPT_BOOL(StringPooling,			L"|GF",						L"StringPooling",			IDS_ClCodeGeneration, VCCLID)
	OPT_BOOL(MinimalRebuild,		L"FD|Gm",					L"MinimalRebuild",			IDS_ClCodeGeneration, VCCLID)
	OPT_BOOL(ExceptionHandling,		L"|EHsc",					L"ExceptionHandling",		IDS_ClCodeGeneration, VCCLID)
	OPT_ENUM(BasicRuntimeChecks,	L"|RTCs|RTCu|RTC1",	StdEnumMin, BasicRTCheckMax, L"BasicRuntimeChecks", IDS_ClCodeGeneration, VCCLID)
	OPT_BOOL(SmallerTypeCheck,		L"|RTCc",					L"SmallerTypeCheck",		IDS_ClCodeGeneration, VCCLID)
	OPT_ENUM(RuntimeLibrary,		L"MT|MTd|MD|MDd|ML|MLd",	StdEnumMin,	RTLibMax,	L"RuntimeLibrary", IDS_ClCodeGeneration, VCCLID)
	OPT_ENUM(StructMemberAlignment,	L"|Zp1|Zp2|Zp4|Zp8|Zp16",	StdEnumMin,	CLStructAlignMax,	L"StructMemberAlignment", IDS_ClCodeGeneration, VCCLID)
	OPT_BOOL(BufferSecurityCheck,	L"|GS",						L"BufferSecurityCheck",		IDS_ClCodeGeneration, VCCLID)
	OPT_BOOL(EnableFunctionLevelLinking, L"|Gy",				L"EnableFunctionLevelLinking", IDS_ClCodeGeneration, VCCLID)	
	// Language
	OPT_BOOL(DisableLanguageExtensions,	L"|Za",	/* Ze */			L"DisableLanguageExtensions", IDS_ClLanguage, VCCLID)
	OPT_BOOL(DefaultCharIsUnsigned,	L"|J",						L"DefaultCharIsUnsigned",	IDS_ClLanguage, VCCLID)
	OPT_BOOL(TreatWChar_tAsBuiltInType,	L"|Zc:wchar_t",			L"TreatWChar_tAsBuiltInType", IDS_ClLanguage, VCCLID)
	OPT_BOOL(ForceConformanceInForLoopScope, L"|Zc:forScope",	L"ForceConformanceInForLoopScope", IDS_ClLanguage, VCCLID)
	OPT_BOOL(RuntimeTypeInfo,		L"|GR",	/* GR- */			L"RuntimeTypeInfo",			IDS_ClLanguage, VCCLID)
	// Precompiled Headers
	OPT_ENUM_SET_COND(UsePrecompiledHeader,	L"|Yc|YX|Yu",	StdEnumMin,	UsePCHMax, 1,	L"UsePrecompiledHeader", PrecompiledHeaderThrough, IDS_ClPrecompiledHeaders, VCCLID)
	OPT_BSTR_COND(PrecompiledHeaderThrough,	L"%s",				L"PrecompiledHeaderThrough",	UsePrecompiledHeader, single, IDS_ClPrecompiledHeaders, VCCLID)
	OPT_BSTR(PrecompiledHeaderFile,	L"Fp%s",					L"PrecompiledHeaderFile", single, IDS_ClPrecompiledHeaders, VCCLID)
	// Output Files
	OPT_BOOL(ExpandAttributedSource, L"|Fx",					L"ExpandAttributedSource",			IDS_ClOutputFiles, VCCLID)
	OPT_ENUM_SET_COND(AssemblerOutput,	L"|FA|FAcs|FAc|FAs", StdEnumMin, AsmListMax, 1, L"AssemblerOutput", AssemblerListingLocation, IDS_ClOutputFiles, VCCLID)
	OPT_BSTR_COND(AssemblerListingLocation, L" /Fa%s",			L"AssemblerListingLocation", AssemblerOutput, single, IDS_ClOutputFiles, VCCLID)
	OPT_BSTR(ObjectFile,			L"Fo%s",					L"ObjectFile",			single, IDS_ClOutputFiles, VCCLID)
	OPT_BSTR(ProgramDataBaseFileName, L"Fd%s",					L"ProgramDataBaseFileName", single, IDS_ClOutputFiles, VCCLID)
	// Browse info helpers
	OPT_ENUM_SET_COND(BrowseInformation, L"|FR|Fr",	StdEnumMin,	BrowseMax, 1,	L"BrowseInformation", BrowseInformationFile, IDS_ClBrowseInfo, VCCLID)
	OPT_BSTR_COND(BrowseInformationFile, L"%s",					L"BrowseInformationFile", BrowseInformation, single, IDS_ClBrowseInfo, VCCLID)
	// General
	OPT_ENUM(WarningLevel,			L"W0|W1|W2|W3|W4",		 StdEnumMin, CLWarnMax, L"WarningLevel",	IDS_ClGeneral, VCCLID)
	OPT_BOOL(WarnAsError,			L"|WX",	/* WX-	*/			L"WarnAsError",						IDS_ClGeneral, VCCLID)
	OPT_BOOL(SuppressStartupBanner, L"|nologo", /* nologo- */	L"SuppressStartupBanner",		IDS_ClGeneral, VCCLID)
	OPT_BOOL(CompileOnly,			L"c|c",						L"CompileOnly",						NO_HYPERLINK, VCCLID)
	OPT_BOOL(Detect64BitPortabilityProblems, L"|Wp64",			L"Detect64BitPortabilityProblems",	IDS_ClGeneral, VCCLID)
	OPT_ENUM(DebugInformationFormat,L"|Z7|Zd|Zi|ZI",	StdEnumMin,	DebugInfoMax,	L"DebugInformationFormat", IDS_ClGeneral, VCCLID)
	OPT_ENUM(CompileAsManaged,		L"|clr:noAssembly|clr",		StdEnumMin, ComRTMax,	L"CompileAsManaged",	IDS_ClGeneral, VCCLID)	
	// Advanced
	OPT_ENUM(CallingConvention,		L"Gd|Gr|Gz",				StdEnumMin,	CallConvMax,	L"CallingConvention",	IDS_ClAdvanced, VCCLID)
	OPT_ENUM_SPECIAL(CompileAs,		L"|TC|TP|Tc|Tp",			StdEnumMin,	CompileAsMax,	L"CompileAs",			IDS_ClAdvanced, VCCLID)
	OPT_BSTR_NOQUOTE(DisableSpecificWarnings,	L"wd%s",		L"DisableSpecificWarnings", multiple,				IDS_ClAdvanced, VCCLID)
	OPT_BSTR(ForcedIncludeFiles,	L"FI %s",					L"ForcedIncludeFiles",		multipleNoCase,			IDS_ClAdvanced, VCCLID)
	OPT_BSTR(ForcedUsingFiles,		L"FU %s",					L"ForcedUsingFiles",		multipleNoCase,			IDS_ClAdvanced, VCCLID)
	OPT_BOOL(ShowIncludes,			L"|showIncludes",			L"ShowIncludes",									IDS_ClAdvanced, VCCLID)
	OPT_BSTR(UndefinePreprocessorDefinitions, L"U %s",			L"UndefinePreprocessorDefinitions",	multiple,		IDS_ClAdvanced, VCCLID)
	OPT_BOOL(UndefineAllPreprocessorDefinitions, L"|u",			L"UndefineAllPreprocessorDefinitions",				IDS_ClAdvanced, VCCLID)
END_OPTION_TABLE()

void CCLCompilerToolOptionHandler::EvaluateSpecialEnumAtIndex(int nVal, LPCOLESTR szOption, long idOption, 
	IVCPropertyContainer* pPropContainer, BOOL bIncludeSlash, CStringW& rstrSwitch) 
{ 
	if (idOption == VCCLID_CompileAs)
	{
		// we will need to do some fancy footwork to determine whether we need the full line or
		// individual file versions of this switch (first position is the same, add two to get
		// to the appropriate file level switch)
	}
	EvaluateEnumAtIndex(nVal, szOption, bIncludeSlash, rstrSwitch); 
}

BOOL CCLCompilerToolOptionHandler::OverrideOptionSet(IVCPropertyContainer* pPropContainer, long idOption)
{
	RETURN_ON_NULL2(g_pProjectEngine, FALSE);

	switch (idOption)
	{
	case VCCLID_Optimization:
	case VCCLID_GlobalOptimizations:
	case VCCLID_InlineFunctionExpansion:
	case VCCLID_EnableIntrinsicFunctions:
	case VCCLID_ImproveFloatingPointConsistency:
	case VCCLID_FavorSizeOrSpeed:
	case VCCLID_OmitFramePointers:
	case VCCLID_WholeProgramOptimization:
		{
			long SKU;
			static_cast<CVCProjectEngine*>(g_pProjectEngine)->GetSKU( &SKU );
			switch( SKU )
			{
				case VSASKUEdition_Book:
				case VSASKUEdition_Standard:
					break;
				default:
					return FALSE;
					break;
			}
			break;
		}
	default:
		return FALSE;
	}

	if (m_bNotifiedNoOptimize)	// if we already told 'em, don't bother trying to tell 'em again
		return TRUE;
	m_bNotifiedNoOptimize = TRUE;

	VARIANT_BOOL bHaveLogging = VARIANT_FALSE;
	g_pProjectEngine->get_BuildLogging(&bHaveLogging);
	if (bHaveLogging == VARIANT_FALSE)
		return TRUE;

	// this whole rigamarole is to try to put a message in the build log about this
	CComQIPtr<IVCBuildableItem> spBldableItem = pPropContainer;
	RETURN_ON_NULL2(spBldableItem, TRUE);

	CComPtr<IVCBuildEngine> spBuildEngine;
	spBldableItem->get_ExistingBuildEngine(&spBuildEngine);
	RETURN_ON_NULL2(spBuildEngine, TRUE);

	CComBSTR bstrInfo;
	bstrInfo.LoadString(IDS_NO_CL_OPTIMIZE);
	spBuildEngine->LogTrace(eLogCommand, bstrInfo);

	return TRUE;
}

void CCLCompilerToolOptionHandler::InitializeFlags()
{
	m_bNotifiedNoOptimize = FALSE;
}

BOOL CCLCompilerToolOptionHandler::SetEvenIfDefault(VARIANT *pvarDefault, long idOption)
{
	CComVariant varDefault;
	BOOL bRet = FALSE;

	switch (idOption)
	{
	case VCCLID_CompileOnly:
	case VCCLID_SuppressStartupBanner:
		varDefault.vt = VT_BOOL;
		varDefault = VARIANT_TRUE;
		bRet = TRUE;
		break;
	case VCCLID_MinimalRebuild:
		varDefault.vt = VT_BOOL;
		varDefault = VARIANT_FALSE;
		bRet = TRUE;
		break;
	}
	varDefault.Detach(pvarDefault);
	return bRet;
}

BOOL CCLCompilerToolOptionHandler::SynthesizeOptionIfNeeded(IVCPropertyContainer* pPropContainer, long idOption)
{
	VSASSERT(pPropContainer != NULL, "Cannot synthesize a tool option without a property container");
	RETURN_ON_NULL2(pPropContainer, FALSE);	// can't synthesize if we don't have a property container...

	switch (idOption)
	{
	case VCCLID_ObjectFile:
	case VCCLID_ProgramDataBaseFileName:	// also tells where to put .idb file
	case VCCLID_CompileAs:
		return TRUE;
	case VCCLID_PrecompiledHeaderFile:
	case VCCLID_PrecompiledHeaderThrough:
		{
			long nPCHType = pchNone;
			HRESULT hr = pPropContainer->GetIntProperty(VCCLID_UsePrecompiledHeader, &nPCHType);
			if (hr != S_OK)
				return FALSE;
			return ((pchOption)nPCHType != pchNone);
		}
	case VCCLID_BufferSecurityCheck:
		{
			long dbgInfo;
			HRESULT hr = pPropContainer->GetIntProperty(VCCLID_DebugInformationFormat, &dbgInfo);
			if (hr != S_OK)
				return FALSE;
			return  (dbgInfo != (long)debugEditAndContinue);
		}
	case VCCLID_BrowseInformationFile:
		{
			browseInfoOption browseInfo;
			HRESULT hr = pPropContainer->GetIntProperty(VCCLID_BrowseInformation, (long *)&browseInfo);
			return (hr == S_OK && browseInfo != brInfoNone);
		}
	case VCCLID_AssemblerListingLocation:
		{
			asmListingOption asmOption;
			HRESULT hr = pPropContainer->GetIntProperty(VCCLID_AssemblerOutput, (long *)&asmOption);
			return (hr == S_OK && asmOption != asmListingNone);
		}
	default:
		return FALSE;
	}
}

BOOL CCLCompilerToolOptionHandler::SynthesizeOption(IVCPropertyContainer* pPropContainer, long idOption, 
	CComVariant& rvar)
{
	switch (idOption)
	{
	case VCCLID_ObjectFile:
	case VCCLID_ProgramDataBaseFileName:
		{
			rvar.Clear();
			VSASSERT(pPropContainer != NULL, "Cannot create an option value without a property container");
			CComBSTR bstrVal;
			GetDefaultValue( idOption, &bstrVal, pPropContainer );
			return SetBstrInVariant(pPropContainer, bstrVal, rvar);
		}
	case VCCLID_PrecompiledHeaderFile:
		{
			CComBSTR bstrPCH;
			if (GetPCHName(pPropContainer, &bstrPCH, FALSE))
				return SetBstrInVariant(pPropContainer, bstrPCH, rvar);
			return FALSE;
		}
	case VCCLID_PrecompiledHeaderThrough:
		{
			CComBSTR bstrPCH;
			if (GetPCHHeaderName(pPropContainer, VCCLID_PrecompiledHeaderThrough, bstrPCH))
				return SetBstrInVariant(pPropContainer, bstrPCH, rvar);
			return FALSE;
		}
	case VCCLID_BrowseInformationFile:
		{
			browseInfoOption browseInfo;
			CComBSTR bstrBrowseInfoFile;
			if (pPropContainer->GetIntProperty(VCCLID_BrowseInformation, (long *)&browseInfo) == S_OK && browseInfo != brInfoNone )
				{
					GetDefaultValue( VCCLID_BrowseInformationFile, &bstrBrowseInfoFile, pPropContainer );
					return SetBstrInVariant(pPropContainer, bstrBrowseInfoFile, rvar);
				}
			return FALSE;
		}
	case VCCLID_BufferSecurityCheck:
		{
			long dbgInfo;
			HRESULT hr = pPropContainer->GetIntProperty(VCCLID_DebugInformationFormat, &dbgInfo);
			if (hr == S_OK && dbgInfo == (long)debugEditAndContinue)
				return FALSE;
			rvar.vt = VT_BOOL;
			rvar.boolVal = VARIANT_TRUE;
			return TRUE;
		}
	case VCCLID_AssemblerListingLocation:
		{
			asmListingOption asmOption;
			CComBSTR bstrAsmLocation;
			if (pPropContainer->GetIntProperty(VCCLID_AssemblerOutput, (long *)&asmOption) == S_OK 
				&& asmOption != asmListingNone )
				{
					GetDefaultValue( VCCLID_AssemblerListingLocation, &bstrAsmLocation, pPropContainer );
					return SetBstrInVariant(pPropContainer, bstrAsmLocation, rvar);
				}
			return FALSE;
		}
	case VCCLID_CompileAs:
		{
			CompileAsOptions compileAs = GetDefaultCompileAs(pPropContainer, FALSE);
			rvar.lVal = (long)compileAs;
			rvar.vt = VT_I4;
			return TRUE;
		}
	default:
		VSASSERT(FALSE, "Case statement mismatch between SynthesizeOptionIfNeeded and SynthesizeOption");	// shouldn't be here!!
		return FALSE;
	}
}


CompileAsOptions CCLCompilerToolOptionHandler::GetDefaultCompileAs(IVCPropertyContainer* pPropContainer, BOOL bCheckExplicit /* = TRUE */)
{
	RETURN_ON_NULL2(pPropContainer, compileAsDefault);

	CComVariant var;
	if (bCheckExplicit && pPropContainer->GetProp(VCCLID_CompileAs, &var) == S_OK && var.vt != VT_EMPTY)
		return (CompileAsOptions)var.lVal;

	CComPtr<IVCPropertyContainer> spActualContainer;
	CVCProjectEngine::ResolvePropertyContainer(pPropContainer, &spActualContainer);
	RETURN_ON_NULL2(spActualContainer, compileAsDefault);

	CComQIPtr<VCConfiguration> spProjCfg = spActualContainer;
	if (spProjCfg)
		return compileAsCPlusPlus;

	CComQIPtr<VCFileConfiguration> spFileCfg = spActualContainer;
	RETURN_ON_NULL2(spFileCfg, compileAsCPlusPlus);

	CComPtr<IDispatch> spDispFile;
	spFileCfg->get_File(&spDispFile);
	CComQIPtr<VCFile> spFile = spDispFile;
	RETURN_ON_NULL2(spFile, compileAsCPlusPlus);

	CComBSTR bstrName;
	spFile->get_Name(&bstrName);
	CStringW strName = bstrName;
	if (strName.GetLength() > 2)
	{
		CStringW strEnd = strName.Right(2);
		strEnd.MakeLower();
		if (strEnd == L".c")
			return compileAsC;
	}

	return compileAsCPlusPlus;
}

static const wchar_t* szScriptItemCL = L"\r\n<a title=\"%s | %s\" onclick=\"onHelp('VC.Project.VCConfiguration.WholeProgramOptimization')\" href=\"settingspage.htm\">%s</a>";

void CCLCompilerToolOptionHandler::FormatScriptItem(const wchar_t* szOptionName, long idOption, long idOptionPage, CStringW& rstrOption)
{
	if (idOption != SPECIAL_HYPERLINK)
	{
		COptionHandlerBase::FormatScriptItem(szOptionName, idOption, idOptionPage, rstrOption);
		return;
	}

	VSASSERT(idOption == VCCLID_WholeProgramOptimization, "Hey, only doing special handling for VCCLID_WholeProgramOptimization for CL!");

	CStringW strProp;
	strProp.LoadString(VCCFGID_WholeProgramOptimization);
	long nColon = strProp.Find(L": ");
	if (nColon)
		strProp = strProp.Left(nColon-1);
	strProp.TrimLeft();
	strProp.TrimRight();

	CStringW strGen;
	strGen.LoadString(IDS_GeneralConfigSettings);
	CStringW strTmp;
	strTmp.Format(szScriptItemCL, strGen, strProp, rstrOption);
	rstrOption = strTmp;
}

BOOL CCLCompilerToolOptionHandler::GetPCHName(IVCPropertyContainer* pPropContainer, BSTR* pbstrPCH,
	BOOL bCheckForExistence /* = TRUE */)
{
	VSASSERT(pPropContainer != NULL, "Cannot create PCHName without property container");
	if (bCheckForExistence)
	{
		CComBSTR bstrPCH;
		if (pPropContainer->GetEvaluatedStrProperty(VCCLID_PrecompiledHeaderFile, pbstrPCH) == S_OK)
			return TRUE;
	}

	GetDefaultValue( VCCLID_PrecompiledHeaderFile, pbstrPCH, pPropContainer );
	HRESULT hr = pPropContainer->Evaluate(*pbstrPCH, pbstrPCH);

	return (SUCCEEDED(hr));
}

BOOL CCLCompilerToolOptionHandler::GetPCHHeaderName(IVCPropertyContainer* pPropContainer, long nID,
	CComBSTR& rbstrPCH, BOOL bCheckForExistence /* = TRUE */)
{
	VSASSERT(pPropContainer != NULL, "Cannot get PCHHeaderName without property container");

	if (bCheckForExistence)
	{
		if (pPropContainer->GetEvaluatedStrProperty(nID, &rbstrPCH) == S_OK)
			return TRUE;
	}

	rbstrPCH = L"stdafx.h";
	return TRUE;
}

// default value handlers
// string props
void CCLCompilerToolOptionHandler::GetDefaultValue( long id, BSTR *pVal, IVCPropertyContainer *pPropContainer )
{
	switch( id )
	{
	case VCCLID_PrecompiledHeaderThrough:
	{
		pchOption usePCH = pchNone;
		if (pPropContainer != NULL && pPropContainer->GetIntProperty(VCCLID_UsePrecompiledHeader, (long *)&usePCH) == S_OK && usePCH != pchNone)
			*pVal = SysAllocString( wszCLDefaultPCHUsingName );
		else
			GetDefaultString( pVal );
		break;
	}
	case VCCLID_PrecompiledHeaderFile:
	{
		pchOption usePCH = pchNone;
		if (pPropContainer != NULL && pPropContainer->GetIntProperty(VCCLID_UsePrecompiledHeader, (long *)&usePCH) == S_OK && usePCH != pchNone)	
			*pVal = SysAllocString( wszCLDefaultPCHFileName );
		else
			GetDefaultString( pVal );
		break;
	}
	case VCCLID_AssemblerListingLocation:
	{
		if (pPropContainer)
		{
			asmListingOption asmOption;
			HRESULT hr = pPropContainer->GetIntProperty(VCCLID_AssemblerOutput, (long *)&asmOption);
			if (hr == S_OK && asmOption != asmListingNone)
				*pVal = SysAllocString( L"$(IntDir)/" );
		}
		else 
			GetDefaultString( pVal );
		break;
	}
	case VCCLID_ObjectFile:
	{
		if (pPropContainer == NULL)
			GetDefaultString( pVal );
		else
			*pVal = SysAllocString( L"$(IntDir)/" );
		break;
	}
	case VCCLID_ProgramDataBaseFileName:
	{
		if (pPropContainer == NULL)
			GetDefaultString( pVal );

		else
		{
			CComBSTR bstrDefault = L"$(IntDir)/";
			bstrDefault += CBldAction::s_bstrDefVCFile;
			bstrDefault += L".pdb";
			*pVal = bstrDefault.Detach();
		}
		break;;
	}
	case VCCLID_BrowseInformationFile:
	{
		browseInfoOption brInfo;
		if (pPropContainer == NULL || pPropContainer->GetIntProperty(VCCLID_BrowseInformation, (long *)&brInfo) != S_OK || brInfo == brInfoNone)
			GetDefaultString( pVal );
		else
			*pVal = SysAllocString( L"$(IntDir)/" );
		break;
	}
// 	case VCCLID_AdditionalOptions:
// 	case VCCLID_AdditionalIncludeDirectories:
// 	case VCCLID_AdditionalUsingDirectories:
// 	case VCCLID_PreprocessorDefinitions:
// 	case VCCLID_UndefinePreprocessorDefinitions:
// 	case VCCLID_ForcedIncludeFiles:
// 	case VCCLID_ForcedUsingFiles:
// 	case VCCLID_DisableSpecificWarnings:
	default:
 		GetDefaultString( pVal );
		break;
	}
}

// integer props
void CCLCompilerToolOptionHandler::GetDefaultValue( long id, long *pVal, IVCPropertyContainer *pPropCnt )
{
	switch( id )
	{
	case VCCLID_Optimization:
		*pVal = lOptimization;
		break;
	case VCCLID_InlineFunctionExpansion:
		*pVal = lInlineFunctionExpansion;
		break;
	case VCCLID_FavorSizeOrSpeed:
		*pVal = lFavorSizeOrSpeed;
		break;
	case VCCLID_GeneratePreprocessedFile:
		*pVal = lPreprocess;
		break;
	case VCCLID_UsePrecompiledHeader:
		*pVal = lUsePrecompiledHeader;
		break;
	case VCCLID_AssemblerOutput:
		*pVal = lAssemblerOutput;
		break;
	case VCCLID_DebugInformationFormat:
		*pVal = lDebugInformation;
		break;
	case VCCLID_BrowseInformation:
		*pVal = lBrowseInformation;
		break;
	case VCCLID_CompileAs:
		*pVal = (long)GetDefaultCompileAs(pPropCnt);
		break;
	case VCCLID_WarningLevel:
		*pVal = lWarnLevel;
		break;
	case VCCLID_BasicRuntimeChecks:
		*pVal = lBasicRuntimeChecks;
		break;
	case VCCLID_CompileAsManaged:
		*pVal = lManagedExtensions;
		break;
	case VCCLID_OptimizeForProcessor:
		*pVal = lOptimizeForProcessor;
		break;
	case VCCLID_CallingConvention:
		*pVal = lCallingConvention;
		break;
	case VCCLID_StructMemberAlignment:
		*pVal = lStructMemberAlignment;
		break;
	case VCCLID_RuntimeLibrary:
		*pVal = lRuntimeLibrary;
		break;

	default:
		*pVal = 0;
		break;
	}
}

// boolean props
void CCLCompilerToolOptionHandler::GetDefaultValue( long id, VARIANT_BOOL *pVal, IVCPropertyContainer *pPropCnt )
{
	switch( id )
	{
	case VCCLID_MinimalRebuild:
	case VCCLID_SuppressStartupBanner:
		GetValueTrue( pVal );
		break;
 	case VCCLID_BufferSecurityCheck:
		{
			if (pPropCnt == NULL)
			{
				GetValueTrue(pVal);
			}
			else
			{
				long dbgInfo;
				HRESULT hr = pPropCnt->GetIntProperty(VCCLID_DebugInformationFormat, &dbgInfo);
				if (hr == S_OK && dbgInfo == (long)debugEditAndContinue)
					GetValueFalse(pVal);
				else
					GetValueTrue(pVal);
			}
			break;
		}
// 	case VCCLID_GlobalOptimizations:
// 	case VCCLID_EnableIntrinsicFunctions:
// 	case VCCLID_ImproveFloatingPointConsistency:
// 	case VCCLID_OmitFramePointers:
// 	case VCCLID_EnableFiberSafeOptimizations:
// 	case VCCLID_WholeProgramOptimization:
// 	case VCCLID_UndefineAllPreprocessorDefinitions:
// 	case VCCLID_IgnoreStandardIncludePath:
// 	case VCCLID_KeepComments:
// 	case VCCLID_ShowIncludes:
// 	case VCCLID_SmallerTypeCheck:
// 	case VCCLID_RuntimeTypeInfo:
// 	case VCCLID_DisableLanguageExtensions:
// 	case VCCLID_DefaultCharIsUnsigned:
// 	case VCCLID_TreatWChar_tAsBuiltInType:
// 	case VCCLID_ForceConformanceInForLoopScope:
// 	case VCCLID_EnableFunctionLevelLinking:
// 	case VCCLID_UsePrecompiledHeader:
// 	case VCCLID_ExpandAttributedSource:
// 	case VCCLID_WarnAsError:
// 	case VCCLID_CompileOnly:
// 	case VCCLID_Detect64BitPortabilityProblems:
//	case VCCLID_ExceptionHandling:
//	case VCCLID_StringPooling:
//	case VCCLID_OptimizeForWindowsApplication:
	default:
		GetValueFalse( pVal );
		break;
	}
}

// Settings pages
GUID CVCCLCompilerTool::s_pPages[10];
BOOL CVCCLCompilerTool::s_bPagesInit = FALSE;

GUID* CVCCLCompilerTool::GetPageIDs()
{ 
	if (s_bPagesInit == FALSE)
	{
		s_pPages[0] = __uuidof(ClGeneral);
		s_pPages[1] = __uuidof(ClOptimization);
		s_pPages[2] = __uuidof(ClPreprocessor);
		s_pPages[3] = __uuidof(ClCodeGeneration);
		s_pPages[4] = __uuidof(ClLanguage);
		s_pPages[5] = __uuidof(ClPrecompiledHeaders);
		s_pPages[6] = __uuidof(ClOutputFiles);
		s_pPages[7] = __uuidof(ClBrowseInfo);
		s_pPages[8] = __uuidof(ClAdvanced);
		s_pPages[9] = __uuidof(ClAdditionalOptions);
		s_bPagesInit = TRUE;
	}
	return s_pPages; 
}

LPCOLESTR CVCCLCompilerTool::GetToolFriendlyName()
{
	InitToolName();
	return s_bstrToolName;
}

void CVCCLCompilerTool::InitToolName()
{
	if (s_bstrToolName.Length() == 0)
	{
		if (!s_bstrToolName.LoadString(IDS_CL_TOOLNAME))
			s_bstrToolName = szCLCompilerToolType;
	}
}

HRESULT CVCCLCompilerTool::CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppCLTool)
{
	CHECK_POINTER_NULL(ppCLTool);
	*ppCLTool = NULL;

	CComObject<CVCCLCompilerTool> *pObj;
	HRESULT hr = CComObject<CVCCLCompilerTool>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		if( pPropContainer )
			pObj->Initialize(pPropContainer);
		CVCCLCompilerTool *pVar = pObj;
		pVar->AddRef();
		*ppCLTool = pVar;
	}
	return hr;
}

STDMETHODIMP CVCCLCompilerTool::PrePerformBuildActions(bldActionTypes type, IVCBuildActionList* pActions, 
	bldAttributes aob, IVCBuildErrorContext* pEC, actReturnStatus* pActReturn)
{
	CHECK_POINTER_NULL(pActReturn);
	*pActReturn = ACT_Complete;

	RETURN_ON_NULL2(pActions, S_OK);	// no actions, nothing to do

	// what stage is this?
	// need to rebuild?

	HRESULT hr = S_OK;
	if (type == TOB_Build)
	{
		// delete the .obj file if we think we should have a .pdb and we don't
		hr = pActions->Reset(NULL);
		if (FAILED(hr))
		{
			VSASSERT(FALSE, "Failed doing Reset on action list!?!");
			return E_UNEXPECTED;
		}
		hr = S_OK;
		while (hr == S_OK && (*pActReturn == ACT_Complete) /* failure */)
		{
			CComPtr<IVCBuildAction> spAction;
			hr = pActions->Next(&spAction, NULL);
			BREAK_ON_DONE(hr);
			VALID_CONTINUE_ON_NULL(spAction);
			CBldFileRegSet* pfrs = NULL;
			HRESULT hr2 = spAction->GetOutput(pEC, (void **)&pfrs);
			CONTINUE_ON_FAIL(hr2);
			VALID_CONTINUE_ON_NULL(pfrs);
			CBldFileRegistry* pActionRegistry = NULL;
			hr2 = spAction->get_Registry((void **)&pActionRegistry);
			CONTINUE_ON_FAIL(hr2);
			CONTINUE_ON_NULL(pActionRegistry);

			BldFileRegHandle frh;
			pfrs->InitFrhEnum();
			CComBSTR bstrProjectDirectory;
			CComPtr<IVCBuildableItem> spItem;
			BOOL bFoundProjectDirectory = FALSE;
			while ((frh = pfrs->NextFrh()) != (BldFileRegHandle)NULL)
			{
				const CPathW* pPath = pActionRegistry->GetRegEntry(frh)->GetFilePath();
				if (_wcsicmp(pPath->GetExtension(), L".pdb") == 0)
				{
					if (!pPath->ExistsOnDisk())
					{
						hr2 = spAction->get_Item(&spItem);
						VSASSERT(SUCCEEDED(hr2), "Every action has an item!");
						if (!bFoundProjectDirectory)
						{
							bFoundProjectDirectory = TRUE;
							CComQIPtr<IVCPropertyContainer> spPropContainer = spItem;
							VSASSERT(spPropContainer != NULL, "Trying to get information out of non-property container");
							bstrProjectDirectory.Empty();
							if (spPropContainer != NULL)
								spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjectDirectory);	// should never have macros in it
						}

						// GetItemOutputFile will write over frh.
						frh->ReleaseFRHRef();
						if (GetItemOutputFile(spAction, spItem, VCCFGID_IntermediateDirectory, frh, L".obj", 
							bstrProjectDirectory, TRUE))
						{
							if (!g_StaticBuildEngine.DeleteFile(frh, pEC, NULL, pActions) && !(aob & AOB_IgnoreErrors))
								*pActReturn = ACT_Error;	// failure
						}
					}
					if (NULL != frh)
						frh->ReleaseFRHRef();
					break;	// done!
				}
				if (NULL != frh)
					frh->ReleaseFRHRef();
			}
		}
	}

	return S_OK;
}

STDMETHODIMP CVCCLCompilerTool::GenerateOutput(long type, IVCBuildActionList* plstActions, IVCBuildErrorContext* pEC)
{
	if (plstActions == NULL)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
	plstActions->Reset(NULL);

	BOOL bFirst = TRUE;
	CDirW dirProject;
	CComBSTR bstrIntermediateDirectory;
	while (TRUE)
	{
		CComPtr<IVCBuildAction> spAction;
		HRESULT hr = plstActions->Next(&spAction, NULL);
		BREAK_ON_DONE(hr);

 		CComPtr<IVCPropertyContainer> spPropContainer;
		spAction->get_PropertyContainer(&spPropContainer);
		VSASSERT(spPropContainer != NULL, "Action does not have a property container associated with it.  Bad initialization.");
		if (spPropContainer == NULL)
			return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

		if (IsExcludedFromBuild(spPropContainer))
			continue;	// nothing to do for things excluded from build...

		if (bFirst)
		{
			bFirst = FALSE;
			CComBSTR bstrProjectDirectory;
			hr = spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjectDirectory);	// should never have macros in it
			VSASSERT(hr == S_OK, "How did we manage to not get a project directory?!?");
			RETURN_ON_FAIL(hr);
			CStringW strProjDir = bstrProjectDirectory;
			if (!dirProject.CreateFromKnown(strProjDir))
			{
				VSASSERT(FALSE, "Invalid project directory!");
				return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
			}
			hr = spPropContainer->GetEvaluatedStrProperty(VCCFGID_IntermediateDirectory, &bstrIntermediateDirectory);
			if (FAILED(hr) || bstrIntermediateDirectory.Length() == 0)
				spPropContainer->GetEvaluatedStrProperty(VCCFGID_OutputDirectory, &bstrIntermediateDirectory);
		}

		// object files
		CComBSTR bstrObjFile;
		hr = spPropContainer->GetStrProperty(VCCLID_ObjectFile, &bstrObjFile);
		HRESULT hrT = S_OK;
		if (hr == S_OK)
		{
			spPropContainer->Evaluate(bstrObjFile, &bstrObjFile);
			hrT = spAction->AddOutputFromFileName(bstrObjFile, pEC, L".obj", (wchar_t *)(const wchar_t*)dirProject, 
				VARIANT_TRUE, VCCLID_ObjectFile, this);
		}
		else
			hrT = spAction->AddOutputInSubDirectory(bstrIntermediateDirectory, pEC, L".obj", 
				(wchar_t *)(const wchar_t*)dirProject, NULL, -1, VARIANT_TRUE, VCCFGID_IntermediateDirectory, this);
		if (hrT != S_OK)
			return hrT;

		// source browser files
		CComBSTR bstrSbrFile;
		hr = spPropContainer->GetStrProperty(VCCLID_BrowseInformationFile, &bstrSbrFile);
		if (hr == S_OK)
		{
			long nBrowseInfo;
			hr = spPropContainer->GetIntProperty(VCCLID_BrowseInformation, &nBrowseInfo);
			if (hr == S_OK && (browseInfoOption)nBrowseInfo != brInfoNone)
			{
				spPropContainer->Evaluate(bstrSbrFile, &bstrSbrFile);
				hrT = spAction->AddOutputFromFileName(bstrSbrFile, pEC, L".sbr", (wchar_t *)(const wchar_t*)dirProject, 
					VARIANT_TRUE, VCCLID_BrowseInformationFile, this);
			}
			else
				hrT = S_OK;
		}
		else
			hrT = spAction->AddOutputInSubDirectory(bstrIntermediateDirectory, pEC, L".sbr", 
				(wchar_t *)(const wchar_t *)dirProject, NULL, VCCLID_BrowseInformation, VARIANT_TRUE, VCCFGID_IntermediateDirectory,
				this);
		if (hrT != S_OK)
			return hrT;
				
		VARIANT_BOOL bExpand = VARIANT_FALSE;
		
		if (spPropContainer->GetBoolProperty(VCCLID_ExpandAttributedSource, &bExpand) == S_OK && bExpand == VARIANT_TRUE)
		{
		   // .mrg.cpp and .mrg.h files -
			// only add to outputs if optioned on for this item
			// Not sure where compiler generates the mrg files, so remove
			// from both workspace directory and output directory
			// BruceMa 12-07-98
			HRESULT hrT = spAction->AddOutputFromDirectory((wchar_t *)(const wchar_t*)dirProject, pEC, 
				VCCLID_ExpandAttributedSource, L"mrg.cpp", -1, this);
			if (hrT != S_OK)
			{
				VSASSERT(FALSE, "Failed to add output for *.mrg.cpp");
				return hrT;
			}
			hrT = spAction->AddOutputFromDirectory((wchar_t *)(const wchar_t*)dirProject, pEC, VCCLID_ExpandAttributedSource, 
				L"mrg.h", -1, this);
			if (hrT != S_OK)
			{
				VSASSERT(FALSE, "Failed to add output for *.mrg.h");
				return hrT;
			}
			hrT = spAction->AddOutputInSubDirectory(bstrIntermediateDirectory, pEC, L".mrg.cpp", 
				(wchar_t *)(const wchar_t*)dirProject, NULL, -1, VARIANT_FALSE, VCCFGID_IntermediateDirectory, this);
			if (hrT != S_OK)
			{
				VSASSERT(FALSE, "Failed to add output for *.mrg.cpp");
				return hrT;
			}
			hrT = spAction->AddOutputInSubDirectory(bstrIntermediateDirectory, pEC, L".mrg.h", 
				(wchar_t *)(const wchar_t*)dirProject, NULL, -1, VARIANT_FALSE, VCCFGID_IntermediateDirectory, this);
			if (hrT != S_OK)
			{
				VSASSERT(FALSE, "Failed to add output for *.mrg.h");
				return hrT;
			}
		}

        // .i files - only add to outputs if optioned on for this item
		long lProp = 0;
		if (spPropContainer->GetIntProperty(VCCLID_GeneratePreprocessedFile, &lProp) == S_OK)
		{
			if (preprocessNo != (preprocessOption)lProp)
			{
				HRESULT hrT = spAction->AddOutputFromDirectory((wchar_t *)(const wchar_t*)dirProject, pEC, -1, L".i", -1, 
					this);
				if (hrT != S_OK)
				{
					VSASSERT(FALSE, "Failed to add output for preprocess file");
					return hrT;
				}
			}
		}
				
		// pdb files (both .pdb and .idb)
		CComBSTR bstrPdb;
		GetResolvedPdbName(spPropContainer, &bstrPdb);
		lProp = 0;
		if (spPropContainer->GetIntProperty(VCCLID_DebugInformationFormat, &lProp) == S_OK)
		{
			if ((debugOption)lProp == debugEditAndContinue)
			{
				spAction->AddOutputInSubDirectory(L"", pEC, L".pdb", (wchar_t *)(const wchar_t*)dirProject, bstrPdb, 
					-1 /* no enable id */, VARIANT_FALSE /* 2ndary output */, -1, this);
				// don't care about return value here for 2ndary output
			}
		}
		
		// Add idb for /Gm and/or /FD.
		spAction->AddOutputInSubDirectory(L"", pEC, L".idb", (wchar_t *)(const wchar_t*)dirProject, bstrPdb, 
			-1 /* no enable id */, FALSE /* 2ndary output */, -1, this);
		// don't care about return value here for 2ndary output

		// check to see if this file produces or uses a PCH file....
		// get the location of of the /Fp setting
		CPathW pathPch;

		// clear the .pch file dep. from the source deps...
		CBldFileRegSet * pfrs = NULL;
		spAction->get_SourceDependencies((void **)&pfrs);
		VSASSERT(pfrs != NULL, "No source dependencies for CL action");
		BldFileRegHandle frh;

		CBldFileRegistry* pRegistry = NULL;
		spAction->get_Registry((void **)&pRegistry);
		VSASSERT(pRegistry != NULL, "No registry associated with action");

		if (pfrs != NULL && pRegistry != NULL)
		{
			pfrs->InitFrhEnum();
			while ((frh = pfrs->NextFrh()) != (BldFileRegHandle)NULL)
			{
				const CPathW* pPath = pRegistry->GetRegEntry(frh)->GetFilePath();
				if (_wcsicmp(pPath->GetExtension(), L".pch") == 0)
				{
					// remove the .pch as a source dep.
					if (spAction->RemoveSourceDependencyForFrh((void *)frh, pEC) != S_OK)
						VSASSERT(FALSE, "Failed to remove a source dependency for an action");
					frh->ReleaseFRHRef();
					break;	// done!
				}
				frh->ReleaseFRHRef();
			}
		}

		CStringW strPch;
		CComBSTR bstrPch;
		if (s_optHandler.GetPCHName(spPropContainer, &bstrPch))
		{
			strPch = bstrPch;
			if (strPch.IsEmpty())
				continue;	// no pch name....
		}
		else
			continue;	// no PCH


		// add a possible .pch file dep.
		long lPchUse = 0;
		if (spPropContainer->GetIntProperty(VCCLID_UsePrecompiledHeader, &lPchUse) == S_OK)
		{
			if ((pchOption)lPchUse == pchCreateUsingSpecific || (pchOption)lPchUse == pchGenerateAuto)
			{
				if (pathPch.CreateFromDirAndFilename(dirProject, strPch))
				{
					pathPch.GetActualCase(TRUE);
					spAction->AddOutputFromPath((wchar_t *)(const wchar_t *)pathPch, pEC,
						(pchOption)lPchUse == pchCreateUsingSpecific ? VARIANT_TRUE : VARIANT_FALSE, // only a primary output for /Yc
						VARIANT_TRUE, VCCLID_PrecompiledHeaderFile, this);
				}
			}
			else if ((pchOption)lPchUse == pchUseUsingSpecific)
			{
				// add the .pch as a source dep.
				if (pathPch.CreateFromDirAndFilename(dirProject, strPch))
				{
					pathPch.GetActualCase(TRUE);
					CStringW strPath = pathPch;
					CComBSTR bstrPath = strPath;
					if (spAction->AddSourceDependencyFromString(bstrPath, pEC) != S_OK)
						VSASSERT(FALSE, "Failed to add dependency for stdafx.h file");
				}
			}
		}
	}

	return S_OK; // success
}

HRESULT CVCCLCompilerTool::GetPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, CPathW& rPath)
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

	BOOL bIsDir = TRUE;
	CComBSTR bstrObjFile;
	CStringW strInt;
	hr = spPropContainer->GetStrProperty(VCCLID_ObjectFile, &bstrObjFile);
	if (hr == S_OK)
	{
		spPropContainer->Evaluate(bstrObjFile, &bstrObjFile);
		CStringW strObjFile = bstrObjFile;
		int nObjLen = strObjFile.GetLength();
		if (nObjLen == 0)
			hr = S_FALSE;
		else if (strObjFile[nObjLen-1] == L'/' || strObjFile[nObjLen-1] == L'\\')
			strObjFile = strObjFile.Left(nObjLen-1);
		else
			bIsDir = FALSE;
		strInt = strObjFile;
	}
	if (hr != S_OK)		// NOT 'else' due to ability to change hr above...
	{
		CComBSTR bstrIntermediateDirectory;
		hr = spPropContainer->GetEvaluatedStrProperty(VCCFGID_IntermediateDirectory, &bstrIntermediateDirectory);
		if (FAILED(hr) || bstrIntermediateDirectory.Length() == 0)
			spPropContainer->GetEvaluatedStrProperty(VCCFGID_OutputDirectory, &bstrIntermediateDirectory);

		strInt = bstrIntermediateDirectory;
		int nlen = strInt.GetLength();
		if (nlen > 0 && (strInt[nlen-1] == L'/' || strInt[nlen-1] == L'\\'))
			strInt = strInt.Left(nlen-1);
	}

	if (bIsDir)
	{
		CPathW pathIntDir;
		if (!pathIntDir.CreateFromDirAndFilename(dirProject, strInt))
			return S_FALSE;

		CDirW dirIntDir;
		if (!dirIntDir.CreateFromPath(pathIntDir, TRUE))
			return S_FALSE;

		CComBSTR bstrItemName;
		if (FAILED(spBldableItem->get_ItemFileName(&bstrItemName)))
			return S_FALSE;

		if (!rPath.CreateFromDirAndFilename(dirIntDir, bstrItemName))
			return S_FALSE;
	}
	else if (!rPath.CreateFromDirAndFilename(dirProject, strInt))
		return S_FALSE;

	rPath.ChangeExtension(L".obj");
	return S_OK;
}

STDMETHODIMP CVCCLCompilerTool::HasPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, VARIANT_BOOL* pbHasPrimaryOutput)
{ 
	CHECK_POINTER_NULL(pbHasPrimaryOutput);
	if (bSchmoozeOnly)
		*pbHasPrimaryOutput = VARIANT_FALSE;	// not primary project output generator
	else
		*pbHasPrimaryOutput = VARIANT_TRUE;	// looking for primary *file* output, so yes, we create one

	return S_OK;
}

STDMETHODIMP CVCCLCompilerTool::GetPrimaryOutputIDFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, long* pnOutputID)
{
	CHECK_POINTER_NULL(pnOutputID);
	*pnOutputID = -1;

	if (bSchmoozeOnly)
		return S_FALSE;

	*pnOutputID = VCCLID_ObjectFile;
	return S_OK;
}

STDMETHODIMP CVCCLCompilerTool::SetPrimaryOutputForTool(IUnknown* pItem, BSTR bstrFile)
{	// sets the primary output for a tool
	CComQIPtr<IVCPropertyContainer> spPropContainer = pItem;
	RETURN_ON_NULL2(spPropContainer, S_FALSE);		// nothing we can do

	CStringW strFileCheck = bstrFile;
	strFileCheck.TrimLeft();
	strFileCheck.TrimRight();
	if (strFileCheck.IsEmpty())
		return S_FALSE;

	// assuming OK to dirty since we shouldn't be here unless we got called by a wizard that already checked
	return spPropContainer->SetStrProperty(VCCLID_ObjectFile, bstrFile);
}

STDMETHODIMP CVCCLCompilerTool::GetDefaultOutputFileFormat(IUnknown* pItem, BSTR* pbstrOutMacro)
{	// macro to use for generating default output file if the original default isn't unique
	CHECK_POINTER_NULL(pbstrOutMacro);
	CComBSTR bstrOutFormat = L"$(IntDir)/$(InputName)%d.obj";
	*pbstrOutMacro = bstrOutFormat.Detach();

	return S_OK;
}

void CVCCLCompilerTool::MakePdbAsFile(IVCPropertyContainer* pPropContainer, CComBSTR& bstrPdb)
{
	if (FAILED(pPropContainer->Evaluate(bstrPdb, &bstrPdb)))
		return;

	CStringW strPdb = bstrPdb;
	strPdb.TrimRight();
	int nLen = strPdb.GetLength();

	if (nLen == 0)
	{
		bstrPdb = strPdb;
		return;
	}

	if (strPdb.GetAt(nLen-1) == L'/' || strPdb.GetAt(nLen-1) == L'\\')
	{	// definitely just a directory
		strPdb += CBldAction::s_szDefVCFile;
		bstrPdb = strPdb;
		return;
	}

	if (strPdb.GetAt(nLen-4) == L'.')
		return;	// already have an extension there

	if (wcsncmp(strPdb, L"./", 2) == 0 || wcsncmp(strPdb, L".\\", 2) == 0)	// strip any leading ./
	{
		strPdb = strPdb.Right(nLen-2);
		nLen -= 2;
	}

	// cheat and see if we're in the intermediate or output directory
	if (CheckPathAgainstDir(pPropContainer, VCCFGID_IntermediateDirectory, strPdb, bstrPdb))
		return;

	if (CheckPathAgainstDir(pPropContainer, VCCFGID_OutputDirectory, strPdb, bstrPdb))
		return;

	// if we get here, just gotta assume it's a file...
}

BOOL CVCCLCompilerTool::CheckPathAgainstDir(IVCPropertyContainer* pPropContainer, long id, CStringW& strPdb, 
	CComBSTR& bstrPdb)
{
	CComBSTR bstrDir;
	pPropContainer->GetEvaluatedStrProperty(id, &bstrDir);
	CStringW strDir = bstrDir;
	int nDirLen = strDir.GetLength();
	if (nDirLen == 0)	// nothing there, nothing we can do
		return FALSE;

	if (strDir.GetAt(nDirLen-1) == L'/' || strDir.GetAt(nDirLen-1) == L'\\')
	{
		strDir = strDir.Left(nDirLen-1);	// strip any trailing slash
		nDirLen -= 1;
	}

	if (wcsncmp(strDir, L"./", 2) == 0 || wcsncmp(strDir, L".\\", 2) == 0)
	{
		strDir = strDir.Right(nDirLen-2);
		nDirLen -= 2;
	}

	if (wcsicmp(strDir, strPdb) == 0)	// same thing
	{
		strPdb += L"/";
		strPdb += CBldAction::s_szDefVCFile;
		bstrPdb = strPdb;
		return TRUE;
	}

	return FALSE;
}

STDMETHODIMP CVCCLCompilerTool::AffectsOutput(long nPropID, VARIANT_BOOL* pbAffectsOutput)
{
	CHECK_POINTER_NULL(pbAffectsOutput);
	*pbAffectsOutput = VARIANT_FALSE;

	switch (nPropID)
	{
	case VCCLID_BrowseInformation:	// enables *.sbr
	case VCCLID_BrowseInformationFile:
	case VCCLID_UsePrecompiledHeader:	// enable *.pch
	case VCCLID_PrecompiledHeaderThrough:
	case VCCLID_PrecompiledHeaderFile:
	case VCCLID_ObjectFile:
	case VCCLID_ProgramDataBaseFileName:
	case VCCLID_DebugInformationFormat:	// enables *.pdb
	case VCCLID_OutputsDirty:	// just a recheck on getting the output regenerated
	case VCFCFGID_ExcludedFromBuild:	// .obj included/excluded from build
		*pbAffectsOutput = VARIANT_TRUE;
		break;
	default:
		*pbAffectsOutput = VARIANT_FALSE;
		break;
	}

	return S_OK;
}

CBldCmdSet* CVCCLCompilerTool::GetCommandLineMatch(CVCPtrList& cmdSet, CStringW& rstrCmd)
{
	VCPOSITION pos = cmdSet.GetHeadPosition();
	while (pos != NULL)
	{
		CBldCmdSet* pSet = (CBldCmdSet *)cmdSet.GetNext(pos);
		if (rstrCmd == pSet->m_strCommandLine)
			return pSet;
	}

	return NULL;
}

/* static */
void CVCCLCompilerTool::RefreshResolvedPdbName(IVCPropertyContainer* pPropContainer, BOOL bForce /* = FALSE */)
{
	CComVariant var;
	if (!bForce && pPropContainer->GetProp(VCCLID_ResolvedPdbName, &var) == S_OK)
		return;

	CComBSTR bstrPdb;
	if (pPropContainer->GetStrProperty(VCCLID_ProgramDataBaseFileName, &bstrPdb) == S_OK)
		MakePdbAsFile(pPropContainer, bstrPdb);
	else
	{
		s_optHandler.GetDefaultValue( VCCLID_ProgramDataBaseFileName, &bstrPdb, pPropContainer );
		pPropContainer->Evaluate(bstrPdb, &bstrPdb);
	}
	CComBSTR bstrProjDir;
	if (pPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjDir) != S_OK)
		return;
	CDirW dirProj;
	if (!dirProj.CreateFromKnown(bstrProjDir))
		return;
	CPathW pathPdb;
	if (!pathPdb.CreateFromDirAndFilename(dirProj, bstrPdb))
		return;
	CStringW strExt = pathPdb.GetExtension();
	if (strExt.IsEmpty())
		pathPdb.ChangeExtension(L".pdb");
	CStringW strPdb = pathPdb;
	bstrPdb = strPdb;

	// want this set at the PROJECT config level, not the FILE config level
	CComQIPtr<IVCFileConfigurationImpl> spFileCfgImpl = pPropContainer;
	if (spFileCfgImpl != NULL)
	{
		CComPtr<IDispatch> spDispProjCfg;
		if (SUCCEEDED(spFileCfgImpl->get_Configuration(&spDispProjCfg)))
		{
			CComQIPtr<IVCPropertyContainer> spParentContainer = spDispProjCfg;
			if (spParentContainer)
				spParentContainer->SetStrProperty(VCCLID_ResolvedPdbName, bstrPdb);
		}
	}
	else
		pPropContainer->SetStrProperty(VCCLID_ResolvedPdbName, bstrPdb);
}

/* static */
void CVCCLCompilerTool::ClearResolvedPdbName(IVCPropertyContainer* pPropContainer)
{
	pPropContainer->Clear(VCCLID_ResolvedPdbName);
}

/* static */
HRESULT CVCCLCompilerTool::GetResolvedPdbName(IVCPropertyContainer* pPropContainer, BSTR* pbstrName)
{
	*pbstrName = NULL;
	RefreshResolvedPdbName(pPropContainer);
	return pPropContainer->GetStrProperty(VCCLID_ResolvedPdbName, pbstrName);
}

STDMETHODIMP CVCCLCompilerTool::GetCommandLinesForBuild(IVCBuildActionList* pActions, bldAttributes attrib, 
	IVCBuildEngine* pBuildEngine, IVCBuildErrorContext* pEC, IVCCommandLineList** ppCmds)
{
	CHECK_POINTER_NULL(ppCmds);
	*ppCmds = NULL;

	RETURN_ON_NULL2(pActions, S_FALSE);

	// generate C/C++ command-line set
	BOOL bHaveBaseCmdLine = FALSE;
	CVCPtrList cmdSet;

	HRESULT hr = S_OK;
	pActions->Reset(NULL);
	s_optHandler.InitializeFlags();
	while (hr == S_OK)
	{
		CComPtr<IVCBuildAction> spAction;
		hr = pActions->Next(&spAction, NULL);
		BREAK_ON_DONE(hr);
		ASSERT_AND_CONTINUE_ON_NULL(spAction);

		CComPtr<IVCBuildableItem> spBuildableItem;
		HRESULT hr1 = spAction->get_Item(&spBuildableItem);
		ASSERT_AND_CONTINUE_ON_FAIL_OR_NULL(hr1, spBuildableItem);

		CComQIPtr<IVCPropertyContainer> spPropContainer = spBuildableItem;
		VSASSERT(spPropContainer != NULL, "Action has no property container associated with it.  Bad initialization.");
		if (spPropContainer == NULL)
			continue;

		VARIANT_BOOL bHaveLocalStorage = VARIANT_TRUE;
		CBldCmdSet* pSet = NULL;
		CompileAsOptions compileAs = s_optHandler.GetDefaultCompileAs(spPropContainer, TRUE /* check explicit */);
		BOOL bLocalStorage = (SUCCEEDED(spPropContainer->HasLocalStorage(VARIANT_FALSE /*not for save*/, &bHaveLocalStorage)) 
			&& bHaveLocalStorage == VARIANT_TRUE);
		if (!bLocalStorage)
		{
			if (bHaveBaseCmdLine)
			{
				pSet = (CBldCmdSet *)cmdSet.GetHead();
				VSASSERT(pSet != NULL, "Failed to get any values of out command set");
				if (pSet == NULL)
					bLocalStorage = TRUE;	// need to generate this after all
				else if (pSet->m_compileAs != compileAs)
					bLocalStorage = TRUE;	// not the same as what we're calling our default, so need to generate after all
			}
			else
			{
				bHaveBaseCmdLine = TRUE;
				pSet = new CBldCmdSet;
				RETURN_ON_NULL2(pSet, E_OUTOFMEMORY);
				hr = GetCommandLineOptions(spBuildableItem, spAction, VARIANT_TRUE /* include additional options */, 
					VARIANT_FALSE /* !for display */, cmdLineForBuild, pSet->m_strCommandLine);
				pchOption usePCH = pchNone;
				if (spPropContainer->GetIntProperty(VCCLID_UsePrecompiledHeader, (long *)&usePCH) == S_OK 
					&& usePCH == pchCreateUsingSpecific)
					pSet->m_bMakeFirst = TRUE;
				pSet->m_compileAs = compileAs;
				cmdSet.AddHead(pSet);
			}
		}

		if (bLocalStorage)
		{
			CStringW strCommandLine;
			hr = GetCommandLineOptions(spBuildableItem, spAction, VARIANT_TRUE /* include additional options */, 
				VARIANT_FALSE /* !for display */, cmdLineForBuild, strCommandLine);
			if (SUCCEEDED(hr))
			{
				pSet = GetCommandLineMatch(cmdSet, strCommandLine);
				if (pSet == NULL)
				{
					pSet = new CBldCmdSet;
					RETURN_ON_NULL2(pSet, E_OUTOFMEMORY);
					pSet->m_strCommandLine = strCommandLine;
					pchOption usePCH = pchNone;
					if (spPropContainer->GetIntProperty(VCCLID_UsePrecompiledHeader, (long *)&usePCH) == S_OK 
						&& usePCH == pchCreateUsingSpecific)
						pSet->m_bMakeFirst = TRUE;
					pSet->m_compileAs = compileAs;
					cmdSet.AddTail(pSet);
				}
			}
		}
		if (SUCCEEDED(hr) && pSet != NULL)
		{
			CComBSTR bstrFile;
			spAction->GetRelativePathsToInputs(FALSE, TRUE, pEC, &bstrFile);
			CStringW strFile = bstrFile;
			pSet->m_strFilesList.AddTail(strFile);
		}
		VSASSERT(SUCCEEDED(hr), "Failed to pick up command line options");

	}

	if (FAILED(hr) && cmdSet.IsEmpty())
		return hr;

	else if (cmdSet.IsEmpty())
		return S_FALSE;

	else if (SUCCEEDED(hr))
	{
		CComBSTR bstrDescription;
		GetBuildDescription(NULL, &bstrDescription);

		CComBSTR bstrToolExe;
		hr = get_ToolPathInternal(&bstrToolExe);	// safe since we don't change the tool path on a per file cfg basis...
		VSASSERT(SUCCEEDED(hr), "Tool doesn't have a path.  Deep trouble.");

		hr = CVCCommandLineList::CreateInstance(ppCmds);
		VSASSERT(SUCCEEDED(hr), "Failed to create a command set;  out of memory?");
		if (SUCCEEDED(hr) && *ppCmds != NULL)
		{
			(*ppCmds)->put_UseConsoleCodePageForSpawner(VARIANT_TRUE /* want console CP */);
			CComPtr<IVCBuildAction> spAction;
			pActions->get_LastAction(&spAction);
			while (!cmdSet.IsEmpty() && SUCCEEDED(hr))
			{
				CBldCmdSet* pSet = (CBldCmdSet *)cmdSet.RemoveHead();
				while (!pSet->m_strFilesList.IsEmpty())
					s_optHandler.AdvanceCommandLine(pSet->m_strCommandLine, pSet->m_strFilesList.RemoveHead(), L"\n");
	
				CComPtr<IVCCommandLine> spCmdLine;
				hr = CVCCommandLine::CreateInstance(&spCmdLine);
				ASSERT_AND_CONTINUE_ON_FAIL_OR_NULL(hr, spCmdLine);

				CComBSTR bstrCmdLineIn = pSet->m_strCommandLine;
				CComBSTR bstrCmdLineOut;
				if (pBuildEngine != NULL)
					hr = pBuildEngine->FormCommandLine(bstrToolExe, bstrCmdLineIn, pEC, TRUE, TRUE, &bstrCmdLineOut);
				if (FAILED(hr) || pBuildEngine == NULL)
				{
					VSASSERT(SUCCEEDED(hr), "Failed to form command line");
					hr = S_OK;
					bstrCmdLineOut = bstrCmdLineIn;
				}
				spCmdLine->put_CommandLineContents(bstrCmdLineOut);
				spCmdLine->put_Description(bstrDescription);
				spCmdLine->put_NumberOfProcessors(1);

				(*ppCmds)->Add(spCmdLine, pSet->m_bMakeFirst);
				delete pSet;
			}
		}

		while (!cmdSet.IsEmpty())
		{
			CBldCmdSet* pSet = (CBldCmdSet *)cmdSet.RemoveHead();
			delete pSet;
		}
	}

	RETURN_ON_FAIL(hr);

	return S_OK;
}

STDMETHODIMP CVCCLCompilerTool::HasDependencies(IVCBuildAction* pBldAction, VARIANT_BOOL* pbHasDependencies)
{
	CHECK_POINTER_NULL(pbHasDependencies);
	*pbHasDependencies = VARIANT_TRUE;	// we've got this set up so that the user *cannot* turn dependencies off...
	return S_OK;
}

STDMETHODIMP CVCCLCompilerTool::GetDependencies(IVCBuildAction* pBldAction, IVCBuildStringCollection** ppStrings, 
	VARIANT_BOOL* bUpToDate)
{
	RETURN_ON_NULL2(pBldAction, E_UNEXPECTED);

	CHECK_POINTER_NULL(ppStrings);
	*ppStrings = NULL;

	CComPtr<IVCBuildableItem> spItem;
	pBldAction->get_Item(&spItem);
#ifdef _DEBUG
	CComQIPtr<VCFileConfiguration> spFileCfg = spItem;
	VSASSERT(spFileCfg != NULL, "File config without IVCFileConfigurationImpl?");
#endif	// _DEBUG
	RETURN_ON_NULL2(spItem, E_UNEXPECTED);

	BldFileRegHandle frh = NULL;
	spItem->get_FileRegHandle((void **)&frh);
	VSASSERT(frh != NULL, "*All* items CL operates on are files.  All files better have reg handles.");
	VSASSERT(frh->IsNodeType(CBldFileRegNotifyRx::nodetypeRegFile), "Reg handle is not for file.  Bad object fed to CL.");
	CBldFileRegFile* frf = (CBldFileRegFile*)g_FileRegistry.GetRegEntry(frh);

	// Get filename of source file.
	const CPathW* pPath = frf->GetFilePath();

	CComBSTR bstrIdb;
	CComPtr<VCConfiguration> spProjCfg;
	spItem->get_ProjectConfiguration(&spProjCfg);
	CComQIPtr<IVCConfigurationImpl> spProjCfgImpl = spProjCfg;
	if (spProjCfgImpl != NULL)
		spProjCfgImpl->get_MrePath(&bstrIdb);

	BOOL bHasDepend = FALSE;

	// Minimal rebuild dependency interface.
	IBldMreDependencies* pMreDepend = IBldMreDependencies::GetMreDependenciesIFace(bstrIdb);
	if (NULL != pMreDepend)
	{
		//
		// Does this file use the pch?
		//
		VARIANT_BOOL bUsePch = VARIANT_FALSE;
		CComQIPtr<IVCPropertyContainer> spPropContainer = spItem;
		if (spPropContainer != NULL)
		{
			pchOption usePCH = pchNone;
			if (spPropContainer->GetIntProperty(VCCLID_UsePrecompiledHeader, (long *)&usePCH) == S_OK && usePCH == pchUseUsingSpecific)
				bUsePch = VARIANT_TRUE;	
		}

//		SuspendCAP();
		CStringW strPath;
		pPath->GetFullPath(strPath);

		CVCBuildStringCollection* pStringColl;
		HRESULT hr1 = CVCBuildStringCollection::CreateInstance(ppStrings, &pStringColl);
		if (SUCCEEDED(hr1))
			bHasDepend = pMreDepend->GetDependencies(strPath, pStringColl, bUsePch == VARIANT_TRUE);

//		ResumeCAP();
		pMreDepend->Release();
	}

	// Are dependencies up-to-date?
	if (NULL != bUpToDate)
	{
		*bUpToDate = FALSE;
	}
	return bHasDepend ? S_OK : S_FALSE;
}

STDMETHODIMP CVCCLCompilerTool::CanScanForDependencies(VARIANT_BOOL* pbIsScannable)
{
	return COptionHandlerBase::GetValueTrue(pbIsScannable);
}

STDMETHODIMP CVCCLCompilerTool::GetDeploymentDependencies(IVCBuildAction* pAction, IVCBuildStringCollection** ppDeployDepStrings)
{
	RETURN_ON_NULL2(pAction, S_FALSE);

	CComPtr<IVCBuildableItem> spItem;
	pAction->get_Item(&spItem);
#ifdef _DEBUG
	CComQIPtr<VCFileConfiguration> spFileCfg = spItem;
	VSASSERT(spFileCfg != NULL, "File config without IVCFileConfigurationImpl?");
#endif	// _DEBUG
	RETURN_ON_NULL2(spItem, E_UNEXPECTED);

	BldFileRegHandle frh = NULL;
	spItem->get_FileRegHandle((void **)&frh);
	VSASSERT(frh != NULL, "*All* items CL operates on are files.  All files better have reg handles.");
	VSASSERT(frh->IsNodeType(CBldFileRegNotifyRx::nodetypeRegFile), "Reg handle is not for file.  Bad object fed to CL.");
	CBldFileRegFile* frf = (CBldFileRegFile*)g_FileRegistry.GetRegEntry(frh);

	// Get filename of source file.
	const CPathW* pPath = frf->GetFilePath();

	// Minimal rebuild dependency interface.
	CComBSTR bstrIdb;
	CComPtr<VCConfiguration> spProjCfg;
	spItem->get_ProjectConfiguration(&spProjCfg);
	CComQIPtr<IVCConfigurationImpl> spProjCfgImpl = spProjCfg;
	if (spProjCfgImpl != NULL)
		spProjCfgImpl->get_MrePath(&bstrIdb);

	IBldMreDependencies* pMreDepend = IBldMreDependencies::GetMreDependenciesIFace(bstrIdb);

	CComPtr<IVCBuildStringCollection> spStrings;
	CVCBuildStringCollection* pStringColl = NULL;
	if (NULL != pMreDepend)
	{
		CStringW strPath;
		pPath->GetFullPath(strPath);

		HRESULT hr1 = S_OK;
		if (pStringColl == NULL)
			hr1 = CVCBuildStringCollection::CreateInstance(&spStrings, &pStringColl);
		if (SUCCEEDED(hr1))
			pMreDepend->GetDeploymentDependencies(strPath, pStringColl);

		pMreDepend->Release();
	}

	if (pStringColl && pStringColl->m_strStrings.GetSize() > 0)	// we actually had something to worry about
	{
		if (*ppDeployDepStrings == NULL)
			*ppDeployDepStrings = spStrings.Detach();
		else
		{
			INT_PTR cDeps = pStringColl->m_strStrings.GetSize();
			for (INT_PTR idx = 0; idx < cDeps; idx++)
			{
				CStringW strDep = pStringColl->m_strStrings.GetAt(idx);
				if (strDep.IsEmpty())
					continue;
				CComBSTR bstrDep = strDep;
				(*ppDeployDepStrings)->Add(bstrDep);
			}
		}
	}

	return S_OK;
}

STDMETHODIMP CVCCLCompilerTool::CreatePageObject(IUnknown **ppUnk, CLSID*pCLSID, IVCPropertyContainer *pPropCntr, IVCSettingsPage *pPage )
{
	// Get the list of pages
	if( CLSID_ClGeneral == *pCLSID )
	{
		CPageObjectImpl<CVCCLCompilerGeneralPage,VCCLCOMPILERTOOL_MIN_DISPID,VCCLCOMPILERTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else if( CLSID_ClOptimization == *pCLSID )
	{
		CPageObjectImpl< CVCCLCompilerOptimizationPage,VCCLCOMPILERTOOL_MIN_DISPID,VCCLCOMPILERTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else if( CLSID_ClPreprocessor == *pCLSID )
	{
		CPageObjectImpl< CVCCLCompilerPreProcessorPage,VCCLCOMPILERTOOL_MIN_DISPID,VCCLCOMPILERTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else if( CLSID_ClAdvanced == *pCLSID )
	{
		CPageObjectImpl< CVCCLCompilerAdvancedPage,VCCLCOMPILERTOOL_MIN_DISPID,VCCLCOMPILERTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else if( CLSID_ClCodeGeneration == *pCLSID )
	{
		CPageObjectImpl< CVCCLCompilerCodeGenPage,VCCLCOMPILERTOOL_MIN_DISPID,VCCLCOMPILERTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else if( CLSID_ClLanguage == *pCLSID )
	{
		CPageObjectImpl< CVCCLCompilerLanguagePage,VCCLCOMPILERTOOL_MIN_DISPID,VCCLCOMPILERTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else if( CLSID_ClPrecompiledHeaders == *pCLSID )
	{
		CPageObjectImpl< CVCCLCompilerPCHPage,VCCLCOMPILERTOOL_MIN_DISPID,VCCLCOMPILERTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else if( CLSID_ClOutputFiles == *pCLSID )
	{
		CPageObjectImpl< CVCCLCompilerOutputPage,VCCLCOMPILERTOOL_MIN_DISPID,VCCLCOMPILERTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else if( CLSID_ClBrowseInfo == *pCLSID )
	{
		CPageObjectImpl< CVCCLCompilerBrowsePage,VCCLCOMPILERTOOL_MIN_DISPID,VCCLCOMPILERTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else
	{
		return S_FALSE;
	}
		
	return S_OK;
}

// general
STDMETHODIMP CVCCLCompilerTool::get_AdditionalOptions(BSTR* pbstrAdditionalOptions)
{	// anything we don't know what to do with, given verbatim, in same order as entered, parent contributions first, goes after all other options
	return ToolGetStrProperty(VCCLID_AdditionalOptions, pbstrAdditionalOptions);
}

STDMETHODIMP CVCCLCompilerTool::put_AdditionalOptions(BSTR bstrAdditionalOptions)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCCLID_AdditionalOptions, bstrAdditionalOptions);
}

STDMETHODIMP CVCCLCompilerTool::get_ToolName(BSTR* pbstrToolName)
{	// friendly name of tool, e.g., "C/C++ Compiler Tool"
	CHECK_POINTER_VALID(pbstrToolName);
	InitToolName();
	s_bstrToolName.CopyTo(pbstrToolName);
	return S_OK;
}

STDMETHODIMP CVCCLCompilerTool::get_WarningLevel(warningLevelOption* poptSetting)
{	// see enum above (/W0 - /W4)
	return ToolGetIntProperty(VCCLID_WarningLevel, (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerTool::put_WarningLevel(warningLevelOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, CLWarnMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCCLID_WarningLevel, optSetting);
}

STDMETHODIMP CVCCLCompilerTool::get_WarnAsError(VARIANT_BOOL* pbWarnAsError)
{	// (/WX, /WX-) treat warnings as errors
	return ToolGetBoolProperty(VCCLID_WarnAsError, pbWarnAsError);
}

STDMETHODIMP CVCCLCompilerTool::put_WarnAsError(VARIANT_BOOL bWarnAsError)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bWarnAsError );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_WarnAsError, bWarnAsError);
}

STDMETHODIMP CVCCLCompilerTool::get_SuppressStartupBanner(VARIANT_BOOL* pbNoLogo)
{	// (/nologo, /nologo-) enable/disable suppression of copyright message
	return ToolGetBoolProperty(VCCLID_SuppressStartupBanner,pbNoLogo );
}

STDMETHODIMP CVCCLCompilerTool::put_SuppressStartupBanner(VARIANT_BOOL bNoLogo)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bNoLogo );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_SuppressStartupBanner, bNoLogo);
}

STDMETHODIMP CVCCLCompilerTool::get_Detect64BitPortabilityProblems(VARIANT_BOOL* pbDetect64BitPortabilityProblems)
{	// (/Wp64) detect 64 bit portability problems
	return ToolGetBoolProperty(VCCLID_Detect64BitPortabilityProblems, pbDetect64BitPortabilityProblems);
}

STDMETHODIMP CVCCLCompilerTool::put_Detect64BitPortabilityProblems(VARIANT_BOOL bDetect64BitPortabilityProblems)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bDetect64BitPortabilityProblems );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_Detect64BitPortabilityProblems, bDetect64BitPortabilityProblems);
}

STDMETHODIMP CVCCLCompilerTool::get_DebugInformationFormat(debugOption* poptSetting)
{	// see enum above (/Z7, Zd, /ZI, /Zi)
	return ToolGetIntProperty(VCCLID_DebugInformationFormat, (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerTool::put_DebugInformationFormat(debugOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, DebugInfoMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCCLID_DebugInformationFormat, optSetting);
}

STDMETHODIMP CVCCLCompilerTool::get_CompileAsManaged(compileAsManagedOptions* poptSetting)
{ 
	return ToolGetIntProperty(VCCLID_CompileAsManaged, (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerTool::put_CompileAsManaged(compileAsManagedOptions optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, ComRTMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCCLID_CompileAsManaged, optSetting);
}

STDMETHODIMP CVCCLCompilerTool::get_AdditionalIncludeDirectories(BSTR* pbstrIncludePath)
{	// (/I![path]) directory to add to include path, may have multiple
	return ToolGetStrProperty(VCCLID_AdditionalIncludeDirectories, pbstrIncludePath, true /* local only */);
}

STDMETHODIMP CVCCLCompilerTool::put_AdditionalIncludeDirectories(BSTR bstrIncludePath)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCCLID_AdditionalIncludeDirectories, bstrIncludePath);
}

STDMETHODIMP CVCCLCompilerTool::get_AdditionalUsingDirectories(BSTR* pbstrIncludePath)
{	// (/AI![path]) directory to add to LIBPATH path, may have multiple
	return ToolGetStrProperty(VCCLID_AdditionalUsingDirectories, pbstrIncludePath, true /* local only */);
}

STDMETHODIMP CVCCLCompilerTool::put_AdditionalUsingDirectories(BSTR bstrIncludePath)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCCLID_AdditionalUsingDirectories, bstrIncludePath);
}

// Optimization
STDMETHODIMP CVCCLCompilerTool::get_Optimization(optimizeOption* poptSetting)
{	// see enum above (/O1, /O2, /Od, /Ox)
	return ToolGetIntProperty(VCCLID_Optimization, (long*)poptSetting);
}

STDMETHODIMP CVCCLCompilerTool::put_Optimization(optimizeOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, OptimizeMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCCLID_Optimization, optSetting);
}

STDMETHODIMP CVCCLCompilerTool::get_GlobalOptimizations(VARIANT_BOOL* pbGlobalOpt)
{ 	// (/Og, /Og-) enable/disable global optimizations incompatible with all RTC options, GZ, ZI, EEf
	return ToolGetBoolProperty(VCCLID_GlobalOptimizations, pbGlobalOpt);
}

STDMETHODIMP CVCCLCompilerTool::put_GlobalOptimizations(VARIANT_BOOL bGlobalOpt)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bGlobalOpt );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_GlobalOptimizations, bGlobalOpt);
}

STDMETHODIMP CVCCLCompilerTool::get_InlineFunctionExpansion(inlineExpansionOption* poptSetting)
{	// see enum above (/Ob0, /Ob1, /Ob2)
	return ToolGetIntProperty(VCCLID_InlineFunctionExpansion, (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerTool::put_InlineFunctionExpansion(inlineExpansionOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, InlineMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCCLID_InlineFunctionExpansion, optSetting);
}

STDMETHODIMP CVCCLCompilerTool::get_EnableIntrinsicFunctions(VARIANT_BOOL* pbEnableIntrinsic)
{	// (/Oi, /Oi-) enable/disable intrinsic functions works best with /Og
	return ToolGetBoolProperty(VCCLID_EnableIntrinsicFunctions, pbEnableIntrinsic);
}

STDMETHODIMP CVCCLCompilerTool::put_EnableIntrinsicFunctions(VARIANT_BOOL bEnableIntrinsic)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bEnableIntrinsic );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_EnableIntrinsicFunctions, bEnableIntrinsic);
}

STDMETHODIMP CVCCLCompilerTool::get_ImproveFloatingPointConsistency(VARIANT_BOOL* pbImproveFloat)
{	// (/Op, /Op-) (don't) improve floating-point consistency
	return ToolGetBoolProperty(VCCLID_ImproveFloatingPointConsistency, pbImproveFloat);
}

STDMETHODIMP CVCCLCompilerTool::put_ImproveFloatingPointConsistency(VARIANT_BOOL bImproveFloat)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bImproveFloat );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_ImproveFloatingPointConsistency, bImproveFloat);
}

STDMETHODIMP CVCCLCompilerTool::get_FavorSizeOrSpeed(favorSizeOrSpeedOption* poptSetting)
{	// (/Os, Ot) favor size/speed works best with /Og
	return ToolGetIntProperty(VCCLID_FavorSizeOrSpeed, (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerTool::put_FavorSizeOrSpeed(favorSizeOrSpeedOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, CodeGenMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCCLID_FavorSizeOrSpeed, optSetting);
}

STDMETHODIMP CVCCLCompilerTool::get_OmitFramePointers(VARIANT_BOOL* poptSetting)
{	// (/Oy, Oy-) enable/disable frame pointer omission
	return ToolGetBoolProperty(VCCLID_OmitFramePointers, poptSetting);
}

STDMETHODIMP CVCCLCompilerTool::put_OmitFramePointers(VARIANT_BOOL optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( optSetting );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_OmitFramePointers, optSetting);
}

STDMETHODIMP CVCCLCompilerTool::get_EnableFiberSafeOptimizations(VARIANT_BOOL* pbFiberSafeTLS)
{	// (/GT) generate fiber-safe TLS accesses
	return ToolGetBoolProperty(VCCLID_EnableFiberSafeOptimizations, pbFiberSafeTLS);
}

STDMETHODIMP CVCCLCompilerTool::put_EnableFiberSafeOptimizations(VARIANT_BOOL bFiberSafeTLS)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bFiberSafeTLS );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_EnableFiberSafeOptimizations, bFiberSafeTLS);
}

STDMETHODIMP CVCCLCompilerTool::get_WholeProgramOptimization(VARIANT_BOOL* pbWholeProgOp)
{	// (/GL) Perform whole program optimization during the link phase
	return ToolGetBoolProperty(VCCLID_WholeProgramOptimization, pbWholeProgOp);
}

STDMETHODIMP CVCCLCompilerTool::put_WholeProgramOptimization(VARIANT_BOOL bWholeProgOp)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bWholeProgOp );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_WholeProgramOptimization, bWholeProgOp);
}

STDMETHODIMP CVCCLCompilerTool::get_OptimizeForProcessor(ProcessorOptimizeOption* poptSetting)
{ 
	return ToolGetIntProperty(VCCLID_OptimizeForProcessor, (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerTool::put_OptimizeForProcessor(ProcessorOptimizeOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, OptProcMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCCLID_OptimizeForProcessor, optSetting);
}

STDMETHODIMP CVCCLCompilerTool::get_OptimizeForWindowsApplication(VARIANT_BOOL* poptSetting)
{ 
	return ToolGetBoolProperty(VCCLID_OptimizeForWindowsApplication, poptSetting);
}

STDMETHODIMP CVCCLCompilerTool::put_OptimizeForWindowsApplication(VARIANT_BOOL optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( optSetting );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_OptimizeForWindowsApplication, optSetting);
}

// Preprocessor
STDMETHODIMP CVCCLCompilerTool::get_PreprocessorDefinitions(BSTR* pbstrDefines)
{		// (/D[name]) defines, can have multiple
	return ToolGetStrProperty(VCCLID_PreprocessorDefinitions, pbstrDefines, true /* local only */);
}

STDMETHODIMP CVCCLCompilerTool::put_PreprocessorDefinitions(BSTR bstrDefines)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCCLID_PreprocessorDefinitions, bstrDefines);
}

STDMETHODIMP CVCCLCompilerTool::get_IgnoreStandardIncludePath(VARIANT_BOOL* pbIgnoreInclPath)
{	// (/X) ignore standard include path
	return ToolGetBoolProperty(VCCLID_IgnoreStandardIncludePath, pbIgnoreInclPath);
}

STDMETHODIMP CVCCLCompilerTool::put_IgnoreStandardIncludePath(VARIANT_BOOL bIgnoreInclPath)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bIgnoreInclPath );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_IgnoreStandardIncludePath, bIgnoreInclPath);
}

STDMETHODIMP CVCCLCompilerTool::get_GeneratePreprocessedFile(preprocessOption* poptSetting)
{	// see enum above (/P, /EP /P)
	return ToolGetIntProperty(VCCLID_GeneratePreprocessedFile, (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerTool::put_GeneratePreprocessedFile(preprocessOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, PreprocessMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCCLID_GeneratePreprocessedFile, optSetting);
}

STDMETHODIMP CVCCLCompilerTool::get_KeepComments(VARIANT_BOOL* pbKeepComments)
{	// (/C) don't strip comments, requires one of /E, /EP, /P switches
	return ToolGetBoolProperty(VCCLID_KeepComments, pbKeepComments);
}

STDMETHODIMP CVCCLCompilerTool::put_KeepComments(VARIANT_BOOL bKeepComments)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bKeepComments );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_KeepComments, bKeepComments);
}

// Code Generation; /MP, /MPlowpri deliberately left out (since we require /FD or /Gm to be thrown)
STDMETHODIMP CVCCLCompilerTool::get_StringPooling(VARIANT_BOOL* pbPool)
{	// (/GF) enable read-only string pooling
	return ToolGetBoolProperty(VCCLID_StringPooling, pbPool);
}

STDMETHODIMP CVCCLCompilerTool::put_StringPooling(VARIANT_BOOL bPool)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bPool );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_StringPooling, bPool);
}

STDMETHODIMP CVCCLCompilerTool::get_MinimalRebuild(VARIANT_BOOL* pbMinimalRebuild)
{	// (/Gm, /Gm-) enable/disable minimal rebuild, /Gm requires /ZI or /Zi
	return ToolGetBoolProperty(VCCLID_MinimalRebuild, pbMinimalRebuild);
}

STDMETHODIMP CVCCLCompilerTool::put_MinimalRebuild(VARIANT_BOOL bMinimalRebuild)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bMinimalRebuild );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_MinimalRebuild, bMinimalRebuild);
}

STDMETHODIMP CVCCLCompilerTool::get_ExceptionHandling(VARIANT_BOOL* pbExcept)
{	// /EHsc
	return ToolGetBoolProperty(VCCLID_ExceptionHandling, pbExcept);
}

STDMETHODIMP CVCCLCompilerTool::put_ExceptionHandling(VARIANT_BOOL bExcept)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bExcept );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_ExceptionHandling, bExcept);
}


STDMETHODIMP CVCCLCompilerTool::get_BasicRuntimeChecks(basicRuntimeCheckOption* poptSetting)
{ 
	return ToolGetIntProperty(VCCLID_BasicRuntimeChecks, (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerTool::put_BasicRuntimeChecks(basicRuntimeCheckOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, BasicRTCheckMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCCLID_BasicRuntimeChecks, optSetting);
}

STDMETHODIMP CVCCLCompilerTool::get_SmallerTypeCheck(VARIANT_BOOL* pbSmallerType)
{ 
	return ToolGetBoolProperty(VCCLID_SmallerTypeCheck, pbSmallerType);
}

STDMETHODIMP CVCCLCompilerTool::put_SmallerTypeCheck(VARIANT_BOOL bSmallerType)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bSmallerType );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_SmallerTypeCheck, bSmallerType);
}

STDMETHODIMP CVCCLCompilerTool::get_RuntimeLibrary(runtimeLibraryOption* poptSetting)
{ 
	return ToolGetIntProperty(VCCLID_RuntimeLibrary, (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerTool::put_RuntimeLibrary(runtimeLibraryOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, RTLibMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCCLID_RuntimeLibrary, optSetting);
}

STDMETHODIMP CVCCLCompilerTool::get_BufferSecurityCheck(VARIANT_BOOL* pbSecure)
{	// (/GS) enable buffer overrun checks; buffer security from hackers
	return ToolGetBoolProperty(VCCLID_BufferSecurityCheck, pbSecure);
}

STDMETHODIMP CVCCLCompilerTool::put_BufferSecurityCheck(VARIANT_BOOL bSecure)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bSecure );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_BufferSecurityCheck, bSecure);
}

STDMETHODIMP CVCCLCompilerTool::get_StructMemberAlignment(structMemberAlignOption* poptSetting)
{ 
	return ToolGetIntProperty(VCCLID_StructMemberAlignment, (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerTool::put_StructMemberAlignment(structMemberAlignOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, CLStructAlignMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCCLID_StructMemberAlignment, optSetting);
}

STDMETHODIMP CVCCLCompilerTool::get_EnableFunctionLevelLinking(VARIANT_BOOL* pbPackage)
{	// (/Gy) enable function level linking (no explicit off)
	return ToolGetBoolProperty(VCCLID_EnableFunctionLevelLinking, pbPackage);
}

STDMETHODIMP CVCCLCompilerTool::put_EnableFunctionLevelLinking(VARIANT_BOOL bPackage)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bPackage );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_EnableFunctionLevelLinking, bPackage);
}

// Language
STDMETHODIMP CVCCLCompilerTool::get_DisableLanguageExtensions(VARIANT_BOOL* pbDisableExtensions)
{	// (/Za, /Ze) disable/enable language extensions (/Ze default)
	return ToolGetBoolProperty(VCCLID_DisableLanguageExtensions, pbDisableExtensions);
}

STDMETHODIMP CVCCLCompilerTool::put_DisableLanguageExtensions(VARIANT_BOOL bDisableExtensions)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bDisableExtensions );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_DisableLanguageExtensions, bDisableExtensions);
}

STDMETHODIMP CVCCLCompilerTool::get_DefaultCharIsUnsigned(VARIANT_BOOL* pbIsUnsigned)
{	// (/J) default char type is unsigned
	return ToolGetBoolProperty(VCCLID_DefaultCharIsUnsigned, pbIsUnsigned);
}

STDMETHODIMP CVCCLCompilerTool::put_DefaultCharIsUnsigned(VARIANT_BOOL bIsUnsigned)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bIsUnsigned );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_DefaultCharIsUnsigned, bIsUnsigned);
}

STDMETHODIMP CVCCLCompilerTool::get_TreatWChar_tAsBuiltInType(VARIANT_BOOL* pbBuiltInType)
{	// (/Zc:wchar_t) treat wchar_t as built-in type
	return ToolGetBoolProperty(VCCLID_TreatWChar_tAsBuiltInType, pbBuiltInType);
}

STDMETHODIMP CVCCLCompilerTool::put_TreatWChar_tAsBuiltInType(VARIANT_BOOL bBuiltInType)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bBuiltInType );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_TreatWChar_tAsBuiltInType, bBuiltInType);
}

STDMETHODIMP CVCCLCompilerTool::get_ForceConformanceInForLoopScope(VARIANT_BOOL* pbConform)
{	// (/Zc:forScope) check for conformance of vars for 'for' scope
	return ToolGetBoolProperty(VCCLID_ForceConformanceInForLoopScope, pbConform);
}

STDMETHODIMP CVCCLCompilerTool::put_ForceConformanceInForLoopScope(VARIANT_BOOL bConform)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bConform );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_ForceConformanceInForLoopScope, bConform);
}

STDMETHODIMP CVCCLCompilerTool::get_RuntimeTypeInfo(VARIANT_BOOL* pbRTTI)
{	// (/GR, /GR-) enable/disable C++ RTTI
	return ToolGetBoolProperty(VCCLID_RuntimeTypeInfo, pbRTTI);
}

STDMETHODIMP CVCCLCompilerTool::put_RuntimeTypeInfo(VARIANT_BOOL bRTTI)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bRTTI );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_RuntimeTypeInfo, bRTTI);
}

// Precompiled Headers
STDMETHODIMP CVCCLCompilerTool::get_UsePrecompiledHeader(pchOption* poptSetting)
{	// see enum above (/Yc, /YX, /Yu)
	return ToolGetIntProperty(VCCLID_UsePrecompiledHeader, (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerTool::put_UsePrecompiledHeader(pchOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, UsePCHMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCCLID_UsePrecompiledHeader, optSetting);
}

STDMETHODIMP CVCCLCompilerTool::get_PrecompiledHeaderThrough(BSTR* pbstrFile)
{	// optional argument to UsePrecompiledHeader property specifying file to use to generate/use PCH
	return ToolGetStrProperty(VCCLID_PrecompiledHeaderThrough, pbstrFile);
}

STDMETHODIMP CVCCLCompilerTool::put_PrecompiledHeaderThrough(BSTR bstrFile)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCCLID_PrecompiledHeaderThrough, bstrFile);
}

STDMETHODIMP CVCCLCompilerTool::get_PrecompiledHeaderFile(BSTR* pbstrPCH)
{	// (/Fp[name]) name the precompiled header file, can be directory location or leave off .pch extension
	return ToolGetStrProperty(VCCLID_PrecompiledHeaderFile, pbstrPCH);
}

STDMETHODIMP CVCCLCompilerTool::put_PrecompiledHeaderFile(BSTR bstrPCH)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCCLID_PrecompiledHeaderFile, bstrPCH);
}

// Output Files
STDMETHODIMP CVCCLCompilerTool::get_ExpandAttributedSource(VARIANT_BOOL* pbExpandAttributedSource)
{ 	// (/Fx) listing with attributed code expanded into source file
	return ToolGetBoolProperty(VCCLID_ExpandAttributedSource, pbExpandAttributedSource);
}

STDMETHODIMP CVCCLCompilerTool::put_ExpandAttributedSource(VARIANT_BOOL bExpandAttributedSource)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bExpandAttributedSource );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_ExpandAttributedSource, bExpandAttributedSource);
}

STDMETHODIMP CVCCLCompilerTool::get_AssemblerOutput(asmListingOption* poptSetting)
{	// see enum above (/FA, /FAc, /FAs, /FAcs)
	return ToolGetIntProperty(VCCLID_AssemblerOutput, (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerTool::put_AssemblerOutput(asmListingOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, AsmListMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCCLID_AssemblerOutput, optSetting);
}

STDMETHODIMP CVCCLCompilerTool::get_AssemblerListingLocation(BSTR* pbstrName)
{	// (/Fa[name]) specify directory and/or name for listing file from AssemblerOutput property
	return ToolGetStrProperty(VCCLID_AssemblerListingLocation, pbstrName);
}

STDMETHODIMP CVCCLCompilerTool::put_AssemblerListingLocation(BSTR bstrName)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCCLID_AssemblerListingLocation, bstrName);
}

STDMETHODIMP CVCCLCompilerTool::get_ObjectFile(BSTR* pbstrName)
{	// (/Fo[name]) name obj file
	return ToolGetStrProperty(VCCLID_ObjectFile, pbstrName);
}

STDMETHODIMP CVCCLCompilerTool::put_ObjectFile(BSTR bstrName)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCCLID_ObjectFile, bstrName);
}

STDMETHODIMP CVCCLCompilerTool::get_ProgramDataBaseFileName(BSTR* pbstrName)
{	// (/Fd[file]) name PDB file.  can be directory name
	return ToolGetStrProperty(VCCLID_ProgramDataBaseFileName, pbstrName);
}

STDMETHODIMP CVCCLCompilerTool::put_ProgramDataBaseFileName(BSTR bstrName)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCCLID_ProgramDataBaseFileName, bstrName);
}

// Browse info helpers
STDMETHODIMP CVCCLCompilerTool::get_BrowseInformation(browseInfoOption* poptSetting)
{	// see enum above, (/FR[name], /Fr[name])
	return ToolGetIntProperty(VCCLID_BrowseInformation, (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerTool::put_BrowseInformation(browseInfoOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, BrowseMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCCLID_BrowseInformation, optSetting);
}

STDMETHODIMP CVCCLCompilerTool::get_BrowseInformationFile(BSTR* pbstrFile)
{		// optional name to go with BrowseInformation property
	return ToolGetStrProperty(VCCLID_BrowseInformationFile, pbstrFile);
}

STDMETHODIMP CVCCLCompilerTool::put_BrowseInformationFile(BSTR bstrFile)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCCLID_BrowseInformationFile, bstrFile);
}

// Hidden
STDMETHODIMP CVCCLCompilerTool::get_CompileOnly(VARIANT_BOOL* pbCompileOnly)
{	// (/c)	compile only, no link
	return ToolGetBoolProperty(VCCLID_CompileOnly, pbCompileOnly);
}

STDMETHODIMP CVCCLCompilerTool::put_CompileOnly(VARIANT_BOOL bCompileOnly)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bCompileOnly );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_CompileOnly, bCompileOnly);
}

// Advanced
STDMETHODIMP CVCCLCompilerTool::get_CallingConvention(callingConventionOption* poptSetting)
{ 
	return ToolGetIntProperty(VCCLID_CallingConvention, (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerTool::put_CallingConvention(callingConventionOption optSetting)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, CallConvMax, optSetting);
	return m_spPropertyContainer->SetIntProperty(VCCLID_CallingConvention, optSetting);
}

STDMETHODIMP CVCCLCompilerTool::get_CompileAs(CompileAsOptions* pcompileAs)
{	// see enum above (/TC, /TP)
	return ToolGetIntProperty(VCCLID_CompileAs, (long *)pcompileAs);
}

STDMETHODIMP CVCCLCompilerTool::put_CompileAs(CompileAsOptions compileAs)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(StdEnumMin, CompileAsMax, compileAs);
	return m_spPropertyContainer->SetIntProperty(VCCLID_CompileAs, compileAs);
}

STDMETHODIMP CVCCLCompilerTool::get_DisableSpecificWarnings(BSTR* pbstrDisableSpecificWarnings)
{	// (/wd<num>) disable specific warnings; multi-prop
	return ToolGetStrProperty(VCCLID_DisableSpecificWarnings, pbstrDisableSpecificWarnings, true /* local only */);
}

STDMETHODIMP CVCCLCompilerTool::put_DisableSpecificWarnings(BSTR bstrDisableSpecificWarnings)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCCLID_DisableSpecificWarnings, bstrDisableSpecificWarnings);
}

STDMETHODIMP CVCCLCompilerTool::get_ForcedIncludeFiles(BSTR* pbstrName)
{	// (/FI![name]) name forced include file, can have multiple
	return ToolGetStrProperty(VCCLID_ForcedIncludeFiles, pbstrName, true /* local only */);
}

STDMETHODIMP CVCCLCompilerTool::put_ForcedIncludeFiles(BSTR bstrName)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCCLID_ForcedIncludeFiles, bstrName);
}

STDMETHODIMP CVCCLCompilerTool::get_ForcedUsingFiles(BSTR* pbstrName)
{	// (/FU![name]) name forced #using file, can have multiple
	return ToolGetStrProperty(VCCLID_ForcedUsingFiles, pbstrName, true /* local only */);
}

STDMETHODIMP CVCCLCompilerTool::put_ForcedUsingFiles(BSTR bstrName)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCCLID_ForcedUsingFiles, bstrName);
}

STDMETHODIMP CVCCLCompilerTool::get_ShowIncludes(VARIANT_BOOL* pbShowInc)
{	// (/showIncludes)
	return ToolGetBoolProperty(VCCLID_ShowIncludes, pbShowInc);
}

STDMETHODIMP CVCCLCompilerTool::put_ShowIncludes(VARIANT_BOOL bShowInc)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bShowInc );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_ShowIncludes, bShowInc);
}

STDMETHODIMP CVCCLCompilerTool::get_UndefinePreprocessorDefinitions(BSTR* pbstrUndefines)
{	// (/U[name]) undefine predefined macro, can have multiple
	return ToolGetStrProperty(VCCLID_UndefinePreprocessorDefinitions, pbstrUndefines, true /* local only */);
}

STDMETHODIMP CVCCLCompilerTool::put_UndefinePreprocessorDefinitions(BSTR bstrUndefines)	
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCCLID_UndefinePreprocessorDefinitions, bstrUndefines);
}

STDMETHODIMP CVCCLCompilerTool::get_UndefineAllPreprocessorDefinitions(VARIANT_BOOL* pbPredefinedMacros)
{	// (/u) undefine all predefined macros 
	return ToolGetBoolProperty(VCCLID_UndefineAllPreprocessorDefinitions, pbPredefinedMacros);
}

STDMETHODIMP CVCCLCompilerTool::put_UndefineAllPreprocessorDefinitions(VARIANT_BOOL bPredefinedMacros)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bPredefinedMacros );
	return m_spPropertyContainer->SetBoolProperty(VCCLID_UndefineAllPreprocessorDefinitions, bPredefinedMacros);
}

// Automation properties
STDMETHODIMP CVCCLCompilerTool::get_ToolPath(BSTR *pVal)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CComBSTR bstrPath( szCLCompilerToolPath );
	*pVal = bstrPath.Detach();
	return S_OK;
}

STDMETHODIMP CVCCLCompilerTool::get_PropertyOption(BSTR bstrProp, long dispidProp, BSTR *pVal)
{ 
	return DoGetPropertyOption(bstrProp, dispidProp, pVal);
}

STDMETHODIMP CVCCLCompilerTool::get_FullIncludePath(BSTR* fullIncludePath)
{	// include path, including all inherited values, plus platform includes
	return get_FullIncludePathInternal(fullIncludePath);
}

// IVCToolImpl
STDMETHODIMP CVCCLCompilerTool::get_DefaultExtensions(BSTR* pVal)
{
	return DoGetDefaultExtensions(s_bstrExtensions, wszCLDefaultExtensions, pVal);
}

STDMETHODIMP CVCCLCompilerTool::put_DefaultExtensions(BSTR newVal)
{
	s_bstrExtensions = newVal;
	return S_OK;
}

STDMETHODIMP CVCCLCompilerTool::GetAdditionalOptionsInternal(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, 
	BSTR* pbstrAdditionalOptions)
{ 
	return GetProcessedAdditionalOptions(pItem, VCCLID_AdditionalOptions, bForBuild, bSkipLocal, pbstrAdditionalOptions);
}

STDMETHODIMP CVCCLCompilerTool::GetBuildDescription(IVCBuildAction* pAction, BSTR* pbstrBuildDescription)
{
	if (s_bstrBuildDescription.Length() == 0)
		s_bstrBuildDescription.LoadString(IDS_DESC_COMPILING);
	return s_bstrBuildDescription.CopyTo(pbstrBuildDescription);
}

STDMETHODIMP CVCCLCompilerTool::GetAdditionalIncludeDirectoriesInternal(IVCPropertyContainer* pPropContainer, BSTR* pbstrIncDirs)
{
	CHECK_POINTER_NULL(pbstrIncDirs);
	RETURN_INVALID_ON_NULL(pPropContainer);
	*pbstrIncDirs = NULL;
	CComBSTR bstrTmp;
	HRESULT hr = pPropContainer->GetMultiProp(VCCLID_AdditionalIncludeDirectories, L";", VARIANT_FALSE, &bstrTmp);
	if (SUCCEEDED(hr))
		pPropContainer->Evaluate(bstrTmp, pbstrIncDirs);
	else
		*pbstrIncDirs = bstrTmp.Detach();
	return hr;
}

STDMETHODIMP CVCCLCompilerTool::get_IncludePathID(long* pnIncludeID)
{
	CHECK_POINTER_NULL(pnIncludeID);
	*pnIncludeID = VCCLID_AdditionalIncludeDirectories;
	return S_OK;
}

STDMETHODIMP CVCCLCompilerTool::get_ToolPathInternal(BSTR* pbstrToolPath)
{ 
	return get_ToolPath(pbstrToolPath);
}

STDMETHODIMP CVCCLCompilerTool::get_Bucket(long *pVal)
{
	CHECK_POINTER_NULL(pVal);
	*pVal = BUCKET_CPP;
	return S_OK;
}

STDMETHODIMP CVCCLCompilerTool::get_ToolShortName(BSTR* pbstrToolName)
{
	CComBSTR bstrName( szCLCompilerToolShortName );
	*pbstrToolName = bstrName.Detach();
	return S_OK;
}

STDMETHODIMP CVCCLCompilerTool::MatchName(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches)
{
	return DoMatchName(bstrName, szCLCompilerToolType, szCLCompilerToolShortName, pbMatches);
}

STDMETHODIMP CVCCLCompilerTool::CreateToolObject(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject)
{ 
	return CreateInstance(pPropContainer, ppToolObject); 
}

STDMETHODIMP CVCCLCompilerTool::HasVirtualLocalStorage(IVCPropertyContainer* pPropContainer, VARIANT_BOOL* pbHasVirtualLocalStorage)
{
	HRESULT hr = CVCToolImpl::HasVirtualLocalStorage(pPropContainer, pbHasVirtualLocalStorage);
	RETURN_ON_FAIL(hr);		// parent took care of evaluating parameters

	static const long nPropList[] =
	{
		VCCLID_AssemblerListingLocation,
		VCCLID_ObjectFile,
		VCCLID_BrowseInformationFile,
		0
	};

	// some compiler options take a parameter that could, with the proper macro, make what appears to be an inherited
	// value need to be evaluated on a per file basis.
	for (int idx = 0; *pbHasVirtualLocalStorage == VARIANT_FALSE && nPropList[idx] != 0; idx++)
	{
		CComBSTR bstrVal;
		if (pPropContainer->GetStrProperty(nPropList[idx], &bstrVal) != S_OK)
			continue;

		CStringW strVal = bstrVal;
		strVal.MakeLower();
		*pbHasVirtualLocalStorage = ((strVal.Find(L"$(input") >= 0) && ((strVal.Find(L"$(inputname)") >= 0) || (strVal.Find(L"$(inputext)") >= 0) ||
			(strVal.Find(L"$(inputpath)") >= 0)));
	}

	// need to set up a separate command line for those things that have /TC or /TP thrown on them
	if (*pbHasVirtualLocalStorage == VARIANT_FALSE)
	{
		CComQIPtr<VCFileConfiguration> spFileCfg = pPropContainer;
		if (spFileCfg)
		{
			CComVariant var;
			*pbHasVirtualLocalStorage = (pPropContainer->GetLocalProp(VCCLID_CompileAs, &var) == S_OK && var.vt != VT_EMPTY && 
				(CompileAsOptions)var.lVal != compileAsDefault);
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Settings pages
/////////////////////////////////////////////////////////////////////////////////////////

// Optimization
STDMETHODIMP CVCCLCompilerOptimizationPage::get_Optimization(optimizeOption* poptSetting)
{	// see enum above (/O1, /O2, /Od, /Ox)
	return ToolGetIntProperty(VCCLID_Optimization, &(CVCCLCompilerTool::s_optHandler), (long*)poptSetting);
}

STDMETHODIMP CVCCLCompilerOptimizationPage::put_Optimization(optimizeOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, OptimizeMax, optSetting);
	return SetIntProperty(VCCLID_Optimization, optSetting);
}

STDMETHODIMP CVCCLCompilerOptimizationPage::get_GlobalOptimizations(enumGlobalOptimizationsBOOL* pbGlobalOpt)
{ 	// (/Og, /Og-) enable/disable global optimizations incompatible with all RTC options, GZ, ZI, EEf
	return GetEnumBoolProperty2(VCCLID_GlobalOptimizations, (long *)pbGlobalOpt);
}

STDMETHODIMP CVCCLCompilerOptimizationPage::put_GlobalOptimizations(enumGlobalOptimizationsBOOL bGlobalOpt)
{
	return SetBoolProperty(VCCLID_GlobalOptimizations, bGlobalOpt);
}

STDMETHODIMP CVCCLCompilerOptimizationPage::get_InlineFunctionExpansion(inlineExpansionOption* poptSetting)
{	// see enum above (/Ob0, /Ob1, /Ob2)
	return ToolGetIntProperty(VCCLID_InlineFunctionExpansion, &(CVCCLCompilerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerOptimizationPage::put_InlineFunctionExpansion(inlineExpansionOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, InlineMax, optSetting);
	return SetIntProperty(VCCLID_InlineFunctionExpansion, optSetting);
}

STDMETHODIMP CVCCLCompilerOptimizationPage::get_EnableIntrinsicFunctions(enumEnableIntrinsicFunctionsBOOL* pbEnableIntrinsic)
{	// (/Oi, /Oi-) enable/disable intrinsic functions works best with /Og
	return GetEnumBoolProperty2(VCCLID_EnableIntrinsicFunctions, (long *)pbEnableIntrinsic);
}

STDMETHODIMP CVCCLCompilerOptimizationPage::put_EnableIntrinsicFunctions(enumEnableIntrinsicFunctionsBOOL bEnableIntrinsic)
{
	return SetBoolProperty(VCCLID_EnableIntrinsicFunctions, bEnableIntrinsic);
}

STDMETHODIMP CVCCLCompilerOptimizationPage::get_ImproveFloatingPointConsistency(enumImproveFloatingPointConsistencyBOOL* pbImproveFloat)
{	// (/Op, /Op-) (don't) improve floating-point consistency
	return GetEnumBoolProperty2(VCCLID_ImproveFloatingPointConsistency, (long *)pbImproveFloat);
}

STDMETHODIMP CVCCLCompilerOptimizationPage::put_ImproveFloatingPointConsistency(enumImproveFloatingPointConsistencyBOOL bImproveFloat)
{
	return SetBoolProperty(VCCLID_ImproveFloatingPointConsistency, bImproveFloat);
}

STDMETHODIMP CVCCLCompilerOptimizationPage::get_FavorSizeOrSpeed(favorSizeOrSpeedOption* poptSetting)
{	// (/Os, Ot) favor size/speed works best with /Og
	return ToolGetIntProperty(VCCLID_FavorSizeOrSpeed, &(CVCCLCompilerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerOptimizationPage::put_FavorSizeOrSpeed(favorSizeOrSpeedOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, CodeGenMax, optSetting);
	return SetIntProperty(VCCLID_FavorSizeOrSpeed, optSetting);
}

STDMETHODIMP CVCCLCompilerOptimizationPage::get_OmitFramePointers(enumOmitFramePointersBOOL* poptSetting)
{	// (/Oy, Oy-) enable/disable frame pointer omission
	return GetEnumBoolProperty2(VCCLID_OmitFramePointers, (long *)poptSetting); 
}

STDMETHODIMP CVCCLCompilerOptimizationPage::put_OmitFramePointers(enumOmitFramePointersBOOL optSetting)
{
	return SetBoolProperty(VCCLID_OmitFramePointers, optSetting);
}

STDMETHODIMP CVCCLCompilerOptimizationPage::get_EnableFiberSafeOptimizations(enumEnableFiberSafeOptimizationsBOOL* pbFiberSafeTLS)
{	// (/GT) generate fiber-safe TLS accesses
	return GetEnumBoolProperty2(VCCLID_EnableFiberSafeOptimizations, (long *)pbFiberSafeTLS); 
}

STDMETHODIMP CVCCLCompilerOptimizationPage::put_EnableFiberSafeOptimizations(enumEnableFiberSafeOptimizationsBOOL bFiberSafeTLS)
{
	return SetBoolProperty(VCCLID_EnableFiberSafeOptimizations, bFiberSafeTLS);
}

// Optimization -- X86-specific
STDMETHODIMP CVCCLCompilerOptimizationPage::get_OptimizeForProcessor(ProcessorOptimizeOption* poptSetting)
{ 
	return ToolGetIntProperty(VCCLID_OptimizeForProcessor, &(CVCCLCompilerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerOptimizationPage::put_OptimizeForProcessor(ProcessorOptimizeOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, OptProcMax, optSetting);
	return SetIntProperty(VCCLID_OptimizeForProcessor, optSetting);
}

STDMETHODIMP CVCCLCompilerOptimizationPage::get_OptimizeForWindowsApplication(enumOptimizeForWindowsApplicationBOOL* poptSetting)
{ 
	return GetEnumBoolProperty2(VCCLID_OptimizeForWindowsApplication, (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerOptimizationPage::put_OptimizeForWindowsApplication(enumOptimizeForWindowsApplicationBOOL optSetting)
{
	return SetBoolProperty(VCCLID_OptimizeForWindowsApplication, optSetting);
}

STDMETHODIMP CVCCLCompilerOptimizationPage::IsPropertyReadOnly( DISPID dispid, BOOL *fReadOnly)
{
	CHECK_POINTER_NULL(fReadOnly);
	*fReadOnly = VARIANT_FALSE;
	switch (dispid)
	{
	case VCCLID_Optimization:
	case VCCLID_GlobalOptimizations:
	case VCCLID_InlineFunctionExpansion:
	case VCCLID_EnableIntrinsicFunctions:
	case VCCLID_ImproveFloatingPointConsistency:
	case VCCLID_FavorSizeOrSpeed:
	case VCCLID_OmitFramePointers:
	case VCCLID_WholeProgramOptimization:
		{
			long SKU;
			static_cast<CVCProjectEngine*>(g_pProjectEngine)->GetSKU( &SKU );
			switch( SKU )
			{
				case VSASKUEdition_Book:
				case VSASKUEdition_Standard:
					*fReadOnly = VARIANT_TRUE;
					break;
				default:
					*fReadOnly = VARIANT_FALSE;
					break;
			}
			break;
		}
	default:
		*fReadOnly = VARIANT_FALSE;
	}

	return S_OK;
};


// Preprocessor
STDMETHODIMP CVCCLCompilerPreProcessorPage::get_PreprocessorDefinitions(BSTR* pbstrDefines)
{		// (/D[name]) defines, can have multiple
	return ToolGetStrProperty(VCCLID_PreprocessorDefinitions, &(CVCCLCompilerTool::s_optHandler), pbstrDefines, true /* local only */);
}

STDMETHODIMP CVCCLCompilerPreProcessorPage::put_PreprocessorDefinitions(BSTR bstrDefines)
{
	return SetStrProperty(VCCLID_PreprocessorDefinitions, bstrDefines);
}

STDMETHODIMP CVCCLCompilerPreProcessorPage::get_IgnoreStandardIncludePath(enumIgnoreStandardIncludePathBOOL* pbIgnoreInclPath)
{	// (/X) ignore standard include path
	return GetEnumBoolProperty2(VCCLID_IgnoreStandardIncludePath, (long *)pbIgnoreInclPath); 
}

STDMETHODIMP CVCCLCompilerPreProcessorPage::put_IgnoreStandardIncludePath(enumIgnoreStandardIncludePathBOOL bIgnoreInclPath)
{
	return SetBoolProperty(VCCLID_IgnoreStandardIncludePath, bIgnoreInclPath);
}

// Preprocessor Diagnostics
STDMETHODIMP CVCCLCompilerPreProcessorPage::get_GeneratePreprocessedFile(preprocessOption* poptSetting)
{	// see enum above (/E, /EP, /P, /EP /P)
	return ToolGetIntProperty(VCCLID_GeneratePreprocessedFile, &(CVCCLCompilerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerPreProcessorPage::put_GeneratePreprocessedFile(preprocessOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, PreprocessMax, optSetting);
	return SetIntProperty(VCCLID_GeneratePreprocessedFile, optSetting);
}

STDMETHODIMP CVCCLCompilerPreProcessorPage::get_KeepComments(enumKeepCommentsBOOL* pbKeepComments)
{	// (/C) don't strip comments, requires one of /E, /EP, /P switches
	return GetEnumBoolProperty2(VCCLID_KeepComments, (long *)pbKeepComments); 
}

STDMETHODIMP CVCCLCompilerPreProcessorPage::put_KeepComments(enumKeepCommentsBOOL bKeepComments)
{
	return SetBoolProperty(VCCLID_KeepComments, bKeepComments);
}

// Code Generation; /MP, /MPlowpri deliberately left out (since we require /FD or /Gm to be thrown)
STDMETHODIMP CVCCLCompilerCodeGenPage::get_StringPooling(enumStringPoolingBOOL* pbPool)
{	// (/GF) enable read-only string pooling
	return GetEnumBoolProperty2(VCCLID_StringPooling, (long *)pbPool, &(CVCCLCompilerTool::s_optHandler)); 
}

STDMETHODIMP CVCCLCompilerCodeGenPage::put_StringPooling(enumStringPoolingBOOL bPool)
{
	return SetBoolProperty(VCCLID_StringPooling, bPool);
}

STDMETHODIMP CVCCLCompilerCodeGenPage::get_MinimalRebuild(enumMinimalRebuildBOOL* pbMinimalRebuild)
{	// (/Gm, /Gm-) enable/disable minimal rebuild, /Gm requires /ZI or /Zi
	return GetEnumBoolProperty2(VCCLID_MinimalRebuild, (long *)pbMinimalRebuild); 
}

STDMETHODIMP CVCCLCompilerCodeGenPage::put_MinimalRebuild(enumMinimalRebuildBOOL bMinimalRebuild)
{
	return SetBoolProperty(VCCLID_MinimalRebuild, bMinimalRebuild);
}

STDMETHODIMP CVCCLCompilerCodeGenPage::get_ExceptionHandling(enumExceptionHandlingBOOL* poptSetting)
{	// /EHsc
	return GetEnumBoolProperty2(VCCLID_ExceptionHandling, (long *)poptSetting); 
}

STDMETHODIMP CVCCLCompilerCodeGenPage::put_ExceptionHandling(enumExceptionHandlingBOOL optSetting)
{
	return SetBoolProperty(VCCLID_ExceptionHandling, optSetting);
}

STDMETHODIMP CVCCLCompilerCodeGenPage::get_BasicRuntimeChecks(basicRuntimeCheckOption* poptSetting)
{ 
	return GetIntProperty(VCCLID_BasicRuntimeChecks, (long *)poptSetting); 
}

STDMETHODIMP CVCCLCompilerCodeGenPage::put_BasicRuntimeChecks(basicRuntimeCheckOption optSetting)
{
	return SetIntProperty(VCCLID_BasicRuntimeChecks, optSetting);
}

STDMETHODIMP CVCCLCompilerCodeGenPage::get_SmallerTypeCheck(enumSmallerTypeCheckBOOL* pbSmallerType)
{ 
	return GetEnumBoolProperty2(VCCLID_SmallerTypeCheck, (long *)pbSmallerType);
}

STDMETHODIMP CVCCLCompilerCodeGenPage::put_SmallerTypeCheck(enumSmallerTypeCheckBOOL bSmallerType)
{
	return SetBoolProperty(VCCLID_SmallerTypeCheck, bSmallerType);
}

STDMETHODIMP CVCCLCompilerCodeGenPage::get_RuntimeLibrary(runtimeLibraryOption* poptSetting)
{ 
	return ToolGetIntProperty(VCCLID_RuntimeLibrary, &(CVCCLCompilerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerCodeGenPage::put_RuntimeLibrary(runtimeLibraryOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, RTLibMax, optSetting);
	return SetIntProperty(VCCLID_RuntimeLibrary, optSetting);
}

STDMETHODIMP CVCCLCompilerCodeGenPage::get_StructMemberAlignment(structMemberAlignOption* poptSetting)
{ 
	return ToolGetIntProperty(VCCLID_StructMemberAlignment, &(CVCCLCompilerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerCodeGenPage::put_StructMemberAlignment(structMemberAlignOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, CLStructAlignMax, optSetting);
	return SetIntProperty(VCCLID_StructMemberAlignment, optSetting);
}

STDMETHODIMP CVCCLCompilerCodeGenPage::get_BufferSecurityCheck(enumBufferSecurityCheckBOOL* pbSecure)
{	// (/GS) enable buffer overrun checks; buffer security from hackers
	return GetEnumBoolProperty2(VCCLID_BufferSecurityCheck, (long *)pbSecure); 
}

STDMETHODIMP CVCCLCompilerCodeGenPage::put_BufferSecurityCheck(enumBufferSecurityCheckBOOL bSecure)
{
	return SetBoolProperty(VCCLID_BufferSecurityCheck, bSecure);
}

STDMETHODIMP CVCCLCompilerCodeGenPage::get_EnableFunctionLevelLinking(enumEnableFunctionLevelLinkingBOOL* pbPackage)
{	// (/Gy) enable function level linking (no explicit off)
	return GetEnumBoolProperty2(VCCLID_EnableFunctionLevelLinking, (long *)pbPackage); 
}

STDMETHODIMP CVCCLCompilerCodeGenPage::put_EnableFunctionLevelLinking(enumEnableFunctionLevelLinkingBOOL bPackage)
{
	return SetBoolProperty(VCCLID_EnableFunctionLevelLinking, bPackage);
}

void CVCCLCompilerCodeGenPage::GetBaseDefault(long id, CComVariant& varValue)
{
	if (id == VCCLID_BufferSecurityCheck)
	{
		VARIANT_BOOL bVal;
		CVCCLCompilerTool::s_optHandler.GetDefaultValue( id, &bVal, m_pContainer );
		varValue = bVal;
	}
	else
		CBase::GetBaseDefault(id, varValue);
}

// Language
STDMETHODIMP CVCCLCompilerLanguagePage::get_DisableLanguageExtensions(enumDisableLanguageExtensionsBOOL* pbDisableExtensions)
{	// (/Za, /Ze) disable/enable language extensions (/Ze default)
	return GetEnumBoolProperty2(VCCLID_DisableLanguageExtensions, (long *)pbDisableExtensions);
}

STDMETHODIMP CVCCLCompilerLanguagePage::put_DisableLanguageExtensions(enumDisableLanguageExtensionsBOOL bDisableExtensions)
{
	return SetBoolProperty(VCCLID_DisableLanguageExtensions, bDisableExtensions);
}

STDMETHODIMP CVCCLCompilerLanguagePage::get_DefaultCharIsUnsigned(enumDefaultCharIsUnsignedBOOL* pbIsUnsigned)
{	// (/J) default char type is unsigned
	return GetEnumBoolProperty2(VCCLID_DefaultCharIsUnsigned, (long *)pbIsUnsigned);
}

STDMETHODIMP CVCCLCompilerLanguagePage::put_DefaultCharIsUnsigned(enumDefaultCharIsUnsignedBOOL bIsUnsigned)
{
	return SetBoolProperty(VCCLID_DefaultCharIsUnsigned, bIsUnsigned);
}

STDMETHODIMP CVCCLCompilerLanguagePage::get_TreatWChar_tAsBuiltInType(enumTreatWChar_tAsBuiltInTypeBOOL* pbBuiltInType)
{	// (/Zc:wchar_t) treat wchar_t as built-in type
	return GetEnumBoolProperty2(VCCLID_TreatWChar_tAsBuiltInType, (long *)pbBuiltInType);
}

STDMETHODIMP CVCCLCompilerLanguagePage::put_TreatWChar_tAsBuiltInType(enumTreatWChar_tAsBuiltInTypeBOOL bBuiltInType)
{
	return SetBoolProperty(VCCLID_TreatWChar_tAsBuiltInType, bBuiltInType);
}

STDMETHODIMP CVCCLCompilerLanguagePage::get_ForceConformanceInForLoopScope(enumForceConformanceInForLoopScopeBOOL* pbConform)
{	// (/Zc:forScope) check for conformance of vars for 'for' scope
	return GetEnumBoolProperty2(VCCLID_ForceConformanceInForLoopScope, (long *)pbConform);
}

STDMETHODIMP CVCCLCompilerLanguagePage::put_ForceConformanceInForLoopScope(enumForceConformanceInForLoopScopeBOOL bConform)
{
	return SetBoolProperty(VCCLID_ForceConformanceInForLoopScope, bConform);
}

STDMETHODIMP CVCCLCompilerLanguagePage::get_RuntimeTypeInfo(enumRTTIBOOL* pbRTTI)
{	// (/GR, /GR-) enable/disable C++ RTTI
	return GetEnumBoolProperty2(VCCLID_RuntimeTypeInfo, (long *)pbRTTI); 
}

STDMETHODIMP CVCCLCompilerLanguagePage::put_RuntimeTypeInfo(enumRTTIBOOL bRTTI)
{
	return SetBoolProperty(VCCLID_RuntimeTypeInfo, bRTTI);
}

// Precompiled Headers
STDMETHODIMP CVCCLCompilerPCHPage::get_UsePrecompiledHeader(pchOption* poptSetting)
{	// see enum above (/Yc, /YX, /Yu)
	return ToolGetIntProperty(VCCLID_UsePrecompiledHeader, &(CVCCLCompilerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerPCHPage::put_UsePrecompiledHeader(pchOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, UsePCHMax, optSetting);
	return SetIntProperty(VCCLID_UsePrecompiledHeader, optSetting);
}

STDMETHODIMP CVCCLCompilerPCHPage::get_PrecompiledHeaderThrough(BSTR* pbstrFile)
{	// optional argument to UsePrecompiledHeader property specifying file to use to generate/use PCH
	return ToolGetStrProperty(VCCLID_PrecompiledHeaderThrough, &(CVCCLCompilerTool::s_optHandler), pbstrFile);
}

STDMETHODIMP CVCCLCompilerPCHPage::put_PrecompiledHeaderThrough(BSTR bstrFile)
{
	return SetStrProperty(VCCLID_PrecompiledHeaderThrough, bstrFile);
}

STDMETHODIMP CVCCLCompilerPCHPage::get_PrecompiledHeaderFile(BSTR* pbstrPCH)
{	// (/Fp[name]) name the precompiled header file, can be directory location or leave off .pch extension
	return ToolGetStrProperty(VCCLID_PrecompiledHeaderFile, &(CVCCLCompilerTool::s_optHandler), pbstrPCH);
}

STDMETHODIMP CVCCLCompilerPCHPage::put_PrecompiledHeaderFile(BSTR bstrPCH)
{
	return SetStrProperty(VCCLID_PrecompiledHeaderFile, bstrPCH);
}

void CVCCLCompilerPCHPage::GetBaseDefault(long id, CComVariant& varValue)
{
	CComBSTR bstrVal;
	switch (id)
	{
	case VCCLID_PrecompiledHeaderThrough:
	case VCCLID_PrecompiledHeaderFile:
		CVCCLCompilerTool::s_optHandler.GetDefaultValue( id, &bstrVal, m_pContainer );
		break;
	default:
		CBase::GetBaseDefault(id, varValue);
		return;
	}

	varValue = bstrVal;
}

// Output Files
STDMETHODIMP CVCCLCompilerOutputPage::get_ExpandAttributedSource(enumExpandAttributedSourceBOOL* pbExpandAttributedSource)
{ 	// (/Fx) listing with attributed code expanded into source file
	return GetEnumBoolProperty2(VCCLID_ExpandAttributedSource, (long *)pbExpandAttributedSource);
}

STDMETHODIMP CVCCLCompilerOutputPage::put_ExpandAttributedSource(enumExpandAttributedSourceBOOL bExpandAttributedSource)
{
	return SetBoolProperty(VCCLID_ExpandAttributedSource, bExpandAttributedSource);
}

STDMETHODIMP CVCCLCompilerOutputPage::get_AssemblerOutput(asmListingOption* poptSetting)
{	// see enum above (/FA, /FAc, /FAs, /FAcs)
	return ToolGetIntProperty(VCCLID_AssemblerOutput, &(CVCCLCompilerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerOutputPage::put_AssemblerOutput(asmListingOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, AsmListMax, optSetting);
	return SetIntProperty(VCCLID_AssemblerOutput, optSetting);
}

STDMETHODIMP CVCCLCompilerOutputPage::get_AssemblerListingLocation(BSTR* pbstrName)
{	// (/Fa[name]) specify directory and/or name for listing file from AssemblerOutput property
	return ToolGetStrProperty(VCCLID_AssemblerListingLocation, &(CVCCLCompilerTool::s_optHandler), pbstrName);
}

STDMETHODIMP CVCCLCompilerOutputPage::put_AssemblerListingLocation(BSTR bstrName)
{
	return SetStrProperty(VCCLID_AssemblerListingLocation, bstrName);
}

STDMETHODIMP CVCCLCompilerOutputPage::get_ObjectFile(BSTR* pbstrName)
{	// (/Fo[name]) name obj file
	return ToolGetStrProperty(VCCLID_ObjectFile, &(CVCCLCompilerTool::s_optHandler), pbstrName);
}

STDMETHODIMP CVCCLCompilerOutputPage::put_ObjectFile(BSTR bstrName)
{
	return SetStrProperty(VCCLID_ObjectFile, bstrName);
}

STDMETHODIMP CVCCLCompilerOutputPage::get_ProgramDataBaseFileName(BSTR* pbstrName)
{	// (/Fd[file]) name PDB file.  can be directory name
	return ToolGetStrProperty(VCCLID_ProgramDataBaseFileName, &(CVCCLCompilerTool::s_optHandler), pbstrName);
}

STDMETHODIMP CVCCLCompilerOutputPage::put_ProgramDataBaseFileName(BSTR bstrName)
{
	CStringW strName = bstrName;
	strName.TrimLeft();
	if (strName.IsEmpty())	// don't want user setting this to blank or NULL
		return S_FALSE;
	else
		return SetStrProperty(VCCLID_ProgramDataBaseFileName, bstrName);
}

void CVCCLCompilerOutputPage::GetBaseDefault(long id, CComVariant& varValue)
{
	CComBSTR bstrVal;
	switch (id)
	{
	case VCCLID_AssemblerListingLocation:
	case VCCLID_ObjectFile:
	case VCCLID_ProgramDataBaseFileName:
		CVCCLCompilerTool::s_optHandler.GetDefaultValue( id, &bstrVal, m_pContainer );
		break;
	default:
		CBase::GetBaseDefault(id, varValue);
		return;
	}

	varValue = bstrVal;
}

// Browse info helpers
STDMETHODIMP CVCCLCompilerBrowsePage::get_BrowseInformation(browseInfoOption* poptSetting)
{	// see enum above, (/FR[name], /Fr[name])
	return ToolGetIntProperty(VCCLID_BrowseInformation, &(CVCCLCompilerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerBrowsePage::put_BrowseInformation(browseInfoOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, BrowseMax, optSetting);
	return SetIntProperty(VCCLID_BrowseInformation, optSetting);
}

STDMETHODIMP CVCCLCompilerBrowsePage::get_BrowseInformationFile(BSTR* pbstrFile)
{		// optional name to go with BrowseInformation property
	return ToolGetStrProperty(VCCLID_BrowseInformationFile, &(CVCCLCompilerTool::s_optHandler), pbstrFile);
}

STDMETHODIMP CVCCLCompilerBrowsePage::put_BrowseInformationFile(BSTR bstrFile)
{
	return SetStrProperty(VCCLID_BrowseInformationFile, bstrFile);
}

void CVCCLCompilerBrowsePage::GetBaseDefault(long id, CComVariant& varValue)
{
	if (id == VCCLID_BrowseInformationFile)
	{
		CComBSTR bstrVal;
		CVCCLCompilerTool::s_optHandler.GetDefaultValue( VCCLID_BrowseInformationFile, &bstrVal, m_pContainer );
		varValue = bstrVal;
	}
	else
		CBase::GetBaseDefault(id, varValue);
}

// Miscellaneous
STDMETHODIMP CVCCLCompilerGeneralPage::get_WarningLevel(warningLevelOption* poptSetting)
{	// see enum above (/W0 - /W4)
	return ToolGetIntProperty(VCCLID_WarningLevel, &(CVCCLCompilerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerGeneralPage::put_WarningLevel(warningLevelOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, CLWarnMax, optSetting);
	return SetIntProperty(VCCLID_WarningLevel, optSetting);
}

STDMETHODIMP CVCCLCompilerGeneralPage::get_WarnAsError(enumWarnAsErrorBOOL* pbWarnAsError)
{	// (/WX, /WX-) treat warnings as errors
	return GetEnumBoolProperty2(VCCLID_WarnAsError, (long *)pbWarnAsError);
}

STDMETHODIMP CVCCLCompilerGeneralPage::put_WarnAsError(enumWarnAsErrorBOOL bWarnAsError)
{
	return SetBoolProperty(VCCLID_WarnAsError, bWarnAsError);
}

STDMETHODIMP CVCCLCompilerGeneralPage::get_SuppressStartupBanner(enumSuppressStartupBannerBOOL* pbNoLogo)
{	// (/nologo, /nologo-) enable/disable suppression of copyright message
	return GetEnumBoolProperty2(VCCLID_SuppressStartupBanner, (long *)pbNoLogo);
}

STDMETHODIMP CVCCLCompilerGeneralPage::put_SuppressStartupBanner(enumSuppressStartupBannerBOOL bNoLogo)
{
	return SetBoolProperty(VCCLID_SuppressStartupBanner, bNoLogo);
}

STDMETHODIMP CVCCLCompilerGeneralPage::get_Detect64BitPortabilityProblems(enumDetect64BitPortabilityProblemsBOOL* pbDetect64BitPortabilityProblems)
{	// (/Wp64) detect 64 bit portability problems
	return GetEnumBoolProperty2(VCCLID_Detect64BitPortabilityProblems, (long *)pbDetect64BitPortabilityProblems);
}

STDMETHODIMP CVCCLCompilerGeneralPage::put_Detect64BitPortabilityProblems(enumDetect64BitPortabilityProblemsBOOL bDetect64BitPortabilityProblems)
{
	return SetBoolProperty(VCCLID_Detect64BitPortabilityProblems, bDetect64BitPortabilityProblems);
}

// Miscellaneous - X86 Specific Functionality
STDMETHODIMP CVCCLCompilerGeneralPage::get_CompileAsManaged(compileAsManagedOptions* poptSetting)
{ 
	return GetIntProperty(VCCLID_CompileAsManaged, (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerGeneralPage::put_CompileAsManaged(compileAsManagedOptions optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, ComRTMax, optSetting);
	return SetIntProperty(VCCLID_CompileAsManaged, optSetting);
}

STDMETHODIMP CVCCLCompilerGeneralPage::get_DebugInformationFormat(debugOption* poptSetting)
{	// see enum above (/Z7, Zd, /ZI, /Zi)
	return ToolGetIntProperty(VCCLID_DebugInformationFormat, &(CVCCLCompilerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerGeneralPage::put_DebugInformationFormat(debugOption optSetting)
{
	return SetIntProperty(VCCLID_DebugInformationFormat, optSetting);
}

STDMETHODIMP CVCCLCompilerGeneralPage::get_AdditionalIncludeDirectories(BSTR* pbstrIncludePath)
{	// (/I![path]) directory to add to include path, may have multiple
	return ToolGetStrProperty(VCCLID_AdditionalIncludeDirectories, &(CVCCLCompilerTool::s_optHandler), pbstrIncludePath, true /* local only */);
}

STDMETHODIMP CVCCLCompilerGeneralPage::put_AdditionalIncludeDirectories(BSTR bstrIncludePath)
{
	return SetStrProperty(VCCLID_AdditionalIncludeDirectories, bstrIncludePath);
}

STDMETHODIMP CVCCLCompilerGeneralPage::get_AdditionalUsingDirectories(BSTR* pbstrIncludePath)
{	// (/AI![path]) directory to add to LIBPATH path, may have multiple
	return ToolGetStrProperty(VCCLID_AdditionalUsingDirectories, &(CVCCLCompilerTool::s_optHandler), pbstrIncludePath, true /* local only */);
}

STDMETHODIMP CVCCLCompilerGeneralPage::put_AdditionalUsingDirectories(BSTR bstrIncludePath)
{
	return SetStrProperty(VCCLID_AdditionalUsingDirectories, bstrIncludePath);
}

void CVCCLCompilerGeneralPage::GetBaseDefault(long id, CComVariant& varValue)
{
	if (id == VCCLID_SuppressStartupBanner)
	{
		VARIANT_BOOL bVal;
		CVCCLCompilerTool::s_optHandler.GetDefaultValue( id, &bVal );
		varValue = bVal;
	}
	else
		CBase::GetBaseDefault(id, varValue);
}

// Advanced
STDMETHODIMP CVCCLCompilerAdvancedPage::get_CallingConvention(callingConventionOption* poptSetting)
{ 
	return ToolGetIntProperty(VCCLID_CallingConvention, &(CVCCLCompilerTool::s_optHandler), (long *)poptSetting);
}

STDMETHODIMP CVCCLCompilerAdvancedPage::put_CallingConvention(callingConventionOption optSetting)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, CallConvMax, optSetting);
	return SetIntProperty(VCCLID_CallingConvention, optSetting);
}

STDMETHODIMP CVCCLCompilerAdvancedPage::get_CompileAs(CompileAsOptions* pcompileAs)
{	// see enum above (/TC, /TP)
	return ToolGetIntProperty(VCCLID_CompileAs, &(CVCCLCompilerTool::s_optHandler), (long *)pcompileAs);
}

STDMETHODIMP CVCCLCompilerAdvancedPage::put_CompileAs(CompileAsOptions compileAs)
{
	CHECK_BOUNDS(INHERIT_PROP_VALUE, CompileAsMax, compileAs);
	return SetIntProperty(VCCLID_CompileAs, compileAs);
}

STDMETHODIMP CVCCLCompilerAdvancedPage::get_DisableSpecificWarnings(BSTR* pbstrDisableSpecificWarnings)
{	// (/wd<num>) disable specific warnings; multi-prop
	return ToolGetStrProperty(VCCLID_DisableSpecificWarnings, &(CVCCLCompilerTool::s_optHandler), pbstrDisableSpecificWarnings, true /* local only */);
}

STDMETHODIMP CVCCLCompilerAdvancedPage::put_DisableSpecificWarnings(BSTR bstrDisableSpecificWarnings)
{
	return SetStrProperty(VCCLID_DisableSpecificWarnings, bstrDisableSpecificWarnings);
}

STDMETHODIMP CVCCLCompilerAdvancedPage::get_ForcedIncludeFiles(BSTR* pbstrName)
{	// (/FI![name]) name forced include file, can have multiple
	return ToolGetStrProperty(VCCLID_ForcedIncludeFiles, &(CVCCLCompilerTool::s_optHandler), pbstrName, true /* local only */);
}

STDMETHODIMP CVCCLCompilerAdvancedPage::put_ForcedIncludeFiles(BSTR bstrName)
{
	return SetStrProperty(VCCLID_ForcedIncludeFiles, bstrName);
}

STDMETHODIMP CVCCLCompilerAdvancedPage::get_ForcedUsingFiles(BSTR* pbstrName)
{	// (/FU![name]) name forced #using file, can have multiple
	return ToolGetStrProperty(VCCLID_ForcedUsingFiles, &(CVCCLCompilerTool::s_optHandler), pbstrName, true /* local only */);
}

STDMETHODIMP CVCCLCompilerAdvancedPage::put_ForcedUsingFiles(BSTR bstrName)
{
	return SetStrProperty(VCCLID_ForcedUsingFiles, bstrName);
}

STDMETHODIMP CVCCLCompilerAdvancedPage::get_ShowIncludes(enumShowIncludesBOOL* pbShowInc)
{	// (/showIncludes)
	return GetEnumBoolProperty2(VCCLID_ShowIncludes, (long *)pbShowInc); 
}

STDMETHODIMP CVCCLCompilerAdvancedPage::put_ShowIncludes(enumShowIncludesBOOL bShowInc)
{
	return SetBoolProperty(VCCLID_ShowIncludes, bShowInc);
}

STDMETHODIMP CVCCLCompilerAdvancedPage::get_UndefinePreprocessorDefinitions(BSTR* pbstrUndefines)
{	// (/U[name]) undefine predefined macro, can have multiple
	return ToolGetStrProperty(VCCLID_UndefinePreprocessorDefinitions, &(CVCCLCompilerTool::s_optHandler), pbstrUndefines, true /* local only */);
}

STDMETHODIMP CVCCLCompilerAdvancedPage::put_UndefinePreprocessorDefinitions(BSTR bstrUndefines)	
{
	return SetStrProperty(VCCLID_UndefinePreprocessorDefinitions, bstrUndefines);
}

STDMETHODIMP CVCCLCompilerAdvancedPage::get_UndefineAllPreprocessorDefinitions(enumUndefineAllPreprocessorDefinitionsBOOL* pbPredefinedMacros)
{	// (/u) undefine all predefined macros 
	return GetEnumBoolProperty2(VCCLID_UndefineAllPreprocessorDefinitions, (long *)pbPredefinedMacros);
}

STDMETHODIMP CVCCLCompilerAdvancedPage::put_UndefineAllPreprocessorDefinitions(enumUndefineAllPreprocessorDefinitionsBOOL bPredefinedMacros)
{
	return SetBoolProperty(VCCLID_UndefineAllPreprocessorDefinitions, bPredefinedMacros);
}
