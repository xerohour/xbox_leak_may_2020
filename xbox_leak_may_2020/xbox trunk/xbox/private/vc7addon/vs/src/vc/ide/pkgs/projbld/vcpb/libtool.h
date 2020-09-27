// LibTool.h: Definition of the CVCLibrarianTool class
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "vctool.h"
#include "LinkLibHelp.h"
#include "SettingsPage.h"

// classes in this header
class CVCLibrarianTool;
class CLibrarianToolOptionHandler;

class CLibrarianToolOptionHandler : public COptionHandlerBase
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

protected:
	virtual BOOL SetEvenIfDefault(VARIANT *pVarDefault, long idOption);
	virtual BOOL SynthesizeOptionIfNeeded(IVCPropertyContainer* pPropContainer, long idOption);
	virtual BOOL SynthesizeOption(IVCPropertyContainer* pPropContainer, long idOption, CComVariant& rvar);
public:
	BOOL CreateOutputName(IVCPropertyContainer* pPropContainer, BSTR *bstrVal, BOOL bCheckForExistence = TRUE);
};

/////////////////////////////////////////////////////////////////////////////
// CVCLibrarianTool

class ATL_NO_VTABLE CVCLibrarianTool : 
	public IDispatchImpl<VCLibrarianTool, &IID_VCLibrarianTool, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public CComObjectRoot,
	public CLinkerLibrarianHelper,
	public CVCToolImpl
{
public:
	CVCLibrarianTool() 
	{
		m_nLowKey = VCLIBRARIANTOOL_MIN_DISPID;
		m_nHighKey = VCLIBRARIANTOOL_MAX_DISPID;
		m_nDirtyKey = VCLIBID_CmdLineOptionsDirty;
		m_nDirtyOutputsKey = VCLIBID_OutputsDirty;
	}
	static HRESULT CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool);

BEGIN_COM_MAP(CVCLibrarianTool)
 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCToolImpl)
	COM_INTERFACE_ENTRY(VCLibrarianTool)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(CVCLibrarianTool) 

// VCLibrarianTool
public:
	// general; /EXTRACT deliberately left out
	STDMETHOD(get_ToolName)(BSTR* pbstrToolName);	// friendly name of tool, e.g., "Librarian Tool"
	STDMETHOD(get_AdditionalOptions)(BSTR* pbstrAdditionalOptions);	// anything we don't know what to do with, given verbatim, in same order as entered, parent contributions first, goes after all other options
	STDMETHOD(put_AdditionalOptions)(BSTR bstrAdditionalOptions);
	STDMETHOD(get_OutputFile)(BSTR* pbstrOut);	// (/OUT:[file]) change the output file name (default is based on 1st lib or obj name on cmd line)
	STDMETHOD(put_OutputFile)(BSTR bstrOut);
	STDMETHOD(get_AdditionalDependencies)(BSTR* pbstrDependencies);	// additional inputs to the librarian tool such as extra libraries
	STDMETHOD(put_AdditionalDependencies)(BSTR bstrDependencies);
	STDMETHOD(get_AdditionalLibraryDirectories)(BSTR* pbstrLibPath);	// (/LIBPATH:[dir]) specify path to search for libraries on, can have multiple
	STDMETHOD(put_AdditionalLibraryDirectories)(BSTR bstrLibPath);
	STDMETHOD(get_SuppressStartupBanner)(VARIANT_BOOL* pbNoLogo);	// (/NOLOGO) enable suppression of copyright message (no explicit off)
	STDMETHOD(put_SuppressStartupBanner)(VARIANT_BOOL bNoLogo);
	STDMETHOD(get_ModuleDefinitionFile)(BSTR* pbstrDefFile);	// (/DEF:file)
	STDMETHOD(put_ModuleDefinitionFile)(BSTR bstrDefFile);
	STDMETHOD(get_IgnoreAllDefaultLibraries)(VARIANT_BOOL* pbNoDefault);	// (/NODEFAULTLIB) no default libraries not in docs
	STDMETHOD(put_IgnoreAllDefaultLibraries)(VARIANT_BOOL bNoDefault);
	STDMETHOD(get_IgnoreDefaultLibraryNames)(BSTR* pbstrLib);	// (/NODEFAULTLIB:[name]) ignore particular default library can have multiple not in docs
	STDMETHOD(put_IgnoreDefaultLibraryNames)(BSTR bstrLib);
	STDMETHOD(get_ExportNamedFunctions)(BSTR* pbstrSymbols);	// (/EXPORT:[symbol]) export function, can have multiple
	STDMETHOD(put_ExportNamedFunctions)(BSTR bstrSymbols);
	STDMETHOD(get_ForceSymbolReferences)(BSTR* pbstrSymbol);	// (/INCLUDE:[symbol]) force symbol reference, can have multiple
	STDMETHOD(put_ForceSymbolReferences)(BSTR bstrSymbol);
	// Automation properties
	STDMETHOD(get_ToolPath)(BSTR *pVal);
	STDMETHOD(get_PropertyOption)(BSTR bstrProp, long dispidProp, BSTR *pVal);
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
	STDMETHOD(HasDependencies)(IVCBuildAction* pAction, VARIANT_BOOL* pbHasDependencies);
	STDMETHOD(GetDependencies)(IVCBuildAction* pAction, IVCBuildStringCollection** ppStrings, VARIANT_BOOL* pbUpToDate);
	STDMETHOD(CanScanForDependencies)(VARIANT_BOOL* pbIsScannable);
	STDMETHOD(CreatePageObject)(IUnknown **, CLSID*, IVCPropertyContainer *, IVCSettingsPage * );
	STDMETHOD(GetPrimaryOutputIDFromTool)(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, long* pnOutputID);	// return a value only if tool generates a primary output (ex: linker)

	STDMETHOD(get_ToolDisplayIndex)(long* pIndex)
	{
		CHECK_POINTER_NULL(pIndex);
		*pIndex = TOOL_DISPLAY_INDEX_LIB;
		return S_OK;
	}

