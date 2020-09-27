//-----------------------------------------------------------------------------
// File: WMVPlayBack.cpp
//
// Desc: Demonstrates how to use the WMVDecoder XMO to play streaming video
//
// Hist: 7.6.01 - New for September XDK
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>
#include <wmvxmo.h>
#include <assert.h>

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle\ndebug output" },
};

#define NUM_HELP_CALLOUTS 2

// Vertex definition for the screen-space quad
typedef struct {
    D3DXVECTOR4 p;
    D3DXVECTOR2 t;
} CUSTOMVERTEX;
#define FVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_TEX1)

// Define audio packet params
#define NUM_PACKETS 48
#define PACKET_SIZE 4096

// Define a video frame structure:
typedef struct _VIDEOFRAME {
    struct _VIDEOFRAME *pNext;          // Next pending video frame
    LPDIRECT3DTEXTURE8  pTexture;       // Texture data
    REFERENCE_TIME      rtTimeStamp;    // Time stamp of video frame
    DWORD               dwSetAtVBlank;  // VBlank count at which frame was started
    BYTE *              pBits;          // Pointer to texture data
    DWORD               dwFrameNumber;  // Frame sequence number (debug)
} VIDEOFRAME;

// Number of video frames to buffer
#define NUM_FRAMES 8

// Maximum # of video frames to decode per frame
#define MAXDECODE_PER_FRAME 3

static DWORD g_dwVidFrame = 0;

// Set this to TRUE to render using the overlay
static const BOOL g_bOverlay = FALSE;

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

    BOOL FindFreePacket( DWORD * pdwIndex );    // Find a free audio packet
    BOOL FindFreeFrame( VIDEOFRAME ** ppFrame );// Find a free video frame
    
    HRESULT ProcessVideoQueues();               // Processes our video queues

    // Font and help
    CXBFont     m_Font;
    CXBHelp     m_Help;
    BOOL        m_bDrawHelp;

    BOOL        m_bDebugInfo;                   // Show video debug info?

    HRESULT         m_hrOpenFile;               // Result from creating decoder
    BOOL            m_bDecode;                  // Set to FALSE at end of video
    LPWMVDECODER    m_pWMVDecoder;              // WMV Decoder
    WMVVIDEOINFO    m_wmvVideoInfo;             // Video info
    REFERENCE_TIME  m_rtDuration;               // Duration of video
    WAVEFORMATEX    m_wfx;                      // Audio format

    DWORD                   m_adwStatus[NUM_PACKETS];   // Audio packet status
    BYTE *                  m_pbSampleData;             // Audio sample data
    LPDIRECTSOUNDSTREAM     m_pStream;                  // DirectSound Stream

    VIDEOFRAME              m_aVideoFrames[NUM_FRAMES]; // Video frames
    VIDEOFRAME *            m_pCurrentFrame;            // Frame to render
    LPDIRECT3DVERTEXBUFFER8 m_pvbQuad;                  // Quad for rendering video

    BOOL            m_bGotData;             // TRUE after we've gotten a packet
    LPDIRECTSOUND8  m_pDSound;              // DirectSound object
    REFERENCE_TIME  m_rtStartTime;          // Reference time at start
    REFERENCE_TIME  m_rtCurrentTime;        // Reference time of current frame
    REFERENCE_TIME  m_rtPreroll;            // Preroll time

    //-------------------------------------------------------------------------
    // Name: class CFrameQueue
    // Desc: Maintains a queue of video frames
    //-------------------------------------------------------------------------
    class CFrameQueue
    {
    public:
        VIDEOFRAME * m_pHead;
        VIDEOFRAME * m_pTail;

        CFrameQueue() { m_pHead = m_pTail = NULL; }
        BOOL IsEmpty() { return m_pHead == NULL; }
        void Enqueue( VIDEOFRAME * pFrame );
        VIDEOFRAME * Dequeue(); 
    };

    CFrameQueue m_fqFree;       // Free frames - these can be filled
    CFrameQueue m_fqBusy;       // Busy frames - possibly being rendered
    CFrameQueue m_fqReady;      // Ready frames - filled, and can be displayed
};


