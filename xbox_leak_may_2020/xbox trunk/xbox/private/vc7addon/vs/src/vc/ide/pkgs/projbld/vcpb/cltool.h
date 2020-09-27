// CLCompilerTool.h: Definition of the CCLCompilerTool class
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "vctool.h"
#include "CLToolBase.h"
#include "CmdLines.h"

// classes in this header
class CVCCLCompilerTool;
class CCLCompilerToolOptionHandler;

class CCLCompilerToolOptionHandler : public COptionHandlerBase
{
public:
	CCLCompilerToolOptionHandler() : m_bNotifiedNoOptimize(FALSE) {}
	DECLARE_OPTION_TABLE();

public:
	// default value handlers
	// string props
	virtual void GetDefaultValue( long id, BSTR *bstrVal, IVCPropertyContainer *pPropCnt = NULL );
	// integer props
	virtual void GetDefaultValue( long id, long *iVal, IVCPropertyContainer *pPropCnt = NULL );
	// boolean props
	virtual void GetDefaultValue( long id, VARIANT_BOOL *bVal, IVCPropertyContainer *pPropCnt = NULL );

public:
	BOOL GetPCHName(IVCPropertyContainer* pPropContainer, BSTR* pbstrPCH, BOOL bCheckForExistence = TRUE);
	BOOL GetPCHHeaderName(IVCPropertyContainer* pPropContainer, long nID, CComBSTR& rbstrPCH, 
		BOOL bCheckForExistence = TRUE);
	CompileAsOptions GetDefaultCompileAs(IVCPropertyContainer* pPropContainer, BOOL bCheckExplicit = TRUE);
	void InitializeFlags();

protected:
	virtual BOOL SetEvenIfDefault(VARIANT *pVarDefault, long idOption);
	virtual void EvaluateSpecialEnumAtIndex(int nVal, LPCOLESTR szOption, long idOption, 
		IVCPropertyContainer* pPropContainer, BOOL bIncludeSlash, CStringW& rstrSwitch);
	virtual BOOL SynthesizeOptionIfNeeded(IVCPropertyContainer* pPropContainer, long idOption);
	virtual BOOL SynthesizeOption(IVCPropertyContainer* pPropContainer, long idOption, CComVariant& rvar);
	virtual void FormatScriptItem(const wchar_t* szOptionName, long idOption, long idOptionPage, CStringW& rstrOption);
	virtual BOOL OverrideOptionSet(IVCPropertyContainer* pPropContainer, long idOption);

private:
	BOOL m_bNotifiedNoOptimize;
};

/////////////////////////////////////////////////////////////////////////////
// CVCCLCompilerTool

class CVCCLCompilerTool : 
	public IDispatchImpl<VCCLCompilerTool, &IID_VCCLCompilerTool, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public CComObjectRoot,
	public CVCToolImpl
{
public:
	CVCCLCompilerTool() 
	{
		m_nLowKey = VCCLCOMPILERTOOL_MIN_DISPID;
		m_nHighKey = VCCLCOMPILERTOOL_MAX_DISPID;
		m_nDirtyKey = VCCLID_CmdLineOptionsDirty;
		m_nDirtyOutputsKey = VCCLID_OutputsDirty;
	}
	static HRESULT CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool);

BEGIN_COM_MAP(CVCCLCompilerTool)
	COM_INTERFACE_ENTRY(VCCLCompilerTool)
	COM_INTERFACE_ENTRY(IVCToolImpl)
 	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(CVCCLCompilerTool) 

