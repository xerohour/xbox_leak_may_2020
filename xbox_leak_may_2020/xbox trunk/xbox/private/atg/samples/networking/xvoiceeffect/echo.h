//-----------------------------------------------------------------------------
// File: Echo.h
//
// Desc: Implements an XMO filter to add an echo effect.
//
// Hist: 01.06.00 - New for February XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>




//-----------------------------------------------------------------------------
// Name: XCreateEchoFxMediaObject()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT XCreateEchoFxMediaObject( XMediaObject** ppMediaObject, DWORD dwDelay,
                                  DWORD dwSampleSize );




//-----------------------------------------------------------------------------
// Name: class CEchoXMO
// Desc: 
//-----------------------------------------------------------------------------
class CEchoXMO : public XMediaObject 
{
    SHORT* m_pPreviousPacket;
    ULONG  m_cRef;
    DWORD  m_dwDelay;
    DWORD  m_dwSampleSize;

public:
    CEchoXMO()
    {
        m_cRef            = 0;
        m_dwDelay         = 0;
        m_dwSampleSize    = 0;
        m_pPreviousPacket = NULL;
    }

    ~CEchoXMO() {}

    STDMETHODIMP_(ULONG)      AddRef() 
    { 
        return InterlockedIncrement((long*)&m_cRef); 
    }
    
    STDMETHODIMP_(ULONG)      Release() 
    {
        long l = InterlockedDecrement( (long*)&m_cRef );
        if( l == 0 )
           delete this;
        return l;
    }
    
    HRESULT STDMETHODCALLTYPE Initialize( DWORD dwDelay, DWORD dwSampleSize );
    HRESULT STDMETHODCALLTYPE GetInfo( XMEDIAINFO* pInfo );
    HRESULT STDMETHODCALLTYPE Process( const XMEDIAPACKET* pSrcBuffer, const XMEDIAPACKET* pDstBuffer );
    HRESULT STDMETHODCALLTYPE GetStatus( DWORD* pdwFlags );
    HRESULT STDMETHODCALLTYPE Flush()         { return S_OK; }
    HRESULT STDMETHODCALLTYPE Discontinuity() { return S_OK; }
};



