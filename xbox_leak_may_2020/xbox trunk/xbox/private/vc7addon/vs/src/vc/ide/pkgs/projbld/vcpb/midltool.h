// VCMidlTool.h: Definition of the CVCMidlTool class
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "vctool.h"
#include "MidlToolBase.h"

// classes in this header
class CVCMidlTool;
class CMidlToolOptionHandler;

class CMidlToolOptionHandler : public COptionHandlerBase
{
public:
	DECLARE_OPTION_TABLE()

public:
	// default value handlers
	// string props
	virtual void GetDefaultValue( long id, BSTR *pVal, IVCPropertyContainer *pPropCnt = NULL );
	// integer props
	virtual void GetDefaultValue( long id, long *pVal, IVCPropertyContainer *pPropCnt = NULL );
	// boolean props
	virtual void GetDefaultValue( long id, VARIANT_BOOL *pVal, IVCPropertyContainer *pPropCnt = NULL );

protected:
	virtual BOOL SetEvenIfDefault(VARIANT *pVarDefault, long idOption);
	virtual BOOL SynthesizeOptionIfNeeded(IVCPropertyContainer* pPropContainer, long idOption);
	virtual BOOL SynthesizeOption(IVCPropertyContainer* pPropContainer, long idOption, CComVariant& rvar);

public:
	BOOL CreatesTypeLib(IVCPropertyContainer* pPropContainer);
};

/////////////////////////////////////////////////////////////////////////////
// CVCMidlTool

class ATL_NO_VTABLE CVCMidlTool : 
	public IDispatchImpl<VCMidlTool, &IID_VCMidlTool, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public CComObjectRoot,
	public CVCToolImpl
{
public:
	CVCMidlTool() 
	{
		m_nLowKey = VCMIDLTOOL_MIN_DISPID;
		m_nHighKey = VCMIDLTOOL_MAX_DISPID;
		m_nDirtyKey = VCMIDLID_CmdLineOptionsDirty;
		m_nDirtyOutputsKey = VCMIDLID_OutputsDirty;
	}
	static HRESULT CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool);

BEGIN_COM_MAP(CVCMidlTool)
	COM_INTERFACE_ENTRY(IVCToolImpl)
 	COM_INTERFACE_ENTRY(VCMidlTool)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(CVCMidlTool) 

