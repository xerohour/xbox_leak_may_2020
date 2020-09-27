//-----------------------------------------------------------------------------
// File: Game.cpp
//
// Desc: Game mode
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Game.h"
#include <XbApp.h>
#include <XbConfig.h>
#include <XbFont.h>
#include "Controller.h"
#include "File.h"
#include "HeightField.h"
#include "LoadSave.h"
#include "Mesh.h"
#include "PhysicsObject.h"
#include "PhysicsSystem.h"
#include "Vehicle.h"
#include "Globals.h"
#include "Text.h"




//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define MAX(A, B)   (((A) > (B)) ? (A) : (B))



//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
// TCR 3-5 Memory Unit Capacity
const DWORD SAVE_GAME_SIZE_MIN = 1024 * 384; // 384K
const DWORD SAVE_GAME_SIZE_MAX = 1024 * 512; // 512K

const DWORD SCREEN_SHOT_SIZE = 256; // width and height

const LONG  VOLUME_MIN = -4000;     // Minimum volume = -40dB
const LONG  VOLUME_RANGE = 4000;    // Range of volume = 40dB



//-----------------------------------------------------------------------------
// Global statics
//-----------------------------------------------------------------------------
Vector3 Game::m_vEyePt    ( 0.0f, 4.0f, -8.0f );
Vector3 Game::m_vLookatPt ( 0.0f, 0.0f,  0.0f );
Vector3 Game::m_vUpVec    ( 0.0f, 1.0f,  0.0f );




//-----------------------------------------------------------------------------
// Vehicle definitions
//-----------------------------------------------------------------------------
struct VehicleData
{
    char* strModel;
    char* strFrontTireModel;
    char* strRearTireModel;

    D3DVECTOR v3WheelOffset[4];
    float fWheelRadius[4];
};

VehicleData g_Vehicles[] =
{
    {
        "D:\\Media\\Models\\Buggy.sgm",
        "D:\\Media\\Models\\FrontTire.sgm",
        "D:\\Media\\Models\\RearTire.sgm",

        {
            {  0.55f,   0.05f,  1.59f },
            { -0.55f,   0.05f,  1.59f },
            {  0.585f, -0.01f, -1.31f },
            { -0.585f, -0.01f, -1.31f }
        },

        {
            0.41f,
            0.41f,
            0.50f,
            0.50f
        }
    }
};




//-----------------------------------------------------------------------------
// Name: Actor()
// Desc: Construct an empty actor
//-----------------------------------------------------------------------------
Actor::Actor( PhysicsSystem* pPhysicsSystem )
:
    m_pPhysicsSystem( pPhysicsSystem ),
    m_Matrix(),
    m_pDrawMesh( NULL ),
    m_bOwnMesh( FALSE ),
    m_pPhysicsController( NULL ),
    m_pParent( NULL )
{
    assert( pPhysicsSystem != NULL );
}




//-----------------------------------------------------------------------------
// Name: ~Actor()
// Desc: Destroy actor
//-----------------------------------------------------------------------------
Actor::~Actor()
{
    Destroy();
}




//-----------------------------------------------------------------------------
// Name: Actor::Render()
// Desc: Draw the actor
//-----------------------------------------------------------------------------
VOID Actor::Render()
{
    assert( m_pDrawMesh != NULL );

    if( m_pParent != NULL )
    {
        D3DXMATRIX mat;
        D3DXMatrixMultiply( &mat, &m_Matrix, &m_pParent->m_Matrix );
        g_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );
    }
    else
    {
        g_pd3dDevice->SetTransform( D3DTS_WORLD, &m_Matrix );
    }

    m_pDrawMesh->Render();
}




//-----------------------------------------------------------------------------
// Name: Actor::Update()
// Desc: Update the actors positions from the physics system.
//-----------------------------------------------------------------------------
VOID Actor::Update()
{
    PhysicsObject* pController = GetController();
    D3DXMATRIX* pMatrix = GetMatrix();
    if( pController != NULL )
    {
        D3DXMatrixTransformation( pMatrix, NULL, NULL, NULL, NULL,
                                  &(pController->GetRotation()), 
                                  &(pController->GetTranslation()) );
    }
}




//-----------------------------------------------------------------------------
// Name: VehicleActor::VehicleActor()
// Desc: Constructor
//-----------------------------------------------------------------------------
VehicleActor::VehicleActor( PhysicsSystem* pPhysicsSystem )
:
    Actor( pPhysicsSystem ),
    m_ID( Vehicle_Buggy ),
    m_pWheels()
{
    for( INT i = 0; i < 4; ++i )
        m_pWheels[i] = NULL;
}