// VCCLCompilerTool
public:
	// General
	STDMETHOD(get_AdditionalOptions)(BSTR* pbstrAdditionalOptions);	// anything we don't know what to do with, given verbatim, in same order as entered, parent contributions first, goes after all other options
	STDMETHOD(put_AdditionalOptions)(BSTR bstrAdditionalOptions);
	STDMETHOD(get_ToolName)(BSTR* pbstrToolName);	// friendly name of tool, e.g., "C/C++ Compiler Tool"
	STDMETHOD(get_AdditionalIncludeDirectories)(BSTR* pbstrIncludePath);	// (/I![path]) directory to add to include path, may have multiple
	STDMETHOD(put_AdditionalIncludeDirectories)(BSTR bstrIncludePath);
	STDMETHOD(get_AdditionalUsingDirectories)(BSTR* pbstrIncludePath);	// (/AI![path]) directory to add to LIBPATH path, may have multiple
	STDMETHOD(put_AdditionalUsingDirectories)(BSTR bstrIncludePath);
	STDMETHOD(get_DebugInformationFormat)(debugOption* poptSetting);	// see debugOption enum (/Z7, Zd, /ZI, /Zi)
	STDMETHOD(put_DebugInformationFormat)(debugOption optSetting);
	STDMETHOD(get_CompileAsManaged)(compileAsManagedOptions* poptSetting);	// (/clr[:noAssembly])
	STDMETHOD(put_CompileAsManaged)(compileAsManagedOptions optSetting);
	STDMETHOD(get_SuppressStartupBanner)(VARIANT_BOOL* pbNoLogo);	// (/nologo, /nologo-) enable/disable suppression of copyright message
	STDMETHOD(put_SuppressStartupBanner)(VARIANT_BOOL bNoLogo);
	STDMETHOD(get_WarningLevel)(warningLevelOption* poptSetting);	// see warningLevelOption enum (/W0 - /W4)
	STDMETHOD(put_WarningLevel)(warningLevelOption optSetting);
	STDMETHOD(get_Detect64BitPortabilityProblems)(VARIANT_BOOL* pbDetect64BitPortabilityProblems);	// (/Wp64) detect 64 bit portability problems
	STDMETHOD(put_Detect64BitPortabilityProblems)(VARIANT_BOOL bDetect64BitPortabilityProblems);
	STDMETHOD(get_WarnAsError)(VARIANT_BOOL* pbWarnAsError);	// (/WX, /WX-) treat warnings as errors
	STDMETHOD(put_WarnAsError)(VARIANT_BOOL bWarnAsError);
	// Optimization
	STDMETHOD(get_Optimization)(optimizeOption* poptSetting);	// see enum above (/O1, /O2, /Od, /Ox)
	STDMETHOD(put_Optimization)(optimizeOption optSetting);
	STDMETHOD(get_GlobalOptimizations)(VARIANT_BOOL* pbGlobalOpt);	// (/Og, /Og-) enable/disable global optimizations incompatible with all RTC options, GZ, ZI, EEf
	STDMETHOD(put_GlobalOptimizations)(VARIANT_BOOL bGlobalOpt);
	STDMETHOD(get_InlineFunctionExpansion)(inlineExpansionOption* poptSetting);	// see enum above (/Ob0, /Ob1, /Ob2)
	STDMETHOD(put_InlineFunctionExpansion)(inlineExpansionOption optSetting);
	STDMETHOD(get_EnableIntrinsicFunctions)(VARIANT_BOOL* pbEnableIntrinsic);	// (/Oi, /Oi-) enable/disable intrinsic functions works best with /Og
	STDMETHOD(put_EnableIntrinsicFunctions)(VARIANT_BOOL bEnableIntrinsic);
	STDMETHOD(get_ImproveFloatingPointConsistency)(VARIANT_BOOL* pbImproveFloat);	// (/Op, /Op-) (don't) improve floating-point consistency
	STDMETHOD(put_ImproveFloatingPointConsistency)(VARIANT_BOOL bImproveFloat);
	STDMETHOD(get_FavorSizeOrSpeed)(favorSizeOrSpeedOption* poptSetting);	// (/Os, Ot) favor size/speed works best with /Og
	STDMETHOD(put_FavorSizeOrSpeed)(favorSizeOrSpeedOption optSetting);
	STDMETHOD(get_OmitFramePointers)(VARIANT_BOOL* poptSetting);	// (/Oy, Oy-) enable/disable frame pointer omission
	STDMETHOD(put_OmitFramePointers)(VARIANT_BOOL optSetting);
	STDMETHOD(get_EnableFiberSafeOptimizations)(VARIANT_BOOL* pbFiberSafeTLS);	// (/GT) generate fiber-safe TLS accesses
	STDMETHOD(put_EnableFiberSafeOptimizations)(VARIANT_BOOL bFiberSafeTLS);
	STDMETHOD(get_WholeProgramOptimization)(VARIANT_BOOL* pbWholeProgOp);	// (/GL) Perform whole program optimization during the link phase
	STDMETHOD(put_WholeProgramOptimization)(VARIANT_BOOL bWholeProgOp);
	STDMETHOD(get_OptimizeForProcessor)(ProcessorOptimizeOption* poptSetting);
	STDMETHOD(put_OptimizeForProcessor)(ProcessorOptimizeOption optSetting);
	STDMETHOD(get_OptimizeForWindowsApplication)(VARIANT_BOOL* poptSetting);
	STDMETHOD(put_OptimizeForWindowsApplication)(VARIANT_BOOL optSetting);
	// Preprocessor
	STDMETHOD(get_PreprocessorDefinitions)(BSTR* pbstrDefines);		// (/D[name]) defines, can have multiple
	STDMETHOD(put_PreprocessorDefinitions)(BSTR bstrDefines);
	STDMETHOD(get_IgnoreStandardIncludePath)(VARIANT_BOOL* pbIgnoreInclPath);	// (/X) ignore standard include path
	STDMETHOD(put_IgnoreStandardIncludePath)(VARIANT_BOOL bIgnoreInclPath);
	STDMETHOD(get_GeneratePreprocessedFile)(preprocessOption* poptSetting);	// see enum above (/P, /EP /P)
	STDMETHOD(put_GeneratePreprocessedFile)(preprocessOption optSetting);
	STDMETHOD(get_KeepComments)(VARIANT_BOOL* pbKeepComments);	// (/C) don't strip comments, requires one of /E, /EP, /P switches
	STDMETHOD(put_KeepComments)(VARIANT_BOOL bKeepComments);
	// Code Generation; /MP, /MPlowpri deliberately left out (since we require /FD or /Gm to be thrown)
	STDMETHOD(get_StringPooling)(VARIANT_BOOL* pbPool);	// (/GF) enable read-only string pooling
	STDMETHOD(put_StringPooling)(VARIANT_BOOL bPool);
	STDMETHOD(get_MinimalRebuild)(VARIANT_BOOL* pbMinimalRebuild);	// (/Gm, /Gm-) enable/disable minimal rebuild, /Gm requires /ZI or /Zi
	STDMETHOD(put_MinimalRebuild)(VARIANT_BOOL bMinimalRebuild);
	STDMETHOD(get_ExceptionHandling)(VARIANT_BOOL* poptSetting);	// /EHsc
	STDMETHOD(put_ExceptionHandling)(VARIANT_BOOL optSetting);
	STDMETHOD(get_SmallerTypeCheck)(VARIANT_BOOL* pbSmallerType);
	STDMETHOD(put_SmallerTypeCheck)(VARIANT_BOOL bSmallerType);
	STDMETHOD(get_BasicRuntimeChecks)(basicRuntimeCheckOption* poptSetting);
	STDMETHOD(put_BasicRuntimeChecks)(basicRuntimeCheckOption optSetting);
	STDMETHOD(get_RuntimeLibrary)(runtimeLibraryOption* poptSetting);
	STDMETHOD(put_RuntimeLibrary)(runtimeLibraryOption optSetting);
	STDMETHOD(get_StructMemberAlignment)(structMemberAlignOption* poptSetting);
	STDMETHOD(put_StructMemberAlignment)(structMemberAlignOption optSetting);
	STDMETHOD(get_BufferSecurityCheck)(VARIANT_BOOL* bSecure);	// (/GS) enable buffer overrun checks; buffer security from hackers
	STDMETHOD(put_BufferSecurityCheck)(VARIANT_BOOL bSecure);
	STDMETHOD(get_EnableFunctionLevelLinking)(VARIANT_BOOL* pbPackage);	// (/Gy) enable function level linking (no explicit off)
	STDMETHOD(put_EnableFunctionLevelLinking)(VARIANT_BOOL bPackage);
	// Language
	STDMETHOD(get_DisableLanguageExtensions)(VARIANT_BOOL* pbDisableExtensions);	// (/Za, /Ze) disable/enable language extensions (/Ze default)
	STDMETHOD(put_DisableLanguageExtensions)(VARIANT_BOOL bDisableExtensions);
	STDMETHOD(get_DefaultCharIsUnsigned)(VARIANT_BOOL* pbIsUnsigned);	// (/J) default char type is unsigned
	STDMETHOD(put_DefaultCharIsUnsigned)(VARIANT_BOOL bIsUnsigned);
	STDMETHOD(get_TreatWChar_tAsBuiltInType)(VARIANT_BOOL* pbBuiltInType);
	STDMETHOD(put_TreatWChar_tAsBuiltInType)(VARIANT_BOOL bBuiltInType);
	STDMETHOD(get_ForceConformanceInForLoopScope)(VARIANT_BOOL* pbConform);	// (/Zc:forScope) check for conformance of vars for 'for' scope
	STDMETHOD(put_ForceConformanceInForLoopScope)(VARIANT_BOOL bConform);
	STDMETHOD(get_RuntimeTypeInfo)(VARIANT_BOOL* pbRTTI);	// (/GR) enable/disable C++ RTTI
	STDMETHOD(put_RuntimeTypeInfo)(VARIANT_BOOL bRTTI);
	// Precompiled Headers
	STDMETHOD(get_UsePrecompiledHeader)(pchOption* poptSetting);	// see enum above (/Yc, /YX, /Yu)
	STDMETHOD(put_UsePrecompiledHeader)(pchOption optSetting);
	STDMETHOD(get_PrecompiledHeaderThrough)(BSTR* pbstrFile);	// optional argument to UsePrecompiledHeader property specifying file to use to generate/use PCH
	STDMETHOD(put_PrecompiledHeaderThrough)(BSTR bstrFile);
	STDMETHOD(get_PrecompiledHeaderFile)(BSTR* pbstrPCH);	// (/Fp[name]) name the precompiled header file, can be directory location or leave off .pch extension
	STDMETHOD(put_PrecompiledHeaderFile)(BSTR bstrPCH);
	// Output Files
	STDMETHOD(get_ExpandAttributedSource)(VARIANT_BOOL* pbExpandAttributedSource);	// (/Fx) listing with attributed code expanded into source file
	STDMETHOD(put_ExpandAttributedSource)(VARIANT_BOOL bExpandAttributedSource);
	STDMETHOD(get_AssemblerOutput)(asmListingOption* poptSetting);	// see enum above (/FA, /FAc, /FAs, /FAcs)
	STDMETHOD(put_AssemblerOutput)(asmListingOption optSetting);
	STDMETHOD(get_AssemblerListingLocation)(BSTR* pbstrName);	// (/Fa[name]) specify directory and/or name for listing file from AssemblerOutput property
	STDMETHOD(put_AssemblerListingLocation)(BSTR bstrName);
	STDMETHOD(get_ObjectFile)(BSTR* pbstrName);	// (/Fo[name]) name obj file
	STDMETHOD(put_ObjectFile)(BSTR bstrName);
	STDMETHOD(get_ProgramDataBaseFileName)(BSTR* pbstrName);	// (/Fd[file]) name PDB file.  can be directory name
	STDMETHOD(put_ProgramDataBaseFileName)(BSTR bstrName);
	// Browse info helpers
	STDMETHOD(get_BrowseInformation)(browseInfoOption* poptSetting);	// see enum above, (/FR[name], /Fr[name])
	STDMETHOD(put_BrowseInformation)(browseInfoOption optSetting);
	STDMETHOD(get_BrowseInformationFile)(BSTR* pbstrFile);		// optional name to go with BrowseInformation property
	STDMETHOD(put_BrowseInformationFile)(BSTR bstrFile);
	// Advanced
	STDMETHOD(get_CallingConvention)(callingConventionOption* poptSetting);
	STDMETHOD(put_CallingConvention)(callingConventionOption optSetting);
	STDMETHOD(get_CompileAs)(CompileAsOptions* pcompileAs);	// see enum above (/TC, /TP)
	STDMETHOD(put_CompileAs)(CompileAsOptions compileAs);
	STDMETHOD(get_DisableSpecificWarnings)(BSTR* pbstrDisableSpecificWarnings);	// (/wd<num>) disable specific warnings; multi-prop
	STDMETHOD(put_DisableSpecificWarnings)(BSTR bstrDisableSpecificWarnings);
	STDMETHOD(get_ForcedIncludeFiles)(BSTR* pbstrName);	// (/FI![name]) name forced include file, can have multiple
	STDMETHOD(put_ForcedIncludeFiles)(BSTR bstrName);
	STDMETHOD(get_ForcedUsingFiles)(BSTR* pbstrName);	// (/FU![name]) name forced #using file, can have multiple
	STDMETHOD(put_ForcedUsingFiles)(BSTR bstrName);
	STDMETHOD(get_ShowIncludes)(VARIANT_BOOL* pbShowInc);	// (/showIncludes)
	STDMETHOD(put_ShowIncludes)(VARIANT_BOOL bShowInc);
	STDMETHOD(get_UndefinePreprocessorDefinitions)(BSTR* pbstrUndefines);	// (/U[name]) undefine predefined macro, can have multiple
	STDMETHOD(put_UndefinePreprocessorDefinitions)(BSTR bstrUndefines);
	STDMETHOD(get_UndefineAllPreprocessorDefinitions)(VARIANT_BOOL* pbPredefinedMacros);	// (/u) undefine all predefined macros 
	STDMETHOD(put_UndefineAllPreprocessorDefinitions)(VARIANT_BOOL bPredefinedMacros);
	// Automation properties
	STDMETHOD(get_ToolPath)(BSTR *pVal);
	STDMETHOD(get_PropertyOption)(BSTR bstrProp, long dispidProp, BSTR *pVal);
	STDMETHOD(get_FullIncludePath)(BSTR* fullIncludePath);	// include path, including all inherited values, plus platform includes
	STDMETHOD(get_VCProjectEngine)(IDispatch** projEngine)	{ return DoGetVCProjectEngine(projEngine); }
	STDMETHOD(get_CompileOnly)(VARIANT_BOOL* pbCompileOnly);	// (/c)	compile only, no link
	STDMETHOD(put_CompileOnly)(VARIANT_BOOL bCompileOnly);

