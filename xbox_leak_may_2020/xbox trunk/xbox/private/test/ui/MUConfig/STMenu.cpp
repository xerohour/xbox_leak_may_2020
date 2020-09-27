/*****************************************************
*** stmenu.cpp
***
*** CPP file for our Soundtrack menu class.
*** This menu class will allow you to perform various
*** operations with Soundtracks.
***
*** by James N. Helm
*** April 3rd, 2001
***
*****************************************************/

#include "stdafx.h"
#include "stmenu.h"

extern CKeypadMenu   g_MenuKeypad;      // Virtual Keypad Menu to be used by all screens
extern CKeyboardMenu g_MenuKeyboard;    // Virtual Keyboard Menu to be used by all screens
extern CHardDrive    g_HardDrive;       // Xbox Hard Drive

// Constructors
CSTMenu::CSTMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_bActionInitiated( FALSE ),
m_dwNumSoundtracks( 0 ),
m_dwNumTracks( 0 ),
m_dwCurrentST( 0 ),
m_dwTrackLength( 0 ),
m_hThreadHandle( NULL ),
m_pwszValueBuffer( NULL )
{
    XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::CSTMenu()" );

    m_uiNumItems = STMENU_ITEM_NUM_ITEMS;
    SetSelectedItem( 0 );

    m_pwszValueBuffer = new WCHAR[MENU_ST_SOUNDTRACKNAME_SIZE];
    if( !m_pwszValueBuffer )
    {
        XDBGERR( APP_TITLE_NAME_A, "CSTMenu::CSTMenu():Failed to allocate memory!!" );
    }

	ZeroMemory( m_pwszTrackName, sizeof( WCHAR ) * MENU_ST_TRACKNAME_SIZE );

    InitializeCriticalSection( &m_CritSec );
}


// Destructor
CSTMenu::~CSTMenu()
{
    if( m_pwszValueBuffer )
    {
        delete[] m_pwszValueBuffer;
        m_pwszValueBuffer = NULL;
    }

    DeleteCriticalSection( &m_CritSec );
}


