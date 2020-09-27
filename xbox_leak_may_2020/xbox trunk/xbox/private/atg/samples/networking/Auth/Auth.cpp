//-----------------------------------------------------------------------------
// File: Auth.cpp
//
// Desc: Shows Xbox online authentication protocols.
//       Includes account creation, PIN entry, validation and logon.
//
// Hist: 08.08.01 - New for Aug M1 release 
//       10.12.01 - Updated for Nov release
//       01.21.02 - Updated for Feb release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Auth.h"
#include "XBMemUnit.h"
#include "XBVoice.h"
#include <cassert>




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
// Name: CXBoxSample()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
: 
CXBApplication(),
m_UI( m_strFrameRate )
{
    m_State       = STATE_SELECT_ACCOUNT;
    m_NextState   = STATE_SELECT_ACCOUNT;
    m_dwCurrItem  = 0;
    m_dwCurrUser  = 0;
    m_dwUserIndex = 0;
    
    // Add whatever services are appropriate for your title, but no
    // more. Each service requires additional authentication time
    // and network traffic.
    m_pServices[0] = XONLINE_MATCHMAKING_SERVICE;
    m_pServices[1] = XONLINE_BILLING_OFFERING_SERVICE;
    
    *m_strUser = 0;
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
    if( m_UserList.size() == 0 )
        m_State = STATE_CREATE_ACCOUNT;
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    if( !m_NetLink.IsActive() )
    {
        m_UI.SetErrorStr( L"This Xbox has lost its online connection" );
        m_State = STATE_ERROR;
        m_NextState = STATE_SELECT_ACCOUNT;
    }
    
    Event ev = GetEvent();
    
    switch( m_State )
    {
    case STATE_CREATE_ACCOUNT:  UpdateStateCreateAccount( ev ); break;
    case STATE_SELECT_ACCOUNT:  UpdateStateSelectAccount( ev ); break;
    case STATE_GET_PIN:         UpdateStateGetPin( ev );        break;
    case STATE_LOGGING_ON:      UpdateStateLoggingOn( ev );     break;
    case STATE_SUCCESS:         UpdateStateSuccess( ev );       break;
    case STATE_ERROR:           UpdateStateError( ev );         break;
    default:                    assert( FALSE );                break;
    }
    
    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
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
    case STATE_CREATE_ACCOUNT:
        m_UI.RenderCreateAccount( TRUE );
        break;
    case STATE_SELECT_ACCOUNT:
        m_UI.RenderSelectAccount( m_dwCurrItem, m_UserList );
        break;
    case STATE_GET_PIN:
        m_UI.RenderGetPin( m_PinEntry, m_strUser );
        break;
    case STATE_LOGGING_ON:
        m_UI.RenderLoggingOn();
        break;
    case STATE_SUCCESS:
        m_UI.RenderSuccess( m_ServiceInfoList );
        break;
    case STATE_ERROR:
        m_UI.RenderError();
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
    
    // "Back"
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        return EV_BUTTON_BACK;
    
    // Movement
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
        return EV_UP;
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
        return EV_DOWN;
    
    return EV_NULL;
}




