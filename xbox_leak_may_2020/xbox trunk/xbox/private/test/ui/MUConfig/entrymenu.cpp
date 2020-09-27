/*****************************************************
*** entrymenu.cpp
***
*** CPP file for our Entry Point menu
*** class. This menu class will allow you to perform
*** various Entry Point operations.
***
*** by James N. Helm
*** April 23rd, 2001
***
*****************************************************/

#include "stdafx.h"
#include "entrymenu.h"

extern CKeypadMenu      g_MenuKeypad;   // Virtual Keypad Menu to be used by all screens
extern CKeyboardMenu    g_MenuKeyboard; // Virtual Keyboard Menu to be used by all screens

// Constructors
CEntryMenu::CEntryMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_pwszValueBuffer( NULL ),
m_pwszBuffer1( NULL ),
m_pwszBuffer2( NULL ),
m_uiTopItemIndex( 0 ),
m_uiNumItemsToDisplay( ENTRYMENU_NUM_ITEMS_TO_DISPLAY )
{
    XDBGWRN( APP_TITLE_NAME_A, "CEntryMenu::CEntryMenu()" );

    m_uiNumItems = ENTRYMENU_ITEM_NUM_ITEMS;
    SetSelectedItem( 0 );

    m_pwszValueBuffer = new WCHAR[MENU_ENTRY_NUMBER_SIZE];
    if( !m_pwszValueBuffer )
    {
        XDBGERR( APP_TITLE_NAME_A, "CEntryMenu::CEntryMenu():Failed to allocate memory!!" );
    }
}


// Destructor
CEntryMenu::~CEntryMenu()
{
    if( m_pwszValueBuffer )
    {
        delete[] m_pwszValueBuffer;
        m_pwszValueBuffer = NULL;
    }

    if( m_pwszBuffer1 )
    {
        delete[] m_pwszBuffer1;
        m_pwszBuffer1 = NULL;
    }

    if( m_pwszBuffer2 )
    {
        delete[] m_pwszBuffer2;
        m_pwszBuffer2 = NULL;
    }
}


