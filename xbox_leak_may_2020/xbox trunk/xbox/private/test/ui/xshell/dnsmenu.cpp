/*****************************************************
*** dnsmenu.cpp
***
*** CPP file for our XShell DNS address menu class.
***
*** by James Helm
*** November 13th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "dnsmenu.h"

// extern BOOL             g_bUpdateNameAddrTexture; // Determines if we need to update the machine name & IP Address
extern CXSettings       g_XboxSettings;           // Settings that are stored on the Xbox
extern CSoundCollection g_Sounds;                 // Used to generate all sounds for the XShell

// Constructors
CDNSMenu::CDNSMenu( CXItem* pParent )
: CKeypadMenu( pParent ),
m_bSetPrimaryDNS( TRUE )
{
    // Use DHCP should be disabled
    m_keyPad[3][3].setRender(FALSE);

    SetTitle( MENU_TITLE_DNS );
}


// Destructor
CDNSMenu::~CDNSMenu()
{
}


// Draws a menu on to the screen
void CDNSMenu::Action( CUDTexture* pTexture )
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

    // Draw the DNS Title Header Information
    float fXPos;
    float fYPos = DNSMENU_ADDRESS_HEADER_TEXT_YPOS;

    if( m_bSetPrimaryDNS )
    {
        fXPos = MENUBOX_TEXTAREA_X_CENTER - GetStringPixelWidth( DNSMENU_PRIMARY_DNS_HEADER_TEXT );
        pTexture->DrawText( fXPos, fYPos, COLOR_MEDIUM_GREEN, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls", DNSMENU_PRIMARY_DNS_HEADER_TEXT );
    }
    else
    {
        fXPos = MENUBOX_TEXTAREA_X_CENTER - GetStringPixelWidth( DNSMENU_SECONDARY_DNS_HEADER_TEXT );
        pTexture->DrawText( fXPos, fYPos, COLOR_MEDIUM_GREEN, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls", DNSMENU_SECONDARY_DNS_HEADER_TEXT );
    }

    pTexture->Unlock();
}


// Handles input (of the BUTTONS) for the current menu
void CDNSMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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

                    if( m_bSetPrimaryDNS )
                    {
                        if( strcmp( g_XboxSettings.GetPrimaryDNS(), pszIPAddress ) == 0 )
                        {
                            m_bUpdateTexture = TRUE;

                            // TODO: Play Error Sound

                            return;
                        }
                    }
                    else
                    {
                        if( strcmp( g_XboxSettings.GetSecondaryDNS(), pszIPAddress ) == 0 )
                        {
                            m_bUpdateTexture = TRUE;

                            // TODO: Play Error Sound

                            return;
                        }
                    }
                }

                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                if( m_bSetPrimaryDNS )
                {
                    if( FAILED( g_XboxSettings.SetPrimaryDNS( m_pwszAddress ) ) )
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "CDNSMenu::HandleInput():Failed to store the settings!!" );
                    }
                }
                else
                {
                    if( FAILED( g_XboxSettings.SetSecondaryDNS( m_pwszAddress ) ) )
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "CDNSMenu::HandleInput():Failed to store the settings!!" );
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
			else
			{
                // Ensure we don't exceed the limit of an IP Address
                if( wcslen( m_pwszAddress ) < DATAVALUE_IPADDRESS_LENGTH )
                {
                    m_bUpdateTexture = TRUE;

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
void CDNSMenu::Leave( CXItem* pItem )
{
    // Call our base-class leave
    CKeypadMenu::Leave( pItem );
}

// This will be called whenever the user enters this menu
void CDNSMenu::Enter()
{
    // Call the base-class enter
    CKeypadMenu::Enter();

    // Make sure we don't process the first keypress, and set our item back to 0
    m_bDropKeyPress = TRUE;
    m_xKeypos = m_yKeypos = 0;
}

// Used to determine if we should set the Title IP, or the Debug IP Address (TRUE for Title)
void CDNSMenu::SetPrimaryDNS( BOOL bSetPrimary )
{
    m_bSetPrimaryDNS = bSetPrimary;
}