//-----------------------------------------------------------------------------
// Name: UpdateStateCreateAccount()
// Desc: Allow player to launch account creation tool
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
        // If any MUs are inserted, update the user list
        // and go to account selection if there are any accounts
        DWORD dwInsertions;
        DWORD dwRemovals;
        if( CXBMemUnit::GetMemUnitChanges( dwInsertions, dwRemovals ) )
        {
            m_UserList.clear();
            XBOnline_GetUserList( m_UserList );
            if( !m_UserList.empty() )
                m_State = STATE_SELECT_ACCOUNT;
        }
        break;
       
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateSelectAccount()
// Desc: Allow player to choose account
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateSelectAccount( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        {
            // Save current account information
            m_dwCurrUser = m_dwCurrItem;
            
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
            (VOID)hr; // avoid compiler warning
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
// Desc: Allow player to enter PIN number
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateGetPin( Event )
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
// Desc: Authentication is underway
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateLoggingOn( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        {
            // Close the task (this cancels the logon process)
            m_hOnlineTask.Close();
            
            // Return to list of user accounts
            m_State = STATE_SELECT_ACCOUNT;
            return;
        }
    }
    
    HRESULT hr = m_hOnlineTask.Continue();
    
    if ( hr != XONLINETASK_S_RUNNING )
    {
        if ( hr != XONLINE_S_LOGON_CONNECTION_ESTABLISHED  )
        {
            m_hOnlineTask.Close();
            m_UI.SetErrorStr( L"Login Failed. Try again." );
            m_State = STATE_ERROR;
            m_NextState = STATE_SELECT_ACCOUNT;
            return;
        }
        
        BOOL bSuccess = TRUE;
        BOOL bServiceErr = FALSE;
        HRESULT hrService = S_OK;
        DWORD i = 0;
        
		// Next, check if the user was actually logged on
		PXONLINE_USER pLoggedOnUsers = XOnlineGetLogonUsers();
		
		assert( pLoggedOnUsers );
		
		hr = pLoggedOnUsers[ m_dwCurrUser ].hr;
		
		if( FAILED( hr ) )
		{
            m_hOnlineTask.Close();
			m_UI.SetErrorStr( L"User Logon Failed (0x%x)",	hr );
            m_State = STATE_ERROR;
            m_NextState = STATE_SELECT_ACCOUNT;
            return;
		}


        // Check for service errors
        for( i = 0; i < NUM_SERVICES; ++i )
        {
            if( FAILED( hrService = XOnlineGetServiceInfo(m_pServices[i],NULL) ) )
            {
                bSuccess = FALSE;
                bServiceErr = TRUE;
                break;
            }
        }
        
        // If no errors, login was successful
        m_State = bSuccess ? STATE_SUCCESS : STATE_ERROR;
        
        if( !bSuccess )
        {
            if( bServiceErr )
            {
                m_UI.SetErrorStr( L"Login failed.\n\n"
                    L"Error %x logging into service %d",
                    hrService, m_pServices[i] );
            }
            else
            {
                m_UI.SetErrorStr( L"Login failed.\n\n"
                    L"Error %x returned by "
                    L"XOnlineTaskContinue", hr );
            }
            m_State = STATE_ERROR;
            m_NextState = STATE_SELECT_ACCOUNT;
            m_hOnlineTask.Close();
        }
        else
        {
            m_ServiceInfoList.clear();
            for( i = 0; i < NUM_SERVICES; ++i )
            {
                // Stored service information for UI
                XONLINE_SERVICE_INFO serviceInfo;
                XOnlineGetServiceInfo( m_pServices[i], &serviceInfo );
                m_ServiceInfoList.push_back( serviceInfo );
            }
            
            // Notify the world of our state change
            DWORD dwState = XONLINE_FRIENDSTATE_FLAG_ONLINE;
            if( XBVoice_HasDevice() )
                dwState |= XONLINE_FRIENDSTATE_FLAG_VOICE;
            SetPlayerState( dwState );
        }
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateSuccess()
// Desc: Authentication is successful
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateSuccess( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        // A or START returns to begin
        m_State = STATE_SELECT_ACCOUNT;
        m_hOnlineTask.Close();
        return;
    }
    
    HRESULT hr = m_hOnlineTask.Continue();
    
    if( FAILED( hr ) )
    {
        m_hOnlineTask.Close();
        m_UI.SetErrorStr( L"Connection was lost. Must relogin" );
        m_State = STATE_ERROR;
        m_NextState = STATE_SELECT_ACCOUNT;
    }
    
}




//-----------------------------------------------------------------------------
// Name: UpdateStateError()
// Desc: An error occurred
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateError( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        // A or START exits
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
    assert( m_hOnlineTask == NULL );
    
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
        m_UI.SetErrorStr( L"Login failed to start.\n\n"
            L"Error %x returned by XOnlineLogon", hr );
        m_State = STATE_ERROR;
        m_NextState = STATE_SELECT_ACCOUNT;
    }
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
