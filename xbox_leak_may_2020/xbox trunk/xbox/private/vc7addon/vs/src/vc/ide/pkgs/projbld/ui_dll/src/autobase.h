#pragma once

template <class TClass, class PrimaryIface, const LPOLESTR TszGuid> 
class CAutoBase
{
public:
	HRESULT DoGetProperties(TClass* pThis, PrimaryIface* pObject, Properties** ppObject)
	{
		CHECK_POINTER_VALID(ppObject);

		// Obtain the Extensibility Service
		CComPtr<IVsExtensibility> spExtService; 
		HRESULT hr = ExternalQueryService(SID_SVsExtensibility, IID_IVsExtensibility, (void **)&spExtService);
		RETURN_ON_FAIL(hr);
		CComQIPtr<IDispatch> spDispatch = pObject;
		RETURN_INVALID_ON_NULL(spDispatch);
		// get the property object
		return spExtService->get_Properties(pThis, spDispatch, (Properties **)ppObject);
	}
	HRESULT DoGetObject(PrimaryIface* pObject, IDispatch** ppProjectModel, long idErr)
	{
		CHECK_ZOMBIE(pObject, idErr);

		CComQIPtr<IDispatch> spDispObject = pObject;
		return spDispObject.CopyTo(ppProjectModel);
	}
	HRESULT DoGetExtender(TClass* pThis, BSTR bstrExtenderName, IDispatch** ppExtender)
	{
		CHECK_POINTER_VALID(ppExtender);
		*ppExtender = NULL;

		CComBSTR bstrCATID(TszGuid);
		RETURN_ON_NULL2(bstrCATID, S_OK);

		// validate params
		RETURN_ON_NULL2(bstrExtenderName, S_FALSE);

		// get internal ExtensionManager svc
		CComPtr<ObjectExtenders> spExtMgr;
		HRESULT hr = ExternalQueryService(SID_SExtensionManager, IID_ObjectExtenders, (void **)&spExtMgr);
		RETURN_ON_FAIL_OR_NULL(hr, spExtMgr);

		// call GetExtension on svc
		return spExtMgr->GetExtender(bstrCATID, bstrExtenderName, (LPDISPATCH)pThis, ppExtender);
	}
	HRESULT DoGetExtenderNames(TClass* pThis, VARIANT *pvarExtenderNames)
	{
		// validate params
		CHECK_POINTER_VALID(pvarExtenderNames);
		VariantInit(pvarExtenderNames);

		CComBSTR bstrCATID(TszGuid);
		if (!bstrCATID)
			return S_OK;

		// get internal ExtensionManager svc
		CComPtr<ObjectExtenders> spExtMgr;
		HRESULT hr = ExternalQueryService(SID_SExtensionManager, IID_ObjectExtenders, (void **)&spExtMgr);
		RETURN_ON_FAIL_OR_NULL(hr, spExtMgr);

		// call GetExtensionNames on svc
		return spExtMgr->GetExtenderNames(bstrCATID, (LPDISPATCH) pThis, pvarExtenderNames);
	}
	HRESULT DoGetExtenderCATID(BSTR *pbstrRetval)
	{
		// validate params
		CHECK_POINTER_VALID(pbstrRetval);
		*pbstrRetval = NULL;

		// Convert string version of GUID into BSTR string
		*pbstrRetval = ::SysAllocString((OLECHAR *) TszGuid);
		RETURN_ON_NULL2(*pbstrRetval, S_FALSE);
		return S_OK;
	}
};

