/*****************************************************
*** maformatcommand.cpp
***
*** CPP file for our XDash CFG Memory Area Format
*** command option.
*** This option will allow the user to format (clean)
*** a Memory Area of all of it's data when a user
*** selects "A"
*** 
*** by James N. Helm
*** November 3rd, 2000
*** 
*****************************************************/

#include "maformatcommand.h"
#include "videoconstants.h"

extern CXDCItem* g_pCurrentScreen;  // Pointer to the current menu object

// Constructor
CMAFormatCommand::CMAFormatCommand()
: CXDCItem()
{
    m_uicNumItems = NUM_MEMORY_AREAS;

    wsprintf( m_Items[XDISK], L"XDisk - Hard drive of the XBox" );
    wsprintf( m_Items[MU1],   L"Memory Unit 1 - Controller 1 Slot 1" );
    wsprintf( m_Items[MU2],   L"Memory Unit 2 - Controller 1 Slot 2" );
    wsprintf( m_Items[MU3],   L"Memory Unit 3 - Controller 2 Slot 1" );
    wsprintf( m_Items[MU4],   L"Memory Unit 4 - Controller 2 Slot 2" );
    wsprintf( m_Items[MU5],   L"Memory Unit 5 - Controller 3 Slot 1" );
    wsprintf( m_Items[MU6],   L"Memory Unit 6 - Controller 3 Slot 2" );
    wsprintf( m_Items[MU7],   L"Memory Unit 7 - Controller 4 Slot 1" );
    wsprintf( m_Items[MU8],   L"Memory Unit 8 - Controller 4 Slot 2" );

    // Must be done after populating the above array
    SetSelectedItem( 0 );
}

// Constructor
CMAFormatCommand::CMAFormatCommand( CXDCItem* pParent, WCHAR* pwszTitle, WCHAR* pwszDescription )
: CXDCItem( pParent, pwszTitle, pwszDescription )
{
    m_uicNumItems = NUM_MEMORY_AREAS;

    wsprintf( m_Items[XDISK], L"XDisk - Hard drive of the XBox" );
    wsprintf( m_Items[MU1],   L"Memory Unit 1 - Controller 1 Slot 1" );
    wsprintf( m_Items[MU2],   L"Memory Unit 2 - Controller 1 Slot 2" );
    wsprintf( m_Items[MU3],   L"Memory Unit 3 - Controller 2 Slot 1" );
    wsprintf( m_Items[MU4],   L"Memory Unit 4 - Controller 2 Slot 2" );
    wsprintf( m_Items[MU5],   L"Memory Unit 5 - Controller 3 Slot 1" );
    wsprintf( m_Items[MU6],   L"Memory Unit 6 - Controller 3 Slot 2" );
    wsprintf( m_Items[MU7],   L"Memory Unit 7 - Controller 4 Slot 1" );
    wsprintf( m_Items[MU8],   L"Memory Unit 8 - Controller 4 Slot 2" );

    // Must be done after populating the above array
    SetSelectedItem( 0 );
}

// Destructor
CMAFormatCommand::~CMAFormatCommand()
{
}


// Draws a menu on to the screen
void CMAFormatCommand::Action( CXBoxVideo* pScreen )
{
    unsigned int nStringWidth       = 0;
    float fFontHeight               = 0.0;

    pScreen->GetFontSize( 0, &fFontHeight, 0, 0 );

    // Display the Title
    pScreen->DrawText( (float)SCREEN_X_CENTER - ( pScreen->GetStringPixelWidth( GetTitle() ) / 2 ), MENU_TITLE_Y, MENU_TITLE_FG_COLOR, MENU_TITLE_BG_COLOR, GetTitle() );

    // Draw the items on the screen
    // If we have any items on the screen, let's draw them

    // Draw a box behind the currently highlighted choice
    float fX1, fY1, fX2, fY2;
    fX1 = SCREEN_X_CENTER - ( pScreen->GetStringPixelWidth( g_apszMemoryAreas[GetSelectedItem()] ) / 2.0f ) - MENU_HIGHLIGHT_BORDER;
    fY1 = MENU_OPTION1_Y + ( ( fFontHeight + MENU_VERT_SPACING ) * GetSelectedItem() ) - MENU_HIGHLIGHT_BORDER;
    fX2 = SCREEN_X_CENTER + ( pScreen->GetStringPixelWidth( g_apszMemoryAreas[GetSelectedItem()] ) / 2.0f ) + MENU_HIGHLIGHT_BORDER;
    fY2 = MENU_OPTION1_Y + ( fFontHeight * ( GetSelectedItem() + 1 ) ) + ( MENU_VERT_SPACING * GetSelectedItem() ) + MENU_HIGHLIGHT_BORDER;

    pScreen->DrawOutline( fX1, fY1, fX2, fY2, MENU_HIGHLIGHT_BORDER_WIDTH, HIGHLIGHT_BOX_COLOR );

    // Draw the items on the menu
    for( unsigned int x = 0; x < GetNumItems(); ++x )
    {
        pScreen->DrawText( SCREEN_X_CENTER - ( pScreen->GetStringPixelWidth( g_apszMemoryAreas[x] ) / 2.0f ), MENU_OPTION1_Y + ( ( fFontHeight + MENU_VERT_SPACING ) * x ), MENU_ITEM_FG_COLOR, MENU_ITEM_BG_COLOR, g_apszMemoryAreas[x] );
    }
}

// Handles input for the current menu
void CMAFormatCommand::HandleInput( enum BUTTONS buttonPressed, bool bFirstPress )
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
                SetFooterText( L"Loser" );
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

// Get the description of a sub-item on our screen
WCHAR* CMAFormatCommand::GetItemDescription( unsigned int itemIndex ) const
{
    if( ( itemIndex < 0 ) || ( itemIndex >= GetNumItems() ) )
    {
        DebugPrint( "CMAFormatCommand::GetItemDescription(): index out of range - '%d', MAX - '%d'\n", itemIndex, GetNumItems() - 1 );
        return NULL;
    }

    return (WCHAR*)m_Items[itemIndex];
}

// Sets the currently selected item on our screen
HRESULT CMAFormatCommand::SetSelectedItem( unsigned int itemNumber )
{
    if( ( itemNumber < 0 ) || ( itemNumber >= GetNumItems() ) )
    {
        DebugPrint( "CMAFormatCommand::SetSelectedItem(): Tried to select item out of range! - '%d', MAX - '%d'\n", itemNumber, GetNumItems() - 1 );
        return E_INVALIDARG;
    }

    m_uiSelectedItem = itemNumber;

    SetFooterText( GetItemDescription( m_uiSelectedItem ) );

    return S_OK;
}
