//-----------------------------------------------------------------------------
// File: Menu.cpp
//
// Desc: Main menu and ingame menu
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Menu.h"
#include <XbApp.h>
#include <XbConfig.h>
#include <XbFont.h>
#include "Controller.h"
#include "App.h"
#include "Text.h"




// The following header file is generated from "MenuResource.rdf" file
// using the Bundler tool. In addition to the header, the tool outputs a binary
// file (MenuResource.xpr) which contains compiled (i.e. bundled) resources
// and is loaded at runtime using the CXBPackedResource class.
#include "MenuResource.h"




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

// Can't change menu items w/ joystick any faster than this (seconds)
const FLOAT JOY_MIN_MENU_MOVE = 0.2f;

// Joystick must be at least this far away from the center position to register
// ( 0.0f - 1.0f scale )
const FLOAT JOY_THRESHOLD = 0.35f;




//-----------------------------------------------------------------------------
// Name: Menu()
// Desc: Constructor
//-----------------------------------------------------------------------------
Menu::Menu( CXBFont& font, CXFont& xFont )
:
    m_xprResource  (),
    m_ptMenuSel    ( NULL ),
    m_Font         ( font ),
    m_XFontJPN     ( xFont ),
    m_InactiveTimer(),
    m_JoyTimer     (),
    m_Options      ( font, xFont ),
    m_MenuMode     ( MENU_MODE_MAIN ),
    m_MenuType     ( Normal ),
    m_arrMenu      (),
    m_strMenu      (),
    m_iCurrIndex   ( 0 ),
    m_iMaxItems    ( 0 )
{
}




//-----------------------------------------------------------------------------
// Name: Start()
// Desc: Called when menu is initially displayed
//-----------------------------------------------------------------------------
VOID Menu::Start( MenuType menuType )
{
    m_MenuMode = MENU_MODE_MAIN;
    m_MenuType = menuType;
    m_iCurrIndex = 0;
    DWORD dwLang = CXBConfig::GetLanguage();

    switch( m_MenuType )
    {
        case Normal:
            m_arrMenu[ 0 ] = MENU_ITEM_START;
            m_arrMenu[ 1 ] = MENU_ITEM_LOAD_GAME;
            m_arrMenu[ 2 ] = MENU_ITEM_OPTIONS;
            m_strMenu[ 0 ] = strMENU_START[ dwLang ];
            m_strMenu[ 1 ] = strMENU_LOAD[ dwLang ];
            m_strMenu[ 2 ] = strMENU_OPTIONS[ dwLang ];
            m_iMaxItems = 3;
            break;
        case InGame:
            m_arrMenu[ 0 ] = MENU_ITEM_RESUME;
            m_arrMenu[ 1 ] = MENU_ITEM_SAVE_GAME;
            m_arrMenu[ 2 ] = MENU_ITEM_LOAD_GAME;
            m_arrMenu[ 3 ] = MENU_ITEM_QUIT;
            m_strMenu[ 0 ] = strMENU_RESUME[ dwLang ];
            m_strMenu[ 1 ] = strMENU_SAVE[ dwLang ];
            m_strMenu[ 2 ] = strMENU_LOAD[ dwLang ];
            m_strMenu[ 3 ] = strMENU_QUIT[ dwLang ];
            m_iMaxItems = 4;
            break;
        default:
            assert( FALSE );
            break;
    }

    m_InactiveTimer.StartZero();

    // Load our textures
    if( FAILED( m_xprResource.Create( g_pd3dDevice, "MenuResource.xpr", 
                                      MenuResource_NUM_RESOURCES ) ) )
    {
        OUTPUT_DEBUG_STRING( "Menu::Start: failed to load textures\n");
        return;
    }

    // Load our textures from the bundled resource
    m_ptMenuSel = m_xprResource.GetTexture( MenuResource_MenuSelect_OFFSET );
}




//-----------------------------------------------------------------------------
// Name: End()
// Desc: Called when menu is no longer displayed
//-----------------------------------------------------------------------------
VOID Menu::End()
{
    m_xprResource.Destroy();
    m_InactiveTimer.Stop();
    m_JoyTimer.Stop();
    m_Options.End();
}




//-----------------------------------------------------------------------------
// Name: GetCurrItem()
// Desc: Returns the currently selected menu item
//-----------------------------------------------------------------------------
Menu::MenuItem Menu::GetCurrItem() const
{
    return m_arrMenu[ m_iCurrIndex ];
}




