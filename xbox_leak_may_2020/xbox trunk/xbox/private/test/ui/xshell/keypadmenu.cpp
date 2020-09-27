/*****************************************************
*** keypadmenu.cpp
***
*** CPP file for our XShell keypad menu class.
***
*** by James Helm
*** December 5th, 2000
***
*****************************************************/

#include "stdafx.h"
#include "keypadmenu.h"

extern CXSettings       g_XboxSettings; // Settings that are stored on the Xbox
extern CSoundCollection g_Sounds;       // Used to generate all sounds for the XShell

// Constructors
CKeypadMenu::CKeypadMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_bDropKeyPress( TRUE )
{
	GenerateKeypad();

    // Default the focus to the OK button
	m_xKeypos = KEYPAD_NUM_COLS - 1;
	m_yKeypos = 0;

	ZeroMemory( m_pwszAddress, sizeof( WCHAR ) * DATAVALUE_IPADDRESS_LENGTH + 1 );
}


// Destructor
CKeypadMenu::~CKeypadMenu()
{
}


// Draws a menu on to the screen
void CKeypadMenu::Action( CUDTexture* pTexture )
{
    // Lock our texture and set it as the render target
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    // Let our base menu draw the items from the data file
    CMenuScreen::Action( pTexture );

	// Draw our keypad
	renderKeypad( pTexture );

	// Draw our IP Address field
	// float fXPos = MENUBOX_TEXTAREA_X_CENTER - 17.0f;
    float fXPos = MENUBOX_TEXTAREA_X_CENTER;
    float fYPos = KEYPAD_IPADDRHEADER_YPOS;
	pTexture->DrawText( fXPos, fYPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls", m_pwszAddress );

    // Unlock our texture
    pTexture->Unlock();
}


// Handle the Joystick Input
void CKeypadMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
void CKeypadMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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

            if( 0 == m_yKeypos )    // Wrap the cursor
            {
                m_yKeypos = 3;
                while( !m_keyPad[m_xKeypos][m_yKeypos].getRender() )
                {
                    m_yKeypos--;
                }
            }
            else if( ( m_yKeypos != 0 ) && ( m_keyPad[m_xKeypos][m_yKeypos - 1].getRender() ) )
			{
				m_yKeypos--;
			}
			else if( ( m_yKeypos != 0 ) && ( !m_keyPad[m_xKeypos][m_yKeypos - 1].getRender() ) )
			{
				m_yKeypos = m_yKeypos - 2;
			}
            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            m_bUpdateTexture = TRUE;

            // Play the Menu Item Select Sound
            g_Sounds.PlayMenuItemSelectSound();

            if( 3 == m_yKeypos )    // Wrap the cursor
            {
                m_yKeypos = 0;
                while( !m_keyPad[m_xKeypos][m_yKeypos].getRender() )
                {
                    m_yKeypos++;
                }
            }
            else if( ( m_yKeypos != 3 ) && ( m_keyPad[m_xKeypos][m_yKeypos + 1].getRender() ) )
			{
				m_yKeypos++;
			}
			else if( ( m_yKeypos != 3 ) && ( !m_keyPad[m_xKeypos][m_yKeypos + 1].getRender() ) )
			{
                // Hack when DHCP is disabled
                if( m_keyPad[m_xKeypos][m_yKeypos + 2].getRender() )
                {
				    m_yKeypos = m_yKeypos + 2;
                }
			}
            break;
        }
    case CONTROL_DPAD_LEFT:
        {
            m_bUpdateTexture = TRUE;

            // Play the Menu Item Select Sound
            g_Sounds.PlayMenuItemSelectSound();

            if( ( 0 == m_xKeypos ) || ( !m_keyPad[m_xKeypos - 1][m_yKeypos].getRender() ) )   // Wrap the cursor
            {
                m_xKeypos = 3;
                while( !m_keyPad[m_xKeypos][m_yKeypos].getRender() )
                {
                    m_xKeypos--;
                }
            }
            else // m_xKeypos != 0
			{
				m_xKeypos--;
			}
            break;
        }
    case CONTROL_DPAD_RIGHT:
        {
            m_bUpdateTexture = TRUE;

            // Play the Menu Item Select Sound
            g_Sounds.PlayMenuItemSelectSound();

            if( ( 3 == m_xKeypos ) || ( !m_keyPad[m_xKeypos + 1][m_yKeypos].getRender() ) )    // Wrap the cursor
            {
                m_xKeypos = 0;
                while( !m_keyPad[m_xKeypos][m_yKeypos].getRender() )
                {
                    m_xKeypos++;
                }
            }
            else if( ( m_xKeypos != 3 ) && ( m_keyPad[m_xKeypos + 1][m_yKeypos].getRender())) 
			{
				m_xKeypos++;
			}
			else if((m_xKeypos != 3) && (!m_keyPad[m_xKeypos + 1][m_yKeypos].getRender()))
			{
				m_xKeypos++;
				m_yKeypos--;

                // This is a hack when we use this keypad with the DHCP button disabled
                if( !m_keyPad[m_xKeypos][m_yKeypos].getRender() )
                {
                    m_yKeypos--;
                }
			}
            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CKeypadMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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
			if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"«" ) )            //backspace
			{
				int len = wcslen( m_pwszAddress );
				
				if(len > 0)
				{
                    m_bUpdateTexture = TRUE;

                    // Play the keyboard stroke sound
                    g_Sounds.PlayKeyboardStrokeSound();

					len = len - 1;
					m_pwszAddress[len] = L'\0';
				}
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"ok" ) )      //ok
			{
                XDBGTRC( APP_TITLE_NAME_A, "CKeypadMenu::HandleInput():OK Pressed..." );

                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                // Go Back one menu
                if( GetParent() )
                {
                    Leave( GetParent() );
                }
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"cancel" ) )   //cancel
			{
                XDBGTRC( APP_TITLE_NAME_A, "CKeypadMenu::HandleInput():Cancel Pressed..." );

                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                // Go Back one menu
                if( GetParent() )
                {
                    Leave( GetParent() );
                }
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"use dhcp" ) )    //dhcp
			{
                XDBGTRC( APP_TITLE_NAME_A, "CKeypadMenu::HandleInput():Use DHCP Pressed..." );

                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                // Go Back one menu
                if( GetParent() )
                {
                    Leave( GetParent() );
                }
			}
			else
			{
                // Ensure we don't exceed the limit of an IP Address
                if( wcslen( m_pwszAddress ) < DATAVALUE_IPADDRESS_LENGTH )
                {
                    m_bUpdateTexture = TRUE;

                    // Play the keyboard stroke sound
                    g_Sounds.PlayKeyboardStrokeSound();

				    wcscat( m_pwszAddress, m_keyPad[m_xKeypos][m_yKeypos].resultChar );
                }
			}
            break;
        }
    case BUTTON_B:
        {
            XDBGTRC( APP_TITLE_NAME_A, "CKeypadMenu::HandleInput():Back Pressed..." );

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
			int len = wcslen( m_pwszAddress );
			
			if(len > 0)
			{
                m_bUpdateTexture = TRUE;

                // Play the keyboard stroke sound
                g_Sounds.PlayKeyboardStrokeSound();

				len = len - 1;
				m_pwszAddress[len] = L'\0';
			}

            break;
        }
    }
}


