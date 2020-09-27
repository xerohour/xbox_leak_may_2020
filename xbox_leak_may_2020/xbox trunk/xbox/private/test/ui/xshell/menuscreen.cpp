/*****************************************************
*** menuscreen.cpp
***
*** CPP file for our XShell Generic menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
*** 
*** by James N. Helm
*** December 1st, 2000
*** 
*****************************************************/

#include "stdafx.h"
#include "menuscreen.h"

extern CXItem*          g_pCurrentScreen;       // Pointer to the current menu object
extern BOOL             g_bUpdateTitleTexture;  // Update the Title Texture
extern CSoundCollection g_Sounds;               // Used to generate all sounds for the XShell

// Constructor
CMenuScreen::CMenuScreen( CXItem* pParent )
: CXItem( pParent ),
m_nSelectedItem( 0 ),
m_uiFontHeightOfItem( 0 ),
m_uiNumItems( 0 ),
m_nJoystickDeadZone( JOYSTICK_DEAD_ZONE )
{
    m_ScreenItems.SetDeleteDataItem( TRUE );

    m_keyPressDelayTimer = GetTickCount();
}


// Destructor
CMenuScreen::~CMenuScreen()
{
    if( NULL != m_pwszMenuTitle )
    {
        delete[] m_pwszMenuTitle;
        m_pwszMenuTitle = NULL;
    }
}


// Draws a menu on to the screen
void CMenuScreen::Action( CUDTexture* pTexture )
{
    pTexture->Lock();

    // Draw the screen items
    m_ScreenItems.MoveTo( 0 ); // Start at the 1st Item
    for( unsigned int x = 0; x < m_ScreenItems.GetNumItems(); ++x )
    {
        m_ScreenItems.GetCurrentNode()->Render( pTexture );
        m_ScreenItems.MoveNext();
    }

    pTexture->Unlock();
} 


// Add the screen and text of the item that will be drawn on to the screen
// to our object
HRESULT CMenuScreen::ImportScreenItems( char* menuFileName )
{
    XDBGTRC( APP_TITLE_NAME_A, "CMenuScreen::ImportScreenItems()" );

    if( !menuFileName )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CMenuScreen::ImportScreenItems():menuFileName not set!!" );

        return E_INVALIDARG;
    }

    FILE* menuFile = fopen( menuFileName, "r" );

    if( !menuFile )
    {
        XDBGERR( APP_TITLE_NAME_A, "CMenuScreen::ImportScreenItems():fopen failed!! - '%s'", menuFileName );

        return E_FAIL;
    }

    HRESULT hr = S_OK;
    WCHAR itemType;
    BOOL bEOF = FALSE;

    while( !bEOF )
    {
        itemType = fgetwc( menuFile );
        if( feof( menuFile ) )
        {
            bEOF = TRUE;
            continue;
        }

        if( L't' == itemType )
        {
            hr = AddTextItemFromFile( menuFile );
        }
        else if( L'p' == itemType )
        {
            hr = AddPanelItemFromFile( menuFile );
        }
        else if( L'l' == itemType )
        {
            hr = AddLineItemFromFile( menuFile );
        }
        else if( L'h' == itemType )
        {
            hr = AddHeaderItemFromFile( menuFile );
        }
        else if( L'o' == itemType )
        {
            hr = AddOutlineItemFromFile( menuFile );
        }
        else if( L'i' == itemType )
        {
            hr = AddBitmapItemFromFile( menuFile );
        }

        // Move to the next line
        while( itemType = fgetwc( menuFile ) )
        {
            if( feof( menuFile ) )
            {
                bEOF = TRUE;
                break;
            }

            if( 10 == itemType )
            {
                break;
            }
        }

        // Check to see if there was a problem, and return if there was
        if( FAILED( hr ) )
        {
            fclose( menuFile );
            return hr;
        }
    }

    fclose( menuFile );

    return S_OK;
}


// Initialize the Menu
HRESULT CMenuScreen::Init( char* menuFileName )
{
    XDBGTRC( APP_TITLE_NAME_A, "CMenuScreen::Init()" );

    // Get our screen information
    ImportScreenItems( menuFileName );

    // Get the height of the font
    unsigned int decent;
    GetFontSize( &m_uiFontHeightOfItem, &decent );

    return S_OK;
}


