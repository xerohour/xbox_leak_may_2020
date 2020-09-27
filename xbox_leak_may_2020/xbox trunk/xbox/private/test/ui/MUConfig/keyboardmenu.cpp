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

extern CXItem* g_pCurrentScreen;	// Pointer to the current menu object
extern USBManager g_USBDevices;		// Handles all USB Device input/output

// Constructors
CKeyboardMenu::CKeyboardMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_pwszTextString( NULL ),
m_pwszValueBuffer( NULL ),
m_uiValueSize( 0 ),
m_bDropKeyPress( TRUE )
{
	GenerateKeypad();

    // Default the focus to the OK Button
	m_xKeypos = KEYBOARD_NUM_COLS - 1;
    m_yKeypos = 0;
}


// Destructor
CKeyboardMenu::~CKeyboardMenu()
{
    if( m_pwszTextString )
    {
        delete[] m_pwszTextString;
        m_pwszTextString = NULL;
    }
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

	// Draw our User defined Text
    pTexture->DrawText( SCREEN_X_CENTER - ( GetStringPixelWidth( m_pwszTextString ) / 2 ), KEYBOARD_TEXT_Y1, SCREEN_DEFAULT_FOREGROUND_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", m_pwszTextString );

    // Draw the current value on to the screen
    pTexture->DrawText( SCREEN_X_CENTER - ( GetStringPixelWidth( m_pwszValueBuffer ) / 2 ), KEYBOARD_VALUE_Y1, SCREEN_DEFAULT_FOREGROUND_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", m_pwszValueBuffer );

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
            for( int x = ( m_yKeypos - 1 ); x >= 0; x-- )
            {
                if( m_keyPad[m_xKeypos][x].getRender() && ( wcscmp( m_keyPad[m_xKeypos][x].m_pwszResultChar, L"place" ) != 0 ) )
                {
                    m_bUpdateTexture = TRUE;
                    m_yKeypos = x;
                    break;
                }

            }
            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            for( int x = ( m_yKeypos + 1 ); x < KEYBOARD_NUM_ROWS; x++ )
            {
                if( m_keyPad[m_xKeypos][x].getRender() && ( wcscmp( m_keyPad[m_xKeypos][x].m_pwszResultChar, L"place" ) != 0 ) )
                {
                    m_bUpdateTexture = TRUE;
                    m_yKeypos = x;
                    break;
                }
            }
            break;
        }
    case CONTROL_DPAD_LEFT:
        {
            for( int x = ( m_xKeypos - 1 ); x >= 0; x-- )
            {
                if( m_keyPad[x][m_yKeypos].getRender() && ( wcscmp( m_keyPad[x][m_yKeypos].m_pwszResultChar, L"place" ) != 0 ) )
                {
                    m_bUpdateTexture = TRUE;
                    m_xKeypos = x;
                    break;
                }

            }
            break;
        }
    case CONTROL_DPAD_RIGHT:
        {
            for( int x = ( m_xKeypos + 1 ); x < KEYBOARD_NUM_COLS; x++ )
            {
                if( m_keyPad[x][m_yKeypos].getRender() && ( wcscmp( m_keyPad[x][m_yKeypos].m_pwszResultChar, L"place" ) != 0 ) )
                {
                    m_bUpdateTexture = TRUE;
                    m_xKeypos = x;
                    break;
                }
            }
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
			if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].m_pwszResultChar, L"«" ) )              //backspace
			{
				int len = wcslen( m_pwszValueBuffer );
				
				if( len > 0 )
				{
                    m_bUpdateTexture = TRUE;

					m_pwszValueBuffer[len - 1] = L'\0';
				}
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].m_pwszResultChar, L"ok" ) )        //ok
			{
                m_bDropKeyPress = TRUE;
                m_xKeypos = m_yKeypos = 0;
                
                // Go Back one menu
                if( GetParent() )
                {
					// g_USBDevices.Rumble( PORT_ANY, CONTROLLER_RUMBLE_SPEED, CONTROLLER_RUMBLE_MILLISECOND_TIME );

                    Leave( GetParent() );
                }
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].m_pwszResultChar, L"cancel" ) )    //cancel
			{
                m_bDropKeyPress = TRUE;
                m_xKeypos = m_yKeypos = 0;

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

                    if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].m_pwszResultChar, L"space" ) )    //space bar
                    //
                    // Space bar
                    //
                    {
                        wcscat( m_pwszValueBuffer, L" " );
                    }
                    else
                    //
                    // Normal Character
                    //
                    {
    				    wcscat( m_pwszValueBuffer, m_keyPad[m_xKeypos][m_yKeypos].m_pwszResultChar );
                    }
                }
			}
            break;
        }
    case BUTTON_B:
        {
            m_bDropKeyPress = TRUE;
            m_xKeypos = m_yKeypos = 0;

            // Set the text to nothing
            m_pwszValueBuffer[0] = '\0';

            // Go Back one menu
            if( GetParent() )
            {
                Leave( GetParent() );
            }
            break;
        }
    }
}


