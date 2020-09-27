//-----------------------------------------------------------------------------
// File: UserInterface.cpp
//
// Desc: Auth rendering functions
//
// Hist: 10.12.01 - New for Nov release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "UserInterface.h"
#include "Resource.h"
#include "XBSockAddr.h"
#include <cassert>




//-----------------------------------------------------------------------------
// Name: UserInterface()
// Desc: Constructor
//-----------------------------------------------------------------------------
UserInterface::UserInterface( WCHAR* strFrameRate )
:   
    m_UI( strFrameRate, L"Auth" )
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
// Name: RenderLoggingOn()
// Desc: Display "logging on" animation
//-----------------------------------------------------------------------------
VOID UserInterface::RenderLoggingOn() const
{
    m_UI.RenderLoggingOn();
}




//-----------------------------------------------------------------------------
// Name: RenderSuccess()
// Desc: Successful authentication
//-----------------------------------------------------------------------------
VOID UserInterface::RenderSuccess( const ServiceInfoList& InfoList ) const
{
    m_UI.RenderHeader();

    m_UI.DrawText( 320, 160, COLOR_NORMAL, L"Successfully logged into "
                   L"services at:", XBFONT_CENTER_X );

    FLOAT fYtop = 220.0f;
    FLOAT fYdelta = 30.0f;

    // Display information about services
    DWORD j = 0;
    for( ServiceInfoList::const_iterator i = InfoList.begin(); 
         i != InfoList.end(); ++i, ++j )
    {
        XONLINE_SERVICE_INFO serviceInfo = *i;
        CXBSockAddr sa( serviceInfo.serviceIP.s_addr, serviceInfo.wServicePort );

        WCHAR strServiceInfo[32];
        sa.GetStr( strServiceInfo );

        m_UI.DrawText( 320, fYtop + (fYdelta * j ), COLOR_NORMAL,
                       strServiceInfo, XBFONT_CENTER_X );
    }

    m_UI.DrawText( 320, fYtop + (fYdelta * (j + 1) ), COLOR_NORMAL, 
                   L"Press A to logoff", 
                   XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: RenderError()
// Desc: Display error message
//-----------------------------------------------------------------------------
VOID UserInterface::RenderError() const
{
    m_UI.RenderError();
}
