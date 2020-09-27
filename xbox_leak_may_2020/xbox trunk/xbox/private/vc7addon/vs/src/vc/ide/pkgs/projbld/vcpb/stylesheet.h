#pragma once

#include "bldtool.h"
#include "propcontainer.h"

#ifdef AARDVARK
#include <StyleDemo.h>
#include "settingspage.h"
#endif	// AARDVARK

#define BSCMAKE_STYLE			L"BuildBsc.vcstyle"
#define MFCSTATIC_STYLE			L"MFCStatic.vcstyle"
#define MFCDYNAMIC_STYLE		L"MFCDynamic.vcstyle"
#define MANAGED_STYLE			L"ManagedExtensions.vcstyle"
#define COREWIN_STYLE			L"CoreWin.vcstyle"
#define WINDLL_STYLE			L"WinDLL.vcstyle"
#define ATLSTATIC_STYLE			L"ATLStatic.vcstyle"
#define ATLDYNAMIC_STYLE		L"ATLDynamic.vcstyle"
#define ATLNOCRT_STYLE			L"ATLNoCRT.vcstyle"
#define ATLWITHCRT_STYLE		L"ATLWithCRT.vcstyle"
#define UNICODE_STYLE			L"UnicodeSupport.vcstyle"
#define MBCS_STYLE				L"MultiByteCharSupport.vcstyle"
#define WHOLEPROGOPTIMIZE_STYLE	L"WholeProgOptimize.vcstyle"

/////////////////////////////////////////////////////////////////////////////
// CStyleSheetBase

class CStyleSheetBase : public CPropertyContainerImpl
{
public:
	CStyleSheetBase();
	virtual ~CStyleSheetBase() {}

// Public Interface Helpers (VCConfiguration, VCStyleSheet)
protected:
	virtual HRESULT DoGetPlatform(IDispatch** ppVal);
	virtual HRESULT DoGetTools(IDispatch** ppVal);

// Private Interface Helpers (IVCConfigurationImpl, IVCStyleSheetImpl)
protected:
	virtual HRESULT InitializeTools( void );
	virtual HRESULT DoGenerateToolWrapperList();
	virtual void ClearTools();
	virtual HRESULT VerifyFileTools() { return S_OK; }
	virtual HRESULT DoGetFileTools(IVCCollection** ppVal);
	virtual HRESULT DoClearToolWrapperList();
	virtual HRESULT FinishClose();
	virtual HRESULT DoSetAppliedStyleSheets(BSTR bstrStyles, BOOL bCheckSpecial);
	virtual HRESULT LoadStyle(BSTR bstrStyle, BOOL bAddFront = TRUE, BOOL bIsSystemStyle = TRUE);
	virtual HRESULT UnloadStyle(BSTR bstrStyle);

	HRESULT AddSingleTool(IVCPropertyContainer* pPropContainer, IVCToolImpl* pTool, BOOL bGenWrapper,
		IDispatch** ppDispTool = NULL);
	HRESULT DoAddTool(LPCOLESTR szToolName, IDispatch** ppTool);

public:
	virtual HRESULT SetDirty(VARIANT_BOOL bDirty) { return E_NOTIMPL; }

// IVCPropertyContainer
public:
	STDMETHOD(IsMultiProp)(long idProp, VARIANT_BOOL* pbIsMulti);
	STDMETHOD(GetMultiPropSeparator)(long id, BSTR* pbstrPreferred, BSTR* pbstrAll);

// IVCPropertyContainer helpers
protected:
	virtual HRESULT DoGetProp(long id, BOOL bCheckSpecialProps, VARIANT *pVarValue);
	virtual HRESULT DoSetProp(long id, BOOL bCheckSpecialProps, VARIANT varValue, long nOverride = -1);
	virtual HRESULT DoClear(long id, long nOverrideID = -1);
	virtual HRESULT DoSetStrProperty(long id, BOOL bCheckSpecialProps, BSTR bstrValue, long nOverrideID = -1);
	virtual HRESULT DoSetIntProperty(long id, BOOL bCheckSpecialProps, long nValue, long nOverrideID = -1);
	virtual HRESULT DoSetBoolProperty(long id, BOOL bCheckSpecialProps, VARIANT_BOOL bValue, long nOverrideID = -1);

// LOCAL METHODS
public:
	static HRESULT GetDefaultValue(long idProp, BSTR* pbstrValue, IVCPropertyContainer* pPropContainer = NULL);
	static HRESULT GetDefaultValue(long idProp, long* pnValue, IVCPropertyContainer* pPropContainer = NULL);
	static HRESULT GetDefaultValue(long idProp, VARIANT_BOOL* pbValue, IVCPropertyContainer* pPropContainer = NULL);

