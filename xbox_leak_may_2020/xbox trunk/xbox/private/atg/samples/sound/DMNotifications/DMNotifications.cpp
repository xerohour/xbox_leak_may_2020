//-----------------------------------------------------------------------------
// File: DMNotifications.cpp
//
// Desc: Demonstrates how to use DirectMusic Notification messages to get
//       information about audio playback.
//
// Hist: 04.06.01 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>
#include <dmusici.h>
#include "dsstdfx.h"
#include "myfactory.h"



//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle\nplayback" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Select\nsegment" },
    { XBHELP_DPAD,         XBHELP_PLACEMENT_2, L"Play control" },
    { XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_2, L"Increase\nvolume" },
    { XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_2, L"Decrease\nvolume" },
};

#define NUM_HELP_CALLOUTS 6

DMUS_SEGF_FLAGS g_adsf[] = 
{
    DMUS_SEGF_FLAGS(0),
    DMUS_SEGF_GRID,
    DMUS_SEGF_BEAT,
    DMUS_SEGF_MEASURE,
};
static const DWORD NUM_FLAGS = sizeof( g_adsf ) / sizeof( g_adsf[0] );

WCHAR * g_szWhen[] = 
{
    L"Immediately",
    L"Grid Boundary",
    L"Beat Boundary",
    L"Measure Boundary",
};

char * g_szSegments[] = 
{
    "Bach Invention.sgt", 
    "ClaireDeLune.sgt", 
    "BrassAction.sgt",  
    "FurElise.sgt",
};
static const DWORD NUM_SEGMENTS = sizeof( g_szSegments ) / sizeof( g_szSegments[0] );

typedef struct {
    DWORD                       dwSegment;
    IDirectMusicSegment8 *      pSegment;
    IDirectMusicSegmentState8 * pSegState;
    BOOL                        bPlaying;
} PLAYSTATE;


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

    HRESULT LoadSegment( PLAYSTATE * ps, DWORD dwSegment );

    // Font and help
    CXBFont     m_Font;
    CXBHelp     m_Help;

    BOOL        m_bDrawHelp;

    DWORD       m_dwBoundary;   // Current Boundary
    DWORD       m_dwCurrent;
    LONG        m_lVolume;

    FLOAT       m_fBeat;
    WCHAR       m_szSegment[100];
    
    PLAYSTATE   m_aStates[2];

    IDirectMusicPerformance8 *     m_pDMPerformance;
    IDirectMusicLoader8 *          m_pDMLoader;
    IDirectMusicAudioPath8 *       m_pMusicAudioPath;
    IDirectMusicGraph8 *           m_pDMGraph;
    LPDIRECTSOUND8                 m_pDSound;           // DirectSound object

    HRESULT DownloadEffectsImage(PCHAR pszScratchFile); // downloads a default DSP image to the GP
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
    ZeroMemory( m_aStates, sizeof( PLAYSTATE ) * 2 );
    m_dwBoundary = 0;
    m_dwCurrent  = 0;
    m_bDrawHelp = FALSE;
    m_szSegment[0] = 0;
}