//-----------------------------------------------------------------------------
// Name: GetInactiveSeconds()
// Desc: Returns the length of time that the menu has been inactive (no button
//       presses, etc.)
//-----------------------------------------------------------------------------
FLOAT Menu::GetInactiveSeconds() const
{
    return m_InactiveTimer.GetElapsedSeconds();
}




//-----------------------------------------------------------------------------
// Name: ChangeMode()
// Desc: Switch to new menu mode
//-----------------------------------------------------------------------------
VOID Menu::ChangeMode( MenuMode iNewMode )
{
    // End the current mode
    switch( m_MenuMode )
    {
        case MENU_MODE_MAIN:                        break;
        case MENU_MODE_OPTIONS: m_Options.End();    break;
        default:                assert( FALSE );    break;
    }

    // Start the new mode
    switch( iNewMode )
    {
        case MENU_MODE_MAIN:                                     break;
        case MENU_MODE_OPTIONS: m_Options.Start( m_ptMenuSel );  break;
        default:                assert( FALSE );                 break;
    }

    m_MenuMode = iNewMode;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame for animating the menu
//-----------------------------------------------------------------------------
HRESULT Menu::FrameMove( const XBGAMEPAD* pGamePad )
{
    if( pGamePad == NULL )
        return S_OK;

    // If any button is active, then reset the inactive timer
    if( Controller::IsAnyButtonActive( pGamePad ) )
        m_InactiveTimer.StartZero();

    switch( m_MenuMode )
    {
        case MENU_MODE_MAIN:
            
            FrameMoveMainMenu( pGamePad );
            break;

        case MENU_MODE_OPTIONS:

            m_Options.FrameMove( pGamePad );
            if( m_Options.ExitMenu() )
                ChangeMode( MENU_MODE_MAIN );
            break;

        default:

            assert( FALSE );
            break;

    }
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame for 3d rendering of the menu
//-----------------------------------------------------------------------------
HRESULT Menu::Render()
{
    switch( m_MenuMode )
    {
        case MENU_MODE_MAIN:        RenderMainMenu();       break;
        case MENU_MODE_OPTIONS:     m_Options.Render();     break;
        default:                    assert( FALSE );        break;
    }
    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FrameMoveMainMenu()
// Desc: Called once per frame for animating the main menu
//-----------------------------------------------------------------------------
VOID Menu::FrameMoveMainMenu( const XBGAMEPAD* pGamePad )
{
    if( pGamePad == NULL )
        return;

    // Detect menu change
    BOOL bMenuUp( FALSE );
    BOOL bMenuDown( FALSE );

    // Is the joystick active
    if( pGamePad->fY1 > JOY_THRESHOLD ||
        pGamePad->fY1 < -JOY_THRESHOLD )
    {
        // If we've previously registered a joystick menu move,
        // ignore the joystick until JOY_MIN_MENU_MOVE seconds
        // has elapsed
        if( m_JoyTimer.IsRunning() )
        {
            if( m_JoyTimer.GetElapsedSeconds() < JOY_MIN_MENU_MOVE )
                return;
            else
                m_JoyTimer.StartZero();
        }
        else
        {
            m_JoyTimer.StartZero();
        }

        if( pGamePad->fY1 > JOY_THRESHOLD )
            bMenuUp = TRUE;
        else
            bMenuDown = TRUE;
    }
    else
    {
        m_JoyTimer.Stop();
    }

    // Gamepad also moves menu cursor
    // TCR 3-17 Menu Navigation
    bMenuUp   = bMenuUp   || pGamePad->wPressedButtons & XINPUT_GAMEPAD_DPAD_UP;
    bMenuDown = bMenuDown || pGamePad->wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN;

    if( bMenuUp )
    {
        --m_iCurrIndex;
        if( m_iCurrIndex < 0 )
            m_iCurrIndex = m_iMaxItems - 1;
    }
    else if( bMenuDown )
    {
        ++m_iCurrIndex;
        if( m_iCurrIndex == m_iMaxItems )
            m_iCurrIndex = 0;
    }

    // "A" button
    // TCR 3-16 Menu Buttons
    if( pGamePad->bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] ||
        pGamePad->wPressedButtons & XINPUT_GAMEPAD_START )
    {
        switch( m_arrMenu[ m_iCurrIndex ] )
        {
            case MENU_ITEM_OPTIONS:
                ChangeMode( MENU_MODE_OPTIONS );
                break;
            default:
                break;
        }
    }
}




//-----------------------------------------------------------------------------
// Name: RenderMainMenu()
// Desc: Called once per frame for 3d rendering of the menu
//-----------------------------------------------------------------------------
VOID Menu::RenderMainMenu()
{
    DWORD dwLang = CXBConfig::GetLanguage();

    if( m_MenuType == Normal )
    {
        if( dwLang != XC_LANGUAGE_JAPANESE )
            m_Font.DrawText( 320, 100, 0xFFFFFFFF, strGAME_NAME[ dwLang ],
                             XBFONT_CENTER_X );
        else
            m_XFontJPN.DrawText( 320, 100, 0xFFFFFFFF, strGAME_NAME[ dwLang ],
                             CXFONT_CENTER_X );
    }

    const DWORD dwHighlight = 0xffffff00; // Yellow
    const DWORD dwNormal    = 0xffffffff;

    FLOAT fYtop = 150.0f;
    FLOAT fYdelta = 50.0f;

    // Show menu
    for( INT i = 0; i < m_iMaxItems; ++i )
    {
        DWORD dwColor = ( m_iCurrIndex == i ) ? dwHighlight : dwNormal;

        if( dwLang != XC_LANGUAGE_JAPANESE )
            m_Font.DrawText( 260, fYtop + (fYdelta * i), dwColor, m_strMenu[i] );
        else
            m_XFontJPN.DrawText( 260, (LONG)fYtop + (LONG)(fYdelta * i), dwColor, m_strMenu[i] );
    }

    // 3-24 Selection User Interface
    // Show selected item with little triangle

    FLOAT fTop = fYtop + (fYdelta * m_iCurrIndex );
    //              Left,   Top,  Right,  Bottom
    D3DXVECTOR4 rc( 220.0f, fTop, 240.0f, fTop + 20.0f );

    // Show selected item
    struct TILEVERTEX
    {
        D3DXVECTOR4 p;
        D3DXVECTOR2 t;
    };
    TILEVERTEX* pVertices;

    LPDIRECT3DVERTEXBUFFER8 pvbTemp;
    g_pd3dDevice->CreateVertexBuffer( 4 * sizeof( TILEVERTEX ), 
                                      D3DUSAGE_WRITEONLY, 
                                      D3DFVF_XYZRHW | D3DFVF_TEX1, 
                                      D3DPOOL_MANAGED, &pvbTemp );

    // Create a quad for us to render our texture on
    pvbTemp->Lock( 0, 0, (BYTE **)&pVertices, 0L );
    pVertices[0].p = D3DXVECTOR4( rc.x - 0.5f, rc.w - 0.5f, 1.0f, 1.0f );  pVertices[0].t = D3DXVECTOR2( 0.0f, 1.0f ); // Bottom Left
    pVertices[1].p = D3DXVECTOR4( rc.x - 0.5f, rc.y - 0.5f, 1.0f, 1.0f );  pVertices[1].t = D3DXVECTOR2( 0.0f, 0.0f ); // Top    Left
    pVertices[2].p = D3DXVECTOR4( rc.z - 0.5f, rc.w - 0.5f, 1.0f, 1.0f );  pVertices[2].t = D3DXVECTOR2( 1.0f, 1.0f ); // Bottom Right
    pVertices[3].p = D3DXVECTOR4( rc.z - 0.5f, rc.y - 0.5f, 1.0f, 1.0f );  pVertices[3].t = D3DXVECTOR2( 1.0f, 0.0f ); // Top    Right
    pvbTemp->Unlock();

    // Set up our state
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetStreamSource( 0, pvbTemp, sizeof( TILEVERTEX ) );

    // Render the quad with our texture
    g_pd3dDevice->SetTexture( 0, m_ptMenuSel );
    g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    g_pd3dDevice->SetTexture( 0, NULL );
    pvbTemp->Release();

}




//-----------------------------------------------------------------------------
// Name: IsVibrationOn()
// Desc: Returns status of vibration
//-----------------------------------------------------------------------------
BOOL Menu::IsVibrationOn() const
{
    return m_Options.IsVibrationOn();
}




//-----------------------------------------------------------------------------
// Name: GetMusicVolume()
// Desc: Returns status of music volume
//-----------------------------------------------------------------------------
FLOAT Menu::GetMusicVolume() const
{
    return m_Options.GetMusicVolume();
}




//-----------------------------------------------------------------------------
// Name: GetEffectsVolume()
// Desc: Returns status of effects volume
//-----------------------------------------------------------------------------
FLOAT Menu::GetEffectsVolume() const
{
    return m_Options.GetEffectsVolume();
}
