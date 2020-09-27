//-----------------------------------------------------------------------------
// File: Splash.h
//
// Desc: Splash screen
//
// Hist: 01.24.01 - New for May XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_SPLASH_H
#define TECH_CERT_GAME_SPLASH_H

#include "Common.h"
#include <XbStopWatch.h>

struct XBGAMEPAD;
class CXBFont;
class CXFont;




//-----------------------------------------------------------------------------
// Name: class Splash
// Desc: Splash screen
//-----------------------------------------------------------------------------
class Splash
{

    CXBFont&     m_Font;
    CXFont&      m_XFontJPN;
    CXBStopWatch m_Timer;

public:

    Splash( CXBFont&, CXFont& );

    VOID    Start();
    VOID    End();
    FLOAT   GetElapsedSeconds() const;
    HRESULT Render();
    HRESULT FrameMove( const XBGAMEPAD* );

private:

    // Disabled
    Splash( const Splash& );
    Splash& operator=( const Splash& );

};




#endif // TECH_CERT_GAME_SPLASH_H
