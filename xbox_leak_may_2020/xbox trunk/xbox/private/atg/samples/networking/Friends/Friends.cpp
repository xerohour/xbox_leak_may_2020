//-----------------------------------------------------------------------------
// File: Friends.cpp
//
// Desc: Illustrates online friends on Xbox.
//
// Hist: 10.20.01 - New for Aug M1 release 
//       01.21.02 - Updated for Feb release
//       02.15.02 - Updated for Mar release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Friends.h"
#include "XBMemUnit.h"
#include "XBVoice.h"
#include <cassert>
#include <algorithm>




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD TITLE_ID = 0xFFFF0110;

const DWORD RANK_PENDING    = 0x01000000;
const DWORD RANK_SAME_TITLE = 0x00100000;
const DWORD RANK_JOINABLE   = 0x00010000;
const DWORD RANK_PLAYING    = 0x00001000;
const DWORD RANK_ONLINE     = 0x00000100;




//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBoxSample xbApp;
    if( FAILED( xbApp.Create() ) )
        return;
    xbApp.Run();
}




//-----------------------------------------------------------------------------
// Name: struct MatchUser
// Desc: Predicate for searching the potential friend list
//-----------------------------------------------------------------------------
struct MatchUser
{
    const CHAR* m_strUser;
    
    MatchUser( const CHAR* strUser ) : m_strUser( strUser )
    {
    }
    
    bool operator()( const XONLINE_USER& user ) const
    {
        return( lstrcmpA( user.name, m_strUser ) == 0 );
    }
};




//-----------------------------------------------------------------------------
// Name: struct FriendCompare
// Desc: Predicate for sorting the friend list
//-----------------------------------------------------------------------------
struct FriendCompare
{
    bool operator()( const XONLINE_FRIEND& x, const XONLINE_FRIEND& y ) const
    {
        DWORD dwRankX = GetRank( x );
        DWORD dwRankY = GetRank( y );
        
        // In the case of matching order, friends are sorted alphabetically
        if( dwRankX == dwRankY )
            return( lstrcmpA( x.username, y.username ) > 0 );
        
        return( dwRankX > dwRankY );
    };
    
private:
    
    DWORD GetRank( const XONLINE_FRIEND& x ) const
    {
        // Order:
        //
        // 1) pending invitations
        // 2) online in same title
        // 3) online in different title
        // 4) in game session
        // 5) online, but not in game session
        // 4) offline
        
        DWORD dwRank = 0;
        
        if( x.friendState & XONLINE_FRIENDSTATE_FLAG_REQUEST )
            dwRank += RANK_PENDING;
        
        if( x.friendState & XONLINE_FRIENDSTATE_FLAG_ONLINE )
        {
            if( x.titleID == TITLE_ID )
                dwRank += RANK_SAME_TITLE;
        }
        
        if( x.friendState & XONLINE_FRIENDSTATE_FLAG_JOINABLE )
            dwRank += RANK_JOINABLE;
        
        if( x.friendState & XONLINE_FRIENDSTATE_FLAG_PLAYING )
            dwRank += RANK_PLAYING;
        
        if( x.friendState & XONLINE_FRIENDSTATE_FLAG_ONLINE )
            dwRank += RANK_ONLINE;
        
        return dwRank;
    }
};