// IVCToolImpl
public:
	STDMETHOD(get_DefaultExtensions)(BSTR* pVal);
	STDMETHOD(put_DefaultExtensions)(BSTR newVal);
	STDMETHOD(GetAdditionalOptionsInternal)(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, BSTR* pbstrAdditionalOptions);
	STDMETHOD(GetBuildDescription)(IVCBuildAction* pAction, BSTR* pbstrBuildDescription);
	STDMETHOD(GetAdditionalIncludeDirectoriesInternal)(IVCPropertyContainer* pPropContainer, BSTR* pbstrIncDirs);
	STDMETHOD(get_IncludePathID)(long* pnIncludeID);
	STDMETHOD(get_ToolPathInternal)(BSTR* pbstrToolPath);
	STDMETHOD(get_Bucket)(long *pVal);
	STDMETHOD(GenerateOutput)(long type, IVCBuildActionList* plstActions, IVCBuildErrorContext* pEC);
	STDMETHOD(AffectsOutput)(long nPropID, VARIANT_BOOL* pbAffectsOutput);
	STDMETHOD(get_ToolShortName)(BSTR* pbstrToolName);
	STDMETHOD(get_ToolNameInternal)(BSTR* pbstrToolName) { return get_ToolName(pbstrToolName); }
	STDMETHOD(MatchName)(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches);
	STDMETHOD(CreateToolObject)(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject);
	STDMETHOD(PrePerformBuildActions)(bldActionTypes type, IVCBuildActionList* pActions, bldAttributes aob, IVCBuildErrorContext* pEC, actReturnStatus* pActReturn);
	STDMETHOD(HasDependencies)(IVCBuildAction* pAction, VARIANT_BOOL* pbHasDependencies);
	STDMETHOD(GetDependencies)(IVCBuildAction* pAction, IVCBuildStringCollection** ppStrings, VARIANT_BOOL* bUpToDate);
	STDMETHOD(CanScanForDependencies)(VARIANT_BOOL* pbIsScannable);
	STDMETHOD(GetCommandLinesForBuild)(IVCBuildActionList* pActions, bldAttributes attrib, IVCBuildEngine* pBldEngine, IVCBuildErrorContext* pErrorContext, IVCCommandLineList** ppCmds);
	STDMETHOD(HasVirtualLocalStorage)(IVCPropertyContainer* pPropContainer, VARIANT_BOOL* pbHasVirtualLocalStorage);
	STDMETHOD(CreatePageObject)(IUnknown **, CLSID*, IVCPropertyContainer *, IVCSettingsPage * );
	STDMETHOD(HasPrimaryOutputFromTool)(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, VARIANT_BOOL* pbHasPrimaryOutput);	// returns TRUE only if tool generates a primary output (ex: linker)
	STDMETHOD(SetPrimaryOutputForTool)(IUnknown* pItem, BSTR bstrFile);		// sets the primary output for a tool
	STDMETHOD(GetPrimaryOutputIDFromTool)(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, long* pnOutputID);	// return a value only if tool generates a primary output (ex: linker)
	STDMETHOD(GetDefaultOutputFileFormat)(IUnknown* pItem, BSTR* pbstrOutMacro);	// macro to use for generating default output file if the original default isn't unique
	STDMETHOD(GetDeploymentDependencies)(IVCBuildAction* pAction, IVCBuildStringCollection** ppDeployDepStrings);

	STDMETHOD(get_ToolDisplayIndex)(long* pIndex)
	{
		CHECK_POINTER_NULL(pIndex);
		*pIndex = TOOL_DISPLAY_INDEX_CPP;
		return S_OK;
	}

	virtual long GetPageCount()	{ return 10; }
	virtual GUID* GetPageIDs();

