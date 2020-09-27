// Collection.h: Definition of the CCollection class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLLECTION_H__A54AAE9C_30C2_11D3_87BF_A04A4CC10000__INCLUDED_)
#define AFX_COLLECTION_H__A54AAE9C_30C2_11D3_87BF_A04A4CC10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comlist.h"
#include "VCProjectEngine.h"


/////////////////////////////////////////////////////////////////////////////
// CCollection
template <class CollType>
class CCollection : 
	public IDispatchImpl<IVCCollection, &__uuidof(IVCCollection), &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public IEnumVARIANT,
	public CComObjectRoot
{
public:
	typedef CComDynamicListTyped< CollType> CDynList;

	static HRESULT CreateInstance(IVCCollection **ppCollection, CDynList *prgDisp, IVCCollectionProvider* pCollectionProvider = NULL, 
		DWORD dwCookie = 0)
	{
		HRESULT hr;
		CCollection *pVar;
		CComObject< CCollection<CollType> > *pObj;
		hr = CComObject< CCollection<CollType> >::CreateInstance(&pObj);
		if (SUCCEEDED(hr))
		{
			pVar = pObj;
			pVar->AddRef();
			*ppCollection = pVar;
			hr = pVar->Initialize(prgDisp, pCollectionProvider, dwCookie);
		}
		return hr;
	}
	STDMETHOD(Initialize)(CDynList *prgDisp, IVCCollectionProvider* pCollectionProvider, DWORD dwCookie);
	CCollection() 
	{
	}
	~CCollection()
	{
	}

BEGIN_COM_MAP(CCollection<CollType>)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCCollection)
	COM_INTERFACE_ENTRY(IEnumVARIANT)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CCollection) 

DECLARE_NO_REGISTRY()

// IVCCollection
public:
	STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);
	STDMETHOD(_NewEnum)(/*[out, retval]*/ IUnknown* *pVal);
	STDMETHOD(Item)(VARIANT Index, /*[out]*/ IDispatch **pItem);
	STDMETHOD(get_VCProjectEngine)(/*[out, retval*/ IDispatch** ppProjectEngine);

// IEnumVARIANT
public:
	STDMETHOD(Next)( ULONG celt, VARIANT *rgVar, ULONG *pCeltFetched);
	STDMETHOD(Skip)(/* [in] */ ULONG celt);
	STDMETHOD(Reset)( void);
	STDMETHOD(Clone)(/* [out] */ IEnumVARIANT __RPC_FAR *__RPC_FAR *ppEnum);

// data
protected:
	CDynList *m_ppDisp;
	int	m_lIndexCurr;
	CComPtr<IVCCollectionProvider> m_pCollectionProvider;
	DWORD m_dwCookie;
};

template <class CollType>
STDMETHODIMP CCollection<CollType>::Initialize(CDynList *prgDisp, IVCCollectionProvider* pCollectionProvider, DWORD dwCookie)
{
	m_ppDisp = prgDisp;
	m_lIndexCurr = 0;
	m_pCollectionProvider = pCollectionProvider;
	m_dwCookie = dwCookie;
	return S_OK;
}

