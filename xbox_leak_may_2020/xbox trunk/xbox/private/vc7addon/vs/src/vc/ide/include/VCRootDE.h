#pragma once

#include "VCCodeModels.h"
#include "VCDesignerObjectInternal.h"
#include "VSTypeInfo.h"

// MACROs
//

#define DECLARE_VCDESIGNELEMENT(nDefaultIconID) \
	\
	STDMETHOD(get_Picture)(IDispatch **	ppIDispatch) \
	{ \
		PICTDESC pictureDescription = { sizeof(PICTDESC), PICTYPE_ICON }; \
		pictureDescription.icon.hicon = static_cast<HICON>(LoadImage( \
			_Module.GetResourceInstance(), MAKEINTRESOURCE(nDefaultIconID), \
			IMAGE_ICON, 16, 16, LR_SHARED)); \
		return OleCreatePictureIndirect(&pictureDescription, __uuidof(IDispatch), \
				FALSE, reinterpret_cast<void **>(ppIDispatch)); \
	} \

// CVCRootDE
//
// Default implementation of a designe time element.
//
// Examples : Workspaces, Files, Configurations, Resources, CodeElements, etc.
//
// If you get a message that FinalConstruct is ambiguous then you need to
// override it in your class and call each base class' version of this
//

// CVCRootDE
//

template<typename TInterfaces>
class ATL_NO_VTABLE CVCRootDE : 

	public CComObjectRootEx<CComSingleThreadModel>,
	public TInterfaces,
	public IVsPerPropertyBrowsing,
	public ISupportErrorInfo

