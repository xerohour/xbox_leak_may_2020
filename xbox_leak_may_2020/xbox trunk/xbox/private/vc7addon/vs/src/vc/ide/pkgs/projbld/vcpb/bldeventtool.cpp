#include "stdafx.h"
#include "BldEventTool.h"

// static initializers
CComBSTR CVCPreBuildEventTool::s_bstrDescription = L"";
GUID CVCPreBuildEventTool::s_pPages[1];
BOOL CVCPreBuildEventTool::s_bPagesInit;
CComBSTR CVCPreBuildEventTool::s_bstrToolName = L"";
CComBSTR CVCPreBuildEventTool::s_bstrExtensions = L"";

CComBSTR CVCPreLinkEventTool::s_bstrDescription = L"";
GUID CVCPreLinkEventTool::s_pPages[1];
BOOL CVCPreLinkEventTool::s_bPagesInit;
CComBSTR CVCPreLinkEventTool::s_bstrToolName = L"";
CComBSTR CVCPreLinkEventTool::s_bstrExtensions = L"";

CComBSTR CVCPostBuildEventTool::s_bstrDescription = L"";
GUID CVCPostBuildEventTool::s_pPages[1];
BOOL CVCPostBuildEventTool::s_bPagesInit;
CComBSTR CVCPostBuildEventTool::s_bstrToolName = L"";
CComBSTR CVCPostBuildEventTool::s_bstrExtensions = L"";

HRESULT CVCPreBuildEventTool::CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool)
{
	CHECK_POINTER_NULL(ppTool);
	*ppTool = NULL;

	CComObject<CVCPreBuildEventTool> *pObj;
	HRESULT hr = CComObject<CVCPreBuildEventTool>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		if( pPropContainer )
			pObj->Initialize(pPropContainer);
		CVCPreBuildEventTool *pVar = pObj;
		pVar->AddRef();
		*ppTool = pVar;
	}
	return hr;
}

HRESULT CVCPreLinkEventTool::CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool)
{
	CHECK_POINTER_NULL(ppTool);
	*ppTool = NULL;

	CComObject<CVCPreLinkEventTool> *pObj;
	HRESULT hr = CComObject<CVCPreLinkEventTool>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		if( pPropContainer )
			pObj->Initialize(pPropContainer);
		CVCPreLinkEventTool *pVar = pObj;
		pVar->AddRef();
		*ppTool = pVar;
	}
	return hr;
}

HRESULT CVCPostBuildEventTool::CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool)
{
	CHECK_POINTER_NULL(ppTool);
	*ppTool = NULL;

	CComObject<CVCPostBuildEventTool> *pObj;
	HRESULT hr = CComObject<CVCPostBuildEventTool>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		if( pPropContainer )
			pObj->Initialize(pPropContainer);
		CVCPostBuildEventTool *pVar = pObj;
		pVar->AddRef();
		*ppTool = pVar;
	}
	return hr;
}

STDMETHODIMP CVCPreBuildEventPage::get_CommandLine(BSTR *pVal)
{
	HRESULT hr = S_OK;
	CStringW str;
	if(GetTruncatedStringForProperty(PREBLDID_CommandLine, str))
	{
		*pVal = str.AllocSysString();
		return S_OK;
	}
	else
	{
		hr = GetStrProperty(PREBLDID_CommandLine, pVal);
		if (hr == S_FALSE)
			GetDefaultValue( PREBLDID_CommandLine, pVal );
	}
	return hr;
}

STDMETHODIMP CVCPreBuildEventPage::put_CommandLine(BSTR newVal)
{
	HRESULT hr = SetStrProperty(PREBLDID_CommandLine, newVal);
	RefreshPropGrid();
	return hr;
}

STDMETHODIMP CVCPreBuildEventPage::get_Description(BSTR *pVal)
{
	HRESULT hr = GetStrProperty(PREBLDID_Description, pVal);
	if (hr == S_FALSE)
		GetDefaultValue( PREBLDID_Description, pVal );
	return hr;
}

STDMETHODIMP CVCPreBuildEventPage::put_Description(BSTR newVal)
{
	return SetStrProperty(PREBLDID_Description, newVal);
}

STDMETHODIMP CVCPreBuildEventPage::get_ExcludedFromBuild(enumBOOL* pbExcludedFromBuild)
{
	return GetEnumBoolProperty(PREBLDID_ExcludedFromBuild, pbExcludedFromBuild);
}

STDMETHODIMP CVCPreBuildEventPage::put_ExcludedFromBuild(enumBOOL bExcludedFromBuild)
{
	return SetBoolProperty(PREBLDID_ExcludedFromBuild, bExcludedFromBuild);
}

