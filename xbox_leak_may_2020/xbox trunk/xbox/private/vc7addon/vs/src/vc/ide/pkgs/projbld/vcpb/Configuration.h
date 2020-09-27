// Configuration.h: Definition of the CConfiguration class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGURATION_H__A54AAE82_30C2_11D3_87BF_A04A4CC10000__INCLUDED_)
#define AFX_CONFIGURATION_H__A54AAE82_30C2_11D3_87BF_A04A4CC10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "configrecords.h"
#include "settingspage.h"
#include "StyleSheet.h"

// Warning C4584 : 'C' : base-class 'A' is already a base-class of 'B'
//
//  A     
//   \
//    B   A
//     \ /
//      C
// 

#pragma warning ( disable : 4584 )

/////////////////////////////////////////////////////////////////////////////
// CConfiguration

class CConfiguration : 
	public IDispatchImpl<VCConfiguration, &__uuidof(VCConfiguration), &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public IVCConfigurationImpl,
	public IVCBuildableItem,
	public CComObjectRoot,
	public CStyleSheetBase
{
public:
	CConfiguration();
	~CConfiguration();

	static HRESULT CreateInstance(VCConfiguration **ppConfiguration, LPCOLESTR szConfigName, VCPlatform *pPlatform, VCProject *pProject);
	HRESULT Initialize(LPCOLESTR szConfigName, VCPlatform *pPlatform, VCProject *pProject);

BEGIN_COM_MAP(CConfiguration)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(VCConfiguration)
	COM_INTERFACE_ENTRY(IVCConfigurationImpl)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
	COM_INTERFACE_ENTRY(IVCBuildableItem)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CConfiguration) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

DECLARE_NO_REGISTRY()

	// VCConfiguration
public:
	STDMETHOD(get_Name)(BSTR * pVal);
	STDMETHOD(put_Name)(BSTR pVal);
	STDMETHOD(get_Platform)(IDispatch * * pVal);
	STDMETHOD(get_OutputDirectory)(BSTR * pVal);
	STDMETHOD(put_OutputDirectory)(BSTR pVal);
	STDMETHOD(get_IntermediateDirectory)(BSTR * pVal);
	STDMETHOD(put_IntermediateDirectory)(BSTR pVal);
	STDMETHOD(get_DeleteExtensionsOnClean)(BSTR* pbstrExt);	// wildcard extension list to delete on clean/rebuild from within the intermediate directory
	STDMETHOD(put_DeleteExtensionsOnClean)(BSTR bstrExt);
	STDMETHOD(Delete)();
	STDMETHOD(Build)();
	STDMETHOD(Rebuild)();
	STDMETHOD(Clean)();
	STDMETHOD(get_DebugSettings)(IDispatch * * pVal);
	STDMETHOD(get_PrimaryOutput)(BSTR * pVal);
	STDMETHOD(get_ImportLibrary)(BSTR* pVal);
	STDMETHOD(get_ProgramDatabase)(BSTR* pbstrPDB);
	STDMETHOD(get_Project)(IDispatch * * pVal);
	STDMETHOD(MatchName)(BSTR bstrNameToMatch, VARIANT_BOOL bFullOnly, VARIANT_BOOL *pbMatched);
	STDMETHOD(get_Tools)(/*[out, retval]*/ IDispatch * *pVal);
	STDMETHOD(get_ConfigurationName)(BSTR * pVal);
	STDMETHOD(put_ConfigurationName)(BSTR pVal);
	STDMETHOD(get_FileTools)(/*[out, retval]*/ IVCCollection * *pVal);
	STDMETHOD(get_UpToDate)(VARIANT_BOOL* pbUpToDate);
	STDMETHOD(get_ConfigurationType)(ConfigurationTypes* pconfigType);
	STDMETHOD(put_ConfigurationType)(ConfigurationTypes configType);
	STDMETHOD(CopyTo)(IDispatch *pDestCfg);
	STDMETHOD(get_AppliedStyleSheets)(BSTR* pbstrStyles);
	STDMETHOD(put_AppliedStyleSheets)(BSTR bstrStyles);
	STDMETHOD(get_BuildBrowserInformation)(VARIANT_BOOL* pbBsc);	// build .bsc file
	STDMETHOD(put_BuildBrowserInformation)(VARIANT_BOOL bBsc);
	STDMETHOD(get_UseOfMFC)(useOfMfc* useMfc);	// how are we using MFC?
	STDMETHOD(put_UseOfMFC)(useOfMfc useMfc);
	STDMETHOD(get_UseOfATL)(useOfATL* useATL);	// how are we using ATL?
	STDMETHOD(put_UseOfATL)(useOfATL useATL);
	STDMETHOD(get_ATLMinimizesCRunTimeLibraryUsage)(VARIANT_BOOL* pbUseCRT);	// does the user want to link with the static or dynamic CRT?
	STDMETHOD(put_ATLMinimizesCRunTimeLibraryUsage)(VARIANT_BOOL bUseCRT);
	STDMETHOD(get_CharacterSet)(charSet* poptSetting);	// character set to use: Unicode, MBCS, default
	STDMETHOD(put_CharacterSet)(charSet optSetting);
	STDMETHOD(get_ManagedExtensions)(VARIANT_BOOL* bManaged);	// is this a managed C++ extensions project?
	STDMETHOD(put_ManagedExtensions)(VARIANT_BOOL bManaged);
	STDMETHOD(get_RegisterOutput)(VARIANT_BOOL* pbRegister);	// register the primary output of the build
	STDMETHOD(get_WholeProgramOptimization)(VARIANT_BOOL* pbOptimized);	// perform whole program optimization on build; more or less requires optimization turned on to be effective
	STDMETHOD(put_WholeProgramOptimization)(VARIANT_BOOL bOptimize);
	STDMETHOD(Evaluate)(BSTR bstrIn, BSTR* pbstrOut) { return CStyleSheetBase::Evaluate(bstrIn, pbstrOut); }
	STDMETHOD(get_StyleSheets)(IDispatch** styles) { return CStyleSheetBase::DoGetStyleSheets(styles); }
	STDMETHOD(get_VCProjectEngine)(IDispatch** projEngine)	{ return DoGetVCProjectEngine(projEngine); }

