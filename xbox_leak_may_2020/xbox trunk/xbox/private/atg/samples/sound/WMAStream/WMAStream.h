//-----------------------------------------------------------------------------
// File: WMAStream.h
//
// Desc: Streaming wave file playback - demonstrates how to run the WMA
//       decoder synchronously on a separate thread
//
// Hist: 12.15.00 - New for December XDK release
//       3.15.01 - Updated for April XDK 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef WMASTREAM_H
#define WMASTREAM_H

//
// the wma decoder will work properly in async mode, with all formats
// only when using a lookahead size of at least 64k. We are using it in 
// synchronous mode but for good measure we still pass 64k
//

#define WMASTRM_LOOKAHEAD_SIZE (4096*16)

// Define the maximum amount of packets we will ever submit to the renderer
#define WMASTRM_PACKET_COUNT 8




// Define the source packet size:
// This value is hard-coded assuming a WMA file of stero, 16bit resolution.  If
// this Value can by dynamically set based on the wma format, keeping in mind
// that wma needs enough buffer for a minimum of 2048 samples worth of PCM data
#define WMASTRM_SOURCE_PACKET_BYTES (2048*2*2)

// ThreadProc for worker thread
DWORD WINAPI WMAFileStreamThreadProc( LPVOID lpParameter );

//-----------------------------------------------------------------------------
// Name: class CWMAFileStream
// Desc: Wave file streaming object
//-----------------------------------------------------------------------------
class CWMAFileStream
{
protected:
    XWmaFileMediaObject*   m_pSourceFilter;                         // Source (wave file) filter
    IDirectSoundStream* m_pRenderFilter;                         // Render (DirectSoundStream) filter
    LPVOID              m_pvSourceBuffer;                        // Source filter data buffer
    LPVOID              m_pvRenderBuffer;                        // Render filter data buffer
    DWORD               m_adwPacketStatus[WMASTRM_PACKET_COUNT]; // Packet status array
    DWORD               m_dwFileProgress;                        // File progress, in bytes
    DWORD               m_dwStartingDataOffset;                  // offset into wma file were data begins
    DWORD *             m_pdwPercentCompleted;                   // Pointer to percent complete variable

    
    CHAR*               m_strFilename;                           // File to load
    HRESULT             m_hrOpenResult;                          // Result of opening file
    WAVEFORMATEX        m_wfxSourceFormat;                       // Wave format

    // Packet processing
    BOOL    FindFreePacket(DWORD* pdwPacketIndex );
    HRESULT ProcessSource(DWORD dwPacketIndex);
    HRESULT ProcessRenderer( DWORD dwPacketIndex );

public:

    WMAXMOFileContDesc  m_WmaContentDescription;                 // text information about author, title, description, etc
	WMAXMOFileHeader    m_WmaFileHeader;                         // basic information about the wma file

    // Processing
    HRESULT Process();

    // Initialization
    HRESULT Initialize( const CHAR* strFileName, DWORD * pdwPercentCompleted = NULL );
    HRESULT InitializeOnThread();
    HRESULT GetOpenResult() { return m_hrOpenResult; }
    
    // Play control
    VOID Pause( DWORD dwPause );

    CWMAFileStream();
    ~CWMAFileStream();
};




#endif // WMASTREAM_H
