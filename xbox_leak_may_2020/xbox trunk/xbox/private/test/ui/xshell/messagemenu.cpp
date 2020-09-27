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

extern CSoundCollection g_Sounds;   // Used to generate all sounds for the XShell

// Constructors and Destructor
CMessageMenu::CMessageMenu( CXItem* pParent ) :
CMenuScreen( pParent )
{
    // Zero out our buffers
    for( unsigned int x = 0; x < gc_uiMESS_MENU_NUM_TEXT_LINES; x++ )
        ZeroMemory( m_apwszTextMessage[x], sizeof( WCHAR ) * ( MAX_PATH + 1 ) );

    SetTitle( MENU_TITLE_MESSAGE );
}

CMessageMenu::~CMessageMenu()
{
}

// Initialize the Menu
HRESULT CMessageMenu::Init( char* menuFileName )
{
    HRESULT hr = CMenuScreen::Init( menuFileName );
    if( FAILED( hr ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CMessageMenu::Init():Failed to initialize base class!!" );
        return hr;
    }

    return hr;
}


// Override so that we can render our own menu items
void CMessageMenu::Action( CUDTexture* pTexture )
{
    // Check to see if we should update our texture.  If not, bail
    if( !m_bUpdateTexture )
        return;

    // Lock our texture and set it as the render target
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    // Let our base menu class draw all the items pulled from the MNU file
    CMenuScreen::Action( pTexture );

	// Draw our Menu Items
	RenderMenuItems( pTexture );

    // Unlock our texture
    pTexture->Unlock();
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
		DWORD dwFGColor = SCREEN_DEFAULT_TEXT_FG_COLOR;
		DWORD dwBGColor = SCREEN_DEFAULT_TEXT_BG_COLOR;

		// Determine if the user wants to display this line of text in a different color
		if( 0 == wcsncmp( m_apwszTextMessage[x], L"/c", 2 ) )
		{
			pwszMessagePtr += 2;	// Skip the initial "/c"

			// Read the DWORD FG color out of the line
			WCHAR* pwszStopString = NULL;
			dwFGColor = wcstol( pwszMessagePtr, &pwszStopString, 10 );
			if( 0 == dwFGColor )
                XDBGWRN( APP_TITLE_NAME_A, "CMessageMenu::RenderMenuItems():Possible error in conversion of the FG color (DWORD)" );

			// Skip over the FG color and the space
			pwszMessagePtr = pwszStopString + 1;

			// Read the DWORD BG color out of the line
			dwBGColor = wcstol( pwszMessagePtr, &pwszStopString, 10 );
			if( 0 == dwBGColor )
                XDBGWRN( APP_TITLE_NAME_A, "CMessageMenu::RenderMenuItems():Possible error in conversion of the BG color (DWORD)" );

			// Skip over the BG color and the space
			pwszMessagePtr = pwszStopString + 1;
		}

		fXPos = MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( pwszMessagePtr ) / 2 );
        fYPos = gc_fMESSAGE_TEXT_YPOS + ( x * ( m_uiFontHeightOfItem + ITEM_VERT_SPACING ) );

        pTexture->DrawText( fXPos, fYPos, dwFGColor, dwBGColor, L"%ls", pwszMessagePtr );
    }

    // Render the OK Button
    int iTextWidth = GetStringPixelWidth( L"Ok" ) / 2;
    fXPos = MENUBOX_TEXTAREA_X_CENTER - iTextWidth;
    fYPos = gc_fOK_BUTTON_YPOS;

    // Draw the box and highlight
    pTexture->DrawBox( fXPos - gc_fOK_BUTTON_BORDER,
                       fYPos - gc_fOK_BUTTON_BORDER,
                       fXPos + ( iTextWidth * 2 ) + gc_fOK_BUTTON_BORDER,
                       fYPos + m_uiFontHeightOfItem + gc_fOK_BUTTON_BORDER,
                       ITEM_SELECTOR_COLOR );

    pTexture->DrawOutline( fXPos - gc_fOK_BUTTON_BORDER,
                           fYPos - gc_fOK_BUTTON_BORDER,
                           fXPos + ( iTextWidth * 2 ) + gc_fOK_BUTTON_BORDER,
                           fYPos + m_uiFontHeightOfItem + gc_fOK_BUTTON_BORDER,
                           2,
                           SCREEN_DEFAULT_TEXT_FG_COLOR );

    // Draw the text on the button
    pTexture->DrawText( fXPos, fYPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"Ok" );

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
            if( GetParent() )   // Go Back one menu
            {
                // Play the Back Sound
                g_Sounds.PlayBackSound();

                Leave( GetParent() );
            }

            break;
        }
    }
}


// Used to set the text message that will be displayed to the user
void CMessageMenu::SetText( WCHAR* pwszMessage )
{
    if( NULL == pwszMessage )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CMessageMenu::SetText():Invalid argument(s) passed in!!" );
        return;
    }

    // Clear out any current text
    ZeroMemory( m_apwszTextMessage, sizeof( m_apwszTextMessage ) );

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
}