// VCConfiguration helpers
protected:
	HRESULT DoSetConfigurationType(ConfigurationTypes configType, BOOL bForceUpdate = FALSE);

// IVCConfigurationImpl
public:
	STDMETHOD(SetDefaultToolForFile)(IDispatch *pDispFileConfiguration);
	STDMETHOD(get_IsValidConfiguration)(VARIANT_BOOL* pbIsValidConfiguration);
	STDMETHOD(get_IsBuildable)(VARIANT_BOOL* pbIsBuildable);
	STDMETHOD(get_CanStartBuild)(VARIANT_BOOL* pbCanStartBuild);
	STDMETHOD(get_BaseToolList)(void** ppPtrList);
	STDMETHOD(GenerateToolWrapperList)();
	STDMETHOD(ClearToolWrapperList)();
	STDMETHOD(Close)();
	STDMETHOD(put_Project)(IDispatch * pVal);
	STDMETHOD(SaveObject)(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent);
	STDMETHOD(WriteToStream)(IStream *pStream);
	STDMETHOD(ReadFromStream)(IStream *pStream);
	STDMETHOD(TopLevelBuild)(bldActionTypes buildType, ULONG cFiles, VCFile* pFiles[], IVCBuildEngine* pBuildEngine,
		IVCBuildErrorContext* pEC, BOOL bRecurse, BOOL fDoIdleAndPump, BOOL fCleanupEngine, BOOL bProvideBanner, long* warnings, 
		long* errors, BOOL* bCancelled);
	STDMETHOD(get_MrePath)(BSTR* pbstrMrePath);
	STDMETHOD(get_Dependencies)(IVCBuildStringCollection** ppDependencies);
	STDMETHOD(get_BuildOutputs)(IVCBuildStringCollection** ppOutputs);
	STDMETHOD(get_ProjectDependencies)(IVCBuildOutputItems** ppProjDependencies);
	STDMETHOD(get_ProjectDependenciesAsStrings)(IVCBuildStringCollection** ppProjDependencies);
	STDMETHOD(get_HasProjectDependencies)(VARIANT_BOOL* pbHasDeps);
	STDMETHOD(get_KnownDeploymentDependencies)(IVCBuildStringCollection** ppDeployDependencies);
	// WARNING: Clone must only be called on a brand new config!!
	STDMETHOD(Clone)(IVCPropertyContainer *pSource );
	STDMETHOD(SupportsBuildType)(bldActionTypes buildType, VARIANT_BOOL* pbSupports, VARIANT_BOOL* pbAvailable);
	STDMETHOD(CreatePageObject)(IUnknown **, CLSID*, IVCPropertyContainer *pSource, IVCSettingsPage * );
	STDMETHOD(CreatesUniqueOutputName)(IUnknown* pItem, VARIANT_BOOL* pbUnique);	// will this item have a unique output name?
	STDMETHOD(GetPrimaryOutputFileID)(IVCToolImpl** ppTool, long* pnOutputID);
	STDMETHOD(get_ErrorContext)(IVCBuildErrorContext** ppEC);
	STDMETHOD(AddTool)(LPCOLESTR szToolName, IDispatch** ppTool);	// adds a new tool to the config
	STDMETHOD(get_DeployableOutputsCount)(long* pnOutputs);	// number of deployable outputs
	STDMETHOD(get_DeployableOutputs)(IVCBuildStringCollection** ppDeployableOutputs);	// the deployable outputs
	STDMETHOD(DirtyOutputs)(long nProjID);	// dirty outputs based on the passed-in project ID

