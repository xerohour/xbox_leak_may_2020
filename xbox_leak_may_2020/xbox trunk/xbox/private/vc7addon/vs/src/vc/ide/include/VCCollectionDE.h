#pragma once

#include "Resource.h"

// VCCollectionDE.h
//


//
// C++ collection implementation for a collection of any IDispatch *
//

enum vcDEPopulate
{
	vcDEPopulateInvalid = -1,
	vcDEPopulatePopulate = 0,
	vcDEPopulateQuery = 1
};


template <class T>
class _CopyVariantInterface
{
public :

	static HRESULT copy(VARIANT * pvDestination, T ** ppIUnknown)
	{
		pvDestination->vt = VT_DISPATCH;
		(*ppIUnknown)->QueryInterface(__uuidof(IDispatch), 
			reinterpret_cast<void **>(&pvDestination->pdispVal));
		return S_OK;
	}
	
	static void init(T ** ppIUnknown) 
	{ 
	}

	static void destroy(VARIANT * pvDestroy) 
	{ 
		VariantClear(pvDestroy); 
	}
};


template<typename TInterfaces, typename Element>
class ATL_NO_VTABLE CVCCollectionDE : public CVCRootDE<TInterfaces>
{
public :

	typedef CSimpleArray<CComPtr<Element> > _ArrayType;

private :

	_ArrayType m_vectorElements;

protected :

	CComPtr<VCDesignElementI> m_spParent;

	CVCCollectionDE(void)
	{
	}

	~CVCCollectionDE(void)
	{
		RemoveAll();
	}

public :

	DECLARE_NOT_AGGREGATABLE(CVCCollectionDE)
	DECLARE_VCDESIGNELEMENT(0)

	BEGIN_COM_MAP(CVCCollectionDE)
		COM_INTERFACE_ENTRY(VCDesignElements)
		COM_INTERFACE_ENTRY(VCDesignElementsI)
		COM_INTERFACE_ENTRY_CHAIN(CVCRootDE<TInterfaces>)
	END_COM_MAP()

	CComPtr<VCDesignElementI> & GetParent(void) 
	{ 
		return m_spParent; 
	}

	_ArrayType & GetElements(void) 
	{ 
		return m_vectorElements; 
	}

	// Ensures that the collection has been populated 
	//

	virtual HRESULT Populate(
		const vcDEPopulate vcdePopulate = vcDEPopulatePopulate)
	{
		// Default is to assume the collection has been populated
		//

		if (m_vectorElements.GetSize())
		{
			return S_OK;
		}
		else
		{
			// Populated, but empty
			//

			return S_FALSE;
		}
	}

	STDMETHOD(get_Parent)(IDispatch ** ppParent)
	{
		VALIDATE_WRITE_POINTER_HR(ppParent);

		HRESULT hr(S_OK);
		FAILED_RETURN_HR(m_spParent.QueryInterface(ppParent));
		return hr;
	}

	STDMETHOD(get_Descendants)(IDispatch ** ppDescendants)
	{
		VALIDATE_WRITE_POINTER_HR(ppDescendants);

		HRESULT hr(S_OK);
		FAILED_RETURN_HR(QueryInterface(__uuidof(IDispatch), 
			reinterpret_cast<void **>(ppDescendants)));
		return hr;
	}

	STDMETHOD(put_Parent)(IDispatch * pParent)
	{
		VALIDATE_READ_POINTER_HR(pParent);
		CHECK_STATE_RETURN(!m_spParent);

		return pParent->QueryInterface(__uuidof(VCDesignElementI), 
			reinterpret_cast<void **>(&m_spParent));
	}

	// REVIEW : Perhaps we do not need to make a copy of elements for _NewEnum
	//

	STDMETHOD(_NewEnum)(IUnknown ** ppEnum)
	{
		VALIDATE_WRITE_POINTER_HR(ppEnum);

		HRESULT hr(S_OK);
		FAILED_RETURN_HR(Populate());

		// Create the Enumerator object.
		//

		typedef CComEnum<IEnumVARIANT, &__uuidof(IEnumVARIANT), 
			VARIANT, _Copy<VARIANT> > CEnumType;
		CComObject<CEnumType> (*pEnumType)(NULL);
		FAILED_RETURN_HR(CComObject<CEnumType>::CreateInstance(&pEnumType));

		UINT cElements(m_vectorElements.GetSize());
		VARIANT (*rgvElements)(NULL);

		// Allocate one extra to handle the zero elements case
		//

		FAILED_RETURN(E_OUTOFMEMORY, rgvElements = new VARIANT[cElements + 1]);

		for (UINT iElement = 0; iElement < cElements; iElement++)
		{
			VariantInit(&rgvElements[iElement]);
			rgvElements[iElement].vt = VT_DISPATCH;
			rgvElements[iElement].pdispVal = NULL;
			m_vectorElements[iElement].QueryInterface(&rgvElements[iElement].pdispVal);
		}

		if (FAILED(hr = pEnumType->Init(&rgvElements[0], &rgvElements[cElements], 
			GetUnknown(), AtlFlagTakeOwnership)))
		{
			delete pEnumType;
			return hr;
		}

		FAILED_RETURN_HR(pEnumType->QueryInterface(__uuidof(IUnknown), 
			reinterpret_cast<void **>(ppEnum)));

		return hr;
	}

