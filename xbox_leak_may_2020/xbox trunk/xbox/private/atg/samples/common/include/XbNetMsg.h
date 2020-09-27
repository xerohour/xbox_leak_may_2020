//-----------------------------------------------------------------------------
// File: XbNetMsg.h
//
// Desc: Simple "waving" game messages to show Xbox communication.
//       Requires linking with XNET[D][S].LIB or XONLINE[D][S].LIB
//
// Hist: 10.20.01 - New for November XDK release
//       02.15.02 - Updated for Mar release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBNETMSG_H
#define XBNETMSG_H

#include <xtl.h>
#include "XBSocket.h"
#include "XBSockAddr.h"

#pragma warning( disable: 4786 )
#include <vector>




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
// matches XONLINE_USERNAME_SIZE for consistency
const DWORD MAX_PLAYER_STR = 28; 
const DWORD MAX_PLAYERS    = 4;  // Game dependent




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
struct CXBNetPlayer
{
    XNADDR xnAddr;                          // player's XNet address
    WCHAR  strPlayerName[ MAX_PLAYER_STR ]; // player's name
};




//-----------------------------------------------------------------------------
// Sent from a client to a host to join a game
//-----------------------------------------------------------------------------
struct CXBNetMsgJoinGame
{
    WCHAR     strPlayerName[ MAX_PLAYER_STR ]; // player who wants to join
    ULONGLONG qwUserID;                        // unique player ID
};




//-----------------------------------------------------------------------------
// Sent from a host to a client in response to a MSG_JOIN_GAME
//-----------------------------------------------------------------------------
struct CXBNetMsgJoinApproved
{
    WCHAR        strHostName[ MAX_PLAYER_STR ]; // host name
    BYTE         byNumPlayers;                  // does not include host
    CXBNetPlayer PlayerList[ MAX_PLAYERS ];     // list of players (no host)
};




//-----------------------------------------------------------------------------
// Sent from a host to a client in response to a MSG_JOIN_GAME
//-----------------------------------------------------------------------------
struct CXBNetMsgJoinDenied
{
    // No payload required
};




//-----------------------------------------------------------------------------
// Sent from a host to other players to notify them that a new player has joined
//-----------------------------------------------------------------------------
struct CXBNetMsgPlayerJoined
{
    CXBNetPlayer player; // The latest player to join the game
};




//-----------------------------------------------------------------------------
// Sent from a player to other players to "wave"
//-----------------------------------------------------------------------------
struct CXBNetMsgWave
{
    // No payload required
};




//-----------------------------------------------------------------------------
// Sent from a player to other players as a "keep-alive"
//-----------------------------------------------------------------------------
struct CXBNetMsgHeartbeat
{
    // No payload required
};




//-----------------------------------------------------------------------------
// End structure packing
//-----------------------------------------------------------------------------
#pragma pack( pop )




//-----------------------------------------------------------------------------
// Name: class CXBNetMessage
// Desc: NetMessage object sent between players and hosts
//-----------------------------------------------------------------------------
class CXBNetMessage
{
    BYTE m_byMessageId;

    union
    {
        CXBNetMsgJoinGame     m_JoinGame;
        CXBNetMsgJoinApproved m_JoinApproved;
        CXBNetMsgJoinDenied   m_JoinDenied;
        CXBNetMsgPlayerJoined m_PlayerJoined;
        CXBNetMsgWave         m_Wave;
        CXBNetMsgHeartbeat    m_Heartbeat;
    };

public:

    // Must construct via ID
    explicit CXBNetMessage( BYTE byMessageId = 0 );

    // Accessors
    BYTE GetId() const      { return m_byMessageId; }
    INT  GetMaxSize() const { return sizeof(*this); }

    CXBNetMsgJoinGame&     GetJoinGame()     { return m_JoinGame;     }
    CXBNetMsgJoinApproved& GetJoinApproved() { return m_JoinApproved; }
    CXBNetMsgJoinDenied&   GetJoinDenied()   { return m_JoinDenied;   }
    CXBNetMsgPlayerJoined& GetPlayerJoined() { return m_PlayerJoined; }
    CXBNetMsgWave&         GetWave()         { return m_Wave;         }
    CXBNetMsgHeartbeat&    GetHeartbeat()    { return m_Heartbeat;    }

