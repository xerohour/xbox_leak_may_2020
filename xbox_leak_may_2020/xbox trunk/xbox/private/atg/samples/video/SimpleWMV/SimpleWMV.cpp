//-----------------------------------------------------------------------------
// File: SimpleWMV.cpp
//
// Desc: [TODO: Describe source file here]
//
// Hist: 9.27.01 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>
#include <assert.h>
#include <dsound.h>
#include <wmvxmo.h>

#define VIDEO 1
#define AUDIO 1

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
};

#define NUM_HELP_CALLOUTS 1

// Vertex definition for the screen-space quad
typedef struct {
    D3DXVECTOR4 p;
    D3DXVECTOR2 t;
} CUSTOMVERTEX;
#define FVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_TEX1)

typedef struct {
    LPDIRECT3DTEXTURE8  pTexture;
    REFERENCE_TIME      rtTimestamp;
    BYTE*               pBits;
} VIDEOFRAME;

DWORD g_Num = 0;

// You'll need to adjust NUM_PACKETS based off your video content
// to ensure that you never run out during the course of playback
// Packet Size needs to be large enough to hold at least 2048
// decoded samples
#define NUM_PACKETS 32
#define PACKET_SIZE 2048 * 2 * 2

#define DUMP 0
#define RECORDAUDIO 0

//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
public:
    CXBoxSample();

    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();

    HRESULT PrepareDecoder();
    HRESULT CleanupDecoder();

    HRESULT DownloadEffectsImage();
    BOOL FindFreePacket( DWORD * pdwIndex );    // Find a free audio packet

#if DUMP
    void DumpAudio( XMEDIAPACKET& xmp );
    void DumpVideo( XMEDIAPACKET& xmp );
#endif // _DEBUG

    // Font and help
    CXBFont     m_Font;
    CXBHelp     m_Help;
    BOOL        m_bDrawHelp;

    LPWMVDECODER        m_pWMVDecoder;              // WMV Decoder XMO
    WMVVIDEOINFO        m_wmvVideoInfo;             // Video info
    WAVEFORMATEX        m_wfx;                      // Audio waveformat
    REFERENCE_TIME      m_rtDuration;               // Duration of video

    VIDEOFRAME          m_aVideoFrames[2];          // 2 frames to ping-pong
    DWORD               m_dwDisplayFrame;           // Which to display
    LPDIRECT3DVERTEXBUFFER8 m_pvbQuad;                  // Quad for rendering video

    LPDIRECTSOUND8      m_pDSound;                  // DirectSound object
    DWORD               m_adwStatus[NUM_PACKETS];   // Audio packet status
    BYTE*               m_pbSampleData;             // Audio sample data
    LPDIRECTSOUNDSTREAM m_pStream;                  // DirectSound stream

    BOOL                m_bGotVideo;               // Reset start time
    REFERENCE_TIME      m_rtStart;                  // Time at start of video
    REFERENCE_TIME      m_rtCurrent;                // Current time

#if RECORDAUDIO
    HANDLE              m_hAudioFile;               // Dump audio to file
#endif
};



//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBoxSample xbApp;
    if( FAILED( xbApp.Create() ) )
        return;
    xbApp.Run();
}