// Draws a menu on to the screen
void CSTMenu::Action( CUDTexture* pTexture )
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

    // Display the number of soundtracks available, but only if our thread is not active
    if( !m_hThreadHandle )
    {
        BeginUseDashSTFuncs();

        DWORD dwNumSTs = DashGetSoundtrackCount();

        pTexture->DrawText( (float)MENUBOX_WINDOW_TITLE_X1 + GetStringPixelWidth( L"Soundtracks " ), MENUBOX_WINDOW_TITLE_Y1, SCREEN_DEFAULT_FOREGROUND_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"(%d/100)", dwNumSTs );

        EndUseDashSTFuncs();
    }

    // Draw the Menu Options
    for( unsigned int x = 0; x < STMENU_ITEM_NUM_ITEMS; ++x )
    {
        float XPos = MENUBOX_MAINAREA_X1 + MENUBOX_MAINAREA_BORDERWIDTH_LEFT;
        float YPos = MENUBOX_MAINAREA_Y1 + MENUBOX_MAINAREA_BORDERWIDTH_TOP + ( ( MENUBOX_ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * x );

        DWORD dwItemColor = SCREEN_ACTIVE_OPTION_COLOR;

        // Check to see if the option is disabled
        if( ( 0 == m_dwCurrentST ) && ( STMENU_ITEM_ADD_NAMED_TRACK_TO_SOUNDTRACK == x ) )
        {
            dwItemColor = SCREEN_INACTIVE_OPTION_COLOR;
        }

        pTexture->DrawText( XPos, YPos, dwItemColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", g_wpszSTMenuItems[x] );
    }

    // Draw the current Soundtrack (if any)
    EnterCritSection();
    pTexture->DrawText( MENUBOX_WINDOW_FOOTER_X1, MENUBOX_WINDOW_FOOTER_Y1, SCREEN_DEFAULT_FOREGROUND_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"Current Soundtrack ID (0 if NONE): %d", m_dwCurrentST );
    LeaveCritSection();

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
            XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::Action():Thread Exit Code - '%d'", dwExitCode );

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
        case STMENU_ITEM_CREATE_SMALL_SOUNDTRACKS_AND_TRACKS:
        case STMENU_ITEM_CREATE_SOUNDTRACKS_AND_TRACKS:
            {
                // Check to see if the user set a value
                if( '\0' == m_pwszValueBuffer[0] )
                {
                    // Set this to 0 (Important!!) so that other actions know when to execute
                    m_dwNumSoundtracks = 0;
					m_dwTrackLength = 0;
					m_dwNumTracks = 0;

                    break;
                }

                WCHAR* pwszStopString = NULL;

				// Check to see what we've stored, and process the next item

                // Check to see if we are in the middle of this action
                // If not, store the number of Soundtracks.  If so, store the number of
                // Tracks
                if( 0 == m_dwNumSoundtracks )
                //
                // Store the number of Soundtracks to create, and prompt for
				// the number of tracks per soundtrack
                //
                {
                    // Store the number of Soundtracks
                    m_dwNumSoundtracks = wcstol( m_pwszValueBuffer, &pwszStopString, 10 );

                    // Mark that we are still in the middle of a transaction
                    m_bActionInitiated = TRUE;
                    m_bUpdateTexture = TRUE;

                    // Prompt the user for the number of Tracks
                    g_MenuKeypad.SetInfo( L"Enter # of Tracks", m_pwszValueBuffer, MENU_ST_NUMBER_SIZE );
                    g_MenuKeypad.SetParent( this );

                    Leave( &g_MenuKeypad );
                }
                else
				//
				// Check to see if we have already recieved the Number of Tracks from the user,
				// and if so, ask for the track length
				//
				{
					if( 0 == m_dwNumTracks )
					//
					// Store the number of Tracks the user would like, and
					// prompt them for the track length
					//
					{
						m_dwNumTracks = wcstol( m_pwszValueBuffer, &pwszStopString, 10 );

						// Create the data
						m_hThreadHandle = CreateThread( NULL,
														0,
														DataCreateThreadFunc,
														(LPVOID)this,
														CREATE_SUSPENDED,
														NULL );

						m_bUpdateTexture = TRUE;

                        /*
						// Mark that we are still in the middle of a transaction
						m_bActionInitiated = TRUE;
						m_bUpdateTexture = TRUE;

						// Prompt the user for the Track Length
						g_MenuKeypad.SetInfo( L"Enter the Length of the Tracks (seconds)", m_pwszValueBuffer, MENU_ST_NUMBER_SIZE );
						g_MenuKeypad.SetParent( this );

						Leave( &g_MenuKeypad );
                        */
					}
					else
					//
					// Store the Length of the tracks the user wishes to use
					//
					{
						m_dwTrackLength = wcstol( m_pwszValueBuffer, &pwszStopString, 10 );

						m_dwTrackLength *= 1000;	// Convert seconds in to milliseconds

						// Create the data
						m_hThreadHandle = CreateThread( NULL,
														0,
														DataCreateThreadFunc,
														(LPVOID)this,
														CREATE_SUSPENDED,
														NULL );

						m_bUpdateTexture = TRUE;
					}
				}

                break;
            }
        case STMENU_ITEM_CREATE_NAMED_SOUNDTRACK:
            {
                // Check to see if the user set a value
                if( '\0' == m_pwszValueBuffer[0] )
                {
                    break;
                }

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
        case STMENU_ITEM_ADD_NAMED_TRACK_TO_SOUNDTRACK:
            {
                // Check to see if the user set a value
                if( ( '\0' == m_pwszValueBuffer[0] ) || ( 0 == m_dwCurrentST ) )
                {
					m_dwTrackLength = 0;

                    break;
                }

				if( L'\0' == m_pwszTrackName[0] )
				//
				// Store the Current track name and
				// prompt the user for the Track Length
				//
				{
					wcscpy( m_pwszTrackName, m_pwszValueBuffer );

					// Mark that we are still in the middle of a transaction
					m_bActionInitiated = TRUE;
					m_bUpdateTexture = TRUE;

					// Prompt the user for the Track Length
					g_MenuKeypad.SetInfo( L"Enter the Length of the Track (seconds)", m_pwszValueBuffer, MENU_ST_NUMBER_SIZE );
					g_MenuKeypad.SetParent( this );

					Leave( &g_MenuKeypad );
				}
				else
				//
				// We have the track length information, store it, and process the data
				//
				{
					WCHAR* pwszStopString = NULL;

					m_dwTrackLength = wcstol( m_pwszValueBuffer, &pwszStopString, 10 );

					m_dwTrackLength *= 1000;	// Convert seconds in to milliseconds

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
		case STMENU_ITEM_CREATE_SORTING:
		case STMENU_ITEM_CREATE_SORTING_JAP:
        case STMENU_ITEM_REMOVE_ALL_SOUNDTRACKS:
            {
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
        }
    }
}


// Handle any requests for the joystick (thumb-pad)
void CSTMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
void CSTMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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

                if( ( 0 == m_dwCurrentST ) && ( STMENU_ITEM_ADD_NAMED_TRACK_TO_SOUNDTRACK == GetSelectedItem() ) )
                {
                    SetSelectedItem( GetSelectedItem() - 1 );
                }
            }
            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            if( GetSelectedItem() < ( STMENU_ITEM_NUM_ITEMS - 1 ) )
            {
                m_bUpdateTexture = TRUE;

                SetSelectedItem( GetSelectedItem() + 1 );

                if( ( 0 == m_dwCurrentST ) && ( STMENU_ITEM_ADD_NAMED_TRACK_TO_SOUNDTRACK == GetSelectedItem() ) )
                {
                    SetSelectedItem( GetSelectedItem() + 1 );
                }
            }
            break;
        }
    }
}


