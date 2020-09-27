//-----------------------------------------------------------------------------
// File: ContentDownload.cpp
//
// Desc: Shows Xbox online content enumeration, download, installation
//       and removal.
//
// Hist: 08.08.01 - New for Aug M1 release
//       09.04.01 - Updated for Nov release; UI moved to UserInterface module
//       01.21.02 - Updated for Feb release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "ContentDownload.h"
#include "XBStorageDevice.h"
#include "XBMemUnit.h"
#include "XBVoice.h"
#include <cassert>




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD MAX_CONTENT_RESULTS   = 5;    // Don't request more than this number
const DWORD MAX_CONTENT_DISPLAYED = 5;    // Number to show on screen




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
    m_State             = STATE_SELECT_DEVICE;
    m_NextState         = STATE_SELECT_DEVICE;
    m_dwCurrItem        = 0;
    m_dwTopItem         = 0;
    m_dwCurrUser        = 0;
    m_dwUserIndex       = 0;
    m_EnumDevice        = XONLINEOFFERING_ENUM_DEVICE_DVD;
    m_bIsLoggedOn       = FALSE;
    m_dwCurrContent     = 0;
    m_pEnumBuffer       = NULL;
    m_fPercentComplete  = 0.0f;
    m_dwBlocksInstalled = 0;
    m_dwBlocksTotal     = 0;
    
    m_EnumParams.dwOfferingType = 0xffffffff; // All offering types
    m_EnumParams.dwBitFilter = 0xffffffff;    // All offers
    m_EnumParams.dwMaxResults = MAX_CONTENT_RESULTS; 
    m_EnumParams.dwDescriptionIndex = 0;
    
    *m_strUser = 0;
    m_pServices[0] = XONLINE_BILLING_OFFERING_SERVICE;
    
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
    // Check the physical connection
    if( !m_NetLink.IsActive() )
    {
        m_UI.SetErrorStr( L"This Xbox has lost its online connection" );
        m_hOnlineTask.Close();
        Reset( TRUE );
    }
    
    // Maintain our connection once we've logged on
    if( m_bIsLoggedOn )
    {
        HRESULT hr = m_hOnlineTask.Continue();    
        if( FAILED( hr ) )
        {
            m_UI.SetErrorStr( L"Connection was lost (error 0x%x). Must relogin", hr );
            m_hOnlineTask.Close();
            Reset( TRUE );
        }
    }
    
    Event ev = GetEvent();
    
    switch( m_State )
    {
    case STATE_CREATE_ACCOUNT:  UpdateStateCreateAccount( ev );   break;
    case STATE_SELECT_ACCOUNT:  UpdateStateSelectAccount( ev );   break;
    case STATE_GET_PIN:         UpdateStateGetPin( ev );          break;
    case STATE_LOGGING_ON:      UpdateStateLoggingOn( ev );       break;
    case STATE_SELECT_DEVICE:   UpdateStateSelectDevice( ev );    break;
    case STATE_ENUM_CONTENT:    UpdateStateEnumContent( ev );     break;
    case STATE_SELECT_CONTENT:  UpdateStateSelectContent( ev );   break;
    case STATE_CONTENT_INFO:    UpdateStateContentInfo( ev );     break;
    case STATE_INSTALL_CONTENT: UpdateStateInstallContent( ev );  break;
    case STATE_VERIFY_CONTENT:  UpdateStateVerifyContent( ev );   break;
    case STATE_CONFIRM_REMOVE:  UpdateStateConfirmRemove( ev );   break;
    case STATE_REMOVE_CONTENT:  UpdateStateRemoveContent( ev );   break;
    case STATE_ERROR:           UpdateStateError( ev );           break;
    default:                    assert( FALSE );                  break;
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
    case STATE_SELECT_DEVICE:
        m_UI.RenderSelectDevice( m_dwCurrItem );
        break;
    case STATE_ENUM_CONTENT:
        m_UI.RenderEnumContent();
        break;
    case STATE_SELECT_CONTENT:
        m_UI.RenderSelectContent( m_EnumDevice, m_ContentList, 
            m_dwCurrItem, m_dwTopItem );
        break;
    case STATE_CONTENT_INFO:
        {
            assert( m_dwCurrItem < m_ContentList.size() );
            const ContentInfo& contentInfo = m_ContentList[ m_dwCurrItem ];
            m_UI.RenderContentInfo( m_EnumDevice, contentInfo );
            break;
        }
    case STATE_INSTALL_CONTENT:
        m_UI.RenderInstallContent( m_fPercentComplete, 
            m_dwBlocksInstalled, m_dwBlocksTotal );
        break;
    case STATE_VERIFY_CONTENT:
        m_UI.RenderVerifyContent();
        break;
    case STATE_CONFIRM_REMOVE: 
        { 
            assert( m_dwCurrContent < m_ContentList.size() );
            const ContentInfo& contentInfo = m_ContentList[ m_dwCurrContent ];
            m_UI.RenderConfirmRemove( contentInfo, m_dwCurrItem );
            break;
        }
    case STATE_REMOVE_CONTENT:
        m_UI.RenderRemoveContent();
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
    
    // "X"
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
        return EV_BUTTON_X;
    
    // "Y"
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_Y ] )
        return EV_BUTTON_Y;
    
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
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        m_State = STATE_SELECT_DEVICE;
        m_dwCurrItem = 0;
        return;
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
// Desc: Spin during authentication
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateLoggingOn( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        {
            // Cancel the task
            m_hOnlineTask.Close();
            
            // Return to list of devices
            m_State = STATE_SELECT_DEVICE;
            m_bIsLoggedOn = FALSE;
            m_dwCurrItem = 0;
            
            return;
        }
    }
    
    HRESULT hr = m_hOnlineTask.Continue();
    if( FAILED( hr ) )
    {
        m_UI.SetErrorStr( L"Login failure (error 0x%x). Try again.", hr );
        m_hOnlineTask.Close();
        Reset( TRUE );
        return;
    }
    
    // Check login status; partial results indicate that login itself
    // has completed.
    if( hr == XONLINE_S_LOGON_CONNECTION_ESTABLISHED )
    {
        BOOL bSuccess = TRUE;
        HRESULT hrService = S_OK;
        
        // Check for general errors
        if( FAILED(hr) )
        {
            m_UI.SetErrorStr( L"Login failed with error 0x%x", hr );
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
                // Check for service errors            // Check for service errors
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
            
            // Begin enumerating content on the selected device
            m_State = STATE_ENUM_CONTENT;
            BeginEnum();
        }
        else
        {
            m_hOnlineTask.Close();
            Reset( TRUE );
        }
    }
}



