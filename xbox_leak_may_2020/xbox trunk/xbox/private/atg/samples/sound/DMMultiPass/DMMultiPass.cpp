//-----------------------------------------------------------------------------
// File: DMMultiPass.cpp
//
// Desc: This sample demonstrates how to use the multipass capabilities of 
//       DirectMusic, by playing several different segments at once on a 3D
//       audiopath moving a sound source and listener in 3D.
//
// Hist: 5.14.01 - New for June XDK Release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBSound.h>
#include <XBHelp.h>
#include <dsound.h>
#include <dmusici.h>
#include <dsstdfx.h>
#include "myfactory.h"


//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle sound" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Change sound" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle source/\nlistener" },
    { XBHELP_Y_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle looping\n(Not when playing)" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_2, L"Toggle Headphones" },
    { XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_2, L"Increase volume" },
    { XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_2, L"Decrease volume" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Move object in Y" },
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Move object in X/Z" },
};

#define NUM_HELP_CALLOUTS 10



//-----------------------------------------------------------------------------
// Globals variables and definitions
//-----------------------------------------------------------------------------

struct D3DVERTEX
{
    D3DXVECTOR3 p;           // position
    D3DCOLOR    c;           // color
};
#define D3DFVF_D3DVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)


// Constants to define our world space
#define XMIN -10
#define XMAX 10
#define ZMIN -10
#define ZMAX 10
#define YMIN 0
#define YMAX 5

// Constants for colors
#define SOURCE_COLOR 0xFFFF0000
#define LISTENER_COLOR 0xFF0000FF

// Constants for scaling input
#define MOTION_SCALE 10.0f
#define VOLUME_SCALE  5.0f

// List of wav files to cycle through
char * g_aszFileNames[] = 
{
    "D:\\Media\\Sounds\\heli.wav",
    "D:\\Media\\Sounds\\DockingMono.wav",
    "D:\\Media\\Sounds\\EngineStartMono.wav",
    "D:\\Media\\Sounds\\MaleDialog1.wav",
    "D:\\Media\\Sounds\\MiningMono.wav",
    "D:\\Media\\Sounds\\MusicMono.wav",
    "D:\\Media\\Sounds\\Dolphin4.wav",
};

#define NUM_SOUNDS 7


//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont                 m_Font;                 // Font object
    CXBHelp                 m_Help;                 // Help object

    LPDIRECTSOUND8              m_pDSound;          // DirectSound object
    IDirectMusicLoader8 *       m_pLoader;          // DM Loader
    IDirectMusicPerformance8 *  m_pPerformance;     // DM Performance
    IDirectMusicSegment8 *      m_apSegments[NUM_SOUNDS];   // DM Segments
    IDirectMusicSegmentState8 * m_apSegStates[NUM_SOUNDS];  // DM Segment states
    IDirectMusicAudioPath8 *    m_pAudioPath;       // DM AudioPath
    DWORD                       m_dwCurrent;        // Current segment
    BOOL                        m_bPlaying[NUM_SOUNDS];     // Are we playing?
    LONG                        m_lVolume[NUM_SOUNDS];      // Current volume
    BOOL                        m_bLooping[NUM_SOUNDS];     // Looping?
    BOOL                        m_bHeadphones;              // True if headphones enabled

    // Sound source and listener positions
    D3DXVECTOR3             m_vSourcePosition;      // Source position vector
    D3DXVECTOR3             m_vListenerPosition;    // Listener position vector

    // Transform matrices
    D3DXMATRIX              m_matWorld;             // World transform
    D3DXMATRIX              m_matView;              // View transform
    D3DXMATRIX              m_matProj;              // Projection transform

    // Models for floor, source, and listener
    LPDIRECT3DVERTEXBUFFER8 m_pvbFloor;             // Quad for the floor
    LPDIRECT3DVERTEXBUFFER8 m_pvbSource;            // Quad for the source
    LPDIRECT3DVERTEXBUFFER8 m_pvbListener;          // Quad for the listener
    LPDIRECT3DVERTEXBUFFER8 m_pvbGrid;              // Lines to grid the floor

    D3DCOLOR        m_cSource;                      // Color for sound source
    D3DCOLOR        m_cListener;                    // Color for listener

    BOOL            m_bDrawHelp;                    // Should we draw help?
    BOOL            m_bControlSource;               // Control source (TRUE) or
                                                    // listener (FALSE)

    HRESULT SwitchToSound( DWORD dwIndex );         // Sets up a different sound
    HRESULT DownloadEffectsImage(PCHAR pszScratchFile);  // downloads a default DSP image to the GP

public:
    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();

    CXBoxSample();
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
// Name: CXBoxSample()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    m_bDrawHelp = FALSE;
    m_bHeadphones = FALSE;

    // Sounds
    m_dwCurrent = 0;

    // Positions
    m_vSourcePosition    = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vListenerPosition = D3DXVECTOR3( 0.0f, 0.0f, ZMIN );
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
// Name: Initialize()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    int i, j;

    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create DirectSound object
    if( FAILED( DirectSoundCreate( NULL, &m_pDSound, NULL ) ) )
        return E_FAIL;

    /* If the application doesn't care about vertical HRTF positioning,
       calling DirectSoundUseLightHRTF can save about 60k of memory. */
    // DirectSoundUseLightHRTF();
    DirectSoundUseFullHRTF();

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


    // Create DirectMusic loader object
    DirectMusicCreateInstance( CLSID_DirectMusicLoader, NULL, 
                               IID_IDirectMusicLoader8, (VOID**)&m_pLoader );

    // Create DirectMusic performance object
    DirectMusicCreateInstance( CLSID_DirectMusicPerformance, NULL,
                               IID_IDirectMusicPerformance8, (VOID**)&m_pPerformance );

    // Initialize the performance with a 3D audio path.
    m_pPerformance->InitAudioX( DMUS_APATH_DYNAMIC_3D, 64, 128, 0 );

    // Tell DirectMusic where the default search path is
    m_pLoader->SetSearchDirectory( GUID_DirectMusicAllTypes, 
                                   "D:\\Media\\Sounds", FALSE );

    // Load each of our sound effect segments
    for( i = 0; i < NUM_SOUNDS; i++ )
    {
        m_pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment, IID_IDirectMusicSegment8, 
                                       g_aszFileNames[i], (VOID**)&m_apSegments[i] );
        m_apSegments[i]->SetRepeats( DMUS_SEG_REPEAT_INFINITE );
        m_bPlaying[i] = FALSE;
        m_lVolume[i] = DSBVOLUME_MAX;
        m_bLooping[i] = TRUE;
        m_apSegStates[i] = NULL;
    }

    // Register for segment notifications
    m_pPerformance->AddNotificationType( GUID_NOTIFICATION_SEGMENT );

    // Get 3D audiopath.
    m_pPerformance->GetDefaultAudioPath( &m_pAudioPath );

    // Play first segment on the 3D audio path[
    m_pPerformance->PlaySegmentEx( m_apSegments[m_dwCurrent], NULL, NULL, DMUS_SEGF_SECONDARY, 
                                   0, &m_apSegStates[m_dwCurrent], NULL, NULL );

    // Max volume for music
    m_apSegStates[m_dwCurrent]->SetVolume( m_lVolume[m_dwCurrent], 0 );

    m_bPlaying[m_dwCurrent] = TRUE;
    

    //
    // Set the transform matrices
    //

    D3DXVECTOR3 vEyePt      = D3DXVECTOR3( XMIN, 45.0f,  ZMAX / 2.0f );
    D3DXVECTOR3 vLookatPt   = D3DXVECTOR3( XMIN,  0.0f,  ZMAX / 2.0f );
    D3DXVECTOR3 vUpVec      = D3DXVECTOR3( 0.0f,  0.0f,  1.0f );
    D3DXMatrixIdentity( &m_matWorld );
    D3DXMatrixLookAtLH( &m_matView, &vEyePt, &vLookatPt, &vUpVec );
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 10000.0f );

    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

    //
    // Create our vertex buffers
    //
    m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( D3DVERTEX ), 0, 0, 0, &m_pvbFloor );
    m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( D3DVERTEX ), 0, 0, 0, &m_pvbSource );
    m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( D3DVERTEX ), 0, 0, 0, &m_pvbListener );
    m_pd3dDevice->CreateVertexBuffer( 2 * ( ( ZMAX - ZMIN + 1 ) + ( XMAX - XMIN + 1 ) ) * sizeof( D3DVERTEX ), 0, 0, 0, &m_pvbGrid );
    
    D3DVERTEX * pVertices;

    // Fill the VB for the floor
    m_pvbFloor->Lock( 0, 0, (BYTE **)&pVertices, 0 );
    pVertices[0].p = D3DXVECTOR3( XMIN, 0.0f, ZMIN ); pVertices[0].c = 0xFF101010;
    pVertices[1].p = D3DXVECTOR3( XMIN, 0.0f, ZMAX ); pVertices[1].c = 0xFF101010;
    pVertices[2].p = D3DXVECTOR3( XMAX, 0.0f, ZMIN ); pVertices[2].c = 0xFF101010;
    pVertices[3].p = D3DXVECTOR3( XMAX, 0.0f, ZMAX ); pVertices[3].c = 0xFF101010;
    m_pvbFloor->Unlock();

    // Fill the VB for the grid
    m_pvbGrid->Lock( 0, 0, (BYTE **)&pVertices, 0 );
    for( i = ZMIN, j = 0; i <= ZMAX; i++, j++ )
    {
        pVertices[ j * 2 ].p     = D3DXVECTOR3( XMIN, 0, (FLOAT)i ); pVertices[ j * 2 ].c     = 0xFF00A000;
        pVertices[ j * 2 + 1 ].p = D3DXVECTOR3( XMAX, 0, (FLOAT)i ); pVertices[ j * 2 + 1 ].c = 0xFF00A000;
    }
    for( i = XMIN; i <= XMAX; i++, j++ )
    {
        pVertices[ j * 2 ].p     = D3DXVECTOR3( (FLOAT)i, 0, ZMIN ); pVertices[ j * 2 ].c     = 0xFF00A000;
        pVertices[ j * 2 + 1 ].p = D3DXVECTOR3( (FLOAT)i, 0, ZMAX ); pVertices[ j * 2 + 1 ].c = 0xFF00A000;
    }
    m_pvbGrid->Unlock();


    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    D3DVERTEX *     pVertices;
    D3DXVECTOR3     vSourceOld   = m_vSourcePosition;
    D3DXVECTOR3     vListenerOld = m_vListenerPosition;
    D3DXVECTOR3 *   pvControl;
    DWORD           dwPulse = DWORD( ( cosf( m_fAppTime * 6.0f ) + 1.0f ) * 50 );
    D3DCOLOR        cBlend = dwPulse | ( dwPulse << 8 ) | ( dwPulse << 16 );
    DMUS_NOTIFICATION_PMSG * pMsg;

    // Check for segment completions
    while( S_OK == m_pPerformance->GetNotificationPMsg( &pMsg ) )
    {
        // If a segment has ended, find it and update state
        if( pMsg->guidNotificationType == GUID_NOTIFICATION_SEGMENT &&
            pMsg->dwNotificationOption == DMUS_NOTIFICATION_SEGABORT ||
            pMsg->dwNotificationOption == DMUS_NOTIFICATION_SEGEND )
        {
            for( int i = 0; i < NUM_SOUNDS; i++ )
            {
                if( pMsg->punkUser == m_apSegStates[i] )
                {
                    m_bPlaying[i] = FALSE;
                }
            }
        }
    }

    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    // Increase/Decrease volume
    m_lVolume[m_dwCurrent] += LONG( ( m_DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_BLACK ] - 
                   m_DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_WHITE ] ) *
                   m_fElapsedTime * 
                   VOLUME_SCALE );

    // Make sure volume is in the appropriate range
    if( m_lVolume[m_dwCurrent] < DSBVOLUME_MIN )
        m_lVolume[m_dwCurrent] = DSBVOLUME_MIN;
    else if( m_lVolume[m_dwCurrent] > DSBVOLUME_MAX )
        m_lVolume[m_dwCurrent] = DSBVOLUME_MAX;

    // Toggle sound on and off
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
    {
        if( m_bPlaying[m_dwCurrent] )
        {
            m_pPerformance->StopEx( m_apSegments[m_dwCurrent], 0, 0 );
            m_apSegStates[m_dwCurrent] = NULL;
        }
        else
            m_pPerformance->PlaySegmentEx( m_apSegments[m_dwCurrent], NULL, NULL, DMUS_SEGF_SECONDARY, 
                                           0, &m_apSegStates[m_dwCurrent], NULL, NULL );

        m_bPlaying[m_dwCurrent] = !m_bPlaying[m_dwCurrent];
    }

    // Cycle through sounds
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] )
    {
        DWORD dwNew = ( m_dwCurrent + 1 ) % NUM_SOUNDS;
        m_dwCurrent = dwNew;
    }

    // Switch which of source vs. listener we are moving
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
    {
        m_bControlSource = !m_bControlSource;
    }

    if( !m_bPlaying[m_dwCurrent] && m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_Y ] )
    {
        m_bLooping[m_dwCurrent] = !m_bLooping[m_dwCurrent];
        m_apSegments[m_dwCurrent]->SetRepeats( m_bLooping[m_dwCurrent] ? DMUS_SEG_REPEAT_INFINITE : 0 );
    }

    // Toggle headphones
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_START )
    {
        m_bHeadphones = !m_bHeadphones;
        m_pDSound->EnableHeadphones( m_bHeadphones );
    }

    // Set up our colors
    m_cSource   = SOURCE_COLOR   | (  m_bControlSource ? cBlend : 0 );
    m_cListener = LISTENER_COLOR | ( !m_bControlSource ? cBlend : 0 );

    // Point to the appropriate vector
    pvControl = m_bControlSource ? &m_vSourcePosition : &m_vListenerPosition;

    //
    // Move selected object and clamp to the appropriate range
    //
    pvControl->x += m_DefaultGamepad.fX1 * m_fElapsedTime * MOTION_SCALE;
    if( pvControl->x < XMIN )
        pvControl->x = XMIN;
    else if( pvControl->x > XMAX )
        pvControl->x = XMAX;

    pvControl->z += m_DefaultGamepad.fY1 * m_fElapsedTime * MOTION_SCALE;
    if( pvControl->z < ZMIN )
        pvControl->z = ZMIN;
    else if( pvControl->z > ZMAX )
        pvControl->z = ZMAX;

    pvControl->y += m_DefaultGamepad.fY2 * m_fElapsedTime * MOTION_SCALE;
    if( pvControl->y < YMIN )
        pvControl->y = YMIN;
    else if( pvControl->y > YMAX )
        pvControl->y = YMAX;

    //
    // Update source/listener vertex buffers
    //
    m_pvbSource->Lock( 0, 0, (BYTE **)&pVertices, 0 );
    pVertices[0].p = m_vSourcePosition + D3DXVECTOR3( -0.5f, 0.0f, -0.5f ); pVertices[0].c = m_cSource;
    pVertices[1].p = m_vSourcePosition + D3DXVECTOR3( -0.5f, 0.0f,  0.5f ); pVertices[1].c = m_cSource;
    pVertices[2].p = m_vSourcePosition + D3DXVECTOR3(  0.5f, 0.0f, -0.5f ); pVertices[2].c = m_cSource;
    pVertices[3].p = m_vSourcePosition + D3DXVECTOR3(  0.5f, 0.0f,  0.5f ); pVertices[3].c = m_cSource;
    m_pvbSource->Lock( 0, 0, (BYTE **)&pVertices, 0 );

    m_pvbListener->Lock( 0, 0, (BYTE **)&pVertices, 0 );
    pVertices[0].p = m_vListenerPosition + D3DXVECTOR3( -0.5f, 0.0f, -0.5f ); pVertices[0].c = m_cListener;
    pVertices[1].p = m_vListenerPosition + D3DXVECTOR3( -0.5f, 0.0f,  0.5f ); pVertices[1].c = m_cListener;
    pVertices[2].p = m_vListenerPosition + D3DXVECTOR3(  0.5f, 0.0f, -0.5f ); pVertices[2].c = m_cListener;
    pVertices[3].p = m_vListenerPosition + D3DXVECTOR3(  0.5f, 0.0f,  0.5f ); pVertices[3].c = m_cListener;
    m_pvbListener->Lock( 0, 0, (BYTE **)&pVertices, 0 );

    if( m_bPlaying[m_dwCurrent] )
        m_apSegStates[m_dwCurrent]->SetVolume( m_lVolume[m_dwCurrent], 0 );

    //
    // Position the sound and listener in 3D. 
    // We use DS3D_DEFERRED so that all the changes will 
    // be commited at once.
    // We scale the velocities by 2 so that doppler effect
    // is a bit more noticable.
    //
    LPDIRECTSOUNDBUFFER8 pBuffer;
    D3DXVECTOR3 vListenerVelocity = 2.0f * ( m_vListenerPosition - vListenerOld ) / m_fElapsedTime;
    D3DXVECTOR3 vSoundVelocity = 2.0f * ( m_vSourcePosition - vSourceOld ) / m_fElapsedTime;


    // Get the 3D DirectSoundBuffer
    m_pAudioPath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_BUFFER, 0, GUID_NULL, 0, GUID_NULL, (VOID **)&pBuffer );

    // Source position/velocity/volume
    pBuffer->SetPosition( m_vSourcePosition.x, m_vSourcePosition.y, m_vSourcePosition.z, DS3D_DEFERRED );
    pBuffer->SetVelocity( vSoundVelocity.x, vSoundVelocity.y, vSoundVelocity.z, DS3D_DEFERRED );

    // Release the buffer
    pBuffer->Release();

    // Listener position/velocity
    m_pDSound->SetPosition( m_vListenerPosition.x, m_vListenerPosition.y, m_vListenerPosition.z, DS3D_DEFERRED  );
    m_pDSound->SetVelocity( vListenerVelocity.x, vListenerVelocity.y, vListenerVelocity.z, DS3D_DEFERRED );

    // Commit position/velocity changes
    m_pDSound->CommitDeferredSettings();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    //
    // call dsound to do some work of low priority queued items such
    // as notifications, stream packets completions, etc
    //
    DirectSoundDoWork();

    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0xFF000000, 1.0f, 0L );

    RenderGradientBackground( 0xFF408040, 0xFF404040 );

    // Set default render states
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetVertexShader( D3DFVF_D3DVERTEX );

    // Draw the floor
    m_pd3dDevice->SetStreamSource( 0, m_pvbFloor, sizeof( D3DVERTEX ) );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    // Draw the grid
    m_pd3dDevice->SetStreamSource( 0, m_pvbGrid, sizeof( D3DVERTEX ) );
    m_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, 2 * ( ( ZMAX - ZMIN + 1 ) + ( XMAX - XMIN + 1 ) ) );

    // Draw the source
    m_pd3dDevice->SetStreamSource( 0, m_pvbSource, sizeof( D3DVERTEX ) );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    // Draw the listener
    m_pd3dDevice->SetStreamSource( 0, m_pvbListener, sizeof( D3DVERTEX ) );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        WCHAR szBuff[200];

		m_Font.Begin();
        // Show frame rate
        m_Font.DrawText(  64, 50, 0xffffffff, L"DMMultiPass" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        // Show status
        swprintf( szBuff, L"Current Sound: %S %s", g_aszFileNames[m_dwCurrent], m_bLooping[m_dwCurrent] ? L"(Looping)" : L"" );
        m_Font.DrawText( 64, 100, m_bPlaying[m_dwCurrent] ? 0xFFFFFFFF : 0xFF404040, szBuff );
        swprintf( szBuff, L"Source: <%0.1f, %0.1f, %0.1f>", m_vSourcePosition.x, m_vSourcePosition.y, m_vSourcePosition.z );
        m_Font.DrawText( 64, 130, m_cSource, szBuff );
        swprintf( szBuff, L"Listener: <%0.1f, %0.1f, %0.1f>", m_vListenerPosition.x, m_vListenerPosition.y, m_vListenerPosition.z );
        m_Font.DrawText( 64, 160, m_cListener, szBuff );
        swprintf( szBuff, L"Volume: %ddB (%0.0f%%)", m_lVolume[m_dwCurrent] / 100, 100.0f * ( m_lVolume[m_dwCurrent] - DSBVOLUME_MIN ) / (DSBVOLUME_MAX - DSBVOLUME_MIN ) );
        m_Font.DrawText( 64, 190, 0xFFFFFF00, szBuff );
        if( m_bHeadphones )
            m_Font.DrawText( 64, 220, 0xFFFFFF00, L"Headphones enabled");
        else
            m_Font.DrawText( 64, 220, 0x80808000, L"Headphones disabled");

		m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




