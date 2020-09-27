#if !defined(TRENTRY_H__36F6EDF3_46CE_11D0_B9DB_00AA00C08146__INCLUDED_)
#define TRENTRY_H__36F6EDF3_46CE_11D0_B9DB_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// TREntry.h : Declaration of CTransportRegEntry and CSecondaryTransportRegEntry

/////////////////////////////////////////////////////////////////////////////
// ITransportRegEntry interface

interface ITransportRegEntry : IUnknown
{
	virtual HRESULT __stdcall GetTransport( IDMUSProdTransport **ppITransport ) = 0;
	virtual HRESULT __stdcall GetFlags( DWORD *pdwFlags ) = 0;
	virtual HRESULT __stdcall SetFlags( DWORD dwFlags ) = 0;
};

// CTransportRegEntry definition
class CTransportRegEntry : ITransportRegEntry
{
public:
	// IUnknown methods
	virtual HRESULT __stdcall QueryInterface( const IID &iid, void **ppv );
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();

	// ITransportRegEntry methods
	HRESULT __stdcall GetTransport( IDMUSProdTransport **ppITransport );
	HRESULT __stdcall GetFlags( DWORD *pdwFlags );
	HRESULT __stdcall SetFlags( DWORD dwFlags );

	// Constructor
	CTransportRegEntry( IDMUSProdTransport *pITransport ) :
		m_cRef(0),
		m_pITransport(pITransport),
		m_dwFlags(0) {}

private:
	// Member variables
	long  m_cRef;
	IDMUSProdTransport	*m_pITransport;
	DWORD m_dwFlags;
};

/////////////////////////////////////////////////////////////////////////////
// ISecondaryTransportRegEntry interface

interface ISecondaryTransportRegEntry : IUnknown
{
	virtual HRESULT __stdcall GetSecondaryTransport( IDMUSProdSecondaryTransport **ppISecondaryTransport ) = 0;
};

// CSecondaryTransportRegEntry definition
class CSecondaryTransportRegEntry : ISecondaryTransportRegEntry
{
public:
	// IUnknown methods
	virtual HRESULT __stdcall QueryInterface( const IID &iid, void **ppv );
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();

	// ISecondaryTransportRegEntry methods
	HRESULT __stdcall GetSecondaryTransport( IDMUSProdSecondaryTransport **ppISecondaryTransport );

	// Constructor
	CSecondaryTransportRegEntry( IDMUSProdSecondaryTransport *pISecondaryTransport ) :
		m_cRef(0),
			m_pISecondaryTransport(pISecondaryTransport) {}

private:
	// Member variables
	long  m_cRef;
	IDMUSProdSecondaryTransport	*m_pISecondaryTransport;
};

#endif // !defined(TRENTRY_H__36F6EDF3_46CE_11D0_B9DB_00AA00C08146__INCLUDED_)
