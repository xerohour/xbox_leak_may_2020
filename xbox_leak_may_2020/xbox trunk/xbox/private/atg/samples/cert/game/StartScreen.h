//-----------------------------------------------------------------------------
// File: StartScreen.h
//
// Desc: Start screen
//
// Hist: 05.07.01 - New for June XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_START_H
#define TECH_CERT_GAME_START_H

#include "Common.h"
#include <XbStopWatch.h>

struct XBGAMEPAD;
class CXBFont;
class CXFont;




//-----------------------------------------------------------------------------
// Name: class StartScreen
// Desc: Start screen
//-----------------------------------------------------------------------------
class StartScreen
{

    CXBFont&     m_Font;
    CXFont&      m_XFontJPN;
    CXBStopWatch m_Timer;

public:

    StartScreen( CXBFont&, CXFont& );

    VOID    Start();
    VOID    End();
    FLOAT   GetElapsedSeconds() const;
    HRESULT Render();
    HRESULT FrameMove( const XBGAMEPAD* );

private:

    // Disabled
    StartScreen( const StartScreen& );
    StartScreen& operator=( const StartScreen& );

};




#endif // TECH_CERT_GAME_START_H
