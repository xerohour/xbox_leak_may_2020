/*****************************************************
*** datetimemenu.h
***
*** Header file for our DATE/TIME menu class.  This
*** menu will allow the user to select a different
*** date / time.
***
*** by James N. Helm
*** June 18th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "datetimemenu.h"

// Constructors
CDateTimeMenu::CDateTimeMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_bDateTimeModified( FALSE )
{
    XDBGWRN( APP_TITLE_NAME_A, "CDateTimeMenu::CDateTimeMenu()" );

    m_uiNumItems = DATETIMEMENU_NUM_ITEMS;
    SetSelectedItem( 0 );

    ZeroMemory( &m_sysTime, sizeof( m_sysTime ) );
    GetSystemTime( &m_sysTime );

    m_dwTickCount = GetTickCount();
}


// Destructor
CDateTimeMenu::~CDateTimeMenu()
{
}


// Draws a menu on to the screen
void CDateTimeMenu::Action( CUDTexture* pTexture )
{
    if( ( ( GetTickCount() - m_dwTickCount ) > 1000 ) && ( !m_bDateTimeModified ) )
    {
        m_dwTickCount = GetTickCount();
        GetSystemTime( &m_sysTime );

        m_bUpdateTexture = TRUE;
    }

    // Check to see if we should update the texture
    // If not, return
    if( !m_bUpdateTexture )
    {
        return;
    }
    else // reset the texture flag to FALSE
    {
        m_bUpdateTexture = FALSE;
    }

    // Lock our texture and set it as the render target
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    // Let our base menu draw the items from the data file
    CMenuScreen::Action( pTexture );

    // Create the Time String to be displayed
    WCHAR pwszTimeString[50];
    ZeroMemory( pwszTimeString, sizeof( WCHAR ) * 50 );
    _snwprintf( pwszTimeString, 49, L"%02d:%02d:%02d  %02d/%02d/%04d", m_sysTime.wHour, m_sysTime.wMinute, m_sysTime.wSecond, m_sysTime.wMonth, m_sysTime.wDay, m_sysTime.wYear );
    /*
    char pszHour[3], pszMinute[3], pszSecond[3], pszMonth[3], pszDay[3], pszYear[5];
    pszHour[2] = pszMinute[2] = pszSecond[2] = pszMonth[2] = pszDay[2] = pszYear[4] = '\0';

    sprintf( pszHour, "%02d", m_sysTime.wHour );
    sprintf( pszMinute, "%02d", m_sysTime.wMinute );
    sprintf( pszSecond, "%02d", m_sysTime.wSecond );
    sprintf( pszMonth, "%02d", m_sysTime.wMonth );
    sprintf( pszDay, "%02d", m_sysTime.wDay );
    sprintf( pszYear, "%04d", m_sysTime.wYear );
    */

    // Calculate the position of the string on the screen
    float XPos = SCREEN_X_CENTER - ( GetStringPixelWidth( pwszTimeString ) / 2.0f );
    float YPos = SCREEN_Y_CENTER - ( m_uiFontHeightOfItem / 2.0f ); // MENUBOX_MAINAREA_Y1 + MENUBOX_MAINAREA_BORDERWIDTH_TOP + ( ( MENUBOX_ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * x );

    // Draw a selector around the current item
    float X1Pos, X2Pos, Y1Pos, Y2Pos;
    WCHAR pwszTempString[50];
    ZeroMemory( pwszTempString, sizeof( WCHAR ) * 50 );

    // We can calculate the YPos for the items no matter what they are
    Y1Pos = YPos - MENUBOX_SELECTOR_BORDER_WIDTH; // MENUBOX_MAINAREA_Y1 + MENUBOX_MAINAREA_BORDERWIDTH_TOP + ( ( MENUBOX_ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * GetSelectedItem() ) - MENUBOX_SELECTOR_BORDER_WIDTH;
    Y2Pos = YPos + m_uiFontHeightOfItem + MENUBOX_SELECTOR_BORDER_WIDTH;

    switch( GetSelectedItem() )
    {
    case DATETIMEMENU_HOURS:
        {
            X1Pos = XPos - MENUBOX_SELECTOR_BORDER_WIDTH; // MENUBOX_SELECTOR_X1;
            _snwprintf( pwszTempString, 49, L"%02d", m_sysTime.wHour );
            X2Pos = XPos + GetStringPixelWidth( pwszTempString ) + MENUBOX_SELECTOR_BORDER_WIDTH;

            break;
        }
    case DATETIMEMENU_MINUTES:
        {
            _snwprintf( pwszTempString, 49, L"%02d:", m_sysTime.wHour );
            X1Pos = XPos + GetStringPixelWidth( pwszTempString ) - MENUBOX_SELECTOR_BORDER_WIDTH; // MENUBOX_SELECTOR_X1;
            _snwprintf( pwszTempString, 49, L"%02d:%02d", m_sysTime.wHour, m_sysTime.wMinute );
            X2Pos = XPos + GetStringPixelWidth( pwszTempString ) + MENUBOX_SELECTOR_BORDER_WIDTH;

            break;
        }
    case DATETIMEMENU_SECONDS:
        {
            _snwprintf( pwszTempString, 49, L"%02d:%02d:", m_sysTime.wHour, m_sysTime.wMinute );
            X1Pos = XPos + GetStringPixelWidth( pwszTempString ) - MENUBOX_SELECTOR_BORDER_WIDTH; // MENUBOX_SELECTOR_X1;
            _snwprintf( pwszTempString, 49, L"%02d:%02d:%02d", m_sysTime.wHour, m_sysTime.wMinute, m_sysTime.wSecond );
            X2Pos = XPos + GetStringPixelWidth( pwszTempString ) + MENUBOX_SELECTOR_BORDER_WIDTH;

            break;
        }
    case DATETIMEMENU_MONTHS:
        {
            _snwprintf( pwszTempString, 49, L"%02d:%02d:%02d  ", m_sysTime.wHour, m_sysTime.wMinute, m_sysTime.wSecond );
            X1Pos = XPos + GetStringPixelWidth( pwszTempString ) - MENUBOX_SELECTOR_BORDER_WIDTH; // MENUBOX_SELECTOR_X1;
            _snwprintf( pwszTempString, 49, L"%02d:%02d:%02d  %02d", m_sysTime.wHour, m_sysTime.wMinute, m_sysTime.wSecond, m_sysTime.wMonth );
            X2Pos = XPos + GetStringPixelWidth( pwszTempString ) + MENUBOX_SELECTOR_BORDER_WIDTH;

            break;
        }
    case DATETIMEMENU_DAYS:
        {
            _snwprintf( pwszTempString, 49, L"%02d:%02d:%02d  %02d/", m_sysTime.wHour, m_sysTime.wMinute, m_sysTime.wSecond, m_sysTime.wMonth );
            X1Pos = XPos + GetStringPixelWidth( pwszTempString ) - MENUBOX_SELECTOR_BORDER_WIDTH; // MENUBOX_SELECTOR_X1;
            _snwprintf( pwszTempString, 49, L"%02d:%02d:%02d  %02d/%02d", m_sysTime.wHour, m_sysTime.wMinute, m_sysTime.wSecond, m_sysTime.wMonth, m_sysTime.wDay );
            X2Pos = XPos + GetStringPixelWidth( pwszTempString ) + MENUBOX_SELECTOR_BORDER_WIDTH;

            break;
        }
    case DATETIMEMENU_YEARS:
        {
            _snwprintf( pwszTempString, 49, L"%02d:%02d:%02d  %02d/%02d/", m_sysTime.wHour, m_sysTime.wMinute, m_sysTime.wSecond, m_sysTime.wMonth, m_sysTime.wDay );
            X1Pos = XPos + GetStringPixelWidth( pwszTempString ) - MENUBOX_SELECTOR_BORDER_WIDTH; // MENUBOX_SELECTOR_X1;
            _snwprintf( pwszTempString, 49, L"%02d:%02d:%02d  %02d/%02d/%04d", m_sysTime.wHour, m_sysTime.wMinute, m_sysTime.wSecond, m_sysTime.wMonth, m_sysTime.wDay, m_sysTime.wYear );
            X2Pos = XPos + GetStringPixelWidth( pwszTempString ) + MENUBOX_SELECTOR_BORDER_WIDTH;

            break;
        }
    }

    pTexture->DrawBox( X1Pos, Y1Pos, X2Pos, Y2Pos, SCREEN_DEFAULT_SELECTOR_COLOR );

    // Draw time
    pTexture->DrawText( XPos, YPos, SCREEN_DEFAULT_FOREGROUND_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%ls", pwszTimeString );

    // Unlock our texture and restore our render target
    pTexture->Unlock();
}