    INT GetSize() const;

private:

    // Disabled
    CXBNetMessage( const CXBNetMessage& );
    CXBNetMessage& operator=( const CXBNetMessage& );

};




//-----------------------------------------------------------------------------
// Name: class CXBNetPlayerInfo
// Desc: Player information used to store list of other players in the game
//-----------------------------------------------------------------------------
struct CXBNetPlayerInfo
{
    XNADDR    xnAddr;                          // XNet address
    IN_ADDR   inAddr;                          // Xbox IP (not a "real" IP)
    ULONGLONG qwUserID;                        // Unique player ID
    WCHAR     strPlayerName[ MAX_PLAYER_STR ]; // player name
    DWORD     dwLastHeartbeat;                 // last heartbeat, in our clocks
};

typedef std::vector< CXBNetPlayerInfo > CXBNetPlayerList;




//-----------------------------------------------------------------------------
// Name: class CXBNetMsgHandler
// Desc: Sends, receives and processes simple network messages
//-----------------------------------------------------------------------------
class CXBNetMsgHandler
{

public:

    static const WORD GAME_PORT = 9137;   // any port is valid

private:

    CXBSocket m_Sock;                       // communication socket
    XNKID     m_xnSessionID;                // session ID
    WCHAR     m_strHost[ MAX_PLAYER_STR ];  // host name
    BOOL      m_bIsHost;                    // TRUE if host

public:

    CXBNetMsgHandler();
    virtual ~CXBNetMsgHandler();

    HRESULT Initialize();
    VOID SetUser( const WCHAR* strUser, BOOL bIsHost );
    VOID SetSessionID( const XNKID& );

    // Send messages
    VOID SendJoinGame( const CXBSockAddr& saGameHost, const WCHAR* strUser,
                       ULONGLONG qwUserID );
    VOID SendWave( const CXBNetPlayerList& );
    VOID SendHeartbeat( const CXBNetPlayerList& );

    // Receive messages
    BOOL ProcessMessages( CXBNetPlayerList& );

    // Process dropouts
    BOOL ProcessPlayerDropouts( const CXBNetPlayerList&, DWORD dwTimeout );

    // Override for custom behavior
    virtual VOID OnJoinGame( const CXBNetPlayerInfo& );
    virtual VOID OnJoinApproved( const CXBNetPlayerInfo& );
    virtual VOID OnJoinApprovedAddPlayer( const CXBNetPlayerInfo& );
    virtual VOID OnJoinDenied();
    virtual VOID OnPlayerJoined( const CXBNetPlayerInfo& );
    virtual VOID OnWave( const CXBNetPlayerInfo& );
    virtual VOID OnHeartbeat( const CXBNetPlayerInfo& );
    virtual VOID OnPlayerDropout( const CXBNetPlayerInfo&, BOOL bIsHost );

private:

    VOID SendJoinApproved( const CXBSockAddr& saClient, const CXBNetPlayerList& );
    VOID SendJoinDenied( const CXBSockAddr& saClient );
    VOID SendPlayerJoined( const CXBNetPlayer&, const CXBSockAddr& saPlayer );

    VOID SendWave( const CXBSockAddr& );
    VOID SendHeartbeat( const CXBSockAddr& );

    VOID ProcessMessage( CXBNetMessage&, const CXBSockAddr&,
                         CXBNetPlayerList& );

    VOID ProcessJoinGame( const CXBNetMsgJoinGame&, const CXBSockAddr&, 
                          CXBNetPlayerList& );
    VOID ProcessJoinApproved( const CXBNetMsgJoinApproved&, const CXBSockAddr& );
    VOID ProcessJoinDenied();
    VOID ProcessPlayerJoined( const CXBNetMsgPlayerJoined&, const CXBSockAddr& );
    VOID ProcessWave( const CXBSockAddr&, const CXBNetPlayerList& );
    VOID ProcessHeartbeat( const CXBSockAddr&, const CXBNetPlayerList& );

};

#endif // XBNETMSG_H
