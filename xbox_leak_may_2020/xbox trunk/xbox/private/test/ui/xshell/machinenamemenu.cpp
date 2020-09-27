/*****************************************************
*** machinenamemenu.cpp
***
*** CPP file for our XShell Machine Name class.
***
*** by Victor Blanco
*** December 2nd, 2000
***
*****************************************************/

#include "stdafx.h"
#include "machinenamemenu.h"
#include "launchmenu.h"

extern CXItem*          g_pRootMenu;                // Pointer to the menu that is our "main, or root" menu
extern DWORD            g_dwShellState;             // Defines the current state of the XShell
extern BOOL             g_bUpdateNameAddrTexture;   // Determines if we need to update the machine name & IP Address
extern CXSettings       g_XboxSettings;             // Settings that are stored on the Xbox
extern CSoundCollection g_Sounds;                   // Used to generate all sounds for the XShell

// Constructors
CMachineNameMenu::CMachineNameMenu( CXItem* pParent )
: CKeyboardMenu( pParent ),
m_bDisplayInUseMessage( FALSE )
{
	ZeroMemory( m_MachineName, sizeof( WCHAR ) * DATAVALUE_MACHINENAME_LENGTH + 1 );

    SetTitle( MENU_TITLE_MACHINE );
}


// Destructor
CMachineNameMenu::~CMachineNameMenu()
{
}


// Draws a menu on to the screen
void CMachineNameMenu::Action( CUDTexture* pTexture )
{
    // If we don't need to update our texture, return
    if( !m_bUpdateTexture )
    {
        return;
    }
    else // Toggle our Texture Update flag
    {
        m_bUpdateTexture = FALSE;
    }

    // Lock our texture and set it as the render target
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    CKeyboardMenu::Action( pTexture );

	// Draw our MachineName field
	pTexture->DrawText( ITEM_MACHINENAME_DATA_X, KEYBOARD_MACHINENAMEHEADER_YPOS, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", m_MachineName );

    // Draw the Machine Name in use error message if needed
    if( m_bDisplayInUseMessage )
    {
        pTexture->DrawText( MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( MENU_MACHINENAME_INUSE_MESSAGE ) / 2.0f ), MENU_MACHINENAME_INUSE_YPOS, COLOR_TVSAFE_RED, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", MENU_MACHINENAME_INUSE_MESSAGE );
    }

    // Unlock our Texture
    pTexture->Unlock();
}


// Handles input (of the BUTTONS) for the current menu
void CMachineNameMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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
				int len = wcslen( m_MachineName );
				
				if(len > 0)
				{
                    m_bUpdateTexture = TRUE;
                    m_bDisplayInUseMessage = FALSE;  // Make sure our in use message is disabled

                    // Play the keyboard stroke sound
                    g_Sounds.PlayKeyboardStrokeSound();

					len = len - 1;
					m_MachineName[len] = L'\0';
				}
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"ok" ) )      //ok
			{
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                if( FAILED( g_XboxSettings.SetMachineName( m_MachineName ) ) )
                {
                    XDBGWRN( APP_TITLE_NAME_A, "CMachineNameMenu::HandleInput():Failed to set the machine name, already in use!!" );

                    m_bUpdateTexture = TRUE;
                    m_bDisplayInUseMessage = TRUE;

                    return;
                }

                g_bUpdateNameAddrTexture = TRUE;    // We changed the machine name, update the Name/Addr Texture
                
                // If we are in Config Mode, return to the root
                if( SHELLSTATE_NOCONFIG & g_dwShellState )
                {
                    g_dwShellState &= ~SHELLSTATE_NOCONFIG;     // Set the state back to normal

                    Leave( g_pRootMenu );                       // Return to the root menu
                }
                else if( GetParent() )                          // Go Back one menu
                {
                    Leave( GetParent() );
                }
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"cancel" ) )  //cancel
			{
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                // If we are in Config Mode, return to the root
                if( SHELLSTATE_NOCONFIG & g_dwShellState )
                {
                    g_dwShellState &= ~SHELLSTATE_NOCONFIG;     // Set the state back to normal
                    Leave( g_pRootMenu );                   // Return to the root menu
                }
                else if( GetParent() ) // Go Back one menu
                {
                    Leave( GetParent() );
                }
			}
			else
			{
                // Ensure we don't exceed the limit of an MachineName
                if( wcslen( m_MachineName ) < DATAVALUE_MACHINENAME_LENGTH )
                {
                    m_bUpdateTexture = TRUE;

                    // Play the keyboard stroke sound
                    g_Sounds.PlayKeyboardStrokeSound();

				    wcscat( m_MachineName, m_keyPad[m_xKeypos][m_yKeypos].resultChar );

                    m_bDisplayInUseMessage = FALSE;  // Make sure our in use message is disabled
                    
                    if( m_bShift )
                    {
                        m_bShift = FALSE;                // Make sure we toggle out of shift mode
                        GenerateKeypad();
                    }
                }
			}
            break;
        }
    case BUTTON_B:
        {
            // If we are in Config Mode, return to the root
            if( SHELLSTATE_NOCONFIG & g_dwShellState )
            {
                // Play the Back Sound
                g_Sounds.PlayBackSound();

                g_dwShellState &= ~SHELLSTATE_NOCONFIG; // Set the state back to normal
                Leave( g_pRootMenu );               // Return to the root menu
            }
            else if( GetParent() ) // Go Back one menu
            {
                // Play the Back Sound
                g_Sounds.PlayBackSound();

                Leave( GetParent() );
            }

            break;
        }
    case BUTTON_WHITE:  // Backspace
        {
			int len = wcslen( m_MachineName );
			
			if(len > 0)
			{
                m_bUpdateTexture = TRUE;
                m_bDisplayInUseMessage = FALSE;  // Make sure our in use message is disabled

                // Play the keyboard stroke sound
                g_Sounds.PlayKeyboardStrokeSound();

				len = len - 1;
				m_MachineName[len] = L'\0';
			}

            break;
        }
    }
}


