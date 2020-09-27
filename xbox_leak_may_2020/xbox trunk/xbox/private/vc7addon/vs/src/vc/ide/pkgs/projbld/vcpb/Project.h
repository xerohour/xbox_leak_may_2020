// Project.h: Definition of the CProject class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECT_H__625C6496_2C7E_11D3_87BF_A0494CC10000__INCLUDED_)
#define AFX_PROJECT_H__625C6496_2C7E_11D3_87BF_A0494CC10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comlist.h"
#include "FileRegistry.h"
#include "globals.h"
#include "webref.h"

#define closeFlagOpenClean		0x00
#define closeFlagMaybeSCCClose	0x01
#define closeFlagClosed			0x02
#define closeFlagBySCC			(closeFlagMaybeSCCClose | closeFlagClosed)

/////////////////////////////////////////////////////////////////////////////
// CProject

class CProject : 
	public IDispatchImpl<VCProject, &IID_VCProject, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public IVCProjectImpl,
	public IVCBuildableItem,
	public IVCExternalCookie,
	public IVsPerPropertyBrowsing,
	public IPerPropertyBrowsing,
	public IVsAddWebReference,			// KPERRY(21:3:01) uncomment this
	public CComObjectRoot
{
public:
	CProject();
	~CProject();
	static HRESULT CreateInstance(VCProject **ppProject);

BEGIN_COM_MAP(CProject)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(VCProject)
	COM_INTERFACE_ENTRY(IVCProjectImpl)
	COM_INTERFACE_ENTRY(IVCBuildableItem)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVCExternalCookie)
	COM_INTERFACE_ENTRY(IVCCollectionProvider)
	COM_INTERFACE_ENTRY(VCProjectItem)
	COM_INTERFACE_ENTRY(IVsAddWebReference) 			// KPERRY(21:3:01) uncomment this
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CProject) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

DECLARE_NO_REGISTRY()

// data
protected:
	CComBSTR	m_bstrName;			// the project name. This is independent of the filename
	CComBSTR	m_bstrFileName;		// just the file name, and no path
	CComBSTR	m_bstrFullDir;		// just the full-path's directory. Eg., "c:\projects\stuff\". Assume final slash is there.
	CComDynamicListTyped<VCFile>	m_rgFiles;		// collection of files for this project
	CComDynamicListTyped<VCPlatform>	m_rgPlatforms;	// collection of platforms for this project
	CComDynamicListTyped<VCConfiguration>	m_rgConfigs;	// collection of configs for this project
	CComDynamicListTyped<VCFilter>	m_rgFilters;		// collection of Filters for this project
	CComDynamicListTyped<VCProjectItem> m_rgTopLevelItems;	// collection of top level files & filters for this project
	// data for support of shell's "globals" object
	// RAW pointer
	IVCGlobals 	   *m_pGlobals;

	CComBSTR		m_bstrKeyword;
	VARIANT_BOOL	m_bDirty;
	// scc integration data
	CComBSTR		m_bstrSccProjectName;
	CComBSTR		m_bstrSccAuxPath;
	CComBSTR		m_bstrSccLocalPath;
	CComBSTR		m_bstrSccProvider;
	// what type of file is the project saved as (ANSI, UTF-8, Unicode)
	enumFileFormat	m_eFileFormat;
	CComBSTR		m_bstrFileEncoding;
	bool			m_bItemsCollectionDirty;
	// IVCBuildableItem data/helpers
	BldFileRegHandle	m_frh;
	CBldFileRegistry	m_ProjectRegistry;			// the project registry
	bool			m_bAssignedActions;
	void 		   *m_pExternalCookie;
	// owner project name, for VSEF nested project support
	CComBSTR		m_bstrOwnerKey;
	// Unique ID for this project
	CComBSTR		m_bstrGuid;
	// a flag for handling when SCC does something unspeakable to us like reload us on checkout
	DWORD			m_wCloseFlag;

