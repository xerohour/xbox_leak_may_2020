//-----------------------------------------------------------------------------
// File: MenuOptions.h
//
// Desc: Options menu
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_MENU_OPTIONS_H
#define TECH_CERT_GAME_MENU_OPTIONS_H

#include "Common.h"
#include <XbStopWatch.h>

struct XBGAMEPAD;
class CXBFont;
class CXFont;




//-----------------------------------------------------------------------------
// Name: class MenuOptions
// Desc: Options menu
//-----------------------------------------------------------------------------
class MenuOptions
{

    CXBFont&            m_Font;
    CXFont&             m_XFontJPN;
    LPDIRECT3DTEXTURE8  m_ptMenuSel;
    CXBStopWatch        m_JoyTimer;
    INT                 m_iCurrIndex;
    BOOL                m_bIsVibrationOn;
    FLOAT               m_fMusicVolume;
    FLOAT               m_fEffectVolume;
    BOOL                m_bExitMenu;

public:

    MenuOptions( CXBFont&, CXFont& );

    VOID     Start( LPDIRECT3DTEXTURE8 );       // Display options menu
    VOID     End();                             // Remove options menu

    HRESULT  FrameMove( const XBGAMEPAD* );
    HRESULT  Render();

    BOOL     ExitMenu() const;

    BOOL     IsVibrationOn() const;
    FLOAT    GetMusicVolume() const;
    FLOAT    GetEffectsVolume() const;

private:

    // Disabled
    MenuOptions( const MenuOptions& );
    MenuOptions& operator=( const MenuOptions& );

};




#endif // TECH_CERT_GAME_MENU_OPTIONS_H