//-----------------------------------------------------------------------------
// Name: VBlankCallback
// Desc: Called every vblank, so we can mantain a count of vblanks
//-----------------------------------------------------------------------------
DWORD g_dwVBlankCount = 0;
void __cdecl VBlankCallback( D3DVBLANKDATA *pData )
{
    g_dwVBlankCount++;
}



//-----------------------------------------------------------------------------
// Name: FrameQueue::Enqueue
// Desc: Adds a frame to tail of queue
//-----------------------------------------------------------------------------
void
CXBoxSample::CFrameQueue::Enqueue( VIDEOFRAME * pFrame )
{ 
    pFrame->pNext = NULL;

    if( m_pTail ) 
    {
        m_pTail->pNext = pFrame;
        m_pTail = pFrame;
    }
    else
        m_pHead = m_pTail = pFrame;
}



//-----------------------------------------------------------------------------
// Name: FrameQueue::Dequeue
// Desc: Returns the head of the queue
//-----------------------------------------------------------------------------
VIDEOFRAME *
CXBoxSample::CFrameQueue::Dequeue()
{
    assert( m_pHead );
    VIDEOFRAME * pPacket = m_pHead;

    m_pHead = m_pHead->pNext;
    if( !m_pHead )
        m_pTail = NULL;

    pPacket->pNext = NULL;
    return pPacket;
}


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
    m_bDrawHelp = FALSE;
    m_bDebugInfo = TRUE;
}