//-----------------------------------------------------------------------------
// Name: UpdateStateSelectDevice()
// Desc: Handle enum device selection
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateSelectDevice( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        /// Begin enumerating content for the selected device
        m_EnumDevice = XONLINEOFFERING_ENUM_DEVICE( m_dwCurrItem );
        assert( m_EnumDevice < XONLINEOFFERING_ENUM_DEVICE_MAX );
        
        // TODO; this release doesn't yet support DVD enumeration
        if( m_EnumDevice == XONLINEOFFERING_ENUM_DEVICE_DVD )
        {
            m_UI.SetErrorStr( L"DVD enum not supported in this release." );
            Reset( TRUE );
            return;
        }
        
        // If the device is online, need to select account and login
        if( m_EnumDevice == XONLINEOFFERING_ENUM_DEVICE_ONLINE )
        {
            if( m_bIsLoggedOn )
            {
                m_State = STATE_ENUM_CONTENT;
                BeginEnum();
            }
            else
            {
                m_State = STATE_SELECT_ACCOUNT;
            }
            m_dwCurrItem = 0;
        }
        else
        {
            m_State = STATE_ENUM_CONTENT;
            BeginEnum();
        }
        
        break;
        
    case EV_UP:
        if( m_dwCurrItem == 0 )
            m_dwCurrItem = XONLINEOFFERING_ENUM_DEVICE_MAX - 1;
        else
            --m_dwCurrItem;
        break;
        
    case EV_DOWN:
        if( m_dwCurrItem == XONLINEOFFERING_ENUM_DEVICE_MAX - 1 )
            m_dwCurrItem = 0;
        else
            ++m_dwCurrItem;
        break;
    }
}



