// Enum.cpp : implementation file
//

#include "stdafx.h"

#include "DLSDesignerDLL.h"
#include "Collection.h"
#include "Instrument.h"
#include "Enum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CEnumCollectionFolder construction

CEnumCollectionFolder::CEnumCollectionFolder()
{
    ASSERT( 1 );
}

CEnumCollectionFolder::CEnumCollectionFolder( AList *pList )
{
    m_dwRef = 1;
    m_dwPos   = 0;
	m_pList = pList;
}


/////////////////////////////////////////////////////////////////////////////
// CEnumCollectionFolder IUnknown implementation

HRESULT CEnumCollectionFolder::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
    if( ::IsEqualIID(riid, IID_IEnumJazzNodes)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CEnumCollectionFolder::AddRef()
{
    return ++m_dwRef;
}

ULONG CEnumCollectionFolder::Release()
{
    ASSERT( m_dwRef != 0 );

    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CEnumCollectionFolder IEnumJazzNodes implementation

HRESULT CEnumCollectionFolder::Next( ULONG celt, IJazzNode __RPC_FAR* __RPC_FAR* rgelt, ULONG __RPC_FAR* pceltFetched )
{
    ULONG ulFetched;
	IJazzNode* pNode;
	CCollection* pCollection;
    HRESULT hr;

    if( celt == 0 || ( celt > 1 && pceltFetched == NULL ) )
    {
        return E_INVALIDARG;
    }

    ulFetched = 0;
    hr = S_OK;
    
	do
    {
		pCollection = (CCollection *) m_pList->GetItem( m_dwPos++ );
		if (!pCollection)	// End of list?
		{
			hr = S_FALSE;
			break;
		}

		if( !SUCCEEDED ( pCollection->QueryInterface( IID_IJazzNode, (void**)&pNode ) ) )
		{
            hr = S_FALSE;
            break;
		}
		pNode->Release();	// intentional Release()
        *rgelt++ = pNode;
        ++ulFetched;
    }
	while( --celt > 0 );

    if( pceltFetched != NULL )
    {
        *pceltFetched = ulFetched;
    }

    return hr;
}

HRESULT CEnumCollectionFolder::Skip( ULONG celt )
{
	m_dwPos += celt;
    return S_OK;
}

HRESULT CEnumCollectionFolder::Reset()
{
    m_dwPos = 0;
    return S_OK;
}

HRESULT CEnumCollectionFolder::Clone( IEnumJazzNodes __RPC_FAR* __RPC_FAR* ppEnum )
{
    CEnumCollectionFolder* pEnum;

    if( ppEnum == NULL )
    {
        return E_INVALIDARG;
    }

    pEnum = new CEnumCollectionFolder( m_pList );

    if( pEnum == NULL )
    {
        return E_OUTOFMEMORY;
    }

    *ppEnum = pEnum;

    return S_OK;
}

