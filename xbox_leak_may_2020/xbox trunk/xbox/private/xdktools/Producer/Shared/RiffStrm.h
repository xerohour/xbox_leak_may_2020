/************************************************************************
*                                                                       *
*   Contains the definition of CRIFFStream                              *
*                                                                       *
*   Copyright (c) 1995-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

#ifndef _CRIFFStream_
#define _CRIFFStream_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#pragma warning( push )
#pragma warning( disable : 4201 )
#include <mmsystem.h>
#pragma warning( pop )
#include <ole2.h>

// {F809DCE1-859D-11d0-89AC-00A0C9054129}
DEFINE_GUID( IID_IDMUSProdRIFFStream, 0xf809dce1, 0x859d, 0x11d0, 0x89, 0xac, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29 );

#undef INTERFACE
#define INTERFACE IDMUSProdRIFFStream
DECLARE_INTERFACE_(IDMUSProdRIFFStream, IUnknown)
{
    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppv) PURE;
    STDMETHOD_(ULONG, AddRef)() PURE;
    STDMETHOD_(ULONG, Release)() PURE;

    // IDMUSProdRIFFStream members
    STDMETHOD_(UINT, Descend)(LPMMCKINFO lpck, LPMMCKINFO lpckParent, UINT wFlags) PURE;
    STDMETHOD_(UINT, Ascend)(LPMMCKINFO lpck, UINT wFlags) PURE;
    STDMETHOD_(UINT, CreateChunk)(LPMMCKINFO lpck, UINT wFlags) PURE;
    STDMETHOD(SetStream)(IStream* pIStream) PURE;
    STDMETHOD_(IStream*, GetStream)() PURE;
};


// **************************************
//
// Platform Independent RIFF Tags
//
// **************************************

// **************************************
//
// CRIFFStream
//
// Implementation of IDMUSProdRIFFStream interface, and IUnknown
//
// **************************************


STDAPI AllocRIFFStream( IStream* pIStream, IDMUSProdRIFFStream** ppIRiffStream );
HRESULT SaveMBStoWCS( IStream* pIStream, const CString* pstrText );
void ReadMBSfromWCS( IStream* pIStream, DWORD dwSize, CString* pstrText );



struct CRIFFStream : IDMUSProdRIFFStream
{
///// object state
    ULONG       m_cRef;         // object reference count
    IStream*    m_pStream;      // stream to operate on

///// construction and destruction
    CRIFFStream(IStream* pStream)
    {
        m_cRef = 1;
		m_pStream = NULL;

        SetStream( pStream );
    }
    ~CRIFFStream()
    {
        if( m_pStream != NULL )
        {
            m_pStream->Release();
        }
    }

///// IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObj)
    {
        if( IsEqualIID( riid, IID_IUnknown ) ||
            IsEqualIID( riid, IID_IDMUSProdRIFFStream ) )
        {
            *ppvObj = (IDMUSProdRIFFStream*)this;
            AddRef();
            return NOERROR;
        }
        *ppvObj = NULL;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef()
    {
        return ++m_cRef;
    }
    STDMETHODIMP_(ULONG) Release()
    {
        if( --m_cRef == 0L )
        {
            delete this;
            return 0;
        }
        return m_cRef;
    }

// IDMUSProdRIFFStream methods
    STDMETHODIMP_(UINT) Descend( LPMMCKINFO lpck, LPMMCKINFO lpckParent, UINT wFlags );
    STDMETHODIMP_(UINT) Ascend( LPMMCKINFO lpck, UINT wFlags );
    STDMETHODIMP_(UINT) CreateChunk( LPMMCKINFO lpck, UINT wFlags );
    STDMETHOD(SetStream)(LPSTREAM pStream)
    {
        if( m_pStream != NULL )
        {
            m_pStream->Release();
        }
        m_pStream = pStream;
        if( m_pStream != NULL )
        {
            m_pStream->AddRef();
        }
        return S_OK;
    }
    STDMETHOD_(LPSTREAM, GetStream)()
    {
        if( m_pStream != NULL )
        {
            m_pStream->AddRef();
        }
        return m_pStream;
    }

// private methods
    long MyRead( void *pv, long cb );
    long MyWrite( const void *pv, long cb );
    long MySeek( long lOffset, int iOrigin );
};


// seeks to a 32-bit position in a stream.
HRESULT __inline StreamSeek( LPSTREAM pStream, long lSeekTo, DWORD dwOrigin )
{
	LARGE_INTEGER li;

	if( lSeekTo < 0 )
	{
		li.HighPart = -1;
	}
	else
	{
        li.HighPart = 0;
	}
	li.LowPart = lSeekTo;
	return pStream->Seek( li, dwOrigin, NULL );
}


// returns the current 32-bit position in a stream.
DWORD __inline StreamTell( LPSTREAM pStream )
{
	LARGE_INTEGER li;
    ULARGE_INTEGER ul;
#ifdef _DEBUG
    HRESULT hr;
#endif

    li.HighPart = 0;
    li.LowPart = 0;
#ifdef _DEBUG
    hr = pStream->Seek( li, STREAM_SEEK_CUR, &ul );
    if( FAILED( hr ) )
#else
    if( FAILED( pStream->Seek( li, STREAM_SEEK_CUR, &ul ) ) )
#endif
    {
        return 0;
    }
    return ul.LowPart;
}

#ifndef _GetMLong_Defined_
#define _GetMLong_Defined_


// this function gets a long that is formatted the correct way
// i.e. the motorola way as opposed to the intel way
BOOL __inline GetMLong( LPSTREAM pStream, DWORD& dw )
{
	union uLong
	{
		unsigned char buf[4];
	DWORD dw;
	} u;

	if( S_OK != pStream->Read( u.buf, 4, NULL ) )
	{
	return FALSE;
	}


#ifndef _MAC
	// swap bytes
	unsigned char ch;
	ch = u.buf[0];
	u.buf[0] = u.buf[3];
	u.buf[3] = ch;

	ch = u.buf[1];
	u.buf[1] = u.buf[2];
	u.buf[2] = ch;
#endif

	dw = u.dw;
	return TRUE;
}

#endif

#endif