// IVSAddWebReference
public:
	STDMETHOD(AddWebReferenceEx)(LPCOLESTR bstrUrl);

// VCProjectItem
public:
	STDMETHOD(MatchName)(BSTR NameToMatch, VARIANT_BOOL FullOnly, VARIANT_BOOL *IsMatch);
	STDMETHOD(get_Project)(IDispatch * *Val);
	STDMETHOD(get_Parent)(IDispatch * *Val);
	STDMETHOD(get_Kind)(BSTR* kind);
	STDMETHOD(get_ItemName)(BSTR *Val) { return get_Name(Val); }

// VCProject
public:
	STDMETHOD(get_Platforms)(IDispatch **pVal);
	STDMETHOD(get_Configurations)(/*[out, retval]*/ IDispatch * *pVal);
	STDMETHOD(get_Items)(/*[out, retval]*/ IDispatch * *pVal);
	STDMETHOD(get_Filters)(/*[out, retval]*/ IDispatch * *pVal);
	STDMETHOD(get_Files)(/*[out, retval]*/ IDispatch * *pVal);
	STDMETHOD(get_ProjectDirectory)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_ProjectFile)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ProjectFile)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Name)(BSTR *Val);
	STDMETHOD(put_Name)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_FileFormat)( enumFileFormat *pType );
	STDMETHOD(put_FileFormat)( enumFileFormat type );
	STDMETHOD(get_FileEncoding)( BSTR *pbstrEncoding );
	STDMETHOD(put_FileEncoding)( BSTR bstrEncoding );
	STDMETHOD(get_IsDirty)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(get_Keyword)( BSTR* bstrKeyword );
	STDMETHOD(put_Keyword)( BSTR bstrKeyword );
	STDMETHOD(get_VCProjectEngine)(IDispatch** projEngine);

	STDMETHOD(get_ProjectGUID)( BSTR* bstrGUID );
	STDMETHOD(put_ProjectGUID)( BSTR bstrGUID );
	STDMETHOD(get_OwnerKey)( BSTR* bstrName );
	STDMETHOD(put_OwnerKey)( BSTR bstrName );
	STDMETHOD(get_SccProjectName)( BSTR* bstrName );
	STDMETHOD(put_SccProjectName)( BSTR bstrName );
	STDMETHOD(get_SccAuxPath)( BSTR* bstrName );
	STDMETHOD(put_SccAuxPath)( BSTR bstrName );
	STDMETHOD(get_SccLocalPath)( BSTR* bstrName );
	STDMETHOD(put_SccLocalPath)( BSTR bstrName );
	STDMETHOD(get_SccProvider)( BSTR* bstrName );
	STDMETHOD(put_SccProvider)( BSTR bstrName );

	STDMETHOD(Save)();
	STDMETHOD(SaveProjectOptions)(/*[in]*/ IUnknown *pStreamUnk);
	STDMETHOD(LoadProjectOptions)(/*[in]*/ IUnknown *pStreamUnk);

	STDMETHOD(AddFilter)(BSTR bstrFilterName, /*[out, retval]*/ IDispatch * *ppDisp);
	STDMETHOD(CanAddFilter)(BSTR bstrFilter, VARIANT_BOOL* pbCanAdd);
	STDMETHOD(RemoveFilter)(/*[in]*/  IDispatch *pItem);

	STDMETHOD(AddWebReference)(BSTR bstrUrl, IDispatch** ppFile);
	STDMETHOD(AddFile)(BSTR bstrPath, /*[out, retval]*/ IDispatch * *ppDisp);
	STDMETHOD(CanAddFile)(BSTR bstrFile, VARIANT_BOOL* pbCanAdd);
	STDMETHOD(RemoveFile)(/*[in]*/	IDispatch *pItem);

	STDMETHOD(AddConfiguration)(BSTR bstrConfigurationName);
	STDMETHOD(RemoveConfiguration)(IDispatch *pDispConfig);

	STDMETHOD(AddPlatform)(BSTR bstrPlatformName);
	STDMETHOD(RemovePlatform)(IDispatch* pPlatform);

