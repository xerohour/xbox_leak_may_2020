#pragma once

// VCDesignerObjectInternal.h
//

// Things the users must never, never see...
//
// Therefore, it is OKAY to use inheritance (yes, even virtual inheritance!)
//

[
	uuid(29BF7883-FE8C-11d2-AAE9-00C04F72DB55),
]
class ATL_NO_VTABLE VCDesignElementI : public IUnknown
{
public :

	virtual HRESULT STDMETHODCALLTYPE put_Parent(IDispatch * pIDispatch) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Name(BSTR bstrName) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Name(BSTR * pbstrName) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Descendants(IDispatch ** ppIDispatch) = 0;
};


// VCDesignElementsI
//
// DE that is a collection of any IDispatch Based objects
//

[
	uuid(29BF7886-FE8C-11d2-AAE9-00C04F72DB55),
]
class ATL_NO_VTABLE VCDesignElementsI : public VCDesignElementI
{
public :

	virtual HRESULT STDMETHODCALLTYPE IsEmpty(BOOL * pbIsEmpty) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddItem(IDispatch * pIDispatch) = 0;
	virtual HRESULT STDMETHODCALLTYPE RemoveIndex(long lIndex) = 0;
	virtual HRESULT STDMETHODCALLTYPE RemoveItem(IDispatch * pIDispatch) = 0;
	virtual HRESULT STDMETHODCALLTYPE RemoveAll(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE Find(BSTR bstrName, IDispatch ** ppDispatch) = 0;
};

