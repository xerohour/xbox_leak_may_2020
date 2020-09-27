// FileConfiguration.h: Definition of the CFileConfiguration class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILECONFIGURATION_H__A54AAE8C_30C2_11D3_87BF_A04A4CC10000__INCLUDED_)
#define AFX_FILECONFIGURATION_H__A54AAE8C_30C2_11D3_87BF_A04A4CC10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vccoll.h>
#include <vcmap.h>
#include "buildengine.h"
#include "settingspage.h"
#include "ProjEvaluator.h"

/////////////////////////////////////////////////////////////////////////////
// CFileConfiguration

class CFileConfiguration : 
	public IDispatchImpl<VCFileConfiguration, &IID_VCFileConfiguration, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public IVCFileConfigurationImpl,
	public IVCPropertyContainer,
	public IVCBuildableItem,
	public CProjectEvaluator,
	public CComObjectRoot
{
public:
	CFileConfiguration()
	{
		m_bIgnoreDefaultTool = VARIANT_FALSE;
		m_pPropMap = NULL;
	}
	~CFileConfiguration()
	{
		if( m_pPropMap )
			delete m_pPropMap;
	}
	static HRESULT CreateInstance(VCFileConfiguration **ppFileConfiguration);

BEGIN_COM_MAP(CFileConfiguration)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(VCFileConfiguration)
	COM_INTERFACE_ENTRY(IVCFileConfigurationImpl)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
	COM_INTERFACE_ENTRY(IVCBuildableItem)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CFileConfiguration) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

DECLARE_NO_REGISTRY()

// VCFileConfiguration
public:
	STDMETHOD(get_Parent)(/*[out, retval]*/ IDispatch* *pVal);
	STDMETHOD(get_Tool)(/*[out, retval]*/ IDispatch* *pVal);
	STDMETHOD(put_Tool)(/*[in]*/ IDispatch* newVal);
	STDMETHOD(get_ExcludedFromBuild)(/*[out, retval]*/  VARIANT_BOOL* pbExcludedFromBuild);
	STDMETHOD(put_ExcludedFromBuild)(/*[in]*/ VARIANT_BOOL bExcludedFromBuild);
	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(MatchName)(BSTR bstrNameToMatch, VARIANT_BOOL bFullOnly, VARIANT_BOOL *pbMatched);
	STDMETHOD(get_VCProjectEngine)(IDispatch** projEngine);

// IVCFileConfigurationImpl
public:
	STDMETHOD(get_File)(/*[out, retval]*/ IDispatch* *pVal);
	STDMETHOD(put_File)(/*[in]*/ IDispatch* newVal);
	STDMETHOD(get_Configuration)(/*[out, retval]*/ IDispatch* *pVal);
	STDMETHOD(put_Configuration)(/*[in]*/ IDispatch* newVal);
	STDMETHOD(get_IgnoreDefaultTool)(/*[out, retval]*/  VARIANT_BOOL* pbIgnoreDefaultTool);	// shortcut way to figure out if the user explicitly set the tool
	STDMETHOD(SaveObject)(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent);
	STDMETHOD(Close)();
	STDMETHOD(SetDefaultTool)(IVCToolImpl* pTool, BOOL bReallyIsDefault);
	// WARNING: Clone must only be called on a brand new config!!
	STDMETHOD(Clone)(IVCPropertyContainer *pSource);
	STDMETHOD(CreatePageObject)(IUnknown **, CLSID *, IVCPropertyContainer *pSource, IVCSettingsPage * );
	STDMETHOD(get_OutputFile)(BSTR* pbstrFile);	// output file name if this file gets built
	STDMETHOD(put_OutputFile)(BSTR bstrFile);
	STDMETHOD(get_DefaultOutputFileFormat)(BSTR* pbstrFormat);
	STDMETHOD(VerifyToolValid)();	// verify that the currently assigned tool works with the current config properties

