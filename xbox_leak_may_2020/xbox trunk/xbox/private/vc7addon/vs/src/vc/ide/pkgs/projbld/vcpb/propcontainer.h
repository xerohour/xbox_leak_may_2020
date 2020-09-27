#pragma once

// basic property container class; must be derived from, not used directly

#include <vccoll.h>
#include <vcmap.h>
#include "comlist.h"
#include "ProjEvaluator.h"

class CPropertyContainerImpl : 
	public IVCPropertyContainer,
	public CProjectEvaluator
{
public:
	CPropertyContainerImpl() {}
	virtual HRESULT SetDirty(VARIANT_BOOL bDirty) { return E_NOTIMPL; }

// IVCPropertyContainer
public:
	STDMETHOD(HasLocalStorage)(VARIANT_BOOL bForSave, VARIANT_BOOL* pbHasLocalStorage);
	STDMETHOD(GetProp)(long id, /*[in]*/ VARIANT *varValue);
	STDMETHOD(SetProp)(long id, VARIANT varValue);
	STDMETHOD(GetLocalProp)(long id, /*[out]*/ VARIANT *pvarValue);
	STDMETHOD(GetParentProp)(long id, VARIANT_BOOL bAllowInherit,  VARIANT *pvarValue);
	STDMETHOD(GetMultiProp)(long id, LPCOLESTR szSeparator, VARIANT_BOOL bSkipLocal, BSTR *varValue);
	STDMETHOD(IsMultiProp)(long idProp, VARIANT_BOOL* pbIsMulti);
	STDMETHOD(GetMultiPropSeparator)(long id, BSTR* pbstrPreferred, BSTR* pbstrAll);
	STDMETHOD(GetStrProperty)(long idProp, BSTR* pbstrValue);
	STDMETHOD(SetStrProperty)(long idProp, BSTR bstrValue);
	STDMETHOD(GetIntProperty)(long idProp, long* pnValue);
	STDMETHOD(SetIntProperty)(long idProp, long nValue);
	STDMETHOD(GetBoolProperty)(long idProp, VARIANT_BOOL* pbValue);
	STDMETHOD(SetBoolProperty)(long idProp, VARIANT_BOOL bValue);
	STDMETHOD(DirtyProp)(long id) { return S_FALSE; }
	STDMETHOD(Clear)(long id);
	STDMETHOD(ClearAll)();
	STDMETHOD(Evaluate)(BSTR bstrIn, BSTR *bstrOut);
	STDMETHOD(EvaluateWithValidation)(BSTR bstrSource, long idProp, BSTR* pbstrExpanded);
	STDMETHOD(GetEvaluatedStrProperty)(long idProp, BSTR* pbstrOut);
	STDMETHOD(GetEvaluatedMultiProp)(long idProp, LPCOLESTR szSeparator, VARIANT_BOOL bCollapseMultiples, 
		VARIANT_BOOL bCaseSensitive, BSTR* pbstrOut);
	STDMETHOD(Commit)(){ return E_NOTIMPL; }
	STDMETHOD(GetStagedPropertyContainer)(VARIANT_BOOL bCreateIfNecessary, IVCStagedPropertyContainer** ppPropContainer);
	STDMETHOD(ReleaseStagedPropertyContainer)();		// intended for use ONLY by the staged property container itself

// IVCPropertyContainer helpers
protected:
	virtual HRESULT DoGetProp(long id, BOOL bCheckSpecialProps, VARIANT *pVarValue);
	virtual HRESULT DoGetParentProp(long id, VARIANT_BOOL bAllowInherit, BOOL bCheckSpecialProps, VARIANT *pVarValue);
	virtual HRESULT DoSetProp(long id, BOOL bCheckSpecial, VARIANT varValue, long nOverrideID = -1);
	virtual HRESULT FinishSetPropOrClear(long id, long nOverrideID);
	virtual HRESULT DoGetLocalProp(long id, BOOL bCheckSpecial, VARIANT *pvarValue);
	virtual HRESULT DoGetStrProperty(long idProp, BOOL bSpecialPropsOnly, BSTR* pbstrValue);
	virtual HRESULT DoSetStrProperty(long id, BOOL bCheckSpecialProps, BSTR bstrValue, long nOverrideID = -1);
	virtual HRESULT DoGetIntProperty(long idProp, BOOL bCheckSpecial, long* pnValue);
	virtual HRESULT GetParentIntProperty(long idProp, long* pnValue);
	virtual HRESULT GetLocalIntProperty(long idProp, long* pnValue);
	virtual HRESULT DoSetIntProperty(long id, BOOL bCheckSpecialProps, long nValue, long nOverrideID = -1);
	virtual HRESULT DoGetBoolProperty(long idProp, BOOL bCheckSpecial, VARIANT_BOOL* pbValue);
	virtual HRESULT DoSetBoolProperty(long id, BOOL bCheckSpecialProps, VARIANT_BOOL bValue, long nOverrideID = -1);
	virtual HRESULT DoClear(long id, long nOverrideID = -1);
	virtual HRESULT DoClearAll();

	virtual HRESULT FinishClose();
	virtual long GetOverrideID(long idProp) { return -1; }
	virtual BOOL CheckCanDirty() { return TRUE; }
	HRESULT DoGetVCProjectEngine(IDispatch** projEngine);

public:
	//Methods to write properties to stream
	static HRESULT WritePropertyToStream(IStream *pStream, VARIANT_BOOL bProp, DISPID dispid);
	static HRESULT WritePropertyToStream(IStream *pStream, CComBSTR& bstrProp, DISPID dispid);
	static HRESULT WritePropertyToStream(IStream *pStream, long lProp, DISPID dispid);
	static HRESULT WritePropertyToStream(IStream *pStream, CComVariant& variantProp, DISPID dispid);

// ISpecifyPropertyPages
public:
    STDMETHOD(GetPages)(/* [out] */ CAUUID *pPages) { return E_NOTIMPL; }
//DATA
protected:
	CComDynamicListTyped<IVCPropertyContainer> m_rgParents;
 	CVCMap<long,long,CComVariant,VARIANT&> m_PropMap;
	CComPtr<IVCStagedPropertyContainer> m_spStagedContainer;
};