template <class TAuto>
class CAutoEnumerator :
	public IEnumVARIANT,
	public CComObjectRoot
{
public:
BEGIN_COM_MAP(CAutoEnumerator)
	COM_INTERFACE_ENTRY(IEnumVARIANT)
END_COM_MAP()

	static HRESULT CreateInstance(CAutoEnumerator **ppCollection, TAuto* pOwner, IDispatch* pAutoProject, IDispatch* pAutoParent,
		IDispatch *pDispCollection)
	{
		CHECK_POINTER_NULL(ppCollection);
		CAutoEnumerator *pVar;
		CComObject<CAutoEnumerator> *pObj;
		HRESULT hr = CComObject<CAutoEnumerator>::CreateInstance(&pObj);
		if (SUCCEEDED(hr))
		{
			pVar = pObj;
			pVar->AddRef();
			*ppCollection = pVar;
			hr = pVar->Initialize(pOwner, pAutoProject, pAutoParent, pDispCollection);
		}
		return hr;
	}
	CAutoEnumerator() 
	{
		m_pAuto = NULL;
	}
	~CAutoEnumerator()
	{
	}

protected:
	HRESULT Initialize(TAuto* pOwner, IDispatch* pAutoParent, IDispatch* pAutoProject, IDispatch* pDispCollection)
	{
		CHECK_READ_POINTER_NULL(pOwner);
		CHECK_READ_POINTER_NULL(pDispCollection);

		m_spOwner = pOwner; // want a ref count...
		m_pAuto = pOwner;
		m_spAutoParent = pAutoParent;
		m_spAutoProject = pAutoProject;

		CComQIPtr<IVCCollection> spCollection = pDispCollection;
		VSASSERT(spCollection != NULL, "Bad initialization of CAutoEnumerator");
		RETURN_ON_NULL2(spCollection, E_UNEXPECTED);
		IUnknown* pUnk = NULL;
		HRESULT hr = spCollection->_NewEnum(&pUnk);
		VSASSERT(SUCCEEDED(hr) && pUnk != NULL, "Collection can't be enumerated!");
		RETURN_ON_FAIL(hr);
		if (pUnk == NULL)
			return DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
		m_spEnumerator = pUnk;
		pUnk->Release();
		return S_OK;
	}

protected:
	CComQIPtr<IEnumVARIANT> m_spEnumerator;
	CComQIPtr<IDispatch> m_spOwner;		// this is a CAutoItems or CAutoProjects collection
	CComPtr<IDispatch> m_spAutoParent;
	CComPtr<IDispatch> m_spAutoProject;
	TAuto* m_pAuto;

// IEnumVARIANT
public:
	STDMETHOD(Next)(ULONG celt, VARIANT *rgVar, ULONG *pCeltFetched)
	{
		CHECK_ZOMBIE(m_spEnumerator, IDS_ERR_ENUMERATOR_ZOMBIE);

		CHECK_POINTER_VALID(rgVar);
		::VariantClear(rgVar);

		ULONG celtFetchedLocal;
		HRESULT hr = m_spEnumerator->Next(celt, rgVar, &celtFetchedLocal);
		if (pCeltFetched)
			*pCeltFetched = celtFetchedLocal;
		RETURN_ON_FAIL(hr);

		for (ULONG idx = 0; idx < celtFetchedLocal; idx++)
		{
			IDispatch* pDispItem = rgVar[idx].pdispVal;
			if (pDispItem != NULL)
			{
				CComPtr<IDispatch> spDispObject;
				hr = TAuto::GetItem(&spDispObject, m_spOwner, m_spAutoParent, m_spAutoProject, pDispItem);
				rgVar[idx].pdispVal = spDispObject.Detach();
				// have to set vt to dispatch (enumerator->Next() returns us
				// unknowns and automation needs disps)
				rgVar[idx].vt = VT_DISPATCH;
			}
		}
		
		return hr;
	}
	STDMETHOD(Skip)(ULONG celt)
	{
		CHECK_ZOMBIE(m_spEnumerator, IDS_ERR_ENUMERATOR_ZOMBIE);
		return m_spEnumerator->Skip(celt);
	}
	STDMETHOD(Reset)(void)
	{
		CHECK_ZOMBIE(m_spEnumerator, IDS_ERR_ENUMERATOR_ZOMBIE);
		return m_spEnumerator->Reset();
	}
	STDMETHOD(Clone)(IEnumVARIANT __RPC_FAR *__RPC_FAR *ppEnum)
	{
		CHECK_ZOMBIE(m_pAuto, IDS_ERR_ENUMERATOR_ZOMBIE);
		return m_pAuto->_NewEnum((IUnknown **)ppEnum);
	}
};
