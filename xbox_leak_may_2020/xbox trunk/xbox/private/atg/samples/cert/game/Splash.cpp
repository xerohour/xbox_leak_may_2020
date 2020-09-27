//-----------------------------------------------------------------------------
// File: Splash.cpp
//
// Desc: Splash screen
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Splash.h"
#include "Text.h"
#include <XbApp.h>
#include <XbConfig.h>
#include <XbFont.h>
#include "cxfont.h"




//-----------------------------------------------------------------------------
// Name: Splash()
// Desc: Constructor
//-----------------------------------------------------------------------------
Splash::Splash( CXBFont& font, CXFont& xFont )
:
    m_Font( font ),
    m_XFontJPN( xFont ),
    m_Timer()
{
}




//-----------------------------------------------------------------------------
// Name: Start()
// Desc: Start the splash
//-----------------------------------------------------------------------------
VOID Splash::Start()
{
    m_Timer.StartZero();
}




//-----------------------------------------------------------------------------
// Name: End()
// Desc: Quit the splash
//-----------------------------------------------------------------------------
VOID Splash::End()
{
    m_Timer.Stop();
}




//-----------------------------------------------------------------------------
// Name: GetElapsedSeconds()
// Desc: Returns the length of time elapsed since Start() called
//-----------------------------------------------------------------------------
FLOAT Splash::GetElapsedSeconds() const
{
    return m_Timer.GetElapsedSeconds();
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame for animating the splash
//-----------------------------------------------------------------------------
HRESULT Splash::FrameMove( const XBGAMEPAD* )
{
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame for 3d rendering of the splash
//-----------------------------------------------------------------------------
HRESULT Splash::Render()
{
    DWORD dwLang = CXBConfig::GetLanguage();

    if( dwLang != XC_LANGUAGE_JAPANESE )
    {
        m_Font.DrawText( 320, 100, 0xFFFFFFFF, strGAME_NAME[ dwLang ],
                         XBFONT_CENTER_X );
        m_Font.DrawText( 320.0f, 240.0f, 0xFFFFFFFF, strMS_XBOX[ dwLang ],
                         XBFONT_CENTER_X | XBFONT_CENTER_Y );
    }
    else
    {
        m_XFontJPN.DrawText( 320, 100, 0xFFFFFFFF, strGAME_NAME[ dwLang ],
                             CXFONT_CENTER_X );
        m_XFontJPN.DrawText( 320, 240, 0xFFFFFFFF, strMS_XBOX[ dwLang ],
                             CXFONT_CENTER_X | CXFONT_CENTER_Y );
    }

    return S_OK;
}