//-----------------------------------------------------------------------------
// Name: LoadSegment
// Desc: Transitions the playstate to a different segment.  Stops playback
//       if it's currently playing, releases current segment, loads and 
//       downloads new segment, and restarts playback if needed.
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::LoadSegment( PLAYSTATE * ps, DWORD dwSegment )
{
    // Shut down the current segment
    if( ps->bPlaying )
    {
        m_pDMPerformance->StopEx( ps->pSegState, 0, 0 );
        ps->pSegState->Release();
        ps->pSegState = NULL;
    }

    if( ps->pSegment )
    {
        ps->pSegment->Release();
        ps->pSegment = NULL;
    }

    if( FAILED( m_pDMLoader->LoadObjectFromFile( CLSID_DirectMusicSegment, 
                                                 IID_IDirectMusicSegment8,
                                                 g_szSegments[ dwSegment ], 
                                                 (VOID **)&ps->pSegment ) ) )
        return E_FAIL;

    ps->dwSegment = dwSegment;
    if( ps->bPlaying )
    {
        m_pDMPerformance->PlaySegmentEx( ps->pSegment, 
                                         NULL, 
                                         NULL, 
                                         0, 
                                         0, 
                                         &ps->pSegState, 
                                         NULL, 
                                         NULL );
    }

    return S_OK;
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

        hr = m_pDSound->DownloadEffectsImage( pBuffer,
                                              dwSize,
                                              &EffectLoc,
                                              &pDesc );
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

    if( FAILED( DirectSoundCreate( NULL, &m_pDSound, NULL ) ) )
        return E_FAIL;

    //
    // download the standard DirectSound effecs image
    //
    if( FAILED( DownloadEffectsImage("d:\\media\\dsstdfx.bin") ) )
        return E_FAIL;

    // Initialize DMusic
    IDirectMusicHeap* pNormalHeap;
    DirectMusicCreateDefaultHeap( &pNormalHeap );

    IDirectMusicHeap* pPhysicalHeap;
    DirectMusicCreateDefaultPhysicalHeap( &pPhysicalHeap );

    DirectMusicInitializeEx( pNormalHeap, pPhysicalHeap, MyFactory );

    pNormalHeap->Release();
    pPhysicalHeap->Release();

    // Create loader object
    DirectMusicCreateInstance( CLSID_DirectMusicLoader, NULL, 
                               IID_IDirectMusicLoader8, (VOID**)&m_pDMLoader );

    // Create performance object
    DirectMusicCreateInstance( CLSID_DirectMusicPerformance, NULL,
                               IID_IDirectMusicPerformance8, (VOID**)&m_pDMPerformance );

    // Initialize the performance with the standard audio path.
    // The flags (final) argument allows us to specify whether or not we want
    // DirectMusic to create a thread on our behalf to process music, using 
    // DMUS_INITAUDIO_NOTHREADS.  The default is for DirectMusic to create its
    // own thread; DMUS_INITAUDIO_NOTHREADS tells DirectMusic not to do this, 
    // and the app will periodically call DirectMusicDoWork().  For software 
    // emulation on alpha hardware, it's generally better to have DirectMusic
    // create its own thread. On real hardware, periodically calling 
    // DirectMusicDoWork may provide a better option.
    m_pDMPerformance->InitAudioX( DMUS_APATH_SHARED_STEREOPLUSREVERB, 96, 128, 0 );

    // Tell DirectMusic where the default search path is
    m_pDMLoader->SetSearchDirectory( GUID_DirectMusicAllTypes, 
                                   "D:\\Media\\Sounds", FALSE );
    
    // Load primary segment
    m_pDMLoader->LoadObjectFromFile( CLSID_DirectMusicSegment, IID_IDirectMusicSegment8, 
                                   g_szSegments[0], (VOID**)&m_aStates[0].pSegment);
    // Load secondary segment
    m_pDMLoader->LoadObjectFromFile( CLSID_DirectMusicSegment, IID_IDirectMusicSegment8, 
                                   g_szSegments[0], (VOID**)&m_aStates[1].pSegment );

    // Play segment on the default audio path
    m_pDMPerformance->PlaySegmentEx( m_aStates[0].pSegment, NULL, NULL, 0, 
                                   0, &m_aStates[0].pSegState, NULL, NULL );
    m_aStates[0].bPlaying = TRUE;

    // Get default (music) audiopath.
    m_pDMPerformance->GetDefaultAudioPath( &m_pMusicAudioPath );

    // Max volume for music
    m_lVolume = DSBVOLUME_MAX;
    m_pMusicAudioPath->SetVolume( m_lVolume, 0 );

    // Set up to receive the notifications we're interested in
    GUID guid = GUID_NOTIFICATION_SEGMENT;
    m_pDMPerformance->AddNotificationType( guid );
    guid = GUID_NOTIFICATION_MEASUREANDBEAT;
    m_pDMPerformance->AddNotificationType( guid );

    return S_OK;
}

