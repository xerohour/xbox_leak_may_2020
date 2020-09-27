// VCLinkerBaseTool.h: Definition of the CVCLinkerBaseTool class
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "vctool.h"
#include "settingspage.h"

template<class T, class IFace>
class ATL_NO_VTABLE CVCLinkerBasePage :
	public IDispatchImpl<IFace, &(__uuidof(IFace)), &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber, 0, CVsTypeInfoHolder>,
	public CPageObjectImpl<T, VCLINKERTOOL_MIN_DISPID, VCLINKERTOOL_MAX_DISPID>,
	public CComObjectRoot
{
public:
BEGIN_COM_MAP(T)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IFace)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
	COM_INTERFACE_ENTRY(IProvidePropertyBuilder)
END_COM_MAP()

	// IDispatch override
	STDMETHOD(Invoke)( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pexcepInfo, UINT *puArgErr )
	{
		IDispatchImpl<IFace, &(__uuidof(IFace)), &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>::Invoke( dispid, riid, lcid, wFlags, pdispParams, pvarResult, pexcepInfo, puArgErr );
		return S_OK;
	}

// IVCPropertyContainer override
public:
	STDMETHOD(Commit)()
	{
		if (m_pContainer)
		{
			// need to set/unset 'generate map file' if name changes and user didn't explictly change the 'generate'
			CComVariant varName;
			if (m_pContainer->GetLocalProp(VCLINKID_MapFileName, &varName) == S_OK)	// name changed
			{
				CComVariant var;
				// if both changed locally, don't do anything about keeping them in sync
				if (m_pContainer->GetLocalProp(VCLINKID_GenerateMapFile, &var) != S_OK)	
				{
					CStringW strName = varName.bstrVal;
					strName.TrimLeft();
					if (m_pContainer->GetProp(VCLINKID_GenerateMapFile, &var) != S_OK)
					{
						var.vt = VT_BOOL;
						var.boolVal = VARIANT_FALSE;
					}
					if (var.boolVal && strName.IsEmpty())
					{
						var.boolVal = VARIANT_FALSE;
						m_pContainer->SetProp(VCLINKID_GenerateMapFile, var);
					}
					else if (!var.boolVal && !strName.IsEmpty())
					{
						var.boolVal = VARIANT_TRUE;
						m_pContainer->SetProp(VCLINKID_GenerateMapFile, var);
					}
				}
			}
		}
		return CPageObjectImpl<T, VCLINKERTOOL_MIN_DISPID, VCLINKERTOOL_MAX_DISPID>::Commit();
	}
};


class ATL_NO_VTABLE CVCLinkerGeneralPage :
	public CVCLinkerBasePage<CVCLinkerGeneralPage, IVCLinkerGeneralPage>
{
// IVCLinkerGeneralPage
public:
	STDMETHOD(get_AdditionalOptions)(BSTR* pbstrAdditionalOptions);
	STDMETHOD(put_AdditionalOptions)(BSTR bstrAdditionalOptions);
	STDMETHOD(get_OutputFile)(BSTR* pbstrOut);	// (/OUT:[file]) change the output file name (default is based on 1st lib or obj name on cmd line)
	STDMETHOD(put_OutputFile)(BSTR bstrOut);
	STDMETHOD(get_ShowProgress)(linkProgressOption* poptSetting);	// see enum above (/VERBOSE)
	STDMETHOD(put_ShowProgress)(linkProgressOption optSetting);
	STDMETHOD(get_Version)(BSTR* pbstrVersion);	// (/VERSION:version) put this version number into header of created image
	STDMETHOD(put_Version)(BSTR bstrVersion);
	STDMETHOD(get_LinkIncremental)(linkIncrementalType* poptSetting);	// (/INCREMENTAL:YES, /INCREMENTAL:NO, not set)
	STDMETHOD(put_LinkIncremental)(linkIncrementalType optSetting);
	STDMETHOD(get_SuppressStartupBanner)(enumSuppressStartupBannerUpBOOL* pbNoLogo);	// (/NOLOGO) enable suppression of copyright message (no explicit off)
	STDMETHOD(put_SuppressStartupBanner)(enumSuppressStartupBannerUpBOOL bNoLogo);
	STDMETHOD(get_IgnoreImportLibrary)(enumBOOL* pbIgnore);	// ignore export .lib
	STDMETHOD(put_IgnoreImportLibrary)(enumBOOL bIgnore);
	STDMETHOD(get_RegisterOutput)(enumBOOL* pbRegister);	// register the primary output of the build
	STDMETHOD(put_RegisterOutput)(enumBOOL bRegister);
	STDMETHOD(get_AdditionalLibraryDirectories)(BSTR* pbstrLibPath);	// (/LIBPATH:[dir]) specify path to search for libraries on, can have multiple
	STDMETHOD(put_AdditionalLibraryDirectories)(BSTR bstrLibPath);

// helpers
public:
	virtual void GetBaseDefault(long id, CComVariant& varValue);
	virtual BOOL UseDirectoryPickerDialog(long id) { return (id == VCLINKID_AdditionalLibraryDirectories); }
};

