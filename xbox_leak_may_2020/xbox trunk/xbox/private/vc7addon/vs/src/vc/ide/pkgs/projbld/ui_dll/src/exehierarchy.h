//---------------------------------------------------------------------------
// Microsoft VC++
//
// Microsoft Confidential
// Copyright 1994 - 1997 Microsoft Corporation. All Rights Reserved.
//
// CExeHierarchy.h: Definition of the CExeHierarchy class
//
// A separate interface object to CVC
//---------------------------------------------------------------------------

#if !defined(AFX_CExeHierarchy_H__BDD4A1A3_7A1F_11D0_AC13_00A0C91E29D5__INCLUDED_)
#define AFX_CExeHierarchy_H__BDD4A1A3_7A1F_11D0_AC13_00A0C91E29D5__INCLUDED_

#include "vssolutn.h"
#include "bldpkg.h"
#include "fpstfmt.h"

//
// When retrieving a shell interface from CExeHierarchy, perform a QI to get the desired interface.
// We can't perform a simple cast from CExeHierarchy to any shell interface because that prevents 
// CExeHierarchy from being aggregated correctly.
//
// Usage:	IVsHierarchy* pHier = VCQI_cast<IVsHierarchy>(this);
//			IVsProject*   pProj = VCQI_cast<IVsProject>(this);
//			etc.
//
// forward declaration
extern BOOL g_bCommandLineDBG;
class CExeHierarchy;

template <typename T> CComPtr<T> VCQI_cast( const CExeHierarchy* pCExeHierarchy) 
{ 
	CComPtr<T>	pT;
	((CExeHierarchy*)pCExeHierarchy)->QueryInterface( __uuidof(T), (void**)&pT );
	ASSERT( pT );
	return pT;
}

class CExeConfig :
	public IVCCfg,
	public ISpecifyPropertyPages,
	public IVsDebuggableProjectCfg,	// derives from IVsProjectCfg -> IVsCfg
	public IVsPersistSolutionOpts,
	public CComObjectRoot
{
public:
	CExeConfig(void) : m_pArchy(NULL) {}
	~CExeConfig(void){}
	static HRESULT CreateInstance(IVsDebuggableProjectCfg **ppGenCfg, LPCOLESTR pszFileName, LPCOLESTR pszArgs, 
		CExeHierarchy* pHierarchy);
	HRESULT Initialize(LPCOLESTR pszFileName, LPCOLESTR pszArgs, CExeHierarchy* pHierarchy);
	
BEGIN_COM_MAP(CExeConfig)
	COM_INTERFACE_ENTRY(IVsCfg)						// Config interfaces
	COM_INTERFACE_ENTRY(IVsProjectCfg)	
	COM_INTERFACE_ENTRY(IVsDebuggableProjectCfg)	
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)		
	COM_INTERFACE_ENTRY(IVsPersistSolutionOpts)
	COM_INTERFACE_ENTRY(IVCCfg)
END_COM_MAP()

private:
	CComQIPtr<IVCDebugSettingsImpl> m_pDbgSettings;
	CExeHierarchy* m_pArchy;

// IVsCfg
public:
	STDMETHOD(get_DisplayName)( /* [out] */ BSTR *pbstrDisplayName);
	STDMETHOD(get_IsDebugOnly)( /* [out] */ BOOL *pfIsDebugOnly);
	STDMETHOD(get_IsReleaseOnly)( /* [out] */ BOOL *pfIsRetailOnly);

// IVsProjectCfg
public:
	STDMETHOD(get_ProjectCfgProvider)( /* [out] */ IVsProjectCfgProvider **ppIVsProjectCfgProvider);
	STDMETHOD(get_CanonicalName)( /* [out] */ BSTR *pbstrCanonicalName);
	STDMETHOD(get_IsRetailOnly)( /* [out] */ BOOL *pfIsRetailOnly);
	STDMETHOD(get_Platform)( /* [out] */ GUID *pguidPlatform)
		{return E_NOTIMPL;	/* CExeConfig::get_Platform */ }
	STDMETHOD(EnumOutputs)( /* [out] */ IVsEnumOutputs **ppIVsEnumOutputs)
		{return E_NOTIMPL; /* CExeConfig::EnumOutputs */ }
	STDMETHOD(OpenOutput)( /* [in] */ LPCOLESTR szOutputCanonicalName, /* [out] */ IVsOutput **ppIVsOutput)
		{return E_NOTIMPL; /* CExeConfig::OpenOutput */ }
	STDMETHOD(get_BuildableProjectCfg)( /* [out] */ IVsBuildableProjectCfg **ppIVsBuildableProjectCfg)
		{return E_NOTIMPL; /* CExeConfig::get_BuildableProjectCfg */ }
	STDMETHOD(get_IsPackaged)( /* [out] */ BOOL *pfIsPackaged);
	STDMETHOD(get_IsSpecifyingOutputSupported)( /* [out] */ BOOL *pfIsSpecifyingOutputSupported);
	STDMETHOD(get_TargetCodePage)( /* [out] */ UINT *puiTargetCodePage);
	STDMETHOD(get_UpdateSequenceNumber)(/* [out] */ ULARGE_INTEGER *puliUSN)
		{return E_NOTIMPL; /* CExeConfig::get_UpdateSequenceNumber */ }
	STDMETHOD(get_RootURL)(/* [out] */ BSTR *pbstrRootURL)
		{return E_NOTIMPL; /* CExeConfig::get_RootURL */ }