// Add a text item to our current screen from a file
HRESULT CMenuScreen::AddTextItemFromFile( FILE* menuFile )
{
    CTextItem* pTextItem = new CTextItem;
    if( !pTextItem )
    {
        XDBGERR( APP_TITLE_NAME_A, "CMenuScreen::AddTextItemFromFile():Out of memory!!" );

        return E_OUTOFMEMORY;
    }

    float fXPos, fYPos;
    DWORD dwFGColor, dwBGColor;
    WCHAR pwszText[SCREEN_ITEM_TEXT_LENGTH];
    
    ZeroMemory( pwszText, sizeof( WCHAR ) * SCREEN_ITEM_TEXT_LENGTH );
    
    fscanf( menuFile,   "%f", &fXPos );
    fscanf( menuFile,   "%f", &fYPos );
    fscanf( menuFile,   "%X", &dwFGColor );
    fscanf( menuFile,   "%X", &dwBGColor );
    fwscanf( menuFile, L"%s", pwszText );

    XDBGTRC( APP_TITLE_NAME_A, "CMenuScreen::AddTextItemFromFile():\n  fXPos: %f\n  fYPos: %f\n  dwFGColor: %X\n  dwBGColor: %X\n  pwszText: %S", fXPos, fYPos, dwFGColor, dwBGColor, pwszText );
    
    // Convert any "_" characters in to spaces
    WCHAR* pChar;
    while( pChar = wcsstr( pwszText, L"_" ) )
    {
        *pChar = L' ';
    }

    // Update the item with the information that we pulled from the file
    pTextItem->UpdateItem( pwszText, fXPos, fYPos, dwFGColor, dwBGColor );

    // Add the item to our list of Screen Items
    m_ScreenItems.AddNode( pTextItem );
    
    return S_OK;
}


// Add a panel item to our current screen
HRESULT CMenuScreen::AddPanelItemFromFile( FILE* menuFile )
{
    CPanelItem* pPanelItem = new CPanelItem;
    if( !pPanelItem )
    {
        XDBGERR( APP_TITLE_NAME_A, "CMenuScreen::AddPanelItemFromFile():Out of memory!!" );

        return E_OUTOFMEMORY;
    }

    float fX1Pos, fY1Pos, fX2Pos, fY2Pos;
    DWORD dwColor;

    fscanf( menuFile,   "%f", &fX1Pos );
    fscanf( menuFile,   "%f", &fY1Pos );
    fscanf( menuFile,   "%f", &fX2Pos );
    fscanf( menuFile,   "%f", &fY2Pos );
    fscanf( menuFile,   "%x", &dwColor );

    XDBGTRC( APP_TITLE_NAME_A, "CMenuScreen::AddPanelItemFromFile():\n  fX1Pos: %f\n  fY1Pos: %f\n  fX2Pos: %f\n  Y2Pos: %f\n  dwColor: %X", fX1Pos, fY1Pos, fX2Pos, fY2Pos, dwColor );

    // Update the item with the information that we pulled from the file
    pPanelItem->UpdateItem( fX1Pos, fY1Pos, fX2Pos, fY2Pos, dwColor );

    // Add the item to our list of Screen Items
    m_ScreenItems.AddNode( pPanelItem );

    return S_OK;
}


// Add a line to our current screen
HRESULT CMenuScreen::AddLineItemFromFile( FILE* menuFile )
{
    CLineItem* pLineItem = new CLineItem;
    if( !pLineItem )
    {
        XDBGERR( APP_TITLE_NAME_A, "CMenuScreen::AddLineItemFromFile():Out of memory!!" );

        return E_OUTOFMEMORY;
    }

    float fX1Pos, fY1Pos, fX2Pos, fY2Pos, fWidth;
    DWORD dwColor;

    fscanf( menuFile,   "%f", &fX1Pos );
    fscanf( menuFile,   "%f", &fY1Pos );
    fscanf( menuFile,   "%f", &fX2Pos );
    fscanf( menuFile,   "%f", &fY2Pos );
    fscanf( menuFile,   "%f", &fWidth );
    fscanf( menuFile,   "%x", &dwColor );

    XDBGTRC( APP_TITLE_NAME_A, "CMenuScreen::AddLineItemFromFile():\n  fX1Pos: %f\n  fY1Pos: %f\n  fX2Pos: %f\n  fY2Pos: %f\n  fWidth: %f\n  dwColor: %X", fX1Pos, fY1Pos, fX2Pos, fY2Pos, fWidth, dwColor );

    // Update the item with the information that we pulled from the file
    pLineItem->UpdateItem( fX1Pos, fY1Pos, fX2Pos, fY2Pos, fWidth, dwColor );

    // Add the item to our list of Screen Items
    m_ScreenItems.AddNode( pLineItem );

    return S_OK;
}


