//-----------------------------------------------------------------------------
// File: App.cpp
//
// Desc: Technical Certification Requirement Sample Game
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "App.h"
#include "Controller.h"
#include "Text.h"
#include <XbConfig.h>




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const CHAR*  const strDEMO_SCRIPT = "D:\\Media\\Demo.Script";
const FLOAT fSOFT_RESET = 3.0f; // time until reset occurs

// The maximum amount of time for inactivity is 75 seconds. Since the game
// actually requires a couple of seconds to start up, we only wait 65 seconds
// before initializing the game.
const FLOAT fINACTIVE_SECONDS = 65.0f; // time until attract mode begins

const FLOAT fSPLASH_SECONDS = 3.0f;





//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    TechCertGame xbApp;

    if( FAILED( xbApp.Create() ) )
        return;

    xbApp.Run();
}




//-----------------------------------------------------------------------------
// Name: TechCertGame()
// Desc: Constructor
//-----------------------------------------------------------------------------
TechCertGame::TechCertGame()
:
    CXBApplication  (),
    m_Font          (),
    m_XFontJPN      (),
    m_MusicManager  (),
    m_SoundEffect   (),
    m_GameMode      ( GAME_MODE_SPLASH ),     // Game begins in splash mode
    m_LastMode      ( GAME_MODE_MENU ),
    m_Splash        ( m_Font, m_XFontJPN ),
    m_StartScreen   ( m_Font, m_XFontJPN ),
    m_Menu          ( m_Font, m_XFontJPN ),
    m_Demo          ( m_Font, m_XFontJPN, m_MusicManager, m_SoundEffect ),
    m_Game          ( m_Font, m_XFontJPN, m_MusicManager, m_SoundEffect ),
    m_LoadSave      (),
    m_SoftResetTimer( FALSE ),
    m_pdsndDevice   ( NULL )
{
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: One time application initialization
//-----------------------------------------------------------------------------
HRESULT TechCertGame::Initialize()
{
    // Set the matrices
    D3DXVECTOR3 vEye(-2.5f, 2.0f, -4.0f );
    D3DXVECTOR3 vAt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );

    D3DXMATRIX matWorld, matView, matProj;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vEye,&vAt, &vUp );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 100.0f );

    m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Create the standard font
    if( FAILED( m_Font.Create( g_pd3dDevice, "Font16.xpr" ) ) )
    {
        OUTPUT_DEBUG_STRING( "Initialize: failed to load fonts\n" );
        return XBAPPERR_MEDIANOTFOUND;
    }

    // Create the JPN font
    // dwCacheSize is just larger then JPNFont.bmf filesize for max performance
    DWORD dwFontCacheSize = 1300000;
    if( FAILED( m_XFontJPN.Create( dwFontCacheSize,
                                   L"D:\\media\\fonts\\JPNFont20.bmf",
                                   m_pBackBuffer) ) )
    {
        OUTPUT_DEBUG_STRING( "Initialize: failed to load JPN Bitmap font" );
        return XBAPPERR_MEDIANOTFOUND;
    }

    // Create the DirectSound device
    if( FAILED( DirectSoundCreate(NULL, &m_pdsndDevice, NULL ) ) )
    {
        OUTPUT_DEBUG_STRING( "Initialize: failed to initialize direct sound\n" );
        return E_FAIL;
    }

    // Initialize the Sound Effect Object
    if( FAILED( m_SoundEffect.Initialize( m_pdsndDevice ) ) )
        return E_FAIL;

    // Enumerate soundtracks
    m_MusicManager.Initialize();

    // Splash starts
    m_Splash.Start();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT TechCertGame::FrameMove()
{
    // Detect controller state
    BOOL bHavePrimaryController = Controller::HavePrimaryController();

    // Access the primary controller (may be NULL)
    const XBGAMEPAD* pGamePad = Controller::GetPrimaryController();

    // TCR 3-62 Soft-reset
    if( pGamePad != NULL )
    {
        if( ( pGamePad->wButtons & XINPUT_GAMEPAD_START ) &&
            ( pGamePad->wButtons & XINPUT_GAMEPAD_BACK ) )
        {
            if( m_SoftResetTimer.IsRunning() )
            {
                // Reboot to self
                if( m_SoftResetTimer.GetElapsedSeconds() > fSOFT_RESET )
                    XLaunchNewImage( "D:\\Default.xbe", NULL );
            }
            else
            {
                m_SoftResetTimer.StartZero();
            }
        }
        else
        {
            m_SoftResetTimer.Stop();
        }
    }

    switch( m_GameMode )
    {
        case GAME_MODE_GAME:

            if( m_Game.IsPaused() )
            {
                m_Menu.FrameMove( pGamePad );

                if( pGamePad == NULL )
                    break;

                // If we just set the primary controller via the Start button,
                // ignore the button so that the initial Start doesn't 
                // activate a menu item
                if( !bHavePrimaryController &&
                    pGamePad->wPressedButtons & XINPUT_GAMEPAD_START )
                    break;

                // TCR 3-16 Menu Buttons
                // "A" button
                if( pGamePad->bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] ||
                    pGamePad->wPressedButtons & XINPUT_GAMEPAD_START )
                {
                    switch( m_Menu.GetCurrItem() )
                    {
                        case Menu::MENU_ITEM_RESUME:
                            m_Game.SetPaused( FALSE );
                            m_Menu.End();
                            break;
                        case Menu::MENU_ITEM_SAVE_GAME:
                        {
                            // Generate a save game name
                            WCHAR strGameName[ MAX_GAMENAME ];
                            GetSaveGameName( strGameName );

                            // Reserve space for the world state
                            m_LoadSave.SetGameData( strGameName, 
                                                    m_Game.GetSaveGameSize(),
                                                    m_Game.GetScreenShot() );
                            m_Game.GetSaveGameData( m_LoadSave.GetGameDataPtr() );

                            ChangeMode( GAME_MODE_SAVE );
                            break;
                        }
                        case Menu::MENU_ITEM_LOAD_GAME:
                            if( ConfirmQuit() )
                            {
                                ChangeMode( GAME_MODE_LOAD );
                            }
                            break;
                        case Menu::MENU_ITEM_QUIT:
                            if( ConfirmQuit() )
                            {
                                m_Game.End();
                                ChangeMode( GAME_MODE_MENU );
                            }
                            break;
                    }
                }

                // "B" button
                if( pGamePad->bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] ||
                    pGamePad->wPressedButtons & XINPUT_GAMEPAD_BACK )
                {
                    m_Game.SetPaused( FALSE );
                    m_Menu.End();
                }

                break;
            }

            m_Game.FrameMove( pGamePad, m_fTime, m_fElapsedTime );

            // TCR 3-18 In-Game Pause
            // TCR 3-22 Loss of Controller
            if( pGamePad == NULL ||
                pGamePad->wPressedButtons & XINPUT_GAMEPAD_START )
            {
                // TCR 3-19 Vibration Function Outside Gameplay
                Controller::SetVibration( pGamePad, 0.0f, 0.0f );
                m_Game.SetPaused( TRUE );
                m_Menu.Start( Menu::InGame );
            }

            break;

        case GAME_MODE_MENU:
        
            m_Menu.FrameMove( pGamePad );

            // TCR 3-38 Attract Mode Cycle
            if( m_Menu.GetInactiveSeconds() > fINACTIVE_SECONDS )
                ChangeMode( GAME_MODE_DEMO );

            if( pGamePad == NULL )
                break;

            // If we just set the primary controller via the Start button,
            // ignore the button so that the initial Start doesn't 
            // activate a menu item
            if( !bHavePrimaryController &&
                pGamePad->wPressedButtons & XINPUT_GAMEPAD_START )
                break;

            // TCR 3-16 Menu Buttons
            // "A" button
            if( pGamePad->bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] ||
                pGamePad->wPressedButtons & XINPUT_GAMEPAD_START )
            {
                switch( m_Menu.GetCurrItem() )
                {
                    case Menu::MENU_ITEM_START:
                        m_Game.SetPaused( FALSE );
                        ChangeMode( GAME_MODE_GAME );
                        break;
                    case Menu::MENU_ITEM_LOAD_GAME:
                        m_Game.SetPaused( FALSE );
                        ChangeMode( GAME_MODE_LOAD );
                        break;
                }
            }

            break;

        case GAME_MODE_DEMO:

            m_Demo.FrameMove( pGamePad, m_fTime, m_fElapsedTime );

            // Return to start screen when demo complete
            if( m_Demo.IsComplete() )
                ChangeMode( GAME_MODE_START );

            // TCR 3-40 Attract Mode Interrupt
            if( Controller::IsAnyButtonActive() || 
                Controller::AnyAdded() || 
                MemUnitWasInserted() )
            {
                ChangeMode( GAME_MODE_START );
            }
            break;

        case GAME_MODE_SPLASH:

            m_Splash.FrameMove( pGamePad );

            // If player pressed a controller button or 3 seconds expired, 
            // move on to start screen
            if( Controller::IsAnyButtonActive() || 
                m_Splash.GetElapsedSeconds() > fSPLASH_SECONDS )
            {
                ChangeMode( GAME_MODE_START );
            }

            break;

        case GAME_MODE_START:
        {
            m_StartScreen.FrameMove( pGamePad );

            // TCR 3-38 Attract Mode Cycle
            if( m_StartScreen.GetElapsedSeconds() > fINACTIVE_SECONDS )
                ChangeMode( GAME_MODE_DEMO );

            if( pGamePad == NULL )
                break;

            // If player pressed the start button, move to the main menu
            if( pGamePad->wPressedButtons & XINPUT_GAMEPAD_START )
                ChangeMode( GAME_MODE_MENU );

            break;
        }
        case GAME_MODE_LOAD:

            m_Game.FrameMove( pGamePad, m_fTime, m_fElapsedTime );
            m_LoadSave.FrameMove( pGamePad );

            if( m_LoadSave.WasCancelled() )
                ChangeMode( m_LastMode );

            if( m_LoadSave.IsGameLoaded() )
            {
                m_Game.End();
                m_Game.SetPaused( FALSE );
                ChangeMode( GAME_MODE_GAME );
            }

            break;

        case GAME_MODE_SAVE:

            m_Game.FrameMove( pGamePad, m_fTime, m_fElapsedTime );
            m_LoadSave.FrameMove( pGamePad );

            if( m_LoadSave.WasCancelled() ||
                m_LoadSave.WasGameSaved() )
            {
                // We don't need the save game information anymore
                m_LoadSave.FreeGameData();
                ChangeMode( GAME_MODE_GAME );
            }

            break;

        default:
            assert( FALSE );
            break;
    }
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT TechCertGame::Render()
{
    // Clear the viewport, zbuffer, and stencil buffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x000A0A6A, 1.0f, 0L );

    DWORD dwLang = CXBConfig::GetLanguage();

    switch( m_GameMode )
    {
        case GAME_MODE_GAME:    
            m_Game.Render( m_strFrameRate );
            if( m_Game.IsPaused() )
            {
                m_Menu.Render();
                const XBGAMEPAD* pGamePad = Controller::GetPrimaryController();
                if( pGamePad == NULL )
                {
                    // TCR 6-7 Lost Controller
                    if( dwLang != XC_LANGUAGE_JAPANESE )
                        m_Font.DrawText( 320, 350, 0xFFFFFFFF, 
                                         strRECONNECT_CNTRLR[ dwLang ],
                                         XBFONT_CENTER_X );
                    else
                        m_XFontJPN.DrawText( 320, 350, 0xFFFFFFFF, 
                                         strRECONNECT_CNTRLR[ dwLang ],
                                         CXFONT_CENTER_X );
                    Controller::ClearPrimaryController();
                }
            }
            break;
        case GAME_MODE_MENU:
        {
            m_Menu.Render();
            const XBGAMEPAD* pGamePad = Controller::GetPrimaryController();
            if( pGamePad == NULL )
            {
                // TCR 6-7 Lost Controller
                if( dwLang != XC_LANGUAGE_JAPANESE )
                    m_Font.DrawText( 320, 350, 0xFFFFFFFF, 
                                     strRECONNECT_CNTRLR[ dwLang ],
                                     XBFONT_CENTER_X );
                else
                    m_XFontJPN.DrawText( 320, 350, 0xFFFFFFFF, 
                                         strRECONNECT_CNTRLR[ dwLang ],
                                         CXFONT_CENTER_X );
                Controller::ClearPrimaryController();
            }
            break;
        }
        case GAME_MODE_DEMO:    m_Demo.Render();                    break;
        case GAME_MODE_SPLASH:  m_Splash.Render();                  break;
        case GAME_MODE_START:   m_StartScreen.Render();             break;
        case GAME_MODE_LOAD:    m_LoadSave.Render();                break;
        case GAME_MODE_SAVE:    m_LoadSave.Render();                break;
        default:                assert( FALSE );                    break;
    }
    
    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ChangeMode()
// Desc: Switch to new game mode
//-----------------------------------------------------------------------------
VOID TechCertGame::ChangeMode( GameMode iNewMode )
{
    // End the current mode
    switch( m_GameMode )
    {
        case GAME_MODE_GAME:
            // We never end a game in progress at this point, because
            // we might resume it later. Games are ended explicitly
            // when new games are started
            break;
        case GAME_MODE_MENU:    m_Menu.End();           break;
        case GAME_MODE_DEMO:    m_Demo.End();           break;
        case GAME_MODE_SPLASH:  m_Splash.End();         break;
        case GAME_MODE_START:   m_StartScreen.End();    break;
        case GAME_MODE_LOAD:    m_LoadSave.End();       break;
        case GAME_MODE_SAVE:    m_LoadSave.End();       break;
        default:                assert( FALSE );        break;
    }

    // Prepare for the new mode
    switch( iNewMode )
    {
        case GAME_MODE_GAME:
        {
            // Initialize the game
            BOOL bRecordDemo = FALSE; // Change to TRUE to record new demo script
            BOOL bPlayDemo = FALSE;
            m_Game.Start( bRecordDemo, bPlayDemo, m_Menu.IsVibrationOn(), 
                          m_Menu.GetMusicVolume(), m_Menu.GetEffectsVolume(),
                          strDEMO_SCRIPT );

            // If it's not a new game, load the saved game
            if( m_LoadSave.IsGameLoaded() )
            {
                m_Game.LoadSaveGame( m_LoadSave.GetGameDataPtr(),
                                     m_LoadSave.GetGameDataSize() );

                // We don't need the save game information anymore
                m_LoadSave.FreeGameData();
            }
            break;
        }
        case GAME_MODE_MENU:    m_Menu.Start( Menu::Normal );   break;
        case GAME_MODE_DEMO:    m_Demo.Start( m_Menu.GetMusicVolume(), 
                                              m_Menu.GetEffectsVolume()); break;
        case GAME_MODE_SPLASH:  m_Splash.Start();               break;
        case GAME_MODE_START:   m_StartScreen.Start();          break;

        case GAME_MODE_LOAD:

            m_LoadSave.Start( LoadSave::MODE_LOAD );
            break;

        case GAME_MODE_SAVE:

            m_LoadSave.Start( LoadSave::MODE_SAVE );
            break;

        default: assert( FALSE ); break;
    }

    // Change modes
    m_LastMode = m_GameMode;
    m_GameMode = iNewMode;
}




//-----------------------------------------------------------------------------
// Name: MemUnitWasInserted()
// Desc: TRUE if any memory unit has been inserted since the last time we checked
//-----------------------------------------------------------------------------
BOOL TechCertGame::MemUnitWasInserted() // static
{
    DWORD dwInsertions;
    DWORD dwRemovals;
    CXBMemUnit::GetMemUnitChanges( dwInsertions, dwRemovals );
    if( dwInsertions )
        return TRUE;
    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: GetSaveGameName()
// Desc: Generates a default save game name
//-----------------------------------------------------------------------------
VOID TechCertGame::GetSaveGameName( WCHAR* strGameName ) // static
{
    assert( strGameName != NULL );

    // TCR 1-13 Display Time
    FILETIME ftZulu;
    GetSystemTimeAsFileTime( &ftZulu );

    // Convert to local time
    FILETIME ftTimeLocal;
    FileTimeToLocalFileTime( &ftZulu, &ftTimeLocal );

    // Extract date/time data
    SYSTEMTIME SystemTime;
    FileTimeToSystemTime( &ftTimeLocal, &SystemTime );

    // Determine day of week
    WCHAR strDayOfWeek[32];

    DWORD dwLang = CXBConfig::GetLanguage();

    switch( SystemTime.wDayOfWeek )
    {
        case 0: lstrcpyW( strDayOfWeek, strSUNDAY[ dwLang ] );    break;
        case 1: lstrcpyW( strDayOfWeek, strMONDAY[ dwLang ] );    break;
        case 2: lstrcpyW( strDayOfWeek, strTUESDAY[ dwLang ] );   break;
        case 3: lstrcpyW( strDayOfWeek, strWEDNESDAY[ dwLang ] ); break;
        case 4: lstrcpyW( strDayOfWeek, strTHURSDAY[ dwLang ] );  break;
        case 5: lstrcpyW( strDayOfWeek, strFRIDAY[ dwLang ] );    break;
        case 6: lstrcpyW( strDayOfWeek, strSATURDAY[ dwLang ] );  break;
        default: assert( FALSE ); break;
    }

    WCHAR strDate[32];
    WCHAR strTime[32];
    CXBConfig::FormatDateTime( ftZulu, strDate, strTime );

    // TCR 2-5 Saved Game Name
    // TCR 2-6 Saved Game Default Name
    wsprintfW( strGameName, strGAME_NAME_FORMAT[ dwLang ], 
               strDayOfWeek, strTime );
}




//-----------------------------------------------------------------------------
// Name: Confirm()
// Desc: Make sure player really wants to leave
//-----------------------------------------------------------------------------
BOOL TechCertGame::ConfirmQuit()
{
    // TCR 5-53 Player Confirmation of Destructive Actions
    m_Game.Render( m_strFrameRate );

    DWORD dwLang = CXBConfig::GetLanguage();

    // Show verification text
    if( dwLang != XC_LANGUAGE_JAPANESE )
        m_Font.DrawText( 320, 240, 0xFFFFFFFF, strCONFIRM_QUIT[ dwLang ],
                         XBFONT_CENTER_X | XBFONT_CENTER_Y );
    else
        m_XFontJPN.DrawText( 320, 240, 0xFFFFFFFF, strCONFIRM_QUIT[ dwLang ],
                             CXFONT_CENTER_X | CXFONT_CENTER_Y );

    // Show the screen
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    for( ;; )
    {
        XBInput_GetInput();
        const XBGAMEPAD* pGamePad = Controller::GetPrimaryController();
        if( pGamePad != NULL )
        {
            if( pGamePad->bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] ||
                pGamePad->wPressedButtons & XINPUT_GAMEPAD_START )
            {
                return TRUE;
            }

            if( pGamePad->bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] ||
                pGamePad->wPressedButtons & XINPUT_GAMEPAD_BACK )
            {
                return FALSE;
            }
        }
    }
}