#define VOLUME_SCALE 5.0f
//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::FrameMove()
{
    DMUS_NOTIFICATION_PMSG * pPMsg;
    PLAYSTATE * ps = &m_aStates[ m_dwCurrent ];

    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    // Toggle playback
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
    {
        if( ps->bPlaying )
        {
            m_pDMPerformance->StopEx( ps->pSegState, 0, g_adsf[ m_dwBoundary ] );
        }
        else
        {
            DWORD dwFlags = ( m_dwCurrent == 0 ) ? 0 : DMUS_SEGF_SECONDARY;

            m_pDMPerformance->PlaySegmentEx( ps->pSegment, 
                                             NULL, 
                                             NULL, 
                                             g_adsf[ m_dwBoundary ] | dwFlags, 
                                             0, 
                                             &ps->pSegState, 
                                             NULL, 
                                             NULL );
            if( m_dwCurrent != 0 && g_adsf[ m_dwBoundary ] != 0 )
                swprintf( m_szSegment, L"Secondary Segment Pending" );
        }
        if( m_dwCurrent == 0 )
            ps->bPlaying ^= 1;
    }

    // Select segment
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] )
    {
        LoadSegment( ps, ( ps->dwSegment + 1 ) % NUM_SEGMENTS );
    }

    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
    {
        m_dwBoundary = ( m_dwBoundary + NUM_FLAGS - 1 ) % NUM_FLAGS;
    }
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
    {
        m_dwBoundary = ( m_dwBoundary + 1 ) % NUM_FLAGS;
    }
    if( m_DefaultGamepad.wPressedButtons & ( XINPUT_GAMEPAD_DPAD_LEFT | XINPUT_GAMEPAD_DPAD_RIGHT ) )
    {
        m_dwCurrent = ( m_dwCurrent + 1 ) % 2;
    }


    // Increase/Decrease volume
    m_lVolume += LONG( ( m_DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_BLACK ] - 
                   m_DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_WHITE ] ) *
                   m_fElapsedTime * 
                   VOLUME_SCALE );

    // Make sure volume is in the appropriate range
    if( m_lVolume < -6000 )
        m_lVolume = -6000;
    else if( m_lVolume > DSBVOLUME_MAX )
        m_lVolume = DSBVOLUME_MAX;
    m_pMusicAudioPath->SetVolume( m_lVolume, 0 );

    m_fBeat -= 0.1f;
    if( m_fBeat < 0.0f )
        m_fBeat = 0.0f;

    // Here's where we look at this shiznit
    while( S_OK == m_pDMPerformance->GetNotificationPMsg( &pPMsg ) )
    {
        IDirectMusicSegmentState8 * pSegState = NULL;

        if( SUCCEEDED( pPMsg->punkUser->QueryInterface( IID_IDirectMusicSegmentState8, (void **)&pSegState ) ) )

        //
        // We're interested in segment notifications on the secondary 
        // segment, and beat notifications on the primary segment
        //
        if( pPMsg->guidNotificationType == GUID_NOTIFICATION_SEGMENT )
        {
            if( pSegState == m_aStates[1].pSegState )
            {
                // Yeah baby
                switch( pPMsg->dwNotificationOption )
                {
                case DMUS_NOTIFICATION_SEGABORT:
                    swprintf( m_szSegment, L"Secondary Segment Aborted" );
                    m_aStates[1].bPlaying = FALSE;
                    break;
                case DMUS_NOTIFICATION_SEGALMOSTEND:
                    swprintf( m_szSegment, L"Secondary Segment almost at end" );
                    break;
                case DMUS_NOTIFICATION_SEGLOOP:
                    swprintf( m_szSegment, L"Secondary Segment has looped" );
                    break;
                case DMUS_NOTIFICATION_SEGSTART:
                    swprintf( m_szSegment, L"Secondary Segment Playing" );
                    m_aStates[1].bPlaying = TRUE;
                    break;
                case DMUS_NOTIFICATION_SEGEND:
                    swprintf( m_szSegment, L"Secondary Segment Completed" );
                    m_aStates[1].bPlaying = FALSE;
                    break;
                }
            }
            else if( pSegState == m_aStates[0].pSegState && 
                     pPMsg->dwNotificationOption == DMUS_NOTIFICATION_SEGEND )
            {
                m_pDMPerformance->PlaySegmentEx( m_aStates[0].pSegment, NULL, NULL, 0, 
                                                 0, &m_aStates[0].pSegState, NULL, NULL );
            }
        }
        else if( pSegState == m_aStates[0].pSegState &&
                 pPMsg->guidNotificationType == GUID_NOTIFICATION_MEASUREANDBEAT )
        {
            m_fBeat = 1.0;
        }

        m_pDMPerformance->FreePMsg( (DMUS_PMSG*)pPMsg );
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
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    // Draw a gradient filled background
    RenderGradientBackground( 0xff408040, 0xff404040 );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        WCHAR sz[100];

		m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"DMNotifications" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        swprintf( sz, L"Controlling %s segment: %S", m_dwCurrent ? L"secondary" : L"primary", 
                                                     g_szSegments[ m_aStates[ m_dwCurrent ].dwSegment ] );
        m_Font.DrawText(  64, 80, m_aStates[ m_dwCurrent ].bPlaying ? 0xffffffff : 0xFF808080, sz );
        m_Font.DrawText(  64, 110, 0xffffffff, L"Boundary:" );
        m_Font.DrawText( 200, 110, 0xffffffff, g_szWhen[ m_dwBoundary ] );
        m_Font.DrawText( 200, 200, 0x00FFFFFF | DWORD( m_fBeat * 255 ) << 24, L"Primary Beat" );
        m_Font.DrawText( 200, 240, 0xFFFFFFFF, m_szSegment );
		m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

