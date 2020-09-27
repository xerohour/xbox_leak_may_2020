#ifdef XQUAKE_WMA

#ifndef QUAKEWMA_H
#define QUAKEWMA_H




// Define the maximum amount of packets we will ever submit to the renderer
#define FILESTRM_PACKET_COUNT 8

// Define the source packet size.  Because we have a transform filter that
// will expand the source data before sending it to the renderer, we have
// to maintain separate packet sizes: one for compressed and one for
// uncompressed.
//
// This value is hard-coded assuming a WMA file of stero, 16bit resolution.  If
// this Value can by dynamically set based on the wma format, keeping in mind
// that wma needs enough buffer for a minimum of 2048 samples worth of PCM data
// the packet is multiplied by 2 so we can have alarge amount of sound per EchoFx processing
// this increases the echo delay
//

#define FILESTRM_SOURCE_PACKET_BYTES ((2048*2*2)*2)

//-----------------------------------------------------------------------------
// Name: class CWmaFileStream
// Desc: Wave file streaming object
//-----------------------------------------------------------------------------
class CWmaFileStream
{
protected:
    XFileMediaObject*   m_pSourceFilter;                          // Source (wave file) filter
    XMediaObject*       m_pTransformFilter;                       // Transform (APDCM decompressor) filter
    IDirectSoundStream* m_pRenderFilter;                          // Render (DirectSoundStream) filter
    LPVOID              m_pvSourceBuffer;                         // Source filter data buffer
    LPVOID              m_pvRenderBuffer;                         // Render filter data buffer
    DWORD               m_adwPacketStatus[FILESTRM_PACKET_COUNT]; // Packet status array
    DWORD               m_dwFileLength;                           // File duration, in bytes
    DWORD               m_dwFileProgress;                         // File progress, in bytes
    DWORD               m_dwStartingDataOffset;                   // offset into wma file were data begins
    BOOLEAN             m_fUseIntermediateXmo;

    // Packet processing
    BOOL    FindFreePacket(DWORD* pdwPacketIndex );
    HRESULT ProcessSource(DWORD dwPacketIndex);
    HRESULT ProcessTransform( DWORD dwPacketIndex );
    HRESULT ProcessRenderer( DWORD dwPacketIndex );

public:
    // Processing
    HRESULT Process( DWORD* pdwPercentCompleted );

    // Initialization
    HRESULT Initialize( const CHAR* strFileName );
    
    CWmaFileStream();
    ~CWmaFileStream();
};




#endif // QUAKEWMA_H

#endif // XQUAKE_WMA