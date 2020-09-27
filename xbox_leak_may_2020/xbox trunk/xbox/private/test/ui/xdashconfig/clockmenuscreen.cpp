/*****************************************************
*** clockmenuscreen.cpp
***
*** CPP file for our XDash CFG Clock menu class.
*** This clock menu contains a list of time and 
*** date objects that will be manipulated
*** 
*** by James N. Helm
*** November 2nd, 2000
*** 
*****************************************************/

#include "clockmenuscreen.h"
#include "videoconstants.h"

extern CXDCItem* g_pCurrentScreen;  // Pointer to the current menu object

// Constructor
CClockMenuScreen::CClockMenuScreen()
: CXDCItem()
{
    m_fUserAdjustedTime = false;
    m_uicNumItems = NUM_CLOCK_ITEMS;

    wsprintf( m_Items[0], L"Month" );
    wsprintf( m_Items[1], L"Day" );
    wsprintf( m_Items[2], L"Year" );
    wsprintf( m_Items[3], L"Hour" );
    wsprintf( m_Items[4], L"Minute" );
    wsprintf( m_Items[5], L"Second" );

    // Must be done after populating the above array
    SetSelectedItem( 0 );

    ZeroMemory( &m_sysUserTime, sizeof( SYSTEMTIME ) );
    GetSystemTime( &m_sysUserTime ); // Get the current time
}

// Constructor
CClockMenuScreen::CClockMenuScreen( CXDCItem* pParent, WCHAR* pwszTitle, WCHAR* pwszDescription )
: CXDCItem( pParent, pwszTitle, pwszDescription )
{
    m_fUserAdjustedTime = false;
    m_uicNumItems = NUM_CLOCK_ITEMS;

    wsprintf( m_Items[0], L"Month" );
    wsprintf( m_Items[1], L"Day" );
    wsprintf( m_Items[2], L"Year" );
    wsprintf( m_Items[3], L"Hour" );
    wsprintf( m_Items[4], L"Minute" );
    wsprintf( m_Items[5], L"Second" );

    // Must be done after populating the above array
    SetSelectedItem( 0 );

    ZeroMemory( &m_sysUserTime, sizeof( SYSTEMTIME ) );
    GetSystemTime( &m_sysUserTime ); // Get the current time
}

// Destructor
CClockMenuScreen::~CClockMenuScreen()
{
}