// IVCPropertyContainer
public:
	STDMETHOD(ClearAll)();
	STDMETHOD(GetLocalProp)(long id, /*[out]*/ VARIANT *pvarValue);
	STDMETHOD(GetParentProp)(long id, VARIANT_BOOL bAllowInherit, VARIANT *pvarValue);
	STDMETHOD(Clear)(long id);
	STDMETHOD(GetProp)(long id, VARIANT *varValue);
	STDMETHOD(DirtyProp)(long id);
	STDMETHOD(IsMultiProp)(long id, VARIANT_BOOL* pbIsMulti);
	STDMETHOD(GetMultiProp)(long id, LPCOLESTR szSeparator, VARIANT_BOOL bSkipLocal, BSTR *varValue);
	STDMETHOD(GetMultiPropSeparator)(long id, BSTR* pbstrPreferred, BSTR* pbstrAll);
	STDMETHOD(SetProp)(long id, VARIANT varValue);
	STDMETHOD(GetStrProperty)(long idProp, BSTR* pbstrValue);
	STDMETHOD(SetStrProperty)(long idProp, BSTR bstrValue);
	STDMETHOD(GetIntProperty)(long idProp, long* pnValue);
	STDMETHOD(SetIntProperty)(long idProp, long nValue);
	STDMETHOD(GetBoolProperty)(long idProp, VARIANT_BOOL* pbValue);
	STDMETHOD(SetBoolProperty)(long idProp, VARIANT_BOOL bValue);
	STDMETHOD(GetEvaluatedStrProperty)(long idProp, BSTR* pbstrValue);
	STDMETHOD(GetEvaluatedMultiProp)(long idProp, LPCOLESTR szSeparator, VARIANT_BOOL bCollapseMultiples, 
		VARIANT_BOOL bCaseSensitive, BSTR* pbstrValue);
	STDMETHOD(Evaluate)(BSTR bstrIn, BSTR *bstrOut);
	STDMETHOD(EvaluateWithValidation)(BSTR bstrSource, long idProp, BSTR* pbstrExpanded);
	STDMETHOD(Commit)(){return E_NOTIMPL;}
	STDMETHOD(HasLocalStorage)(VARIANT_BOOL bForSave, VARIANT_BOOL* pbHasLocalStorage);
	STDMETHOD(GetStagedPropertyContainer)(VARIANT_BOOL bCreateIfNecessary, IVCStagedPropertyContainer** ppPropContainer);
	STDMETHOD(ReleaseStagedPropertyContainer)();		// intended for use ONLY by the staged property container itself

// IVCBuildableItem
public:
	STDMETHOD(get_ItemFileName)(BSTR *pVal);
	STDMETHOD(get_ItemFullPath)(BSTR* pbstrFullPath);
	STDMETHOD(get_ActionList)(IVCBuildActionList** ppActions);
	STDMETHOD(get_FileRegHandle)(void** pfrh);
	STDMETHOD(AssignActions)(VARIANT_BOOL bOnOpen);	// set up actions for being able to build
	STDMETHOD(UnAssignActions)(VARIANT_BOOL bOnClose);	// close down actions for being able to build
	STDMETHOD(get_ContentList)(IEnumVARIANT** ppContentList);
	STDMETHOD(get_Registry)(void** pFileRegistry);
	STDMETHOD(get_ProjectConfiguration)(VCConfiguration** ppProjCfg);
	STDMETHOD(get_ProjectInternal)(VCProject** ppProject);
	STDMETHOD(get_ExistingBuildEngine)(IVCBuildEngine** ppBldEngine);
	STDMETHOD(get_PersistPath)(BSTR* pbstrPersistPath);
	STDMETHOD(RefreshActionOutputs)(long nPropID, IVCBuildErrorContext* pEC);
	STDMETHOD(HandlePossibleCommandLineOptionsChange)(IVCBuildErrorContext* pEC);
	STDMETHOD(ClearDirtyCommandLineOptionsFlag)();

// ISpecifyPropertyPages
public:
	STDMETHOD(GetPages)( /* [out] */ CAUUID *pPages );

