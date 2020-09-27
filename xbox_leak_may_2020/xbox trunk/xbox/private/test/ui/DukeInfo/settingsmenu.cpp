/*****************************************************
*** settingsmenu.cpp
***
*** CPP file for our XShell Launch menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** December 2nd, 2000
***
*****************************************************/

#include "constants.h"
#include "settingsmenu.h"

extern CXItem* g_pCurrentScreen;  // Pointer to the current menu object

// Constructors
CSettingsMenu::CSettingsMenu()
: CMenuScreen()
{
    XDBGTRC( APP_TITLE_NAME_A, "CSettingsMenu::CSettingsMenu()" );

    m_uiNumItems = NUM_MAX_ENUM + 1;
    SetSelectedItem( 0 );
}


CSettingsMenu::CSettingsMenu( CXItem* pParent )
: CMenuScreen( pParent )
{
    XDBGTRC( APP_TITLE_NAME_A, "CSettingsMenu::CSettingsMenu(ex)" );

    m_uiNumItems = NUM_MAX_ENUM + 1;
    SetSelectedItem( 0 );
}


// Destructor
CSettingsMenu::~CSettingsMenu()
{
}


// Draws a menu on to the screen
void CSettingsMenu::Action( CXBoxVideo* pScreen )
{
    unsigned int nStringWidth = 0;
    float fScreenCenter = ( ( MENUBOX_SELECTBAR_X2 - MENUBOX_SELECTBAR_X1 ) / 2.0f ) + MENUBOX_SELECTBAR_X1;

    // Draw the bitmap items
    m_bitmapItems.MoveTo( 0 );  // Start at the 1st Item
    for( unsigned int x = 0; x < m_bitmapItems.GetNumItems(); ++x )
    {
        m_bitmapItems.GetCurrentNode()->render( pScreen->GetD3DDevicePtr() );
        m_bitmapItems.MoveNext();
    }
    
    // Draw the panel items
    m_panelItems.MoveTo( 0 );  // Start at the 1st Item
    for( x = 0; x < m_panelItems.GetNumItems(); ++x )
    {
        pScreen->DrawBox( m_panelItems.GetCurrentNode()->X1Pos, m_panelItems.GetCurrentNode()->Y1Pos, m_panelItems.GetCurrentNode()->X2Pos, m_panelItems.GetCurrentNode()->Y2Pos, m_panelItems.GetCurrentNode()->color );
        m_panelItems.MoveNext();
    }

    // Draw the line items
    m_lineItems.MoveTo( 0 );  // Start at the 1st Item
    for( x = 0; x < m_lineItems.GetNumItems(); ++x )
    {
        pScreen->DrawLine( m_lineItems.GetCurrentNode()->X1Pos, m_lineItems.GetCurrentNode()->Y1Pos, m_lineItems.GetCurrentNode()->X2Pos, m_lineItems.GetCurrentNode()->Y2Pos, m_lineItems.GetCurrentNode()->width, m_lineItems.GetCurrentNode()->color );
        m_lineItems.MoveNext();
    }

    // Draw the outline items
    m_outlineItems.MoveTo( 0 );  // Start at the 1st Item
    for( x = 0; x < m_outlineItems.GetNumItems(); ++x )
    {
        pScreen->DrawOutline( m_outlineItems.GetCurrentNode()->X1Pos, m_outlineItems.GetCurrentNode()->Y1Pos, m_outlineItems.GetCurrentNode()->X2Pos, m_outlineItems.GetCurrentNode()->Y2Pos, m_outlineItems.GetCurrentNode()->width, m_outlineItems.GetCurrentNode()->color );
        m_outlineItems.MoveNext();
    }

    // Draw the header items
    m_headerItems.MoveTo( 0 );  // Start at the 1st Item
    for( x = 0; x < m_headerItems.GetNumItems(); ++x )
    {
        pScreen->DrawText( m_headerItems.GetCurrentNode()->XPos, m_headerItems.GetCurrentNode()->YPos, m_headerItems.GetCurrentNode()->fgcolor, m_headerItems.GetCurrentNode()->bgcolor, L"%s", m_headerItems.GetCurrentNode()->text );
        m_headerItems.MoveNext();
    } 
    
    // Draw the text items
    m_textItems.MoveTo( NUM_MAX_ENUM + 1 );  // Start at the item past our screen index
    for( x = NUM_MAX_ENUM + 1; x < m_textItems.GetNumItems(); ++x )
    {
        pScreen->DrawText( m_textItems.GetCurrentNode()->XPos, m_textItems.GetCurrentNode()->YPos, m_textItems.GetCurrentNode()->fgcolor, m_textItems.GetCurrentNode()->bgcolor, L"%s", m_textItems.GetCurrentNode()->text );
        m_textItems.MoveNext();
    }

    // Draw a box behind the currently highlighted choice
    float X1Pos = MENUBOX_SELECTBAR_X1;
    float Y1Pos = m_textItems[GetSelectedItem()]->YPos - ITEM_SELECTOR_BORDER_SPACE;
    float X2Pos = MENUBOX_SELECTBAR_X2;
    float Y2Pos = Y1Pos + m_uiFontHeightOfItem + ( 2 * ITEM_SELECTOR_BORDER_SPACE );

    pScreen->DrawBox( X1Pos, Y1Pos, X2Pos, Y2Pos, ITEM_SELECTOR_COLOR );

    // Load the settings in to our object from the config sector of the
    // XBox
    m_XBoxSettings.LoadSettings();
    
    WCHAR wideString[100];

    // IP Address
    m_textItems.MoveTo( ENUM_IPADDRESS );
    float XPos = fScreenCenter  - ( pScreen->GetStringPixelWidth( m_textItems.GetCurrentNode()->text ) );
    pScreen->DrawText( XPos, m_textItems.GetCurrentNode()->YPos, m_textItems.GetCurrentNode()->fgcolor, m_textItems.GetCurrentNode()->bgcolor, L"%s", m_textItems.GetCurrentNode()->text );

    ZeroMemory( wideString, sizeof( WCHAR ) * 100 );
    XPos = fScreenCenter + 5.0f;

    // If our IP Address is null, display "dhcp" info
    if( '\0' == m_XBoxSettings.GetSetting( XC_ONLINE_IP_ADDRESS )[0] )
    {
        _snwprintf( wideString, 100, L"(use dhcp)" );
    }
    else
    {
        _snwprintf( wideString, 100, L"%S", m_XBoxSettings.GetSetting( XC_ONLINE_IP_ADDRESS ) );
    }
    pScreen->DrawText( XPos, m_textItems.GetCurrentNode()->YPos, COLOR_DARK_GREEN, m_textItems.GetCurrentNode()->bgcolor, L"%s", wideString );

    // Subnet Mask
    m_textItems.MoveTo( ENUM_SUBNETMASK );
    XPos = fScreenCenter - ( pScreen->GetStringPixelWidth( m_textItems.GetCurrentNode()->text ) );
    pScreen->DrawText( XPos, m_textItems.GetCurrentNode()->YPos, m_textItems.GetCurrentNode()->fgcolor, m_textItems.GetCurrentNode()->bgcolor, L"%s", m_textItems.GetCurrentNode()->text );
    
    ZeroMemory( wideString, sizeof( WCHAR ) * 100 );
    XPos = fScreenCenter + 5.0f;
    _snwprintf( wideString, 100, L"%S", m_XBoxSettings.GetSetting( XC_ONLINE_SUBNET_ADDRESS ) );
    pScreen->DrawText( XPos, m_textItems.GetCurrentNode()->YPos, COLOR_DARK_GREEN, m_textItems.GetCurrentNode()->bgcolor, L"%s", wideString );

    // Gateway
    m_textItems.MoveTo( ENUM_GATEWAY );
    XPos = fScreenCenter - ( pScreen->GetStringPixelWidth( m_textItems.GetCurrentNode()->text ) );
    pScreen->DrawText( XPos, m_textItems.GetCurrentNode()->YPos, m_textItems.GetCurrentNode()->fgcolor, m_textItems.GetCurrentNode()->bgcolor, L"%s", m_textItems.GetCurrentNode()->text );

    ZeroMemory( wideString, sizeof( WCHAR ) * 100 );
    XPos = fScreenCenter + 5.0f;
    _snwprintf( wideString, 100, L"%S", m_XBoxSettings.GetSetting( XC_ONLINE_DEFAULT_GATEWAY_ADDRESS ) );
    pScreen->DrawText( XPos, m_textItems.GetCurrentNode()->YPos, COLOR_DARK_GREEN, m_textItems.GetCurrentNode()->bgcolor, L"%s", wideString );

    // Display Settings
    m_textItems.MoveTo( ENUM_DISPLAYSETTING );
    XPos = fScreenCenter - ( pScreen->GetStringPixelWidth( m_textItems.GetCurrentNode()->text ) );
    pScreen->DrawText( XPos, m_textItems.GetCurrentNode()->YPos, m_textItems.GetCurrentNode()->fgcolor, m_textItems.GetCurrentNode()->bgcolor, L"%s", m_textItems.GetCurrentNode()->text );

    ZeroMemory( wideString, sizeof( WCHAR ) * 100 );
    XPos = fScreenCenter + 5.0f;
    _snwprintf( wideString, 100, L"%S", m_XBoxSettings.GetSetting( XC_VIDEO_FLAGS ) );
    pScreen->DrawText( XPos, m_textItems.GetCurrentNode()->YPos, COLOR_DARK_GREEN, m_textItems.GetCurrentNode()->bgcolor, L"%s", wideString );

    // Machine Name
    m_textItems.MoveTo( ENUM_MACHINENAME );
    XPos = fScreenCenter - ( pScreen->GetStringPixelWidth( m_textItems.GetCurrentNode()->text ) );
    pScreen->DrawText( XPos, m_textItems.GetCurrentNode()->YPos, m_textItems.GetCurrentNode()->fgcolor, m_textItems.GetCurrentNode()->bgcolor, L"%s", m_textItems.GetCurrentNode()->text );

    ZeroMemory( wideString, sizeof( WCHAR ) * 100 );
    XPos = fScreenCenter + 5.0f;
    _snwprintf( wideString, 100, L"%S", m_XBoxSettings.GetSetting( XSETTINGS_MACHINE_NAME ) );
    pScreen->DrawText( XPos, m_textItems.GetCurrentNode()->YPos, COLOR_DARK_GREEN, m_textItems.GetCurrentNode()->bgcolor, L"%s", wideString );
}