class ATL_NO_VTABLE CVCLinkerInputPage :
	public CVCLinkerBasePage<CVCLinkerInputPage, IVCLinkerInputPage>
{
// IVCLinkerInputPage
public:
	STDMETHOD(get_AdditionalDependencies)(BSTR* pbstrInputs);	// additional inputs for the link path (comdlg32.lib, etc.)
	STDMETHOD(put_AdditionalDependencies)(BSTR bstrInputs);
	STDMETHOD(get_IgnoreAllDefaultLibraries)(enumIgnoreAllDefaultLibrariesBOOL* pbNoDefaults);	// (/NODEFAULTLIB) ignore all default libraries
	STDMETHOD(put_IgnoreAllDefaultLibraries)(enumIgnoreAllDefaultLibrariesBOOL bNoDefaults);
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
};

class ATL_NO_VTABLE CVCLinkerDebugPage :
	public CVCLinkerBasePage<CVCLinkerDebugPage, IVCLinkerDebugPage>
{
// IVCLinkerDebugPage
public:
	STDMETHOD(get_GenerateDebugInformation)(enumGenerateDebugInformationBOOL* pbDebug);	// (/DEBUG) generate debug info
	STDMETHOD(put_GenerateDebugInformation)(enumGenerateDebugInformationBOOL bDebug);
	STDMETHOD(get_ProgramDatabaseFile)(BSTR* pbstrFile);	// (/PDB:file) use program database
	STDMETHOD(put_ProgramDatabaseFile)(BSTR bstrFile);
	STDMETHOD(get_StripPrivateSymbols)(BSTR* pbstrStrippedPDB);	// (/PDBSTRIPPED:file) create PDB with no private symbols
	STDMETHOD(put_StripPrivateSymbols)(BSTR bstrStrippedPDB);
	STDMETHOD(get_GenerateMapFile)(enumGenerateMapFileBOOL* pbMap);	// (/MAP[:file]) generate map file during linking
	STDMETHOD(put_GenerateMapFile)(enumGenerateMapFileBOOL bMap);
	STDMETHOD(get_MapFileName)(BSTR* pbstrMapFile);	// optional argument to GenerateMapFile property
	STDMETHOD(put_MapFileName)(BSTR bstrMapFile);
	STDMETHOD(get_MapExports)(enumMapExportsBOOL* pbExports);	// (/MAPINFO:EXPORTS) include exported functions in map info
	STDMETHOD(put_MapExports)(enumMapExportsBOOL bExports);
	STDMETHOD(get_MapLines)(enumMapLinesBOOL* pbLines);	// (/MAPINFO:LINES) include line number info in map info
	STDMETHOD(put_MapLines)(enumMapLinesBOOL bLines);

// helpers
public:
	virtual void GetBaseDefault(long id, CComVariant& varValue);
};

class ATL_NO_VTABLE CVCLinkerSystemPage :
	public CVCLinkerBasePage<CVCLinkerSystemPage, IVCLinkerSystemPage>
{
// IVsPerPropertyBrowsing
public:
	STDMETHOD(HideProperty)(DISPID dispid, BOOL *pfHide);

// IVCLinkerSystemPage
public:
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
	STDMETHOD(get_SwapRunFromCD)(enumSwapRunFromCDBOOL* pbRun);	// swap run from the CD (/SWAPRUN:CD)
	STDMETHOD(put_SwapRunFromCD)(enumSwapRunFromCDBOOL bRun);
	STDMETHOD(get_SwapRunFromNet)(enumSwapRunFromNetBOOL* pbRun);	// swap run from the net (/SWAPRUN:NET)
	STDMETHOD(put_SwapRunFromNet)(enumSwapRunFromNetBOOL bRun);
};

class ATL_NO_VTABLE CVCLinkerOptimizationPage :
	public CVCLinkerBasePage<CVCLinkerOptimizationPage, IVCLinkerOptimizationPage>
{
// IVCLinkerOptimizationPage
public:
	STDMETHOD(get_OptimizeReferences)(optRefType* poptSetting);	// (/OPT:REF, /OPT:NOREF, not set) eliminate/keep functions & data never referenced
	STDMETHOD(put_OptimizeReferences)(optRefType optSetting);
	STDMETHOD(get_EnableCOMDATFolding)(optFoldingType* poptSetting);	// (/OPT:ICF, /OPT:NOICF, not set) eliminate/keep redundant COMDAT data (data folding)
	STDMETHOD(put_EnableCOMDATFolding)(optFoldingType optSetting);
	STDMETHOD(get_OptimizeForWindows98)(optWin98Type* poptSetting);	// (/OPT:WIN98, /OPT:NOWIN98, not set) 
	STDMETHOD(put_OptimizeForWindows98)(optWin98Type optSetting);
	STDMETHOD(get_FunctionOrder)(BSTR* pbstrOrder);	// (/ORDER:@[file]) place functions in order specified in file
	STDMETHOD(put_FunctionOrder)(BSTR bstrOrder);
};

