// XboxDeployTool.h: Definition of the CXboxDeploymentTool class
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "vctool.h"
#include "settingspage.h"

// classes in this header
class CXboxDeploymentTool;
class CXboxDeployToolOptionHandler;
class CXboxDeploymentGeneralPage;

class CXboxDeployToolOptionHandler : public COptionHandlerBase
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
};

/////////////////////////////////////////////////////////////////////////////
// CXboxDeploymentTool

class ATL_NO_VTABLE CXboxDeploymentTool : 
	public IDispatchImpl<XboxDeploymentTool, &IID_XboxDeploymentTool, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public IVCBuildEventToolInternal,
	public CComObjectRoot,
	public CVCToolImpl
{
public:
	CXboxDeploymentTool() 
	{
		m_nLowKey = XBOXVCDPLYTOOL_MIN_DISPID;
		m_nHighKey = XBOXVCDPLYTOOL_MAX_DISPID;
		m_nDirtyKey = XBOXDPLYID_CmdLineOptionsDirty;
	}
	static HRESULT CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool);

BEGIN_COM_MAP(CXboxDeploymentTool)
	COM_INTERFACE_ENTRY(IVCToolImpl)
 	COM_INTERFACE_ENTRY(XboxDeploymentTool)
	COM_INTERFACE_ENTRY(IVCBuildEventToolInternal)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(CXboxDeploymentTool) 

// XboxDeploymentTool
public:
	// general
	STDMETHOD(get_ExcludedFromBuild)(VARIANT_BOOL* disableDeploy);	// exclude from build? default: yes
	STDMETHOD(put_ExcludedFromBuild)(VARIANT_BOOL disableDeploy);
	STDMETHOD(get_RemotePath)(BSTR* dir);	// remote path to deploy to
	STDMETHOD(put_RemotePath)(BSTR dir);
	STDMETHOD(get_AdditionalFiles)(BSTR* files);	// additional files to deploy
	STDMETHOD(put_AdditionalFiles)(BSTR files);

 	STDMETHOD(get_ToolName)(BSTR* pbstrToolName);	// friendly name of tool, e.g., "C/C++ Compiler Tool"
	STDMETHOD(get_VCProjectEngine)(IDispatch** projEngine)	{ return DoGetVCProjectEngine(projEngine); }

	// Automation properties
	STDMETHOD(get_ToolPath)(BSTR *pVal);
	STDMETHOD(get_PropertyOption)(BSTR bstrProp, long dispidProp, BSTR *pVal);

// IVCBuildEventToolInternal
public:
	STDMETHOD(get_CommandLineInternal)(BSTR *pVal);
	STDMETHOD(put_CommandLineInternal)(BSTR newVal);
	STDMETHOD(get_DescriptionInternal)(BSTR *pVal);
	STDMETHOD(put_DescriptionInternal)(BSTR newVal);
	STDMETHOD(get_ExcludedFromBuildInternal)(VARIANT_BOOL* pbExcludedFromBuild);
	STDMETHOD(put_ExcludedFromBuildInternal)(VARIANT_BOOL bExcludedFromBuild);

