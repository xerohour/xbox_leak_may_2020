/*****************************************************
*** dumpcachemenu.cpp
***
*** CPP file for our XShell Dump Cache partition
*** menu class.  This is a standard menu contains a
*** list of menu items to be selected.
***
*** by James N. Helm
*** May 30th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "dumpcachemenu.h"

extern CSoundCollection g_Sounds;               // Used to generate all sounds for the XShell
extern BOOL             g_bDisplayUpArrow;      // Used to determine if we should render the Up Arrow
extern BOOL             g_bDisplayDownArrow;    // Used to determine if we should render the Down Arrow
extern XFONT*           g_pFont;

// Constructors
CDumpCacheMenu::CDumpCacheMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_dwTitleID( 0l ),
m_uiTopItemIndex( 0 ),
m_pCacheEntries( NULL )
{
    XDBGTRC( APP_TITLE_NAME_A, "CDumpCacheMenu::CDumpCacheMenu(ex)" );

    m_uiNumItems = 0;
    SetSelectedItem( 0 );

    SetTitle( MENU_TITLE_DUMPCACHE );
}


// Destructor
CDumpCacheMenu::~CDumpCacheMenu()
{
    if( m_pCacheEntries )
    {
        delete[] m_pCacheEntries;
        m_pCacheEntries = NULL;
    }
}

// Draws a menu on to the screen
void CDumpCacheMenu::Action( CUDTexture* pTexture )
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
    XFONT_SetTextHeight( g_pFont, ITEM_DUMPCACHEMENU_FONT_HEIGHT );
    m_uiFontHeightOfItem = ITEM_DUMPCACHEMENU_FONT_HEIGHT;

    // Let our base menu class draw all the items pulled from the MNU file
    CMenuScreen::Action( pTexture );

    if( 0 == GetNumItems() )
    {
        float XPos = MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( MENU_DUMPCACHE_NOITEM_TEXT ) / 2.0f );
        float YPos = MENUBOX_TEXTAREA_Y1 + MENUBOX_TEXT_TOP_BORDER + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * 3 );

        pTexture->DrawText( XPos, YPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", MENU_DUMPCACHE_NOITEM_TEXT );
    }
    else
    {
        // Draw a box behind the currently highlighted choice
        float X1Pos = MENUBOX_SELECTBAR_X1;
        float Y1Pos = MENUBOX_TEXTAREA_Y1 + MENUBOX_TEXT_TOP_BORDER + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * ( GetSelectedItem() - m_uiTopItemIndex ) ) - ITEM_SELECTOR_BORDER_SPACE;
        float X2Pos = MENUBOX_SELECTBAR_X2;
        float Y2Pos = Y1Pos + m_uiFontHeightOfItem + ( 2 * ITEM_SELECTOR_BORDER_SPACE );
    
        pTexture->DrawBox( X1Pos, Y1Pos, X2Pos, Y2Pos, ITEM_SELECTOR_COLOR );

        //
        // Draw our Menu Items
        //

        for( unsigned int x = m_uiTopItemIndex; x < ( MENU_DUMPCACHE_NUM_ITEMS_TO_DISPLAY + m_uiTopItemIndex ); x++ )
        {
            // Make sure we don't try to draw items that don't exist
            if( x >= GetNumItems() )
            {
                break;
            }

            float XPos = MENUBOX_TEXTAREA_X1;
            float YPos = MENUBOX_TEXTAREA_Y1 + MENUBOX_TEXT_TOP_BORDER + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * ( x - m_uiTopItemIndex ) );

            pTexture->DrawText( XPos, YPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%0.8X", m_pCacheEntries[x].dwTitleId );
        }
    }

    // Correct our Font Height
    m_uiFontHeightOfItem = FONT_DEFAULT_HEIGHT;
    XFONT_SetTextHeight( g_pFont, FONT_DEFAULT_HEIGHT );

    // Unlock the texture
    pTexture->Unlock();
}


// Handle any requests for the joystick (thumb-pad)
void CDumpCacheMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
void CDumpCacheMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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

                    // Make sure we are displaying the correct items
                    if( GetSelectedItem() < (int)m_uiTopItemIndex )
                    {
                        m_uiTopItemIndex = GetSelectedItem();

                        AdjustDisplayArrows();
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
                    g_Sounds.PlayMenuItemSelectSound();

                    SetSelectedItem( GetSelectedItem() + 1 );

                    // Make sure we are displaying the correct items
                    if( GetSelectedItem() >= (int)( MENU_DUMPCACHE_NUM_ITEMS_TO_DISPLAY + m_uiTopItemIndex ) )
                    {
                        m_uiTopItemIndex += 1;

                        AdjustDisplayArrows();
                    }
                }
            }
            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CDumpCacheMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // If it's a repeat press, or input is disabled, let's bail
    if( !bFirstPress )
    {
        return;
    }

    // Handle Buttons being pressed
    switch( buttonPressed )
    {
    case BUTTON_A:
        {
            // Play the Select Sound
            g_Sounds.PlaySelectSound();

            if( GetNumItems() > 0 )
            {
                m_dwTitleID = m_pCacheEntries[GetSelectedItem()].dwTitleId;                
            }

            // Go Back one menu
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
                // Play the Back Sound
                g_Sounds.PlayBackSound();

                Leave( GetParent() );
            }

            break;
        }
    }
}


// Initialize the Menu
HRESULT CDumpCacheMenu::Init( char* menuFileName )
{
    XDBGTRC( APP_TITLE_NAME_A, "CDumpCacheMenu::Init()" );

    HRESULT hr = CMenuScreen::Init( menuFileName );

    return hr;
}


// Should be called whenever a user leaves the menu
void CDumpCacheMenu::Leave( CXItem* pItem )
{
    // Call our base class Leave
    CMenuScreen::Leave( pItem );

    g_bDisplayUpArrow = FALSE;
    g_bDisplayDownArrow = FALSE;
}

// This CDumpCacheMenu be called whenever the user enters this menu
void CDumpCacheMenu::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();

    // Set our selected item to the top of the menu
    SetSelectedItem( 0 );

    // Make sure the proper arrows are displayed
    AdjustDisplayArrows();

    // Populate our CACHE Entries
    m_dwTitleID = 0l;
    m_uiNumItems = 0;
    DWORD dwNumCacheEntries = 0l;

    // First, get the number of entries in the cache
    XapiGetCachePartitions( NULL, 0l, &dwNumCacheEntries );

    if( dwNumCacheEntries > 0l )
    {
        if( m_pCacheEntries )
        {
            delete[] m_pCacheEntries;
            m_pCacheEntries = NULL;
        }

        m_pCacheEntries = new X_CACHE_DB_ENTRY[dwNumCacheEntries];
        ZeroMemory( m_pCacheEntries, sizeof( X_CACHE_DB_ENTRY ) * dwNumCacheEntries );

        XapiGetCachePartitions( m_pCacheEntries, sizeof( X_CACHE_DB_ENTRY ) * dwNumCacheEntries, &dwNumCacheEntries );

        m_uiNumItems = dwNumCacheEntries;
    }
}


// Adjust the UP and DOWN arrows on the screen
void CDumpCacheMenu::AdjustDisplayArrows()
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
    if( ( GetNumItems() < MENU_DUMPCACHE_NUM_ITEMS_TO_DISPLAY ) ||
        ( ( m_uiTopItemIndex + MENU_DUMPCACHE_NUM_ITEMS_TO_DISPLAY ) == GetNumItems() ) )
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
