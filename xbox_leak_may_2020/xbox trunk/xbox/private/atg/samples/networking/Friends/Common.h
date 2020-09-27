//-----------------------------------------------------------------------------
// File: Common.h
//
// Desc: Friends global header
//
// Hist: 10.20.01 - New for Nov release
//       02.15.02 - Updated for Mar release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBOXFRIENDS_COMMON_H
#define XBOXFRIENDS_COMMON_H

#include "xtl.h"
#include "xonline.h"

#pragma warning( disable: 4786 )
#include <vector>




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD MAX_FRIENDS_DISPLAYED = 5;

enum
{
    // Action menu
    ACTION_INVITE = 0,
    ACTION_JOIN_GAME_ACCEPT,
    ACTION_JOIN_GAME_DECLINE,
    ACTION_JOIN_GAME_REMOVE,
	ACTION_INVITATION_ACCEPT,
    ACTION_INVITATION_DECLINE,
	ACTION_INVITATION_BLOCK,
    ACTION_REMOVE,
    ACTION_MAX,

    // Confirm removal menu
    CONFIRM_REMOVE_YES = 0,
    CONFIRM_REMOVE_NO,
    CONFIRM_REMOVE_MAX
};




//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------
typedef std::vector< XONLINE_FRIEND > FriendList;




#endif // XBOXFRIENDS_COMMON_H
