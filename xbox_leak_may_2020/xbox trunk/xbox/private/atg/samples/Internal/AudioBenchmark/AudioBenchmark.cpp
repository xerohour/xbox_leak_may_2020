//-----------------------------------------------------------------------------
// File: AudioBenchmark.cpp
//
// Desc: [TODO: Describe source file here]
//
// Hist: 8.2.01 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>
#include "dsstdfx.h"
#include <vtuneapi.h>

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Run test" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle playback" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Reinitialize\nbuffers" },
};

#define NUM_HELP_CALLOUTS ( sizeof( g_HelpCallouts ) / sizeof( g_HelpCallouts[0] ) )

#define NUM_3D_BUFFERS 64
#define DSBCAPS DSBCAPS_CTRL3D

BOOL g_bTesting = FALSE;
#define BEFORETEST( xxTest ) \
    g_bTesting = TRUE; \
    LARGE_INTEGER liBefore, liAfter; \
    QueryPerformanceCounter( &liBefore ); \

#define AFTERTEST( xxTest ) \
    QueryPerformanceCounter( &liAfter ); \
    g_aTests[ xxTest ].fTime = ( liAfter.QuadPart - liBefore.QuadPart ) * m_fMSPerTick; \
    g_bTesting = FALSE; \

#define FLUSH_BEFORE_CALL 0
#if FLUSH_BEFORE_CALL
#define FLUSH_PIO() \
    while( *(volatile DWORD *)0xfe820010 < 0x80 );
#else
#define FLUSH_PIO()
#endif


#define RECORDTEST( xxTest ) g_aTests[ xxTest ].fTime = ( liAfter.QuadPart - liBefore.QuadPart ) * m_fMSPerTick;
#define DSoundInitHRTF() DirectSoundUseFullHRTF()

HRESULT CreateSineWaveBuffer( double dFrequency, LPDIRECTSOUNDBUFFER8 * ppBuffer );

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
    HRESULT DownloadEffectsImage(PCHAR pszScratchFile);  // downloads a default DSP image to the GP

    HRESULT SetCurrentPosition();
    HRESULT PlayAll();
    HRESULT StopAll();
    HRESULT InitializeBuffers();

    HRESULT PositionAllImmediate();
    HRESULT VelocityAllImmediate();
    HRESULT PositionListenerImmediate();
    HRESULT VelocityListenerImmediate();

    HRESULT PositionAll();
    HRESULT VelocityAll();
    HRESULT PositionListener();
    HRESULT VelocityListener();
    HRESULT Commit3D();

    HRESULT SetFrequency();
    HRESULT SetVolume();
    HRESULT SetPitch();
    HRESULT SetEG();
    HRESULT SetFilter();

    // Font and help
    CXBFont     m_Font;
    CXBHelp     m_Help;

    BOOL        m_bDrawHelp;

    LPDIRECTSOUND8  m_pDSound;
    LPDIRECTSOUNDBUFFER8 m_apBuffers[ NUM_3D_BUFFERS ];
    BOOL        m_bPlaying;
    DWORD       m_dwTest;

    D3DXVECTOR3 m_avPositions[ NUM_3D_BUFFERS ];
    D3DXVECTOR3 m_avVelocities[ NUM_3D_BUFFERS ];

    D3DXVECTOR3 m_vListenerPosition;
    D3DXVECTOR3 m_vListenerVelocity;

    FLOAT       m_fMSPerTick;
};

typedef enum {
    AB_SETCURRENTPOSITION,
    AB_PLAY,
    AB_STOP,
    AB_RELEASE,
    AB_POSITIONSOURCE,
    AB_VELOCITYSOURCE,
    AB_POSITIONLISTENER,
    AB_VELOCITYLISTENER,
    AB_POSITIONSOURCE_DEFERRED,
    AB_VELOCITYSOURCE_DEFERRED,
    AB_POSITIONLISTENER_DEFERRED,
    AB_VELOCITYLISTENER_DEFERRED,
    AB_COMMIT3D,
    AB_FREQUENCY,
    AB_VOLUME,
    AB_PITCH,
    AB_ENVELOPE,
    AB_SETFILTER,

    AB_MAX
} AB_TESTENUM;

