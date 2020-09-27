#pragma once

#include <ShellVer.h>

// CVsRootCE
//

#define DECLARE_VSCODEELEMENT(Kind, nDefaultIconID, wszExtenderCATID) \
	\
	DECLARE_VCDESIGNELEMENT(nDefaultIconID) \
	\
	virtual vsCMElement GetKind(void) { return Kind; } \
	STDMETHOD(get_Kind)(vsCMElement * pKind) \
	{ \
		VALIDATE_AND_CLEAR(pKind); \
		*pKind = Kind; \
		return S_OK; \
	} \
	\
	STDMETHOD(get_ExtenderCATID)(BSTR * pbstrExtenderCATID) \
	{ \
		VALIDATE_AND_CLEAR(pbstrExtenderCATID); \
		*pbstrExtenderCATID = CComBSTR(wszExtenderCATID).Detach(); \
		FAILED_RETURN(E_OUTOFMEMORY, *pbstrExtenderCATID); \
		return S_OK; \
	}

template<typename TInterfaces>
class ATL_NO_VTABLE CVsRootCE : public CVCRootDE<TInterfaces>
{
public :

	BEGIN_COM_MAP(CVsRootCE)
		COM_INTERFACE_ENTRY(CodeElement)
		COM_INTERFACE_ENTRY_CHAIN(CVCRootDE<TInterfaces>)
	END_COM_MAP()

	virtual vsCMElement GetKind(void) { return vsCMElementOther; }

	// CodeElement (VS)
	//

	STDMETHOD(get_Kind)(vsCMElement * pCodeEltKind);
	STDMETHOD(get_ExtenderNames)(VARIANT * pvExtenderNames); 
	STDMETHOD(get_Extender)(BSTR bstrExtenderName, IDispatch ** ppExtender); 
	STDMETHOD(get_ExtenderCATID)(BSTR * pbstrExtenderCATID)
		{ VSFAIL(L"Derived class must use DECLARE_VSCODEELEMENT!"); return E_UNEXPECTED; }
	STDMETHOD(get_InfoLocation)(vsCMInfoLocation *pInfoLocation); 
	STDMETHOD(get_IsCodeType)(VARIANT_BOOL *pIsCodeType);
};

template <class T, const IID* piid = &__uuidof(T), 
	const GUID* plibid = &LIBID_EnvDTE, WORD wMajor = DTE_LIB_VERSION_MAJ,
WORD wMinor = DTE_LIB_VERSION_MIN, class tihclass = CComTypeInfoHolder>
class ATL_NO_VTABLE CVCIDispatchImplDTE : public CVCIDispatchImpl<T, piid, plibid, wMajor, wMinor, tihclass>
{
};

template<typename TInterfaces>
STDMETHODIMP CVsRootCE<TInterfaces>::get_ExtenderNames(VARIANT * pvExtenderNames) 
{ 
	VALIDATE_ARGUMENT(pvExtenderNames);

	HRESULT hr(S_OK);
	CComBSTR bstrCATID;
	CComPtr<ObjectExtenders> spExtensionManager;
	CComPtr<IDispatch> spThis;

	FAILED_RETURN_HR(get_ExtenderCATID(&bstrCATID));
	FAILED_RETURN(E_OUTOFMEMORY, bstrCATID);
	FAILED_RETURN_HR(CVCUtilityLibrary::QueryService(&spExtensionManager));
	FAILED_RETURN_HR(GetUnknown()->QueryInterface(__uuidof(IDispatch), reinterpret_cast<void **>(&spThis)));
	FAILED_RETURN_HR(spExtensionManager->GetExtenderNames(bstrCATID, spThis, pvExtenderNames));

	return hr;
}

template<typename TInterfaces>
STDMETHODIMP CVsRootCE<TInterfaces>::get_Extender(BSTR bstrExtenderName, IDispatch ** ppExtender) 
{ 
	VALIDATE_ARGUMENT(bstrExtenderName);
	VALIDATE_AND_CLEAR(ppExtender);

	HRESULT hr(S_OK);
	CComBSTR bstrCATID;
	CComPtr<ObjectExtenders> spExtensionManager;
	CComPtr<IDispatch> spThis;

	FAILED_RETURN_HR(get_ExtenderCATID(&bstrCATID));
	FAILED_RETURN(E_OUTOFMEMORY, bstrCATID);
	FAILED_RETURN_HR(CVCUtilityLibrary::QueryService(&spExtensionManager));
	FAILED_RETURN_HR(GetUnknown()->QueryInterface(__uuidof(IDispatch), reinterpret_cast<void **>(&spThis)));
	FAILED_RETURN(S_FALSE, spExtensionManager->GetExtender(bstrCATID, bstrExtenderName, spThis, ppExtender));

	return hr; 
}

template<typename TInterfaces>
STDMETHODIMP CVsRootCE<TInterfaces>::get_InfoLocation(vsCMInfoLocation *pInfoLocation) 
{ 
	VALIDATE_AND_CLEAR(pInfoLocation);
	*pInfoLocation = vsCMInfoLocationNone;
	return S_OK;
}

template<typename TInterfaces>
STDMETHODIMP CVsRootCE<TInterfaces>::get_IsCodeType(VARIANT_BOOL * pbIsCodeType) 
{ 
	VALIDATE_AND_CLEAR(pbIsCodeType);
	*pbIsCodeType = VARIANT_FALSE;
	return S_OK;
}
