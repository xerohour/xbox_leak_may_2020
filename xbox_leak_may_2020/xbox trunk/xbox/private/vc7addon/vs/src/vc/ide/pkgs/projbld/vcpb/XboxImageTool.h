//////////////////////////////////////////////////////////////////////
//
// XboxImageTool.h: Definition of the Xbox Image Tool classes
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "vctool.h"
#include "settingspage.h"

template<class T, class IFace>
class ATL_NO_VTABLE CXboxImageBasePage :
	public IDispatchImpl<IFace, &(__uuidof(IFace)), &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber, 0, CVsTypeInfoHolder>,
	public CPageObjectImpl<T, XBOXIMAGETOOL_MIN_DISPID, XBOXIMAGETOOL_MAX_DISPID>,
	public CComObjectRoot
{
public:
BEGIN_COM_MAP(T)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IFace)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
	COM_INTERFACE_ENTRY(IProvidePropertyBuilder)
END_COM_MAP()

	// IDispatch override
	STDMETHOD(Invoke)( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pexcepInfo, UINT *puArgErr )
	{
		IDispatchImpl<IFace, &(__uuidof(IFace)), &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>::Invoke( dispid, riid, lcid, wFlags, pdispParams, pvarResult, pexcepInfo, puArgErr );
		return S_OK;
	}

	// Can override Commit here, if nescessary. See XboxImage tool for an example.
};

class ATL_NO_VTABLE CXboxImageGeneralPage :
	public CXboxImageBasePage<CXboxImageGeneralPage, IXboxImageGeneralPage>
{
// IXboxImageGeneralPage
public:
	STDMETHOD(get_AdditionalOptions)(BSTR* pbstrAdditionalOptions);
	STDMETHOD(put_AdditionalOptions)(BSTR pbstrAdditionalOptions);
	STDMETHOD(get_FileName)(BSTR* pbstrFileName);
	STDMETHOD(put_FileName)(BSTR pbstrFileName);
	STDMETHOD(get_XBEFlags)(long* pnFlags);
	STDMETHOD(put_XBEFlags)(long pnFlags);
	STDMETHOD(get_StackSize)(long* pnStackSize);
	STDMETHOD(put_StackSize)(long nStackSize);
	STDMETHOD(get_IncludeDebugInfo)(enumBOOL* pbIncludeDebugInfo);
	STDMETHOD(put_IncludeDebugInfo)(enumBOOL bIncludeDebugInfo);
	STDMETHOD(get_LimitAvailableMemoryTo64MB)(enumBOOL* pbLimitAvailableMemoryTo64MB);
	STDMETHOD(put_LimitAvailableMemoryTo64MB)(enumBOOL bLimitAvailableMemoryTo64MB);

// helpers
public:
	virtual void GetBaseDefault(long id, CComVariant& varValue);
};

class ATL_NO_VTABLE CXboxImageCertificatePage :
	public CXboxImageBasePage<CXboxImageCertificatePage, IXboxImageCertificatePage>
{
// IVCXboxImageCertificatePage
public:
	STDMETHOD(get_TitleID)(long* pnTitleID);
	STDMETHOD(put_TitleID)(long pnTitleID);
	STDMETHOD(get_XboxGameRegion)(long* pnXboxGameRegion);
	STDMETHOD(put_XboxGameRegion)(long nXboxGameRegion);
	STDMETHOD(get_XboxGameRating)(enumXboxGameRating* peXboxGameRating);
	STDMETHOD(put_XboxGameRating)(enumXboxGameRating eXboxGameRating);
};

class ATL_NO_VTABLE CXboxImageTitleInfoPage :
	public CXboxImageBasePage<CXboxImageTitleInfoPage, IXboxImageTitleInfoPage>
{
// IVCXboxImageTitleInfoPage
public:
	STDMETHOD(get_TitleInfo)(BSTR* pbstrTitleInfo);
	STDMETHOD(put_TitleInfo)(BSTR pbstrTitleInfo);
	STDMETHOD(get_TitleName)(BSTR* pbstrTitleName);
	STDMETHOD(put_TitleName)(BSTR pbstrTitleName);
	STDMETHOD(get_TitleImage)(BSTR* pbstrTitleImage);
	STDMETHOD(put_TitleImage)(BSTR pbstrTitleImage);
	STDMETHOD(get_SaveGameImage)(BSTR* pbstrSaveGameImage);
	STDMETHOD(put_SaveGameImage)(BSTR pbstrSaveGameImage);
};

// classes in this header
class CXboxImageTool;
class CXboxImageToolOptionHandler;

