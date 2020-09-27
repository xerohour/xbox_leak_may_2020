//-----------------------------------------------------------------------------
// File: MatchMaking.cpp
//
// Desc: Illustrates online matchmaking on Xbox.
//       Allows player to find/create a new multiplayer game, using
//       either the QuickMatch or OptiMatch metaphor.
//       Once game has been chosen, allows players to connect and send
//       messages to other players.
//
// Hist: 08.08.01 - New for Aug M1 release 
// Hist: 01.16.02 - Updated for Feb release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "MatchMaking.h"
#include "XBMemUnit.h"
#include "XBVoice.h"
#include <cassert>
#pragma warning( disable: 4355 )




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD MAX_SESSION_NAMES = 6;      // Max names to choose from
const FLOAT PLAYER_HEARTBEAT  = 3.0f;   // every 3 seconds
const DWORD PLAYER_TIMEOUT    = 12000;  // 12 seconds
const FLOAT GAME_JOIN_TIME    = 5.0f;   // 5 seconds
const DWORD MAX_RESULTS       = 5;      // Max sessions to get
const DWORD MAX_RESULTS_LEN   = 2048;   // Results buffer size



// The following search ID has been custom-designed specifically
// for this sample. IT ONLY WORKS FOR THIS SAMPLE. For maximum speed
// and flexibility, game developers will create and establish their
// own search IDs in conjunction with Xbox Developer Support.
// These search functions will be hosted on the Xbox matchmaking
// servers and allow game developers to tailor session searching to
// the requirements of their game.
const DWORD SEARCH_ID_CUSTOM_1 = 1;

// Session attributes
const DWORD XATTRIB_TYPE  = X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC |
X_ATTRIBUTE_DATATYPE_INTEGER     | 0x0001;
const DWORD XATTRIB_LEVEL = X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC |
X_ATTRIBUTE_DATATYPE_STRING      | 0x0002;
const DWORD XATTRIB_SNAME = X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC |
X_ATTRIBUTE_DATATYPE_STRING      | 0x0003;
const DWORD XATTRIB_STYLE = X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC |
X_ATTRIBUTE_DATATYPE_BLOB        | 0x0004;
const DWORD XATTRIB_PNAME = X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC |
X_ATTRIBUTE_DATATYPE_STRING      | 0x0005;




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
// Name: class MatchInAddr
// Desc: Predicate functor used to match on IN_ADDRs in player lists
//-----------------------------------------------------------------------------
struct MatchInAddr
{
    IN_ADDR ia;
    
    explicit MatchInAddr( IN_ADDR inaddr )
        : 
    ia( inaddr )
    {
    }
    
    bool operator()( const CXBNetPlayerInfo& playerInfo ) const
    {
        return playerInfo.inAddr.s_addr == ia.s_addr;
    }
};




//-----------------------------------------------------------------------------
// Name: SessionInfo
// Desc: Default Constructor
//-----------------------------------------------------------------------------
SessionInfo::SessionInfo()
{
    ZeroMemory( &m_xms, sizeof( m_xms ) );
    
    m_Attributes.m_qwGameType = ULONGLONG(-1);
    
    *m_Attributes.m_strPlayerLevel = 0;
    *m_Attributes.m_strSessionName = 0;
    
    ZeroMemory( m_Attributes.m_GameStyle, MAX_STYLE_BLOB );
    m_Attributes.m_wStyleLen = 0;
}




//-----------------------------------------------------------------------------
// Name: SessionInfo
// Desc: Constructor
//-----------------------------------------------------------------------------
SessionInfo::SessionInfo( XMATCH_SEARCHRESULT& xms, 
                         const CXBOnlineTask& hSearch, DWORD dwResult )
                         :
m_xms( xms )
{
    m_Attributes.m_qwGameType = ULONGLONG(-1);
    
    *m_Attributes.m_strPlayerLevel = 0;
    *m_Attributes.m_strSessionName = 0;
    *m_Attributes.m_strOwnerName   = 0;
    
    ZeroMemory( m_Attributes.m_GameStyle, MAX_STYLE_BLOB );
    m_Attributes.m_wStyleLen = 0;
    
    XONLINE_ATTRIBUTE_SPEC AttributeSpecs[] = 
    {
        { X_ATTRIBUTE_DATATYPE_INTEGER, sizeof( m_Attributes.m_qwGameType ) },
        { X_ATTRIBUTE_DATATYPE_STRING,  sizeof( m_Attributes.m_strPlayerLevel ) },
        { X_ATTRIBUTE_DATATYPE_STRING,  sizeof( m_Attributes.m_strSessionName ) },
        { X_ATTRIBUTE_DATATYPE_BLOB,    sizeof( m_Attributes.m_GameStyle ) },
        { X_ATTRIBUTE_DATATYPE_STRING,  sizeof( m_Attributes.m_strOwnerName ) }
    };
    
    HRESULT hr;
    
    hr = XOnlineMatchSearchParse( &xms, 5, AttributeSpecs, &m_Attributes );
    
    assert( SUCCEEDED(hr) );
    
    
}




//-----------------------------------------------------------------------------
// Name: SetGameType
// Desc: Set session game type
//-----------------------------------------------------------------------------
VOID SessionInfo::SetGameType( ULONGLONG qwGameType )
{
    m_Attributes.m_qwGameType = qwGameType;
}




//-----------------------------------------------------------------------------
// Name: SetPlayerLevel
// Desc: Set session player level
//-----------------------------------------------------------------------------
VOID SessionInfo::SetPlayerLevel( const WCHAR* strPlayerLevel )
{
    assert( strPlayerLevel != NULL );
    lstrcpynW( m_Attributes.m_strPlayerLevel, strPlayerLevel, MAX_LEVEL_STR );
}




//-----------------------------------------------------------------------------
// Name: SetSessionName
// Desc: Set session name
//-----------------------------------------------------------------------------
VOID SessionInfo::SetSessionName( const WCHAR* strSessionName )
{
    assert( strSessionName != NULL );
    lstrcpynW( m_Attributes.m_strSessionName, strSessionName, MAX_SESSION_STR );
}




//-----------------------------------------------------------------------------
// Name: SetOwnerName
// Desc: Set owner name
//-----------------------------------------------------------------------------
VOID SessionInfo::SetOwnerName( const WCHAR* strOwnerName )
{
    assert( strOwnerName != NULL );
    lstrcpynW( m_Attributes.m_strOwnerName, 
        strOwnerName, XONLINE_USERNAME_SIZE );
}