//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
:
CXBApplication(),
m_UI( m_strFrameRate ),
m_StatusTimer( FALSE )
{
    m_State     = STATE_SELECT_ACCOUNT;
    m_NextState = STATE_SELECT_ACCOUNT;
    
    // Login to matchmaking service for access to friends
    m_pServices[0] = XONLINE_MATCHMAKING_SERVICE;
    
    m_dwCurrItem  = 0;
    m_dwTopItem   = 0;
    m_dwCurrUser  = 0;
    m_dwCurrFriend = 0;
    
    m_qwUserID    = 0;
    m_dwUserIndex = 0;
    m_bIsLoggedOn = FALSE;
    *m_strUser    = 0;
    *m_strStatus  = 0;
    m_bCloaked    = FALSE;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Initialize game UI
    if( FAILED( m_UI.Initialize() ) )
        return E_FAIL;
    
    // Initialize the network stack
    if( FAILED( XBNet_OnlineInit( 0 ) ) )
        return E_FAIL;
    
    // Get information on all accounts for this Xbox
    if( FAILED( XBOnline_GetUserList( m_UserList ) ) )
        return E_FAIL;
    
    // If no accounts, then player needs to create an account.
    // For development purposes, accounts are created using the OnlineSetup
    // tool in the XDK Launcher. For retail Xbox consoles, accounts are
    // created in the Xbox Dashboard.
    if( m_UserList.empty() )
    {
        m_State = STATE_CREATE_ACCOUNT;
        return S_OK;
    }
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Check the physical connection
    if( !m_NetLink.IsActive() )
    {
        m_UI.SetErrorStr( L"This Xbox has lost its online connection" );
        Reset( TRUE );
        return S_OK;
   }
    
    // Maintain our connection once we've logged on
    if( m_bIsLoggedOn )
    {
        
        if( FAILED( m_hOnlineTask.Continue() )||
            FAILED( m_hFriendsTask.Continue() ) )
        {
            m_UI.SetErrorStr( L"Connection was lost. Must relogin" );
            Reset( TRUE );
            return S_OK;
        }
    }
    
    // Clear status string after 3 seconds
    if( m_StatusTimer.GetElapsedSeconds() > 3.0f )
    {
        SetStatus( L"" );
        m_StatusTimer.Stop();
    }
    
    
    Event ev = GetEvent();
    
    switch( m_State )
    {
    case STATE_CREATE_ACCOUNT:   UpdateStateCreateAccount( ev ); break;
    case STATE_SELECT_ACCOUNT:   UpdateStateSelectAccount( ev ); break;
    case STATE_GET_PIN:          UpdateStateGetPin( ev );        break;
    case STATE_LOGGING_ON:       UpdateStateLoggingOn( ev );     break;
        
    case STATE_FRIEND_LIST:      UpdateStateFriendList( ev );       break;
    case STATE_ACTION_MENU:      UpdateStateActionMenu( ev );      break;
    case STATE_NEW_FRIEND:       UpdateStateNewFriend( ev );        break;
        
    case STATE_CONFIRM_REMOVE:   UpdateStateConfirmRemove( ev ); break;
    case STATE_ERROR:            UpdateStateError( ev );         break;
    default:                     assert( FALSE );                break;
    }
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3D
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
        0x000A0A6A, 1.0f, 0L );
    
    switch( m_State )
    {
    case STATE_LOGGING_ON:     m_UI.RenderLoggingOn();     break;
    case STATE_ERROR:          m_UI.RenderError();         break;
        
    case STATE_CREATE_ACCOUNT:
        m_UI.RenderCreateAccount( TRUE );
        break;
    case STATE_SELECT_ACCOUNT:
        m_UI.RenderSelectAccount( m_dwCurrItem, m_UserList );
        break;
    case STATE_GET_PIN:
        m_UI.RenderGetPin( m_PinEntry, m_strUser );
        break;
    case STATE_FRIEND_LIST:
        m_UI.RenderFriendList( m_dwTopItem, m_dwCurrItem, m_FriendList,
            m_strStatus, m_bCloaked );
        break;
    case STATE_ACTION_MENU:
        assert( m_dwCurrFriend < m_FriendList.size() );
        m_UI.RenderActionMenu( m_dwCurrItem, &m_FriendList[ m_dwCurrFriend ] );
        break;
    case STATE_NEW_FRIEND:
        m_UI.RenderNewFriend( m_dwCurrItem, m_PotentialFriendList );
        break;
    case STATE_CONFIRM_REMOVE:
        assert( m_dwCurrFriend < m_FriendList.size() );
        m_UI.RenderConfirmRemove( m_dwCurrItem, 
            &m_FriendList[ m_dwCurrFriend ] );
        break;
    default:
        assert( FALSE );
        break;
    }
    
    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetEvent()
// Desc: Return the state of the controller
//-----------------------------------------------------------------------------
CXBoxSample::Event CXBoxSample::GetEvent() const
{
    // "A" or "Start"
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] ||
        m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_START )
    {
        return EV_BUTTON_A;
    }
    
    // "B"
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] )
        return EV_BUTTON_B;
    
    // "X"
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
        return EV_BUTTON_X;
    
    // "Y"
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_Y ] )
        return EV_BUTTON_Y;
    
    // "Back"
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        return EV_BUTTON_BACK;
    
    // "Black"
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_BLACK ] )
        return EV_BUTTON_BLACK;
    
    // "White"
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_WHITE ] )
        return EV_BUTTON_WHITE;
    
    // Movement
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
        return EV_UP;
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
        return EV_DOWN;
    
    return EV_NULL;
}




