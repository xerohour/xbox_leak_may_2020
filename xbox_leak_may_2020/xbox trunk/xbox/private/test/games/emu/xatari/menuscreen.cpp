/*****************************************************
*** menuscreen.cpp
***
*** CPP file for our Generic menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** November 17th, 2000
***
*****************************************************/

#include "stdafx.h"
#include "menuscreen.h"

extern WCHAR* g_pszCurrentItem;     // Pointer to the current menu item
extern bool g_bBreakLoop;           // Used to determine if an item has been selected from the main loop

// Constructor
CMenuScreen::CMenuScreen()
: CXDCItem(),
m_uicNumItems( 0 ),
m_uiSelectedItem( -1 ),
m_uiPage( 0 )
{
}

// Constructor
CMenuScreen::CMenuScreen( CXDCItem* pParent, WCHAR* pwszTitle, WCHAR* pwszDescription )
: CXDCItem( pParent, pwszTitle, pwszDescription ),
m_uicNumItems( 0 ),
m_uiSelectedItem( -1 ),
m_uiPage( 0 )
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
	unsigned int colNumber = 0;
	unsigned int rowNumber = 0;
	float fFontHeight = 0.0;
    float clockDelay = 300;

    pScreen->GetFontSize( 0, &fFontHeight, 0, 0 );

    // Display the Title
    // pScreen->DrawText( (float)SCREEN_X_CENTER - ( pScreen->GetStringPixelWidth( GetTitle() ) / 2 ), MENU_TITLE_Y, MENU_TITLE_FG_COLOR, MENU_TITLE_BG_COLOR, GetTitle() );

    // If we have any items on the screen, let's draw them
    if( GetNumItems() > 0 )
    {
        // Draw a box behind the currently highlighted choice
        float fX1, fY1, fX2, fY2;
		fX1 = MENU_HORIZ_SPACING + ( GetColNumber( GetSelectedItem() ) * pScreen->GetStringPixelWidth( L"WWWWWWWW" ) ) - MENU_HIGHLIGHT_BORDER_SPACE;
        fY1 = MENU_OPTION1_Y + ( ( fFontHeight + MENU_VERT_SPACING ) * GetRowNumber( GetSelectedItem() ) ) - MENU_HIGHLIGHT_BORDER_SPACE;

		fX2 = fX1 + pScreen->GetStringPixelWidth( GetItemName( GetSelectedItem() ) ) + ( 2 * MENU_HIGHLIGHT_BORDER_SPACE );
		fY2 = fY1 + fFontHeight + ( 2 * MENU_HIGHLIGHT_BORDER_SPACE );

        pScreen->DrawOutline( fX1, fY1, fX2, fY2, MENU_HIGHLIGHT_BORDER_WIDTH, HIGHLIGHT_BOX_COLOR );

        // Draw the items on the menu
		float xValue = 0.0f;
		float yValue = 0.0f;

        for( unsigned int x = GetPageStart(); x <= GetPageEnd(); ++x )
        {
			xValue = MENU_HORIZ_SPACING + ( GetColNumber( x ) * pScreen->GetStringPixelWidth( L"WWWWWWWW" ) );
			yValue = MENU_OPTION1_Y + ( ( fFontHeight + MENU_VERT_SPACING ) * GetRowNumber( x ) );

            pScreen->DrawText( xValue, yValue, MENU_ITEM_FG_COLOR, MENU_ITEM_BG_COLOR, GetItemName( x ) );
        }
    }
}

// Add a pointer to an item that lives on this screen
void CMenuScreen::AddItem( WCHAR* pItem )
{
    // If we were not passed a valid pointer, notify the user and bail
    if( !pItem )
    {
        DebugPrint( "CMenuScreen::AddItem(): pItem was not a valid pointer!\n" );
        return;
    }

    // Add the pointer to our list
    m_Items.AddNode( pItem );

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
	//TODO -- Get meaningful item descriptions for the games in the menu
    if( ( itemIndex < 0 ) || ( itemIndex >= GetNumItems() ) )
    {
        DebugPrint( "CMenuScreen::GetItemDescription(): index out of range - '%d', MAX - '%d'\n", itemIndex, GetNumItems() - 1 );
        return NULL;
    }

	return m_Items[itemIndex];
    // return m_Items[itemIndex]->GetDescription();
}

