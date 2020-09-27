// VCProjectEngine.h : Declaration of the CVCProjectEngine

#ifndef __VCPROJECTENGINE_H_
#define __VCPROJECTENGINE_H_

#include "VcpbengCP.h"
#include "comlist.h"
#include "scriptexecutor.h"
#include <vccolls.h>
#include <vcmap.h>
#include "PropContainer.h"
#include "vcprojcnvt2.h"
#include <iadmw.h>
#include <iiscnfg.h>

interface IVSMDPropertyGrid;
interface IVsHelp;

class CBldCfgRecord;

// forward declarations
class CAdsiWrapper;

/////////////////////////////////////////////////////////////////////////////
// CVCProjectEngine
class ATL_NO_VTABLE CVCProjectEngineEvents : 
	public CComObjectRoot,
	public IVCProjectEngineEventsImpl,
	public IConnectionPointContainerImpl<CVCProjectEngineEvents>,
	public CProxy_IVCProjectEngineEvents<CVCProjectEngineEvents, &DIID__dispVCProjectEngineEvents>,
	public IProvideClassInfo2Impl<&CLSID_VCProjectEngineEvents, &DIID__dispVCProjectEngineEvents, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber, /*unused =*/ 0, CVsTypeInfoHolder>,
	public IDispatchImpl<_VCProjectEngineEvents, &IID__VCProjectEngineEvents, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber, /*unused =*/ 0, CVsTypeInfoHolder>
{
public:
BEGIN_COM_MAP(CVCProjectEngineEvents)
	COM_INTERFACE_ENTRY(_VCProjectEngineEvents)
	COM_INTERFACE_ENTRY(IVCProjectEngineEventsImpl)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CVCProjectEngineEvents)
	CONNECTION_POINT_ENTRY(DIID__dispVCProjectEngineEvents)
END_CONNECTION_POINT_MAP()

public:
	STDMETHOD(DoFireItemAdded)(IDispatch *pDispObj, IDispatch *pDispParent);
	STDMETHOD(DoFireItemRemoved)(IDispatch *pDispObj, IDispatch *pDispParent);
	STDMETHOD(DoFireItemRenamed)(IDispatch *pDispObj, IDispatch *pDispParent, BSTR bstrOldName);
	STDMETHOD(DoFireItemMoved)(IDispatch* pDispObj, IDispatch* pNewParent, IDispatch* pOldParent);
	STDMETHOD(DoFireItemPropertyChange)(IDispatch *pDispObj, long lDispid);
	STDMETHOD(DoFireSccEvent)(IDispatch* pItem, enumSccEvent event, VARIANT_BOOL *pRet);
	STDMETHOD(DoFireReportError)(BSTR bstrErrMsg, HRESULT hrReport, BSTR bstrHelpKeyword);
	STDMETHOD(DoFireProjectBuildStarted)(IDispatch *pCfg);
	STDMETHOD(DoFireProjectBuildFinished)(IDispatch *pCfg, long errors, long warnings, VARIANT_BOOL bCancelled);

	static HRESULT CreateInstance(_VCProjectEngineEvents** ppDisp)
	{
		CComObject<CVCProjectEngineEvents> *pObj;
		HRESULT hr = CComObject<CVCProjectEngineEvents>::CreateInstance(&pObj);
		if (SUCCEEDED(hr))
		{
			CVCProjectEngineEvents *pVar = pObj;
			pVar->AddRef();
			*ppDisp = pVar;
		}
		return hr;
	}
};

class ATL_NO_VTABLE CVCProjectEngine : 
	public CComObjectRoot,
	public CComCoClass<CVCProjectEngine, &CLSID_VCProjectEngineObject>,
	public IConnectionPointContainerImpl<CVCProjectEngine>,
	public CPropertyContainerImpl,
	public CProxy_IVCProjectEngineEvents<CVCProjectEngine, &__uuidof(IVCProjectEngineEvents)>,
	public IDispatchImpl<VCProjectEngine, &IID_VCProjectEngine, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber, /*unused =*/ 0, CVsTypeInfoHolder>,
	public IVCProjectEngineImpl
{
public:
	CVCProjectEngine();
	~CVCProjectEngine();


BEGIN_COM_MAP(CVCProjectEngine)
	COM_INTERFACE_ENTRY(VCProjectEngine)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCProjectEngineImpl)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CVCProjectEngine)
	CONNECTION_POINT_ENTRY(__uuidof(IVCProjectEngineEvents))
