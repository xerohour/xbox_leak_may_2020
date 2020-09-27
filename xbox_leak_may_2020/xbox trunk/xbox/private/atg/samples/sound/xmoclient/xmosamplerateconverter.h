//-----------------------------------------------------------------------------
// Name: XMOSampleRateConverter.h
//
// Desc: Sample rate converter interface. This is an example of deriving the
//       base XMediaObject into a custom interface.
//
// Hist: 12.15.00 - New for December XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once
#include <xtl.h>




//-----------------------------------------------------------------------------
// Name: class CSampleRateConverter
// Desc: Sample-rate converter XMO
//-----------------------------------------------------------------------------
class CSampleRateConverter : public XMediaObject
{
protected:
    DWORD       m_dwChannelCount;           // Channel count
    DWORD       m_dwSourceFrequency;        // Source sampling rate
    DWORD       m_dwDestinationFrequency;   // Destination sampling rate
    DWORD       m_dwSourceAlignment;        // Source sample alignment
    DWORD       m_dwDestinationAlignment;   // Source sample alignment
    DWORD       m_dwIndex;                  // Sample rate converter index
    ULONG       m_ulRefCount;               // Object reference count

public:
    CSampleRateConverter( DWORD dwChannelCount, DWORD dwSourceFrequency, 
                          DWORD dwDestinationFrequency );

public:
    // IUnknown methods
    virtual ULONG   STDMETHODCALLTYPE AddRef();
    virtual ULONG   STDMETHODCALLTYPE Release();

    // XMediaObject methods
    virtual HRESULT STDMETHODCALLTYPE GetInfo( XMEDIAINFO* pInfo );
    virtual HRESULT STDMETHODCALLTYPE GetStatus( DWORD* pdwStatus );
    virtual HRESULT STDMETHODCALLTYPE Process( const XMEDIAPACKET* pInputPacket, 
                                               const XMEDIAPACKET* pOutputPacket );
    virtual HRESULT STDMETHODCALLTYPE Discontinuity();
    virtual HRESULT STDMETHODCALLTYPE Flush();
};



