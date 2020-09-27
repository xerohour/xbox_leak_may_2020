//-----------------------------------------------------------------------------
// File: XBoxFriends.h
//
// Desc: Illustrates online friends on Xbox.
//
// Hist: 08.08.01 - New for Aug M1 release 
//       10.19.01 - Updated for Nov release
//       01.18.02 - Updated for Feb release
//       02.15.02 - Updated for Mar release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBOXFRIENDS_H
#define XBOXFRIENDS_H

#include "Common.h"
#include "UserInterface.h"
#include "XBApp.h"
#include "XBNet.h"
#include "XBOnlineTask.h"




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD MAX_STATUS_STR = 64;

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
        STATE_CREATE_ACCOUNT,       // Create user account
            STATE_SELECT_ACCOUNT,       // Select user account
            STATE_GET_PIN,              // Enter user PIN
            STATE_LOGGING_ON,           // Perform authentication
            
            STATE_FRIEND_LIST,           // Friend list
            STATE_ACTION_MENU,          // Invite/join/remove friend
            STATE_NEW_FRIEND,            // Invite
            
            STATE_CONFIRM_REMOVE,       // Confirm removal action
            STATE_ERROR                 // Error screen
    };
    
    enum Event
    {
        EV_BUTTON_A,
            EV_BUTTON_B,
            EV_BUTTON_X,
            EV_BUTTON_Y,
            EV_BUTTON_BACK,
            EV_BUTTON_BLACK,
            EV_BUTTON_WHITE,
            EV_UP,
            EV_DOWN,
            EV_NULL
    };
    
    UserInterface   m_UI;                 // UI object
    State           m_State;              // current state
    State           m_NextState;          // return to this state
    DWORD           m_dwCurrItem;         // current selected menu item
    DWORD           m_dwTopItem;          // tracks the index of the top item
    XBUserList      m_UserList;           // available accounts
    DWORD           m_dwCurrUser;         // index of curr user in m_UserList
    ULONGLONG       m_qwUserID;           // unique player ID
    DWORD           m_dwUserIndex;        // which controller
    WCHAR           m_strUser[ XONLINE_USERNAME_SIZE ]; // current user name
    CXBNetLink      m_NetLink;            // network link checking
    CXBPinEntry     m_PinEntry;           // PIN entry object
    DWORD           m_pServices[ NUM_SERVICES ]; // desired services
    BOOL            m_bIsLoggedOn;      
    CXBStopWatch    m_StatusTimer;
    WCHAR           m_strStatus[ MAX_STATUS_STR ];
    CXBOnlineTask   m_hOnlineTask;
    CXBOnlineTask   m_hFriendsTask;        // friends online task
    CXBOnlineTask   m_hFriendEnumTask;     // friend enumerate task
    XBUserList      m_PotentialFriendList; // potential friends
    DWORD           m_dwCurrFriend;
    FriendList       m_FriendList;          // current friends
    BOOL            m_bCloaked;           // currently cloaked?
    
public:
    
    virtual HRESULT Initialize();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();
    
    CXBoxSample();
    
private:
    
    Event GetEvent() const;
    
    VOID UpdateStateCreateAccount( Event );
    VOID UpdateStateSelectAccount( Event );
    VOID UpdateStateGetPin( Event );
    VOID UpdateStateLoggingOn( Event );
    VOID UpdateStateCancelLogon( Event );
    
    VOID UpdateStateFriendList( Event );
    VOID UpdateStateActionMenu( Event );
    VOID UpdateStateNewFriend( Event );
    
    VOID UpdateStateConfirmRemove( Event );
    VOID UpdateStateError( Event );
    
    VOID BeginLogin();
    VOID BeginFriends();
    
    VOID SetPlayerState( DWORD );
    VOID ProcessNotifications();
    BOOL IsNotificationPending( DWORD );
    VOID UpdateFriends();
    VOID FriendRequest();
    VOID GameInvite();
    VOID AnswerRequest( XONLINE_REQUEST_ANSWER_TYPE );
    VOID AnswerGameInvite( XONLINE_GAMEINVITE_ANSWER_TYPE );
    VOID RemoveFriend();
    VOID SetStatus( const WCHAR* );
    VOID Reset( BOOL bIsError );
    
};




#endif // XBOXFRIENDS_H
