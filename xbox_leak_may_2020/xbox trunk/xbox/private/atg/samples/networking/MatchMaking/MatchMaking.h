//-----------------------------------------------------------------------------
// File: MatchMaking.h
//
// Desc: Illustrates online matchmaking on Xbox.
//       Allows player to find/create a new multiplayer game, using
//       either the QuickMatch or CustomMatch metaphor.
//       Once game has been chosen, allows players to connect and send
//       messages to other players.
//
// Hist: 08.08.01 - New for Aug M1 release 
//       10.19.01 - Updated for Nov release
//       01.21.02 - Updated for Feb release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef MATCHMAKING_H
#define MATCHMAKING_H

#include "Common.h"
#include "UserInterface.h"
#include "GameMsg.h"
#include "XBApp.h"
#include "XBNet.h"
#include "XBOnlineTask.h"
#include "XBRandName.h"



//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

// Number of services to authenticate
const DWORD NUM_SERVICES = 1;




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    enum State
    {
        STATE_CREATE_ACCOUNT,   // Create user account
        STATE_SELECT_ACCOUNT,   // Select user account
        STATE_GET_PIN,          // Enter user PIN
        STATE_LOGGING_ON,       // Perform authentication

        STATE_SELECT_MATCH,     // QuickMatch or CustomMatch
        STATE_OPTIMATCH,        // Specify game parameters
        STATE_SELECT_TYPE,      // Choose game type
        STATE_SELECT_LEVEL,     // Choose player level
        STATE_SELECT_STYLE,     // Choose game style
        STATE_SELECT_NAME,      // Choose game name
        STATE_SELECT_SESSION,   // Choose a game session

        STATE_GAME_SEARCH,      // Searching for game session
        STATE_REQUEST_JOIN,     // Joining game session
        STATE_CREATE_SESSION,   // Creating game session
        STATE_PLAY_GAME,        // Play game session
        STATE_DELETE_SESSION,   // Deleting game session
        STATE_ERROR             // Error screen
    };

    enum Event
    {
        EV_BUTTON_A,
        EV_BUTTON_B,
        EV_BUTTON_BACK,
        EV_UP,
        EV_DOWN,
        EV_NULL
    };

    UserInterface   m_UI;                // UI object
    GameMsg         m_GameMsg;           // game message object
    State           m_State;             // current state
    State           m_NextState;         // return to this state
    DWORD           m_dwCurrItem;        // current selected menu item
    XBUserList      m_UserList;          // available accounts
    DWORD           m_dwCurrUser;        // index of curr user in m_UserList
    ULONGLONG       m_qwUserID;          // unique player ID
    DWORD           m_dwUserIndex;       // which controller
    DWORD           m_dwSessionIndex;    // index of selected session
                                         //    in m_SessionList
    WCHAR           m_strUser[ XONLINE_USERNAME_SIZE ]; // current user name
    CXBNetLink      m_NetLink;           // network link checking
    CXBPinEntry     m_PinEntry;          // PIN entry object
    SessionInfo     m_Session;           // Our session
    CXBOnlineTask   m_hOnlineTask;
    CXBRandName     m_rand;              // random name generator
    BOOL            m_bIsLoggedOn;
    BOOL            m_bIsSessionRegistered;
    BOOL            m_bIsQuickMatch;    // TRUE if QuickMatch
    BOOL            m_bIsHost;          // TRUE if we are host
    CXBOnlineTask   m_hMatchTask;       // matchmaking online task
    XNKID           m_xnSessionID;      // ID for the current session
    XNKEY           m_xnKeyExchangeKey; // Key Exchange Key for the current session
    IN_ADDR         m_inHostAddr;       // "IP" address of host
    DWORD           m_dwSlotsInUse;     // count of players in session
    WCHAR           m_strStatus[ MAX_STATUS_STR ];
    DWORD           m_pServices[ NUM_SERVICES ]; // List of desired services

    CXBStopWatch    m_GameJoinTimer;
    CXBStopWatch    m_HeartbeatTimer;

    SessionNameList   m_SessionNames;     // list of potential game sessions
    SessionList       m_SessionList;      // list of session information
    CXBNetPlayerList  m_Players;          // list of session players
    SessionUpdateQ    m_SessionUpdateQ;   // queue of session updates to apply
    XONLINE_ATTRIBUTE m_SessionAttributes[ MAX_SESSION_ATTRIBS ];
                                          // Session Attributes

public:

    virtual HRESULT Initialize();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();

    CXBoxSample();

    VOID OnJoinGame( const CXBNetPlayerInfo& );
    VOID OnJoinApproved( const CXBNetPlayerInfo& );
    VOID OnJoinApprovedAddPlayer( const CXBNetPlayerInfo& );
    VOID OnJoinDenied();
    VOID OnPlayerJoined( const CXBNetPlayerInfo& );
    VOID OnWave( const CXBNetPlayerInfo& );
    VOID OnHeartbeat( const CXBNetPlayerInfo& );
    VOID OnPlayerDropout( const CXBNetPlayerInfo&, BOOL bIsHost );

private:

    Event GetEvent() const;

    VOID UpdateStateCreateAccount( Event );
    VOID UpdateStateSelectAccount( Event );
    VOID UpdateStateGetPin( Event );
    VOID UpdateStateLoggingOn( Event );

    VOID UpdateStateSelectMatch( Event );
    VOID UpdateStateOptiMatch( Event );
    VOID UpdateStateSelectType( Event );
    VOID UpdateStateSelectStyle( Event );
    VOID UpdateStateSelectLevel( Event );
    VOID UpdateStateSelectName( Event );
    VOID UpdateStateSelectSession( Event );

    VOID UpdateStateGameSearch( Event );
    VOID UpdateStateRequestJoin( Event );
    VOID UpdateStateCreateSession( Event );
    VOID UpdateStatePlayGame( Event );
    VOID UpdateStateDeleteSession( Event );
    VOID UpdateStateError( Event );

    VOID BeginLogin();
    VOID BeginSessionSearch();
    VOID BeginCreateSession();
    VOID BeginSessionUpdate( SESSION_UPDATE_ACTION dwAction );
    VOID BeginDeleteSession();
    VOID BeginJoinSession();
    VOID BeginAddPlayer();
    VOID BeginRemovePlayer();

    VOID SendWave();
    VOID LeaveGame();
    VOID __cdecl SetStatus( const WCHAR*, ... );

    VOID CancelMatch();
    VOID SetPlayerState( DWORD );
    VOID Reset( BOOL bIsError );

};




#endif // MATCHMAKING_H
