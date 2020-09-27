/*****************************************************
*** savegamemenu.cpp
***
*** CPP file for our Game Title menu class.  This
*** menu will list all of the Saved Games for a
*** specified Game Title, and allow the user to
*** select one
***
*** by James N. Helm
*** June 17th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "savegamemenu.h"

// extern BOOL                 g_bDisplayUpArrow;      // Used to determine if we should render the Up Arrow
// extern BOOL                 g_bDisplayDownArrow;    // Used to determine if we should render the Down Arrow
extern XFONT*                g_pFont;

// Constructors
CSaveGameMenu::CSaveGameMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_uiTopItemIndex( 0 ),
m_pGameTitle( NULL )
{
    XDBGTRC( APP_TITLE_NAME_A, "CSaveGameMenu::CSaveGameMenu(ex)" );

    m_uiNumItems = 0;
    SetSelectedItem( 0 );
}


// Destructor
CSaveGameMenu::~CSaveGameMenu()
{
}

// Draws a menu on to the screen
void CSaveGameMenu::Action( CUDTexture* pTexture )
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

    // Update our Font Height
    unsigned int uiDefaultFontHeight = m_uiFontHeightOfItem;
    XFONT_SetTextHeight( g_pFont, SAVE_GAME_MENU_ITEM_FONT_HEIGHT );
    m_uiFontHeightOfItem = SAVE_GAME_MENU_ITEM_FONT_HEIGHT;

    // Let our base menu class draw all the items pulled from the MNU file
    CMenuScreen::Action( pTexture );

    if( GetNumItems() != 0 )
    {
        // Draw a box behind the currently highlighted choice
        float X1Pos = MENUBOX_SELECTOR_X1;
        float Y1Pos = MENUBOX_MAINAREA_Y1 + MENUBOX_MAINAREA_BORDERWIDTH_TOP + ( ( MENUBOX_ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * ( GetSelectedItem() - m_uiTopItemIndex ) ) - MENUBOX_SELECTOR_BORDER_WIDTH;
        float X2Pos = MENUBOX_SELECTOR_X2;
        float Y2Pos = Y1Pos + m_uiFontHeightOfItem + ( 2 * MENUBOX_SELECTOR_BORDER_WIDTH );
    
        pTexture->DrawBox( X1Pos, Y1Pos, X2Pos, Y2Pos, SCREEN_DEFAULT_SELECTOR_COLOR );

        //
        // Draw our Menu Items
        //

        for( unsigned int x = m_uiTopItemIndex; x < ( SAVE_GAME_MENU_NUM_ITEMS_TO_DISPLAY + m_uiTopItemIndex ); x++ )
        {
            // Make sure we don't try to access out of range items
            if( x == GetNumItems() )
            {
                break;
            }

            float XPos = MENUBOX_MAINAREA_X1 + MENUBOX_MAINAREA_BORDERWIDTH_LEFT;
            float YPos = MENUBOX_MAINAREA_Y1 + MENUBOX_MAINAREA_BORDERWIDTH_TOP + ( ( MENUBOX_ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * ( x - m_uiTopItemIndex ) );

            pTexture->DrawText( XPos, YPos, SCREEN_DEFAULT_FOREGROUND_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%hs (%ls)", m_pGameTitle->GetSavedGameID( x ), m_pGameTitle->GetSavedGameName( x ) );
        }
    }

    // Correct our Font Height
    m_uiFontHeightOfItem = uiDefaultFontHeight;
    XFONT_SetTextHeight( g_pFont, uiDefaultFontHeight );

    // Unlock the texture
    pTexture->Unlock();
}


// Handle any requests for the joystick (thumb-pad)
void CSaveGameMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
void CSaveGameMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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
                    // g_Sounds.PlayMenuItemSelectSound();

                    SetSelectedItem( GetSelectedItem() - 1 );

                    // Make sure we are displaying the correct items
                    if( GetSelectedItem() < (int)m_uiTopItemIndex )
                    {
                        m_uiTopItemIndex = GetSelectedItem();

                        // AdjustDisplayArrows();
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
                if( ( (unsigned int)GetSelectedItem() + 1 ) < GetNumItems() )
                {
                    m_bUpdateTexture = TRUE;

                    // Play the Menu Item Select Sound
                    // g_Sounds.PlayMenuItemSelectSound();

                    SetSelectedItem( GetSelectedItem() + 1 );

                    // Make sure we are displaying the correct items
                    if( GetSelectedItem() >= (int)( SAVE_GAME_MENU_NUM_ITEMS_TO_DISPLAY + m_uiTopItemIndex ) )
                    {
                        m_uiTopItemIndex += 1;

                        // AdjustDisplayArrows();
                    }
                }
            }
            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CSaveGameMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // If it's a repeat press let's bail
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
                // g_Sounds.PlaySelectSound();

                m_bUpdateTexture = TRUE;
            }

            break;
        }
    case BUTTON_B:
        {
            // Go Back one menu
            if( GetParent() )
            {
                // Play the Back Sound
                // g_Sounds.PlayBackSound();

                Leave( GetParent() );
            }

            break;
        }
    case BUTTON_Y:
        {
            break;
        }
    }
}


// Initialize the Menu
HRESULT CSaveGameMenu::Init( IDirect3DDevice8* pD3DDevice, char* menuFileName )
{
    XDBGTRC( APP_TITLE_NAME_A, "CSaveGameMenu::Init()" );

    HRESULT hr = CMenuScreen::Init( pD3DDevice, menuFileName );

    return hr;
}


// Sets the Xbox Game object the menu should use
void CSaveGameMenu::SetInfo( CGameTitle* pGameTitle )
{
    if( NULL == pGameTitle )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CSaveGameMenu::SetInfo():Invalid parameter passed in!!" );

        return;
    }

    m_pGameTitle = pGameTitle;
    m_uiNumItems = m_pGameTitle->GetNumSavedGames();

    if( GetNumItems() > 0 )
    {
        SetSelectedItem( 0 );
    }
}


// Should be called whenever a user leaves the menu
void CSaveGameMenu::Leave( CXItem* pItem )
{
    // Call our base class Leave
    CMenuScreen::Leave( pItem );

    // g_bDisplayUpArrow = FALSE;
    // g_bDisplayDownArrow = FALSE;

    // Make sure we set that we have no items
    m_pGameTitle = NULL;
    m_uiNumItems = 0;
    m_uiTopItemIndex = 0;
}

// This will be called whenever the user enters this menu
void CSaveGameMenu::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();

    // Set our selected item to the top of the menu
    if( GetNumItems() > 0 )
    {
        SetSelectedItem( 0 );
        m_uiTopItemIndex = 0;
    }

    // Make sure the proper arrows are displayed
    // AdjustDisplayArrows();
}


/*
// TODO: Implement the arrows
// Adjust the UP and DOWN arrows on the screen
void CSaveGameMenu::AdjustDisplayArrows()
{
    // Decide if we need to turn on the Up Arrow
    if( 0 == m_uiTopItemIndex )
    //
    // We can turn off the Up Arrow, because there is nothing above us
    //
    {
        g_bDisplayUpArrow = FALSE;
    }
    else
    //
    // We have to turn on the Up Arrow, because a menu choice is above us
    //
    {
        g_bDisplayUpArrow = TRUE;
    }

    // Decide if we need to turn on the Down Arrow
    if( ( m_uiTopItemIndex + SAVE_GAME_MENU_NUM_ITEMS_TO_DISPLAY ) == GetNumItems() )
    //
    // We can turn off the Down arrow, because nothing is below us
    //
    {
        g_bDisplayDownArrow = FALSE;
    }
    else
    //
    // We have to turn on the Down Arrow, because there are menu choices below us
    //
    {
        g_bDisplayDownArrow = TRUE;
    }
}
*/