	STDMETHOD(Item)(VARIANT vItem, IDispatch ** ppItem)
	{
		VALIDATE_WRITE_POINTER_HR(ppItem);

		HRESULT hr(S_OK);

		switch (vItem.vt)
		{
		case VT_EMPTY :
			return E_INVALIDARG;
		case VT_BSTR :
			if (S_OK != (hr = Find(vItem.bstrVal, ppItem)))
			{
				return E_INVALIDARG;
			}
			else
			{
				return hr;
			}
		case VT_BSTR | VT_BYREF :
			if (S_OK != (hr = Find(*vItem.pbstrVal, ppItem)))
			{
				return E_INVALIDARG;
			}
			else
			{
				return hr;
			}
		case VT_VARIANT | VT_BYREF :
			return Item(*vItem.pvarVal, ppItem);
		}

		CComVariant vLongIndex;
		FAILED_RETURN_HR(VariantChangeType(&vLongIndex, &vItem, 0, VT_I4));
		FAILED_RETURN_HR(Populate());

		// Adjust for 1 based collection
		//

		vLongIndex.lVal--;

		if (vLongIndex.lVal < 0 || vLongIndex.lVal >= m_vectorElements.GetSize())
		{
			return E_INVALIDARG;
		}

		return m_vectorElements[vLongIndex.lVal].QueryInterface(ppItem);
	}

	STDMETHODIMP Item(VARIANT index, Element ** lppcReturn)
	{
		HRESULT hr(S_OK);
		CComPtr<IDispatch> spItem;
		FAILED_RETURN_HR(Item(index, &spItem));
		return spItem.QueryInterface(lppcReturn);
	}

	STDMETHOD(get_Count)(long * plCount)
	{
		VALIDATE_WRITE_POINTER_HR(plCount);

		HRESULT hr(S_OK);
		FAILED_RETURN_HR(Populate());

		*plCount = m_vectorElements.GetSize();

		return hr;
	}

	STDMETHOD(Find)(BSTR bstrName, IDispatch **	ppFound)
	{
		VALIDATE_AND_CLEAR(ppFound);

		HRESULT hr(S_OK);
		FAILED_RETURN_HR(Populate());

		UINT cElements(m_vectorElements.GetSize());

		for (UINT iElement = 0; iElement < cElements; iElement++)
		{
			CComBSTR bstrElementName;
			CComPtr<VCDesignElement> spElement;

            FAILED_ASSERT_RETURN_HR(m_vectorElements[iElement].QueryInterface(&spElement));
            bool bMatch(false);

    		VARIANT_BOOL bIsCaseSensitive(VARIANT_FALSE);
			FAILED_RETURN_HR(spElement->get_Name(&bstrElementName));
			FAILED_RETURN_HR(spElement->get_IsCaseSensitive(&bIsCaseSensitive));

			if (!bstrElementName.Length())
			{  
				bMatch = (!bstrName || !bstrName[0]);
			}
			else
			{
				if (VARIANT_TRUE == bIsCaseSensitive)
				{
					bMatch = !wcscmp(bstrElementName, bstrName);
				}
				else
				{
					bMatch = !_wcsicmp(bstrElementName, bstrName);
				}
			}

			if (bMatch)
			{
				FAILED_ASSERT_RETURN_HR(spElement.QueryInterface(ppFound));

				return S_OK;
			}
		}

		// Name was not found in the collection
		//

		return S_FALSE;
	}

	STDMETHOD(IsEmpty)(BOOL * pbIsEmpty)
	{
		VALIDATE_WRITE_POINTER_HR(pbIsEmpty);

		HRESULT hr(S_OK);

		hr = Populate(vcDEPopulateQuery); 

		*pbIsEmpty = S_FALSE == hr;

		return hr;
	}

	STDMETHOD(AddItem)(IDispatch * pAdd)
	{
		VALIDATE_READ_POINTER_HR(pAdd);

		HRESULT hr(S_OK);
		CComPtr<Element> spAdd;
		FAILED_RETURN_HR(pAdd->QueryInterface(__uuidof(Element),
			reinterpret_cast<void **>(&spAdd)));

		m_vectorElements.Add(spAdd);

		return hr;
	}

	STDMETHOD(RemoveIndex)(LONG iRemove)
	{
		VALIDATE_RANGE_HR(iRemove, 1, m_vectorElements.GetSize());

		m_vectorElements.RemoveAt(iRemove - 1);
		return S_OK;
	}

	STDMETHOD(RemoveItem)(IDispatch * pRemove)
	{
		VALIDATE_READ_POINTER_HR(pRemove);

		HRESULT hr(S_OK);
		CComPtr<Element> spRemove;
		FAILED_RETURN_HR(pRemove->QueryInterface(__uuidof(Element), 
			reinterpret_cast<void **>(&spRemove)));

		m_vectorElements.Remove(spRemove);
		return hr;
	}

	STDMETHOD(RemoveAll)(void)
	{
		m_vectorElements.RemoveAll();
		return S_OK;
	}
};


// Passthroughs
//

class ATL_NO_VTABLE IVCCollectionDEPT : 
	public VCDesignElement,
	public CVCIDispatchImplDE<VCDesignElements>,
	public VCDesignElementsI
{ 
};

// CVCDesignElements
//

typedef CVCCollectionDE<IVCCollectionDEPT, VCDesignElement> CVCDesignElements;
