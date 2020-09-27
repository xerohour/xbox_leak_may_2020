/*****************************************************
*** certmenu.h
***
*** CPP file for our XShell Certification menu 
*** class.  This is a standard menu contains a list
*** of menu items to be selected.
***
*** by James N. Helm
*** May 26th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "certmenu.h"

extern CXSettings           g_XboxSettings;         // Settings that are stored on the Xbox
extern IDirect3DDevice8*    g_pD3DDevice;           // Pointer to our Direct3D Device Object
extern CSoundCollection     g_Sounds;               // Used to generate all sounds for the XShell
extern BOOL                 g_bDisplayUpArrow;      // Used to determine if we should render the Up Arrow
extern BOOL                 g_bDisplayDownArrow;    // Used to determine if we should render the Down Arrow
extern CHardDrive           g_XboxHardDrive;        // Used to configure the Xbox Hard Drive
extern XFONT*               g_pFont;
extern CMADisplayMenu       g_MADisplayMenu;        // Used to display our Memory Areas

// Constructors
CCertMenu::CCertMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_HexKeypadMenu( NULL ),
m_DumpCacheMenu( NULL ),
m_LanguageMenu( NULL ),
m_TimeZoneMenu( NULL ),
m_EEPROMMenu( NULL ),
m_uiTopItemIndex( 0 ),
m_bProcessing( FALSE ),
m_bStartThread( FALSE ),
m_bDisableInput( FALSE ),
m_hThread( NULL ),
m_bAborting( FALSE ),
m_bDispHDTVSafe( FALSE ),
m_bDispNonHDTVSafe( FALSE )
{
    XDBGTRC( APP_TITLE_NAME_A, "CCertMenu::CCertMenu(ex)" );

    m_uiNumItems = CERTMENU_ITEM_NUM_ITEMS;
    SetSelectedItem( 0 );

    // Clear out our pre-allocated memory
    ZeroMemory( m_pwszTitleID, sizeof( WCHAR ) * MENU_CERT_TITLE_ID_STR_LEN+1 );

    SetTitle( MENU_TITLE_CERT );
}


// Destructor
CCertMenu::~CCertMenu()
{
}

// Draws a menu on to the screen
void CCertMenu::Action( CUDTexture* pTexture )
{
    // Check to see if we should take action
    if( m_bStartThread )
    {
        m_bUpdateTexture = TRUE;
        m_bProcessing = TRUE;
        m_bDisableInput = TRUE;
        m_bStartThread = FALSE;

        m_hThread = CreateThread( NULL,                     // Security Attributes
                                  0,                        // Stack Size (0 uses default)
                                  ProcessDataThreadFunc,    // Thread function
                                  this,                     // Parameter
                                  0,                        // Creation Flags
                                  NULL );                   // Thread ID
    }

    // Check on our Thread to see if it's still running
    DWORD dwThreadExitCode = 0;
    GetExitCodeThread( m_hThread, &dwThreadExitCode );

    // Check to see if our thread is done processing, and if so, udpate our texture, and re-enable input
    if( m_bProcessing && ( dwThreadExitCode != STILL_ACTIVE ) )
    {
        m_bUpdateTexture = TRUE;
        m_bProcessing = FALSE;
        m_bDisableInput = FALSE;
        g_XboxHardDrive.SetAbortProcess( FALSE );
        m_bAborting = FALSE;
    }

    if( m_bDispHDTVSafe )
    {
        DrawBoxPercentBased( SCREEN_HDTV_SAFE_PERCENTAGE, COLOR_YELLOW );
    }

    if( m_bDispNonHDTVSafe )
    {
        DrawBoxPercentBased( SCREEN_NONHDTV_SAFE_PERCENTAGE, COLOR_WHITE );
    }

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
    XFONT_SetTextHeight( g_pFont, ITEM_OPTIONMENU_FONT_HEIGHT );
    m_uiFontHeightOfItem = ITEM_OPTIONMENU_FONT_HEIGHT;

    // If we are processing a user action, update the texture and
    // bail
    if( STILL_ACTIVE == dwThreadExitCode )
    {
        float fXPos = MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( MENU_CERT_PROCESSING_TEXT ) / 2.0f );
        float fYPos = MENUBOX_TEXTAREA_Y_CENTER - GetFontHeightOfItem() - ( 2 * ITEM_VERT_SPACING );
        pTexture->DrawText( fXPos, fYPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls", MENU_CERT_PROCESSING_TEXT );

        fXPos = MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( g_pwszCertMenuItems[GetSelectedItem()] ) / 2.0f );
        fYPos = MENUBOX_TEXTAREA_Y_CENTER + ITEM_VERT_SPACING;
        pTexture->DrawText( fXPos, fYPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls", g_pwszCertMenuItems[GetSelectedItem()] );

        // We should display the file name in this case
        if( CERTMENU_ITEM_REQUIREMENT10_3_33 == GetSelectedItem() )
        {
            char* pszFileName = strrchr( FILE_DATA_FILELIST_FILENAME, '\\' );
            char pszDestFileName[100];
            pszDestFileName[0] = 0;

            _snprintf( pszDestFileName, 99, "%hs%hs", FILE_DEVELOPER_DRIVE_A, pszFileName );

            fXPos = MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( pszDestFileName ) / 2.0f );
            fYPos = fYPos + m_uiFontHeightOfItem + ITEM_VERT_SPACING;
            pTexture->DrawText( fXPos, fYPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%hs", pszDestFileName );
        }

        fYPos = MENUBOX_TEXTAREA_Y2 - m_uiFontHeightOfItem;
        if( m_bAborting )
        {
            fXPos = MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( "Aborting..." ) / 2.0f );
            pTexture->DrawText( fXPos, fYPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"Aborting..." );
        }
        else
        {
            // Draw our aborting message
            fXPos = MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( "Press 'B' to abort" ) / 2.0f );
            pTexture->DrawText( fXPos, fYPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"Press 'B' to abort" );
        }

        // Unlock the texture
        pTexture->Unlock();

        // Turn off our arrows
        g_bDisplayUpArrow = FALSE;
        g_bDisplayDownArrow = FALSE;

        return;
    }
    else
    {
        m_hThread = NULL;

        // Fix our arrows
        AdjustDisplayArrows();
    }

    // Let our base menu class draw all the items pulled from the MNU file
    CMenuScreen::Action( pTexture );

    // Draw a box behind the currently highlighted choice
    float X1Pos = MENUBOX_SELECTBAR_X1;
    float Y1Pos = MENUBOX_TEXTAREA_Y1 + MENUBOX_TEXT_TOP_BORDER + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * ( GetSelectedItem() - m_uiTopItemIndex ) ) - ITEM_SELECTOR_BORDER_SPACE;
    float X2Pos = MENUBOX_SELECTBAR_X2;
    float Y2Pos = Y1Pos + m_uiFontHeightOfItem + ( 2 * ITEM_SELECTOR_BORDER_SPACE );
    
    pTexture->DrawBox( X1Pos, Y1Pos, X2Pos, Y2Pos, ITEM_SELECTOR_COLOR );

    //
    // Draw our Menu Items
    //

    for( unsigned int x = m_uiTopItemIndex; x < ( MENU_CERT_NUM_ITEMS_TO_DISPLAY + m_uiTopItemIndex ); x++ )
    {
        if( x == GetNumItems() )
        {
            break;
        }

        float XPos = MENUBOX_TEXTAREA_X1;
        float YPos = MENUBOX_TEXTAREA_Y1 + MENUBOX_TEXT_TOP_BORDER + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * ( x - m_uiTopItemIndex ) );

        pTexture->DrawText( XPos, YPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", g_pwszCertMenuItems[x] );
    }

    // Correct our Font Height
    m_uiFontHeightOfItem = FONT_DEFAULT_HEIGHT;
    XFONT_SetTextHeight( g_pFont, FONT_DEFAULT_HEIGHT );

    // Unlock the texture
    pTexture->Unlock();
}


// Handle any requests for the joystick (thumb-pad)
void CCertMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
void CCertMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
    // Check to see if input is enabled or disabled
    if( m_bDisableInput )
    {
        return;
    }

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
                    if( GetSelectedItem() >= (int)( MENU_CERT_NUM_ITEMS_TO_DISPLAY + m_uiTopItemIndex ) )
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
void CCertMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // If it's a repeat press, or input is disabled, let's bail
    if( !bFirstPress )
    {
        return;
    }

    if( ( m_bDisableInput ) && ( buttonPressed != BUTTON_B ) )
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
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                m_bUpdateTexture = TRUE;

                switch( GetSelectedItem() )
                {
                case CERTMENU_ITEM_REQUIREMENT10_1_12:  // Display all settings to the user
                    {
                        // Display all of the settings to the user
                        Leave( &m_EEPROMMenu );

                        break;
                    }
                case CERTMENU_ITEM_REQUIREMENT10_2_07:  // Create 4096 udata\<titleid> entries
                    {
                        m_bStartThread = TRUE;

                        // Get the TITLE ID from the Hex Keypad
                        Leave( &m_HexKeypadMenu );

                        break;
                    }
                case CERTMENU_ITEM_REQUIREMENT10_2_10:  // Clear utility partition for <titleid>
                    {
                        m_bStartThread = TRUE;

                        // Get the Title ID from the Cache Partition Info
                        Leave( &m_DumpCacheMenu );

                        break;
                    }
                case CERTMENU_ITEM_REQUIREMENT10_2_18:  // Toggle non-HDTV safe area box
                    {
                        m_bDispNonHDTVSafe = !m_bDispNonHDTVSafe;

                        break;
                    }
                case CERTMENU_ITEM_REQUIREMENT10_2_19:  // Toggle HDTV safe area box
                    {
                        m_bDispHDTVSafe = !m_bDispHDTVSafe;

                        break;
                    }
                case CERTMENU_ITEM_REQUIREMENT10_3_11:  // Display all Memory Areas to the user
                    {
                        // Display all of the settings to the user
                        g_MADisplayMenu.SetParent( this );
                        Leave( &g_MADisplayMenu );

                        break;
                    }
                case CERTMENU_ITEM_REQUIREMENT10_3_14:  // Change the language
                    {
                        // Let the user specify a language
                        Leave( &m_LanguageMenu );

                        break;
                    }
                case CERTMENU_ITEM_REQUIREMENT10_3_15:  // Change the TimeZone
                    {
                        Leave( &m_TimeZoneMenu );

                        break;
                    }
                case CERTMENU_ITEM_REQUIREMENT10_3_26_4:  // Create Saved Game name with Loc Text
                    {
                        m_bStartThread = TRUE;

                        // Get the TITLE ID from the Hex Keypad
                        Leave( &m_HexKeypadMenu );

                        break;
                    }
                case CERTMENU_ITEM_REQUIREMENT10_3_04:
                    {
                        m_bStartThread = TRUE;

                        // Clean the Udata directory
                        // m_CertReqs.CleanDir( "X:", ".xdk" );

                        // Get the TITLE ID from the Hex Keypad
                        WCHAR pwszPrompt[100];

                        ZeroMemory( m_pwszTitleID, sizeof( WCHAR ) * MENU_CERT_TITLE_ID_STR_LEN+1 );
                        ZeroMemory( pwszPrompt, sizeof( WCHAR ) * 100 );

                        _snwprintf( pwszPrompt, 99, L"Enter # of blocks to fill (%d free):", g_XboxHardDrive.GetFreeBlocks() );
                        m_HexKeypadMenu.SetInfo( pwszPrompt, m_pwszTitleID, MENU_CERT_TITLE_ID_STR_LEN );
                        Leave( &m_HexKeypadMenu );

                        break;
                    }
                case CERTMENU_ITEM_REQUIREMENT10_6_05:  // Corrupt Saved Games for a particular Title ID
                    {
                        m_bStartThread = TRUE;

                        // Get the TITLE ID from the Hex Keypad
                        Leave( &m_HexKeypadMenu );

                        break;
                    }
                default:
                    {
                        m_bStartThread = TRUE;
                    }
                }
            }

            break;
        }
    case BUTTON_B:
        {
            if( m_bDisableInput )
            {
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                m_bUpdateTexture = TRUE;
                m_bAborting = TRUE;
                g_XboxHardDrive.SetAbortProcess( TRUE );
            }
            else
            {
                // Go Back one menu
                if( GetParent() )
                {
                    // Play the Back Sound
                    g_Sounds.PlayBackSound();

                    Leave( GetParent() );
                }
            }

            break;
        }
    case BUTTON_Y:
        {
            break;
        }
    }
}


// Initialize the Menu
HRESULT CCertMenu::Init( char* menuFileName )
{
    XDBGTRC( APP_TITLE_NAME_A, "CCertMenu::Init()" );

    HRESULT hr = CMenuScreen::Init( menuFileName );

    // Initialize our sub-menus
    m_DumpCacheMenu.Init( MENU_DUMPCACHE_MENU_FILENAME_A );
    m_DumpCacheMenu.SetParent( this );

    m_LanguageMenu.Init( MENU_LANGUAGE_MENU_FILENAME_A );
    m_LanguageMenu.SetParent( this );

    m_TimeZoneMenu.Init( TIMEZONEMENU_FILENAME_A );
    m_TimeZoneMenu.SetParent( this );

    m_EEPROMMenu.Init( EEPROMMENU_FILENAME_A );
    m_EEPROMMenu.SetParent( this );

    return hr;
}


// Should be called whenever a user leaves the menu
void CCertMenu::Leave( CXItem* pItem )
{
    // Call our base class Leave
    CMenuScreen::Leave( pItem );

    g_bDisplayUpArrow = FALSE;
    g_bDisplayDownArrow = FALSE;
}

// This will be called whenever the user enters this menu
void CCertMenu::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();

    // Make sure our input is enabled
    m_bDisableInput = FALSE;

    // Set our selected item to the top of the menu
    // SetSelectedItem( 0 );

    // Make sure the proper arrows are displayed
    AdjustDisplayArrows();

    // Setup our Hex Keypad Menu
    m_HexKeypadMenu.SetParent( this );
    m_HexKeypadMenu.SetInfo( L"Enter an 8 digit Title ID:", m_pwszTitleID, MENU_CERT_TITLE_ID_STR_LEN );
}


// Adjust the UP and DOWN arrows on the screen
void CCertMenu::AdjustDisplayArrows()
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
    if( ( m_uiTopItemIndex + MENU_CERT_NUM_ITEMS_TO_DISPLAY ) == GetNumItems() )
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


// Thread proc
DWORD WINAPI ProcessDataThreadFunc( LPVOID lpParameter )
{
    CCertMenu* pCertMenu = (CCertMenu*)lpParameter;
    DWORD dwReturnVal = 0;

    switch( pCertMenu->GetSelectedItem() )
    {
    case CERTMENU_ITEM_REQUIREMENT10_1_11:
        {
            if( FAILED( pCertMenu->m_CertReqs.Req10_1_11() ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!!" );

                dwReturnVal = 1;
            }

            break;
        }
    // Lists all global settings
    case CERTMENU_ITEM_REQUIREMENT10_1_12:
        {
            if( FAILED( pCertMenu->m_CertReqs.Req10_1_12() ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!!" );

                dwReturnVal = 1;
            }

            break;
        }

    // Fragments TDATA and UDATA partitions
    case CERTMENU_ITEM_REQUIREMENT10_2_01:
        {
            if( FAILED( pCertMenu->m_CertReqs.Req10_2_01() ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!! (udata)" );

                dwReturnVal = 1;
            }

            break;
        }
    case CERTMENU_ITEM_REQUIREMENT10_2_02:
        {
            if( FAILED( pCertMenu->m_CertReqs.Req10_2_02() ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!!" );

                dwReturnVal = 1;
            }

            break;
        }
    // Fills udata\<titleid> with 4096 entries
    case CERTMENU_ITEM_REQUIREMENT10_2_07:
        {
            if( wcslen( pCertMenu->m_pwszTitleID ) != 8 )
            {
                // TODO: Display an error message
                return 2;
            }

            char pszTitleID[9];
            ZeroMemory( pszTitleID, 9 );

            sprintf( pszTitleID, "%ls", pCertMenu->m_pwszTitleID );

            if( FAILED( pCertMenu->m_CertReqs.Req10_2_07( pszTitleID ) ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!!" );

                dwReturnVal = 1;
            }

            break;
        }
    case CERTMENU_ITEM_REQUIREMENT10_2_10:
        {
            if( FAILED( pCertMenu->m_CertReqs.Req10_2_10( pCertMenu->m_DumpCacheMenu.GetSelectedTitleID() ) ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!!" );

                dwReturnVal = 1;
            }

            break;
        }
    case CERTMENU_ITEM_REQUIREMENT10_2_18:
        {
            if( FAILED( pCertMenu->m_CertReqs.Req10_2_18() ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!!" );

                dwReturnVal = 1;
            }

            break;
        }
    case CERTMENU_ITEM_REQUIREMENT10_2_19:
        {
            if( FAILED( pCertMenu->m_CertReqs.Req10_2_19() ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!!" );

                dwReturnVal = 1;
            }

            break;
        }
    case CERTMENU_ITEM_REQUIREMENT10_3_04:
        {
            if( L'\0' == pCertMenu->m_pwszTitleID[0] )
            {
                return 0;
            }

            WCHAR* pwszStopString = NULL;
            DWORD dwFileSize = wcstol( pCertMenu->m_pwszTitleID, &pwszStopString, 10 );
            
            XDBGTRC( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():pCertMenu->m_pwszTitleID - '%ls'", pCertMenu->m_pwszTitleID );
            if( FAILED( pCertMenu->m_CertReqs.Req10_3_04( dwFileSize ) ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!!" );

                dwReturnVal = 1;
            }

            break;
        }
    case CERTMENU_ITEM_REQUIREMENT10_3_11:
        {
            if( FAILED( pCertMenu->m_CertReqs.Req10_3_11() ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!!" );

                dwReturnVal = 1;
            }

            break;
        }
    case CERTMENU_ITEM_REQUIREMENT10_3_14:
        {
            if( FAILED( pCertMenu->m_CertReqs.Req10_3_14() ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!!" );

                dwReturnVal = 1;
            }

            break;
        }
    case CERTMENU_ITEM_REQUIREMENT10_3_15:
        {
            if( FAILED( pCertMenu->m_CertReqs.Req10_3_15() ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!!" );

                dwReturnVal = 1;
            }

            break;
        }
    case CERTMENU_ITEM_REQUIREMENT10_3_26_1:    // Soundtracks with Loc Text
        {
            if( FAILED( pCertMenu->m_CertReqs.Req10_3_26_1() ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!!" );

                dwReturnVal = 1;
            }

            break;
        }
    case CERTMENU_ITEM_REQUIREMENT10_3_26_2:    // Nicknames with Loc Text
        {
            if( FAILED( pCertMenu->m_CertReqs.Req10_3_26_2() ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!!" );

                dwReturnVal = 1;
            }

            break;
        }
    case CERTMENU_ITEM_REQUIREMENT10_3_26_3:    // MU Names with Loc Text
        {
            if( FAILED( pCertMenu->m_CertReqs.Req10_3_26_3() ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!!" );

                dwReturnVal = 1;
            }

            break;
        }
    case CERTMENU_ITEM_REQUIREMENT10_3_26_4:    // Saved Game Names with Loc Text
        {
            if( wcslen( pCertMenu->m_pwszTitleID ) != 8 )
            {
                // TODO: Display an error message
                return 2;
            }

            char pszTitleID[9];
            ZeroMemory( pszTitleID, 9 );

            sprintf( pszTitleID, "%ls", pCertMenu->m_pwszTitleID );

            if( FAILED( pCertMenu->m_CertReqs.Req10_3_26_4( pszTitleID ) ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!!" );

                dwReturnVal = 1;
            }

            break;
        }
    case CERTMENU_ITEM_REQUIREMENT10_3_33:
        {
            if( FAILED( pCertMenu->m_CertReqs.Req10_3_33() ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!!" );

                dwReturnVal = 1;
            }

            break;
        }
    case CERTMENU_ITEM_REQUIREMENT10_6_05:
        {
            if( wcslen( pCertMenu->m_pwszTitleID ) != 8 )
            {
                // TODO: Display an error message
                return 2;
            }

            char pszTitleID[9];
            ZeroMemory( pszTitleID, 9 );

            sprintf( pszTitleID, "%ls", pCertMenu->m_pwszTitleID );

            if( FAILED( pCertMenu->m_CertReqs.Req10_6_05( pszTitleID ) ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!!" );

                dwReturnVal = 1;
            }

            break;
        }
    case CERTMENU_ITEM_CLEANUP:
        {
            if( FAILED( pCertMenu->m_CertReqs.CleanUp( NULL ) ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::ProcessDataThreadFunc():Failed to properly configure the Xbox!!" );

                dwReturnVal = 1;
            }

            break;
        }
    }

    return dwReturnVal;
}


// Draw a box on the screen of the specified color
void CCertMenu::DrawBox( float x1, float y1, float x2, float y2, DWORD dwColor )
{
    struct MYVERTEX box[5];

    box[0].v.x = x1; box[0].v.y = y2; box[0].v.z = 0.000001f; box[0].fRHW = 1.0f/box[0].v.z; box[0].cDiffuse = dwColor;
    box[1].v.x = x1; box[1].v.y = y1; box[1].v.z = 0.000001f; box[1].fRHW = 1.0f/box[1].v.z; box[1].cDiffuse = dwColor;
    box[2].v.x = x2; box[2].v.y = y1; box[2].v.z = 0.000001f; box[2].fRHW = 1.0f/box[2].v.z; box[2].cDiffuse = dwColor;
    box[3].v.x = x2; box[3].v.y = y2; box[3].v.z = 0.000001f; box[3].fRHW = 1.0f/box[3].v.z; box[3].cDiffuse = dwColor;
    box[4].v.x = x1; box[4].v.y = y2; box[4].v.z = 0.000001f; box[4].fRHW = 1.0f/box[4].v.z; box[4].cDiffuse = dwColor;

    g_pD3DDevice->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
    g_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    g_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    g_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
    g_pD3DDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, box, sizeof( struct MYVERTEX ) );
    g_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    g_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
}


// Draw a box on the screen based on the percentage passed in
// The box will be centered on the screen
void CCertMenu::DrawBoxPercentBased( float fPercentage, DWORD dwColor )
{
    // Calculate the pixel width / height based on the resolution
    float fScreenWidth = (float)::GetScreenWidth();
    float fScreenHeight = (float)::GetScreenHeight();

    float fBoxWidth = ( fScreenWidth * fPercentage ) / 100.0f;
    float fBoxHeight = ( fScreenHeight * fPercentage ) / 100.0f;

    float x1 = ( fScreenWidth - fBoxWidth ) / 2.0f;
    float x2 = fScreenWidth - x1;
    float y1 = ( fScreenHeight - fBoxHeight ) / 2.0f;
    float y2 = fScreenHeight - y1;

    DrawBox( x1, y1, x2, y2, dwColor );
}
