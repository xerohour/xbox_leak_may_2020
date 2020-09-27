/*****************************************************
*** menuscreen.cpp
***
*** CPP file for our XDash CFG Generic menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
*** 
*** by James N. Helm
*** November 2nd, 2000
*** 
*****************************************************/

#include "stdafx.h"
#include "menuscreen.h"

extern CXDCItem* g_pCurrentScreen;  // Pointer to the current menu object

// Constructor
CMenuScreen::CMenuScreen()
: CXDCItem(),
m_uicNumItems( 0 ),
m_uiSelectedItem( -1 )
{
}

// Constructor
CMenuScreen::CMenuScreen( CXDCItem* pParent, WCHAR* pwszTitle, WCHAR* pwszDescription )
: CXDCItem( pParent, pwszTitle, pwszDescription ),
m_uicNumItems( 0 ),
m_uiSelectedItem( -1 )
{
}

// Destructor
CMenuScreen::~CMenuScreen()
{
}

// Draws a menu on to the screen
void CMenuScreen::Action( CXBoxVideo* pScreen )
{
    unsigned int nStringWidth = 0;
    float fFontHeight = 0.0;

    pScreen->GetFontSize( 0, &fFontHeight, 0, 0 );

    // Display the Title
    pScreen->DrawText( (float)SCREEN_X_CENTER - ( pScreen->GetStringPixelWidth( GetTitle() ) / 2 ), MENU_TITLE_Y, MENU_TITLE_FG_COLOR, MENU_TITLE_BG_COLOR, GetTitle() );

    // If we have any items on the screen, let's draw them
    if( GetNumItems() > 0 )
    {
        // Draw a box behind the currently highlighted choice
        float fX1, fY1, fX2, fY2;
        fX1 = SCREEN_X_CENTER - ( pScreen->GetStringPixelWidth( GetItemTitle( GetSelectedItem() ) ) / 2.0f ) - MENU_HIGHLIGHT_BORDER;
        fY1 = MENU_OPTION1_Y + ( ( fFontHeight + MENU_VERT_SPACING ) * GetSelectedItem() ) - MENU_HIGHLIGHT_BORDER;
        fX2 = SCREEN_X_CENTER + ( pScreen->GetStringPixelWidth( GetItemTitle( GetSelectedItem() ) ) / 2.0f ) + MENU_HIGHLIGHT_BORDER;
        fY2 = MENU_OPTION1_Y + ( fFontHeight * ( GetSelectedItem() + 1 ) ) + ( MENU_VERT_SPACING * GetSelectedItem() ) + MENU_HIGHLIGHT_BORDER;

        pScreen->DrawOutline( fX1, fY1, fX2, fY2, MENU_HIGHLIGHT_BORDER_WIDTH, HIGHLIGHT_BOX_COLOR );

        // Draw the items on the menu
        for( unsigned int x = 0; x < GetNumItems(); ++x )
        {
            pScreen->DrawText( SCREEN_X_CENTER - ( pScreen->GetStringPixelWidth( GetItemTitle( x ) ) / 2.0f ), MENU_OPTION1_Y + ( ( fFontHeight + MENU_VERT_SPACING ) * x ), MENU_ITEM_FG_COLOR, MENU_ITEM_BG_COLOR, GetItemTitle( x ) );
        }
    }
}

// Add a pointer to an item that lives on this screen
void CMenuScreen::AddItem( CXDCItem* pScreen )
{
    // If we were not passed a valid pointer, notify the user and bail
    if( !pScreen )
    {
        DebugPrint( "CMenuScreen::AddItem(): pScreen was not a valid pointer!\n" );
        return;
    }

    // Add the pointer to our list
    m_Items.AddNode( pScreen );

    // Increment the number of items on our screen
    ++m_uicNumItems;

    // If this is our first item, let's set our selected item to it (item 0)
    if( 1 == m_uicNumItems )
    {
        SetSelectedItem( 0 );
    };
}

// Get the description of a sub-item on our screen
WCHAR* CMenuScreen::GetItemDescription( unsigned int itemIndex ) const
{
    if( ( itemIndex < 0 ) || ( itemIndex >= GetNumItems() ) )
    {
        DebugPrint( "CMenuScreen::GetItemDescription(): index out of range - '%d', MAX - '%d'\n", itemIndex, GetNumItems() - 1 );
        return NULL;
    }

    return m_Items[itemIndex]->GetDescription();
}

// Get the title of a sub-item on our screen
WCHAR* CMenuScreen::GetItemTitle( unsigned int itemIndex ) const
{
    if( ( itemIndex < 0 ) || ( itemIndex >= GetNumItems() ) )
    {
        DebugPrint( "CMenuScreen::GetItemTitle(): index out of range - '%d', MAX - '%d'\n", itemIndex, GetNumItems() - 1 );
        return NULL;
    }

    return m_Items[itemIndex]->GetTitle();
}

// Handles input for the current menu
void CMenuScreen::HandleInput( enum BUTTONS buttonPressed, bool bFirstPress )
{
    // DebugPrint( "CMenuScreen::HandleInput()\n" );

    // If it's a repeat button press, bail
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
                g_pCurrentScreen = m_Items[GetSelectedItem()];
            }
            break;
        }
    case BUTTON_B:
        {
            // Go Back one menu
            if( GetParent() )
            {
                SetSelectedItem( 0 );
                g_pCurrentScreen = GetParent();
            }
            break;
        }
    case BUTTON_C:
        {
            break;
        }
    case BUTTON_D:
        {
            break;
        }
    case BUTTON_E:
        {
            break;
        }
    case BUTTON_F:
        {
            break;
        }
    case BUTTON_7:
        {
            break;
        }
    case BUTTON_8:
        {
            break;
        }
    case BUTTON_9:
        {
            break;
        }
    case BUTTON_10:
        {
            break;
        }
    case DPAD_TOP:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
                if( GetSelectedItem() <= 0 )
                {
                    SetSelectedItem( GetNumItems() - 1 );
                }
                else
                {
                    SetSelectedItem( GetSelectedItem() - 1 );
                }
            }
            break;
        }
    case DPAD_BOTTOM:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
                if( GetSelectedItem() >= GetNumItems() - 1 )
                {
                    SetSelectedItem( 0 );
                }
                else
                {
                    SetSelectedItem( GetSelectedItem() + 1 );
                }
            }
            break;
        }
    case DPAD_LEFT:
        {
            break;
        }
    case DPAD_RIGHT:
        {
            break;
        }
    }
}

// Sets the currently selected item on our screen
HRESULT CMenuScreen::SetSelectedItem( unsigned int itemNumber )
{
    if( ( itemNumber < 0 ) || ( itemNumber >= GetNumItems() ) )
    {
        DebugPrint( "CMenuScreen::SetSelectedItem(): Tried to select item out of range! - '%d', MAX - '%d'\n", itemNumber, GetNumItems() - 1 );
        return E_INVALIDARG;
    }

    m_uiSelectedItem = itemNumber;
    SetFooterText( GetItemDescription( m_uiSelectedItem ) );

    return S_OK;
}
