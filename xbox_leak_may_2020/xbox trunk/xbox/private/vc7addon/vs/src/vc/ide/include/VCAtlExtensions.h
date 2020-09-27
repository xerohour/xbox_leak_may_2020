#pragma once

#include <VCAssert.h>

template<typename T, typename Element>
class CComCollectionIterator
{
private :

	CComPtr<T> m_spT;

public :

	CComCollectionIterator(T * pT) : m_spT(pT)
	{
	}

	CComCollectionIterator(const CComPtr<T> & pT) : m_spT(pT)
	{
	}

	UINT Count(void)
	{
		FAILED_THROW(E_UNEXPECTED, m_spT);

		HRESULT hr(S_OK);
		LONG lCount(0);

		FAILED_THROW_HR(m_spT->get_Count(&lCount));

		return lCount;
	}

	Element operator [] (UINT iIndex)
	{
		FAILED_THROW(E_UNEXPECTED, m_spT);

		CComVariant vIndex;

		vIndex.vt = VT_I4;
		vIndex.lVal = iIndex + 1; // iIndex is zero based

		HRESULT hr(S_OK); 

		Element aElement(Element());

		FAILED_THROW_HR(m_spT->Item(vIndex, &aElement));

		return aElement;
	}
};

template 
<
	typename T,
	typename ISecondaryA = IUnknown,
	typename ISecondaryB = IUnknown,
	typename ISecondaryC = IUnknown,
	typename ISecondaryD = IUnknown,
	typename ISecondaryE = IUnknown,
	typename ISecondaryF = IUnknown,
	typename ISecondaryG = IUnknown,
	typename ISecondaryH = IUnknown,
	typename ISecondaryI = IUnknown
>
class CComMQIPtr : public CComQIPtr<T>
{
private :

	CComPtr<ISecondaryA> m_spSecondaryA;
	CComPtr<ISecondaryB> m_spSecondaryB;
	CComPtr<ISecondaryC> m_spSecondaryC;
	CComPtr<ISecondaryD> m_spSecondaryD;
	CComPtr<ISecondaryE> m_spSecondaryE;
	CComPtr<ISecondaryF> m_spSecondaryF;
	CComPtr<ISecondaryG> m_spSecondaryG;
	CComPtr<ISecondaryH> m_spSecondaryH;
	CComPtr<ISecondaryI> m_spSecondaryI;

	HRESULT QueryInterfaceSecondaryAll(void)
	{
		FAILED_RETURN(E_NOINTERFACE, p);

		if (m_spSecondaryA)
		{
			return S_OK;
		}

		HRESULT hr(S_OK);

		do
		{
			FAILED_ASSERT_BREAK_HR(QueryInterface(&m_spSecondaryA));
			FAILED_ASSERT_BREAK_HR(QueryInterface(&m_spSecondaryB));
			FAILED_ASSERT_BREAK_HR(QueryInterface(&m_spSecondaryC));
			FAILED_ASSERT_BREAK_HR(QueryInterface(&m_spSecondaryD));
			FAILED_ASSERT_BREAK_HR(QueryInterface(&m_spSecondaryE));
			FAILED_ASSERT_BREAK_HR(QueryInterface(&m_spSecondaryF));
			FAILED_ASSERT_BREAK_HR(QueryInterface(&m_spSecondaryG));
			FAILED_ASSERT_BREAK_HR(QueryInterface(&m_spSecondaryH));
			FAILED_ASSERT_BREAK_HR(QueryInterface(&m_spSecondaryI));
		}
		while (false);

		if (FAILED(hr))
		{
			Release();
			m_spSecondaryA.Release();
			m_spSecondaryB.Release();
			m_spSecondaryC.Release();
			m_spSecondaryD.Release();
			m_spSecondaryE.Release();
			m_spSecondaryF.Release();
			m_spSecondaryG.Release();
			m_spSecondaryH.Release();
			m_spSecondaryI.Release();
		}

		return hr;
	}

protected :

