//-----------------------------------------------------------------------------
// File: MenuOptions.cpp
//
// Desc: Options menu
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "MenuOptions.h"
#include "Controller.h"
#include "File.h"
#include "Text.h"
#include <XbApp.h>
#include <XbConfig.h>
#include <XbFont.h>
#include "cxfont.h"



//-----------------------------------------------------------------------------
// Local structs
//-----------------------------------------------------------------------------

// Format of the options file
struct Options
{
    BOOL  bIsVibrationOn;
    FLOAT fMusicVolume;
    FLOAT fEffectVolume;
};




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const CHAR* const strOPTIONS_FILE = "T:\\GameOptions.opt";

// Can't change menu items w/ joystick any faster than this (seconds)
const FLOAT JOY_MIN_MENU_MOVE = 0.2f;

// Joystick must be at least this far away from the center position to register
// ( 0.0f - 1.0f scale )
const FLOAT JOY_THRESHOLD = 0.35f;

// Adjust volume by this amount per frame
const FLOAT fVOLUME_ADJUST = 1.0f;

// Max volume level (0.0f is minimum)
const FLOAT fVOLUME_MAX = 100.0f;

// Menu items
enum
{
    VIBRATION_INDEX,
    MUSIC_VOLUME_INDEX,
    EFFECT_VOLUME_INDEX,
    SAVE_INDEX,

    MENU_ITEM_MAX
};




//-----------------------------------------------------------------------------
// Name: MenuOptions()
// Desc: Constructor
//-----------------------------------------------------------------------------
MenuOptions::MenuOptions( CXBFont& font, CXFont &xFont )
:
    m_Font          ( font ),
    m_XFontJPN      ( xFont ),
    m_ptMenuSel     ( NULL ),
    m_JoyTimer      (),
    m_iCurrIndex    ( VIBRATION_INDEX ),
    m_bIsVibrationOn( TRUE ),
    m_fMusicVolume  ( fVOLUME_MAX ),
    m_fEffectVolume ( fVOLUME_MAX ),
    m_bExitMenu     ( FALSE )
{
    // Open the options file (if it exists)
    File OptionsFile;
    if( !OptionsFile.Open( strOPTIONS_FILE, GENERIC_READ, 0 ) )
        return;

    // Read option information from disk
    Options options;
    DWORD dwRead;
    if( !OptionsFile.Read( &options, sizeof( options ), dwRead ) )
        return;

    // Store options
    m_bIsVibrationOn = options.bIsVibrationOn;
    m_fMusicVolume   = options.fMusicVolume;
    m_fEffectVolume  = options.fEffectVolume;
}




//-----------------------------------------------------------------------------
// Name: Start()
// Desc: Called when option menu is initially displayed
//-----------------------------------------------------------------------------
VOID MenuOptions::Start( LPDIRECT3DTEXTURE8 ptMenuSel )
{
    m_ptMenuSel = ptMenuSel;
    m_iCurrIndex = VIBRATION_INDEX;
    m_bExitMenu = FALSE;
}




