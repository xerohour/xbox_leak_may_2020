/*****************************************************
*** genkeyboardmenu.cpp
***
*** CPP file for our Generic Keyboard Menu class.
*** This is a base menu type
***
*** by James N. Helm
*** December 6th, 2000
***
*** Modified for XDK Launcher February 13th, 2002
***
*****************************************************/

#include "stdafx.h"
#include "genkeyboardmenu.h"

extern CSoundCollection g_Sounds;           // Used to generate all sounds for the XShell

// Constructors
CGenKeyboardMenu::CGenKeyboardMenu( CXItem* pParent ) :
CMenuScreen( pParent ),
m_pwszTextString( NULL ),
m_uiLength( 0 ),
m_bCancelled( FALSE ),
m_xKeypos( gc_uiGENKEYBOARD_NUM_COLS - 1 ),
m_yKeypos( 0 ),
m_ucState( 0 ),
m_bShift( FALSE ),
m_bKeyFirstPress( TRUE )
{
    ZeroMemory( m_pwszValueBuffer, sizeof( WCHAR ) * ( MAX_PATH + 1 ) );

    GenerateKeypad();

    SetTitle( MENU_TITLE_GENKEYBOARD );
}


// Destructor
CGenKeyboardMenu::~CGenKeyboardMenu()
{
    if( m_pwszTextString )
    {
        delete[] m_pwszTextString;
        m_pwszTextString = NULL;
    }
}


// Initialize the Menu
HRESULT CGenKeyboardMenu::Init( char* menuFileName )
{
    HRESULT hr = CMenuScreen::Init( menuFileName );
    if( FAILED( hr ) )
        XDBGWRN( APP_TITLE_NAME_A, "CGenKeyboardMenu::Init():Failed to initialize base class!!" );

    return hr;
}


// Draws a menu on to the screen
void CGenKeyboardMenu::Action( CUDTexture* pTexture )
{
    // Check to see if we should update our texture.  If not, bail
    if( !m_bUpdateTexture )
        return;

    // Lock our texture and set it as the render target
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    // Let our base menu class draw all the items pulled from the MNU file
    CMenuScreen::Action( pTexture );

	// Draw our keypad
	renderKeypad( pTexture );

	// Draw our User defined Text
    pTexture->DrawText( MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( m_pwszTextString ) / 2 ), (float)gc_uiGENKEYBOARD_TEXTSTRING_Y1, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls", m_pwszTextString );

    // Draw the current value on to the screen
    pTexture->DrawText( MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( m_pwszValueBuffer ) / 2 ), (float)gc_uiGENKEYBOARD_VALUE_Y1, ITEM_HEADER_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls", m_pwszValueBuffer );

    // Unlock our texture
    pTexture->Unlock();
}


