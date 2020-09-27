// VCFGrp.h: interface for the CVCFileGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VCGROUP_H__7F438FBE_AB8B_11D0_8D1E_01A0C91BC943__INCLUDED_)
#define AFX_VCGROUP_H__7F438FBE_AB8B_11D0_8D1E_01A0C91BC943__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "vc.h"
#include "VCNode.h"
#include "VCBsNode.h"
#include "vcarchy.h"
#include "vssolutn.h"

class CVCFileGroup :	public CVCBaseFolderNode,
						public IDispatchImpl<Filter, &IID_Filter, &LIBID_VCPrivateProjectLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
						public IOleCommandTarget,
						public IVsPerPropertyBrowsing,
						public ICategorizeProperties,
						public IPerPropertyBrowsing
{
public:
	CVCFileGroup(void);
	virtual ~CVCFileGroup();

	BOOL Open();
	static HRESULT CreateInstance(CVCFileGroup **, IDispatch* pItem);
	virtual HRESULT OnDelete(CVCNode *pRoot, DWORD dwDelItemOp, BOOL bCloseIfOpen = TRUE);

BEGIN_COM_MAP(CVCFileGroup)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(Filter)
	COM_INTERFACE_ENTRY(IOleCommandTarget)
	COM_INTERFACE_ENTRY_IID(IID_ICategorizeProperties, ICategorizeProperties)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CVCFileGroup)

// ICategorizeProperties
public:
	STDMETHOD(MapPropertyToCategory)( DISPID dispid, PROPCAT* ppropcat);
	STDMETHOD(GetCategoryName)( PROPCAT propcat, LCID lcid, BSTR* pbstrName);

// IPerPropertyBrowsing
	STDMETHOD(GetDisplayString)( DISPID dispID, BSTR *pBstr ) { return E_NOTIMPL; /* CVCFileGroup::GetDisplayString */ }
	STDMETHOD(MapPropertyToPage)( DISPID dispID, CLSID *pClsid ) { return E_NOTIMPL; /* CVCFileGroup::MapPropertyToPage */ }
	STDMETHOD(GetPredefinedStrings)( DISPID dispID, CALPOLESTR *pCaStringsOut, CADWORD *pCaCookiesOut )
		{ return E_NOTIMPL; /* CVCFileGroup::GetPredefinedStrings */ }
	STDMETHOD(GetPredefinedValue)( DISPID dispID, DWORD dwCookie, VARIANT* pVarOut )
		{ return E_NOTIMPL; /* CVCFileGroup::GetPredefinedValue */ }

// IVsPerPropertyBrowsing
public:
	STDMETHOD(HideProperty)( DISPID dispid, BOOL *pfHide) { return E_NOTIMPL; /* CVCFileGroup::HideProperty */ }
	STDMETHOD(DisplayChildProperties)( DISPID dispid, BOOL *pfDisplay) { return E_NOTIMPL; /* CVCFileGroup::DisplayChildProperties */ }
	STDMETHOD(HasDefaultValue)( DISPID dispid, BOOL *pfDefault) { return E_NOTIMPL; /* CVCFileGroup::HasDefaultValue */ }
	STDMETHOD(IsPropertyReadOnly)( DISPID dispid, BOOL *fReadOnly) { return E_NOTIMPL; /* CVCFileGroup::IsPropertyReadOnly */ }
	STDMETHOD(GetLocalizedPropertyInfo)( DISPID dispid, LCID localeID, BSTR *pbstrName, BSTR *pbstrDesc);
	STDMETHOD(GetClassName)(BSTR* pbstrClassName);
    STDMETHOD(CanResetPropertyValue)(DISPID , BOOL* ) {return E_NOTIMPL;}	
    STDMETHOD(ResetPropertyValue)(DISPID ) {return E_NOTIMPL;}

public:
	UINT GetIconIndex(ICON_TYPE);
	HRESULT GetGuidProperty( VSHPROPID propid, GUID *pguid);
	HRESULT SetProperty(VSHPROPID propid, const VARIANT& var);
	HRESULT GetProperty(VSHPROPID propid, VARIANT *pvar);

	// Filter
	STDMETHOD(get_Name)( BSTR *pVal );
	STDMETHOD(put_Name)( BSTR newVal );
	STDMETHOD(get_Filter)( BSTR *pVal );
	STDMETHOD(put_Filter)( BSTR newVal );
	STDMETHOD(get_ParseFiles)(VARIANT_BOOL* pbParse);
	STDMETHOD(put_ParseFiles)(VARIANT_BOOL bParse);
	STDMETHOD(get_SourceControlFiles)(VARIANT_BOOL* pbSCC);
	STDMETHOD(put_SourceControlFiles)(VARIANT_BOOL bSCC);
#ifdef AARDVARK
	CComBSTR m_bstrDirectory;
	STDMETHOD(get_Directory)( BSTR *pVal ){ m_bstrDirectory.CopyTo( pVal ); return S_OK; }
	STDMETHOD(put_Directory)( BSTR newVal ){ m_bstrDirectory = newVal; return S_OK; }
#endif // AARDVARK

	// automation extender methods
	STDMETHOD(get_Extender)(BSTR bstrName, IDispatch **ppDisp);
	STDMETHOD(get_ExtenderNames)(VARIANT *pvarNames);
	STDMETHOD(get_ExtenderCATID)(BSTR *pbstrGuid);

	// IOleCommandTarget
	STDMETHOD(QueryStatus)(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText);
	STDMETHOD(Exec)(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut);

	virtual HRESULT QueryStatusAtIndex(CVCNode::enumKnownGuidList eGuid, const GUID* pguidCmdGroup, 
		OLECMD prgCmds[], OLECMDTEXT *pCmdText, ULONG nIndex);

	virtual HRESULT DisplayContextMenu(VARIANT *pvaIn);
	virtual UINT GetKindOf(void) const;
	virtual	HRESULT	CleanUpBeforeDelete();
	virtual HRESULT CleanUpUI( CVCNode *pRoot, DWORD dwDelItemOp, BOOL bCloseIfOpen = TRUE );
	HRESULT CleanUpObjectModel(IDispatch* pParentDisp, IDispatch* pItemDisp);
	HRESULT GetObjectsForDelete(CVCFileGroup* pFilter, IDispatch** ppParentDisp, IDispatch** ppItemDisp);

	virtual UINT GetKindOf( void )
	{
		return Type_CVCFileGroup;
	}

	
	virtual HRESULT DoDefaultAction(BOOL fNewFile){ return E_NOTIMPL; };
	virtual HRESULT GetCanonicalName(BSTR* pbstrCanonicalName);
	virtual HRESULT GetName(BSTR* pbstrName);

	virtual CVCProjectNode* GetVCProjectNode();
	virtual HRESULT GetExtObject(CComVariant& varRes);
	HRESULT GetVCFilter(VCFilter** ppFilter)
	{	
		CHECK_POINTER_NULL(ppFilter);
		CComQIPtr<VCFilter> spFilter = m_dispkeyItem;
		*ppFilter = spFilter.Detach();
		return S_OK;
	}

protected:
	void Initialize(IDispatch* pItem);
	BOOL m_fDeleted;

private:
	// automation extender CATID
	static const LPOLESTR s_wszCATID;

#ifdef AARDVARK
public:
	eMagic m_eMagic;
#endif // AARDVARK
};

#endif // !defined(AFX_VCGROUP_H__7F438FBE_AB8B_11D0_8D1E_01A0C91BC943__INCLUDED_)