// IVsDebuggableProjectCfg
public:
    STDMETHOD(DebugLaunch)(/* [in] */ VSDBGLAUNCHFLAGS grfLaunch);
    STDMETHOD(QueryDebugLaunch)(/* [in] */ VSDBGLAUNCHFLAGS grfLaunch, /* [out] */ BOOL *pfCanLaunch);

// ISpecifyPropertyPages
public:
    STDMETHOD(GetPages)( /* [out] */ CAUUID *pPages );

// IVCCfg
public:
	STDMETHOD(get_Tool)(/* [in] */ VARIANT toolIndex, /* [out] */ IUnknown **ppConfig)
		{return E_NOTIMPL; /* CExeConfig::get_Tool */ }
	STDMETHOD(get_Object)(/* [out] */ IDispatch **ppDebugSettings);
	
// IVsPersistSolutionOpts
public:
    STDMETHOD(SaveUserOptions)(IVsSolutionPersistence *pPersistence)
		{return E_NOTIMPL; /* CExeConfig::SaveUserOptions */ }
    STDMETHOD(LoadUserOptions)(IVsSolutionPersistence *pPersistence, VSLOADUSEROPTS grfLoadOpts)
		{return E_NOTIMPL; /* CExeConfig::LoadUserOptions */ }

    STDMETHOD(WriteUserOptions)(IStream *pOptionsStream, LPCOLESTR pszKey);
    STDMETHOD(ReadUserOptions)(IStream *pOptionsStream, LPCOLESTR pszKey);
};

class CExeHierarchy : 
	public IVsUIHierarchy,			// derives from IVsHierarchy
	public IVsHierarchyDeleteHandler,
	public IVsProject,				
	public IVsProjectCfgProvider,	// derives drom IVsCfgProvider
	public IOleCommandTarget,	
	public IVsPersistHierarchyItem,
	public IPersistFileFormat,		// derives from IPersist
	public IVsPersistSolutionOpts,
	public IVsPerPropertyBrowsing,
	public IDispatchImpl<_Project, &IID__Project, &LIBID_DTE, 7, 0>,
	public CComObjectRoot
{
private:
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IVsDebuggableProjectCfg> m_pConfig;
	CComBSTR m_bstrName;
	CComBSTR m_bstrFullPath;
	// un-ref-counted ptr to parent hierarchy
	IUnknown *m_pParentHierarchy;
	VSITEMID m_dwParentHierarchyItemID;
	
public:
	CExeHierarchy(void){}
	~CExeHierarchy(void){}
	HRESULT Initialize(LPCOLESTR pszFileName, LPCOLESTR pszArgs );
	
DECLARE_NOT_AGGREGATABLE(CExeHierarchy)

BEGIN_COM_MAP(CExeHierarchy)
	COM_INTERFACE_ENTRY(IVsProject)					// Project interfaces
	COM_INTERFACE_ENTRY(IVsHierarchy)			
	COM_INTERFACE_ENTRY(IVsUIHierarchy)			
	COM_INTERFACE_ENTRY(IVsHierarchyDeleteHandler)
	COM_INTERFACE_ENTRY(IOleCommandTarget)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(_Project)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)

	COM_INTERFACE_ENTRY(IVsProjectCfgProvider)		// Config provider interfaces
	COM_INTERFACE_ENTRY(IVsCfgProvider)				

	COM_INTERFACE_ENTRY(IPersistFileFormat)			// Persistance interfaces
	COM_INTERFACE_ENTRY(IVsPersistHierarchyItem)			
	COM_INTERFACE_ENTRY(IVsPersistSolutionOpts)
END_COM_MAP()