// Get the name of the item based on the passed in index
WCHAR* CMenuScreen::GetItemName( unsigned int itemIndex ) const
{
    if( ( itemIndex < 0 ) || ( itemIndex >= GetNumItems() ) )
    {
        DebugPrint( "CMenuScreen::GetItemName(): index out of range - '%d', MAX - '%d'\n", itemIndex, GetNumItems() - 1 );
        return NULL;
    }

	return m_Items[itemIndex];
}


// The ending index of the item on the current page
unsigned int CMenuScreen::GetColNumber( unsigned int index ) const
{
	unsigned int colNumber = ( index - GetPageStart() ) / MENU_NUM_ROW;

	return colNumber;
}


// The ending index of the item on the current page
unsigned int CMenuScreen::GetRowNumber( unsigned int index ) const
{
	unsigned int rowNumber = ( index - GetPageStart() ) - ( GetColNumber( index ) * MENU_NUM_ROW );

	return rowNumber;
}


// The ending index of the item on the current page
unsigned int CMenuScreen::GetPageEnd() const
{
	unsigned int pageEnd = GetPageStart() + ( MENU_NUM_COL * MENU_NUM_ROW ) - 1; // Minus 1 so that the pageStart and pageEnd are 0 based indexes
	if( pageEnd >= GetNumItems() )
	{
		pageEnd = GetNumItems() - 1;
	}

	return pageEnd;
}


// The starting index of the item on the current page
unsigned int CMenuScreen::GetPageStart() const
{
	unsigned int pageStart = m_uiPage * MENU_NUM_COL * MENU_NUM_ROW;

	return pageStart;
}


void CMenuScreen::HandleInput( enum CONTROLS controlPressed, bool bFirstPress )
{
    // DebugPrint( "CMenuScreen::HandleInput()\n" );

    // If it's a repeat button press, bail
    if( !bFirstPress )
    {
        return;
    }

    // Handle Buttons being pressed
    DebugPrint( "Control - '%d'\n", controlPressed );
    switch( controlPressed )
    {
	case CONTROL_START:
		{
			DebugPrint( "Start Button\n" );
			break;
		}
	case CONTROL_SELECT:
		{
			DebugPrint( "Select Button\n" );
			break;
		}
	case CONTROL_TRIGGER_LEFT:
		{
			DebugPrint( "Left Trigger\n" );
			break;
		}
	case CONTROL_TRIGGER_RIGHT:
		{
			DebugPrint( "Right Trigger\n" );
			break;
		}
    case CONTROL_DPAD_TOP:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
                if( GetSelectedItem() > GetPageStart() )
                {
                    SetSelectedItem( GetSelectedItem() - 1 );
                }
            }
            break;
        }
    case CONTROL_DPAD_BOTTOM:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
				// Make sure we are not at the end of our page
                if( GetSelectedItem() < GetPageEnd() )
                {
					SetSelectedItem( GetSelectedItem() + 1 );
                }
            }
            break;
        }
    case CONTROL_DPAD_LEFT:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
				// Make sure there is an item to the left
				if( GetColNumber( GetSelectedItem() ) > 0 )
				{
					SetSelectedItem( GetSelectedItem() - MENU_NUM_ROW );
				}
            }
            break;
        }
    case CONTROL_DPAD_RIGHT:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
				// Make sure there is an item to the right
				if( ( ( GetSelectedItem() + MENU_NUM_ROW ) < GetNumItems() ) && ( ( GetSelectedItem() + MENU_NUM_ROW ) <= GetPageEnd() ) )
				{
					SetSelectedItem( GetSelectedItem() + MENU_NUM_ROW );
				}
            }
            break;
        }
	}
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
    DebugPrint( "Button - '%d'\n", buttonPressed );
    switch( buttonPressed )
    {
    case BUTTON_A:
        {
            break;
        }
    case BUTTON_B:
        {
            break;
        }
    case BUTTON_C:
        {
            break;
        }
    case BUTTON_D:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
                g_pszCurrentItem = m_Items[GetSelectedItem()];
                g_bBreakLoop = true;
            }
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
			if( GetPageStart() > 0 )
			{
				--m_uiPage;
				SetSelectedItem( GetPageStart() );
			}
            break;
        }
    case BUTTON_8:
        {
			if( GetPageEnd() < ( GetNumItems() - 1 ) )
			{
				++m_uiPage;
				SetSelectedItem( GetPageStart() );
			}
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

    return S_OK;
}