typedef HRESULT (CXBoxSample::* TESTFUNCTION)();
typedef struct {
    WCHAR *         szDescription;
    FLOAT           fTime;
    TESTFUNCTION    pfn;
} AB_TEST;

static AB_TEST g_aTests[] =
{
    { L"SetCurrentPosition", 0.0f, CXBoxSample::SetCurrentPosition },               // AB_SETCURRENTPOSITION
    { L"Play", 0.0f, CXBoxSample::PlayAll },                                        // AB_PLAY
    { L"Stop", 0.0f, CXBoxSample::StopAll },                                        // AB_STOP
    { L"Release", 0.0f, CXBoxSample::InitializeBuffers },                           // AB_RELEASE
    { L"Position sources immediate", 0.0f, CXBoxSample::PositionAllImmediate },     // AB_POSITIONSOURCE
    { L"Velocity sources immediate", 0.0f, CXBoxSample::VelocityAllImmediate },     // AB_VELOCITYSOURCE
    { L"Position listener immediate", 0.0f, CXBoxSample::PositionListenerImmediate },   // AB_POSITIONLISTENER
    { L"Velocity listener immediate", 0.0f, CXBoxSample::VelocityListenerImmediate },   // AB_VELOCITYLISTENER
    { L"Position sources deferred", 0.0f, CXBoxSample::PositionAll },               // AB_POSITIONSOURCE_DEFERRED
    { L"Velocity sources deferred", 0.0f, CXBoxSample::VelocityAll },               // AB_POSITIONSOURCE_DEFERRED
    { L"Position listener deferred", 0.0f, CXBoxSample::PositionListener },         // AB_POSITIONSOURCE_DEFERRED
    { L"Velocity listener deferred", 0.0f, CXBoxSample::VelocityListener },         // AB_POSITIONSOURCE_DEFERRED
    { L"CommitDeferredSettings", 0.0f, CXBoxSample::Commit3D },                     // AB_COMMIT3D
    { L"Set frequency", 0.0f, CXBoxSample::SetFrequency },                          // AB_FREQUENCY
    { L"Set volume", 0.0f, CXBoxSample::SetVolume },                                // AB_VOLUME
    { L"Set pitch", 0.0f, CXBoxSample::SetPitch },                                  // AB_PITCH
    { L"Set amp envelope", 0.0f, CXBoxSample::SetEG },                              // AB_ENVELOPE
    { L"Set DLS2 fliter", 0.0f, CXBoxSample::SetFilter },                           // AB_SETFILTER
};
#define NUM_TESTS ( sizeof(g_aTests) / sizeof( g_aTests[0] ) )



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
    memset( m_apBuffers, 0, sizeof( m_apBuffers ) );
    m_bDrawHelp = FALSE;
    m_dwTest = 0;
}