END_CONNECTION_POINT_MAP()

DECLARE_VS_REGISTRY_RESOURCEID(IDR_VCPROJECTENGINE)

	static HRESULT CreateInstance(CVCProjectEngine **ppProjectEngine);

// IVCProjectEngineImpl
public:
	// Review: this should probably be public ?
	STDMETHOD(get_SuppressUI)(/*[out, retval]*/ VARIANT_BOOL *pVal); // don't trigger anything that would cause ui
	STDMETHOD(put_SuppressUI)(/*[in]*/ VARIANT_BOOL newVal);
	// Does any and all initialization
	STDMETHOD(Initialize)(LONG_PTR hInst, IUnknown *pSP, IVSMDPropertyBrowser *pBrowser, IVCBuildPackageInternal* pBuildPackage);
	STDMETHOD(Close)();
	STDMETHOD(DoFireItemAdded)(IDispatch *pDispObj, IDispatch *pDispParent);
	STDMETHOD(DoFireItemRemoved)(IDispatch *pDispObj, IDispatch *pDispParent);
	STDMETHOD(DoFireItemRenamed)(IDispatch *pDispObj, IDispatch *pDispParent, BSTR bstrOldName);
	STDMETHOD(DoFireItemMoved)(IDispatch* pDispObj, IDispatch* pNewParent, IDispatch* pOldParent);
	STDMETHOD(DoFireItemPropertyChange)(IDispatch *pDispObj, long lDispid);
	STDMETHOD(DoFireSccEvent)(IDispatch* pItem, enumSccEvent event, VARIANT_BOOL *pRet);
	STDMETHOD(DoFireReportError)(BSTR bstrErrMsg, HRESULT hrReport, BSTR bstrHelpKeyword);
	STDMETHOD(DoFireProjectBuildStarted)(IDispatch *pCfg);
	STDMETHOD(DoFireProjectBuildFinished)(IDispatch *pCfg, long errors, long warnings, VARIANT_BOOL bCancelled);
	STDMETHOD(CreatePropertyGrid)(IVSMDPropertyGrid**);
	STDMETHOD(GetHelp)(IVsHelp **ppHelp);
	STDMETHOD(get_BuildEngine)(IDispatch **ppBuildEngine);
	STDMETHOD(WriteLog)(LPCOLESTR szOut, BOOL bAppendCRLF);
	STDMETHOD(get_DialogOwner)(HWND *pHwnd);
	STDMETHOD(EnableModeless)(BOOL bEnable);
	STDMETHOD(LoadProjectUnderShell)(LPCOLESTR szProject, IDispatch **pProject);
	STDMETHOD(RemoveProjectUnderShell)(IDispatch * pProject);
	STDMETHOD(UnloadStyleSheet)(IDispatch* pStyleSheet);	// call this to tell project engine we're done with this style sheet
	STDMETHOD(get_AllStyleSheets)(IDispatch** ppStyleSheets);
	STDMETHOD(CreateHiddenStyleSheet)(LPCOLESTR szName, IDispatch **ppDispStyle);
	STDMETHOD(LoadHiddenStyleSheet)(LPCOLESTR szStyleSheet, IDispatch **ppDispStyle);
	STDMETHOD(GetIVCProjConvert)( IUnknown ** ppDispProjConvert );
	STDMETHOD(GetSKU)( long *pSKU );
	STDMETHOD(GetDialogFont)( BOOL bBold, HFONT *pHFont );
	STDMETHOD(get_VendorTools)(IDispatch** ppVendorTools);
	STDMETHOD(AddVendorTool)(LPCOLESTR szToolName, LPCOLESTR szToolGUID, IDispatch** ppVendorTool);
	STDMETHOD(GetStringForPropertyID)(long nID, BSTR* pbstrString);

