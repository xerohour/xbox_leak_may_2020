/*****************************************************
*** keypadmenu.cpp
***
*** CPP file for our Kaypad Menu class.
***
*** by James N. Helm
*** December 18th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "keypadmenu.h"

// Constructors
CKeypadMenu::CKeypadMenu( void ) :
m_pwszTextString( NULL ),
m_uiLength( 0 ),
m_bCancelled( FALSE ),
m_xKeypos( gc_uiKEYPAD_NUM_COLS - 1 ),
m_yKeypos( 0 ),
m_bFirstPress( TRUE )
{
    ZeroMemory( m_pwszValueBuffer, sizeof( WCHAR ) * ( MAX_PATH + 1 ) );

    GenerateKeypad();
}


// Destructor
CKeypadMenu::~CKeypadMenu( void )
{
    if( m_pwszTextString )
    {
        delete[] m_pwszTextString;
        m_pwszTextString = NULL;
    }
}


// Initialize the Menu
HRESULT CKeypadMenu::Init( XFONT* pMenuItemFont, XFONT* pMenuTitleFont )
{
    HRESULT hr = CMenuScreen::Init( pMenuItemFont, pMenuTitleFont );
    if( FAILED( hr ) )
    {
        DebugPrint( "CKeypadMenu::Init():Failed to initialize base class!!\n" );

        return hr;
    }

	// Set the menu title
	SetMenuTitle( pMenuTitleFont, L"Keypad" );

    /////////////////////////
    // Add screen items here
    /////////////////////////
    AddBitmapItem( "d:\\media\\images\\background.bmp", 0, 0 );

    // Status Bar Column 1
    AddTextItem( GetButtonFont(), L"A",       gc_fSTATUS_TEXT_COL1_XPOS,                           gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwCOLOR_A_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Select", gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( GetButtonFont(), L"B",       gc_fSTATUS_TEXT_COL1_XPOS,                           gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwCOLOR_B_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Cancel", gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );
	
    return hr;
}


// Draws a menu on to the screen
void CKeypadMenu::Action( CUDTexture* pTexture )
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
    pTexture->DrawText( GetBodyFont(), (float)gc_fMENU_AREA_HORIZ_CENTER - ( GetStringPixelWidth( GetBodyFont(), m_pwszTextString ) / 2 ), (float)gc_uiKEYPAD_TEXTSTRING_Y1, m_dwMenuItemEnabledFGColor, m_dwMenuItemEnabledBGColor, L"%ls", m_pwszTextString );

    // Draw the current value on to the screen
    pTexture->DrawText( GetBodyFont(), (float)gc_fMENU_AREA_HORIZ_CENTER - ( GetStringPixelWidth( GetBodyFont(), m_pwszValueBuffer ) / 2 ), (float)gc_uiKEYPAD_VALUE_Y1, m_dwMenuItemHighlightedFGColor, m_dwMenuItemHighlightedBGColor, L"%ls", m_pwszValueBuffer );

    // Unlock our texture
    pTexture->Unlock();
}


// Handles input (of the CONTROLS) for the current menu
void CKeypadMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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
                m_yKeypos = gc_uiKEYPAD_NUM_ROWS - 1;
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
                else if( 0 == x )
                {
                    x = gc_uiKEYPAD_NUM_ROWS;
                }
            }

            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            // Wrap the cursor
            if( ( gc_uiKEYPAD_NUM_ROWS - 1 ) == m_yKeypos )
                m_yKeypos = 0;
            else // m_yKeypos < ( gc_uiKEYPAD_NUM_ROWS - 1 )
                m_yKeypos++;
            
            for( unsigned int x = m_yKeypos; x < gc_uiKEYPAD_NUM_ROWS; x++ )
            {
                if( m_keyPad[m_xKeypos][x].getRender() && ( wcscmp( m_keyPad[m_xKeypos][x].m_pwszResultChar, L"place" ) != 0 ) )
                {
                    UpdateTexture();
                    m_yKeypos = x;

                    break;
                }
                else if( ( gc_uiKEYPAD_NUM_ROWS - 1 ) == x )
                {
                    x = -1;
                }
            }

            break;
        }
    case CONTROL_DPAD_LEFT:
        {
            // Wrap the cursor
            if( 0 == m_xKeypos )
                m_xKeypos = gc_uiKEYPAD_NUM_COLS - 1;
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
                else if( 0 == x )
                {
                    x = gc_uiKEYPAD_NUM_COLS;
                }
            }

            break;
        }
    case CONTROL_DPAD_RIGHT:
        {
            // Wrap the cursor
            if( ( gc_uiKEYPAD_NUM_COLS - 1 ) == m_xKeypos )
                m_xKeypos = 0;
            else // m_xKeypos < ( gc_uiKEYPAD_NUM_COLS - 1 )
                m_xKeypos++;

            for( unsigned int x = m_xKeypos; x < gc_uiKEYPAD_NUM_COLS; x++ )
            {
                if( m_keyPad[x][m_yKeypos].getRender() && ( wcscmp( m_keyPad[x][m_yKeypos].m_pwszResultChar, L"place" ) != 0 ) )
                {
                    UpdateTexture();
                    m_xKeypos = x;

                    break;
                }
                else if( ( gc_uiKEYPAD_NUM_COLS - 1 ) == x )
                {
                    x = -1;
                }
            }

            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CKeypadMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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

                    // If it's the first press, clear the text value
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
    /*
    case BUTTON_BLACK:  // Space
        {
            // Ensure we don't exceed the limit of an IP Address
            if( wcslen( m_pwszValueBuffer ) < m_uiLength )
            {
                UpdateTexture();
                wcscat( m_pwszValueBuffer, L" " );  // Space bar
            }

            break;
        }
    */
    }
}


