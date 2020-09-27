/*****************************************************
*** rootmenu.cpp
***
*** CPP file for our root menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** March 28th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "rootmenu.h"

extern void Render();               // Render the video on to our screen and present it to the user
extern CHardDrive   g_HardDrive;    // Xbox Hard Drive

// Constructors and Destructors
CRootMenu::CRootMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_MenuMemory( NULL ),
m_MenuST( NULL ),
m_MenuEntry( NULL ),
m_MenuXBConfig( NULL )
{
	m_uiNumItems = ROOTMENU_ITEM_NUM_ITEMS;
	SetSelectedItem( 0 );
}

// Destructor
CRootMenu::~CRootMenu()
{
}

void CRootMenu::Action( CUDTexture* pTexture )
{
	// Check the "dirty" flag, and draw our items
	if( !m_bUpdateTexture )
	{
		return;
	}

	pTexture->Lock();

	// Call our base call method to handle generic input
	CMenuScreen::Action( pTexture );

    if( m_bProcessing )
    {
        m_bProcessing = FALSE;

        WCHAR pwszProcText[] = L"Installing...";

        float XPos = SCREEN_X_CENTER - ( GetStringPixelWidth( pwszProcText ) / 2.0f );
        float YPos = SCREEN_Y_CENTER - ( m_uiFontHeightOfItem / 2.0f );;

		pTexture->DrawText( XPos, YPos, SCREEN_DEFAULT_FOREGROUND_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", pwszProcText );
        pTexture->Unlock();

        Render();

        switch( GetSelectedItem() )
        {
            case ROOTMENU_ITEM_INSTALL_MUCONFIG:
            {
                // Intall files to the hard disk
                InstallMUConfig();

                break;
            }
        }
        
        return;
    }

	// Draw our selector, if necessary
	if( GetNumItems() > 0 )
	{
        float X1Pos = MENUBOX_SELECTOR_X1;
        float Y1Pos = MENUBOX_MAINAREA_Y1 + MENUBOX_MAINAREA_BORDERWIDTH_TOP + ( ( MENUBOX_ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * GetSelectedItem() ) - MENUBOX_SELECTOR_BORDER_WIDTH;
        float X2Pos = MENUBOX_SELECTOR_X2;
        float Y2Pos = Y1Pos + m_uiFontHeightOfItem + ( 2 * MENUBOX_SELECTOR_BORDER_WIDTH );

        pTexture->DrawBox( X1Pos, Y1Pos, X2Pos, Y2Pos, SCREEN_DEFAULT_SELECTOR_COLOR );
	}

    // Draw our menu items
	for( unsigned int x = 0; x < ROOTMENU_ITEM_NUM_ITEMS; x++ )
	{
        float XPos = MENUBOX_MAINAREA_X1 + MENUBOX_MAINAREA_BORDERWIDTH_LEFT;
        float YPos = MENUBOX_MAINAREA_Y1 + MENUBOX_MAINAREA_BORDERWIDTH_TOP + ( ( MENUBOX_ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * x );

		pTexture->DrawText( XPos, YPos, SCREEN_DEFAULT_FOREGROUND_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", g_wpszRootMenuItems[x] );
	}

    pTexture->Unlock();
}

void CRootMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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
            case ROOTMENU_ITEM_MEMORY:
                {
                    Leave( &m_MenuMemory );

                    break;
                }
            case ROOTMENU_ITEM_SOUNDTRACKS:
                {
                    Leave( &m_MenuST );

                    break;
                }
            case ROOTMENU_ITEM_XDASH_ENTRY_POINTS:
                {
					Leave( &m_MenuEntry );

                    break;
                }
			case ROOTMENU_ITEM_XBOX_CONFIGURATION:
				{
					Leave( &m_MenuXBConfig );

					break;
				}
            case ROOTMENU_ITEM_INSTALL_MUCONFIG:
                {
                    m_bProcessing = TRUE;
                    m_bUpdateTexture = TRUE;

                    break;
                }
            }

            // If we are on a menu, let's navigate there
            if( GetParent() )
            {
                Leave( GetParent() );
            }
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

void CRootMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
                if( GetSelectedItem() > 0 )
                {
                    m_bUpdateTexture = TRUE;

                    SetSelectedItem( GetSelectedItem() - 1 );
                }
            }
            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
                if( GetSelectedItem() < ( ROOTMENU_ITEM_NUM_ITEMS - 1 ) )
                {
                    m_bUpdateTexture = TRUE;

                    SetSelectedItem( GetSelectedItem() + 1 );
                }
            }
            break;
        }
    }
}


void CRootMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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

// Initialize our font and settings
HRESULT CRootMenu::Init( IDirect3DDevice8* pD3DDevice, char* menuFileName )
{
    XDBGWRN( APP_TITLE_NAME_A, "CRootMenu::Init()" );

	// Call our base class method to handle the generic menu stuff
	CMenuScreen::Init( pD3DDevice, menuFileName );

	// Initialize our sub-menus
	m_MenuMemory.SetParent( this );
    m_MenuMemory.Init( pD3DDevice, MENU_MEMORY_MENU_FILENAME_A );

    m_MenuST.SetParent( this );
    m_MenuST.Init( pD3DDevice, MENU_ST_MENU_FILENAME_A );

	m_MenuEntry.SetParent( this );
	m_MenuEntry.Init( pD3DDevice, MENU_ENTRY_MENU_FILENAME_A );

	m_MenuXBConfig.SetParent( this );
	m_MenuXBConfig.Init( pD3DDevice, MENU_XBCONFIG_MENU_FILENAME_A );

    return S_OK;
}

// Install MU Config to the Hard Disk
void CRootMenu::InstallMUConfig()
{
    XDBGWRN( APP_TITLE_NAME_A, "CRootMenu::InstallMUConfig():Installing..." );

    // Create the MUConfig Directory on the E Drive
    g_HardDrive.UnMapDrive( 'E' );
    if( !NT_SUCCESS( g_HardDrive.MapDrive( 'E', "\\Device\\Harddisk0\\Partition1\\devkit" ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CRootMenu::InstallMUConfig():Failed to map the E Drive to Devkit!!" );

        return;
    }

    // Create the destination directory
    if( !CreateDirectory( "e:\\muconfig", NULL ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CRootMenu::InstallMUConfig():Failed to Create the MUConfig directory!!" );
        g_HardDrive.UnMapDrive( 'E' );
        
        return;
    }

    // Copy the contents of the CD/DVD to the Hard Disk
    g_HardDrive.CopyDirs( "d:", "e:\\muconfig" );

    // Rename the XBE to be more meaningful
    MoveFile( "e:\\muconfig\\default.xbe", "e:\\muconfig\\muconfig.xbe" );

    g_HardDrive.UnMapDrive( 'E' );
}

/*
// Called whenever this menu is entered / activated
void CRootMenu::Enter()
{
}

// This should be called whenever a user leaves this menu
void CRootMenu::Leave( CXItem* pItem )
{
}
*/