//-----------------------------------------------------------------------------
// Name: SetStyle
// Desc: Set session style
//-----------------------------------------------------------------------------
VOID SessionInfo::SetStyle( const VOID* pStyle, WORD wStyleLen )
{
    assert( wStyleLen < MAX_STYLE_BLOB );
    
    if( wStyleLen )
    {
        assert( pStyle != NULL );
        CopyMemory( &m_Attributes.m_GameStyle, pStyle, wStyleLen );
    }
    
    m_Attributes.m_wStyleLen = wStyleLen; 
}




//-----------------------------------------------------------------------------
// Name: GenRandSessionName
// Desc: Set name of session to randomly generated value
//-----------------------------------------------------------------------------
VOID SessionInfo::GenRandSessionName( const CXBRandName& rand )
{
    rand.GetName( m_Attributes.m_strSessionName, MAX_SESSION_STR );
}




//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
:
CXBApplication(),
m_UI( m_strFrameRate ),
m_GameMsg( *this ),
m_rand( GetTickCount() )
{
    m_State     = STATE_SELECT_ACCOUNT;
    m_NextState = STATE_SELECT_ACCOUNT;
    
    // Login to matchmaking service
    m_pServices[0] = XONLINE_MATCHMAKING_SERVICE;
    
    m_dwCurrItem     = 0;
    m_dwCurrUser     = 0;
    m_dwUserIndex    = 0;
    m_qwUserID       = 0;
    m_dwSessionIndex = 0;
    
    m_bIsLoggedOn          = FALSE;
    m_bIsSessionRegistered = FALSE;
    m_bIsQuickMatch        = FALSE;
    m_bIsHost              = FALSE;
    
    m_inHostAddr.s_addr = 0;
    m_dwSlotsInUse      = 0;
    
    *m_strUser   = 0;
    *m_strStatus = 0;
    
    ZeroMemory( &m_xnSessionID, sizeof( XNKID ) );
    ZeroMemory( &m_xnKeyExchangeKey, sizeof( XNKEY ) );
    ZeroMemory( &m_SessionAttributes, sizeof( m_SessionAttributes ) );
    
    // Set default session info
    m_Session.SetGameType( TYPE_ANY );
    m_Session.SetPlayerLevel( L"" );
    m_Session.SetOwnerName( L"" );
    m_Session.GenRandSessionName( m_rand );
    m_Session.SetStyle( NULL, 0 );
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
    
    if( FAILED( m_GameMsg.Initialize() ) )
        return E_FAIL;
    
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
        m_bIsLoggedOn = FALSE;
        Reset( TRUE );
        m_State = STATE_ERROR;
        m_NextState = STATE_SELECT_ACCOUNT;
    }
    
    // Maintain our connection once we've logged on
    if( m_bIsLoggedOn )
    {
        HRESULT hr = m_hOnlineTask.Continue();    
        if( FAILED( hr ) )
        {
            m_UI.SetErrorStr( L"Connection was lost. Must relogin" );
            m_hOnlineTask.Close();
            m_bIsLoggedOn = FALSE;
            Reset( TRUE );
            m_State = STATE_ERROR;
            m_NextState = STATE_SELECT_ACCOUNT;
            
        }
    }
    
    Event ev = GetEvent();
    
    switch( m_State )
    {
    case STATE_CREATE_ACCOUNT: UpdateStateCreateAccount( ev ); break;
    case STATE_SELECT_ACCOUNT: UpdateStateSelectAccount( ev ); break;
    case STATE_GET_PIN:        UpdateStateGetPin( ev );        break;
    case STATE_LOGGING_ON:     UpdateStateLoggingOn( ev );     break;
        
    case STATE_SELECT_MATCH:   UpdateStateSelectMatch( ev );   break;
    case STATE_OPTIMATCH:      UpdateStateOptiMatch( ev );     break;
    case STATE_SELECT_TYPE:    UpdateStateSelectType( ev );    break;
    case STATE_SELECT_STYLE:   UpdateStateSelectStyle( ev );   break;
    case STATE_SELECT_LEVEL:   UpdateStateSelectLevel( ev );   break;
    case STATE_SELECT_NAME:    UpdateStateSelectName( ev );    break;
    case STATE_SELECT_SESSION: UpdateStateSelectSession( ev ); break;
        
    case STATE_GAME_SEARCH:    UpdateStateGameSearch( ev );    break;
    case STATE_REQUEST_JOIN:   UpdateStateRequestJoin( ev );   break;
    case STATE_CREATE_SESSION: UpdateStateCreateSession( ev ); break;
    case STATE_PLAY_GAME:      UpdateStatePlayGame( ev );      break;
    case STATE_DELETE_SESSION: UpdateStateDeleteSession( ev ); break;
    case STATE_ERROR:          UpdateStateError( ev );         break;
    default:                   assert( FALSE );                break;
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
    case STATE_GAME_SEARCH:    m_UI.RenderGameSearch();    break;
    case STATE_REQUEST_JOIN:   m_UI.RenderRequestJoin();   break;
    case STATE_CREATE_SESSION: m_UI.RenderCreateSession(); break;
    case STATE_DELETE_SESSION: m_UI.RenderDeleteSession(); break;
    case STATE_ERROR:          m_UI.RenderError();         break;
        
    case STATE_SELECT_MATCH:   m_UI.RenderSelectMatch( m_dwCurrItem ); break;
    case STATE_SELECT_TYPE:    m_UI.RenderSelectType( m_dwCurrItem );  break;
    case STATE_SELECT_STYLE:   m_UI.RenderSelectStyle( m_dwCurrItem ); break;
    case STATE_SELECT_LEVEL:   m_UI.RenderSelectLevel( m_dwCurrItem ); break;
        
    case STATE_CREATE_ACCOUNT:
        m_UI.RenderCreateAccount( TRUE );
        break;
    case STATE_SELECT_ACCOUNT:
        m_UI.RenderSelectAccount( m_dwCurrItem, m_UserList );
        break;
    case STATE_GET_PIN:
        m_UI.RenderGetPin( m_PinEntry, m_strUser );
        break;
    case STATE_OPTIMATCH:
        m_UI.RenderOptiMatch( m_Session, m_dwCurrItem );
        break;
    case STATE_SELECT_NAME:
        m_UI.RenderSelectName( m_dwCurrItem, m_SessionNames );
        break;
    case STATE_SELECT_SESSION:
        m_UI.RenderSelectSession( m_dwCurrItem, m_SessionList );
        break;
    case STATE_PLAY_GAME:
        {
            // m_Players list doesn't include ourself, so we add one
            // to get the number of total players
            DWORD dwTotalPlayers = m_Players.size() + 1;
            m_UI.RenderPlayGame( m_Session, m_strUser, m_strStatus, 
                dwTotalPlayers, m_dwCurrItem );
            break;
        }
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
            assert( XONLINE_USERNAME_SIZE == MAX_PLAYER_STR );
            
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
    
    HRESULT hr = m_hOnlineTask.Continue();
    
    if( FAILED( hr ) )
    {
        m_UI.SetErrorStr( L"Login Failed. Try again." );
        m_hOnlineTask.Close();
        Reset( TRUE );
        return;
    }
    
    // Check login status to see if it has completed
    if( hr == XONLINE_S_LOGON_CONNECTION_ESTABLISHED )
    {
        BOOL bSuccess = TRUE;
        HRESULT hrService = S_OK;
        
        // Check for general errors
        if( FAILED(hr) )
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
				// Check for service errors
				for( DWORD i = 0; i < NUM_SERVICES; ++i )
				{
					if( FAILED( hrService = XOnlineGetServiceInfo( 
						m_pServices[i],NULL ) ) )
					{
						m_UI.SetErrorStr( L"Login failed.\n\n"
							L"Error 0x%x logging into service %d",
							hrService, m_pServices[i] );
						bSuccess = FALSE;
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
            DWORD dwState = XONLINE_FRIENDSTATE_FLAG_ONLINE;
            if( XBVoice_HasDevice() )
                dwState |= XONLINE_FRIENDSTATE_FLAG_VOICE;
            SetPlayerState( dwState );
            
            // Allow player to select match type
            m_dwCurrItem = 0;
            m_State = STATE_SELECT_MATCH;
        }
        else
        {
            Reset( TRUE );
            m_State = STATE_ERROR;
            m_NextState = STATE_SELECT_ACCOUNT;
            m_hOnlineTask.Close();
        }
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateSelectMatch()
// Desc: Player chooses matchmaking type
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateSelectMatch( Event ev )
{
    // If we're not logged in, need to do that before we can start matchmaking
    if( !m_bIsLoggedOn )
    {
        m_State = STATE_SELECT_ACCOUNT;
        return;
    }
    
    switch( ev )
    {
    case EV_BUTTON_A:
        
        // Match type chosen
        if( m_dwCurrItem == MATCH_QUICK )
        {
            // Start lookin' for games
            m_State = STATE_GAME_SEARCH;
            m_bIsQuickMatch = TRUE;
            BeginSessionSearch();
        }
        else
        {
            // Start building search criteria
            m_bIsQuickMatch = FALSE;
            m_dwCurrItem = 0;
            m_State = STATE_OPTIMATCH;
        }
        break;
        
    case EV_UP:
        // Move to previous item; allow wrap to bottom
        if( m_dwCurrItem == 0 )
            m_dwCurrItem = MATCH_MAX - 1;
        else
            --m_dwCurrItem;
        break;
        
    case EV_DOWN:
        // Move to next item; allow wrap to top
        if( m_dwCurrItem == MATCH_MAX - 1 )
            m_dwCurrItem = 0;
        else
            ++m_dwCurrItem;
        break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateOptiMatch()
// Desc: Player customizes game settings
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateOptiMatch( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        switch( m_dwCurrItem )
        {
        case CUSTOM_TYPE:
            m_State = STATE_SELECT_TYPE;
            m_dwCurrItem = DWORD( m_Session.GetGameType() );
            break;
        case CUSTOM_LEVEL:
            m_State = STATE_SELECT_LEVEL;
            switch( *m_Session.GetPlayerLevel() )
            {
            case 0    : m_dwCurrItem = 0; break; 
            case L'B' : m_dwCurrItem = 1; break;
            case L'I' : m_dwCurrItem = 2; break;
            case L'A' : m_dwCurrItem = 3; break;
            default  : assert( FALSE );  break;
            }
            break;
            case CUSTOM_STYLE:
                m_State = STATE_SELECT_STYLE;
                switch( *m_Session.GetStylePtr() )
                {
                case BYTE(  0  ): m_dwCurrItem = 0; break;
                case BYTE( 'H' ): m_dwCurrItem = 1; break;
                case BYTE( 'L' ): m_dwCurrItem = 2; break;
                case BYTE( 'M' ): m_dwCurrItem = 3; break;
                default:          assert( FALSE );  break;
                }
                if( m_Session.GetStyleLen() == 0 )
                    m_dwCurrItem = 0;
                break;
                case CUSTOM_NAME:
                    m_State = STATE_SELECT_NAME;
                    m_dwCurrItem = 0;
                    
                    // Build a list of potential session names
                    m_SessionNames.clear();
                    for( DWORD i = 0; i < MAX_SESSION_NAMES; ++i )
                    {
                        WCHAR strSessionName[ MAX_SESSION_STR ];
                        m_rand.GetName( strSessionName, MAX_SESSION_STR );
                        m_SessionNames.push_back( strSessionName );
                    }
                    break;
                case CUSTOM_FIND:
                    // Time to initiate search
                    m_State = STATE_GAME_SEARCH;
                    BeginSessionSearch();
                    break;
                default:
                    assert( FALSE );
                    break;
        }
        break;
        
        case EV_BUTTON_B:
        case EV_BUTTON_BACK:
            // Return to match menu
            Reset( FALSE );
            break;
            
        case EV_UP:
            // Move to previous item; allow wrap to bottom
            if( m_dwCurrItem == 0 )
                m_dwCurrItem = CUSTOM_MAX - 1;
            else
                --m_dwCurrItem;
            break;
            
        case EV_DOWN:
            // Move to next item; allow wrap to top
            if( m_dwCurrItem == CUSTOM_MAX - 1 )
                m_dwCurrItem = 0;
            else
                ++m_dwCurrItem;
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateSelectType()
// Desc: Select game type
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateSelectType( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        // Game type was chosen
        m_Session.SetGameType( m_dwCurrItem );
        // Fall thru to return to previous menu
        
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        // Return to customize menu
        m_State = STATE_OPTIMATCH;
        m_dwCurrItem = 0;
        break;
        
    case EV_UP:
        // Move to previous item; allow wrap to bottom
        if( m_dwCurrItem == 0 )
            m_dwCurrItem = TYPE_MAX - 1;
        else
            --m_dwCurrItem;
        break;
        
    case EV_DOWN:
        // Move to next item; allow wrap to top
        if( m_dwCurrItem == TYPE_MAX - 1 )
            m_dwCurrItem = 0;
        else
            ++m_dwCurrItem;
        break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateSelectLevel()
// Desc: Select player rating
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateSelectLevel( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        // Player level was chosen
        switch( m_dwCurrItem )
        {
        case LEVEL_ANY:
            m_Session.SetPlayerLevel( L"" );
            break;
        case LEVEL_BEGINNER:
            m_Session.SetPlayerLevel( strBEGINNER );
            break;
        case LEVEL_INTERMEDIATE:
            m_Session.SetPlayerLevel( strINTERMEDIATE );
            break;
        case LEVEL_ADVANCED:
            m_Session.SetPlayerLevel( strADVANCED );
            break;
        default:
            assert( FALSE );
            break;
        }
        // Fall thru to return to previous menu
        
        case EV_BUTTON_B:
        case EV_BUTTON_BACK:
            // Return to customize menu
            m_State = STATE_OPTIMATCH;
            m_dwCurrItem = 0;
            break;
            
        case EV_UP:
            // Move to previous item; allow wrap to bottom
            if( m_dwCurrItem == 0 )
                m_dwCurrItem = LEVEL_MAX - 1;
            else
                --m_dwCurrItem;
            break;
            
        case EV_DOWN:
            // Move to next item; allow wrap to top
            if( m_dwCurrItem == LEVEL_MAX - 1 )
                m_dwCurrItem = 0;
            else
                ++m_dwCurrItem;
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateSelectStyle()
// Desc: Select game style
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateSelectStyle( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        // Game style was chosen
        m_Session.SetStyle( "\0", 0 );
        switch( m_dwCurrItem )
        {
        case STYLE_ANY:
            break;
        case STYLE_HEAVY:
            m_Session.SetStyle( strHEAVY, WORD( lstrlenA( strHEAVY ) ) );
            break;
        case STYLE_LIGHT:
            m_Session.SetStyle( strLIGHT, WORD( lstrlenA( strLIGHT ) ) );
            break;
        case STYLE_MIXED:
            m_Session.SetStyle( strMIXED, WORD( lstrlenA( strMIXED ) ) );
            break;
        default:
            assert( FALSE );
            break;
        }
        // Fall thru to return to previous menu
        
        case EV_BUTTON_B:
        case EV_BUTTON_BACK:
            // Return to customize menu
            m_State = STATE_OPTIMATCH;
            m_dwCurrItem = 0;
            break;
            
        case EV_UP:
            // Move to previous item; allow wrap to bottom
            if( m_dwCurrItem == 0 )
                m_dwCurrItem = STYLE_MAX - 1;
            else
                --m_dwCurrItem;
            break;
            
        case EV_DOWN:
            // Move to next item; allow wrap to top
            if( m_dwCurrItem == STYLE_MAX - 1 )
                m_dwCurrItem = 0;
            else
                ++m_dwCurrItem;
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateSelectName()
// Desc: Select game name
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateSelectName( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        // Game name was chosen
        m_Session.SetSessionName( m_SessionNames[ m_dwCurrItem ].c_str() );
        // Fall thru to return to previous menu
        
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        // Return to customize menu
        m_State = STATE_OPTIMATCH;
        m_dwCurrItem = 0;
        break;
        
    case EV_UP:
        // Move to previous item; allow wrap to bottom
        if( m_dwCurrItem == 0 )
            m_dwCurrItem = m_SessionNames.size() - 1;
        else
            --m_dwCurrItem;
        break;
        
    case EV_DOWN:
        // Move to next item; allow wrap to top
        if( m_dwCurrItem == m_SessionNames.size() - 1 )
            m_dwCurrItem = 0;
        else
            ++m_dwCurrItem;
        break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateSelectSession()
// Desc: Select game session
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateSelectSession( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        // Join selected session
        m_dwSessionIndex = m_dwCurrItem;
        m_State = STATE_REQUEST_JOIN;
        BeginJoinSession();
        break;
        
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        // Return to customize menu
        m_State = STATE_OPTIMATCH;
        m_dwCurrItem = 0;
        break;
        
    case EV_UP:
        // Move to previous item; allow wrap to bottom
        if( m_dwCurrItem == 0 )
            m_dwCurrItem = m_SessionList.size() - 1;
        else
            --m_dwCurrItem;
        break;
        
    case EV_DOWN:
        // Move to next item; allow wrap to top
        if( m_dwCurrItem == m_SessionList.size() - 1 )
            m_dwCurrItem = 0;
        else
            ++m_dwCurrItem;
        break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateGameSearch()
// Desc: Searching for matching game session
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateGameSearch( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        CancelMatch();
        m_dwCurrItem = 0;
        if( m_bIsQuickMatch )
        {
            m_State = STATE_SELECT_MATCH;
        }
        else
        {
            m_State = STATE_OPTIMATCH;
        }
        return;
    }
    
    // Wait for matchmaking server to return results
    HRESULT hr = m_hMatchTask.Continue();
    if( hr != XONLINETASK_S_RUNNING )
    {
        // Handle errors
        if( FAILED(hr) )
        {
            m_hMatchTask.Close();
            m_UI.SetErrorStr( L"Game search failed with error %x", hr );
            Reset( TRUE );
            return;
        }
        
        // Get the list returned by the matchmaking server
        PXMATCH_SEARCHRESULT *ppSearchResult;
        DWORD dwResults;
        hr = XOnlineMatchSearchGetResults( m_hMatchTask, &ppSearchResult,
            &dwResults );
        
        // Handle errors
        if( FAILED(hr) )
        {
            m_hMatchTask.Close();
            m_UI.SetErrorStr( L"Game search get results failed with error %x", hr );
            Reset( TRUE );
            return;
        }
        
        m_SessionList.clear();
        
        // Save the results
        for( DWORD i = 0; i < dwResults; ++i )
        {
            m_SessionList.push_back( SessionInfo( *ppSearchResult[i],
                m_hMatchTask, i ) );
        }
        
        // The search is over
        m_hMatchTask.Close();
        
        // If we found at least one game, join it automatically
        if( dwResults > 0 )
        {
            // If there was more than one session available and
            // this isn't a quick match then display a list of
            // session to select from
            if( !m_bIsQuickMatch )
            {
                m_dwCurrItem = 0;
                m_State = STATE_SELECT_SESSION;
            }
            else
            {
                m_State = STATE_REQUEST_JOIN;
                BeginJoinSession();
            }
        }
        // We didn't find any sessions, so we'll create our own
        else
        {
            m_State = STATE_CREATE_SESSION;
            BeginCreateSession();
        }
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateRequestJoin()
// Desc: Joining session
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateRequestJoin( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        // Cancel request; return to match menu
        Reset( FALSE );
        m_GameJoinTimer.Stop();
        break;
    }
    
    // See if the game has replied
    m_GameMsg.ProcessMessages( m_Players );
    
    // We wait for up to GAME_JOIN_TIME seconds. If the game didn't
    // respond, display an error message, then create our own session.
    if( m_GameJoinTimer.GetElapsedSeconds() > GAME_JOIN_TIME )
    {
        m_GameJoinTimer.Stop();
        m_State = STATE_ERROR;
        m_UI.SetErrorStr( L"Game did not respond" );
        m_NextState = STATE_CREATE_SESSION;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateCreateSession()
// Desc: Creating session
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateCreateSession( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        CancelMatch();
        return;
    }
    
    // Wait for matchmaking server to save new session info
    HRESULT hr = m_hMatchTask.Continue();
    if( hr != XONLINETASK_S_RUNNING )
    {
        // Clear any registered sessions
        if( m_bIsSessionRegistered )
        {
            INT iResult = XNetUnregisterKey( &m_xnSessionID );
            assert( iResult == NO_ERROR );
            (VOID)iResult;
            m_bIsSessionRegistered = FALSE;
            ZeroMemory( &m_xnSessionID, sizeof( XNKID ) );
        }
        
        // Extract the new session ID
        hr = XOnlineMatchSessionGetInfo( m_hMatchTask, &m_xnSessionID, &m_xnKeyExchangeKey );
        
        // Handle errors
        if( FAILED(hr) )
        {
            m_hMatchTask.Close();
            m_UI.SetErrorStr( L"Game session creation failed with error %x", hr );
            Reset( TRUE );
            return;
        }
        INT iKeyRegistered = XNetRegisterKey( &m_xnSessionID, 
            &m_xnKeyExchangeKey );
        
        if( iKeyRegistered != NO_ERROR )
        {
            m_hMatchTask.Close();
            hr = E_FAIL;
            m_UI.SetErrorStr( L"XNetRegisterKey failed");
            Reset( TRUE );
            return;
        }
        
        m_bIsSessionRegistered = TRUE;
        
        // We are now the host of a new game
        m_bIsHost = TRUE;
        m_State = STATE_PLAY_GAME;
        m_dwCurrItem = 0;
        m_HeartbeatTimer.StartZero();
        SetStatus( L"Created Session" );
        
        // Notify the world of our state change
        DWORD dwState = XONLINE_FRIENDSTATE_FLAG_ONLINE |
            XONLINE_FRIENDSTATE_FLAG_PLAYING |
            XONLINE_FRIENDSTATE_FLAG_JOINABLE;
        if( XBVoice_HasDevice() )
            dwState |= XONLINE_FRIENDSTATE_FLAG_VOICE;
        SetPlayerState( dwState );
        
        m_GameMsg.SetUser( m_strUser, m_bIsHost );
        m_GameMsg.SetSessionID( m_xnSessionID );
        
        // Note that the session remains "active" (m_hMatchTask isn't
        // closed), because we want to update the session attributes
        // as players come and go.
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStatePlayGame()
// Desc: Play game
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStatePlayGame( Event ev )
{
    // Handle net messages
    if( m_GameMsg.ProcessMessages( m_Players ) )
        return;
    
    // Send keep-alives
    if( m_HeartbeatTimer.GetElapsedSeconds() > PLAYER_HEARTBEAT )
    {
        m_GameMsg.SendHeartbeat( m_Players );
        m_HeartbeatTimer.StartZero();
    }
    
    // Handle other players dropping
    if( m_GameMsg.ProcessPlayerDropouts( m_Players, PLAYER_TIMEOUT ) )
        return;
    
    // Handle session updates
    if( m_bIsHost )
    {
        HRESULT hr = m_hMatchTask.Continue();
        if( hr != XONLINETASK_S_RUNNING )
        {
            // Handle errors
            if( FAILED(hr) )
            {
                m_hMatchTask.Close();
                m_UI.SetErrorStr( L"XMatch failed with error %x", hr );
                Reset( TRUE );
                return;
            }
            
            // If there are session updates that need to be processed,
            // remove them from the queue and send the update to the
            // matchmaking server.
            if( !m_SessionUpdateQ.empty() )
            {
                BeginSessionUpdate( m_SessionUpdateQ.front() );
                m_SessionUpdateQ.pop();
            }
        }
    }
    
    switch( ev )
    {
    case EV_BUTTON_A:
        switch( m_dwCurrItem )
        {
        case GAME_WAVE:  SendWave();      break;
        case GAME_LEAVE: LeaveGame();     break;
        default:         assert( FALSE ); break;
        }
        break;
        
        case EV_UP:
            // Move to previous item; allow wrap to bottom
            if( m_dwCurrItem == 0 )
                m_dwCurrItem = GAME_MAX - 1;
            else
                --m_dwCurrItem;
            break;
            
        case EV_DOWN:
            // Move to next item; allow wrap to top
            if( m_dwCurrItem == GAME_MAX - 1 )
                m_dwCurrItem = 0;
            else
                ++m_dwCurrItem;
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateDeleteSession()
// Desc: Delete game session from matchmaking server
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateStateDeleteSession( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        CancelMatch();
        return;
    }
    
    HRESULT hr = m_hMatchTask.Continue();
    if( hr != XONLINETASK_S_RUNNING )
    {
        m_hMatchTask.Close();
        
        // Handle errors
        if( FAILED(hr) )
        {
            m_UI.SetErrorStr( L"Game session deletion failed with error %x", hr );
            Reset( TRUE );
            return;
        }
        
        // Return to matchmaking
        Reset( FALSE );
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
        
        // Special case: if the next state is "create session," we must
        // begin the session creation process
        if( m_State == STATE_CREATE_SESSION )
            BeginCreateSession();
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
        Reset( TRUE );
    }
}




//-----------------------------------------------------------------------------
// Name: BeginSessionSearch()
// Desc: Initiate the game search process
//-----------------------------------------------------------------------------
VOID CXBoxSample::BeginSessionSearch()
{
    assert( m_hMatchTask == NULL );
    
    
    // Initialize the search request
    XONLINE_ATTRIBUTE SearchAttributes[ 3 ];
    
    if( !m_bIsQuickMatch )
    {
        // Note: These parameters are specific to the stored procedure
        // SEARCH_ID_CUSTOM_1 for this specific sample (based
        // on Title ID). This method is shown as an example of what
        // games can specify when searching for a particular session.
        // This code ONLY WORKS IN THE CONTEXT OF THIS SAMPLE. 
        // In the future, game developers will create and establish
        // their own stored procedures in conjunction with the Xbox 
        // ATG and Xbox online teams. These stored procedures will 
        // be hosted on the Xbox matchmaking servers and allow game 
        // developers to tailor session searching to the requirements 
        // of their game.
        
        // The first (and only) integer parameter is the game type.
        // To match on any game type, pass -1
        SearchAttributes[0].dwAttributeID = X_ATTRIBUTE_DATATYPE_INTEGER;
        SearchAttributes[0].info.integer.qwValue = 
            ( m_Session.GetGameType() == 0 ) ? -1 : m_Session.GetGameType();
        
        // The first (and only) string parameter is the player level.
        // If level == any, empty specifies match all
        SearchAttributes[1].dwAttributeID = X_ATTRIBUTE_DATATYPE_STRING;
        SearchAttributes[1].info.string.pwszValue = m_Session.GetPlayerLevel();
        
        // The first (and only) blob parameter is the game style.
        // If style == any, 0 length specifies match all
        VOID* pBlob = (VOID*)( m_Session.GetStylePtr() );
        
        SearchAttributes[2].dwAttributeID = X_ATTRIBUTE_DATATYPE_BLOB;
        SearchAttributes[2].info.blob.pvValue = pBlob;
        SearchAttributes[2].info.blob.dwLength = m_Session.GetStyleLen();
        
    }
    else
    {
        // The first (and only) integer parameter is the game type.
        // To match on any game type, pass -1
        SearchAttributes[0].dwAttributeID = X_ATTRIBUTE_DATATYPE_INTEGER;
        SearchAttributes[0].info.integer.qwValue = ULONGLONG(-1);
        
        // The first (and only) string parameter is the player level.
        // If level == any, empty specifies match all
        SearchAttributes[1].dwAttributeID = X_ATTRIBUTE_DATATYPE_STRING;
        SearchAttributes[1].info.string.pwszValue = L"";
        
        // The first (and only) blob parameter is the game style.
        // If style == any, 0 length specifies match all
        SearchAttributes[2].dwAttributeID = X_ATTRIBUTE_DATATYPE_BLOB;
        SearchAttributes[2].info.blob.pvValue = NULL;
        SearchAttributes[2].info.blob.dwLength = 0;
    }
    
    
    HRESULT hr = XOnlineMatchSearch( SEARCH_ID_CUSTOM_1,
        MAX_RESULTS,
        3,
        SearchAttributes,
        MAX_RESULTS_LEN,
        NULL, &m_hMatchTask );
    
    if( FAILED(hr) )
    {
        m_hMatchTask.Close();
        m_UI.SetErrorStr( L"Game search failed to start. Error %x", hr );
        Reset( TRUE );
    }
}




//-----------------------------------------------------------------------------
// Name: BeginCreateSession()
// Desc: Initiate the game session creation process
//-----------------------------------------------------------------------------
VOID CXBoxSample::BeginCreateSession()
{
    // Generate a random session name if we don't currently have one
    if( *m_Session.GetSessionName() == 0 )
        m_Session.GenRandSessionName( m_rand );
    
    assert( m_hMatchTask == NULL );
    
    // Initialize the create request
    
    
    // Set session attributes 
    
    // Game type
    //---------------------------------------------------------------------
    // If not specified, default to TYPE_SHORT.
    // Game type is the first and only session integer parameter.
    if( m_Session.GetGameType() == TYPE_ANY )
        m_Session.SetGameType( TYPE_SHORT );
    m_SessionAttributes[0].fChanged = TRUE;
    m_SessionAttributes[0].dwAttributeID = XATTRIB_TYPE;
    m_SessionAttributes[0].info.integer.qwValue = m_Session.GetGameType();
    
    // Player level
    //---------------------------------------------------------------------
    // If not specified, default to LEVEL_BEGINNER.
    // Player level is the first session string parameter.
    if( *m_Session.GetPlayerLevel() == 0 )
        m_Session.SetPlayerLevel( strBEGINNER );    
    m_SessionAttributes[1].fChanged = TRUE;
    m_SessionAttributes[1].dwAttributeID = XATTRIB_LEVEL;
    m_SessionAttributes[1].info.string.pwszValue = m_Session.GetPlayerLevel();
    
    // Session name
    //---------------------------------------------------------------------
    // Always specified.
    // The second session string parameter.
    assert( *m_Session.GetSessionName() != 0 );
    m_SessionAttributes[2].fChanged = TRUE;
    m_SessionAttributes[2].dwAttributeID = XATTRIB_SNAME;
    m_SessionAttributes[2].info.string.pwszValue = m_Session.GetSessionName();
    
    // Game style
    //---------------------------------------------------------------------
    // If not specified, default to STYLE_HEAVY.
    // The first (and only) session blob parameter.
    if( m_Session.GetStyleLen() == 0 )
        m_Session.SetStyle( strHEAVY,  WORD( lstrlenA( strHEAVY ) ) );
    VOID* pBlob = (VOID*)( m_Session.GetStylePtr() );   
    m_SessionAttributes[3].fChanged = TRUE;
    m_SessionAttributes[3].dwAttributeID = XATTRIB_STYLE;
    m_SessionAttributes[3].info.blob.pvValue = pBlob;
    m_SessionAttributes[3].info.blob.dwLength = m_Session.GetStyleLen();
    
    // The first (and only) user parameter is the player name
    m_Session.SetOwnerName( m_strUser );
    m_SessionAttributes[4].fChanged = TRUE;
    m_SessionAttributes[4].dwAttributeID = XATTRIB_PNAME;
    m_SessionAttributes[4].info.string.pwszValue = m_Session.GetOwnerName();
    
    m_dwSlotsInUse = 1;
    
    // Limit the number of players to MAX_PLAYERS public slots and no
    // private (invitation only) slots. Note that we add ourself as a player.
    HRESULT hr = XOnlineMatchSessionCreate( m_dwSlotsInUse, MAX_PLAYERS - m_dwSlotsInUse, // Public Slots
        0, 0, // Private Slots
        MAX_SESSION_ATTRIBS,
        m_SessionAttributes,  // Attributes
        NULL, &m_hMatchTask );
    
    // Reset the changed flag on each attribute
    // so that during the next session update, 
    // needless processing isn't wasted on
    // attributes which have not changed
    for(DWORD i = 0; i < MAX_SESSION_ATTRIBS; ++i)
        m_SessionAttributes[i].fChanged = FALSE;
    
    if( FAILED( hr ) )
    {
        m_hMatchTask.Close();
        m_UI.SetErrorStr( L"Game session failed to start. Error %x", hr );
        Reset( TRUE );
    }
}




//-----------------------------------------------------------------------------
// Name: BeginSessionUpdate()
// Desc: Notify match server about session change
//-----------------------------------------------------------------------------
VOID CXBoxSample::BeginSessionUpdate( SESSION_UPDATE_ACTION dwAction )
{
    assert( m_hMatchTask != NULL );
    
    
    if( dwAction == ADD_PLAYER )
        ++m_dwSlotsInUse;
    else // remove player
        --m_dwSlotsInUse;
    
    m_hMatchTask.Close();
    
    HRESULT hr = XOnlineMatchSessionUpdate( m_xnSessionID,
        m_dwSlotsInUse, 
        MAX_PLAYERS - m_dwSlotsInUse,
        0, 0, // Private Slots
        MAX_SESSION_ATTRIBS,
        m_SessionAttributes,  // Attributes
        NULL, &m_hMatchTask );
    
    // Notify the match server
    if( FAILED(hr) )
    {
        m_UI.SetErrorStr( L"Game session failed to update. Error %x", hr );
        Reset( TRUE );
    }
    
}




//-----------------------------------------------------------------------------
// Name: BeginDeleteSession()
// Desc: Initiate the game session removal process
//-----------------------------------------------------------------------------
VOID CXBoxSample::BeginDeleteSession()
{
    assert( m_hMatchTask == NULL );
    
    // Initialize the delete request
    HRESULT hr = XOnlineMatchSessionDelete( m_xnSessionID, NULL, &m_hMatchTask );
    if( FAILED(hr) )
    {
        m_hMatchTask.Close();
        m_UI.SetErrorStr( L"Failed to start session deletion. Error %x", hr );
        Reset( TRUE );
    }
}




//-----------------------------------------------------------------------------
// Name: BeginJoinSession()
// Desc: Attempt to join a game
//-----------------------------------------------------------------------------
VOID CXBoxSample::BeginJoinSession()
{
    assert( m_SessionList.size() > 0 );
    
    SessionInfo& Session = m_SessionList[ m_dwSessionIndex ];
    
    // Clear any registered sessions
    if( m_bIsSessionRegistered )
    {
        INT iResult = XNetUnregisterKey( &m_xnSessionID );
        assert( iResult == NO_ERROR );
        (VOID)iResult;
        m_bIsSessionRegistered = FALSE;
        ZeroMemory( &m_xnSessionID, sizeof( XNKID ) );
    }
    
    // We found a valid session with an available player slot.
    // Register the session key.
    INT iResult = XNetRegisterKey( Session.GetSessionID(),
        Session.GetKeyExchangeKey() );
    if( iResult != NO_ERROR )
    {
        m_UI.SetErrorStr( L"Unable to establish session with game" );
        Reset( TRUE );
        return;
    }
    m_bIsSessionRegistered = TRUE;
    
    // Save the key ID because we need to unregister it laer
    CopyMemory( &m_xnSessionID, Session.GetSessionID(), sizeof( XNKID ) );
    
    // Store the game name
    m_Session.SetSessionName( Session.GetSessionName() );
    m_Session.SetGameType( Session.GetGameType() );
    m_Session.SetPlayerLevel( Session.GetPlayerLevel() );
    m_Session.SetOwnerName( Session.GetOwnerName () );
    m_Session.SetStyle( Session.GetStylePtr(), Session.GetStyleLen() );
    
    // Convert the XNADDR of the host to the INADDR we'll use to
    // join the game
    iResult = XNetXnAddrToInAddr( Session.GetHostAddr(),
        &m_xnSessionID, &m_inHostAddr );
    assert( iResult == NO_ERROR );
    
    m_GameMsg.SetUser( m_strUser, FALSE );
    m_GameMsg.SetSessionID( m_xnSessionID );
    
    // Request join approval from the game and await a response
    m_GameMsg.SendJoinGame( CXBSockAddr( m_inHostAddr, GameMsg::GAME_PORT ),
        m_strUser, m_qwUserID );
    m_GameJoinTimer.StartZero();
}




//-----------------------------------------------------------------------------
// Name: BeginAddPlayer()
// Desc: Notify match server about new player
//-----------------------------------------------------------------------------
VOID CXBoxSample::BeginAddPlayer()
{
    // Match tasks need to be queued up so that there's never 2+
    // XOnlineMatchSessionUpdates in progress at a time
    m_SessionUpdateQ.push( ADD_PLAYER );
}




//-----------------------------------------------------------------------------
// Name: BeginRemovePlayer()
// Desc: Notify match server about player departure
//-----------------------------------------------------------------------------
VOID CXBoxSample::BeginRemovePlayer()
{
    // Match tasks need to be queued up so that there's never 2+
    // XOnlineMatchSessionUpdates in progress at a time
    m_SessionUpdateQ.push( REMOVE_PLAYER );
}




//-----------------------------------------------------------------------------
// Name: SendWave()
// Desc: Wave to all other players in game
//-----------------------------------------------------------------------------
VOID CXBoxSample::SendWave()
{
    // Indicate that you waved
    SetStatus( L"You waved" );
    m_GameMsg.SendWave( m_Players );
}




//-----------------------------------------------------------------------------
// Name: LeaveGame()
// Desc: Exit the game
//-----------------------------------------------------------------------------
VOID CXBoxSample::LeaveGame()
{
    // Other players will detect that we left because they will no longer
    // receive heartbeats
    
    // If the host leaves, remove the session from the matchmaking server
    if( m_bIsHost )
    {
        m_bIsHost = FALSE;
        m_State = STATE_DELETE_SESSION;
        m_hMatchTask.Close();
        m_HeartbeatTimer.Stop();
        m_Players.clear();
        BeginDeleteSession();
    }
    else
    {
        Reset( FALSE );
    }
    
    // Notify the world of our state change
    DWORD dwState = XONLINE_FRIENDSTATE_FLAG_ONLINE;
    if( XBVoice_HasDevice() )
        dwState |= XONLINE_FRIENDSTATE_FLAG_VOICE;
    SetPlayerState( dwState );
}




//-----------------------------------------------------------------------------
// Name: OnJoinGame
// Desc: Handle new player joining game that we host
//-----------------------------------------------------------------------------
VOID CXBoxSample::OnJoinGame( const CXBNetPlayerInfo& playerInfo )
{
    m_Players.push_back( playerInfo );
    SetStatus( L"%.*s has joined the game", MAX_PLAYER_STR, playerInfo.strPlayerName );
    
    // Queue up this request for the matchmaking server
    BeginAddPlayer();
}




//-----------------------------------------------------------------------------
// Name: OnJoinApproved
// Desc: We've been approved for game entry by the given host
//-----------------------------------------------------------------------------
VOID CXBoxSample::OnJoinApproved( const CXBNetPlayerInfo& hostInfo )
{
    m_Players.push_back( hostInfo );
    
    // Enter into the game UI
    m_State = STATE_PLAY_GAME;
    
    // Set the default game item to "wave"
    m_dwCurrItem = GAME_WAVE;
    
    SetStatus( L"You have joined the game" );
    m_HeartbeatTimer.StartZero();
    
    m_GameMsg.SetUser( m_strUser, FALSE );
    
    // Notify the world of our state change
    DWORD dwState = XONLINE_FRIENDSTATE_FLAG_ONLINE |
        XONLINE_FRIENDSTATE_FLAG_PLAYING;
    if( XBVoice_HasDevice() )
        dwState |= XONLINE_FRIENDSTATE_FLAG_VOICE;
    SetPlayerState( dwState );
}




//-----------------------------------------------------------------------------
// Name: OnJoinApprovedAddPlayer
// Desc: Receiving information on others players already in the game
//-----------------------------------------------------------------------------
VOID CXBoxSample::OnJoinApprovedAddPlayer( const CXBNetPlayerInfo& playerInfo )
{
    m_Players.push_back( playerInfo );
}




//-----------------------------------------------------------------------------
// Name: OnJoinDenied
// Desc: Handle join denied
//-----------------------------------------------------------------------------
VOID CXBoxSample::OnJoinDenied()
{
    // Only clients who are not currently playing should receive this message
    assert( m_State != STATE_PLAY_GAME );
    
    // If for some reason we receive a "join denied" message and we're
    // already playing a game, ignore the message.
    if( m_State == STATE_PLAY_GAME )
        return;
    
    // The session we wanted to join is full. Display error
    m_UI.SetErrorStr( L"The session is full.\nChoose another session." );
    m_State = STATE_ERROR;
}




//-----------------------------------------------------------------------------
// Name: OnPlayerJoined
// Desc: The given player joined our game
//-----------------------------------------------------------------------------
VOID CXBoxSample::OnPlayerJoined( const CXBNetPlayerInfo& playerInfo )
{
    MatchInAddr matchInAddr( playerInfo.inAddr );
    
    // First check to make sure the player isn't already in the list.
    // If so, remove the player first.  This can happen if the player
    // drops out of a game and rejoins before the next heartbeat.
    CXBNetPlayerList::iterator i = 
        std::find_if( m_Players.begin(), m_Players.end(), matchInAddr );
    
    if( i != m_Players.end() )
    {       
        m_Players.erase( i );
    }
    
    m_Players.push_back( playerInfo );
    SetStatus( L"%.*s has joined the game", MAX_PLAYER_STR, playerInfo.strPlayerName );
}




//-----------------------------------------------------------------------------
// Name: OnWave
// Desc: The given player waved to us
//-----------------------------------------------------------------------------
VOID CXBoxSample::OnWave( const CXBNetPlayerInfo& playerInfo )
{
    SetStatus( L"%.*s waved", MAX_PLAYER_STR, playerInfo.strPlayerName );
}




//-----------------------------------------------------------------------------
// Name: OnHeartbeat
// Desc: The given player sent us a heartbeat
//-----------------------------------------------------------------------------
VOID CXBoxSample::OnHeartbeat( const CXBNetPlayerInfo& playerInfo )
{
    MatchInAddr matchInAddr( playerInfo.inAddr );
    
    // Find out who sent a heartbeat by matching on name
    CXBNetPlayerList::iterator i = 
        std::find_if( m_Players.begin(), m_Players.end(), matchInAddr );
    
    // We expect that we know about the player
    assert( i != m_Players.end() );
    
    i->dwLastHeartbeat = GetTickCount();
}




//-----------------------------------------------------------------------------
// Name: OnPlayerDropout
// Desc: The given player left the game
//-----------------------------------------------------------------------------
VOID CXBoxSample::OnPlayerDropout( const CXBNetPlayerInfo& playerInfo, BOOL bIsHost )
{
    if( bIsHost )
    {
        SetStatus( L"Host %.*s left game.\nGame closed to new players.",
            MAX_PLAYER_STR, playerInfo.strPlayerName );
        BeginRemovePlayer();
    }
    else
    {
        SetStatus( L"%.*s left the game", MAX_PLAYER_STR, playerInfo.strPlayerName );
    }
    
    MatchInAddr matchInAddr( playerInfo.inAddr );
    
    // Find out who we need to delete by matching on name
    CXBNetPlayerList::iterator i = 
        std::find_if( m_Players.begin(), m_Players.end(), matchInAddr );
    
    // We expect that we know about the player
    assert( i != m_Players.end() );
    
    m_Players.erase( i );
}




//-----------------------------------------------------------------------------
// Name: SetStatus
// Desc: Set the status string
//-----------------------------------------------------------------------------
VOID __cdecl CXBoxSample::SetStatus( const WCHAR* strFormat, ... )
{
    va_list pArgList;
    va_start( pArgList, strFormat );
    
    INT iChars = wvsprintfW( m_strStatus, strFormat, pArgList );
    assert( iChars < MAX_STATUS_STR );
    (VOID)iChars; // avoid compiler warning
    
    va_end( pArgList );
}




//-----------------------------------------------------------------------------
// Name: CancelMatch()
// Desc: Cancel the match task and prepare to return to front menu
//-----------------------------------------------------------------------------
VOID CXBoxSample::CancelMatch()
{
    // Cancel the task
    m_hMatchTask.Close();    
    m_dwCurrItem = 0;
}




//-----------------------------------------------------------------------------
// Name: SetPlayerState()
// Desc: Broadcast current player state for the world
//-----------------------------------------------------------------------------
VOID CXBoxSample::SetPlayerState( DWORD dwState )
{
    BOOL bSuccess = XOnlineNotificationSetState( m_dwUserIndex, dwState,
        m_xnSessionID, 0, NULL );
    assert( bSuccess );
    (VOID)bSuccess; // avoid compiler warning
}




//-----------------------------------------------------------------------------
// Name: Reset()
// Desc: Prepare to restart the application at the front menu
//-----------------------------------------------------------------------------
VOID CXBoxSample::Reset( BOOL bIsError )
{
    m_hMatchTask.Close();
    if( bIsError )
    {
        m_State = STATE_ERROR;
        m_NextState = STATE_SELECT_MATCH;
    }
    else
    {
        m_State = STATE_SELECT_MATCH;
    }
    
    m_dwCurrItem = 0;
    
    m_bIsHost = FALSE;
    m_HeartbeatTimer.Stop();
    m_Players.clear();
    
    // Clear any registered sessions
    if( m_bIsSessionRegistered )
    {
        INT iResult = XNetUnregisterKey( &m_xnSessionID );
        assert( iResult == NO_ERROR );
        (VOID)iResult; // avoid compiler warnings
        m_bIsSessionRegistered = FALSE;
        ZeroMemory( &m_xnSessionID, sizeof( XNKID ) );
    }
}