//-----------------------------------------------------------------------------
// Name: VehicleActor::Update()
// Desc: Update the vehicles actos postions and wheels.
//-----------------------------------------------------------------------------
VOID VehicleActor::Update()
{
    Actor::Update();

    Vehicle* pVehicle = reinterpret_cast< Vehicle* >( GetController() );
    FLOAT fTurnAngle = pVehicle->GetTurnAngle();

    // Update wheels
    for (int i = 0; i < 4; i++)
    {
        FLOAT fOffsetY = pVehicle->GetSupportLength(i);

        Vector3 pos = g_Vehicles[m_ID].v3WheelOffset[i];
        pos.y += g_Vehicles[m_ID].fWheelRadius[i] - fOffsetY;

        FLOAT fAngle = 0.0f;

        if( (i & 1) == 0 )
            fAngle += D3DX_PI;

        if( i < 2 )
            fAngle += fTurnAngle;

        Quaternion quat( Vector3(0.0f, 1.0f, 0.0f), fAngle );

        D3DXMATRIX mat;
        D3DXMatrixTransformation( &mat, NULL, NULL, NULL, NULL, &quat, &pos );
        m_pWheels[i]->SetMatrix( mat );
    }
}




//-----------------------------------------------------------------------------
// Name: Actor::Destroy()
// Desc: Destroy actor
//-----------------------------------------------------------------------------
VOID Actor::Destroy()
{
    if( m_bOwnMesh )
        delete m_pDrawMesh;

    if( m_pPhysicsController != NULL )
        m_pPhysicsSystem->RemoveObject( m_pPhysicsController );

    delete m_pPhysicsController;

    m_pDrawMesh = NULL;
    m_pPhysicsController = NULL;
}




//-----------------------------------------------------------------------------
// Name: Game()
// Desc: Constructor
//-----------------------------------------------------------------------------
Game::Game( CXBFont& font, CXFont& xFont, CMusicManager& musicmgr, CSoundEffect& sndeffect )
:
    m_Font          ( font ),
    m_XFontJPN      ( xFont ),
    m_MusicManager  ( musicmgr ),
    m_SoundEffect   ( sndeffect ),
    m_pPhysicsSystem( new PhysicsSystem ),
    m_VibrationTimer( FALSE ),
    m_VehicleFlippedTimer( FALSE ),
    m_bIsPaused     ( FALSE ),
    m_bIsVibrationOn( FALSE ),
    m_iNumActors    ( 0 ),
    m_pVehicle      ( NULL ),
    m_ptScreenShot  ( NULL ),
    m_bRecordingDemo( FALSE ),
    m_bPlayingDemo  ( FALSE ),
    m_DemoFile      (),
    m_fMusicVolume  ( DSBVOLUME_MAX ),
    m_fEffectVolume ( DSBVOLUME_MAX )
{
}




//-----------------------------------------------------------------------------
// Name: Start()
// Desc: Begin a new game
//-----------------------------------------------------------------------------
VOID Game::Start( BOOL bRecordDemo, BOOL bPlayDemo, BOOL bIsVibrationOn, FLOAT fMusicVolume, FLOAT fEffectVolume, const CHAR* strDemoName )
{
    m_bIsVibrationOn = bIsVibrationOn;
    m_fMusicVolume = fMusicVolume;
    m_fEffectVolume = fEffectVolume;

    // If the game is already running, no need to restart
    if( m_iNumActors )
        return;

    // Safety check to make sure there's room to save this game
    if( !bPlayDemo )
        CheckForFreeSpace();

    // Show loading screen (except in demo mode)
    if( !bPlayDemo )
        ShowLoadingScreen();

    // Create vehicle
    m_pVehicle = AddVehicleActor( D3DXVECTOR3(0.0f, 5.0f, 0.0f), Quaternion(), 
                                  Vehicle_Buggy );

    // Load terrain model
    File fileTerrain;
    if( !fileTerrain.Open( "D:\\Media\\Models\\TestTerrain.sgm", GENERIC_READ ) )
    {
        OUTPUT_DEBUG_STRING( "Failed to open terrain file" );
        return;
    }

    m_VibrationTimer.StartZero();

    Mesh* pMesh = new Mesh();
    pMesh->Load( fileTerrain );
    AddMeshActor( D3DXVECTOR3( 0.0f, 0.0f, 0.0f), Quaternion(), pMesh );

    // Create height field from mesh.
    HeightField* pHeightField = new HeightField();
    pHeightField->BuildFromMesh( pMesh );
    m_pPhysicsSystem->AddHeightField( pHeightField );

    // Add invisible bounding box to prevent vehicle from getting out of world.
    PhysicsObject* pController = new PhysicsObject( m_pPhysicsSystem, 
                                        Vector3(0.0f, 16.0f, 0.0f), Quaternion() );
    Polyhedron* pBox = new Polyhedron( 400.0f, 10.0f, 200.0f );
    pController->SetShape( pBox );
    m_pPhysicsSystem->AddObject( pController );

    // Set the transform matrices
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &m_vEyePt, &m_vLookatPt, &m_vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 640.0f/480.0f, 1.0f, 1000.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Demo record/playback.
    m_bRecordingDemo = FALSE;
    m_bPlayingDemo = FALSE;

    if( bRecordDemo )
    {
        assert( strDemoName != NULL );
        if( m_DemoFile.Create( strDemoName, GENERIC_WRITE ) )
            m_bRecordingDemo = TRUE;
    }
    else if ( bPlayDemo )
    {
        assert( strDemoName != NULL );
        if( m_DemoFile.Open( strDemoName, GENERIC_READ ) )
            m_bPlayingDemo = TRUE;
    }

    // Prepare a screen shot surface
    LPDIRECT3DSURFACE8 pBackBuffer;
    HRESULT hr = g_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO,
                                              &pBackBuffer );
    if( SUCCEEDED(hr) )
    {
        D3DSURFACE_DESC Desc;
        hr = pBackBuffer->GetDesc( &Desc );
        assert( SUCCEEDED(hr) );
        pBackBuffer->Release();

        // Create a texture in the same format as the back buffer
        SAFE_RELEASE( m_ptScreenShot );
        hr = g_pd3dDevice->CreateTexture( SCREEN_SHOT_SIZE, 
                                          SCREEN_SHOT_SIZE,
                                          1, 0, Desc.Format,
                                          D3DPOOL(), &m_ptScreenShot );
        assert( SUCCEEDED(hr) );
    }

    // Start a song playing from a randomly selected soundtrack
    m_MusicManager.SetVolume( VOLUME_MIN + LONG( m_fMusicVolume * VOLUME_RANGE / 100.0f ) );
    m_MusicManager.RandomSong( TRUE );
    m_MusicManager.Play();

    // Start with the low RPM sound
    m_SoundEffect.SetVolume( m_fEffectVolume );
    m_SoundEffect.PlaySound();
}




