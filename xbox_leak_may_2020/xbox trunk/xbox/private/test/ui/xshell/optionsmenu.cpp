/*****************************************************
*** optionsmenu.cpp
***
*** CPP file for our XShell Options menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** May 19th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "optionsmenu.h"

extern CSoundCollection g_Sounds;           // Used to generate all sounds for the XShell
extern XFONT*           g_pFont;

// Constructors
COptionsMenu::COptionsMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_GameRegionMenu( NULL ),
m_AVRegionMenu( NULL ),
m_VideoResMenu( NULL ),
m_MemoryMenu( NULL ),
m_NetworkMenu( NULL ),
m_CertMenu( NULL )
{
    XDBGTRC( APP_TITLE_NAME_A, "COptionsMenu::COptionsMenu(ex)" );

    m_uiNumItems = ENUM_OPTIONS_MAX_MENUITEMS;
    SetSelectedItem( 0 );

    SetTitle( MENU_TITLE_OPTIONS );
}


// Destructor
COptionsMenu::~COptionsMenu()
{
}

// Draws a menu on to the screen
void COptionsMenu::Action( CUDTexture* pTexture )
{
    // Only update the texture if we need to, otherwise return
    if( !m_bUpdateTexture )
    {
        return;
    }
    else
    {
        m_bUpdateTexture = FALSE;
    }
    
    // Lock our texture and set it as the render target
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    // Let our base menu class draw all the items pulled from the MNU file
    CMenuScreen::Action( pTexture );

    // Update our Font Height
    XFONT_SetTextHeight( g_pFont, ITEM_OPTIONMENU_FONT_HEIGHT );
    m_uiFontHeightOfItem = ITEM_OPTIONMENU_FONT_HEIGHT;

    // Draw a box behind the currently highlighted choice
    float X1Pos = MENUBOX_SELECTBAR_X1;
    float Y1Pos = MENUBOX_TEXTAREA_Y1 + MENUBOX_TEXT_TOP_BORDER + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * GetSelectedItem() ) - ITEM_SELECTOR_BORDER_SPACE;
    float X2Pos = MENUBOX_SELECTBAR_X2;
    float Y2Pos = Y1Pos + m_uiFontHeightOfItem + ( 2 * ITEM_SELECTOR_BORDER_SPACE );
    
    pTexture->DrawBox( X1Pos, Y1Pos, X2Pos, Y2Pos, ITEM_SELECTOR_COLOR );

    //
    // Draw our Menu Items
    //

    for( unsigned int x = 0; x < ENUM_OPTIONS_MAX_MENUITEMS; x++ )
    {
        float XPos = MENUBOX_TEXTAREA_X1;
        float YPos = MENUBOX_TEXTAREA_Y1 + MENUBOX_TEXT_TOP_BORDER + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * x );

        pTexture->DrawText( XPos, YPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", g_pwszOptionsMenuItems[x] );
    }

    // Correct our Font Height
    m_uiFontHeightOfItem = FONT_DEFAULT_HEIGHT;
    XFONT_SetTextHeight( g_pFont, FONT_DEFAULT_HEIGHT );

    // Unlock the texture
    pTexture->Unlock();
}


// Handle any requests for the joystick (thumb-pad)
void COptionsMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
void COptionsMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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
                if( GetSelectedItem() > 0 )
                {
                    m_bUpdateTexture = TRUE;

                    // Play the Menu Item Select Sound
                    g_Sounds.PlayMenuItemSelectSound();

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
                if( ( (unsigned int)GetSelectedItem() + 1 ) < GetNumItems() )
                {
                    m_bUpdateTexture = TRUE;

                    // Play the Menu Item Select Sound
                    g_Sounds.PlayMenuItemSelectSound();

                    SetSelectedItem( GetSelectedItem() + 1 );
                }
            }

            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void COptionsMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                switch( GetSelectedItem() )
                {
                case ENUM_OPTIONS_SET_GAMEREGION:
                    {
                        Leave( &m_GameRegionMenu );

                        break;
                    }
                case ENUM_OPTIONS_SET_AVREGION:
                    {
                        Leave( &m_AVRegionMenu );

                        break;
                    }
                case ENUM_OPTIONS_SET_VIDEORES:
                    {
                        Leave( &m_VideoResMenu );

                        break;
                    }
                case ENUM_OPTIONS_MEMORY:
                    {
                        Leave( &m_MemoryMenu );

                        break;
                    }
                case ENUM_OPTIONS_NETWORK:
                    {
                        Leave( &m_NetworkMenu );

                        break;
                    }
                case ENUM_OPTIONS_CERT_TOOLS:
                    {
                        Leave( &m_CertMenu );

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
                // Play the Back Sound
                g_Sounds.PlayBackSound();

                Leave( GetParent() );
            }

            break;
        }
    }
}


// Initialize the Menu
HRESULT COptionsMenu::Init( char* menuFileName )
{
    XDBGTRC( APP_TITLE_NAME_A, "COptionsMenu::Init()" );

    HRESULT hr = CMenuScreen::Init( menuFileName );

    m_GameRegionMenu.Init( MENU_GAMEREGION_MENU_FILENAME_A );
    m_GameRegionMenu.SetParent( this );

    m_AVRegionMenu.Init( MENU_AVREGION_MENU_FILENAME_A );
    m_AVRegionMenu.SetParent( this );

    m_VideoResMenu.Init( VIDEORESMENU_FILENAME_A );
    m_VideoResMenu.SetParent( this );

    m_MemoryMenu.Init( FILE_DATA_MENU_DIRECTORY_A "\\memory.mnu" );
    m_MemoryMenu.SetParent( this );

    m_NetworkMenu.Init( NETWORKMENU_FILENAME_A );
    m_NetworkMenu.SetParent( this );

    m_CertMenu.Init( MENU_CERT_MENU_FILENAME_A );
    m_CertMenu.SetParent( this );

    return hr;
}


// Get a pointer to a sub-menu
CXItem* COptionsMenu::GetItemPtr( enum XShellMenuIds menuId )
{
    CXItem* pReturn = NULL;

    if( ENUM_SHELLMENUID_OPTIONS_NETWORK == menuId )
    {
        pReturn = &m_NetworkMenu;
    }
    else if( ENUM_SHELLMENUID_OPTIONS_GAMEREGION == menuId )
    {
        pReturn = &m_GameRegionMenu;
    }
    else if( ENUM_SHELLMENUID_OPTIONS_AVREGION == menuId )
    {
        pReturn = &m_AVRegionMenu;
    }
    else if( ENUM_SHELLMENUID_OPTIONS_MEMORY == menuId )
    {
        pReturn = &m_MemoryMenu;
    }
    else if( ENUM_SHELLMENUID_OPTIONS_CERT == menuId )
    {
        pReturn = &m_CertMenu;
    }

    return pReturn;
}


// This will be called whenever the user enters this menu
void COptionsMenu::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();
}
