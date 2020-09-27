/*****************************************************
*** memorymenu.cpp
***
*** CPP file for our XShell Memory menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** January 10th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "memorymenu.h"

extern CSoundCollection g_Sounds;           // Used to generate all sounds for the XShell
extern BOOL             g_bMUsChanged;      // Tracks whether MU's have been inserted or removed
extern DWORD            g_dwInsertedMUs;    // Tracks which MUs are inserted or removed
extern CMemoryUnit      g_MemoryUnits[8];   // Memory Units that can be inserted, or removed
extern XFONT*           g_pFont;

// Constructors
CMemoryMenu::CMemoryMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_MemoryOptionsMenu( NULL )
{
    XDBGTRC( APP_TITLE_NAME_A, "CMemoryMenu::CMemoryMenu(ex)" );

    m_uiNumItems = NUM_XBOX_MU_SLOTS;
    SetSelectedItem( 0 );

    SetTitle( MENU_TITLE_MEMORY );
}


// Destructor
CMemoryMenu::~CMemoryMenu()
{
}


// Draws a menu on to the screen
void CMemoryMenu::Action( CUDTexture* pTexture )
{
    // If the state has changed, update our texture and our previous mask
    if( g_bMUsChanged || m_bUpdateTexture )
    {
        m_bUpdateTexture = TRUE;
    }

    // Check to see if we should update the texture
    // If not, return
    if( !m_bUpdateTexture )
    {
        return;
    }
    else // reset the texture flag to FALSE
    {
        m_bUpdateTexture = FALSE;
    }

    // Lock our texture and set it as the render target
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    // Let our base menu draw the items from the data file
    CMenuScreen::Action( pTexture );

    // List our Controllers and MU's (if needed)
    // Update our Font Height
    XFONT_SetTextHeight( g_pFont, ITEM_FORMATMU_FONT_HEIGHT );
    m_uiFontHeightOfItem = ITEM_FORMATMU_FONT_HEIGHT;

    BOOL bDrawSelector = TRUE;

    // Draw a box behind the currently highlighted choice if needed
    if( !( g_dwInsertedMUs & g_dwMUBitMask[GetSelectedItem()] ) )
    {
        bDrawSelector = FALSE;

        // Determine if there is another item we can select
        for( int y = 0; y < NUM_XBOX_MU_SLOTS; ++y )
        {
            if( g_dwInsertedMUs & g_dwMUBitMask[y] )
            {
                SetSelectedItem( y );
                bDrawSelector = TRUE;
                break;
            }
        }
    }

    // Draw the selector bar if we have to
    if( bDrawSelector )
    {
        float X1Pos = MENUBOX_SELECTBAR_X1;
        float Y1Pos = MENUBOX_TEXTAREA_Y1 + MUFORMATDLG_TOP_BORDER_SIZE + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * GetSelectedItem() ) - ITEM_SELECTOR_BORDER_SPACE;
        float X2Pos = MENUBOX_SELECTBAR_X2;
        float Y2Pos = Y1Pos + m_uiFontHeightOfItem + ( 2 * ITEM_SELECTOR_BORDER_SPACE );
        
        pTexture->DrawBox( X1Pos, Y1Pos, X2Pos, Y2Pos, ITEM_SELECTOR_COLOR );
    }

    // Draw the MU Devices items
    for( unsigned int x = 0; x < NUM_XBOX_MU_SLOTS; ++x )
    {
        DWORD dwColor;

        // Check to see if the MU is inserted
        if( g_dwInsertedMUs & g_dwMUBitMask[x] )
        //
        // Yes, the MU is inserted
        //
        {
            // Check to see if the MU is formatted
            if( !g_MemoryUnits[x].IsFormatted() )
            //
            // No, it's not formatted, color the MU accordingly
            //
            {
                dwColor = MUFORMATDLG_UNFORMATTED_MU_COLOR;
            }
            else
            //
            // Yes, it is formatted, determine if it's the selected MU, and color it
            // accordingly
            //
            {
                // Check to see if the MU is selected
                if( GetSelectedItem() == (int)x )
                //
                // Yes, the current MU is the selected one
                //
                {
                    dwColor = MUFORMATDLG_SELECTED_MU_COLOR;
                }
                else
                //
                // No, the current MU is not the selected one
                //
                {
                    dwColor = MUFORMATDLG_AVAIL_MU_COLOR;
                }
            }
        }
        else
        //
        // No, the MU is not inserted
        //
        {
            dwColor = MUFORMATDLG_UNAVAIL_MU_COLOR;
        }
        
        float XPos = MENUBOX_TEXTAREA_X1;
        float YPos = MENUBOX_TEXTAREA_Y1 + MUFORMATDLG_TOP_BORDER_SIZE + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * x );

        // Check to see if our MU is formatted or not
        if( ( g_dwInsertedMUs & g_dwMUBitMask[x] ) && ( !g_MemoryUnits[x].IsFormatted() ) )
        //
        // No, the MU is not formatted, display the appropriate message
        //
        {
            pTexture->DrawText( XPos, YPos, dwColor, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s (Not Formatted)", g_pwszMUPortNames[x] );
        }
        else
        //
        // Yes, the MU is formatted, display the appropriate message
        //
        {
            // Check to see if the MU has a name
            WCHAR pwszCurrentMUName[MAX_MUNAME];
            ZeroMemory( pwszCurrentMUName, sizeof( WCHAR ) * MAX_MUNAME );

            g_MemoryUnits[x].GetName( pwszCurrentMUName, MAX_MUNAME );

            if( pwszCurrentMUName[0] != '\0' )
            //
            // Yes, the MU has a name
            //
            {
                pTexture->DrawText( XPos, YPos, dwColor, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s (%s)", g_pwszMUPortNames[x], pwszCurrentMUName );
            }
            else
            //
            // No, the MU doesn't have a name
            //
            {
                pTexture->DrawText( XPos, YPos, dwColor, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", g_pwszMUPortNames[x] );
            }
        }
    }

    // Correct our Font Height
    m_uiFontHeightOfItem = FONT_DEFAULT_HEIGHT;
    XFONT_SetTextHeight( g_pFont, FONT_DEFAULT_HEIGHT );

    // Unlock our texture and restore our render target
    pTexture->Unlock();
}


// Handle any requests for the joystick (thumb-pad)
void CMemoryMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
void CMemoryMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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
                for( int x = GetSelectedItem() - 1; x >= 0; --x )
                {
                    if( g_dwInsertedMUs & g_dwMUBitMask[x] )
                    {
                        // Since we are changing the selection, mark our texture for updating
                        m_bUpdateTexture = TRUE;

                        // Play the Menu Item Select Sound
                        g_Sounds.PlayMenuItemSelectSound();

                        SetSelectedItem( x );

                        break;
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
                for( int x = GetSelectedItem() + 1; x < NUM_XBOX_MU_SLOTS; ++x )
                {
                    if( g_dwInsertedMUs & g_dwMUBitMask[x] )
                    {
                        // Since we are changing the selection, mark our texture for updating
                        m_bUpdateTexture = TRUE;

                        // Play the Menu Item Select Sound
                        g_Sounds.PlayMenuItemSelectSound();

                        SetSelectedItem( x );

                        break;
                    }
                }
            }
            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CMemoryMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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
            // Only act if we have any MUs inserted
            if( g_dwInsertedMUs & g_dwMUBitMask[GetSelectedItem()] )
            {
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                m_MemoryOptionsMenu.SetMemoryUnit( &g_MemoryUnits[GetSelectedItem()] );

                Leave( &m_MemoryOptionsMenu );
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
HRESULT CMemoryMenu::Init( char* menuFileName )
{
    XDBGTRC( APP_TITLE_NAME_A, "CMemoryMenu::Init()" );

    CMenuScreen::Init( menuFileName );

    // Init our Memory Options Menu Screen
    m_MemoryOptionsMenu.SetParent( this ); // Must call this BEFORE Init, since Init of MemOpts will use "GetParent()"
    m_MemoryOptionsMenu.Init( FILE_DATA_MENU_DIRECTORY_A "\\memopts.mnu" );

    return S_OK;
}


// This will be called whenever the user enters this menu
void CMemoryMenu::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();
}