// Handle any requests for the joystick (thumb-pad)
void CSettingsMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, bool bFirstYPress, bool bFirstXPress )
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
                HandleInput( CONTROL_DPAD_BOTTOM, bFirstYPress );
            }
            else if( ( nThumbY > 0 ) && ( abs( nThumbY ) > m_nJoystickDeadZone ) )// Move left joystick up
            {
                // Hand off this to the "DPad" handler
                HandleInput( CONTROL_DPAD_TOP, bFirstYPress );
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
    // This is the RIGHT Thumb Joystick on the controller
    case JOYSTICK_RIGHT:
        {
            // Y
            if( nThumbX < 0 ) // Move the right joystick to the left
            {
            }
            else if( nThumbX > 0 ) // Move right joystick to the right
            {
            }
            
            // X
            if( nThumbX < 0 ) // Move the right joystick left
            {
            }
            else if( nThumbX > 0 ) // Move the right joystick right
            {
            }

            break;
        }
    }
}


// Handles input (of the CONTROLS) for the current menu
void CSettingsMenu::HandleInput( enum CONTROLS controlPressed, bool bFirstPress )
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
	case CONTROL_START:
		{
			break;
		}
	case CONTROL_SELECT:
		{
			break;
		}
	case CONTROL_TRIGGER_LEFT:
		{
			break;
		}
	case CONTROL_TRIGGER_RIGHT:
		{
			break;
		}
    case CONTROL_DPAD_TOP:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
                if( GetSelectedItem() > 0 )
                {
                    // Check to see if we are on the display settings
                    // If so, skip over the Subnet Mask and Gateway if our IP Address is NULL
                    if( ENUM_MACHINENAME == GetSelectedItem() )
                    {
                        if( '\0' == m_XBoxSettings.GetSetting( XC_ONLINE_IP_ADDRESS )[0] )
                        {
                            SetSelectedItem( GetSelectedItem() - 3 );
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
    case CONTROL_DPAD_BOTTOM:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
                if( ( (unsigned int)GetSelectedItem() + 1 ) < GetNumItems() )
                {
                    // If we have the IP Address highlighted, and it's set to NULL, we
                    // should skip over the Subnet mask and Gateway
                    if( ENUM_IPADDRESS == GetSelectedItem() )
                    {
                        if( '\0' == m_XBoxSettings.GetSetting( XC_ONLINE_IP_ADDRESS )[0] )
                        {
                            SetSelectedItem( GetSelectedItem() + 3 );
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
    case CONTROL_DPAD_LEFT:
        {
            break;
        }
    case CONTROL_DPAD_RIGHT:
        {
            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CSettingsMenu::HandleInput( enum BUTTONS buttonPressed, bool bFirstPress )
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
                switch( GetSelectedItem() )
                {
                case ENUM_IPADDRESS:
                    {
                        m_IPMenu.SetAddress( m_XBoxSettings.GetSetting( XC_ONLINE_IP_ADDRESS ) );
                        g_pCurrentScreen = &m_IPMenu;
                        break;                
                    }
                case ENUM_SUBNETMASK:
                    {
                        m_SubnetMenu.SetAddress( m_XBoxSettings.GetSetting( XC_ONLINE_SUBNET_ADDRESS ) );
                        g_pCurrentScreen = &m_SubnetMenu;
                        break;
                    }
                case ENUM_GATEWAY:
                    {
                        m_GatewayMenu.SetAddress( m_XBoxSettings.GetSetting( XC_ONLINE_DEFAULT_GATEWAY_ADDRESS ) );
                        g_pCurrentScreen = &m_GatewayMenu;
                        break;
                    }
                case ENUM_DISPLAYSETTING:
                    {
                        m_DisplaySettingsMenu.SetDisplaySetting( m_XBoxSettings.GetDisplaySettingDW() );
                        g_pCurrentScreen = &m_DisplaySettingsMenu;
                        break;
                    }
                case ENUM_MACHINENAME:
                    {
                        m_MachineNameMenu.SetMachineName( m_XBoxSettings.GetSetting( XSETTINGS_MACHINE_NAME ) );
                        g_pCurrentScreen = &m_MachineNameMenu;
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
                g_pCurrentScreen = GetParent();
            }
            break;
        }
    case BUTTON_C:
        {
            break;
        }
    case BUTTON_D:
        {
            break;
        }
    case BUTTON_E:
        {
            break;
        }
    case BUTTON_F:
        {
            break;
        }
    case BUTTON_7:
        {
            break;
        }
    case BUTTON_8:
        {
            break;
        }
    case BUTTON_9:
        {
            break;
        }
    case BUTTON_10:
        {
            break;
        }
    }
}


// Initialize the Menu
HRESULT CSettingsMenu::Init( CXBoxVideo* pScreen, char* menuFileName )
{
    XDBGTRC( APP_TITLE_NAME_A, "CSettingsMenu::Init()" );

     // Get our XBox Settings
    m_XBoxSettings.Init();

    // Get our screen information
    ImportScreenItems( menuFileName );

    // Get the height of the font
    unsigned int decent;
    pScreen->GetFontSize( &m_uiFontHeightOfItem, &decent );

    // Initialize our IP Menu
    m_IPMenu.Init( pScreen, FILE_DATA_MENU_DIRECTORY_A "\\ip.mnu" );
    m_IPMenu.SetParent( this );
    m_IPMenu.SetSettingsPtr( &m_XBoxSettings );

    // Initialize our Subnet Menu
    m_SubnetMenu.Init( pScreen, FILE_DATA_MENU_DIRECTORY_A "\\subnet.mnu" );
    m_SubnetMenu.SetParent( this );
    m_SubnetMenu.SetSettingsPtr( &m_XBoxSettings );

    // Initialize our Gateway Menu
    m_GatewayMenu.Init( pScreen, FILE_DATA_MENU_DIRECTORY_A "\\gateway.mnu" );
    m_GatewayMenu.SetParent( this );
    m_GatewayMenu.SetSettingsPtr( &m_XBoxSettings );

    // Initialize our Display Settings Menu
    m_DisplaySettingsMenu.Init( pScreen, FILE_DATA_MENU_DIRECTORY_A "\\dispset.mnu" );
    m_DisplaySettingsMenu.SetParent( this );
    m_DisplaySettingsMenu.SetSettingsPtr( &m_XBoxSettings );

    // Initialize our Machine Name Menu
    m_MachineNameMenu.Init( pScreen, FILE_DATA_MENU_DIRECTORY_A "\\machine.mnu" );
    m_MachineNameMenu.SetParent( this );
    m_MachineNameMenu.SetSettingsPtr( &m_XBoxSettings );

    return S_OK;
}