// Handles input (of the CONTROLS) for the current menu
void CGenKeyboardMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
    // Process the default button delay
    if( !HandleInputDelay( bFirstPress ) )
        return;

    // Handle Buttons being pressed
    switch( controlPressed )
    {
    case CONTROL_DPAD_UP:
        {
            // Wrap the cursor
            if( 0 == m_yKeypos )
                m_yKeypos = gc_uiGENKEYBOARD_NUM_ROWS - 1;
            else // m_yKeypos > 0
                m_yKeypos--;

            for( int x = m_yKeypos; x >= 0; x-- )
            {
                if( m_keyPad[m_xKeypos][x].getRender() && ( 0 != wcscmp( m_keyPad[m_xKeypos][x].m_pwszResultChar, L"place" ) ) )
                {
                    UpdateTexture();

                    // Play the Menu Item Select Sound
                    g_Sounds.PlayMenuItemSelectSound();

                    m_yKeypos = x;

                    break;
                }
            }

            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            // Wrap the cursor
            if( ( gc_uiGENKEYBOARD_NUM_ROWS - 1 ) == m_yKeypos )
                m_yKeypos = 0;
            else // m_yKeypos < ( gc_uiGENKEYBOARD_NUM_ROWS - 1 )
                m_yKeypos++;
            
            for( int x = m_yKeypos; x < gc_uiGENKEYBOARD_NUM_ROWS; x++ )
            {
                if( m_keyPad[m_xKeypos][x].getRender() && ( wcscmp( m_keyPad[m_xKeypos][x].m_pwszResultChar, L"place" ) != 0 ) )
                {
                    UpdateTexture();

                    // Play the Menu Item Select Sound
                    g_Sounds.PlayMenuItemSelectSound();

                    m_yKeypos = x;

                    break;
                }
            }

            break;
        }
    case CONTROL_DPAD_LEFT:
        {
            // Wrap the cursor
            if( 0 == m_xKeypos )
                m_xKeypos = gc_uiGENKEYBOARD_NUM_COLS - 1;
            else // m_xKeypos > 0
                m_xKeypos--;
            
            for( int x = m_xKeypos; x >= 0; x-- )
            {
                if( m_keyPad[x][m_yKeypos].getRender() && ( wcscmp( m_keyPad[x][m_yKeypos].m_pwszResultChar, L"place" ) != 0 ) )
                {
                    UpdateTexture();

                    // Play the Menu Item Select Sound
                    g_Sounds.PlayMenuItemSelectSound();

                    m_xKeypos = x;

                    break;
                }
            }

            break;
        }
    case CONTROL_DPAD_RIGHT:
        {
            // Wrap the cursor
            if( ( gc_uiGENKEYBOARD_NUM_COLS - 1 ) == m_xKeypos )
                m_xKeypos = 0;
            else // m_xKeypos < ( gc_uiGENKEYBOARD_NUM_COLS - 1 )
                m_xKeypos++;

            for( int x = m_xKeypos; x < gc_uiGENKEYBOARD_NUM_COLS; x++ )
            {
                if( m_keyPad[x][m_yKeypos].getRender() && ( wcscmp( m_keyPad[x][m_yKeypos].m_pwszResultChar, L"place" ) != 0 ) )
                {
                    UpdateTexture();

                    // Play the Menu Item Select Sound
                    g_Sounds.PlayMenuItemSelectSound();

                    m_xKeypos = x;

                    break;
                }
            }

            break;
        }
    case CONTROL_LEFT_THUMB:    // Shift
        {
            UpdateTexture();

            m_bShift = !m_bShift;

            GenerateKeypad();

            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CGenKeyboardMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // Process the default button delay
    if( !bFirstPress )
        return;

    // Handle Buttons being pressed
    switch( buttonPressed )
    {
    case BUTTON_A:
        {
			// Deal with keyboard input
			if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].m_pwszResultChar, L"«" ) )              //backspace
			{
                m_bKeyFirstPress = FALSE;

				int len = wcslen( m_pwszValueBuffer );
				
				if( len > 0 )
				{
                    UpdateTexture();

                    // Play the keyboard stroke sound
                    g_Sounds.PlayKeyboardStrokeSound();

					m_pwszValueBuffer[len - 1] = L'\0';
				}
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].m_pwszResultChar, L"ok" ) )        //ok
			{
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                if( GetParent() )   // Go Back one menu
                    Leave( GetParent() );
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].m_pwszResultChar, L"cancel" ) )    //cancel
			{
                m_bCancelled = TRUE;

                if( GetParent() )   // Go Back one menu
                {
                    // Play the Select Sound
                    g_Sounds.PlaySelectSound();

                    Leave( GetParent() );
                }
			}
			else
			{
                if( m_bKeyFirstPress )
                {
                    m_bKeyFirstPress = FALSE;
                    ZeroMemory( m_pwszValueBuffer, sizeof( WCHAR ) * ( MAX_PATH + 1 ) );
                }

                // Ensure we don't exceed the limit of our buffer
                if( wcslen( m_pwszValueBuffer ) < m_uiLength )
                {
                    UpdateTexture();

                    // Play the keyboard stroke sound
                    g_Sounds.PlayKeyboardStrokeSound();

                    if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].m_pwszResultChar, L"space" ) )    //space bar
                        wcscat( m_pwszValueBuffer, L" " );  // Space bar
                    else    // Normal character
                    {
    				    wcscat( m_pwszValueBuffer, m_keyPad[m_xKeypos][m_yKeypos].m_pwszResultChar );

                        // Fix our shift state if necessary
                        if( m_bShift )
                        {
                            m_bShift = FALSE;
                            GenerateKeypad();
                        }
                    }
                }
			}

            break;
        }
    case BUTTON_B:
        {
            m_bCancelled = TRUE;

            if( GetParent() )   // Go Back one menu
            {
                // Play the Back Sound
                g_Sounds.PlayBackSound();

                Leave( GetParent() );
            }

            break;
        }
    case BUTTON_X:    // State Change
        {
            UpdateTexture();

            // Play the keyboard stroke sound
            g_Sounds.PlayKeyboardStrokeSound();

            m_ucState++;
            if( m_ucState >= ENUM_GENKEYBOARDSTATE_NUM_STATES )
                m_ucState = ENUM_GENKEYBOARDSTATE_FIRST_STATE;

            GenerateKeypad();

            break;
        }
    case BUTTON_WHITE:  // Backspace
        {
            m_bKeyFirstPress = FALSE;
			int len = wcslen( m_pwszValueBuffer );
			
			if( len > 0 )
			{
                UpdateTexture();

                // Play the keyboard stroke sound
                g_Sounds.PlayKeyboardStrokeSound();

				m_pwszValueBuffer[len - 1] = L'\0';
			}

            break;
        }
    case BUTTON_BLACK:  // Space
        {
            if( m_bKeyFirstPress )
            {
                m_bKeyFirstPress = FALSE;
                ZeroMemory( m_pwszValueBuffer, sizeof( WCHAR ) * ( MAX_PATH + 1 ) );
            }

            // Ensure we don't exceed the limit of an IP Address
            if( wcslen( m_pwszValueBuffer ) < m_uiLength )
            {
                UpdateTexture();

                // Play the keyboard stroke sound
                g_Sounds.PlayKeyboardStrokeSound();

                wcscat( m_pwszValueBuffer, L" " );  // Space bar
            }

            break;
        }
    }
}