//-----------------------------------------------------------------------------
// Name: DownloadEffectsImage
// Desc: Downloads an effects image to the DSP
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DownloadEffectsImage(PCHAR pszScratchFile)
{
    HANDLE hFile;
    DWORD dwSize;
    PVOID pBuffer = NULL;
    HRESULT hr = S_OK;
    LPDSEFFECTIMAGEDESC pDesc;
    DSEFFECTIMAGELOC EffectLoc;

    // open scratch image file generated by xps2 tool
    hFile = CreateFile( pszScratchFile,
                        GENERIC_READ,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL );

    if( hFile == INVALID_HANDLE_VALUE )
    {
        DWORD err;

        err = GetLastError();
        OUTPUT_DEBUG_STRING( "Failed to open the dsp image file.\n" );
        hr = HRESULT_FROM_WIN32(err);
    }

    if( SUCCEEDED(hr) )
    {
        // Determine the size of the scratch image by seeking to
        // the end of the file
        dwSize = SetFilePointer( hFile, 0, NULL, FILE_END );
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    
        // Allocate memory to read the scratch image from disk
        pBuffer = new BYTE[dwSize];

        // Read the image in
        DWORD dwBytesRead;
        BOOL bResult = ReadFile( hFile,
                                 pBuffer,
                                 dwSize,
                                 &dwBytesRead,
                                 0 );
        
        if (!bResult)
        {
            DWORD err;

            err = GetLastError();
            OUTPUT_DEBUG_STRING( "\n Failed to open the dsp image file.\n" );
            hr = HRESULT_FROM_WIN32(err);
        }

    }

    if( SUCCEEDED(hr) )
    {
        // call dsound api to download the image..
        EffectLoc.dwI3DL2ReverbIndex = I3DL2_CHAIN_I3DL2_REVERB;
        EffectLoc.dwCrosstalkIndex = I3DL2_CHAIN_XTALK;

        if (SUCCEEDED(hr))
        {
            hr = m_pDSound->DownloadEffectsImage( pBuffer,
                                                  dwSize,
                                                  &EffectLoc,
                                                  &pDesc );
        }
    }

    delete[] pBuffer;

    if( hFile != INVALID_HANDLE_VALUE ) 
    {
        CloseHandle( hFile );
    }
    
    return hr;
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

    LARGE_INTEGER liFreq;
    QueryPerformanceFrequency( &liFreq );
    m_fMSPerTick = 1000.0f / liFreq.QuadPart;

    DirectSoundCreate( NULL, &m_pDSound, NULL );
    DSoundInitHRTF();
    //
    // download the standard DirectSound effecs image
    //
    if( FAILED( DownloadEffectsImage("d:\\media\\dsstdfx.bin") ) )
        return E_FAIL;


    InitializeBuffers();

    m_bPlaying = FALSE;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::FrameMove()
{
#if 0
    if( m_bPlaying )
        StopAll();
    else
        PlayAll();
    m_bPlaying = !m_bPlaying;
#endif // 0
    
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    // Run test
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
    {
        (this->*g_aTests[m_dwTest].pfn)();
    }

    // Shortcut for toggling playback status
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] )
    {
        if( m_bPlaying )
            StopAll();
        else
            PlayAll();
    }

    // Shortcut to re-initialize buffers
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
    {
        InitializeBuffers();
    }

    // Test select
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
    {
        m_dwTest = (m_dwTest + NUM_TESTS - 1 ) % NUM_TESTS;
    }
    else if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
    {
        m_dwTest = (m_dwTest + 1 ) % NUM_TESTS;
    }

    return S_OK;
}