//-----------------------------------------------------------------------------
// Name: UpdateStateEnumContent()
// Desc: Spin in content enumeration
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateEnumContent( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        
        delete [] m_pEnumBuffer;
        m_pEnumBuffer = NULL;
        
        // Cancel the task
        // Return to list of devices
        m_hContentTask.Close();
        m_State = STATE_SELECT_DEVICE;
        m_dwCurrItem = 0;
        return;
    }
    
    HRESULT hr = m_hContentTask.Continue();
    if( hr != XONLINETASK_S_RUNNING )
    {
        // Handle pump errors
        if( FAILED(hr) )
        {
            delete [] m_pEnumBuffer;
            m_pEnumBuffer = NULL;
            m_UI.SetErrorStr( L"Enumeration failed with error 0x%x", hr );
            Reset( TRUE );
            return;
        }
        
        // Extract the results
        PXONLINEOFFERING_INFO* ppInfo;
        DWORD dwItems;
        BOOL bPartial;
        
        hr = XOnlineOfferingEnumerateGetResults( m_hContentTask,
            &ppInfo, &dwItems, &bPartial );
        
        // Handle errors
        if( FAILED(hr) )
        {
            m_UI.SetErrorStr( L"Enumeration failed with error 0x%x", hr  );
            Reset( TRUE );
            return;
        }
        
        // Save the results
        for( DWORD i = 0; i < dwItems; ++i )
            m_ContentList.push_back( ContentInfo( *ppInfo[i] ) );
        
        // If enumeration is not complete, continue enumerating
        if( bPartial )
        {
            // Enumeration is not complete, keep pumping for more results
            return;
        }
        
        // Enumeration is complete
        delete [] m_pEnumBuffer;
        m_pEnumBuffer = NULL;
        m_hContentTask.Close();
        m_State = STATE_SELECT_CONTENT;
        m_dwCurrItem = 0;
        m_dwTopItem = 0;
    }
}



//-----------------------------------------------------------------------------
// Name: UpdateStateSelectContent()
// Desc: Handle content selection
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateSelectContent( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        
        // If no items, return to device list
        if( m_ContentList.empty() )
        {
            Reset( FALSE );
            return;
        }
        
        // Display content detail
        m_dwCurrContent = m_dwCurrItem;
        m_State = STATE_CONTENT_INFO;
        break;
        
    case EV_BUTTON_X:
        
        if( !m_ContentList.empty() && 
            m_EnumDevice == XONLINEOFFERING_ENUM_DEVICE_HD )
        {
            // Remove content
            m_dwCurrContent = m_dwCurrItem;
            m_State = STATE_CONFIRM_REMOVE;
            m_dwCurrItem = CONFIRM_NO;
        }
        break;
        
    case EV_BUTTON_Y:
        
        if( !m_ContentList.empty() && 
            m_EnumDevice != XONLINEOFFERING_ENUM_DEVICE_HD )
        {
            // Install content
            m_dwCurrContent = m_dwCurrItem;
            m_State = STATE_INSTALL_CONTENT;
            BeginInstall();
        }
        break;
        
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        // Return to device menu
        Reset( FALSE );
        break;
        
    case EV_UP:
        if( m_ContentList.empty() )
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
        if( m_ContentList.empty() )
            break;
        
        // If we're at the bottom of the displayed list, shift the display
        if( m_dwCurrItem == m_dwTopItem + MAX_CONTENT_DISPLAYED - 1 )
        {
            if( m_dwTopItem + MAX_CONTENT_DISPLAYED < m_ContentList.size() )
                ++m_dwTopItem;
        }
        
        // Move to next item
        if( m_dwCurrItem < m_ContentList.size() - 1 )
            ++m_dwCurrItem;
        
        break;
    }
}