// Generate the kepad we will be using
void CKeypadMenu::GenerateKeypad(void) 
{
	
	// First create numberpad
	int xgrid  = gc_uiKEYPAD_NUM_COLS - 1;
	int ygrid  = gc_uiKEYPAD_NUM_ROWS;
	int xstart = gc_uiKEYPAD_TOPROW_X1;
	int ystart = gc_uiKEYPAD_TOPROW_Y1;
	int width  = 32;
	int height = 32;
	int spacer = 2;
	int currentx = xstart;
	int currenty = ystart;

	WCHAR buffer[3];
    ZeroMemory( buffer, sizeof( WCHAR ) * 3 );

    unsigned short alpha = 49;

	for(int y = 0; y < ygrid; y++)
	{
		for(int x = 0; x < xgrid; x++)
		{
			m_keyPad[x][y].define( currentx, currenty, width, height, m_dwMenuItemEnabledFGColor );
			currentx += width + spacer;
			buffer[0] = alpha;
			m_keyPad[x][y].defineText(buffer);
			alpha++;
		}
		currentx = xstart;
		currenty += height + spacer;
	}

    m_keyPad[0][3].defineText(L"place");
    m_keyPad[1][3].defineText(L"0");
    m_keyPad[2][3].defineText(L"place");


	// Second generate larger buttons
	int xbase  = xgrid;
	xgrid  = 1;
	ygrid  = gc_uiKEYPAD_NUM_ROWS;
	xstart = gc_uiKEYPAD_TOPROW_X1 + 125;
	ystart = gc_uiKEYPAD_TOPROW_Y1;
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
	
	m_keyPad[gc_uiKEYPAD_NUM_COLS - 1][0].defineText(L"ok");
	m_keyPad[gc_uiKEYPAD_NUM_COLS - 1][1].defineText(L"cancel");
	m_keyPad[gc_uiKEYPAD_NUM_COLS - 1][2].defineText(L"place");
	m_keyPad[gc_uiKEYPAD_NUM_COLS - 1][3].defineText(L"«");

}


// Draw our keypad
BOOL CKeypadMenu::renderKeypad( CUDTexture* pTexture )
{
	
	// Render outlines of keys	
	float x1, x2;
	float y1, y2;
	for( unsigned int x = 0; x < gc_uiKEYPAD_NUM_COLS; x++ )
	{
		for( unsigned int y = 0; y < gc_uiKEYPAD_NUM_ROWS; y++ )
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

                pTexture->DrawOutline( x1, y1, x2, y2, gc_fKEYPAD_LINE_WIDTH, m_keyPad[x][y].m_dwSelectColor );
				
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
void CKeypadMenu::Enter( void )
{
    // Call the base-class enter
    CMenuScreen::Enter();

    // Default the focus to the OK Button
	m_xKeypos = gc_uiKEYPAD_NUM_COLS - 1;
    m_yKeypos = 0;

    // Make sure our cancelled option is set to FALSE
    m_bCancelled = FALSE;

    // Set the first keypress back to true
    m_bFirstPress = TRUE;
}


// This should be called before the Keyboard is invoked
HRESULT CKeypadMenu::SetInfo( WCHAR* pwszTextString, unsigned int uiLength, WCHAR* pwszValueDefault/*=NULL*/ )
{
    if( NULL == pwszTextString )
    {
        DebugPrint( "CKeypadMenu::SetInfo():Invalid argument passed in!!\n" );

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