//  IVsProject
public:
	STDMETHOD (IsDocumentInProject) ( LPCOLESTR pszMkDocument, BOOL *pfFound, VSDOCUMENTPRIORITY *pdwPriority, VSITEMID *pvsid)
		{return E_NOTIMPL; /* CExeHierarchy::IsDocumentInProject */ }
	STDMETHOD (GetMkDocument) (VSITEMID vsid, BSTR *pbstrMkDocument) {return E_NOTIMPL; /* CExeHierarchy::GetMkDocument */ }
	STDMETHOD (OpenItem) (VSITEMID vsid, REFGUID guidDocViewType, IUnknown *punkDocDataExisting, IVsWindowFrame **ppWindowFrame)
		{return E_NOTIMPL; /* CExeHierarchy::OpenItem */ }
	STDMETHOD (GetItemContext) (VSITEMID vsid, IServiceProvider **ppSP) {return E_NOTIMPL; /* CExeHierarchy::GetItemContext */ }
	STDMETHOD (GenerateUniqueItemName) ( VSITEMID itemidLoc, LPCOLESTR pszExt, LPCOLESTR pszSuggestedRoot, BSTR * pbstrItemName)
		{return E_NOTIMPL; /* CExeHierarchy::GenerateUniqueItemName */ }
	STDMETHOD (AddItem)( /* [in] */ VSITEMID itemidLoc, /* [in] */ VSADDITEMOPERATION dwAddItemOperation, /* [in] */ LPCOLESTR pszItemName, /* [in] */ DWORD cFilesToOpen, /* [in, size_is(cFilesToOpen)] */ LPCOLESTR rgpszFilesToOpen[], /* [in] */ HWND hwndDlg, /* [out, retval] */ VSADDRESULT * pResult)
		{return E_NOTIMPL; /* CExeHierarchy::AddItem */ }
	STDMETHOD (Close)(void) {return S_OK;}

// Project
public:
    /* _Project methods */
    STDMETHOD(get_Name)(BSTR FAR* pbstrName);
    STDMETHOD(put_Name)(BSTR bstrName) { return E_NOTIMPL; }
    STDMETHOD(get_FileName)(BSTR FAR* pbstrName);
    STDMETHOD(get_IsDirty)(VARIANT_BOOL FAR* lpfReturn);
    STDMETHOD(put_IsDirty)(VARIANT_BOOL Dirty){ return E_NOTIMPL; }
    STDMETHOD(get_Collection)(Projects FAR* FAR* lppaReturn){ return E_NOTIMPL; }
    STDMETHOD(SaveAs)(BSTR FileName){ return E_NOTIMPL; }
    STDMETHOD(get_DTE)(DTE FAR* FAR* lppaReturn);
    STDMETHOD(get_Kind)(BSTR FAR* lpbstrFileName);
    STDMETHOD(get_ProjectItems)(ProjectItems FAR* FAR* lppcReturn){ return E_NOTIMPL; }
    STDMETHOD(get_Properties)(Properties FAR* FAR* ppObject){ return E_NOTIMPL; }
    STDMETHOD(get_UniqueName)(BSTR FAR* pbstrName){ return get_FileName(pbstrName); }
    STDMETHOD(get_Object)(IDispatch **ppProjectModel){ return E_NOTIMPL; }
    STDMETHOD(get_Extender)(BSTR bstrExtenderName, IDispatch **ppExtender){ return E_NOTIMPL; }
    STDMETHOD(get_ExtenderNames)(VARIANT *pvarExtenderNames){ return E_NOTIMPL; }
    STDMETHOD(get_ExtenderCATID)(BSTR *pbstrRetval){ return E_NOTIMPL; }
    STDMETHOD(get_FullName)(BSTR *lpbstrReturn);
    STDMETHOD(get_Saved)(VARIANT_BOOL *lpfReturn);
    STDMETHOD(put_Saved)(VARIANT_BOOL Dirty) { return S_OK; }
    STDMETHOD(get_SourceControl)(SourceControl **) { return E_NOTIMPL; /* CExeHierarchy::get_SourceControl */ }
    STDMETHOD(get_ConfigurationManager)(ConfigurationManager **ppConfigurationManager);
    STDMETHOD(get_Globals)(Globals ** /*ppGlobals*/) { return E_NOTIMPL;	/* CExeHierarchy::get_Globals*/ }
    STDMETHOD(Save)(BSTR /*FileName*/) { return E_NOTIMPL; /* CExeHierarchy::Save */ }
    STDMETHOD(get_ParentProjectItem)(ProjectItem ** ppProjectItem);
    STDMETHOD(get_CodeModel)(CodeModel ** ppCodeModel) { return E_NOTIMPL; }
    STDMETHOD(Delete)(void) { return E_NOTIMPL; }

    STDMETHOD(Reserved1)(BSTR ProgID, VARIANT FileName) { return E_NOTIMPL; }
    STDMETHOD(Reserved2)(BSTR Section, BSTR Key, BSTR Value) { return E_NOTIMPL; }
    STDMETHOD(Reserved3)(BSTR Section, BSTR Key, BSTR FAR* lpbstrValue) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved4)(BSTR FAR* lpbstrHelpFile) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved4)(BSTR bstrHelpFile) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved5)(long FAR* lpdwContextID) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved5)(long dwContextID) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved6)(BSTR FAR* lpbstrDescription) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved6)(BSTR bstrDescription) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved7)(IDispatch * FAR* lppReferences) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved8)(IDispatch * FAR* lppaReturn) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved9)(long FAR* lpStartMode) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved9)(long StartMode) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved10)(BSTR FAR* lpbstrCompatibleOleServer) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved10)(BSTR bstrCompatibleOleServer) { return E_NOTIMPL; }
    STDMETHOD(Reserved11)() { return E_NOTIMPL; }
    STDMETHOD(get_Reserved12)(long FAR* lpkind) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved12)(long Type) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved13)(long FAR* lpIconState) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved13)(long IconState) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved17)(VARIANT_BOOL FAR* lpfReturn) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved15)(IDispatch * FAR* lppcReturn) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved16)(BSTR FAR* lpbstrBldFName) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved16)(BSTR bstrBldFName) { return E_NOTIMPL; }

