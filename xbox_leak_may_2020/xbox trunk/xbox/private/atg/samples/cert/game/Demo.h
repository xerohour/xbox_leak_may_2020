//-----------------------------------------------------------------------------
// File: Demo.h
//
// Desc: Demo mode
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_DEMO_H
#define TECH_CERT_GAME_DEMO_H

#include "Common.h"
#include "Game.h"
#include <XbStopWatch.h>

struct XBGAMEPAD;
class CXBFont;
class CXFont;




//-----------------------------------------------------------------------------
// Name: class Demo
// Desc: Demo mode
//-----------------------------------------------------------------------------
class Demo
{

    CXBFont&     m_Font;
    CXFont&      m_XFontJPN;
    CXBStopWatch m_Timer;
    Game         m_GameDemo;

public:

    Demo( CXBFont&, CXFont&, CMusicManager& musicmgr, CSoundEffect& sndeffect );

    VOID    Start( FLOAT fMusicVolume, FLOAT fEffectVolume ); // Begin demo
    VOID    End();   // End demo

    HRESULT FrameMove( const XBGAMEPAD*, FLOAT fTime, FLOAT fElapsedTime );
    HRESULT Render();

    BOOL    IsComplete() const; // TRUE when demo over

private:

    // Disabled
    Demo( const Demo& );
    Demo& operator=( const Demo& );

};




#endif // TECH_CERT_GAME_DEMO_H