// IVCPropertyContainer; most methods are in a base class
public:
	STDMETHOD(DirtyProp)(long id);

// IVCBuildableItem
public:
	STDMETHOD(get_ItemFileName)(BSTR *pVal);
	STDMETHOD(get_ItemFullPath)(BSTR* pbstrFullPath);
	STDMETHOD(get_ActionList)(IVCBuildActionList** ppActions);
	STDMETHOD(get_FileRegHandle)(void** pfrh);
	STDMETHOD(AssignActions)(VARIANT_BOOL bOnLoad);
	STDMETHOD(UnAssignActions)(VARIANT_BOOL bOnClose);
	STDMETHOD(get_ContentList)(IEnumVARIANT** ppContentList);
	STDMETHOD(get_Registry)(void** ppFileRegistry);
	STDMETHOD(get_ProjectConfiguration)(VCConfiguration** ppProjCfg);
	STDMETHOD(get_ProjectInternal)(VCProject** ppProject);
	STDMETHOD(get_ExistingBuildEngine)(IVCBuildEngine** ppBuildEngine);
	STDMETHOD(get_PersistPath)(BSTR* pbstrPersistPath);
	STDMETHOD(RefreshActionOutputs)(long nPropID, IVCBuildErrorContext* pEC);
	STDMETHOD(HandlePossibleCommandLineOptionsChange)(IVCBuildErrorContext* pEC);
	STDMETHOD(ClearDirtyCommandLineOptionsFlag)();

// ISpecifyPropertyPages
public:
    STDMETHOD(GetPages)( /* [out] */ CAUUID *pPages );
       
// IVCConfigurationImpl helpers
	HRESULT DoPreparedBuild(ULONG cFiles, VCFile* pFiles[], bldActionTypes buildType, BOOL bContinueAfterErrors, 
		IVCBuildErrorContext* pEC, BuildType bt, BOOL bRecurse, BuildResults* pResults);
	void InitializeDepGraph(CBldCfgRecord* pCfgRecord);
	void DoInitializeDepGraph(CBldCfgRecord** ppCfgRecord);
	void InitActions(IVCBuildActionList* pBldActions);
	void LateBindActions(IVCBuildActionList* pBldActions);
	HRESULT DoBuild(bldActionTypes bldType);
	virtual HRESULT DoClearToolWrapperList();

	// IVCPropertyContainer helpers
	virtual HRESULT DoGetStrProperty(long idProp, BOOL bSpecialPropsOnly, BSTR* pbstrValue);
	virtual HRESULT DoGetBoolProperty(long idProp, BOOL bCheckSpecial, VARIANT_BOOL* pbValue);
	virtual HRESULT DoSetIntProperty(long idProp, BOOL bCheckSpecialProps, long nValue, long nOverrideID = -1);
	virtual HRESULT DoSetBoolProperty(long id, BOOL bCheckSpecialProps, VARIANT_BOOL bValue, long nOverrideID = -1);
	virtual HRESULT FinishSetPropOrClear(long id, long nOverride);

	// IVCBuildableItem helpers
	HRESULT GetNextFileConfig(IEnumVARIANT* pEnum, VCFileConfiguration** ppFileCfg);
	HRESULT GetNextFileConfig(IEnumVARIANT* pEnum, IVCBuildableItem** ppBldableFileCfg);
	HRESULT StartCommandLineOptionChange(IVCBuildErrorContext* pEC);
	HRESULT RefreshCommandLineOptionsAndOutputs(IVCBuildErrorContext* pEC);
	HRESULT DirtyActionOutputsAndCommandLineOptions(long nLowPropID, long nHighPropID, long nOverride);

