/*****************************************************
*** memareaselectmenu.cpp
***
*** CPP file for our Memory Area Select Menu class.
*** This menu will allow the user to select from one
*** of the 9 available Memory Areas
***
*** by James N. Helm
*** November 30th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "memareaselectmenu.h"

// Constructors and Destructor
CMemAreaSelectMenu::CMemAreaSelectMenu( void ) :
m_bCancelled( FALSE )
{
}

CMemAreaSelectMenu::~CMemAreaSelectMenu( void )
{
}

// Initialize the Menu
HRESULT CMemAreaSelectMenu::Init( XFONT* pMenuItemFont, XFONT* pMenuTitleFont )
{
    HRESULT hr = CMenuScreen::Init( pMenuItemFont, pMenuTitleFont );
    if( FAILED( hr ) )
    {
        DebugPrint( "CMemAreaSelectMenu::Init():Failed to initialize base class!!\n" );

        return hr;
    }

	// Set the menu title
	SetMenuTitle( pMenuTitleFont, L"Select Memory Area" );

    // Add screen items here
    AddBitmapItem( "d:\\media\\images\\background.bmp", 0, 0 );

    // Status Bar Column 1
    AddTextItem( GetButtonFont(), L"A",              gc_fSTATUS_TEXT_COL1_XPOS,                           gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwCOLOR_A_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Select",        gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( GetButtonFont(), L"B",              gc_fSTATUS_TEXT_COL1_XPOS,                           gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwCOLOR_B_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Back / Cancel", gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );
	
    return hr;
}

// Draws a menu on to the screen
void CMemAreaSelectMenu::Action( CUDTexture* pTexture )
{
    if( GetMUsChanged() )
        UpdateMenuItems();

    // Let our base menu draw the items from the data file
    CMenuScreen::Action( pTexture );
}

// Handles input (of the BUTTONS) for the current menu
void CMemAreaSelectMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // If it's a repeat press, let's bail
    if( !bFirstPress )
        return;

    // Handle Buttons being pressed
    switch( buttonPressed )
    {
    case BUTTON_A:
        {
            GoBack();

            break;
        }
    case BUTTON_B:
        {
            m_bCancelled = TRUE;

            GoBack();

            break;
        }
    }
}


// This should be called whenever the menu is entered or activated
void CMemAreaSelectMenu::Enter( void )
{
    // Base class Enter
    CMenuScreen::Enter();

    // Make sure our cancelled state is set correctly
    m_bCancelled = FALSE;

    // Update our menu items
    UpdateMenuItems();
}


// Update the menu items on our screen
void CMemAreaSelectMenu::UpdateMenuItems( void )
{
    ClearMenuItems();

    // Add the menu items (Memory Areas) to our Menu
    AddMenuItem( GetBodyFont(), L"Xbox Hard Disk", FALSE, TRUE, 8 );

    for( unsigned int x = 0; x < 8; x++ )
    {
        if( GetMUPtr( x )->IsMounted() )
            AddMenuItem( GetBodyFont(), GetMUPtr( x )->GetDefaultName(), FALSE, TRUE, x );
    }

    UpdateTexture();
}