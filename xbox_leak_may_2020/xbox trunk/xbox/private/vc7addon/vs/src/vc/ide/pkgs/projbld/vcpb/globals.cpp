// Globals.cpp
// implementation of CVCGlobals
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "projwriter.h"
#include "globals.h"


// CVCGlobals
///////////////////////////////////////////////////////////////////////////

CVCGlobals::CVCGlobals()
{
}

CVCGlobals::~CVCGlobals()
{
}

HRESULT CVCGlobals::CreateInstance( CVCGlobals** ppGlobals )
{
	CComObject<CVCGlobals> *pObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CVCGlobals>::CreateInstance( &pObj );
	if( SUCCEEDED( hr ) )
		pObj->AddRef();
	*ppGlobals = pObj;
	return hr;
}

// IVCGlobals
STDMETHODIMP CVCGlobals::GetValue( BSTR bstrName, BSTR *pbstrValue )
{
	CStringW strValue;
	// look up name in the map
	if( !m_GlobalsMap.Lookup( bstrName, strValue ) )
		RETURN_INVALID();

	// if we found it, set the return value
	CComBSTR bstrRet( strValue );
	*pbstrValue = bstrRet.Detach();
	return S_OK;
}

STDMETHODIMP CVCGlobals::SetValue( BSTR bstrName, BSTR bstrValue )
{
	m_GlobalsMap.SetAt( bstrName, bstrValue );
	return S_OK;
}

STDMETHODIMP CVCGlobals::GetPersistance( BSTR bstrName, VARIANT_BOOL *pbPersists )
{
	// if the name is not in the persistance map, it doesn't persist
	int *junk = NULL;
	if( !m_GlobalsPersistanceMap.Lookup( bstrName, (void*&)junk ) )
		*pbPersists = VARIANT_FALSE;
	else 
		*pbPersists = VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP CVCGlobals::SetPersistance( BSTR bstrName, VARIANT_BOOL bPersists )
{
	int *junk = NULL;
	if( bPersists )
		m_GlobalsPersistanceMap.SetAt( bstrName, (void*&)junk );
	else
		m_GlobalsPersistanceMap.RemoveKey( bstrName );

	return S_OK;
}

STDMETHODIMP CVCGlobals::GetCount( ULONG *count )
{
	*count = (ULONG) m_GlobalsMap.GetCount();
	return S_OK;
}

static VCPOSITION s_pos = 0;

STDMETHODIMP CVCGlobals::GetFirstItem( BSTR *pbstrName, BSTR *pbstrValue )
{
	s_pos = m_GlobalsMap.GetStartPosition();
	if( !s_pos )
	{
		// no items
		*pbstrName = NULL;
		*pbstrValue = NULL;
		return E_FAIL;
	}
	CStringW strName, strValue;
	m_GlobalsMap.GetNextAssoc( s_pos, strName, strValue );

	CComBSTR bstrName( strName );
	CComBSTR bstrValue( strValue );
	*pbstrName = bstrName.Detach();
	*pbstrValue = bstrValue.Detach();
	return S_OK;
}

STDMETHODIMP CVCGlobals::GetNextItem( BSTR *pbstrName, BSTR *pbstrValue )
{
	if( !s_pos )
	{
		// no more items
		*pbstrName = NULL;
		*pbstrValue = NULL;
		return E_FAIL;
	}
	CStringW strName, strValue;
	m_GlobalsMap.GetNextAssoc( s_pos, strName, strValue );

	CComBSTR bstrName( strName );
	CComBSTR bstrValue( strValue );
	*pbstrName = bstrName.Detach();
	*pbstrValue = bstrValue.Detach();
	return S_OK;
}

STDMETHODIMP CVCGlobals::SaveObject( IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent )
{
	// for each name in the persistance map
	VCPOSITION pos;
	pos = m_GlobalsPersistanceMap.GetStartPosition();
	while( pos )
	{
		StartNodeHeader( xml, L"Global", false );
		CStringW strName, strValue;
		int *junk = NULL;
		m_GlobalsPersistanceMap.GetNextAssoc( pos, strName, (void*&)junk );
		// get the value for the item
		m_GlobalsMap.Lookup( strName, strValue );
		// write it out
		if( strName )
		{
			NodeAttributeWithSpecialChars( xml, L"Name", CComBSTR( strName ) );
			NodeAttributeWithSpecialChars( xml, L"Value", CComBSTR( strValue ) );
		}
		EndNodeHeader( xml, false );
		EndNode( xml, L"Globals", false );
	}

	return S_OK;
}
