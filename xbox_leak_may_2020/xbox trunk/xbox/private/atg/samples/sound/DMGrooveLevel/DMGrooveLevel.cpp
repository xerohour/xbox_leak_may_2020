//-----------------------------------------------------------------------------
// File: DMGrooveLevel.cpp
//
// Desc: Simple app to play some sounds using DirectMusic.
//
// Hist: 01.12.00 - New for February XDK release
//       02.16.01 - Renamed from DMApp for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBUtil.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <dmusici.h>
#include <dsstdfx.h>
#include "myfactory.h"



//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Adjust\nvolume" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Adjust groove\nlevel" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_1, L"Sound Effect #1" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_1, L"Sound Effect #2" },
};

#define NUM_HELP_CALLOUTS 5



//-----------------------------------------------------------------------------
// Global DirectMusic variables
//-----------------------------------------------------------------------------
IDirectMusicLoader8*      g_pLoader         = NULL;
IDirectMusicPerformance8* g_pPerformance    = NULL;
IDirectMusicSegment8*     g_pMusic          = NULL;
IDirectMusicSegment8*     g_pFastMusic      = NULL;
IDirectMusicSegment8*     g_pSound1         = NULL;
IDirectMusicSegment8*     g_pSound2         = NULL;
IDirectMusicAudioPath8*   g_p3DAudioPath1   = NULL;
IDirectMusicAudioPath8*   g_pMusicAudioPath = NULL;




