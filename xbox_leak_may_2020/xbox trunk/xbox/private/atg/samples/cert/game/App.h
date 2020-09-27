//-----------------------------------------------------------------------------
// File: App.h
//
// Desc: Technical Certification Requirement Sample Game
//
//       Fully playable game, including save/load, multiplayer (future)
//       Meets all technical certification requirements
//       Showcases integrated graphics and audio effects (future)
//       Includes elements of the standard reference UI (future)
//       Stresses both GPU and CPU (future)
//
// Hist: 01.19.01 - New for May XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_H
#define TECH_CERT_GAME_H

#include "dsound.h"
#include "dsstdfx.h"
#include "cxfont.h"
#include "Common.h"
#include <XbApp.h>
#include <XbFont.h>
#include <XbStopWatch.h>
#include "Splash.h"
#include "StartScreen.h"
#include "Demo.h"
#include "LoadSave.h"
#include "Menu.h"
#include "Game.h"


//-----------------------------------------------------------------------------
// Name: class TechCertGame
// Desc: Main application object
//-----------------------------------------------------------------------------
class TechCertGame : public CXBApplication
{

    enum GameMode
    {
        GAME_MODE_GAME,     // game is running
        GAME_MODE_MENU,     // menu subsystem
        GAME_MODE_DEMO,     // demo mode
        GAME_MODE_SPLASH,   // splash screen
        GAME_MODE_START,    // start screen
        GAME_MODE_LOAD,     // load game
        GAME_MODE_SAVE      // save game
    };

    CXBFont           m_Font;
    CXFont            m_XFontJPN;
    GameMode          m_GameMode;
    GameMode          m_LastMode;
    Splash            m_Splash;
    StartScreen       m_StartScreen;
    Demo              m_Demo;
    Menu              m_Menu;
    Game              m_Game;
    LoadSave          m_LoadSave;
    CXBStopWatch      m_SoftResetTimer;
    CMusicManager     m_MusicManager;
    LPDIRECTSOUND8    m_pdsndDevice;
    CSoundEffect      m_SoundEffect;

public:

    TechCertGame();

    virtual HRESULT Initialize();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();

private:

    HRESULT DownloadEffectsImage( PCHAR );
    VOID    ChangeMode( GameMode );

    static BOOL MemUnitWasInserted();
    static VOID GetSaveGameName( WCHAR* );

    BOOL ConfirmQuit();

    // Disabled
    TechCertGame( const TechCertGame& );
    TechCertGame& operator=( const TechCertGame& );

};

#endif // TECH_CERT_GAME_H
