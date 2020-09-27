//-----------------------------------------------------------------------------
// File: Auth.h
//
// Desc: Shows Xbox online authentication protocols.
//       Includes account creation, PIN entry, validation and logon.
//
// Hist: 08.08.01 - New for Aug M1 release 
//       10.12.01 - Updated for Nov release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef AUTH_H
#define AUTH_H

#include "Common.h"
#include "UserInterface.h"
#include "XBApp.h"
#include "XBNet.h"
#include "XBOnlineTask.h"




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

// Number of services to authenticate
const DWORD NUM_SERVICES = 2;




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    enum State
    {
        STATE_CREATE_ACCOUNT,   // Create user account
        STATE_SELECT_ACCOUNT,   // Select user account
        STATE_GET_PIN,          // Enter user PIN
        STATE_LOGGING_ON,       // Perform authentication
        STATE_SUCCESS,          // Successful authentication
        STATE_ERROR,            // Error
    };

    enum Event
    {
        EV_BUTTON_A,
        EV_BUTTON_B,
        EV_BUTTON_BACK,
        EV_UP,
        EV_DOWN,
        EV_NULL
    };

    UserInterface       m_UI;                // UI object
    State               m_State;             // current state
    State               m_NextState;         // return to this state
    DWORD               m_dwCurrItem;        // current selected menu item
    XBUserList          m_UserList;          // list of available accounts
    DWORD               m_dwCurrUser;        // index of curr user in m_UserList
    DWORD               m_dwUserIndex;       // which controller
    WCHAR               m_strUser[ XONLINE_USERNAME_SIZE ]; // Current user
    CXBNetLink          m_NetLink;                   // Network link checking
    CXBPinEntry         m_PinEntry;                  // PIN entry object
    DWORD               m_pServices[ NUM_SERVICES ]; // List of desired services
    ServiceInfoList     m_ServiceInfoList;           // List of service info
    CXBOnlineTask       m_hOnlineTask;               // Online task

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
    VOID UpdateStateSuccess( Event );
    VOID UpdateStateError( Event );

    VOID BeginLogin();
    VOID SetPlayerState( DWORD );

};




#endif // AUTH_H
