/*****************************************************
*** hdoptions.cpp
***
*** CPP file for our Hard Drive Options menu class.
*** This menu class will allow you to perform various
*** operations on the Xbox Hard Drive.
***
*** by James N. Helm
*** April 2nd, 2001
***
*****************************************************/

#include "stdafx.h"
#include "hdoptions.h"

extern CKeypadMenu   g_MenuKeypad;      // Virtual Keypad Menu to be used by all screens
extern CKeyboardMenu g_MenuKeyboard;    // Virtual Keyboard Menu to be used by all screens
extern CHardDrive    g_HardDrive;       // Xbox Hard Drive

// Constructors
CHDOptions::CHDOptions( CXItem* pParent )
: CMenuScreen( pParent ),
m_bActionInitiated( FALSE ),
m_dwNumTitles( 0 ),
m_dwNumSavedGames( 0 ),
m_dwSavedGameSize( 0 ),
m_dwFileSize( 0 ),
m_hThreadHandle( NULL ),
m_pwszValueBuffer( NULL )
{
    XDBGWRN( APP_TITLE_NAME_A, "CHDOptions::CHDOptions()" );

    m_uiNumItems = HDOPTIONSMENU_ITEM_NUM_ITEMS;
    SetSelectedItem( 0 );

    m_pwszValueBuffer = new WCHAR[MENU_HDOPTIONS_SAVEGAMENAME_SIZE];
    if( !m_pwszValueBuffer )
    {
        XDBGERR( APP_TITLE_NAME_A, "CHDOptions::CHDOptions():Failed to allocate memory!!" );
    }
    ZeroMemory( m_pwszValueBuffer, sizeof( WCHAR ) * MENU_HDOPTIONS_SAVEGAMENAME_SIZE );

    m_pwszSavedGameName = new WCHAR[MENU_HDOPTIONS_SAVEGAMENAME_SIZE];
    if( !m_pwszSavedGameName )
    {
        XDBGERR( APP_TITLE_NAME_A, "CHDOptions::CHDOptions():Failed to allocate memory!!" );
    }
    ZeroMemory( m_pwszSavedGameName, sizeof( WCHAR ) * MENU_HDOPTIONS_SAVEGAMENAME_SIZE );
}


// Destructor
CHDOptions::~CHDOptions()
{
    if( m_pwszValueBuffer )
    {
        delete[] m_pwszValueBuffer;
        m_pwszValueBuffer = NULL;
    }

    if( m_pwszSavedGameName )
    {
        delete[] m_pwszSavedGameName;
        m_pwszSavedGameName = NULL;
    }
}


