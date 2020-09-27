// NMakeTool.h: Definition of the CVCNMakeTool class
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "vctool.h"
#include "settingspage.h"

// constants
const wchar_t* const wszNMakeDefaultExtensions = L"";	// remember to update vcpb.rgs if you change this...

// classes in this header
class CVCNMakeTool;

/////////////////////////////////////////////////////////////////////////////
// CVCNMakeTool

class ATL_NO_VTABLE CVCNMakeTool : 
	public IDispatchImpl<VCNMakeTool, &IID_VCNMakeTool, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public CVCToolImpl,
	public CComObjectRoot
{
public:
	CVCNMakeTool() 
	{
		m_nLowKey = VCNMAKETOOL_MIN_DISPID;
		m_nHighKey = VCNMAKETOOL_MAX_DISPID;
		m_nDirtyKey = VCNMAKEID_CmdLineOptionsDirty;
		m_nDirtyOutputsKey = VCNMAKEID_OutputsDirty;
	}
	static HRESULT CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool);

BEGIN_COM_MAP(CVCNMakeTool)
 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCToolImpl)
	COM_INTERFACE_ENTRY(VCNMakeTool)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(CVCNMakeTool) 

// VCNMakeTool
public:
	// General
	STDMETHOD(get_ToolName)(BSTR* pbstrToolName);	// friendly name of tool, e.g., "C/C++ Compiler Tool"
	STDMETHOD(get_BuildCommandLine)(BSTR *pbstrCommandLine);
	STDMETHOD(put_BuildCommandLine)(BSTR bstrCommandLine);
	STDMETHOD(get_ReBuildCommandLine)(BSTR *pbstrCommandLine);
	STDMETHOD(put_ReBuildCommandLine)(BSTR bstrCommandLine);
	STDMETHOD(get_CleanCommandLine)(BSTR *pbstrCommandLine);
	STDMETHOD(put_CleanCommandLine)(BSTR bstrCommandLine);
	STDMETHOD(get_Output)(BSTR *pbstrOutput);
	STDMETHOD(put_Output)(BSTR bstrOutput);
	
	// Automation properties
	STDMETHOD(get_ToolPath)(BSTR *pbstrToolPath);
	STDMETHOD(get_VCProjectEngine)(IDispatch** projEngine)	{ return DoGetVCProjectEngine(projEngine); }

// IVCToolImpl
public:
	STDMETHOD(get_DefaultExtensions)(BSTR* pVal);
	STDMETHOD(put_DefaultExtensions)(BSTR newVal);
	STDMETHOD(GetAdditionalOptionsInternal)(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, BSTR* pbstrOptions);
	STDMETHOD(GetBuildDescription)(IVCBuildAction* pAction, BSTR* pbstrBuildDescription);
	STDMETHOD(get_ToolPathInternal)(BSTR* pbstrToolPath);
	STDMETHOD(get_Bucket)(long *pVal);
	STDMETHOD(GetCommandLineEx)(IVCBuildAction*,IVCBuildableItem* pBuildableItem, IVCBuildEngine* pBuildEngine, 
		IVCBuildErrorContext* pEC, BSTR* bstrCmd);
	STDMETHOD(GetCleanCommandLineEx)(IVCBuildAction*,IVCBuildableItem* pBuildableItem, IVCBuildEngine* pBuildEngine, 
		IVCBuildErrorContext* pEC, BSTR* bstrCmd);
	STDMETHOD(GetRebuildCommandLineEx)(IVCBuildAction*,IVCBuildableItem* pBuildableItem, IVCBuildEngine* pBuildEngine, 
		IVCBuildErrorContext* pEC, BSTR* bstrCmd);
	STDMETHOD(IsTargetTool)(IVCBuildAction* pAction, VARIANT_BOOL* pbTargetTool);
	STDMETHOD(GenerateOutput)(long type, IVCBuildActionList* plstActions, IVCBuildErrorContext* pEC);
	STDMETHOD(CreateToolObject)(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject)
		{ return CreateInstance(pPropContainer, ppToolObject); }
	STDMETHOD(get_ToolShortName)(BSTR* pbstrToolName);
	STDMETHOD(get_ToolNameInternal)(BSTR* pbstrToolName) { return get_ToolName(pbstrToolName); }
	STDMETHOD(MatchName)(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches);
	STDMETHOD(get_IsComspecTool)(VARIANT_BOOL* pbIsComspecTool);
	STDMETHOD(SaveObject)(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent);
	STDMETHOD(get_ToolDisplayIndex)(long* pIndex);
	STDMETHOD(AffectsOutput)(long nPropID, VARIANT_BOOL* pbAffectsOutput);
	STDMETHOD(HasPrimaryOutputFromTool)(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, VARIANT_BOOL* pbHasPrimaryOutput);	// returns TRUE only if tool generates a primary output (ex: linker)
	STDMETHOD(GetPrimaryOutputFromTool)(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, BSTR* pbstrPath);	// return a value only if tool generates a primary output (ex: linker)
	STDMETHOD(GetPrimaryOutputIDFromTool)(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, long* pnOutputID);	// return a value only if tool generates a primary output (ex: linker)
	STDMETHOD(CreatePageObject)(IUnknown **, CLSID*, IVCPropertyContainer *, IVCSettingsPage * );

	virtual long GetPageCount()	{ return 1; }
	virtual GUID* GetPageIDs();