// Handles input (of the BUTTONS) for the current menu
void CSTMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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
            case STMENU_ITEM_CREATE_SMALL_SOUNDTRACKS_AND_TRACKS:
            case STMENU_ITEM_CREATE_SOUNDTRACKS_AND_TRACKS:
                {
                    // Prompt the user for the number of Soundtracks
                    g_MenuKeypad.SetInfo( L"Enter # of Soundtracks", m_pwszValueBuffer, MENU_ST_NUMBER_SIZE );
                    g_MenuKeypad.SetParent( this );
                    m_bActionInitiated = TRUE;

                    Leave( &g_MenuKeypad );
                    break;
                }
            case STMENU_ITEM_CREATE_NAMED_SOUNDTRACK:
                {
                    g_MenuKeyboard.SetInfo( L"Enter the name of the Soundtrack:", m_pwszValueBuffer, MENU_ST_SOUNDTRACKNAME_SIZE );
                    g_MenuKeyboard.SetParent( this );
                    m_bActionInitiated = TRUE;

                    Leave( &g_MenuKeyboard );
                    break;
                }
            case STMENU_ITEM_ADD_NAMED_TRACK_TO_SOUNDTRACK:
                {
                    // Check to make sure the user has selected "Create new soundtrack"
                    if( 0 != m_dwCurrentST )
                    {
                        g_MenuKeyboard.SetInfo( L"Enter the name of the track:", m_pwszValueBuffer, MENU_ST_TRACKNAME_SIZE );
                        g_MenuKeyboard.SetParent( this );
                        m_bActionInitiated = TRUE;

                        Leave( &g_MenuKeyboard );
                    }
                    break;
                }

			case STMENU_ITEM_CREATE_SORTING:
			case STMENU_ITEM_CREATE_SORTING_JAP:
            case STMENU_ITEM_REMOVE_ALL_SOUNDTRACKS:
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
HRESULT CSTMenu::Init( IDirect3DDevice8* pD3DDevice, char* menuFileName )
{
    XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::Init()" );

    CMenuScreen::Init( pD3DDevice, menuFileName );

    return S_OK;
}


