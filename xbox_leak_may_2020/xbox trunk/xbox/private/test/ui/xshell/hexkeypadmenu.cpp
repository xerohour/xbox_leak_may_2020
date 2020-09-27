/*****************************************************
*** hexkeypadmenu.cpp
***
*** CPP file for our XShell hex keypad menu class.
*** This class will allow the user to input a hex
*** value, and return that value to the caller.
***
*** by James Helm
*** May 28th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "hexkeypadmenu.h"

extern CSoundCollection g_Sounds;           // Used to generate all sounds for the XShell


// Constructors
CHexKeypadMenu::CHexKeypadMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_pwszTextString( NULL ),
m_pwszValueBuffer( NULL ),
m_uiValueSize( 0 ),
m_bDropKeyPress( TRUE )
{
	GenerateKeypad();

    // Default the focus to the OK button
	m_xKeypos = HEXKEYPAD_NUM_ROWS - 1;
	m_yKeypos = 0;

    SetTitle( MENU_TITLE_HEXKEYPAD );
}


// Destructor
CHexKeypadMenu::~CHexKeypadMenu()
{
    if( m_pwszTextString )
    {
        delete[] m_pwszTextString;
        m_pwszTextString = NULL;
    }
}


// Draws a menu on to the screen
void CHexKeypadMenu::Action( CUDTexture* pTexture )
{
    // Lock our texture and set it as the render target
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    // Let our base menu draw the items from the data file
    CMenuScreen::Action( pTexture );

	// Draw our keypad
	renderKeypad( pTexture );

	// Draw our User defined Text
    pTexture->DrawText( MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( m_pwszTextString ) / 2.0f ), HEXKEYPAD_TEXT_Y1, ITEM_HEADER_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", m_pwszTextString );

    // Draw the current value on to the screen
    pTexture->DrawText( MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( m_pwszValueBuffer ) / 2.0f ), HEXKEYPAD_VALUE_Y1, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", m_pwszValueBuffer );
    
    // Unlock our texture
    pTexture->Unlock();
}


// Handle the Joystick Input
void CHexKeypadMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
                HandleInput( CONTROL_DPAD_RIGHT, bFirstXPress );
            }
            break;
        }
    }
}


// Handles input (of the CONTROLS) for the current menu
void CHexKeypadMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
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
            m_bUpdateTexture = TRUE;

            // Play the Menu Item Select Sound
            g_Sounds.PlayMenuItemSelectSound();

            BOOL bWrap = TRUE;
            for( int x = ( m_yKeypos - 1 ); x >= 0; x-- )
            {
                if( m_keyPad[m_xKeypos][x].getRender() )
                {
                    m_yKeypos = x;
                    bWrap = FALSE;

                    break;
                }
            }

            if( bWrap )
            {
                m_yKeypos = HEXKEYPAD_NUM_COLS - 1;
                while( !m_keyPad[m_xKeypos][m_yKeypos].getRender() )
                {
                    m_yKeypos--;
                }
            }

            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            m_bUpdateTexture = TRUE;

            // Play the Menu Item Select Sound
            g_Sounds.PlayMenuItemSelectSound();

            BOOL bWrap = TRUE;
            for( int x = ( m_yKeypos + 1 ); x < HEXKEYPAD_NUM_COLS; x++ )
            {
                if( m_keyPad[m_xKeypos][x].getRender() )
                {
                    m_yKeypos = x;
                    bWrap = FALSE;
                
                    break;
                }
            }

            if( bWrap )
            {
                m_yKeypos = 0;
                while( !m_keyPad[m_xKeypos][m_yKeypos].getRender() )
                {
                    m_yKeypos++;
                }
            }
            
            break;
        }
    case CONTROL_DPAD_LEFT:
        {
            m_bUpdateTexture = TRUE;

            // Play the Menu Item Select Sound
            g_Sounds.PlayMenuItemSelectSound();

            BOOL bWrap = TRUE;
            for( int x = ( m_xKeypos - 1 ); x >= 0; x-- )
            {
                if( m_keyPad[x][m_yKeypos].getRender() && ( wcscmp( m_keyPad[x][m_yKeypos].resultChar, L"place" ) != 0 ) )
                {
                    m_xKeypos = x;
                    bWrap = FALSE;
                
                    break;
                }
            }

            if( bWrap )
            {
                m_xKeypos = HEXKEYPAD_NUM_ROWS - 1;
                while( !m_keyPad[m_xKeypos][m_yKeypos].getRender() )
                {
                    m_xKeypos--;
                }
            }

            break;
        }
    case CONTROL_DPAD_RIGHT:
        {
            m_bUpdateTexture = TRUE;

            // Play the Menu Item Select Sound
            g_Sounds.PlayMenuItemSelectSound();

            BOOL bWrap = TRUE;
            for( int x = ( m_xKeypos + 1 ); x < HEXKEYPAD_NUM_ROWS; x++ )
            {
                if( m_keyPad[x][m_yKeypos].getRender() && ( wcscmp( m_keyPad[x][m_yKeypos].resultChar, L"place" ) != 0 ) )
                {
                    m_xKeypos = x;
                    bWrap = FALSE;
                
                    break;
                }
            }
            
            if( bWrap )
            {
                m_xKeypos = 0;
                while( !m_keyPad[m_xKeypos][m_yKeypos].getRender() )
                {
                    m_xKeypos++;
                }
            }

            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CHexKeypadMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    if( bFirstPress )
    {
        m_bDropKeyPress = FALSE;

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

    if ( m_bDropKeyPress )
    {
        return;
    }

    // Handle Buttons being pressed
    switch( buttonPressed )
    {
    case BUTTON_A:
        {
			// Deal with keyboard input
			if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"«" ) )                //backspace
			{
				int len = wcslen( m_pwszValueBuffer );
				
				if( len > 0 )
				{
                    m_bUpdateTexture = TRUE;

                    // Play the keyboard stroke sound
                    g_Sounds.PlayKeyboardStrokeSound();

					m_pwszValueBuffer[len - 1] = L'\0';
				}
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"ok" ) )          //ok
			{
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                // Go Back one menu
                if( GetParent() )
                {
                    Leave( GetParent() );
                }
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"cancel" ) )      //cancel
			{
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                // Set the text to nothing
                m_pwszValueBuffer[0] = '\0';

                // Go Back one menu
                if( GetParent() )
                {
                    Leave( GetParent() );
                }
			}
			else
			{
                // Ensure we don't exceed the limit of an IP Address
                if( wcslen( m_pwszValueBuffer ) < m_uiValueSize )
                {
                    m_bUpdateTexture = TRUE;

                    // Play the keyboard stroke sound
                    g_Sounds.PlayKeyboardStrokeSound();

				    wcscat( m_pwszValueBuffer, m_keyPad[m_xKeypos][m_yKeypos].resultChar );
                }
			}
            break;
        }
    case BUTTON_B:
        {
            // Set the text to nothing
            m_pwszValueBuffer[0] = '\0';

            // Go Back one menu
            if( GetParent() )
            {
                // Play the Back Sound
                g_Sounds.PlayBackSound();

                Leave( GetParent() );
            }
            break;
        }
    case BUTTON_WHITE:  // Backspace
        {
			int len = wcslen( m_pwszValueBuffer );
			
			if( len > 0 )
			{
                m_bUpdateTexture = TRUE;

                // Play the keyboard stroke sound
                g_Sounds.PlayKeyboardStrokeSound();

				m_pwszValueBuffer[len - 1] = L'\0';
			}

            break;
        }
    }
}


void CHexKeypadMenu::GenerateKeypad(void) 
{
	// First create numberpad
	int xgrid  = HEXKEYPAD_NUM_ROWS - 1;
	int ygrid  = HEXKEYPAD_NUM_COLS;
	int xstart = HEXKEYPAD_TOPROW_X1;
	int ystart = HEXKEYPAD_TOPROW_Y1;
	int width  = 32;
	int height = 32;
	int spacer = 2;

	int currentx = xstart;
	int currenty = ystart;

    for(int y = 0; y < ygrid; y++)
    {
	    for(int x = 0; x < xgrid; x++)
	    {
			m_keyPad[x][y].define( currentx, currenty, width, height, SCREEN_DEFAULT_TEXT_FG_COLOR );
            if( 1 == x )
            {
                currentx += width + ( spacer * 3 );
            }
            else
            {
                currentx += width + spacer;
            }
		}
		currentx = xstart;
		currenty += height + spacer;
	}


	// Second generate larger buttons
	int xbase  = xgrid;
	xgrid  = 1;
	ygrid  = HEXKEYPAD_NUM_COLS;
	xstart = HEXKEYPAD_TOPROW_LARGE_BUTTONS_X1;
	ystart = HEXKEYPAD_TOPROW_Y1;
	width  = 103;
	height = 32;
	spacer = 2;
	
	currentx = xstart;
	currenty = ystart;
    for(int y = 0; y < ygrid; y++)
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
	m_keyPad[0][0].defineText(L"A");        m_keyPad[1][0].defineText(L"D");        m_keyPad[2][0].defineText(L"1");        m_keyPad[3][0].defineText(L"2");    m_keyPad[4][0].defineText(L"3");        m_keyPad[5][0].defineText(L"ok");
    m_keyPad[0][1].defineText(L"B");        m_keyPad[1][1].defineText(L"E");        m_keyPad[2][1].defineText(L"4");        m_keyPad[3][1].defineText(L"5");    m_keyPad[4][1].defineText(L"6");        m_keyPad[5][1].defineText(L"cancel");
    m_keyPad[0][2].defineText(L"C");        m_keyPad[1][2].defineText(L"F");        m_keyPad[2][2].defineText(L"7");        m_keyPad[3][2].defineText(L"8");    m_keyPad[4][2].defineText(L"9");        m_keyPad[5][2].defineText(L"«");
    m_keyPad[0][3].defineText(L"place");    m_keyPad[1][3].defineText(L"place");    m_keyPad[2][3].defineText(L"place");    m_keyPad[3][3].defineText(L"0");    m_keyPad[4][3].defineText(L"place");    m_keyPad[5][3].defineText(L"place");

    // Make sure we don't render any buttons that are place holders
    for(int x = 0; x < HEXKEYPAD_NUM_ROWS; x++)
    {
	    for(int y = 0; y < HEXKEYPAD_NUM_COLS; y++)
        {
            if( 0 == wcscmp( m_keyPad[x][y].resultChar, L"place" ) )
            {
                m_keyPad[x][y].setRender( FALSE );
            }
        }
    }
}


BOOL CHexKeypadMenu::renderKeypad( CUDTexture* pTexture )
{
	
	// Render outlines of keys	
	float x1, x2;
	float y1, y2;
	for(int x = 0; x < HEXKEYPAD_NUM_ROWS; x++)
	{
		for(int y = 0; y < HEXKEYPAD_NUM_COLS; y++)
		{
			if( m_keyPad[x][y].getRender() )
			{
                // XDBGWRN( APP_TITLE_NAME_A, "CHexKeypadMenu::renderKeypad():x - '%d', y - '%d'", x, y );

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
                    // Draw the selector box around the currently highlighted number
					pTexture->DrawBox( x1, y1, x2, y2, ITEM_SELECTOR_COLOR );
                    
                    dwCharColor = ITEM_SELECTED_VALUE_COLOR;
                }
                else
                {
                    dwCharColor = SCREEN_DEFAULT_TEXT_FG_COLOR;
                }

                pTexture->DrawOutline( x1, y1, x2, y2, HEXKEYPAD_LINE_WIDTH, m_keyPad[x][y].selectColor );
				
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


// This will be called whenever this menu is entered / activated
void CHexKeypadMenu::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();

    // Make sure we don't process the first keypress
    m_bDropKeyPress = TRUE;

    // Default the focus to the OK button
	m_xKeypos = HEXKEYPAD_NUM_ROWS - 1;
	m_yKeypos = 0;
}


// This should be called before the Key pad is invoked
HRESULT CHexKeypadMenu::SetInfo( WCHAR* pwszTextString, WCHAR* pwszValueBuffer, unsigned int uiValueSize )
{
    // Check to make sure we were passed valid buffers
    if( ( !pwszTextString ) || ( !pwszValueBuffer ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CHexKeypadMenu::SetInfo():Invalid argument passed in!!" );

        return E_INVALIDARG;
    }

    // Clean up memory if necessary
    if( m_pwszTextString )
    {
        delete[] m_pwszTextString;
        m_pwszTextString = NULL;
    }

    // Set our properties
    int iStrLen = wcslen( pwszTextString );
    m_pwszTextString = new WCHAR[iStrLen + 1];
    if( NULL == m_pwszTextString )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CHexKeypadMenu::SetInfo():Failed to allocate memory!!" );

        return E_FAIL;
    }
    else
    {
        ZeroMemory( m_pwszTextString, sizeof( WCHAR ) * ( iStrLen + 1 ) );
        wcscpy( m_pwszTextString, pwszTextString );
    }
    
    m_pwszValueBuffer = pwszValueBuffer;
    m_uiValueSize = uiValueSize;

    // Clear Memory
    // m_pwszValueBuffer[0] = L'\0';

    return S_OK;
}