protected:
	void AddSaveLine(IStream *xml, IVCPropertyContainer* pPropContainer, LPOLESTR bszPropertyName, long nPropertyID);
	virtual LPCOLESTR GetToolFriendlyName();
	void InitToolName();
	virtual long GetToolID() { return IDS_NMake; }

protected:
	static GUID s_pPages[1];
	static BOOL s_bPagesInit;
 	static CComBSTR s_bstrDescription;
 	static CComBSTR s_bstrToolName;
	static CComBSTR s_bstrExtensions;
};

class ATL_NO_VTABLE CVCNMakePage :
	public IDispatchImpl<IVCNMakePage, &IID_IVCNMakePage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public CPageObjectImpl<CVCNMakePage,VCNMAKETOOL_MIN_DISPID,VCNMAKETOOL_MAX_DISPID>,
	public CComObjectRoot
{
public:

BEGIN_COM_MAP(CVCNMakePage)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCNMakePage)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
	COM_INTERFACE_ENTRY(IProvidePropertyBuilder)
END_COM_MAP()

	// IDispatch override
	STDMETHOD(Invoke)( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pexcepInfo, UINT *puArgErr )
	{
		IDispatchImpl<IVCNMakePage, &IID_IVCNMakePage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>::Invoke( dispid, riid, lcid, wFlags, pdispParams, pvarResult, pexcepInfo, puArgErr );
		return S_OK;
	}

	STDMETHOD(HasDefaultValue)( DISPID dispid, BOOL *pfDefault)
	{
		CHECK_POINTER_NULL(pfDefault);
		*pfDefault = TRUE;
		return S_OK;
	}

// IVCNMakePage
public:
	STDMETHOD(get_BuildCommandLine)(BSTR *pVal);
	STDMETHOD(put_BuildCommandLine)(BSTR newVal);
	STDMETHOD(get_ReBuildCommandLine)(BSTR *pVal);
	STDMETHOD(put_ReBuildCommandLine)(BSTR newVal);
	STDMETHOD(get_CleanCommandLine)(BSTR *pVal);
	STDMETHOD(put_CleanCommandLine)(BSTR newVal);
	STDMETHOD(get_Output)(BSTR *pVal);
	STDMETHOD(put_Output)(BSTR newVal);

protected:
	virtual BOOL UseCommandsDialog(long id) 
		{ return (id == VCNMAKEID_BuildCommandLine || id == VCNMAKEID_ReBuildCommandLine || id == VCNMAKEID_CleanCommandLine ); }
};