//-----------------------------------------------------------------------------
// Name: UpdateStateCreateAccount()
// Desc: Inform player that account must be generated using external tool
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateCreateAccount( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        
        // Return to Dashboard. Retail Dashboard will include
        // online account creation. Development XDK Launcher
        // includes the Xbox Online Setup Tool for creating accounts.
        LD_LAUNCH_DASHBOARD ld;
        ZeroMemory( &ld, sizeof(ld) );
        ld.dwReason = XLD_LAUNCH_DASHBOARD_MAIN_MENU;
        XLaunchNewImage( NULL, PLAUNCH_DATA( &ld ) );
        break;
        
    default:
        // If any MUs are inserted/removed, need to update the
        // user account list
        DWORD dwInsertions;
        DWORD dwRemovals;
        if( CXBMemUnit::GetMemUnitChanges( dwInsertions, dwRemovals ) )
        {
            m_UserList.clear();
            XBOnline_GetUserList( m_UserList );
            if( m_UserList.empty() )
                m_State = STATE_CREATE_ACCOUNT;
            else
                m_dwCurrItem = 0;
        }
        break;
        
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateSelectAccount()
// Desc: Allow player to select user account
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateSelectAccount( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        {
            // Save current account information
            m_dwCurrUser = m_dwCurrItem;
            m_qwUserID = m_UserList[ m_dwCurrUser ].xuid.qwUserID;
            
            // Make WCHAR copy of user name
            XBUtil_GetWide( m_UserList[ m_dwCurrUser ].name, m_strUser, 
                XONLINE_NAME_SIZE );
            
            // If this user doesn't required PIN entry, begin authentication
            DWORD dwPinReq = m_UserList[ m_dwCurrUser ].dwUserOptions & 
                XONLINE_USER_OPTION_REQUIRE_PIN;
            if( !dwPinReq )
            {
                m_State = STATE_LOGGING_ON;
                BeginLogin();
                break;
            }
            
            // Begin PIN input
            m_State = STATE_GET_PIN;
            HRESULT hr = m_PinEntry.BeginInput( m_DefaultGamepad );
            assert( SUCCEEDED(hr) );
            break;
        }
        
    case EV_UP:
        // Move to previous user account; allow wrap to bottom
        if( m_dwCurrItem == 0 )
            m_dwCurrItem = m_UserList.size() - 1;
        else
            --m_dwCurrItem;
        break;
        
    case EV_DOWN:
        // Move to next user account; allow wrap to top
        if( m_dwCurrItem == m_UserList.size() - 1 )
            m_dwCurrItem = 0;
        else
            ++m_dwCurrItem;
        break;
        
    default:
        // If any MUs are inserted/removed, need to update the
        // user account list
        DWORD dwInsertions;
        DWORD dwRemovals;
        if( CXBMemUnit::GetMemUnitChanges( dwInsertions, dwRemovals ) )
        {
            m_UserList.clear();
            XBOnline_GetUserList( m_UserList );
            if( m_UserList.empty() )
                m_State = STATE_CREATE_ACCOUNT;
            else
                m_dwCurrItem = 0;
        }
        break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateGetPin()
// Desc: PIN entry
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateGetPin( Event ev )
{
    HRESULT hr = m_PinEntry.DecodeInput( m_DefaultGamepad );
    switch( hr )
    {
    case E_ABORT:
        // B or BACK
        m_State = STATE_SELECT_ACCOUNT;
        break;
    case S_OK:
        if( m_PinEntry.IsPinComplete() )
        {
            // Validate PIN
            assert( m_dwCurrUser < m_UserList.size() );
            BYTE* pin = m_UserList[ m_dwCurrUser ].pin;
            if( m_PinEntry.IsPinValid( pin ) )
            {
                m_PinEntry.EndInput();
                
                // PIN is valid; initiate login
                m_State = STATE_LOGGING_ON;
                BeginLogin();
            }
            else
            {
                // The default PIN codes for users created by the Xbox 
                // Online Setup Tool are always "right, down, right, Y"
                m_UI.SetErrorStr( L"That was not the correct pass code" );
                m_State = STATE_ERROR;
                m_NextState = STATE_GET_PIN;
                hr = m_PinEntry.BeginInput( m_DefaultGamepad );
                assert( SUCCEEDED(hr) );
            }
        }
        break;
    default:
        m_UI.SetErrorStr( L"PIN entry failure" );
        m_State = STATE_ERROR;
        m_NextState = STATE_GET_PIN;
        hr = m_PinEntry.BeginInput( m_DefaultGamepad );
        assert( SUCCEEDED(hr) );
        break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateLoggingOn()
// Desc: Spin during authentication
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateLoggingOn( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        
        // Cancel the task
        m_hOnlineTask.Close();
        m_State = STATE_SELECT_ACCOUNT;
        return;
    }
    
    HRESULT hr;
    
    hr = m_hOnlineTask.Continue();
    if( FAILED( hr ) )
    {
        m_UI.SetErrorStr( L"Connection was lost. Try again." );
        Reset( TRUE );
        return;
    }
    
    // Check login status
    if( hr != XONLINETASK_S_RUNNING )
    {
        BOOL bSuccess = TRUE;
        HRESULT hrService = S_OK;
        
        // Check for general errors
        if( hr != XONLINE_S_LOGON_CONNECTION_ESTABLISHED )
        {
            m_UI.SetErrorStr( L"Login failed.\n\n"
                L"Error %x returned by "
                L"XOnlineTaskContinue", hr );
            bSuccess = FALSE;
        }
        else
        {

            // Next, check if the user was actually logged on
            PXONLINE_USER pLoggedOnUsers = XOnlineGetLogonUsers();
            
            assert( pLoggedOnUsers );
            
            hr = pLoggedOnUsers[ m_dwCurrUser ].hr;
            
            if( FAILED( hr ) )
            {
                m_UI.SetErrorStr( L"User Login failed (Error 0x%x)",
                    hr );
                bSuccess = FALSE;
            }
            else
            {
                // Check for service errors
                for( DWORD i = 0; i < NUM_SERVICES; ++i )
                {
                    if( FAILED( hrService = XOnlineGetServiceInfo( 
                        m_pServices[i],NULL ) ) )
                    {
                        m_UI.SetErrorStr( L"Login failed.\n\n"
                            L"Error 0x%x logging into service %d",
                            hrService, m_pServices[i] );
                        bSuccess    = FALSE;
                        break;
                    }
                }
            }
        }
        
        if( bSuccess )
        {
            // We're now on the system
            m_bIsLoggedOn = TRUE;
            
            // Notify the world
            DWORD dwState = XONLINE_FRIENDSTATE_FLAG_ONLINE |
                XONLINE_FRIENDSTATE_FLAG_PLAYING;
            if( XBVoice_HasDevice() )
                dwState |= XONLINE_FRIENDSTATE_FLAG_VOICE;
            
            SetPlayerState( dwState );
            
            m_State = STATE_FRIEND_LIST;
            m_dwCurrItem = 0;
            
            // Begin handling friend notifications
            BeginFriends();
        }
        else
        {
            Reset( TRUE );
        }
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateFriendList()
// Desc: Friend list navigation
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateFriendList( Event ev )
{

    UpdateFriends();

    // Handle the special case of an empty list
    if( m_FriendList.empty() )
    {
        switch( ev )
        {
        case EV_BUTTON_Y:
            m_State = STATE_NEW_FRIEND;
            m_dwCurrItem = 0;
            break;
        }
        return;
    }
    
    // If the list shrunk, may need to update last element ptr
    if( m_dwCurrItem >= m_FriendList.size() )
    {
        m_dwCurrItem = m_FriendList.size() - 1;
        
        // If we're at the top of the displayed list, shift the display
        if( m_dwCurrItem == m_dwTopItem )
        {
            if( m_dwTopItem > 0 )
                --m_dwTopItem;
        }
        
        // If we're at the bottom of the displayed list, shift the display
        if( m_dwCurrItem == m_dwTopItem + MAX_FRIENDS_DISPLAYED - 1 )
        {
            if( m_dwTopItem + MAX_FRIENDS_DISPLAYED < m_FriendList.size() )
                ++m_dwTopItem;
        }
    }
    
    switch( ev )
    {
    case EV_BUTTON_A:
        // Track the current friend
        m_dwCurrFriend = m_dwCurrItem;
        m_State = STATE_ACTION_MENU;
        m_dwCurrItem = 0;
        break;

    case EV_BUTTON_Y:
        m_State = STATE_NEW_FRIEND;
        m_dwCurrItem = 0;
        break;

    case EV_BUTTON_BLACK:
        {
            // Cloak/uncloak ourself
            
            m_bCloaked = !m_bCloaked;
            
            DWORD dwState = XONLINE_FRIENDSTATE_FLAG_ONLINE |
                XONLINE_FRIENDSTATE_FLAG_PLAYING;
            
            if( m_bCloaked )
                dwState |= XONLINE_FRIENDSTATE_FLAG_CLOAKED;
            
            if( XBVoice_HasDevice() )
                dwState |= XONLINE_FRIENDSTATE_FLAG_VOICE;
            
            SetPlayerState( dwState );
            break;
        }
    case EV_UP:
        // If we're at the top of the displayed list, shift the display
        if( m_dwCurrItem == m_dwTopItem )
        {
            if( m_dwTopItem > 0 )
                --m_dwTopItem;
        }
        
        // Move to the previous item
        if( m_dwCurrItem > 0 )
            --m_dwCurrItem;
        
        break;
        
    case EV_DOWN:
        // If we're at the bottom of the displayed list, shift the display
        if( m_dwCurrItem == m_dwTopItem + MAX_FRIENDS_DISPLAYED - 1 )
        {
            if( m_dwTopItem + MAX_FRIENDS_DISPLAYED < m_FriendList.size() )
                ++m_dwTopItem;
        }
        
        // Move to next item
        if( m_dwCurrItem < m_FriendList.size() - 1 )
            ++m_dwCurrItem;
        
        break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateActionMenu()
// Desc: Friend action menu navigation
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateActionMenu( Event ev )
{
    // Determine the state of the action menu
    assert( m_dwCurrFriend < m_FriendList.size() );
    BOOL bJoinable = m_FriendList[ m_dwCurrFriend ].friendState & 
        XONLINE_FRIENDSTATE_FLAG_PLAYING;
    
    BOOL bFriendRequest = m_FriendList[ m_dwCurrFriend ].friendState & 
        XONLINE_FRIENDSTATE_FLAG_REQUEST;

    BOOL bGameJoinInvite   = bJoinable && 
        ( m_FriendList[ m_dwCurrFriend ].friendState & 
          XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE );
    

    if( bFriendRequest )
    {
        if( m_dwCurrItem != ACTION_INVITATION_ACCEPT &&
            m_dwCurrItem != ACTION_INVITATION_DECLINE && 
            m_dwCurrItem != ACTION_INVITATION_BLOCK )
            m_dwCurrItem =  ACTION_INVITATION_ACCEPT;
    }
    else if( bGameJoinInvite )
    {

        if( m_dwCurrItem != ACTION_JOIN_GAME_ACCEPT &&
            m_dwCurrItem != ACTION_JOIN_GAME_DECLINE &&
            m_dwCurrItem != ACTION_JOIN_GAME_REMOVE )
            m_dwCurrItem =  ACTION_JOIN_GAME_ACCEPT;
        
    }

    switch( ev )
    {
    case EV_BUTTON_A:
        switch( m_dwCurrItem )
        {
        case ACTION_INVITE:
            GameInvite();
            m_UI.SetErrorStr( L"Invitation sent" );
            break;
        case ACTION_JOIN_GAME_ACCEPT:
            AnswerGameInvite( XONLINE_GAMEINVITE_YES );
            m_UI.SetErrorStr( L"Insert game disc" );
            break;
        case ACTION_JOIN_GAME_DECLINE:
            AnswerGameInvite( XONLINE_GAMEINVITE_NO );
            m_UI.SetErrorStr( L"Declined to join game" );
            break;
        case ACTION_JOIN_GAME_REMOVE:
            AnswerGameInvite( XONLINE_GAMEINVITE_REMOVE );
            m_UI.SetErrorStr( L"Friend removed" );
            break;
        case ACTION_INVITATION_ACCEPT:
            AnswerRequest( XONLINE_REQUEST_YES );
            m_UI.SetErrorStr( L"Invitation accepted" );
            break;
        case ACTION_INVITATION_DECLINE:
            AnswerRequest( XONLINE_REQUEST_NO );
            m_UI.SetErrorStr( L"Invitation declined" );
            break;
        case ACTION_INVITATION_BLOCK:
            AnswerRequest( XONLINE_REQUEST_BLOCK  );
            m_UI.SetErrorStr( L"Invitation declined, and blocked" );
            break;
        case ACTION_REMOVE:
            m_State = STATE_CONFIRM_REMOVE;
            m_dwCurrItem = CONFIRM_REMOVE_NO;
            return;
        }
        m_dwTopItem = m_dwCurrItem = 0;
        m_State = STATE_ERROR;
        m_NextState = STATE_FRIEND_LIST;
        break;
        
        case EV_BUTTON_B:
        case EV_BUTTON_BACK:
            m_State = STATE_FRIEND_LIST;
            m_dwCurrItem = 0;
            break;
            
        case EV_UP:
            
            if( bFriendRequest )
            {
                switch( m_dwCurrItem )
                {
                case ACTION_INVITATION_ACCEPT:
                    m_dwCurrItem = ACTION_INVITATION_BLOCK;
                    break;
                case ACTION_INVITATION_DECLINE:
                    m_dwCurrItem = ACTION_INVITATION_ACCEPT;
                    break;
                case ACTION_INVITATION_BLOCK:
                    m_dwCurrItem = ACTION_INVITATION_DECLINE;
                    break;
                default:
                    assert( 0 );
                }
            }
            else 
            {
                if( bGameJoinInvite )
                {
                    switch( m_dwCurrItem )
                    {
                    case ACTION_JOIN_GAME_ACCEPT:
                        m_dwCurrItem = ACTION_JOIN_GAME_REMOVE;
                        break;
                    case ACTION_JOIN_GAME_DECLINE:
                        m_dwCurrItem = ACTION_JOIN_GAME_ACCEPT;
                        break;
                    case ACTION_JOIN_GAME_REMOVE:
                        m_dwCurrItem = ACTION_JOIN_GAME_DECLINE;
                        break;
                    default:
                        assert( 0 );
                    }
                    
                }
                else
                {
                    switch( m_dwCurrItem )
                    {
                    case ACTION_INVITE:
                        m_dwCurrItem = ACTION_REMOVE;
                        break;
                    case ACTION_REMOVE:
                        m_dwCurrItem = ACTION_INVITE;
                        break;
                    default:
                        assert( 0 );
                    }

                }
            }
            break;
            
        case EV_DOWN:
            
            if( bFriendRequest )
            {
                switch( m_dwCurrItem )
                {
                case ACTION_INVITATION_ACCEPT:
                    m_dwCurrItem = ACTION_INVITATION_DECLINE;
                    break;
                case ACTION_INVITATION_DECLINE:
                    m_dwCurrItem = ACTION_INVITATION_BLOCK;
                    break;
                case ACTION_INVITATION_BLOCK:
                    m_dwCurrItem = ACTION_INVITATION_ACCEPT;
                    break;
                default:
                    assert( 0 );
                }
            }
            else 
            {
                if( bGameJoinInvite )
                {
                    switch( m_dwCurrItem )
                    {
                    case ACTION_JOIN_GAME_ACCEPT:
                        m_dwCurrItem = ACTION_JOIN_GAME_DECLINE;
                        break;
                    case ACTION_JOIN_GAME_DECLINE:
                        m_dwCurrItem = ACTION_JOIN_GAME_REMOVE;
                        break;
                    case ACTION_JOIN_GAME_REMOVE:
                        m_dwCurrItem = ACTION_JOIN_GAME_ACCEPT;
                        break;
                    default:
                        assert( 0 );
                    }
                    
                }
                else
                {
                    switch( m_dwCurrItem )
                    {
                    case ACTION_INVITE:
                        m_dwCurrItem = ACTION_REMOVE;
                        break;
                    case ACTION_REMOVE:
                        m_dwCurrItem = ACTION_INVITE;
                        break;
                    default:
                        assert( 0 );
                    }

                }
            }
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateNewFriend()
// Desc: Add new friend
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateNewFriend( Event ev )
{
    // Handle the special case of an empty list
    if( m_PotentialFriendList.empty() )
    {
        switch( ev )
        {
        case EV_BUTTON_A:
        case EV_BUTTON_B:
        case EV_BUTTON_BACK:
            m_State = STATE_FRIEND_LIST;
            m_dwCurrItem = 0;
            break;
        }
        return;
    }
    
    switch( ev )
    {
    case EV_BUTTON_A:
        FriendRequest();
        m_UI.SetErrorStr( L"Friend request issued" );
        m_dwTopItem = m_dwCurrItem = 0;
        m_State = STATE_ERROR;
        m_NextState = STATE_NEW_FRIEND;
        break;
        
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        m_State = STATE_FRIEND_LIST;
        m_dwTopItem = m_dwCurrItem = 0;
        break;
        
    case EV_UP:
        if( m_PotentialFriendList.empty() )
            break;
        
        // If we're at the top of the displayed list, shift the display
        if( m_dwCurrItem == m_dwTopItem )
        {
            if( m_dwTopItem > 0 )
                --m_dwTopItem;
        }
        
        // Move to the previous item
        if( m_dwCurrItem > 0 )
            --m_dwCurrItem;
        
        break;
        
    case EV_DOWN:
        if( m_PotentialFriendList.empty() )
            break;
        
        // If we're at the bottom of the displayed list, shift the display
        if( m_dwCurrItem == m_dwTopItem + MAX_FRIENDS_DISPLAYED - 1 )
        {
            if( m_dwTopItem + MAX_FRIENDS_DISPLAYED < m_PotentialFriendList.size() )
                ++m_dwTopItem;
        }
        
        // Move to next item
        if( m_dwCurrItem < m_PotentialFriendList.size() - 1 )
            ++m_dwCurrItem;
        
        break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateConfirmRemove()
// Desc: Confirmation dialog for friend removal
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateConfirmRemove( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        if( m_dwCurrItem == CONFIRM_REMOVE_YES )
        {
            RemoveFriend();
            m_UI.SetErrorStr( L"Friend removed" );
            m_dwTopItem = m_dwCurrItem = 0;
            m_State = STATE_ERROR;
            m_NextState = STATE_FRIEND_LIST;
        }
        else // CONFIRM_REMOVE_NO
        {
            m_State = STATE_ACTION_MENU;
            m_dwTopItem = m_dwCurrItem = 0;
        }
        break;
        
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        m_State = STATE_ACTION_MENU;
        m_dwTopItem = m_dwCurrItem = 0;
        break;
        
    case EV_UP:
        if( m_dwCurrItem == 0 )
            m_dwCurrItem = CONFIRM_REMOVE_MAX - 1;
        else
            --m_dwCurrItem;
        break;
        
    case EV_DOWN:
        if( m_dwCurrItem == CONFIRM_REMOVE_MAX - 1 )
            m_dwCurrItem = 0;
        else
            ++m_dwCurrItem;
        break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateError()
// Desc: Handle error screen
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateError( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        m_State = m_NextState;
        break;
    }
}




//-----------------------------------------------------------------------------
// Name: BeginLogin()
// Desc: Initiate the authentication process
//-----------------------------------------------------------------------------
VOID CXBoxSample::BeginLogin()
{
    // Select a reasonable controller for the current player by choosing
    // the first controller found. Game code should do this much more
    // precisely. See below for details.
    for( m_dwUserIndex = 0; m_dwUserIndex < XGetPortCount(); ++m_dwUserIndex )
    {
        if( m_Gamepad[m_dwUserIndex].hDevice )
            break;
    }
    if( m_dwUserIndex >= XGetPortCount() )
        m_dwUserIndex = 0;
    
    // XOnlineLogon() allows a list of up to 4 players (1 per controller)
    // to login in a single call. This sample shows how to authenticate
    // a single user. The list must be a one-to-one match of controller 
    // to player in order for the online system to recognize which player
    // is using which controller.
    XONLINE_USER pUserList[ XGetPortCount() ] = { 0 };
    CopyMemory( &pUserList[ m_dwUserIndex ], &m_UserList[ m_dwCurrUser ],
        sizeof( XONLINE_USER ) );
    
    // Initiate the login process. XOnlineTaskContinue() is used to poll
    // the status of the login.
    HRESULT hr = XOnlineLogon( pUserList, m_pServices, NUM_SERVICES, 
        NULL, &m_hOnlineTask );
    
    if( FAILED(hr) )
    {
        m_hOnlineTask.Close();
        m_UI.SetErrorStr( L"Login failed to start. Error %x", hr );
        m_State = STATE_ERROR;
        m_NextState = STATE_SELECT_ACCOUNT;
    }
}




//-----------------------------------------------------------------------------
// Name: BeginFriends()
// Desc: Initiate friends handling
//-----------------------------------------------------------------------------
VOID CXBoxSample::BeginFriends()
{
    // Standard init
    HRESULT hr = XOnlineFriendsStartup( NULL, &m_hFriendsTask );
    if( FAILED(hr) )
    {
        m_UI.SetErrorStr( L"Friends failed to initialize. Error %x", hr );
        Reset( TRUE );
    }
    
    // Query server for latest list of friends
    hr = XOnlineFriendsEnumerate( m_dwUserIndex, NULL, &m_hFriendEnumTask );
    if( FAILED(hr) )
    {
        m_UI.SetErrorStr( L"Friend enum failed to initialize. Error %x", hr );
        Reset( TRUE );
    }
    
    // Build the list of potential friends. The "potential" friend list is
    // contrived for this sample. A real game would typically allow the
    // player to select anybody they happened to be playing with and request
    // that person be their friend. In other words, your game should not
    // be doing this!
    //
    // The list of potential friends starts out as the list of all users
    // known by this particular Xbox.
    m_PotentialFriendList.clear();
    m_PotentialFriendList = m_UserList;
}




//-----------------------------------------------------------------------------
// Name: SetPlayerState()
// Desc: Broadcast current player state for the world
//-----------------------------------------------------------------------------
VOID CXBoxSample::SetPlayerState( DWORD dwState )
{
    BOOL bSuccess = XOnlineNotificationSetState( m_dwUserIndex, dwState,
        XNKID(), 0, NULL );
    assert( bSuccess );
    (VOID)bSuccess; // avoid compiler warning
}




//-----------------------------------------------------------------------------
// Name: IsNotificationPending()
// Desc: TRUE if notification is pending of the given type
//-----------------------------------------------------------------------------
BOOL CXBoxSample::IsNotificationPending( DWORD dwType )
{
    return XOnlineNotificationIsPending( m_dwUserIndex, dwType );
}




//-----------------------------------------------------------------------------
// Name: UpdateFriends()
// Desc: Make sure our local copy of the friend list reflects the server copy
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateFriends()
{
    HRESULT hr = m_hFriendEnumTask.Continue();
    // Enumeration failed
    if( FAILED( hr ) )
    {
        m_UI.SetErrorStr( L"Friend enumeration failed. Error %x", hr );
        Reset( TRUE );
        return;
    }
    
    // Update generic status
    if( IsNotificationPending( XONLINE_NOTIFICATION_TYPE_FRIENDREQUEST ) )
        SetStatus( L"You have received a friend request" );
    else if( IsNotificationPending( XONLINE_NOTIFICATION_TYPE_GAMEINVITE ) )
        SetStatus( L"You have received a game invitation" );
    else
        SetStatus( L"Friend list refreshed" );
    
    m_FriendList.reserve( MAX_FRIENDS );
    m_FriendList.resize( MAX_FRIENDS );
    XONLINE_FRIEND* pFriendList = &m_FriendList[0];
    DWORD dwNumFriends = XOnlineFriendsGetLatest( m_dwUserIndex,
        MAX_FRIENDS,
        pFriendList );
    m_FriendList.resize( dwNumFriends );
    
    // Resort
    std::sort( m_FriendList.begin(), m_FriendList.end(), FriendCompare() );
    
    // Cull the "potential" friend list accordingly
    m_PotentialFriendList.clear();
    m_PotentialFriendList = m_UserList; // XONLINE_USER
    
    // Remove any match between the two lists from the potential
    // friend list, where a match is a matching username
    for( DWORD i = 0; i < m_FriendList.size(); ++i )
    {
        XBUserList::iterator j = std::find_if( 
            m_PotentialFriendList.begin(),
            m_PotentialFriendList.end(),
            MatchUser( m_FriendList[i].username ) );
        if( j != m_PotentialFriendList.end() )
            m_PotentialFriendList.erase( j );
    }
    
    // Remove "ourself" from the list
    XBUserList::iterator j = std::find_if(
        m_PotentialFriendList.begin(),
        m_PotentialFriendList.end(),
        MatchUser( m_UserList[ m_dwCurrUser ].name ) );
    if( j != m_PotentialFriendList.end() )
        m_PotentialFriendList.erase( j );
}




//-----------------------------------------------------------------------------
// Name: FriendRequest()
// Desc: Request that a player be our friend
//-----------------------------------------------------------------------------
VOID CXBoxSample::FriendRequest()
{
    assert( m_dwCurrItem < m_PotentialFriendList.size() );
    HRESULT hr = XOnlineFriendsRequest( m_dwUserIndex, m_PotentialFriendList[ m_dwCurrItem ].xuid );
    assert( SUCCEEDED(hr) );
    (VOID)hr; // avoid compiler warnings
}




//-----------------------------------------------------------------------------
// Name: GameInvite()
// Desc: Invite a friend to the game
//-----------------------------------------------------------------------------
VOID CXBoxSample::GameInvite()
{
    XNKID SessionID = { 0 };
    
    // Only shown as an example; not currently implemented
    assert( m_dwCurrFriend < m_FriendList.size() );
    XONLINE_FRIEND* pFriend = &m_FriendList[ m_dwCurrFriend ];
    HRESULT hr = XOnlineFriendsGameInvite( m_dwUserIndex, SessionID, 1, pFriend );
    (VOID)hr; // avoid compiler warnings
}




//-----------------------------------------------------------------------------
// Name: AnswerRequest()
// Desc: Respond to friend request
//-----------------------------------------------------------------------------
VOID CXBoxSample::AnswerRequest( XONLINE_REQUEST_ANSWER_TYPE Answer )
{
    assert( m_dwCurrFriend < m_FriendList.size() );
    HRESULT hr = XOnlineFriendsAnswerRequest( m_dwUserIndex, &m_FriendList[ m_dwCurrFriend ], Answer );
    assert( SUCCEEDED(hr) );
    (VOID)hr; // avoid compiler warnings
}




//-----------------------------------------------------------------------------
// Name: AnswerGameInvite()
// Desc: Respond to friend request
//-----------------------------------------------------------------------------
VOID CXBoxSample::AnswerGameInvite( XONLINE_GAMEINVITE_ANSWER_TYPE Answer )
{
    assert( m_dwCurrFriend < m_FriendList.size() );
    HRESULT hr = XOnlineFriendsAnswerGameInvite( m_dwUserIndex, &m_FriendList[ m_dwCurrFriend ], Answer );
    assert( SUCCEEDED(hr) );
    (VOID)hr; // avoid compiler warnings
}




//-----------------------------------------------------------------------------
// Name: RemoveFriend()
// Desc: Remove friend from list
//-----------------------------------------------------------------------------
VOID CXBoxSample::RemoveFriend()
{
    assert( m_dwCurrFriend < m_FriendList.size() );
    XONLINE_FRIEND* pFriend = &m_FriendList[ m_dwCurrFriend ];
    HRESULT hr = XOnlineFriendsRemove( m_dwUserIndex, pFriend );
    assert( SUCCEEDED(hr) );
    (VOID)hr; // avoid compiler warnings
}




//-----------------------------------------------------------------------------
// Name: Reset()
// Desc: Prepare to restart the application at the front menu
//-----------------------------------------------------------------------------
VOID CXBoxSample::SetStatus( const WCHAR* strStatus )
{
    lstrcpynW( m_strStatus, strStatus, MAX_STATUS_STR );
    m_StatusTimer.StartZero();
}




//-----------------------------------------------------------------------------
// Name: Reset()
// Desc: Prepare to restart the application at the front menu
//-----------------------------------------------------------------------------
VOID CXBoxSample::Reset( BOOL bIsError )
{
    m_hFriendsTask.Close();
    m_hFriendEnumTask.Close();
    
    if( bIsError )
    {
        m_hOnlineTask.Close();        
        m_State = STATE_ERROR;
        m_NextState = STATE_SELECT_ACCOUNT;
        m_bIsLoggedOn = FALSE;
    }
    else
    {
        m_State = STATE_FRIEND_LIST;
    }
    
    m_dwCurrItem = 0;
    m_dwTopItem = 0;
}