//-----------------------------------------------------------------------------
// Name: CreateSineWaveBuffer
// Desc: Creates a DirectSound Buffer and fills it with a sine wave.  This
//       can be useful for DSP effects that are driven off an oscillator.
//       Note that non-integral frequencies will end up with a discontinuity
//       at the loop point.
//-----------------------------------------------------------------------------
HRESULT
CreateSineWaveBuffer( double dFrequency, LPDIRECTSOUNDBUFFER8 * ppBuffer )
{
    HRESULT hr = S_OK;
    LPDIRECTSOUNDBUFFER8 pBuffer = NULL;
    BYTE * pData = NULL;
    DWORD dwBytes = 0;
    double dArg = 0.0;
    double dSinVal = 0.0;
    BYTE bVal = 0;

    // Check arguments
    if( !ppBuffer || dFrequency < 0 )
        return E_INVALIDARG;

    *ppBuffer = NULL;

    //
    // Initialize a wave format structure
    //
    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof( WAVEFORMATEX ) );

    wfx.wFormatTag      = WAVE_FORMAT_PCM;      // PCM data
    wfx.nChannels       = 1;                    // Mono
    wfx.nSamplesPerSec  = 1000;                 // 1kHz
    wfx.nAvgBytesPerSec = 1000;                 // 1kHz * 1 bytes / sample
    wfx.nBlockAlign     = 1;                    // sample size in bytes
    wfx.wBitsPerSample  = 8;                    // 8 bit samples
    wfx.cbSize          = 0;                    // No extra data

    //
    // Intialize the buffer description
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );

    dsbd.dwSize = sizeof( DSBUFFERDESC );
    dsbd.dwFlags = DSBCAPS;
    dsbd.dwBufferBytes = wfx.nAvgBytesPerSec;
    dsbd.lpwfxFormat = (WAVEFORMATEX *)&wfx;

    // Create the buffer
    hr = DirectSoundCreateBuffer( &dsbd, &pBuffer );
    if( FAILED( hr ) )
        return hr;

    // Get a pointer to buffer data to fill
    hr = pBuffer->Lock( 0, dsbd.dwBufferBytes, (VOID **)&pData, &dwBytes, NULL, NULL, 0 );
    if( FAILED( hr ) )
        return hr;

    // Now fill the buffer, 1 8-bit sample at a time
    for( DWORD i = 0; i < dwBytes; i++ )
    {
        // Convert sample offset to radians
        dArg = (double)i / wfx.nSamplesPerSec * D3DX_PI * 2;

        // Calculate the sin
        dSinVal = sin( dFrequency * dArg );

        // Scale to sample format
        bVal = BYTE( dSinVal * 127 );

        // Store the sample
        pData[i] = bVal;
    }

    // return the buffer
    *ppBuffer = pBuffer;

    return hr;
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

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"AudioBenchmark" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        WCHAR sz[100];
        swprintf( sz, L"%d buffers (%splaying)", NUM_3D_BUFFERS, m_bPlaying ? L"" : L"NOT " );
        m_Font.DrawText(  64, 80, 0xffffffff, sz );
#if 0
        for( int n = 0; n < NUM_TESTS; n++ )
        {
            swprintf( sz, L"%s: %0.3fms", g_aTests[n].szDescription, g_aTests[n].fTime );
            m_Font.DrawText(  64, 110.0f + 30 * n, 0xffffffff, sz );
        }
#endif // 0
        swprintf( sz, L"Test %d: %s\nTime: %0.3fms", m_dwTest, g_aTests[m_dwTest].szDescription, g_aTests[m_dwTest].fTime );
        m_Font.DrawText( 64, 110, 0xffffffff, sz );

        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}





//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++[ Test routines ]++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++





//-----------------------------------------------------------------------------
// Name: SetCurrentPosition
// Desc: Resets position of all buffers.
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::SetCurrentPosition()
{
    BEFORETEST( AB_SETCURRENTPOSITION );
    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        FLUSH_PIO();
        m_apBuffers[n]->SetCurrentPosition(0);
    }
    AFTERTEST( AB_SETCURRENTPOSITION );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: PlayAll
// Desc: Plays all buffers.
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::PlayAll()
{
    BEFORETEST( AB_PLAY );
    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        FLUSH_PIO();
        m_apBuffers[n]->Play( 0, 0, DSBPLAY_LOOPING );
    }
    AFTERTEST( AB_PLAY );

    m_bPlaying = TRUE;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: StopAll
// Desc: Stops all buffers
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::StopAll()
{
    BEFORETEST( AB_STOP );
    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        FLUSH_PIO();
        m_apBuffers[n]->Stop();
    }
    AFTERTEST( AB_STOP );

    m_bPlaying = FALSE;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitializeBuffers
// Desc: Times release of buffers if non-NULL, and creates new ones.
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::InitializeBuffers()
{
    BEFORETEST( AB_RELEASE );
    for( int i = 0; i < NUM_3D_BUFFERS; i++ )
    {
        if( m_apBuffers[i] )
        {
            FLUSH_PIO();
            m_apBuffers[i]->Release();
        }
    }
    AFTERTEST( AB_RELEASE );

    for( int i = 0; i < NUM_3D_BUFFERS; i++ )
    {
        CreateSineWaveBuffer( 16000 / ( i + 1 ), &m_apBuffers[i] );
    }

    m_pDSound->SetPosition( 0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE );
    m_pDSound->SetVelocity( 0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE );

    m_bPlaying = FALSE;

    return S_OK;
}
    


