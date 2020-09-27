// VCBscMakeTool.h: Definition of the CVCBscMakeTool class
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "vctool.h"
#include "SettingsPage.h"

// classes in this header:
class CVCBscMakeTool;
class CBscMakeToolOptionHandler;

class CBscMakeToolOptionHandler : public COptionHandlerBase
{
public:
	DECLARE_OPTION_TABLE();

public:
	// default value handlers
	// string props
	virtual void GetDefaultValue( long id, BSTR *bstrVal, IVCPropertyContainer *pPropCnt = NULL );
	// integer props
	virtual void GetDefaultValue( long id, long *iVal, IVCPropertyContainer *pPropCnt = NULL );
	// boolean props
	virtual void GetDefaultValue( long id, VARIANT_BOOL *bVal, IVCPropertyContainer *pPropCnt = NULL );

protected:
	virtual BOOL SetEvenIfDefault(VARIANT *pVarDefault, long idOption);
};

/////////////////////////////////////////////////////////////////////////////
// CVCBscMakeTool

class ATL_NO_VTABLE CVCBscMakeTool : 
	public IDispatchImpl<VCBscMakeTool, &IID_VCBscMakeTool, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public CVCToolImpl,
	public CComObjectRoot
{
public:
	CVCBscMakeTool() 
	{
		m_nLowKey = VCBSCMAKETOOL_MIN_DISPID;
		m_nHighKey = VCBSCMAKETOOL_MAX_DISPID;
		m_nDirtyKey = VCBSCID_CmdLineOptionsDirty;
		m_nDirtyOutputsKey = VCBSCID_OutputsDirty;
	}
	static HRESULT CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool);

BEGIN_COM_MAP(CVCBscMakeTool)
 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCToolImpl)
	COM_INTERFACE_ENTRY(VCBscMakeTool)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(CVCBscMakeTool) 

protected:
	// static member data
	static GUID s_pPages[2];
	static BOOL s_bPagesInit;
	static CComBSTR s_bstrBuildDescription;
	static CComBSTR s_bstrExtensions;

// VCBscMakeTool
public:
	// general; @response deliberately left out
	STDMETHOD(get_ToolName)(BSTR* pbstrToolName);	// friendly name of tool, e.g., "BSCMake Tool"
	STDMETHOD(get_AdditionalOptions)(BSTR* pbstrAdditionalOptions);	// anything we don't know what to do with, given verbatim, in same order as entered, parent contributions first, goes after all other switches
	STDMETHOD(put_AdditionalOptions)(BSTR bstrAdditionalOptions);
	STDMETHOD(get_SuppressStartupBanner)(VARIANT_BOOL* pbNoLogo);	// (/nologo) enable suppression of copyright message
	STDMETHOD(put_SuppressStartupBanner)(VARIANT_BOOL bNoLogo);
	STDMETHOD(get_OutputFile)(BSTR* pbstrOutputFile);	// (/o [file]) change the output file name (default is based on 1st lib or obj name on cmd line)
	STDMETHOD(put_OutputFile)(BSTR bstrOutputFile);
	STDMETHOD(get_RunBSCMakeTool)(VARIANT_BOOL* pbRun);	// run the BSCMake tool; acts as inverse of ExcludeFromBuild
	STDMETHOD(put_RunBSCMakeTool)(VARIANT_BOOL bRun);

	// Automation properties
	STDMETHOD(get_ToolPath)(BSTR *pbstrToolPath);
	STDMETHOD(get_PropertyOption)(BSTR bstrProp, long dispidProp, BSTR *pVal)
	{ return DoGetPropertyOption(bstrProp, dispidProp, pVal); }
	STDMETHOD(get_VCProjectEngine)(IDispatch** projEngine)	{ return DoGetVCProjectEngine(projEngine); }

