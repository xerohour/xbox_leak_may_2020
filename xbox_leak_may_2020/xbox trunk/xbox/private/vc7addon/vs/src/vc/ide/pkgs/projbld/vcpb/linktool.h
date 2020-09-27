// VCLinkerTool.h: Definition of the CVCLinkerTool class
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "vctool.h"
#include "LinkLibHelp.h"
#include "LinkToolBase.h"

// classes in this header
class CVCLinkerTool;
class CLinkerToolOptionHandler;

class CLinkerToolOptionHandler : public COptionHandlerBase
{
public:
	DECLARE_OPTION_TABLE();

public:
	// default value handlers
	// string props
	virtual void GetDefaultValue( long id, BSTR *pVal, IVCPropertyContainer *pPropCnt = NULL );
	// integer props
	virtual void GetDefaultValue( long id, long *pVal, IVCPropertyContainer *pPropCnt = NULL );
	// boolean props
	virtual void GetDefaultValue( long id, VARIANT_BOOL *pVal, IVCPropertyContainer *pPropCnt = NULL );

	virtual HRESULT GenerateCommandLine(IVCPropertyContainer* pPropContainer, BOOL bForDisplay, commandLineOptionStyle fStyle, 
		CStringW& rstrCmdLine);

protected:
	virtual BOOL SetEvenIfDefault(VARIANT *pVarDefault, long idOption);
	virtual BOOL SynthesizeOptionIfNeeded(IVCPropertyContainer* pPropContainer, long idOption);
	virtual BOOL SynthesizeOption(IVCPropertyContainer* pPropContainer, long idOption, CComVariant& rvar);
	virtual void FormatScriptItem(const wchar_t* szOptionName, long idOption, long idOptionPage, CStringW& rstrOption);
	virtual void EvaluateSpecialString(CStringW& rstrVal, BOOL bIsMultiple, LPCOLESTR szOption, long idOption,
		LPCOLESTR szOptionName, long idOptionPage, IVCPropertyContainer* pPropContainer, BOOL bIncludeSlash, 
		BOOL bForDisplay, commandLineOptionStyle fStyle, CStringW& rstrTrailing, CStringW& rstrSwitch);
	virtual BOOL OverrideOptionSet(IVCPropertyContainer* pPropContainer, long idOption);

public:
	BOOL CreateOutputName(IVCPropertyContainer* pPropContainer, BSTR *bstrVal, BOOL bCheckForExistence = TRUE);
	BOOL CreatePDBName(IVCPropertyContainer* pPropContainer, BSTR *pbstrVal, BOOL bCheckForExistence = TRUE);
};

/////////////////////////////////////////////////////////////////////////////
// CVCLinkerTool

class ATL_NO_VTABLE CVCLinkerTool : 
	public IDispatchImpl<VCLinkerTool, &IID_VCLinkerTool, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public CComObjectRoot,
	public CLinkerLibrarianHelper,
	public CVCToolImpl
{
public:
	CVCLinkerTool() 
	{
		m_nLowKey = VCLINKERTOOL_MIN_DISPID;
		m_nHighKey = VCLINKERTOOL_MAX_DISPID;
		m_nDirtyKey = VCLINKID_CmdLineOptionsDirty;
		m_nDirtyOutputsKey = VCLINKID_OutputsDirty;
	}
	static HRESULT CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool);

BEGIN_COM_MAP(CVCLinkerTool)
 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCToolImpl)
	COM_INTERFACE_ENTRY(VCLinkerTool)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(CVCLinkerTool) 