// Add a line to our current screen
HRESULT CMenuScreen::AddOutlineItemFromFile( FILE* menuFile )
{
    COutlineItem* pOutlineItem = new COutlineItem;
    if( !pOutlineItem )
    {
        XDBGERR( APP_TITLE_NAME_A, "CMenuScreen::AddOutlineItemFromFile():Out of memory!!" );

        return E_OUTOFMEMORY;
    }

    float fX1Pos, fY1Pos, fX2Pos, fY2Pos, fWidth;
    DWORD dwColor;

    fscanf( menuFile,   "%f", &fX1Pos );
    fscanf( menuFile,   "%f", &fY1Pos );
    fscanf( menuFile,   "%f", &fX2Pos );
    fscanf( menuFile,   "%f", &fY2Pos );
    fscanf( menuFile,   "%f", &fWidth );
    fscanf( menuFile,   "%x", &dwColor );

    XDBGTRC( APP_TITLE_NAME_A, "CMenuScreen::AddOutlineItemFromFile():\n  fX1Pos: %f\n  fY1Pos: %f\n  fX2Pos: %f\n  fY2Pos: %f\n  fWidth: %f\n  dwColor: %X", fX1Pos, fY1Pos, fX2Pos, fY2Pos, fWidth, dwColor );

    // Update the item with the information that we pulled from the file
    pOutlineItem->UpdateItem( fX1Pos, fY1Pos, fX2Pos, fY2Pos, fWidth, dwColor );

    // Add the item to our list of Screen Items
    m_ScreenItems.AddNode( pOutlineItem );

    return S_OK;
}


// Add a Bitmap Item to our Screen from a file
HRESULT CMenuScreen::AddBitmapItemFromFile( FILE* menuFile )
{
    CBitmapItem* pBitmapItem = new CBitmapItem;
    if( !pBitmapItem )
    {
        XDBGERR( APP_TITLE_NAME_A, "CMenuScreen::AddBitmapItemFromFile():Out of memory!!" );

        return E_OUTOFMEMORY;
    }

    int x, y;
    char filename[MAX_PATH];
    char imagename[MAX_PATH];
    ZeroMemory( filename, MAX_PATH );
    ZeroMemory( imagename, MAX_PATH );

    fscanf( menuFile,   "%d", &x );
    fscanf( menuFile,   "%d", &y );
    fscanf( menuFile,   "%s", imagename );

    _snprintf( filename, MAX_PATH, "%s\\%s", FILE_DATA_IMAGE_DIRECTORY_A, imagename );

    XDBGTRC( APP_TITLE_NAME_A, "CMenuScreen::AddBitmapItemFromFile():\n  XPos: %d\n  YPos: %d\n  Bitmap: %s", x, y, filename );

    // Update the item with the information that we pulled from the file
    pBitmapItem->UpdateItem( filename, x, y );

    // Add the item to our list of Screen Items
    m_ScreenItems.AddNode( pBitmapItem );

    return S_OK;

}

// Add a panel item to our current screen
HRESULT CMenuScreen::AddHeaderItemFromFile( FILE* menuFile )
{
    CTextItem* pHeaderItem = new CTextItem;
    if( !pHeaderItem )
    {
        XDBGERR( APP_TITLE_NAME_A, "CMenuScreen::AddHeaderItemFromFile():Out of memory!!" );

        return E_OUTOFMEMORY;
    }

    float fXPos, fYPos;
    DWORD dwFGColor, dwBGColor;
    WCHAR pwszText[SCREEN_ITEM_TEXT_LENGTH];
    
    ZeroMemory( pwszText, sizeof( WCHAR ) * SCREEN_ITEM_TEXT_LENGTH );
    
    fscanf( menuFile,   "%f", &fXPos );
    fscanf( menuFile,   "%f", &fYPos );
    fscanf( menuFile,   "%X", &dwFGColor );
    fscanf( menuFile,   "%X", &dwBGColor );
    fwscanf( menuFile, L"%s", pwszText );

    XDBGTRC( APP_TITLE_NAME_A, "CMenuScreen::AddHeaderItemFromFile():\n  fXPos: %f\n  fYPos: %f\n  dwFGColor: %X\n  dwBGColor: %X\n  pwszText: %S", fXPos, fYPos, dwFGColor, dwBGColor, pwszText );
    
    // Convert any "_" characters in to spaces
    WCHAR* pChar;
    while( pChar = wcsstr( pwszText, L"_" ) )
    {
        *pChar = L' ';
    }

    // Update the item with the information that we pulled from the file
    pHeaderItem->UpdateItem( pwszText, fXPos, fYPos, dwFGColor, dwBGColor );

    // Add the item to our list of Screen Items
    m_ScreenItems.AddNode( pHeaderItem );
    
    return S_OK;
}