// helpers
public:
	virtual COptionHandlerBase* GetOptionHandler() { return &s_optHandler; }
	virtual HRESULT GetPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, CPathW& rPath);	// return a value only if tool generates a primary output (ex: linker)
	static void MakePdbAsFile(IVCPropertyContainer* pPropContainer, CComBSTR& bstrPdb);
	static void ClearResolvedPdbName(IVCPropertyContainer* pPropContainer);
	static void RefreshResolvedPdbName(IVCPropertyContainer* pPropContainer, BOOL bForce = FALSE);
	static HRESULT GetResolvedPdbName(IVCPropertyContainer* pPropContainer, BSTR* pbstrName);

protected:
	virtual BOOL UsesResponseFiles() { return TRUE; }
	CBldCmdSet* GetCommandLineMatch(CVCPtrList& cmdSet, CStringW& rstrCmd);
	virtual LPCOLESTR GetToolFriendlyName();
	void InitToolName();
	virtual long GetToolID() { return IDS_Cl; }

private:
	static BOOL CheckPathAgainstDir(IVCPropertyContainer* pPropContainer, long id, CStringW& strPdb, CComBSTR& bstrPdb);

protected:
	static GUID s_pPages[10];
	static BOOL s_bPagesInit;
	static CComBSTR s_bstrBuildDescription;
	static CComBSTR s_bstrToolName;
	static CComBSTR s_bstrExtensions;

public:
	static CCLCompilerToolOptionHandler s_optHandler;
};

// These macros are used to define enum ranges for tool switches.  If you add an enum property to the tool option
// table, please add a matching macro for it to this list.  Use of macros for range definitions helps in keeping
// maintenance of enum range checks as low as possible.
#define OptimizeMax			4
#define CodeGenMax			2
#define InlineMax			2
#define OptProcMax			2
#define PreprocessMax		2
#define EHMax				4
#define RTCheckMax			3
#define StringPoolMax		2
#define MemPtrMax			4
#define DLLStyleMax			2
#define UsePCHMax			3
#define AsmListMax			4
#define DebugInfoMax		4
#define BrowseMax			2
#define CompileAsMax		2
#define CLWarnMax			4
#define CallConvMax			2
#define CLStructAlignMax	5
#define RTLibMax			5
#define ComRTMax            2
#define BasicRTCheckMax		3