//-----------------------------------------------------------------------------
// Name: UpdateStateContentInfo()
// Desc: Handle content detail
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateContentInfo( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_X:
        if( m_EnumDevice == XONLINEOFFERING_ENUM_DEVICE_HD )
        {
            // Remove content
            m_State = STATE_CONFIRM_REMOVE;
            m_dwCurrItem = CONFIRM_NO;
        }
        break;
        
    case EV_BUTTON_Y:
        if( m_EnumDevice != XONLINEOFFERING_ENUM_DEVICE_HD )
        {
            // Install content
            m_State = STATE_INSTALL_CONTENT;
            BeginInstall();
        }
        break;
        
    case EV_BUTTON_A:
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        // Return to content list
        m_State = STATE_SELECT_CONTENT;
        break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateInstallContent()
// Desc: Spin during download/installation
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateInstallContent( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        
        // If a player expressly cancels an installation, it's up to the title
        // to handle content removal. However, if the installation
        // is aborted because of network failure or other reasons, it can
        // be resumed without requiring a full download. This is handled
        // automatically.
        
        m_hContentTask.Close();
        m_State = STATE_CONFIRM_REMOVE; // STATE_REMOVE_CONTENT;
        //BeginRemove();
        
        return;
    }
    
    // Determine the download progress
    DWORD dwPercent;
    ULONGLONG qwBytesInstalled;
    ULONGLONG qwBytesTotal;
    
    HRESULT hr = XOnlineContentInstallGetProgress( m_hContentTask,
        &dwPercent, &qwBytesInstalled, &qwBytesTotal);
    m_fPercentComplete = FLOAT(dwPercent) / 100.0f;
    
    // Convert bytes to blocks
    const ULONGLONG qwBlockSize = ULONGLONG( CXBStorageDevice::GetBlockSize() );
    m_dwBlocksInstalled = DWORD( ( qwBytesInstalled + (qwBlockSize-1) ) / 
        qwBlockSize );
    m_dwBlocksTotal = DWORD( ( qwBytesTotal + (qwBlockSize-1) ) / 
        qwBlockSize );
    
    hr = m_hContentTask.Continue();
    if( hr != XONLINETASK_S_RUNNING )
    {
        // Installation complete
        m_hContentTask.Close();
        
        // Handle errors
        if( FAILED(hr) )
        {
            m_UI.SetErrorStr( L"Installation failed with error 0x%x", hr  );
            m_State = STATE_ERROR;
            m_NextState = STATE_SELECT_CONTENT;
            return;
        }
        
        // Move to the verification phase
        BeginVerify();
        m_State = STATE_VERIFY_CONTENT;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateVerifyContent()
// Desc: Spin during content verification phase
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateVerifyContent( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        
        m_hContentTask.Close();
        m_State = STATE_SELECT_CONTENT;
        return;
    }
    
    HRESULT hr = m_hContentTask.Continue();
    if( hr != XONLINETASK_S_RUNNING)
    {
        // Verification complete
        m_hContentTask.Close();
        
        // Handle errors
        if( FAILED(hr) )
        {
            m_UI.SetErrorStr( L"Verification failed with error 0x%x", hr );
            m_State = STATE_ERROR;
            m_NextState = STATE_SELECT_CONTENT;
            return;
        }
        
        // Success!
        m_UI.SetErrorStr( L"Content installed" );
        Reset( TRUE );
    }
}



//-----------------------------------------------------------------------------
// Name: UpdateStateConfirmRemove()
// Desc: Confirm content removal
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateConfirmRemove( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        if( m_dwCurrItem == 0 ) // "Yes"
        {
            m_State = STATE_REMOVE_CONTENT;
            BeginRemove();
        }
        else // "No"
        {
            m_State = STATE_SELECT_CONTENT;
            m_dwCurrItem = 0;
            m_dwTopItem = 0;
        }
        break;
        
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        m_State = STATE_SELECT_CONTENT;
        m_dwCurrItem = 0;
        m_dwTopItem = 0;
        break;
        
    case EV_UP:
        if( m_dwCurrItem == 0 )
            m_dwCurrItem = CONFIRM_MAX - 1;
        else
            --m_dwCurrItem;
        break;
        
    case EV_DOWN:
        if( m_dwCurrItem == CONFIRM_MAX - 1 )
            m_dwCurrItem = 0;
        else
            ++m_dwCurrItem;
        break;
    }
}



//-----------------------------------------------------------------------------
// Name: UpdateStateRemoveContent()
// Desc: Spin during content removal
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateRemoveContent( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        
        // XOnlineTaskClose() will not cancel the removal process,
        // because content files would be "stranded," so cancelling is 
        // specifically not allowed during this phase.
        //
        // Removal is generally very fast, unless there are hundreds of
        // files to remove, so developers are encouraged to use packages
        // with a small number of files.
        break;
    }
    
    HRESULT hr = m_hContentTask.Continue();
    
    if( hr != XONLINETASK_S_RUNNING )
    {
        // Removal complete
        m_hContentTask.Close();
        
        // Handle errors
        if( FAILED(hr) )
        {
            m_UI.SetErrorStr( L"Remove failed with error 0x%x", hr );
            m_State = STATE_ERROR;
            m_NextState = STATE_SELECT_CONTENT;
            return;
        }
        
        // Success! Return to front end.
        Reset( FALSE );
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
    // If we're already logged on, go directly to content enumeration
    if( m_bIsLoggedOn )
    {
        // Begin enumerating content on the selected device
        m_State = STATE_ENUM_CONTENT;
        BeginEnum();
        return;
    }
    
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
        m_UI.SetErrorStr( L"Login failed to start. Error 0x%x", hr );
        Reset( TRUE );
    }
}




