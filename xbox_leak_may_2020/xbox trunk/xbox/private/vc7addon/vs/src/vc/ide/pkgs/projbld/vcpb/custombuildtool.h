// CustomBuildTool.h: Definition of the CVCCustomBuildTool class
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "vctool.h"
#include "settingspage.h"
#include "vshelp.h"

// classes in this header
class CVCCustomBuildTool;

/////////////////////////////////////////////////////////////////////////////
// CVCCustomBuildTool

class ATL_NO_VTABLE CVCCustomBuildTool : 
	public IDispatchImpl<VCCustomBuildTool, &IID_VCCustomBuildTool, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public CVCToolImpl,
	public CComObjectRoot
{
public:
	CVCCustomBuildTool() {}
	static HRESULT CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool);

BEGIN_COM_MAP(CVCCustomBuildTool)
 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCToolImpl)
	COM_INTERFACE_ENTRY(VCCustomBuildTool)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(CVCCustomBuildTool) 

protected:
	virtual void Initialize(IVCPropertyContainer* pPropContainer);

// VCCustomBuildTool
public:
	// General
	STDMETHOD(get_ToolName)(BSTR* pbstrToolName);	// friendly name of tool, e.g., "C/C++ Compiler Tool"
	STDMETHOD(get_CommandLine)(BSTR *pbstrCommandLine);
	STDMETHOD(put_CommandLine)(BSTR bstrCommandLine);
	STDMETHOD(get_Description)(BSTR *pbstrDescription);
	STDMETHOD(put_Description)(BSTR bstrDescription);
	STDMETHOD(get_Outputs)(BSTR *pbstrOutputs);
	STDMETHOD(put_Outputs)(BSTR bstrOutputs);
	STDMETHOD(get_AdditionalDependencies)(BSTR *pbstrAdditionalDependencies);
	STDMETHOD(put_AdditionalDependencies)(BSTR bstrAdditionalDependencies);
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
	STDMETHOD(IsTargetTool)(IVCBuildAction* pAction, VARIANT_BOOL* pbTargetTool);
	STDMETHOD(GenerateOutput)(long type, IVCBuildActionList* plstActions, IVCBuildErrorContext* pEC);
	STDMETHOD(CreateToolObject)(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject);
	STDMETHOD(get_ToolShortName)(BSTR* pbstrToolName);
	STDMETHOD(get_ToolNameInternal)(BSTR* pbstrToolName) { return get_ToolName(pbstrToolName); }
	STDMETHOD(MatchName)(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches);
	STDMETHOD(get_IsComspecTool)(VARIANT_BOOL* pbIsComspecTool);
	STDMETHOD(SaveObject)(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent);
	STDMETHOD(get_ToolDisplayIndex)(long* pIndex);
	STDMETHOD(AffectsOutput)(long nPropID, VARIANT_BOOL* pbAffectsOutput);
	STDMETHOD(CreatePageObject)(IUnknown **, CLSID*, IVCPropertyContainer *, IVCSettingsPage * );
	STDMETHOD(GetDeployableOutputsCount)(IVCPropertyContainer* pPropContainer, long* pnOutputs);	// number of deployable outputs
	STDMETHOD(GetDeployableOutputs)(IVCPropertyContainer* pPropContainer, IVCBuildStringCollection** ppDeployableOutputs);	// the deployable outputs

	virtual long GetPageCount()	{ return 1; }
	virtual GUID* GetPageIDs();

protected:
	int m_nOffset;	// what's the offset from the proj cfg level IDs?
	static GUID s_pPages[1];
	static BOOL s_bPagesInit;
	static CComBSTR s_bstrDescription;
	static CComBSTR s_bstrToolName;
	static CComBSTR s_bstrExtensions;

protected:
	void AddSaveLine(IStream *xml, IVCPropertyContainer* pPropContainer, LPOLESTR bszPropertyName, long nPropertyID );
	virtual LPCOLESTR GetToolFriendlyName();
	void InitToolName();
	virtual long GetToolID() { return IDS_CustomBuild; }
	virtual HRESULT GetCommandLineOptions(IUnknown* pItem, IVCBuildAction* pAction, VARIANT_BOOL bIncludeAdditional, 
		VARIANT_BOOL bForDisplay, commandLineOptionStyle fStyle, CStringW& rstrCmdLine);
	BOOL GetProcessedOutputString(IVCPropertyContainer* pPropContainer, CStringW& strOutputs);
	void AddCustomBuildError(IVCBuildErrorContext* pEC, long idErrFile, BSTR bstrErrNumFile, long idErrProj, BSTR bstrErrNumProj,
		const wchar_t* szWithMacros, const wchar_t* szNoMacros, IVCPropertyContainer* pPropContainer);

public:
	static HRESULT SGetDescription( BSTR * pVal );
};


class ATL_NO_VTABLE CVCCustomBuildPage :
	public IDispatchImpl<IVCCustomBuildPage, &IID_IVCCustomBuildPage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public CPageObjectImpl<CVCCustomBuildPage,VCCUSTOMBUILDTOOL_MIN_DISPID,VCCUSTOMBUILDTOOL_MAX_DISPID>,
	public CComObjectRoot
{
public:

BEGIN_COM_MAP(CVCCustomBuildPage)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCCustomBuildPage)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
	COM_INTERFACE_ENTRY(IProvidePropertyBuilder)
	COM_INTERFACE_ENTRY(IVCPropertyPageObject)
END_COM_MAP()

// IVCCustomBuildPage
public:
	STDMETHOD(get_CommandLine)(BSTR *pVal);
	STDMETHOD(put_CommandLine)(BSTR newVal);
	STDMETHOD(get_Description)(BSTR *pVal);
	STDMETHOD(put_Description)(BSTR newVal);
	STDMETHOD(get_Outputs)(BSTR *pVal);
	STDMETHOD(put_Outputs)(BSTR newVal);
	STDMETHOD(get_AdditionalDependencies)(BSTR *pVal);
	STDMETHOD(put_AdditionalDependencies)(BSTR newVal);

// helper functions
public:
	virtual void ResetParentIfNeeded();
	virtual void GetBaseDefault(long id, CComVariant& varValue);
	virtual BOOL UseCommandsDialog(long id) { return (id == VCCUSTID_CommandLine || id == (VCCUSTID_CommandLine + m_nOffset)); }
	virtual BOOL SupportsMultiLine(long id) { return (id == VCCUSTID_Outputs || id == (VCCUSTID_Outputs + m_nOffset)); }
	virtual long KludgeLocID(long id);

protected:
	int m_nOffset;	// what's the offset from the proj cfg level IDs?
};
