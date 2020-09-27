//-----------------------------------------------------------------------------
// File: Menu.h
//
// Desc: Main menu
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_MENU_H
#define TECH_CERT_GAME_MENU_H

#include "Common.h"
#include <XbStopWatch.h>
#include <XbResource.h>
#include "MenuOptions.h"

struct XBGAMEPAD;
class CXBFont;
class CXFont;



//-----------------------------------------------------------------------------
// Name: class Menu
// Desc: Main menu
//-----------------------------------------------------------------------------
class Menu
{

public:

    enum MenuItem
    {
        // Main menu
        MENU_ITEM_START,
        MENU_ITEM_LOAD_GAME,
        MENU_ITEM_OPTIONS,

        // Ingame menu
        MENU_ITEM_RESUME,
        MENU_ITEM_SAVE_GAME,
        MENU_ITEM_QUIT,

        MENU_ITEM_MAX
    };

    enum MenuType
    {
        Normal,
        InGame
    };

private:

    enum MenuMode
    {
        MENU_MODE_MAIN,     // main menu
        MENU_MODE_OPTIONS   // options
    };

	CXBPackedResource   m_xprResource;
    LPDIRECT3DTEXTURE8  m_ptMenuSel;
    CXBFont&            m_Font;
    CXFont&             m_XFontJPN;
    CXBStopWatch        m_InactiveTimer;
    CXBStopWatch        m_JoyTimer;
    MenuOptions         m_Options;
    MenuMode            m_MenuMode;
    MenuType            m_MenuType;
    MenuItem            m_arrMenu[ MENU_ITEM_MAX ];
    const WCHAR*        m_strMenu[ MENU_ITEM_MAX ];
    INT                 m_iCurrIndex;
    INT                 m_iMaxItems;

public:

    Menu( CXBFont&, CXFont& );

    VOID     Start( MenuType );   // Begin menu display
    VOID     End();               // End menu display

    MenuItem GetCurrItem() const;
    FLOAT    GetInactiveSeconds() const;
    BOOL     StartGame() const;
    BOOL     LoadGame() const;
    VOID     ChangeMode( MenuMode );
    VOID     FrameMoveMainMenu( const XBGAMEPAD* );
    VOID     RenderMainMenu();

    HRESULT  FrameMove( const XBGAMEPAD* );
    HRESULT  Render();

    BOOL     IsVibrationOn() const;
    FLOAT    GetMusicVolume() const;
    FLOAT    GetEffectsVolume() const;

private:

    // Disabled
    Menu( const Menu& );
    Menu& operator=( const Menu& );
};




#endif // TECH_CERT_GAME_MENU_H
