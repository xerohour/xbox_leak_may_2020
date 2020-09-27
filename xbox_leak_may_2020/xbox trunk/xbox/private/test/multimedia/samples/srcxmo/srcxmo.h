/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       SRCXMO.h
 *  Content:    Sample rate converter interface
 *
 ****************************************************************************/
#pragma once

#include <xtl.h>

typedef short *LPSHORT;

//
// Sample-rate converter XMO
//

class CSrcMediaObject
    : public XMediaObject
{
protected:
    const DWORD             m_dwChannelCount;           // Channel count
    const DWORD             m_dwSourceFrequency;        // Source sampling rate
    const DWORD             m_dwDestinationFrequency;   // Destination sampling rate
    DWORD                   m_dwSourceAlignment;        // Source sample alignment
    DWORD                   m_dwDestinationAlignment;   // Source sample alignment
    DWORD                   m_dwSrcIndex;               // SRC index
    ULONG                   m_ulRefCount;               // Object reference count

public:
    CSrcMediaObject(DWORD dwChannelCount, DWORD dwSourceFrequency, DWORD dwDestinationFrequency);

public:
    // IUnknown methods
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // XMediaObject methods
    virtual HRESULT STDMETHODCALLTYPE GetInfo(LPXMEDIAINFO pInfo);
    virtual HRESULT STDMETHODCALLTYPE GetStatus(LPDWORD pdwStatus);
    virtual HRESULT STDMETHODCALLTYPE Process(LPCXMEDIAPACKET pInputPacket, LPCXMEDIAPACKET pOutputPacket);
    virtual HRESULT STDMETHODCALLTYPE Discontinuity(void);
    virtual HRESULT STDMETHODCALLTYPE Flush(void);
};