// VCLinkerTool
public:
	// general
	STDMETHOD(get_ToolName)(BSTR* pbstrToolName);	// friendly name of tool, e.g., "Linker Tool"
	STDMETHOD(get_AdditionalOptions)(BSTR* pbstrAdditionalOptions);	// anything we don't know what to do with, given verbatim, in same order as entered, parent contributions first, goes after all other options
	STDMETHOD(put_AdditionalOptions)(BSTR bstrAdditionalOptions);
	STDMETHOD(get_OutputFile)(BSTR* pbstrOut);	// (/OUT:[file]) change the output file name (default is based on 1st lib or obj name on cmd line)
	STDMETHOD(put_OutputFile)(BSTR bstrOut);
	STDMETHOD(get_ShowProgress)(linkProgressOption* poptSetting);	// see enum above (/VERBOSE)
	STDMETHOD(put_ShowProgress)(linkProgressOption optSetting);
	STDMETHOD(get_Version)(BSTR* pbstrVersion);	// (/VERSION:version) put this version number into header of created image
	STDMETHOD(put_Version)(BSTR bstrVersion);
	STDMETHOD(get_LinkIncremental)(linkIncrementalType* poptSetting);	// (/INCREMENTAL:YES, /INCREMENTAL:NO, not set)
	STDMETHOD(put_LinkIncremental)(linkIncrementalType optSetting);
	STDMETHOD(get_SuppressStartupBanner)(VARIANT_BOOL* pbNoLogo);	// (/NOLOGO) enable suppression of copyright message (no explicit off)
	STDMETHOD(put_SuppressStartupBanner)(VARIANT_BOOL bNoLogo);
	STDMETHOD(get_IgnoreImportLibrary)(VARIANT_BOOL* pbIgnoreExportLib);	// ignore export .lib
	STDMETHOD(put_IgnoreImportLibrary)(VARIANT_BOOL bIgnoreExportLib);
	STDMETHOD(get_RegisterOutput)(VARIANT_BOOL* pbRegister);	// register the primary output of the build
	STDMETHOD(put_RegisterOutput)(VARIANT_BOOL bRegister);
	STDMETHOD(get_AdditionalLibraryDirectories)(BSTR* pbstrLibPath);	// (/LIBPATH:[dir]) specify path to search for libraries on, can have multiple
	STDMETHOD(put_AdditionalLibraryDirectories)(BSTR bstrLibPath);
	STDMETHOD(get_LinkDLL)(VARIANT_BOOL* pbDLL);	// (/DLL) build a DLL as the main output
	STDMETHOD(put_LinkDLL)(VARIANT_BOOL bDLL);
	// input
	STDMETHOD(get_AdditionalDependencies)(BSTR* pbstrDependencies);	// additional inputs to the link line (comdlg32.lib, etc.)
	STDMETHOD(put_AdditionalDependencies)(BSTR bstrDependencies);
	STDMETHOD(get_IgnoreAllDefaultLibraries)(VARIANT_BOOL* pbNoDefaults);	// (/NODEFAULTLIB) ignore all default libraries
	STDMETHOD(put_IgnoreAllDefaultLibraries)(VARIANT_BOOL bNoDefaults);
	STDMETHOD(get_IgnoreDefaultLibraryNames)(BSTR* pbstrLib);	// (/NODEFAULTLIB:[name]) ignore particular default library, can have multiple
	STDMETHOD(put_IgnoreDefaultLibraryNames)(BSTR bstrLib);
	STDMETHOD(get_ModuleDefinitionFile)(BSTR* pbstrDefFile);	// (/DEF:file) use/specify module definition file
	STDMETHOD(put_ModuleDefinitionFile)(BSTR bstrDefFile);
	STDMETHOD(get_AddModuleNamesToAssembly)(BSTR* pbstrNonAssy);	// (/ASSEMBLYMODULE:file) imports a non-assembly file
	STDMETHOD(put_AddModuleNamesToAssembly)(BSTR bstrNonAssy);
	STDMETHOD(get_EmbedManagedResourceFile)(BSTR* pbstrRes);	// (/ASSEMBLYRESOURCE:file) embed an assembly resource file
	STDMETHOD(put_EmbedManagedResourceFile)(BSTR bstrRes);
	STDMETHOD(get_ForceSymbolReferences)(BSTR* pbstrSymbol);	// (/INCLUDE:[symbol]) force symbol reference, can have multiple
	STDMETHOD(put_ForceSymbolReferences)(BSTR bstrSymbol);
	STDMETHOD(get_DelayLoadDLLs)(BSTR* pbstrDLLName);	// (/DELAYLOAD:[dll_name]) delay load specified DLL, can have multiple
	STDMETHOD(put_DelayLoadDLLs)(BSTR bstrDLLName);
	// MIDL
	STDMETHOD(get_MidlCommandFile)(BSTR* pbstrMidlCmdFile);	// (/midl:<@midl cmd file>) specify response file for MIDL commands to use
	STDMETHOD(put_MidlCommandFile)(BSTR bstrMidlCmdFile);
	STDMETHOD(get_IgnoreEmbeddedIDL)(VARIANT_BOOL* pbIgnoreIDL);	// (/ignoreidl) ignore .idlsym sections of .obj files
	STDMETHOD(put_IgnoreEmbeddedIDL)(VARIANT_BOOL bIgnoreIDL);
	STDMETHOD(get_MergedIDLBaseFileName)(BSTR* pbstrIDLFile);	// (/idlout:<filename>) name intermediate IDL output file
	STDMETHOD(put_MergedIDLBaseFileName)(BSTR bstrIDLFile);
	STDMETHOD(get_TypeLibraryFile)(BSTR* pbstrTLBFile);	// (/tlbout:<filename>) name intermediate typelib output file
	STDMETHOD(put_TypeLibraryFile)(BSTR bstrTLBFile);
	STDMETHOD(get_TypeLibraryResourceID)(long* pnResID);	// (/tlbid:<id>) specify resource ID for generated .tlb file
	STDMETHOD(put_TypeLibraryResourceID)(long nResID);
	// debug
	STDMETHOD(get_GenerateDebugInformation)(VARIANT_BOOL* pbDebug);	// (/DEBUG) generate debug info
	STDMETHOD(put_GenerateDebugInformation)(VARIANT_BOOL bDebug);
	STDMETHOD(get_ProgramDatabaseFile)(BSTR* pbstrFile);	// (/PDB:file) use program database
	STDMETHOD(put_ProgramDatabaseFile)(BSTR bstrFile);
	STDMETHOD(get_StripPrivateSymbols)(BSTR* pbstrStrippedPDB);	// (/PDBSTRIPPED:file) strip private symbols from PDB
	STDMETHOD(put_StripPrivateSymbols)(BSTR bstrStrippedPDB);
	STDMETHOD(get_GenerateMapFile)(VARIANT_BOOL* pbMap);	// (/MAP[:file]) generate map file during linking
	STDMETHOD(put_GenerateMapFile)(VARIANT_BOOL bMap);
	STDMETHOD(get_MapFileName)(BSTR* pbstrMapFile);	// optional argument to GenerateMapFile property
	STDMETHOD(put_MapFileName)(BSTR bstrMapFile);
	STDMETHOD(get_MapExports)(VARIANT_BOOL* pbExports);	// (/MAPINFO:EXPORTS) include exported functions in map info
	STDMETHOD(put_MapExports)(VARIANT_BOOL bExports);
	STDMETHOD(get_MapLines)(VARIANT_BOOL* pbLines);	// (/MAPINFO:LINES) include line number info in map info
	STDMETHOD(put_MapLines)(VARIANT_BOOL bLines);
	// System
	STDMETHOD(get_SubSystem)(subSystemOption* poptSetting);	// see subSystem enum (/SUBSYSTEM)
	STDMETHOD(put_SubSystem)(subSystemOption optSetting);
	STDMETHOD(get_HeapReserveSize)(long* pnReserveSize);	// (/HEAP:reserve[,commit]) total heap allocation size in virtual memory
	STDMETHOD(put_HeapReserveSize)(long nReserveSize);
	STDMETHOD(get_HeapCommitSize)(long* pnCommitSize);	// (/HEAP:reserve[,commit]) total heap allocation size in physical memory
	STDMETHOD(put_HeapCommitSize)(long nCommitSize);
	STDMETHOD(get_StackReserveSize)(long* pnReserveSize);	// (/STACK:reserve[,commit]) total stack allocation size in virtual memory
	STDMETHOD(put_StackReserveSize)(long nReserveSize);
	STDMETHOD(get_StackCommitSize)(long* pnCommitSize);	// (/STACK:reserve[,commit]) total stack allocation size in physical memory
	STDMETHOD(put_StackCommitSize)(long nCommitSize);
	STDMETHOD(get_LargeAddressAware)(addressAwarenessType* poptSetting);	// (/LARGEADDRESSAWARE[:NO]) tells the linker the app can handle addresses greater than 2GB
	STDMETHOD(put_LargeAddressAware)(addressAwarenessType optSetting);
	STDMETHOD(get_TerminalServerAware)(termSvrAwarenessType* poptSetting);	// (/TSAWARE, /TSAWARE:NO, not set) not in docs
	STDMETHOD(put_TerminalServerAware)(termSvrAwarenessType optSetting);
	STDMETHOD(get_SwapRunFromCD)(VARIANT_BOOL* pbRun);	// swap run from the CD (/SWAPRUN:CD)
	STDMETHOD(put_SwapRunFromCD)(VARIANT_BOOL bRun);
	STDMETHOD(get_SwapRunFromNet)(VARIANT_BOOL* pbRun);	// swap run from the net (/SWAPRUN:NET)
	STDMETHOD(put_SwapRunFromNet)(VARIANT_BOOL bRun);
	// optimization
	STDMETHOD(get_OptimizeReferences)(optRefType* poptSetting);	// (/OPT:REF, /OPT:NOREF, not set) eliminate/keep functions & data never referenced
	STDMETHOD(put_OptimizeReferences)(optRefType optSetting);
	STDMETHOD(get_EnableCOMDATFolding)(optFoldingType* poptSetting);	// (/OPT:ICF, /OPT:NOICF, not set) eliminate/keep redundant COMDAT data (data folding)
	STDMETHOD(put_EnableCOMDATFolding)(optFoldingType optSetting);
	STDMETHOD(get_OptimizeForWindows98)(optWin98Type* poptSetting);	// (/OPT:WIN98, /OPT:NOWIN98, not set) 
	STDMETHOD(put_OptimizeForWindows98)(optWin98Type optSetting);
	STDMETHOD(get_FunctionOrder)(BSTR* pbstrOrder);	// (/ORDER:@[file]) place functions in order specified in file
	STDMETHOD(put_FunctionOrder)(BSTR bstrOrder);
	STDMETHOD(get_LinkTimeCodeGeneration)(VARIANT_BOOL* pbCodeGen);	// (/LTCG) enable link time code gen of objects compiled with /GL
	STDMETHOD(put_LinkTimeCodeGeneration)(VARIANT_BOOL bCodeGen);
	// advanced
	STDMETHOD(get_EntryPointSymbol)(BSTR* pbstrEntry);	// (/ENTRY:[symbol]) set entry point address for EXE or DLL incompatible with /NOENTRY
	STDMETHOD(put_EntryPointSymbol)(BSTR bstrEntry);
	STDMETHOD(get_ResourceOnlyDLL)(VARIANT_BOOL* pbNoEntry);	// (/NOENTRY) no entry point.  required for resource-only DLLs incompatible with /ENTRY
	STDMETHOD(put_ResourceOnlyDLL)(VARIANT_BOOL bNoEntry);
	STDMETHOD(get_SetChecksum)(VARIANT_BOOL* pbRelease);	// (/RELEASE) set the checksum in the header of a .exe
	STDMETHOD(put_SetChecksum)(VARIANT_BOOL bRelease);
	STDMETHOD(get_BaseAddress)(BSTR* pbstrAddress);	// (/BASE:{address| filename,key}) base address to place program at can be numeric or string
	STDMETHOD(put_BaseAddress)(BSTR bstrAddress);
	STDMETHOD(get_TurnOffAssemblyGeneration)(VARIANT_BOOL* pbNoAssy);	// (/NOASSEMBLY) cause the output file to be built without an assembly
	STDMETHOD(put_TurnOffAssemblyGeneration)(VARIANT_BOOL bNoAssy);
	STDMETHOD(get_SupportUnloadOfDelayLoadedDLL)(VARIANT_BOOL* pbDelay);	// (/DELAY:UNLOAD) use to allow explicit unloading of the DLL
	STDMETHOD(put_SupportUnloadOfDelayLoadedDLL)(VARIANT_BOOL bDelay);
	STDMETHOD(get_MergeSections)(BSTR* pbstrMerge);	// (/MERGE:from=to) merge section 'from' into section 'to'
	STDMETHOD(put_MergeSections)(BSTR bstrMerge);
	STDMETHOD(get_ImportLibrary)(BSTR* pbstrImportLib);	// (/IMPLIB:[library]) generate specified import library
	STDMETHOD(put_ImportLibrary)(BSTR bstrImportLib);
	STDMETHOD(get_TargetMachine)(machineTypeOption* poptSetting);	// (/MACHINE:type) specify target platform
	STDMETHOD(put_TargetMachine)(machineTypeOption optSetting);

	// Automation properties
	STDMETHOD(get_ToolPath)(BSTR *pVal);
	STDMETHOD(get_PropertyOption)(BSTR bstrProp, long dispidProp, BSTR *pVal) { return DoGetPropertyOption(bstrProp, dispidProp, pVal); }
	STDMETHOD(get_VCProjectEngine)(IDispatch** projEngine)	{ return DoGetVCProjectEngine(projEngine); }