class CXboxImageToolOptionHandler : public COptionHandlerBase
{
public:
	DECLARE_OPTION_TABLE();

public:
	// default value handlers
	// string props
	virtual void GetDefaultValue( long id, BSTR *pVal, IVCPropertyContainer *pPropCnt = NULL );
	// integer props
	virtual void GetDefaultValue( long id, long *pVal, IVCPropertyContainer *pPropCnt = NULL );
	// boolean props
	virtual void GetDefaultValue( long id, VARIANT_BOOL *pVal, IVCPropertyContainer *pPropCnt = NULL );

protected:
	virtual BOOL SynthesizeOptionIfNeeded(IVCPropertyContainer* pPropContainer, long idOption);
	virtual BOOL SynthesizeOption(IVCPropertyContainer* pPropContainer, long idOption, CComVariant& rvar);
public:
	BOOL CreateFileName(IVCPropertyContainer* pPropContainer, BSTR *bstrVal, BOOL bCheckForExistence = TRUE);
};

class CXboxImageToolHelper
{
public:
	HRESULT DoGenerateOutput(long type, IVCBuildActionList* pActions, IVCBuildErrorContext* pEC,
		IVCToolImpl* pTool);
	HRESULT DoGetPrimaryOutputFromTool(IUnknown* pItem, CPathW& rpath);
	HRESULT DoHasDependencies(IVCBuildAction* pAction, VARIANT_BOOL* pbHasDependencies);
	HRESULT DoGetDependencies(IVCBuildAction* pAction, IVCBuildStringCollection** ppStrings, VARIANT_BOOL* pbUpToDate,
		BOOL bAllowDirty = TRUE);

protected:
	virtual BOOL CreateFileName(IVCPropertyContainer* pPropContainer, BSTR* pbstrVal) PURE;
	virtual HRESULT DoHasPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, VARIANT_BOOL* pbHasPrimaryOutput) PURE;
	int GetNextDep(int nStartDep, CStringW& strDepList, int nDepLen, CStringW& strDep);
	HRESULT DoGetAdditionalDependenciesInternal(IVCPropertyContainer* pItem, IVCBuildAction* pAction, BOOL bForSave, 
		BSTR* pbstrInputs, COptionHandlerBase* poptHandler, UINT idUserDeps, UINT idProjDeps, LPCOLESTR szExtras = L"");

public:
	static void GetProjectCfgForItem(IUnknown* pItem, VCConfiguration** pProjCfg);
};

/////////////////////////////////////////////////////////////////////////////
// CXboxImageTool

class ATL_NO_VTABLE CXboxImageTool	: 
	public IDispatchImpl<XboxImageTool, &IID_XboxImageTool, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public CComObjectRoot,
	public CXboxImageToolHelper,
	public CVCToolImpl
{
public:
	CXboxImageTool(); 
	static HRESULT CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool);

BEGIN_COM_MAP(CXboxImageTool)
 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCToolImpl)
	COM_INTERFACE_ENTRY(XboxImageTool)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(CXboxImageTool) 

#define XBOXIMAGETOOL_NUMPAGES 3