	IUnknown ** GetPredefined(const IID & riid)
	{
		// If this fails, you failed to call QueryInterfaceAll()
		//

		FAILED_ASSERT_RETURN(NULL, p);
		
		if (__uuidof(ISecondaryA) == riid)
		{
			return reinterpret_cast<IUnknown **>(&m_spSecondaryA);
		}
		else if (__uuidof(ISecondaryB) == riid)
		{
			return reinterpret_cast<IUnknown **>(&m_spSecondaryB);
		}
		else if (__uuidof(ISecondaryC) == riid)
		{
			return reinterpret_cast<IUnknown **>(&m_spSecondaryC);
		}
		else if (__uuidof(ISecondaryD) == riid)
		{
			return reinterpret_cast<IUnknown **>(&m_spSecondaryD);
		}
		else if (__uuidof(ISecondaryE) == riid)
		{
			return reinterpret_cast<IUnknown **>(&m_spSecondaryE);
		}
		else if (__uuidof(ISecondaryF) == riid)
		{
			return reinterpret_cast<IUnknown **>(&m_spSecondaryF);
		}
		else if (__uuidof(ISecondaryG) == riid)
		{
			return reinterpret_cast<IUnknown **>(&m_spSecondaryG);
		}
		else if (__uuidof(ISecondaryH) == riid)
		{
			return reinterpret_cast<IUnknown **>(&m_spSecondaryH);
		}
		else if (__uuidof(ISecondaryI) == riid)
		{
			return reinterpret_cast<IUnknown **>(&m_spSecondaryI);
		}
		else
		{
			return NULL;
		}
	}

public :

	CComMQIPtr() throw()
	{
	}
	CComMQIPtr(T* lp) throw() : CComQIPtr<T>(lp)
	{
		QueryInterfaceSecondaryAll();
	}
	CComMQIPtr(const CComMQIPtr<T, ISecondaryA, ISecondaryB, ISecondaryC, ISecondaryD, ISecondaryE, ISecondaryF, ISecondaryG, ISecondaryH, ISecondaryI>& lp) throw() : CComQIPtr<T>(lp.p)
	{
		QueryInterfaceSecondaryAll();
	}
	CComMQIPtr(IUnknown* lp) throw()
	{
		if (lp != NULL)
		{
			lp->QueryInterface(__uuidof(T), (void **)&p);
			QueryInterfaceSecondaryAll();
		}
	}
	T* operator=(T* lp) throw()
	{
		AtlComPtrAssign((IUnknown**)&p, lp);
		QueryInterfaceSecondaryAll();
		return p;
	}
	T* operator=(const CComMQIPtr<T, ISecondaryA, ISecondaryB, ISecondaryC, ISecondaryD, ISecondaryE, ISecondaryF, ISecondaryG, ISecondaryH, ISecondaryI>& lp) throw()
	{
		AtlComPtrAssign((IUnknown**)&p, lp.p);
		QueryInterfaceSecondaryAll();
		return p;
	}
	T* operator=(IUnknown* lp) throw()
	{
		AtlComQIPtrAssign((IUnknown**)&p, lp, __uuidof(T));
		QueryInterfaceSecondaryAll();
		return p;
	}

	HRESULT QueryInterfaceAll(void)
	{
		return QueryInterfaceSecondaryAll();
	}

	// Predefined Accessors.  Add your own at will.  Yes, these will AV if you use them incorrectly...
	//
	// 

	CComPtr<IDispatch> & IDispatch(void)
	{
		return *reinterpret_cast<CComPtr<::IDispatch> *>(GetPredefined(__uuidof(::IDispatch)));
	}
};

#define SERVICE_ENTRY_INTERFACE(i) \
		if (InlineIsEqualGUID(guidService, __uuidof(i))) \
			return QueryService(reinterpret_cast<i **>(ppvObject));