//-----------------------------------------------------------------------------
// Name: End()
// Desc: Game over, man!  Game Over!
//-----------------------------------------------------------------------------
VOID Game::End()
{
    // Stop playing any sound
    m_MusicManager.Stop();
    m_SoundEffect.Stop();

    // Clear any font textures from device (CXBFont doesn't do this automatically
    // for performance reasons)
    g_pd3dDevice->SetTexture( 0, NULL );

    // Destroy actors
    for( INT i = 0; i < m_iNumActors; ++i )
    {
        delete m_pActors[i];
        m_pActors[i] = NULL;
    }
    m_pVehicle = NULL;

    m_iNumActors = 0;

    // Shutdown physics system
    m_pPhysicsSystem->Shutdown();

    m_DemoFile.Close();

    SAFE_RELEASE( m_ptScreenShot );
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame for animating the game
//-----------------------------------------------------------------------------
HRESULT Game::FrameMove( const XBGAMEPAD* pGamePadIn, FLOAT fTime, 
                         FLOAT fElapsedTime )
{
    const XBGAMEPAD* pGamepad = pGamePadIn;

    if( pGamepad == NULL && !m_bPlayingDemo )
    {
        // Primary controller was removed; pause the game
        m_bIsPaused = TRUE;
        return S_OK;
    }

    // Demo recording/playback
    struct PlaybackData
    {
        FLOAT fTime;
        FLOAT fElapsedTime;
        FLOAT fX1;
        WORD  wPressedButtons;
        WORD  wButtons;
        BYTE  bAnalogButtons[8];
    };

    XBGAMEPAD GamepadDemo;
    if( m_bRecordingDemo && pGamepad != NULL )
    {
        // Record the important information (time and controller input)
        // in a local struct 
        PlaybackData playbackData = 
        { 
            fTime, 
            fElapsedTime, 
            pGamepad->fX1,
            pGamepad->wPressedButtons,
            pGamepad->wButtons,
        };
        for( INT i = 0; i < 8; ++i )
            playbackData.bAnalogButtons[i] = pGamepad->bAnalogButtons[i];

        // Write the data to disk
        m_DemoFile.Write( &playbackData, sizeof( playbackData ) );
    }
    else if( m_bPlayingDemo )
    {
        // Grab the playback data from the demo file
        DWORD dwBytes;
        PlaybackData playbackData;
        m_DemoFile.Read( &playbackData, sizeof( playbackData ), dwBytes );

        // Store the data locally
        fTime        = playbackData.fTime;
        fElapsedTime = playbackData.fElapsedTime;

        ZeroMemory( &GamepadDemo, sizeof( GamepadDemo ) );
        GamepadDemo.fX1             = playbackData.fX1;
        GamepadDemo.wPressedButtons = playbackData.wPressedButtons;
        GamepadDemo.wButtons        = playbackData.wButtons;
        for( INT i = 0; i < 8; ++i )
            GamepadDemo.bAnalogButtons[i] = playbackData.bAnalogButtons[i];
        pGamepad = &GamepadDemo;
    }

    assert( pGamepad != NULL );

    FLOAT fTurnControl = 0.0f;
    FLOAT fSpeedControl = 0.0f;
    if( !m_bIsPaused )
    {
        // Left thumbstick/dpad controls the buggy wheels 
        fTurnControl = pGamepad->fX1;
        if( fTurnControl == 0.0f )
        {
            if( pGamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT )
                fTurnControl = -1.0f;
            else if( pGamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
                fTurnControl = 1.0f;
        }

        // Right trigger controls acceleration
        // Left trigger controls braking
        BYTE nAccel = pGamepad->bAnalogButtons[ XINPUT_GAMEPAD_RIGHT_TRIGGER ];
        BYTE nBrake = pGamepad->bAnalogButtons[ XINPUT_GAMEPAD_LEFT_TRIGGER ];
        FLOAT fAccel = (FLOAT)nAccel / 255.0f;
        FLOAT fBrake = -(FLOAT)nBrake / 255.0f;
        fSpeedControl = fAccel > 0.0f ? fAccel : fBrake;

        fTurnControl *= fabsf(fTurnControl);
        fSpeedControl *= fabsf(fSpeedControl);
    }

    // Update vehicle
    if( m_pVehicle != NULL )
    {
        Vehicle* pVehicle = reinterpret_cast< Vehicle* >
                                        ( m_pVehicle->GetController() );
        pVehicle->SetTurnAngle( fTurnControl * 0.5f );
        pVehicle->SetSpeedControl( fSpeedControl );
    }

    // Run physics
    FLOAT fVibration = m_pPhysicsSystem->Simulate( fTime - fElapsedTime, fTime );

    // Do controller vibration (no more than once every tenth second)
    if( m_bIsVibrationOn && !m_bIsPaused && 
        m_VibrationTimer.GetElapsedSeconds() > 0.1f )
    {
        if( fVibration < 0.2f && fSpeedControl != 0.0f )
            fVibration = 0.2f;
        Controller::SetVibration( pGamepad, fVibration, fVibration );
        m_VibrationTimer.StartZero();
    }

    // Detect the state where the vehicle has overturned and start over.
    // The quick and dirty way to easily detect is examine the rumble 
    // state, which is correlated to the physics collision states.
    if( fVibration == 1.0f && !m_bPlayingDemo )
    {
        if( m_VehicleFlippedTimer.IsRunning() )
        {
            if( m_VehicleFlippedTimer.GetElapsedSeconds() > 6.0f )
            {
                Controller::SetVibration( pGamepad, 0.0f, 0.0f );
                End();
                Start( FALSE, FALSE, m_bIsVibrationOn, m_fMusicVolume, 
                       m_fEffectVolume, NULL );
            }
        }
        else
        {
            m_VehicleFlippedTimer.StartZero();
        }
    }
    else
    {
        m_VehicleFlippedTimer.Stop();
    }

    // Update draw matrices from physics state
    for( INT i = 0; i < m_iNumActors; ++i )
    {
        m_pActors[i]->Update();
    }

    // Update the camera to follow the vehicle
    if( m_pVehicle != NULL )
    {
        PhysicsObject* pController = m_pVehicle->GetController();
        Vector3 vVel = pController->GetVelocity();

        m_vUpVec = Vector3(0.0f, 1.0f, 0.0f);

        Vector3 back = Vector3(0.0f, 0.0f, -10.0f) * 
                       pController->GetRotation() +
                       pController->GetTranslation();

        Vector3 p1 = Vector3( 0.0f, 10000.0f, 0.0f ) + back;
        Vector3 p2 = Vector3( 0.0f, -10000.0f, 0.0f ) + back;

        FLOAT t;
        Vector3 q = p2;
        m_pPhysicsSystem->CheckLineSegment( p1, p2, &t, &q );

        m_vEyePt = q + Vector3( 0.0f, 2.0f, 0.0f );

        m_vLookatPt = Vector3( 0.0f, 1.0f, 0.0f ) +
                      pController->GetTranslation();

        D3DXMATRIX matView;
        D3DXMatrixLookAtLH( &matView, &m_vEyePt, &m_vLookatPt, &m_vUpVec );
        g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

        // Set the pitch of the engine sound to the current speed of the vehicle
        // Not the best equation, but it works fairly well...  The places
        // where it doesn't work is when the accelerator is held, but the buggy
        // isn't going anywhere.
        m_SoundEffect.SetPitch( MAX( (LONG)(10 * vVel * vVel), (LONG)(10 * fSpeedControl) ) );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame for 3d rendering of the game
//-----------------------------------------------------------------------------
HRESULT Game::Render( const WCHAR* strFrameRate )
{
    // Play any sound waiting to happen
    if( m_bIsPaused )
    {
        m_MusicManager.Pause();
        m_SoundEffect.Pause();
    } else
    {
        // Unpause music if paused
        if( m_MusicManager.GetStatus() == MM_PAUSED )
            m_MusicManager.Play();

        m_SoundEffect.PlaySound();
        DirectSoundDoWork();
    }

    // Clear the viewport
    g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | 
                         D3DCLEAR_STENCIL, 0x000000FF, 1.0f, 0L );

    // Restore default states
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

    // Add default light
    D3DLIGHT8 light;
    ZeroMemory( &light, sizeof(light) );
    
    light.Type       = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r  = 1.0f;
    light.Diffuse.g  = 1.0f;
    light.Diffuse.b  = 1.0f;
    D3DXVECTOR3 v(2.0f, -2.0f, 1.0f);
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &v );

    g_pd3dDevice->SetLight( 0, &light );
    g_pd3dDevice->LightEnable( 0, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xff606060 );

    // Draw the actors
    for( INT i = 0; i < m_iNumActors; ++i )
        m_pActors[i]->Render();

    if( g_bDrawDebugLines && g_DebugLines.size() > 0 )
    {
        // Draw debug info
        D3DXMATRIX matWorld;
        D3DXMatrixIdentity( &matWorld );
        g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
        g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
        g_pd3dDevice->SetVertexShader( D3DFVF_XYZ | D3DFVF_DIFFUSE );
        g_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, g_DebugLines.size(), 
                                       &g_DebugLines[0], sizeof(DebugLine)/2 );
    }

/*
    // Show frame rate
    m_pStatsFont->DrawText( 2,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );

    TCHAR strStats[128];
    _stprintf(strStats, _T("%.02f %.02f %.02f %.02f"), g_PhysicsTime*1000.0, g_CollisionTime*1000.0, g_TransformTime*1000.0f, g_ClipTime*1000.0f);
    m_pStatsFont->DrawText( 2, 16, D3DCOLOR_ARGB(255,255,255,0), strStats );
*/

    g_PhysicsTime = 0.0f;
    g_CollisionTime = 0.0f;
    g_TransformTime = 0.0f;
    g_ClipTime = 0.0f;

    DWORD dwLang = CXBConfig::GetLanguage();

    if( dwLang != XC_LANGUAGE_JAPANESE )
        m_Font.DrawText( 64, 50, 0xffffff00, strGAME_NAME[ dwLang ] );
    else
        m_XFontJPN.DrawText( 64, 50, 0xffffff00, strGAME_NAME[ dwLang ] );

    // Show frame rate in debug mode
#if _DEBUG
    if( strFrameRate != NULL )
        m_Font.DrawText( 450, 50, 0xffffff00, strFrameRate );
#else
    (VOID)strFrameRate;
#endif

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ShowLoadingScreen()
// Desc: Renders load screen
//-----------------------------------------------------------------------------
VOID Game::ShowLoadingScreen()
{
    // Clear the viewport
    g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | 
                         D3DCLEAR_STENCIL, 0x000A0A6A, 1.0f, 0L );

    DWORD dwLang = CXBConfig::GetLanguage();

    // Show loading text
    if( dwLang != XC_LANGUAGE_JAPANESE )
        m_Font.DrawText( 320, 220, 0xFFFFFFFF, strLOADINGX[ dwLang ], 
                         XBFONT_CENTER_X );
    else
        m_XFontJPN.DrawText( 320, 220, 0xFFFFFFFF, strLOADINGX[ dwLang ], 
                         CXFONT_CENTER_X );

    // Show the screen
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}




//-----------------------------------------------------------------------------
// Name: SetPaused()
// Desc: Sets the pause state of the game
//-----------------------------------------------------------------------------
VOID Game::SetPaused( BOOL bPaused )
{
    m_bIsPaused = bPaused;

    // Take a snapshot of the screen for saved games
    if( m_bIsPaused )
    {
        assert( m_ptScreenShot != NULL );

        LPDIRECT3DSURFACE8 pSurfScreenShot;
        HRESULT hr = m_ptScreenShot->GetSurfaceLevel( 0, &pSurfScreenShot );
        assert( SUCCEEDED(hr) );

        // Copy the center of the backbuffer into the screen shot texture
        const INT nWidth  = INT( SCREEN_SHOT_SIZE );
        const INT nHeight = INT( SCREEN_SHOT_SIZE );

        const POINT ptSrc = { ( 640 - nWidth ) / 2, ( 480 - nHeight ) / 2 };
        const RECT rc = { ptSrc.x,          ptSrc.y, 
                          ptSrc.x + nWidth, ptSrc.y + nWidth };
        const POINT ptDest = { 0, 0 };

        LPDIRECT3DSURFACE8 pBackBuffer;
        hr = g_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO,
                                          &pBackBuffer );
        assert( SUCCEEDED(hr) );

        hr = g_pd3dDevice->CopyRects( pBackBuffer, &rc, 1, 
                                      pSurfScreenShot, &ptDest );
        assert( SUCCEEDED(hr) );
        pSurfScreenShot->Release();
        pBackBuffer->Release();
    }
}




