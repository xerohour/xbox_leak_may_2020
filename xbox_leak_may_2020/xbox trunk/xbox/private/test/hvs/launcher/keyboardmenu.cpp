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

// Constructors
CKeyboardMenu::CKeyboardMenu( void ) :
m_pwszTextString( NULL ),
m_uiLength( 0 ),
m_bCancelled( FALSE ),
m_xKeypos( gc_uiKEYBOARD_NUM_COLS - 1 ),
m_yKeypos( 0 ),
m_ucState( 0 ),
m_bShift( FALSE ),
m_bFirstPress( TRUE )
{
    ZeroMemory( m_pwszValueBuffer, sizeof( WCHAR ) * ( MAX_PATH + 1 ) );

    GenerateKeypad();
}


// Destructor
CKeyboardMenu::~CKeyboardMenu( void )
{
    if( m_pwszTextString )
    {
        delete[] m_pwszTextString;
        m_pwszTextString = NULL;
    }
}


// Initialize the Menu
HRESULT CKeyboardMenu::Init( XFONT* pMenuItemFont, XFONT* pMenuTitleFont )
{
    HRESULT hr = CMenuScreen::Init( pMenuItemFont, pMenuTitleFont );
    if( FAILED( hr ) )
    {
        DebugPrint( "CKeyboardMenu::Init():Failed to initialize base class!!\n" );

        return hr;
    }

	// Set the menu title
	SetMenuTitle( pMenuTitleFont, L"Keyboard" );

    /////////////////////////
    // Add screen items here
    /////////////////////////
    AddBitmapItem( "d:\\media\\images\\background.bmp", 0, 0 );

    // Status Bar Column 1
    AddTextItem( GetButtonFont(), L"A",       gc_fSTATUS_TEXT_COL1_XPOS,                           gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwCOLOR_A_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Select", gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( GetButtonFont(), L"B",       gc_fSTATUS_TEXT_COL1_XPOS,                           gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwCOLOR_B_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Cancel", gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );

    // Status Bar Column 2
    AddTextItem( GetButtonFont(), L"C",             gc_fSTATUS_TEXT_COL2_XPOS,                           gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwCOLOR_X_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Change State", gc_fSTATUS_TEXT_COL2_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );

	
    return hr;
}


// Draws a menu on to the screen
void CKeyboardMenu::Action( CUDTexture* pTexture )
{
    // Check to see if we should update our texture.  If not, bail
    if( !m_bUpdateTexture )
        return;

    // Let our base menu draw the items from the data file
    CMenuScreen::Action( pTexture );

    // Lock our texture and set it as the render target
    pTexture->Lock();

	// Draw our keypad
	renderKeypad( pTexture );

	// Draw our User defined Text
    pTexture->DrawText( GetBodyFont(), (float)gc_fMENU_AREA_HORIZ_CENTER - ( GetStringPixelWidth( GetBodyFont(), m_pwszTextString ) / 2 ), (float)gc_uiKEYBOARD_TEXTSTRING_Y1, m_dwMenuItemEnabledFGColor, m_dwMenuItemEnabledBGColor, L"%ls", m_pwszTextString );

    // Draw the current value on to the screen
    pTexture->DrawText( GetBodyFont(), (float)gc_fMENU_AREA_HORIZ_CENTER - ( GetStringPixelWidth( GetBodyFont(), m_pwszValueBuffer ) / 2 ), (float)gc_uiKEYBOARD_VALUE_Y1, m_dwMenuItemHighlightedFGColor, m_dwMenuItemHighlightedBGColor, L"%ls", m_pwszValueBuffer );

    // Unlock our texture
    pTexture->Unlock();
}


// Handles input (of the CONTROLS) for the current menu
void CKeyboardMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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
                m_yKeypos = gc_uiKEYBOARD_NUM_ROWS - 1;
            else // m_yKeypos > 0
                m_yKeypos--;

            for( int x = m_yKeypos; x >= 0; x-- )
            {
                if( m_keyPad[m_xKeypos][x].getRender() && ( 0 != wcscmp( m_keyPad[m_xKeypos][x].m_pwszResultChar, L"place" ) ) )
                {
                    UpdateTexture();
                    m_yKeypos = x;

                    break;
                }
            }

            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            // Wrap the cursor
            if( ( gc_uiKEYBOARD_NUM_ROWS - 1 ) == m_yKeypos )
                m_yKeypos = 0;
            else // m_yKeypos < ( gc_uiKEYBOARD_NUM_ROWS - 1 )
                m_yKeypos++;
            
            for( unsigned int x = m_yKeypos; x < gc_uiKEYBOARD_NUM_ROWS; x++ )
            {
                if( m_keyPad[m_xKeypos][x].getRender() && ( wcscmp( m_keyPad[m_xKeypos][x].m_pwszResultChar, L"place" ) != 0 ) )
                {
                    UpdateTexture();
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
                m_xKeypos = gc_uiKEYBOARD_NUM_COLS - 1;
            else // m_xKeypos > 0
                m_xKeypos--;
            
            for( int x = m_xKeypos; x >= 0; x-- )
            {
                if( m_keyPad[x][m_yKeypos].getRender() && ( wcscmp( m_keyPad[x][m_yKeypos].m_pwszResultChar, L"place" ) != 0 ) )
                {
                    UpdateTexture();
                    m_xKeypos = x;

                    break;
                }
            }

            break;
        }
    case CONTROL_DPAD_RIGHT:
        {
            // Wrap the cursor
            if( ( gc_uiKEYBOARD_NUM_COLS - 1 ) == m_xKeypos )
                m_xKeypos = 0;
            else // m_xKeypos < ( gc_uiKEYBOARD_NUM_COLS - 1 )
                m_xKeypos++;

            for( unsigned int x = m_xKeypos; x < gc_uiKEYBOARD_NUM_COLS; x++ )
            {
                if( m_keyPad[x][m_yKeypos].getRender() && ( wcscmp( m_keyPad[x][m_yKeypos].m_pwszResultChar, L"place" ) != 0 ) )
                {
                    UpdateTexture();
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
void CKeyboardMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // Process the default button delay
    if( !HandleInputDelay( bFirstPress ) )
        return;

    // Handle Buttons being pressed
    switch( buttonPressed )
    {
    case BUTTON_A:
        {
			// Deal with keyboard input
			if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].m_pwszResultChar, L"«" ) )              //backspace
			{
                m_bFirstPress = FALSE;

				int len = wcslen( m_pwszValueBuffer );
				
				if( len > 0 )
				{
                    UpdateTexture();

					m_pwszValueBuffer[len - 1] = L'\0';
				}
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].m_pwszResultChar, L"ok" ) )        //ok
			{
                GoBack();
			}
			else if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].m_pwszResultChar, L"cancel" ) )    //cancel
			{
                m_bCancelled = TRUE;

                GoBack();
			}
			else
			{
                // Ensure we don't exceed the limit of our buffer
                if( wcslen( m_pwszValueBuffer ) < m_uiLength )
                {
                    UpdateTexture();

                    if( m_bFirstPress )
                    {
                        m_bFirstPress = FALSE;
                        ZeroMemory( m_pwszValueBuffer, sizeof( WCHAR ) * ( MAX_PATH + 1 ) );
                    }

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

            GoBack();

            break;
        }
    case BUTTON_X:    // State Change
        {
            UpdateTexture();

            m_ucState++;
            if( m_ucState >= ENUM_KEYBOARDSTATE_NUM_STATES )
                m_ucState = ENUM_KEYBOARDSTATE_FIRST_STATE;

            GenerateKeypad();

            break;
        }
    case BUTTON_WHITE:  // Backspace
        {
            m_bFirstPress = FALSE;
			int len = wcslen( m_pwszValueBuffer );
			
			if( len > 0 )
			{
                UpdateTexture();

				m_pwszValueBuffer[len - 1] = L'\0';
			}

            break;
        }
    case BUTTON_BLACK:  // Space
        {
            if( m_bFirstPress )
            {
                m_bFirstPress = FALSE;
                ZeroMemory( m_pwszValueBuffer, sizeof( WCHAR ) * ( MAX_PATH + 1 ) );
            }

            // Ensure we don't exceed the limit of an IP Address
            if( wcslen( m_pwszValueBuffer ) < m_uiLength )
            {
                UpdateTexture();
                wcscat( m_pwszValueBuffer, L" " );  // Space bar
            }

            break;
        }
    }
}


