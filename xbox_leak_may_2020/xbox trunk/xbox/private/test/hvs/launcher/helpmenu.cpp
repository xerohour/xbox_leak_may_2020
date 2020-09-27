/*****************************************************
*** helpmenu.cpp
***
*** Header file for our Help Menu class.
*** This menu will display help for whatever menu
*** specifies it
***
*** by James N. Helm
*** December 18th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "helpmenu.h"

// Constructors and Destructor
CHelpMenu::CHelpMenu( void )
{
}

CHelpMenu::~CHelpMenu( void )
{
}


// Initialize the Menu
HRESULT CHelpMenu::Init( XFONT* pMenuItemFont, XFONT* pMenuTitleFont )
{
    HRESULT hr = CMenuScreen::Init( pMenuItemFont, pMenuTitleFont );
    if( FAILED( hr ) )
    {
        DebugPrint( "CHelpMenu::Init():Failed to initialize base class!!\n" );

        return hr;
    }

	// Set the menu title
	SetMenuTitle( pMenuTitleFont, L"Help Menu" );

    // Add screen items here
    AddBitmapItem( "d:\\media\\images\\background.bmp", 0, 0 );

    // Status Bar Column 1
    AddTextItem( GetButtonFont(), L"B",     gc_fSTATUS_TEXT_COL1_XPOS,                           gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwCOLOR_B_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Back", gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );

    return hr;
}


// Render the menu items properly on our menu
void CHelpMenu::RenderMenuItems( CUDTexture* pTexture )
{
    // Don't render anything if we don't have any menu items
    if( 0 == GetNumMenuItems() )
        return;

    m_MenuItems.MoveTo( GetTopItemIndex() );    // Start at menu item that needs to be displayed
    for( unsigned int x = GetTopItemIndex(); x <= GetBottomItemIndex(); x++ )
    {
        // Determine where on the screen the menu item should be
        float fXPos = m_fLeftMenuAreaBoundary + m_fLeftMenuAreaBorder;
        float fYPos = m_fTopMenuAreaBoundary + m_fTopMenuAreaBorder + ( ( m_fMenuItemVertSpace + m_uiMenuItemFontHeight ) * ( x - GetTopItemIndex() ) );

        // Distinguish between enabled and disabled items
        DWORD dwFGColor, dwBGColor;
        if( m_MenuItems.GetCurrentNode()->m_bEnabled )
        {
            // Check to see if it's highlighted
            if( m_MenuItems.GetCurrentNode()->m_bHighlighted )
            {
                dwFGColor = m_dwMenuItemHighlightedFGColor;
                dwBGColor = m_dwMenuItemHighlightedBGColor;
            }
            else
            {
                dwFGColor = m_dwMenuItemEnabledFGColor;
                dwBGColor = m_dwMenuItemEnabledBGColor;
            }
        }
        else
        {
            dwFGColor = m_dwMenuItemDisabledFGColor;
            dwBGColor = m_dwMenuItemDisabledBGColor;
        }

        // Check to see if the value is a main value, or an indented info item
        if( 0 != wcsncmp( m_MenuItems.GetCurrentNode()->m_pwszItemName, L"*-*", 3 ) )
        {
            // Draw the item text to the screen (left justified)
            pTexture->DrawText( m_MenuItems.GetCurrentNode()->m_pFont, fXPos, fYPos, dwFGColor, dwBGColor, L"%ls", m_MenuItems.GetCurrentNode()->m_pwszItemName );
        }
        else
        {
            pTexture->DrawText( m_MenuItems.GetCurrentNode()->m_pFont, fXPos, fYPos, gc_dwCOLOR_TEXT_YELLOW, dwBGColor, L"     %ls", m_MenuItems.GetCurrentNode()->m_pwszItemName + 3 );
        }

        m_MenuItems.MoveNext();
    }
}


// Handle any requests for the joystick (thumb-pad)
void CHelpMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
{
    switch( joystick )
    {
    // This is the LEFT Thumb Joystick on the controller
    case JOYSTICK_RIGHT:
        {
            // Y
            if( nThumbY < 0 )  // Move the right joystick down
            {
                // Process the default button delay
                if( !HandleInputDelay( bFirstYPress ) )
                    return;

                // Only act if we have any items on our screen
                if( GetNumMenuItems() > 0 )
                {
                    UpdateTexture();

                    // Check to see if the menu wouldn't fill the screen
                    int iNewTopIndex = GetTopItemIndex() + GetNumMenuItemsToDisplay();

                    if( ( iNewTopIndex + GetNumMenuItemsToDisplay() ) > GetNumMenuItems() )
                    {
                        iNewTopIndex = GetNumMenuItems() - GetNumMenuItemsToDisplay();

                        if( iNewTopIndex < 0 )
                            iNewTopIndex = 0;

                        SetTopItemIndex( iNewTopIndex );
                    }
                    else
                    {
                        SetTopItemIndex( iNewTopIndex );
                    }
                }
                break;
            }
            else if( nThumbY > 0 ) // Move right joystick up
            {
                // Process the default button delay
                if( !HandleInputDelay( bFirstYPress ) )
                    return;

                // Only act if we have any items on our screen
                if( GetNumMenuItems() > 0 )
                {
                    if( GetTopItemIndex() > 0 )
                    {
                        UpdateTexture();

                        int iNewTopIndex = GetTopItemIndex() - GetNumMenuItemsToDisplay();
                        if( iNewTopIndex < 0 )
                            iNewTopIndex = 0;

                        SetTopItemIndex( iNewTopIndex );
                    }
                }
                break;
            }

            // X
            if( nThumbX < 0 )       // Move the right joystick left
            {
                CMenuScreen::HandleInput( joystick, nThumbY, nThumbX, bFirstYPress, bFirstXPress );
            }
            else if( nThumbX > 0 )  // Move the right joystick right
            {
                CMenuScreen::HandleInput( joystick, nThumbY, nThumbX, bFirstYPress, bFirstXPress );
            }
            break;
        }
    default:
        {
            CMenuScreen::HandleInput( joystick, nThumbY, nThumbX, bFirstYPress, bFirstXPress );
            break;
        }
    }
}


// Handles input (of the BUTTONS) for the current menu
void CHelpMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // If it's a repeat press, let's bail
    if( !bFirstPress )
        return;

    // Handle Buttons being pressed
    switch( buttonPressed )
    {
    case BUTTON_B:
        {
            GoBack();

            break;
        }
    }
}


// Handle control input for a single list menu
void CHelpMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
    // Process the default button delay
    if( !HandleInputDelay( bFirstPress ) )
        return;

    // Handle Controls being pressed
    switch( controlPressed )
    {
    case CONTROL_DPAD_UP:
        {
            // Only act if we have any items on our screen
            if( GetNumMenuItems() > 0 )
            {
                if( GetTopItemIndex() > 0 )
                {
                    UpdateTexture();

                    SetTopItemIndex( GetTopItemIndex() - 1 );
                }
            }
            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            // Only act if we have any items on our screen
            if( GetNumMenuItems() > 0 )
            {
                UpdateTexture();

                // Check to see if the menu wouldn't fill the screen
                int iNewTopIndex = GetTopItemIndex() + 1;

                if( ( iNewTopIndex + GetNumMenuItemsToDisplay() ) > GetNumMenuItems() )
                {
                    iNewTopIndex = GetNumMenuItems() - GetNumMenuItemsToDisplay();

                    if( iNewTopIndex < 0 )
                        iNewTopIndex = 0;

                    SetTopItemIndex( iNewTopIndex );
                }
                else
                {
                    SetTopItemIndex( iNewTopIndex );
                }
            }
            break;
        }
	}
}



// Add a help item to our menu class
void CHelpMenu::AddHelpItem( XFONT* pItemFont, WCHAR* pwszHelpItem, XFONT* pValueFont, WCHAR* pwszHelpValue )
{
    if( NULL == pItemFont || NULL == pwszHelpItem || NULL == pValueFont || NULL == pwszHelpValue )
    {
        DebugPrint( "CHelpMenu::AddHelpItem():Invalid argument(s) passed in!!\n" );

        return;
    }

    // DebugPrint( "Main Item - %ls\n", pwszHelpItem );

    // Add the item to our menu
    AddMenuItem( pItemFont, pwszHelpItem, FALSE, TRUE, 0 );

    // Parse the Help Value and determine if there should be wrapping, etc
    WCHAR pwszTempBuffer[1024];
    pwszTempBuffer[1023] = L'\0';

    // Don't add a 'value' if the user didn't pass one in
    if( 0 == wcslen( pwszHelpValue ) )
        return;

    WCHAR* pwszParser2 = wcsstr( pwszHelpValue, L"\\n" );
    if( NULL == pwszParser2 )
    {
        // Copy the indent character in to our temp variable
        wcscpy( pwszTempBuffer, L"*-*" );
        wcsncat( pwszTempBuffer, pwszHelpValue, 1021 );
        // DebugPrint( "Value (no parse) - %ls\n", pwszTempBuffer );
        AddMenuItem( pValueFont, pwszTempBuffer, FALSE, TRUE, 0 );
    }
    else
    {
        WCHAR* pwszParser = pwszHelpValue;
        do
        {
            // Copy the indent character in to our temp variable
            wcscpy( pwszTempBuffer, L"*-*" );

            // Append the first string to our temp variable
            unsigned int uiCatAmount = pwszParser2 - pwszParser;
            if( uiCatAmount > 1021 )
                uiCatAmount = 1021;
            wcsncat( pwszTempBuffer, pwszParser, uiCatAmount );

            // DebugPrint( "Value1 (parse) - %ls\n", pwszTempBuffer );

            // Add the menu item to our List
            AddMenuItem( pValueFont, pwszTempBuffer, FALSE, TRUE, 0 );

            // Update our string pointer to point at the beginning of our new string
            pwszParser = pwszParser2 + 2;

            // Find the next \n (if any) in our string
            pwszParser2 = wcsstr( pwszParser, L"\\n" );
        } while( NULL != pwszParser2 );

        // Check to see if there is a string left to add
        if( L'\0' != pwszParser )
        {
            // Copy the indent character in to our temp variable
            wcscpy( pwszTempBuffer, L"*-*" );

            // Append the first string to our temp variable
            wcsncat( pwszTempBuffer, pwszParser, 1021 );

            // DebugPrint( "Value2 (parse) - %ls\n", pwszTempBuffer );

            // Add the menu item to our List
            AddMenuItem( pValueFont, pwszTempBuffer, FALSE, TRUE, 0 );
        }
    }
}


// Clear all the help items from our screen
void CHelpMenu::ClearHelpItems( void )
{
    ClearMenuItems();
}


// This should be called whenever the menu is entered or activated
void CHelpMenu::Enter( void )
{
    CMenuScreen::Enter();

    SetTopItemIndex( 0 );
}