//-----------------------------------------------------------------------------
// File: GameMsg.cpp
//
// Desc: Implementation of CXBNetMsgHandler derived class
//
// Hist: 10.19.01 - Updated for Nov release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "GameMsg.h"
#include "MatchMaking.h"




//-----------------------------------------------------------------------------
// Name: GameMsg
// Desc: Constructor
//-----------------------------------------------------------------------------
GameMsg::GameMsg( CXBoxSample& App )
:
    CXBNetMsgHandler(),
    m_App( App )
{
}




//-----------------------------------------------------------------------------
// Name: OnJoinGame
// Desc: Handle new player joining game
//-----------------------------------------------------------------------------
VOID GameMsg::OnJoinGame( const CXBNetPlayerInfo& playerInfo )
{
    m_App.OnJoinGame( playerInfo );
}




//-----------------------------------------------------------------------------
// Name: OnJoinApproved
// Desc: We've been approved for game entry by the given host
//-----------------------------------------------------------------------------
VOID GameMsg::OnJoinApproved( const CXBNetPlayerInfo& hostInfo )
{
    m_App.OnJoinApproved( hostInfo );
}




//-----------------------------------------------------------------------------
// Name: OnJoinApprovedAddPlayer
// Desc: Receiving information on player already in the game
//-----------------------------------------------------------------------------
VOID GameMsg::OnJoinApprovedAddPlayer( const CXBNetPlayerInfo& playerInfo )
{
    m_App.OnJoinApprovedAddPlayer( playerInfo );
}




//-----------------------------------------------------------------------------
// Name: OnJoinDenied
// Desc: Handle join denied
//-----------------------------------------------------------------------------
VOID GameMsg::OnJoinDenied()
{
    m_App.OnJoinDenied();
}




//-----------------------------------------------------------------------------
// Name: OnPlayerJoined
// Desc: The given player joined our game
//-----------------------------------------------------------------------------
VOID GameMsg::OnPlayerJoined( const CXBNetPlayerInfo& playerInfo )
{
    m_App.OnPlayerJoined( playerInfo );
}




//-----------------------------------------------------------------------------
// Name: OnWave
// Desc: The given player waved to us
//-----------------------------------------------------------------------------
VOID GameMsg::OnWave( const CXBNetPlayerInfo& playerInfo )
{
    m_App.OnWave( playerInfo );
}




//-----------------------------------------------------------------------------
// Name: OnHeartbeat
// Desc: The given player sent us a heartbeat
//-----------------------------------------------------------------------------
VOID GameMsg::OnHeartbeat( const CXBNetPlayerInfo& playerInfo )
{
    m_App.OnHeartbeat( playerInfo );
}




//-----------------------------------------------------------------------------
// Name: OnPlayerDropout
// Desc: The given player left the game
//-----------------------------------------------------------------------------
VOID GameMsg::OnPlayerDropout( const CXBNetPlayerInfo& playerInfo, BOOL bIsHost )
{
    m_App.OnPlayerDropout( playerInfo, bIsHost );
}