//-----------------------------------------------------------------------------
// Name: IsPaused()
// Desc: TRUE if the game is in the paused state
//-----------------------------------------------------------------------------
BOOL Game::IsPaused() const
{
    return m_bIsPaused;
}




//-----------------------------------------------------------------------------
// Name: GetScreenShot()
// Desc: Returns a screen shot texture for saved games
//-----------------------------------------------------------------------------
LPDIRECT3DTEXTURE8 Game::GetScreenShot() const
{
    return m_ptScreenShot;
}




//-----------------------------------------------------------------------------
// Name: GetSaveGameSize()
// Desc: Get saved game data size in bytes
//-----------------------------------------------------------------------------
DWORD Game::GetSaveGameSize()
{
    DWORD dwSize = m_pPhysicsSystem->GetStateSize() * sizeof(FLOAT);

    // Make this a realistic save game size for testing purposes
    if( dwSize < SAVE_GAME_SIZE_MIN )
        dwSize = SAVE_GAME_SIZE_MIN;
    return dwSize;
}




//-----------------------------------------------------------------------------
// Name: GetSaveGameData()
// Desc: Get saved game data into buffer.
//-----------------------------------------------------------------------------
VOID Game::GetSaveGameData( BYTE* pData )
{
    // TCR 2-4 Saved Game Roaming
    // TCR 3-6 Memory Unit Roaming
    // None of the data within the saved game references other content

    // In the future we would save the world objects here

    // Save physics system state
    m_pPhysicsSystem->GetState( (FLOAT*)pData );
}