// IVCToolImpl
public:
	STDMETHOD(get_DefaultExtensions)(BSTR* pVal);
	STDMETHOD(put_DefaultExtensions)(BSTR newVal);
	STDMETHOD(GetAdditionalOptionsInternal)(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, BSTR* pbstrAdditionalOptions);
	STDMETHOD(GetBuildDescription)(IVCBuildAction* pAction, BSTR* pbstrBuildDescription);
	STDMETHOD(GetAdditionalIncludeDirectoriesInternal)(IVCPropertyContainer* pPropContainer, BSTR* pbstrIncDirs);
	STDMETHOD(get_ToolPathInternal)(BSTR* pbstrToolPath);
	STDMETHOD(get_Bucket)(long *pVal);
	STDMETHOD(GenerateOutput)(long type, IVCBuildActionList* plstActions, IVCBuildErrorContext* pEC);
	STDMETHOD(AffectsOutput)(long nPropID, VARIANT_BOOL* pbAffectsOutput);
	STDMETHOD(IsTargetTool)(IVCBuildAction* pAction, VARIANT_BOOL* pbTargetTool); // tool always operates on target, not on file
	STDMETHOD(IsDeleteOnRebuildFile)(LPCOLESTR szFile, VARIANT_BOOL* pbDelOnRebuild);
	STDMETHOD(get_ToolShortName)(BSTR* pbstrToolName);
	STDMETHOD(MatchName)(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches);
	STDMETHOD(CreateToolObject)(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject);
	STDMETHOD(HasPrimaryOutputFromTool)(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, VARIANT_BOOL* pbHasPrimaryOutput);	// returns TRUE only if tool generates a primary output (ex: linker)
	STDMETHOD(GetImportLibraryFromTool)(IUnknown* pItem, BSTR* pbstrPath);	// returns TRUE only if tool generates an import library
	STDMETHOD(HasDependencies)(IVCBuildAction* pAction, VARIANT_BOOL* pbHasDependencies);
	STDMETHOD(GetDependencies)(IVCBuildAction* pAction, IVCBuildStringCollection** ppStrings, VARIANT_BOOL* pbUpToDate);
	STDMETHOD(CanScanForDependencies)(VARIANT_BOOL* pbIsScannable);
	STDMETHOD(IsSpecialConsumable)(LPCOLESTR szPath, VARIANT_BOOL* pbSpecial);
	STDMETHOD(GetCommandLinesForBuild)(IVCBuildActionList* pActions, bldAttributes attrib, IVCBuildEngine* pBldEngine, 
		IVCBuildErrorContext* pErrorContext, IVCCommandLineList** ppCmds);
	STDMETHOD(CreatePageObject)(IUnknown **, CLSID*, IVCPropertyContainer *, IVCSettingsPage * );
	STDMETHOD(GetProgramDatabaseFromTool)(IVCPropertyContainer* pContainer, VARIANT_BOOL bSchmoozeOnly, BSTR* pbstrPDB);
	STDMETHOD(GetPrimaryOutputIDFromTool)(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, long* pnOutputID);	// return a value only if tool generates a primary output (ex: linker)

	STDMETHOD(get_ToolDisplayIndex)(long* pIndex)
	{
		CHECK_POINTER_NULL(pIndex);
		*pIndex = TOOL_DISPLAY_INDEX_LINK;
		return S_OK;
	}