//-----------------------------------------------------------------------------
// Name: PositionAllImmediate
// Desc: Positions all buffers with DS3D_IMMEDIATE
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::PositionAllImmediate()
{
    D3DXVECTOR3 v[NUM_3D_BUFFERS];

    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        v[n].x = FLOAT( sin( n ) );
        v[n].y = FLOAT( cos( n ) );
        v[n].z = FLOAT( sin( n ) * cos( n ) );
    }

    BEFORETEST( AB_POSITIONSOURCE );
    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        FLUSH_PIO();
        m_apBuffers[n]->SetPosition( v[n].x, v[n].y, v[n].z, DS3D_IMMEDIATE );
    }
    AFTERTEST( AB_POSITIONSOURCE );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: VelocityAllImmediate
// Desc: Sets velocity of all buffers with DS3D_IMMEDIATE
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::VelocityAllImmediate()
{
    D3DXVECTOR3 v[NUM_3D_BUFFERS];

    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        v[n].x = FLOAT( sin( n ) );
        v[n].y = FLOAT( cos( n ) );
        v[n].z = FLOAT( sin( n ) * cos( n ) );
    }

    BEFORETEST( AB_VELOCITYSOURCE );
    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        FLUSH_PIO();
        m_apBuffers[n]->SetVelocity( v[n].x, v[n].y, v[n].z, DS3D_IMMEDIATE );
    }
    AFTERTEST( AB_VELOCITYSOURCE );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: PositionListenerImmediate
// Desc: Positions the listener with DS3D_IMMEDIATE
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::PositionListenerImmediate()
{
    BEFORETEST( AB_POSITIONLISTENER );
    FLUSH_PIO();
    m_pDSound->SetPosition( 1.0f, 2.0f, 3.0f, DS3D_IMMEDIATE );
    AFTERTEST( AB_POSITIONLISTENER );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: VelocityListenerImmediate
// Desc: Sets the velocity of the listener with DS3D_IMMEDIATE
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::VelocityListenerImmediate()
{
    BEFORETEST( AB_VELOCITYLISTENER );
    FLUSH_PIO();
    m_pDSound->SetVelocity( 3.0f, 2.0f, 1.0f, DS3D_IMMEDIATE );
    AFTERTEST( AB_VELOCITYLISTENER );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: PositionAll
// Desc: Positions all the buffers with DS3D_DEFERRED
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::PositionAll()
{
    D3DXVECTOR3 v[NUM_3D_BUFFERS];

    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        v[n].x = FLOAT( sin( n ) );
        v[n].y = FLOAT( cos( n ) );
        v[n].z = FLOAT( sin( n ) * cos( n ) );
    }

    BEFORETEST( AB_POSITIONSOURCE_DEFERRED );
    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        FLUSH_PIO();
        m_apBuffers[n]->SetPosition( v[n].x, v[n].y, v[n].z, DS3D_DEFERRED );
    }
    AFTERTEST( AB_POSITIONSOURCE_DEFERRED );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: VelocityAll
// Desc: Sets the velocity of all buffers with DS3D_DEFERRED
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::VelocityAll()
{
    D3DXVECTOR3 v[NUM_3D_BUFFERS];

    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        v[n].x = FLOAT( sin( n ) );
        v[n].y = FLOAT( cos( n ) );
        v[n].z = FLOAT( sin( n ) * cos( n ) );
    }

    BEFORETEST( AB_VELOCITYSOURCE_DEFERRED );
    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        FLUSH_PIO();
        m_apBuffers[n]->SetVelocity( v[n].x, v[n].y, v[n].z, DS3D_DEFERRED );
    }
    AFTERTEST( AB_VELOCITYSOURCE_DEFERRED );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: PositionListener
