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
*** Modified
*** 03-16-2001 by James N. Helm
*****************************************************/

#include "stdafx.h"
#include "memorymenu.h"

extern CHardDrive    g_HardDrive;       // Xbox Hard Drive

// Constructors
CMemoryMenu::CMemoryMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_MenuMUOptions( NULL ),
m_MenuHDOptions( NULL ),
m_dwInsertedMUs( 0 ),
m_dwMUInsertions( 0 ),
m_dwMURemovals( 0 )
{
    XDBGWRN( APP_TITLE_NAME_A, "CMemoryMenu::CMemoryMenu(ex)" );

    m_uiNumItems = MEMORYMENU_ITEM_NUM_ITEMS;
    SetSelectedItem( 0 );

    // Initialize our Memory Units
    m_MemoryUnits[0].SetPortSlot( XDEVICE_PORT0, XDEVICE_TOP_SLOT );
    m_MemoryUnits[1].SetPortSlot( XDEVICE_PORT0, XDEVICE_BOTTOM_SLOT );
    m_MemoryUnits[2].SetPortSlot( XDEVICE_PORT1, XDEVICE_TOP_SLOT );
    m_MemoryUnits[3].SetPortSlot( XDEVICE_PORT1, XDEVICE_BOTTOM_SLOT );
    m_MemoryUnits[4].SetPortSlot( XDEVICE_PORT2, XDEVICE_TOP_SLOT );
    m_MemoryUnits[5].SetPortSlot( XDEVICE_PORT2, XDEVICE_BOTTOM_SLOT );
    m_MemoryUnits[6].SetPortSlot( XDEVICE_PORT3, XDEVICE_TOP_SLOT );
    m_MemoryUnits[7].SetPortSlot( XDEVICE_PORT3, XDEVICE_BOTTOM_SLOT );
}


// Destructor
CMemoryMenu::~CMemoryMenu()
{
    // Unmount any MU's that are still mounted
    for( unsigned int x = 0; x < 8; x++ )
    {
        if( m_MemoryUnits[x].IsMounted() )
        {
            m_MemoryUnits[x].Unmount();
        }
    }
}


