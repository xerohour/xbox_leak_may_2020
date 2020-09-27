/*****************************************************
*** memoryoptions.cpp
***
*** CPP file for our XShell Memory Options class.
*** This screen will allow the user to select an
*** action that they would like to perform on a
*** memory area
***
*** by James N. Helm
*** February 15th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "memoryoptions.h"

extern CSoundCollection g_Sounds;           // Used to generate all sounds for the XShell
extern BOOL             g_bMUsChanged;      // Tracks whether MU's have been inserted or removed
extern DWORD            g_dwInsertedMUs;    // Tracks which MUs are inserted or removed

// Constructors
CMemoryOptions::CMemoryOptions( CXItem* pParent )
: CKeypadMenu( pParent ),
m_pMemoryUnit( NULL ),
m_MUFormatDlg( NULL ),
m_MUUnFormatDlg( NULL ),
m_MUNameDlg( NULL ),
m_HexKeypadMenu( NULL )
{
    XDBGTRC( APP_TITLE_NAME_A, "CMemoryOptions::CMemoryOptions(ex)" );

    m_uiNumItems = NUM_XBOX_MU_SLOTS;
    SetSelectedItem( 0 );

    GenerateKeypad();
	m_xKeypos = 0;
	m_yKeypos = 0;

    SetTitle( MENU_TITLE_MEMORYOPTIONS );

    ZeroMemory( m_pwszBlockCount, sizeof( WCHAR ) * 4 );

    // Set the parent of our HexKeypad Menu to be this menu
    m_HexKeypadMenu.SetParent( this );
}


// Destructor
CMemoryOptions::~CMemoryOptions()
{
}

// Initialize the Menu
HRESULT CMemoryOptions::Init( char* menuFileName )
{
    XDBGTRC( APP_TITLE_NAME_A, "CMemoryOptions::Init()" );

    CKeypadMenu::Init( menuFileName );

    // Initialize our MU Format Dialog-Like Menu
    m_MUFormatDlg.Init( FILE_DATA_MENU_DIRECTORY_A "\\muformat.mnu" );
    m_MUFormatDlg.SetParent( GetParent() );

    // Initialize our MU Format Dialog-Like Menu
    m_MUUnFormatDlg.Init( FILE_DATA_MENU_DIRECTORY_A "\\muunformat.mnu" );
    m_MUUnFormatDlg.SetParent( GetParent() );

    // Init our MU Name Dialog Menu
    m_MUNameDlg.Init( FILE_DATA_MENU_DIRECTORY_A "\\muname.mnu" );
    m_MUNameDlg.SetParent( GetParent() );

    return S_OK;
}


// Draws a menu on to the screen
void CMemoryOptions::Action( CUDTexture* pTexture )
{
    //
    // Ensure the MU we are dealing with is still plugged in
    //
    
    // Determine the state of the MU's and Controllers
    // If the MU is no longer plugged in, immediately return to the previous screen
    if( !( g_dwInsertedMUs & g_dwMUBitMask[GetSelectedItem()] ) )
    {
        if( GetParent() )
        {
            Leave( GetParent() );
        }
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

    // Check to see if we should create our MU file, and then leave
    if( m_bCollectingBlocks )
    {
        m_bCollectingBlocks = FALSE;

        // Check to see if the user set a value
        if( '\0' != m_pwszBlockCount[0] )
        {
            // Store the value in to a usable number
            WCHAR* pwszStopString = NULL;
            DWORD dwFileSize = wcstol( m_pwszBlockCount, &pwszStopString, 10 );

            if( dwFileSize > m_pMemoryUnit->GetFreeBlocks() )
            {
                dwFileSize = m_pMemoryUnit->GetFreeBlocks();
            }

            // Create the file on the MU
            char pszDir[4];

            sprintf( pszDir, "%c:\\", m_pMemoryUnit->GetUDataDrive() );
            m_pMemoryUnit->FillArea( pszDir, dwFileSize );
        }

        // Exit this menu
        Leave( GetParent() );

        return;
    }

    // Lock our texture so we can render to it
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    // Let our base menu draw the items from the data file
    CKeypadMenu::Action( pTexture );

    // Draw our Text Message
    pTexture->DrawText( MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( MENU_MEMORYOPTIONS_ACTION_TEXT ) / 2.0f ), MENU_MEMORYOPTOINS_ACTION_TEXT_YPOS, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", MENU_MEMORYOPTIONS_ACTION_TEXT );

    // Insert the MU and Port the user is dealing with on to the texture
    WCHAR pwszMUName[MAX_MUNAME];
    ZeroMemory( pwszMUName, sizeof( WCHAR ) * MAX_MUNAME );

    m_pMemoryUnit->GetName( pwszMUName, MAX_MUNAME );
    if( L'\0' == pwszMUName[0] )
    //
    // The MU doesn't have a name, so don't use it
    //
    {
        pTexture->DrawText( MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( g_pwszMUPortNames[GetSelectedItem()] ) / 2.0f ), MENU_MEMORYOPTOINS_ACTION_TEXT_YPOS + m_uiFontHeightOfItem + ITEM_SELECTOR_BORDER_SPACE, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", g_pwszMUPortNames[GetSelectedItem()] );
    }
    else
    //
    // The MU has a name, so use it
    //
    {
        WCHAR pwszFullMUName[255];
        ZeroMemory( pwszFullMUName, sizeof( WCHAR ) * 255 );

        wsprintf( pwszFullMUName, L"%s (%s)", g_pwszMUPortNames[GetSelectedItem()], pwszMUName );
        pTexture->DrawText( MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( pwszFullMUName ) / 2.0f ), MENU_MEMORYOPTOINS_ACTION_TEXT_YPOS + m_uiFontHeightOfItem + ITEM_SELECTOR_BORDER_SPACE, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", pwszFullMUName );
    }

    // Unlock our texture
    pTexture->Unlock();
}


void CMemoryOptions::GenerateKeypad(void) 
{
	// First create numberpad
	int xgrid  = 1;
	int width  = 103;
	int height = 32;
	int spacer = 10;
	int ygrid  = MEMORYOPTIONS_NUM_ROWS;
	int xstart = MENU_MEMORYOPTIONS_BUTTON_X1;
	int ystart = MENU_MEMORYOPTIONS_BUTTON_Y1;

	int currentx = xstart;
	int currenty = ystart;

	for(int y = 0; y < ygrid; y++)
	{
		for(int x = 0; x < xgrid; x++)
		{
			m_keyPad[x][y].define( currentx, currenty, width, height, SCREEN_DEFAULT_TEXT_FG_COLOR );
			currentx += width + spacer;
		}
		currentx = xstart;
		currenty += height + spacer;
	}


	// Second generate larger buttons
	int xbase  = xgrid;
	xgrid  = 1;
	width  = 103;
	height = 32;
	spacer = 10;
    ygrid  = MEMORYOPTIONS_NUM_ROWS;
	xstart = MENU_MEMORYOPTIONS_BUTTON_X1 + width + spacer;
	ystart = MENU_MEMORYOPTIONS_BUTTON_Y1;
	
	currentx = xstart;
	currenty = ystart;
	for(y = 0; y < ygrid; y++)
	{
		for(int x = xbase; x < (xbase + xgrid); x++)
		{
			m_keyPad[x][y].define( currentx, currenty, width, height, SCREEN_DEFAULT_TEXT_FG_COLOR );
			currentx += width + spacer;
		}
		currentx = xstart;
		currenty += height + spacer;
	}


	// Set the text of the keys
    m_keyPad[0][0].defineText( MENU_MEMORYOPTIONS_OPTION1_TEXT ); m_keyPad[1][0].defineText( MENU_MEMORYOPTIONS_OPTION3_TEXT );
    m_keyPad[0][1].defineText( MENU_MEMORYOPTIONS_OPTION2_TEXT ); m_keyPad[1][1].defineText( MENU_MEMORYOPTIONS_OPTION4_TEXT );
}


BOOL CMemoryOptions::renderKeypad( CUDTexture* pTexture )
{
	
	// Render outlines of keys	
	float x1, x2;
	float y1, y2;
	for(int x = 0; x < MEMORYOPTIONS_NUM_COLS; x++)
	{
		for(int y = 0; y < MEMORYOPTIONS_NUM_ROWS; y++)
		{
			if(m_keyPad[x][y].getRender())
			{
                //
                // BUGBUG: HARDCODE: If the MU isn't formatted, don't display the 2nd button
                // The second button is the "Name" button
                //
                if( ( y != 0 || x != 0 ) && ( !m_pMemoryUnit->IsFormatted() ) )
                {
                    break;
                }

                if( !m_pMemoryUnit->IsFormatted() )
                {
                    if( y == 0 && x == 0 )
                    {
                        // Correct our button placement if there is only one button on this screen
                        m_keyPad[x][y].xorigin += 65;
                    }
                    else
                    {
                        break;
                    };
                }

                // Lock our Texture
                pTexture->Lock();

				x1 = (float)m_keyPad[x][y].xorigin;
				x2 = (float)(m_keyPad[x][y].xorigin + m_keyPad[x][y].width);
				
				y1 = (float)m_keyPad[x][y].yorigin;
				y2 = (float)(m_keyPad[x][y].yorigin + m_keyPad[x][y].height);
	
				// hilight key if currently selected
                DWORD dwCharColor;
				if((m_xKeypos == x) && (m_yKeypos == y))
                {
					pTexture->DrawBox( x1, y1, x2, y2, ITEM_SELECTOR_COLOR );
                    dwCharColor = ITEM_SELECTED_TEXT_COLOR;
                }
                else
                {
                    dwCharColor = SCREEN_DEFAULT_TEXT_FG_COLOR;
                }

                pTexture->DrawOutline( x1, y1, x2, y2, KEYPAD_LINE_WIDTH, m_keyPad[x][y].selectColor );
				
				unsigned int decent;
				unsigned int fontHeight;
				GetFontSize( &fontHeight, &decent );
				
				float textX = (float)((m_keyPad[x][y].width / 2) + m_keyPad[x][y].xorigin) - GetStringPixelWidth( m_keyPad[x][y].resultChar ) / 2;
				float textY = (float)((m_keyPad[x][y].height / 2) + m_keyPad[x][y].yorigin) - (fontHeight / 2);
				
                pTexture->DrawText( textX, textY, dwCharColor, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", m_keyPad[x][y].resultChar);

                // Unlock our Texture
                pTexture->Unlock();

                // Correct our button placement if there is only one button on this screen
                if( !m_pMemoryUnit->IsFormatted() )
                {
                    m_keyPad[x][y].xorigin -= 65;
                }
            }
		}
	}
	
	return TRUE;
}


// Handle any requests for the joystick (thumb-pad)
void CMemoryOptions::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
void CMemoryOptions::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
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
			if( ( m_yKeypos != 0 ) && ( m_keyPad[m_xKeypos][m_yKeypos - 1].getRender() ) )
			{
                m_bUpdateTexture = TRUE;

                // Play the Menu Item Select Sound
                g_Sounds.PlayMenuItemSelectSound();

				m_yKeypos--;
			}

            break;
        }
    case CONTROL_DPAD_DOWN:
        {
			if( ( m_yKeypos != ( MEMORYOPTIONS_NUM_ROWS - 1 ) ) &&
                ( m_keyPad[m_xKeypos][m_yKeypos + 1].getRender() ) &&
                ( TRUE == m_pMemoryUnit->IsFormatted() ) )
			{
                m_bUpdateTexture = TRUE;

                // Play the Menu Item Select Sound
                g_Sounds.PlayMenuItemSelectSound();

				m_yKeypos++;
			}

            break;
        }
    case CONTROL_DPAD_LEFT:
        {
			if( ( m_xKeypos != 0 ) && ( m_keyPad[m_xKeypos - 1][m_yKeypos].getRender() ) )
			{
                m_bUpdateTexture = TRUE;

                // Play the Menu Item Select Sound
                g_Sounds.PlayMenuItemSelectSound();

				m_xKeypos--;
			}

            break;
        }
    case CONTROL_DPAD_RIGHT:
        {
			if( ( m_xKeypos != ( MEMORYOPTIONS_NUM_COLS - 1 ) ) &&
                ( m_keyPad[m_xKeypos][m_yKeypos + 1].getRender() ) &&
                ( m_pMemoryUnit->IsFormatted() ) )
			{
                m_bUpdateTexture = TRUE;

                // Play the Menu Item Select Sound
                g_Sounds.PlayMenuItemSelectSound();

				m_xKeypos++;
			}

            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CMemoryOptions::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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
			// Deal with keyboard input
			if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, MENU_MEMORYOPTIONS_OPTION1_TEXT ) )      // format
			{
                m_bUpdateTexture = TRUE;

                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                m_MUFormatDlg.SetMemoryUnit( m_pMemoryUnit );
                Leave( &m_MUFormatDlg );
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, MENU_MEMORYOPTIONS_OPTION2_TEXT ) )  // unformat
			{
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                m_MUUnFormatDlg.SetMemoryUnit( m_pMemoryUnit );
                Leave( &m_MUUnFormatDlg );
    		}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, MENU_MEMORYOPTIONS_OPTION3_TEXT ) )  // name
			{
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                m_MUNameDlg.SetMemoryUnit( m_pMemoryUnit );
                Leave( &m_MUNameDlg );
    		}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, MENU_MEMORYOPTIONS_OPTION4_TEXT ) )  // create file
			{
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                WCHAR pwszUserPrompt[50];
                pwszUserPrompt[49] = L'\0';

                _snwprintf( pwszUserPrompt, 49, L"Free Blocks %d. Enter File Size in Blocks:", m_pMemoryUnit->GetFreeBlocks() );

                // Ask the user for the block amount
                // Get the TITLE ID from the Hex Keypad
                m_pwszBlockCount[0] = L'\0';
                m_HexKeypadMenu.SetInfo( pwszUserPrompt, m_pwszBlockCount, 3 );
                m_bCollectingBlocks = TRUE;

                Leave( &m_HexKeypadMenu );
    		}

            break;
        }
    case BUTTON_B:
        {
            // Go Back one menu
            if( GetParent() )
            {
                // Play the Back Sound
                g_Sounds.PlayBackSound();

                Leave( GetParent() );
            }

            break;
        }
    }
}


// This will be called whenever this menu is exited
void CMemoryOptions::Leave( CXItem* pItem )
{
    // Call the base-class Leave
    CKeypadMenu::Leave( pItem );
}

// This will be called whenever this menu is entered / activated
void CMemoryOptions::Enter()
{
    // Call the base-class enter
    CKeypadMenu::Enter();

    // Default the focus to the format button
	m_xKeypos = 0;
	m_yKeypos = 0;

    // Make sure we don't process the first keypress
    m_bDropKeyPress = TRUE;
}


// Sets the local member to point at the proper memory unit
HRESULT CMemoryOptions::SetMemoryUnit( CMemoryUnit* pMemoryUnit )
{
    XDBGTRC( APP_TITLE_NAME_A, "CMemoryOptions::SetMemoryUnit()" );

    if( !pMemoryUnit )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CMemoryOptions::SetMemoryUnit():Invalid Argument passed in!!" );

        return E_INVALIDARG;
    }

    m_pMemoryUnit = pMemoryUnit;

    // Set the Selected Item via an index
    SetSelectedItem( m_pMemoryUnit->GetIndex() );

    return S_OK;
}