// LOCAL METHODS
public:
	HRESULT SetDirty(VARIANT_BOOL bDirty);
	static HRESULT SGetConfigurationType(ConfigurationTypes* pconfigType);

protected:
	HRESULT GetFileListEnumerator(IEnumVARIANT** ppFileList);
	virtual void DoGetToolSetType(toolSetType& listStyle);
	virtual HRESULT VerifyFileTools();
	void ClearToolExtensionList();
	void InitToolExtensionList(BOOL bReInit = FALSE);
	void InitToolExtensionListForOneTool(IVCToolImpl* pToolImpl);
	void CleanUpEngine(BOOL fCleanupEngine);
	HRESULT HandleReadingDebugSettings(IStream* pStream);
	HRESULT HandleReadingToolSettings(IStream* pStream, CComBSTR& bstrName);
	HRESULT HandleReadingGeneralConfigSettings(IStream* pStream);
#ifndef _SHIP
	HRESULT ForceDirty();
#endif	// _SHIP

//DATA
protected:
	// ordered like this for packing, not because it makes logical sense
	CComQIPtr<VCProject>		m_pProject;
	CComPtr<VCDebugSettings>	m_pDebug;
	CComPtr<IVCBuildActionList> m_spActionList;
	CComDynamicListTyped<VCFileConfiguration> m_rgFileCfgs;		// collection of file cfgs for this project; temporary use only
	CComPtr<IVCBuildEngine>		m_spBuildEngine;
	CComPtr<IVCBuildErrorContext> m_spBuildErrorContext;
	CVCMapStringWToPtr			m_ExtensionMap;
	CComBSTR					m_bstrFullName;
	static CTestableSection		g_sectionBuildEngine;

	VARIANT_BOOL				m_bIsValidConfiguration;
	VARIANT_BOOL				m_bIsBuildable;
	BOOL						m_bInitExtensionList;
	BOOL						m_fDoIdleAndPump;
};

/////////////////////////////////////////////////////////////////////////////
// CVConfigSettingsPageBase
template<class T, class I>
class CVCConfigSettingsPageBase :
	public IDispatchImpl<I, &__uuidof(I), &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public CPageObjectImpl<T, VCCONFIGURATION_MIN_DISPID, VCCONFIGURATION_MAX_DISPID>,
	public CComObjectRoot
{
public:
	// IDispatch override
	STDMETHOD(Invoke)( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pexcepInfo, UINT *puArgErr )
	{
		IDispatchImpl<I, &__uuidof(I), &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>::Invoke( dispid, riid, lcid, wFlags, pdispParams, pvarResult, pexcepInfo, puArgErr );
		return S_OK;
	}

// IVCPropertyContainer
public:
	STDMETHOD(HasDefaultValue)( DISPID dispid, BOOL *pfDefault)
	{
		CHECK_POINTER_NULL(pfDefault);
		*pfDefault = TRUE;
		return S_OK;
	}
	STDMETHOD(Commit)()
	{
		CComVariant varNewConfigType;
		if (GetLocalProp(VCCFGID_ConfigurationType, &varNewConfigType) == S_OK)
		{
			CComVariant varOldConfigType;
			if (GetParentProp(VCCFGID_ConfigurationType, VARIANT_TRUE /* allow inherit */, &varOldConfigType) != S_OK)
				varOldConfigType = typeApplication;
			if (varNewConfigType != varOldConfigType)	// fastest check is to see if they're the same...
			{
				toolSetType oldToolSet = CStyleSheetBase::DetermineBaseType((ConfigurationTypes)varOldConfigType.lVal);
				toolSetType newToolSet = CStyleSheetBase::DetermineBaseType((ConfigurationTypes)varNewConfigType.lVal);
				if (oldToolSet != newToolSet)
					CVCProjectEngine::DoUpdateAfterApply();
			}
		}

		return CPageObjectImpl<T, VCCONFIGURATION_MIN_DISPID, VCCONFIGURATION_MAX_DISPID>::Commit();
	}
};

