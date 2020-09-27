// SdlTool.h: Definition of the CVCWebServiceProxyGeneratorTool class
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "vctool.h"
#include "settingspage.h"

// classes in this header
class CVCWebServiceProxyGeneratorTool;
class CWebToolOptionHandler;
class CVCWebServiceGeneralPage;

class CWebToolOptionHandler : public COptionHandlerBase
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
};

/////////////////////////////////////////////////////////////////////////////
// CVCWebServiceProxyGeneratorTool

class ATL_NO_VTABLE CVCWebServiceProxyGeneratorTool : 
	public IDispatchImpl<VCWebServiceProxyGeneratorTool, &IID_VCWebServiceProxyGeneratorTool, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public CComObjectRoot,
	public CVCToolImpl
{
public:
	CVCWebServiceProxyGeneratorTool() 
	{
		m_nLowKey = VCWEBTOOL_MIN_DISPID;
		m_nHighKey = VCWEBTOOL_MAX_DISPID;
		m_nDirtyKey = VCWEBID_CmdLineOptionsDirty;
		m_nDirtyOutputsKey = VCWEBID_OutputsDirty;
	}
	static HRESULT CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool);

BEGIN_COM_MAP(CVCWebServiceProxyGeneratorTool)
	COM_INTERFACE_ENTRY(IVCToolImpl)
 	COM_INTERFACE_ENTRY(VCWebServiceProxyGeneratorTool)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(CVCWebServiceProxyGeneratorTool) 

// VCWebServiceProxyGeneratorTool
public:
	// general
	STDMETHOD(get_Output)(BSTR *pbstrOutput);
	STDMETHOD(put_Output)(BSTR bstrOutput);
	STDMETHOD(get_SuppressStartupBanner)(VARIANT_BOOL* suppress);		// /nologo
	STDMETHOD(put_SuppressStartupBanner)(VARIANT_BOOL suppress);
	STDMETHOD(get_GeneratedProxyLanguage)(genProxyLanguage* language);	// generated proxy language: native or managed C++
	STDMETHOD(put_GeneratedProxyLanguage)(genProxyLanguage language);
	STDMETHOD(get_AdditionalOptions)(BSTR* options);	// any additional options
	STDMETHOD(put_AdditionalOptions)(BSTR options);
	STDMETHOD(get_URL)(BSTR* url);
	STDMETHOD(put_URL)(BSTR url);

 	STDMETHOD(get_ToolName)(BSTR* pbstrToolName);	// friendly name of tool, e.g., "C/C++ Compiler Tool"
	STDMETHOD(get_VCProjectEngine)(IDispatch** projEngine)	{ return DoGetVCProjectEngine(projEngine); }

	// Automation properties
	STDMETHOD(get_ToolPath)(BSTR *pVal);
	STDMETHOD(get_PropertyOption)(BSTR bstrProp, long dispidProp, BSTR *pVal);

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
	STDMETHOD(get_ToolShortName)(BSTR* pbstrToolName);
	STDMETHOD(get_ToolNameInternal)(BSTR* pbstrToolName) { return get_ToolName(pbstrToolName); }
	STDMETHOD(MatchName)(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches);
	STDMETHOD(CanScanForDependencies)(VARIANT_BOOL* pbIsScannable);
	STDMETHOD(GetCommandLineEx)(IVCBuildAction*, IVCBuildableItem* pBuildableItem, IVCBuildEngine* pBuildEngine, 
		IVCBuildErrorContext* pEC, BSTR* bstrCmd);
	STDMETHOD(CreatePageObject)(IUnknown **, CLSID*, IVCPropertyContainer *, IVCSettingsPage * );
	STDMETHOD(HasPrimaryOutputFromTool)(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, VARIANT_BOOL* pbHasPrimaryOutput);	// returns TRUE only if tool generates a primary output (ex: linker)
	STDMETHOD(SetPrimaryOutputForTool)(IUnknown* pItem, BSTR bstrFile);		// sets the primary output for a tool
	STDMETHOD(GetPrimaryOutputIDFromTool)(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, long* pnOutputID);	// return a value only if tool generates a primary output (ex: linker)
	STDMETHOD(GetDefaultOutputFileFormat)(IUnknown* pItem, BSTR* pbstrOutMacro);	// macro to use for generating default output file if the original default isn't unique

	STDMETHOD(get_ToolDisplayIndex)(long* pIndex)
	{
		CHECK_POINTER_NULL(pIndex);
		*pIndex = TOOL_DISPLAY_INDEX_WEB;
		return S_OK;
	}

	virtual long GetPageCount()	{ return 2; }
	virtual GUID* GetPageIDs();

// helpers
public:
	virtual COptionHandlerBase* GetOptionHandler() { return &s_optHandler; }
	virtual HRESULT GetPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, CPathW& rPath);	// return a value only if tool generates a primary output (ex: linker)
	virtual LPCOLESTR GetToolFriendlyName();
	void InitToolName();
	virtual long GetToolID() { return IDS_WEBTOOL; }
	virtual void SaveSpecialProps(IStream *xml, IVCPropertyContainer *pPropCnt);

protected:
	HRESULT DoGetToolPath(BSTR* pbstrToolPath, IVCPropertyContainer* pPropContainer);

public:
	static CWebToolOptionHandler s_optHandler;

protected:
	static GUID s_pPages[2];
	static BOOL s_bPagesInit;
	static CComBSTR s_bstrBuildDescription;
	static CComBSTR s_bstrToolName;
	static CComBSTR s_bstrExtensions;
};

class ATL_NO_VTABLE CVCWebServiceGeneralPage :
	public IDispatchImpl<IVCWebServiceGeneralPage, &IID_IVCWebServiceGeneralPage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public CPageObjectImpl<CVCWebServiceGeneralPage,VCWEBTOOL_MIN_DISPID,VCWEBTOOL_MAX_DISPID>,
	public CComObjectRoot
{
public:

BEGIN_COM_MAP(CVCWebServiceGeneralPage)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
	COM_INTERFACE_ENTRY(IVCWebServiceGeneralPage)
END_COM_MAP()

	// IDispatch override
	STDMETHOD(Invoke)( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pexcepInfo, UINT *puArgErr )
	{
		IDispatchImpl<IVCWebServiceGeneralPage, &IID_IVCWebServiceGeneralPage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>::Invoke( dispid, riid, lcid, wFlags, pdispParams, pvarResult, pexcepInfo, puArgErr );
		return S_OK;
	}


// IVCWebServiceGeneralPage
public:
	// general 
	STDMETHOD(get_Output)(BSTR *pbstrOutput);
	STDMETHOD(put_Output)(BSTR bstrOutput);
	STDMETHOD(get_SuppressStartupBanner)(enumSuppressStartupBannerBOOL* suppress);		// /nologo
	STDMETHOD(put_SuppressStartupBanner)(enumSuppressStartupBannerBOOL suppress);
	STDMETHOD(get_GeneratedProxyLanguage)(genProxyLanguage* language);	// generated proxy language: native or managed C++
	STDMETHOD(put_GeneratedProxyLanguage)(genProxyLanguage language);
	void GetBaseDefault(long id, CComVariant& varValue);
};


#define WebProxyMax 1