//data
protected:
	CComPtr<IVCBuildActionList> m_spActionList;
	CVCMap<long,long,CComVariant,VARIANT&> *m_pPropMap;
	CComPtr<VCConfiguration> m_pConfig;
	CComPtr<VCFile> m_pFile;
	CComPtr<IVCToolImpl> m_pTool;
	CComPtr<IVCStagedPropertyContainer> m_spStagedContainer;
	VARIANT_BOOL m_bIgnoreDefaultTool;

// LOCAL METHODS
public:
	HRESULT SetDirty(VARIANT_BOOL bDirty);
	HRESULT GetFileCfgPCR(CBldCfgRecord** ppCfgRecord);
	HRESULT DoSetTool(IVCToolImpl* pTool, BOOL bDoAssign);
	HRESULT DirtyCommandLineOptions(long nLowKey, long nHighKey);
	static HRESULT SGetExcludedFromBuild(VARIANT_BOOL* pbExcluded);

protected:
	// IVCPropertyContainer helpers
	HRESULT DoGetStrProperty(long idProp, BOOL bSpecialPropsOnly, BSTR* pbstrValue);
	HRESULT DoGetProp(long id, BOOL bCheckSpecialProps, VARIANT *pVarValue);
	void ResetRegRef(VARIANT_BOOL bReleaseRef);
	void SetToolsDirty();
	void StartSetOrClearProp(long id, BOOL& bCareAboutChange, VARIANT_BOOL& bOldVal);
	void FinishSetOrClearProp(long id, BOOL bCareAboutChange, VARIANT_BOOL bOldVal, VARIANT_BOOL bNewVal);
};

/////////////////////////////////////////////////////////////////////////////
// CVCGeneralFileConfigSettingsPage

class ATL_NO_VTABLE CVCGeneralFileConfigSettingsPage :
	public IDispatchImpl<IVCGeneralFileConfigurationSettings, &IID_IVCGeneralFileConfigurationSettings, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public CPageObjectImpl<CVCGeneralFileConfigSettingsPage,VCFILECONFIGURATION_MIN_DISPID,VCFILECONFIGURATION_MAX_DISPID>,
	public CComObjectRoot
{
public:

BEGIN_COM_MAP(CVCGeneralFileConfigSettingsPage)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCGeneralFileConfigurationSettings)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
END_COM_MAP()

// IVCGeneralFileConfigurationSettings
public:
	STDMETHOD(get_ExcludedFromBuild)(VARIANT_BOOL *pbExcluded);
	STDMETHOD(put_ExcludedFromBuild)(VARIANT_BOOL bExcluded);
	STDMETHOD(get_Tool)(int* pdwTool);
	STDMETHOD(put_Tool)(int dwTool);

// IVCPropertyContainer methods
public:
	STDMETHOD(Commit)();

// IPerPropertyBrowsing methods
public:
	STDMETHOD(GetDisplayString)(DISPID dispID, BSTR *pBstr);
	STDMETHOD(GetPredefinedStrings)(DISPID dispID, CALPOLESTR *pCaStringsOut, CADWORD *pCaCookiesOut);
	STDMETHOD(GetPredefinedValue)(DISPID dispID, DWORD dwCookie, VARIANT* pVarOut);
	STDMETHOD(HasDefaultValue)(DISPID dispid, BOOL *pfDefault);

// helper functions
public:
	virtual void ResetParentIfNeeded();

protected:
	BOOL IsDefaultTool(int nToolID);

protected:
	CComDynamicListTyped<IVCToolImpl> m_rgFileTools;	// collection of tools available for this file
	CVCStringWArray m_strToolNames;						// names to match the file tools
	int m_cTools;										// number of tools available
	int m_nCurrentTool;									// currently assigned tool
	int m_nLastTool;									// tool assigned when this page was brought up/last applied
	int m_nDefaultTool;									// ID for default tool
	CStringW m_strFileExt;								// file extension
	bool m_bAllowInherit;								// assigned tool is default tool
	bool m_bNoFileTools;								// config has no file tools
};

#endif // !defined(AFX_FILECONFIGURATION_H__A54AAE8C_30C2_11D3_87BF_A04A4CC10000__INCLUDED_)
