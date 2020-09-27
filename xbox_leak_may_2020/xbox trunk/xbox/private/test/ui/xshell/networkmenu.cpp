/*****************************************************
*** networkmenu.cpp
***
*** CPP file for our XShell Network menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** May 19th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "networkmenu.h"

extern CXSettings       g_XboxSettings;     // Settings that are stored on the Xbox
extern CSoundCollection g_Sounds;           // Used to generate all sounds for the XShell

// Constructors
CNetworkMenu::CNetworkMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_IPMenu( NULL ),
m_DNSMenu( NULL ),
m_SubnetMenu( NULL ),
m_GatewayMenu( NULL ),
m_MachineNameMenu( NULL )
{
    XDBGTRC( APP_TITLE_NAME_A, "CNetworkMenu::CNetworkMenu(ex)" );

    m_uiNumItems = NETWORKMENU_ITEM_NUM_ITEMS;
    SetSelectedItem( 0 );

    SetTitle( MENU_TITLE_NETWORK );
}


// Destructor
CNetworkMenu::~CNetworkMenu()
{
}


// Draws a menu on to the screen
void CNetworkMenu::Action( CUDTexture* pTexture )
{
    // Load the settings in to our object from the config sector of the Xbox
    g_XboxSettings.LoadSettings();

    // Check to see if the settings have changed.  If so, update the texture
    if( g_XboxSettings.SettingsHaveChanged() )
    {
        m_bUpdateTexture = TRUE;
    }

    // Only update the texture if we need to, otherwise return
    if( !m_bUpdateTexture )
    {
        return;
    }
    else
    {
        m_bUpdateTexture = FALSE;
    }
    
    // Lock our texture and set it as the render target
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    // Let our base menu class draw all the items pulled from the MNU file
    CMenuScreen::Action( pTexture );

    // Draw a box behind the currently highlighted choice
    float X1Pos = MENUBOX_SELECTBAR_X1;
    float Y1Pos = g_fNetworkItemYPos[GetSelectedItem()] - ITEM_SELECTOR_BORDER_SPACE;
    float X2Pos = MENUBOX_SELECTBAR_X2;
    float Y2Pos = Y1Pos + m_uiFontHeightOfItem + ( 2 * ITEM_SELECTOR_BORDER_SPACE );

    pTexture->DrawBox( X1Pos, Y1Pos, X2Pos, Y2Pos, ITEM_SELECTOR_COLOR );

    WCHAR wideString[100];
    DWORD dwHeadTextCol, dwValueTextCol;

    ///////////////////
    // Title IP Address
    ///////////////////

    // If we are on the currently selected item, let's set the text color
    if( GetSelectedItem() == (int)NETWORKMENU_ITEM_IPADDRESS )
    {
        dwHeadTextCol = ITEM_SELECTED_HEADER_COLOR;
        dwValueTextCol = ITEM_SELECTED_VALUE_COLOR;
    }
    else
    {
        dwHeadTextCol = NETWORKMENU_OPTIONHEADER_COLOR;
        dwValueTextCol = SCREEN_DEFAULT_TEXT_FG_COLOR;
    }

    float XPos = MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( g_pwszNetworkMenuItems[NETWORKMENU_ITEM_IPADDRESS] ) );
    pTexture->DrawText( XPos, NETWORKMENU_IPADDR_YPOS, dwHeadTextCol, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", g_pwszNetworkMenuItems[NETWORKMENU_ITEM_IPADDRESS] );

    ZeroMemory( wideString, sizeof( WCHAR ) * 100 );
    XPos = MENUBOX_TEXTAREA_X_CENTER + 5.0f;

    // If our IP Address is null, display "dhcp" info
    if( '\0' == g_XboxSettings.GetIPAddress()[0] )
    {
        _snwprintf( wideString, 100, L"(use dhcp)" );
    }
    else
    {
        _snwprintf( wideString, 100, L"%S", g_XboxSettings.GetIPAddress() );
    }
    pTexture->DrawText( XPos, NETWORKMENU_IPADDR_YPOS, dwValueTextCol, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", wideString );


    ///////////////////
    // Debug IP Address
    ///////////////////

    // If we are on the currently selected item, let's set the text color
    if( GetSelectedItem() == (int)NETWORKMENU_ITEM_DEBUGIPADDRESS )
    {
        dwHeadTextCol = ITEM_SELECTED_HEADER_COLOR;
        dwValueTextCol = ITEM_SELECTED_VALUE_COLOR;
    }
    else
    {
        dwHeadTextCol = NETWORKMENU_OPTIONHEADER_COLOR;
        dwValueTextCol = SCREEN_DEFAULT_TEXT_FG_COLOR;
    }

    XPos = MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( g_pwszNetworkMenuItems[NETWORKMENU_ITEM_DEBUGIPADDRESS] ) );
    pTexture->DrawText( XPos, NETWORKMENU_DEBUGIPADDR_YPOS, dwHeadTextCol, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", g_pwszNetworkMenuItems[NETWORKMENU_ITEM_DEBUGIPADDRESS] );

    ZeroMemory( wideString, sizeof( WCHAR ) * 100 );
    XPos = MENUBOX_TEXTAREA_X_CENTER + 5.0f;

    // If our Debug IP Address is null, display "dhcp" info
    if( '\0' == g_XboxSettings.GetDebugIPAddress()[0] )
    {
        _snwprintf( wideString, 100, L"(use dhcp)" );
    }
    else
    {
        _snwprintf( wideString, 100, L"%hs", g_XboxSettings.GetDebugIPAddress() );
    }
    pTexture->DrawText( XPos, NETWORKMENU_DEBUGIPADDR_YPOS, dwValueTextCol, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%ls", wideString );


    //////////////////////
    // Primary DNS Address
    //////////////////////

    // If we are on the currently selected item, let's set the text color
    if( GetSelectedItem() == (int)NETWORKMENU_ITEM_PRIMARYDNS )
    {
        dwHeadTextCol = ITEM_SELECTED_HEADER_COLOR;
        dwValueTextCol = ITEM_SELECTED_VALUE_COLOR;
    }
    else
    {
        dwHeadTextCol = NETWORKMENU_OPTIONHEADER_COLOR;
        dwValueTextCol = SCREEN_DEFAULT_TEXT_FG_COLOR;
    }

    XPos = MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( g_pwszNetworkMenuItems[NETWORKMENU_ITEM_PRIMARYDNS] ) );
    pTexture->DrawText( XPos, NETWORKMENU_PRIMARYDNS_YPOS, dwHeadTextCol, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", g_pwszNetworkMenuItems[NETWORKMENU_ITEM_PRIMARYDNS] );

    ZeroMemory( wideString, sizeof( WCHAR ) * 100 );
    XPos = MENUBOX_TEXTAREA_X_CENTER + 5.0f;
	_snwprintf( wideString, 100, L"%S", g_XboxSettings.GetPrimaryDNS() );
    pTexture->DrawText( XPos, NETWORKMENU_PRIMARYDNS_YPOS, dwValueTextCol, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%ls", wideString );

    ////////////////////////
    // Secondary DNS Address
    ////////////////////////

    // If we are on the currently selected item, let's set the text color
    if( GetSelectedItem() == (int)NETWORKMENU_ITEM_SECONDARYDNS )
    {
        dwHeadTextCol = ITEM_SELECTED_HEADER_COLOR;
        dwValueTextCol = ITEM_SELECTED_VALUE_COLOR;
    }
    else
    {
        dwHeadTextCol = NETWORKMENU_OPTIONHEADER_COLOR;
        dwValueTextCol = SCREEN_DEFAULT_TEXT_FG_COLOR;
    }

    XPos = MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( g_pwszNetworkMenuItems[NETWORKMENU_ITEM_SECONDARYDNS] ) );
    pTexture->DrawText( XPos, NETWORKMENU_SECONDARYDNS_YPOS, dwHeadTextCol, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", g_pwszNetworkMenuItems[NETWORKMENU_ITEM_SECONDARYDNS] );

    ZeroMemory( wideString, sizeof( WCHAR ) * 100 );
    XPos = MENUBOX_TEXTAREA_X_CENTER + 5.0f;
	_snwprintf( wideString, 100, L"%S", g_XboxSettings.GetSecondaryDNS() );
    pTexture->DrawText( XPos, NETWORKMENU_SECONDARYDNS_YPOS, dwValueTextCol, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%ls", wideString );


    ///////////////
    // Subnet Mask
    ///////////////

    // If we are on the currently selected item, let's set the text color
    if( GetSelectedItem() == (int)NETWORKMENU_ITEM_SUBNETMASK )
    {
        dwHeadTextCol = ITEM_SELECTED_HEADER_COLOR;
        dwValueTextCol = ITEM_SELECTED_VALUE_COLOR;
    }
    else
    {
        dwHeadTextCol = NETWORKMENU_OPTIONHEADER_COLOR;
        dwValueTextCol = SCREEN_DEFAULT_TEXT_FG_COLOR;
    }

    XPos = MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( g_pwszNetworkMenuItems[NETWORKMENU_ITEM_SUBNETMASK] ) );
    pTexture->DrawText( XPos, NETWORKMENU_SUBNET_YPOS, dwHeadTextCol, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", g_pwszNetworkMenuItems[NETWORKMENU_ITEM_SUBNETMASK] );
    
    ZeroMemory( wideString, sizeof( WCHAR ) * 100 );
    XPos = MENUBOX_TEXTAREA_X_CENTER + 5.0f;
    _snwprintf( wideString, 100, L"%S", g_XboxSettings.GetSubnetMask() );
    pTexture->DrawText( XPos, NETWORKMENU_SUBNET_YPOS, dwValueTextCol, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", wideString );


    ///////////////
    // Gateway
    ///////////////

    // If we are on the currently selected item, let's set the text color
    if( GetSelectedItem() == (int)NETWORKMENU_ITEM_GATEWAY )
    {
        dwHeadTextCol = ITEM_SELECTED_HEADER_COLOR;
        dwValueTextCol = ITEM_SELECTED_VALUE_COLOR;
    }
    else
    {
        dwHeadTextCol = NETWORKMENU_OPTIONHEADER_COLOR;
        dwValueTextCol = SCREEN_DEFAULT_TEXT_FG_COLOR;
    }

    XPos = MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( g_pwszNetworkMenuItems[NETWORKMENU_ITEM_GATEWAY] ) );
    pTexture->DrawText( XPos, NETWORKMENU_GATEWY_YPOS, dwHeadTextCol, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", g_pwszNetworkMenuItems[NETWORKMENU_ITEM_GATEWAY] );

    ZeroMemory( wideString, sizeof( WCHAR ) * 100 );
    XPos = MENUBOX_TEXTAREA_X_CENTER + 5.0f;
    _snwprintf( wideString, 100, L"%S", g_XboxSettings.GetGateway() );
    pTexture->DrawText( XPos, NETWORKMENU_GATEWY_YPOS, dwValueTextCol, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", wideString );


    ///////////////
    // Machine Name
    ///////////////

    // If we are on the currently selected item, let's set the text color
    if( GetSelectedItem() == (int)NETWORKMENU_ITEM_MACHINENAME )
    {
        dwHeadTextCol = ITEM_SELECTED_HEADER_COLOR;
        dwValueTextCol = ITEM_SELECTED_VALUE_COLOR;
    }
    else
    {
        dwHeadTextCol = NETWORKMENU_OPTIONHEADER_COLOR;
        dwValueTextCol = SCREEN_DEFAULT_TEXT_FG_COLOR;
    }

    XPos = MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( g_pwszNetworkMenuItems[NETWORKMENU_ITEM_MACHINENAME] ) );
    pTexture->DrawText( XPos, NETWORKMENU_MANAME_YPOS, dwHeadTextCol, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", g_pwszNetworkMenuItems[NETWORKMENU_ITEM_MACHINENAME] );

    ZeroMemory( wideString, sizeof( WCHAR ) * 100 );
    XPos = MENUBOX_TEXTAREA_X_CENTER + 5.0f;
    _snwprintf( wideString, 100, L"%S", g_XboxSettings.GetMachineName() );
    pTexture->DrawText( XPos, NETWORKMENU_MANAME_YPOS, dwValueTextCol, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", wideString );

    // Unlock the texture
    pTexture->Unlock();
}


// Handle any requests for the joystick (thumb-pad)
void CNetworkMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
{
    switch( joystick )
    {
    // This is the LEFT Thumb Joystick on the controller
    case JOYSTICK_LEFT:
        {
            // Y
            if( ( nThumbY < 0 ) && ( abs( nThumbY ) > m_nJoystickDeadZone ) ) // Move the left joystick down
            {
                // Hand off this to the "DPad" handler
                HandleInput( CONTROL_DPAD_DOWN, bFirstYPress );
            }
            else if( ( nThumbY > 0 ) && ( abs( nThumbY ) > m_nJoystickDeadZone ) )// Move left joystick up
            {
                // Hand off this to the "DPad" handler
                HandleInput( CONTROL_DPAD_UP, bFirstYPress );
            }

            // X
            if( ( nThumbX < 0 ) && ( abs( nThumbX ) > m_nJoystickDeadZone ) ) // Move the left joystick left
            {
            }
            else if( ( nThumbX > 0 ) && ( abs( nThumbX ) > m_nJoystickDeadZone ) )// Move the left joystick right
            {
            }
            break;
        }
    }
}


// Handles input (of the CONTROLS) for the current menu
void CNetworkMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
    // If this is a repeat button press, let's delay a bit
    if( bFirstPress )
    {
        m_keyPressDelayTimer = GetTickCount();
        m_keyPressDelayInterval = KEY_PRESS_INITIAL_DELAY;
    }
    else // Check to see if the repeat press is within our timer, otherwise bail
    {
        // If the interval is too small, bail
        if( ( GetTickCount() - m_keyPressDelayTimer ) < m_keyPressDelayInterval )
        {
            return;
        }
        m_keyPressDelayTimer = GetTickCount();
        m_keyPressDelayInterval = KEY_PRESS_REPEAT_DELAY;
    }

    // Handle Buttons being pressed
    switch( controlPressed )
    {
    case CONTROL_DPAD_UP:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
                if( GetSelectedItem() > 0 )
                {
                    m_bUpdateTexture = TRUE;

                    // Play the Menu Item Select Sound
                    g_Sounds.PlayMenuItemSelectSound();

                    // Check to see if we are on the display settings
                    // If so, skip over the Subnet Mask and Gateway if our IP Address is NULL
                    if( NETWORKMENU_ITEM_MACHINENAME == GetSelectedItem() )
                    {
                        if( ( '\0' == g_XboxSettings.GetIPAddress()[0] ) && ( '\0' == g_XboxSettings.GetDebugIPAddress()[0] ) )
                        {
                            SetSelectedItem( GetSelectedItem() - 5 );
                        }
                        else
                        {
                            SetSelectedItem( GetSelectedItem() - 1 );
                        }
                    }
                    else // Otherwise, just hop to the previous item
                    {
                        SetSelectedItem( GetSelectedItem() - 1 );
                    }
                }
            }
            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
                if( ( (unsigned int)GetSelectedItem() + 1 ) < GetNumItems() )
                {
                    m_bUpdateTexture = TRUE;

                    // Play the Menu Item Select Sound
                    g_Sounds.PlayMenuItemSelectSound();

                    // If we have the IP Address highlighted, and it's set to NULL, we
                    // should skip over the Subnet mask and Gateway
                    if( NETWORKMENU_ITEM_DEBUGIPADDRESS == GetSelectedItem() )
                    {
                        if( ( '\0' == g_XboxSettings.GetIPAddress()[0] ) && ( '\0' == g_XboxSettings.GetDebugIPAddress()[0] ) )
                        {
                            SetSelectedItem( GetSelectedItem() + 5 );
                        }
                        else
                        {
                            SetSelectedItem( GetSelectedItem() + 1 );
                        }
                    }
                    else // Otherwise, just hop to the next item
                    {
                        SetSelectedItem( GetSelectedItem() + 1 );
                    }
                }
            }
            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CNetworkMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // If it's a repeat press, let's bail
    if( !bFirstPress )
    {
        return;
    }

    // Handle Buttons being pressed
    switch( buttonPressed )
    {
    case BUTTON_A:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                switch( GetSelectedItem() )
                {
                case NETWORKMENU_ITEM_IPADDRESS:
                    {
                        m_IPMenu.SetTitleIP( TRUE );
                        m_IPMenu.SetAddress( g_XboxSettings.GetIPAddress() );
                        Leave( &m_IPMenu );

                        break;                
                    }
                case NETWORKMENU_ITEM_DEBUGIPADDRESS:
                    {
                        m_IPMenu.SetTitleIP( FALSE );
                        m_IPMenu.SetAddress( g_XboxSettings.GetDebugIPAddress() );
                        Leave( &m_IPMenu );

                        break;                
                    }
				case NETWORKMENU_ITEM_PRIMARYDNS:
					{
						m_DNSMenu.SetPrimaryDNS( TRUE );
						m_DNSMenu.SetAddress( g_XboxSettings.GetPrimaryDNS() );
						Leave( &m_DNSMenu );

						break;
					}
				case NETWORKMENU_ITEM_SECONDARYDNS:
					{
						m_DNSMenu.SetPrimaryDNS( FALSE );
						m_DNSMenu.SetAddress( g_XboxSettings.GetSecondaryDNS() );
						Leave( &m_DNSMenu );

						break;
					}
                case NETWORKMENU_ITEM_SUBNETMASK:
                    {
                        m_SubnetMenu.SetAddress( g_XboxSettings.GetSubnetMask() );
                        Leave( &m_SubnetMenu );

                        break;
                    }
                case NETWORKMENU_ITEM_GATEWAY:
                    {
                        m_GatewayMenu.SetAddress( g_XboxSettings.GetGateway() );
                        Leave( &m_GatewayMenu );

                        break;
                    }
                case NETWORKMENU_ITEM_MACHINENAME:
                    {
                        m_MachineNameMenu.SetMachineName( g_XboxSettings.GetMachineName() );
                        Leave( &m_MachineNameMenu );

                        break;
                    }
                }
            }

            break;
        }
    case BUTTON_B:
        {
            // Go Back one menu
            if( GetParent() )
            {
                // Play the Back Sound
                g_Sounds.PlayBackSound();

                Leave( GetParent() );
            }

            break;
        }
    }
}


// Initialize the Menu
HRESULT CNetworkMenu::Init( char* menuFileName )
{
    XDBGTRC( APP_TITLE_NAME_A, "CNetworkMenu::Init()" );

    HRESULT hr = CMenuScreen::Init( menuFileName );

    // Initialize our IP Menu
    m_IPMenu.Init( FILE_DATA_MENU_DIRECTORY_A "\\ip.mnu" );
    m_IPMenu.SetParent( this );

	m_DNSMenu.Init( FILE_DATA_MENU_DIRECTORY_A "\\dns.mnu" );
	m_DNSMenu.SetParent( this );

    // Initialize our Subnet Menu
    m_SubnetMenu.Init( FILE_DATA_MENU_DIRECTORY_A "\\subnet.mnu" );
    m_SubnetMenu.SetParent( this );

    // Initialize our Gateway Menu
    m_GatewayMenu.Init( FILE_DATA_MENU_DIRECTORY_A "\\gateway.mnu" );
    m_GatewayMenu.SetParent( this );

    // Initialize our Machine Name Menu
    m_MachineNameMenu.Init( FILE_DATA_MENU_DIRECTORY_A "\\machine.mnu" );
    m_MachineNameMenu.SetParent( this );

    return hr;
}


// Get a pointer to a sub-menu
CXItem* CNetworkMenu::GetItemPtr( enum XShellMenuIds menuId )
{
    CXItem* pReturn = NULL;

    if( ENUM_SHELLMENUID_NETWORK_IPADDRESS == menuId )
    {
        pReturn = &m_IPMenu;
    }
    else if( ENUM_SHELLMENUID_NETWORK_SUBNET == menuId )
    {
        pReturn = &m_SubnetMenu;
    }
    else if( ENUM_SHELLMENUID_NETWORK_PRIMARYDNS == menuId )
    {
        pReturn = &m_DNSMenu;
    }
    else if( ENUM_SHELLMENUID_NETWORK_SECONDARYDNS == menuId )
    {
        pReturn = &m_DNSMenu;
    }
    else if( ENUM_SHELLMENUID_NETWORK_GATEWAY == menuId )
    {
        pReturn = &m_GatewayMenu;
    }
    else if( ENUM_SHELLMENUID_NETWORK_MACHINENAME == menuId )
    {
        pReturn = &m_MachineNameMenu;
    }

    return pReturn;
}


// This will be called whenever the user enters this menu
void CNetworkMenu::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();
}