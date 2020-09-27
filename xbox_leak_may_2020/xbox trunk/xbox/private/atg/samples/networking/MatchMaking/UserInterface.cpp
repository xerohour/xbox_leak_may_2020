//-----------------------------------------------------------------------------
// File: UserInterface.cpp
//
// Desc: Matchmaking rendering functions
//
// Hist: 10.19.01 - Updated for Nov release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "UserInterface.h"
#include "Resource.h"
#include <cassert>




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const WCHAR* const strANY          = L"Any";

const WCHAR* const strSHORT        = L"Short";
const WCHAR* const strMEDIUM       = L"Medium";
const WCHAR* const strLONG         = L"Long";

const WCHAR* const strBEGINNER     = L"Beginner";
const WCHAR* const strINTERMEDIATE = L"Intermediate";
const WCHAR* const strADVANCED     = L"Advanced";

const  CHAR* const strHEAVY        =  "Heavy";
const  CHAR* const strLIGHT        =  "Light";
const  CHAR* const strMIXED        =  "Mixed";

const WCHAR* const strHEAVYw       = L"Heavy";
const WCHAR* const strLIGHTw       = L"Light";
const WCHAR* const strMIXEDw       = L"Mixed";




//-----------------------------------------------------------------------------
// Name: UserInterface()
// Desc: Constructor
//-----------------------------------------------------------------------------
UserInterface::UserInterface( WCHAR* strFrameRate )
:
    m_UI( strFrameRate, L"MatchMaking" )
{
}