//-----------------------------------------------------------------------------
// Name: LoadSaveGame()
// Desc: Load saved game from buffer.
//-----------------------------------------------------------------------------
VOID Game::LoadSaveGame( const BYTE* pData, DWORD dwSize )
{
    USED( dwSize );

    // In the future we would create the world objects here

    // Load physics system state
    m_pPhysicsSystem->SetState( (const FLOAT*)pData );
}




//-----------------------------------------------------------------------------
// Name: GetMaxSaveGameSize()
// Desc: Get maximum saved game data size in bytes
//-----------------------------------------------------------------------------
DWORD Game::GetMaxSaveGameSize() const
{
    DWORD dwMaxSize = m_pPhysicsSystem->GetMaxStateSize() * sizeof(FLOAT);

    // Make this a realistic save game size for testing purposes
    if( dwMaxSize < SAVE_GAME_SIZE_MAX )
        dwMaxSize = SAVE_GAME_SIZE_MAX;
    return dwMaxSize;
}




//-----------------------------------------------------------------------------
// Name: AddBoxActor()
// Desc: Adds a "box" actor to the scene and returns a pointer to the
//       new actor.
//-----------------------------------------------------------------------------
Actor* Game::AddBoxActor( const D3DXVECTOR3& pos, const D3DXQUATERNION& quat, 
                          D3DCOLOR color, FLOAT width, FLOAT height, 
                          FLOAT depth, FLOAT mass)
{
    // Create render mesh
    D3DMATERIAL8 Material;
    ZeroMemory( &Material, sizeof(Material) );
    Material.Diffuse.r = Material.Ambient.r = ((color >> 16) & 0xff) / 256.0f;
    Material.Diffuse.g = Material.Ambient.g = ((color >> 8) & 0xff) / 256.0f;
    Material.Diffuse.b = Material.Ambient.b = ((color >> 0) & 0xff) / 256.0f;
    Material.Diffuse.a = Material.Ambient.a = ((color >> 24) & 0xff) / 256.0f;
    Mesh* pMesh = new Box( width, height, depth, Material ); 

    // Create physics controller
    PhysicsObject* pController = new PhysicsObject( m_pPhysicsSystem, 
                                            Vector3(pos), Quaternion(quat) );

    // Create shape
    Polyhedron* pBox = new Polyhedron( width, height, depth );

    Matrix3 InertiaTensor;
    if( mass > 0.0f )
    {
        // Compute inertia tensor
        FLOAT coeff = mass / 12.0f;

        // Inertia tensor of a block
        InertiaTensor.m[0][0] = coeff * (height*height + depth*depth);
        InertiaTensor.m[1][1] = coeff * (width*width + depth*depth);
        InertiaTensor.m[2][2] = coeff * (width*width + height*height);
    }

    return AddActor( pos, quat, pMesh, pController, pBox, mass, InertiaTensor );
}