// Draws a menu on to the screen
void CEntryMenu::Action( CUDTexture* pTexture )
{
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

    DWORD dwItemColor = 0;

    // Draw a selector around the current item
    float X1Pos = MENUBOX_SELECTOR_X1;
    float Y1Pos = MENUBOX_MAINAREA_Y1 + MENUBOX_MAINAREA_BORDERWIDTH_TOP + ( ( MENUBOX_ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * ( GetSelectedItem() - m_uiTopItemIndex ) ) - MENUBOX_SELECTOR_BORDER_WIDTH;
    float X2Pos = MENUBOX_SELECTOR_X2;
    float Y2Pos = Y1Pos + m_uiFontHeightOfItem + ( 2 * MENUBOX_SELECTOR_BORDER_WIDTH );

    pTexture->DrawBox( X1Pos, Y1Pos, X2Pos, Y2Pos, SCREEN_DEFAULT_SELECTOR_COLOR );

    // Draw the Menu Options
    for( unsigned int x = m_uiTopItemIndex; x < ( m_uiNumItemsToDisplay + m_uiTopItemIndex ); ++x )
    {
        // Make sure we don't try to access out of range items
        if( x == GetNumItems() )
        {
            break;
        }

        float XPos = MENUBOX_MAINAREA_X1 + MENUBOX_MAINAREA_BORDERWIDTH_LEFT;
        float YPos = MENUBOX_MAINAREA_Y1 + MENUBOX_MAINAREA_BORDERWIDTH_TOP + ( ( MENUBOX_ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * ( x - m_uiTopItemIndex ) );

        DWORD dwItemColor = SCREEN_ACTIVE_OPTION_COLOR;

        pTexture->DrawText( XPos, YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", g_wpszEntryMenuItems[x] );
    }

    // Unlock our texture and restore our render target
    pTexture->Unlock();

	if( m_bActionInitiated )
	{
        m_bActionInitiated = FALSE;

        switch( GetSelectedItem() )
        {
		/*
        case ENTRYMENU_ITEM_ROOT_MENU:
			{
			}
		*/
        case ENTRYMENU_ITEM_MEMORY_CLEANUP:
            {
                // Check the value passed for the number of blocks, and asked
                // for the drive letter

                // Check to see if the user set an initial value
                if( L'\0' == m_pwszBuffer1[0] )
                {
                    break;
                }

                // Get the drive letter
                if( L'\0' == m_pwszBuffer2[0] )
                {
				    // Prompt the user for the Drive Letter
                    g_MenuKeyboard.SetInfo( L"Enter the drive letter", m_pwszBuffer2, 1 );
                    g_MenuKeyboard.SetParent( this );
                    m_bActionInitiated = TRUE;

                    Leave( &g_MenuKeyboard );

                    break;
                }

                // Process our values
                
                WCHAR* pwszStopString = NULL;

                // Convert the user value to a DWORD
                DWORD dwNumBlocks = wcstol( m_pwszBuffer1, &pwszStopString, 10 );

				// Reboot to the Memory Cleanup menu
				LD_LAUNCH_DASHBOARD launchData;
				ZeroMemory( &launchData, sizeof( launchData ) );

				launchData.dwReason     = XLD_LAUNCH_DASHBOARD_MEMORY;
                launchData.dwContext    = XSHELL_LAUNCH_CONTEXT_MEMORY;
                launchData.dwParameter1 = m_pwszBuffer2[0];
				launchData.dwParameter2 = dwNumBlocks;

				XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );
			}
        case ENTRYMENU_ITEM_MUSIC:
            {
                break;
            }
        case ENTRYMENU_ITEM_SETTINGS:
            {
                // Check to see if the user set a value
                if( L'\0' == m_pwszBuffer1[0] )
                {
                    break;
                }


                // Reboot to the XDash Settings Menu
                WCHAR* pwszStopString = NULL;

				LD_LAUNCH_DASHBOARD launchData;
				ZeroMemory( &launchData, sizeof( launchData ) );

				launchData.dwReason  = XLD_LAUNCH_DASHBOARD_SETTINGS;
                launchData.dwContext = XSHELL_LAUNCH_CONTEXT_SETTINGS;
				launchData.dwParameter1 = wcstol( m_pwszBuffer1, &pwszStopString, 10 );

				XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );

                break;
            }
        case ENTRYMENU_ITEM_ERROR:
            {
                // Check to see if the user set a value
                if( L'\0' == m_pwszBuffer1[0] )
                {
                    break;
                }

                // Get the second error code
                if( L'\0' == m_pwszBuffer2[0] )
                {
				    // Prompt the user for the Drive Letter
                    g_MenuKeypad.SetInfo( L"Enter the 2nd Error Code", m_pwszBuffer2, MENU_ENTRY_NUMBER_SIZE - 1 );
                    g_MenuKeypad.SetParent( this );
                    m_bActionInitiated = TRUE;

                    Leave( &g_MenuKeypad );

                    break;
                }

                WCHAR* pwszStopString = NULL;

				// Reboot to the Error Display
				LD_LAUNCH_DASHBOARD launchData;
				ZeroMemory( &launchData, sizeof( launchData ) );

				launchData.dwReason     = XLD_LAUNCH_DASHBOARD_ERROR;
                launchData.dwContext    = XSHELL_LAUNCH_CONTEXT_ERROR;
				launchData.dwParameter1 = wcstol( m_pwszBuffer1, &pwszStopString, 10 );
                launchData.dwParameter2 = wcstol( m_pwszBuffer2, &pwszStopString, 10 );

				XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );
			}
		}
	}
}