// Handles input (of the CONTROLS) for the current menu
void CMenuScreen::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
    // Process the default button delay
    if( !HandleInputDelay( bFirstPress ) )
        return;

    // Handle Controls being pressed
    switch( controlPressed )
    {
    case CONTROL_DPAD_UP:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
                if( GetSelectedItem() > 0 )
                {
                    m_bUpdateTexture = TRUE;

                    // Play the Menu Item Select Sound
                    g_Sounds.PlayMenuItemSelectSound();

                    SetSelectedItem( GetSelectedItem() - 1 );
                }
            }
            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
                if( (unsigned int)GetSelectedItem() < ( GetNumItems() - 1 ) )
                {
                    m_bUpdateTexture = TRUE;

                    // Play the Menu Item Select Sound
                    g_Sounds.PlayMenuItemSelectSound();

                    SetSelectedItem( GetSelectedItem() + 1 );
                }
            }
            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CMenuScreen::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // Process the default button delay
    if( !bFirstPress )
        return;

    // Handle Buttons being pressed
    switch( buttonPressed )
    {
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


// Handle any requests for the joystick (thumb-pad)
void CMenuScreen::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
{
    switch( joystick )
    {
    // This is the LEFT Thumb Joystick on the controller
    case JOYSTICK_LEFT:
        {
            // Yxc
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
                // Hand off this to the "DPad" handler
                HandleInput( CONTROL_DPAD_RIGHT, bFirstXPress );
            }
            break;
        }
    }
}


// Sets the currently selected item on our screen
HRESULT CMenuScreen::SetSelectedItem( int itemNumber )
{
    if( ( itemNumber < 0 ) || ( (unsigned int)itemNumber > GetNumItems() ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CMenuScreen::SetSelectedItem():Tried to select item out of range! - '%d', MAX - '%d'", itemNumber, GetNumItems() - 1 );

        return E_INVALIDARG;
    }

    m_nSelectedItem = itemNumber;

    return S_OK;
}


// Should be called whenever a user enters or activates this menu
void CMenuScreen::Enter()
{
    // Also signal the texture to be updated when a user enters a menu
    m_bUpdateTexture = TRUE;
    g_bUpdateTitleTexture = TRUE;   // Update the Title Texture
}


// Should be called whenever a user leaves the menu
void CMenuScreen::Leave( CXItem* pItem )
{
    g_pCurrentScreen = pItem;
}

// Handle the default delay before for Input
BOOL CMenuScreen::HandleInputDelay( BOOL bFirstPress )
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
            return FALSE;
        }
        m_keyPressDelayTimer = GetTickCount();
        m_keyPressDelayInterval = KEY_PRESS_REPEAT_DELAY;
    }

    return TRUE;
}

// Key Class by Victor Blanco
Key::Key()
{
	xorigin = 0;
	yorigin = 0;
	width = 0;
	height = 0;		
	selectColor = 0x00000000;
	render = TRUE;
    resultChar = NULL;
}

Key::~Key()
{
    if( resultChar )
    {
        delete[] resultChar;
        resultChar = NULL;
    }
}

void Key::define(int x, int y, int w, int h, DWORD color)
{
	xorigin = x;
	yorigin = y;
	width = w;
	height = h;		
	selectColor = color;	
}

void Key::defineText(WCHAR* result)
{
	int length = wcslen(result);
	resultChar = new WCHAR[length + 1];
	wcscpy(resultChar, result);
}

void Key::setRender(BOOL value) {
	render = value;
}

BOOL Key::getRender(void) {
	return render;
}