// Handle any requests for the joystick (thumb-pad)
void CDateTimeMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
                // Hand off this to the "DPad" handler
                HandleInput( CONTROL_DPAD_LEFT, bFirstXPress );
            }
            else if( ( nThumbX > 0 ) && ( abs( nThumbX ) > m_nJoystickDeadZone ) )// Move the left joystick right
            {
                // Hand off this to the "DPad" handler
                HandleInput( CONTROL_DPAD_RIGHT, bFirstXPress );
            }
            break;
        }
    }
}


// Handles input (of the CONTROLS) for the current menu
void CDateTimeMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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
    case CONTROL_DPAD_LEFT:
        {
            if( GetSelectedItem() > 0 )
            {
                m_bUpdateTexture = TRUE;

                SetSelectedItem( GetSelectedItem() - 1 );
            }
            break;
        }
    case CONTROL_DPAD_RIGHT:
        {
            if( GetSelectedItem() < (int)( GetNumItems() - 1 ) )
            {
                m_bUpdateTexture = TRUE;

                SetSelectedItem( GetSelectedItem() + 1 );
            }
            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            m_bUpdateTexture = TRUE;
            m_bDateTimeModified = TRUE;

            switch( GetSelectedItem() )
            {
            case DATETIMEMENU_HOURS:
                {
                    if( m_sysTime.wHour > 0 )
                    {
                        m_sysTime.wHour--;
                    }
                    else
                    {
                        m_sysTime.wHour = 23;
                    }

                    break;
                }
            case DATETIMEMENU_MINUTES:
                {
                    if( m_sysTime.wMinute > 0 )
                    {
                        m_sysTime.wMinute--;
                    }
                    else
                    {
                        m_sysTime.wMinute = 59;
                    }

                    break;
                }
            case DATETIMEMENU_SECONDS:
                {
                    if( m_sysTime.wSecond > 0 )
                    {
                        m_sysTime.wSecond--;
                    }
                    else
                    {
                        m_sysTime.wSecond = 59;
                    }

                    break;
                }
            case DATETIMEMENU_MONTHS:
                {
                    if( m_sysTime.wMonth > 1 )
                    {
                        m_sysTime.wMonth--;
                    }
                    else
                    {
                        m_sysTime.wMonth = 12;
                    }

                    break;
                }
            case DATETIMEMENU_DAYS:
                {
                    if( m_sysTime.wDay > 1 )
                    {
                        m_sysTime.wDay--;
                    }
                    else
                    {
                        m_sysTime.wDay = 31;
                    }

                    break;
                }
            case DATETIMEMENU_YEARS:
                {
                    if( m_sysTime.wYear > 1980 )
                    {
                        m_sysTime.wYear--;
                    }
                    else
                    {
                        m_sysTime.wYear = 2099;
                    }

                    break;
                }
            }

            break;
        }
    case CONTROL_DPAD_UP:
        {
            m_bUpdateTexture = TRUE;
            m_bDateTimeModified = TRUE;

            switch( GetSelectedItem() )
            {
            case DATETIMEMENU_HOURS:
                {
                    if( m_sysTime.wHour < 23 )
                    {
                        m_sysTime.wHour++;
                    }
                    else
                    {
                        m_sysTime.wHour = 0;
                    }

                    break;
                }
            case DATETIMEMENU_MINUTES:
                {
                    if( m_sysTime.wMinute < 59 )
                    {
                        m_sysTime.wMinute++;
                    }
                    else
                    {
                        m_sysTime.wMinute = 0;
                    }

                    break;
                }
            case DATETIMEMENU_SECONDS:
                {
                    if( m_sysTime.wSecond < 59 )
                    {
                        m_sysTime.wSecond++;
                    }
                    else
                    {
                        m_sysTime.wSecond = 0;
                    }

                    break;
                }
            case DATETIMEMENU_MONTHS:
                {
                    if( m_sysTime.wMonth < 12 )
                    {
                        m_sysTime.wMonth++;
                    }
                    else
                    {
                        m_sysTime.wMonth = 1;
                    }

                    break;
                }
            case DATETIMEMENU_DAYS:
                {
                    if( m_sysTime.wDay < 31 )
                    {
                        m_sysTime.wDay++;
                    }
                    else
                    {
                        m_sysTime.wDay = 1;
                    }

                    break;
                }
            case DATETIMEMENU_YEARS:
                {
                    if( m_sysTime.wYear < 2099 )
                    {
                        m_sysTime.wYear++;
                    }
                    else
                    {
                        m_sysTime.wYear = 1980;
                    }

                    break;
                }
            }

            break;
        }
    }
}