// IVCProjectImpl
public:
	STDMETHOD(Close)();
	STDMETHOD(AddNewFile)(IDispatch * pFile, LPCOLESTR szRelativePath, VARIANT_BOOL bFireEvents);
	STDMETHOD(CanAddFile2)(BSTR bstrFile);
	STDMETHOD(CanAddFilter2)(BSTR bstrFilter, VARIANT_BOOL bSetErrorInfo);
	STDMETHOD(RemoveExistingFile)(IDispatch* pFile);
	STDMETHOD(SaveObject)(IStream *xml, IVCPropertyContainer *pPropContainer, long nIndent);
	STDMETHOD(FileNameValid)(LPCOLESTR szFileName, VARIANT_BOOL bSetErrorInfo);
	STDMETHOD(put_IsDirty)(VARIANT_BOOL boolDirty);
	STDMETHOD(put_IsConverted)(VARIANT_BOOL boolDirty);
	STDMETHOD(get_CanDirty)(VARIANT_BOOL* pbCanDirty);
	STDMETHOD(DoSave)(LPCOLESTR szFileName);
	STDMETHOD(GetGlobals)(IVCGlobals **ppGlobals);
	STDMETHOD(AddChild)(IDispatch* pChild);
	STDMETHOD(RemoveChild)(IDispatch* pChild);
	STDMETHOD(AddConfigurationInternal)(LPCOLESTR szConfigName);
	STDMETHOD(AddPlatformInternal)(LPCOLESTR szPlatformName);
	STDMETHOD(RemovePlatformInternal)(IDispatch* pPlatform);
	STDMETHOD(IsFileProbablyInProject)(LPCOLESTR szFile, VARIANT_BOOL* pb);
	STDMETHOD(AddWSDL)(BSTR bstrUrl, VCFilter* pFilter, IDispatch** ppFile);
	STDMETHOD(UpdateWSDL)(BSTR bstrUrl, VARIANT_BOOL bExists, BSTR *bstrWSDL);
	STDMETHOD(MakeOutputFileUnique)(VCFile* pFile);

protected:
	// for own use
	HRESULT FindFilter(BSTR bstrFileName, VCFilter **ppFilter);
	HRESULT FindSubFilter(BSTR bstrFileName, VCFilter *pFilterStart, VCFilter **ppFilter);
	HRESULT GetConfigFromVariant(VARIANT varConfiguration, IDispatch **ppDispVal);
	HRESULT AddPlatformConfig(IDispatch *pDispPlatform, LPCOLESTR bstrConfigName, BOOL bSearchExisting);
	HRESULT SetDirty(VARIANT_BOOL bDirty) { return put_IsDirty(bDirty); }
	HRESULT FindFile(BSTR bstrPath, IDispatch** ppDisp, BOOL bSetErrorInfo);
	HRESULT FindExistingFilter(BSTR bstrFilter, CStringW& strCleanedUpName, IDispatch** ppDisp, BOOL bSetErrorInfo);
	HRESULT WrapCanDirty();
	HRESULT DoRemoveExistingFile(IDispatch* pFile, BOOL bRemoveFromFilter);
	void BuildTopLevelItemsCollection();
	HRESULT RemoveConfigurationInternal(VCConfiguration *pConfig);
	HRESULT CheckCanAddFile(BSTR bstrFile, BOOL bSetErrorInfo);
	HRESULT CheckCanAddFilter(BSTR bstrFilter, CStringW& strCleanedUpName, BOOL bSetErrorInfo);

public:
	// helpers
	static HRESULT QueryCanAddFile(VCProject* pProj, VCFilter* pFilter, LPCOLESTR szFile, BOOL bSetErrorInfo);
	static BOOL ForwardTrackEvents(VCFilter* pFilterParent);
	static HRESULT InformFileAdded(VCProject* pProject, VCFile* pFile, BSTR bstrFilePath);
	HRESULT QueryCanRemoveFile(VCFile* pFile);
	HRESULT InformFileRemoved(VCFile* pFile, BSTR bstrFilePath);

