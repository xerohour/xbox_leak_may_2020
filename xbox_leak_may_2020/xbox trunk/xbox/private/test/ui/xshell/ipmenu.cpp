/*****************************************************
*** ipmenu.cpp
***
*** CPP file for our XShell ip menu class.
***
*** by Victor Blanco
*** December 2nd, 2000
***
*****************************************************/

#include "stdafx.h"
#include "ipmenu.h"

extern BOOL             g_bUpdateNameAddrTexture; // Determines if we need to update the machine name & IP Address
extern CXSettings       g_XboxSettings;           // Settings that are stored on the Xbox
extern CSoundCollection g_Sounds;                 // Used to generate all sounds for the XShell

// Constructors
CIPMenu::CIPMenu( CXItem* pParent )
: CKeypadMenu( pParent ),
m_bSetTitleIP( TRUE ),
m_bDisplayInUseMessage( FALSE )
{
    SetTitle( MENU_TITLE_IP );
}


// Destructor
CIPMenu::~CIPMenu()
{
}


// Draws a menu on to the screen
void CIPMenu::Action( CUDTexture* pTexture )
{
    // Check to see if we should update our texture, if not, return
    if( !m_bUpdateTexture )
    {
        return;
    }
    else // Reset the Update flag
    {
        m_bUpdateTexture = FALSE;
    }

    // Let our base menu draw the items from the data file
    CKeypadMenu::Action( pTexture );

    pTexture->Lock();

    // Draw the IP Title Header Information
    float fXPos;
    float fYPos = IPMENU_ADDRESS_HEADER_TEXT_YPOS;

    if( m_bSetTitleIP )
    {
        fXPos = MENUBOX_TEXTAREA_X_CENTER - GetStringPixelWidth( IPMENU_TITLE_IP_HEADER_TEXT );
        pTexture->DrawText( fXPos, fYPos, COLOR_MEDIUM_GREEN, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls", IPMENU_TITLE_IP_HEADER_TEXT );
    }
    else
    {
        fXPos = MENUBOX_TEXTAREA_X_CENTER - GetStringPixelWidth( IPMENU_DEBUG_IP_HEADER_TEXT );
        pTexture->DrawText( fXPos, fYPos, COLOR_MEDIUM_GREEN, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls", IPMENU_DEBUG_IP_HEADER_TEXT );
    }

    // Draw the Machine Name in use error message if needed
    if( m_bDisplayInUseMessage )
    {
        pTexture->DrawText( MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( IPMENU_ADDRESS_INUSE_MESSAGE ) / 2.0f ), IPMENU_ADDRESS_INUSE_YPOS, COLOR_TVSAFE_RED, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", IPMENU_ADDRESS_INUSE_MESSAGE );
    }

    pTexture->Unlock();
}


// Handles input (of the BUTTONS) for the current menu
void CIPMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    if( bFirstPress )
    {
        m_bDropKeyPress = FALSE;

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

    if ( m_bDropKeyPress )
    {
        return;
    }

    // Handle Buttons being pressed
    switch( buttonPressed )
    {
    case BUTTON_A:
        {
			// Deal with keyboard input
			if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"«" ) )		//backspace
			{
				int len = wcslen( m_pwszAddress );
				
				if(len > 0)
				{
                    m_bUpdateTexture = TRUE;
                    m_bDisplayInUseMessage = FALSE;  // Make sure our in use message is disabled

                    // Play the keyboard stroke sound
                    g_Sounds.PlayKeyboardStrokeSound();

					len = len - 1;
					m_pwszAddress[len] = L'\0';
				}
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"ok" ) )		//ok
			{
                // See if the user is trying to set an address
                if( m_pwszAddress[0] != 0 )
                {
                    // Check to see if our IP Address is already in use
                    // If so, warn the user, and do NOT set the IP Address
                    char pszIPAddress[DATAVALUE_IPADDRESS_LENGTH+1];
                    _snprintf( pszIPAddress, DATAVALUE_IPADDRESS_LENGTH, "%ls", m_pwszAddress );

                    if( m_bSetTitleIP )
                    {
                        if( strcmp( g_XboxSettings.GetDebugIPAddress(), pszIPAddress ) == 0 )
                        {
                            m_bUpdateTexture = TRUE;
                            m_bDisplayInUseMessage = TRUE;

                            // TODO: Play Error Sound

                            return;
                        }
                    }
                    else
                    {
                        if( strcmp( g_XboxSettings.GetIPAddress(), pszIPAddress ) == 0 )
                        {
                            m_bUpdateTexture = TRUE;
                            m_bDisplayInUseMessage = TRUE;

                            // TODO: Play Error Sound

                            return;
                        }
                    }
                }

                g_bUpdateNameAddrTexture = TRUE;    // Update our Name / Addr Texture

                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                if( m_bSetTitleIP )
                {
                    if( FAILED( g_XboxSettings.SetIPAddress( m_pwszAddress ) ) )
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "CIPMenu::HandleInput():Failed to store the settings!!" );
                    }
                }
                else
                {
                    if( FAILED( g_XboxSettings.SetDebugIPAddress( m_pwszAddress ) ) )
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "CIPMenu::HandleInput():Failed to store the settings!!" );
                    }
                }

                // Go Back one menu
                if( GetParent() )
                {
                    Leave( GetParent() );
                }
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"cancel" ) )	//cancel
			{
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                // Go Back one menu
                if( GetParent() )
                {
                    Leave( GetParent() );
                }
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"use dhcp" ) )	//dhcp
			{
                g_bUpdateNameAddrTexture = TRUE;    // Update our Name / Addr Texture

                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                if( m_bSetTitleIP )
                {
                    if( FAILED( g_XboxSettings.SetIPAddress( "\0" ) ) )
                    {
                        XDBGERR( APP_TITLE_NAME_A, "CIPMenu::HandleInput():Failed to store the settings!!" );
                    }
                }
                else
                {
                    if( FAILED( g_XboxSettings.SetDebugIPAddress( "\0" ) ) )
                    {
                        XDBGERR( APP_TITLE_NAME_A, "CIPMenu::HandleInput():Failed to store the settings!!" );
                    }
                }

                // Go Back one menu
                if( GetParent() )
                {
                    Leave( GetParent() );
                }
			}
			else
			{
                // Ensure we don't exceed the limit of an IP Address
                if( wcslen( m_pwszAddress ) < DATAVALUE_IPADDRESS_LENGTH )
                {
                    m_bUpdateTexture = TRUE;
                    m_bDisplayInUseMessage = FALSE;  // Make sure our in use message is disabled

                    // Play the keyboard stroke sound
                    g_Sounds.PlayKeyboardStrokeSound();

				    wcscat(m_pwszAddress, m_keyPad[m_xKeypos][m_yKeypos].resultChar);
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
    case BUTTON_WHITE:  // Backspace
        {
			int len = wcslen( m_pwszAddress );
			
			if(len > 0)
			{
                m_bUpdateTexture = TRUE;
                m_bDisplayInUseMessage = FALSE;  // Make sure our in use message is disabled

                // Play the keyboard stroke sound
                g_Sounds.PlayKeyboardStrokeSound();

				len = len - 1;
				m_pwszAddress[len] = L'\0';
			}

            break;
        }
    }
}


// This will be called whenever the user enters this menu
void CIPMenu::Leave( CXItem* pItem )
{
    // Call our base-class leave
    CKeypadMenu::Leave( pItem );

    m_bDisplayInUseMessage = FALSE;
}

// This will be called whenever the user enters this menu
void CIPMenu::Enter()
{
    // Call the base-class enter
    CKeypadMenu::Enter();

    // Make sure we don't process the first keypress, and set our item back to 0
    m_bDropKeyPress = TRUE;
    m_xKeypos = m_yKeypos = 0;
}

// Used to determine if we should set the Title IP, or the Debug IP Address (TRUE for Title)
void CIPMenu::SetTitleIP( BOOL bSetTitle )
{
    m_bSetTitleIP = bSetTitle;
}
