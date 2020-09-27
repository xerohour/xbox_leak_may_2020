// Filter.h: Definition of the CFilter class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTER_H__625C649C_2C7E_11D3_87BF_A0494CC10000__INCLUDED_)
#define AFX_FILTER_H__625C649C_2C7E_11D3_87BF_A0494CC10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comlist.h"
#include "collection.h"

/////////////////////////////////////////////////////////////////////////////
// CFilter

class CFilter : 
	public IDispatchImpl<VCFilter, &IID_VCFilter, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public IVCFilterImpl,
	public IVCExternalCookie,
	public CComObjectRoot
{
public:
	CFilter() : m_pExternalCookie(NULL), m_bItemsCollectionDirty(true), m_bParseFiles(VARIANT_TRUE), m_bSCCFiles(VARIANT_TRUE)
	{
	}
	~CFilter()
	{
	}
	static HRESULT CreateInstance(VCFilter **ppFilter);

BEGIN_COM_MAP(CFilter)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(VCFilter)
	COM_INTERFACE_ENTRY(IVCFilterImpl)
	COM_INTERFACE_ENTRY(IVCExternalCookie)
	COM_INTERFACE_ENTRY(VCProjectItem)
	COM_INTERFACE_ENTRY(IVCCollectionProvider)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CFilter) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

DECLARE_NO_REGISTRY()

// VCFilter
public:
	STDMETHOD(Remove)();
	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Name)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_CanonicalName)(BSTR* pbstrCanonicalName);
	STDMETHOD(get_UniqueIdentifier)(BSTR* pbstrID);
	STDMETHOD(put_UniqueIdentifier)(BSTR bstrID);
	STDMETHOD(AddFile)(BSTR bstrPath, /*[out, retval]*/ IDispatch * *ppDispFile);
	STDMETHOD(CanAddFile)(BSTR bstrFile, VARIANT_BOOL* pbCanAdd);
	STDMETHOD(AddFilter)(BSTR bstrName, /*[out, retval]*/ IDispatch **ppDispFilter);
	STDMETHOD(CanAddFilter)(BSTR bstrFilter, VARIANT_BOOL* pbCanAdd);
	STDMETHOD(get_Filter)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Filter)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Filters)(/*[out, retval]*/ IDispatch **pVal);
	STDMETHOD(get_Files)(/*[out, retval]*/ IDispatch **pVal);
	STDMETHOD(get_Items)(/*[out, retval]*/ IDispatch **pVal);
	STDMETHOD(RemoveFile)(/*[in]*/  IDispatch *pItem);
	STDMETHOD(RemoveFilter)(/*[in]*/  IDispatch *pItem);
	STDMETHOD(get_ParseFiles)(VARIANT_BOOL* pbParse);
	STDMETHOD(put_ParseFiles)(VARIANT_BOOL bParse);
	STDMETHOD(get_SourceControlFiles)(VARIANT_BOOL* pbSCC);
	STDMETHOD(put_SourceControlFiles)(VARIANT_BOOL bSCC);
	STDMETHOD(AddWebReference)(BSTR bstrUrl, IDispatch** ppFile);
	STDMETHOD(Move)(IDispatch *pParent);
	STDMETHOD(CanMove)(IDispatch *pParent, VARIANT_BOOL* pbCanMove);

// IVCFilterImpl
public:
	STDMETHOD(Close)();
	STDMETHOD(AddItem)(IDispatch * pDisp);
	STDMETHOD(CanAddFile2)(BSTR bstrFile);
	STDMETHOD(CanAddFilter2)(BSTR bstrFilter, VARIANT_BOOL bSetErrorInfo);
	STDMETHOD(RemoveExistingFile)(IDispatch* pDisp);
	STDMETHOD(IsMatch)(LPCOLESTR szFileName, VARIANT_BOOL *pbIsMatch);
	STDMETHOD(SaveObject)(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent);
	STDMETHOD(put_Parent)(/*[in]*/ IDispatch *pVal);
	STDMETHOD(AddChild)(IDispatch* pChild);
	STDMETHOD(RemoveChild)(IDispatch* pChild);
	STDMETHOD(RemoveContents)();

// VCProjectItem
public:
	STDMETHOD(MatchName)(BSTR bstrNameToMatch, VARIANT_BOOL bFullOnly, VARIANT_BOOL *pbMatched);
	STDMETHOD(get_Project)(IDispatch **ppProject);
	STDMETHOD(get_Parent)(/*[out, retval]*/ IDispatch * *pVal);
	STDMETHOD(get_ItemName)(BSTR *Val) { return get_Name(Val); }
	STDMETHOD(get_Kind)(BSTR* kind);
	STDMETHOD(get_VCProjectEngine)(IDispatch** projEngine);

// IVCCollectionProvider
public:
	STDMETHOD(UpdateItemsCollection)(DWORD dwCookie);

//	IVCExternalCookie
public:
	STDMETHOD(get_ExternalCookie)(void** ppVal) { *ppVal = m_pExternalCookie; return S_OK; }
	STDMETHOD(put_ExternalCookie)(void* pVal) { m_pExternalCookie = pVal; return S_OK; }

// Data
protected:
	CComPtr<IDispatch> 	m_pParent;
	CComDynamicListTyped<VCFile> m_rgFiles;	// collection of files
	CComDynamicListTyped<VCFilter> m_rgFilters;	// collection of filters
	CComDynamicListTyped<VCProjectItem> m_rgItems;	// collection of files & filters immediately in this filter
	void* m_pExternalCookie;
	CComBSTR 			m_bstrFilter;
	CComBSTR 			m_bstrName;
	CComBSTR			m_bstrUniqueIdentifier;
	bool				m_bItemsCollectionDirty;
	VARIANT_BOOL		m_bParseFiles;
	VARIANT_BOOL		m_bSCCFiles;

protected:
	HRESULT SetDirty(VARIANT_BOOL bDirty);
	HRESULT FindParentProject(IDispatch* pObject, IDispatch** ppParentProject);
	HRESULT FindExistingFile(BSTR bstrFile, IVCProjectImpl** ppProjImpl, IDispatch** ppFile, BOOL bSetErrorInfo);
	HRESULT FindExistingFilter(BSTR bstrFilter, CStringW& strCleanedUpName, IDispatch** ppDispFilter, BOOL bSetErrorInfo);
	HRESULT DoRemoveExistingFile(IDispatch* pFile, BOOL bTellProject);
	HRESULT RemoveExistingFilter(IDispatch* pDispFilter, long index);
	HRESULT DoRemoveContents(BOOL bTellParent);
	bool FindAsParent(IDispatch* pItem, IDispatch* pParent);
	void BuildItemsCollection();
	HRESULT CheckCanMove(IDispatch *pParent);
	HRESULT WrapCanDirty(UINT idCheck);
	HRESULT CheckCanAddFile(BSTR bstrFile, IVCProjectImpl** ppProjImpl, BOOL bSetErrorInfo);
	HRESULT CheckCanAddFilter(BSTR bstrFilter, CStringW& strCleanedUpName, BOOL bSetErrorInfo);
	HRESULT QueryCanRemoveFile(VCFile* pFile);
};

#endif // !defined(AFX_FILTER_H__625C649C_2C7E_11D3_87BF_A0494CC10000__INCLUDED_)
