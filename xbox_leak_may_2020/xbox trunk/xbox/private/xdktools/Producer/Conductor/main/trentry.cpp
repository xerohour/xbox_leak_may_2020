// CTransportRegEntry implementation

#include "stdafx.h"
#include "CConduct.h"
#include "TREntry.h"

// {439C9E01-CDFA-11d0-8C10-00A0C92E1CAC}
static const GUID IID_ITransportRegEntry = 
{ 0x439c9e01, 0xcdfa, 0x11d0, { 0x8c, 0x10, 0x0, 0xa0, 0xc9, 0x2e, 0x1c, 0xac } };

// IUnknown
HRESULT __stdcall CTransportRegEntry::QueryInterface( const IID &iid, void **ppv )
{
    if( ::IsEqualIID( iid, IID_ITransportRegEntry )  ||
		::IsEqualIID( iid, IID_IUnknown ) )
	{
		*ppv = static_cast<ITransportRegEntry *>(this);
	}
	else  {
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	static_cast<IUnknown *>(*ppv)->AddRef();
	return S_OK;
}

ULONG __stdcall CTransportRegEntry::AddRef()
{
	return InterlockedIncrement( &m_cRef );
}

ULONG __stdcall CTransportRegEntry::Release()
{
	if( InterlockedDecrement( &m_cRef ) == 0 )
	{
		delete this;
		return 0;
	}
	return m_cRef;
}

// ITransportRegEntry
HRESULT __stdcall CTransportRegEntry::GetTransport( IDMUSProdTransport **ppITransport )
{
	ASSERT( ppITransport != NULL);
	if (ppITransport == NULL)
	{
		return E_POINTER;
	}
	*ppITransport = m_pITransport;
	return S_OK;
}

HRESULT __stdcall CTransportRegEntry::GetFlags( DWORD *pdwFlags )
{
	ASSERT( pdwFlags != NULL);
	if (pdwFlags == NULL)
	{
		return E_POINTER;
	}
	*pdwFlags = m_dwFlags;
	return S_OK;
}

HRESULT __stdcall CTransportRegEntry::SetFlags( DWORD dwFlags )
{
	m_dwFlags = dwFlags;
	return S_OK;
}


// {DD630C6C-228C-11d2-88F7-00C04FBF8D15}
static const GUID IID_ISecondaryTransportRegEntry = 
{ 0xdd630c6c, 0x228c, 0x11d2, { 0x88, 0xf7, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15 } };

// IUnknown
HRESULT __stdcall CSecondaryTransportRegEntry::QueryInterface( const IID &iid, void **ppv )
{
    if( ::IsEqualIID( iid, IID_ISecondaryTransportRegEntry )  ||
		::IsEqualIID( iid, IID_IUnknown ) )
	{
		*ppv = static_cast<ISecondaryTransportRegEntry *>(this);
	}
	else  {
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	static_cast<IUnknown *>(*ppv)->AddRef();
	return S_OK;
}

ULONG __stdcall CSecondaryTransportRegEntry::AddRef()
{
	return InterlockedIncrement( &m_cRef );
}

ULONG __stdcall CSecondaryTransportRegEntry::Release()
{
	if( InterlockedDecrement( &m_cRef ) == 0 )
	{
		delete this;
		return 0;
	}
	return m_cRef;
}

// ISecondaryTransportRegEntry
HRESULT __stdcall CSecondaryTransportRegEntry::GetSecondaryTransport( IDMUSProdSecondaryTransport **ppISecondaryTransport )
{
	ASSERT( ppISecondaryTransport != NULL);
	if (ppISecondaryTransport == NULL)
	{
		return E_POINTER;
	}
	*ppISecondaryTransport = m_pISecondaryTransport;
	return S_OK;
}