class ATL_NO_VTABLE CVCLinkerAdvancedPage :
	public CVCLinkerBasePage<CVCLinkerAdvancedPage, IVCLinkerAdvancedPage>
{
// IVCLinkerAdvancedPage
public:
	STDMETHOD(get_EntryPointSymbol)(BSTR* pbstrEntry);	// (/ENTRY:[symbol]) set entry point address for EXE or DLL; incompatible with /NOENTRY
	STDMETHOD(put_EntryPointSymbol)(BSTR bstrEntry);
	STDMETHOD(get_ResourceOnlyDLL)(enumResourceOnlyDLLBOOL* pbNoEntry);	// (/NOENTRY) no entry point.  required for resource-only DLLs; incompatible with /ENTRY
	STDMETHOD(put_ResourceOnlyDLL)(enumResourceOnlyDLLBOOL bNoEntry);
	STDMETHOD(get_SetChecksum)(enumSetChecksumBOOL* pbRelease);	// (/RELEASE) set the checksum in the header of a .exe
	STDMETHOD(put_SetChecksum)(enumSetChecksumBOOL bRelease);
	STDMETHOD(get_BaseAddress)(BSTR* pbstrAddress);	// (/BASE:{address| filename,key}) base address to place program at; can be numeric or string
	STDMETHOD(put_BaseAddress)(BSTR bstrAddress);
	STDMETHOD(get_TurnOffAssemblyGeneration)(enumTurnOffAssemblyGenerationBOOL* pbNoAssy);	// (/NOASSEMBLY) cause the output file to be built without an assembly
	STDMETHOD(put_TurnOffAssemblyGeneration)(enumTurnOffAssemblyGenerationBOOL bNoAssy);
	STDMETHOD(get_SupportUnloadOfDelayLoadedDLL)(enumSupportUnloadOfDelayLoadedDLLBOOL* pbDelay);	// (/DELAY:UNLOAD) use to allow explicit unloading of the DLL
	STDMETHOD(put_SupportUnloadOfDelayLoadedDLL)(enumSupportUnloadOfDelayLoadedDLLBOOL bDelay);
	STDMETHOD(get_ImportLibrary)(BSTR* pbstrImportLib);	// (/IMPLIB:[library]) generate specified import library
	STDMETHOD(put_ImportLibrary)(BSTR bstrImportLib);
	STDMETHOD(get_MergeSections)(BSTR* pbstrMerge);	// (/MERGE:from=to) merge section 'from' into section 'to'
	STDMETHOD(put_MergeSections)(BSTR bstrMerge);
	STDMETHOD(get_TargetMachine)(machineTypeOption* poptSetting);	// (/MACHINE:type) specify target platform
	STDMETHOD(put_TargetMachine)(machineTypeOption optSetting);
};

class ATL_NO_VTABLE CVCLinkerMIDLPage :
	public CVCLinkerBasePage<CVCLinkerMIDLPage, IVCLinkerMIDLPage>
{
// IVCLinkerMIDLPage
public:
	STDMETHOD(get_MidlCommandFile)(BSTR* pbstrMidlCmdFile);	// (/midl:<@midl cmd file>) specify response file for MIDL commands to use
	STDMETHOD(put_MidlCommandFile)(BSTR bstrMidlCmdFile);
	STDMETHOD(get_IgnoreEmbeddedIDL)(enumIgnoreEmbeddedIDLBOOL* pbIgnoreIDL);	// (/ignoreidl) ignore .idlsym sections of .obj files
	STDMETHOD(put_IgnoreEmbeddedIDL)(enumIgnoreEmbeddedIDLBOOL bIgnoreIDL);
	STDMETHOD(get_MergedIDLBaseFileName)(BSTR* pbstrIDLFile);	// (/idlout:<filename>) name intermediate IDL output file
	STDMETHOD(put_MergedIDLBaseFileName)(BSTR bstrIDLFile);
	STDMETHOD(get_TypeLibraryFile)(BSTR* pbstrTLBFile);	// (/tlbout:<filename>) name intermediate typelib output file
	STDMETHOD(put_TypeLibraryFile)(BSTR bstrTLBFile);
	STDMETHOD(get_TypeLibraryResourceID)(long* pnResID);	// (/tlbid:<id>) specify resource ID for generated .tlb file
	STDMETHOD(put_TypeLibraryResourceID)(long nResID);

protected:
	virtual BOOL UseCommandsDialog(long id) { return (id == VCLINKID_MidlCommandFile); }
};

