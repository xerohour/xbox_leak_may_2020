//-----------------------------------------------------------------------------
// File: WinsockPeer.cpp
//
// Desc: Illustrates Winsock peer-to-peer networking on Xbox.
//       Based on the DX8 SimplePeer sample.
//       Allows player to start/host a new multiplayer game, connect
//       to an existing multiplayer game, and send simple messages
//       to other players. Uses the secure Xbox network stack. Properly
//       handles player disconnects. Meets System Link certification 
//       requirements. Does not include host migration.
//
// Hist: 05.14.01 - New for June XDK release 
//       10.10.01 - Updated for Nov XDK release
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
#include <XBNet.h>
#include <XBRandName.h>
#include <cassert>
#include <vector>
#include <algorithm>
#include "Resource.h"





//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD COLOR_HIGHLIGHT   = 0xffffff00; // Yellow
const DWORD COLOR_GREEN       = 0xff00ff00;
const DWORD COLOR_NORMAL      = 0xffffffff;
const DWORD MAX_ERROR_STR     = 64;
const DWORD MAX_STATUS_STR    = 128;
const DWORD MAX_GAME_NAMES    = 6;      // Number of game names to choose from
const DWORD NONCE_BYTES       = 8;      // Larger means less chance of random matches
const DWORD MAX_GAME_NAME     = 12;     // Includes null
const DWORD MAX_PLAYER_NAME   = 12;     // Includes null
const DWORD PLAYER_TIMEOUT    = 2000;   // 2 seconds
const FLOAT CHECK_LINK_STATUS = 0.5f;   // Check status twice/sec
const FLOAT PLAYER_HEARTBEAT  = 0.3f;   // ~3 times per second
const FLOAT GAME_JOIN_TIME    = 2.0f;   // 2 seconds
const WORD  BROADCAST_PORT    = 10983;  // could be any port
const WORD  DIRECT_PORT       = 10984;  // any port other than BROADCAST_PORT

// TCR 3-46 Maximum Number of Connected Xboxes
const DWORD MAX_PLAYERS = 4;            // Max players (may not exceed 16)

// TCR 3-59 Session Discovery Time
const FLOAT GAME_SEARCH_TIME = 2.0f;   // 2 seconds (may not exceed 3)

const WCHAR* const strLOST_CONNECTION = L"This Xbox has lost its "
                                        L"System Link connection";




//-----------------------------------------------------------------------------
// Name: class GameInfo
// Desc: Game information used by clients to store available games
//-----------------------------------------------------------------------------
struct GameInfo
{
    XNKID  xnHostKeyID;                    // host key ID
    XNKEY  xnHostKey;                      // host key
    XNADDR xnHostAddr;                     // host XNet address
    BYTE   byNumPlayers;                   // number of players in game
    WCHAR  strGameName[ MAX_GAME_NAME ];   // name of the game
    WCHAR  strHostName[ MAX_PLAYER_NAME ]; // name of the host player
};




//-----------------------------------------------------------------------------
// Name: class PlayerInfo
// Desc: Player information used by players to store list of other players
//       in the game
//-----------------------------------------------------------------------------
struct PlayerInfo
{
    XNADDR  xnAddr;                           // XNet address
    IN_ADDR inAddr;                           // Xbox IP (not a "real" IP)
    WCHAR   strPlayerName[ MAX_PLAYER_NAME ]; // player name
    DWORD   dwLastHeartbeat;                  // last heartbeat, in our clocks
};




//-----------------------------------------------------------------------------
// Name: class MatchInAddr
// Desc: Predicate functor used to match on IN_ADDRs in player lists
//-----------------------------------------------------------------------------
struct MatchInAddr
{
    IN_ADDR ia;
    explicit MatchInAddr( const CXBSockAddr& sa ) : ia( sa.GetInAddr() ) { }
    bool operator()( const PlayerInfo& playerInfo )
    {
        return playerInfo.inAddr.s_addr == ia.s_addr;
    }
};




//-----------------------------------------------------------------------------
// Message IDs
//
// A "host" is the player who started the game.
// A "client" is a potential player. A client is not currently playing a game.
// A "player" is anyone playing a game.
//-----------------------------------------------------------------------------
enum
{                       // From     To      Type        Expected response
                        //-----------------------------------------------------
    MSG_FIND_GAME,      // client   host    broadcast   MSG_GAME_FOUND
    MSG_GAME_FOUND,     // host     client  broadcast   <none>
    MSG_JOIN_GAME,      // client   host    direct      MSG_JOIN_APPROVED/DENIED
    MSG_JOIN_APPROVED,  // host     client  direct      <none>
    MSG_JOIN_DENIED,    // host     client  direct      <none>
    MSG_PLAYER_JOINED,  // host     player  direct      <none>
    MSG_WAVE,           // player   player  direct      <none>
    MSG_HEARTBEAT       // player   player  direct      <none>
};




//-----------------------------------------------------------------------------
// Message payloads
//-----------------------------------------------------------------------------
// Pack to minimize network traffic
#pragma pack( push )
#pragma pack( 1 )

//-----------------------------------------------------------------------------
// Local Player struct used by some messages
//-----------------------------------------------------------------------------
struct Player
{
    XNADDR xnAddr;                           // player's XNet address
    WCHAR  strPlayerName[ MAX_PLAYER_NAME ]; // player's name
};

//-----------------------------------------------------------------------------
// Local Nonce struct used by some messages
//-----------------------------------------------------------------------------
struct Nonce
{
    // Used for client verification. The larger the number of NONCE_BYTES,
    // the less likely there is to be an accidental match between client & host
    BYTE byRandom[ NONCE_BYTES ]; 
};

//-----------------------------------------------------------------------------
// Broadcast by a client looking for available games
//-----------------------------------------------------------------------------
struct MsgFindGame
{
    Nonce nonce;    // Generated by client; used to verify host response
};

//-----------------------------------------------------------------------------
// Broadcast by a host in response to a MSG_FIND_GAME
//-----------------------------------------------------------------------------
struct MsgGameFound
{
    Nonce  nonce;                          // used for client verification
    XNKID  xnHostKeyID;                    // host key ID
    XNKEY  xnHostKey;                      // host key
    XNADDR xnHostAddr;                     // host XNet address
    BYTE   byNumPlayers;                   // number of players in game
    WCHAR  strGameName[ MAX_GAME_NAME ];   // game name
    WCHAR  strHostName[ MAX_PLAYER_NAME ]; // game host player name
};

//-----------------------------------------------------------------------------
// Sent from a client to a host to join a game
//-----------------------------------------------------------------------------
struct MsgJoinGame
{
    WCHAR  strPlayerName[ MAX_PLAYER_NAME ];  // player who wants to join
};

//-----------------------------------------------------------------------------
// Sent from a host to a client in response to a MSG_JOIN_GAME
//-----------------------------------------------------------------------------
struct MsgJoinApproved
{
    WCHAR  strHostName[ MAX_PLAYER_NAME ]; // host name
    BYTE   byNumPlayers;                   // Players in the game (not incl host)
    Player PlayerList[ MAX_PLAYERS ];      // List of players (not incl host)
};

//-----------------------------------------------------------------------------
// Sent from a host to other players to notify them that a new player has joined
//-----------------------------------------------------------------------------
struct MsgPlayerJoined
{
    Player player; // The latest player to join the game
};

#pragma pack( pop )




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
        GAME_MENU_LEAVE_GAME = 1,
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
    CXBRandName  m_rand;                             // Random name generator

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
    VOID Heartbeat();

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

    // Handle keep-alive
    BOOL ProcessPlayerDropouts();

    // Utility
    VOID DestroyGameList();
    VOID DestroyPlayerList();

    VOID LogXNetError( const CHAR*, INT ) const;

};




//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBoxSample xbApp;
    if( FAILED( xbApp.Create() ) )
        return;
    xbApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