// IVsHierarchy
public:
	STDMETHOD(SetSite)( /* [in] */ IServiceProvider *pSP);
	STDMETHOD(GetSite)( /* [out] */ IServiceProvider **ppSP); 	
	STDMETHOD(QueryClose)( /* [out] */ BOOL *pfCanClose);
	STDMETHOD(GetGuidProperty)( /* [in] */ VSITEMID itemid, /* [in] */ VSHPROPID propid, /* [out] */ GUID *pguid);
	STDMETHOD(SetGuidProperty)( /* [in] */ VSITEMID itemid, /* [in] */ VSHPROPID propid, /* [in] */ REFGUID guid);
	STDMETHOD(GetProperty)( VSITEMID itemid, /* [in] */ VSHPROPID propid, /* [out] */ VARIANT *pvar);
	STDMETHOD(SetProperty)( VSITEMID itemid, /* [in] */ VSHPROPID propid, /* [in] */ VARIANT var);
	STDMETHOD(GetNestedHierarchy)( /* [in] */ VSITEMID itemid, /* [in] */ REFIID riid, /* [out] */ void **ppHierarchyNested, /* [out] */ VSITEMID *pitemidNested)
		{return E_NOTIMPL; /* CExeHierarchy::GetNestedHierarchy */ }
	STDMETHOD(GetCanonicalName)( /* [in] */ VSITEMID itemid, /* [out] */ BSTR *ppszName);
	STDMETHOD(ParseCanonicalName)( /* [in] */ LPCOLESTR pszName, /* [out] */ VSITEMID *pitemid);
	STDMETHOD(AdviseHierarchyEvents)( /* [in] */ IVsHierarchyEvents *pEventSink, /* [out] */ VSCOOKIE *pdwCookie);
	STDMETHOD(UnadviseHierarchyEvents)( /* [in] */ VSCOOKIE dwCookie) {return S_OK;}
	STDMETHOD(Unused0)( void ){return E_NOTIMPL;}
	STDMETHOD(Unused1)( void ){return E_NOTIMPL;}
	STDMETHOD(Unused2)( void ){return E_NOTIMPL;}
	STDMETHOD(Unused3)( void ){return E_NOTIMPL;}
	STDMETHOD(Unused4)( void ){return E_NOTIMPL;}

// IVsHierarchyDeleteHandler
public:
	STDMETHOD(QueryDeleteItem)(	/* [in] */ VSDELETEITEMOPERATION dwDelItemOp, /* [in] */ VSITEMID itemid, /* [retval][out] */ BOOL __RPC_FAR *pfCanDelete);
	STDMETHOD(DeleteItem)( /* [in] */ VSDELETEITEMOPERATION dwDelItemOp, /* [in] */ VSITEMID itemid);

// IVsUIHierarchy
public:
    STDMETHOD (QueryStatusCommand)(VSITEMID itemid, const GUID* pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[],
		OLECMDTEXT *pCmdText);
	STDMETHOD(ExecCommand)(VSITEMID itemid, const GUID* pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT* pvaIn,
		VARIANT* pvaOut);

