#pragma once

#include "ProjectOptions.h"
#include <vccoll.h>
#include <path2.h>
#include "FileRegistry.h"

#ifdef _SHIP
#define FORCE_DIRTY() ;
#else
#define FORCE_DIRTY() \
	{ \
		HRESULT hr = ForceDirty(); \
		if (hr != S_OK) \
			return hr; \
	}
#endif


class CVCToolImpl : 
	public IVCToolImpl
{
public:
	CVCToolImpl() 
	{
		m_nLowKey = -1;
		m_nHighKey = -1;
		m_nDirtyKey = -1;
		m_nDirtyOutputsKey = -1;
	}
	virtual ~CVCToolImpl() {}

protected:
	virtual void Initialize(IVCPropertyContainer* pPropContainer) 
	{
		m_spPropertyContainer = pPropContainer;
	}

// IVCToolImpl
public:
	STDMETHOD(get_DefaultExtensions)(BSTR *pVal)
	{ VSASSERT(FALSE, "get_DefaultExtensions must be overridden");	return VCPROJ_E_INTERNAL_ERR; }	// required override
	STDMETHOD(put_DefaultExtensions)(BSTR newVal)
	{ VSASSERT(FALSE, "put_DefaultExtensions must be overridden");	return VCPROJ_E_INTERNAL_ERR; }	// required override
	STDMETHOD(GetAdditionalOptionsInternal)(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, BSTR* pbstrOptions)
	{ VSASSERT(FALSE, "GetAdditionalOptionsInternal must be overridden");	return VCPROJ_E_INTERNAL_ERR; }	// required override
	STDMETHOD(GetBuildDescription)(IVCBuildAction* pAction, BSTR* pbstrBuildDescription)
	{ VSASSERT(FALSE, "GetBuildDescription must be overridden");	return VCPROJ_E_INTERNAL_ERR; }	// required override
	STDMETHOD(get_ToolPathInternal)(BSTR* pbstrToolPath)	// repeat of ToolPath from tools so as to make command line building easier
	{ VSASSERT(FALSE, "get_ToolPathInternal must be overridden");	return VCPROJ_E_INTERNAL_ERR; }	// required override
	STDMETHOD(get_Bucket)(long *pVal)
	{ VSASSERT(FALSE, "get_Bucket must be overridden");	return VCPROJ_E_INTERNAL_ERR; }	// required override
	STDMETHOD(get_ToolNameInternal)(BSTR* pbstrToolName)
	{ VSASSERT(FALSE, "get_ToolNameInternal must be overridden by file tools"); return VCPROJ_E_INTERNAL_ERR; }	// required override for file tools
	STDMETHOD(CreateToolObject)(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject)
	{ VSASSERT(FALSE, "CreateToolObject must be overridden");	return VCPROJ_E_INTERNAL_ERR; }	// required override
	STDMETHOD(get_ToolDisplayIndex)(long* index)
	{ VSASSERT( false, "get_ToolDisplayIndex must be overridden" ); return VCPROJ_E_INTERNAL_ERR; }	// required override
	
	STDMETHOD(GetAdditionalIncludeDirectoriesInternal)(IVCPropertyContainer* pPropContainer, BSTR* pbstrIncPaths);
	STDMETHOD(get_IncludePathID)(long* pnIncludeID);
	STDMETHOD(GetCommandLineEx)(IVCBuildAction*, IVCBuildableItem* pBuildableItem, IVCBuildEngine* pBuildEngine, IVCBuildErrorContext* pEC, BSTR* bstrCmd);
	STDMETHOD(GetCommandLineOptions)(IUnknown* pItem, IVCBuildAction* pAction, VARIANT_BOOL bIncludeAdditional, commandLineOptionStyle fStyle, BSTR* pVal);
	STDMETHOD(GetCommandLineOptionsForDisplay)(IUnknown* pItem, VARIANT_BOOL bIncludeAdditional, BSTR* pVal);
	STDMETHOD(GetPages)(CAUUID* pPages);
	STDMETHOD(IsTargetTool)(IVCBuildAction* pAction, VARIANT_BOOL* pbTargetTool); // tool operates on target, not on file
	STDMETHOD(get_PerformIfAble)(VARIANT_BOOL* pbPerformIfAble); // tool can run, even given previous errors
	STDMETHOD(get_IsComspecTool)(VARIANT_BOOL* pbIsComspecTool); // tool requires comspec
	STDMETHOD(get_IsCustomBuildTool)(VARIANT_BOOL* pbIsCustomTool);	// tool counts as custom
	STDMETHOD(PrePerformBuildActions)(bldActionTypes type, IVCBuildActionList* pActions, bldAttributes aob, IVCBuildErrorContext* pEC, actReturnStatus* pActReturn);
	STDMETHOD(PostPerformBuildActions)(bldActionTypes type, IVCBuildActionList* pActions, bldAttributes aob, IVCBuildErrorContext* pEC, actReturnStatus* pActReturn);
	STDMETHOD(IsDeleteOnRebuildFile)(LPCOLESTR szFile, VARIANT_BOOL* pbDelOnRebuild);
	STDMETHOD(GetCommandLinesForBuild)(IVCBuildActionList* pActions, bldAttributes attrib, IVCBuildEngine* pBldEngine, IVCBuildErrorContext* pErrorContext, IVCCommandLineList** ppCmds);
	STDMETHOD(GenerateOutput)(long type, IVCBuildActionList* pActions, IVCBuildErrorContext* pEC);
	STDMETHOD(AffectsOutput)(long nPropID, VARIANT_BOOL* pbAffectsOutput);
	STDMETHOD(GetPropertyOptionInternal)(IUnknown* pItem, BSTR bstrProp, long dispidProp, BSTR *pVal);
	STDMETHOD(SaveObject)(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent);
	STDMETHOD(WriteToStream)(IStream *pStream, IVCPropertyContainer* pPropContainer) { return S_FALSE; }		// nothing written
	STDMETHOD(ReadFromStream)(IStream *pStream, IVCPropertyContainer* pPropContainer) { return S_FALSE; }		// nothing written
	STDMETHOD(HasPrimaryOutputFromTool)(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, VARIANT_BOOL* pbHasPrimaryOutput);	// returns TRUE only if tool generates a primary output (ex: linker)
	STDMETHOD(GetPrimaryOutputFromTool)(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, BSTR* pbstrPath);	// return a value only if tool generates a primary output (ex: linker)
	STDMETHOD(GetPrimaryOutputIDFromTool)(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, long* pnOutputID);	// return a value only if tool generates a primary output (ex: linker)
	STDMETHOD(GetImportLibraryFromTool)(IUnknown* pItem, BSTR* pbstrPath);	// return a value only if the tool generates an import library
	STDMETHOD(SetPrimaryOutputForTool)(IUnknown* pItem, BSTR bstrFile);		// sets the primary output for a tool
	STDMETHOD(GetDefaultOutputFileFormat)(IUnknown* pItem, BSTR* pbstrOutMacro);	// macro to use for generating default output file if the original default isn't unique
	STDMETHOD(GetProgramDatabaseFromTool)(IVCPropertyContainer* pContainer, VARIANT_BOOL bSchmoozeOnly, BSTR* pbstrPDB);
	STDMETHOD(get_IsFileTool)(VARIANT_BOOL* pbIsFileTool);
	STDMETHOD(Close)() { m_spPropertyContainer.Release(); return S_OK; }
	STDMETHOD(HasDependencies)(IVCBuildAction* pAction, VARIANT_BOOL* pbHasDependencies);
	STDMETHOD(GetDependencies)(IVCBuildAction* pAction, IVCBuildStringCollection** ppStrings, VARIANT_BOOL* bUpToDate);
	STDMETHOD(CanScanForDependencies)(VARIANT_BOOL* pbIsScannable);
	STDMETHOD(GetCleanCommandLineEx)( IVCBuildAction *pAction, IVCBuildableItem* pBuildableItem, IVCBuildEngine* pBuildEngine, IVCBuildErrorContext* pEC, BSTR* bstrCmd );
	STDMETHOD(GetCommandLinesForClean)( IVCBuildActionList* pActions, bldAttributes attrib, IVCBuildEngine* pBldEngine, IVCBuildErrorContext* pErrorContext, IVCCommandLineList** ppCmds );
	STDMETHOD(GetRebuildCommandLineEx)( IVCBuildAction *pAction, IVCBuildableItem* pBuildableItem, IVCBuildEngine* pBuildEngine, IVCBuildErrorContext* pEC, BSTR* bstrCmd );
	STDMETHOD(GetCommandLinesForRebuild)(IVCBuildActionList* pActions, bldAttributes attrib, IVCBuildEngine* pBldEngine, IVCBuildErrorContext* pErrorContext, IVCCommandLineList** ppCmds);
	STDMETHOD(IsSpecialConsumable)(LPCOLESTR szPath, VARIANT_BOOL* pbSpecial);
	STDMETHOD(DirtyCommandLineOptions)(long nLowPropID, long nHighPropID, VARIANT_BOOL bDirty);
	STDMETHOD(AffectsTool)(long nLowPropID, long nHighPropID, VARIANT_BOOL* pbAffects);
	STDMETHOD(CommandLineOptionsAreDirty)(IVCPropertyContainer* pPropContainer, VARIANT_BOOL* pbIsDirty);
	STDMETHOD(OutputsAreDirty)(IVCPropertyContainer* pPropContainer, VARIANT_BOOL* pbIsDirty);
	STDMETHOD(ClearDirtyOutputs)(IVCPropertyContainer* pPropContainer);
	STDMETHOD(IsMultiProp)(long idProp, VARIANT_BOOL* pbIsMulti);
	STDMETHOD(GetMultiPropSeparator)(long id, BSTR* pbstrPreferred, BSTR* pbstrAll);
	STDMETHOD(HasVirtualLocalStorage)(IVCPropertyContainer* pPropContainer, VARIANT_BOOL* pbHasVirtualLocalStorage);
	STDMETHOD(CreatePageObject)(IUnknown **, CLSID*, IVCPropertyContainer *, IVCSettingsPage * ) {return E_NOTIMPL;};
	STDMETHOD(GenerateDependencies)(IVCBuildAction* pAction, void* frhFile, IVCBuildErrorContext* pErrorContext);
	STDMETHOD(ScanDependencies)(IVCBuildAction* pAction, IVCBuildErrorContext* pEC, VARIANT_BOOL bWriteOutput);
	STDMETHOD(InformUserAboutBadFileName)(IVCBuildErrorContext* pEC, IVCPropertyContainer* pPropContainer, long nErrPropID, 
		LPCOLESTR szFile);
	STDMETHOD(get_FullIncludePathInternal)(BSTR* pbstrFullIncludePath);
	STDMETHOD(GetDeploymentDependencies)(IVCBuildAction* pAction, IVCBuildStringCollection** ppDeployDepStrings);
	STDMETHOD(GetDeployableOutputsCount)(IVCPropertyContainer* pPropContainer, long* pnOutputs);	// number of deployable outputs
	STDMETHOD(GetDeployableOutputs)(IVCPropertyContainer* pPropContainer, IVCBuildStringCollection** ppDeployableOutputs);	// the deployable outputs

// helpers
public:	
	virtual COptionHandlerBase* GetOptionHandler() { return NULL; }
	virtual HRESULT GetPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, CPathW& rPath);	// return a value only if tool generates a primary output (ex: linker)

	virtual long GetPageCount() { return 0; }
	virtual GUID* GetPageIDs() { return (GUID*)NULL; }
	virtual BOOL HaveCommandLine(IVCBuildAction* pAction) { return TRUE; }
	virtual BOOL UsesResponseFiles() { return FALSE; }
	virtual void SaveSpecialProps(IStream *xml, IVCPropertyContainer *pPropCnt) { return; }

	HRESULT ToolGetStrProperty(long idProp, BSTR* pVal, bool bLocalOnly = false);
	HRESULT ToolGetIntProperty(long idProp, long* pVal);
	HRESULT ToolGetBoolProperty(long idProp, VARIANT_BOOL* pVal);

	void GetEnumVal(IUnknown *pUnk, SOptionEntry *pEntry, CComVariant &var, IDispatch *pDispTool);
	BOOL FileNameMatchesExtension(const CPathW *pPath, const wchar_t *pExtensions);
	
	static BOOL ResolveIncludeDirectivesToPath(const CPathW& pathSrc, IVCBuildAction* pAction, const CVCStringWList& strlstIncs, CVCStringWList& lstPath, BOOL fIsInclude = TRUE);
	static void ConvertPathToBSTR(CPathW& path, CComBSTR& rbstrPath);
	static void CreateDirFromBSTR(BSTR bstrPath, CDirW& rdir);
	static BOOL CreatePathRelativeToProject(IVCPropertyContainer* pPropContainer, CStringW& strName, CPathW& pathName);
	static BOOL ConfigBuildProp(long idProp);