// Generate the kepad we will be using
void CKeyboardMenu::GenerateKeypad(void) 
{
	
	// First create numberpad
	int xgrid  = gc_uiKEYBOARD_NUM_COLS - 1;
	int ygrid  = gc_uiKEYBOARD_NUM_ROWS;
	int xstart = gc_uiKEYBOARD_TOPROW_X1;
	int ystart = gc_uiKEYBOARD_TOPROW_Y1;
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
			m_keyPad[x][y].define( currentx, currenty, width, height, m_dwMenuItemEnabledFGColor );
			currentx += width + spacer;
			buffer[0] = alpha;
			m_keyPad[x][y].defineText(buffer);
			alpha++;
			
            if( ENUM_KEYBOARDSTATE_SYMBOLS == m_ucState )
            {
                if( 65 == alpha )
                    alpha = 33;
                if( 48 == alpha )
                    alpha = 91;
                if( 97 == alpha )
                    alpha = 123;
            }
            else if( ENUM_KEYBOARDSTATE_UPPER_ALPHA == m_ucState )
            {
                if( 58 == alpha )
                {
                    if( m_bShift )
                        alpha = 97;
                    else
                        alpha = 65;
                }
            }
            else if( ENUM_KEYBOARDSTATE_LOWER_ALPHA == m_ucState )
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
	ygrid  = gc_uiKEYBOARD_NUM_ROWS;
	xstart = gc_uiKEYBOARD_TOPROW_X1 + 313;
	ystart = gc_uiKEYBOARD_TOPROW_Y1;
	width  = 103;
	height = 32;
	spacer = 2;
	currentx = xstart;
	currenty = ystart;

	for(y = 0; y < ygrid; y++)
	{
		for(int x = xbase; x < (xbase + xgrid); x++)
		{
			m_keyPad[x][y].define( currentx, currenty, width, height, m_dwMenuItemEnabledFGColor );
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
	for( unsigned int x = 0; x < gc_uiKEYBOARD_NUM_COLS; x++ )
	{
		for( unsigned int y = 0; y < gc_uiKEYBOARD_NUM_ROWS; y++ )
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
					pTexture->DrawBox( x1, y1, x2, y2, m_dwMenuItemSelectorColor );
                    
                    dwCharColor = m_dwSelectedItemFGColor;
                }
                else
                {
                    dwCharColor = m_dwMenuItemEnabledFGColor;
                }

                pTexture->DrawOutline( x1, y1, x2, y2, gc_fKEYBOARD_LINE_WIDTH, m_keyPad[x][y].m_dwSelectColor );
				
				unsigned int decent;
				unsigned int fontHeight;
				GetFontSize( GetBodyFont(), &fontHeight, &decent );
				
				float textX = (float)((m_keyPad[x][y].m_iWidth / 2) + m_keyPad[x][y].m_iXOrigin) - GetStringPixelWidth( GetBodyFont(), m_keyPad[x][y].m_pwszResultChar ) / 2;
				float textY = (float)((m_keyPad[x][y].m_iHeight / 2) + m_keyPad[x][y].m_iYOrigin) - (fontHeight / 2);
				
                pTexture->DrawText( GetBodyFont(), textX, textY, dwCharColor, m_dwMenuItemEnabledBGColor, L"%s", m_keyPad[x][y].m_pwszResultChar);

                // Unlock our texture
                pTexture->Unlock();
			}
		}
	}
	
	return TRUE;
}


// This will be called whenever this menu is entered / activated
void CKeyboardMenu::Enter( void )
{
    // Call the base-class enter
    CMenuScreen::Enter();

    // Default the focus to the OK Button
	m_xKeypos = gc_uiKEYBOARD_NUM_COLS - 1;
    m_yKeypos = 0;

    // Make sure our cancelled option is set to FALSE
    m_bCancelled = FALSE;

    // Put the keyboard in it's original state
    m_bFirstPress = TRUE;
    m_bShift = FALSE;
    m_ucState = ENUM_KEYBOARDSTATE_FIRST_STATE;
    GenerateKeypad();
}


// This should be called before the Keyboard is invoked
HRESULT CKeyboardMenu::SetInfo( WCHAR* pwszTextString, unsigned int uiLength, WCHAR* pwszValueDefault/*=NULL*/ )
{
    if( NULL == pwszTextString )
    {
        DebugPrint( "CKeyboardMenu::SetInfo():Invalid argument passed in!!\n" );

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
