// OutsEnum.cpp : Implementation of CDevbldApp and DLL registration.

#include "stdafx.h"
#include <vssolutn.h>
#include <BldMarshal.h>
#include "OutsEnum.h"

/////////////////////////////////////////////////////////////////////////////
//

STDMETHODIMP COutsEnum::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IVsEnumOutputs,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

HRESULT COutsEnum::CreateInstance(IVsEnumOutputs** ppOutsEnum, VCConfiguration* pConfig, CVCPtrList *pList)
{
	CComObject<COutsEnum> *pOutsEnumObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<COutsEnum>::CreateInstance(&pOutsEnumObj);
	if (SUCCEEDED(hr))
	{
		((IVsEnumOutputs *)pOutsEnumObj)->AddRef();
		pOutsEnumObj->Initialize(pConfig, pList);
	}
	*ppOutsEnum = (IVsEnumOutputs *)pOutsEnumObj;
	return hr;

}


void COutsEnum::Initialize(VCConfiguration* pConfig, CVCPtrList *pList)
{
	m_pConfig = pConfig;
	VCPOSITION pos = pList->GetHeadPosition();
	while (pos)
		m_ListOuts.AddTail(pList->GetNext(pos));
}


STDMETHODIMP COutsEnum::Reset()
{
	HRESULT hr = S_OK;
	m_iOut = 0;
	return(hr);
}

STDMETHODIMP COutsEnum::Next(/*[in]*/ ULONG cElements, /*[in, out, size_is(cElements)]*/ IVsOutput *rgpIVsOutput[], /*[out]*/ ULONG *pcElementsFetched)
{
	HRESULT hr = S_FALSE;
	UINT i = 0;
	UINT iMax;
	VCPOSITION pos;

	iMax = (UINT) m_ListOuts.GetCount();
	rgpIVsOutput[i] = (IVsOutput *)NULL;
	if (m_iOut < iMax)
	{
		pos = m_ListOuts.FindIndex(m_iOut);
		while (pos && cElements > 0)
		{
			CBldMarshaller<IVsOutput> marshalVsOutput;
			marshalVsOutput.Init((IVsOutput *)m_ListOuts.GetNext(pos));
			CComPtr<IVsOutput> spOutput;
			marshalVsOutput.UnmarshalPtr(&spOutput);
			rgpIVsOutput[i] = spOutput.Detach();
			i++;
			cElements--;
		}
		m_iOut += i;
		hr = i > 0 ? S_OK : S_FALSE;
	}
	if (pcElementsFetched)
	{
		*pcElementsFetched = i;
	}
	return(hr);
}

STDMETHODIMP COutsEnum::Skip(/*[in]*/ ULONG cElements)
{
	HRESULT hr = S_OK;
	UINT iMax;

	iMax = (UINT) m_ListOuts.GetCount();
	if (iMax > m_iOut + cElements)
		m_iOut += cElements;
	else
		m_iOut = iMax - 1;
	return(hr);
}

STDMETHODIMP COutsEnum::Clone(/*[out]*/ IVsEnumOutputs **ppIVsEnumOutputs)
{
	RETURN_INVALID_ON_NULL(ppIVsEnumOutputs);
	*ppIVsEnumOutputs = NULL;

	CComPtr<IVsEnumOutputs> spCfgEnum;
	HRESULT hr = COutsEnum::CreateInstance(&spCfgEnum, m_pConfig, &m_ListOuts);
	if (SUCCEEDED(hr))
	{
		CBldMarshaller<IVsEnumOutputs> marshalEnumOutputs;
		hr = marshalEnumOutputs.Init(spCfgEnum);
		if (SUCCEEDED(hr))
			hr = marshalEnumOutputs.UnmarshalPtr(ppIVsEnumOutputs);
	}

	return(hr);
}

