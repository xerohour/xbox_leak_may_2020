//-----------------------------------------------------------------------------
// File: VoiceChat.h
//
// Hist: 08.08.01 - New for Aug M1 release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma warning( disable: 4786 )
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBStopWatch.h>
#include <XBSocket.h>
#include <XBSockAddr.h>
#include <cassert>
#include <vector>
#include <algorithm>
#include "Resource.h"
#include "Voice.h"


//-----------------------------------------------------------------------------
// Name: class Message
// Desc: Message object sent between players and hosts
//-----------------------------------------------------------------------------
class Message
{
    BYTE m_byMessageId;

    union
    {
        MsgFindGame     m_FindGame;
        MsgGameFound    m_GameFound;
        MsgJoinGame     m_JoinGame;
        MsgJoinApproved m_JoinApproved;
        MsgPlayerJoined m_PlayerJoined;
        MsgVoicePort    m_VoicePort;
    };

public:

    explicit Message( BYTE byMessageId = 0 ) : m_byMessageId( byMessageId ) {}
    ~Message() {}

    BYTE GetId() const      { return m_byMessageId; }
    INT  GetMaxSize() const { return sizeof(*this); }

    MsgFindGame&     GetFindGame()     { return m_FindGame;     }
    MsgGameFound&    GetGameFound()    { return m_GameFound;    }
    MsgJoinGame&     GetJoinGame()     { return m_JoinGame;     }
    MsgJoinApproved& GetJoinApproved() { return m_JoinApproved; }
    MsgPlayerJoined& GetPlayerJoined() { return m_PlayerJoined; }
    MsgVoicePort&    GetMsgVoicePort() { return m_VoicePort;    }

    INT GetSize() const
    { 
        switch( m_byMessageId )
        {
            case MSG_FIND_GAME:     return sizeof(BYTE) + sizeof(MsgFindGame);
            case MSG_GAME_FOUND:    return sizeof(BYTE) + sizeof(MsgGameFound);
            case MSG_JOIN_GAME:     return sizeof(BYTE) + sizeof(MsgJoinGame);
            case MSG_JOIN_APPROVED: return sizeof(BYTE) + sizeof(MsgJoinApproved);
            case MSG_JOIN_DENIED:   return sizeof(BYTE);
            case MSG_PLAYER_JOINED: return sizeof(BYTE) + sizeof(MsgPlayerJoined);
            case MSG_WAVE:          return sizeof(BYTE);
            case MSG_HEARTBEAT:     return sizeof(BYTE);
            case MSG_VOICEPORT:     return sizeof(BYTE) + sizeof(MsgVoicePort);
            default: assert( FALSE ); return 0;
        }
    }

private:

    // Disabled
    Message( const Message& );
    Message& operator=( const Message& );

};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    enum State
    {
        STATE_MENU,             // Main menu
        STATE_GAME,             // Game menu
        STATE_HELP,             // Help screen
        STATE_SELECT_NAME,      // Select game name screen
        STATE_START_NEW_GAME,   // Starting new game
        STATE_GAME_SEARCH,      // Searching for game
        STATE_SELECT_GAME,      // Game selection menu
        STATE_REQUEST_JOIN,     // Joining game
        STATE_ERROR             // Error screen
    };

    enum Event
    {
        EV_BUTTON_A,
        EV_BUTTON_B,
        EV_BUTTON_BACK,
        EV_UP,
        EV_DOWN,
        EV_LEFT,
        EV_RIGHT,
        EV_DISCONNECT,
        EV_NULL
    };

    enum
    {
        // Main menu
        MAIN_MENU_START_GAME = 0,
        MAIN_MENU_JOIN_GAME  = 1,
        MAIN_MENU_MAX,

        // Game menu
        GAME_MENU_WAVE       = 0,
        GAME_MENU_VOICE      = 1,
        GAME_MENU_LEAVE_GAME = 2,
        GAME_MENU_MAX
    };

    enum InitStatus
    {
        Success,
        NotConnected,
        InitFailed
    };

    typedef std::vector< std::wstring > NameList;
    typedef std::vector< GameInfo >     GameList;
    typedef std::vector< PlayerInfo >   PlayerList;

    CXBPackedResource   m_xprResource;   // app resources
    LPDIRECT3DTEXTURE8  m_ptMenuSel;     // menu selection image
    mutable HANDLE      m_hLogFile;      // Log file
    CXBFont      m_Font;                 // game font
    CXBHelp      m_Help;                 // help screen
    State        m_State;                // game state
    State        m_LastState;            // last state
    DWORD        m_CurrItem;             // current menu item
    NameList     m_GameNames;            // list of potential game names
    GameList     m_Games;                // list of available games
    PlayerList   m_Players;              // list of current players (not incl self)
    WCHAR        m_strError[ MAX_ERROR_STR ];   // error message
    WCHAR        m_strStatus[ MAX_STATUS_STR ]; // status
    CXBStopWatch m_LinkStatusTimer;      // wait to check link status
    CXBStopWatch m_GameSearchTimer;      // wait for game search to complete
    CXBStopWatch m_GameJoinTimer;        // wait for game join to complete
    CXBStopWatch m_HeartbeatTimer;       // keep-alive timer
    BOOL         m_bIsOnline;            // TRUE if link status good
    BOOL         m_bXnetStarted;         // TRUE if networking initialized
    BOOL         m_bIsHost;              // TRUE if we're hosting the game
    BOOL         m_bIsSessionRegistered; // TRUE if session key registered
    BOOL         m_bHaveLocalAddress;    // TRUE if local address acquired
    XNKID        m_xnHostKeyID;          // Host key ID
    XNKEY        m_xnHostKeyExchange;    // Host key exchange key
    XNADDR       m_xnTitleAddress;       // The XNet address of this machine/game
    IN_ADDR      m_inHostAddr;           // The "IP" address of the the host
    CXBSocket    m_BroadSock;            // Broadcast socket for broadcast msgs
    CXBSocket    m_DirectSock;           // Direct socket for direct msgs
    WCHAR        m_strGameName[ MAX_GAME_NAME ];     // Game name
    WCHAR        m_strPlayerName[ MAX_PLAYER_NAME ]; // This player name
    WCHAR        m_strHostName[ MAX_PLAYER_NAME ];   // Host player name
    Nonce        m_Nonce;                            // Client identifier


    CCommunicatorMgr      *m_pCommunicatorMgr; // The headset 
    MsgVoicePort           m_msgCommunicatorStatus[COMMUNICATOR_COUNT]; // The voiceports for each of the communicators

