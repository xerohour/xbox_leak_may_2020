/*****************************************************
*** subnetmenu.cpp
***
*** CPP file for our XShell subnet mask menu class.
***
*** by Victor Blanco
*** December 2nd, 2000
***
*****************************************************/

#include "stdafx.h"
#include "subnetmenu.h"

extern CXSettings       g_XboxSettings;     // Settings that are stored on the Xbox
extern CSoundCollection g_Sounds;           // Used to generate all sounds for the XShell

// Constructors
CSubnetMenu::CSubnetMenu( CXItem* pParent )
: CKeypadMenu( pParent )
{
    // Use DHCP should be disabled
    m_keyPad[3][3].setRender(FALSE);

    SetTitle( MENU_TITLE_SUBNET );
}


// Destructor
CSubnetMenu::~CSubnetMenu()
{
}


// Draws a menu on to the screen
void CSubnetMenu::Action( CUDTexture* pTexture )
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

    // Draw the Subnet Header Information
    float fXPos= MENUBOX_TEXTAREA_X_CENTER - GetStringPixelWidth( SUBNETMENU_HEADER_TEXT );
    float fYPos = 24.0;

    pTexture->DrawText( fXPos, fYPos, COLOR_MEDIUM_GREEN, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls", SUBNETMENU_HEADER_TEXT );

    pTexture->Unlock();
}


// Handles input (of the BUTTONS) for the current menu
void CSubnetMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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
            if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"«" ) )            //backspace
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
            else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"ok" ) )      //ok
            {
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                if( FAILED( g_XboxSettings.SetSubnetMask( m_pwszAddress ) ) )
                {
                    XDBGWRN( APP_TITLE_NAME_A, "CSubnetMenu::HandleInput():Failed to store the settings!!" );
                }

                // Go Back one menu
                if( GetParent() )
                {
                    Leave( GetParent() );
                }
            }
            else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"cancel" ) )  //cancel
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
                // Ensure we don't exceed the limit of an Subnet Mask
                if( wcslen( m_pwszAddress ) < DATAVALUE_IPADDRESS_LENGTH )
                {
                    m_bUpdateTexture = TRUE;

                    // Play the keyboard stroke sound
                    g_Sounds.PlayKeyboardStrokeSound();

                    wcscat( m_pwszAddress, m_keyPad[m_xKeypos][m_yKeypos].resultChar );
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
void CSubnetMenu::Enter()
{
    // Call the base-class enter
    CKeypadMenu::Enter();

    // Make sure we don't process the first keypres and set our item to 0
    m_bDropKeyPress = TRUE;
    m_xKeypos = m_yKeypos = 0;
}