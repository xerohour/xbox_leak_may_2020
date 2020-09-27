//-----------------------------------------------------------------------------
// File: GameMsg.h
//
// Desc: Implementation of CXBNetMsgHandler base
//
// Hist: 10.19.01 - Updated for Nov release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef GAMEMSG_H
#define GAMEMSG_H

#include "Common.h"
#include "XBNetMsg.h"

class CXBoxSample; // forward declaration




//-----------------------------------------------------------------------------
// Name: class GameMsg
// Desc: Sends, receives and processes simple network messages
//-----------------------------------------------------------------------------
class GameMsg : public CXBNetMsgHandler
{

    CXBoxSample& m_App;

public:

    explicit GameMsg( CXBoxSample& );

    // Overloads
    virtual VOID OnJoinGame( const CXBNetPlayerInfo& );
    virtual VOID OnJoinApproved( const CXBNetPlayerInfo& );
    virtual VOID OnJoinApprovedAddPlayer( const CXBNetPlayerInfo& );
    virtual VOID OnJoinDenied();
    virtual VOID OnPlayerJoined( const CXBNetPlayerInfo& );
    virtual VOID OnWave( const CXBNetPlayerInfo& );
    virtual VOID OnHeartbeat( const CXBNetPlayerInfo& );
    virtual VOID OnPlayerDropout( const CXBNetPlayerInfo&, BOOL bIsHost );

private:

    // Disabled
    GameMsg();
    GameMsg( const GameMsg& );
    GameMsg& operator=( const GameMsg& );

};

#endif // GAMEMSG_H
