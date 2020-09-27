//-----------------------------------------------------------------------------
// File: UserInterface.h
//
// Desc: Matchmaking rendering functions
//
// Hist: 10.19.01 - Updated for Nov release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef MATCHMAKING_UI_H
#define MATCHMAKING_UI_H

#include "Common.h"
#include "XBOnline.h"




//-----------------------------------------------------------------------------
// Name: class UserInterface
// Desc: Main UI class for Matchmaking sample
//-----------------------------------------------------------------------------
class UserInterface
{
    CXBOnlineUI m_UI;

    static const D3DCOLOR COLOR_NORMAL    = CXBOnlineUI::COLOR_NORMAL;
    static const D3DCOLOR COLOR_HIGHLIGHT = CXBOnlineUI::COLOR_HIGHLIGHT;
    static const D3DCOLOR COLOR_GREEN     = CXBOnlineUI::COLOR_GREEN;

public:

    explicit UserInterface( WCHAR* );

    HRESULT Initialize();

    // Accessors
    VOID __cdecl SetErrorStr( const WCHAR*, ... );

    // UI functions
    VOID RenderSelectMatch( DWORD dwCurrItem ) const;
    VOID RenderOptiMatch(  SessionInfo&, DWORD dwCurrItem ) const;
    VOID RenderSelectType( DWORD dwCurrItem ) const;
    VOID RenderSelectStyle( DWORD dwCurrItem ) const;
    VOID RenderSelectLevel( DWORD dwCurrItem ) const;
    VOID RenderSelectName( DWORD dwCurrItem, const SessionNameList& ) const;
    VOID RenderSelectSession( DWORD dwCurrItem, SessionList& ) const;

    VOID RenderCreateAccount( BOOL bHasMachineAccount ) const;
    VOID RenderSelectAccount( DWORD, XBUserList& ) const;
    VOID RenderGetPin( const CXBPinEntry&, const WCHAR* ) const;
    VOID RenderLoggingOn() const;
    VOID RenderCancel() const;

    VOID RenderGameSearch() const;
    VOID RenderRequestJoin() const;
    VOID RenderCreateSession() const;
    VOID RenderPlayGame( SessionInfo&, WCHAR* strUser,
                         WCHAR* strStatus, DWORD dwPlayerCount, 
                         DWORD dwCurrItem ) const;
    VOID RenderDeleteSession() const;
    VOID RenderError() const;

private:

    // Disabled
    UserInterface();
    UserInterface( const UserInterface& );

};

#endif // MATCHMAKING_UI_H