protected:
	static BOOL DetermineFileParentFilter(VCFile* pFile, CComQIPtr<VCFilter>& rspFilterParent);

// IVCCollectionProvider
public:
	STDMETHOD(UpdateItemsCollection)(DWORD dwCookie);

// IVCBuildableItem
public:
	STDMETHOD(get_ItemFileName)(BSTR *pVal);
	STDMETHOD(get_ItemFullPath)(BSTR* pbstrFullPath);
	STDMETHOD(get_ActionList)(IVCBuildActionList** ppActions);
	STDMETHOD(get_FileRegHandle)(void** pfrh);
	STDMETHOD(AssignActions)(VARIANT_BOOL bOnOpen);
	STDMETHOD(UnAssignActions)(VARIANT_BOOL bOnClose);
	STDMETHOD(get_ContentList)(IEnumVARIANT** ppContentList);
	STDMETHOD(get_Registry)(void** pFileRegistry);
	STDMETHOD(get_ProjectConfiguration)(VCConfiguration** ppProjCfg);
	STDMETHOD(get_ProjectInternal)(VCProject** ppProject);
	STDMETHOD(get_ExistingBuildEngine)(IVCBuildEngine** ppBldEngine);
	STDMETHOD(get_PersistPath)(BSTR* pbstrPersistPath);
	STDMETHOD(RefreshActionOutputs)(long nPropID, IVCBuildErrorContext* pEC);
	STDMETHOD(HandlePossibleCommandLineOptionsChange)(IVCBuildErrorContext* pEC);
	STDMETHOD(ClearDirtyCommandLineOptionsFlag)();

//	IVCExternalCookie
	STDMETHOD(get_ExternalCookie)(void** ppVal) { *ppVal =	m_pExternalCookie; return S_OK; }
	STDMETHOD(put_ExternalCookie)(void* pVal) { m_pExternalCookie = pVal; return S_OK; }

// IPerPropertyBrowsing
public:
	STDMETHOD(GetDisplayString)( DISPID dispID, BSTR *pBstr ) { return E_NOTIMPL; }
	STDMETHOD(MapPropertyToPage)( DISPID dispID, CLSID *pClsid ) { return E_NOTIMPL; }
	STDMETHOD(GetPredefinedStrings)( DISPID dispID, CALPOLESTR *pCaStringsOut, CADWORD *pCaCookiesOut ) { return E_NOTIMPL; }
	STDMETHOD(GetPredefinedValue)( DISPID dispID, DWORD dwCookie, VARIANT* pVarOut ) { return E_NOTIMPL; }
// IVsPerPropertyBrowsing
public:
	STDMETHOD(HideProperty)( DISPID dispid, BOOL *pfHide) { return E_NOTIMPL; }
	STDMETHOD(DisplayChildProperties)( DISPID dispid, BOOL *pfDisplay) { return E_NOTIMPL; }
	STDMETHOD(HasDefaultValue)( DISPID dispid, BOOL *pfDefault) { return E_NOTIMPL; }
	STDMETHOD(IsPropertyReadOnly)( DISPID dispid, BOOL *fReadOnly);
	STDMETHOD(GetLocalizedPropertyInfo)( DISPID dispid, LCID localeID, BSTR *pbstrName, BSTR *pbstrDesc);
	STDMETHOD(GetClassName)(BSTR* ) {return E_NOTIMPL;} 
	STDMETHOD(CanResetPropertyValue)(DISPID , BOOL* ) {return E_NOTIMPL;}	
	STDMETHOD(ResetPropertyValue)(DISPID ) {return E_NOTIMPL;}

};

#endif // !defined(AFX_PROJECT_H__625C6496_2C7E_11D3_87BF_A0494CC10000__INCLUDED_)