// VCMidlTool
public:
	// general (/ms_ext, /c_ext left out because they're archaic); @response deliberately left out
	STDMETHOD(get_ToolName)(BSTR* pbstrToolName);	// friendly name of tool, e.g., "C/C++ Compiler Tool"
	STDMETHOD(get_AdditionalOptions)(BSTR* pbstrAdditionalOptions);	// anything we don't know what to do with, given verbatim, in same order as entered, parent contributions first, goes after all other options
	STDMETHOD(put_AdditionalOptions)(BSTR bstrAdditionalOptions);
	STDMETHOD(get_PreprocessorDefinitions)(BSTR* pbstrDefines);	// (/D![macro]) define macro for MIDL.  can have multiple.
	STDMETHOD(put_PreprocessorDefinitions)(BSTR bstrDefines);
	STDMETHOD(get_AdditionalIncludeDirectories)(BSTR* pbstrIncludePath);	// (/I![path]) directory to add to include path, may have multiple
	STDMETHOD(put_AdditionalIncludeDirectories)(BSTR bstrIncludePath);
	STDMETHOD(get_IgnoreStandardIncludePath)(VARIANT_BOOL* pbIgnore);	// (/no_def_idir) ignore current and INCLUDE path
	STDMETHOD(put_IgnoreStandardIncludePath)(VARIANT_BOOL bIgnore);
	STDMETHOD(get_MkTypLibCompatible)(VARIANT_BOOL* pbCompatible);	// (/mktyplib203) forces compatibility with mktyplib.exe version 2.03
	STDMETHOD(put_MkTypLibCompatible)(VARIANT_BOOL bCompatible);
	STDMETHOD(get_WarningLevel)(midlWarningLevelOption* poptSetting);	// see enum above (/W0 - /W4)
	STDMETHOD(put_WarningLevel)(midlWarningLevelOption optSetting);
	STDMETHOD(get_WarnAsError)(VARIANT_BOOL* pbWarnAsError);	// (/WX) treat warnings as errors
	STDMETHOD(put_WarnAsError)(VARIANT_BOOL bWarnAsError);
	STDMETHOD(get_SuppressStartupBanner)(VARIANT_BOOL* pbNoLogo);	// (/nologo) enable suppression of copyright message
	STDMETHOD(put_SuppressStartupBanner)(VARIANT_BOOL bNoLogo);
	STDMETHOD(get_DefaultCharType)(midlCharOption* poptSetting);	// see enum above (/char)
	STDMETHOD(put_DefaultCharType)(midlCharOption optSetting);
	STDMETHOD(get_TargetEnvironment)(midlTargetEnvironment* poptSetting);	// (/env win64 vs. /env win32) target Win64 (or Win32) environment
	STDMETHOD(put_TargetEnvironment)(midlTargetEnvironment optSetting);
	STDMETHOD(get_GenerateStublessProxies)(VARIANT_BOOL* poptSetting);	// (/Oicf)
	STDMETHOD(put_GenerateStublessProxies)(VARIANT_BOOL optSetting);
	// output 
	STDMETHOD(get_GenerateTypeLibrary)(VARIANT_BOOL* poptSetting);	// (/notlb for no) generate a type library or not; default: yes
	STDMETHOD(put_GenerateTypeLibrary)(VARIANT_BOOL optSetting);
	STDMETHOD(get_OutputDirectory)(BSTR* pbstrOut);	// (/out dir) directory to place output files in; default: project directory
	STDMETHOD(put_OutputDirectory)(BSTR bstrOut);
	STDMETHOD(get_HeaderFileName)(BSTR* pbstrHeaderFile);	// (/h[eader] filename) specify name of header file generated; default: <idlfile>.h
	STDMETHOD(put_HeaderFileName)(BSTR bstrHeaderFile);
	STDMETHOD(get_DLLDataFileName)(BSTR* pbstrDLLData);	// (/dlldata filename) specify name of DLLDATA file generated; default: dlldata.c
	STDMETHOD(put_DLLDataFileName)(BSTR bstrDLLData);
	STDMETHOD(get_InterfaceIdentifierFileName)(BSTR* pbstrIID);	// (/iid filename) specify name of IID file; default: <idlfile>_i.c
	STDMETHOD(put_InterfaceIdentifierFileName)(BSTR bstrIID);
	STDMETHOD(get_ProxyFileName)(BSTR* pbstrProxyFile);	// (/proxy filename) specify name of proxy file; default: <idlfile>_p.c
	STDMETHOD(put_ProxyFileName)(BSTR bstrProxyFile);
	STDMETHOD(get_TypeLibraryName)(BSTR* pbstrTLBFile);	// (/tlb filename) specify name of TLB file; default: <idlfile>.tlb
	STDMETHOD(put_TypeLibraryName)(BSTR bstrTLBFile);
	// advanced (/nocpp ignored as it is not in docs)
	STDMETHOD(get_EnableErrorChecks)(midlErrorCheckOption* poptSetting);	// see enum above (/error {none|all}) 
	STDMETHOD(put_EnableErrorChecks)(midlErrorCheckOption optSetting);
	STDMETHOD(get_ErrorCheckAllocations)(VARIANT_BOOL* pbErrorCheck);	// (/error allocation) check for out of memory errors
	STDMETHOD(put_ErrorCheckAllocations)(VARIANT_BOOL bErrorCheck);
	STDMETHOD(get_ErrorCheckBounds)(VARIANT_BOOL* pbErrorCheck);	// (/error bounds_check) check size	vs. transmission length specifications
	STDMETHOD(put_ErrorCheckBounds)(VARIANT_BOOL bErrorCheck);
	STDMETHOD(get_ErrorCheckEnumRange)(VARIANT_BOOL* pbErrorCheck);	// (/error enum) check enum values to be in allowable range not in docs
	STDMETHOD(put_ErrorCheckEnumRange)(VARIANT_BOOL bErrorCheck);
	STDMETHOD(get_ErrorCheckRefPointers)(VARIANT_BOOL* pbErrorCheck);	// (/error ref) check ref pointers to be non-NULL
	STDMETHOD(put_ErrorCheckRefPointers)(VARIANT_BOOL bErrorCheck);
	STDMETHOD(get_ErrorCheckStubData)(VARIANT_BOOL* pbErrorCheck);	// (/error stub_data) emit additional check for server side data stub validity
	STDMETHOD(put_ErrorCheckStubData)(VARIANT_BOOL bErrorCheck);
	STDMETHOD(get_ValidateParameters)(VARIANT_BOOL* pbValidate);	// (/robust) generate additional information to validate parameters not in docs
	STDMETHOD(put_ValidateParameters)(VARIANT_BOOL bValidate);
	STDMETHOD(get_StructMemberAlignment)(midlStructMemberAlignOption* poptSetting);	// see enum above (/Zp[num])
	STDMETHOD(put_StructMemberAlignment)(midlStructMemberAlignOption optSetting);
	STDMETHOD(get_RedirectOutputAndErrors)(BSTR* pbstrOutput);	// (/o file) redirect output from screen to a file
	STDMETHOD(put_RedirectOutputAndErrors)(BSTR bstrOutput);
	STDMETHOD(get_CPreprocessOptions)(BSTR* pbstrOpt);	// (/cpp_opt"[option]") C-compiler preprocessor option to pass to MIDL.  must be in exact format for desired switches
	STDMETHOD(put_CPreprocessOptions)(BSTR bstrOpt);
	STDMETHOD(get_UndefinePreprocessorDefinitions)(BSTR* pbstrUndefines);	// (/U![macro]) remove any previous definition of symbol.  can have multiple.
	STDMETHOD(put_UndefinePreprocessorDefinitions)(BSTR bstrUndefines);
	// Automation properties
	STDMETHOD(get_ToolPath)(BSTR *pVal);
	STDMETHOD(get_PropertyOption)(BSTR bstrProp, long dispidProp, BSTR *pVal);
	STDMETHOD(get_FullIncludePath)(BSTR* fullIncludePath);	// include path, including all inherited values, plus platform includes
	STDMETHOD(get_VCProjectEngine)(IDispatch** projEngine)	{ return DoGetVCProjectEngine(projEngine); }