// Draws a menu on to the screen
void CHDOptions::Action( CUDTexture* pTexture )
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
    float Y1Pos = MENUBOX_MAINAREA_Y1 + MENUBOX_MAINAREA_BORDERWIDTH_TOP + ( ( MENUBOX_ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * GetSelectedItem() ) - MENUBOX_SELECTOR_BORDER_WIDTH;
    float X2Pos = MENUBOX_SELECTOR_X2;
    float Y2Pos = Y1Pos + m_uiFontHeightOfItem + ( 2 * MENUBOX_SELECTOR_BORDER_WIDTH );

    pTexture->DrawBox( X1Pos, Y1Pos, X2Pos, Y2Pos, SCREEN_DEFAULT_SELECTOR_COLOR );

    // Display the space information about the Hard Drive
    pTexture->DrawText( (float)MENUBOX_WINDOW_TITLE_X1 + GetStringPixelWidth( L"HD Options " ), MENUBOX_WINDOW_TITLE_Y1, SCREEN_DEFAULT_FOREGROUND_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"(%d/%d)", g_HardDrive.GetFreeBlocks(), g_HardDrive.GetTotalBlocks() );

    // Draw the Menu Options
    for( unsigned int x = 0; x < HDOPTIONSMENU_ITEM_NUM_ITEMS; ++x )
    {
        float XPos = MENUBOX_MAINAREA_X1 + MENUBOX_MAINAREA_BORDERWIDTH_LEFT;
        float YPos = MENUBOX_MAINAREA_Y1 + MENUBOX_MAINAREA_BORDERWIDTH_TOP + ( ( MENUBOX_ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * x );

        pTexture->DrawText( XPos, YPos, SCREEN_DEFAULT_FOREGROUND_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", g_wpszHDOptionsMenuItems[x] );
    }

    // Draw our processing message if necessary
    if( m_hThreadHandle )
    {
#define TEXT_BOX_BUFFER_SIZE    20

        pTexture->DrawBox( MENUBOX_MAINAREA_X1 + TEXT_BOX_BUFFER_SIZE, MENUBOX_MAINAREA_Y1 + TEXT_BOX_BUFFER_SIZE, MENUBOX_MAINAREA_X2 - TEXT_BOX_BUFFER_SIZE, MENUBOX_MAINAREA_Y2 - TEXT_BOX_BUFFER_SIZE, COLOR_BLACK );
        pTexture->DrawOutline( MENUBOX_MAINAREA_X1 + TEXT_BOX_BUFFER_SIZE, MENUBOX_MAINAREA_Y1 + TEXT_BOX_BUFFER_SIZE, MENUBOX_MAINAREA_X2 - TEXT_BOX_BUFFER_SIZE, MENUBOX_MAINAREA_Y2 - TEXT_BOX_BUFFER_SIZE, 2, COLOR_DARK_GREEN );
        pTexture->DrawText( SCREEN_X_CENTER - ( GetStringPixelWidth( L"Processing..." ) / 2 ), SCREEN_Y_CENTER, SCREEN_DEFAULT_FOREGROUND_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"Processing..." );

        // Check to see if the thread is done
        DWORD dwExitCode = 0;
        GetExitCodeThread( m_hThreadHandle, &dwExitCode );

        if( STILL_ACTIVE != dwExitCode )
        {
            XDBGWRN( APP_TITLE_NAME_A, "CHDOptions::Action():Thread Exit Code - '%d'", dwExitCode );

            // Our thread has stopped, close it, and reset the handle
            CloseHandle( m_hThreadHandle );
            m_hThreadHandle = NULL;
        }

        m_bUpdateTexture = TRUE;

        // Start our thread on it's merry way
        ResumeThread( m_hThreadHandle );
    }

    // Unlock our texture and restore our render target
    pTexture->Unlock();

    // Take Action if necessary
    if( m_bActionInitiated )
    {
        m_bActionInitiated = FALSE;

        switch( GetSelectedItem() )
        {
        case HDOPTIONSMENU_ITEM_UDATA_CREATE_TITLES:
            {
                // Check to see if the user set a value
                if( '\0' == m_pwszValueBuffer[0] )
                {
                    break;
                }
                
                WCHAR* pwszStopString = NULL;
                char pszGameTitle[9];
                ZeroMemory( pszGameTitle, 9 );

                // Convert the user value to a DWORD
                m_dwNumTitles = wcstol( m_pwszValueBuffer, &pwszStopString, 10 );
                m_dwNumSavedGames = 0;

                m_hThreadHandle = CreateThread( NULL,
                                                0,
                                                DataCreateThreadFunc,
                                                (LPVOID)this,
                                                CREATE_SUSPENDED,
                                                NULL );

                m_bUpdateTexture = TRUE;

                break;
            }
        case HDOPTIONSMENU_ITEM_UDATA_CREATE_TITLES_AND_SGAMES:
            {
                // Check to see if the user set a value
                if( '\0' == m_pwszValueBuffer[0] )
                {
                    // Set this to 0 (Important!!) so that other actions know when to execute
                    m_dwNumTitles = 0;

                    break;
                }

                WCHAR* pwszStopString = NULL;

                // Check to see if we are in the middle of this action
                // If not, store the number of titles.  If so, store the number of
                // Saved Games
                if( 0 == m_dwNumTitles )
                //
                // Store the number of Game Titles to create
                //
                {
                    // Store the number of titles
                    m_dwNumTitles = wcstol( m_pwszValueBuffer, &pwszStopString, 10 );

                    // Mark that we are still in the middle of a transaction
                    m_bActionInitiated = TRUE;
                    m_bUpdateTexture = TRUE;

                    // Prompt the user for the number of Saved Games
                    g_MenuKeypad.SetInfo( L"Enter # of Saved Games", m_pwszValueBuffer, MENU_HDOPTIONS_NUMBER_SIZE );
                    g_MenuKeypad.SetParent( this );

                    Leave( &g_MenuKeypad );
                }
                else
                //
                // Store the number of saved games to create
                //
                {
                    m_dwNumSavedGames = wcstol( m_pwszValueBuffer, &pwszStopString, 10 );

                    // Create the data
                    m_hThreadHandle = CreateThread( NULL,
                                                    0,
                                                    DataCreateThreadFunc,
                                                    (LPVOID)this,
                                                    CREATE_SUSPENDED,
                                                    NULL );

                    m_bUpdateTexture = TRUE;
                }
                break;
            }
        case HDOPTIONSMENU_ITEM_UDATA_CREATE_SAVED_GAME:
            {
                // Check to see if the user set a value
                if( '\0' == m_pwszValueBuffer[0] )
                {
                    m_dwSavedGameSize = 0;
                    m_pwszSavedGameName[0] = 0;

                    break;
                }

                if( 0 == m_pwszSavedGameName[0] )
                {
                    // Store the value the user set
                    wcscpy( m_pwszSavedGameName, m_pwszValueBuffer );

                    // Mark that we are still in the middle of a transaction
                    m_bActionInitiated = TRUE;
                    m_bUpdateTexture = TRUE;

                    // Prompt the user for the Size of the Saved Game
                    g_MenuKeypad.SetInfo( L"Enter size of the Saved Game Fill File", m_pwszValueBuffer, MENU_HDOPTIONS_NUMBER_SIZE );
                    g_MenuKeypad.SetParent( this );

                    Leave( &g_MenuKeypad );
                }
                else
                {
                    WCHAR* pwszStopString = NULL;

                    // Convert the Game Size in to a useable number and launch our thread
                    m_dwSavedGameSize = wcstol( m_pwszValueBuffer, &pwszStopString, 10 );

                    // Create the data
                    m_hThreadHandle = CreateThread( NULL,
                                                    0,
                                                    DataCreateThreadFunc,
                                                    (LPVOID)this,
                                                    CREATE_SUSPENDED,
                                                    NULL );

                    m_bUpdateTexture = TRUE;
                }

                break;
            }
        case HDOPTIONSMENU_ITEM_CREATE_FILE:
            {
                // Check to see if the user set a value
                if( '\0' == m_pwszValueBuffer[0] )
                {
                    break;
                }

                WCHAR* pwszStopString = NULL;

                m_dwFileSize = wcstol( m_pwszValueBuffer, &pwszStopString, 10 );

                // Create the data
                m_hThreadHandle = CreateThread( NULL,
                                                0,
                                                DataCreateThreadFunc,
                                                (LPVOID)this,
                                                CREATE_SUSPENDED,
                                                NULL );

                m_bUpdateTexture = TRUE;

                break;
            }
        case HDOPTIONSMENU_ITEM_TDATA_ERASE:
        case HDOPTIONSMENU_ITEM_UDATA_ERASE:
        case HDOPTIONSMENU_ITEM_CONFIG_METAVARS:
        case HDOPTIONSMENU_ITEM_CONFIG_SORTORDER:
            {
                // Perform the action
                m_hThreadHandle = CreateThread( NULL,
                                                0,
                                                DataCreateThreadFunc,
                                                (LPVOID)this,
                                                CREATE_SUSPENDED,
                                                NULL );

                m_bUpdateTexture = TRUE;
                
                break;
            }

		case HDOPTIONSMENU_ITEM_CONFIG_SORTORDER_JAP:
			{
                // Perform the action
                m_hThreadHandle = CreateThread( NULL,
                                                0,
                                                DataCreateThreadFunc,
                                                (LPVOID)this,
                                                CREATE_SUSPENDED,
                                                NULL );

                m_bUpdateTexture = TRUE;
                
                break;
            }
        }
    }
}


// Handle any requests for the joystick (thumb-pad)
void CHDOptions::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
void CHDOptions::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
    // Make sure the system is not working
    if( m_hThreadHandle )
    {
        return;
    }

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
            }
            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            if( GetSelectedItem() < ( HDOPTIONSMENU_ITEM_NUM_ITEMS - 1 ) )
            {
                m_bUpdateTexture = TRUE;

                SetSelectedItem( GetSelectedItem() + 1 );
            }
            break;
        }
    }
}


