//-----------------------------------------------------------------------------
// File: SimpleContentDownload.cpp
//
// Desc: Shows Xbox online content enumeration, download, installation
//       and removal.
//
// Hist: 11.09.01 - Created
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
#include "common.h"

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
};

#define NUM_HELP_CALLOUTS ( sizeof( g_HelpCallouts ) / sizeof( g_HelpCallouts[0] ) )

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD MAX_STATUS_STR = 64;

const DWORD MAX_CONTENT_RESULTS   = 5;    // Don't request more than this number


//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
public:
    
    // Number of services to authenticate
    static const DWORD NUM_SERVICES = 1;
    
    // Index into m_UserList of acount to use
    static const DWORD DEFAULT_USER_ACCOUNT = 0;     // Just use the first one
    
    // Controller to use for login
    static const DWORD DEFAULT_CONTROLLER = 0;       // Just use the first one
    
    CXBoxSample();
    
    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    
    HRESULT         Logon();
    HRESULT         EnumerateContent();
    HRESULT         DownloadContent();
    HRESULT         VerifyContent();
    HRESULT         RemoveContent();
    void __cdecl    SetStatus( const WCHAR*, ... );
    void            DisplayStatus(BOOL bWaitForKeyPress = FALSE);
private:
    CXBFont             m_Font;                    // Font object
    CXBOnlineTask       m_hLogonTask;             // Online task
    CXBNetLink          m_NetLink;                 // Network link checking
    XBUserList        m_UserList;                  // List of accounts
    WCHAR             m_strStatus[MAX_STATUS_STR]; // Logon/Connection status
    BOOL              m_bIsDone;                   // Finished logon attempt
    XNADDR            m_xnTitleAddress;   // XNet address of this machine/game
    CXBHelp           m_Help;             // Help object
    BOOL              m_bDrawHelp;        // TRUE to draw help screen  
    DWORD             m_pServices[NUM_SERVICES] ;  // services to authenticate
    ContentList       m_ContentList;      // list of content
    DWORD             m_dwContentIndex;   // Index of item to be downloaded
    
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
    m_bDrawHelp = FALSE;
    m_strStatus[0] = L'\0';
    m_bIsDone      = FALSE;
    m_pServices[0] = XONLINE_BILLING_OFFERING_SERVICE;
    m_dwContentIndex = 0;
    
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
    
    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;
    
    srand( GetTickCount() ); // for picking random content
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
// Name: DisplayStatus()
// Desc: Display Status
//-----------------------------------------------------------------------------
void CXBoxSample::DisplayStatus(BOOL bWaitForKeyPress)
{ 
    
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, 
        D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
        0x000A0A6A, 1.0f, 0L );
    
    m_Font.Begin();
    m_Font.DrawText(  64, 50, CXBOnlineUI::COLOR_NORMAL, L"SimpleContentDownload" );
    m_Font.DrawText(  64, 80, CXBOnlineUI::COLOR_NORMAL, m_strStatus );
    
    if( bWaitForKeyPress )
    {
        m_Font.DrawText(  64, 140, CXBOnlineUI::COLOR_NORMAL, 
            L"Press A to continue" );
    }
    
    m_Font.End();
    
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
    
    if( bWaitForKeyPress )
    {
        for(;;)
        {
            // Read the input for all connected gampads, looking for
            // one that has the "A" button pressed
            XBInput_GetInput( m_Gamepad );
            for( DWORD i=0; i<4; i++ )
            {
                if( m_Gamepad[i].hDevice && m_Gamepad[i].bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
                {
                    return;
                }
            }
        }
    }
    
}