// VCXboxImageTool
public:
	// general
	STDMETHOD(get_ToolName)(BSTR* pbstrToolName);	// friendly name of tool, e.g., "XboxImage Tool"
	STDMETHOD(get_AdditionalOptions)(BSTR* pbstrAdditionalOptions);	// anything we don't know what to do with, given verbatim, in same order as entered, parent contributions first, goes after all other options
	STDMETHOD(put_AdditionalOptions)(BSTR bstrAdditionalOptions);
	STDMETHOD(get_FileName)(BSTR* pbstrFileName);
	STDMETHOD(put_FileName)(BSTR pbstrFileName);
	STDMETHOD(get_XBEFlags)(long* pnFlags);
	STDMETHOD(put_XBEFlags)(long pnFlags);
	STDMETHOD(get_StackSize)(long* pnStackSize);
	STDMETHOD(put_StackSize)(long nStackSize);
	STDMETHOD(get_IncludeDebugInfo)(VARIANT_BOOL* pbIncludeDebugInfo);
	STDMETHOD(put_IncludeDebugInfo)(VARIANT_BOOL bIncludeDebugInfo);
	STDMETHOD(get_LimitAvailableMemoryTo64MB)(VARIANT_BOOL* pbLimitAvailableMemoryTo64MB);
	STDMETHOD(put_LimitAvailableMemoryTo64MB)(VARIANT_BOOL bLimitAvailableMemoryTo64MB);
	// certificate
	STDMETHOD(get_TitleID)(long* pnTitleID);
	STDMETHOD(put_TitleID)(long pnTitleID);
	STDMETHOD(get_XboxGameRegion)(long* pnXboxGameRegion);
	STDMETHOD(put_XboxGameRegion)(long nXboxGameRegion);
	STDMETHOD(get_XboxGameRating)(enumXboxGameRating* peXboxGameRating);
	STDMETHOD(put_XboxGameRating)(enumXboxGameRating eXboxGameRating);
	// title info
	STDMETHOD(get_TitleInfo)(BSTR* pbstrTitleInfo);
	STDMETHOD(put_TitleInfo)(BSTR pbstrTitleInfo);
	STDMETHOD(get_TitleName)(BSTR* pbstrTitleName);
	STDMETHOD(put_TitleName)(BSTR pbstrTitleName);
	STDMETHOD(get_TitleImage)(BSTR* pbstrTitleImage);
	STDMETHOD(put_TitleImage)(BSTR pbstrTitleImage);
	STDMETHOD(get_SaveGameImage)(BSTR* pbstrSaveGameImage);
	STDMETHOD(put_SaveGameImage)(BSTR pbstrSaveGameImage);

	// Automation properties
	STDMETHOD(get_ToolPath)(BSTR *pVal);
	STDMETHOD(get_PropertyOption)(BSTR bstrProp, long dispidProp, BSTR *pVal) { return DoGetPropertyOption(bstrProp, dispidProp, pVal); }
	STDMETHOD(get_VCProjectEngine)(IDispatch** projEngine)	{ return DoGetVCProjectEngine(projEngine); }

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
	STDMETHOD(IsTargetTool)(IVCBuildAction* pAction, VARIANT_BOOL* pbTargetTool); // tool always operates on target, not on file
	STDMETHOD(IsDeleteOnRebuildFile)(LPCOLESTR szFile, VARIANT_BOOL* pbDelOnRebuild);
	STDMETHOD(get_ToolShortName)(BSTR* pbstrToolName);
	STDMETHOD(MatchName)(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches);
	STDMETHOD(CreateToolObject)(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject);
	STDMETHOD(HasPrimaryOutputFromTool)(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, VARIANT_BOOL* pbHasPrimaryOutput);	// returns TRUE only if tool generates a primary output (ex: XboxImage)
	STDMETHOD(HasDependencies)(IVCBuildAction* pAction, VARIANT_BOOL* pbHasDependencies);
	STDMETHOD(GetDependencies)(IVCBuildAction* pAction, IVCBuildStringCollection** ppStrings, VARIANT_BOOL* pbUpToDate);
	STDMETHOD(CanScanForDependencies)(VARIANT_BOOL* pbIsScannable);
	STDMETHOD(IsSpecialConsumable)(LPCOLESTR szPath, VARIANT_BOOL* pbSpecial);
	STDMETHOD(CreatePageObject)(IUnknown **, CLSID*, IVCPropertyContainer *, IVCSettingsPage * );
	STDMETHOD(GetPrimaryOutputIDFromTool)(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, long* pnOutputID);	// return a value only if tool generates a primary output (ex: XboxImage)

	STDMETHOD(get_ToolDisplayIndex)(long* pIndex)
	{
		CHECK_POINTER_NULL(pIndex);
		*pIndex = TOOL_DISPLAY_INDEX_XBOXIMAGE;
		return S_OK;
	}

// helpers

public:
	virtual COptionHandlerBase* GetOptionHandler() { return &s_optHandler; }
	virtual HRESULT GetPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, CPathW& rpath);	// return a value only if tool generates a primary output (ex: XboxImage)
	virtual long GetPageCount();
	virtual GUID* GetPageIDs();

protected:
	virtual BOOL UsesResponseFiles() { return TRUE; }
	virtual BOOL CreateFileName(IVCPropertyContainer* pPropContainer, BSTR* pbstrVal);
	virtual HRESULT DoHasPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, VARIANT_BOOL* pbHasPrimaryOutput)
	{ return HasPrimaryOutputFromTool(pItem, bSchmoozeOnly, pbHasPrimaryOutput); }
	virtual LPCOLESTR GetToolFriendlyName();
	void InitToolName();
	virtual long GetToolID() { return IDS_XBOXIMAGETOOL; }

protected:
	static const wchar_t *s_pszDelOnRebuildExtensions;
	static CComBSTR s_bstrBuildDescription;
	static CComBSTR s_bstrToolName;
	static CComBSTR s_bstrExtensions;

	static GUID s_pPages[XBOXIMAGETOOL_NUMPAGES];
	static BOOL s_bPagesInit;

public:
	static CXboxImageToolOptionHandler s_optHandler;
};

// These macros are used to define enum ranges for tool switches.  If you add an enum property to the tool option
// table, please add a matching macro for it to this list.  Use of macros for range definitions helps in keeping
// maintenance of enum range checks as low as possible.
#define XboxGameRatingMin			 0
#define XboxGameRatingMax			 7
