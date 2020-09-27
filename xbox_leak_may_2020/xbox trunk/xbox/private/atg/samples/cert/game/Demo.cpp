//-----------------------------------------------------------------------------
// File: Demo.cpp
//
// Desc: Demo mode
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Demo.h"
#include "Controller.h"
#include "Text.h"
#include <XbConfig.h>
#include <XbFont.h>
#include <XbApp.h>




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const CHAR* const strDEMO_SCRIPT = "D:\\Media\\Demo.Script";




//-----------------------------------------------------------------------------
// Name: Demo()
// Desc: Constructor
//-----------------------------------------------------------------------------
Demo::Demo( CXBFont& font, CXFont& xFont, CMusicManager& musicmgr, CSoundEffect& sndeffect )
:
    m_Font( font ),
    m_XFontJPN( xFont ), 
    m_Timer(),
    m_GameDemo( font, xFont, musicmgr, sndeffect )
{
}




//-----------------------------------------------------------------------------
// Name: Start()
// Desc: Start the game "attract mode"
//-----------------------------------------------------------------------------
VOID Demo::Start( FLOAT fMusicVolume, FLOAT fEffectVolume )
{
    m_Timer.StartZero();

    // In demo mode, any controller can become the primary controller
    Controller::ClearPrimaryController();

    // In order to detect if new MUs have been inserted during demo mode,
    // we initialize the MU device status (and throw away the result)
    XGetDevices( XDEVICE_TYPE_MEMORY_UNIT );

    BOOL bRecordDemo = FALSE;
    BOOL bPlayDemo = TRUE;
    BOOL bVibration = FALSE;
    m_GameDemo.Start( bRecordDemo, bPlayDemo, bVibration, fMusicVolume, 
                      fEffectVolume, strDEMO_SCRIPT );
}




//-----------------------------------------------------------------------------
// Name: End()
// Desc: End "attract mode"
//-----------------------------------------------------------------------------
VOID Demo::End()
{
    m_Timer.Stop();
    m_GameDemo.End();
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame for animating the demo
//-----------------------------------------------------------------------------
HRESULT Demo::FrameMove( const XBGAMEPAD* pGamePad, FLOAT fTime, FLOAT fElapsedTime )
{
    m_GameDemo.FrameMove( pGamePad, fTime, fElapsedTime );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame for rendering of the demo
//-----------------------------------------------------------------------------
HRESULT Demo::Render()
{
    m_GameDemo.Render();

    DWORD dwLang = CXBConfig::GetLanguage();

    if( dwLang != XC_LANGUAGE_JAPANESE )
    {
        m_Font.DrawText( 500, 50, 0xffffff00, strDEMO[ dwLang ] );
        m_Font.DrawText( 320, 400, 0xffffff00, strRETURN_TO_MENU[ dwLang ],
                         XBFONT_CENTER_X );
    }
    else
    {
        m_XFontJPN.DrawText( 500, 50, 0xffffff00, strDEMO[ dwLang ] );
        m_XFontJPN.DrawText( 320, 400, 0xffffff00, strRETURN_TO_MENU[ dwLang ],
                             CXFONT_CENTER_X );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: IsComplete()
// Desc: TRUE if demo mode has finished its cycle
//-----------------------------------------------------------------------------
BOOL Demo::IsComplete() const
{
    // TCR 3-37 Attract Mode
    return( m_Timer.GetElapsedSeconds() > 60.0f );
}