void CKeypadMenu::GenerateKeypad(void) 
{
	// First create numberpad
	int xgrid  = 3;
	int ygrid  = 4;
	int xstart = KEYPAD_TOPROW_X1;
	int ystart = KEYPAD_TOPROW_Y1;
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
			currentx += width + spacer;
		}
		currentx = xstart;
		currenty += height + spacer;
	}


	// Second generate larger buttons
	int xbase  = xgrid;
	xgrid  = 1;
	ygrid  = 4;
	xstart = KEYPAD_TOPROW_X1 + 113;
	ystart = KEYPAD_TOPROW_Y1;
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

			// clear out placeholder
			if(y == 2) m_keyPad[x][y].setRender(FALSE);
		}
		currentx = xstart;
		currenty += height + spacer;
	}


	// Set the text of the keys
	m_keyPad[0][0].defineText(L"1");  m_keyPad[1][0].defineText(L"2");  m_keyPad[2][0].defineText(L"3");
	m_keyPad[0][1].defineText(L"4");  m_keyPad[1][1].defineText(L"5");  m_keyPad[2][1].defineText(L"6");
	m_keyPad[0][2].defineText(L"7");  m_keyPad[1][2].defineText(L"8");	m_keyPad[2][2].defineText(L"9");
	m_keyPad[0][3].defineText(L".");  m_keyPad[1][3].defineText(L"0");	m_keyPad[2][3].defineText(L"«");
	m_keyPad[3][0].defineText(L"ok");
	m_keyPad[3][1].defineText(L"cancel");
	m_keyPad[3][2].defineText(L"place");
	m_keyPad[3][3].defineText(L"use dhcp");
}


BOOL CKeypadMenu::renderKeypad( CUDTexture* pTexture )
{
	
	// Render outlines of keys	
	float x1, x2;
	float y1, y2;
	for(int x = 0; x < KEYPAD_NUM_ROWS; x++)
	{
		for(int y = 0; y < KEYPAD_NUM_COLS; y++)
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
                    // Draw the selector box around the currently highlighted number
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


// Sets the IP address in to our local member
HRESULT CKeypadMenu::SetAddress( char* Address )
{
    if( !Address )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CKeypadMenu::SetAddress( char ):Address was not set!!" );

        return E_INVALIDARG;
    }

    ZeroMemory( m_pwszAddress, sizeof( WCHAR ) * DATAVALUE_IPADDRESS_LENGTH + 1 );
    _snwprintf( m_pwszAddress, DATAVALUE_IPADDRESS_LENGTH + 1, L"%S", Address );

    return S_OK;
}


// Sets the IP address in to our local member
HRESULT CKeypadMenu::SetAddress( WCHAR* Address )
{
    if( !Address )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CKeypadMenu::SetAddress( WCHAR ):Address was not set!!" );

        return E_INVALIDARG;
    }

    ZeroMemory( m_pwszAddress, sizeof( WCHAR ) * DATAVALUE_IPADDRESS_LENGTH + 1 );
    _snwprintf( m_pwszAddress, DATAVALUE_IPADDRESS_LENGTH + 1, Address );

    return S_OK;
}

// This will be called whenever this menu is entered / activated
void CKeypadMenu::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();

    // Make sure we don't process our first keypress, and set our item back to 0
    m_bDropKeyPress = TRUE;
    m_xKeypos = m_yKeypos = 0;

    // Default the focus to the OK button
	m_xKeypos = KEYPAD_NUM_COLS - 1;
	m_yKeypos = 0;
}