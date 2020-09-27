// ProjOuts.cpp : Implementation of CProjectOutput

#include "stdafx.h"
#include <vssolutn.h>
#include <path2.h>
#include "ProjOuts.h"

/////////////////////////////////////////////////////////////////////////////
//
CProjectOutput::~CProjectOutput()
{
}

HRESULT CProjectOutput::CreateInstance(IVsOutput** ppProjOut, VCConfiguration* pConfig, CPathW& path)
{
	CComObject<CProjectOutput> *pProjOutObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CProjectOutput>::CreateInstance(&pProjOutObj);
	if (SUCCEEDED(hr))
	{
		((IVsOutput *)pProjOutObj)->AddRef();
		pProjOutObj->Initialize(pConfig, path);
	}
	*ppProjOut = (IVsOutput *)pProjOutObj;
	return hr;

}

void CProjectOutput::Initialize(VCConfiguration* pConfig, CPathW& path)
{
	m_pConfig = pConfig;
	m_path = path;
}

STDMETHODIMP CProjectOutput::get_DisplayName(/*[out]*/ BSTR *pbstrDisplayName)
{
	CHECK_POINTER_NULL(pbstrDisplayName)

	CStringW strName = m_path.GetFileName();
	*pbstrDisplayName = strName.AllocSysString();
	return S_OK;
}

STDMETHODIMP CProjectOutput::get_CanonicalName(/*[out]*/ BSTR *pbstrCanonicalName)
{
	CHECK_POINTER_NULL(pbstrCanonicalName)

	CStringW strName = m_path.GetFullPath();
	*pbstrCanonicalName = strName.AllocSysString();
	return S_OK;
}


// The DeploySourceURL is the web location of the item.  For items in the
// local filesystem, the URL should begin with the eight characters:
// "file:///".  Consumers of outputs may not be able to process URLs of
// other forms, so it's very important for projects that are generating URLs
// for local items to try to use this form of URL as much as possible.
STDMETHODIMP CProjectOutput::get_DeploySourceURL(/*[out]*/ BSTR *pbstrDeploySourceURL)
{
	CHECK_POINTER_NULL(pbstrDeploySourceURL)

	CStringW strUrl = L"file:///";
	strUrl += m_path.GetFullPath();
	*pbstrDeploySourceURL = strUrl.AllocSysString();
	return S_OK;
}


STDMETHODIMP CProjectOutput::get_Type(/*[out]*/ GUID *pguidType)
{
	CHECK_POINTER_NULL(pguidType)

	*pguidType = GUID_NULL;
	return S_OK;
}