	static int		FindStyle(CComBSTR& rbstrStyles, BSTR bstrFindStyle);
	static HRESULT AddStyle(CStyleSheetBase* pContainer, CComBSTR& rbstrStyles, BSTR bstrAddStyle);
	static HRESULT RemoveStyle(CStyleSheetBase* pContainer, CComBSTR& rbstrStyles, BSTR bstrRemoveStyle);
	static useOfMfc DetermineMfcStyle(CComBSTR& rbstrStyles);
	static HRESULT AddMfcStyle(CComBSTR& rbstrStyles, useOfMfc useMfc, CStyleSheetBase* pContainerClass);
	static HRESULT RemoveMfcStyle(CStyleSheetBase* pContainer, CComBSTR& rbstrStyles, useOfMfc useMfc);
	static int	   GetNextStyle(CStringW& strStyleList, int nStartStyle, int nMaxIdx, CComBSTR& bstrStyle);
	static HRESULT ChangeMfcStyle(CStyleSheetBase* pContainerClass, CComBSTR& rbstrStyles, useOfMfc useMfc);
	static HRESULT ChangeATLStyle(CStyleSheetBase* pContainerClass, CComBSTR& rbstrStyles, useOfATL useATL);
	static HRESULT ChangeATLCRTStyle(CStyleSheetBase* pContainerClass, CComBSTR& rbstrStyles, VARIANT_BOOL bNoCRT);
	static HRESULT ChangeCharSetStyle(CStyleSheetBase* pContainerClass, CComBSTR& rbstrStyles, charSet set);
	static toolSetType DetermineBaseType(ConfigurationTypes configType);

	virtual long GetOverrideID(long idProp);

protected:
	static int DoFindStyle(CStringW& rstrStyles, CStringW& rstrFindStyle, int nStart);

protected:
	void ResolveStyleSheetToPath(BSTR bstrStyle, CComBSTR& bstrStylePath);
#ifndef _SHIP
	void ResolveStyleSheetToPathBatch(BSTR bstrStyle, CComBSTR& bstrStylePath);
#endif	// ifndef _SHIP
	virtual void DoGetToolSetType(toolSetType& listStyle) { VSASSERT(FALSE, "DoGetToolSetType must be overridden!"); listStyle = toolSetUtility; }
	HRESULT DoSetUseOfMFC(useOfMfc useMfc, bool bDirtyProp = true);
	HRESULT DoSetUseOfATL(useOfATL useATL);
	HRESULT DoSetCharSet(charSet set);
	HRESULT DoSetATLCRTStyle(VARIANT_BOOL bNoCRT, bool bDirtyProp = true);
	HRESULT DoSetStyle(long idProp, BSTR bstrStyle, VARIANT_BOOL bValue);
	HRESULT DoGetStyleSheets(IDispatch** ppStyles);

//DATA
protected:
	CComBSTR					m_bstrName;	
	CComPtr<VCPlatform>			m_pPlatform;
	CVCPtrList					m_toolWrapperList;

	CComDynamicListTyped<IVCToolImpl> m_rgTools;		// collection of tools for this style sheet
	CComDynamicListTyped<IVCToolImpl> m_rgFileTools;		// collection of tools for files for this style sheet
	CComDynamicListTyped<VCStyleSheet> m_rgStyleSheets;	// collection of style sheets on this style sheet; temporary use only
};

/////////////////////////////////////////////////////////////////////////////
// CStyleSheet

class CStyleSheet : 
	public IDispatchImpl<VCStyleSheet, &__uuidof(VCStyleSheet), &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public IVCStyleSheetImpl,
	public CComObjectRoot,
#ifdef AARDVARK
	public IVCExternalCookie,
#endif	// AARDVARK
	public CStyleSheetBase
{
public:
	CStyleSheet();
	~CStyleSheet() {}

	static HRESULT CreateInstance(VCStyleSheet **ppStyleSheet);
	HRESULT Initialize();

BEGIN_COM_MAP(CStyleSheet)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(VCStyleSheet)
	COM_INTERFACE_ENTRY(IVCStyleSheetImpl)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
#ifdef AARDVARK
	COM_INTERFACE_ENTRY(IVCExternalCookie)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
#endif	// AARDVARK
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CStyleSheet) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

DECLARE_NO_REGISTRY()