//-----------------------------------------------------------------------------
// Name: AddSphereActor()
// Desc: Adds a "sphere" actor to the scene and returns a pointer to the
//       new actor.
//-----------------------------------------------------------------------------
Actor* Game::AddSphereActor( const D3DXVECTOR3& pos, const D3DXQUATERNION& quat, 
                             D3DCOLOR color, FLOAT radius, FLOAT mass )
{
    // Create render mesh
    D3DMATERIAL8 Material;
    ZeroMemory( &Material, sizeof(D3DMATERIAL8) );
    Material.Diffuse.r = Material.Ambient.r = ((color >> 16) & 0xff) / 256.0f;
    Material.Diffuse.g = Material.Ambient.g = ((color >> 8) & 0xff) / 256.0f;
    Material.Diffuse.b = Material.Ambient.b = ((color >> 0) & 0xff) / 256.0f;
    Material.Diffuse.a = Material.Ambient.a = ((color >> 24) & 0xff) / 256.0f;
    Mesh* pMesh = new Sphere( radius, 12, 12, Material );

    // Create physics controller
    PhysicsObject* pController = new PhysicsObject( m_pPhysicsSystem, 
                                                Vector3(pos), Quaternion(quat) );

    // Create shape
    Polyhedron* pSphere = new Polyhedron( radius, 8, 8 );

    Matrix3 InertiaTensor;
    if( mass > 0.0f )
    {
        // Compute inertia tensor.
        FLOAT coeff = mass * 1.0f / 8.0f * fPI * radius * radius;

        // Inertia tensor of a sphere
        InertiaTensor.m[0][0] = coeff;
        InertiaTensor.m[1][1] = coeff;
        InertiaTensor.m[2][2] = coeff;

        /*
        float fDensity = mass / (4.0f / 3.0f * fPI * radius * radius * radius);
        float fMass;
        Vector3 v3CoM;
        Matrix3 m3IT;
        pSphere->ComputeMassProperties(fDensity, &fMass, &v3CoM, &m3IT);
        */
    }

    return AddActor( pos, quat, pMesh, pController, pSphere, mass, InertiaTensor );
}