{
public :

	BEGIN_COM_MAP(CVCRootDE)
		COM_INTERFACE_ENTRY2(IDispatch, _D)
		COM_INTERFACE_ENTRY(VCDesignElement)
		COM_INTERFACE_ENTRY(VCDesignElementI)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
		COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
		COM_INTERFACE_ENTRY_CHAIN(_D)
	END_COM_MAP()

	// IDispatch
	//
	// All interfaces must defer to a single, common IDispatch implementation...
	//

	STDMETHOD(GetTypeInfoCount)(UINT * pctinfo) { return _D::GetTypeInfoCount(pctinfo); }
	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo) { return _D::GetTypeInfo(iTInfo, lcid, ppTInfo); }
	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId) { return _D::GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId); }
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr) { return _D::Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr); }

	// IVsPerPropertyBrowsing
	//

	STDMETHOD(HideProperty)(DISPID dispid, BOOL* pfHide)
		{ return S_FALSE; }
	STDMETHOD(DisplayChildProperties)(DISPID dispid, BOOL* pfDisplay) 
		{ return S_FALSE; }
	STDMETHOD(GetLocalizedPropertyInfo)(DISPID dispid, LCID lcid, BSTR* pbstrName, BSTR* pbstrDesc)
		{
			CComPtr<ITypeInfo> pTypeInfo;
			HRESULT hr = GetTypeInfo(0, lcid, &pTypeInfo);
			CComQIPtr<ITypeInfo2> pTypeInfo2 = pTypeInfo;
			if( FAILED(hr) || (pTypeInfo2 == NULL) )
				return E_FAIL;

			CComBSTR bstrDoc;
			hr = pTypeInfo2->GetDocumentation2(dispid, lcid, &bstrDoc, NULL, NULL);
			if( FAILED(hr) || (bstrDoc.m_str == NULL) )
				return E_FAIL;


			if( pbstrName != NULL )
			{
				return E_NOTIMPL;
			}

			if (pbstrDesc != NULL)
				*pbstrDesc = bstrDoc.Detach();
			return S_OK;
		}

	STDMETHOD(HasDefaultValue)(DISPID dispid, BOOL* fDefault)
		{ return S_FALSE; }
	STDMETHOD(IsPropertyReadOnly)(DISPID dispid, BOOL* fReadOnly)
		{ return S_FALSE; }
   	STDMETHOD(GetClassName)(BSTR* )
		{ return E_NOTIMPL; }
    STDMETHOD(CanResetPropertyValue)(DISPID , BOOL* )
		{ return E_NOTIMPL; }
    STDMETHOD(ResetPropertyValue)(DISPID )
		{ return E_NOTIMPL; }

	// ISupportErrorInfo
	//

	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{ 
		return (InlineIsEqualGUID(riid,__uuidof(_DI))) ? S_OK : S_FALSE;
	}

	// VCDesignElement
	//

	STDMETHOD(get_DTE)(DTE ** ppDTE) 
	{ 
		VALIDATE_AND_CLEAR(ppDTE);
		return CVCUtilityLibrary::QueryService(reinterpret_cast<_DTE **>(ppDTE)); 
	}

	STDMETHOD(get_Name)(BSTR * pbstrName)
	{
		return S_FALSE;
	}

	STDMETHOD(get_DisplayName)(BSTR * pbstrDisplayName)
	{
		return get_Name(pbstrDisplayName);
	}

	STDMETHOD(get_IsCaseSensitive)(VARIANT_BOOL * pbIsCaseSensitive)
	{
		VALIDATE_WRITE_POINTER_HR(pbIsCaseSensitive);

		*pbIsCaseSensitive = VARIANT_FALSE;

		return S_OK;
	}

	STDMETHOD(get_Parent)(IDispatch ** ppParent)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(get_IsZombie)(VARIANT_BOOL * pbIsZombie)
	{
		VALIDATE_AND_CLEAR(pbIsZombie);
		*pbIsZombie = VARIANT_FALSE;
		return S_OK;
	}

	STDMETHOD(get_IsReadOnly)(VARIANT_BOOL * pbIsReadOnly)
	{
		VALIDATE_AND_CLEAR(pbIsReadOnly);
		*pbIsReadOnly = VARIANT_FALSE;
		return S_OK;
	}

	STDMETHOD(IsSelf)(IDispatch * pOther, VARIANT_BOOL * pbIsSelf)
	{
		VALIDATE_AND_CLEAR_POINTER(pbIsSelf);

		HRESULT hr(S_OK);
		CComPtr<IUnknown> spThis;
		CComPtr<IUnknown> spUnknown;

		FAILED_RETURN_HR(QueryInterface(__uuidof(IUnknown), reinterpret_cast<void **>(&spThis)));
		FAILED_RETURN_HR(pOther->QueryInterface(__uuidof(IUnknown), reinterpret_cast<void **>(&spUnknown)));

		*pbIsSelf = (spThis == spUnknown ? VARIANT_TRUE : VARIANT_FALSE);

		return S_OK;
	}

	// Internal
	//

	STDMETHOD(put_Parent)(IDispatch * pIDispatch)
	{
		VSFAIL("Derived class must implement put_Parent");
		return E_UNEXPECTED;
	}

	STDMETHOD(put_Name)(BSTR bstrName)
	{
		VSFAIL("Derived class must implement put_Name");
		return E_UNEXPECTED;
	}

	STDMETHOD(get_Descendants)(IDispatch ** ppDescendants)
	{
		VSFAIL("Derived class must implement get_Descendants");
		return E_UNEXPECTED;
	}
};

template <class T, const IID* piid = &__uuidof(T), const GUID* plibid = &CAtlModule::m_libid, WORD wMajor = 1,
WORD wMinor = 0, class tihclass = CComTypeInfoHolder>
class ATL_NO_VTABLE CVCIDispatchImpl : public IDispatchImpl<T, piid, plibid, wMajor, wMinor, tihclass>
{
public :

	typedef CVCIDispatchImpl _D;
	typedef T _DI;

	BEGIN_COM_MAP(CVCIDispatchImpl)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()
};

template <class T, const IID* piid = &__uuidof(T), const GUID* plibid = &LIBID_VCCodeModelLibrary, 
WORD wMajor = 1 /* For CVsTypeInfoHolder, this is TYPELIB Resource ID, NOT Version! */,
WORD wMinor = 0, class tihclass = CVsTypeInfoHolder>
class ATL_NO_VTABLE CVCIDispatchImplDE : public CVCIDispatchImpl<T, piid, plibid, wMajor, wMinor, tihclass>
{
};

// Passthroughs
//

class ATL_NO_VTABLE IVCRootDEPT : 
	public CVCIDispatchImplDE<VCDesignElement>, 
	public VCDesignElementI
{ 
};
