//-----------------------------------------------------------------------------
// File: WmaFileStream.cpp
//
// Desc: WMA file playback
//
// Hist: 02.16.01 - New for March XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBWMA_FILE_STREAM_H
#define XBWMA_FILE_STREAM_H

#include <xtl.h>
#include <XbSound.h>




//-----------------------------------------------------------------------------
// Name: class CWMAFileStream
// Desc: Wave file streaming object
//-----------------------------------------------------------------------------
class CWMAFileStream
{
    // The maximum amount of packets we will ever submit to the renderer
    static const DWORD WMASTRM_PACKET_COUNT = 8;

    XFileMediaObject*   m_pSourceFilter;    // Source (wave file) filter
    IDirectSoundStream* m_pRenderFilter;    // Render (DirectSoundStream) filter
    BYTE*               m_pSourceBuffer;    // Source filter data buffer

    // Packet status array
    DWORD m_adwPacketStatus[ WMASTRM_PACKET_COUNT ];

public:

    CWMAFileStream();
    ~CWMAFileStream();

    HRESULT Start( HANDLE hFile );
    VOID    Stop();
    BOOL    IsStopped() const;
    HRESULT Process();

private:

    // Packet processing
    BOOL    FindFreePacket( DWORD& dwPacketIndex ) const;
    HRESULT ProcessSource( DWORD dwPacketIndex ) const;
    HRESULT ProcessRenderer( DWORD dwPacketIndex );

};

#endif // XBWMA_FILE_STREAM_H