// Handles input (of the BUTTONS) for the current menu
void CDateTimeMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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
            // Refresh our Texture
            m_bUpdateTexture = TRUE;

            switch( GetSelectedItem() )
            {
            case DATETIMEMENU_HOURS:
                {
                    break;
                }
            case DATETIMEMENU_MINUTES:
                {
                    break;
                }
            case DATETIMEMENU_SECONDS:
                {
                    break;
                }
            case DATETIMEMENU_MONTHS:
                {
                    break;
                }
            case DATETIMEMENU_DAYS:
                {
                    break;
                }
            case DATETIMEMENU_YEARS:
                {
                    break;
                }
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
    case BUTTON_X:
        {
            break;
        }
    case BUTTON_Y:
        {
            break;
        }
    case BUTTON_BLACK:
        {
            break;
        }
    case BUTTON_WHITE:
        {
            break;
        }
    }
}


// Initialize the Menu
HRESULT CDateTimeMenu::Init( IDirect3DDevice8* pD3DDevice, char* menuFileName )
{
    XDBGWRN( APP_TITLE_NAME_A, "CDateTimeMenu::Init()" );

    HRESULT hr = CMenuScreen::Init( pD3DDevice, menuFileName );

    return hr;
}


/*
// Should be called whenever a user leaves the menu
void CDateTimeMenu::Leave( CXItem* pItem )
{
    // Call our base class leave
    CMenuScreen::Leave( pItem );

    // Default our selection to the top item when the user returns
    // SetSelectedItem( 0 );
}

// This will be called whenever the user enters this menu
void CDateTimeMenu::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();

    //
    // Update the MU's on our screen
    //
}
*/