// IVCToolImpl
public:
	STDMETHOD(get_DefaultExtensions)(BSTR* pVal);
	STDMETHOD(put_DefaultExtensions)(BSTR newVal);
	STDMETHOD(GetAdditionalOptionsInternal)(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, BSTR* pbstrAdditionalOptions);
	STDMETHOD(GetBuildDescription)(IVCBuildAction* pAction, BSTR* pbstrBuildDescription);
	STDMETHOD(get_ToolPathInternal)(BSTR* pbstrToolPath);
	STDMETHOD(get_Bucket)(long *pVal);
	STDMETHOD(GenerateOutput)(long type, IVCBuildActionList* plstActions, IVCBuildErrorContext* pEC);
	STDMETHOD(AffectsOutput)(long nPropID, VARIANT_BOOL* pbAffectsOutput);
	STDMETHOD(CreateToolObject)(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject);
	STDMETHOD(GetAdditionalIncludeDirectoriesInternal)(IVCPropertyContainer* pPropContainer, BSTR* pbstrIncDirs);
	STDMETHOD(get_IncludePathID)(long* pnIncludeID);
	STDMETHOD(get_ToolShortName)(BSTR* pbstrToolName);
	STDMETHOD(get_ToolNameInternal)(BSTR* pbstrToolName) { return get_ToolName(pbstrToolName); }
	STDMETHOD(MatchName)(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches);
	STDMETHOD(CanScanForDependencies)(VARIANT_BOOL* pbIsScannable);
	STDMETHOD(GenerateDependencies)(IVCBuildAction* pAction, void* frhFile, IVCBuildErrorContext* pErrorContext);
	STDMETHOD(CreatePageObject)(IUnknown **, CLSID*, IVCPropertyContainer *, IVCSettingsPage * );

	STDMETHOD(get_ToolDisplayIndex)(long* pIndex)
	{
		CHECK_POINTER_NULL(pIndex);
		*pIndex = TOOL_DISPLAY_INDEX_MIDL;
		return S_OK;
	}

	virtual long GetPageCount()	{ return 4; }
	virtual GUID* GetPageIDs();

// helpers
public:
	virtual COptionHandlerBase* GetOptionHandler() { return &s_optHandler; }

protected:
	virtual BOOL UsesResponseFiles() { return TRUE; }
	void GenerateTypeLibDep(IVCBuildAction* pAction, IVCPropertyContainer* pPropContainer, IVCBuildErrorContext* pEC, 
		CDirW& dirBase);
	virtual LPCOLESTR GetToolFriendlyName();
	void InitToolName();
	virtual long GetToolID() { return IDS_Midl; }

public:
	static CMidlToolOptionHandler s_optHandler;

protected:
	static GUID s_pPages[4];
	static BOOL s_bPagesInit;
	static CComBSTR s_bstrBuildDescription;
	static CComBSTR s_bstrToolName;
	static CComBSTR s_bstrExtensions;
};

// These macros are used to define enum ranges for tool switches.  If you add an enum property to the tool option
// table, please add a matching macro for it to this list.  Use of macros for range definitions helps in keeping
// maintenance of enum range checks as low as possible.
#define midlWarnMax				4
#define charTypeMax				2
#define envMax					2
#define errCheckMax				2
#define midlStructAlignMax		4
