#ifndef _PLAYER_H_
#define _PLAYER_H_


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
const DWORD PLAYER_TIMEOUT    = 2000;   // 2 seconds
const FLOAT CHECK_LINK_STATUS = 0.5f;   // Check status twice/sec
const FLOAT PLAYER_HEARTBEAT  = 0.3f;   // ~3 times per second
const FLOAT GAME_JOIN_TIME    = 2.0f;   // 2 seconds


// TCR 3-46 Maximum Number of Connected Xboxes
const DWORD MAX_PLAYERS = 4;            // Max players (may not exceed 16)

// TCR 3-59 Session Discovery Time
const FLOAT GAME_SEARCH_TIME = 2.0f;   // 2 seconds (may not exceed 3)

const DWORD MAX_PLAYER_NAME   = 12;     // Includes null

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
// Local Player struct used by some messages
//-----------------------------------------------------------------------------
struct Player
{
    XNADDR xnAddr;                           // player's XNet address
    WCHAR  strPlayerName[ MAX_PLAYER_NAME ]; // player's name
};

typedef std::vector< PlayerInfo >   PlayerList;

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
    MSG_HEARTBEAT,      // player   player  direct      <none>
	MSG_VOICEPORT       // player   player  direct      <MSG_VOICE_PORT> if requested 
    
};

//-----------------------------------------------------------------------------
// Message payloads
//-----------------------------------------------------------------------------
// Pack to minimize network traffic
#pragma pack( push )
#pragma pack( 1 )


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

enum ACTION
{
	NO_ACTION , 
	ADD_PORT , 
	DELETE_PORT , 
	ADD_AND_RESPOND
};

struct MsgVoicePort
{
	int  action;      // Action requested from the other player     
	WORD    wVoicePort;  // Port to which the chatter is bound ; 0 means no port for that communicator
};
#pragma pack( pop )



#endif // _PLAYER_H_