// Handles input (of the BUTTONS) for the current menu
void CHDOptions::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // If it's a repeat press, let's bail
    if( ( !bFirstPress ) || m_hThreadHandle )
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
            case HDOPTIONSMENU_ITEM_UDATA_CREATE_TITLES:
                {
                    // Prompt the user for the number of Game Titles
                    g_MenuKeypad.SetInfo( L"Enter # of Game Titles", m_pwszValueBuffer, MENU_HDOPTIONS_NUMBER_SIZE );
                    g_MenuKeypad.SetParent( this );
                    m_bActionInitiated = TRUE;

                    Leave( &g_MenuKeypad );

                    break;
                }
            case HDOPTIONSMENU_ITEM_UDATA_CREATE_TITLES_AND_SGAMES:
                {
                    // Prompt the user for the number of Game Titles
                    g_MenuKeypad.SetInfo( L"Enter # of Game Titles", m_pwszValueBuffer, MENU_HDOPTIONS_NUMBER_SIZE );
                    g_MenuKeypad.SetParent( this );
                    m_bActionInitiated = TRUE;

                    Leave( &g_MenuKeypad );
                    break;
                }
            case HDOPTIONSMENU_ITEM_UDATA_CREATE_SAVED_GAME:
                {
                    g_MenuKeyboard.SetInfo( L"Enter the name of the Saved Game:", m_pwszValueBuffer, MENU_HDOPTIONS_SAVEGAMENAME_SIZE );
                    g_MenuKeyboard.SetParent( this );
                    m_bActionInitiated = TRUE;

                    Leave( &g_MenuKeyboard );
                    break;
                }
            case HDOPTIONSMENU_ITEM_CREATE_FILE:
                {
                    // Prompt the user for the size of the file
                    WCHAR pwszHDInfo[100];
                    ZeroMemory( pwszHDInfo, sizeof( WCHAR) * 100 );

                    swprintf( pwszHDInfo, L"(%d) blocks free; Enter Size of File (in blocks):", g_HardDrive.GetFreeBlocks() );

                    g_MenuKeypad.SetInfo( pwszHDInfo, m_pwszValueBuffer, MENU_HDOPTIONS_NUMBER_SIZE );
                    g_MenuKeypad.SetParent( this );
                    m_bActionInitiated = TRUE;

                    Leave( &g_MenuKeypad );
                    break;
                }
            case HDOPTIONSMENU_ITEM_TDATA_ERASE:
            case HDOPTIONSMENU_ITEM_UDATA_ERASE:
            case HDOPTIONSMENU_ITEM_CONFIG_METAVARS:
            case HDOPTIONSMENU_ITEM_CONFIG_SORTORDER:
                {
                    m_bActionInitiated = TRUE;

                    break;
                }
			case HDOPTIONSMENU_ITEM_CONFIG_SORTORDER_JAP:
                {
                    m_bActionInitiated = TRUE;

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
HRESULT CHDOptions::Init( IDirect3DDevice8* pD3DDevice, char* menuFileName )
{
    XDBGWRN( APP_TITLE_NAME_A, "CHDOptions::Init()" );

    CMenuScreen::Init( pD3DDevice, menuFileName );

    return S_OK;
}


DWORD WINAPI DataCreateThreadFunc( LPVOID lpParameter )
{
    CHDOptions* pHDOptions = (CHDOptions*)lpParameter;
    switch( pHDOptions->GetSelectedItem() )
    {
    case HDOPTIONSMENU_ITEM_UDATA_CREATE_TITLES:
    case HDOPTIONSMENU_ITEM_UDATA_CREATE_TITLES_AND_SGAMES:
        {
            // Create the Game Titles
            DWORD dwNumTries = 0;
            DWORD dwXModifier = 0;
            for( DWORD x = 0; x < pHDOptions->m_dwNumTitles; x++ )
            {
                char pszTitleID[9];
                pszTitleID[8] = '\0';
                sprintf( pszTitleID, "%0.8X", x + dwXModifier );

                while( FAILED( g_HardDrive.CreateGameTitle( g_HardDrive.GetUDataDrive(), pszTitleID ) ) )
                {
                    ++dwXModifier;
                    sprintf( pszTitleID, "%0.8X", x + dwXModifier );

                    if( ( ERROR_DISK_FULL == GetLastError() ) || ( dwNumTries++ == 100 ) )
                    {
                        break;
                    }
                }

                // Map the UData Partition to the proper directory
                if( FAILED ( g_HardDrive.MapUDataToTitleID( x + dwXModifier ) ) )
                {
                    XDBGWRN( APP_TITLE_NAME_A, "CHDOptions::DataCreateThreadFunc():Failed to mount the title ID!!" );
                }
                else
                {
                    // Create the Saved Games
                    WCHAR pwszSavedGameName[50];
                    for( DWORD y = 0; y < pHDOptions->m_dwNumSavedGames; y++ )
                    {
                        _snwprintf( pwszSavedGameName, 49, L"Saved Game #%02d%c", y, L'\0' );

                        SYSTEMTIME sysTime;
                        ZeroMemory( &sysTime, sizeof( sysTime ) );

                        sysTime.wMonth = ( rand() % 12 ) + 1;
                        sysTime.wDay = ( rand() % 28 ) + 1;
                        sysTime.wYear = ( rand() % 99 ) + 2001;
                        sysTime.wHour = rand() % 24;
                        sysTime.wMinute = rand() % 60;
                        sysTime.wSecond = rand() % 60;

                        g_HardDrive.CreateSavedGame( pwszSavedGameName,
                                                     NULL,
                                                     0,
                                                     &sysTime,
                                                     "",
                                                     FALSE );
                    }
                }
            }

            // Map the UData Partition back to the original location
            g_HardDrive.MapUDataToOrigLoc();
            // g_HardDrive.MapUDataToTitleID( FILE_DATA_CONFIG_TITLEID );

            break;
        }
    case HDOPTIONSMENU_ITEM_UDATA_CREATE_SAVED_GAME:
        {
            // Create the Game Title to hold our Game
            char pszTitleID[9];
            pszTitleID[8] = '\0';

            // Create the Game Titles
            DWORD dwNumTries = 0;
            DWORD dwXModifier = 0;
            for( DWORD x = 0x00005000; x < 0x00005001; x++ )
            {
                sprintf( pszTitleID, "%0.8X", x + dwXModifier );

                while( FAILED( g_HardDrive.CreateGameTitle( g_HardDrive.GetUDataDrive(), pszTitleID ) ) )
                {
                    ++dwXModifier;
                    sprintf( pszTitleID, "%0.8X", x + dwXModifier );

                    if( ( ERROR_DISK_FULL == GetLastError() ) || ( dwNumTries++ == 100 ) )
                    {
                        break;
                    }
                }

                // Map the UData Partition to the proper directory
                g_HardDrive.MapUDataToTitleID( x + dwXModifier );
            }

            char pszGamePath[MAX_PATH+1];
            pszGamePath[MAX_PATH] = 0;

            SYSTEMTIME sysTime;
            ZeroMemory( &sysTime, sizeof( sysTime ) );

            sysTime.wMonth = ( rand() % 12 ) + 1;
            sysTime.wDay = ( rand() % 28 ) + 1;
            sysTime.wYear = ( rand() % 99 ) + 2001;
            sysTime.wHour = rand() % 24;
            sysTime.wMinute = rand() % 60;
            sysTime.wSecond = rand() % 60;

            // Create the Saved Games
            g_HardDrive.CreateSavedGame( pHDOptions->m_pwszSavedGameName,
                                         pszGamePath,
                                         MAX_PATH,
                                         &sysTime,
                                         "",
                                         FALSE );

            // Create a file of a certain size within the Saved Game

            // If the user is settings the game to a block sized larger than possible, Max it out
            if( pHDOptions->m_dwSavedGameSize > g_HardDrive.GetFreeBlocks() )
            {
                pHDOptions->m_dwSavedGameSize = g_HardDrive.GetFreeBlocks();
            }

            strcat( pszGamePath, "\\" );

            g_HardDrive.FillArea( pszGamePath, pHDOptions->m_dwSavedGameSize );

            // Map the UData Partition back to the original location
            g_HardDrive.MapUDataToTitleID( FILE_DATA_CONFIG_TITLEID );

            break;
        }
    case HDOPTIONSMENU_ITEM_CREATE_FILE:
        {
            // Create a file of the specified size
            char pszDirectory[4];
            ZeroMemory( pszDirectory, 4 );

            sprintf( pszDirectory, "%c:\\", g_HardDrive.GetTDataDrive() );
            g_HardDrive.FillArea( pszDirectory, pHDOptions->m_dwFileSize );

            break;
        }
    case HDOPTIONSMENU_ITEM_TDATA_ERASE:
        {
            g_HardDrive.Erase( FILE_DATA_TDATA_DRIVE_LETTER_A );

            break;
        }
    case HDOPTIONSMENU_ITEM_UDATA_ERASE:
        {
            g_HardDrive.Erase();

            break;
        }
    case HDOPTIONSMENU_ITEM_CONFIG_METAVARS:
        {
            // Create the games
            CreateMetaVarGames( &g_HardDrive );

            break;
        }
    case HDOPTIONSMENU_ITEM_CONFIG_SORTORDER:
        {
            CreateSortOrderGames( &g_HardDrive, TRUE );

            break;
        }
	case HDOPTIONSMENU_ITEM_CONFIG_SORTORDER_JAP:
        {
            CreateSortOrderGames( &g_HardDrive, FALSE );

            break;
        }
    }

    // Set this to 0 (Important!!) so that other actions know when to execute
    pHDOptions->m_dwNumTitles = 0;
    pHDOptions->m_dwNumSavedGames = 0;
    pHDOptions->m_dwFileSize = 0;
    pHDOptions->m_pwszSavedGameName[0] = 0;
    pHDOptions->m_dwSavedGameSize = 0;

    return 0;
}