// Sets the MachineName in to our local member
HRESULT CMachineNameMenu::SetMachineName( char* MachineName )
{
    if( !MachineName )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CMachineNameMenu::SetMachineName( char ):MachineName was not set!!" );

        return E_INVALIDARG;
    }

    ZeroMemory( m_MachineName, sizeof( WCHAR ) * DATAVALUE_MACHINENAME_LENGTH + 1 );
    _snwprintf( m_MachineName, DATAVALUE_MACHINENAME_LENGTH, L"%S", MachineName );

    return S_OK;
}


// Sets the MachineName in to our local member
HRESULT CMachineNameMenu::SetMachineName( WCHAR* MachineName )
{
    if( !MachineName )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CMachineNameMenu::SetMachineName( WCHAR ):MachineName was not set!!" );

        return E_INVALIDARG;
    }

    ZeroMemory( m_MachineName, sizeof( WCHAR ) * DATAVALUE_MACHINENAME_LENGTH + 1 );
    _snwprintf( m_MachineName, DATAVALUE_MACHINENAME_LENGTH, MachineName );

    return S_OK;
}


// This should be called whenever a user leaves this menu
void CMachineNameMenu::Leave( CXItem* pItem )
{
    // Call our base-class leave
    CKeyboardMenu::Leave( pItem );

    if( m_bShift )
    {
        m_bShift = FALSE;
        GenerateKeypad();
    }
    m_bDisplayInUseMessage = FALSE;
    m_bDropKeyPress = TRUE;
    m_xKeypos = m_yKeypos = 0;
}


// This will be called whenever the user enters this menu
void CMachineNameMenu::Enter()
{
    // Call the base-class enter
    CKeyboardMenu::Enter();
}