class CVCStagedPropertyContainer : 
	public IVCStagedPropertyContainer, 
	public CPropertyContainerImpl,
	public CComObjectRoot
{
public:
	CVCStagedPropertyContainer() : m_pos(NULL), m_cStaged(0) {}
	~CVCStagedPropertyContainer() {}

	static HRESULT CreateInstance(IVCPropertyContainer* pParentPropContainer, IVCStagedPropertyContainer **ppStagedPropertyContainer);
	HRESULT Initialize(IVCPropertyContainer* pParentPropContainer);

BEGIN_COM_MAP(CVCStagedPropertyContainer)
	COM_INTERFACE_ENTRY(IVCStagedPropertyContainer)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
END_COM_MAP()
DECLARE_NO_REGISTRY()

// IVCPropertyContainer overrides
public:
	STDMETHOD(GetMultiProp)(long id, LPCOLESTR szSeparator, VARIANT_BOOL bSkipLocal, BSTR *varValue);
	STDMETHOD(GetMultiPropSeparator)(long id, BSTR* pbstrPreferred, BSTR* pbstrAll);
	STDMETHOD(Commit)();

// CPropertyContainerImpl overrides
	HRESULT DoGetParentProp(long id, VARIANT_BOOL bAllowInherit, BOOL bCheckSpecialProps, VARIANT *pVarValue);

// IVCStagedPropertyContainer
public:
	STDMETHOD(get_Parent)(IVCPropertyContainer** ppParent);
	STDMETHOD(ClearPropertyRange)(long nMinID, long nMaxID);
	STDMETHOD(GetGrandParentProp)(long id, VARIANT* pvarValue);		// skip to parent of our non-staged parent
	STDMETHOD(Close)();
	STDMETHOD(CommitPropertyRange)(long nMinID, long nMaxID);
	STDMETHOD(Reset)(void);
	STDMETHOD(NextElement)(long* pKey, VARIANT* pVal);
	STDMETHOD(HoldStagedContainer)();			// number of calls to Hold/Release StagedContainer need to match
	STDMETHOD(ReleaseStagedContainer)();		// number of calls to Hold/Release StagedContainer need to match
	STDMETHOD(HasDefaultValue)(DISPID dispid, BOOL *pfDefault);

// helpers
protected:
	HRESULT DoCommitPropertyRange(long nMinID, long nMaxID, bool bIgnoreRange);
	virtual BOOL CheckCanDirty() { return FALSE; }

protected:
	CComPtr<IVCPropertyContainer> m_spParentPropContainer;

	VCPOSITION m_pos;
	int m_cStaged;
};