/////////////////////////////////////////////////////////////////////////////
// CVCGeneralConfigSettingsObject

class ATL_NO_VTABLE CVCProjConfigSettingsPage :
	public CVCConfigSettingsPageBase<CVCProjConfigSettingsPage, IVCProjectConfigurationSettings>,
	public ICategorizeProperties
{
public:

BEGIN_COM_MAP(CVCProjConfigSettingsPage)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCProjectConfigurationSettings)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
	COM_INTERFACE_ENTRY_IID(IID_ICategorizeProperties, ICategorizeProperties)
END_COM_MAP()


// IVCProjectConfigurationSettings
public:
	STDMETHOD(get_OutputDirectory)(BSTR *pVal);
	STDMETHOD(put_OutputDirectory)(BSTR newVal);
	STDMETHOD(get_IntermediateDirectory)(BSTR *pVal);
	STDMETHOD(put_IntermediateDirectory)(BSTR newVal);
	STDMETHOD(get_DeleteExtensionsOnClean)(BSTR* pbstrExt);	// wildcard extension list to delete on clean/rebuild from within the intermediate directory
	STDMETHOD(put_DeleteExtensionsOnClean)(BSTR bstrExt);
	STDMETHOD(get_ConfigurationType)(ConfigurationTypes* pconfigType);		// configuration type: application, DLL, etc.
	STDMETHOD(put_ConfigurationType)(ConfigurationTypes configType);
	STDMETHOD(get_BuildBrowserInformation)(VARIANT_BOOL* pbBsc);	// build .bsc file
	STDMETHOD(put_BuildBrowserInformation)(VARIANT_BOOL bBsc);
	STDMETHOD(get_UseOfMFC)(useOfMfc* useMfc);	// how are we using MFC?
	STDMETHOD(put_UseOfMFC)(useOfMfc useMfc);
	STDMETHOD(get_UseOfATL)(useOfATL* useATL);	// how are we using ATL?
	STDMETHOD(put_UseOfATL)(useOfATL useATL);
	STDMETHOD(get_ATLMinimizesCRunTimeLibraryUsage)(VARIANT_BOOL* pbUseCRT);	// does the user want to link with the static or dynamic CRT?
	STDMETHOD(put_ATLMinimizesCRunTimeLibraryUsage)(VARIANT_BOOL bUseCRT);
	STDMETHOD(get_CharacterSet)(charSet* poptSetting);	// character set to use: Unicode, MBCS, default
	STDMETHOD(put_CharacterSet)(charSet optSetting);
	STDMETHOD(get_ManagedExtensions)(VARIANT_BOOL* bManaged);	// is this a managed C++ extensions project?
	STDMETHOD(put_ManagedExtensions)(VARIANT_BOOL bManaged);
	STDMETHOD(get_WholeProgramOptimization)(VARIANT_BOOL* pbOptimized);	// perform whole program optimization on build; more or less requires optimization turned on to be effective
	STDMETHOD(put_WholeProgramOptimization)(VARIANT_BOOL bOptimize);

// ICategorizeProperties
public:
	STDMETHOD(MapPropertyToCategory)( DISPID dispid, PROPCAT* ppropcat);
	STDMETHOD(GetCategoryName)( PROPCAT propcat, LCID lcid, BSTR* pbstrName);