: 
    CXBApplication(),
    m_xprResource (),
    m_ptMenuSel   ( NULL ),
    m_hLogFile    ( INVALID_HANDLE_VALUE ),
    m_Font        (),
    m_Help        (),
    m_State       ( STATE_MENU ),
    m_LastState   ( STATE_MENU ),
    m_CurrItem    ( 0 ),
    m_GameNames   (),
    m_Games       (),
    m_Players     (),
    m_strError    (),
    m_strStatus   (),
    m_LinkStatusTimer     ( FALSE ),
    m_GameSearchTimer     ( FALSE ),
    m_GameJoinTimer       ( FALSE ),
    m_HeartbeatTimer      ( FALSE ),
    m_bIsOnline           ( FALSE ),
    m_bXnetStarted        ( FALSE ),
    m_bIsHost             ( FALSE ),
    m_bIsSessionRegistered( FALSE ),
    m_bHaveLocalAddress   ( FALSE ),
    m_xnHostKeyID         (),
    m_xnHostKeyExchange   (),
    m_xnTitleAddress      (),
    m_inHostAddr          (),
    m_BroadSock           (),
    m_DirectSock          (),
    m_strGameName         (),
    m_strPlayerName       (),
    m_strHostName         (),
    m_Nonce               (),
    m_rand                ( GetTickCount() )
{
    Init();
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Initialize the help system
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load our textures
    if( FAILED( m_xprResource.Create( g_pd3dDevice, "Resource.xpr", 
                                      Resource_NUM_RESOURCES ) ) )
    {
        OUTPUT_DEBUG_STRING( "Failed to load textures\n" );
        return XBAPPERR_MEDIANOTFOUND;
    }

    // Set up texture ptrs
    m_ptMenuSel = m_xprResource.GetTexture( Resource_MenuSelect_OFFSET );

    // Set projection transform
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 640.0f/480.0f, 0.1f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	// Set view position
    D3DXMATRIX matView;
    D3DXMatrixTranslation( &matView, 0.0f, 0.0f, 40.0f);
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // TCR 2-16 Lost Link
    // Check network status periodically
    if( !m_LinkStatusTimer.IsRunning() ||
        m_LinkStatusTimer.GetElapsedSeconds() > CHECK_LINK_STATUS )
    {
        m_LinkStatusTimer.StartZero();
        DWORD dwStatus = XNetGetEthernetLinkStatus();
        m_bIsOnline = ( dwStatus & XNET_ETHERNET_LINK_ACTIVE ) != 0;
    }

    Event ev = GetEvent();

    // Handle lost link errors
    switch( m_State )
    {
        case STATE_MENU: break;  // main menu handles lost link differently
        case STATE_HELP: break;  // help screen doesn't need to handle
        case STATE_ERROR: break; // error screen doesn't need to handle

        default:
            if( !m_bIsOnline )
            {
                // TCR 2-16 Lost Link
                m_State = STATE_ERROR;
                lstrcpynW( m_strError, strLOST_CONNECTION, MAX_ERROR_STR );
            }
            break;
    }

    switch( m_State )
    {
        case STATE_MENU:            FrameMoveMenu( ev );        break;
        case STATE_GAME:            FrameMoveGame( ev );        break;
        case STATE_HELP:            FrameMoveHelp( ev );        break;
        case STATE_SELECT_NAME:     FrameMoveSelectName( ev );  break;
        case STATE_START_NEW_GAME:  FrameMoveStartGame( ev );   break;
        case STATE_GAME_SEARCH:     FrameMoveGameSearch( ev );  break;
        case STATE_SELECT_GAME:     FrameMoveSelectGame( ev );  break;
        case STATE_REQUEST_JOIN:    FrameMoveRequestJoin( ev ); break;
        case STATE_ERROR:           FrameMoveError( ev );       break;
    }
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
                         0x000A0A6A, 1.0f, 0L );

    switch( m_State )
    {
        case STATE_MENU:            RenderMenu();        break;
        case STATE_GAME:            RenderGame();        break;
        case STATE_HELP:            RenderHelp();        break;
        case STATE_SELECT_NAME:     RenderSelectName();  break;
        case STATE_START_NEW_GAME:  RenderStartGame();   break;
        case STATE_GAME_SEARCH:     RenderGameSearch();  break;
        case STATE_SELECT_GAME:     RenderSelectGame();  break;
        case STATE_REQUEST_JOIN:    RenderRequestJoin(); break;
        case STATE_ERROR:           RenderError();       break;
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetEvent()
// Desc: Return the state of the controller
//-----------------------------------------------------------------------------
CXBoxSample::Event CXBoxSample::GetEvent()
{
    // "A" or "Start"
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] ||
        m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_START )
    {
        return EV_BUTTON_A;
    }

    // "B"
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] )
        return EV_BUTTON_B;

    // "Back"
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        return EV_BUTTON_BACK;

    // Movement
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
        return EV_UP;
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
        return EV_DOWN;
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT )
        return EV_LEFT;
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
        return EV_RIGHT;

    return EV_NULL;
}




