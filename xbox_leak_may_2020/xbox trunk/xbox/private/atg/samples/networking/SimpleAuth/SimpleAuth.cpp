//-----------------------------------------------------------------------------
// File: SimpleAuth.cpp
//
// Desc: Shows Xbox online authentication protocols.
//
// Hist: 11.01.01 - Created for December Release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "xtl.h"
#include "xonline.h"
#include <vector>
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>
#include "XBNet.h"
#include "XBOnlineTask.h"
#include "XBOnline.h"
#include "XBVoice.h"
#include <cassert>


//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD MAX_STATUS_STR = 64;


//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
public:
    // Number of services to authenticate
    static const DWORD NUM_SERVICES = 2;
    
    // Index into m_UserList of acount to use
    static const DWORD DEFAULT_USER_ACCOUNT = 0;     // Just use the first one
    
    // Controller to use for login
    static const DWORD DEFAULT_CONTROLLER = 0;       // Just use the first one
    
    CXBoxSample();
    
    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    HRESULT         Logon();
    void __cdecl    SetStatus( const WCHAR*, ... );
    void            RenderLoginProgress( DWORD dwElapsedSeconds );
private:
    CXBFont             m_Font;                    // Font object
    CXBOnlineTask       m_hOnlineTask;             // Online task
    CXBNetLink          m_NetLink;                 // Network link checking
    DWORD             m_Services[ NUM_SERVICES ];  // List of desired services
    XBUserList        m_UserList;                  // List of accounts
    WCHAR             m_strStatus[MAX_STATUS_STR]; // Logon/Connection status
    BOOL              m_bIsDone;                   // Finished logon attempt
    XNADDR            m_xnTitleAddress;    // XNet address of this machine/game
    
};



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
// Name: CXBoxSample (constructor)
// Desc: Constructor for CXBoxSample class
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample() 
:CXBApplication()
{
    m_strStatus[0] = L'\0';
    m_bIsDone      = FALSE;
}



//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetStatus()
// Desc: Set status string
//-----------------------------------------------------------------------------
void __cdecl CXBoxSample::SetStatus( const WCHAR* strFormat, ... )
{
    va_list pArgList;
    va_start( pArgList, strFormat );
    
    INT iChars = wvsprintfW( m_strStatus, strFormat, pArgList );
    assert( iChars < MAX_STATUS_STR );
    (void)iChars; // avoid compiler warning
    
    va_end( pArgList );
}




//-----------------------------------------------------------------------------
// Name: RenderLoginProgress
// Desc: Render login progress
//-----------------------------------------------------------------------------
void CXBoxSample::RenderLoginProgress( DWORD dwElapsedSeconds )
{
    WCHAR strTime[40];
    DWORD dwHours, dwMinutes, dwSeconds;
    
    dwHours   = dwElapsedSeconds / 3600;
    dwMinutes = ( dwElapsedSeconds - dwHours * 3600 ) / 60;
    dwSeconds = dwElapsedSeconds - ( dwHours * 3600 + dwMinutes * 60 );
    
    swprintf( strTime, L"%.2d:%.2d:%.2d", dwHours, dwMinutes, dwSeconds );
    
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, 
        D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
        0x000A0A6A, 1.0f, 0L );
    
    m_Font.Begin();
    m_Font.DrawText(  64, 50, CXBOnlineUI::COLOR_NORMAL, L"SimpleAuth" );
    m_Font.DrawText(  64, 80, CXBOnlineUI::COLOR_NORMAL, L"Logging In" );
    m_Font.DrawText( 450, 50, CXBOnlineUI::COLOR_HIGHLIGHT, strTime );
    m_Font.End();
    
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}