//-----------------------------------------------------------------------------
// Name: Logon
// Desc: Performs user login and authentication
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Logon()
{
    
    
    // Initialize the network stack
    HRESULT hr = XBNet_OnlineInit( 0 );
    if( FAILED( hr ) )
    {
        SetStatus( L"Network Initialization Failed (error 0x%x)", 
            hr );
        return hr;
    }
    
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
    
    SetStatus( L"Starting Logon..." );
    DisplayStatus();
    
    // Initiate the login process. XOnlineTaskContinue() is used to poll
    // the status of the login.
    hr = XOnlineLogon( Controllers, m_pServices, NUM_SERVICES, 
        NULL, &m_hLogonTask );
    
    if( FAILED( hr ) )
    {
        SetStatus( L"Logon Failed (error 0x%x)", hr );
        return hr;
    }
    
    // Go into a loop, pumping the task and presenting status
    // information until we finish logging on
    do
    {
        DWORD dwElapsedSeconds = (DWORD) LogonTimer.GetElapsedSeconds();
        DWORD dwHours, dwMinutes, dwSeconds;
        
        dwHours   = dwElapsedSeconds / 3600;
        dwMinutes = ( dwElapsedSeconds - dwHours * 3600 ) / 60;
        dwSeconds = dwElapsedSeconds - ( dwHours * 3600 + dwMinutes * 60 );
        
        SetStatus( L"Logging On (%.2d:%.2d:%.2d)", dwHours, dwMinutes, dwSeconds ); 
        
        DisplayStatus();
        
        hr = m_hLogonTask.Continue();
    } while ( hr == XONLINETASK_S_RUNNING );
    
    if( hr != XONLINE_S_LOGON_CONNECTION_ESTABLISHED )
    {
        SetStatus( L"Authentication Failed (0x%x)", 
            hr );
        return hr;
    }

    // Next, check if the user was actually logged on
    PXONLINE_USER pLoggedOnUsers = XOnlineGetLogonUsers();

    assert( pLoggedOnUsers );

    hr = pLoggedOnUsers[ DEFAULT_CONTROLLER ].hr;

    if( FAILED( hr ) )
    {
        SetStatus( L"User Logon Failed (0x%x)", hr );
        return hr;
    }

    for( DWORD i = 0; i < NUM_SERVICES; ++i )
    {
        hr = XOnlineGetServiceInfo( m_pServices[i], NULL );
        
        if( FAILED( hr ) )
        {
            SetStatus( L"Service %d Unavailable (0x%x)",
                m_pServices[i], hr );
            return hr;
        }
    }

    
    // Notify the world of our state change
    DWORD dwState = XONLINE_FRIENDSTATE_FLAG_ONLINE;
    
    if( XBVoice_HasDevice() )
        dwState |= XONLINE_FRIENDSTATE_FLAG_VOICE;
    
    if(!XOnlineNotificationSetState(
        DEFAULT_CONTROLLER, dwState, XNKID(), 0, NULL ) )
    {
        SetStatus( L"Failed to Set Player Notification State." );
        return E_UNEXPECTED;
    }
    
    SetStatus( L"Logged On" );
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: EnumerateContent
// Desc: Enumerate Content
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::EnumerateContent()
{
    
    
    XONLINEOFFERING_ENUM_PARAMS EnumParams;
    
    // Look for content created after this date. This is an optional parameter.
    // If NULL is passed, all items are enumerated.
    SYSTEMTIME SysTime = { 0 };
    SysTime.wYear = 2001;
    SysTime.wMonth = 1;
    SysTime.wDay = 1;
    BOOL bSuccess = SystemTimeToFileTime( &SysTime, &EnumParams.ftActiveAfter );
    assert( bSuccess );
    (VOID)bSuccess;
    
    EnumParams.dwOfferingType = 0xffffffff; // All offering types
    EnumParams.dwBitFilter    = 0xffffffff;    // All offers
    EnumParams.dwMaxResults   = MAX_CONTENT_RESULTS; 
    EnumParams.dwDescriptionIndex = 0;
    
    // Determine the buffer size required for enumeration
    DWORD dwBufferSize = XOnlineOfferingEnumerateMaxSize( &EnumParams, 0 ); 
    
    CXBOnlineTask hContentTask;
    
    SetStatus( L"Enumerating Content..." );
    DisplayStatus();
    
    // Initiate the online enumeration
    HRESULT hr = XOnlineOfferingEnumerate( XONLINEOFFERING_ENUM_DEVICE_ONLINE, 
        DEFAULT_USER_ACCOUNT, &EnumParams, 
        NULL, dwBufferSize,
        NULL, &hContentTask );
    
    if( FAILED( hr ) )
    {
        SetStatus( L"XOnlineOfferingEnumerate Failed (error 0x%x)", hr );
        return hr;
    }
    
    CXBStopWatch EnumTimer(FALSE); // Elapsed time during enumeration
    
    // Start Logon timer (for display purposes)
    EnumTimer.Start();
    
    BOOL bPartial;
    
    for(;;)
    {
        DWORD dwElapsedSeconds = (DWORD) EnumTimer.GetElapsedSeconds();
        DWORD dwHours, dwMinutes, dwSeconds;
        
        dwHours   = dwElapsedSeconds / 3600;
        dwMinutes = ( dwElapsedSeconds - dwHours * 3600 ) / 60;
        dwSeconds = dwElapsedSeconds - ( dwHours * 3600 + dwMinutes * 60 );
        
        SetStatus( L"Enumerating (%.2d:%.2d:%.2d)", dwHours, dwMinutes, dwSeconds ); 
        
        DisplayStatus();     
        
        hr = m_hLogonTask.Continue(); 
        
        // Handle errors
        if( FAILED( hr ) )
        {
            SetStatus( L"Connection was lost (error 0x%x)", hr );
            break;
        }
        
        hr = hContentTask.Continue();
        
        if( hr != XONLINETASK_S_RUNNING )
        {
            // Handle errors
            if( FAILED( hr ) )
            {
                SetStatus( L"Enumeration Failed (error 0x%x)", hr );
                break;
            }
            
            // Extract the results
            PXONLINEOFFERING_INFO* ppInfo;
            DWORD dwItems;
            
            hr = XOnlineOfferingEnumerateGetResults( hContentTask,
                &ppInfo, &dwItems, &bPartial );
            
            // Handle errors
            if( FAILED(hr) )
            {
                SetStatus( L"Enumeration failed with error 0x%x", hr  );
                break;
            }
            
            // Save the results
            for( DWORD i = 0; i < dwItems; ++i )
            {
                m_ContentList.push_back( ContentInfo( *ppInfo[i] ) );
            }
            
            if( !bPartial )
            {
                break;
            }
        }
        
    }
    
    if( SUCCEEDED( hr ) )
    {
        DWORD dwSize = m_ContentList.size();
        if( dwSize == 0 )
        {
            hr = E_UNEXPECTED;
            SetStatus( L"No New Content" );
        }
        else
        {
            SetStatus( L"Enumeration Succeeded (%lu Items enumerated)", dwSize );
            DisplayStatus(TRUE);
        }
    }
    
    return hr;
    
    
}




//-----------------------------------------------------------------------------
// Name: DownloadContent
// Desc: Download Content
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DownloadContent()
{
    CXBOnlineTask hContentTask;
    
    // Pick a random entry...
    m_dwContentIndex = rand() % m_ContentList.size();
    
    XONLINEOFFERING_ID id = m_ContentList[ m_dwContentIndex ].GetId();
    
    SetStatus( L"Starting Download of Item %lu", id );
    DisplayStatus();
    
    // Initiate the installation of the selected content
    HRESULT hr = XOnlineContentInstall( id, NULL, &hContentTask );
    if( FAILED( hr ) )
    {
        SetStatus( L"XOnlineContentInstall Failed (error 0x%x)", hr );
        return hr;
    }
    
    // Pump the tasks...
    do
    {
        hr = m_hLogonTask.Continue(); 
        // Handle errors
        if( FAILED( hr ) )
        {
            SetStatus( L"Connection was lost (error 0x%x)", hr );
            return hr;
        }
        
        // Determine the download progress
        DWORD dwPercent;
        ULONGLONG ullBytesInstalled;
        ULONGLONG ullBytesTotal;
        hr = XOnlineContentInstallGetProgress( hContentTask,
            &dwPercent, &ullBytesInstalled, &ullBytesTotal );
        
        SetStatus( L"Downloading %lu ... %lu%% complete", id, 
            dwPercent );
        
        DisplayStatus();
        hr = hContentTask.Continue();
        
    } while ( hr == XONLINETASK_S_RUNNING );
    
    if( SUCCEEDED( hr ) )
    {
        
        char strDirectory[ MAX_PATH ];
        DWORD cbDirectory = sizeof( strDirectory );
        
        hr = XOnlineContentGetRootDirectory( id, FALSE, &cbDirectory, 
            strDirectory );
        if( FAILED( hr ) )
        {
            SetStatus( L"GetRootDirectory Failed (error 0x%x)", hr );
        }  
        else
        {
            
            WCHAR strUnicodeDirectory [ MAX_PATH ];
            XBUtil_GetWide( strDirectory , strUnicodeDirectory, 
                cbDirectory + 1 );
            
            SetStatus( L"Downloaded into %s", strUnicodeDirectory );
            DisplayStatus( TRUE );
        }
    }
    else
    {
        SetStatus( L"Download Failed (error 0x%x)", hr );       
    }
    
    return hr;
}




//-----------------------------------------------------------------------------
// Name: VerifyContent
// Desc: Verify Downloaded Content
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::VerifyContent()
{
    CXBOnlineTask hContentTask;
    
    XONLINEOFFERING_ID id = m_ContentList[ m_dwContentIndex ].GetId();
    
    // The verification buffer must be a minimum of two sectors in size.
    // If the buffer is not provided by the title, it will be automatically
    // created
    DWORD dwBufferSize = XGetDiskSectorSize( "U:\\" ) * 2;
    
    SetStatus( L"Verifying Content..." );
    DisplayStatus();
    
    // Initiate the verification of the selected content
    HRESULT hr = XOnlineContentVerify( id, NULL, &dwBufferSize,
        NULL, &hContentTask );
    
    if( FAILED( hr ) )
    {
        SetStatus( L"XOnlineContentVerify Failed (error 0x%x)", hr );
        return hr;
    }
    
    do
    {
        hr = m_hLogonTask.Continue();
        if( FAILED( hr ) )
        {
            SetStatus( L"Connection was lost (error 0x%x)", hr );
            return hr;          
        }
        hr = hContentTask.Continue(); 
    }  
    while ( hr == XONLINETASK_S_RUNNING );
    
    if( SUCCEEDED( hr ) )
    {
        SetStatus( L"Verification Succeeded" );
        DisplayStatus( TRUE );
    }
    else
    {
        SetStatus( L"Verification Failed (error 0x%x)", hr );
    }
    
    
    return hr;
    
} 




//-----------------------------------------------------------------------------
// Name: RemoveContent
// Desc: Remove Downloaded Content
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RemoveContent()
{
    CXBOnlineTask hContentTask;
    
    XONLINEOFFERING_ID id = m_ContentList[ m_dwContentIndex ].GetId();
    
    SetStatus( L"Removing Content..." );
    DisplayStatus();
    
    // Initiate the removal the selected content
    HRESULT hr = XOnlineContentRemove( id, NULL,  &hContentTask );
    
    if( SUCCEEDED( hr ) )
    {
        do
        {
            hr = m_hLogonTask.Continue();
            if( FAILED( hr ) )
            {
                SetStatus( L"Connection was lost (error 0x%x)", hr );
                return hr;          
            }
            hr = hContentTask.Continue(); 
        }  
        while ( hr == XONLINETASK_S_RUNNING );
        
        if( SUCCEEDED( hr ) )
        {
            SetStatus( L"Removal Succeeded" );
            DisplayStatus( TRUE );
        }
    }
    else
    {
        SetStatus( L"Content Removal Failed (error 0x%x)", hr );
    }
    
    
    return hr;
}




//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    HRESULT hr;
    
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }
    
    
    if(m_bIsDone)
    {
        // If we are logged on, pump the task
        if( m_hLogonTask.IsOpen() )
        {           
            
            hr = m_hLogonTask.Continue();
            if( FAILED( hr ) )
            {
                SetStatus( L"Connection was lost (0x%x)", hr );
            }
            
        }
        
        return S_OK;
    }
    
    hr = Logon();
    
    if( SUCCEEDED( hr ) )
    {
        hr = EnumerateContent();
    }
    
    if( SUCCEEDED( hr ) )
    {
        hr = DownloadContent();
    }
    
    if( SUCCEEDED( hr ) )
    {
        hr = VerifyContent();
    }
    
    if( SUCCEEDED( hr ) )
    {
        hr = RemoveContent();
    }
    
    if( FAILED( hr ) )
    {
        // clean up
        m_hLogonTask.Close();
    }
    
    // Toggle flag so that we don't attempt another logon
    m_bIsDone = TRUE;
    
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
    m_Font.DrawText(  64, 50, CXBOnlineUI::COLOR_NORMAL, 
        L"SimpleContentDownload" );
    m_Font.DrawText( 450, 50, CXBOnlineUI::COLOR_HIGHLIGHT, m_strFrameRate );
    m_Font.DrawText(  64, 80, CXBOnlineUI::COLOR_NORMAL, 
        m_strStatus );
    m_Font.End();
    
    // draw help
    if(m_bDrawHelp)
    {
        m_Help.Render(&m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS);
    }
    
    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
    
    return S_OK;
}