public:

    virtual HRESULT Initialize();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();

    CXBoxSample();

private:

    Event GetEvent();

    VOID FrameMoveMenu( Event );
    VOID FrameMoveGame( Event );
    VOID FrameMoveHelp( Event );
    VOID FrameMoveSelectName( Event );
    VOID FrameMoveStartGame( Event );
    VOID FrameMoveGameSearch( Event );
    VOID FrameMoveSelectGame( Event );
    VOID FrameMoveRequestJoin( Event );
    VOID FrameMoveError( Event );

    VOID RenderMenu();
    VOID RenderGame();
    VOID RenderHelp();
    VOID RenderSelectName();
    VOID RenderStartGame();
    VOID RenderGameSearch();
    VOID RenderSelectGame();
    VOID RenderRequestJoin();
    VOID RenderError();
    VOID RenderHeader();
    VOID RenderMenuSelector( FLOAT, FLOAT );

    VOID InitiateJoin( DWORD );
    VOID Wave();
    VOID StartVoice();
    VOID ProcessVoice();
    VOID Heartbeat();
    VOID AdvertiseVoicePorts();
    VOID RespondToVoicePorts(const CXBSockAddr&);

    VOID Init();

    // Initialization
    InitStatus InitXNet();

    // Send messages
    VOID SendFindGame();
    VOID SendGameFound( const Nonce& );
    VOID SendJoinGame( const CXBSockAddr& );
    VOID SendJoinApproved( const CXBSockAddr& );
    VOID SendJoinDenied( const CXBSockAddr& );
    VOID SendPlayerJoined( const Player&, const CXBSockAddr& );
    VOID SendWave( const CXBSockAddr& );
    VOID SendHeartbeat( const CXBSockAddr& );
    VOID SendVoicePort( ACTION action , WORD wVoicePort , const CXBSockAddr& );
    

    // Receive messages
    BOOL ProcessBroadcastMessage();
    BOOL ProcessDirectMessage();
    VOID ProcessMessage( Message& );
    VOID ProcessMessage( Message&, const CXBSockAddr& );

    // Process incoming messages
    VOID ProcessFindGame( const MsgFindGame& );
    VOID ProcessGameFound( const MsgGameFound& );
    VOID ProcessJoinGame( const MsgJoinGame&, const CXBSockAddr& );
    VOID ProcessJoinApproved( const MsgJoinApproved&, const CXBSockAddr& );
    VOID ProcessJoinDenied( const CXBSockAddr& );
    VOID ProcessPlayerJoined( const MsgPlayerJoined&, const CXBSockAddr& );
    VOID ProcessWave( const CXBSockAddr& );
    VOID ProcessHeartbeat( const CXBSockAddr& );
    VOID ProcessVoicePort( const MsgVoicePort& , const CXBSockAddr& );

    // Handle keep-alive
    BOOL ProcessPlayerDropouts();

    // Utility
    VOID DestroyGameList();
    VOID DestroyPlayerList();
    static VOID GenRandom( WCHAR*, DWORD );
    static WCHAR GetRandVowel();
    static WCHAR GetRandConsonant();
    static VOID AppendConsonant( WCHAR*, BOOL );
    static VOID AppendVowel( WCHAR* );

    VOID LogXNetError( const CHAR*, INT ) const;

};