// VCProjectEngine
public:
	STDMETHOD(get_BuildLogging)(VARIANT_BOOL * pbLog);
	STDMETHOD(put_BuildLogging)(VARIANT_BOOL pbLog);
	STDMETHOD(get_BuildTiming)(VARIANT_BOOL * pbTime);
	STDMETHOD(put_BuildTiming)(VARIANT_BOOL pbTime);
	STDMETHOD(get_Platforms)(IDispatch * * ppPlatforms);
	STDMETHOD(get_Projects)(IDispatch * * ppProjects);
	STDMETHOD(get_StyleSheets)(IDispatch** ppStyleSheets);
	STDMETHOD(CreateProject)(BSTR szProject, IDispatch * * pProject);
	STDMETHOD(LoadProject)(BSTR szProject, IDispatch * * pProject);
	STDMETHOD(RemoveProject)(IDispatch * pProject);
	STDMETHOD(LoadStyleSheet)(BSTR bstrName, IDispatch **ppDispStyle);
	STDMETHOD(CreateStyleSheet)(BSTR bstrName, IDispatch **ppDispStyle);
	STDMETHOD(RemoveStyleSheet)(IDispatch* pDispStyle);
	STDMETHOD(get_PerformanceLogging)(VARIANT_BOOL *pbLog);
	STDMETHOD(put_PerformanceLogging)(VARIANT_BOOL bLog);
	STDMETHOD(IsSystemInclude)(BSTR bstrInclude, VARIANT_BOOL *pbInclude);
	STDMETHOD(get_Events)(IDispatch **pProjecEngineEvents);
	STDMETHOD(Evaluate)(BSTR In, BSTR* Out) { return CPropertyContainerImpl::Evaluate(In, Out); }

// IVCPropertyContainer; most methods in base class
public:
	STDMETHOD(GetMultiProp)(long id, LPCOLESTR szSeparator, VARIANT_BOOL bSkipLocal, BSTR *varValue);

// default ISpecifyPropertyPages method in base class
       
protected:
	HRESULT GetPlatforms();
	HRESULT DoGetProp(long id, VARIANT* pvarValue);
	HRESULT DoSetProp(long id, BOOL bCheckSpecial, VARIANT varValue, long nOverrideID = -1);
	HRESULT DoSetStrProperty(long idProp, BOOL bCheckSpecial, BSTR bstrValue, long nOverrideID = -1);
	HRESULT DoSetBoolProperty(long idProp, BOOL bCheckSpecial, VARIANT_BOOL bValue, long nOverrideID = -1);
	virtual BOOL CheckCanDirty() { return FALSE; }
	HRESULT DoLoadHiddenStyleSheet(LPCOLESTR szStyleSheet, VCStyleSheet** ppStyleSheet, IDispatch** ppDispStyleSheet);
	HRESULT DoCreateHiddenStyleSheet(LPCOLESTR szStyleSheet, VCStyleSheet** ppStyle, IDispatch** ppDispStyle);
	HRESULT DoRemoveStyleSheet(IDispatch* pDispStyle, BOOL bRemoveFromHiddenList);
	HRESULT SetBuildLogging(VARIANT_BOOL bLog, bool bPersist);
	HRESULT SetBuildTiming(VARIANT_BOOL bTime, bool bPersist);
	HRESULT SetPerformanceLogging(VARIANT_BOOL bLog, bool bPersist);

// helper functions
protected:
	HRESULT LoadXML( BSTR bstrFile, IDispatch **ppDispProject );
	HRESULT DoInitialize(LONG_PTR hInst, IUnknown *pSP, IVSMDPropertyBrowser *pBrowser, IVCBuildPackageInternal* pBuildPackage,
		BOOL bForceInit);
	void ClearPlatforms();

