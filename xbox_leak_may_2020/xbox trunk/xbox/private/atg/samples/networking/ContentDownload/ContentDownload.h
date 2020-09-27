//-----------------------------------------------------------------------------
// File: ContentDownload.h
//
// Desc: Shows Xbox online content enumeration, download, installation
//       and removal.
//
// Hist: 09.10.01 - New for Nov release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef CONTENTDOWNLOAD_H
#define CONTENTDOWNLOAD_H

#include "Common.h"
#include "XBApp.h"
#include "UserInterface.h"
#include "XBStopwatch.h"
#include "XBNet.h"
#include "XBOnlineTask.h"



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
        STATE_CREATE_ACCOUNT,   // Create user account
        STATE_SELECT_ACCOUNT,   // Select user account
        STATE_GET_PIN,          // Enter user PIN
        STATE_LOGGING_ON,       // Perform authentication
        STATE_SELECT_DEVICE,    // Allow player to select device
        STATE_ENUM_CONTENT,     // Get list of content on device
        STATE_SELECT_CONTENT,   // Allow player to select content to download
        STATE_CONTENT_INFO,     // Display content detailed information
        STATE_INSTALL_CONTENT,  // Download/install content
        STATE_VERIFY_CONTENT,   // Verify content
        STATE_CONFIRM_REMOVE,   // Verify content removal
        STATE_REMOVE_CONTENT,   // Remove content
        STATE_ERROR,            // Error
    };

    enum Event
    {
        EV_BUTTON_A,
        EV_BUTTON_B,
        EV_BUTTON_X,
        EV_BUTTON_Y,
        EV_BUTTON_BACK,
        EV_UP,
        EV_DOWN,
        EV_NULL
    };

    UserInterface       m_UI;                // UI object
    State               m_State;             // current state
    State               m_NextState;         // return to this state
    DWORD               m_dwCurrItem;        // current selected menu item
    DWORD               m_dwTopItem;         // tracks the index of the top item
    XBUserList          m_UserList;          // list of available accounts
    DWORD               m_dwCurrUser;        // index of curr user in m_UserList
    DWORD               m_dwUserIndex;       // which controller
    WCHAR               m_strUser[ XONLINE_USERNAME_SIZE ]; // current user name
    DWORD               m_pServices[ NUM_SERVICES ];        // List of desired services
    CXBNetLink          m_NetLink;           // Network link checking
    CXBPinEntry         m_PinEntry;          // PIN entry object
    XONLINEOFFERING_ENUM_DEVICE m_EnumDevice; // selected device
    XONLINEOFFERING_ENUM_PARAMS m_EnumParams; // enumerate params
    BOOL                m_bIsLoggedOn;       // TRUE if authenticated
    ContentList         m_ContentList;       // list of content
    DWORD               m_dwCurrContent;     // selected content
    CXBOnlineTask       m_hOnlineTask;       // online task for pumping
    CXBOnlineTask       m_hContentTask;      // content task handle
    BYTE*               m_pEnumBuffer;       // enumeration buffer
    FLOAT               m_fPercentComplete;  // for progress bars
    DWORD               m_dwBlocksInstalled; // blocks installed
    DWORD               m_dwBlocksTotal;     // total blocks to install

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
    VOID UpdateStateSelectDevice( Event );
    VOID UpdateStateEnumContent( Event );
    VOID UpdateStateSelectContent( Event );
    VOID UpdateStateContentInfo( Event );
    VOID UpdateStateInstallContent( Event );
    VOID UpdateStateVerifyContent( Event );
    VOID UpdateStateConfirmRemove( Event );
    VOID UpdateStateRemoveContent( Event );
    VOID UpdateStateError( Event );

    VOID BeginLogin();
    VOID BeginEnum();
    VOID BeginInstall();
    VOID BeginVerify();
    VOID BeginRemove();

    VOID EndEnum();
    VOID SetPlayerState( DWORD );
    VOID Reset( BOOL bIsError );

};

#endif // CONTENTDOWNLOAD_H