// Generate the kepad we will be using
void CGenKeyboardMenu::GenerateKeypad(void) 
{
	
	// First create numberpad
	int xgrid  = gc_uiGENKEYBOARD_NUM_COLS - 1;
	int ygrid  = gc_uiGENKEYBOARD_NUM_ROWS;
	int xstart = gc_uiGENKEYBOARD_TOPROW_X1;
	int ystart = gc_uiGENKEYBOARD_TOPROW_Y1;
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
			m_keyPad[x][y].define( currentx, currenty, width, height, SCREEN_DEFAULT_TEXT_FG_COLOR );
			currentx += width + spacer;
			buffer[0] = alpha;
			m_keyPad[x][y].defineText(buffer);
			alpha++;
			
            if( ENUM_GENKEYBOARDSTATE_SYMBOLS == m_ucState )
            {
                if( 65 == alpha )
                    alpha = 33;
                if( 48 == alpha )
                    alpha = 91;
                if( 94 == alpha )
                    alpha = 95;
                if( 97 == alpha )
                    alpha = 123;
            }
            else if( ENUM_GENKEYBOARDSTATE_UPPER_ALPHA == m_ucState )
            {
                if( 58 == alpha )
                {
                    if( m_bShift )
                        alpha = 97;
                    else
                        alpha = 65;
                }
            }
            else if( ENUM_GENKEYBOARDSTATE_LOWER_ALPHA == m_ucState )
            {
                if( 58 == alpha )
                {
                    if( m_bShift )
                        alpha = 65;
                    else
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
	ygrid  = gc_uiGENKEYBOARD_NUM_ROWS;
	xstart = gc_uiGENKEYBOARD_TOPROW_X1 + 313;
	ystart = gc_uiGENKEYBOARD_TOPROW_Y1;
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
	
	m_keyPad[9][0].defineText(L"ok");
	m_keyPad[9][1].defineText(L"cancel");
	m_keyPad[9][2].defineText(L"space");
	m_keyPad[9][3].defineText(L"«");

}


// Draw our keypad
BOOL CGenKeyboardMenu::renderKeypad( CUDTexture* pTexture )
{
	
	// Render outlines of keys	
	float x1, x2;
	float y1, y2;
	for(int x = 0; x < gc_uiGENKEYBOARD_NUM_COLS; x++)
	{
		for(int y = 0; y < gc_uiGENKEYBOARD_NUM_ROWS; y++)
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
					pTexture->DrawBox( x1, y1, x2, y2, ITEM_SELECTOR_COLOR );
                    
                    dwCharColor = ITEM_SELECTED_TEXT_COLOR;
                }
                else
                {
                    dwCharColor = SCREEN_DEFAULT_TEXT_FG_COLOR;
                }

                pTexture->DrawOutline( x1, y1, x2, y2, gc_fGENKEYBOARD_LINE_WIDTH, m_keyPad[x][y].m_dwSelectColor );
				
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
void CGenKeyboardMenu::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();

    // Default the focus to the OK Button
	m_xKeypos = gc_uiGENKEYBOARD_NUM_COLS - 1;
    m_yKeypos = 0;

    // Make sure our cancelled option is set to FALSE
    m_bCancelled = FALSE;

    // Put the keyboard in it's original state
    m_bKeyFirstPress = TRUE;
    m_bShift = FALSE;
    m_ucState = ENUM_GENKEYBOARDSTATE_FIRST_STATE;
    GenerateKeypad();
}


// This should be called before the Keyboard is invoked
HRESULT CGenKeyboardMenu::SetInfo( WCHAR* pwszTextString, unsigned int uiLength, WCHAR* pwszValueDefault/*=NULL*/ )
{
    if( NULL == pwszTextString )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CGenKeyboardMenu::SetInfo():Invalid argument passed in!!" );

        return E_INVALIDARG;
    }

    if( NULL != m_pwszTextString )
    {
        delete[] m_pwszTextString;
        m_pwszTextString = NULL;
    }

    m_pwszTextString = new WCHAR[wcslen(pwszTextString)+1];
    wcscpy( m_pwszTextString, pwszTextString );

    // Set the length of the string
    // TODO: Check to make sure it will fit in the buffer!!
    m_uiLength = uiLength;

    // Copy the default value (if any) in to our value buffer
    if( NULL != pwszValueDefault )
        wcsncpy( m_pwszValueBuffer, pwszValueDefault, MAX_PATH );
    else
        m_pwszValueBuffer[0] = L'\0';

    return S_OK;
}