//-----------------------------------------------------------------------------
// Name: FrameMoveMenu()
// Desc: Animate menu
//-----------------------------------------------------------------------------
VOID CXBoxSample::FrameMoveMenu( Event ev )
{
    switch( ev )
    {
        case EV_BUTTON_A:

            // Prepare networking
            switch( InitXNet() )
            {
                case Success:
                    break;
                case NotConnected:
                    m_State = STATE_ERROR;
                    lstrcpynW( m_strError, L"This Xbox is not connected to\n"
                                           L"a hub or another Xbox",
                                           MAX_ERROR_STR );
                    return;
                case InitFailed:
                    m_State = STATE_ERROR;
                    lstrcpynW( m_strError, L"Failure initializing network\n"
                                           L"connections", 
                                           MAX_ERROR_STR );
                    return;
                default: assert( FALSE ); break;
            }

            switch( m_CurrItem )
            {
                case MAIN_MENU_START_GAME:
                {    
                    assert( !m_bIsSessionRegistered );

                    // Create the session key ID and exchange key
                    INT iKeyCreated = XNetCreateKey( &m_xnHostKeyID, 
                                                     &m_xnHostKeyExchange );

                    // Register the session
                    INT iKeyRegistered = XNetRegisterKey( &m_xnHostKeyID, 
                                                          &m_xnHostKeyExchange );
                    if( iKeyCreated != NO_ERROR || iKeyRegistered != NO_ERROR )
                    {
                        m_State = STATE_ERROR;
                        lstrcpynW( m_strError, L"Unable to start game session",
                                   MAX_ERROR_STR );

                        if( iKeyCreated != NO_ERROR )
                            LogXNetError( "XNetCreateKey", iKeyCreated );
                        if( iKeyRegistered != NO_ERROR )
                            LogXNetError( "XNetRegisterKey", iKeyRegistered );

                        break;
                    }

                    m_bIsSessionRegistered = TRUE;

                    // We're the host
                    m_bIsHost = TRUE;

                    // TCR 3-58 Naming of Game Sessions
                    // Build a list of potential game names
                    assert( m_GameNames.empty() );
                    for( DWORD i = 0; i < MAX_GAME_NAMES; ++i )
                    {
                        WCHAR strGameName[ MAX_GAME_NAME ];
                        m_rand.GetName( strGameName, MAX_GAME_NAME );
                        m_GameNames.push_back( strGameName );
                    }

                    // Start at the top of the list
                    m_CurrItem = 0;

                    m_State = STATE_SELECT_NAME;
                    break;
                }
                case MAIN_MENU_JOIN_GAME:

                    // Begin searching for games on the network
                    SendFindGame();
                    m_GameSearchTimer.StartZero();
                    m_State = STATE_GAME_SEARCH;

                    break;
            }
            break;

        case EV_UP:
            if( m_CurrItem == 0 )
                m_CurrItem = MAIN_MENU_MAX - 1;
            else
                --m_CurrItem;
            break;
        case EV_DOWN:
            if( m_CurrItem == MAIN_MENU_MAX - 1 )
                m_CurrItem = 0;
            else
                ++m_CurrItem;
            break;
        case EV_BUTTON_BACK:
            m_LastState = m_State;
            m_State = STATE_HELP;
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: FrameMoveGame()
// Desc: Animate game
//-----------------------------------------------------------------------------
VOID CXBoxSample::FrameMoveGame( Event ev )
{
    // Handle net messages
    if( ProcessBroadcastMessage() )
        return;
    if( ProcessDirectMessage() )
        return;

    // Send keep-alives
    if( m_HeartbeatTimer.GetElapsedSeconds() > PLAYER_HEARTBEAT )
    {
        Heartbeat();
        m_HeartbeatTimer.StartZero();
    }

    // Handle other players dropping
    if( ProcessPlayerDropouts() )
        return;

    switch( ev )
    {
        case EV_BUTTON_A:
            switch( m_CurrItem )
            {
                case GAME_MENU_WAVE:
                    Wave();
                    break;
                case GAME_MENU_LEAVE_GAME:
                    Init();
                    break;
            }
            break;
        case EV_UP:
            if( m_CurrItem == 0 )
                m_CurrItem = GAME_MENU_MAX - 1;
            else
                --m_CurrItem;
            break;
        case EV_DOWN:
            if( m_CurrItem == GAME_MENU_MAX - 1 )
                m_CurrItem = 0;
            else
                ++m_CurrItem;
            break;
        case EV_BUTTON_BACK:
            m_LastState = m_State;
            m_State = STATE_HELP;
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: FrameMoveHelp()
// Desc: Animate help
//-----------------------------------------------------------------------------
VOID CXBoxSample::FrameMoveHelp( Event ev )
{
    // Handle net messages
    if( ProcessBroadcastMessage() )
        return;
    if( ProcessDirectMessage() )
        return;

    if( ev != EV_NULL )
        m_State = m_LastState;
}




//-----------------------------------------------------------------------------
// Name: FrameMoveSelectName()
// Desc: Animate game name selection
//-----------------------------------------------------------------------------
VOID CXBoxSample::FrameMoveSelectName( Event ev )
{
    // TCR 3-58 Naming of Game Sessions

    // Allow the player to cancel out of game name selection
    if( ev == EV_BUTTON_B )
    {
        Init();
        return;
    }

    switch( ev )
    {
        case EV_BUTTON_A:

            // Use the selected namne
            lstrcpyW( m_strGameName, m_GameNames[ m_CurrItem ].c_str() );

            // Destroy the name list; we don't need it anymore
            m_GameNames.clear();

            // Set the default game item to "wave"
            m_CurrItem = 0;

            // Display when game begins
            lstrcpynW( m_strStatus, L"Game started", MAX_STATUS_STR );

            // If we have the local address, begin the game.
            // Otherwise, acquire the local address.
            if( m_bHaveLocalAddress )
            {
                m_State = STATE_GAME;
                m_HeartbeatTimer.StartZero();
            }
            else
                m_State = STATE_START_NEW_GAME;
            break;

        case EV_UP:
            if( m_CurrItem == 0 )
                m_CurrItem = m_GameNames.size() - 1;
            else
                --m_CurrItem;
            break;

        case EV_DOWN:
            if( m_CurrItem == m_GameNames.size() - 1 )
                m_CurrItem = 0;
            else
                ++m_CurrItem;
            break;

        case EV_BUTTON_B:
            Init();
            break;

        case EV_BUTTON_BACK:
            m_LastState = m_State;
            m_State = STATE_HELP;
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: FrameMoveStartGame()
// Desc: Animate start game
//-----------------------------------------------------------------------------
VOID CXBoxSample::FrameMoveStartGame( Event ev )
{
    // Allow the player to cancel out of game startup
    if( ev == EV_BUTTON_B )
    {
        Init();
        return;
    }

    // Asynchronous local address acquisition
    DWORD dwStatus = XNetGetTitleXnAddr( &m_xnTitleAddress );
    assert( dwStatus != XNET_GET_XNADDR_NONE );

    // If we've retrieved the local address, we're done
    m_bHaveLocalAddress = ( dwStatus != XNET_GET_XNADDR_PENDING );

    // When startup is complete, enter the game
    if( m_bHaveLocalAddress )
    {   
        m_HeartbeatTimer.StartZero();
        m_State = STATE_GAME;
    }
}




//-----------------------------------------------------------------------------
// Name: FrameMoveGameSearch()
// Desc: Animate game search
//-----------------------------------------------------------------------------
VOID CXBoxSample::FrameMoveGameSearch( Event ev )
{
    // Allow the player to cancel out of game search
    if( ev == EV_BUTTON_B )
    {
        Init();
        return;
    }

    // See if any games have replied
    if( ProcessBroadcastMessage() )
        return;

    // We search for up to GAME_SEARCH_TIME seconds. If the game 
    // search is complete, display the list of available games. If no games
    // were found, display an error message
    if( m_GameSearchTimer.GetElapsedSeconds() > GAME_SEARCH_TIME )
    {
        m_GameSearchTimer.Stop();
        if( m_Games.empty() )
        {
            m_State = STATE_ERROR;
            lstrcpynW( m_strError, L"No games available", MAX_ERROR_STR );
        }
        else if( m_Games.size() == 1 )
        {
            // TCR 3-57 Default Choices for System Link Play
            // One game; join automatically
            InitiateJoin( 0 );
        }
        else // at least two games
        {
            // at least two games; allow player selection
            m_State = STATE_SELECT_GAME;
            m_CurrItem = 0;
        }
    }
}




//-----------------------------------------------------------------------------
// Name: FrameMoveSelectGame()
// Desc: Animate game selection
//-----------------------------------------------------------------------------
VOID CXBoxSample::FrameMoveSelectGame( Event ev )
{
    switch( ev )
    {
        case EV_BUTTON_A:
            InitiateJoin( m_CurrItem );
            break;

        case EV_UP:
            if( m_CurrItem == 0 )
                m_CurrItem = m_Games.size() - 1;
            else
                --m_CurrItem;
            break;

        case EV_DOWN:
            if( m_CurrItem == m_Games.size() - 1 )
                m_CurrItem = 0;
            else
                ++m_CurrItem;
            break;

        case EV_BUTTON_B:
            Init();
            break;

        case EV_BUTTON_BACK:
            m_LastState = m_State;
            m_State = STATE_HELP;
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: FrameMoveRequestJoin()
// Desc: Animate join request
//-----------------------------------------------------------------------------
VOID CXBoxSample::FrameMoveRequestJoin( Event ev )
{
    // Allow the player to cancel out of join request
    if( ev == EV_BUTTON_B )
    {
        Init();
        return;
    }

    // See if the game has replied
    ProcessDirectMessage();

    // We wait for up to GAME_JOIN_TIME seconds. If the game didn't
    // respond, display an error message
    if( m_GameJoinTimer.GetElapsedSeconds() > GAME_JOIN_TIME )
    {
        m_GameJoinTimer.Stop();
        m_State = STATE_ERROR;
        lstrcpynW( m_strError, L"Game did not respond", MAX_ERROR_STR );
    }
}




//-----------------------------------------------------------------------------
// Name: FrameMoveError()
// Desc: Animate error message
//-----------------------------------------------------------------------------
VOID CXBoxSample::FrameMoveError( Event ev )
{
    // Handle net messages
    if( ProcessBroadcastMessage() )
        return;
    if( ProcessDirectMessage() )
        return;

    // Any button exits
    if( ev != EV_NULL )
        Init();
}





//-----------------------------------------------------------------------------
// Name: RenderMenu()
// Desc: Display menu
//-----------------------------------------------------------------------------
VOID CXBoxSample::RenderMenu()
{
    RenderHeader();

    const WCHAR* const strMenu[] =
    {
        L"Start New Game",
        L"Join Existing Game",
    };

    FLOAT fYtop = 200.0f;
    FLOAT fYdelta = 50.0f;

    // TCR 3-44 System Link Play Menu Option
    for( DWORD i = 0; i < MAIN_MENU_MAX; ++i )
    {
        DWORD dwColor = ( m_CurrItem == i && m_bIsOnline ) ? COLOR_HIGHLIGHT : 
                                                             COLOR_NORMAL;
        m_Font.DrawText( 260, fYtop + (fYdelta * i), dwColor, strMenu[i] );
    }

    // Show selected item with little triangle
    if( m_bIsOnline )
        RenderMenuSelector( 220.0f, fYtop + (fYdelta * m_CurrItem ) );

    m_Font.DrawText( 320, 400, COLOR_NORMAL, m_bIsOnline ? 
                     L"System Link Connected" :
                     L"System Link NOT Connected", XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: RenderGame()
// Desc: Display game
//-----------------------------------------------------------------------------
VOID CXBoxSample::RenderGame()
{
    RenderHeader();

    // Game name and player name
    WCHAR strGameInfo[ 32 + MAX_GAME_NAME + MAX_PLAYER_NAME ];
    wsprintfW( strGameInfo, L"Game name: %.*s\nYour name: %.*s", 
               MAX_GAME_NAME, m_strGameName, MAX_PLAYER_NAME, m_strPlayerName );
    m_Font.DrawText( 220, 120, COLOR_GREEN, strGameInfo );

    // Number of players and current status
    wsprintfW( strGameInfo, L"Players in game: %lu", m_Players.size() + 1 );
    m_Font.DrawText( 220, 168, COLOR_GREEN, strGameInfo );
    m_Font.DrawText( 320, 214, COLOR_GREEN, m_strStatus, XBFONT_CENTER_X );

    // Game options menu
    const WCHAR* const strMenu[] =
    {
        L"Wave To Other Players",
        L"Leave Game",
    };

    FLOAT fYtop = 270.0f;
    FLOAT fYdelta = 50.0f;

    // Show menu
    for( DWORD i = 0; i < GAME_MENU_MAX; ++i )
    {
        DWORD dwColor = ( m_CurrItem == i ) ? COLOR_HIGHLIGHT : COLOR_NORMAL;
        m_Font.DrawText( 260, fYtop + (fYdelta * i), dwColor, strMenu[i] );
    }

    // Show selected item with little triangle
    RenderMenuSelector( 220.0f, fYtop + (fYdelta * m_CurrItem ) );
}




//-----------------------------------------------------------------------------
// Name: RenderHelp()
// Desc: Display help
//-----------------------------------------------------------------------------
VOID CXBoxSample::RenderHelp()
{
    XBHELP_CALLOUT HelpCallouts[] =
    {
        { XBHELP_BACK_BUTTON, XBHELP_PLACEMENT_1, L"Display\nhelp" },
        { XBHELP_A_BUTTON, XBHELP_PLACEMENT_1, L"Select menu\nitem" },
        { XBHELP_B_BUTTON, XBHELP_PLACEMENT_1, L"Cancel" },
        { XBHELP_DPAD, XBHELP_PLACEMENT_1, L"Menu navigation" },
    };
    m_Help.Render( &m_Font, HelpCallouts, 4 );
}




//-----------------------------------------------------------------------------
// Name: RenderSelectName()
// Desc: Display game name selection
//-----------------------------------------------------------------------------
VOID CXBoxSample::RenderSelectName()
{
    assert( !m_GameNames.empty() );

    RenderHeader();

    m_Font.DrawText( 320, 110, 0xffffffff, L"Select a game name\n\n"
                                           L"Press B to cancel", 
                     XBFONT_CENTER_X );

    FLOAT fYtop = 220.0f;
    FLOAT fYdelta = 30.0f;

    // Show list of game names
    for( DWORD i = 0; i < m_GameNames.size(); ++i )
    {
        DWORD dwColor = ( m_CurrItem == i ) ? COLOR_HIGHLIGHT : COLOR_NORMAL;
        m_Font.DrawText( 280, fYtop + (fYdelta * i), dwColor, 
                         m_GameNames[i].c_str() );
    }

    // Show selected item with little triangle
    RenderMenuSelector( 240.0f, fYtop + (fYdelta * m_CurrItem ) );
}




//-----------------------------------------------------------------------------
// Name: RenderStartGame()
// Desc: Display game startup sequence
//-----------------------------------------------------------------------------
VOID CXBoxSample::RenderStartGame()
{
    RenderHeader();
    m_Font.DrawText( 320, 240, 0xffffffff, L"Starting Game\n\n"
                                           L"Press B to cancel",
                     XBFONT_CENTER_X | XBFONT_CENTER_Y );
}




//-----------------------------------------------------------------------------
// Name: RenderGameSearch()
// Desc: Display game search sequence
//-----------------------------------------------------------------------------
VOID CXBoxSample::RenderGameSearch()
{
    RenderHeader();
    m_Font.DrawText( 320, 240, 0xffffffff, L"Searching For Active Games\n\n"
                                           L"Press B to cancel",
                     XBFONT_CENTER_X | XBFONT_CENTER_Y );
}




//-----------------------------------------------------------------------------
// Name: RenderSelectGame()
// Desc: Display list of available games
//-----------------------------------------------------------------------------
VOID CXBoxSample::RenderSelectGame()
{
    assert( !m_Games.empty() );

    RenderHeader();

    m_Font.DrawText( 320, 110, 0xffffffff, L"Select game to join\n\n"
                                           L"Press B to cancel", 
                     XBFONT_CENTER_X );

    FLOAT fYtop = 220.0f;
    FLOAT fYdelta = 50.0f;

    // Show list of games
    for( DWORD i = 0; i < m_Games.size(); ++i )
    {
        DWORD dwColor = ( m_CurrItem == i ) ? COLOR_HIGHLIGHT : COLOR_NORMAL;
        WCHAR strGameInfo[ 64 + MAX_GAME_NAME + MAX_PLAYER_NAME ];
        wsprintfW( strGameInfo, L"\"%.*s\" hosted by \"%.*s\"; players: %d",
                   MAX_GAME_NAME, m_Games[i].strGameName,
                   MAX_PLAYER_NAME, m_Games[i].strHostName,
                   INT(m_Games[i].byNumPlayers) );

        // Denote full games
        if( m_Games[i].byNumPlayers == MAX_PLAYERS )
            lstrcatW( strGameInfo, L" (full)" );

        m_Font.DrawText( 140, fYtop + (fYdelta * i), dwColor, strGameInfo );
    }

    // Show selected item with little triangle
    RenderMenuSelector( 100.0f, fYtop + (fYdelta * m_CurrItem ) );
}




//-----------------------------------------------------------------------------
// Name: RenderRequestJoin()
// Desc: Display join request sequence
//-----------------------------------------------------------------------------
VOID CXBoxSample::RenderRequestJoin()
{
    RenderHeader();
    m_Font.DrawText( 320, 240, 0xffffffff, L"Joining game", 
                     XBFONT_CENTER_X | XBFONT_CENTER_Y );
}




//-----------------------------------------------------------------------------
// Name: RenderError()
// Desc: Display error message
//-----------------------------------------------------------------------------
VOID CXBoxSample::RenderError()
{
    RenderHeader();
    m_Font.DrawText( 320, 200, 0xffffffff, m_strError, XBFONT_CENTER_X );
    m_Font.DrawText( 320, 260, 0xffffffff, L"Press A to continue", 
                     XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: RenderHeader()
// Desc: Display standard text
//-----------------------------------------------------------------------------
VOID CXBoxSample::RenderHeader()
{
    WCHAR strName[32];
    lstrcpyW( strName, L"WinsockPeer" );
    if( m_bIsHost )
        lstrcatW( strName, L" (host)" );

    m_Font.DrawText(  64, 50, 0xffffffff, strName );
    m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
}




//-----------------------------------------------------------------------------
// Name: RenderMenuSelector()
// Desc: Display menu selector
//-----------------------------------------------------------------------------
VOID CXBoxSample::RenderMenuSelector( FLOAT fLeft, FLOAT fTop )
{
    D3DXVECTOR4 rc( fLeft, fTop, fLeft + 20.0f, fTop + 20.0f );

    // Show selected item
    struct TILEVERTEX
    {
        D3DXVECTOR4 p;
        D3DXVECTOR2 t;
    };
    TILEVERTEX* pVertices;

    LPDIRECT3DVERTEXBUFFER8 pvbTemp;
    g_pd3dDevice->CreateVertexBuffer( 4 * sizeof( TILEVERTEX ), 
                                      D3DUSAGE_WRITEONLY, 
                                      D3DFVF_XYZRHW | D3DFVF_TEX1, 
                                      D3DPOOL_MANAGED, &pvbTemp );

    // Create a quad for us to render our texture on
    pvbTemp->Lock( 0, 0, (BYTE **)&pVertices, 0L );
    pVertices[0].p = D3DXVECTOR4( rc.x - 0.5f, rc.w - 0.5f, 1.0f, 1.0f );  pVertices[0].t = D3DXVECTOR2( 0.0f, 1.0f ); // Bottom Left
    pVertices[1].p = D3DXVECTOR4( rc.x - 0.5f, rc.y - 0.5f, 1.0f, 1.0f );  pVertices[1].t = D3DXVECTOR2( 0.0f, 0.0f ); // Top    Left
    pVertices[2].p = D3DXVECTOR4( rc.z - 0.5f, rc.w - 0.5f, 1.0f, 1.0f );  pVertices[2].t = D3DXVECTOR2( 1.0f, 1.0f ); // Bottom Right
    pVertices[3].p = D3DXVECTOR4( rc.z - 0.5f, rc.y - 0.5f, 1.0f, 1.0f );  pVertices[3].t = D3DXVECTOR2( 1.0f, 0.0f ); // Top    Right
    pvbTemp->Unlock();

    // Set up our state
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetStreamSource( 0, pvbTemp, sizeof( TILEVERTEX ) );

    // Render the quad with our texture
    g_pd3dDevice->SetTexture( 0, m_ptMenuSel );
    g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    g_pd3dDevice->SetTexture( 0, NULL );
    pvbTemp->Release();
}




//-----------------------------------------------------------------------------
// Name: InitiateJoin()
// Desc: Send a join request to the specified game
//-----------------------------------------------------------------------------
VOID CXBoxSample::InitiateJoin( DWORD iCurrGame )
{
    // Determine which game the player wants to join
    GameInfo gameInfo = m_Games[ iCurrGame ];

    // Establish a session with the host game
    INT iResult = XNetRegisterKey( &gameInfo.xnHostKeyID, 
                                   &gameInfo.xnHostKey );
    assert( iResult == NO_ERROR );
    if( iResult == NO_ERROR )
    {
        assert( m_bIsSessionRegistered == FALSE );
        m_bIsSessionRegistered = TRUE;

        // Save the key ID because we need to unregister it
        // Note that we don't need the key itself once it's been registered.
        CopyMemory( &m_xnHostKeyID, &gameInfo.xnHostKeyID, sizeof( XNKID ) );

        // Save the game and player name of the host
        lstrcpynW( m_strGameName, gameInfo.strGameName, MAX_GAME_NAME );
        lstrcpynW( m_strHostName, gameInfo.strHostName, MAX_PLAYER_NAME );

        // Convert the XNADDR of the host to the INADDR we'll use to
        // join the game
        iResult = XNetXnAddrToInAddr( &gameInfo.xnHostAddr,
                                      &m_xnHostKeyID, &m_inHostAddr );
        assert( iResult == NO_ERROR );

        // Request join approval from the game and await a response
        SendJoinGame( CXBSockAddr( m_inHostAddr, DIRECT_PORT ) );
        m_GameJoinTimer.StartZero();
        m_State = STATE_REQUEST_JOIN;
    }
    else
    {
        m_State = STATE_ERROR;
        lstrcpynW( m_strError, L"Unable to establish session with game",
                   MAX_ERROR_STR );
        LogXNetError( "XNetRegisterKey", iResult );
    }

    // Don't need the game list anymore
    DestroyGameList();
}




//-----------------------------------------------------------------------------
// Name: Wave()
// Desc: Wave to other players in the game
//-----------------------------------------------------------------------------
VOID CXBoxSample::Wave()
{
    // Indicate that you waved
    lstrcpynW( m_strStatus, L"You waved", MAX_STATUS_STR );

    // Send a "wave" message to each of the other players in the game
    for( PlayerList::iterator i = m_Players.begin(); i != m_Players.end(); ++i )
        SendWave( CXBSockAddr( i->inAddr, DIRECT_PORT ) );
}




//-----------------------------------------------------------------------------
// Name: Heartbeat()
// Desc: Send heartbeat to players in the game
//-----------------------------------------------------------------------------
VOID CXBoxSample::Heartbeat()
{
    // Send a "heartbeat" message to each of the other players in the game
    // to let them know we're alive
    for( PlayerList::iterator i = m_Players.begin(); i != m_Players.end(); ++i )
        SendHeartbeat( CXBSockAddr( i->inAddr, DIRECT_PORT ) );
}




//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Teardown any active games and player lists and return to main menu
//       Unregisters any active sessions.
//-----------------------------------------------------------------------------
VOID CXBoxSample::Init()
{
    // Don't clear m_bXnetStarted. We don't need to reinitialize Xnet once
    // it's been started.

    // Don't clear m_bHaveLocalAddress or m_xnTitleAddress. We don't need to 
    // reacquire the address once we have it.

    m_State     = STATE_MENU;
    m_LastState = STATE_MENU;
    m_CurrItem  = 0;

    m_GameNames.clear();
    DestroyGameList();
    DestroyPlayerList();

    *m_strError = 0;
    *m_strStatus = 0;

    m_LinkStatusTimer.Stop();
    m_GameSearchTimer.Stop();
    m_GameJoinTimer.Stop();
    m_HeartbeatTimer.Stop();

    m_bIsHost = FALSE;

    // Unregister the game session key
    if( m_bIsSessionRegistered )
    {
        INT iResult = XNetUnregisterKey( &m_xnHostKeyID );
        assert( iResult == NO_ERROR );
        (VOID)iResult;
        m_bIsSessionRegistered = FALSE;
    }

    // Obliterate old keys and XNADDR
    ZeroMemory( &m_xnHostKeyID,       sizeof( XNKID ) );
    ZeroMemory( &m_xnHostKeyExchange, sizeof( XNKEY ) );
    m_inHostAddr.s_addr = 0;

    // Close down the sockets
    m_BroadSock.Close();
    m_DirectSock.Close();

    *m_strGameName   = 0;
    *m_strPlayerName = 0;
    *m_strHostName   = 0;

    ZeroMemory( &m_Nonce, sizeof(m_Nonce) );

    // Generate random player name
    // A real Xbox game would get this information from the player
    m_rand.GetName( m_strPlayerName, MAX_PLAYER_NAME );
}




//-----------------------------------------------------------------------------
// Name: InitXNet()
// Desc: Initialize the network stack. Returns FALSE if Xbox is not connected.
//-----------------------------------------------------------------------------
CXBoxSample::InitStatus CXBoxSample::InitXNet()
{
    if( !m_bIsOnline )
        return NotConnected;

    // Only need to initialize network stack one time
    if( !m_bXnetStarted )
    {
        // XBNet_Init in XbNet.cpp handles standard Xbox socket startup
        HRESULT hr = XBNet_Init( 0 );
        if( FAILED(hr) )
        {
            LogXNetError( "XNetStartup/WSAStartup", hr );
            return InitFailed;
        }
        m_bXnetStarted = TRUE;
    }

    // The broadcast socket is a non-blocking socket on port BROADCAST_PORT.
    // All broadcast messages are automatically always encrypted.
    BOOL bSuccess = m_BroadSock.Open( CXBSocket::Type_UDP );
    if( !bSuccess )
    {
        LogXNetError( "Broadcast socket open", WSAGetLastError() );
        return InitFailed;
    }

    CXBSockAddr broadAddr( INADDR_ANY, BROADCAST_PORT );
    INT iResult = m_BroadSock.Bind( broadAddr.GetPtr() );
    assert( iResult != SOCKET_ERROR );
    DWORD dwNonBlocking = 1;
    iResult = m_BroadSock.IoCtlSocket( FIONBIO, &dwNonBlocking );
    assert( iResult != SOCKET_ERROR );
    BOOL bBroadcast = TRUE;
    iResult = m_BroadSock.SetSockOpt( SOL_SOCKET, SO_BROADCAST,
                                      &bBroadcast, sizeof(bBroadcast) );
    assert( iResult != SOCKET_ERROR );

    // The direct socket is a non-blocking socket on port DIRECT_PORT.
    // Sockets are encrypted by default, but can have encryption disabled
    // as an optimization for non-secure messaging
    bSuccess = m_DirectSock.Open( CXBSocket::Type_UDP );
    if( !bSuccess )
    {
        LogXNetError( "Direct socket open", WSAGetLastError() );
        return InitFailed;
    }

    CXBSockAddr directAddr( INADDR_ANY, DIRECT_PORT );
    iResult = m_DirectSock.Bind( directAddr.GetPtr() );
    assert( iResult != SOCKET_ERROR );
    iResult = m_DirectSock.IoCtlSocket( FIONBIO, &dwNonBlocking );
    assert( iResult != SOCKET_ERROR );

    // Note that this sample does not call either WSACleanup() or 
    // XNetCleanup(). These functions should be called by your game to
    // free system resources when the player is no longer online but
    // is still playing the game (e.g. switched to single-player mode).

    return Success;
}




//-----------------------------------------------------------------------------
// Name: SendFindGame()
// Desc: Broadcast a MSG_FIND_GAME from our client to any available host
//-----------------------------------------------------------------------------
VOID CXBoxSample::SendFindGame()
{
    assert( !m_bIsHost );
    Message msgFindGame( MSG_FIND_GAME );
    MsgFindGame& msg = msgFindGame.GetFindGame();

    // Generate a nonce (random bytes). When a potential host responds with
    // information about a game, he must respond via a broadcast message
    // since a secure session hasn't been established. The broadcast message
    // will contain the same nonce so we can verify that message is really
    // for us. If we receive a broadcast "found game" message with a different
    // nonce, we ignore it, because it was broadcast to a different client
    // than us.
    INT iResult = XNetRandom( (BYTE*)(&msg.nonce), sizeof(msg.nonce) );
    assert( iResult == NO_ERROR );
    (VOID)iResult;

    // Save the nonce for comparison later
    CopyMemory( &m_Nonce, &msg.nonce, sizeof(msg.nonce) );

    CXBSockAddr saBroad( INADDR_BROADCAST, BROADCAST_PORT );
    INT nBytes = m_BroadSock.SendTo( &msgFindGame, msgFindGame.GetSize(),
                                     saBroad.GetPtr() );
    assert( nBytes == msgFindGame.GetSize() );
    (VOID)nBytes;
}




//-----------------------------------------------------------------------------
// Name: SendGameFound()
// Desc: Broadcast a MSG_GAME_FOUND from our host to the world
//-----------------------------------------------------------------------------
VOID CXBoxSample::SendGameFound( const Nonce& nonceClient )
{
    assert( m_bIsHost );
    Message msgGameFound( MSG_GAME_FOUND );
    MsgGameFound& msg = msgGameFound.GetGameFound();

    // Resend the nonce that we received from the client so he can verify
    // that this message is really for him
    CopyMemory( &msg.nonce, &nonceClient, sizeof(nonceClient) );

    // Send information about the session that we're hosting
    CopyMemory( &msg.xnHostKeyID, &m_xnHostKeyID,       sizeof(XNKID) );
    CopyMemory( &msg.xnHostKey,   &m_xnHostKeyExchange, sizeof(XNKEY) );
    CopyMemory( &msg.xnHostAddr,  &m_xnTitleAddress,    sizeof(XNADDR) );

    // Send the current information about the game
    msg.byNumPlayers = BYTE( m_Players.size() + 1 );
    lstrcpynW( msg.strGameName, m_strGameName, MAX_GAME_NAME );
    lstrcpynW( msg.strHostName, m_strPlayerName, MAX_PLAYER_NAME );

    // We don't have the XNADDR of the requesting client, so we
    // can't send this message directly back. Instead, we broadcast the
    // message to everybody on the net. The requesting client can
    // check the nonce to verify that the response is really for them.
    // Broadcast messages are automatically encrypted.

    CXBSockAddr saBroad( INADDR_BROADCAST, BROADCAST_PORT );
    INT nBytes = m_BroadSock.SendTo( &msgGameFound, msgGameFound.GetSize(),
                                     saBroad.GetPtr() );
    assert( nBytes == msgGameFound.GetSize() );
    (VOID)nBytes;
}




//-----------------------------------------------------------------------------
// Name: SendJoinGame()
// Desc: Issue a MSG_JOIN_GAME from our client to the game host
//-----------------------------------------------------------------------------
VOID CXBoxSample::SendJoinGame( const CXBSockAddr& saGameHost )
{
    assert( !m_bIsHost );
    Message msgJoinGame( MSG_JOIN_GAME );
    MsgJoinGame& msg = msgJoinGame.GetJoinGame();

    // Include our player name
    lstrcpynW( msg.strPlayerName, m_strPlayerName, MAX_PLAYER_NAME );

    // We can send this message directly to the host
    INT nBytes = m_DirectSock.SendTo( &msgJoinGame, msgJoinGame.GetSize(),
                                      saGameHost.GetPtr() );
    assert( nBytes == msgJoinGame.GetSize() );
    (VOID)nBytes;
}




//-----------------------------------------------------------------------------
// Name: SendJoinApproved()
// Desc: Issue a MSG_JOIN_APPROVED from our host to the requesting client.
//-----------------------------------------------------------------------------
VOID CXBoxSample::SendJoinApproved( const CXBSockAddr& saClient )
{
    assert( m_bIsHost );
    Message msgJoinApproved( MSG_JOIN_APPROVED );
    MsgJoinApproved& msg = msgJoinApproved.GetJoinApproved();

    // The host is us
    lstrcpynW( msg.strHostName, m_strPlayerName, MAX_PLAYER_NAME );

    // Send the list of all the current players to the new player.
    // We don't send the host player info, since the new player 
    // already has all of the information it needs about the host player.
    msg.byNumPlayers = BYTE( m_Players.size() );
    BYTE j = 0;
    for( PlayerList::const_iterator i = m_Players.begin(); 
         i != m_Players.end(); ++i, ++j )
    {
        PlayerInfo playerInfo = *i;
        CopyMemory( &msg.PlayerList[j].xnAddr, &playerInfo.xnAddr, 
                    sizeof( XNADDR ) );
        lstrcpynW( msg.PlayerList[j].strPlayerName, 
                   playerInfo.strPlayerName, MAX_PLAYER_NAME );
    }

    // We can send this message directly back to the requesting client
    INT nBytes = m_DirectSock.SendTo( &msgJoinApproved, msgJoinApproved.GetSize(),
                                      saClient.GetPtr() );
    assert( nBytes == msgJoinApproved.GetSize() );
    (VOID)nBytes;
}




//-----------------------------------------------------------------------------
// Name: SendJoinDenied()
// Desc: Issue a MSG_JOIN_DENIED from our host to the requesting client
//-----------------------------------------------------------------------------
VOID CXBoxSample::SendJoinDenied( const CXBSockAddr& saClient )
{
    assert( m_bIsHost );
    Message msgJoinDenied( MSG_JOIN_DENIED );

    // We can send this message directly back to the requesting client
    INT nBytes = m_DirectSock.SendTo( &msgJoinDenied, msgJoinDenied.GetSize(),
                                      saClient.GetPtr() );
    assert( nBytes == msgJoinDenied.GetSize() );
    (VOID)nBytes;
}




//-----------------------------------------------------------------------------
// Name: SendPlayerJoined()
// Desc: Issue a MSG_PLAYER_JOINED from our host to a player in the game
//-----------------------------------------------------------------------------
VOID CXBoxSample::SendPlayerJoined( const Player& player, const CXBSockAddr& saPlayer )
{
    assert( m_bIsHost );
    Message msgPlayerJoined( MSG_PLAYER_JOINED );
    MsgPlayerJoined& msg = msgPlayerJoined.GetPlayerJoined();

    // The payload is the information about the player who just joined
    CopyMemory( &msg.player, &player, sizeof(player) );

    // We send this message directly to the player
    INT nBytes = m_DirectSock.SendTo( &msgPlayerJoined, msgPlayerJoined.GetSize(),
                                      saPlayer.GetPtr() );
    assert( nBytes == msgPlayerJoined.GetSize() );
    (VOID)nBytes;
}




//-----------------------------------------------------------------------------
// Name: SendWave()
// Desc: Issue a MSG_WAVE from ourself (either a host or player) to another
//       player
//-----------------------------------------------------------------------------
VOID CXBoxSample::SendWave( const CXBSockAddr& saPlayer )
{
    Message msgWave( MSG_WAVE );
    INT nBytes = m_DirectSock.SendTo( &msgWave, msgWave.GetSize(), 
                                      saPlayer.GetPtr() );
    assert( nBytes == msgWave.GetSize() );
    (VOID)nBytes;
}




//-----------------------------------------------------------------------------
// Name: SendHeartbeat()
// Desc: Issue a MSG_HEARTBEAT from ourself (either a host or player) to
//       another player
//-----------------------------------------------------------------------------
VOID CXBoxSample::SendHeartbeat( const CXBSockAddr& saPlayer )
{
    // Send the heartbeat
    Message msgHeartbeat( MSG_HEARTBEAT );
    INT nBytes = m_DirectSock.SendTo( &msgHeartbeat, msgHeartbeat.GetSize(),
                                      saPlayer.GetPtr() );
    assert( nBytes == msgHeartbeat.GetSize() );
    (VOID)nBytes;
}




//-----------------------------------------------------------------------------
// Name: ProcessBroadcastMessage()
// Desc: Checks to see if any broadcast messages are waiting on the broadcast
//       socket. If a message is waiting, it is routed and processed.
//       If no messages are waiting, the function returns immediately.
//       Returns TRUE if a message was processed.
//-----------------------------------------------------------------------------
BOOL CXBoxSample::ProcessBroadcastMessage()
{
    if( !m_BroadSock.IsOpen() )
        return FALSE;

    // See if a network broadcast message is waiting for us
    Message msg;
    INT iResult = m_BroadSock.Recv( &msg, msg.GetMaxSize() );

    // If message waiting, process it
    if( iResult != SOCKET_ERROR && iResult > 0 )
    {
        assert( iResult == msg.GetSize() );
        ProcessMessage( msg );
        return TRUE;
    }
    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: ProcessDirectMessage()
// Desc: Checks to see if any direct messages are waiting on the direct socket.
//       If a message is waiting, it is routed and processed.
//       If no messages are waiting, the function returns immediately.
//       Returns TRUE if a message was processed.
//-----------------------------------------------------------------------------
BOOL CXBoxSample::ProcessDirectMessage()
{
    if( !m_DirectSock.IsOpen() )
        return FALSE;

    // See if a network message is waiting for us
    Message msg;
    SOCKADDR_IN saFromIn;
    INT iResult = m_DirectSock.RecvFrom( &msg, msg.GetMaxSize(), &saFromIn );
    CXBSockAddr saFrom( saFromIn );

    // If message waiting, process it
    if( iResult != SOCKET_ERROR && iResult > 0 )
    {
        assert( iResult == msg.GetSize() );
        ProcessMessage( msg, saFrom );
        return TRUE;
    }
    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: ProcessMessage()
// Desc: Routes broadcast messages
//-----------------------------------------------------------------------------
VOID CXBoxSample::ProcessMessage( Message& msg )
{
    // Process the message
    switch( msg.GetId() )
    {
        // From client to host; processed by host
        case MSG_FIND_GAME:  ProcessFindGame( msg.GetFindGame() );   break;

        // From host to client: processed by client
        case MSG_GAME_FOUND: ProcessGameFound( msg.GetGameFound() ); break;

        // Any other message on this port is invalid and we ignore it
        default: assert( FALSE ); break;
    }
}




//-----------------------------------------------------------------------------
// Name: ProcessMessage()
// Desc: Routes any direct messages
//-----------------------------------------------------------------------------
VOID CXBoxSample::ProcessMessage( Message& msg, const CXBSockAddr& saFrom )
{
    // Process the message
    switch( msg.GetId() )
    {
        // From client to host; processed by host
        case MSG_JOIN_GAME:     ProcessJoinGame( msg.GetJoinGame(), saFrom ); break;

        // From host to client: processed by client
        case MSG_JOIN_APPROVED: ProcessJoinApproved( msg.GetJoinApproved(), saFrom ); break;
        case MSG_JOIN_DENIED:   ProcessJoinDenied( saFrom ); break;
        case MSG_PLAYER_JOINED: ProcessPlayerJoined( msg.GetPlayerJoined(), saFrom ); break;

        // From player to player: processed by client player
        case MSG_WAVE:          ProcessWave( saFrom ); break;
        case MSG_HEARTBEAT:     ProcessHeartbeat( saFrom ); break;

        // Any other message on this port is invalid and we ignore it
        default: assert( FALSE ); break;
    }
}




//-----------------------------------------------------------------------------
// Name: ProcessFindGame()
// Desc: Process the find game message
//-----------------------------------------------------------------------------
VOID CXBoxSample::ProcessFindGame( const MsgFindGame& findGame )
{
    // If we're not hosting a game, we don't care about receiving "find game"
    // messages. Only hosts respond to "find game" messages
    if( !m_bIsHost )
        return;

    // We're hosting a game
    // Respond with the game information
    SendGameFound( findGame.nonce );
}




//-----------------------------------------------------------------------------
// Name: ProcessGameFound()
// Desc: Process the game found message
//-----------------------------------------------------------------------------
VOID CXBoxSample::ProcessGameFound( const MsgGameFound& gameFound )
{
    // If we're hosting, we don't care about receiving "game found" messages.
    // Only potential clients care about "game found" messages.
    if( m_bIsHost )
        return;

    // If we didn't send the corresponding "find game" message, we don't
    // care about this particular "game found" message
    if( memcmp( &gameFound.nonce, &m_Nonce, NONCE_BYTES ) != 0 )
        return;

    // We found a game!
    // Add it to our list of potential games
    GameInfo gameInfo;
    CopyMemory( &gameInfo.xnHostKeyID, &gameFound.xnHostKeyID, sizeof( XNKID ) );
    CopyMemory( &gameInfo.xnHostKey,   &gameFound.xnHostKey,   sizeof( XNKEY ) );
    CopyMemory( &gameInfo.xnHostAddr,  &gameFound.xnHostAddr,  sizeof( XNADDR ) );
    gameInfo.byNumPlayers = gameFound.byNumPlayers;
    lstrcpynW( gameInfo.strGameName, gameFound.strGameName, MAX_GAME_NAME );
    lstrcpynW( gameInfo.strHostName, gameFound.strHostName, MAX_PLAYER_NAME );

    m_Games.push_back( gameInfo );
}




//-----------------------------------------------------------------------------
// Name: ProcessJoinGame()
// Desc: Process the join game message
//-----------------------------------------------------------------------------
VOID CXBoxSample::ProcessJoinGame( const MsgJoinGame& joinGame,
                                   const CXBSockAddr& saFrom )
{
    // Only hosts should receive "join game" messages
    assert( m_bIsHost );

    // If for some reason we receive a "join game" message and we're not a
    // host, ignore it. Only hosts respond to "join game" messages
    if( !m_bIsHost )
        return;

    // We're hosting

    // A session exists between us (the host) and the client. We can now
    // convert the incoming IP address (saFrom) into a valid XNADDR.
    XNADDR xnAddrClient;
    INT iResult = XNetInAddrToXnAddr( saFrom.GetInAddr(), &xnAddrClient, NULL );
    if( iResult == SOCKET_ERROR )
    {
        // If the client INADDR can't be converted to an XNADDR, then
        // this client does not have a valid session established, and
        // we ignore the message.
        LogXNetError( "XNetInAddrToXnAddr", iResult );
        assert( FALSE );
        return;
    }

    // A player may join if we haven't reached the player limit.
    // In a real game, you would need to "lock" the game during a join
    // or track the number of joins in progress so that if multiple
    // players were attempting to join at the same time, they wouldn't
    // all be granted access and then exceed the player maximum.
    if( m_Players.size() + 1 < MAX_PLAYERS )
    {
        SendJoinApproved( saFrom );

        // Notify the other players about the new guy
        Player player;
        CopyMemory( &player.xnAddr, &xnAddrClient, sizeof( XNADDR ) );
        lstrcpynW( player.strPlayerName, joinGame.strPlayerName, MAX_PLAYER_NAME );
        for( PlayerList::iterator i = m_Players.begin(); i != m_Players.end(); ++i )
            SendPlayerJoined( player, CXBSockAddr( i->inAddr, DIRECT_PORT ) );

        // Add this new player to our player list
        PlayerInfo playerInfo;
        CopyMemory( &playerInfo.xnAddr, &xnAddrClient, sizeof( XNADDR ) );
        playerInfo.inAddr = saFrom.GetInAddr();
        lstrcpynW( playerInfo.strPlayerName, joinGame.strPlayerName, 
                   MAX_PLAYER_NAME );
        playerInfo.dwLastHeartbeat = GetTickCount();

        m_Players.push_back( playerInfo );

        // Update status
        wsprintfW( m_strStatus, L"%.*s has joined the game", 
                   MAX_PLAYER_NAME, player.strPlayerName );
    }
    else
    {
        SendJoinDenied( saFrom );
    }
}




//-----------------------------------------------------------------------------
// Name: ProcessJoinApproved()
// Desc: Process the join approved message
//-----------------------------------------------------------------------------
VOID CXBoxSample::ProcessJoinApproved( const MsgJoinApproved& joinApproved, 
                                       const CXBSockAddr& saFrom )
{
    // Only clients should receive "join approved" messages
    assert( !m_bIsHost );

    // If for some reason we receive a "join approved" message and we're hosting
    // a game, ignore the message. Only clients handle this message
    if( m_bIsHost )
        return;

    // Add the host to the list of players (always at position 0 )
    PlayerInfo hostInfo;

    // Client doesn't need the XNADDR of the host, so we just leave it zero.
    // This data member is only used by hosts.
    ZeroMemory( &hostInfo.xnAddr, sizeof( XNADDR ) );
    hostInfo.inAddr = saFrom.GetInAddr();
    lstrcpynW( hostInfo.strPlayerName, joinApproved.strHostName, MAX_PLAYER_NAME );
    hostInfo.dwLastHeartbeat = GetTickCount();

    assert( m_Players.empty() );
    m_Players.push_back( hostInfo );

    // Build the list of the other players
    for( BYTE i = 0; i < joinApproved.byNumPlayers; ++i )
    {
        PlayerInfo playerInfo;

        // Convert the XNADDR of the player to the INADDR we'll use to wave
        // to the player
        INT iResult = XNetXnAddrToInAddr( &joinApproved.PlayerList[ i ].xnAddr,
                                          &m_xnHostKeyID, &playerInfo.inAddr );
        if( iResult == SOCKET_ERROR )
        {
            // If the client XNADDR can't be converted to an INADDR, then
            // the client does not have a valid session established, and
            // we ignore that client.
            assert( FALSE );
            LogXNetError( "XNetXnAddrToInAddr", iResult );
            continue;
        }

        // Client doesn't need the XNADDR of the host anymore, 
        // so we just leave it zero. This data member is only used by hosts.
        ZeroMemory( &playerInfo.xnAddr, sizeof( XNADDR ) );

        // Save the player name
        lstrcpynW( playerInfo.strPlayerName, 
                   joinApproved.PlayerList[ i ].strPlayerName, MAX_PLAYER_NAME );

        // Last heartbeat
        playerInfo.dwLastHeartbeat = GetTickCount();

        m_Players.push_back( playerInfo );
    }

    // Enter into the game UI
    m_State = STATE_GAME;

    // Set the default game item to "wave"
    m_CurrItem = 0;

    lstrcpynW( m_strStatus, L"You have joined the game", MAX_STATUS_STR );
    m_HeartbeatTimer.StartZero();
}




//-----------------------------------------------------------------------------
// Name: ProcessJoinDenied()
// Desc: Process the join denied message
//-----------------------------------------------------------------------------
VOID CXBoxSample::ProcessJoinDenied( const CXBSockAddr& )
{
    // Only clients should receive "join denied" messages
    assert( !m_bIsHost );

    // If for some reason we receive a "join denied" message and we're hosting
    // a game, ignore the message. Only clients handle this message
    if( m_bIsHost )
        return;

    // Only clients who are not currently playing should receive this message
    assert( m_State != STATE_GAME );

    // If for some reason we receive a "join denied" message and we're
    // already playing a game, ignore the message.
    if( m_State == STATE_GAME )
        return;

    // The game we wanted to join is full. Display error
    m_State = STATE_ERROR;
    lstrcpynW( m_strError, L"The game is full.\nChoose another game.",
               MAX_ERROR_STR );
}




//-----------------------------------------------------------------------------
// Name: ProcessPlayerJoined()
// Desc: Process the player joined message
//-----------------------------------------------------------------------------
VOID CXBoxSample::ProcessPlayerJoined( const MsgPlayerJoined& playerJoined, 
                                       const CXBSockAddr& saFrom )
{
    // saFrom is the address of the host that sent this message, but we
    // we already have his address, so throw it away
    (VOID)saFrom;

    const Player& player = playerJoined.player;
    PlayerInfo playerInfo;

    // Convert the XNADDR of the player to the INADDR we'll use to wave
    // to the player
    INT iResult = XNetXnAddrToInAddr( &player.xnAddr, &m_xnHostKeyID,
                                      &playerInfo.inAddr );
    if( iResult == SOCKET_ERROR )
    {
        // If the client XNADDR can't be converted to an INADDR, then
        // this client does not have a valid session established, and
        // we ignore the message.
        LogXNetError( "XNetXnAddrToInAddr", iResult );
        assert( FALSE );
        return;
    }

    // Client doesn't need the XNADDR of the host anymore, 
    // so we just leave it zero. This data member is only used by hosts.
    ZeroMemory( &playerInfo.xnAddr, sizeof( XNADDR ) );

    // Save the player name
    lstrcpynW( playerInfo.strPlayerName, player.strPlayerName, MAX_PLAYER_NAME );

    // Last heartbeat
    playerInfo.dwLastHeartbeat = GetTickCount();

    // Add the new player to our list
    m_Players.push_back( playerInfo );

    // Update status
    wsprintfW( m_strStatus, L"%.*s has joined the game", 
               MAX_PLAYER_NAME, player.strPlayerName );
}




//-----------------------------------------------------------------------------
// Name: ProcessWave()
// Desc: Process the wave message
//-----------------------------------------------------------------------------
VOID CXBoxSample::ProcessWave( const CXBSockAddr& saFrom )
{
    MatchInAddr matchInAddr( saFrom );

    // Find out who waved by matching the INADDR
    PlayerList::iterator i = std::find_if( m_Players.begin(), m_Players.end(), 
                                           matchInAddr );

    // We expect that we know about the player
    assert( i != m_Players.end() );

    // Update status
    if( i != m_Players.end() )
    {
        wsprintfW( m_strStatus, L"%.*s waved", 
                   MAX_PLAYER_NAME, i->strPlayerName );
    }
}




//-----------------------------------------------------------------------------
// Name: ProcessHeartbeat()
// Desc: Process the heartbeat message
//-----------------------------------------------------------------------------
VOID CXBoxSample::ProcessHeartbeat( const CXBSockAddr& saFrom )
{
    MatchInAddr matchInAddr( saFrom );

    // Find out who sent a heartbeat by matching the INADDR
    PlayerList::iterator i = std::find_if( m_Players.begin(), m_Players.end(), 
                                           matchInAddr );

    // We expect that we know about the player
    assert( i != m_Players.end() );

    // Update that player's heartbeat time
    if( i != m_Players.end() )
        i->dwLastHeartbeat = GetTickCount();
}




//-----------------------------------------------------------------------------
// Name: ProcessPlayersDropouts()
// Desc: Process players and determine if anybody has left the game
//-----------------------------------------------------------------------------
BOOL CXBoxSample::ProcessPlayerDropouts()
{
    DWORD dwTickCount = GetTickCount();
    for( PlayerList::iterator i = m_Players.begin(); i != m_Players.end(); ++i )
    {
        PlayerInfo playerInfo = *i;
        DWORD dwElapsed = dwTickCount - playerInfo.dwLastHeartbeat;
        if( dwElapsed > PLAYER_TIMEOUT )
        {
            // This player hasn't sent a heartbeat message in a long time.
            // Assume they left the game.

            if( !m_bIsHost && i == m_Players.begin() )
            {
                wsprintfW( m_strStatus, L"Host %.*s left game. "
                                        L"Game closed to new players.",
                           MAX_PLAYER_NAME, playerInfo.strPlayerName );
            }
            else
            {
                wsprintfW( m_strStatus, L"%.*s left the game", 
                           MAX_PLAYER_NAME, playerInfo.strPlayerName );
            }

            // Delete the player from the list
            m_Players.erase( i );
            return TRUE;
        }
    }
    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: DestroyGameList()
// Desc: Clear the list of games
//-----------------------------------------------------------------------------
VOID CXBoxSample::DestroyGameList()
{
    // Physically clear the list of games to obliterate the key and XNADDR info
    // from prying eyes. This particular method works because m_Games 
    // is a vector; if m_Games is not a vector each game must be 
    // cleared individually
    if( !m_Games.empty() )
    {
        GameInfo* pGameList = &m_Games[0];
        ZeroMemory( pGameList, m_Games.size() * sizeof( GameInfo ) );

        // Destroy the list of games
        m_Games.clear();
    }
}




//-----------------------------------------------------------------------------
// Name: DestroyPlayerList()
// Desc: Clear the list of players
//-----------------------------------------------------------------------------
VOID CXBoxSample::DestroyPlayerList()
{
    // Physically clear the list of players to obliterate the XNADDR info
    // from prying eyes. This particular method works because m_Players
    // is a vector; if m_Players is not a vector each player must be 
    // cleared individually
    if( !m_Players.empty() )
    {
        PlayerInfo* pPlayerList = &m_Players[0];
        ZeroMemory( pPlayerList, m_Players.size() * sizeof( PlayerInfo ) );

        // Destroy the list of players
        m_Players.clear();
    }
}




//-----------------------------------------------------------------------------
// Name: LogXNetError()
// Desc: Log errors to the hard drive. When testing xnets.lib, there's no
//       debugging channel, so it's useful to log failures to the hard drive.
//-----------------------------------------------------------------------------
VOID CXBoxSample::LogXNetError( const CHAR* strError, INT iError ) const
{

    // Make sure that we're not logging anything in the final release
#ifndef FINAL_BUILD

    if( m_hLogFile == INVALID_HANDLE_VALUE )
    {
        m_hLogFile = CreateFile( "U:\\XNetError.log", GENERIC_WRITE, 0, NULL,
                                 CREATE_ALWAYS, 0, NULL );
        if( m_hLogFile == INVALID_HANDLE_VALUE )
            return;
    }

    // Write out the error message
    CHAR strBuffer[256];
    wsprintfA( strBuffer, "%s error: %d\r\n", strError, iError );
    DWORD dwWritten;
    WriteFile( m_hLogFile, strBuffer, lstrlenA( strBuffer ), &dwWritten, NULL );

    // Make sure the message makes it to the disk
    FlushFileBuffers( m_hLogFile );

#endif

}