// IVCToolImpl
public:
	STDMETHOD(get_DefaultExtensions)(BSTR* pVal);
	STDMETHOD(put_DefaultExtensions)(BSTR newVal);
	STDMETHOD(GetAdditionalOptionsInternal)(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, BSTR* pbstrAdditionalOptions);
	STDMETHOD(GetBuildDescription)(IVCBuildAction* pAction, BSTR* pbstrBuildDescription);
	STDMETHOD(get_ToolPathInternal)(BSTR* pbstrToolPath);
	STDMETHOD(get_Bucket)(long *pVal);
	STDMETHOD(CreateToolObject)(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject);
	STDMETHOD(GetAdditionalIncludeDirectoriesInternal)(IVCPropertyContainer* pPropContainer, BSTR* pbstrIncDirs);
	STDMETHOD(get_ToolShortName)(BSTR* pbstrToolName);
	STDMETHOD(get_ToolNameInternal)(BSTR* pbstrToolName) { return get_ToolName(pbstrToolName); }
	STDMETHOD(MatchName)(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches);
	STDMETHOD(CreatePageObject)(IUnknown **, CLSID*, IVCPropertyContainer *, IVCSettingsPage * );
	STDMETHOD(IsTargetTool)(IVCBuildAction* pAction, VARIANT_BOOL* pbTargetTool); // tool always operates on target, not on file
	STDMETHOD(get_IsFileTool)(VARIANT_BOOL* pbIsFileTool);
	STDMETHOD(GetCommandLineEx)(IVCBuildAction* pAction, IVCBuildableItem* pBuildableItem, IVCBuildEngine* pBuildEngine, 
		IVCBuildErrorContext* pEC, BSTR *pVal);

	STDMETHOD(get_ToolDisplayIndex)(long* pIndex)
	{
		CHECK_POINTER_NULL(pIndex);
		*pIndex = TOOL_DISPLAY_INDEX_XBOXDEPLOY;
		return S_OK;
	}

	virtual long GetPageCount()	{ return 1; }
	virtual GUID* GetPageIDs();

// helpers
public:
	virtual COptionHandlerBase* GetOptionHandler() { return &s_optHandler; }
	virtual LPCOLESTR GetToolFriendlyName();
	void InitToolName();
	virtual long GetToolID() { return IDS_DEPLOYTOOL; }

protected:
	HRESULT DoGetToolPath(BSTR* pbstrToolPath, IVCPropertyContainer* pPropContainer);

public:
	static CXboxDeployToolOptionHandler s_optHandler;

protected:
	static GUID s_pPages[1];
	static BOOL s_bPagesInit;
	static CComBSTR s_bstrBuildDescription;
	static CComBSTR s_bstrToolName;
	static CComBSTR s_bstrExtensions;
};

class ATL_NO_VTABLE CXboxDeploymentGeneralPage :
	public IDispatchImpl<IXboxDeploymentGeneralPage, &IID_IXboxDeploymentGeneralPage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public ICategorizeProperties,
	public CPageObjectImpl<CXboxDeploymentGeneralPage,XBOXVCDPLYTOOL_MIN_DISPID,XBOXVCDPLYTOOL_MAX_DISPID>,
	public CComObjectRoot
{
public:

BEGIN_COM_MAP(CXboxDeploymentGeneralPage)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
	COM_INTERFACE_ENTRY(IXboxDeploymentGeneralPage)
	COM_INTERFACE_ENTRY(IProvidePropertyBuilder)
	COM_INTERFACE_ENTRY_IID(IID_ICategorizeProperties, ICategorizeProperties)
END_COM_MAP()

	// IDispatch override
	STDMETHOD(Invoke)( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pexcepInfo, UINT *puArgErr )
	{
		IDispatchImpl<IXboxDeploymentGeneralPage, &IID_IXboxDeploymentGeneralPage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>::Invoke( dispid, riid, lcid, wFlags, pdispParams, pvarResult, pexcepInfo, puArgErr );
		return S_OK;
	}


// ICategorizeProperties
public:
	STDMETHOD(MapPropertyToCategory)(DISPID dispid, PROPCAT* ppropcat);
	STDMETHOD(GetCategoryName)(PROPCAT propcat, LCID lcid, BSTR* pbstrName);

// IXboxDeploymentGeneralPage
public:
	// general 
	STDMETHOD(get_ExcludedFromBuild)(VARIANT_BOOL* disableDeploy);	// exclude from build? default: yes
	STDMETHOD(put_ExcludedFromBuild)(VARIANT_BOOL disableDeploy);
	STDMETHOD(get_RemotePath)(BSTR* dir);	// remote path to deploy to
	STDMETHOD(put_RemotePath)(BSTR dir);
	STDMETHOD(get_AdditionalFiles)(BSTR* files);	// additional files to deploy
	STDMETHOD(put_AdditionalFiles)(BSTR files);

// helpers
public:
	virtual BOOL UseMultiLineNoInheritDialog(long id) { return (id == XBOXDPLYID_AdditionalFiles); }
};

#define DeployProtectMax 2
