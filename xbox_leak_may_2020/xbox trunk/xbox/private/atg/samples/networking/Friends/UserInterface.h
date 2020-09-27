//-----------------------------------------------------------------------------
// File: UserInterface.h
//
// Desc: Friends rendering functions
//
// Hist: 10.20.01 - Updated for Nov release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBOXFRIENDS_UI_H
#define XBOXFRIENDS_UI_H

#include "Common.h"
#include "XBOnline.h"




//-----------------------------------------------------------------------------
// Name: class UserInterface
// Desc: Main UI class for Friends sample
//-----------------------------------------------------------------------------
class UserInterface
{
    CXBOnlineUI m_UI;
    
    static const D3DCOLOR COLOR_NORMAL    = CXBOnlineUI::COLOR_NORMAL;
    static const D3DCOLOR COLOR_HIGHLIGHT = CXBOnlineUI::COLOR_HIGHLIGHT;
    static const D3DCOLOR COLOR_GREEN     = CXBOnlineUI::COLOR_GREEN;
    
public:
    
    explicit UserInterface( WCHAR* );
    
    HRESULT Initialize();
    
    // Accessors
    VOID __cdecl SetErrorStr( const WCHAR*, ... );
    
    // UI functions
    VOID RenderCreateAccount( BOOL bHasMachineAccount ) const;
    VOID RenderSelectAccount( DWORD, const XBUserList& ) const;
    VOID RenderGetPin( const CXBPinEntry&, const WCHAR* ) const;
    VOID RenderLoggingOn() const;
    VOID RenderFriendList( DWORD dwTopItem, DWORD dwCurrItem,
        const FriendList& friendList, 
        const WCHAR* strStatus, BOOL bCloaked ) const;
    VOID RenderActionMenu( DWORD dwCurrItem, const XONLINE_FRIEND* ) const;
    VOID RenderNewFriend( DWORD dwCurrItem, 
        const XBUserList& potentialFriendList ) const;
    VOID RenderConfirmRemove( DWORD dwCurrItem, const XONLINE_FRIEND* ) const;
    VOID RenderError() const;
    
private:
    
    static const WCHAR* GetGameName( DWORD );
    
    // Disabled
    UserInterface();
    UserInterface( const UserInterface& );
    
};

#endif // XBOXFRIENDS_UI_H
