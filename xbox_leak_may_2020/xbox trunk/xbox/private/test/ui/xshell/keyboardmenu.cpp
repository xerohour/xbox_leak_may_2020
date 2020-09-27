/*****************************************************
*** keyboardmenu.cpp
***
*** CPP file for our Keyboard Menu class.
***
*** by James N. Helm
*** December 6th, 2000
***
*****************************************************/

#include "stdafx.h"
#include "keyboardmenu.h"

extern CXSettings       g_XboxSettings; // Settings that are stored on the Xbox
extern CSoundCollection g_Sounds;       // Used to generate all sounds for the XShell

// Constructors
CKeyboardMenu::CKeyboardMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_bDropKeyPress( TRUE ),
m_bShift( FALSE )
{
	GenerateKeypad();

    // Default the focus to the OK Button
	m_xKeypos = KEYBOARD_NUM_COLS - 1;
    m_yKeypos = 0;
}


// Destructor
CKeyboardMenu::~CKeyboardMenu()
{
}


// Draws a menu on to the screen
void CKeyboardMenu::Action( CUDTexture* pTexture )
{
    // Lock our texture and set it as the render target
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    // Let our base menu draw the items from the data file
    CMenuScreen::Action( pTexture );

	// Draw our keypad
	renderKeypad( pTexture );

    // Unlock our texture
    pTexture->Unlock();
}


// Handle the Joystick Input
void CKeyboardMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
void CKeyboardMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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
				m_yKeypos = m_yKeypos + 2;
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
                m_xKeypos = 9;
                while( !m_keyPad[m_xKeypos][m_yKeypos].getRender() )
                {
                    m_xKeypos--;
                }
            }
            else    // m_xKeypos != 0
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

            if( ( 9 == m_xKeypos ) || ( !m_keyPad[m_xKeypos + 1][m_yKeypos].getRender() ) )    // Wrap the cursor
            {
                m_xKeypos = 0;
                while( !m_keyPad[m_xKeypos][m_yKeypos].getRender() )
                {
                    m_xKeypos++;
                }
            }
            else if( ( m_xKeypos != 9 ) && ( m_keyPad[m_xKeypos + 1][m_yKeypos].getRender())) 
			{
				m_xKeypos++;
			}
			else if((m_xKeypos != 9) && (!m_keyPad[m_xKeypos + 1][m_yKeypos].getRender()))
			{
                m_xKeypos++;
				m_yKeypos--;
			}

            break;
        }
    case CONTROL_LEFT_THUMB:    // Shift
        {
            m_bUpdateTexture = TRUE;
            m_bShift = !m_bShift;

            GenerateKeypad();

            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CKeyboardMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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
			if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"«" ) )        //backspace
			{
                XDBGTRC( APP_TITLE_NAME_A, "CKeyboardMenu::HandleInput():BackSpace Selected" );

                m_bUpdateTexture = TRUE;

                // Play the keyboard stroke sound
                g_Sounds.PlayKeyboardStrokeSound();
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"ok" ) )  //ok
			{
                XDBGTRC( APP_TITLE_NAME_A, "CKeyboardMenu::HandleInput():Ok Selected" );

                // Play the Select Sound
                g_Sounds.PlaySelectSound();
                
                // Go Back one menu
                if( GetParent() )
                {
                    Leave( GetParent() );
                }
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"cancel" ) )  //cancel
			{
                XDBGTRC( APP_TITLE_NAME_A, "CKeyboardMenu::HandleInput():Cancel Selected" );

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


// Generate the kepad we will be using
void CKeyboardMenu::GenerateKeypad(void) 
{
	
	XDBGTRC( APP_TITLE_NAME_A, "Generating Alpha-Numeric Keypad" );

	// First create numberpad
	int xgrid  = KEYBOARD_NUM_COLS - 1;
	int ygrid  = KEYBOARD_NUM_ROWS;
	int xstart = KEYBOARD_TOPROW_X1;
	int ystart = KEYBOARD_TOPROW_Y1;
	int width  = 32;
	int height = 32;
	int spacer = 2;
	int currentx = xstart;
	int currenty = ystart;

	WCHAR buffer[3];
	buffer[1] = '\0';
	unsigned short alpha = 48;

	for(int y = 0; y < ygrid; y++)
	{
		for(int x = 0; x < xgrid; x++)
		{
			m_keyPad[x][y].define( currentx, currenty, width, height, SCREEN_DEFAULT_TEXT_FG_COLOR );
			currentx += width + spacer;
			buffer[0] = alpha;
			m_keyPad[x][y].defineText(buffer);
			alpha++;
			
			//cycle to letters after we hit 9
			if(alpha == 58)
            {
                if( m_bShift )
                {
                    alpha = 65;
                }
                else
                {
                    alpha = 97;
                }
            }
		}
		currentx = xstart;
		currenty += height + spacer;
	}


	// Second generate larger buttons
	int xbase  = xgrid;
	xgrid  = 1;
	ygrid  = KEYBOARD_NUM_ROWS;
	xstart = KEYBOARD_TOPROW_X1 + 313;
	ystart = KEYBOARD_TOPROW_Y1;
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
			//if(y == 2) m_keyPad[x][y].setRender(FALSE);
		}
		currentx = xstart;
		currenty += height + spacer;
	}
	
	m_keyPad[9][0].defineText(L"ok");
	m_keyPad[9][1].defineText(L"cancel");
	m_keyPad[9][2].defineText(L"-");
	m_keyPad[9][3].defineText(L"«");

}


// Draw our keypad
BOOL CKeyboardMenu::renderKeypad( CUDTexture* pTexture )
{
	
	// Render outlines of keys	
	float x1, x2;
	float y1, y2;
	for(int x = 0; x < KEYBOARD_NUM_COLS; x++)
	{
		for(int y = 0; y < KEYBOARD_NUM_ROWS; y++)
		{
			if(m_keyPad[x][y].getRender())
			{
                // Lock our texture so that we can render to it
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

                pTexture->DrawOutline( x1, y1, x2, y2, KEYBOARD_LINE_WIDTH, m_keyPad[x][y].selectColor );
				
				unsigned int decent;
				unsigned int fontHeight;
				GetFontSize( &fontHeight, &decent );
				
				float textX = (float)((m_keyPad[x][y].width / 2) + m_keyPad[x][y].xorigin) - GetStringPixelWidth( m_keyPad[x][y].resultChar ) / 2;
				float textY = (float)((m_keyPad[x][y].height / 2) + m_keyPad[x][y].yorigin) - (fontHeight / 2);
				
                pTexture->DrawText( textX, textY, dwCharColor, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", m_keyPad[x][y].resultChar);

                // Unlock our texture
                pTexture->Unlock();
			}
		}
	}
	
	return TRUE;
}

// This will be called whenever this menu is entered / activated
void CKeyboardMenu::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();

    // Make sure we don't process the first keypress, and set our menu item back to 0
    m_bDropKeyPress = TRUE;

    // Default the focus to the OK Button
	m_xKeypos = KEYBOARD_NUM_COLS - 1;
    m_yKeypos = 0;

    // Toggle the SHIFT key off
    if( m_bShift )
    {
        m_bShift = FALSE;
        GenerateKeypad();
    }
}