// IVCToolImpl
public:
	STDMETHOD(get_DefaultExtensions)(BSTR* pVal);
	STDMETHOD(put_DefaultExtensions)(BSTR newVal);
	STDMETHOD(GetAdditionalOptionsInternal)(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, BSTR* pbstrOptions);
	STDMETHOD(GetBuildDescription)(IVCBuildAction* pAction, BSTR* pbstrBuildDescription);
	STDMETHOD(get_ToolPathInternal)(BSTR* pbstrToolPath);
	STDMETHOD(get_ToolShortName)(BSTR* pbstrToolName);
	STDMETHOD(CreateToolObject)(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject);
	STDMETHOD(MatchName)(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches);
	STDMETHOD(get_Bucket)(long *pVal);
	STDMETHOD(IsTargetTool)(IVCBuildAction* pAction, VARIANT_BOOL* pbIsTargetTool);
	STDMETHOD(get_PerformIfAble)(VARIANT_BOOL* pbPerformIfAble);
	STDMETHOD(get_IsFileTool)(VARIANT_BOOL* pbIsFileTool);
	STDMETHOD(PrePerformBuildActions)(bldActionTypes type, IVCBuildActionList* pActions, bldAttributes aob, IVCBuildErrorContext* pEC, actReturnStatus* pActReturn);
	STDMETHOD(PostPerformBuildActions)(bldActionTypes type, IVCBuildActionList* pActions, bldAttributes aob, IVCBuildErrorContext* pEC, actReturnStatus* pActReturn);
	STDMETHOD(IsDeleteOnRebuildFile)(LPCOLESTR szFile, VARIANT_BOOL* pbDelOnRebuild);
	STDMETHOD(GenerateOutput)(long type, IVCBuildActionList* pActions, IVCBuildErrorContext* pEC);
	STDMETHOD(AffectsOutput)(long nPropID, VARIANT_BOOL* pbAffectsOutput);
	STDMETHOD(CreatePageObject)(IUnknown **, CLSID*, IVCPropertyContainer *, IVCSettingsPage * );
	STDMETHOD(GetPrimaryOutputIDFromTool)(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, long* pnOutputID);	// return a value only if tool generates a primary output (ex: linker)
	STDMETHOD(SaveObject)(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent) { return S_FALSE; }	// not sharable
	STDMETHOD(WriteToStream)(IStream *pStream, IVCPropertyContainer* pPropContainer);
	STDMETHOD(ReadFromStream)(IStream *pStream, IVCPropertyContainer* pPropContainer);

	STDMETHOD(get_ToolDisplayIndex)(long* pIndex)
	{
		*pIndex = TOOL_DISPLAY_INDEX_BSC;
		return S_OK;
	}

	virtual long GetPageCount() { return 2; }
	virtual GUID* GetPageIDs();

// helpers
public:
	virtual COptionHandlerBase* GetOptionHandler() { return &s_optHandler; }
	virtual HRESULT GetPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, CPathW& rpath);	// return a value only if tool generates a primary output (ex: linker)

protected:
	virtual BOOL UsesResponseFiles() { return TRUE; }
	virtual LPCOLESTR GetToolFriendlyName();
	void InitToolName();
	virtual long GetToolID() { return IDS_BSC; }

public:
	static CBscMakeToolOptionHandler s_optHandler;

protected:
	static CComBSTR s_bstrToolName;
};

class ATL_NO_VTABLE CVCBscMakePage :
	public IDispatchImpl<IVCBscMakePage, &IID_IVCBscMakePage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public CPageObjectImpl<CVCBscMakePage,VCBSCMAKETOOL_MIN_DISPID,VCBSCMAKETOOL_MAX_DISPID>,
	public CComObjectRoot
{
public:

BEGIN_COM_MAP(CVCBscMakePage)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
	COM_INTERFACE_ENTRY(IVCBscMakePage)
END_COM_MAP()

	// IDispatch override
	STDMETHOD(Invoke)( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pexcepInfo, UINT *puArgErr )
	{
		IDispatchImpl<IVCBscMakePage, &IID_IVCBscMakePage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>::Invoke( dispid, riid, lcid, wFlags, pdispParams, pvarResult, pexcepInfo, puArgErr );
		return S_OK;
	}


// IVCBscMakePage
public:
	// general; @response deliberately left out
	STDMETHOD(get_AdditionalOptions)(BSTR* pbstrAdditionalOptions);	// anything we don't know what to do with, given verbatim, in same order as entered, parent contributions first, goes after all other options
	STDMETHOD(put_AdditionalOptions)(BSTR bstrAdditionalOptions);
	STDMETHOD(get_SuppressStartupBanner)(enumSuppressStartupBannerBOOL* pbNoLogo);	// (/nologo) enable suppression of copyright message
	STDMETHOD(put_SuppressStartupBanner)(enumSuppressStartupBannerBOOL bNoLogo);
	STDMETHOD(get_OutputFile)(BSTR* pbstrOutputFile);	// (/o [file]) change the output file name (default is based on 1st lib or obj name on cmd line)
	STDMETHOD(put_OutputFile)(BSTR bstrOutputFile);

// helpers
public:
	virtual void GetBaseDefault(long id, CComVariant& varValue);
};