// VCStyleSheet
public:
	STDMETHOD(get_Name)(BSTR *pVal);
	STDMETHOD(put_Name)(BSTR newVal);
	STDMETHOD(get_Platform)(IDispatch** ppPlatform);
	STDMETHOD(get_PlatformName)(BSTR* pbstrPlatformName);
	STDMETHOD(put_PlatformName)(BSTR bstrPlatformName);
	STDMETHOD(MatchName)(BSTR bstrNameToMatch, VARIANT_BOOL bFullOnly, VARIANT_BOOL *pbIsMatch);
	STDMETHOD(get_IsDirty)(VARIANT_BOOL *pVal);
	STDMETHOD(get_StyleSheetName)(BSTR *pVal);
	STDMETHOD(put_StyleSheetName)(BSTR newVal);
	STDMETHOD(get_StyleSheetFile)(BSTR *pVal);
	STDMETHOD(put_StyleSheetFile)(BSTR newVal);
	STDMETHOD(get_Tools)(IDispatch * *pVal);
	STDMETHOD(get_FileTools)(IVCCollection * *pVal);
	STDMETHOD(get_ToolSet)(toolSetType* pToolSet);
	STDMETHOD(put_ToolSet)(toolSetType ToolSet);
	STDMETHOD(get_OutputDirectory)(BSTR *pVal);
	STDMETHOD(put_OutputDirectory)(BSTR newVal);
	STDMETHOD(get_IntermediateDirectory)(BSTR *pVal);
	STDMETHOD(put_IntermediateDirectory)(BSTR newVal);
	STDMETHOD(get_DeleteExtensionsOnClean)(BSTR* pbstrExt);	// wildcard extension list to delete on clean/rebuild from within the intermediate directory
	STDMETHOD(put_DeleteExtensionsOnClean)(BSTR bstrExt);
	STDMETHOD(get_ConfigurationType)(ConfigurationTypes* pconfigType);		// Configuration type: application, DLL, etc.
	STDMETHOD(put_ConfigurationType)(ConfigurationTypes configType);
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
	STDMETHOD(get_WholeProgramOptimization)(VARIANT_BOOL* pbOptimized);	// perform whole program optimization on build; more or less requires optimization turned on to be effective
	STDMETHOD(put_WholeProgramOptimization)(VARIANT_BOOL bOptimize);
	STDMETHOD(get_StyleSheets)(IDispatch** styles) { return CStyleSheetBase::DoGetStyleSheets(styles); }
	STDMETHOD(get_VCProjectEngine)(IDispatch** projEngine)	{ return DoGetVCProjectEngine(projEngine); }

// IVCStyleSheetImpl
public:
	STDMETHOD(put_IsDirty)(VARIANT_BOOL boolDirty);
	STDMETHOD(Close)();
	STDMETHOD(SaveObject)(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent);
	STDMETHOD(Clone)(IVCPropertyContainer *pSource);
	STDMETHOD(AddDependent)(IVCPropertyContainer* pPropContainer, BOOL bInformAboutAllProperties);
	STDMETHOD(RemoveDependent)(IVCPropertyContainer* pPropContainer, BOOL bInformAboutAllProperties);
	STDMETHOD(InformAboutProperties)(IVCPropertyContainer* pPropContainer, BOOL bAllProperties);
	STDMETHOD(get_LoadCount)(long* pnLoadCount);	// how many projects/style sheets have this style sheet loaded; used by project engine only
	STDMETHOD(put_LoadCount)(long nLoadCount);
	STDMETHOD(CreatePageObject)(IUnknown **, CLSID*, IVCPropertyContainer *pSource, IVCSettingsPage * );
	STDMETHOD(AddTool)(LPCOLESTR szToolName, IDispatch** ppTool);	// adds a new tool to the style sheet

#ifdef AARDVARK
//	IVCExternalCookie
public:
	STDMETHOD(get_ExternalCookie)(void** ppVal) { *ppVal = m_pExternalCookie; return S_OK; }
	STDMETHOD(put_ExternalCookie)(void* pVal) { m_pExternalCookie = pVal; return S_OK; }

protected:
	void* m_pExternalCookie;

// ISpecifyPropertyPages
public:
    STDMETHOD(GetPages)( /* [out] */ CAUUID *pPages );
#endif	// AARDVARK

// default IVCPropertyContainer methods are all in a base class
// default ISpecifyPropertyPages method is in a base class
     
// VCStyleSheet helpers
public:
	static HRESULT SGetToolSet(toolSetType* pToolSet);
protected:
	HRESULT DoSetToolSet(toolSetType toolSet, BOOL bForceUpdate = FALSE);

// IVCStyleSheetImpl helpers
public:
	virtual HRESULT SetDirty(VARIANT_BOOL bDirty) { return put_IsDirty(VARIANT_TRUE); }

// IVCPropertyContainer helpers
protected:
	virtual HRESULT DoSetIntProperty(long id, BOOL bCheckSpecialProps, long nValue, long nOverrideID = -1);

// LOCAL METHODS
public:

protected:
	virtual void DoGetToolSetType(toolSetType& listStyle);