template <class CollType>
STDMETHODIMP CCollection<CollType>::Item(VARIANT Index, IDispatch **ppDispItem)
{
	CHECK_ZOMBIE(m_ppDisp, IDS_ERR_COLLECTION_ZOMBIE);
	if (m_pCollectionProvider)
		m_pCollectionProvider->UpdateItemsCollection(m_dwCookie);

	CComVariant varIndex;
	varIndex= Index;
	
	CHECK_POINTER_VALID(ppDispItem);
	*ppDispItem = NULL;

	if ((varIndex.vt == (VT_VARIANT | VT_BYREF)) && varIndex.pvarVal)
	{
		varIndex = *varIndex.pvarVal;
	}
	if( varIndex.vt == VT_I4 || varIndex.vt == VT_I2 )
	{
		CollType *pItem = NULL;
		varIndex.ChangeType(VT_I4);

		if (m_ppDisp->HasAnItemBeenRemoved())
		{
			long nIndexTarget = varIndex.lVal;
			long nIndexT = 0, nIndexItem = 0;
			long nMaxSize = m_ppDisp->GetSize();
			if (nIndexItem < m_ppDisp->GetSize())
			{
				do
				{
					pItem = m_ppDisp->GetAt(nIndexItem);
					if (pItem)
						nIndexT++;
					nIndexItem++;
				}
				while (nIndexItem < nMaxSize && nIndexT != nIndexTarget);
			}

			if (pItem && nIndexT == nIndexTarget)
			{
				pItem->QueryInterface(_uuidof(IDispatch), (void**)ppDispItem);
				return S_OK;
			}
			else
			{
				*ppDispItem = NULL;
				return S_FALSE;
			}
		}
		else
		{
			if (varIndex.lVal < 1)
			{
				RETURN_INVALID();
			}
			else if (varIndex.lVal <= m_ppDisp->GetSize())
				pItem = m_ppDisp->GetAt(varIndex.lVal - 1);
			else
				pItem = NULL;

			if (pItem)
			{
				pItem->QueryInterface(_uuidof(IDispatch), (void**)ppDispItem);
				return S_OK;
			}
			else
			{
				*ppDispItem = NULL;
				return S_FALSE;
			}
		}
	}
	if( varIndex.vt == VT_BSTR )
	{
		CComQIPtr<CollType> pItem;
		CComBSTR bstrName;
		long nIndex;
		long lcItems;
		VARIANT_BOOL bMatch;
		lcItems = m_ppDisp->GetSize();
		for(nIndex = 0; nIndex < lcItems; nIndex++ )
		{
			pItem = m_ppDisp->GetAt(nIndex);
			if (pItem)
			{
				pItem->MatchName(varIndex.bstrVal, VARIANT_FALSE /* any match*/, &bMatch);
				if (bMatch)
				{
					pItem.QueryInterface(ppDispItem);
					return S_OK;
				}
			}
		}
	}
	if (varIndex.vt == VT_DISPATCH || varIndex.vt == VT_UNKNOWN)
	{
		CComQIPtr<CollType> pInItem = varIndex.pdispVal;
		long lcItems = m_ppDisp->GetSize();
		for (long nIndex = 0; nIndex < lcItems; nIndex++)
		{
			CComQIPtr<CollType> pThisItem = m_ppDisp->GetAt(nIndex);
			if (pThisItem == NULL)
				continue;

			if (pThisItem == pInItem)
			{
				pThisItem.QueryInterface(ppDispItem);
				return S_OK;
			}
		}
	}
	return S_OK;
}

template <class CollType>
STDMETHODIMP CCollection<CollType>::_NewEnum(IUnknown **ppenum)
{
	return QueryInterface(__uuidof(IEnumVARIANT), (LPVOID *)ppenum);
}

template <class CollType>
STDMETHODIMP CCollection<CollType>::get_Count(long *pVal)
{
	CHECK_POINTER_VALID(pVal);

	if (m_pCollectionProvider)
		m_pCollectionProvider->UpdateItemsCollection(m_dwCookie);

	*pVal =  m_ppDisp->GetCount();
	return S_OK;
}

template <class CollType>
STDMETHODIMP CCollection<CollType>::get_VCProjectEngine(IDispatch** ppProjectEngine)
{
	CHECK_POINTER_VALID(ppProjectEngine);
	VSASSERT(g_pProjectEngine != NULL, "Project Engine not initialized!!!");

	CComQIPtr<IDispatch> spDispEngine = g_pProjectEngine;
	return spDispEngine.CopyTo(ppProjectEngine);
}	