//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Initialize()
{
    m_bGotData = FALSE;
    m_bDecode = TRUE;

    // If we're using the overlay, enable it
    if( g_bOverlay )
        m_pd3dDevice->EnableOverlay( TRUE );

    // Initialize our timing values
    m_rtStartTime = 0;
    m_pCurrentFrame = NULL;
    m_pd3dDevice->SetVerticalBlankCallback( VBlankCallback );

    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    DirectSoundCreate( NULL, &m_pDSound, NULL );

    // Create the video decoder.  We only load
    // the section containing the decoder on demand
    XLoadSection("WMVDEC");
    m_hrOpenFile = WmvCreateDecoder( "D:\\Media\\Videos\\bike.wmv",
                                     NULL,
                                     WMVVIDEOFORMAT_YUY2,
                                     NULL,
                                     &m_pWMVDecoder );

    // If that succeeded, set up for decoding:
    if( SUCCEEDED( m_hrOpenFile ) )
    {
        // Get info about audio and video
        m_pWMVDecoder->GetVideoInfo( &m_wmvVideoInfo );
        m_pWMVDecoder->GetAudioInfo( &m_wfx );
        m_pWMVDecoder->GetPlayDuration( &m_rtDuration, &m_rtPreroll );
        m_rtDuration -= m_rtPreroll;

        // Set up video frame buffers
        for( int i = 0; i < NUM_FRAMES; i++ )
        {
            // Create the texture
            m_pd3dDevice->CreateTexture(
                m_wmvVideoInfo.dwWidth,
                m_wmvVideoInfo.dwHeight,
                0,
                0,
                D3DFMT_YUY2,
                NULL,
                &m_aVideoFrames[i].pTexture );

            // Get a pointer to the texture data
            D3DLOCKED_RECT lr;
            m_aVideoFrames[i].pTexture->LockRect( 0, &lr, NULL, 0 );
            m_aVideoFrames[i].pBits = (BYTE *)lr.pBits;
            m_aVideoFrames[i].pTexture->UnlockRect( 0 );

            m_aVideoFrames[i].dwSetAtVBlank = 0;
            m_fqFree.Enqueue( &m_aVideoFrames[i] );
        }

        // Set up a stream for audio playback
        DSSTREAMDESC dssd = {0};
        dssd.dwFlags =  0;
        dssd.dwMaxAttachedPackets = NUM_PACKETS;
        dssd.lpwfxFormat = &m_wfx;

        DirectSoundCreateStream( &dssd, &m_pStream );

        // Allocate sample data
        m_pbSampleData = new BYTE[ NUM_PACKETS * PACKET_SIZE ];

        // Initialize audio packet status
        for( int i = 0; i < NUM_PACKETS; i++ )
            m_adwStatus[ i ] = XMEDIAPACKET_STATUS_SUCCESS;
    }

    if( !g_bOverlay )
    {
        // Create a vertex buffer for the quad
        FLOAT fLeft   = 320.0f - m_wmvVideoInfo.dwWidth / 2;
        FLOAT fRight  = fLeft + m_wmvVideoInfo.dwWidth;
        FLOAT fTop    = 240.0f - m_wmvVideoInfo.dwHeight / 2;
        FLOAT fBottom = fTop + m_wmvVideoInfo.dwHeight;

        m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( CUSTOMVERTEX ), 0, 0, 0, &m_pvbQuad );
        CUSTOMVERTEX * pVertices;
        m_pvbQuad->Lock( 0, 0, (BYTE **)&pVertices, 0 );
        pVertices[ 0 ].p = D3DXVECTOR4(  fLeft-0.5f,    fTop-0.5f, 1.0f, 1.0f ); pVertices[0].t = D3DXVECTOR2( 0.0f, 0.0f );
        pVertices[ 1 ].p = D3DXVECTOR4( fRight-0.5f,    fTop-0.5f, 1.0f, 1.0f ); pVertices[1].t = D3DXVECTOR2( (FLOAT)m_wmvVideoInfo.dwWidth, 0.0f );
        pVertices[ 2 ].p = D3DXVECTOR4( fRight-0.5f, fBottom-0.5f, 1.0f, 1.0f ); pVertices[2].t = D3DXVECTOR2( (FLOAT)m_wmvVideoInfo.dwWidth, (FLOAT)m_wmvVideoInfo.dwHeight );
        pVertices[ 3 ].p = D3DXVECTOR4(  fLeft-0.5f, fBottom-0.5f, 1.0f, 1.0f ); pVertices[3].t = D3DXVECTOR2( 0.0f, (FLOAT)m_wmvVideoInfo.dwHeight );
        m_pvbQuad->Unlock();
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ProcessVideoQueues()
// Desc: Processes our video queues:
//       1) Free up any busy frames that we know are no longer in use.
//          For overlay, we know a frame is no longer in use if we're at
//          least one VBlank after updating with the next frame.
//          For textures, we could have up to 3 frames in the pushbuffer,
//          so we know a frame is no longer in use if we're at least 3 VBlanks
//          after rendering the next frame
//       2) Discard any frames from the ready queue that are obsolete.  A
//          frame is obsolete if the frame after it is displayable.
//       3) Figure out if we need to render a new frame - that is, is there
//          a frame in the ready queue with a timestamp less than current time
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::ProcessVideoQueues()
{
    // Step 1: Move packets from busy->free if they're not being used.
    // The only time the busy queue should be empty is starting up
    while( !m_fqBusy.IsEmpty() )
    {
        // If frame Y was submitted after frame X, and we're sure
        // frame Y is the one being displayed, free frame X
        if( m_fqBusy.m_pHead->pNext &&
            m_fqBusy.m_pHead->pNext->dwSetAtVBlank + ( g_bOverlay ? 0 : 2 )  < g_dwVBlankCount )
        {
            m_fqFree.Enqueue( m_fqBusy.Dequeue() );
        }
        else
            break;
    }

    // Step 2: Discard dropped frames.  We drop frame X if there's 
    // frame Y after it, and frame Y is displayable
    while( !m_fqReady.IsEmpty() && 
            m_fqReady.m_pHead->pNext &&
            m_fqReady.m_pHead->pNext->rtTimeStamp <= m_rtCurrentTime )
    {
        CHAR sz[100];
        sprintf( sz, "Dropped frame %d\n", m_fqReady.m_pHead->dwFrameNumber );
        OUTPUT_DEBUG_STRING( sz );
        m_fqFree.Enqueue( m_fqReady.Dequeue() );
    }

    // Step 3: Now we have at most 1 frame starting before current time
    // See if he should be submitted to render
    if( !m_fqReady.IsEmpty() &&
        m_fqReady.m_pHead->rtTimeStamp <= m_rtCurrentTime )
    {
        // Render the packet
        m_pCurrentFrame = m_fqReady.Dequeue();
        m_fqBusy.Enqueue( m_pCurrentFrame );
    }

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



//-----------------------------------------------------------------------------
// Name: FindFreeFrame
// Desc: Attempts to find an available video frame (one that is not in the
//          pending video frame queue).  Returns TRUE if one was available
//-----------------------------------------------------------------------------
BOOL
CXBoxSample::FindFreeFrame( VIDEOFRAME ** ppFrame )
{
    // Do we have any frames in the free queue?
    if( !m_fqFree.IsEmpty() )
    {
        *ppFrame = m_fqFree.m_pHead;
        return TRUE;
    }

    return FALSE;
}



//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::FrameMove()
{
    DWORD dwIndex;
    VIDEOFRAME *pFrame;

    // Toggle debug info on/off
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
        m_bDebugInfo = !m_bDebugInfo;

    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    // Set our time for this frame:
    // Video Time = Current Time - Start Time
    m_pDSound->GetTime( &m_rtCurrentTime );
    m_rtCurrentTime = m_rtCurrentTime - m_rtStartTime;
    
    // Process our 3 video queues
    ProcessVideoQueues();

    // If we are actively decoding...
    if( SUCCEEDED( m_hrOpenFile ) && m_bDecode )
    {
        DWORD dwVideoFrames = 0;

        // Find an audio packet and a video frame
        while( dwVideoFrames < MAXDECODE_PER_FRAME && FindFreePacket( &dwIndex ) && FindFreeFrame( &pFrame ) )
        {
            //
            // Set up an audio packet:
            //
            DWORD audStat, audSize;
            XMEDIAPACKET xmpAudio = {0};
            REFERENCE_TIME rtAudio;

            xmpAudio.dwMaxSize = PACKET_SIZE;
            xmpAudio.pvBuffer  = m_pbSampleData + dwIndex * PACKET_SIZE;
            xmpAudio.pdwStatus = &audStat;
            xmpAudio.pdwCompletedSize = &audSize;
            xmpAudio.prtTimestamp = &rtAudio;

            //
            // Set up a video packet
            //
            DWORD vidStat, vidSize;
            XMEDIAPACKET xmpVideo = {0};
            REFERENCE_TIME rtVideo;

            xmpVideo.dwMaxSize = m_wmvVideoInfo.dwWidth * 
                                 m_wmvVideoInfo.dwHeight * 
                                 m_wmvVideoInfo.dwOutputBitsPerPixel / 8;
            xmpVideo.pdwStatus = &vidStat;
            xmpVideo.pdwCompletedSize = &vidSize;
            xmpVideo.prtTimestamp = &rtVideo;

            // We shouldn't have put the frame in the free queue
            // until we were sure the next one was being used
            assert( !pFrame->pTexture->IsBusy() );

            xmpVideo.pvBuffer  = pFrame->pBits;
        
            // Process the decoder
            HRESULT hr = m_pWMVDecoder->ProcessMultiple( &xmpVideo, &xmpAudio );

            // S_FALSE means that we hit the end of the video
            if( S_FALSE == hr )
            {
                m_bDecode = FALSE;
                m_pStream->Discontinuity();
                OUTPUT_DEBUG_STRING( "Done playing video\n" );

                // Now that we're done with the video, we can
                // unload the decoder
                XFreeSection("WMVDEC");
                break;
            }

            // Get the system start time as of first packet, so 
            // that we can calculate the reference time 
            if( !m_bGotData && ( audSize > 0 || vidSize > 0 ) )
            {
                m_bGotData = TRUE;
                m_pDSound->GetTime( &m_rtStartTime );
                m_rtCurrentTime = 0;
            }
            
            // If we got a video frame, append it to the queue
            if( vidSize > 0 )
            {
                ++dwVideoFrames;

                if( rtVideo < m_rtDuration )
                {
                    // Update the pending video frame queue
                    pFrame->rtTimeStamp = rtVideo;
                    pFrame->dwFrameNumber = g_dwVidFrame++;
                    m_fqReady.Enqueue( m_fqFree.Dequeue() );
                }
            }

            // If we got an audio packet, send it to the stream
            if( audSize > 0 )
            {
                // Process the audio packet
                xmpAudio.dwMaxSize = audSize;
                xmpAudio.pdwCompletedSize = NULL;
                xmpAudio.pdwStatus = &( m_adwStatus[ dwIndex ] );
                xmpAudio.prtTimestamp = NULL;
                m_pStream->Process( &xmpAudio, NULL );
            }
        }
    }
    
    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Render()
{
    DirectSoundDoWork();

    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 );

    if( m_pCurrentFrame )
    {
        if( g_bOverlay )
        {
            RECT rcSrc = { 0, 0, m_wmvVideoInfo.dwWidth, m_wmvVideoInfo.dwHeight };
            RECT rcDest = { 320 - m_wmvVideoInfo.dwWidth / 2, 240 - m_wmvVideoInfo.dwHeight / 2, 320 + m_wmvVideoInfo.dwWidth / 2, 240 + m_wmvVideoInfo.dwHeight / 2 };
            LPDIRECT3DSURFACE8 pSurface;

            // Update the overlay with the current frame
            m_pCurrentFrame->pTexture->GetSurfaceLevel( 0, &pSurface );
            m_pd3dDevice->UpdateOverlay( pSurface, &rcSrc, &rcDest, FALSE, 0 );
            pSurface->Release();
        }
        else
        {
            // Set up for rendering
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
            m_pd3dDevice->SetTexture( 0, m_pCurrentFrame->pTexture );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
            m_pd3dDevice->SetStreamSource( 0, m_pvbQuad, sizeof( CUSTOMVERTEX ) );
            m_pd3dDevice->SetVertexShader( FVF_CUSTOMVERTEX );
            m_pd3dDevice->SetRenderState( D3DRS_YUVENABLE, TRUE );

            // Render the quad
            m_pd3dDevice->DrawVertices( D3DPT_QUADLIST, 0, 4 );

            m_pd3dDevice->SetRenderState( D3DRS_YUVENABLE, FALSE );
        }

        // Remember when we attempted to render this frame.
        // The time it actually gets rendered depends on the method
        // For overlay: We can be sure it's been rendered after next vblank
        // For texture: We can be sure it's been rendered 3 vblanks later
        m_pCurrentFrame->dwSetAtVBlank = g_dwVBlankCount;
    }

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"WMVPlayBack" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        if( FAILED( m_hrOpenFile ) )
        {
            m_Font.DrawText( 64, 80, 0xFFFF0000, L"Couldn't open WMV file" );
        }
        else
        {
            // Display some useful info on video playback
            if( m_bDebugInfo )
            {
                WCHAR sz[100];

                swprintf( sz, L"Video is %d x %d", m_wmvVideoInfo.dwWidth, m_wmvVideoInfo.dwHeight );
                m_Font.DrawText( 64, 80, 0xffffff00, sz );
                swprintf( sz, L"%d bits per second", m_wmvVideoInfo.dwBitsPerSecond );
                m_Font.DrawText( 64, 110, 0xffffff00, sz );

                // If we're still decoding, show some statistics
                if( m_bDecode )
                {
                    swprintf( sz, L"Position: %0.1fs / %0.1fs", m_rtCurrentTime / 10000000.0f, m_rtDuration / 10000000.0f );
                    m_Font.DrawText( 64, 140, 0xffffff00, sz );

                    // See how we're doing on audio packets
                    DWORD dwPending = 0;
                    DWORD dwSucceeded = 0;
                    for( int i = 0; i < NUM_PACKETS; i++ )
                    {
                        switch( m_adwStatus[ i ] )
                        {
                        case XMEDIAPACKET_STATUS_PENDING:
                            dwPending++;
                            break;
                        case XMEDIAPACKET_STATUS_SUCCESS:
                            dwSucceeded++;
                            break;
                        }
                    }
                    swprintf( sz, L"Audio - P: %d S: %d", dwPending, dwSucceeded );
                    m_Font.DrawText( 64, 170, 0xffffff00, sz );

                    // Now check video frames
                    DWORD dwBusy = 0;
                    dwPending = dwSucceeded = 0;
                    for( VIDEOFRAME * pFrame = m_fqFree.m_pHead; pFrame; pFrame = pFrame->pNext )
                        dwSucceeded++;
                    for( VIDEOFRAME * pFrame = m_fqReady.m_pHead; pFrame; pFrame = pFrame->pNext )
                        dwPending++;
                    for( VIDEOFRAME * pFrame = m_fqBusy.m_pHead; pFrame; pFrame = pFrame->pNext )
                        dwBusy++;

                    swprintf( sz, L"Video - F: %d R: %d B: %d", dwSucceeded, dwPending, dwBusy );
                    m_Font.DrawText( 64, 200, 0xffffff00, sz );
                }
            }
        }

        m_Font.End();
    }

    if( !g_bOverlay )
        m_pd3dDevice->BlockUntilVerticalBlank();

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

