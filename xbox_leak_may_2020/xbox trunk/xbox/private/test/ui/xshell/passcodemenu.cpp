/*****************************************************
*** passcodemenu.h
***
*** Header file for our Passcode Menu class.
***
*** by James N. Helm
*** February 13th, 2002
***
*****************************************************/

#include "stdafx.h"
#include "passcodemenu.h"

extern CSoundCollection g_Sounds;   // Used to generate all sounds for the XShell
extern USBManager Controllers;      // Used to determine what buttons / direction the user is pressing
extern BOOL g_bToggleIPInfo;        // Menu's can use this to turn off toggling of the IP Address info

// Constructors
CPassCodeMenu::CPassCodeMenu( CXItem* pParent ) :
m_usPassCodeIndex( 0 ),
CMenuScreen( pParent ),
m_bCancelled( FALSE ),
m_pButtonFont( NULL ),
m_bDisplayInvalidMsg( FALSE )
{
    ZeroMemory( &m_pbyPassCode, XONLINE_PIN_LENGTH );

    SetTitle( MENU_TITLE_PASSCODE );
}


// Destructor
CPassCodeMenu::~CPassCodeMenu()
{
    if( NULL != m_pButtonFont )
    {
        XFONT_Release( m_pButtonFont );
        m_pButtonFont = NULL;
    }
}


// Initialize the Menu
HRESULT CPassCodeMenu::Init( char* menuFileName )
{
    HRESULT hr = CMenuScreen::Init( menuFileName );
    if( FAILED( hr ) )
        XDBGWRN( APP_TITLE_NAME_A, "CPassCodeMenu::Init():Failed to initialize base class!!" );

    // Open the Button Font for this menu
    XFONT_OpenTrueTypeFont( FILE_DATA_BUTTON_FONT_FILENAME, 4096, &m_pButtonFont );

    if( NULL == m_pButtonFont )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CPassCodeMenu::CPassCodeMenu():Failed to open the Button Font!!" );
    }
    else
    {
        XFONT_SetTextColor( m_pButtonFont, SCREEN_DEFAULT_TEXT_FG_COLOR );
        XFONT_SetBkColor( m_pButtonFont, SCREEN_DEFAULT_TEXT_BG_COLOR );
        XFONT_SetTextHeight( m_pButtonFont, 30 );
        XFONT_SetTextAntialiasLevel( m_pButtonFont, FONT_ALIAS_LEVEL );
        XFONT_SetTextStyle( m_pButtonFont, FONT_DEFAULT_STYLE );
    }

    return hr;
}