// helpers

public:
	virtual COptionHandlerBase* GetOptionHandler() { return &s_optHandler; }
	virtual HRESULT GetPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, CPathW& rpath);	// return a value only if tool generates a primary output (ex: linker)
	virtual long GetPageCount()	{ return 8; }
	virtual GUID* GetPageIDs();

	virtual void SaveSpecialProps(IStream *xml, IVCPropertyContainer *pPropCnt);

protected:
	virtual BOOL UsesResponseFiles() { return TRUE; }
	virtual BOOL CreateOutputName(IVCPropertyContainer* pPropContainer, BSTR* pbstrVal);
	virtual HRESULT DoHasPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, VARIANT_BOOL* pbHasPrimaryOutput)
	{ return HasPrimaryOutputFromTool(pItem, bSchmoozeOnly, pbHasPrimaryOutput); }
	virtual LPCOLESTR GetToolFriendlyName();
	void InitToolName();
	virtual long GetToolID() { return IDS_Link; }
	virtual BOOL IsExtraMultiProp(long idProp) { return (idProp == VCLINKID_AdditionalDependencies); }	// only override this if you have a multi-prop not in an option table
	virtual HRESULT GetAdditionalDependenciesInternal(IVCPropertyContainer* pItem, IVCBuildAction* pAction, BOOL bForSave, 
		BSTR* pbstrInputs);		// additional things to add to command line

protected:
	static const wchar_t *s_pszDelOnRebuildExtensions;
	static CComBSTR s_bstrBuildDescription;
	static CComBSTR s_bstrToolName;
	static CComBSTR s_bstrExtensions;

	static GUID s_pPages[8];
	static BOOL s_bPagesInit;

public:
	static CLinkerToolOptionHandler s_optHandler;
};

// These macros are used to define enum ranges for tool switches.  If you add an enum property to the tool option
// table, please add a matching macro for it to this list.  Use of macros for range definitions helps in keeping
// maintenance of enum range checks as low as possible.
#define ShowProgressMax			 2
#define LinkIncrMax				 2
#define TSAwareMax				 2
#define LinkSubSystemMax		 2
#define LargeAddrMax			 2
#define OptRefMax				 2
#define OptFoldMax				 2
#define OptWin98Max				 2
#define TargetMachineMax		 1