//DATA
protected:
	CComDynamicListTyped<IVCPropertyContainer> m_rgDependents;		// collection of dependents for this style sheet
	VARIANT_BOOL m_bDirty;
	CComBSTR m_bstrFileName;
	CComBSTR m_bstrFullDir;
	long m_nLoadCount;
};

#ifdef AARDVARK
class ATL_NO_VTABLE CGeneralStyleSheetSettingsPage :
	public IDispatchImpl<IGeneralStyleSheetSettingsPage, &IID_IGeneralStyleSheetSettingsPage, &LIBID_StyleDemoLibrary, 3,0, CVsTypeInfoHolder>,
	public CPageObjectImpl<CGeneralStyleSheetSettingsPage,VCSTYLESHEET_MIN_DISPID, VCCONFIGURATION_MAX_DISPID>,
	public CComObjectRoot
{
public:
BEGIN_COM_MAP(CGeneralStyleSheetSettingsPage)
 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IGeneralStyleSheetSettingsPage)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
END_COM_MAP()

	// IDispatch override
	STDMETHOD(Invoke)( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pexcepInfo, UINT *puArgErr )
	{
		IDispatchImpl<IGeneralStyleSheetSettingsPage, &IID_IGeneralStyleSheetSettingsPage, &LIBID_StyleDemoLibrary, 3,0, CVsTypeInfoHolder>::Invoke( dispid, riid, lcid, wFlags, pdispParams, pvarResult, pexcepInfo, puArgErr );
		return S_OK;
	}


// IGeneralStyleSheetSettingsPage
public:
	STDMETHOD(get_ToolSet)(toolSetType* ToolSetList);	// what type of tools do we use?
	STDMETHOD(put_ToolSet)(toolSetType ToolSetList);

	// yes, the use of VCCFGID_ IDs is intentional here...  these *are* things that the configuration can use to default in values for
	STDMETHOD(get_AppliedStyleSheets)(BSTR* StyleNames);	// style sheet names applied to this configuration; purposely *NOT* in UI, but still browsable
	STDMETHOD(put_AppliedStyleSheets)(BSTR StyleNames);
	STDMETHOD(get_BuildBrowserInformation)(enumBOOL* Bsc);	// build .bsc file
	STDMETHOD(put_BuildBrowserInformation)(enumBOOL Bsc);
	STDMETHOD(get_UseOfMFC)(useOfMfc* useMfc);	// how are we using MFC?
	STDMETHOD(put_UseOfMFC)(useOfMfc useMfc);
	STDMETHOD(get_UseOfATL)(useOfATL* useATL);	// how are we using ATL?
	STDMETHOD(put_UseOfATL)(useOfATL useATL);
	STDMETHOD(get_ATLMinimizesCRunTimeLibraryUsage)(enumBOOL* UseCRT);	// does the user want to link with the static or dynamic CRT?
	STDMETHOD(put_ATLMinimizesCRunTimeLibraryUsage)(enumBOOL UseCRT);
	STDMETHOD(get_CharacterSet)(charSet* optSetting);	// character set to use: Unicode, MBCS, default
	STDMETHOD(put_CharacterSet)(charSet optSetting);
	STDMETHOD(get_ManagedExtensions)(enumBOOL* Managed);	// is this a managed C++ project?
	STDMETHOD(put_ManagedExtensions)(enumBOOL Managed);
	STDMETHOD(get_DeleteExtensionsOnClean)(BSTR* ExtList);	// wildcard extension list to delete on clean/rebuild from within the intermediate directory
	STDMETHOD(put_DeleteExtensionsOnClean)(BSTR ExtList);
	STDMETHOD(get_WholeProgramOptimization)(enumBOOL* Optimize);	// perform whole program optimization on build; more or less requires optimization turned on to be effective
	STDMETHOD(put_WholeProgramOptimization)(enumBOOL Optimize);
};

class CGeneralStyleSheetSettings :
	public CSettingsPageBase<CGeneralStyleSheetSettings, &CLSID_GeneralStyleSheetSettings, IDS_GeneralConfigSettings, 0>
{
public:
	CGeneralStyleSheetSettings() {}
// IVSPropertyPage
    STDMETHOD(get_CategoryTitle)( UINT iLevel, BSTR *pbstrCategory )
	{ return S_FALSE; }

protected:
	virtual void DoCreatePageObject(UINT i, IDispatch* pDisp, IVCPropertyContainer* pLiveContainer, IVCSettingsPage* pPage)
	{
		CPageObjectImpl<CGeneralStyleSheetSettingsPage,VCSTYLESHEET_MIN_DISPID, VCCONFIGURATION_MAX_DISPID>::CreateInstance(&m_ppUnkArray[i], pLiveContainer, pPage);
	}
};
#endif	// AARDVARK