protected:
	DWORD m_dwCookieComposite;	// cookie for RunningObjectTable for the composite moniker

	VARIANT_BOOL m_bPerformanceLogging;	// Do we log performance statistics to stdout?; NOTE: if unhidden, USE PROPCONTAINER SPACE FOR IT
	VARIANT_BOOL m_bSuppressUI;	// Try not to trigger any UI when this is true

	bool m_bInitialized;		// Have we truly been initialized....
	bool m_bSoftInitialized;	// Have we been even sort of initialized...
	CComPtr<IVSMDPropertyBrowser>	m_pPropBrowser;
	CComDynamicListTyped<VCProject> m_rgProjects; // collection of projects. 
	CComDynamicListTyped<VCStyleSheet> m_rgStyleSheets; // collection of 'visible' style sheets. 
	CComDynamicListTyped<VCStyleSheet> m_rgAllStyleSheets; // collection of *all* style sheets. 
	CComDynamicListTyped<VCPlatform> m_rgPlatforms; // collection of Platforms. 
	CComDynamicListTyped<IVCToolImpl> m_rgVendorTools;	// collection of vendor-supplied tools
	CComPtr<IActiveScriptSite> m_pScriptSite; // we use this to clean up...
	CComPtr<IVCProjConvert> m_pProjConvert;		// this is needed on the UI side to setup VC 4.x dependencies

	CComPtr<_VCProjectEngineEvents> m_pProjectEngineEvents;

	long m_nSKU;
	long m_nNextVendorID;

public:
	static BOOL DoWriteLog(LPCOLESTR szOut, BOOL bAppendCRLF = FALSE);
	static void InitializeForCommandLineUsage();
	static void AddProjectError(IVCBuildErrorContext* pEC, BSTR bstrErr, BSTR bstrPrjErrCode, IVCPropertyContainer* pPropContainer);
	static void AddProjectError(IVCBuildErrorContext* pEC, BSTR bstrErr, BSTR bstrPrjErrCode, IVCBuildActionList* pActionList);
	static void AddProjectError(IVCBuildErrorContext* pEC, BSTR bstrErr, BSTR bstrPrjErrCode, IVCBuildAction* pAction);
	static void AddProjectError(IVCBuildErrorContext* pEC, BSTR bstrErr, BSTR bstrPrjErrCode, CBldCfgRecord* pCfgRecord);
	static void AddProjectError(IVCBuildErrorContext* pEC, CStringW& strErr, BSTR bstrPrjErrCode,
		IVCPropertyContainer* pPropContainer);
	static void AddProjectError(IVCBuildErrorContext* pEC, CStringW& strErr, BSTR bstrPrjErrCode, IVCBuildAction* pAction);
	static void AddProjectError(IVCBuildErrorContext* pEC, CStringW& strErr, BSTR bstrPrjErrCode, IVCBuildActionList* pActionList);
	static void AddProjectError(IVCBuildErrorContext* pEC, CStringW& strErr, BSTR bstrPrjErrCode, CBldCfgRecord* pCfgRecord);
	static void AddProjectWarning(IVCBuildErrorContext* pEC, BSTR bstrWarn, BSTR bstrPrjWarnCode,
		IVCPropertyContainer* pPropContainer);
	static void AddProjectWarning(IVCBuildErrorContext* pEC, CStringW& strWarn, BSTR bstrPrjWarnCode,
		IVCPropertyContainer* pPropContainer);
	static void AddProjectWarning(IVCBuildErrorContext* pEC, CStringW& strWarn, BSTR bstrPrjWarnCode,
		CBldCfgRecord* pCfgRecord);
	static void DoUpdateAfterApply();
	static void GetBasePlatformInfoKey( BSTR *pbstrKey );
	static void GetPlatformInfoKey( BSTR bstrPlatform, BSTR bstrMainKey, BSTR *pbstrKey );
	static void ResolvePropertyContainer(IVCPropertyContainer* pPropContainer, IVCPropertyContainer** ppActualContainer);
	static BOOL FLoadMlang();
	static UINT ConvertFromUnicodeMlang(UINT cp, BOOL fNoBestFit, BOOL fWriteEntities, LPCWSTR rgchUtf16, UINT cchUtf16, 
		LPSTR rgchMbcs, UINT cchMbcs, BOOL *pfDefCharUsed);
	static BOOL GetCodePageName(int iCodePage, CComBSTR& bstrEncoding);
	static int GetCodePageFromCharSet(CComBSTR& bstrCharSet);
	static HRESULT GetPathWWWRoot(CComBSTR& bstrRoot);
	static HRESULT IISGetLocalPathForVirtualRoot(LPCWSTR pszWebUrl, CStringW& strLocalPath);
	static HRESULT DoSetErrorInfo(HRESULT hrErr, long idErr, BOOL bReportError = TRUE, BSTR bstrHelpTopic = NULL, 
		BOOL bErrorInfoOnly = TRUE);
	static HRESULT DoSetErrorInfo2(HRESULT hrErr, long idErr, LPCOLESTR szParam, BOOL bReportError = TRUE, 
		BSTR bstrHelpTopic = NULL, BOOL bErrorInfoOnly = TRUE);
	static HRESULT DoSetErrorInfo3(HRESULT hrErr, long idErr, LPCOLESTR szParam1, LPCOLESTR szParam2, 
		BOOL bReportError = TRUE, BSTR bstrHelpTopic = NULL, BOOL bErrorInfoOnly = TRUE);
	static HRESULT HandleSetErrorInfo(HRESULT hrErr, LPCOLESTR szErr, BSTR bstrHelpTopic, BOOL bErrorInfoOnly);

	HRESULT GetOpenFileNameViaDlg( VSOPENFILENAMEW* pOpenFileName );
	HRESULT GetDirViaDlg( VSBROWSEINFOW* pDir );

