// filegencfg.cpp
// implementation of CFileGenCfg

#include "stdafx.h"
#include "vssolutn.h"
#include "filegencfg.h"

//////////////////////////////////////////////////////////////////////////////
// CFileGenCfg
// 

CFileGenCfg::CFileGenCfg( void ) : m_pFileConfig( NULL )
{
}

CFileGenCfg::~CFileGenCfg( void )
{
}

void CFileGenCfg::Initialize( VCFileConfiguration *pCfg )
{
	m_pFileConfig = pCfg;
}

HRESULT CFileGenCfg::CreateInstance( IVsCfg **ppCfg, VCFileConfiguration* pFileCfg )
{
	CComObject<CFileGenCfg> *pFileGenCfgObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CFileGenCfg>::CreateInstance( &pFileGenCfgObj );
	if( SUCCEEDED( hr ) )
	{
		pFileGenCfgObj->Initialize( pFileCfg );
		hr = pFileGenCfgObj->QueryInterface( _uuidof( IVsCfg ), (void**)ppCfg );
	}
	return hr;
}


//////////////////////////////////////////////////////////////////////////////
// IVCCfg methods

STDMETHODIMP CFileGenCfg::get_Tool( VARIANT toolIndex, IUnknown **ppTool )
{
	CHECK_POINTER_NULL(ppTool)
	
	HRESULT hr;
	CComPtr<IDispatch> spTool;

	hr = m_pFileConfig->get_Tool(&spTool);		//Get the tool
	if (SUCCEEDED(hr)) 
	{
		//Return the unk interface of the tool
		hr = spTool->QueryInterface(IID_IUnknown, (void **) ppTool);
	}
	return hr;	
}

STDMETHODIMP CFileGenCfg::get_Object( IDispatch **ppConfig)
{
	CHECK_POINTER_NULL(ppConfig)
	
	HRESULT hr;
	hr = m_pFileConfig->QueryInterface(IID_IDispatch, (void **) ppConfig);
	return hr;
}

//////////////////////////////////////////////////////////////////////////////
// IVsCfg
STDMETHODIMP CFileGenCfg::get_DisplayName( BSTR *pVal )
{
	return m_pFileConfig->get_Name( pVal );
}

STDMETHODIMP CFileGenCfg::get_IsDebugOnly( BOOL *pVal )
{
	CHECK_POINTER_NULL(pVal)
	
    CComBSTR bstrName;
	m_pFileConfig->get_Name( &bstrName );
	if( !wcsstr( bstrName, L"Debug" ) )
		*pVal = TRUE;
	else
		*pVal = FALSE;
	return S_OK;
}

STDMETHODIMP CFileGenCfg::get_IsReleaseOnly( BOOL *pVal )
{
	CHECK_POINTER_NULL(pVal)

    CComBSTR bstrName;
	m_pFileConfig->get_Name( &bstrName );
	if( !wcsstr( bstrName, L"Release" ) )
		*pVal = TRUE;
	else
		*pVal = FALSE;
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////////
// ISpecifyPropertyPages methods
STDMETHODIMP CFileGenCfg::GetPages( CAUUID *pPages )
{
	CHECK_POINTER_NULL(pPages)
	
	// QI for ISpecifyPropertyPages from the config
	CComQIPtr<ISpecifyPropertyPages> pSPP = GetVCFileConfiguration();
	if( !pSPP )
		return E_NOINTERFACE;
	return pSPP->GetPages( pPages );
}