// Draws a menu on to the screen
void CClockMenuScreen::Action( CXBoxVideo* pScreen )
{
    unsigned int nStringWidth       = 0;
    float fFontHeight               = 0.0;
    int datePixelSize               = 0;
    int timePixelSize               = 0;
    int spacePixelSize              = 0;

    float date1_x1, date2_x1, date3_x1;         // X1 Positions of each date field
    float date1_x2, date2_x2, date3_x2;         // X2 Positions of each date field
    float time1_x1, time2_x1, time3_x1;         // X1 Positions of each time field
    float time1_x2, time2_x2, time3_x2;         // X1 Positions of each time field

    pScreen->GetFontSize( 0, &fFontHeight, 0, 0 );

    if( false == m_fUserAdjustedTime )
    {
        GetSystemTime( &m_sysUserTime ); // Get the current time
    }

    spacePixelSize = pScreen->GetStringPixelWidth( L" " );

    date1_x1 = (float)SCREEN_X_CENTER - ( pScreen->GetStringPixelWidth( L"September 00 0000" ) / 2 );
    date2_x1 = date1_x1 + pScreen->GetStringPixelWidth( L"September " );
    date3_x1 = date2_x1 + pScreen->GetStringPixelWidth( L"00 " );

    date1_x2 = date1_x1 + pScreen->GetStringPixelWidth( L"September" );
    date2_x2 = date2_x1 + pScreen->GetStringPixelWidth( L"00" );
    date3_x2 = date3_x1 + pScreen->GetStringPixelWidth( L"0000" );

    time1_x1 = (float)SCREEN_X_CENTER - ( pScreen->GetStringPixelWidth( L"00 : 00 : 00" ) / 2 );
    time2_x1 = time1_x1 + pScreen->GetStringPixelWidth( L"00 : " );
    time3_x1 = time2_x1 + pScreen->GetStringPixelWidth( L"00 : " );

    time1_x2 = time1_x1 + pScreen->GetStringPixelWidth( L"00" );
    time2_x2 = time2_x1 + pScreen->GetStringPixelWidth( L"00" );
    time3_x2 = time3_x1 + pScreen->GetStringPixelWidth( L"00" );

    // Display the Title
    pScreen->DrawText( (float)SCREEN_X_CENTER - ( pScreen->GetStringPixelWidth( GetTitle() ) / 2 ), MENU_TITLE_Y, MENU_TITLE_FG_COLOR, MENU_TITLE_BG_COLOR, GetTitle() );

    // Draw date and time on screen
    //   Month
    pScreen->DrawText( date1_x1, MENU_OPTION1_Y, MENU_ITEM_FG_COLOR, MENU_ITEM_BG_COLOR, g_apszMonths[m_sysUserTime.wMonth] );
    //   Day
    pScreen->DrawText( date2_x1, MENU_OPTION1_Y, MENU_ITEM_FG_COLOR, MENU_ITEM_BG_COLOR, L"%02d", m_sysUserTime.wDay );
    //   Year
    pScreen->DrawText( date3_x1, MENU_OPTION1_Y, MENU_ITEM_FG_COLOR, MENU_ITEM_BG_COLOR, L"%d", m_sysUserTime.wYear );

    //   Hour
    pScreen->DrawText( time1_x1, MENU_OPTION1_Y + ( fFontHeight + MENU_VERT_SPACING ), MENU_ITEM_FG_COLOR, MENU_ITEM_BG_COLOR, L"%02d : ", m_sysUserTime.wHour );
    //   Minute
    pScreen->DrawText( time2_x1, MENU_OPTION1_Y + ( fFontHeight + MENU_VERT_SPACING ), MENU_ITEM_FG_COLOR, MENU_ITEM_BG_COLOR, L"%02d : ", m_sysUserTime.wMinute );
    //   Second
    pScreen->DrawText( time3_x1, MENU_OPTION1_Y + ( fFontHeight + MENU_VERT_SPACING ), MENU_ITEM_FG_COLOR, MENU_ITEM_BG_COLOR, L"%02d", m_sysUserTime.wSecond );
    
    // Highlight current choice
    switch( GetSelectedItem() )
    {
    case MONTH:
        {
            pScreen->DrawOutline( date1_x1 - MENU_HIGHLIGHT_BORDER, MENU_OPTION1_Y - MENU_HIGHLIGHT_BORDER, date1_x2 + MENU_HIGHLIGHT_BORDER, MENU_OPTION1_Y + fFontHeight + MENU_HIGHLIGHT_BORDER, MENU_HIGHLIGHT_BORDER_WIDTH, HIGHLIGHT_BOX_COLOR );
            break;
        }
    case DAY:
        {
            pScreen->DrawOutline( date2_x1 - MENU_HIGHLIGHT_BORDER, MENU_OPTION1_Y - MENU_HIGHLIGHT_BORDER, date2_x2 + MENU_HIGHLIGHT_BORDER, MENU_OPTION1_Y + fFontHeight + MENU_HIGHLIGHT_BORDER, MENU_HIGHLIGHT_BORDER_WIDTH, HIGHLIGHT_BOX_COLOR );
            break;
        }
    case YEAR:
        {
            pScreen->DrawOutline( date3_x1 - MENU_HIGHLIGHT_BORDER, MENU_OPTION1_Y - MENU_HIGHLIGHT_BORDER, date3_x2 + MENU_HIGHLIGHT_BORDER, MENU_OPTION1_Y + fFontHeight + MENU_HIGHLIGHT_BORDER, MENU_HIGHLIGHT_BORDER_WIDTH, HIGHLIGHT_BOX_COLOR );
            break;
        }
    case HOUR:
        {
            pScreen->DrawOutline( time1_x1 - MENU_HIGHLIGHT_BORDER, MENU_OPTION1_Y + ( fFontHeight + MENU_VERT_SPACING ) - MENU_HIGHLIGHT_BORDER, time1_x2 + MENU_HIGHLIGHT_BORDER, MENU_OPTION1_Y + ( fFontHeight * 2 ) + MENU_VERT_SPACING + MENU_HIGHLIGHT_BORDER, MENU_HIGHLIGHT_BORDER_WIDTH, HIGHLIGHT_BOX_COLOR );
            break;
        }
    case MINUTE:
        {
            pScreen->DrawOutline( time2_x1 - MENU_HIGHLIGHT_BORDER, MENU_OPTION1_Y + ( fFontHeight + MENU_VERT_SPACING ) - MENU_HIGHLIGHT_BORDER, time2_x2 + MENU_HIGHLIGHT_BORDER, MENU_OPTION1_Y + ( fFontHeight * 2 ) + MENU_VERT_SPACING + MENU_HIGHLIGHT_BORDER, MENU_HIGHLIGHT_BORDER_WIDTH, HIGHLIGHT_BOX_COLOR );
            break;
        }
    case SECOND:
        {
            pScreen->DrawOutline( time3_x1 - MENU_HIGHLIGHT_BORDER, MENU_OPTION1_Y + ( fFontHeight + MENU_VERT_SPACING ) - MENU_HIGHLIGHT_BORDER, time3_x2 + MENU_HIGHLIGHT_BORDER, MENU_OPTION1_Y + ( fFontHeight * 2 ) + MENU_VERT_SPACING + MENU_HIGHLIGHT_BORDER, MENU_HIGHLIGHT_BORDER_WIDTH, HIGHLIGHT_BOX_COLOR );
            break;
        }
    }
}

