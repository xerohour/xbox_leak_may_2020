// VCProjConvert.h : Declaration of the CVCProjConvert

#ifndef __VCPROJCONVERT_H_
#define __VCPROJCONVERT_H_

#include "resource.h"       // main symbols
#include "bldpack.h"

extern _ATL_REGMAP_ENTRY g_rm[];

/////////////////////////////////////////////////////////////////////////////
// CVCProjConvert
class ATL_NO_VTABLE CVCProjConvert : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CVCProjConvert, &CLSID_VCProjConvert>,
	public CPackage,
	public IVCProjConvert
{
public:
	CVCProjConvert()
	{
		m_bInit = false;
	}
	~CVCProjConvert();

	static HRESULT WINAPI UpdateRegistry(BOOL bRegister) {
		return _Module.UpdateRegistryFromResource( IDR_VCPROJCONVERT, bRegister, g_rm );
	}

BEGIN_COM_MAP(CVCProjConvert)
	COM_INTERFACE_ENTRY(IVCProjConvert)
END_COM_MAP()

// IVCProjConvert
public:
	STDMETHOD(Initialize)(BSTR bstrSourceFilesString);
	STDMETHOD(GetPackage)(long **pPackage);
	STDMETHOD(OpenProject)(BSTR bstrProjectPath, IUnknown *pProjEngine, IUnknown *pUnkSP);
	STDMETHOD(GetProject)(IDispatch **ppDisp);
	STDMETHOD(get_HasProjectDependencies)( VARIANT_BOOL *pbHasDeps );
	STDMETHOD(get_ProjectDependenciesCount)( long *plNumDeps );
	STDMETHOD(GetNextProjectDependency)( BSTR* pbstrProject, BSTR* pbstrDependentProject );
	STDMETHOD(ResetProjectDependencyEnum)();

// CPackage
	BOOL DoBldSysCompRegister(CProjComponentMgr * pcompmgr, DWORD blc_type, DWORD blc_id);
protected:
	bool m_bInit;
	CComPtr<VCProject> m_pProject;
	// map of project dependencies
	CVCMapStringToString m_Dependencies;
	VCPOSITION m_DependencyEnumPos;

public:
	static CString s_strSourceFilesString;	// the one, the only, UI string this DLL needs...

protected:
	void AddFolders(VCProject *pProject, CSlob *pSlob);
	void AddSubFolders(VCFilter *pFilter, CSlob *pSlob);
	void AddFolderForFile(VCProject *pProject, CSlob *pSlob, VCFile *pFile, VCFilter **ppFilter = NULL);
	COptStr * GetOptStrForTool(LPCTSTR szTool,  COptionHandler * &popthdlr, long& lItem);
	void SetToolSettings(CProjItem *pFileItem, CProject* pProject, LPCTSTR szTool, VCFileConfiguration *pFileConfig, VCConfiguration *pConfig);
	void SetLinkerDefSettings(CProjItem *pFileItem, VCFileConfiguration *pFileConfig);
	void SetToolObject(VCFileConfiguration *pFileConfig, LPCTSTR szTool, IVCToolImpl **ppTool);
	void SetToolObject(VCConfiguration *pConfig, LPCTSTR szTool, IVCToolImpl **ppTool);
	void SetToolSetting(IVCToolImpl *pTool,  IVCPropertyContainer* pPropContainer, CComVariant& varVal, 
		COptStr * pOptStr,  CProjItem *pProjItem, COptionHandler *pOptHandler, bool bIsBase);
	bool GetConfigName(CString& strTargetName, CString& strConfigName, BOOL bFileConfig = FALSE );
	bool SplitOutV2ConfigName(UINT idPlatform, CString& strTargetName, CString& strConfigName, BOOL bFileConfig = FALSE);
	BOOL IsToolMatch(IVCToolImpl *pTool, LPCTSTR szTool, bool bLinkerAllowed);
	BOOL SetCLToolSetting(IVCToolImpl *pTool, IVCPropertyContainer* pPropContainer, CComVariant& varVal, 
		COptStr * pOptStr,  CProjItem *pProjItem, COptionHandler *pOptHandler, bool bIsBase);
	BOOL SetLNKToolSetting(IVCToolImpl *pTool, IVCPropertyContainer* pPropContainer, CComVariant& varVal, 
		COptStr * pOptStr,  CProjItem *pProjItem, COptionHandler *pOptHandler, bool bIsBase);
	BOOL SetLIBToolSetting(IVCToolImpl *pTool, IVCPropertyContainer* pPropContainer, CComVariant& varVal, 
		COptStr * pOptStr,  CProjItem *pProjItem, COptionHandler *pOptHandler);
	BOOL SetMIDLToolSetting(IVCToolImpl *pTool, IVCPropertyContainer* pPropContainer, CComVariant& varVal, 
		COptStr * pOptStr,  CProjItem *pProjItem, COptionHandler *pOptHandle, bool bIsBaser);
	BOOL SetRCToolSetting(IVCToolImpl *pTool, IVCPropertyContainer* pPropContainer, CComVariant& varVal, 
		COptStr * pOptStr,  CProjItem *pProjItem, COptionHandler *pOptHandler, bool bIsBase);
	BOOL SetBSCToolSetting(IVCToolImpl *pTool, IVCPropertyContainer* pPropContainer, CComVariant& varVal, 
		COptStr * pOptStr,  CProjItem *pProjItem, COptionHandler *pOptHandler);
	BOOL AddCustomBuildEventItem(CProjItem *pItem, VCConfiguration *pFileConfiguration);
	BOOL AddCustomBuildItem(CProjItem *pItem, VCCustomBuildTool* pCustomBuildTool, VCFileConfiguration *pFileConfig, VCConfiguration *pConfig);
	HRESULT CreateEmptyProject(CProject* pProject, int num_project, BSTR bstrProjectPath, IUnknown* pUnkSP, 
		VCProjectEngine* pProjEngine, VCProject** ppNewProject);
	void AddRelevantPlatforms(CProject* pProject, CStringList& lststrPlatforms, VCProject* pNewProject);
	bool IsUnsupportedProject(CProject* pProject);
	void SetConfigurationType(CProjType* pProjType, VCConfiguration* pConfig);
	void AddNewConfiguration(CString& strTargetName, CString& strConfigName, VCProject* pNewProject, BOOL bCheckForExisting,
		VCConfiguration** ppNewConfig);
	void GetConfig(CString& strTargetName, CString& strConfigName, IVCCollection* pConfigs, VCConfiguration** ppNewConfig);
	void CreatePlatformsList(CStringList& lststrPlatforms);
	void DoMakefileProjectProperties( CProject* pProject, VCProject* pNewProject, VCConfiguration *pConfig );
	void DoGenericAndSpecialCaseProperties(CProject* pProject, VCConfiguration* pConfig);
	void HandleEvent(CProject* pProject, VCConfiguration* pConfig, UINT idCmd, UINT idDesc, 
		CString& strEventName);
	void DoStyleSheetFixups(VCProject* pNewProject, VCConfiguration* pConfig);
	void DoSharedFileDelete(VCConfiguration* pConfig);
	bool RemoveSymbols(IVCPropertyContainer* pPropContainer, long idProp, BSTR bstrSeparatorList, BSTR bstrSymbols);
	void ClearBoolProperty(IVCPropertyContainer* pPropContainer, long idProp, VARIANT_BOOL bDefault);
	void ClearIntProperty(IVCPropertyContainer* pPropContainer, long idProp, long nDefault);
	int	 FindFirstSeparator(CStringW& strList, CStringW& strSeparatorList, int nStartIdx);
	int	 GetNextItem(CStringW& strList, int nStartIdx, int nMaxIdx, CStringW& strSeparator, CStringW& strItem);
	int	 GetNextQuotedItem(CStringW& strList, int nStartIdx, int nMaxIdx, int nSep, int nLastQuote,
		BOOL bOddQuote, CStringW& strSeparator, CStringW& strItem);
	bool RemoveItem(CStringW& strList, CStringW& strItem, CStringW& strSeparatorList);
	int	 FindItem(CStringW& rstrItems, CStringW& rstrFindItem, CStringW& strSeparator, int nStart);
	void RemoveMultiples(CStringW& strList, CStringW& strSeparatorList);
	void HandleFileProperties(CProject* pProject, VCProject* pNewProject);
	HRESULT GetNamedProjectConfiguration(CString& strTarget, VCProject* pNewProject, 
		VCConfiguration** ppNewConfig);
	HRESULT GetNamedFileConfiguration(CString& strTarget, VCFile* pFile, VCFileConfiguration** ppNewFileConfig);
	HRESULT DoATLProjectConversionHack(VCProject* pNewProject, bool bHasRelease, bool bHasReleaseMinSize);
	BOOL DoSpecialLinkerProps(IVCToolImpl* pTool, IVCPropertyContainer* pPropContainer, COptionHandler* pOptHandler, 
		CProjItem* pFileItem);
	BOOL DoSpecialMidlProps(IVCToolImpl* pTool, COptionHandler* pOptHandler, CProjItem* pFileItem);
	BOOL DoSpecialUnknownStringProps(IVCToolImpl* pTool, IVCPropertyContainer* pPropContainer, COptionHandler* pOptHandler, 
		CProjItem* pFileItem);
	BOOL DoUnknownStringProp(IVCPropertyContainer* pPropContainer, COptionHandler* pOptHandler, CProjItem* pFileItem, 
		long nOptionID);
	void DoUnknownStringPropHelper(CProjItem* pProjItem, UINT nIDUnknown, IVCPropertyContainer* pPropContainer, 
		long nOptionID);
	void GetUnknownToken(CString& rstrString, CString& rstrOpt, int& rnIdx);
	void CleanupDirSpec(CString& strDir);

	BOOL PropertyIsDifferent(CComBSTR& bstrOld, CComBSTR& bstrNew, BOOL bBlankOK = FALSE);
	BOOL PropertyIsDifferent(CComBSTR& bstrOld, BSTR bstrNew, BOOL bBlankOK = FALSE);
	VARIANT_BOOL ConvertToBoolValue(long lVal);
	BOOL EnumIsTrue(long lVal);
	void AddExtraOption(IVCPropertyContainer* pPropContainer, long propID, long lVal, BSTR bstrOption);
	void AddAdditionalOptions(IVCPropertyContainer* pPropContainer, long propID, BSTR bstrOption);
	void SetBoolProperty(IVCPropertyContainer* pPropContainer, long propID, long lVal, BOOL bFlipIt = FALSE);
	void SetStrProperty(IVCPropertyContainer* pPropContainer, long propID, BSTR bstrNewVal, BOOL bBlankOK = FALSE);
	void SetMultiStrProperty(IVCPropertyContainer* pPropContainer, long propID, BSTR bstrNewVal, bool bIsBase);
	void SetEnumProperty(IVCPropertyContainer* pPropContainer, long propID, long nNewValue);
	void SetIntProperty(IVCPropertyContainer* pPropContainer, long propID, long nNewValue, BOOL bZeroOK = FALSE);

	void SetPtrToMember1(IVCPropertyContainer* pPropContainer, long lVal);
	void SetPtrToMember2(IVCPropertyContainer* pPropContainer, long lVal);
	void SetDebugInfo(VCCLCompilerTool* pCompiler, long lVal);
	void SetOptimization(VCCLCompilerTool* pCompiler, long lVal);
	void SetOptimizeX(VCCLCompilerTool* pCompiler, long lVal);
	void SetCodeGenFavor(VCCLCompilerTool* pCompiler, UINT idOption, long lVal);
	void SetStringPooling(VCCLCompilerTool* pCompiler, long lVal);
	void SetAsmOptions(VCCLCompilerTool* pCompiler, CProjItem* pProjItem, COptionHandler* pOptHandler, long lVal);
	void SetListBsc(VCCLCompilerTool* pCompiler, IVCPropertyContainer* pPropContainer, CProjItem* pProjItem, 
		COptionHandler* pOptHandler, UINT idOption, long lVal);
	void SetAutoPCH(VCCLCompilerTool* pCompiler, IVCPropertyContainer* pPropContainer,
		COptionHandler* pOptHandler, CProjItem* pProjItem, long lVal);
	void SetCreatePCH(VCCLCompilerTool* pCompiler, IVCPropertyContainer* pPropContainer, 
		COptionHandler* pOptHandler, CProjItem* pProjItem, long lVal);
	void SetUsePCH(VCCLCompilerTool* pCompiler, IVCPropertyContainer* pPropContainer, 
		COptionHandler* pOptHandler, CProjItem* pProjItem, long lVal);
	void SetStackCheck(IVCPropertyContainer* pPropContainer, long lVal);
	void SetPreprocess1(VCCLCompilerTool* pCompiler, UINT idOption, long lVal);
	void SetPreprocess2(VCCLCompilerTool* pCompiler, CProjItem* pProjItem, long lVal);
	void SetTreatFileAs(VCCLCompilerTool* pCompiler, UINT idOption, long lVal);
	void SetProcOptimize(VCCLCompilerTool* pCompiler, long lVal);
	void SetCallConvention(VCCLCompilerTool* pCompiler, long lVal);
	void SetRuntimeChecks(VCCLCompilerTool* pCompiler, long lVal, basicRuntimeCheckOption newOpt);
	void SetByteAlign(VCCLCompilerTool* pCompiler, long lVal);
	void SetRuntimeLib(VCCLCompilerTool* pCompiler, long lVal);
	void SetOldEH(VCCLCompilerTool* pCompiler, long lVal);
	void SetLinkShowProgress(VCLinkerTool* pLinker, long lVal);
	void SetUsePDB(IVCPropertyContainer* pPropContainer, CProjItem* pProjItem, COptionHandler* pOptHandler, 
		long lVal);
	void SetMapGen(IVCPropertyContainer* pPropContainer, CProjItem* pProjItem, COptionHandler* pOptHandler, 
		long lVal);
	void SetLinkerSubSystem(IVCPropertyContainer* pPropContainer, long lVal);
	void SetLinkerLargeAddressAwareness(IVCPropertyContainer* pPropContainer, long lVal);
	void SetLinkerVersion(IVCPropertyContainer* pPropContainer, CProjItem* pProjItem, 
		COptionHandler* pOptHandler, long lVal);
	void SetLinkerDriver(IVCPropertyContainer* pPropContainer, long lVal);
	void SetLinkerAlign(IVCPropertyContainer* pPropContainer, long lVal);
	void SetLinkerFixed(IVCPropertyContainer* pPropContainer, long lVal);
	void SetLinkIncremental(VCLinkerTool* pLinkTool, BOOL bSetIt);
	void SetMidlOutput(IVCPropertyContainer* pPropContainer, long lPropID, BSTR bstrVal);
	void SetMidlStublessProxy(IVCPropertyContainer* pPropContainer, long lVal);
	void SetMidlChar(IVCPropertyContainer* pPropContainer, long lVal);
	void SetMidlClient(IVCPropertyContainer* pPropContainer, long lVal);
	void SetMidlServer(IVCPropertyContainer* pPropContainer, long lVal);
};

#endif //__VCPROJCONVERT_H_
