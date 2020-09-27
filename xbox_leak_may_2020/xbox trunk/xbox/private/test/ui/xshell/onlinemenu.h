/*****************************************************
*** onlinemenu.h
***
*** Header file for our XShell Online menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** February 8th, 2002
***
*****************************************************/

#ifndef _ONLINEMENU_H_
#define _ONLINEMENU_H_

#include "menuscreen.h"
#include "xonlinefuncs.h"

// Sub menus
#include "olusermenu.h"
#include "oluseroptionmenu.h"
#include "passcodemenu.h"

enum OnlineUserEvents
{
    ENUM_ONLINE_NO_ACTION,
    ENUM_ONLINE_PING_START,
    ENUM_ONLINE_PING_IN_PROGRESS,
    ENUM_ONLINE_PING_COMPLETE,
    ENUM_ONLINE_GET_USERNAME,
    ENUM_ONLINE_GOT_INVALIDNAME,
    ENUM_ONLINE_GET_PASSCODE,
    ENUM_ONLINE_GENERATE_USER_START,
    ENUM_ONLINE_GENERATE_USER_IN_PROGRESS,
    ENUM_ONLINE_GENERATE_USER_COMPLETE,
    ENUM_ONLINE_MANAGE_GETMEMAREA,
    ENUM_ONLINE_MANAGE_NAVTOGETSOURCEMEMAREA,
    ENUM_ONLINE_MANAGE_NAVTOSOURCEMEMAREAUSERS,
    ENUM_ONLINE_MANAGE_GETUSERFROMHD,
    ENUM_ONLINE_MANAGE_GETUSEROPTION,
    ENUM_ONLINE_MANAGE_GETDESTMEMAREA,
    ENUM_ONLINE_MANAGE_OVERWRITE
};

class COnlineMenu : public CMenuScreen
{
public:
    COnlineMenu( CXItem* pParent );
    ~COnlineMenu();

    // Must be overridden for this to be a Item on our XShell program
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );
    void Enter();   // Will be called whenever the user enters this menu

    // Public Methods
    HRESULT Init( char* menuFileName );         // Initialize our font and settings

private:
    // Private Properties
    OnlineUserEvents m_UserEvent;               // Used to track the current user event
    HRESULT m_hrOnlineResult;                   // Used to track success or failure of an Online Task
    BOOL m_bDisableInput;                       // Used to disable input for the menu
    HRESULT m_hrPingResult;                     // Used to track the success or failure of a Partner Net Ping
    BOOL m_bPNetPinged;                         // Used to determine if we've attempted a ping to Partner Net
    HRESULT m_hrCreateResult;                   // Used to track the success or failure of creating a user
    BOOL m_bUserCreated;                        // Used to determine if we've attempted to create a user previously
    HANDLE m_hThreadHandle;                     // Handle to our worker thread
    unsigned int m_uiSourceMA;                  // Used to track the source memory area for a copy

    // Online Users
    XONLINE_USER  m_aOnlineUsers[XONLINE_MAX_STORED_ONLINE_USERS]; // Online users that will live on Memory Areas

    CPassCodeMenu       m_PassCodeMenu;         // Allows the user to set a PassCode
    COLUserMenu         m_OLUserMenu;           // Enumerates users on the hard drive
    COLUserOptionMenu   m_OLUserOptionMenu;     // Options for a user
    
    // Private Methods
    static DWORD WINAPI ProcessUserEvents( LPVOID lpParam );    // Handles User initiated events (runs in a separate thread)
    HRESULT CopyUserToMA( PXONLINE_USER pOnlineUser,            // Copies a user to the specified memory area
                          unsigned int uiMemArea );
    void CreateUserStart( WCHAR* pwszUserName );                // Used to start the Create User process
    BOOL NavToSourceMAUsers();                                  // Returns TRUE if it navigates to the Memory Area screen, FALSE if otherwise
};

#endif // _ONLINEMENU_H_
