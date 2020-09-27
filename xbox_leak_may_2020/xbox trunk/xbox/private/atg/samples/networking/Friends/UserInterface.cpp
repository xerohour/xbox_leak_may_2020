//-----------------------------------------------------------------------------
// File: UserInterface.cpp
//
// Desc: Friends rendering functions
//
// Hist: 10.20.01 - Updated for Nov release
//       01.21.02 - Updated for Feb release
//       02.15.02 - Updated for Mar release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "UserInterface.h"
#include "Resource.h"
#include <cassert>




//-----------------------------------------------------------------------------
// Name: UserInterface()
// Desc: Constructor
//-----------------------------------------------------------------------------
UserInterface::UserInterface( WCHAR* strFrameRate )
:
m_UI( strFrameRate, L"Friends" )
{
}




//-----------------------------------------------------------------------------
// Name: SetErrorStr()
// Desc: Set error string
//-----------------------------------------------------------------------------
VOID __cdecl UserInterface::SetErrorStr( const WCHAR* strFormat, ... )
{
    va_list pArgList;
    va_start( pArgList, strFormat );
    
    m_UI.SetErrorStr( strFormat, pArgList );
    
    va_end( pArgList );
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects
//-----------------------------------------------------------------------------
HRESULT UserInterface::Initialize()
{
    return m_UI.Initialize( Resource_NUM_RESOURCES, Resource_MenuSelect_OFFSET );
}




//-----------------------------------------------------------------------------
// Name: RenderCreateAccount()
// Desc: Allow player to launch account creation tool
//-----------------------------------------------------------------------------
VOID UserInterface::RenderCreateAccount( BOOL bHasMachineAccount ) const
{
    m_UI.RenderCreateAccount( bHasMachineAccount );
}




//-----------------------------------------------------------------------------
// Name: RenderSelectAccount()
// Desc: Display list of accounts
//-----------------------------------------------------------------------------
VOID UserInterface::RenderSelectAccount( DWORD dwCurrItem, 
                                        const XBUserList& UserList ) const
{
    m_UI.RenderSelectAccount( dwCurrItem, UserList );
}




//-----------------------------------------------------------------------------
// Name: RenderGetPin()
// Desc: Display PIN entry screen
//-----------------------------------------------------------------------------
VOID UserInterface::RenderGetPin( const CXBPinEntry& PinEntry, 
                                 const WCHAR* strUser ) const
{
    m_UI.RenderGetPin( PinEntry, strUser );
}




//-----------------------------------------------------------------------------
// Name: RenderLogginOn()
// Desc: Display login message
//-----------------------------------------------------------------------------
VOID UserInterface::RenderLoggingOn() const
{
    m_UI.RenderLoggingOn();
}




//-----------------------------------------------------------------------------
// Name: RenderFriendList()
// Desc: Display current list of friends
//-----------------------------------------------------------------------------
VOID UserInterface::RenderFriendList( DWORD dwTopItem, DWORD dwCurrItem,
                                    const FriendList& friendList,
                                    const WCHAR* strStatus,
                                    BOOL bCloaked ) const
{
    m_UI.RenderHeader();
    
    if( friendList.empty() )
    {
        m_UI.DrawText( 320, 140, COLOR_NORMAL, L"No friends",
            XBFONT_CENTER_X );
        m_UI.DrawText( 320, 180, COLOR_NORMAL, L"Press Y to add friend", 
            XBFONT_CENTER_X );
        m_UI.DrawText( 320, 220, COLOR_NORMAL, L"Press B to cancel", 
            XBFONT_CENTER_X );
        return;
    }
    
    m_UI.DrawText( 320, 70, COLOR_NORMAL, L"Friend List",
        XBFONT_CENTER_X );
    
    const FLOAT fYtop = 120.0f;
    const FLOAT fYdelta = 30.0f;
    
    DWORD j = 0;
    for( DWORD i = dwTopItem; i < friendList.size() &&
        j < MAX_FRIENDS_DISPLAYED; ++i, ++j )
    {
        DWORD dwColor = ( dwCurrItem == i ) ? COLOR_HIGHLIGHT : COLOR_NORMAL;
        if( dwCurrItem == i )
        {
            // Show selected item with little triangle
            m_UI.RenderMenuSelector( 160.0f, fYtop + (fYdelta * j ) );
        }
        
        // Convert user name to wide string
        WCHAR strUser[ XONLINE_USERNAME_SIZE ];
        XBUtil_GetWide( friendList[i].username, strUser, XONLINE_USERNAME_SIZE );
        
        m_UI.DrawText( 200, fYtop + (fYdelta * j), dwColor, strUser );
    }
    
    // Show ultra-sexy scroll arrows
    BOOL bShowTopArrow = dwTopItem > 0;
    BOOL bShowBtmArrow = dwTopItem + MAX_FRIENDS_DISPLAYED < friendList.size();
    if( bShowTopArrow )
        m_UI.DrawText( 170, 100, COLOR_GREEN, L"/\\" );
    if( bShowBtmArrow )
        m_UI.DrawText( 170, 270, COLOR_GREEN, L"\\/" );
    
    // Friend status
    if( !friendList.empty() )
    {
        assert( dwCurrItem < friendList.size() );
        const XONLINE_FRIEND* pFriend = &friendList[dwCurrItem];
        DWORD dwState = pFriend->friendState;
        WCHAR strState[256] = { 0 };
        
        if (  ( dwState & XONLINE_FRIENDSTATE_FLAG_ONLINE ) &&
            !( dwState & XONLINE_FRIENDSTATE_FLAG_CLOAKED ) )
        {
            if( dwState & XONLINE_FRIENDSTATE_FLAG_PLAYING )
			{
				if( dwState & XONLINE_FRIENDSTATE_FLAG_JOINABLE )
				{
					wsprintfW( strState, L"Online playing %.*s (joinable)", 128, 
						GetGameName( pFriend->titleID ) );
				}
				else
					wsprintfW( strState, L"Online playing %.*s", 128, 
						GetGameName( pFriend->titleID ) );
			}
            else 
                wsprintfW( strState, L"Online in %.*s", 128, 
                GetGameName( pFriend->titleID ) );
        }
        else
            lstrcpyW( strState, L"Offline" );
        
        if( dwState & XONLINE_FRIENDSTATE_FLAG_REQUEST )
            lstrcatW( strState, L"\nHas asked you to accept a friend request" );
        else if( dwState & XONLINE_FRIENDSTATE_FLAG_PENDING )
            lstrcatW( strState, L"\nRequest has been sent to friend" );
        else 
        {
            if( dwState & XONLINE_FRIENDSTATE_FLAG_SENTINVITE )
            {
                if( dwState & XONLINE_FRIENDSTATE_FLAG_INVITEACCEPTED )
                    lstrcatW( strState, 
                        L"\nHas accepted your invitation to play" );
                else if( dwState & XONLINE_FRIENDSTATE_FLAG_INVITEREJECTED )                        
                    lstrcatW( strState, 
                        L"\nHas declined your invitation to play" );
                else
                    lstrcatW( strState, 
                        L"\nGame invitation has been sent to friend" );
            }
            
            if( dwState & XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE )
                lstrcatW( strState, 
                    L"\nYou have received an invitaton to play" );
        }

        m_UI.DrawText( 320, 290, COLOR_GREEN, strState, XBFONT_CENTER_X );
    }
    
    // External status
    m_UI.DrawText( 320, 380, COLOR_GREEN, strStatus, XBFONT_CENTER_X );
    
    // Button descriptions
    m_UI.DrawText(  80, 410, COLOR_NORMAL, L"A  Actions" );
    m_UI.DrawText( 200, 410, COLOR_NORMAL, L"Y  New Friend" );
    
    if( bCloaked )
    {
        m_UI.DrawText( 360, 410, COLOR_NORMAL, L"Black  Appear Online" );
    }
    else
    {
        m_UI.DrawText( 360, 410, COLOR_NORMAL, L"Black  Appear Offline" );
    }
}




//-----------------------------------------------------------------------------
// Name: RenderActionMenu()
// Desc: Display invite/join/remove menu
//-----------------------------------------------------------------------------
VOID UserInterface::RenderActionMenu( DWORD dwCurrItem, 
                                     const XONLINE_FRIEND* pFriend ) const
{
    m_UI.RenderHeader();
    
    FLOAT fYtop = 140.0f;
    FLOAT fYdelta = 60.0f;
    
    const WCHAR* const strMenu[] =
    {
            L"Invite %.*s to play this game",
            L"Join %.*s in %.*s",
            L"Decline to join %.*s in %.*s",
            L"Remove %.*s from friend list",
            L"Accept friend request",
            L"Decline friend request",
            L"Never",
            L"Remove from friend list",
    };
    
    // Determine what menu items to display based on the friend state
    assert( pFriend != NULL );
    BOOL bJoinable = pFriend->friendState & XONLINE_FRIENDSTATE_FLAG_PLAYING;
    BOOL bFriendRequest = pFriend->friendState & XONLINE_FRIENDSTATE_FLAG_REQUEST;
    BOOL bGameJoinInvite   = bJoinable && 
        ( pFriend->friendState & XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE );
    
    // Display menu
    DWORD j = 0;
    for( DWORD i = 0; i < ACTION_MAX; ++i )
    {
        DWORD dwColor = ( dwCurrItem == i ) ? COLOR_HIGHLIGHT : COLOR_NORMAL;
        
        const WCHAR* strGameName = GetGameName( pFriend->titleID );
        
        // Convert user name to wide string
        WCHAR strUser[ XONLINE_USERNAME_SIZE ];
        XBUtil_GetWide( pFriend->username, strUser, XONLINE_USERNAME_SIZE );
        
        WCHAR strItem[ 32 + XONLINE_USERNAME_SIZE ];
        wsprintfW( strItem, strMenu[i], XONLINE_USERNAME_SIZE, strUser,
            16, strGameName );
        
        // Determine whether or not to display this menu item
        BOOL bDisplay = FALSE;
        switch( i )
        {
        case ACTION_INVITE: 
             bDisplay = !bFriendRequest && !bGameJoinInvite; break;
        case ACTION_JOIN_GAME_ACCEPT:   bDisplay = bGameJoinInvite; break;
        case ACTION_JOIN_GAME_DECLINE:  bDisplay = bGameJoinInvite; break;
        case ACTION_JOIN_GAME_REMOVE:   bDisplay = bGameJoinInvite; break;
        case ACTION_INVITATION_ACCEPT:  bDisplay = bFriendRequest;  break;
        case ACTION_INVITATION_DECLINE: bDisplay = bFriendRequest;  break;
        case ACTION_INVITATION_BLOCK:   bDisplay = bFriendRequest;  break;
        case ACTION_REMOVE:  bDisplay = !bFriendRequest && !bGameJoinInvite; break;
        }
        
        if( bDisplay )
        {
            m_UI.DrawText( 160, fYtop + (fYdelta * j), dwColor, strItem );
            if( dwCurrItem == i )
            {
                // Show selected item with little triangle
                m_UI.RenderMenuSelector( 120.0f, fYtop + (fYdelta * j ) );
            }
            ++j;
        }
    }
    
}




//-----------------------------------------------------------------------------
// Name: RenderNewFriend()
// Desc: Display list of potential new friends
//-----------------------------------------------------------------------------
VOID UserInterface::RenderNewFriend( DWORD dwCurrItem,
                                   const XBUserList& potentialFriendList ) const
{
    m_UI.RenderHeader();
    
    if( potentialFriendList.empty() )
    {
        m_UI.DrawText( 320, 140, COLOR_NORMAL, 
            L"You have exhausted the list of potential friends.\n\n"
            L"Everybody is your friend!\n\n"
            L"Press B to cancel",
            XBFONT_CENTER_X );
        return;
    }
    
    m_UI.DrawText( 320, 140, COLOR_NORMAL, L"Potential new friends",
        XBFONT_CENTER_X );
    
    FLOAT fYtop = 220.0f;
    FLOAT fYdelta = 30.0f;
    
    // Show list of potential friends
    for( DWORD i = 0; i < potentialFriendList.size(); ++i )
    {
        DWORD dwColor = ( dwCurrItem == i ) ? COLOR_HIGHLIGHT : COLOR_NORMAL;
        
        // Convert user name to WCHAR string
        WCHAR strUser[ XONLINE_NAME_SIZE ];
        XBUtil_GetWide( potentialFriendList[i].name, strUser, 
            XONLINE_NAME_SIZE );
        
        m_UI.DrawText( 160, fYtop + (fYdelta * i), dwColor, strUser );
    }
    
    // Show selected item with little triangle
    m_UI.RenderMenuSelector( 120.0f, fYtop + (fYdelta * dwCurrItem ) );
    
    // Button descriptions
    m_UI.DrawText(  80, 410, COLOR_NORMAL, L"A  Request" );
    m_UI.DrawText( 360, 410, COLOR_NORMAL, L"B  Back" );
}




//-----------------------------------------------------------------------------
// Name: RenderConfirmRemove()
// Desc: Display confirmation dialog for friend removal
//-----------------------------------------------------------------------------
VOID UserInterface::RenderConfirmRemove( DWORD dwCurrItem, 
                                        const XONLINE_FRIEND* pFriend ) const
{
    m_UI.RenderHeader();
    
    // Convert user name to wide string
    assert( pFriend != NULL );
    WCHAR strUser[ XONLINE_USERNAME_SIZE ];
    XBUtil_GetWide( pFriend->username, strUser, XONLINE_USERNAME_SIZE );
    
    // Build confirmation string
    WCHAR strConfirm[ XONLINE_USERNAME_SIZE + 64 ];
    wsprintfW( strConfirm, L"Are you sure you want to remove\n"
        L"'%.*s' from your friend list?", XONLINE_USERNAME_SIZE,
        strUser );
    
    m_UI.DrawText( 320, 140, COLOR_NORMAL, strConfirm, XBFONT_CENTER_X );
    
    const WCHAR* const strMenu[] =
    {
        L"Yes",
        L"No",
    };
    
    const FLOAT fYtop = 240.0f;
    const FLOAT fYdelta = 50.0f;
    
    for( DWORD i = 0; i < CONFIRM_REMOVE_MAX; ++i )
    {
        DWORD dwColor = ( dwCurrItem == i ) ? COLOR_HIGHLIGHT : COLOR_NORMAL;
        m_UI.DrawText( 280, fYtop + (fYdelta * i), dwColor, strMenu[i] );
    }
    
    // Show selected item with little triangle
    m_UI.RenderMenuSelector( 240.0f, fYtop + (fYdelta * dwCurrItem ) );
}




//-----------------------------------------------------------------------------
// Name: RenderError()
// Desc: Display error message
//-----------------------------------------------------------------------------
VOID UserInterface::RenderError() const
{
    m_UI.RenderError();
}




//-----------------------------------------------------------------------------
// Name: GetGameName()
// Desc: Extract game name given title ID
//-----------------------------------------------------------------------------
const WCHAR* UserInterface::GetGameName( DWORD dwTitleID ) // static
{
    // In future versions of the XOnline API, game name will be available.
    // For now, there's just a simple mapping for each of the online samples.
    
    switch( dwTitleID )
    {
    case 0xffff010b: return L"Auth";
    case 0xffff010c: return L"ContentDownload";
    case 0xffff010d: return L"MatchMaking";
    case 0xffff0111: return L"Friends";
    case 0xffff0112: return L"SimpleAuth";
    case 0xffff0113: return L"SimpleContentDownload";
    default:         return L"Unknown";
    }
}