void CVCPreBuildEventPage::GetBaseDefault(long id, CComVariant& varValue)
{
	if (id == PREBLDID_Description)
	{
		CComBSTR bstrVal;
		GetDefaultValue( PREBLDID_Description, &bstrVal );
		varValue = bstrVal;
	}
	else
		CBase::GetBaseDefault(id, varValue);
}

STDMETHODIMP CVCPreLinkEventPage::get_CommandLine(BSTR *pVal)
{
	HRESULT hr = S_OK;
	CStringW str;
	if(GetTruncatedStringForProperty(PRELINKID_CommandLine, str))
	{
		*pVal = str.AllocSysString();
		return S_OK;
	}
	else
	{
		hr = GetStrProperty(PRELINKID_CommandLine, pVal);
		if (hr == S_FALSE)
			GetDefaultValue( PRELINKID_CommandLine, pVal );
	}
	return hr;
}

STDMETHODIMP CVCPreLinkEventPage::put_CommandLine(BSTR newVal)
{
	HRESULT hr = SetStrProperty(PRELINKID_CommandLine, newVal);
	RefreshPropGrid();
	return hr;
}

STDMETHODIMP CVCPreLinkEventPage::get_Description(BSTR *pVal)
{
	HRESULT hr = GetStrProperty(PRELINKID_Description, pVal);
	if (hr == S_FALSE)
		GetDefaultValue( PRELINKID_Description, pVal );
	return hr;
}

STDMETHODIMP CVCPreLinkEventPage::put_Description(BSTR newVal)
{
	return SetStrProperty(PRELINKID_Description, newVal);
}

STDMETHODIMP CVCPreLinkEventPage::get_ExcludedFromBuild(enumBOOL* pbExcludedFromBuild)
{
	return GetEnumBoolProperty(PRELINKID_ExcludedFromBuild, pbExcludedFromBuild);
}

STDMETHODIMP CVCPreLinkEventPage::put_ExcludedFromBuild(enumBOOL bExcludedFromBuild)
{
	return SetBoolProperty(PRELINKID_ExcludedFromBuild, bExcludedFromBuild);
}

void CVCPreLinkEventPage::GetBaseDefault(long id, CComVariant& varValue)
{
	if (id == PRELINKID_Description)
	{
		CComBSTR bstrVal;
		GetDefaultValue( PRELINKID_Description, &bstrVal );
		varValue = bstrVal;
	}
	else
		CBase::GetBaseDefault(id, varValue);
}

STDMETHODIMP CVCPostBuildEventPage::get_CommandLine(BSTR *pVal)
{
	HRESULT hr = S_OK;
	CStringW str;
	if(GetTruncatedStringForProperty(POSTBLDID_CommandLine, str))
	{
		*pVal = str.AllocSysString();
		return S_OK;
	}
	else
	{
		hr = GetStrProperty(POSTBLDID_CommandLine, pVal);
		if (hr == S_FALSE)
			GetDefaultValue( POSTBLDID_CommandLine, pVal );
	}
	return hr;
}

STDMETHODIMP CVCPostBuildEventPage::put_CommandLine(BSTR newVal)
{
	HRESULT hr = SetStrProperty(POSTBLDID_CommandLine, newVal);
	RefreshPropGrid();
	return hr;
}

STDMETHODIMP CVCPostBuildEventPage::get_Description(BSTR *pVal)
{
	HRESULT hr = GetStrProperty(POSTBLDID_Description, pVal);
	if (hr == S_FALSE)
		GetDefaultValue( POSTBLDID_Description, pVal );
	return hr;
}

STDMETHODIMP CVCPostBuildEventPage::put_Description(BSTR newVal)
{
	return SetStrProperty(POSTBLDID_Description, newVal);
}

STDMETHODIMP CVCPostBuildEventPage::get_ExcludedFromBuild(enumBOOL* pbExcludedFromBuild)
{
	return GetEnumBoolProperty(POSTBLDID_ExcludedFromBuild, pbExcludedFromBuild);
}

STDMETHODIMP CVCPostBuildEventPage::put_ExcludedFromBuild(enumBOOL bExcludedFromBuild)
{
	return SetBoolProperty(POSTBLDID_ExcludedFromBuild, bExcludedFromBuild);
}

void CVCPostBuildEventPage::GetBaseDefault(long id, CComVariant& varValue)
{
	if (id == POSTBLDID_Description)
	{
		CComBSTR bstrVal;
		GetDefaultValue( POSTBLDID_Description, &bstrVal );
		varValue = bstrVal;
	}
	else
		CBase::GetBaseDefault(id, varValue);
}