// Draws a menu on to the screen
void CPassCodeMenu::Action( CUDTexture* pTexture )
{
    // Check to see if we should update our texture.  If not, bail
    if( !m_bUpdateTexture )
        return;

    // Lock our texture and set it as the render target
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    // Let our base menu class draw all the items pulled from the MNU file
    CMenuScreen::Action( pTexture );

    // Draw our enter message
    float fXPos = MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( MENU_PASSCODE_ENTER_CODE_MSG ) / 2 );
    float fYPos = 24;
    pTexture->DrawText( fXPos, fYPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls", MENU_PASSCODE_ENTER_CODE_MSG );

    // Draw our invalid message if needed
    if( m_bDisplayInvalidMsg )
    {
        fXPos = MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( MENU_PASSCODE_INVALID_MSG ) / 2 );
        fYPos += m_uiFontHeightOfItem + ITEM_VERT_SPACING;
        pTexture->DrawText( fXPos, fYPos, COLOR_TVSAFE_RED, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls", MENU_PASSCODE_INVALID_MSG );
    }

    // Draw our help text
    fXPos = MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( MENU_PASSCODE_HELP_TEXT1 ) / 2 );
    fYPos = 170;
    pTexture->DrawText( fXPos, fYPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls", MENU_PASSCODE_HELP_TEXT1 );
    
    // Draw boxes to surround our passcode
    float fBoxStartXPos = 60;
    float fBoxStartYPos = 90;
    float fBoxBuffer = 30;
    float fBoxWidth = 60;
    float fBoxHeight = 60;

    fXPos = fBoxStartXPos;
    fYPos = fBoxStartYPos;
    for( unsigned int x = 0; x < XONLINE_PIN_LENGTH; x++ )
    {
        pTexture->DrawOutline( fXPos, fYPos, fXPos + fBoxWidth, fYPos + fBoxHeight, 2, SCREEN_DEFAULT_TEXT_FG_COLOR );
        fXPos += fBoxWidth + fBoxBuffer;
    }


    // Draw our current passcode
    fXPos = fBoxStartXPos + 13;
    fYPos = fBoxStartYPos + 15;
    char cDisplayChar = 0;
    DWORD dwFGColor;
    for( unsigned int x = 0; x < XONLINE_PIN_LENGTH; x++ )
    {
        // If the code is 0, then the user has not yet entered a value for it
        if( 0 == m_pbyPassCode[x] )
            break;

        switch( m_pbyPassCode[x] )
        {
        case 1: // DPAD UP
            {
                dwFGColor = COLOR_DPAD_UP_BUTTON;
                cDisplayChar = 'Q';
                break;
            }
        case 2: // DPAD DOWN
            {
                dwFGColor = COLOR_DPAD_DOWN_BUTTON;
                cDisplayChar = 'M';
                break;
            }
        case 3: // DPAD LEFT
            {
                dwFGColor = COLOR_DPAD_LEFT_BUTTON;
                cDisplayChar = 'K';
                break;
            }
        case 4: // DPAD RIGHT
            {
                dwFGColor = COLOR_DPAD_RIGHT_BUTTON;
                cDisplayChar = 'O';
                break;
            }
        case 5: // X BUTTON
            {
                dwFGColor = COLOR_X_BUTTON;
                cDisplayChar = 'C';
                break;
            }
        case 6: // Y BUTTON
            {
                dwFGColor = COLOR_Y_BUTTON;
                cDisplayChar = 'D';
                break;
            }
        case 7: // BLACK BUTTON
            {
                dwFGColor = COLOR_BLACK_BUTTON;
                cDisplayChar = 'J';
                break;
            }
        case 8: // WHITE BUTTON
            {
                dwFGColor = COLOR_WHITE_BUTTON;
                cDisplayChar = 'I';
                break;
            }
        case 9: // LEFT TRIGGER BUTTON
            {
                dwFGColor = COLOR_L_BUTTON;
                cDisplayChar = 'E';
                break;
            }
        case 10: // RIGHT TRIGGER BUTTON
            {
                dwFGColor = COLOR_R_BUTTON;
                cDisplayChar = 'F';
                break;
            }
        }
        
        pTexture->DrawText( m_pButtonFont, fXPos, fYPos, dwFGColor, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%hc", cDisplayChar );
        fXPos += fBoxWidth + fBoxBuffer;
    }

    // Unlock our texture
    pTexture->Unlock();
}


// Handles input (of the CONTROLS) for the current menu
void CPassCodeMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
    // Process the default button delay
    if( !bFirstPress )
        return;

    m_bDisplayInvalidMsg = FALSE;

    // Handle Buttons being pressed
    switch( controlPressed )
    {
    case CONTROL_DPAD_UP:
        {
            if( m_usPassCodeIndex < XONLINE_PIN_LENGTH )
            {
                // Play the keyboard stroke sound
                g_Sounds.PlayKeyboardStrokeSound();

                m_pbyPassCode[m_usPassCodeIndex++] = 1;
            }

            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            if( m_usPassCodeIndex < XONLINE_PIN_LENGTH )
            {
                // Play the keyboard stroke sound
                g_Sounds.PlayKeyboardStrokeSound();

                m_pbyPassCode[m_usPassCodeIndex++] = 2;
            }

            break;
        }
    case CONTROL_DPAD_LEFT:
        {
            if( m_usPassCodeIndex < XONLINE_PIN_LENGTH )
            {
                // Play the keyboard stroke sound
                g_Sounds.PlayKeyboardStrokeSound();

                m_pbyPassCode[m_usPassCodeIndex++] = 3;
            }

            break;
        }
    case CONTROL_DPAD_RIGHT:
        {
            if( m_usPassCodeIndex < XONLINE_PIN_LENGTH )
            {
                // Play the keyboard stroke sound
                g_Sounds.PlayKeyboardStrokeSound();

                m_pbyPassCode[m_usPassCodeIndex++] = 4;
            }

            break;
        }
    case CONTROL_LEFT_THUMB:
        {
            UpdateTexture();

            m_usPassCodeIndex = 0;
            ZeroMemory( &m_pbyPassCode, XONLINE_PIN_LENGTH );

            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CPassCodeMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // Process the default button delay
    if( !bFirstPress )
        return;

    m_bDisplayInvalidMsg = FALSE;

    // BUGBUG: Don't really have to do this for all buttons, but since so many of them trigger it
    // BUGBUG: We'll put this here
    UpdateTexture();

    // Handle Buttons being pressed
    switch( buttonPressed )
    {
    case BUTTON_A:
        {
            // Play the Select Sound
            g_Sounds.PlaySelectSound();

            // Check to see if the user has entered in enough passcode digits
            if( ( m_usPassCodeIndex != XONLINE_PIN_LENGTH ) && ( 0 != m_usPassCodeIndex ) )
            {
                m_bDisplayInvalidMsg = TRUE;
                break;
            }

            if( GetParent() )   // Go Back one menu
                Leave( GetParent() );

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
    case BUTTON_X:
        {
            if( m_usPassCodeIndex < XONLINE_PIN_LENGTH )
            {
                // Play the keyboard stroke sound
                g_Sounds.PlayKeyboardStrokeSound();

                m_pbyPassCode[m_usPassCodeIndex++] = 5;
            }
            break;
        }
    case BUTTON_Y:
        {
            if( m_usPassCodeIndex < XONLINE_PIN_LENGTH )
            {
                // Play the keyboard stroke sound
                g_Sounds.PlayKeyboardStrokeSound();

                m_pbyPassCode[m_usPassCodeIndex++] = 6;
            }
            break;
        }
    case BUTTON_BLACK:
        {
            if( m_usPassCodeIndex < XONLINE_PIN_LENGTH )
            {
                // Play the keyboard stroke sound
                g_Sounds.PlayKeyboardStrokeSound();

                m_pbyPassCode[m_usPassCodeIndex++] = 7;
            }
            break;
        }
    case BUTTON_WHITE:
        {
            if( m_usPassCodeIndex < XONLINE_PIN_LENGTH )
            {
                // Play the keyboard stroke sound
                g_Sounds.PlayKeyboardStrokeSound();

                m_pbyPassCode[m_usPassCodeIndex++] = 8;
            }
            break;
        }
    case BUTTON_LEFT_TRIGGER:
        {
            if( m_usPassCodeIndex < XONLINE_PIN_LENGTH )
            {
                // Play the keyboard stroke sound
                g_Sounds.PlayKeyboardStrokeSound();

                m_pbyPassCode[m_usPassCodeIndex++] = 9;
            }
            break;
        }
    case BUTTON_RIGHT_TRIGGER:
        {
            if( m_usPassCodeIndex < XONLINE_PIN_LENGTH )
            {
                // Play the keyboard stroke sound
                g_Sounds.PlayKeyboardStrokeSound();

                m_pbyPassCode[m_usPassCodeIndex++] = 10;
            }
            break;
        }
    }
}


// This will be called when the user leaves this menu
void CPassCodeMenu::Leave( CXItem* pItem )
{
    CMenuScreen::Leave( pItem );

    // Turn on updating of the address texture
    g_bToggleIPInfo = TRUE;
}


// This will be called whenever this menu is entered / activated
void CPassCodeMenu::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();

    // Make sure our cancelled option is set to FALSE
    m_bCancelled = FALSE;

    // Put the keyboard in it's original state
    m_bDisplayInvalidMsg = FALSE;
    m_usPassCodeIndex = 0;
    ZeroMemory( &m_pbyPassCode, XONLINE_PIN_LENGTH );

    // Turn off updating of the address texture
    g_bToggleIPInfo = FALSE;
}
