/*****************************************************
*** yesnomenu.h
***
*** CPP file for our Yes/No Menu class.
*** This will allow the user to select Yes or No
***
*** by James N. Helm
*** December 13th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "yesnomenu.h"

// Constructors
CYesNoMenu::CYesNoMenu( void ) :
m_bCancelled( FALSE ),
m_xKeypos( 0 ),
m_yKeypos( 0 )
{
    // Zero out our buffers
    for( unsigned int x = 0; x < gc_uiNUM_TEXT_LINES; x++ )
        ZeroMemory( m_apwszTextMessage[x], sizeof( WCHAR ) * ( MAX_PATH + 1 ) );

    GenerateKeypad();
}


// Destructor
CYesNoMenu::~CYesNoMenu( void )
{
}


// Initialize the Menu
HRESULT CYesNoMenu::Init( XFONT* pMenuItemFont, XFONT* pMenuTitleFont )
{
    HRESULT hr = CMenuScreen::Init( pMenuItemFont, pMenuTitleFont );
    if( FAILED( hr ) )
    {
        DebugPrint( "CYesNoMenu::Init():Failed to initialize base class!!\n" );

        return hr;
    }

	// Set the menu title
	SetMenuTitle( pMenuTitleFont, L"Yes / No" );

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
void CYesNoMenu::Action( CUDTexture* pTexture )
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

    // Render the text message
    for( unsigned int x = 0; x < gc_uiNUM_TEXT_LINES; x++ )
    {
		WCHAR* pwszMessagePtr = m_apwszTextMessage[x];
		DWORD dwFGColor = m_dwMenuItemEnabledFGColor;
		DWORD dwBGColor = m_dwMenuItemEnabledBGColor;

		// Determine if the user wants to display this line of text in a different color
		if( 0 == wcsncmp( m_apwszTextMessage[x], L"/c", 2 ) )
		{
			pwszMessagePtr += 2;	// Skip the initial "/c"

			// Read the DWORD FG color out of the line
			WCHAR* pwszStopString = NULL;
			dwFGColor = wcstol( pwszMessagePtr, &pwszStopString, 10 );
			if( 0 == dwFGColor )
				DebugPrint( "CMessageMenu::RenderMenuItems():Possible error in conversion of the FG color (DWORD)\n" );

			// Skip over the FG color and the space
			pwszMessagePtr = pwszStopString + 1;

			// Read the DWORD BG color out of the line
			dwBGColor = wcstol( pwszMessagePtr, &pwszStopString, 10 );
			if( 0 == dwBGColor )
				DebugPrint( "CMessageMenu::RenderMenuItems():Possible error in conversion of the BG color (DWORD)\n" );

			// Skip over the BG color and the space
			pwszMessagePtr = pwszStopString + 1;
		}

        float fXPos = gc_fMENU_AREA_HORIZ_CENTER - ( GetStringPixelWidth( GetBodyFont(), pwszMessagePtr ) / 2 );
        float fYPos = gc_fYESNO_TEXT_YPOS + ( x * ( m_uiMenuItemFontHeight + m_fMenuItemVertSpace ) );

        pTexture->DrawText( GetBodyFont(), fXPos, fYPos, dwFGColor, dwBGColor, L"%ls", pwszMessagePtr );
    }

    // Unlock our texture
    pTexture->Unlock();
}


// Handles input (of the CONTROLS) for the current menu
void CYesNoMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
    // Process the default button delay
    if( !HandleInputDelay( bFirstPress ) )
        return;

    // Handle Buttons being pressed
    switch( controlPressed )
    {
    case CONTROL_DPAD_LEFT:
        {
            // Wrap the cursor
            if( 0 == m_xKeypos )
                m_xKeypos = gc_uiYESNO_NUM_COLS - 1;
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
            if( ( gc_uiYESNO_NUM_COLS - 1 ) == m_xKeypos )
                m_xKeypos = 0;
            else // m_xKeypos < ( gc_uiYESNO_NUM_COLS - 1 )
                m_xKeypos++;

            for( unsigned int x = m_xKeypos; x < gc_uiYESNO_NUM_COLS; x++ )
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
	}
}


// Handles input (of the BUTTONS) for the current menu
void CYesNoMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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
			if( 0 == wcscmp( m_keyPad[m_xKeypos][m_yKeypos].m_pwszResultChar, L"No" ) )    // No
                m_bCancelled = TRUE;

            GoBack();

            break;
        }
    case BUTTON_B:
        {
            m_bCancelled = TRUE;
            
            GoBack();

            break;
        }
    }
}


// Generate the kepad we will be using
void CYesNoMenu::GenerateKeypad(void) 
{
	
	// Create the buttons
	int xstart = gc_uiYESNO_TOPROW_X1;
	int ystart = gc_uiYESNO_TOPROW_Y1;
	int width  = gc_uiYESNO_BUTTON_WIDTH;
	int height = gc_uiYESNO_BUTTON_HEIGHT;
	int spacer = gc_uiYESNO_BUTTON_SPACER;
	int currentx = xstart;
	int currenty = ystart;

    for(unsigned int y = 0; y < gc_uiYESNO_NUM_ROWS; y++ )
	{
		for( unsigned int x = 0; x < gc_uiYESNO_NUM_COLS; x++ )
		{
			m_keyPad[x][y].define( currentx, currenty, width, height, m_dwMenuItemEnabledFGColor );
			currentx += width + spacer;
		}
		currentx = xstart;
		currenty += height + spacer;
	}

    m_keyPad[0][0].defineText( L"Yes" );
    m_keyPad[1][0].defineText( L"No" );
}


// Draw our keypad
BOOL CYesNoMenu::renderKeypad( CUDTexture* pTexture )
{
	
	// Render outlines of keys	
	float x1, x2;
	float y1, y2;
	for( unsigned int x = 0; x < gc_uiYESNO_NUM_COLS; x++ )
	{
		for( unsigned int y = 0; y < gc_uiYESNO_NUM_ROWS; y++ )
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

                pTexture->DrawOutline( x1, y1, x2, y2, gc_fYESNO_LINE_WIDTH, m_keyPad[x][y].m_dwSelectColor );
				
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
void CYesNoMenu::Enter( void )
{
    // Call the base-class enter
    CMenuScreen::Enter();

    // Default the focus to the NO Button
	m_xKeypos = 1;
    m_yKeypos = 0;

    // Make sure our cancelled option is set to FALSE
    m_bCancelled = FALSE;
}


// Used to set the text message that will be displayed to the user
void CYesNoMenu::SetText( WCHAR* pwszMessage )
{
    if( NULL == pwszMessage )
    {
        DebugPrint( "CYesNoMenu::SetText():Invalid argument(s) passed in!!\n" );

        return;
    }

    for( unsigned int x = 0; x < gc_uiNUM_TEXT_LINES; x++ )
    {
        unsigned int uiCount = 0;
        while( ( *pwszMessage != L'\n' ) && ( *pwszMessage != L'\0' ) )
        {
            m_apwszTextMessage[x][uiCount] = *pwszMessage;
            uiCount++;
            pwszMessage++;

            // Exit when we find the end of the current string
            if( uiCount == MAX_PATH + 1 )
                break;
        }
        m_apwszTextMessage[x][uiCount] = L'\0';

        // Exit early if we are at the end of the message
        if( *pwszMessage == L'\0' )
            break;

        uiCount++;
        pwszMessage++;
    }

    /*
    for( x = 0; x < gc_uiNUM_TEXT_LINES; x++ )
    {
        DebugPrint("m_apwszTextMessage[x] - '%ls'\n", m_apwszTextMessage[x] );
        DebugPrint( "Length - '%d'\n", wcslen( m_apwszTextMessage[x] ) );
    }
    */
}