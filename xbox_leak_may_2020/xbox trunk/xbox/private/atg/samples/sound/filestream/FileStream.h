//-----------------------------------------------------------------------------
// File: FileStream.h
//
// Desc: Streaming wave file playback.
//
// Hist: 12.15.00 - New for December XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef FILESTREAM_H
#define FILESTREAM_H




// Define the maximum amount of packets we will ever submit to the renderer
#define FILESTRM_PACKET_COUNT 3




// This value is hard-coded assuming an ADPCM frame of 36 samples and 16
// bit stereo (128 ADPCM frames per packet)
#define FILESTRM_PACKET_BYTES 2 * 2 * 36 * 128


// ThreadProc for worker thread
DWORD WINAPI WAVFileStreamThreadProc( LPVOID lpParameter );


//-----------------------------------------------------------------------------
// Name: class CWaveFileStream
// Desc: Wave file streaming object
//-----------------------------------------------------------------------------
class CWaveFileStream
{
protected:
    XFileMediaObject*   m_pSourceFilter;                          // Source (wave file) filter
    IDirectSoundStream* m_pRenderFilter;                          // Render (DirectSoundStream) filter
    LPVOID              m_pvSourceBuffer;                         // Source filter data buffer
    DWORD               m_adwPacketStatus[FILESTRM_PACKET_COUNT]; // Packet status array
    DWORD               m_dwFileLength;                           // File duration, in bytes
    DWORD               m_dwFileProgress;                         // File progress, in bytes

    DWORD *             m_pdwPercentCompleted;                    // Pointer to percentage completed

    // Packet processing
    BOOL    FindFreePacket(DWORD* pdwPacketIndex );
    HRESULT ProcessSource( DWORD dwPacketIndex );
    HRESULT ProcessRenderer( DWORD dwPacketIndex );

public:
    // Processing
    HRESULT Process();

    // Initialization
    HRESULT Initialize( const CHAR* strFileName, DWORD * pdwPercentCompleted );

    // Play control
    VOID Pause( DWORD dwPause );
    
    CWaveFileStream();
    ~CWaveFileStream();
};




#endif // FILESTREAMM_H