// Generate the kepad we will be using
void CKeyboardMenu::GenerateKeypad(void) 
{
	
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
    ZeroMemory( buffer, sizeof( WCHAR ) * 3 );

    unsigned short alpha = 48;

	for(int y = 0; y < ygrid; y++)
	{
		for(int x = 0; x < xgrid; x++)
		{
			m_keyPad[x][y].define( currentx, currenty, width, height, SCREEN_DEFAULT_FOREGROUND_COLOR );
			currentx += width + spacer;
			buffer[0] = alpha;
			m_keyPad[x][y].defineText(buffer);
			alpha++;
			
			//cycle to numbers after we hit 'Z'
			if(alpha == 58) alpha = 97;
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
			m_keyPad[x][y].define( currentx, currenty, width, height, SCREEN_DEFAULT_FOREGROUND_COLOR );
			currentx += width + spacer;
		}
		currentx = xstart;
		currenty += height + spacer;
	}
	
	m_keyPad[9][0].defineText(L"ok");
	m_keyPad[9][1].defineText(L"cancel");
	m_keyPad[9][2].defineText(L"space");
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
			if( m_keyPad[x][y].getRender() && ( wcscmp( m_keyPad[x][y].m_pwszResultChar, L"place" ) != 0 ) )
			{
                // Lock our texture so that we can render to it
                pTexture->Lock();

				x1 = (float)m_keyPad[x][y].m_iXOrigin;
				x2 = (float)(m_keyPad[x][y].m_iXOrigin + m_keyPad[x][y].m_iWidth);
				
				y1 = (float)m_keyPad[x][y].m_iYOrigin;
				y2 = (float)(m_keyPad[x][y].m_iYOrigin + m_keyPad[x][y].m_iHeight);
	
				// hilight key if currently selected
                DWORD dwCharColor;
				if((m_xKeypos == x) && (m_yKeypos == y))
                {
					pTexture->DrawBox( x1, y1, x2, y2, SCREEN_DEFAULT_SELECTOR_COLOR );
                    
                    dwCharColor = SCREEN_SELECTED_OPTION_COLOR;
                }
                else
                {
                    dwCharColor = SCREEN_DEFAULT_FOREGROUND_COLOR;
                }

                pTexture->DrawOutline( x1, y1, x2, y2, KEYBOARD_LINE_WIDTH, m_keyPad[x][y].m_dwSelectColor );
				
				unsigned int decent;
				unsigned int fontHeight;
				GetFontSize( &fontHeight, &decent );
				
				float textX = (float)((m_keyPad[x][y].m_iWidth / 2) + m_keyPad[x][y].m_iXOrigin) - GetStringPixelWidth( m_keyPad[x][y].m_pwszResultChar ) / 2;
				float textY = (float)((m_keyPad[x][y].m_iHeight / 2) + m_keyPad[x][y].m_iYOrigin) - (fontHeight / 2);
				
                pTexture->DrawText( textX, textY, dwCharColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", m_keyPad[x][y].m_pwszResultChar);

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

    // Default the focus to the OK Button
	m_xKeypos = KEYBOARD_NUM_COLS - 1;
    m_yKeypos = 0;
}


// This should be called before the Keyboard is invoked
HRESULT CKeyboardMenu::SetInfo( WCHAR* pwszTextString, WCHAR* pwszValueBuffer, unsigned int uiValueSize )
{
    // Check to make sure we were passed valid buffers
    if( ( !pwszTextString ) || ( !pwszValueBuffer ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CKeyboardMenu::SetInfo():Invalid argument passed in!!" );

        return E_INVALIDARG;
    }

    // Clean up memory if necessary
    if( m_pwszTextString )
    {
        delete[] m_pwszTextString;
        m_pwszTextString = NULL;
    }

    // Set our properties
    m_pwszTextString = _wcsdup( pwszTextString );
    m_pwszValueBuffer = pwszValueBuffer;
    m_uiValueSize = uiValueSize;

    // Clear Memory
    m_pwszValueBuffer[0] = L'\0';

    return S_OK;
}