// Handle any requests for the joystick (thumb-pad)
void CEntryMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
void CEntryMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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
    case CONTROL_DPAD_UP:
        {
            if( GetSelectedItem() > 0 )
            {
                m_bUpdateTexture = TRUE;
                
                SetSelectedItem( GetSelectedItem() - 1 );

                // Make sure we are displaying the correct items
                if( GetSelectedItem() < (int)m_uiTopItemIndex )
                {
                    m_uiTopItemIndex = GetSelectedItem();
                }
            }
            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            if( (unsigned int)GetSelectedItem() < ( GetNumItems() - 1 ) )
            {
                m_bUpdateTexture = TRUE;

                SetSelectedItem( GetSelectedItem() + 1 );

                // Make sure we are displaying the correct items
                if( GetSelectedItem() >= (int)( m_uiNumItemsToDisplay + m_uiTopItemIndex ) )
                {
                    m_uiTopItemIndex += 1;
                }
            }
            break;
        }
    }
}


// Handles input (of the BUTTONS) for the current menu
void CEntryMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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
            case ENTRYMENU_ITEM_ROOT_MENU:
                {
					// Reboot to the XDash Root Menu
					LD_LAUNCH_DASHBOARD launchData;
					ZeroMemory( &launchData, sizeof( launchData ) );

                    launchData.dwContext = XSHELL_LAUNCH_CONTEXT_ROOT;
					launchData.dwReason = XLD_LAUNCH_DASHBOARD_MAIN_MENU;

					XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );
                    
                    break;
                }
            case ENTRYMENU_ITEM_MEMORY_CLEANUP:
                {
                    // Make sure we setup Buffer 1
                    if( m_pwszBuffer1 )
                    {
                        delete[] m_pwszBuffer1;
                        m_pwszBuffer1 = NULL;
                    }

                    m_pwszBuffer1 = new WCHAR[MENU_ENTRY_NUMBER_SIZE];
                    if( NULL == m_pwszBuffer1 )
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "CEntryMenu::HandleInput()(MEMORY)Failed to allocate memory for Buffer 1!!" );

                        break;
                    }
                    ZeroMemory( m_pwszBuffer1, sizeof( WCHAR ) * MENU_ENTRY_NUMBER_SIZE );

                    // Make sure we setup Buffer 2
                    if( m_pwszBuffer2 )
                    {
                        delete[] m_pwszBuffer2;
                        m_pwszBuffer2 = NULL;
                    }

                    m_pwszBuffer2 = new WCHAR[2];
                    if( NULL == m_pwszBuffer2 )
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "CEntryMenu::HandleInput()(MEMORY)Failed to allocate memory for Buffer 2!!" );

                        break;
                    }
                    ZeroMemory( m_pwszBuffer2, sizeof( WCHAR ) * 2 );

					// Prompt the user for the amount of space needed (blocks)
                    g_MenuKeypad.SetInfo( L"Enter # of Blocks", m_pwszBuffer1, MENU_ENTRY_NUMBER_SIZE - 1 );
                    g_MenuKeypad.SetParent( this );
                    m_bActionInitiated = TRUE;

                    Leave( &g_MenuKeypad );

                    break;
                }
            case ENTRYMENU_ITEM_MUSIC:
                {
					// Reboot to the XDash Music Menu
					LD_LAUNCH_DASHBOARD launchData;
					ZeroMemory( &launchData, sizeof( launchData ) );

                    launchData.dwContext = XSHELL_LAUNCH_CONTEXT_MUSIC;
					launchData.dwReason  = XLD_LAUNCH_DASHBOARD_MUSIC;

					XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );
                    
                    break;
                }
            case ENTRYMENU_ITEM_SETTINGS:
                {
                    // Make sure we setup Buffer 1
                    if( m_pwszBuffer1 )
                    {
                        delete[] m_pwszBuffer1;
                        m_pwszBuffer1 = NULL;
                    }

                    m_pwszBuffer1 = new WCHAR[MENU_ENTRY_NUMBER_SIZE];
                    if( NULL == m_pwszBuffer1 )
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "CEntryMenu::HandleInput()(MEMORY)Failed to allocate memory for Buffer 1!!" );

                        break;
                    }
                    ZeroMemory( m_pwszBuffer1, sizeof( WCHAR ) * MENU_ENTRY_NUMBER_SIZE );
					
					// Prompt the user for the amount of space needed (blocks)
                    g_MenuKeypad.SetInfo( L"Enter Settings Location #", m_pwszBuffer1, MENU_ENTRY_NUMBER_SIZE - 1 );
                    g_MenuKeypad.SetParent( this );
                    m_bActionInitiated = TRUE;

                    Leave( &g_MenuKeypad );

                    break;
                }

			case ENTRYMENU_ITEM_SETTINGS_CLOCK:
				{
					// Reboot to the XDash Clock
					LD_LAUNCH_DASHBOARD launchData;
					ZeroMemory( &launchData, sizeof( launchData ) );

                    launchData.dwContext    = XSHELL_LAUNCH_CONTEXT_SETTINGS;
					launchData.dwReason     = XLD_LAUNCH_DASHBOARD_SETTINGS;
					launchData.dwParameter1 = XLD_SETTINGS_CLOCK;

					XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );

                    break;
				}

			case ENTRYMENU_ITEM_SETTINGS_TIMEZONE:
				{
					// Reboot to the XDash TimeZone
					LD_LAUNCH_DASHBOARD launchData;
					ZeroMemory( &launchData, sizeof( launchData ) );

                    launchData.dwContext    = XSHELL_LAUNCH_CONTEXT_SETTINGS;
					launchData.dwReason     = XLD_LAUNCH_DASHBOARD_SETTINGS;
					launchData.dwParameter1 = XLD_SETTINGS_TIMEZONE;

					XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );

                    break;
				}
			case ENTRYMENU_ITEM_SETTINGS_LANGUAGE:
				{
					// Reboot to the XDash Language
					LD_LAUNCH_DASHBOARD launchData;
					ZeroMemory( &launchData, sizeof( launchData ) );

                    launchData.dwContext    = XSHELL_LAUNCH_CONTEXT_SETTINGS;
					launchData.dwReason     = XLD_LAUNCH_DASHBOARD_SETTINGS;
					launchData.dwParameter1 = XLD_SETTINGS_LANGUAGE;

					XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );

                    break;
				}

			case ENTRYMENU_ITEM_SETTINGS_CLOCK_TIMEZONE_LANGUAGE:
				{
					// Reboot to the XDash Clock, Timezone and language
					LD_LAUNCH_DASHBOARD launchData;
					ZeroMemory( &launchData, sizeof( launchData ) );

                    launchData.dwContext    = XSHELL_LAUNCH_CONTEXT_SETTINGS;
					launchData.dwReason     = XLD_LAUNCH_DASHBOARD_SETTINGS;
					launchData.dwParameter1 = XLD_SETTINGS_CLOCK + XLD_SETTINGS_TIMEZONE + XLD_SETTINGS_LANGUAGE;

					XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );

                    break;
				}
			case ENTRYMENU_ITEM_SETTINGS_CLOCK_TIMEZONE:
				{
					// Reboot to the XDash Clock and Timezone
					LD_LAUNCH_DASHBOARD launchData;
					ZeroMemory( &launchData, sizeof( launchData ) );

                    launchData.dwContext    = XSHELL_LAUNCH_CONTEXT_SETTINGS;
					launchData.dwReason     = XLD_LAUNCH_DASHBOARD_SETTINGS;
					launchData.dwParameter1 = XLD_SETTINGS_CLOCK + XLD_SETTINGS_TIMEZONE;

					XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );

                    break;
				}
			case ENTRYMENU_ITEM_SETTINGS_CLOCK_LANGUAGE:
				{
					// Reboot to the XDash Clock and language
					LD_LAUNCH_DASHBOARD launchData;
					ZeroMemory( &launchData, sizeof( launchData ) );

                    launchData.dwContext    = XSHELL_LAUNCH_CONTEXT_SETTINGS;
					launchData.dwReason     = XLD_LAUNCH_DASHBOARD_SETTINGS;
					launchData.dwParameter1 = XLD_SETTINGS_CLOCK + XLD_SETTINGS_LANGUAGE;

					XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );

                    break;
				}

			case ENTRYMENU_ITEM_SETTINGS_TIMEZONE_LANGUAGE:
				{
					// Reboot to the XDash Timezone and language
					LD_LAUNCH_DASHBOARD launchData;
					ZeroMemory( &launchData, sizeof( launchData ) );

                    launchData.dwContext    = XSHELL_LAUNCH_CONTEXT_SETTINGS;
					launchData.dwReason     = XLD_LAUNCH_DASHBOARD_SETTINGS;
					launchData.dwParameter1 = XLD_SETTINGS_TIMEZONE + XLD_SETTINGS_LANGUAGE;

					XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );

                    break;
				}

            case ENTRYMENU_ITEM_SETTINGS_COLLECTION:
                {
					// Reboot to the XDash Settings Collection Menu
					LD_LAUNCH_DASHBOARD launchData;
					ZeroMemory( &launchData, sizeof( launchData ) );

                    launchData.dwContext    = XSHELL_LAUNCH_CONTEXT_SETTINGS;
					launchData.dwReason     = XLD_LAUNCH_DASHBOARD_SETTINGS;

					XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );

                    break;
                }
            case ENTRYMENU_ITEM_SETTINGS_VIDEO:
                {
					// Reboot to the XDash Settings Video Menu
					LD_LAUNCH_DASHBOARD launchData;
					ZeroMemory( &launchData, sizeof( launchData ) );

                    launchData.dwContext    = XSHELL_LAUNCH_CONTEXT_SETTINGS;
					launchData.dwReason     = XLD_LAUNCH_DASHBOARD_SETTINGS;
                    launchData.dwParameter1 = XLD_SETTINGS_VIDEO;

					XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );

                    break;
                }
            case ENTRYMENU_ITEM_SETTINGS_AUDIO:
                {
					// Reboot to the XDash Settings Audio Menu
					LD_LAUNCH_DASHBOARD launchData;
					ZeroMemory( &launchData, sizeof( launchData ) );

                    launchData.dwContext    = XSHELL_LAUNCH_CONTEXT_SETTINGS;
					launchData.dwReason     = XLD_LAUNCH_DASHBOARD_SETTINGS;
                    launchData.dwParameter1 = XLD_SETTINGS_AUDIO;

					XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );

                    break;
                }
            case ENTRYMENU_ITEM_ERROR:
                {
                    // Make sure we setup Buffer 1
                    if( m_pwszBuffer1 )
                    {
                        delete[] m_pwszBuffer1;
                        m_pwszBuffer1 = NULL;
                    }

                    m_pwszBuffer1 = new WCHAR[MENU_ENTRY_NUMBER_SIZE];
                    if( NULL == m_pwszBuffer1 )
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "CEntryMenu::HandleInput()(MEMORY)Failed to allocate memory for Buffer 1!!" );

                        break;
                    }
                    ZeroMemory( m_pwszBuffer1, sizeof( WCHAR ) * MENU_ENTRY_NUMBER_SIZE );

                    // Make sure we setup Buffer 2
                    if( m_pwszBuffer2 )
                    {
                        delete[] m_pwszBuffer2;
                        m_pwszBuffer2 = NULL;
                    }

                    m_pwszBuffer2 = new WCHAR[MENU_ENTRY_NUMBER_SIZE];
                    if( NULL == m_pwszBuffer2 )
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "CEntryMenu::HandleInput()(MEMORY)Failed to allocate memory for Buffer 2!!" );

                        break;
                    }
                    ZeroMemory( m_pwszBuffer2, sizeof( WCHAR ) * MENU_ENTRY_NUMBER_SIZE );

					// Propmt user for error code
                    g_MenuKeypad.SetInfo( L"Enter the 1st Error Code", m_pwszBuffer1, MENU_ENTRY_NUMBER_SIZE - 1 );
                    g_MenuKeypad.SetParent( this );
                    m_bActionInitiated = TRUE;

                    Leave( &g_MenuKeypad );

                    break;
                }
            case ENTRYMENU_ITEM_ERROR_INVALID_XBE:
                {
					// Reboot to the XDash Error Menu
					LD_LAUNCH_DASHBOARD launchData;
					ZeroMemory( &launchData, sizeof( launchData ) );

				    launchData.dwReason     = XLD_LAUNCH_DASHBOARD_ERROR;
                    launchData.dwContext    = XSHELL_LAUNCH_CONTEXT_ERROR;
				    launchData.dwParameter1 = XLD_ERROR_INVALID_XBE;

					XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );

                    break;
                }
            case ENTRYMENU_ITEM_ERROR_INVALID_HARD_DISK:
                {
					// Reboot to the XDash Error Menu
					LD_LAUNCH_DASHBOARD launchData;
					ZeroMemory( &launchData, sizeof( launchData ) );

				    launchData.dwReason     = XLD_LAUNCH_DASHBOARD_ERROR;
                    launchData.dwContext    = XSHELL_LAUNCH_CONTEXT_ERROR;
				    launchData.dwParameter1 = XLD_ERROR_INVALID_HARD_DISK;

					XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );

                    break;
                }
            case ENTRYMENU_ITEM_ERROR_INVALID_XBE_REGION:
                {
					// Reboot to the XDash Error Menu
					LD_LAUNCH_DASHBOARD launchData;
					ZeroMemory( &launchData, sizeof( launchData ) );

				    launchData.dwReason     = XLD_LAUNCH_DASHBOARD_ERROR;
                    launchData.dwContext    = XSHELL_LAUNCH_CONTEXT_ERROR;
				    launchData.dwParameter1 = XLD_ERROR_XBE_REGION;

					XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );

                    break;
                }
            case ENTRYMENU_ITEM_ERROR_INVALID_XBE_PARENTAL_CONTROL:
                {
					// Reboot to the XDash Error Menu
					LD_LAUNCH_DASHBOARD launchData;
					ZeroMemory( &launchData, sizeof( launchData ) );

				    launchData.dwReason     = XLD_LAUNCH_DASHBOARD_ERROR;
                    launchData.dwContext    = XSHELL_LAUNCH_CONTEXT_ERROR;
				    launchData.dwParameter1 = XLD_ERROR_XBE_PARENTAL_CONTROL;

					XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );

                    break;
                }
            case ENTRYMENU_ITEM_ERROR_INVALID_XBE_MEDIA_TYPE:
                {
					// Reboot to the XDash Error Menu
					LD_LAUNCH_DASHBOARD launchData;
					ZeroMemory( &launchData, sizeof( launchData ) );

				    launchData.dwReason     = XLD_LAUNCH_DASHBOARD_ERROR;
                    launchData.dwContext    = XSHELL_LAUNCH_CONTEXT_ERROR;
				    launchData.dwParameter1 = XLD_ERROR_XBE_MEDIA_TYPE;

					XLaunchNewImage( FILE_DATA_XDASH_XBE_LOCATION, (PLAUNCH_DATA)&launchData );

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
HRESULT CEntryMenu::Init( IDirect3DDevice8* pD3DDevice, char* menuFileName )
{
    XDBGTRC( APP_TITLE_NAME_A, "CEntryMenu::Init()" );

    CMenuScreen::Init( pD3DDevice, menuFileName );

    return S_OK;
}
