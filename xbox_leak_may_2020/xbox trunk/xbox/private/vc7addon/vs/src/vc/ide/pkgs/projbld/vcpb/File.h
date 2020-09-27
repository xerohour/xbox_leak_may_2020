// File.h: Definition of the CPEFile class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILE_H__625C6499_2C7E_11D3_87BF_A0494CC10000__INCLUDED_)
#define AFX_FILE_H__625C6499_2C7E_11D3_87BF_A0494CC10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comlist.h"
#include "FileRegistry.h"

/////////////////////////////////////////////////////////////////////////////
// CPEFile

class CPEFile : 
	public IDispatchImpl<VCFile, &IID_VCFile, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public IVCFileImpl,
	public IVCBuildableItem,
	public IVCExternalCookie,
	public CComObjectRoot
{
public:
	CPEFile();
	~CPEFile();
	static HRESULT CreateInstance(VCFile **ppVCFile);

BEGIN_COM_MAP(CPEFile)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(VCFile)
	COM_INTERFACE_ENTRY(IVCFileImpl)
	COM_INTERFACE_ENTRY(IVCBuildableItem)
	COM_INTERFACE_ENTRY(IVCExternalCookie)
	COM_INTERFACE_ENTRY(VCProjectItem)
END_COM_MAP()

DECLARE_NO_REGISTRY()

// VCFile
public:
	STDMETHOD(Remove)();
	STDMETHOD(get_FullPath)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_RelativePath)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_RelativePath)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_FileConfigurations)(IDispatch **ppVal);
	STDMETHOD(get_DeploymentContent)(VARIANT_BOOL *pVal);
	STDMETHOD(put_DeploymentContent)(VARIANT_BOOL newVal);
	STDMETHOD(get_Extension)(BSTR *pVal);
	STDMETHOD(Move)(IDispatch *pParent);
	STDMETHOD(CanMove)(IDispatch *pParent, VARIANT_BOOL* pbCanMove);

// IVCFileImpl
public:
	STDMETHOD(AddConfiguration)(VCConfiguration * pCfg);
	STDMETHOD(Initialize)(IDispatch * pParent);
	STDMETHOD(Close)();
	STDMETHOD(put_Parent)(/*[in]*/ IDispatch *pVal);
	STDMETHOD(put_Project)(/*[in]*/ IDispatch *pVal);
	STDMETHOD(GetFileConfigurationForProjectConfiguration)(VCConfiguration* pProjCfg, VCFileConfiguration** ppFileCfg);
	STDMETHOD(SaveObject)(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent);
	STDMETHOD(RemoveConfiguration)( IDispatch *pItem );

// VCProjectItem
public:
	STDMETHOD(MatchName)(BSTR bstrNameToMatch, VARIANT_BOOL bFullOnly, VARIANT_BOOL *pbMatched);
	STDMETHOD(get_Project)(/*[out, retval]*/ IDispatch * *pVal);
	STDMETHOD(get_Parent)(/*[out, retval]*/ IDispatch * *pVal);
	STDMETHOD(get_ItemName)(BSTR *Val) { return get_Name(Val); }
	STDMETHOD(get_Kind)(BSTR* kind);
	STDMETHOD(get_VCProjectEngine)(IDispatch** projEngine);

// IVCBuildableItem
public:
	STDMETHOD(get_ItemFileName)(BSTR *pVal);
	STDMETHOD(get_ItemFullPath)(BSTR* pbstrFullPath);
	STDMETHOD(get_ActionList)(IVCBuildActionList** ppActions);
	STDMETHOD(get_FileRegHandle)(void** pfrh);
	STDMETHOD(AssignActions)(VARIANT_BOOL bOnLoad);
	STDMETHOD(UnAssignActions)(VARIANT_BOOL bOnClose);
	STDMETHOD(get_ContentList)(IEnumVARIANT** ppContentList);
	STDMETHOD(get_Registry)(void** pFileRegistry);
	STDMETHOD(get_ProjectConfiguration)(VCConfiguration** ppProjCfg);
	STDMETHOD(get_ProjectInternal)(VCProject** ppProject);
	STDMETHOD(get_ExistingBuildEngine)(IVCBuildEngine** ppBuildEngine);
	STDMETHOD(get_PersistPath)(BSTR* pbstrPersistPath);
	STDMETHOD(RefreshActionOutputs)(long nPropID, IVCBuildErrorContext* pEC);
	STDMETHOD(HandlePossibleCommandLineOptionsChange)(IVCBuildErrorContext* pEC);
	STDMETHOD(ClearDirtyCommandLineOptionsFlag)();

//	IVCExternalCookie
public:
	STDMETHOD(get_ExternalCookie)(void** ppVal) { *ppVal = m_pExternalCookie; return S_OK; }
	STDMETHOD(put_ExternalCookie)(void* pVal) { m_pExternalCookie = pVal; return S_OK; }

// Data
protected:
	CComBSTR m_bstrRelativePath;
	// pointer into m_bstrRelativePath, to just the file name portion
	wchar_t *m_wszFileName;
	CComPtr<IUnknown> m_pParent;
	CComPtr<VCProject> m_pProject;
	CComDynamicListTyped<VCFileConfiguration> m_configfiles;	// collection of configs for this file
	BldFileRegHandle m_frh;
	void* m_pExternalCookie;
	VARIANT_BOOL m_bDeploymentContent;

// INTERNAL METHODS
public:
	HRESULT SetDirty(VARIANT_BOOL bDirty);
	HRESULT CheckCanMove(IDispatch *pParent);
};

#endif // !defined(AFX_FILE_H__625C6499_2C7E_11D3_87BF_A0494CC10000__INCLUDED_)