// helpers
public:
	virtual COptionHandlerBase* GetOptionHandler() { return &s_optHandler; }
	virtual HRESULT GetPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, CPathW& rpath);	// return a value only if tool generates a primary output (ex: linker)
	virtual long GetPageCount()	{ return 2; }
	virtual GUID* GetPageIDs();

protected:
	virtual BOOL UsesResponseFiles() { return TRUE; }
	virtual BOOL CreateOutputName(IVCPropertyContainer* pPropContainer, BSTR* pbstrVal);
	virtual HRESULT DoHasPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, VARIANT_BOOL* pbHasPrimaryOutput)
	{ return HasPrimaryOutputFromTool(pItem, bSchmoozeOnly, pbHasPrimaryOutput); }
	virtual LPCOLESTR GetToolFriendlyName();
	void InitToolName();
	virtual long GetToolID() { return IDS_Lib; }
	virtual BOOL IsExtraMultiProp(long idProp) { return (idProp == VCLIBID_AdditionalDependencies); }	// only override this if you have a multi-prop not in an option table
	virtual HRESULT GetAdditionalDependenciesInternal(IVCPropertyContainer* pItem, IVCBuildAction* pAction, BOOL bForSave, 
		BSTR* pbstrDependencies);		// additional things to add to command line

public:
	static CLibrarianToolOptionHandler s_optHandler;

protected:
	static GUID s_pPages[2];
	static BOOL s_bPagesInit;
	static CComBSTR s_bstrBuildDescription;
	static CComBSTR s_bstrToolName;
	static CComBSTR s_bstrExtensions;
};


/////////////////////////////////////////////////////////////////////////////
// CVCLibrarianPage

class ATL_NO_VTABLE CVCLibrarianPage :
	public IDispatchImpl<IVCLibrarianPage, &IID_IVCLibrarianPage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public CPageObjectImpl<CVCLibrarianPage,VCLIBRARIANTOOL_MIN_DISPID,VCLIBRARIANTOOL_MAX_DISPID>,
	public CComObjectRoot
{
public:
BEGIN_COM_MAP(CVCLibrarianPage)
 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCLibrarianPage)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
	COM_INTERFACE_ENTRY(IProvidePropertyBuilder)
END_COM_MAP()

	// IDispatch override
	STDMETHOD(Invoke)( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pexcepInfo, UINT *puArgErr )
	{
		IDispatchImpl<IVCLibrarianPage, &IID_IVCLibrarianPage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>::Invoke( dispid, riid, lcid, wFlags, pdispParams, pvarResult, pexcepInfo, puArgErr );
		return S_OK;
	}


// IVCLibrarianPage
public:
	STDMETHOD(get_OutputFile)(BSTR* pbstrOut);	// (/OUT:[file]) change the output file name (default is based on 1st lib or obj name on cmd line)
	STDMETHOD(put_OutputFile)(BSTR bstrOut);
	STDMETHOD(get_AdditionalDependencies)(BSTR* pbstrDependencies);	// additional inputs to the librarian tool such as extra libraries
	STDMETHOD(put_AdditionalDependencies)(BSTR bstrDependencies);
	STDMETHOD(get_AdditionalLibraryDirectories)(BSTR* pbstrLibPath);	// (/LIBPATH:[dir]) specify path to search for libraries on, can have multiple
	STDMETHOD(put_AdditionalLibraryDirectories)(BSTR bstrLibPath);
	STDMETHOD(get_SuppressStartupBanner)(enumSuppressStartupBannerUpBOOL* pbNoLogo);	// (/NOLOGO) enable suppression of copyright message (no explicit off)
	STDMETHOD(put_SuppressStartupBanner)(enumSuppressStartupBannerUpBOOL bNoLogo);
	STDMETHOD(get_ModuleDefinitionFile)(BSTR* pbstrDefFile);	// (/DEF:file)
	STDMETHOD(put_ModuleDefinitionFile)(BSTR bstrDefFile);
	STDMETHOD(get_IgnoreAllDefaultLibraries)(enumIgnoreAllDefaultLibrariesBOOL* pbNoDefault);	// (/NODEFAULTLIB) no default libraries not in docs
	STDMETHOD(put_IgnoreAllDefaultLibraries)(enumIgnoreAllDefaultLibrariesBOOL bNoDefault);
	STDMETHOD(get_IgnoreDefaultLibraryNames)(BSTR* pbstrLib);	// (/NODEFAULTLIB:[name]) ignore particular default library can have multiple not in docs
	STDMETHOD(put_IgnoreDefaultLibraryNames)(BSTR bstrLib);
	STDMETHOD(get_ExportNamedFunctions)(BSTR* pbstrSymbols);	// (/EXPORT:[symbol]) export function, can have multiple
	STDMETHOD(put_ExportNamedFunctions)(BSTR bstrSymbols);
	STDMETHOD(get_ForceSymbolReferences)(BSTR* pbstrSymbol);	// (/INCLUDE:[symbol]) force symbol reference, can have multiple
	STDMETHOD(put_ForceSymbolReferences)(BSTR bstrSymbol);

// helpers
public:
	virtual void GetBaseDefault(long id, CComVariant& varValue);
	virtual BOOL UseDirectoryPickerDialog(long id) { return (id == VCLIBID_AdditionalLibraryDirectories); }
};

// These macros are used to define enum ranges for tool switches.  If you add an enum property to the tool option
// table, please add a matching macro for it to this list.  Use of macros for range definitions helps in keeping
// maintenance of enum range checks as low as possible.