// IOleCommandTarget
public:
	STDMETHOD(QueryStatus)(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText);
	STDMETHOD(Exec)(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut);
		   
// IVsCfgProvider
public:
	STDMETHOD(GetCfgs)(ULONG celt, IVsCfg *rgpcfg[], ULONG *pcActual, VSCFGFLAGS *prgfFlags); 

// IVsProjectCfgProvider
public:
	STDMETHOD(OpenProjectCfg)(/*[in]*/ LPCOLESTR szProjectCfgCanonicalName, IVsProjectCfg **ppIVsProjectCfg)
		{return E_NOTIMPL; /* CExeHierarchy::OpenProjectCfg */ }
	STDMETHOD(get_UsesIndependentConfigurations)(/*[out]*/ BOOL *pfUsesIndependentConfigurations);

// IPersist
public:
	STDMETHOD(GetClassID)( /* [out] */ CLSID *pClassID);

// IPersistFileFormat
public:
	STDMETHOD(IsDirty)(BOOL __RPC_FAR *pfIsDirty);
	STDMETHOD(InitNew)( DWORD nFormatIndex) { return E_NOTIMPL; }	// CExeHierarchy::InitNew
	STDMETHOD(Load)( LPCOLESTR pszFilename, DWORD grfMode, BOOL fReadOnly) { return S_OK; }
	STDMETHOD(Save)( LPCOLESTR pszFilename, BOOL fRemember, DWORD nFormatIndex) { return S_OK; }
	STDMETHOD(SaveCompleted)( LPCOLESTR pszFileName) {return S_OK; }
	STDMETHOD(GetCurFile)( LPOLESTR __RPC_FAR *ppszFilename, DWORD __RPC_FAR *pnFormatIndex);
	STDMETHOD(GetFormatList)( LPOLESTR __RPC_FAR *ppszFormatList ) {return E_NOTIMPL; }	// CExeHierarchy::GetFormatList

// IVsPersistHierarchyItem
public:
    STDMETHOD(IsItemDirty)( VSITEMID vsid, IUnknown __RPC_FAR *punkDocData, BOOL __RPC_FAR *pfDirty)
		{return E_NOTIMPL; /* CExeHierarchy::IsItemDirty */ }
    STDMETHOD(SaveItem)( VSSAVEFLAGS dwSave, LPCOLESTR lpstrMkDocument, VSITEMID vsid, IUnknown __RPC_FAR *punkDocData, BOOL __RPC_FAR *pfCanceled)
		{return E_NOTIMPL; /* CExeHierarchy::SaveItem */ }

// IVsPersistSolutionOpts
public:
    STDMETHOD(SaveUserOptions)(IVsSolutionPersistence *pPersistence) {return E_NOTIMPL; /* CExeHierarchy::SaveUserOptions */ }
    STDMETHOD(LoadUserOptions)(IVsSolutionPersistence *pPersistence, VSLOADUSEROPTS grfLoadOpts) {return E_NOTIMPL; /* CExeHierarchy::LoadUserOptions */ }
    STDMETHOD(WriteUserOptions)(IStream *pOptionsStream, LPCOLESTR pszKey);
    STDMETHOD(ReadUserOptions)(IStream *pOptionsStream, LPCOLESTR pszKey);
    
// IVsPerPropertyBrowsing
public:
	STDMETHOD(HideProperty)( DISPID dispid, BOOL *pfHide);
	STDMETHOD(DisplayChildProperties)( DISPID dispid, BOOL *pfDisplay) { return E_NOTIMPL; /* CExeHierarchy::DisplayChildProperties */ }
	STDMETHOD(HasDefaultValue)( DISPID dispid, BOOL *pfDefault) { return E_NOTIMPL; /* CExeHierarchy::HasDefaultValue */ }
	STDMETHOD(IsPropertyReadOnly)( DISPID dispid, BOOL *fReadOnly);
	STDMETHOD(GetLocalizedPropertyInfo)( DISPID dispid, LCID localeID, BSTR *pbstrName, BSTR *pbstrDesc);
	STDMETHOD(GetClassName)(BSTR* pbstrClassName);
    STDMETHOD(CanResetPropertyValue)(DISPID , BOOL* ) {return E_NOTIMPL;}	
    STDMETHOD(ResetPropertyValue)(DISPID ) {return E_NOTIMPL;}
};

class CCrashDumpHierarchy : public CExeHierarchy
{
// IPersist
public:
	STDMETHOD(GetClassID)( /* [out] */ CLSID *pClassID);
};

#endif // !defined(AFX_CExeHierarchy_H__BDD4A1A3_7A1F_11D0_AC13_00A0C91E29D5__INCLUDED_)