// IVCPropertyContainer
public:
	STDMETHOD(Commit)();
	virtual void GetBaseDefault(long id, CComVariant& varValue);

// helpers
protected:
	virtual BOOL SupportsMultiLine(long id) { return (id == VCCFGID_DeleteExtensionsOnClean); }
};

class ATL_NO_VTABLE CVCGeneralMakefileSettingsPage :
	public CVCConfigSettingsPageBase<CVCGeneralMakefileSettingsPage, IVCGeneralMakefileSettings>
{
public:

BEGIN_COM_MAP(CVCGeneralMakefileSettingsPage)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCGeneralMakefileSettings)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
END_COM_MAP()

// IVCProjectConfigurationSettings
public:
	STDMETHOD(get_ConfigurationType)(ConfigurationTypes* pconfigType);		// configuration type: application, DLL, etc.
	STDMETHOD(put_ConfigurationType)(ConfigurationTypes configType);
};


class CGeneralConfigSettingsPage :
	public CSettingsPageBase<CGeneralConfigSettingsPage, &CLSID_GeneralConfigSettingsPage, IDS_GeneralConfigSettings, 0>
{
public:
	CGeneralConfigSettingsPage() {}
// IVSPropertyPage
    STDMETHOD(get_CategoryTitle)( UINT iLevel, BSTR *pbstrCategory )
	{ return S_FALSE; }

protected:
	virtual void DoCreatePageObject(UINT i, IDispatch* pDisp, IVCPropertyContainer* pLiveContainer, IVCSettingsPage* pPage)
	{
		ConfigurationTypes configType = typeUnknown;
		pLiveContainer->GetIntProperty(VCCFGID_ConfigurationType, (long*)&configType);
		if( configType != typeUnknown )
		{
			CPageObjectImpl<CVCProjConfigSettingsPage,VCCONFIGURATION_MIN_DISPID,VCCONFIGURATION_MAX_DISPID>::CreateInstance(&m_ppUnkArray[i], pLiveContainer, pPage);
		}
		else
		{
			CPageObjectImpl<CVCGeneralMakefileSettingsPage,VCCONFIGURATION_MIN_DISPID,VCCONFIGURATION_MAX_DISPID>::CreateInstance(&m_ppUnkArray[i], pLiveContainer, pPage);
		}
	}
};


/////////////////////////////////////////////////////////////////////////////
// CBldDummyGenCfg

class CBldDummyGenCfg : 
	public CComObjectRoot,
	public IVCGenericConfiguration
{
public:
	CBldDummyGenCfg() {}
	~CBldDummyGenCfg() {}
	void Initialize(VCConfiguration *pCfg);
	static HRESULT CreateInstance(CBldDummyGenCfg** ppDummyCfg, VCConfiguration* pCfg);

BEGIN_COM_MAP(CBldDummyGenCfg)
	COM_INTERFACE_ENTRY(IVCGenericConfiguration)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CBldDummyGenCfg) 

// IVCGenericConfiguration
public:
	STDMETHOD(InformBuildBegin)();
	STDMETHOD(InformBuildEnd)(BOOL fOK);
	STDMETHOD(StartAutomationBuild)(bldActionTypes bldType);
	STDMETHOD(StartFileBuild)(ULONG celt, VCFile* pFile[]);
	STDMETHOD(SupportsBuildType)(bldActionTypes buildType, VARIANT_BOOL* pbSupports, VARIANT_BOOL* pbAvailable);
	STDMETHOD(get_NoBuildIsInProgress)(BOOL* pbNoneInProgress);	// no build currently in progress

// CBldDummyGenCfg
public:
	HRESULT DoStartBuild(bldActionTypes bldType, VCFile* pFile[] = NULL, ULONG celtFiles = 0);

// variables
protected:	
	CComPtr<VCConfiguration> m_spConfig;		// the 'real' configuration to build
	CComPtr<IVCBuildThread> m_spBuildThread;	// thread for building
	CComPtr<IVCBuildEngine> m_spBuildEngine;
};

#endif // !defined(AFX_CONFIGURATION_H__A54AAE82_30C2_11D3_87BF_A04A4CC10000__INCLUDED_)
