/*****************************************************
*** errormsgmenu.cpp
***
*** CPP file for our XShell Error Message class.
*** This screen will display an error message and an
*** "ok" menu choice
***
*** by James N. Helm
*** January 17th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "errormsgmenu.h"

extern CSoundCollection g_Sounds;           // Used to generate all sounds for the XShell

// Constructors

CErrorMsgMenu::CErrorMsgMenu( CXItem* pParent )
: CKeypadMenu( pParent ),
m_dwErrorCode( ERRORCODE_NO_ERROR )
{
    XDBGTRC( APP_TITLE_NAME_A, "CErrorMsgMenu::CErrorMsgMenu(ex)" );

    GenerateKeypad();
	m_xKeypos = ERRORMSGMENU_NUM_COLS - 1;
	m_yKeypos = 0;

    SetTitle( MENU_TITLE_ERRORMSG );
}


// Destructor
CErrorMsgMenu::~CErrorMsgMenu()
{
}

// Draws a menu on to the screen
void CErrorMsgMenu::Action( CUDTexture* pTexture )
{
    // Check to see if we should update our texture, if not, return
    if( !m_bUpdateTexture )
    {
        return;
    }
    else // Reset the Update flag
    {
        m_bUpdateTexture = FALSE;
    }

    // Lock our texture so we can render to it
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    // Let our base menu draw the items from the data file
    CKeypadMenu::Action( pTexture );

    // Display the error message specified to the user
    pTexture->DrawText( MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( g_pwszERRORCODEMSG[m_dwErrorCode] ) / 2.0f ), ERRORMSGMENU_MSG_YPOS, ERRORMSGMENU_MSG_FG_COLOR, ERRORMSGMENU_MSG_BG_COLOR, L"%s", g_pwszERRORCODEMSG[m_dwErrorCode] );

    // Lock our texture so we can render to it
    pTexture->Unlock();
}


void CErrorMsgMenu::GenerateKeypad(void) 
{
	// First create numberpad
	int xgrid  = 1;
	int ygrid  = ERRORMSGMENU_NUM_ROWS;
	int xstart = ERRORMSGMENU_BUTTON_X1;
	int ystart = ERRORMSGMENU_BUTTON_Y1;
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
	ygrid  = ERRORMSGMENU_NUM_ROWS;
	xstart = 323;
	ystart = ERRORMSGMENU_BUTTON_Y1;
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

	// Set the text of the key
    m_keyPad[0][0].defineText( ERRORMSGMENU_BUTTON1_TEXT );
}


BOOL CErrorMsgMenu::renderKeypad( CUDTexture* pTexture )
{
	// Render outlines of keys	
	float x1, x2;
	float y1, y2;
	for(int x = 0; x < ERRORMSGMENU_NUM_COLS; x++)
	{
		for(int y = 0; y < ERRORMSGMENU_NUM_ROWS; y++)
		{
			if(m_keyPad[x][y].getRender())
			{
                // Lock our texture so we can render to it
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
void CErrorMsgMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
void CErrorMsgMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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
			if( ( m_yKeypos != ( ERRORMSGMENU_NUM_ROWS - 1 ) ) && ( m_keyPad[m_xKeypos][m_yKeypos + 1].getRender() ) )
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
void CErrorMsgMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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
			if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].resultChar, ERRORMSGMENU_BUTTON1_TEXT ) )		//Ok
			{
                m_bDropKeyPress = TRUE;
                m_xKeypos = m_yKeypos = 0;

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


// This will be called whenever the user enters this menu
void CErrorMsgMenu::Enter()
{
    // Call the base-class enter
    CKeypadMenu::Enter();

    // Set the focus to our OK button
	m_xKeypos = ERRORMSGMENU_NUM_COLS - 1;
	m_yKeypos = 0;
}