template <class CollType>
STDMETHODIMP CCollection<CollType>::Next( 
        /* [in] */ ULONG celt,
        /* [length_is][size_is][out] */ VARIANT *rgVar,
        /* [out] */ ULONG *pceltFetched)
{
	if (m_pCollectionProvider)
		m_pCollectionProvider->UpdateItemsCollection(m_dwCookie);

	HRESULT hr = S_OK;
	if (pceltFetched != NULL)
		*pceltFetched = 0;
	if (celt == 0)
		RETURN_INVALID();
	CHECK_POINTER_VALID(rgVar);
	if (celt != 1)
		CHECK_POINTER_VALID(pceltFetched);
	CHECK_ZOMBIE(m_ppDisp, IDS_ERR_COLLECTION_ZOMBIE);
	ULONG lMax = m_ppDisp->GetCount();
	ULONG nRem = (ULONG)(lMax - m_lIndexCurr);
	if (lMax < (ULONG)m_lIndexCurr)
	{
		nRem = 0;
		hr = S_FALSE;
	}
	else if (nRem < celt)
		hr = S_FALSE;
	ULONG nMin = min(celt, nRem);
	ULONG idx = 0;
	CollType *pItem = NULL;

	if (m_ppDisp->HasAnItemBeenRemoved())
	{
		while (nMin > 0)
		{
			int nIndexT = 0, nIndexItem = 0;
			// go through the list again for the correct index (just in case a delete/add happened)
			if (nIndexItem < m_ppDisp->GetSize())
			{
				do
				{
					pItem = (CollType*)m_ppDisp->GetAt(nIndexItem);
					if (pItem)
					{
						nIndexT++;
					}
					nIndexItem++;
				}
				while (nIndexItem < m_ppDisp->GetSize()  && nIndexT != m_lIndexCurr + 1);
			}
			if (pItem && nIndexT == m_lIndexCurr + 1) // plus one is because we come out of the loop after incrementing
			{
				CComPtr<IUnknown> pUnk;
				pItem->QueryInterface(_uuidof(IUnknown),(void**)&pUnk);
				if( pUnk )
				{
					CComVariant varRes;
					varRes = pUnk;
					hr = varRes.Detach(&rgVar[idx]);
					m_lIndexCurr++;
					idx++;
					if (pceltFetched != NULL)
						(*pceltFetched)++;
				}
			}
			nMin--;
		}
	}
	else
	{
		while (nMin--)
		{
			pItem = m_ppDisp->GetAt(m_lIndexCurr);
			CComPtr<IUnknown> pUnk;
			if (pItem)
				pItem->QueryInterface(_uuidof(IUnknown),(void**)&pUnk);

			if (pUnk)
			{
				CComVariant vItem(pUnk);
				vItem.Detach(&rgVar[idx++]);
				m_lIndexCurr++;
			}
		}

		if (pceltFetched)
		{
			*pceltFetched = idx;
		}
	}

	return hr;
}
        
template <class CollType>
STDMETHODIMP CCollection<CollType>::Skip(/* [in] */ ULONG celt)
{
	if (m_pCollectionProvider)
		m_pCollectionProvider->UpdateItemsCollection(m_dwCookie);

	HRESULT hr = S_OK;
	m_lIndexCurr += celt;
	return hr;
}

        
template <class CollType>
STDMETHODIMP CCollection<CollType>::Reset( void)
{
	HRESULT hr = S_OK;
	m_lIndexCurr = 0;
	return hr;
}

        
template <class CollType>
STDMETHODIMP CCollection<CollType>::Clone(/* [out] */ IEnumVARIANT  **ppEnum)
{
	HRESULT hr = S_OK;
	CComPtr<IVCCollection> pCollection;

	hr = CreateInstance(&pCollection, m_ppDisp, m_pCollectionProvider, m_dwCookie);
	if (SUCCEEDED(hr) && pCollection)
	{
		hr = pCollection->_NewEnum(reinterpret_cast<IUnknown**>(ppEnum));
	}
	return hr;
}


#endif // !defined(AFX_COLLECTION_H__A54AAE9C_30C2_11D3_87BF_A04A4CC10000__INCLUDED_)