protected:
	static HRESULT InitAdminBase();
	static HRESULT IISGetWebNumberForUrl(LPCWSTR pszWebUrl, CStringW& strWebNum);
	static HRESULT GetWebNumberForServer(LPCWSTR pszADSRootPath,           // IN: path to ads root
                              const CStringW& strIISBindingsGenericMatch, // IN: generic IIS bindings
                              const CStringW& strIISBindingsBestMatch,   // IN: best IIS bindings
                              CStringW &strWebNum);               // OUT: web number

public:
	static VARIANT_BOOL s_bBuildLogging;	// Do we log builds to a file
											// NOTE: keep propcontainer IN PARALLEL WITH s_bBuildLogging
	static BOOL s_fInitializedCom;
	static IVCBuildPackageInternal* s_pBuildPackage;
	static CComPtr<IServiceProvider> s_pServiceProvider;
	static CComPtr<IMultiLanguage2> s_spMultiLanguage2;
	static CComPtr<IMultiLanguage> s_spMultiLanguage;
	static CComPtr<IMSAdminBase> s_spAdminBase;
	static HANDLE s_hLogFile;				// log file handle
	static CComBSTR s_bstrAlternateRegistryRoot;
	static CComBSTR s_bstrIISRoot;		// root to where IIS is expecting PathWWWRoot to be
	static CAdsiWrapper* s_pAdsiWrapper;
};

_declspec(selectany) VCProjectEngine * g_pProjectEngine = NULL; // NOTE: Not Addref'd.

extern BOOL g_bInProjLoad;			// project load in progres
extern HRESULT ExternalQueryService(REFGUID rsid, REFIID iid, void **ppvObj);

inline BOOL UsingRegistry( void )
{
	wchar_t buff[4096];
	buff[0] = L'0';
	::GetEnvironmentVariableW(L"VC_PROJECT_ENGINE_NOT_USING_REGISTRY_FOR_INIT", buff, 4095);
	if( buff[0] == L'1' )
	{
	    return FALSE;
	}
	return TRUE;
}

class CInLoadProject
{
public:
	CInLoadProject() { m_bOldInLoad = g_bInProjLoad; g_bInProjLoad = TRUE; }
	~CInLoadProject() { g_bInProjLoad = m_bOldInLoad; }
	BOOL m_bOldInLoad;
};

