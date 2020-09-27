#ifndef _PLAYER_H_
#define _PLAYER_H_

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

#endif // _PLAYER_H_