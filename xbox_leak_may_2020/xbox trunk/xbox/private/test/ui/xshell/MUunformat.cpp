/*****************************************************
*** muunformat.h
***
*** Header file for our XShell MU UnFormat class.
*** This screen will unformat a selected Memory Unit.
***
*** by James N. Helm
*** June 22nd, 2001
***
*****************************************************/

#include "stdafx.h"
#include "muunformat.h"

extern CSoundCollection g_Sounds;           // Used to generate all sounds for the XShell
extern BOOL             g_bMUsChanged;      // Tracks whether MU's have been inserted or removed
extern DWORD            g_dwInsertedMUs;    // Tracks which MUs are inserted or removed

// Constructors
CMUUnFormat::CMUUnFormat( CXItem* pParent )
: CKeypadMenu( pParent ),
m_pMemoryUnit( NULL ),
m_bUnFormatMU( FALSE ),
m_bUnFormatBegun( FALSE ),
m_bSleepAndExit( FALSE ),
m_bDisableInput( FALSE ),
m_dwTimeSlept( 0 )
{
    XDBGTRC( APP_TITLE_NAME_A, "CMUUnFormat::CMUUnFormat(ex)" );

    m_uiNumItems = NUM_XBOX_MU_SLOTS;
    SetSelectedItem( 0 );

    GenerateKeypad();
	m_xKeypos = 0;
	m_yKeypos = 0;

    SetTitle( MENU_TITLE_MUUNFORMAT );
}


// Destructor
CMUUnFormat::~CMUUnFormat()
{
}