//-----------------------------------------------------------------------------
// Name: End()
// Desc: Called when option menu is removed from screen
//-----------------------------------------------------------------------------
VOID MenuOptions::End()
{
    m_ptMenuSel = NULL;
    m_JoyTimer.Stop();

    // If we're exiting because of inactivity (e.g. attract mode is starting)
    // restore the old settings
    if( !m_bExitMenu )
    {
        // Open the options file
        File OptionsFile;
        if( OptionsFile.Open( strOPTIONS_FILE, GENERIC_READ, 0 ) )
        {
            // Read option information from disk
            Options options;
            DWORD dwRead;
            if( OptionsFile.Read( &options, sizeof( options ), dwRead ) )
            {
                // Store options locally
                m_bIsVibrationOn = options.bIsVibrationOn;
                m_fMusicVolume   = options.fMusicVolume;
                m_fEffectVolume  = options.fEffectVolume;
            }
        }
    }
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame for animating the menu
//-----------------------------------------------------------------------------
HRESULT MenuOptions::FrameMove( const XBGAMEPAD* pGamePad )
{
    if( pGamePad == NULL )
        return S_OK;

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
                return S_OK;
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
            m_iCurrIndex = MENU_ITEM_MAX - 1;
    }
    else if( bMenuDown )
    {
        ++m_iCurrIndex;
        if( m_iCurrIndex == MENU_ITEM_MAX )
            m_iCurrIndex = 0;
    }

    // "A" button
    // TCR 3-16 Menu Buttons
    if( pGamePad->bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] ||
        pGamePad->wPressedButtons & XINPUT_GAMEPAD_START )
    {
        if( m_iCurrIndex == VIBRATION_INDEX )
        {
            // TCR 3-56 Vibration Option
            // Toggle vibration state
            m_bIsVibrationOn = !m_bIsVibrationOn;
        }
        else if( m_iCurrIndex == SAVE_INDEX )
        {
            // Save option information to disk
            File OptionsFile;
            if( OptionsFile.Create( strOPTIONS_FILE, GENERIC_WRITE, 0 ) )
            {
                Options options;
                options.bIsVibrationOn = m_bIsVibrationOn;
                options.fMusicVolume   = m_fMusicVolume;
                options.fEffectVolume  = m_fEffectVolume;
                OptionsFile.Write( &options, sizeof( options ) );
            }
            m_bExitMenu = TRUE;
        }
    }

    // Handle volume control
    FLOAT fAdjust = 0.0f;
    if( pGamePad->fX2 > JOY_THRESHOLD || 
        ( pGamePad->wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) )
    {
        fAdjust = fVOLUME_ADJUST;
    }
    else if( pGamePad->fX2 < -JOY_THRESHOLD || 
          ( pGamePad->wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT ) )
    {
        fAdjust = -fVOLUME_ADJUST;
    }

    if( fAdjust != 0.0f )
    {
        switch( m_iCurrIndex )
        {
            // TCR 3-52 Game Soundtrack Volume Control
            case MUSIC_VOLUME_INDEX:
                m_fMusicVolume += fAdjust;
                break;
            // TCR 3-60 Game Sound Volume Control
            case EFFECT_VOLUME_INDEX:
                m_fEffectVolume += fAdjust;
                break;
        }

        // Clamp
        if( m_fMusicVolume > fVOLUME_MAX )
            m_fMusicVolume = fVOLUME_MAX;
        if( m_fEffectVolume > fVOLUME_MAX )
            m_fEffectVolume = fVOLUME_MAX;

        if( m_fMusicVolume < 0.0f )
            m_fMusicVolume = 0.0f;
        if( m_fEffectVolume < 0.0f )
            m_fEffectVolume = 0.0f;
    }

    // "B" button cancels
    if( pGamePad->bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] ||
        pGamePad->wPressedButtons & XINPUT_GAMEPAD_BACK )
    {
        // Restore default settings
        m_bIsVibrationOn = TRUE;
        m_fMusicVolume = m_fEffectVolume = fVOLUME_MAX;

        // Open the options file
        File OptionsFile;
        if( OptionsFile.Open( strOPTIONS_FILE, GENERIC_READ, 0 ) )
        {
            // Read option information from disk
            Options options;
            DWORD dwRead;
            if( OptionsFile.Read( &options, sizeof( options ), dwRead ) )
            {
                // Store options locally
                m_bIsVibrationOn = options.bIsVibrationOn;
                m_fMusicVolume   = options.fMusicVolume;
                m_fEffectVolume  = options.fEffectVolume;
            }
        }
        m_bExitMenu = TRUE;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame for 3d rendering of the menu
//-----------------------------------------------------------------------------
HRESULT MenuOptions::Render()
{
    DWORD dwLang = CXBConfig::GetLanguage();

    // Game name
    if( dwLang != XC_LANGUAGE_JAPANESE )
        m_Font.DrawText( 320, 100, 0xFFFFFFFF, strGAME_NAME[ dwLang ],
                         XBFONT_CENTER_X );
    else
        m_XFontJPN.DrawText( 320, 100, 0xFFFFFFFF, strGAME_NAME[ dwLang ],
                             CXFONT_CENTER_X );

    // Game options
    if( dwLang != XC_LANGUAGE_JAPANESE )
        m_Font.DrawText( 320, 150, 0xFFFFFFFF, strMENU_OPTIONS[ dwLang ],
                         XBFONT_CENTER_X );
    else
        m_XFontJPN.DrawText( 320, 150, 0xFFFFFFFF, strMENU_OPTIONS[ dwLang ],
                             CXFONT_CENTER_X );

    const WCHAR* strMenu[ MENU_ITEM_MAX ] =
    {
        strMENU_VIBRATION[ dwLang ],
        strMENU_MUSIC_VOLUME[ dwLang ],
        strMENU_EFFECT_VOLUME[ dwLang ],
        strMENU_SAVE_OPTIONS[ dwLang ]
    };

    const DWORD dwHighlight = 0xFFFFFF00; // Yellow
    const DWORD dwNormal    = 0xFFFFFFFF;

    FLOAT fYtop = 200.0f;
    FLOAT fYdelta = 50.0f;

    // Show menu
    for( INT i = 0; i < MENU_ITEM_MAX; ++i )
    {
        DWORD dwColor = ( m_iCurrIndex == i ) ? dwHighlight : dwNormal;

        if( dwLang != XC_LANGUAGE_JAPANESE )
            m_Font.DrawText( 150, fYtop + (fYdelta * i), dwColor, strMenu[i] );
        else
            m_XFontJPN.DrawText( 150, (LONG)fYtop + (LONG)(fYdelta * i), dwColor, strMenu[i] );

        switch( i )
        {
            case VIBRATION_INDEX:
                if( dwLang != XC_LANGUAGE_JAPANESE)
                    m_Font.DrawText( 360, fYtop + (fYdelta * i), dwColor, 
                                     m_bIsVibrationOn ? strON[ dwLang ] : 
                                                        strOFF[ dwLang ] );
                else
                    m_XFontJPN.DrawText( 360, (LONG)fYtop + (LONG)(fYdelta * i), dwColor, 
                                         m_bIsVibrationOn ? strON[ dwLang ] : 
                                                            strOFF[ dwLang ] );
                break;
            case MUSIC_VOLUME_INDEX:
            {
                struct BACKGROUNDVERTEX
                { 
                    D3DXVECTOR4 p;
                    D3DCOLOR color;
                };
                BACKGROUNDVERTEX v[4];
                FLOAT x1 = 360.0f;
                FLOAT x2 = x1 + (150.0f * m_fMusicVolume) / 100.0f;
                FLOAT y1 = fYtop + (fYdelta * i);
                FLOAT y2 = y1 + 20.0f;
                v[0].p = D3DXVECTOR4( x1 - 0.5f, y1 - 0.5f, 1.0f, 1.0f );  v[0].color = 0xffffffff;
                v[1].p = D3DXVECTOR4( x2 - 0.5f, y1 - 0.5f, 1.0f, 1.0f );  v[1].color = 0xffffffff;
                v[2].p = D3DXVECTOR4( x1 - 0.5f, y2 - 0.5f, 1.0f, 1.0f );  v[2].color = 0xff00ff00;
                v[3].p = D3DXVECTOR4( x2 - 0.5f, y2 - 0.5f, 1.0f, 1.0f );  v[3].color = 0xff00ff00;

                g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, 
                                                    D3DTOP_DISABLE );
                g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
                g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, 
                                               sizeof(v[0]) );

                WCHAR strPercent[32];
                wsprintfW( strPercent, L"%d %%", INT(m_fMusicVolume) );
                m_Font.DrawText( 520, fYtop + (fYdelta * i), dwColor, strPercent );
                break;
            }

            case EFFECT_VOLUME_INDEX:
            {
                struct BACKGROUNDVERTEX
                { 
                    D3DXVECTOR4 p;
                    D3DCOLOR color;
                };
                BACKGROUNDVERTEX v[4];
                FLOAT x1 = 360.0f;
                FLOAT x2 = x1 + (150.0f * m_fEffectVolume) / 100.0f;
                FLOAT y1 = fYtop + (fYdelta * i);
                FLOAT y2 = y1 + 20.0f;
                v[0].p = D3DXVECTOR4( x1 - 0.5f, y1 - 0.5f, 1.0f, 1.0f );  v[0].color = 0xffffffff;
                v[1].p = D3DXVECTOR4( x2 - 0.5f, y1 - 0.5f, 1.0f, 1.0f );  v[1].color = 0xffffffff;
                v[2].p = D3DXVECTOR4( x1 - 0.5f, y2 - 0.5f, 1.0f, 1.0f );  v[2].color = 0xff00ff00;
                v[3].p = D3DXVECTOR4( x2 - 0.5f, y2 - 0.5f, 1.0f, 1.0f );  v[3].color = 0xff00ff00;

                g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, 
                                                    D3DTOP_DISABLE );
                g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
                g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, 
                                               sizeof(v[0]) );

                WCHAR strPercent[32];
                wsprintfW( strPercent, L"%d %%", INT(m_fEffectVolume) );
                m_Font.DrawText( 520, fYtop + (fYdelta * i), dwColor, strPercent );
                break;
            }
        }
    }

    // Show selected item with little triangle

    FLOAT fTop = fYtop + (fYdelta * m_iCurrIndex );
    //              Left,   Top,  Right,  Bottom
    D3DXVECTOR4 rc( 110.0f, fTop,  130.0f,  fTop + 20.0f );

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

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ExitMenu()
// Desc: Returns TRUE if time to leave the options menu
//-----------------------------------------------------------------------------
BOOL MenuOptions::ExitMenu() const
{
    return m_bExitMenu;
}




//-----------------------------------------------------------------------------
// Name: IsVibrationOn()
// Desc: Returns status of vibration
//-----------------------------------------------------------------------------
BOOL MenuOptions::IsVibrationOn() const
{
    return m_bIsVibrationOn;
}




//-----------------------------------------------------------------------------
// Name: GetMusicVolume()
// Desc: Returns status of music volume
//-----------------------------------------------------------------------------
FLOAT MenuOptions::GetMusicVolume() const
{
    return m_fMusicVolume;
}




//-----------------------------------------------------------------------------
// Name: GetEffectsVolume()
// Desc: Returns status of effects volume
//-----------------------------------------------------------------------------
FLOAT MenuOptions::GetEffectsVolume() const
{
    return m_fEffectVolume;
}