void CreateTrackForST( DWORD dwSoundtrackID, BOOL bUseSmallTracks )
{
    char pszTrackIDText[30];

    if( bUseSmallTracks )
    {
        strcpy( pszTrackIDText, "SMTracks" );
    }
    else
    {
        strcpy( pszTrackIDText, "Tracks" );
    }

    char pszINIBuffer[MAX_PATH+1];
    ZeroMemory( pszINIBuffer, MAX_PATH+1 );

    // Get the number of tracks from the INI file
    GetPrivateProfileStringA( pszTrackIDText, "NumTracks", "0", pszINIBuffer, MAX_PATH, FILE_TRACKINI_LOCATION_A );
    unsigned long ulNumTracks = strtoul( pszINIBuffer, NULL, 10 );
    if( 0 == ulNumTracks )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CreateTrackForST():Failed to get the number of tracks!!" );

        return;
    }

    XDBGTRC( APP_TITLE_NAME_A, "CreateTrackForST():NumTracks - '%d'", ulNumTracks );
    
    // Look up a random track
    unsigned int uiTrackNum = rand() % ( ulNumTracks );
    char pszTrackNameIndex[20+1];
    char pszTrackLengthIndex[20+1];
    WCHAR pwszTrackName[MAX_PATH+1];

    ZeroMemory( pszTrackNameIndex, 20+1 );
    ZeroMemory( pszTrackLengthIndex, 20+1 );
    ZeroMemory( pwszTrackName, sizeof( WCHAR ) * ( MAX_PATH+1 ) );
    ZeroMemory( pszINIBuffer, MAX_PATH+1 );

    _snprintf( pszTrackNameIndex, 20, "%hs%uName", pszTrackIDText, uiTrackNum );
    _snprintf( pszTrackLengthIndex, 20, "%hs%uLength", pszTrackIDText, uiTrackNum );

    XDBGTRC( APP_TITLE_NAME_A, "CreateTrackForST():pszTrackNameIndex - '%hs'", pszTrackNameIndex );
    XDBGTRC( APP_TITLE_NAME_A, "CreateTrackForST():pszTrackLengthIndex - '%hs'", pszTrackLengthIndex );

    // Get the track name and length
    GetPrivateProfileStringA( pszTrackIDText, pszTrackNameIndex, "Def Name", pszINIBuffer, MAX_PATH, FILE_TRACKINI_LOCATION_A );
    _snwprintf( pwszTrackName, MAX_PATH, L"%hs", pszINIBuffer );
    ZeroMemory( pszINIBuffer, MAX_PATH+1 );
    GetPrivateProfileStringA( pszTrackIDText, pszTrackLengthIndex, "0", pszINIBuffer, MAX_PATH, FILE_TRACKINI_LOCATION_A );
    DWORD dwTrackLength = strtoul( pszINIBuffer, NULL, 10 );

    XDBGTRC( APP_TITLE_NAME_A, "CreateTrackForST():pwszTrackName - '%ls'", pwszTrackName );
    XDBGTRC( APP_TITLE_NAME_A, "CreateTrackForST():dwTrackLength - '%d'", dwTrackLength );
    
    // Add the track to the ST DB
    DWORD dwSongID = DashGetNewSongId( dwSoundtrackID );
    // Create the Track
    if( !DashAddSongToSoundtrack( dwSoundtrackID, dwSongID, dwTrackLength * 1000, pwszTrackName ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CreateTrackForST():Failed to add song to soundtrack!! - '%ls', Length - '%d'", pwszTrackName, dwTrackLength * 1000 );
    }
    else
    {
        // Copy the file to represent the song
        char pszSourceMusicFile[MAX_PATH+1];
        char pszMusicFile[MAX_PATH+1];
        
        ZeroMemory( pszMusicFile, MAX_PATH+1 );
        ZeroMemory( pszSourceMusicFile, MAX_PATH+1 );

        _snprintf( pszMusicFile, MAX_PATH, "%c:\\%s\\music\\%.4X\\%.8X.WMA", FILE_DATA_TDATA_DRIVE_LETTER_A, FILE_DATA_XDASH_TITLEID, dwSoundtrackID, dwSongID );
        _snprintf( pszSourceMusicFile, MAX_PATH, FILE_WMATRACK_LOCATION_A, pszTrackIDText, uiTrackNum );

        if( !CopyFile( pszSourceMusicFile, pszMusicFile, FALSE ) )
        {
            XDBGWRN( APP_TITLE_NAME_A, "CreateTrackForST():Failed to copy the file!! Source - '%hs', Dest - '%hs'", pszSourceMusicFile, pszMusicFile );
        }
        else
        {
            SetFileAttributes( pszMusicFile, FILE_ATTRIBUTE_NORMAL );
        }
    }
}