//-----------------------------------------------------------------------------
// Name: SetErrorStr()
// Desc: Set error string
//-----------------------------------------------------------------------------
VOID __cdecl UserInterface::SetErrorStr( const WCHAR* strFormat, ... )
{
    va_list pArgList;
    va_start( pArgList, strFormat );

    m_UI.SetErrorStr( strFormat, pArgList );

    va_end( pArgList );
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects
//-----------------------------------------------------------------------------
HRESULT UserInterface::Initialize()
{
    return m_UI.Initialize( Resource_NUM_RESOURCES, Resource_MenuSelect_OFFSET );
}




//-----------------------------------------------------------------------------
// Name: RenderSelectMatch()
// Desc: Display matchmaking menu
//-----------------------------------------------------------------------------
VOID UserInterface::RenderSelectMatch( DWORD dwCurrItem ) const
{
    m_UI.RenderHeader();

    m_UI.DrawText( 320, 140, COLOR_NORMAL, L"Find Game Session",
                   XBFONT_CENTER_X );

    const WCHAR* const strMatch[] =
    {
        L"QuickMatch",
        L"OptiMatch"
    };

    const FLOAT fYtop = 200.0f;
    const FLOAT fYdelta = 50.0f;

    for( DWORD i = 0; i < MATCH_MAX; ++i )
    {
        DWORD dwColor = ( dwCurrItem == i ) ? COLOR_HIGHLIGHT : COLOR_NORMAL;
        m_UI.DrawText( 260, fYtop + (fYdelta * i), dwColor, strMatch[i] );
    }

    // Show selected item with little triangle
    m_UI.RenderMenuSelector( 220.0f, fYtop + (fYdelta * dwCurrItem ) );
}




//-----------------------------------------------------------------------------
// Name: RenderOptiMatch()
// Desc: Display OptiMatch screen
//-----------------------------------------------------------------------------
VOID UserInterface::RenderOptiMatch( SessionInfo& session,
                                     DWORD dwCurrItem ) const
{
    m_UI.RenderHeader();

    m_UI.DrawText( 320, 140, COLOR_NORMAL, L"Session Settings",
                   XBFONT_CENTER_X );

    const WCHAR* const strSetting[] =
    {
        L"Set Game Type",
        L"Set Player Level",
        L"Set Game Style",
        L"Set Session Name",
        L"Find Matching Session"
    };

    FLOAT fYtop = 200.0f;
    const FLOAT fYdelta = 40.0f;

    for( DWORD i = 0; i < CUSTOM_MAX; ++i )
    {
        DWORD dwColor = ( dwCurrItem == i ) ? COLOR_HIGHLIGHT : 
                                                COLOR_NORMAL;
        m_UI.DrawText( 160, fYtop + (fYdelta * i), dwColor, strSetting[i] );
    }

    // Show selected item with little triangle
    m_UI.RenderMenuSelector( 120.0f, fYtop + (fYdelta * dwCurrItem ) );

    // Determine current settings as strings
    const WCHAR* strCurrent[ CUSTOM_MAX ];
    switch( session.GetGameType() )
    {
        case TYPE_ANY:    strCurrent[ CUSTOM_TYPE ] = strANY;    break;
        case TYPE_SHORT:  strCurrent[ CUSTOM_TYPE ] = strSHORT;  break;
        case TYPE_MEDIUM: strCurrent[ CUSTOM_TYPE ] = strMEDIUM; break;
        case TYPE_LONG:   strCurrent[ CUSTOM_TYPE ] = strLONG;   break;
        default:          assert( FALSE );                       break;
    }

    switch( *session.GetStylePtr() )
    {
        case BYTE(  0  ): strCurrent[ CUSTOM_STYLE ] = strANY;    break;
        case BYTE( 'H' ): strCurrent[ CUSTOM_STYLE ] = strHEAVYw; break;
        case BYTE( 'L' ): strCurrent[ CUSTOM_STYLE ] = strLIGHTw; break;
        case BYTE( 'M' ): strCurrent[ CUSTOM_STYLE ] = strMIXEDw; break;
        default:          assert( FALSE );                       break;
    }
    if( session.GetStyleLen() == 0 )
        strCurrent[ CUSTOM_STYLE ] = strANY;

    const WCHAR* strPlayerLevel = session.GetPlayerLevel();
    strCurrent[ CUSTOM_LEVEL ] = *strPlayerLevel ? strPlayerLevel : strANY;
    strCurrent[ CUSTOM_NAME ] = session.GetSessionName();
    strCurrent[ CUSTOM_FIND ] = L"";

    // Show current settings
    fYtop = 200.0f;
    for( DWORD i = 0; i < CUSTOM_MAX; ++i )
        m_UI.DrawText( 380, fYtop + (fYdelta * i), COLOR_GREEN, strCurrent[i] );
}




//-----------------------------------------------------------------------------
// Name: RenderSelectType()
// Desc: Display game type screen
//-----------------------------------------------------------------------------
VOID UserInterface::RenderSelectType( DWORD dwCurrItem ) const
{
    m_UI.RenderHeader();

    m_UI.DrawText( 320, 140, COLOR_NORMAL, L"Game Type",
                   XBFONT_CENTER_X );

    const WCHAR* const strType[] =
    {
        strANY,
        strSHORT,
        strMEDIUM,
        strLONG
    };

    const FLOAT fYtop = 200.0f;
    const FLOAT fYdelta = 50.0f;

    for( DWORD i = 0; i < TYPE_MAX; ++i )
    {
        DWORD dwColor = ( dwCurrItem == i ) ? COLOR_HIGHLIGHT : 
                                                COLOR_NORMAL;
        m_UI.DrawText( 260, fYtop + (fYdelta * i), dwColor, strType[i] );
    }

    // Show selected item with little triangle
    m_UI.RenderMenuSelector( 220.0f, fYtop + (fYdelta * dwCurrItem ) );
}




//-----------------------------------------------------------------------------
// Name: RenderSelectLevel()
// Desc: Display player rating screen
//-----------------------------------------------------------------------------
VOID UserInterface::RenderSelectLevel( DWORD dwCurrItem ) const
{
    m_UI.RenderHeader();

    m_UI.DrawText( 320, 140, COLOR_NORMAL, L"Player Level", XBFONT_CENTER_X );

    const WCHAR* const strLevel[] =
    {
        strANY,
        strBEGINNER,
        strINTERMEDIATE,
        strADVANCED
    };

    const FLOAT fYtop = 200.0f;
    const FLOAT fYdelta = 50.0f;

    for( DWORD i = 0; i < LEVEL_MAX; ++i )
    {
        DWORD dwColor = ( dwCurrItem == i ) ? COLOR_HIGHLIGHT : 
                                                COLOR_NORMAL;
        m_UI.DrawText( 260, fYtop + (fYdelta * i), dwColor, strLevel[i] );
    }

    // Show selected item with little triangle
    m_UI.RenderMenuSelector( 220.0f, fYtop + (fYdelta * dwCurrItem ) );
}




//-----------------------------------------------------------------------------
// Name: RenderSelectStyle()
// Desc: Display game style screen
//-----------------------------------------------------------------------------
VOID UserInterface::RenderSelectStyle( DWORD dwCurrItem ) const
{
    m_UI.RenderHeader();

    m_UI.DrawText( 320, 140, COLOR_NORMAL, L"Game Style", XBFONT_CENTER_X );

    const WCHAR* const strStyle[] =
    {
        strANY,
        strHEAVYw,
        strLIGHTw,
        strMIXEDw
    };

    const FLOAT fYtop = 200.0f;
    const FLOAT fYdelta = 50.0f;

    for( DWORD i = 0; i < STYLE_MAX; ++i )
    {
        DWORD dwColor = ( dwCurrItem == i ) ? COLOR_HIGHLIGHT : 
                                                COLOR_NORMAL;
        m_UI.DrawText( 260, fYtop + (fYdelta * i), dwColor, strStyle[i] );
    }

    // Show selected item with little triangle
    m_UI.RenderMenuSelector( 220.0f, fYtop + (fYdelta * dwCurrItem ) );
}




//-----------------------------------------------------------------------------
// Name: RenderSelectName()
// Desc: Display game name screen
//-----------------------------------------------------------------------------
VOID UserInterface::RenderSelectName( DWORD dwCurrItem, 
                                      const SessionNameList& SessionNames ) const
{
    assert( dwCurrItem < SessionNames.size() );

    m_UI.RenderHeader();
    m_UI.DrawText( 320, 140, COLOR_NORMAL, L"Session Name", XBFONT_CENTER_X );

    const FLOAT fYtop = 200.0f;
    const FLOAT fYdelta = 40.0f;

    for( DWORD i = 0; i < SessionNames.size(); ++i )
    {
        DWORD dwColor = ( dwCurrItem == i ) ? COLOR_HIGHLIGHT : COLOR_NORMAL;
        m_UI.DrawText( 260, fYtop + (fYdelta * i), dwColor, SessionNames[i].c_str() );
    }

    // Show selected item with little triangle
    m_UI.RenderMenuSelector( 220.0f, fYtop + (fYdelta * dwCurrItem ) );
}




//-----------------------------------------------------------------------------
// Name: RenderSelectSession()
// Desc: Display session name screen
//-----------------------------------------------------------------------------
VOID UserInterface::RenderSelectSession( DWORD dwCurrItem, 
                                         SessionList& Sessions ) const
{
    assert( dwCurrItem < Sessions.size() );

    m_UI.RenderHeader();
    m_UI.DrawText( 320, 120, COLOR_NORMAL, L"Sessions", XBFONT_CENTER_X );

    const FLOAT fYtop      = 200.0f;
    const FLOAT fYdelta    =  40.0f;
    const FLOAT SESSION_POSITION =  70.0f;
    const FLOAT LEVEL_POSITION   = 210.0f;
    const FLOAT STYLE_POSITION   = 340.0f;
    const FLOAT TYPE_POSITION    = 420.0f;
    const FLOAT PLAYER_POSITION  = 480.0f;

    m_UI.DrawText( SESSION_POSITION, 160, COLOR_NORMAL,  L"Session", 0 );
    m_UI.DrawText( LEVEL_POSITION,   160, COLOR_NORMAL,  L"Level", 0 );
    m_UI.DrawText( STYLE_POSITION,   160, COLOR_NORMAL,  L"Style", 0 );
    m_UI.DrawText( TYPE_POSITION,    160, COLOR_NORMAL,  L"Type", 0 );
    m_UI.DrawText( PLAYER_POSITION,  160, COLOR_NORMAL,  L"Open", 0 );

    for( DWORD i = 0; i < Sessions.size(); ++i )
    {
        FLOAT fYPosition = fYtop + (fYdelta * i);
        DWORD dwColor = ( dwCurrItem == i ) ? COLOR_HIGHLIGHT : COLOR_NORMAL;

        ULONGLONG qwGameType = Sessions[i].GetGameType();
        PWSTR strType;

        switch (qwGameType)
        {
        case TYPE_SHORT:
            strType = L"Short";
            break;
        case TYPE_MEDIUM:
            strType = L"Med";
            break;
        case TYPE_LONG:
            strType = L"Long";
            break;
        default:
            strType = L"";
            assert(0);
        }

        m_UI.DrawText( SESSION_POSITION, fYPosition, dwColor, 
            Sessions[i].GetSessionName() );
        m_UI.DrawText( LEVEL_POSITION,   fYPosition, dwColor, 
            Sessions[i].GetPlayerLevel() );

        // Fetch the style (which is stored as a blob)
        CHAR  strStyle[ MAX_STYLE_BLOB ];
        WCHAR strStyleW[ MAX_STYLE_BLOB ];

        CopyMemory( strStyle, Sessions[i].GetStylePtr(),  Sessions[i].GetStyleLen() );
        strStyle[ Sessions[i].GetStyleLen() ] = '\0'; // Terminate the string

        XBUtil_GetWide( strStyle, strStyleW, MAX_STYLE_BLOB );

        m_UI.DrawText( STYLE_POSITION,   fYPosition, dwColor, 
            strStyleW ); 
        m_UI.DrawText( TYPE_POSITION,   fYPosition, dwColor, 
            strType ); 

        WCHAR strPlayers[ 32 ];

        wsprintfW( strPlayers, L"%lu", Sessions[i].GetPublicAvail() );
        m_UI.DrawText( PLAYER_POSITION + 20.0f,   fYPosition, dwColor, 
            strPlayers );

    }

    // Show selected item with little triangle
    m_UI.RenderMenuSelector( 40.0f, fYtop + (fYdelta * dwCurrItem ) );

}


//-----------------------------------------------------------------------------
// Name: RenderCreateAccount()
// Desc: Allow player to launch account creation tool
//-----------------------------------------------------------------------------
VOID UserInterface::RenderCreateAccount( BOOL bHasMachineAccount ) const
{
    m_UI.RenderCreateAccount( bHasMachineAccount );
}




//-----------------------------------------------------------------------------
// Name: RenderSelectAccount()
// Desc: Display list of accounts
//-----------------------------------------------------------------------------
VOID UserInterface::RenderSelectAccount( DWORD dwCurrItem, 
                                         XBUserList& UserList ) const
{
    m_UI.RenderSelectAccount( dwCurrItem, UserList );
}




//-----------------------------------------------------------------------------
// Name: RenderGetPin()
// Desc: Display PIN entry screen
//-----------------------------------------------------------------------------
VOID UserInterface::RenderGetPin( const CXBPinEntry& PinEntry, 
                                  const WCHAR* strUser ) const
{
    m_UI.RenderGetPin( PinEntry, strUser );
}




//-----------------------------------------------------------------------------
// Name: RenderLogginOn()
// Desc: Display login message
//-----------------------------------------------------------------------------
VOID UserInterface::RenderLoggingOn() const
{
    m_UI.RenderLoggingOn();
}




//-----------------------------------------------------------------------------
// Name: RenderGameSearch()
// Desc: Display game search screen
//-----------------------------------------------------------------------------
VOID UserInterface::RenderGameSearch() const
{
    m_UI.RenderHeader();
    m_UI.DrawText( 320, 200, COLOR_NORMAL, L"Searching for Game Sessions", 
                   XBFONT_CENTER_X );
    m_UI.DrawText( 320, 260, COLOR_NORMAL, L"Press B to cancel", 
                   XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: RenderRequestJoin()
// Desc: Display game join screen
//-----------------------------------------------------------------------------
VOID UserInterface::RenderRequestJoin() const
{
    m_UI.RenderHeader();
    m_UI.DrawText( 320, 200, COLOR_NORMAL, L"Joining Game", 
                   XBFONT_CENTER_X );
    m_UI.DrawText( 320, 260, COLOR_NORMAL, L"Press B to cancel", 
                   XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: RenderCreateSession()
// Desc: Display game create screen
//-----------------------------------------------------------------------------
VOID UserInterface::RenderCreateSession() const
{
    m_UI.RenderHeader();
    m_UI.DrawText( 320, 200, COLOR_NORMAL, L"Registering Game Session", 
                   XBFONT_CENTER_X );
    m_UI.DrawText( 320, 260, COLOR_NORMAL, L"Press B to cancel", 
                   XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: RenderPlayGame()
// Desc: Display game
//-----------------------------------------------------------------------------
VOID UserInterface::RenderPlayGame( SessionInfo& session,
                                    WCHAR* strUser,
                                    WCHAR* strStatus,
                                    DWORD dwPlayerCount,
                                    DWORD dwCurrItem ) const
{
    m_UI.RenderHeader();

    // Game name and player name
    WCHAR strGameInfo[ 32 + MAX_SESSION_STR + MAX_PLAYER_STR ];
    wsprintfW( strGameInfo, L"Session name: %.*s\nYour name: %.*s", 
               MAX_SESSION_STR, session.GetSessionName(), 
               MAX_PLAYER_STR, strUser );
    m_UI.DrawText( 320, 100, COLOR_GREEN, strGameInfo, XBFONT_CENTER_X );

    // Determine current game type as string
    const WCHAR* strType = L"";
    switch( session.GetGameType() )
    {
        case TYPE_SHORT:  strType = strSHORT;  break;
        case TYPE_MEDIUM: strType = strMEDIUM; break;
        case TYPE_LONG:   strType = strLONG;   break;
        default:          assert( FALSE );     break;
    }

    // Determine current game style as string
    const WCHAR* strStyle = L"";
    switch( *session.GetStylePtr() )
    {
        case BYTE( 'H' ): strStyle = strHEAVYw; break;
        case BYTE( 'L' ): strStyle = strLIGHTw; break;
        case BYTE( 'M' ): strStyle = strMIXEDw; break;
        default:          assert( FALSE );      break;
    }

    // Game info
    wsprintfW( strGameInfo, L"Type: %.*s, Level: %.*s, Style: %.*s",
               MAX_TYPE_STR, strType,
               MAX_LEVEL_STR, session.GetPlayerLevel(),
               MAX_STYLE_BLOB + 1, strStyle );
    m_UI.DrawText( 320, 150, COLOR_GREEN, strGameInfo, XBFONT_CENTER_X );

    // Number of players and current status
    wsprintfW( strGameInfo, L"Players in game: %lu", dwPlayerCount );
    m_UI.DrawText( 320, 180, COLOR_GREEN, strGameInfo, XBFONT_CENTER_X );
    m_UI.DrawText( 320, 220, COLOR_GREEN, strStatus, XBFONT_CENTER_X );

    // Game options menu
    const WCHAR* const strMenu[] =
    {
        L"Wave To Other Players",
        L"Leave Game"
    };

    FLOAT fYtop = 280.0f;
    FLOAT fYdelta = 50.0f;

    // Show menu
    for( DWORD i = 0; i < GAME_MAX; ++i )
    {
        DWORD dwColor = ( dwCurrItem == i ) ? COLOR_HIGHLIGHT : COLOR_NORMAL;
        m_UI.DrawText( 260, fYtop + (fYdelta * i), dwColor, strMenu[i] );
    }

    // Show selected item with little triangle
    m_UI.RenderMenuSelector( 220.0f, fYtop + (fYdelta * dwCurrItem ) );
}




//-----------------------------------------------------------------------------
// Name: RenderDeleteSession()
// Desc: Display game deletion screen
//-----------------------------------------------------------------------------
VOID UserInterface::RenderDeleteSession() const
{
    m_UI.RenderHeader();
    m_UI.DrawText( 320, 200, COLOR_NORMAL, L"Unregistering Game Session", 
                   XBFONT_CENTER_X );
    m_UI.DrawText( 320, 260, COLOR_NORMAL, L"Press B to cancel", 
                   XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: RenderError()
// Desc: Display error message
//-----------------------------------------------------------------------------
VOID UserInterface::RenderError() const
{
    m_UI.RenderError();
}
