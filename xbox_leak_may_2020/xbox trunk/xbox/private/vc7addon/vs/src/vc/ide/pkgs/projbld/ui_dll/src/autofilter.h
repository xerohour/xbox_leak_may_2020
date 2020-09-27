// AutoFilter.h
// declares what we implement for the DTE ProjectItem

#include "PrjNode.h"
#include "AutoBase.h"
#include "vcfgrp.h"

// JOSHEP_TODO: you will need to set these classes up so that they know their relationship to:
//	a) the collection they belong to
//	b) any collection they own

// classes in this header
class CAutoFilter;

// string version of the guid
_declspec(selectany) LPOLESTR szguidCATIDAutoFilter = L"{610d461d-d0d5-11d2-8599-006097c68e81}";

class CAutoFilter : 
	public CAutoBase<CAutoFilter, VCFilter, szguidCATIDAutoFilter>,
	public CComDualImpl<_ProjectItem, &IID__ProjectItem, &LIBID_DTE, 7, 0>, 
	public ISupportVSProperties,
	public CComObjectRoot
{
public:
	CAutoFilter();
	~CAutoFilter();
	void Initialize(IDispatch* pAutoItems, IDispatch* pAutoProject, IDispatch* pItem);
	static HRESULT CreateInstance(CAutoFilter** ppAutoFilter, IDispatch* pAutoItems, IDispatch* pAutoProject,
		IDispatch* pItem);
	VCFilter *GetFilter() { return(m_spFilter); }

BEGIN_COM_MAP(CAutoFilter)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(_ProjectItem)
	COM_INTERFACE_ENTRY(ISupportVSProperties)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CAutoFilter) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

// ProjectItem
public:
    /* _ProjectItem methods */
    STDMETHOD(get_IsDirty)(VARIANT_BOOL FAR* lpfReturn);
    STDMETHOD(put_IsDirty)(VARIANT_BOOL DirtyFlag);
    STDMETHOD(get_FileNames)(short Index, BSTR FAR* lpbstrReturn);
    STDMETHOD(SaveAs)(BSTR NewFileName, VARIANT_BOOL FAR* lpfReturn);
    STDMETHOD(get_FileCount)(short FAR* lpsReturn);
    STDMETHOD(get_Reserved1)(long FAR* lpulReturn) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved1)(long Value) { return E_NOTIMPL; }
    STDMETHOD(Reserved2)(BSTR FileName) { return E_NOTIMPL; }
    STDMETHOD(Reserved3)() { return E_NOTIMPL; }
    STDMETHOD(get_Name)(BSTR FAR* pbstrReturn);
    STDMETHOD(put_Name)(BSTR bstrName);
    STDMETHOD(get_Reserved14)(IDispatch * FAR* ppDispatch) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved4)(IDispatch * FAR* ppVbaModule) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved5)(long FAR* pkind) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved6)(IDispatch * FAR* lppaReturn) { return E_NOTIMPL; }
    STDMETHOD(get_Collection)(ProjectItems FAR* FAR* lppcReturn);
    STDMETHOD(get_Reserved15)(VARIANT_BOOL FAR* lpfReturn) { return E_NOTIMPL; }
    STDMETHOD(get_Properties)(Properties FAR* FAR* ppObject);
    STDMETHOD(Reserved7)(IDispatch * FAR* lppcReturn) { return E_NOTIMPL; }
    STDMETHOD(Reserved8)(BSTR Key, BSTR FAR* lpbstrValue) { return E_NOTIMPL; }
    STDMETHOD(Reserved9)(BSTR Key, BSTR Value) { return E_NOTIMPL; }
    STDMETHOD(Reserved10)() { return E_NOTIMPL; }
    STDMETHOD(get_Reserved11)(long FAR* lpdwContextID) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved11)(long dwContextID) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved12)(BSTR FAR* lpbstrDescription) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved12)(BSTR bstrDescription) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved13)(BSTR FAR* pbstrReturn) { return E_NOTIMPL; }
    STDMETHOD(get_DTE)(DTE FAR* FAR* lppaReturn);
    STDMETHOD(get_Kind)(BSTR FAR* lpbstrFileName);
    STDMETHOD(get_ProjectItems)(ProjectItems FAR* FAR* lppcReturn);
    STDMETHOD(get_IsOpen)(BSTR LogView, VARIANT_BOOL FAR* lpfReturn);
    STDMETHOD(Open)(BSTR LogView, Window FAR* FAR* lppfReturn);
    STDMETHOD(Remove)();
    STDMETHOD(Delete)();
    STDMETHOD(ExpandView)();
    STDMETHOD(get_Object)(IDispatch **ppProjectItemModel);
    STDMETHOD(get_Extender)(BSTR bstrExtenderName, IDispatch **ppExtender);
    STDMETHOD(get_ExtenderNames)(VARIANT *pvarExtenderNames);
    STDMETHOD(get_ExtenderCATID)(BSTR *pbstrRetval);

    STDMETHOD(get_Saved)(VARIANT_BOOL *lpfReturn);
    STDMETHOD(put_Saved)(VARIANT_BOOL Dirty);
    STDMETHOD(get_SourceControl)(SourceControl ** /*ppSourceControl*/);
    STDMETHOD(get_ConfigurationManager)(ConfigurationManager ** /*ppConfigurationManager*/);
    STDMETHOD(get_CodeModel)(CodeModel ** /*ppCodeModel*/);
    STDMETHOD(Save)(BSTR /*FileName*/);
    STDMETHOD(get_Document)(Document ** /*ppDocument*/);
    STDMETHOD(get_SubProject)(Project ** /*ppProject*/);
    STDMETHOD(get_ContainingProject)(Project ** /*ppProject*/);
    STDMETHOD(get_FileCodeModel)(FileCodeModel ** ppFileCodeModel);
    STDMETHOD(AddFolder)(BSTR Name, BSTR Kind, ProjectItem **pProjectItem);

protected:
	CComPtr<IDispatch> m_spAutoItems;	// automation collection filter belongs in
	CComPtr<IDispatch> m_spAutoProject;	// project this filter belongs to
	CComQIPtr<VCFilter> m_spFilter;

	CVCFileGroup* GetFileGroup();
	CVCProjectNode* GetProjectNode(CVCFileGroup* pFileGroup = NULL);

// ISupportVSProperties
public:
	STDMETHOD(NotifyPropertiesDelete)()
	{
		return E_NOTIMPL;	// CAutoFilter::NotifyPropertiesDelete
	}
};