// Handles input for the current menu
void CClockMenuScreen::HandleInput( enum BUTTONS buttonPressed, bool bFirstPress )
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
            m_fUserAdjustedTime = true;
            switch( GetSelectedItem() )
            {
            case MONTH:
                {
                    if( 11 == m_sysUserTime.wMonth )
                    {
                        m_sysUserTime.wMonth = 0;
                    }
                    else
                    {
                        ++m_sysUserTime.wMonth;
                    }
                    break;
                }
            case DAY:
                {
                    if( 31 == m_sysUserTime.wDay )
                    {
                        m_sysUserTime.wDay = 1;
                    }
                    else
                    {
                        ++m_sysUserTime.wDay;
                    }
                    break;
                }
            case YEAR:
                {
                    if( 2032 == m_sysUserTime.wYear )
                    {
                        m_sysUserTime.wYear = 2000;
                    }
                    else
                    {
                        ++m_sysUserTime.wYear;
                    }
                    break;
                }
            case HOUR:
                {
                    if( 23 == m_sysUserTime.wHour )
                    {
                        m_sysUserTime.wHour = 0;
                    }
                    else
                    {
                        ++m_sysUserTime.wHour;
                    }
                    break;
                }
            case MINUTE:
                {
                    if( 59 == m_sysUserTime.wMinute )
                    {
                        m_sysUserTime.wMinute = 0;
                    }
                    else
                    {
                        ++m_sysUserTime.wMinute;
                    }
                    break;
                }
            case SECOND:
                {
                    if( 59 == m_sysUserTime.wSecond )
                    {
                        m_sysUserTime.wSecond = 0;
                    }
                    else
                    {
                        ++m_sysUserTime.wSecond;
                    }
                    break;
                }
            };
            break;
        }
    case DPAD_BOTTOM:
        {
            m_fUserAdjustedTime = true;
            switch( GetSelectedItem() )
            {
            case MONTH:
                {
                    if( 0 == m_sysUserTime.wMonth )
                    {
                        m_sysUserTime.wMonth = 11;
                    }
                    else
                    {
                        --m_sysUserTime.wMonth;
                    }
                    break;
                }
            case DAY:
                {
                    if( 1 == m_sysUserTime.wDay )
                    {
                        m_sysUserTime.wDay = 31;
                    }
                    else
                    {
                        --m_sysUserTime.wDay;
                    }
                    break;
                }
            case YEAR:
                {
                    if( 2000 == m_sysUserTime.wYear )
                    {
                        m_sysUserTime.wYear = 2032;
                    }
                    else
                    {
                        --m_sysUserTime.wYear;
                    }
                    break;
                }
            case HOUR:
                {
                    if( 0 == m_sysUserTime.wHour )
                    {
                        m_sysUserTime.wHour = 23;
                    }
                    else
                    {
                        --m_sysUserTime.wHour;
                    }
                    break;
                }
            case MINUTE:
                {
                    if( 0 == m_sysUserTime.wMinute )
                    {
                        m_sysUserTime.wMinute = 59;
                    }
                    else
                    {
                        --m_sysUserTime.wMinute;
                    }
                    break;
                }
            case SECOND:
                {
                    if( 0 == m_sysUserTime.wSecond )
                    {
                        m_sysUserTime.wSecond = 59;
                    }
                    else
                    {
                        --m_sysUserTime.wSecond;
                    }
                    break;
                }
            };
            break;
        }
    case DPAD_LEFT:
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
    case DPAD_RIGHT:
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
    }
}

// Get the description of a sub-item on our screen
WCHAR* CClockMenuScreen::GetItemDescription( unsigned int itemIndex ) const
{
    if( ( itemIndex < 0 ) || ( itemIndex >= GetNumItems() ) )
    {
        DebugPrint( "CXDCItem::GetItemDescription(): index out of range - '%d'\n", itemIndex );
        return NULL;
    }

    return (WCHAR*)m_Items[itemIndex];
}

// Sets the currently selected item on our screen
HRESULT CClockMenuScreen::SetSelectedItem( unsigned int itemNumber )
{
    if( ( itemNumber < 0 ) || ( itemNumber >= GetNumItems() ) )
    {
        DebugPrint( "CClockMenuScreen::SetSelectedItem(): Tried to select item out of range! - '%d', MAX - '%d'\n", itemNumber, GetNumItems() - 1 );
        return E_INVALIDARG;
    }

    m_uiSelectedItem = itemNumber;

    SetFooterText( GetItemDescription( m_uiSelectedItem ) );

    return S_OK;
}