void CreateMusicFile( char* pszMusicFile )
{
    if( NULL == pszMusicFile )
    {
        return;
    }

    char pszSourceMusicFile[MAX_PATH+1];
    ZeroMemory( pszSourceMusicFile, MAX_PATH+1 );
    _snprintf( pszSourceMusicFile, MAX_PATH, FILE_WMATRACK_LOCATION_A, "Track", 1 );

    if( !CopyFile( pszSourceMusicFile, pszMusicFile, FALSE ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CreateMusicFile():Failed to copy the file!! Source - '%hs', Dest - '%hs'", pszSourceMusicFile, pszMusicFile );
    }
    else
    {
        SetFileAttributes( pszMusicFile, FILE_ATTRIBUTE_NORMAL );
    }
}

DWORD WINAPI DataCreateThreadFunc( LPVOID lpParameter )
{
    CSTMenu* pSTMenu = (CSTMenu*)lpParameter;

    pSTMenu->BeginUseDashSTFuncs();

    switch( pSTMenu->GetSelectedItem() )
    {
    case STMENU_ITEM_CREATE_SMALL_SOUNDTRACKS_AND_TRACKS:
    case STMENU_ITEM_CREATE_SOUNDTRACKS_AND_TRACKS:
        {
            BOOL bUseSmallTracks = FALSE;
            if( STMENU_ITEM_CREATE_SMALL_SOUNDTRACKS_AND_TRACKS == pSTMenu->GetSelectedItem() )
            {
                bUseSmallTracks = TRUE;
            }

            WCHAR pwszSoundtrack[MENU_ST_SOUNDTRACKNAME_SIZE];
            WCHAR pwszTrack[MENU_ST_TRACKNAME_SIZE];

            // Create the Soundtracks
            DWORD dwNumSTs = DashGetSoundtrackCount();
            for( DWORD x = 0; x < pSTMenu->m_dwNumSoundtracks; x++ )
            {
                ZeroMemory( pwszSoundtrack, sizeof( WCHAR ) * MENU_ST_SOUNDTRACKNAME_SIZE );

                DWORD dwNumSTs = DashGetSoundtrackCount();

                _snwprintf( pwszSoundtrack, MENU_ST_SOUNDTRACKNAME_SIZE - 1, L"Test Soundtrack %d", ( ++dwNumSTs ) );

                // Create the Soundtrack and store the ST ID
                if( !DashAddSoundtrack( pwszSoundtrack, &pSTMenu->m_dwCurrentST ) )
                {
                    XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%ls'", pwszSoundtrack );

                    // Break out of the loop early
                    break;
                }

                // Create Track for Soundtrack
                for( DWORD y = 0; y < pSTMenu->m_dwNumTracks; y++ )
                {
                    CreateTrackForST( pSTMenu->m_dwCurrentST, bUseSmallTracks );
                }
            }

            // Set this to zero, so the user doesn't try to create Tracks
            pSTMenu->m_dwCurrentST = 0;

            break;
        }
    case STMENU_ITEM_CREATE_NAMED_SOUNDTRACK:
        {
            // Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( pSTMenu->m_pwszValueBuffer, &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }

            break;
        }
    case STMENU_ITEM_ADD_NAMED_TRACK_TO_SOUNDTRACK:
        {
            // Create the Track using the current ST ID
            DWORD dwSongID = DashGetNewSongId( pSTMenu->m_dwCurrentST );

            // Create the Track
            if( !DashAddSongToSoundtrack( pSTMenu->m_dwCurrentST, dwSongID, pSTMenu->m_dwTrackLength, pSTMenu->m_pwszValueBuffer ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to add song to soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
            }
            else
            {
                // Create a file to represent the song
                char pszMusicFile[MAX_PATH+1];
                ZeroMemory( pszMusicFile, MAX_PATH+1 );

                _snprintf( pszMusicFile, MAX_PATH, "%c:\\%s\\music\\%.4X\\%.8X.WMA", FILE_DATA_TDATA_DRIVE_LETTER_A, FILE_DATA_XDASH_TITLEID, pSTMenu->m_dwCurrentST, dwSongID );
                CreateMusicFile( pszMusicFile );
            }
    
            break;
        }
	case STMENU_ITEM_CREATE_SORTING:
		{
            // Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( L"Z", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }

			// Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( L"zZ", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }

            if( !DashAddSoundtrack( L"Ba", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }

			// Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( L"bA", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            } 
			if( !DashAddSoundtrack( L"AA", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }

			// Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( L"a", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            } 
			if( !DashAddSoundtrack( L"y", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }

			// Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( L"A!", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }
            if( !DashAddSoundtrack( L"Yx", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }

			// Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( L"$6", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }
			if( !DashAddSoundtrack( L"Z*", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }

			// Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( L"#A#", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }
			if( !DashAddSoundtrack( L"Z1", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }

			// Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( L"5Y", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }
			if( !DashAddSoundtrack( L"@k@", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }
			if( !DashAddSoundtrack( L"b&b", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }


            break;
        }



	case STMENU_ITEM_CREATE_SORTING_JAP:
		{
           // Create the Soundtrack and store the ST ID
			if( !DashAddSoundtrack( L"\x3088" L"z", &pSTMenu->m_dwCurrentST ) )	//Hirigana then English
			{
					XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
					XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
			}
			if( !DashAddSoundtrack( L"z" L"\x3088", &pSTMenu->m_dwCurrentST ) )	//English then Hirigana
			{
					XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
					XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
			}
			if( !DashAddSoundtrack( L"\x30E4" L"X", &pSTMenu->m_dwCurrentST ) )	//Katakana then English
			{
					XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
					XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
			}
			if( !DashAddSoundtrack( L"X" L"\x30E4", &pSTMenu->m_dwCurrentST ) )	//English the Kataka
			{
					XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
					XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
			}
			if( !DashAddSoundtrack( L"\x3093" L"F", &pSTMenu->m_dwCurrentST ) )	//Hirigana then English
			{
					XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
					XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
			}
			if( !DashAddSoundtrack( L"F" L"\x3093", &pSTMenu->m_dwCurrentST ) )	//English then Hirigana
			{
					XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
					XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
			}
			if( !DashAddSoundtrack( L"A9", &pSTMenu->m_dwCurrentST ) )
			{
					XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
					XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
			}
			if( !DashAddSoundtrack( L"B6", &pSTMenu->m_dwCurrentST ) )
			{
					XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
					XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
			}
			if( !DashAddSoundtrack( L"B8", &pSTMenu->m_dwCurrentST ) )
			{
					XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
					XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
			}
			
			// Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( L"\x30ED\x30ED", &pSTMenu->m_dwCurrentST ) ) //last char of Katakana
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }

			// Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( L"\x30ED", &pSTMenu->m_dwCurrentST ) )  //last char of Katakana
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }

            if( !DashAddSoundtrack( L"\x3041", &pSTMenu->m_dwCurrentST ) )  //first char of Hirigana
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }

			// Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( L"\x3041\x3041", &pSTMenu->m_dwCurrentST ) )  //first char of Hirigana
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            } 
			if( !DashAddSoundtrack( L"z", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }

			// Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( L"ZZ", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            } 
			if( !DashAddSoundtrack( L"Aa", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }

			// Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( L"\x30FC", &pSTMenu->m_dwCurrentST ) )  //last char of Hirigana
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }
            if( !DashAddSoundtrack( L"\x30FC\x30FC", &pSTMenu->m_dwCurrentST ) )  //last char of Hirigana
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }

			// Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( L"\x30A1", &pSTMenu->m_dwCurrentST ) )  //first char of katakana
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }
			if( !DashAddSoundtrack( L"\x30A1\x30A1", &pSTMenu->m_dwCurrentST ) )  //first char of katakana
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }

			// Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( L"\x3041" L"1", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }
			if( !DashAddSoundtrack( L"01", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }

			// Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( L"!!", &pSTMenu->m_dwCurrentST ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }
			// Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( L"@"L"\x3088\x30E6", &pSTMenu->m_dwCurrentST ) ) //@ then Hirigana then Katakana
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }
						// Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack( L"\x30E8\x3082", &pSTMenu->m_dwCurrentST ) ) //Katakana then Hirigana
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }
						// Create the Soundtrack and store the ST ID
            if( !DashAddSoundtrack(L"\x3082\x30E8", &pSTMenu->m_dwCurrentST ) )  //Hirigana then Katakana
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to create Soundtrack!! - '%S'", pSTMenu->m_pwszValueBuffer );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }

            break;
        }



    case STMENU_ITEM_REMOVE_ALL_SOUNDTRACKS:
        {
            HANDLE hSTHandle = NULL;
            XSOUNDTRACK_DATA stData;
            ZeroMemory( &stData, sizeof( stData ) );

            // Get the number of Soundtracks
            DWORD dwNumSTs = DashGetSoundtrackCount();
            
            XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():dwNumSTs - '%d'", dwNumSTs );

            // Check to see if there are any soundtracks.  If not, leave
            if( 0 == dwNumSTs )
            {
                break;
            }

            // Create a buffer to hold all of the ST Id's
            DWORD* pdwSTIDs = new DWORD[dwNumSTs];
            // Make sure our allocation succeeded
            if( !pdwSTIDs )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to allocated memory!!" );

                break;
            }

            unsigned int uiCounter = 0;

            pSTMenu->EndUseDashSTFuncs();  // Must call this before XFindFirstSoundtrack and XFindNextSoundtrack will work

            hSTHandle = XFindFirstSoundtrack( &stData );
            if( INVALID_HANDLE_VALUE != hSTHandle )
            {
                // Enumerate through the soundtracks and store the ID info
                pdwSTIDs[uiCounter++] = stData.uSoundtrackId;

                // Find the rest of the soundtracks
                while( XFindNextSoundtrack( hSTHandle, &stData ) )
                {
                    pdwSTIDs[uiCounter++] = stData.uSoundtrackId;
                }

                // Close our handle to the soundtracks
                XFindClose( hSTHandle );

                unsigned int uiNumDeleted = 0;

                // Now that we are done with FindFirst/Next, we can re-activate this
                pSTMenu->BeginUseDashSTFuncs();

                // Remove the soundtracks
                for( unsigned int x = 0; x < dwNumSTs; x++ )
                {
                    if( !DashDeleteSoundtrack( pdwSTIDs[x] ) )
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to delete soundtrack - '%d'!!", pdwSTIDs[x] );
                        XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
                    }
                    else
                    {
                        uiNumDeleted++;
                    }
                }

                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Found   %d soundtracks", dwNumSTs );
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Deleted %d soundtracks", uiNumDeleted );
            }
            else
            //
            // We failed to find a soundtrack
            //
            {
                XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to find a Soundtrack!! - '0x%.8X', '%d'", GetLastError(), GetLastError() );

                // Make sure we don't call close on this
                hSTHandle = NULL;
            }

            // Clean up memory
            if( pdwSTIDs )
            {
                delete[] pdwSTIDs;
                pdwSTIDs = NULL;
            }

            // Make sure we don't have an ST active
            pSTMenu->EnterCritSection();
            pSTMenu->m_dwCurrentST = 0;
            pSTMenu->LeaveCritSection();

            break;
        }
    }

    // Reset these two variables so that menu's know where they are in certain processes
    pSTMenu->EnterCritSection();
    pSTMenu->m_dwNumSoundtracks = 0;
    pSTMenu->m_dwNumTracks = 0;
	pSTMenu->m_dwTrackLength = 0;
	ZeroMemory( pSTMenu->m_pwszTrackName, sizeof( WCHAR ) * MENU_ST_TRACKNAME_SIZE );
    pSTMenu->LeaveCritSection();

    // Stop using the Soundtrack APIs
    pSTMenu->EndUseDashSTFuncs();

    return 0;
}


// Signal that we will begin using the Dash ST funcs
BOOL CSTMenu::BeginUseDashSTFuncs()
{
    // Enter our critical section since we will access shared memory
    EnterCritSection();

    // Check to see if somebody is using the Dash Soundtrack APIs
    if( m_bUsingDashSTFuncs )
    {
        LeaveCritSection();

        return FALSE;
    }

    // Map the TDATA directory to the Dashboard Directory
    g_HardDrive.MapTDataToTitleID( FILE_DATA_XDASH_TITLEID );

    BOOL fReturn = DashBeginUsingSoundtracks();

    // Begin using the Dash Sountrack APIs -- Try to open the ST DB file
    if( !fReturn )
    //
    // No, we failed to open the ST DB file
    //
    {
        // Map the TDATA directory back to the Original Location
        g_HardDrive.MapTDataToTitleID( FILE_DATA_CONFIG_TITLEID );

        XDBGWRN( APP_TITLE_NAME_A, "CSTMenu::DataCreateThreadFunc():Failed to Begin Using Soundtracks!!" );
    }
    else
    //
    // Yes, we opened the ST DB file
    //
    {
        m_bUsingDashSTFuncs = TRUE;
    }

    // Leave our critical section
    LeaveCritSection();

    return fReturn;
}


// Signal that we will stop using the Dash ST funcs
BOOL CSTMenu::EndUseDashSTFuncs()
{
    // Enter our critical section since we will access shared memory
    EnterCritSection();

    // Check to make sure somebody is using the Dash ST Funcs
    if( m_bUsingDashSTFuncs )
    {
        // Stop using the Dash Soundtrack APIs
        DashEndUsingSoundtracks();

        // Map the TDATA directory back to the Original Location
        g_HardDrive.MapTDataToTitleID( FILE_DATA_CONFIG_TITLEID );
    }

    m_bUsingDashSTFuncs = FALSE;

    // Leave our critical section
    LeaveCritSection();

    return TRUE;
}


void CSTMenu::EnterCritSection()
{
    // Enter our critical section since we will access shared memory
    EnterCriticalSection( &m_CritSec );
}

void CSTMenu::LeaveCritSection()
{
    // Leave our critical section
    LeaveCriticalSection( &m_CritSec );
}