//-----------------------------------------------------------------------------
// Name: AddMeshActor()
// Desc: Adds a mesh actor to the scene and returns a pointer to the new actor
//-----------------------------------------------------------------------------
Actor* Game::AddMeshActor( const D3DXVECTOR3& pos, const D3DXQUATERNION& quat, 
                           Mesh* pMesh, BOOL bOwnMesh )
{
    assert( m_iNumActors < MAX_ACTORS );

    m_pActors[ m_iNumActors ] = new Actor( m_pPhysicsSystem );
    Actor& act = *m_pActors[ m_iNumActors ];
    ++m_iNumActors;

    // Set actor position
    D3DXMATRIX mat;
    D3DXMatrixTransformation( &mat, NULL, NULL, NULL, NULL, &quat, &pos );
    act.SetMatrix( mat );

    // Set actor render mesh
    act.SetMesh( pMesh, bOwnMesh );

    return &act;
}



//-----------------------------------------------------------------------------
// Name: AddVehicleActor()
// Desc: Adds a vehicle to the scene and returns a pointer to the new actor.
//-----------------------------------------------------------------------------
VehicleActor* Game::AddVehicleActor( const D3DXVECTOR3& pos, const D3DXQUATERNION& quat, 
                              VehicleIDs ID )
{
    // Create the meshes.
    File fileBaseMesh;
    if( !fileBaseMesh.Open( g_Vehicles[ID].strModel, GENERIC_READ ) )
    {
        OUTPUT_DEBUG_STRING( "Failed to open vehicle mesh file" );
        return 0;
    }

    Mesh* pBaseMesh = new Mesh();
    pBaseMesh->Load( fileBaseMesh );

    File fileFrontTireMesh;
    if( !fileFrontTireMesh.Open( g_Vehicles[ID].strFrontTireModel, GENERIC_READ ) )
    {
        OUTPUT_DEBUG_STRING( "Failed to open vehicle mesh file" );
        return 0;
    }

    Mesh* pFrontTireMesh = new Mesh();
    pFrontTireMesh->Load( fileFrontTireMesh );

    File fileRearTireMesh;
    if( !fileRearTireMesh.Open( g_Vehicles[ID].strRearTireModel, GENERIC_READ ) )
    {
        OUTPUT_DEBUG_STRING( "Failed to open vehicle mesh file" );
        return 0;
    }

    Mesh* pRearTireMesh = new Mesh();
    pRearTireMesh->Load( fileRearTireMesh );

    // Create physics controller
    PhysicsObject* pController = new Vehicle( m_pPhysicsSystem, Vector3(pos), 
                                          Quaternion(quat), 
                                          (Vector3*)g_Vehicles[ID].v3WheelOffset, 
                                          g_Vehicles[ID].fWheelRadius );

    D3DXVECTOR3 dim = pBaseMesh->GetBoxMax() - pBaseMesh->GetBoxMin();

    float fMass = 100.0f;
    float width = dim.x;
    float height = dim.y;
    float depth = dim.z;

    // Create the shape
    Polyhedron* pShape = new Polyhedron( pBaseMesh->GetBoxMin(), pBaseMesh->GetBoxMax() );

    Matrix3 InertiaTensor;

    // Compute inertia tensor
    FLOAT coeff = fMass / 12.0f;

    // Inertia tensor of a block.
    InertiaTensor.m[0][0] = coeff * (height*height + depth*depth);
    InertiaTensor.m[1][1] = coeff * (width*width + depth*depth);
    InertiaTensor.m[2][2] = coeff * (width*width + height*height);

    // Body.
    VehicleActor* pBaseActor = new VehicleActor( m_pPhysicsSystem );
    m_pActors[ m_iNumActors ] = pBaseActor;
    Actor& act = *m_pActors[ m_iNumActors ];
    ++m_iNumActors;

    pBaseActor->m_ID = ID;

    // Set actor position
    D3DXMATRIX mat;
    D3DXMatrixTransformation( &mat, NULL, NULL, NULL, NULL, &quat, &pos );
    act.SetMatrix( mat );

    // Set actor render mesh
    act.SetMesh( pBaseMesh );

    // Create physics controller
    act.SetController( pController );
    pController->SetShape( pShape );
    pController->SetPhysicalProperties( fMass, InertiaTensor );

    // Add to the physics system
    m_pPhysicsSystem->AddObject( pController );

    // Wheels.
    Actor* pWheel;

    pWheel = AddMeshActor( g_Vehicles[ID].v3WheelOffset[0], 
                           Quaternion( Vector3(0.0f, 1.0f, 0.0f), D3DX_PI), pFrontTireMesh );

    pWheel->SetParent( pBaseActor );
    pBaseActor->m_pWheels[0] = pWheel;

    pWheel = AddMeshActor( g_Vehicles[ID].v3WheelOffset[1], 
                           Quaternion(), pFrontTireMesh, FALSE );

    pWheel->SetParent( pBaseActor );
    pBaseActor->m_pWheels[1] = pWheel;

    pWheel = AddMeshActor( g_Vehicles[ID].v3WheelOffset[2], 
                           Quaternion( Vector3(0.0f, 1.0f, 0.0f), D3DX_PI), pRearTireMesh );

    pWheel->SetParent( pBaseActor );
    pBaseActor->m_pWheels[2] = pWheel;

    pWheel = AddMeshActor( g_Vehicles[ID].v3WheelOffset[3], 
                           Quaternion(), pRearTireMesh, FALSE );

    pWheel->SetParent( pBaseActor );
    pBaseActor->m_pWheels[3] = pWheel;

    return pBaseActor;
}




