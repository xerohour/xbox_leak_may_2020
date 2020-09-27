/*****************************************************
*** muname.cpp
***
*** CPP file for our XShell MU Name class.
*** This screen will name a selected Memory Unit.
***
*** by James N. Helm
*** February 15th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "muname.h"

extern CSoundCollection g_Sounds;           // Used to generate all sounds for the XShell
extern BOOL             g_bMUsChanged;      // Tracks whether MU's have been inserted or removed
extern DWORD            g_dwInsertedMUs;    // Tracks which MUs are inserted or removed

// Constructors
CMUName::CMUName( CXItem* pParent )
: CKeyboardMenu( pParent ),
m_pMemoryUnit( NULL )
{
    XDBGTRC( APP_TITLE_NAME_A, "CMUName::CMUName(ex)" );

    m_uiNumItems = NUM_XBOX_MU_SLOTS;
    SetSelectedItem( 0 );

    ZeroMemory( m_pwszMUName, sizeof( WCHAR ) * MAX_MUNAME );

    SetTitle( MENU_TITLE_MUNAME );
}


// Destructor
CMUName::~CMUName()
{
}

// Draws a menu on to the screen
void CMUName::Action( CUDTexture* pTexture )
{
    //
    // Ensure the MU we are dealing with is still plugged in
    //

    // Determine the state of the MU's and Controllers
    DWORD dwDeviceInsertions;
    DWORD dwDeviceRemovals;

    if( g_bMUsChanged )
    {
        // If the MU is no longer plugged in, immediately return to the previous screen
        if( !( g_dwInsertedMUs & g_dwMUBitMask[GetSelectedItem()] ) )
        {
            if( GetParent() )
            {
                Leave( GetParent() );
            }
        }
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

    // Lock our texture so we can render to it
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    // Let our base menu draw the items from the data file
    CKeyboardMenu::Action( pTexture );

    // Insert the MU and Port the user is dealing with on to the texture
    pTexture->DrawText( MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( g_pwszMUPortNames[GetSelectedItem()] ) / 2.0f ), MUNAMEDLG_MU_TEXT_YPOS, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", g_pwszMUPortNames[GetSelectedItem()] );

	// Draw our MU Name field
	pTexture->DrawText( MUNAMEDLG_VALUE_XPOS, MUNAMEDLG_HEADER_YPOS, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", m_pwszMUName );

    // Unlock our texture
    pTexture->Unlock();
}


// Handles input (of the BUTTONS) for the current menu
void CMUName::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    if( bFirstPress )
        m_bDropKeyPress = FALSE;

    if ( m_bDropKeyPress )
        return;

    if( !HandleInputDelay( bFirstPress ) )
        return;

    // Handle Buttons being pressed
    switch( buttonPressed )
    {
    case BUTTON_A:
        {
			// Deal with keyboard input
			if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"«" ) )            //backspace
			{
				int len = wcslen( m_pwszMUName );
				
				if( len > 0 )
				{
                    m_bUpdateTexture = TRUE;

                    // Play the keyboard stroke sound
                    g_Sounds.PlayKeyboardStrokeSound();

					len = len - 1;
					m_pwszMUName[len] = L'\0';
				}
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"ok" ) )      //ok
			{
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                // Write the name to the MU
                if( FAILED( m_pMemoryUnit->Name( m_pwszMUName ) ) )
                {
                    XDBGWRN( APP_TITLE_NAME_A, "CMUName::HandleInput():Failed to write the name to the MU!!" );
                }

                if( GetParent() )   // Go Back one menu
                    Leave( GetParent() );
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"cancel" ) )  //cancel
			{
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                if( GetParent() )   // Go Back one menu
                {
                    Leave( GetParent() );
                }
			}
			else
			{
                // Ensure we don't exceed the limit of an MU name
                if( wcslen( m_pwszMUName ) < MAX_MUNAME - 1 )
                {
                    m_bUpdateTexture = TRUE;

                    // Play the keyboard stroke sound
                    g_Sounds.PlayKeyboardStrokeSound();

				    wcscat( m_pwszMUName, m_keyPad[m_xKeypos][m_yKeypos].resultChar );
                }
			}
            break;
        }
    case BUTTON_B:
        {
            if( GetParent() )   // Go Back one menu
            {
                // Play the Back Sound
                g_Sounds.PlayBackSound();

                Leave( GetParent() );
            }

            break;
        }
    }
}


// Sets the local member to point at the proper memory unit
HRESULT CMUName::SetMemoryUnit( CMemoryUnit* pMemoryUnit )
{
    XDBGTRC( APP_TITLE_NAME_A, "CMUName::SetMemoryUnit()" );

    if( !pMemoryUnit )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CMUName::SetMemoryUnit():Invalid Argument passed in!!" );

        return E_INVALIDARG;
    }

    m_pMemoryUnit = pMemoryUnit;

    // Set the Selected Item via an index
    SetSelectedItem( m_pMemoryUnit->GetIndex() );

    return S_OK;
}


// This will be called whenever this menu is entered / activated
void CMUName::Enter()
{
    // Call the base-class enter
    CKeyboardMenu::Enter();

    // Make sure we don't process the first keypress and reset our item to 0
    m_bDropKeyPress = TRUE;

    // Setup the MU Name -- Remove the previous name, and load any existing name off of the disk
    ZeroMemory( m_pwszMUName, sizeof( WCHAR ) * MAX_MUNAME );  // Destroy any previous name

    m_pMemoryUnit->GetName( m_pwszMUName, MAX_MUNAME );
}


// Should be called whenever a user leaves the menu
void CMUName::Leave( CXItem* pItem )
{
    CKeyboardMenu::Leave( pItem );
}