//-----------------------------------------------------------------------------
// Name: BeginEnum()
// Desc: Initiate the enumeration process
//-----------------------------------------------------------------------------
VOID CXBoxSample::BeginEnum()
{
    // Clear the existing list
    m_ContentList.clear();
    
    // Determine the buffer size required for enumeration
    DWORD dwBufferSize = XOnlineOfferingEnumerateMaxSize( &m_EnumParams, 0 ); 
    
    // Allocate the enumeration buffer
    assert( m_pEnumBuffer == NULL );
    m_pEnumBuffer = new BYTE [ dwBufferSize ];
    
    // Look for content created after this date. This is an optional parameter.
    // If NULL is passed, all items are enumerated.
    SYSTEMTIME SysTime = { 0 };
    SysTime.wYear = 2001;
    SysTime.wMonth = 1;
    SysTime.wDay = 1;
    BOOL bSuccess = SystemTimeToFileTime( &SysTime, &(m_EnumParams.ftActiveAfter) );
    assert( bSuccess );
    (VOID)bSuccess;
    
    // Initiate the enumeration on the selected device, using the
    // credentials of the user on the current controller
    HRESULT hr = XOnlineOfferingEnumerate( m_EnumDevice, 
        m_dwUserIndex, &m_EnumParams, 
        m_pEnumBuffer, dwBufferSize,
        NULL, &m_hContentTask );
    
    if( FAILED(hr) )
    {
        EndEnum();
        m_UI.SetErrorStr( L"Enumeration failed to start. Error 0x%x", hr );
        Reset( TRUE );
    }
}




//-----------------------------------------------------------------------------
// Name: BeginInstall()
// Desc: Initiate the download and installation process
//-----------------------------------------------------------------------------
VOID CXBoxSample::BeginInstall()
{
    // Clear progress bar
    m_fPercentComplete  = 0.0f;
    m_dwBlocksInstalled = 0;
    m_dwBlocksTotal     = 0;
    
    // Get the ID of the selected content
    assert( m_dwCurrContent < m_ContentList.size() );
    XONLINEOFFERING_ID id = m_ContentList[ m_dwCurrContent ].GetId();
    
    // Initiate the installation of the selected content
    HRESULT hr = XOnlineContentInstall( id, NULL, &m_hContentTask );
    
    if( FAILED(hr) )
    {
        m_hContentTask.Close();
        m_UI.SetErrorStr( L"Installation failed to start. Error 0x%x", hr );
        m_State = STATE_ERROR;
        m_NextState = STATE_SELECT_CONTENT;
    }
}




//-----------------------------------------------------------------------------
// Name: BeginVerify()
// Desc: Initiate the verification phase
//-----------------------------------------------------------------------------
VOID CXBoxSample::BeginVerify()
{
    // Get the ID of the selected content
    assert( m_dwCurrContent < m_ContentList.size() );
    XONLINEOFFERING_ID id = m_ContentList[ m_dwCurrContent ].GetId();
    
    // The verification buffer must be a minimum of two sectors in size.
    // If the buffer is not provided by the title, it will be automatically
    // created
    DWORD dwBufferSize = XGetDiskSectorSize( "U:\\" ) * 2;
    
    // Initiate the verification of the selected content
    HRESULT hr = XOnlineContentVerify( id, NULL, &dwBufferSize,
        NULL, &m_hContentTask );
    
    if( FAILED(hr) )
    {
        m_hContentTask.Close();
        m_UI.SetErrorStr( L"Verification failed to start. Error 0x%x", hr );
        m_State = STATE_ERROR;
        m_NextState = STATE_SELECT_CONTENT;
    }
}




//-----------------------------------------------------------------------------
// Name: BeginRemove()
// Desc: Initiate the removal process
//-----------------------------------------------------------------------------
VOID CXBoxSample::BeginRemove()
{
    // Get the ID of the selected content
    assert( m_dwCurrContent < m_ContentList.size() );
    XONLINEOFFERING_ID id = m_ContentList[ m_dwCurrContent ].GetId();
    
    // Initiate the removal of the selected content
    HRESULT hr = XOnlineContentRemove( id, NULL, &m_hContentTask );
    
    if( FAILED(hr) )
    {
        m_hContentTask.Close();
        m_UI.SetErrorStr( L"Removal failed to start. Error 0x%x", hr );
        m_State = STATE_ERROR;
        m_NextState = STATE_SELECT_CONTENT;
    }
}




//-----------------------------------------------------------------------------
// Name: EndEnum()
// Desc: Finish enum task
//-----------------------------------------------------------------------------
VOID CXBoxSample::EndEnum()
{
    m_hContentTask.Close();
    delete [] m_pEnumBuffer;
    m_pEnumBuffer = NULL;
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
// Name: Reset()
// Desc: Prepare to restart the application at the front menu
//-----------------------------------------------------------------------------
VOID CXBoxSample::Reset( BOOL bIsError )
{
    m_hContentTask.Close();
    if( bIsError )
    {
        m_State = STATE_ERROR;
        m_NextState = STATE_SELECT_DEVICE;
    }
    else
    {
        m_State = STATE_SELECT_DEVICE;
    }
    
    m_dwCurrItem = 0;
    m_dwTopItem  = 0;
    m_dwCurrContent = 0;
}