// Draws a menu on to the screen
void CMUUnFormat::Action( CUDTexture* pTexture )
{
    //
    // Ensure the MU we are dealing with is still plugged in
    //

    // Determine the state of the MU's and Controllers
    if( g_bMUsChanged )
    {
        // If the MU is no longer plugged in, immediately return to the previous screen
        if( !( g_dwInsertedMUs & g_dwMUBitMask[GetSelectedItem()] ) && ( !m_bUnFormatMU ) )
        {
            if( GetParent() )
            {
                Leave( GetParent() );
            }
        }
    }


    // Check to see if the MU is formatting, if so, check the status, and
    // determine if the texture should be updated
    if( m_bUnFormatMU )
    {
        // Ensure the user cannot leave the menu -- Disable button input
        m_bDisableInput = TRUE;

        if( m_bUnFormatBegun )
        {
            if( m_MUUnFormatThread.IsUnFormatComplete() )
            {
                m_bUpdateTexture = TRUE;

                // Signal the menu to pause, and then return to the previous menu
                if( FALSE == m_bSleepAndExit )
                {
                    // Setup our and sleep timer
                    m_dwTimeSlept = GetTickCount();
                    
                    m_bSleepAndExit = TRUE;
                }
            }
        }
        else
        {
            m_bUpdateTexture = TRUE;
            m_bUnFormatBegun = TRUE;

            // Begin the format
            m_MUUnFormatThread.BeginUnFormat( m_pMemoryUnit );
        }

        // Check to see if it's time to exit this menu
        if( m_bSleepAndExit )
        {
            // Make sure our final Texture message made it to the user
            // BUGBUG: This feels like a hack
            if( ( GetTickCount() - m_dwTimeSlept ) > MUUNFORMATDLG_MSG_PAUSE )
            {
                Leave( GetParent() );
            }
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

    // Lock our texture so we can render to it
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    // Let our base menu draw the items from the data file
    CKeypadMenu::Action( pTexture );

    // Insert the MU and Port the user is dealing with on to the texture
    WCHAR pwszMUName[MAX_MUNAME];
    ZeroMemory( pwszMUName, sizeof( WCHAR ) * MAX_MUNAME );

    m_pMemoryUnit->GetName( pwszMUName, MAX_MUNAME );
    if( L'\0' == pwszMUName[0] )
    //
    // The MU doesn't have a name, so don't use it
    //
    {
        pTexture->DrawText( MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( g_pwszMUPortNames[GetSelectedItem()] ) / 2.0f ), MUUNFORMATDLG_DELETE_MSG_YPOS + m_uiFontHeightOfItem + ITEM_SELECTOR_BORDER_SPACE, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", g_pwszMUPortNames[GetSelectedItem()] );
    }
    else
    //
    // The MU has a name, so use it
    //
    {
        WCHAR pwszFullMUName[255];
        ZeroMemory( pwszFullMUName, sizeof( WCHAR ) * 255 );

        wsprintf( pwszFullMUName, L"%s (%s)", g_pwszMUPortNames[GetSelectedItem()], pwszMUName );
        pTexture->DrawText( MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( pwszFullMUName ) / 2.0f ), MUUNFORMATDLG_DELETE_MSG_YPOS + m_uiFontHeightOfItem + ITEM_SELECTOR_BORDER_SPACE, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", pwszFullMUName );
    }

    // Perform the format if the user has selected it
    if( m_bUnFormatMU )
    {
        WCHAR wpszMUActionText[30];                             // Used to display action text on the screen (i.e., formatting, etc)
        ZeroMemory( wpszMUActionText, 30 * sizeof( WCHAR ) );

        // Update our texture with the info that we are formatting
        _snwprintf( wpszMUActionText, 29, MUUNFORMATDLG_FORMATTING_MSG );

        pTexture->DrawBox( MENUBOX_MAINAREA_X1, MUUNFORMATDLG_DELETE_MSG_YPOS, MENUBOX_MAINAREA_X2, MUUNFORMATDLG_DELETE_MSG_YPOS + m_uiFontHeightOfItem, SCREEN_DEFAULT_TEXT_BG_COLOR );
        pTexture->DrawText( MENUBOX_TEXTAREA_X_CENTER  - ( GetStringPixelWidth( wpszMUActionText ) / 2.0f ), MUUNFORMATDLG_DELETE_MSG_YPOS, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", wpszMUActionText );

        // If the formatting has begun, check it's status, and update the
        // texture if necessary
        if( m_bUnFormatBegun )
        {
            if( m_MUUnFormatThread.IsUnFormatComplete() )
            {
                // Print the result of the format on to our texture
                if( m_MUUnFormatThread.IsUnFormatSuccessful() )
                {
                    _snwprintf( wpszMUActionText, 29, MUUNFORMATDLG_SUCCESS_MSG );
                }
                else
                {
                    _snwprintf( wpszMUActionText, 29, MUUNFORMATDLG_FAIL_MSG );
                }
                
                pTexture->DrawBox( MENUBOX_MAINAREA_X1, MUUNFORMATDLG_DELETE_MSG_YPOS, MENUBOX_MAINAREA_X2, MUUNFORMATDLG_DELETE_MSG_YPOS + m_uiFontHeightOfItem, SCREEN_DEFAULT_TEXT_BG_COLOR );
                pTexture->DrawText( MENUBOX_TEXTAREA_X_CENTER  - ( GetStringPixelWidth( wpszMUActionText ) / 2.0f ), MUUNFORMATDLG_DELETE_MSG_YPOS, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", wpszMUActionText );
            }
        }
    }
    else // The user is not formatting an MU, display the warning message
    {
        // Draw our Text Message
        pTexture->DrawText( MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( MUUNFORMATDLG_DELETE_MSG ) / 2.0f ), MUUNFORMATDLG_DELETE_MSG_YPOS, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", MUUNFORMATDLG_DELETE_MSG );
    }

    // Unlock our texture
    pTexture->Unlock();
}


void CMUUnFormat::GenerateKeypad(void) 
{
	// First create numberpad
	int xgrid  = 1;
	int ygrid  = MUUNFORMAT_NUM_ROWS;
	int xstart = MUUNFORMATDLG_BUTTON_X1;
	int ystart = MUUNFORMATDLG_BUTTON_Y1;
	int width  = 103;
	int height = 32;
	int spacer = 2;

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
	ygrid  = MUUNFORMAT_NUM_ROWS;
	xstart = 323;
	ystart = MUUNFORMATDLG_BUTTON_Y1;
	width  = 103;
	height = 32;
	spacer = 2;
	
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
    m_keyPad[0][0].defineText(L"yes");
    m_keyPad[0][1].defineText(L"no");
}


BOOL CMUUnFormat::renderKeypad( CUDTexture* pTexture )
{
	
	// Render outlines of keys	
	float x1, x2;
	float y1, y2;
	for(int x = 0; x < MUUNFORMAT_NUM_COLS; x++)
	{
		for(int y = 0; y < MUUNFORMAT_NUM_ROWS; y++)
		{
			if(m_keyPad[x][y].getRender())
			{
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
			}
		}
	}
	
	return TRUE;
}


// Handle any requests for the joystick (thumb-pad)
void CMUUnFormat::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
{
    // If the input is disabled, simply return, and do not process any commands
    if( m_bDisableInput )
    {
        return;
    }

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
void CMUUnFormat::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
    // If the input is disabled, simply return, and do not process any commands
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
			if( ( m_yKeypos != ( MUUNFORMAT_NUM_ROWS - 1 ) ) && ( m_keyPad[m_xKeypos][m_yKeypos + 1].getRender() ) )
			{
                m_bUpdateTexture = TRUE;

                // Play the Menu Item Select Sound
                g_Sounds.PlayMenuItemSelectSound();

				m_yKeypos++;
			}
            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CMUUnFormat::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // If the input is disabled, simply return, and do not process any commands
    if( m_bDisableInput )
    {
        return;
    }

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
			if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"yes" ) )      //Yes
			{
                m_bUpdateTexture = TRUE;

                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                m_bUnFormatMU = TRUE;
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"no" ) )  //No
			{
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                // Go Back one menu
                if( GetParent() )
                {
                    Leave( GetParent() );
                }
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


// Sets the local member to point at the proper memory unit
HRESULT CMUUnFormat::SetMemoryUnit( CMemoryUnit* pMemoryUnit )
{
    XDBGTRC( APP_TITLE_NAME_A, "CMUUnFormat::SetMemoryUnit()" );

    if( !pMemoryUnit )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CMUUnFormat::SetMemoryUnit():Invalid Argument passed in!!" );

        return E_INVALIDARG;
    }

    m_pMemoryUnit = pMemoryUnit;

    // Set the Selected Item via an index
    SetSelectedItem( m_pMemoryUnit->GetIndex() );

    return S_OK;
}


// This will be called whenever this menu is exited
void CMUUnFormat::Leave( CXItem* pItem )
{
    // Call our base-class Leave
    CKeypadMenu::Leave( pItem );
}


// This will be called whenever this menu is entered / activated
void CMUUnFormat::Enter()
{
    // Call the base-class enter
    CKeypadMenu::Enter();

    // Ensure our input is enabled when the user arrives
    m_bDisableInput = FALSE;

    // Toggle the Format status
    m_bUnFormatMU = FALSE;
    m_bUnFormatBegun = FALSE;

    // Ensure the menu is not marked exit the next time the user visits
    m_bSleepAndExit = FALSE;

    // Default the focus to the yes button
	m_xKeypos = MUUNFORMAT_NUM_COLS - 1;
	m_yKeypos = 0;

    // Make sure we don't process the first keypress
    m_bDropKeyPress = TRUE;
}
