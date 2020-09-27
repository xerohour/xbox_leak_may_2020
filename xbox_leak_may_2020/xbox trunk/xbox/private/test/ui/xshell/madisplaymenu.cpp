/*****************************************************
*** madisplaymenu.cpp
***
*** CPP file for our XShell Memory Area Display 
*** menu class.  This class will list out all Memory
*** devices on the system, and display their names
*** in the proper format, with the amount of space
*** free on each device
***
*** by James N. Helm
*** June 7th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "madisplaymenu.h"

extern CSoundCollection g_Sounds;           // Used to generate all sounds for the XShell
extern BOOL             g_bMUsChanged;      // Tracks whether MU's have been inserted or removed
extern DWORD            g_dwInsertedMUs;    // Tracks which MUs are inserted or removed
extern CMemoryUnit      g_MemoryUnits[8];   // Memory Units that can be inserted, or removed
extern CHardDrive       g_XboxHardDrive;    // Used to configure the Xbox Hard Drive
extern XFONT*           g_pFont;

// Constructors
CMADisplayMenu::CMADisplayMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_bCancelled( FALSE ),
m_bDisplaySize( TRUE ),
m_bAllowSelect( FALSE )
{
    XDBGTRC( APP_TITLE_NAME_A, "CMADisplayMenu::CMADisplayMenu(ex)" );

    m_uiNumItems = NUM_XBOX_MU_SLOTS + 1;
    SetSelectedItem( 0 );

    // Zero out our users
    ZeroMemory( m_aOnlineUsers, sizeof( m_aOnlineUsers ) );

    SetTitle( MENU_TITLE_MADISPLAY );
}


// Destructor
CMADisplayMenu::~CMADisplayMenu()
{
}


// Draws a menu on to the screen
void CMADisplayMenu::Action( CUDTexture* pTexture )
{
    // If the state has changed, update our texture and our previous mask
    if( g_bMUsChanged || m_bUpdateTexture )
        m_bUpdateTexture = TRUE;

    // Check to see if we should update the texture
    // If not, return
    if( !m_bUpdateTexture )
        return;
    else // reset the texture flag to FALSE
        m_bUpdateTexture = FALSE;

    // Lock our texture and set it as the render target
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    // Let our base menu draw the items from the data file
    CMenuScreen::Action( pTexture );

    // List our Controllers and MU's (if needed)
    // Update our Font Height
    XFONT_SetTextHeight( g_pFont, ITEM_MADISPLAYMENU_FONT_HEIGHT );
    m_uiFontHeightOfItem = ITEM_MADISPLAYMENU_FONT_HEIGHT;

    // Draw a selector if needed
    if( ( GetNumItems() - 1 ) != GetSelectedItem() ) // A Memory Unit must be selected
    {
        // Make sure our selected item still exists
        if( !( g_dwInsertedMUs & g_dwMUBitMask[GetSelectedItem()] ) )
        {
            // Determine if there is another item we can select
            for( int y = 0; y < NUM_XBOX_MU_SLOTS; ++y )
            {
                if( g_dwInsertedMUs & g_dwMUBitMask[y] )
                    break;
            }

            SetSelectedItem( y );
        }
    }

    if( m_bAllowSelect )
    {
        float X1Pos = MENUBOX_SELECTBAR_X1;
        float Y1Pos = MENUBOX_TEXTAREA_Y1 + MADISPLAYMENU_TOP_BORDER_SIZE + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * GetSelectedItem() ) - ITEM_SELECTOR_BORDER_SPACE;
        float X2Pos = MENUBOX_SELECTBAR_X2;
        float Y2Pos = Y1Pos + m_uiFontHeightOfItem + ( 2 * ITEM_SELECTOR_BORDER_SPACE );
        
        pTexture->DrawBox( X1Pos, Y1Pos, X2Pos, Y2Pos, ITEM_SELECTOR_COLOR );
    }

    // Draw the MU Devices items
    for( unsigned int x = 0; x < NUM_XBOX_MU_SLOTS; ++x )
    {
        float XPos = MENUBOX_TEXTAREA_X1;
        float YPos = MENUBOX_TEXTAREA_Y1 + MADISPLAYMENU_TOP_BORDER_SIZE + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * x );

        // Skip any MU's that are not inserted
        if( !( g_dwInsertedMUs & g_dwMUBitMask[x] ) )
        {
            pTexture->DrawText( XPos, YPos, MUFORMATDLG_UNAVAIL_MU_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls", g_MemoryUnits[x].GetDefaultNamePtr() );
            continue;
        }
        
        // Check to see if our MU is formatted or not
        if( !g_MemoryUnits[x].IsFormatted() )
        //
        // No, the MU is not formatted, display the appropriate message
        //
        {
            pTexture->DrawText( XPos, YPos, MADISPLAYMENU_UNFORMATTED_MU_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls (Not Formatted)", g_MemoryUnits[x].GetDefaultNamePtr() );
        }
        else
        //
        // Yes, the MU is formatted, display the appropriate message
        //
        {
            if( m_bDisplaySize )
            {
                // Check to see if the MU has a name
                if( g_MemoryUnits[x].IsNamed() )    // Yes, the MU has a name
                    pTexture->DrawText( XPos, YPos, MADISPLAYMENU_AVAIL_MU_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls - %ls (%d/%d)", g_MemoryUnits[x].GetDefaultNamePtr(), g_MemoryUnits[x].GetNamePtr(), g_MemoryUnits[x].GetFreeBlocks(), g_MemoryUnits[x].GetTotalBlocks() - 1 );
                else    // No, the MU doesn't have a name
                    pTexture->DrawText( XPos, YPos, MADISPLAYMENU_AVAIL_MU_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls (%d/%d)", g_MemoryUnits[x].GetDefaultNamePtr(), g_MemoryUnits[x].GetFreeBlocks(), g_MemoryUnits[x].GetTotalBlocks() - 1 );
            }
            else // Display user information
            {
                // Get the name (if any) of user from each memory area
                BOOL bUserExists = FALSE;
                HRESULT hr = OnlineGetUserFromMU( g_MemoryUnits[x].GetPort(),
                                                  g_MemoryUnits[x].GetSlot(),
                                                  (PXONLINE_USER) m_aOnlineUsers,
                                                  &bUserExists );
                if( FAILED( hr ) )
                    XDBGWRN( APP_TITLE_NAME_A, "CMADisplayMenu::Action():Failed to get the users from the MU!! - '#%d', Error - '0x%08X', '%d'", x, hr, hr );

                CHAR pszMessage[MAX_PATH+1];
                pszMessage[MAX_PATH] = '\0';

                if( bUserExists )
                    sprintf( pszMessage, "%hs.%hs", m_aOnlineUsers[0].name, m_aOnlineUsers[0].kingdom );
                else
                    strcpy( pszMessage, "0 Users" );                    

                // Display the proper message to the user (No MU Name)
                pTexture->DrawText( XPos, YPos, MADISPLAYMENU_AVAIL_MU_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls (%hs)", g_MemoryUnits[x].GetDefaultNamePtr(), pszMessage );
            }
        }
    }

    float XPos = MENUBOX_TEXTAREA_X1;
    float YPos = MENUBOX_TEXTAREA_Y1 + MADISPLAYMENU_TOP_BORDER_SIZE + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * x );

    if( m_bDisplaySize )
    {
        // Add the block sizes to the hard drive memory unit
        if( g_XboxHardDrive.GetFreeBlocks() >= MEMORY_AREA_HIDDEN_BLOCK_BOUNDS )
            pTexture->DrawText( XPos, YPos, MADISPLAYMENU_AVAIL_MU_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s (%d+ blocks available)", g_XboxHardDrive.GetDefaultNamePtr(), MEMORY_AREA_HIDDEN_BLOCK_BOUNDS );
        else
            pTexture->DrawText( XPos, YPos, MADISPLAYMENU_AVAIL_MU_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s (%d blocks available)", g_XboxHardDrive.GetDefaultNamePtr(), g_XboxHardDrive.GetFreeBlocks() );
    }
    else // Display Users
    {
        // Get the number of users that live on the Hard Drive
        DWORD dwNumUsers = 0;
        HRESULT hr = OnlineGetUsersFromHD( (PXONLINE_USER) m_aOnlineUsers, &dwNumUsers );
        if( FAILED( hr ) )
            XDBGWRN( APP_TITLE_NAME_A, "CMADisplayMenu::Action():Failed to get the users from the hard drive!!" );

        if( 1 == dwNumUsers )
            pTexture->DrawText( XPos, YPos, MADISPLAYMENU_AVAIL_MU_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s (%hs.%hs)", g_XboxHardDrive.GetDefaultNamePtr(), m_aOnlineUsers[0].name, m_aOnlineUsers[0].kingdom );
        else
            pTexture->DrawText( XPos, YPos, MADISPLAYMENU_AVAIL_MU_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s (%d %ls)", g_XboxHardDrive.GetDefaultNamePtr(), dwNumUsers, dwNumUsers == 1 ? L"User" : L"Users" );
    }


    // Correct our Font Height
    m_uiFontHeightOfItem = FONT_DEFAULT_HEIGHT;
    XFONT_SetTextHeight( g_pFont, FONT_DEFAULT_HEIGHT );

    // Unlock our texture and restore our render target
    pTexture->Unlock();
}


// Handle any requests for the joystick (thumb-pad)
void CMADisplayMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
void CMADisplayMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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
            return;

        m_keyPressDelayTimer = GetTickCount();
        m_keyPressDelayInterval = KEY_PRESS_REPEAT_DELAY;
    }

    // Handle Buttons being pressed
    switch( controlPressed )
    {
    case CONTROL_DPAD_UP:
        {
            // Only act if we have any items on our screen
            if( ( GetNumItems() != 0 ) && m_bAllowSelect && ( GetSelectedItem() > 0 ) )
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
            if( ( GetNumItems() != 0 ) && m_bAllowSelect && ( GetSelectedItem() < (int)( GetNumItems() - 1 ) ) )
            {
                for( int x = GetSelectedItem() + 1; x < NUM_XBOX_MU_SLOTS; ++x )
                {
                    if( g_dwInsertedMUs & g_dwMUBitMask[x] )
                        break;
                }

                // Since we are changing the selection, mark our texture for updating
                m_bUpdateTexture = TRUE;

                // Play the Menu Item Select Sound
                g_Sounds.PlayMenuItemSelectSound();

                SetSelectedItem( x );
            }

            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CMADisplayMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // If it's a repeat press, let's bail
    if( !bFirstPress )
        return;

    // Handle Buttons being pressed
    switch( buttonPressed )
    {
    case BUTTON_A:
        {
            // Play the Select Sound
            g_Sounds.PlaySelectSound();

            // Go Back one menu
            if( GetParent() )
                Leave( GetParent() );

            break;
        }
    case BUTTON_B:
        {
            m_bCancelled = TRUE;

            // Play the Back Sound
            g_Sounds.PlayBackSound();

            // Go Back one menu
            if( GetParent() )
                Leave( GetParent() );

            break;
        }
    }
}


// Initialize the Menu
HRESULT CMADisplayMenu::Init( char* menuFileName )
{
    XDBGTRC( APP_TITLE_NAME_A, "CMADisplayMenu::Init()" );

    HRESULT hr = CMenuScreen::Init( menuFileName );

    return hr;
}


// This will be called whenever the user leaves this menu
void CMADisplayMenu::Leave( CXItem* pItem )
{
    CMenuScreen::Leave( pItem );

    // Reset the menu to it's defaults
    SetTitle( MENU_TITLE_MADISPLAY );
    SetInfo( TRUE, FALSE );
}


// This will be called whenever the user enters this menu
void CMADisplayMenu::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();

    // Reset our cancelled state
    m_bCancelled = FALSE;

    // Zero out our users
    ZeroMemory( m_aOnlineUsers, sizeof( m_aOnlineUsers ) );

    // SetSelectedItem( 0 );
}


// Returns the Memory Area that the user selected
unsigned int CMADisplayMenu::GetSelectedMemArea()
{
    // Hard Drive Selected
    if( ( GetNumItems() - 1 ) == GetSelectedItem() )
        return 0;

    return( GetSelectedItem() + 1 );
}


// Sets the menu to allow certain actions
void CMADisplayMenu::SetInfo( BOOL bDisplaySize, BOOL bAllowSelect )
{
    m_bDisplaySize = bDisplaySize;
    m_bAllowSelect = bAllowSelect;
}