//-----------------------------------------------------------------------------
// Name: AddActor()
// Desc: Adds a generic actor to the scene and returns a pointer to the
//       new actor.
//-----------------------------------------------------------------------------
Actor* Game::AddActor( const D3DXVECTOR3& pos, const D3DXQUATERNION& quat,
                       Mesh* pMesh, PhysicsObject* pController,
                       Polyhedron* pPoly, FLOAT fMass, 
                       const Matrix3& InertiaTensor )
{
    assert( pMesh != NULL );
    assert( pController != NULL );
    assert( pPoly != NULL );
    assert( m_iNumActors < MAX_ACTORS );

    m_pActors[ m_iNumActors ] = new Actor( m_pPhysicsSystem );
    Actor& act = *m_pActors[m_iNumActors];
    ++m_iNumActors;

    // Set actor position
    D3DXMATRIX mat;
    D3DXMatrixTransformation( &mat, NULL, NULL, NULL, NULL, &quat, &pos );
    act.SetMatrix( mat );

    // Set actor render mesh
    act.SetMesh( pMesh );

    // Create physics controller
    act.SetController( pController );
    act.GetController()->SetShape( pPoly );
    act.GetController()->SetPhysicalProperties( fMass, InertiaTensor );

    // Add to the physics system
    m_pPhysicsSystem->AddObject( pController );

    return &act;
}




//-----------------------------------------------------------------------------
// Name: CheckForFreeSpace()
// Desc: Allows reboot to Dash if not enough space for saving games on the
//       hard drive
//-----------------------------------------------------------------------------
VOID Game::CheckForFreeSpace() const
{
    // TCR 3-2 Hard Disk Saved Game Free Space
    DWORD dwBytesRequired = LoadSave::GetGameSaveMaxSize( GetMaxSaveGameSize() );

    ULONGLONG qwTotalBytes;
    ULONGLONG qwUsedBytes;
    ULONGLONG qwFreeBytes;
    CXBStorageDevice HardDisk( 'U' );
    HardDisk.GetSize( qwTotalBytes, qwUsedBytes, qwFreeBytes );

    DWORD dwLang = CXBConfig::GetLanguage();

    if( qwFreeBytes < ULONGLONG(dwBytesRequired) )
    {
        // Clear the viewport
        g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | 
                             D3DCLEAR_STENCIL, 0x000A0A6A, 1.0f, 0L );

        DWORD dwBlocksNeeded = dwBytesRequired /
                               CXBStorageDevice::GetBlockSize();

        // TCR 6-8 Low Storage
        // Show text
        WCHAR strFree[ 256 ];
        wsprintfW( strFree, strFREE[ dwLang ], dwBlocksNeeded );
        if( dwLang != XC_LANGUAGE_JAPANESE )
            m_Font.DrawText( 320, 160, 0xFFFFFFFF, strFree, XBFONT_CENTER_X );
        else
            m_XFontJPN.DrawText( 320, 160, 0xFFFFFFFF, strFree, CXFONT_CENTER_X );

        // Show the screen
        g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        CXBStopWatch SoftResetTimer( FALSE );
        for( ;; )
        {
            XBInput_GetInput();
            const XBGAMEPAD* pGamepad = Controller::GetPrimaryController();
            if( pGamepad != NULL )
            {
                if( pGamepad->bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] ||
                    pGamepad->wPressedButtons & XINPUT_GAMEPAD_START )
                    return;

                // TCR 3-4 Cleanup Support
                // Launch to the Dash memory area to clean up
                if( pGamepad->bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] )
                {
                    // Reboot to Dash
                    LD_LAUNCH_DASHBOARD LaunchDash;
                    LaunchDash.dwReason = XLD_LAUNCH_DASHBOARD_MEMORY;
                    LaunchDash.dwContext = 0;
                    LaunchDash.dwParameter1 = DWORD( HardDisk.GetDrive() );
                    LaunchDash.dwParameter2 = dwBlocksNeeded;

                    XLaunchNewImage( NULL, (PLAUNCH_DATA)(&LaunchDash) );

                    // We never get here
                }
            }
        }
    }
}