#if 0
/////////////////////////////////////////////////////////////////////////////
// CBuildPackage
// This Pkg only exists to provide VsCoCreate functionality and to provide SetSite
// The DLL is deisgned to act a co-creatable engine independant of the shell.
// This IVsPackage should provide absolute minimal functionality.
class CEnginePkg: 
    public IVsPackage,
    public IServiceProvider,
	public CComObjectRoot,
	public CComCoClass<CEnginePkg, &CLSID_EnginePackage>
{
public:
	CEnginePkg(){};
	~CEnginePkg(){};

DECLARE_REGISTRY_RESOURCEID(IDR_ENGINEPACKAGE)

BEGIN_COM_MAP(CBuildPackage)
	COM_INTERFACE_ENTRY(IVsPackage)
	COM_INTERFACE_ENTRY(IServiceProvider)
END_COM_MAP()

static CComPtr<IServiceProvider> s_pServiceProvider;
static CComPtr<IVSMDPropertyBrowser> s_pBrowser;
static CComBSTR s_bstrAlternateRegistryRoot;

// IVsPackage
public:
   	STDMETHOD(SetSite)(IServiceProvider *pSP);
	{
		RETURN_ON_NULL(pSP);
		s_pServiceProvider = pSP;

		CComPtr<IVsShell> pShell;
		HRESULT hr = pSP->QueryService(SID_SVsShell, IID_IVsShell, (void**)&pShell);
		RETURN_ON_NULL2(pShell, hr);

		CComVariant var;
		hr = pShell->GetProperty(VSSPROPID_VirtualRegistryRoot, &var);
		if(SUCCEEDED(hr) && var.vt == VT_BSTR)
			s_bstrAlternateRegistryRoot = var.bstrVal;

		pSP->QueryService( SID_SVSMDPropertyBrowser, __uuidof(IVSMDPropertyBrowser), (LPVOID *)&s_pBrowser );
		return hr;
	}

	STDMETHOD(QueryClose)(BOOL *pCanClose);
	{
		if (pCanClose != NULL)
		{
			*pCanClose = TRUE;
			return S_OK;
		}
		RETURN_INVALID();
	}
	
	STDMETHOD(Close)();
	{
	    m_pServiceProvider = NULL;
		s_pBrowser = NULL;
		s_bstrAlternateRegistryRoot = NULL;
	}

	STDMETHOD(GetAutomationObject)(LPCOLESTR pszPropName, IDispatch **ppDisp);
		{ return E_NOTIMPL; }
	STDMETHOD(GetPropertyPage)(REFGUID rguidPage, VSPROPSHEETPAGE *ppage);
		{ return E_NOTIMPL; }
	STDMETHOD(ResetDefaults)(PKGRESETFLAGS dwFlags);
		{ return E_NOTIMPL; }
	STDMETHOD(CreateTool)(REFGUID rguidPersistenceSlot);
		{ return E_NOTIMPL; }

// IServiceProvider
	STDMETHOD(QueryService)(THIS_ REFGUID rsid, REFIID iid, void ** ppvObj);
};
#endif

// This class does a loadlibrary on the active directory services dll, and 
// exposes methods to call its functions.
//---------------------------------------------------------------------------
#pragma once

interface IADsContainer;

// ADSI Function prototypes
typedef HRESULT (WINAPI *fnpADsGetObject)(LPCWSTR, REFIID, void**);
typedef HRESULT (WINAPI *fnpADsBuildEnumerator)(IADsContainer*, IEnumVARIANT**);
typedef HRESULT (WINAPI *fnpADsEnumerateNext)(IEnumVARIANT*, ULONG , VARIANT*, ULONG*);
typedef HRESULT (WINAPI *fnpADsFreeEnumerator)(IEnumVARIANT*);
typedef HRESULT (WINAPI *fnpADsGetLastError)(LPDWORD, LPWSTR, DWORD, LPWSTR, DWORD );

class CAdsiWrapper
{
public:
    CAdsiWrapper();
    ~CAdsiWrapper();
    BOOL Init();

    HRESULT ADsGetObject(LPCWSTR pszADSRootPath, REFIID riid, void** ppADsRoot);
    HRESULT ADsBuildEnumerator(IADsContainer *pADsContainer, IEnumVARIANT **ppEnumVariant);
    HRESULT ADsEnumerateNext(IEnumVARIANT *pEnumVariant, ULONG cElements, VARIANT* pvar, ULONG* pcElementsFetched);
    HRESULT ADsFreeEnumerator(IEnumVARIANT *pEnumVariant);
    HRESULT ADsGetLastError(LPDWORD lpError, LPWSTR lpErrorBuf, DWORD dwErrorBufLen, LPWSTR lpNameBuf, DWORD dwNameBufLen);
protected:
    HINSTANCE             m_hAdsiLib;
    fnpADsGetObject       m_pADsGetObject;
    fnpADsBuildEnumerator m_pADsBuildEnumerator;
    fnpADsEnumerateNext   m_pADsEnumerateNext;
    fnpADsFreeEnumerator  m_pADsFreeEnumerator;
    fnpADsGetLastError    m_pADsGetLastError;
};

#endif //__VCPROJECTENGINE_H_
