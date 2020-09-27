/*****************************************************
*** xbconfigmenu.cpp
***
*** CPP file for our Xbox Config menu
*** class. This menu class will allow you to change
*** various options on the Xbox
***
*** by James N. Helm
*** May 10th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "xbconfigmenu.h"

extern CXSettings g_XboxSettings;   // Xbox Settings Object
extern CHardDrive g_HardDrive;      // Xbox Hard Drive

// Constructors
CXBConfigMenu::CXBConfigMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_MenuGameRegion( NULL ),
m_MenuAVRegion( NULL ),
m_MenuDVDRegion( NULL ),
m_bXboxReset( FALSE ),
m_bCDriveCorrupt( FALSE ),
m_bYDriveCorrupt( FALSE ),
m_bTimeZoneReset( FALSE ),
m_bLanguageReset( FALSE ),
m_bClockReset( FALSE ),
m_uiTopItemIndex( 0 ),
m_uiNumItemsToDisplay( MUOPTIONS_MENU_NUM_ITEMS_TO_DISPLAY )
{
    XDBGWRN( APP_TITLE_NAME_A, "CXBConfigMenu::CXBConfigMenu()" );

    m_uiNumItems = XBCONFIGMENU_ITEM_NUM_ITEMS;
    SetSelectedItem( 0 );
}


// Destructor
CXBConfigMenu::~CXBConfigMenu()
{
}

// Draws a menu on to the screen
void CXBConfigMenu::Action( CUDTexture* pTexture )
{
    // Check to see if our settings have changed
    g_XboxSettings.LoadSettings();

    if( g_XboxSettings.SettingsHaveChanged() )
    {
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

        pTexture->DrawText( XPos, YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", g_wpszXBConfigMenuItems[x] );

        switch( x )
        {
        case XBCONFIGMENU_ITEM_RESET_XBOX_TO_DEFAULTS:
            {
                if( m_bXboxReset )
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (TRUE)" );
                }
                else
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (FALSE)" );
                }

                break;
            }
        case XBCONFIGMENU_ITEM_RESET_CLOCK:
            {
                if( m_bClockReset )
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (TRUE)" );
                }
                else
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (FALSE)" );
                }

                break;
            }
        case XBCONFIGMENU_ITEM_RESET_TIMEZONE:
            {
                if( m_bTimeZoneReset )
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (TRUE)" );
                }
                else
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (FALSE)" );
                }

                break;
            }
        case XBCONFIGMENU_ITEM_RESET_LANGUAGE:
            {
                if( m_bLanguageReset )
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (TRUE)" );
                }
                else
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (FALSE)" );
                }

                break;
            }
        case XBCONFIGMENU_ITEM_SET_GAME_REGION:
            {
                pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (%d)", g_XboxSettings.GetGameRegion() );

                break;
            }
        case XBCONFIGMENU_ITEM_SET_DVD_REGION:
            {
                pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (%d)", g_XboxSettings.GetDVDRegion() );

                break;
            }
        case XBCONFIGMENU_ITEM_SET_AV_REGION:
            {
                pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (%d)", g_XboxSettings.GetAVRegion() );

                break;
            }
        case XBCONFIGMENU_ITEM_DISPLAY_RECKEY:
            {
                UCHAR pszRecKey[RECOVERY_KEY_LEN+1];
                ZeroMemory( pszRecKey, RECOVERY_KEY_LEN+1 );
                
                g_XboxSettings.GetHDRecoveryKey( pszRecKey, RECOVERY_KEY_LEN );

                pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (%hs)", pszRecKey );
                
                break;
            }
        case XBCONFIGMENU_ITEM_CLEAR_REFERB:
            {
                NTSTATUS status;
                XBOX_REFURB_INFO refurbInfo;

                status = ExReadWriteRefurbInfo(&refurbInfo, sizeof(refurbInfo), FALSE);
                if( !NT_SUCCESS(status) )
                {
                    XDBGWRN( APP_TITLE_NAME_A, "CXBConfigMenu::Action():Failed to Read the RefurbInfo!! Error - '0x%0.8X (%d)'", status, status );
                }
                else
                {
                    SYSTEMTIME systime;
                    FileTimeToSystemTime((FILETIME*) &refurbInfo.FirstSetTime, &systime);

                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (SC:%u %d/%d/%d %d %02d:%02d:%02d.%03d)", refurbInfo.PowerCycleCount, systime.wMonth, systime.wDay, systime.wYear, systime.wDayOfWeek, systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds );
                }

                break;
            }
        case XBCONFIGMENU_ITEM_CORRUPT_CDRIVE:
            {
                if( m_bCDriveCorrupt )
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Corrupt)" );
                }
                else
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Healthly)" );
                }

                break;
            }
        case XBCONFIGMENU_ITEM_CORRUPT_YDRIVE:
            {
                if( m_bYDriveCorrupt )
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Corrupt)" );
                }
                else
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Healthly)" );
                }

                break;
            }
        case XBCONFIGMENU_ITEM_TOGGLE_DTS:
            {
                if( g_XboxSettings.GetDTS() )
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Enabled)" );
                }
                else
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Disabled)" );
                }

                break;
            }
        case XBCONFIGMENU_ITEM_TOGGLE_DOLBY:
            {
                if( g_XboxSettings.GetDolbyDigital() )
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Enabled)" );
                }
                else
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Disabled)" );
                }

                break;
            }
        case XBCONFIGMENU_ITEM_TOGGLE_1080I:
            {
                if( g_XboxSettings.Get1080i() )
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Enabled)" );
                }
                else
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Disabled)" );
                }

                break;
            }
        case XBCONFIGMENU_ITEM_TOGGLE_720P:
            {
                if( g_XboxSettings.Get720p() )
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Enabled)" );
                }
                else
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Disabled)" );
                }

                break;
            }
        case XBCONFIGMENU_ITEM_TOGGLE_480P:
            {
                if( g_XboxSettings.Get480p() )
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Enabled)" );
                }
                else
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Disabled)" );
                }

                break;
            }
        case XBCONFIGMENU_ITEM_TOGGLE_PAL60:
            {
                if( g_XboxSettings.GetPAL60() )
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Enabled)" );
                }
                else
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Disabled)" );
                }

                break;
            }
        case XBCONFIGMENU_ITEM_TOGGLE_WIDESCREEN:
            {
                if( g_XboxSettings.GetWidescreen() )
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Enabled)" );
                }
                else
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Disabled)" );
                }

                break;
            }
        case XBCONFIGMENU_ITEM_TOGGLE_LETTERBOX:
            {
                if( g_XboxSettings.GetLetterbox() )
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Enabled)" );
                }
                else
                {
                    pTexture->DrawText( XPos + GetStringPixelWidth( g_wpszXBConfigMenuItems[x] ), YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L" (Disabled)" );
                }

                break;
            }
        }
    }

    // Unlock our texture and restore our render target
    pTexture->Unlock();
}


// Handle any requests for the joystick (thumb-pad)
void CXBConfigMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
void CXBConfigMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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
void CXBConfigMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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
            case XBCONFIGMENU_ITEM_RESET_XBOX_TO_DEFAULTS:
                {
                    m_bClockReset = TRUE;
                    m_bTimeZoneReset = TRUE;
                    m_bLanguageReset = TRUE;

					// Reset all of the Xbox settings to the their default values
                    if( FAILED( g_XboxSettings.ResetAllToDefaults() ) )
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "CXBConfigMenu::HandleInput():Failed to Reset all settings to their initial state!!" );
                    }
                    else
                    {
                        m_bXboxReset = TRUE;
                        XDBGWRN( APP_TITLE_NAME_A, "CXBConfigMenu::HandleInput():Success!!  Reset the Xbox Config Sector!!" );
                    }

                    break;
                }
            case XBCONFIGMENU_ITEM_RESET_CLOCK:
                {
                    m_bClockReset = TRUE;

                    //
                    // Set the Clock to a very early date
                    //
                    SYSTEMTIME sysTime;
                    ZeroMemory( &sysTime, sizeof( sysTime ) );

                    sysTime.wHour = 0;
                    sysTime.wMinute = 0;
                    sysTime.wSecond = 0;
                    sysTime.wMilliseconds = 0;
                    sysTime.wMonth = 1;
                    sysTime.wDay = 1;
                    sysTime.wDayOfWeek = 0;
                    sysTime.wYear = 1980;

                    XapiSetLocalTime(&sysTime);

                    break;
                }
            case XBCONFIGMENU_ITEM_RESET_TIMEZONE:
                {
                    m_bTimeZoneReset = TRUE;

                    TIME_ZONE_INFORMATION tzinfo;
                    ZeroMemory( &tzinfo, sizeof( tzinfo ) );

                    XapipSetTimeZoneInformation(&tzinfo);

                    break;
                }
            case XBCONFIGMENU_ITEM_RESET_LANGUAGE:
                {
                    m_bLanguageReset = TRUE;

                    g_XboxSettings.SetLanguage( 0 );

                    break;
                }
            case XBCONFIGMENU_ITEM_SET_GAME_REGION:
                {
					// Allow the user to set the Xbox Game Region
                    Leave( &m_MenuGameRegion );

                    break;
                }
            case XBCONFIGMENU_ITEM_SET_DVD_REGION:
                {
                    m_bXboxReset = FALSE;

					// Allow the user to set the Xbox DVD Region
                    Leave( &m_MenuDVDRegion );

                    break;
                }
            case XBCONFIGMENU_ITEM_SET_AV_REGION:
                {
					// Allow the user to set the Xbox AV Region
                    Leave( &m_MenuAVRegion );

                    break;
                }
            case XBCONFIGMENU_ITEM_CHANGE_HDKEY:
                {
                    UCHAR puRandomKey[XBOX_KEY_LENGTH];
                    ZeroMemory( puRandomKey, XBOX_KEY_LENGTH * sizeof( UCHAR ) );

                    // Change the HD Key Randomly
                    for( unsigned int x = 0; x < XBOX_KEY_LENGTH; x++ )
                    {
                        int iResult = rand() % 255;
                        puRandomKey[x] = (UCHAR)iResult;
                    }

                    g_XboxSettings.SetHDKey( puRandomKey );

                    break;
                }
            case XBCONFIGMENU_ITEM_RESET_HDKEY:
                {
                    UCHAR puZeroKey[XBOX_KEY_LENGTH];
                    ZeroMemory( puZeroKey, XBOX_KEY_LENGTH * sizeof( UCHAR ) );

                    g_XboxSettings.SetHDKey( puZeroKey );

                    break;
                }
            case XBCONFIGMENU_ITEM_CLEAR_REFERB:
                {
                    NTSTATUS status;
                    XBOX_REFURB_INFO refurbInfo;

                    ZeroMemory( &refurbInfo, sizeof( refurbInfo ) );

                    status = ExReadWriteRefurbInfo(&refurbInfo, sizeof(refurbInfo), TRUE);
                    if( !NT_SUCCESS(status) )
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "CXBConfigMenu::HandleInput():Failed to Clear the RefurbInfo!! Error - '0x%0.8X (%d)'", status, status );
                    }

                    break;
                }
            case XBCONFIGMENU_ITEM_CORRUPT_CDRIVE:
                {
                    OBJECT_STRING oCPart;

                    RtlInitObjectString( &oCPart, FILE_DATA_CDRIVE_PARTITION );

                    // Corrupt the C Partition
                    if( !g_HardDrive.CorruptPartition( &oCPart ) )
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "CXBConfigMenu::HandleInput():Failed to corrupt the C Partition!! Partition - '%hs', Error - '0x%0.8X (%d)'", FILE_DATA_CDRIVE_PARTITION, GetLastError(), GetLastError() );
                    }
                    else
                    {
                        m_bCDriveCorrupt = TRUE;
                    }

                    break;
                }
            case XBCONFIGMENU_ITEM_CORRUPT_YDRIVE:
                {
                    OBJECT_STRING oYPart;

                    RtlInitObjectString( &oYPart, FILE_DATA_YDRIVE_PARTITION );

                    // Corrupt the C Partition
                    if( !g_HardDrive.CorruptPartition( &oYPart ) )
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "CXBConfigMenu::HandleInput():Failed to corrupt the Y Partition!! Partition - '%hs', Error - '0x%0.8X (%d)'", FILE_DATA_CDRIVE_PARTITION, GetLastError(), GetLastError() );
                    }
                    else
                    {
                        m_bYDriveCorrupt = TRUE;
                    }

                    break;
                }
            case XBCONFIGMENU_ITEM_TOGGLE_DTS:
                {
                    m_bXboxReset = FALSE;

                    g_XboxSettings.SetDTS( !g_XboxSettings.GetDTS() );

                    break;
                }
            case XBCONFIGMENU_ITEM_TOGGLE_DOLBY:
                {
                    m_bXboxReset = FALSE;

                    g_XboxSettings.SetDolbyDigital( !g_XboxSettings.GetDolbyDigital() );

                    break;
                }
            case XBCONFIGMENU_ITEM_TOGGLE_1080I:
                {
                    m_bXboxReset = FALSE;

                    g_XboxSettings.Set1080i( !g_XboxSettings.Get1080i() );

                    break;
                }
            case XBCONFIGMENU_ITEM_TOGGLE_720P:
                {
                    m_bXboxReset = FALSE;

                    g_XboxSettings.Set720p( !g_XboxSettings.Get720p() );

                    break;
                }
            case XBCONFIGMENU_ITEM_TOGGLE_480P:
                {
                    m_bXboxReset = FALSE;

                    g_XboxSettings.Set480p( !g_XboxSettings.Get480p() );

                    break;
                }
            case XBCONFIGMENU_ITEM_TOGGLE_PAL60:
                {
                    m_bXboxReset = FALSE;

                    g_XboxSettings.SetPAL60( !g_XboxSettings.GetPAL60() );

                    break;
                }
            case XBCONFIGMENU_ITEM_TOGGLE_WIDESCREEN:
                {
                    m_bXboxReset = FALSE;

                    g_XboxSettings.SetWidescreen( !g_XboxSettings.GetWidescreen() );

                    break;
                }
            case XBCONFIGMENU_ITEM_TOGGLE_LETTERBOX:
                {
                    m_bXboxReset = FALSE;

                    g_XboxSettings.SetLetterbox( !g_XboxSettings.GetLetterbox() );

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
HRESULT CXBConfigMenu::Init( IDirect3DDevice8* pD3DDevice, char* menuFileName )
{
    XDBGWRN( APP_TITLE_NAME_A, "CXBConfigMenu::Init()" );

    CMenuScreen::Init( pD3DDevice, menuFileName );

    // Sub-Menu Initialization
    m_MenuGameRegion.SetParent( this );
    m_MenuGameRegion.Init( pD3DDevice, MENU_GAMEREGION_MENU_FILENAME_A );

    m_MenuDVDRegion.SetParent( this );
    m_MenuDVDRegion.Init( pD3DDevice, MENU_DVDREGION_MENU_FILENAME_A );

    m_MenuAVRegion.SetParent( this );
    m_MenuAVRegion.Init( pD3DDevice, MENU_AVREGION_MENU_FILENAME_A );

    return S_OK;
}
