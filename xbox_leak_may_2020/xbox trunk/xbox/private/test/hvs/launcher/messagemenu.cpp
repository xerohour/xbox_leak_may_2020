/*****************************************************
*** messagemenu.h
***
*** CPP file for our Message Menu class.
*** This menu will present a message to the user, and
*** simply allow them to navigate back from where they
*** came.
***
*** by James N. Helm
*** December 11th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "messagemenu.h"

// Constructors and Destructor
CMessageMenu::CMessageMenu( void )
{
    // Zero out our buffers
    for( unsigned int x = 0; x < gc_uiMESS_MENU_NUM_TEXT_LINES; x++ )
        ZeroMemory( m_apwszTextMessage[x], sizeof( WCHAR ) * ( MAX_PATH + 1 ) );
}

CMessageMenu::~CMessageMenu( void )
{
}

// Initialize the Menu
HRESULT CMessageMenu::Init( XFONT* pMenuItemFont, XFONT* pMenuTitleFont )
{
    HRESULT hr = CMenuScreen::Init( pMenuItemFont, pMenuTitleFont );
    if( FAILED( hr ) )
    {
        DebugPrint( "CMessageMenu::Init():Failed to initialize base class!!\n" );

        return hr;
    }

	// Set the menu title
	SetMenuTitle( pMenuTitleFont, L"Message" );

    /////////////////////////
    // Add screen items here
    /////////////////////////
    AddBitmapItem( "d:\\media\\images\\background.bmp", 0, 0 );
	
    // Status Bar Column 1
    // AddBitmapAlphaItem( "d:\\media\\images\\Abutton.bmp", "d:\\media\\images\\buttonalpha.bmp",     (int)gc_fSTATUS_TEXT_COL1_XPOS, (int)gc_fSTATUS_TEXT_ROW1_YPOS );
    // AddBitmapAlphaItem( "d:\\media\\images\\Bbutton.bmp", "d:\\media\\images\\buttonalpha.bmp",     (int)gc_fSTATUS_TEXT_COL1_XPOS, (int)gc_fSTATUS_TEXT_ROW1_YPOS );
    // AddBitmapItem( "d:\\media\\images\\Bbutton.bmp", (int)gc_fSTATUS_TEXT_COL1_XPOS, (int)gc_fSTATUS_TEXT_ROW1_YPOS );
    // AddBitmapItem( "d:\\media\\images\\Abutton.bmp", (int)gc_fSTATUS_TEXT_COL1_XPOS, (int)gc_fSTATUS_TEXT_ROW1_YPOS );
    AddTextItem( GetButtonFont(), L"A",        gc_fSTATUS_TEXT_COL1_XPOS,                           gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwCOLOR_A_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Select",  gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( GetButtonFont(), L"B",        gc_fSTATUS_TEXT_COL1_XPOS,                           gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwCOLOR_B_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Back",    gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );

    ///////////////////////
	// Add menu items here
    ///////////////////////

    return hr;
}


// Render the menu items properly on our menu
void CMessageMenu::RenderMenuItems( CUDTexture* pTexture )
{
    float fXPos = 0.0f;
    float fYPos = 0.0f;

    // Render the text message
    for( unsigned int x = 0; x < gc_uiMESS_MENU_NUM_TEXT_LINES; x++ )
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

		fXPos = gc_fMENU_AREA_HORIZ_CENTER - ( GetStringPixelWidth( GetBodyFont(), pwszMessagePtr ) / 2 );
        fYPos = gc_fMESSAGE_TEXT_YPOS + ( x * ( m_uiMenuItemFontHeight + m_fMenuItemVertSpace ) );

        pTexture->DrawText( GetBodyFont(), fXPos, fYPos, dwFGColor, dwBGColor, L"%ls", pwszMessagePtr );
    }

    // Render the OK Button
    int iTextWidth = GetStringPixelWidth( GetBodyFont(), L"Ok" ) / 2;
    fXPos = gc_fMENU_AREA_HORIZ_CENTER - iTextWidth;
    fYPos = gc_fOK_BUTTON_YPOS;

    // Draw the box and highlight
    pTexture->DrawBox( fXPos - gc_fOK_BUTTON_BORDER,
                       fYPos - gc_fOK_BUTTON_BORDER,
                       fXPos + ( iTextWidth * 2 ) + gc_fOK_BUTTON_BORDER,
                       fYPos + m_uiMenuItemFontHeight + gc_fOK_BUTTON_BORDER,
                       gc_dwCOLOR_SELECTOR_BLACK );

    pTexture->DrawOutline( fXPos - gc_fOK_BUTTON_BORDER,
                           fYPos - gc_fOK_BUTTON_BORDER,
                           fXPos + ( iTextWidth * 2 ) + gc_fOK_BUTTON_BORDER,
                           fYPos + m_uiMenuItemFontHeight + gc_fOK_BUTTON_BORDER,
                           2,
                           m_dwMenuItemEnabledFGColor );

    // Draw the text on the button
    pTexture->DrawText( GetBodyFont(), fXPos, fYPos, m_dwSelectedItemFGColor, m_dwSelectedItemBGColor, L"Ok" );

}


// Handles input (of the BUTTONS) for the current menu
void CMessageMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // If it's a repeat press, let's bail
    if( !bFirstPress )
        return;

    // Handle Buttons being pressed
    switch( buttonPressed )
    {
    case BUTTON_A:
    case BUTTON_B:
        {
            GoBack();

            break;
        }
    }
}


// Used to set the text message that will be displayed to the user
void CMessageMenu::SetText( WCHAR* pwszMessage )
{
    if( NULL == pwszMessage )
    {
        DebugPrint( "CMessageMenu::SetText():Invalid argument(s) passed in!!\n" );

        return;
    }

    for( unsigned int x = 0; x < gc_uiMESS_MENU_NUM_TEXT_LINES; x++ )
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
    for( x = 0; x < gc_uiMESS_MENU_NUM_TEXT_LINES; x++ )
    {
        DebugPrint("m_apwszTextMessage[x] - '%ls'\n", m_apwszTextMessage[x] );
        DebugPrint( "Length - '%d'\n", wcslen( m_apwszTextMessage[x] ) );
    }
    */
}