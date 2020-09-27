//-----------------------------------------------------------------------------
// File: WMAInMemory.h
//
// Desc: Streaming wave file playback.
//
// Hist: 3.15.01 - New for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef WMAINMEMORY_H
#define WMAINMEMORY_H

//
// the wma decoder will work properly in async mode, with all formats
// only when using a lookahead size of at least 64k. We are using it synchronous mode
// but for good measure we still pass 64k
//

#define WMASTRM_LOOKAHEAD_SIZE (4096*16)

// Define the maximum amount of packets we will ever submit to the renderer
#define WMASTRM_PACKET_COUNT 8




// Define the source packet size:
// This value is hard-coded assuming a WMA file of stero, 16bit resolution.  If
// this Value can by dynamically set based on the wma format, keeping in mind
// that wma needs enough buffer for a minimum of 2048 samples worth of PCM data
#define WMASTRM_SOURCE_PACKET_BYTES (2048*2*2)

DWORD CALLBACK WMAStreamCallback (
    LPVOID pContext,
    DWORD offset,
    DWORD num_bytes,
    LPVOID *ppData);


//-----------------------------------------------------------------------------
// Name: class CWMAFileStream
// Desc: Wave file streaming object
//-----------------------------------------------------------------------------
class CWMAFileStream
{
protected:
    XMediaObject*       m_pSourceFilter;                         // Source (wave file) filter
    IDirectSoundStream* m_pRenderFilter;                         // Render (DirectSoundStream) filter
    LPVOID              m_pvSourceBuffer;                        // Source filter data buffer
    LPVOID              m_pvRenderBuffer;                        // Render filter data buffer
    DWORD               m_adwPacketStatus[WMASTRM_PACKET_COUNT]; // Packet status array
    DWORD               m_dwFileLength;                          // File duration, in bytes
    DWORD               m_dwFileProgress;                        // File progress, in bytes

    PUCHAR              m_pFileBuffer;
    HANDLE              m_hFile;

    // Packet processing
    BOOL    FindFreePacket(DWORD* pdwPacketIndex );
    HRESULT ProcessSource(DWORD dwPacketIndex);
    HRESULT ProcessRenderer( DWORD dwPacketIndex );

    friend DWORD CALLBACK WMAStreamCallback (
    LPVOID pContext,
    ULONG offset,
    ULONG num_bytes,
    LPVOID *ppData);
     

public:
    // Processing
    HRESULT Process( DWORD* pdwPercentCompleted );

    // Initialization
    HRESULT Initialize( const CHAR* strFileName );

    // Play control
    VOID Pause( DWORD dwPause );
    
    CWMAFileStream();
    ~CWMAFileStream();
};




#endif // WMAINMEMORY_H