//-----------------------------------------------------------------------------
// Name: CXBoxSample (constructor)
// Desc: Constructor for CXBoxSample class
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample() 
            :CXBApplication()
{
    m_bDrawHelp         = FALSE;

    m_pWMVDecoder       = NULL;
    m_pbSampleData      = NULL;
    m_pStream           = NULL;
    m_dwDisplayFrame    = 0;

    m_bGotVideo        = TRUE;

    m_rtCurrent = 0;
    m_aVideoFrames[0].rtTimestamp = 0;
    m_aVideoFrames[1].rtTimestamp = 0;

    // m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}



HRESULT CXBoxSample::PrepareDecoder()
{
    char strFilename[100];
    sprintf( strFilename, "d:\\media\\videos\\test%02d.wmv", g_Num++ );

    // Create the decoder
    if( FAILED( WmvCreateDecoder( strFilename,
                                  NULL,
                                  WMVVIDEOFORMAT_RGB32,
                                  NULL,
                                  &m_pWMVDecoder ) ) )
    {
        CHAR str[100];
        sprintf( str, "Couldn't open video file %s.\n", strFilename );
        OUTPUT_DEBUG_STRING( str );
        return E_FAIL;
    }

    // Get information about the video
    REFERENCE_TIME rtPreroll;
    m_pWMVDecoder->GetVideoInfo( &m_wmvVideoInfo );
    m_pWMVDecoder->GetAudioInfo( &m_wfx );
    m_pWMVDecoder->GetPlayDuration( &m_rtDuration, &rtPreroll );
    m_rtDuration -= rtPreroll;

    // Create our video frames
    for( int i = 0; i < 2; i++ )
    {
        m_pd3dDevice->CreateTexture( m_wmvVideoInfo.dwWidth,
                                     m_wmvVideoInfo.dwHeight,
                                     0,
                                     0,
                                     D3DFMT_LIN_X8R8G8B8,
                                     NULL,
                                     &m_aVideoFrames[i].pTexture );

        D3DLOCKED_RECT lr;
        m_aVideoFrames[i].pTexture->LockRect( 0, &lr, NULL, 0 );
        m_aVideoFrames[i].pBits = (BYTE *)lr.pBits;
        m_aVideoFrames[i].pTexture->UnlockRect( 0 );
        m_aVideoFrames[i].rtTimestamp = 0;
    }
    
    // Create a stream for audio playback
    DSSTREAMDESC dssd = {0};
    dssd.dwFlags = 0;
    dssd.dwMaxAttachedPackets = NUM_PACKETS;
    dssd.lpwfxFormat = &m_wfx;

    DirectSoundCreateStream( &dssd, &m_pStream );
    m_pStream->Pause( DSSTREAMPAUSE_PAUSE );

    // Allocate sample data
    m_pbSampleData = new BYTE[ NUM_PACKETS * PACKET_SIZE ];

    for( int j = 0; j < NUM_PACKETS; j++ )
        m_adwStatus[ j ] = XMEDIAPACKET_STATUS_SUCCESS;
    
    // Create a vertex buffer for the quad
    FLOAT fLeft = 320.0f - m_wmvVideoInfo.dwWidth / 2;
    FLOAT fRight = fLeft + m_wmvVideoInfo.dwWidth;
    FLOAT fTop = 240.0f - m_wmvVideoInfo.dwHeight / 2;
    FLOAT fBottom = fTop + m_wmvVideoInfo.dwHeight;

    m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( CUSTOMVERTEX ), 0, 0, 0, &m_pvbQuad );
    CUSTOMVERTEX * pVertices;
    m_pvbQuad->Lock( 0, 0, (BYTE **)&pVertices, 0 );
    pVertices[ 0 ].p = D3DXVECTOR4(  fLeft-0.5f,    fTop-0.5f, 1.0f, 1.0f ); pVertices[0].t = D3DXVECTOR2( 0.0f, 0.0f );
    pVertices[ 1 ].p = D3DXVECTOR4( fRight-0.5f,    fTop-0.5f, 1.0f, 1.0f ); pVertices[1].t = D3DXVECTOR2( (FLOAT)m_wmvVideoInfo.dwWidth, 0.0f );
    pVertices[ 2 ].p = D3DXVECTOR4( fRight-0.5f, fBottom-0.5f, 1.0f, 1.0f ); pVertices[2].t = D3DXVECTOR2( (FLOAT)m_wmvVideoInfo.dwWidth, (FLOAT)m_wmvVideoInfo.dwHeight );
    pVertices[ 3 ].p = D3DXVECTOR4(  fLeft-0.5f, fBottom-0.5f, 1.0f, 1.0f ); pVertices[3].t = D3DXVECTOR2( 0.0f, (FLOAT)m_wmvVideoInfo.dwHeight );
    m_pvbQuad->Unlock();

#if RECORDAUDIO
    CHAR strPCMFile[MAX_FILE];
    sprintf( strPCMFile, "%s.pcm", strFilename );
    m_hAudioFile = CreateFile( strPCMFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#endif

    m_bGotVideo = FALSE;

    return S_OK;
}



HRESULT CXBoxSample::CleanupDecoder()
{
    if( m_pWMVDecoder )
    {
        m_pWMVDecoder->Release();
        m_pWMVDecoder = NULL;
    }

    for( int i = 0; i < 2; i++ )
    {
        if( m_aVideoFrames[i].pTexture )
        {
            m_aVideoFrames[i].pTexture->Release();
            m_aVideoFrames[i].pTexture = NULL;
            m_aVideoFrames[i].pBits    = NULL;
        }
    }

    if( m_pStream )
    {
        m_pStream->Release();
        m_pStream = NULL;
    }

    if( m_pbSampleData )
    {
        delete[] m_pbSampleData;
        m_pbSampleData = NULL;
    }

    if( m_pvbQuad )
    {
        m_pvbQuad->Release();
        m_pvbQuad = NULL;
    }

#if RECORDAUDIO
    if( m_hAudioFile != INVALID_HANDLE_VALUE )
    {
        CloseHandle( m_hAudioFile );
        m_hAudioFile = INVALID_HANDLE_VALUE;
    }
#endif

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Initialize()
{
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( DirectSoundCreate( NULL, &m_pDSound, NULL ) ) )
        return E_FAIL;

    if( FAILED( PrepareDecoder() ) )
        return E_FAIL;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FindFreePacket
// Desc: Attempts to find a free audio packet (one that isn't currently 
//       submitted to the stream).  Returns TRUE if it found one
//-----------------------------------------------------------------------------
BOOL
CXBoxSample::FindFreePacket( DWORD * pdwIndex )
{
    // Check the status of each packet
    for( int i = 0; i < NUM_PACKETS; i++ )
    {
        // If we find a non-pending packet, return it
        if( m_adwStatus[ i ] != XMEDIAPACKET_STATUS_PENDING )
        {
            *pdwIndex = i;
            return TRUE;
        }
    }

    return FALSE;
}


#if DUMP
void CXBoxSample::DumpAudio( XMEDIAPACKET& xmp )
{
    static DWORD dwPacket = 0;
    static FLOAT fTotal = 0.0f;
    CHAR sz[100];
    fTotal += 1000.0f * *xmp.pdwCompletedSize / (float)m_wfx.nAvgBytesPerSec;
    sprintf( sz, "Audio [%d]: %d bytes = %.1fms\ttimestamp: %I64d(%.1fms)\tTotal: %.1fms\n", 
             dwPacket++,
             *xmp.pdwCompletedSize,
             1000.0f * *xmp.pdwCompletedSize / (float)m_wfx.nAvgBytesPerSec,
             *xmp.prtTimestamp,
             *xmp.prtTimestamp / 10000.0f,
             fTotal );
    OUTPUT_DEBUG_STRING( sz );
}

void CXBoxSample::DumpVideo( XMEDIAPACKET& xmp )
{
    static DWORD dwPacket = 0;
    CHAR sz[100];
    sprintf( sz, "\tVideo [%d]: timestamp: %I64d(%.1fms)\n", dwPacket++, *xmp.prtTimestamp, *xmp.prtTimestamp / 10000.0f );
    OUTPUT_DEBUG_STRING( sz );
}
#endif


//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::FrameMove()
{
    HRESULT hr;

    DirectSoundDoWork();
    static bool bSkip = TRUE;

    // This will generally not be necessary.  This is to get around the CPU
    // stall that happens on the very first call to present().  We don't want
    // to start our audio before that, otherwise we might glitch.
    if( bSkip )
    {
        bSkip = FALSE;
        return S_OK;
    }


    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    // If the timestamp of the frame we're displaying is greater than
    // the timestamp of the other frame, then we can fill the other
    // frame.  
    do
    {
        if( m_bGotVideo )
        {
            // Update timing
            m_pDSound->GetTime( &m_rtCurrent );
            m_rtCurrent -= m_rtStart;
#if VIDEO
            if( m_rtCurrent >= m_aVideoFrames[ !m_dwDisplayFrame ].rtTimestamp )
                m_dwDisplayFrame ^= 1;
#endif
#if DUMP
            CHAR sz[100];
            sprintf( sz, "Time is: %.1fms\n", m_rtCurrent / 10000.0f );
            OUTPUT_DEBUG_STRING( sz );
#endif // DUMP
    
        }

        DWORD dwConsecutiveAudio = 0;
        BOOL  bCanDecodeVideo = FALSE;
        DWORD dwIndex;

        // Audio packet
        DWORD audStat, audSize = 0;
        XMEDIAPACKET xmpAudio = {0};
        REFERENCE_TIME rtAudio;

        // Video packet
        DWORD vidStat, vidSize = 0;
        XMEDIAPACKET xmpVideo = {0};
        REFERENCE_TIME rtVideo;
        XMEDIAPACKET* pxmpVideo = NULL;

        // See if we have space to decode video
        if( m_aVideoFrames[ m_dwDisplayFrame ].rtTimestamp >=
            m_aVideoFrames[ !m_dwDisplayFrame ].rtTimestamp )
        {
            bCanDecodeVideo = TRUE;
#if VIDEO
             while( m_aVideoFrames[ !m_dwDisplayFrame ].pTexture->IsBusy() );
#endif
            pxmpVideo = &xmpVideo;
        }

        // Verify that we can decode audio
        BOOL bCanDecodeAudio = FindFreePacket( &dwIndex );
        assert( bCanDecodeAudio );

        xmpAudio.dwMaxSize        = PACKET_SIZE;
        xmpAudio.pvBuffer         = m_pbSampleData + dwIndex * PACKET_SIZE;
        xmpAudio.pdwStatus        = &audStat;
        xmpAudio.pdwCompletedSize = &audSize;
        xmpAudio.prtTimestamp     = &rtAudio;

        xmpVideo.dwMaxSize        = m_wmvVideoInfo.dwWidth *
                                    m_wmvVideoInfo.dwHeight *
                                    m_wmvVideoInfo.dwOutputBitsPerPixel / 8;
        xmpVideo.pdwStatus        = &vidStat;
        xmpVideo.pdwCompletedSize = &vidSize;
        xmpVideo.prtTimestamp     = &rtVideo;
        xmpVideo.pvBuffer         = m_aVideoFrames[ !m_dwDisplayFrame ].pBits;

        hr = m_pWMVDecoder->ProcessMultiple( pxmpVideo, &xmpAudio );
        if( S_FALSE == hr )
        {
            OUTPUT_DEBUG_STRING( "Done!\n" );
#if RECORDAUDIO
            if( m_hAudioFile != INVALID_HANDLE_VALUE )
                CloseHandle( m_hAudioFile );
#endif
            m_pStream->Discontinuity();

            do
            {
                CleanupDecoder();
                hr = PrepareDecoder();
            } while( FAILED( hr ) );

            // Spin
            // while( 1 );
        }

        if( vidSize > 0 )
        {
#if DUMP
            DumpVideo( xmpVideo );
#endif // DUMP
            static DWORD dwFrame = 0;
            
            ++dwFrame;
            m_aVideoFrames[ !m_dwDisplayFrame ].rtTimestamp = rtVideo;
            if( !m_bGotVideo )
            {
                m_bGotVideo = TRUE;
                m_pStream->Pause( DSSTREAMPAUSE_RESUME );
                m_pDSound->GetTime( &m_rtStart );
            }
        }

        if( audSize > 0 )
        {
#if DUMP
            XMEDIAPACKET xmpClone = xmpAudio;
#endif // DUMP
#if RECORDAUDIO
            if( m_hAudioFile != INVALID_HANDLE_VALUE )
            {
                DWORD dw;
                WriteFile( m_hAudioFile, xmpAudio.pvBuffer, audSize, &dw, NULL );
            }
#endif
            ++dwConsecutiveAudio;
            xmpAudio.dwMaxSize        = audSize;
            xmpAudio.pdwCompletedSize = NULL;
            xmpAudio.pdwStatus        = &m_adwStatus[ dwIndex ];
            xmpAudio.prtTimestamp     = NULL;
#if AUDIO
            m_pStream->Process( &xmpAudio, NULL );
#endif
#if DUMP
            DumpAudio( xmpClone );
#endif // DUMP
        }
    } while( DS_OK == hr );
    
/*    if( E_PENDING == hr )
        OUTPUT_DEBUG_STRING( "E_PENDING\n" );
    else
        OUTPUT_DEBUG_STRING( "Bailed for some other reason?" );*/

    assert( m_aVideoFrames[ m_dwDisplayFrame ].rtTimestamp <=
            m_aVideoFrames[ !m_dwDisplayFrame ].rtTimestamp );
    assert( m_aVideoFrames[ m_dwDisplayFrame ].rtTimestamp <= m_rtCurrent );
//    assert( m_aVideoFrames[ !m_dwDisplayFrame ].rtTimestamp >= m_rtCurrent );

    DWORD dwTimeToSleep = DWORD( ( m_aVideoFrames[ !m_dwDisplayFrame ].rtTimestamp - m_rtCurrent ) / 10000 );
    if( dwTimeToSleep < 6 )
        dwTimeToSleep = 0;
    else
        dwTimeToSleep -= 6;
#if 0 // DUMP
    CHAR sz[100];
    sprintf( sz, "->Current: %ldms, next: %ldms, sleeping for %ldms\n", DWORD(m_rtCurrent / 10000), DWORD(m_aVideoFrames[ !m_dwDisplayFrame ].rtTimestamp / 10000), dwTimeToSleep );
    OUTPUT_DEBUG_STRING( sz );
#endif
    // Sleep( dwTimeToSleep );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Render()
{
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 );

    // Set up for rendering
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTexture( 0, m_aVideoFrames[ m_dwDisplayFrame ].pTexture );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetStreamSource( 0, m_pvbQuad, sizeof( CUSTOMVERTEX ) );
    m_pd3dDevice->SetVertexShader( FVF_CUSTOMVERTEX );

    // Render the quad
    m_pd3dDevice->DrawVertices( D3DPT_QUADLIST, 0, 4 );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"SimpleWMV" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

