/*****************************************************
*** muoptions.cpp
***
*** CPP file for our Memory Unit Options menu class.
*** This menu class will allow you to perform various
*** operations on an MU.  (Format, etc)
***
*** by James N. Helm
*** March 29th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "muoptions.h"

extern void Render();                   // Render the video on to our screen and present it to the user
extern CKeypadMenu   g_MenuKeypad;      // Virtual Keypad Menu to be used by all screens
extern CKeyboardMenu g_MenuKeyboard;    // Virtual Keyboard Menu to be used by all screens
extern CHardDrive    g_HardDrive;       // Xbox Hard Drive

// Constructors
CMUOptions::CMUOptions( CXItem* pParent )
: CMenuScreen( pParent ),
m_pMemoryUnit( NULL ), 
m_bActionInitiated( FALSE ),
m_uiActionStage( 0 ),
m_dwNumTitles( 0 ),
m_dwNumSavedGames( 0 ),
m_dwSavedGameSize( 0 ),
m_dwFileSize( 0 ),
m_hThreadHandle( NULL ),
m_pwszValueBuffer( NULL ),
m_uiTopItemIndex( 0 ),
m_MenuGameTitles( NULL ),
m_MenuSaveGames( NULL )
{
    XDBGWRN( APP_TITLE_NAME_A, "CMUOptions::CMUOptions()" );

    m_uiNumItems = MUOPTIONSMENU_ITEM_NUM_ITEMS;
    SetSelectedItem( 0 );

    m_pwszValueBuffer = new WCHAR[MENU_MUOPTIONS_SAVEGAMENAME_SIZE];
    if( !m_pwszValueBuffer )
    {
        XDBGERR( APP_TITLE_NAME_A, "CMUOptions::CMUOptions():Failed to allocate memory!!" );
    }
    ZeroMemory( m_pwszValueBuffer, sizeof( WCHAR ) * MENU_MUOPTIONS_SAVEGAMENAME_SIZE );

    m_pwszSavedGameName = new WCHAR[MENU_MUOPTIONS_SAVEGAMENAME_SIZE];
    if( !m_pwszSavedGameName )
    {
        XDBGERR( APP_TITLE_NAME_A, "CMUOptions::CMUOptions():Failed to allocate memory!!" );
    }
    ZeroMemory( m_pwszSavedGameName, sizeof( WCHAR ) * MENU_MUOPTIONS_SAVEGAMENAME_SIZE );
}


// Destructor
CMUOptions::~CMUOptions()
{
    if( m_pwszValueBuffer )
    {
        delete[] m_pwszValueBuffer;
        m_pwszValueBuffer = NULL;
    }
}


// Draws a menu on to the screen
void CMUOptions::Action( CUDTexture* pTexture )
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
    if( m_pMemoryUnit->IsFormatted() )
    {
        // Draw a selector around the current item
        float X1Pos = MENUBOX_SELECTOR_X1;
        float Y1Pos = MENUBOX_MAINAREA_Y1 + MENUBOX_MAINAREA_BORDERWIDTH_TOP + ( ( MENUBOX_ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * ( GetSelectedItem() - m_uiTopItemIndex ) ) - MENUBOX_SELECTOR_BORDER_WIDTH;
        float X2Pos = MENUBOX_SELECTOR_X2;
        float Y2Pos = Y1Pos + m_uiFontHeightOfItem + ( 2 * MENUBOX_SELECTOR_BORDER_WIDTH );

        pTexture->DrawBox( X1Pos, Y1Pos, X2Pos, Y2Pos, SCREEN_DEFAULT_SELECTOR_COLOR );

        // Display the space information about the MU
        pTexture->DrawText( (float)MENUBOX_WINDOW_TITLE_X1 + GetStringPixelWidth( L"MU Options " ), MENUBOX_WINDOW_TITLE_Y1, SCREEN_DEFAULT_FOREGROUND_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"(%d/%d)", m_pMemoryUnit->GetFreeBlocks(), m_pMemoryUnit->GetTotalBlocks() );

        dwItemColor = SCREEN_DEFAULT_FOREGROUND_COLOR;
    }
    else
    //
    // The MU is not formatted
    //
    {
        // Display the unformatted message
        pTexture->DrawText( (float)MENUBOX_WINDOW_TITLE_X1 + GetStringPixelWidth( L"MU Options " ), MENUBOX_WINDOW_TITLE_Y1, SCREEN_DEFAULT_FOREGROUND_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"(unformatted)" );

        dwItemColor = SCREEN_INACTIVE_OPTION_COLOR;
    }

    // Draw the Menu Options
    for( unsigned int x = m_uiTopItemIndex; x < ( MUOPTIONS_MENU_NUM_ITEMS_TO_DISPLAY + m_uiTopItemIndex ); ++x )
    {
        // Make sure we don't try to access out of range items
        if( x == GetNumItems() )
        {
            break;
        }

        float XPos = MENUBOX_MAINAREA_X1 + MENUBOX_MAINAREA_BORDERWIDTH_LEFT;
        float YPos = MENUBOX_MAINAREA_Y1 + MENUBOX_MAINAREA_BORDERWIDTH_TOP + ( ( MENUBOX_ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * ( x - m_uiTopItemIndex ) );

        pTexture->DrawText( XPos, YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", g_wpszMUOptionsMenuItems[x] );
    }

    // Draw our processing message if necessary
    if( m_hThreadHandle )
    {
#define TEXT_BOX_BUFFER_SIZE    20

        pTexture->DrawBox( MENUBOX_MAINAREA_X1 + TEXT_BOX_BUFFER_SIZE, MENUBOX_MAINAREA_Y1 + TEXT_BOX_BUFFER_SIZE, MENUBOX_MAINAREA_X2 - TEXT_BOX_BUFFER_SIZE, MENUBOX_MAINAREA_Y2 - TEXT_BOX_BUFFER_SIZE, COLOR_BLACK );
        pTexture->DrawOutline( MENUBOX_MAINAREA_X1 + TEXT_BOX_BUFFER_SIZE, MENUBOX_MAINAREA_Y1 + TEXT_BOX_BUFFER_SIZE, MENUBOX_MAINAREA_X2 - TEXT_BOX_BUFFER_SIZE, MENUBOX_MAINAREA_Y2 - TEXT_BOX_BUFFER_SIZE, 2, COLOR_DARK_GREEN );
        pTexture->DrawText( SCREEN_X_CENTER - ( GetStringPixelWidth( L"Processing..." ) / 2 ), SCREEN_Y_CENTER, SCREEN_DEFAULT_FOREGROUND_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"Processing..." );

        // Unlock our texture and restore our render target
        pTexture->Unlock();

        Render();

        // Start our thread on it's merry way
        ResumeThread( m_hThreadHandle );

        // Wait for the thread to exit
        WaitForSingleObject( m_hThreadHandle, INFINITE );

        CloseHandle( m_hThreadHandle );
        m_hThreadHandle = NULL;

        m_bUpdateTexture = TRUE;

        return;
    }

    // Unlock our texture and restore our render target
    pTexture->Unlock();

    // Take Action if necessary
    if( m_bActionInitiated )
    {
        m_bActionInitiated = FALSE;

        switch( GetSelectedItem() )
        {
        case MUOPTIONSMENU_ITEM_CREATE_TITLES_AND_SGAMES:
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
                    g_MenuKeypad.SetInfo( L"Enter # of Saved Games", m_pwszValueBuffer, MENU_MUOPTIONS_NUMBER_SIZE );
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
        case MUOPTIONSMENU_ITEM_CREATE_SAVED_GAME:
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
        case MUOPTIONSMENU_ITEM_SET_DATETIME_FOR_SAVED_GAME:
            {
                switch( m_uiActionStage )
                {
                case 1:
                    {
                        m_pMemoryUnit->ProcessGameTitles();
                        m_MenuGameTitles.SetInfo( m_pMemoryUnit->GetGameTitlePtr() );

                        Leave( &m_MenuGameTitles );

                        m_bActionInitiated = TRUE;
                        m_uiActionStage = 2;

                        break;
                    }
                case 2:
                    {
                        // TODO: See if the user canceled the previous menu

                        XDBGWRN( APP_TITLE_NAME_A, "CMUOptions::Action():Selected Game Title   - '%u'", m_MenuGameTitles.GetSelectedItem() );

                        for( unsigned int x = 0; x < m_pMemoryUnit->GetGameTitlePtr()->GetNumGames(); x++ )
                        {
                            XDBGWRN( APP_TITLE_NAME_A, "CMUOptions::Action():Number of Saved Games - '%u'", m_pMemoryUnit->GetGameTitlePtr()->GetGame( x )->GetNumSavedGames() );
                        }
                        
                        m_MenuSaveGames.SetInfo( m_pMemoryUnit->GetGameTitlePtr()->GetGame( m_MenuGameTitles.GetSelectedItem() ) );
                        Leave( &m_MenuSaveGames );
                        
                        break;
                    }
                }

                break;
            }
        case MUOPTIONSMENU_ITEM_CREATE_FILE:
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
        case MUOPTIONSMENU_ITEM_ERASE:
        case MUOPTIONSMENU_ITEM_CREATEMU_A:
        case MUOPTIONSMENU_ITEM_CREATEMU_B:
        case MUOPTIONSMENU_ITEM_CREATEMU_C:
        case MUOPTIONSMENU_ITEM_CREATEMU_D:
        case MUOPTIONSMENU_ITEM_CREATEMU_E:
        case MUOPTIONSMENU_ITEM_CREATEMU_F:
        case MUOPTIONSMENU_ITEM_CREATEMU_G:
        case MUOPTIONSMENU_ITEM_CREATEMU_H:
        case MUOPTIONSMENU_ITEM_CREATEMU_I:
        case MUOPTIONSMENU_ITEM_CREATEMU_J:
        case MUOPTIONSMENU_ITEM_CREATEMU_K:
        case MUOPTIONSMENU_ITEM_CREATEMU_L:
        case MUOPTIONSMENU_ITEM_CREATEMU_M:
        case MUOPTIONSMENU_ITEM_CREATEMU_N:
        case MUOPTIONSMENU_ITEM_CREATEMU_O:
        case MUOPTIONSMENU_ITEM_CREATEMU_P:
        case MUOPTIONSMENU_ITEM_CREATEMU_Q:
        case MUOPTIONSMENU_ITEM_CREATEMU_R:
        case MUOPTIONSMENU_ITEM_CREATEMU_S:
        case MUOPTIONSMENU_ITEM_CREATEMU_T:
        case MUOPTIONSMENU_ITEM_CREATEMU_U:
        case MUOPTIONSMENU_ITEM_CREATEMU_V:
        case MUOPTIONSMENU_ITEM_CREATEMU_W:
        case MUOPTIONSMENU_ITEM_CREATEMU_X:
        case MUOPTIONSMENU_ITEM_CREATEMU_Y:
        case MUOPTIONSMENU_ITEM_CREATEMU_Z:
        case MUOPTIONSMENU_ITEM_CREATEMU_AA:
		case MUOPTIONSMENU_ITEM_CREATEMU_BB:
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
void CMUOptions::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
void CMUOptions::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
    // Make sure the system is not active
    if( ( m_hThreadHandle ) || ( !m_pMemoryUnit->IsFormatted() ) )
    {
        return;
    }

    // If the memory unit isn't formatted, don't accept any navigation input
    if( !m_pMemoryUnit->IsFormatted() )
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

                // Make sure we are displaying the correct items
                if( GetSelectedItem() < (int)m_uiTopItemIndex )
                {
                    m_uiTopItemIndex = GetSelectedItem();

                    // AdjustDisplayArrows();
                }
            }
            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            if( GetSelectedItem() < ( MUOPTIONSMENU_ITEM_NUM_ITEMS - 1 ) )
            {
                m_bUpdateTexture = TRUE;

                SetSelectedItem( GetSelectedItem() + 1 );

                // Make sure we are displaying the correct items
                if( GetSelectedItem() >= (int)( MUOPTIONS_MENU_NUM_ITEMS_TO_DISPLAY + m_uiTopItemIndex ) )
                {
                    m_uiTopItemIndex += 1;

                    // AdjustDisplayArrows();
                }
            }
            break;
        }
    }
}


// Handles input (of the BUTTONS) for the current menu
void CMUOptions::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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
            if( !m_pMemoryUnit->IsFormatted() )
            {
                // If the memory unit isn't formatted, don't accept "A" button input

                return;
            }

            // Refresh our Texture
            m_bUpdateTexture = TRUE;

            switch( GetSelectedItem() )
            {
            case MUOPTIONSMENU_ITEM_CREATE_TITLES_AND_SGAMES:
                {
                    // Prompt the user for the number of Game Titles
                    g_MenuKeypad.SetInfo( L"Enter # of Game Titles", m_pwszValueBuffer, MENU_MUOPTIONS_NUMBER_SIZE );
                    g_MenuKeypad.SetParent( this );
                    m_bActionInitiated = TRUE;

                    Leave( &g_MenuKeypad );
                    break;
                }
            case MUOPTIONSMENU_ITEM_CREATE_SAVED_GAME:
                {
                    g_MenuKeyboard.SetInfo( L"Enter the name of the Saved Game:", m_pwszValueBuffer, MENU_MUOPTIONS_SAVEGAMENAME_SIZE );
                    g_MenuKeyboard.SetParent( this );
                    m_bActionInitiated = TRUE;

                    Leave( &g_MenuKeyboard );
                    break;
                }
            case MUOPTIONSMENU_ITEM_SET_DATETIME_FOR_SAVED_GAME:
                {
                    m_bActionInitiated = TRUE;
                    m_uiActionStage = 1;

                    break;
                }
            case MUOPTIONSMENU_ITEM_CREATE_FILE:
                {
                    // Prompt the user for the size of the file
                    WCHAR pwszMUInfo[100];
                    ZeroMemory( pwszMUInfo, sizeof( WCHAR) * 100 );

                    swprintf( pwszMUInfo, L"(%d) blocks free; Enter Size of File (in blocks):", m_pMemoryUnit->GetFreeBlocks() );

                    g_MenuKeypad.SetInfo( pwszMUInfo, m_pwszValueBuffer, MENU_MUOPTIONS_NUMBER_SIZE );
                    g_MenuKeypad.SetParent( this );
                    m_bActionInitiated = TRUE;

                    Leave( &g_MenuKeypad );
                    break;
                }
            case MUOPTIONSMENU_ITEM_ERASE:
            case MUOPTIONSMENU_ITEM_CREATEMU_A:
            case MUOPTIONSMENU_ITEM_CREATEMU_B:
            case MUOPTIONSMENU_ITEM_CREATEMU_C:
            case MUOPTIONSMENU_ITEM_CREATEMU_D:
            case MUOPTIONSMENU_ITEM_CREATEMU_E:
            case MUOPTIONSMENU_ITEM_CREATEMU_F:
            case MUOPTIONSMENU_ITEM_CREATEMU_G:
            case MUOPTIONSMENU_ITEM_CREATEMU_H:
            case MUOPTIONSMENU_ITEM_CREATEMU_I:
            case MUOPTIONSMENU_ITEM_CREATEMU_J:
            case MUOPTIONSMENU_ITEM_CREATEMU_K:
            case MUOPTIONSMENU_ITEM_CREATEMU_L:
            case MUOPTIONSMENU_ITEM_CREATEMU_M:
            case MUOPTIONSMENU_ITEM_CREATEMU_N:
            case MUOPTIONSMENU_ITEM_CREATEMU_O:
            case MUOPTIONSMENU_ITEM_CREATEMU_P:
            case MUOPTIONSMENU_ITEM_CREATEMU_Q:
            case MUOPTIONSMENU_ITEM_CREATEMU_R:
            case MUOPTIONSMENU_ITEM_CREATEMU_S:
            case MUOPTIONSMENU_ITEM_CREATEMU_T:
            case MUOPTIONSMENU_ITEM_CREATEMU_U:
            case MUOPTIONSMENU_ITEM_CREATEMU_V:
            case MUOPTIONSMENU_ITEM_CREATEMU_W:
            case MUOPTIONSMENU_ITEM_CREATEMU_X:
            case MUOPTIONSMENU_ITEM_CREATEMU_Y:
            case MUOPTIONSMENU_ITEM_CREATEMU_Z:
            case MUOPTIONSMENU_ITEM_CREATEMU_AA:
			case MUOPTIONSMENU_ITEM_CREATEMU_BB:
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
            // Refresh our Texture
            m_bUpdateTexture = TRUE;

            // Unformat the selected MU
            m_pMemoryUnit->Unformat();
            break;
        }
    case BUTTON_Y:
        {
            // Refresh our Texture
            m_bUpdateTexture = TRUE;

            // Unformat the selected MU
            m_pMemoryUnit->Format( TRUE );
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
HRESULT CMUOptions::Init( IDirect3DDevice8* pD3DDevice, char* menuFileName )
{
    XDBGWRN( APP_TITLE_NAME_A, "CMUOptions::Init()" );

    CMenuScreen::Init( pD3DDevice, menuFileName );

    m_MenuGameTitles.Init( pD3DDevice, MENU_GAMETITLE_MENU_FILENAME_A );
    m_MenuGameTitles.SetParent( this );

    m_MenuSaveGames.Init( pD3DDevice, MENU_SAVEGAME_MENU_FILENAME_A );
    m_MenuSaveGames.SetParent( this );

    return S_OK;
}


// Sets the local member to point at the proper memory unit
HRESULT CMUOptions::SetMemoryUnit( CMemoryUnit* pMemoryUnit )
{
    XDBGWRN( APP_TITLE_NAME_A, "CMUOptions::SetMemoryUnit()" );

    if( !pMemoryUnit )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CMUOptions::SetMemoryUnit():Invalid Argument passed in!!" );

        return E_INVALIDARG;
    }

    m_pMemoryUnit = pMemoryUnit;

    return S_OK;
}

DWORD WINAPI DataCreateThreadFunc( LPVOID lpParameter )
{
    CMUOptions* pMUOptions = (CMUOptions*)lpParameter;
    switch( pMUOptions->GetSelectedItem() )
    {
    case MUOPTIONSMENU_ITEM_CREATE_TITLES_AND_SGAMES:
        {
            // Create the Game Titles
            DWORD dwNumTries = 0;
            DWORD dwXModifier = 0;
            for( DWORD x = 0; x < pMUOptions->m_dwNumTitles; x++ )
            {
                char pszTitleID[9];
                pszTitleID[8] = '\0';
                sprintf( pszTitleID, "%0.8X", x + dwXModifier );

                while( FAILED( pMUOptions->m_pMemoryUnit->CreateGameTitle( pMUOptions->m_pMemoryUnit->GetUDataDrive(), pszTitleID ) ) )
                {
                    ++dwXModifier;
                    sprintf( pszTitleID, "%0.8X", x + dwXModifier );

                    if( ( ERROR_DISK_FULL == GetLastError() ) || ( dwNumTries++ == 100 ) )
                    {
                        break;
                    }
                }

                // Unmount the MU, and Mount it to our Title Drive
                pMUOptions->m_pMemoryUnit->Unmount();
                if( FAILED( pMUOptions->m_pMemoryUnit->MapUDataToTitleID( x + dwXModifier ) ) )
                {
                    XDBGWRN( APP_TITLE_NAME_A, "CMUOptions::DataCreateThreadFunc():Failed to MapUDataToTitleID!! ID - '%d', Error - '0x%0.8X (%d)'", x + dwXModifier, GetLastError(), GetLastError() );
                }
                else
                {
                    // Create the Saved Games
                    WCHAR pwszSavedGameName[50];
                    for( DWORD y = 0; y < pMUOptions->m_dwNumSavedGames; y++ )
                    {
                        _snwprintf( pwszSavedGameName, 49, L"Saved Game %02d%c", y, '\0' );

                        SYSTEMTIME sysTime;
                        ZeroMemory( &sysTime, sizeof( sysTime ) );

                        sysTime.wMonth = ( rand() % 12 ) + 1;
                        sysTime.wDay = ( rand() % 28 ) + 1;
                        sysTime.wYear = ( rand() % 99 ) + 2001;
                        sysTime.wHour = rand() % 24;
                        sysTime.wMinute = rand() % 60;
                        sysTime.wSecond = rand() % 60;

                        pMUOptions->m_pMemoryUnit->CreateSavedGame( pwszSavedGameName,
                                                                    NULL,
                                                                    0,
                                                                    &sysTime,
                                                                    "",
                                                                    FALSE );
                    }
                }

                // Restore the proper Memory Unit location
                pMUOptions->m_pMemoryUnit->Unmount();
                pMUOptions->m_pMemoryUnit->Mount();
            }

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATE_SAVED_GAME:
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

                while( FAILED( pMUOptions->m_pMemoryUnit->CreateGameTitle( pMUOptions->m_pMemoryUnit->GetUDataDrive(), pszTitleID ) ) )
                {
                    ++dwXModifier;
                    sprintf( pszTitleID, "%0.8X", x + dwXModifier );

                    if( ( ERROR_DISK_FULL == GetLastError() ) || ( dwNumTries++ == 100 ) )
                    {
                        break;
                    }
                }

                // Unmount the MU, and Mount it to our Title Drive
                pMUOptions->m_pMemoryUnit->Unmount();
                pMUOptions->m_pMemoryUnit->MapUDataToTitleID( pszTitleID );
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

            pMUOptions->m_pMemoryUnit->CreateSavedGame( pMUOptions->m_pwszValueBuffer,
                                                        pszGamePath,
                                                        MAX_PATH,
                                                        &sysTime,
                                                        "",
                                                        FALSE );

            // Create a file of a certain size within the Saved Game

            // If the user is settings the game to a block sized larger than possible, Max it out
            if( pMUOptions->m_dwSavedGameSize > pMUOptions->m_pMemoryUnit->GetFreeBlocks() )
            {
                pMUOptions->m_dwSavedGameSize = pMUOptions->m_pMemoryUnit->GetFreeBlocks();
            }

            strcat( pszGamePath, "\\" );

            pMUOptions->m_pMemoryUnit->FillArea( pszGamePath, pMUOptions->m_dwSavedGameSize );
            
            // Restore the proper Memory Unit location
            pMUOptions->m_pMemoryUnit->Unmount();
            pMUOptions->m_pMemoryUnit->Mount();

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATE_FILE:
        {
            // Create a file of the specified size
            char pszDirectory[4];
            ZeroMemory( pszDirectory, 4 );

            sprintf( pszDirectory, "%c:\\", pMUOptions->m_pMemoryUnit->GetUDataDrive() );
            pMUOptions->m_pMemoryUnit->FillArea( pszDirectory, pMUOptions->m_dwFileSize );

            break;
        }
    case MUOPTIONSMENU_ITEM_ERASE:
        {
            pMUOptions->m_pMemoryUnit->Erase();

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_A:
        {
            CreateMU_A( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_B:
        {
            CreateMU_B( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_C:
        {
            CreateMU_C( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_D:
        {
            CreateMU_D( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_E:
        {
            CreateMU_E( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_F:
        {
            CreateMU_F( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_G:
        {
            CreateMU_G( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_H:
        {
            CreateMU_H( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_I:
        {
            CreateMU_I( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_J:
        {
            CreateMU_J( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_K:
        {
            CreateMU_K( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_L:
        {
            CreateMU_L( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_M:
        {
            CreateMU_M( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_N:
        {
            CreateMU_N( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_O:
        {
            CreateMU_O( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_P:
        {
            CreateMU_P( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_Q:
        {
            CreateMU_Q( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_R:
        {
            CreateMU_R( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_S:
        {
            CreateMU_S( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_T:
        {
            CreateMU_T( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_U:
        {
            CreateMU_U( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_V:
        {
            CreateMU_V( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_W:
        {
            CreateMU_W( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_X:
        {
            CreateMU_X( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_Y:
        {
            CreateMU_Y( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_Z:
        {
            CreateMU_Z( pMUOptions->m_pMemoryUnit );

            break;
        }
    case MUOPTIONSMENU_ITEM_CREATEMU_AA:
        {
            CreateMU_AA( pMUOptions->m_pMemoryUnit );

            break;
        }
	case MUOPTIONSMENU_ITEM_CREATEMU_BB:
        {
            CreateMU_BB( pMUOptions->m_pMemoryUnit );

            break;
        }

    }

    // Set this to 0 (Important!!) so that other actions know when to execute
    pMUOptions->m_dwNumTitles = 0;
    pMUOptions->m_dwNumSavedGames = 0;
    pMUOptions->m_dwFileSize = 0;
    pMUOptions->m_pwszSavedGameName[0] = 0;
    pMUOptions->m_dwSavedGameSize = 0;

    pMUOptions->m_bUpdateTexture = TRUE;

    return 0;
}


// Should be called whenever a user leaves the menu
void CMUOptions::Leave( CXItem* pItem )
{
    // Call our base class leave
    CMenuScreen::Leave( pItem );
}

// This will be called whenever the user enters this menu
void CMUOptions::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();
}