// Desc: Positions the listener with DS3D_DEFERRED
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::PositionListener()
{
    BEFORETEST( AB_POSITIONLISTENER_DEFERRED );
    FLUSH_PIO();
    m_pDSound->SetPosition( 1.0f, 2.0f, 3.0f, DS3D_DEFERRED );
    AFTERTEST( AB_POSITIONLISTENER_DEFERRED );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: VelocityListener
// Desc: Set the velocity of the listener with DS3D_DEFERRED
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::VelocityListener()
{
    BEFORETEST( AB_VELOCITYLISTENER_DEFERRED );
    FLUSH_PIO();
    m_pDSound->SetVelocity( 3.0f, 2.0f, 1.0f, DS3D_DEFERRED );
    AFTERTEST( AB_VELOCITYLISTENER_DEFERRED );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Commit3D
// Desc: Commits deferred 3d calculations
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Commit3D()
{
    BEFORETEST( AB_COMMIT3D );
    FLUSH_PIO();
    m_pDSound->CommitDeferredSettings();
    AFTERTEST( AB_COMMIT3D );

    return S_OK;
}




HRESULT
CXBoxSample::SetFrequency()
{
    DWORD dw[NUM_3D_BUFFERS];

    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        dw[n] = DSBFREQUENCY_MIN + n * ( DSBFREQUENCY_MAX - DSBFREQUENCY_MIN ) / NUM_3D_BUFFERS;
    }

    BEFORETEST( AB_FREQUENCY );
    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        FLUSH_PIO();
        m_apBuffers[n]->SetFrequency( dw[n] );
    }
    AFTERTEST( AB_FREQUENCY );

    return S_OK;
}




HRESULT
CXBoxSample::SetVolume()
{
    LONG l[NUM_3D_BUFFERS];

    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        l[n] = DSBVOLUME_MIN + n * ( DSBVOLUME_MAX - DSBVOLUME_MIN ) / NUM_3D_BUFFERS;
    }

    BEFORETEST( AB_VOLUME );
    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        FLUSH_PIO();
        m_apBuffers[n]->SetVolume( l[n] );
    }
    AFTERTEST( AB_VOLUME );

    return S_OK;
}



HRESULT
CXBoxSample::SetPitch()
{
    LONG l[NUM_3D_BUFFERS];

    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        l[n] = DSBPITCH_MIN + n * ( DSBPITCH_MAX - DSBPITCH_MIN ) / NUM_3D_BUFFERS;
    }

    BEFORETEST( AB_PITCH );
    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        FLUSH_PIO();
        m_apBuffers[n]->SetPitch( l[n] );
    }
    AFTERTEST( AB_PITCH );

    return S_OK;
}


HRESULT
CXBoxSample::SetEG()
{
    DSENVELOPEDESC dsed[NUM_3D_BUFFERS];

    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        ZeroMemory( &dsed[n], sizeof( DSENVELOPEDESC ) );
        dsed[n].dwEG = DSEG_AMPLITUDE;
        dsed[n].dwSustain = n;
    }

    BEFORETEST( AB_ENVELOPE );
    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        FLUSH_PIO();
        m_apBuffers[n]->SetEG( &dsed[n] );
    }
    AFTERTEST( AB_ENVELOPE );

    return S_OK;
}


HRESULT
CXBoxSample::SetFilter()
{
    DSFILTERDESC dsfd[NUM_3D_BUFFERS];

    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        dsfd[n].dwMode = DSFILTER_MODE_DLS2;
        dsfd[n].adwCoefficients[0] = 0xF800;
        dsfd[n].adwCoefficients[1] = 0x8800;
        dsfd[n].adwCoefficients[2] = 0;
        dsfd[n].adwCoefficients[3] = 0;
        dsfd[n].dwQCoefficient = 0;
    }

    BEFORETEST( AB_SETFILTER );
    for( int n = 0; n < NUM_3D_BUFFERS; n++ )
    {
        FLUSH_PIO();
        m_apBuffers[n]->SetFilter( &dsfd[n] );
    }
    AFTERTEST( AB_SETFILTER );

    return S_OK;
}