//-----------------------------------------------------------------------------
// File: StartScreen.cpp
//
// Desc: Start screen
//
// Hist: 05.07.01 - New for June XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "StartScreen.h"
#include "Controller.h"
#include "Text.h"
#include <XbApp.h>
#include <XbConfig.h>
#include <XbFont.h>
#include "cxfont.h"




//-----------------------------------------------------------------------------
// Name: StartScreen()
// Desc: Constructor
//-----------------------------------------------------------------------------
StartScreen::StartScreen( CXBFont& font, CXFont& xFont )
:
    m_Font( font ),
    m_XFontJPN( xFont ),
    m_Timer()
{
}




//-----------------------------------------------------------------------------
// Name: Start()
// Desc: Start the start screen
//-----------------------------------------------------------------------------
VOID StartScreen::Start()
{
    m_Timer.StartZero();
}




//-----------------------------------------------------------------------------
// Name: End()
// Desc: Quit the start screen
//-----------------------------------------------------------------------------
VOID StartScreen::End()
{
    m_Timer.Stop();
}




//-----------------------------------------------------------------------------
// Name: GetElapsedSeconds()
// Desc: Returns the length of time elapsed since Start() called
//-----------------------------------------------------------------------------
FLOAT StartScreen::GetElapsedSeconds() const
{
    return m_Timer.GetElapsedSeconds();
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame for animating the start screen
//-----------------------------------------------------------------------------
HRESULT StartScreen::FrameMove( const XBGAMEPAD* )
{
    // Allow any controller to become the primary controller.
    // We do this on every loop because we're waiting for the Start button
    // to indicate the primary controller
    Controller::ClearPrimaryController();
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame for 3d rendering of the start screen
//-----------------------------------------------------------------------------
HRESULT StartScreen::Render()
{
    DWORD dwLang = CXBConfig::GetLanguage();

    if( dwLang != XC_LANGUAGE_JAPANESE )
    {
        m_Font.DrawText( 320, 100, 0xffffffff, strGAME_NAME[ dwLang ],
                         XBFONT_CENTER_X );
        m_Font.DrawText( 320, 200, 0xffffffff, strINTRO[ dwLang ],
                         XBFONT_CENTER_X | XBFONT_CENTER_Y );

        // TCR 6-6 Press Start
        m_Font.DrawText( 320, 300, 0xffffffff, strPRESS_START[ dwLang ],
                         XBFONT_CENTER_X );
    }
    else
    {
        m_XFontJPN.DrawText( 320, 100, 0xffffffff, strGAME_NAME[ dwLang ],
                             CXFONT_CENTER_X );
        m_XFontJPN.DrawText( 320, 200, 0xffffffff, strINTRO[ dwLang ],
                             CXFONT_CENTER_X | CXFONT_CENTER_Y );

        // TCR 6-6 Press Start
        m_XFontJPN.DrawText( 320, 300, 0xffffffff, strPRESS_START[ dwLang ],
                             CXFONT_CENTER_X );
    }

    return S_OK;
}
