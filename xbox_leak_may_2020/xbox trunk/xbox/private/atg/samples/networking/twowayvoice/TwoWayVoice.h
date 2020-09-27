#pragma warning( disable: 4786 )
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBStopWatch.h>
#include <XBSocket.h>
#include <cassert>
#include <vector>
#include <algorithm>
#include "Resource.h"
#include "Voice.h"





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


// TCR 3-46 Maximum Number of Connected Xboxes
const DWORD MAX_PLAYERS = 2;            // Max players (may not exceed 16)

// TCR 3-59 Session Discovery Time
const FLOAT GAME_SEARCH_TIME = 2.0f;   // 2 seconds (may not exceed 3)



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

    // Voice Related Data
    HANDLE       m_hVoiceThread;
    HANDLE       m_hVoiceMutex;
    HANDLE       m_hVoiceDeleteEvent;
    DWORD        m_dwVoiceThreadId;
    CNetVoiceUnit *m_pVoiceUnit;            // The single headset


public:
	HRESULT VoiceThreadProc();
	static DWORD WINAPI StaticVoiceThreadProc(LPVOID pParameter);

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
    static VOID GenRandom( WCHAR*, DWORD );
    static WCHAR GetRandVowel();
    static WCHAR GetRandConsonant();
    static VOID AppendConsonant( WCHAR*, BOOL );
    static VOID AppendVowel( WCHAR* );

    VOID LogXNetError( const CHAR*, INT ) const;

    // Voice related functions
    VOID KillVoiceProcessing();
    int FindHeadsetPort();
    VOID StartVoiceProcessing();

};