//-----------------------------------------------------------------------------
// Name: InitDirectMusic
// Desc: Init DM
//-----------------------------------------------------------------------------
HRESULT InitDirectMusic() 
{
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
                               IID_IDirectMusicLoader8, (VOID**)&g_pLoader );

    // Create performance object
    DirectMusicCreateInstance( CLSID_DirectMusicPerformance, NULL,
                               IID_IDirectMusicPerformance8, (VOID**)&g_pPerformance );

    // Initialize the performance with the standard audio path.
    // The flags (final) argument allows us to specify whether or not we want
    // DirectMusic to create a thread on our behalf to process music, using 
    // DMUS_INITAUDIO_NOTHREADS.  The default is for DirectMusic to create its
    // own thread; DMUS_INITAUDIO_NOTHREADS tells DirectMusic not to do this, 
    // and the app will periodically call DirectMusicDoWork().  For software 
    // emulation on alpha hardware, it's generally better to have DirectMusic
    // create its own thread. On real hardware, periodically calling 
    // DirectMusicDoWork may provide a better option.
    g_pPerformance->InitAudioX( DMUS_APATH_SHARED_STEREOPLUSREVERB, 64, 128, 0 );

    // Tell DirectMusic where the default search path is
    g_pLoader->SetSearchDirectory( GUID_DirectMusicAllTypes, 
                                   "D:\\Media\\Sounds", FALSE );
    
    // Load main music
    g_pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment, IID_IDirectMusicSegment8, 
                                   "xdk1.sgt", (VOID**)&g_pMusic );

    // Load a sound effect
    g_pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment, IID_IDirectMusicSegment8, 
                                   "xdk1.wav", (VOID**)&g_pSound1 );

    // load another sound effect
    g_pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment, IID_IDirectMusicSegment8, 
                                   "xdk2.wav", (VOID**)&g_pSound2 );

    // Set the music to repeat for a while...
    g_pMusic->SetRepeats( 100 );
    
    // Play segment on the default audio path
    g_pPerformance->PlaySegmentEx( g_pMusic, NULL, NULL, 0, 
                                   0, NULL, NULL, NULL );

    // Get default (music) audiopath.
    g_pPerformance->GetDefaultAudioPath( &g_pMusicAudioPath );

    // Max volume for music
    g_pMusicAudioPath->SetVolume( (100*100)-10000, 0 );

    /* If the application doesn't care about vertical HRTF positioning,
       calling DirectSoundUseLightHRTF can save about 60k of memory. */
    // DirectSoundUseLightHRTF();
    DirectSoundUseFullHRTF();

    // Create a 3D audiopath
    g_pPerformance->CreateStandardAudioPath( DMUS_APATH_DYNAMIC_3D, 64,
                                             TRUE, &g_p3DAudioPath1 );

    // Now DirectMusic will play in the background, so continue on with our task
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: PlaySound()
// Desc: Play sound on a single audiopath
//-----------------------------------------------------------------------------
HRESULT PlaySound( IDirectMusicSegment8* pSound )
{
    g_pPerformance->PlaySegmentEx( pSound, NULL, NULL, DMUS_SEGF_SECONDARY, 
                                   0, NULL, NULL, g_p3DAudioPath1 );
        
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetVolume()
// Desc: Set volume on the main music audipath
// Note: The argument to IDirectMusicAudioPath::SetVolume is attenuation in
//       hundredths of a dB.  Attenuation more than 60dB is silent, so
//       that's our effective minimum.  
//-----------------------------------------------------------------------------
HRESULT SetVolume( LONG lVolume )
{
    // Change range from [0,100] to [-6000,0]
    lVolume = lVolume * 60 - 6000;

    g_pMusicAudioPath->SetVolume( lVolume, 0 );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetGrooveLevel()
// Desc: Change the groove level of the performance
//-----------------------------------------------------------------------------
HRESULT SetGrooveLevel( INT dwGrooveLevel )
{
    CHAR chGrooveLevel = (CHAR)dwGrooveLevel;
    g_pPerformance->SetGlobalParam( GUID_PerfMasterGrooveLevel, 
                                    &chGrooveLevel, sizeof(CHAR) );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont           m_Font;         // Font object
    CXBHelp           m_Help;         // Help object
    BOOL              m_bDrawHelp;    // Should we draw help?

    FLOAT             m_fGrooveLevel;
    FLOAT             m_fVolume;
    LPDIRECTSOUND8    m_pDSound;      // DSound object

public:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();

    CXBoxSample();

    HRESULT DownloadEffectsImage(PCHAR pszScratchFile);  // downloads a default DSP image to the GP
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
// Desc: Application class constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
{
    m_fGrooveLevel =  50.0f;
    m_fVolume      = 100.0f;        
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
    if( FAILED( InitDirectMusic() ) )
        return E_FAIL;

    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED(DirectSoundCreate( NULL, &m_pDSound, NULL ) ) )
		return E_FAIL;

    DownloadEffectsImage("d:\\media\\dsstdfx.bin");

    m_bDrawHelp = FALSE;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: IsAnyButtonActive()
// Desc: TRUE if any button depressed or any thumbstick offset on the given
//       controller.
//-----------------------------------------------------------------------------
BOOL IsAnyButtonActive( const XBGAMEPAD* pGamePad )
{
    // Check digital buttons
    if( pGamePad->wButtons )
        return TRUE;

    // Check analog buttons
    for( DWORD i = 0; i < 8; ++i )
    {
        if( pGamePad->bAnalogButtons[ i ] )
            return TRUE;
    }

    // Check thumbsticks
    if( pGamePad->fX1 || pGamePad->fY1 || pGamePad->fX2 || pGamePad->fX2  )
        return TRUE;

    // Nothing active
    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: GetPrimaryController()
// Desc: The primary controller is the first controller used by a player.
//       If no controller has been used or the controller has been removed,
//       the primary controller is the controller inserted at the lowest 
//       port number. Function returns NULL if no controller is inserted.
//-----------------------------------------------------------------------------
const XBGAMEPAD* GetPrimaryController()
{
    static INT nPrimaryController = -1;

    // If primary controller has been set and hasn't been removed, use it
    const XBGAMEPAD* pGamePad = NULL;
    if( nPrimaryController != -1 )
    {
        pGamePad = &g_Gamepads[ nPrimaryController ];
        if( pGamePad->hDevice != NULL )
            return pGamePad;
    }

    // Primary controller hasn't been set or has been removed...

    // Examine each inserted controller to see if any is being used
    INT nFirst = -1;
    for( DWORD i=0; i < XGetPortCount(); ++i )
    {
        pGamePad = &g_Gamepads[i];
        if( pGamePad->hDevice != NULL )
        {
            // Remember the lowest inserted controller ID
            if( nFirst == -1 )
                nFirst = i;

            // If any button is active, we found the primary controller
            if( IsAnyButtonActive( pGamePad ) )
            {
                nPrimaryController = i;
                return pGamePad;
            }
        }
    }

    // No controllers are inserted
    if( nFirst == -1 )
        return NULL;

    // The primary controller hasn't been set and no controller has been
    // used yet, so return the controller on the lowest port number
    pGamePad = &g_Gamepads[ nFirst ];
    return pGamePad;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    const XBGAMEPAD* pGamepad = GetPrimaryController();
    if( pGamepad == NULL )
        return S_OK;

    // Toggle help
    if( pGamepad->wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    // Adjust volume
    m_fVolume += pGamepad->fY1 * m_fElapsedTime * 100.0f;
    if( m_fVolume <   1.0f )   m_fVolume =   1.0f;
    if( m_fVolume > 100.0f )   m_fVolume = 100.0f;
    SetVolume( (INT)m_fVolume);

    // Adjust sound
    m_fGrooveLevel += pGamepad->fY2 * m_fElapsedTime * 100.0f;
    if( m_fGrooveLevel <   1.0f )   m_fGrooveLevel =   1.0f;
    if( m_fGrooveLevel > 100.0f )   m_fGrooveLevel = 100.0f;
    SetGrooveLevel( (INT)(m_fGrooveLevel - 50) );

    // Play sound based on button pressed
    if( pGamepad->bPressedAnalogButtons[XINPUT_GAMEPAD_X] ) 
        PlaySound( g_pSound1 );

    if( pGamepad->bPressedAnalogButtons[XINPUT_GAMEPAD_A] ) 
        PlaySound( g_pSound2 );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0xff0000ff, 1.0f, 0L );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.DrawText(  64, 50, 0xffffffff, L"DMGrooveLevel" );
        m_Font.DrawText( 64, 150, 0xffffff00, L"Volume:" );
        {
            struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
            BACKGROUNDVERTEX v[4];
            FLOAT x1 = 200, x2 = x1 + (340*m_fVolume)/100;
            FLOAT y1 = 150, y2 = y1 + 20;
            v[0].p = D3DXVECTOR4( x1-0.5f, y1-0.5f, 1.0f, 1.0f );  v[0].color = 0xffffffff;
            v[1].p = D3DXVECTOR4( x2-0.5f, y1-0.5f, 1.0f, 1.0f );  v[1].color = 0xffffffff;
            v[2].p = D3DXVECTOR4( x1-0.5f, y2-0.5f, 1.0f, 1.0f );  v[2].color = 0xffff0000;
            v[3].p = D3DXVECTOR4( x2-0.5f, y2-0.5f, 1.0f, 1.0f );  v[3].color = 0xffff0000;

            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
            m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
            m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );
        }

        m_Font.DrawText( 64, 200, 0xffffff00, L"Groove Level:" );
        {
            struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
            BACKGROUNDVERTEX v[4];
            FLOAT x1 = 200, x2 = x1 + (340*m_fGrooveLevel)/100;
            FLOAT y1 = 200, y2 = y1 + 20;
            v[0].p = D3DXVECTOR4( x1-0.5f, y1-0.5f, 1.0f, 1.0f );  v[0].color = 0xffffffff;
            v[1].p = D3DXVECTOR4( x2-0.5f, y1-0.5f, 1.0f, 1.0f );  v[1].color = 0xffffffff;
            v[2].p = D3DXVECTOR4( x1-0.5f, y2-0.5f, 1.0f, 1.0f );  v[2].color = 0xff00ff00;
            v[3].p = D3DXVECTOR4( x2-0.5f, y2-0.5f, 1.0f, 1.0f );  v[3].color = 0xff00ff00;

            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
            m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
            m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );
        }
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}