//-----------------------------------------------------------------------------
// Name: Logon
// Desc: Performs user login and authentication
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Logon()
{
    
    // Add whatever services are appropriate for your title, but no
    // more. Each service requires additional authentication time
    // and network traffic.
    m_Services[0] = XONLINE_MATCHMAKING_SERVICE;
    m_Services[1] = XONLINE_BILLING_OFFERING_SERVICE;
    
    // Initialize the network stack
    HRESULT hr = XBNet_OnlineInit( 0 );
    if( FAILED( hr ) )
    {
        SetStatus( L"Network Initialization Failed (error 0x%x)", 
            hr );
        return hr;
    }
    
    // If no accounts, then player needs to create an account.
    // For development purposes, accounts are created using the OnlineSetup
    // tool in the XDK Launcher. For retail Xbox consoles, accounts are
    // created in the Xbox Dashboard.
    
    // Get information on all accounts for this Xbox
    hr = XBOnline_GetUserList( m_UserList );
    if( FAILED( hr ) )
    {
        SetStatus( L"Failed to Retrieve User Accounts (error 0x%x)", 
            hr );
        return hr;
    }
    
    // Make sure there is at least one user account
    if( m_UserList.empty() )
    {
        SetStatus( L"No User Accounts Found" );
        return E_UNEXPECTED;
    }
    
    // XOnlineLogon() allows a list of up to 4 players (1 per controller)
    // to login in a single call. This sample shows how to authenticate
    // a single user. The list must be a one-to-one match of controller 
    // to player in order for the online system to recognize which player
    // is using which controller.  For brevity, we arbitrarily select
    // an account (DEFAULT_PLAYER) and (DEFAULT_CONTROLLER) controller.
    XONLINE_USER Controllers[ XGetPortCount() ] = { 0 };
    
    // Copy account information to selected controller
    Controllers[ DEFAULT_CONTROLLER ] = m_UserList[ DEFAULT_USER_ACCOUNT ];
        
    CXBStopWatch LogonTimer(FALSE); // Elapsed time during login
    
    // Start Logon timer (for display purposes)
    LogonTimer.Start();
    
    // Initiate the login process. XOnlineTaskContinue() is used to poll
    // the status of the login.
    hr = XOnlineLogon( Controllers, m_Services, NUM_SERVICES, 
        NULL, &m_hOnlineTask );
    
    if( FAILED( hr ) )
    {
        SetStatus( L"Logon Failed (error 0x%x)", hr );
        return hr;
    }
    
    
    // Go into a loop, pumping the task and presenting status
    // information until we finish logging on
    do
    {
        RenderLoginProgress( (DWORD) LogonTimer.GetElapsedSeconds() );
        hr = m_hOnlineTask.Continue();
    } while ( hr == XONLINETASK_S_RUNNING );
    
    if( hr != XONLINE_S_LOGON_CONNECTION_ESTABLISHED )
    {
        SetStatus( L"Authentication Failed (0x%x)", 
            hr );
        return hr;
    }   // Check to make sure the services that were requested are available
    
    // Next, check if the user was actually logged on
    PXONLINE_USER pLoggedOnUsers = XOnlineGetLogonUsers();

    assert( pLoggedOnUsers );

    hr = pLoggedOnUsers[ DEFAULT_CONTROLLER ].hr;

    if( FAILED( hr ) )
    {
        SetStatus( L"User Logon Failed (0x%x)", hr );
        return hr;
    }


    // Finally, check to make sure all requested services were available
    for(DWORD i = 0; i < NUM_SERVICES; ++i )
    {
        hr = XOnlineGetServiceInfo( m_Services[i], NULL );
        
        if( FAILED( hr ) )
        {
            SetStatus( L"Service %lu unavailable (0x%x)",
                m_Services[i], hr );
            return hr;
        }
    }
    
    // Notify our friends of our state change
    DWORD dwState = XONLINE_FRIENDSTATE_FLAG_ONLINE;
    
    if( XBVoice_HasDevice() )
        dwState |= XONLINE_FRIENDSTATE_FLAG_VOICE;
    
    if( !XOnlineNotificationSetState(
        DEFAULT_CONTROLLER, dwState, XNKID(), 0, NULL ) )
    {
        SetStatus( L"Failed to Set Player Notification State." );
        return E_UNEXPECTED;
    }
    
    SetStatus( L"Logged On" );
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    HRESULT hr;
    
    if(m_bIsDone)
    {
        // If we are logged on, pump the task
        if( m_hOnlineTask.IsOpen() )
        {                       
            hr  = m_hOnlineTask.Continue();
            if( FAILED( hr ) )
            {
                SetStatus( L"Connection was lost (0x%x)", hr );
            }           
        }
        
    }
    else
    {
        
        hr = Logon();
        
        if( FAILED( hr ) )
        {        
            m_hOnlineTask.Close();
        }
        
        // Toggle flag so that we don't attempt another logon
        m_bIsDone = TRUE;
    }
    
    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, 
        D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
        0x000A0A6A, 1.0f, 0L );
    
    m_Font.Begin();
    m_Font.DrawText(  64, 50, CXBOnlineUI::COLOR_NORMAL,     L"SimpleAuth" );
    m_Font.DrawText( 450, 50, CXBOnlineUI::COLOR_HIGHLIGHT, m_strFrameRate );
    m_Font.DrawText(  64, 80, CXBOnlineUI::COLOR_NORMAL, 
        m_strStatus );
    m_Font.End();
    
    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