// Draws a menu on to the screen
void CMemoryMenu::Action( CUDTexture* pTexture )
{
    // If the state has changed, update our texture and our previous mask
    if( XGetDeviceChanges( XDEVICE_TYPE_MEMORY_UNIT, &m_dwMUInsertions, &m_dwMURemovals ) || m_bUpdateTexture )
    {
        m_bUpdateTexture = TRUE;

        // Mount or Unmount our MU's
        for( unsigned int x = 0; x < ARRAYSIZE( g_wpszMUPortNames ); ++x )
        {
            // Check to see if the MU was removed
            if( m_dwMURemovals & g_dwMUBitMask[x] )
            {
                // Remove the MU from our Mask
                m_dwInsertedMUs = m_dwInsertedMUs & ( ~ g_dwMUBitMask[x] );

                // Unmount the MU
                m_MemoryUnits[x].Unmount();
            }

            // Check to see if the MU is inserted
            if( m_dwMUInsertions & g_dwMUBitMask[x] )
            {
                // Add the MU to our Mask
                m_dwInsertedMUs = m_dwInsertedMUs | g_dwMUBitMask[x];

                // Mount the MU
                m_MemoryUnits[x].Mount();
            }
        }
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

    // Draw a box behind the currently highlighted choice
    // Check to see if the selector bar IS NOT on the XBox Hard Drive, and make sure the device is still plugged in
    if( ( !( m_dwInsertedMUs & g_dwMUBitMask[GetSelectedItem()] ) ) && ( GetSelectedItem() != MEMORYMENU_ITEM_XBOX_HARD_DRIVE ) )
    {
        // Determine if there is another item we can select
        SetSelectedItem( MEMORYMENU_ITEM_XBOX_HARD_DRIVE ); // Default to the Xbox Hard Drive
        
        for( int y = 0; y < ( XBOX_CONSOLE_NUM_PORTS * XBOX_CONTROLLER_NUM_SLOTS ); ++y )
        {
            if( m_dwInsertedMUs & g_dwMUBitMask[y] )
            {
                SetSelectedItem( y );
                break;
            }
        }
    }

    // Draw the selector bar
    float X1Pos = MENUBOX_SELECTOR_X1;
    float Y1Pos = MENUBOX_MAINAREA_Y1 + MENUBOX_MAINAREA_BORDERWIDTH_TOP + ( ( MENUBOX_ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * GetSelectedItem() ) - MENUBOX_SELECTOR_BORDER_WIDTH;
    float X2Pos = MENUBOX_SELECTOR_X2;
    float Y2Pos = Y1Pos + m_uiFontHeightOfItem + ( 2 * MENUBOX_SELECTOR_BORDER_WIDTH );

    pTexture->DrawBox( X1Pos, Y1Pos, X2Pos, Y2Pos, SCREEN_DEFAULT_SELECTOR_COLOR );

    // Draw the Menu items
    for( unsigned int x = 0; x < MEMORYMENU_ITEM_NUM_ITEMS; ++x )
    {
        DWORD dwColor;                  // Used to display the memory area in the proper color        
        WCHAR pwszAreaName[MAX_PATH];   // Used to display the name of the Area on the screen
        
        ZeroMemory( pwszAreaName, sizeof( WCHAR ) * MAX_PATH );


        // Calculate the position of the item on the menu
        float XPos = MENUBOX_MAINAREA_X1 + MENUBOX_MAINAREA_BORDERWIDTH_LEFT;
        float YPos = MENUBOX_MAINAREA_Y1 + MENUBOX_MAINAREA_BORDERWIDTH_TOP + ( ( MENUBOX_ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * x );

        //
        // Set the default colors for our options
        //

        // If the current item is the selected item, let's change it's color
        if( GetSelectedItem() == (int)x )
        //
        // Yes, the current Memory Area is selected
        //
        {
            dwColor = SCREEN_SELECTED_OPTION_COLOR;
        }
        else
        //
        // No, the Memory Area is not selected
        //
        {
            dwColor = SCREEN_ACTIVE_OPTION_COLOR;
        }

        // Check to see if we are dealing with the hard drive
        if( x != MEMORYMENU_ITEM_XBOX_HARD_DRIVE )
        //
        // No, we are dealing with a Memory Unit
        //
        {
            if( m_dwInsertedMUs & g_dwMUBitMask[x] )
            //
            // Yes, the MU is inserted
            //
            {
                // Check the format state of the MU
                if( !m_MemoryUnits[x].IsFormatted() )
                //
                // No, the MU is NOT formatted
                {
                    dwColor = SCREEN_UNFORMATTED_OPTION_COLOR;
                }
            }
            else
            //
            // No, the MU is not inserted
            //
            {
                dwColor = SCREEN_INACTIVE_OPTION_COLOR;
            }

            // Setup a buffer to hold the on-screen display for the MU
            int NumChars = 0;

            // Determine if we should put in place the drive letter & Disk Space
            if( m_MemoryUnits[x].IsMounted() )
            {
                // Set the color of the MU
                dwColor = SCREEN_MOUNTED_OPTION_COLOR;

                // Insert the Drive Letter in to our path variable
                NumChars = _snwprintf( pwszAreaName, MAX_PATH - 1, L"%c: - ", m_MemoryUnits[x].GetUDataDrive() );

                // Get the Free Space to display on the screen
                NumChars += _snwprintf( pwszAreaName + NumChars, ( MAX_PATH - 1 ) - NumChars, L"(%d/%d) ", m_MemoryUnits[x].GetFreeBlocks(), m_MemoryUnits[x].GetTotalBlocks() );
            }

            // Determine if we should draw the MU Name
            WCHAR pwszCurrentMUName[XBOX_MU_NAME_LENGTH+1];
            ZeroMemory( pwszCurrentMUName, sizeof( WCHAR ) * ( XBOX_MU_NAME_LENGTH + 1 ) );

            m_MemoryUnits[x].GetName( pwszCurrentMUName, XBOX_MU_NAME_LENGTH );

            if( pwszCurrentMUName[0] != '\0' )
            {
                NumChars += _snwprintf( pwszAreaName + NumChars, ( MAX_PATH - 1 ) - NumChars, L"(%s)", pwszCurrentMUName );
            }
            else // Draw the Generic Name
            {
                NumChars += _snwprintf( pwszAreaName + NumChars, ( MAX_PATH - 1 ) - NumChars, L"%s", g_wpszMUPortNames[x] );

                // Determine if we should display the 'not formatted' message
                // if( ( '\0' == m_acDriveLetters[x] ) && ( m_dwInsertedMUs & g_dwMUBitMask[x] ) )
                if( ( m_dwInsertedMUs & g_dwMUBitMask[x] ) && ( !m_MemoryUnits[x].IsFormatted() ) )
                {
                    NumChars += _snwprintf( pwszAreaName + NumChars, ( MAX_PATH - 1 ) - NumChars, L" (Not Formatted)" );
                }
            }
        }
        else
        //
        // Yes, we are dealing with the Xbox Hard Drive
        //
        {
            _snwprintf( pwszAreaName, ( MAX_PATH - 1 ), L"(%d/%d) %s", g_HardDrive.GetFreeBlocks(), g_HardDrive.GetTotalBlocks(), g_wpszMemoryMenuItems[x] );
        }
        
        // Draw the Memory Area name to our screen
        pTexture->DrawText( XPos, YPos, dwColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", pwszAreaName );
    }

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
        m_keyPressDelayInterval = INPUT_KEYPRESS_INITIAL_DELAY;
    }
    else // Check to see if the repeat press is within our timer, otherwise bail
    {
        // If the interval is too small, bail
        if( ( GetTickCount() - m_keyPressDelayTimer ) < m_keyPressDelayInterval )
        {
            return;
        }
        m_keyPressDelayTimer = GetTickCount();
        m_keyPressDelayInterval = INPUT_KEYPRESS_REPEAT_DELAY;
    }

    // Handle Buttons being pressed
    switch( controlPressed )
    {
    case CONTROL_DPAD_UP:
        {
            for( int x = GetSelectedItem() - 1; x >= 0; --x )
            {
                if( m_dwInsertedMUs & g_dwMUBitMask[x] )
                {
                    // Since we are changing the selection, mark our texture for updating
                    m_bUpdateTexture = TRUE;

                    SetSelectedItem( x );
                    break;
                }
            }
            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            // Check to see if there are any options below us
            if( GetSelectedItem() < ( MEMORYMENU_ITEM_NUM_ITEMS - 1 ) )
            {
                // Since we are changing the selection, mark our texture for updating
                m_bUpdateTexture = TRUE;
                
                // Look through the memory units (if necessary) and choose a new one if it
                // is inserted, and lower on the list
                for( int x = GetSelectedItem() + 1; x < MEMORYMENU_ITEM_NUM_ITEMS; ++x )
                {
                    // Check to see if we are dealing with the hard drive -- Special Case
                    if( x == MEMORYMENU_ITEM_XBOX_HARD_DRIVE )
                    //
                    // Yes, we are dealing with the Hard Drive
                    //
                    {
                        SetSelectedItem( x );

                        break;
                    }
                    else
                    //
                    // No, we are dealing with an MU
                    //
                    {
                        if( m_dwInsertedMUs & g_dwMUBitMask[x] )
                        {
                            SetSelectedItem( x );
                     
                            break;
                        }
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
            switch( GetSelectedItem() )
            {
            case MEMORYMENU_ITEM_XBOX_HARD_DRIVE:
                {
                    Leave( &m_MenuHDOptions );
                }
                break;
            default:    // Memory Unit
                {
                    m_MenuMUOptions.SetMemoryUnit( &m_MemoryUnits[GetSelectedItem()] );
                    Leave( &m_MenuMUOptions );
                }
                break;
            }

            // Fill up the XDisk
            // FillDisk( FILE_DATA_DRIVE_A "\\" );

            // Refresh our Texture
            // m_bUpdateTexture = TRUE;

            break;
        }
    case BUTTON_B:
        {
            // Go Back one menu
            if( GetParent() )
            {
                Leave( GetParent() );
            }
            break;
        }
    }
}


// Initialize the Menu
HRESULT CMemoryMenu::Init( IDirect3DDevice8* pD3DDevice, char* menuFileName )
{
    XDBGWRN( APP_TITLE_NAME_A, "CMemoryMenu::Init()" );

    CMenuScreen::Init( pD3DDevice, menuFileName );

    // Initialize our sub-menus
    m_MenuMUOptions.SetParent( this );
    m_MenuMUOptions.Init( pD3DDevice, MENU_MUOPTIONS_MENU_FILENAME_A );

    m_MenuHDOptions.SetParent( this );
    m_MenuHDOptions.Init( pD3DDevice, MENU_HDOPTIONS_MENU_FILENAME_A );

    return S_OK;
}


/*
// Should be called whenever a user leaves the menu
void CMemoryMenu::Leave( CXItem* pItem )
{
    // Default our selection to the top item when the user returns
    // SetSelectedItem( 0 );

    g_pCurrentScreen = pItem;
}
*/

// This will be called whenever the user enters this menu
void CMemoryMenu::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();

    //
    // Update the MU's on our screen
    //

    // Unmount any previously mounted MU
    for( unsigned int x = 0; x < 8; ++x )
    {
        if( m_MemoryUnits[x].IsMounted() )
        {
            m_MemoryUnits[x].Unmount();
        }
    }

    // Mount any MU that is inserted
    m_dwInsertedMUs = XGetDevices( XDEVICE_TYPE_MEMORY_UNIT );
    for( unsigned int x = 0; x < 8; ++x )
    {
        if( m_dwInsertedMUs & g_dwMUBitMask[x] )
        {
            m_MemoryUnits[x].Mount();
        }
    }
}