protected:
	static BOOL GetIncludePathI(IVCBuildAction* pAction, CStringW& strIncDirs, const CPathW* ppathSrc, CStringW& strPlatDirs, BOOL fIsInclude);
	static BOOL ResolveIncludeDirectivesI(IVCBuildAction* pAction, CStringW& strlstIncDirs, const CDirW& dirBase, const CVCStringWList& strlstIncs, CVCStringWList& lstPath, const CPathW *pathSrc, CStringW& strPlatDirs, BOOL fIsInclude);

	virtual HRESULT GetCommandLineOptions(IUnknown* pItem, IVCBuildAction* pAction, VARIANT_BOOL bIncludeAdditional, VARIANT_BOOL bForDisplay, commandLineOptionStyle fStyle, CStringW& rstrCmdLine);
	HRESULT DoGetPropertyOption(BSTR bstrProp, long dispidProp, BSTR *pVal) { return E_NOTIMPL; }	// DIANEME_TODO: CVCToolImpl::DoGetPropertyOption
	BOOL IsExcludedFromBuild(IVCPropertyContainer* pPropContainer);
	HRESULT GetProcessedAdditionalOptions(IUnknown* pItem, long idProp, VARIANT_BOOL bForSave, VARIANT_BOOL bSkipLocal, BSTR* pbstrAdditionalOptions);
	HRESULT DoMatchName(BSTR bstrName, const wchar_t* szToolType, const wchar_t* szShortName, VARIANT_BOOL* pbMatches);
	HRESULT DoGetDefaultExtensions(CComBSTR& bstrSetExtensions, const wchar_t* const wszBaseDefaultExtensions, BSTR* pbstrVal);
	void BuildCommandLineBatchFile(IVCBuildAction* pAction, CComBSTR& bstrCommandLineEnd, CComBSTR& bstrCmdLine);
	HRESULT DoGetVCProjectEngine(IDispatch** projEngine);
	virtual BOOL IsExtraMultiProp(long idProp) { return FALSE; }	// only override this if you have a multi-prop not in an option table
	virtual HRESULT GetAdditionalDependenciesInternal(IVCPropertyContainer* pItem, IVCBuildAction* pAction, BOOL bForSave, BSTR* pbstrInputs);		// additional things to add to command line

	virtual LPCOLESTR GetToolFriendlyName() PURE;
	virtual long GetToolID() PURE;
#ifndef _SHIP
	HRESULT ForceDirty();
#endif	// _SHIP

protected:
	CComPtr<IVCPropertyContainer> m_spPropertyContainer;
	long m_nLowKey;
	long m_nHighKey;
	long m_nDirtyKey;
	long m_nDirtyOutputsKey;
};
