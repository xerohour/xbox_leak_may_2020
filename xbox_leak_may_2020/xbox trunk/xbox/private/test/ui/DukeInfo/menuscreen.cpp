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

extern CXItem* g_pCurrentScreen;  // Pointer to the current menu object
extern USBManager Controllers;    // Used to determine what buttons / direction the user is pressing

// Constructor
CMenuScreen::CMenuScreen()
: CXItem(),
m_nJoystickDeadZone( 0 ),
m_port( 0 ),
m_keyPressDelayTimer( 0 )
{
    XDBGTRC( APP_TITLE_NAME_A, "CMenuScreen::CMenuScreen()" );

    m_textItems.SetDeleteDataItem( true );
    m_panelItems.SetDeleteDataItem( true );
    m_lineItems.SetDeleteDataItem( true );
    m_outlineItems.SetDeleteDataItem( true );
    m_bitmapItems.SetDeleteDataItem( true );
    m_headerItems.SetDeleteDataItem( true );
}


// Constructor
CMenuScreen::CMenuScreen( CXItem* pParent )
: CXItem( pParent ),
m_nJoystickDeadZone( 0 ),
m_port( 0 ),
m_keyPressDelayTimer( 0 )
{
    XDBGTRC( APP_TITLE_NAME_A, "CMenuScreen::CMenuScreen(ex)" );

    m_textItems.SetDeleteDataItem( true );
    m_panelItems.SetDeleteDataItem( true );
    m_lineItems.SetDeleteDataItem( true );
    m_outlineItems.SetDeleteDataItem( true );
    m_bitmapItems.SetDeleteDataItem( true );
    m_headerItems.SetDeleteDataItem( true );
}


// Destructor
CMenuScreen::~CMenuScreen()
{
}


// Draws a menu on to the screen
void CMenuScreen::Action( CXBoxVideo* pScreen )
{
    unsigned int nStringWidth = 0;

    // Draw the bitmap items
    m_bitmapItems.MoveTo( 0 );  // Start at the 1st Item
    for( unsigned int x = 0; x < m_bitmapItems.GetNumItems(); ++x )
    {
        m_bitmapItems.GetCurrentNode()->render( pScreen->GetD3DDevicePtr() );
        m_bitmapItems.MoveNext();
    }
    
    // Draw the panel items
    m_panelItems.MoveTo( 0 );  // Start at the 1st Item
    for( x = 0; x < m_panelItems.GetNumItems(); ++x )
    {
        pScreen->DrawBox( m_panelItems.GetCurrentNode()->X1Pos, m_panelItems.GetCurrentNode()->Y1Pos, m_panelItems.GetCurrentNode()->X2Pos, m_panelItems.GetCurrentNode()->Y2Pos, m_panelItems.GetCurrentNode()->color );
        m_panelItems.MoveNext();
    }

    // Draw the line items
    m_lineItems.MoveTo( 0 );  // Start at the 1st Item
    for( x = 0; x < m_lineItems.GetNumItems(); ++x )
    {
        pScreen->DrawLine( m_lineItems.GetCurrentNode()->X1Pos, m_lineItems.GetCurrentNode()->Y1Pos, m_lineItems.GetCurrentNode()->X2Pos, m_lineItems.GetCurrentNode()->Y2Pos, (float)m_lineItems.GetCurrentNode()->width, m_lineItems.GetCurrentNode()->color );
        m_lineItems.MoveNext();
    }

    // Draw the outline items
    m_outlineItems.MoveTo( 0 );  // Start at the 1st Item
    for( x = 0; x < m_outlineItems.GetNumItems(); ++x )
    {
        pScreen->DrawOutline( m_outlineItems.GetCurrentNode()->X1Pos, m_outlineItems.GetCurrentNode()->Y1Pos, m_outlineItems.GetCurrentNode()->X2Pos, m_outlineItems.GetCurrentNode()->Y2Pos, (float)m_outlineItems.GetCurrentNode()->width, m_outlineItems.GetCurrentNode()->color );
        m_outlineItems.MoveNext();
    }

    // Draw the header items
    m_headerItems.MoveTo( 0 );  // Start at the 1st Item
    for( x = 0; x < m_headerItems.GetNumItems(); ++x )
    {
        pScreen->DrawText( m_headerItems.GetCurrentNode()->XPos, m_headerItems.GetCurrentNode()->YPos, m_headerItems.GetCurrentNode()->fgcolor, m_headerItems.GetCurrentNode()->bgcolor, L"%s", m_headerItems.GetCurrentNode()->text );
        m_headerItems.MoveNext();
    } 
    
    // Draw the text items
    m_textItems.MoveTo( 0 );  // Start at the 1st Item
    for( x = 0; x < m_textItems.GetNumItems(); ++x )
    {
        pScreen->DrawText( m_textItems.GetCurrentNode()->XPos, m_textItems.GetCurrentNode()->YPos, m_textItems.GetCurrentNode()->fgcolor, m_textItems.GetCurrentNode()->bgcolor, L"%s", m_textItems.GetCurrentNode()->text );
        m_textItems.MoveNext();
    }

    // Info on current port
    pScreen->DrawText( 450.0f, 106.0f, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"Port - '%d'", m_port+1 );

    // Get the state of the Input Device
    XINPUT_STATE* pState;
    Controllers.ProcessInput();
    pState = Controllers.GetControllerState( m_port );

    pScreen->DrawText( MENUBOX_TEXTAREA_X1, MENUBOX_TEXTAREA_Y1, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"A Button" );
    pScreen->DrawText( MENUBOX_VALUE_X1,    MENUBOX_TEXTAREA_Y1, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", pState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] );

    pScreen->DrawText( MENUBOX_TEXTAREA_X1, MENUBOX_TEXTAREA_Y1 + ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"B Button" );
    pScreen->DrawText( MENUBOX_VALUE_X1,    MENUBOX_TEXTAREA_Y1 + ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", pState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] );

    pScreen->DrawText( MENUBOX_TEXTAREA_X1, MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 2), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"X Button" );
    pScreen->DrawText( MENUBOX_VALUE_X1,    MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 2), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", pState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] );

    pScreen->DrawText( MENUBOX_TEXTAREA_X1, MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 3), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"Y Button" );
    pScreen->DrawText( MENUBOX_VALUE_X1,    MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 3), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", pState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] );

    pScreen->DrawText( MENUBOX_TEXTAREA_X1, MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 4), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"Black Button" );
    pScreen->DrawText( MENUBOX_VALUE_X1,    MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 4), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", pState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] );

    pScreen->DrawText( MENUBOX_TEXTAREA_X1, MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 5), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"White Button" );
    pScreen->DrawText( MENUBOX_VALUE_X1,    MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 5), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", pState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] );

    pScreen->DrawText( MENUBOX_TEXTAREA_X1, MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 6), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"Left Trigger" );
    pScreen->DrawText( MENUBOX_VALUE_X1,    MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 6), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", pState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] );

    pScreen->DrawText( MENUBOX_TEXTAREA_X1, MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 7), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"Right Trigger" );
    pScreen->DrawText( MENUBOX_VALUE_X1,    MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 7), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", pState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] );

    pScreen->DrawText( MENUBOX_RIGHT_COL_X1,   MENUBOX_TEXTAREA_Y1, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"Left Thumbstick X" );
    pScreen->DrawText( MENUBOX_RIGHT_VALUE_X1, MENUBOX_TEXTAREA_Y1, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", pState->Gamepad.sThumbLX );

    pScreen->DrawText( MENUBOX_RIGHT_COL_X1,   MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 1), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"Left Thumbstick Y" );
    pScreen->DrawText( MENUBOX_RIGHT_VALUE_X1, MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 1), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", pState->Gamepad.sThumbLY );

    pScreen->DrawText( MENUBOX_RIGHT_COL_X1,   MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 2), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"Right Thumbstick X" );
    pScreen->DrawText( MENUBOX_RIGHT_VALUE_X1, MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 2), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", pState->Gamepad.sThumbRX );

    pScreen->DrawText( MENUBOX_RIGHT_COL_X1,   MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 3), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"Right Thumbstick Y" );
    pScreen->DrawText( MENUBOX_RIGHT_VALUE_X1, MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 3), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", pState->Gamepad.sThumbRY );

    pScreen->DrawText( MENUBOX_RIGHT_COL_X1,   MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 4), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"DPad Up" );
    pScreen->DrawText( MENUBOX_RIGHT_VALUE_X1, MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 4), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", !!( pState->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ) );

    pScreen->DrawText( MENUBOX_RIGHT_COL_X1,   MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 5), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"DPad Down" );
    pScreen->DrawText( MENUBOX_RIGHT_VALUE_X1, MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 5), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", !!( pState->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) );

    pScreen->DrawText( MENUBOX_RIGHT_COL_X1,   MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 6), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"DPad Left" );
    pScreen->DrawText( MENUBOX_RIGHT_VALUE_X1, MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 6), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", !!( pState->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ) );

    pScreen->DrawText( MENUBOX_RIGHT_COL_X1,   MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 7), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"DPad Right" );
    pScreen->DrawText( MENUBOX_RIGHT_VALUE_X1, MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 7), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", !!( pState->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) );

    pScreen->DrawText( MENUBOX_RIGHT_COL_X1,   MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 8), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"Start" );
    pScreen->DrawText( MENUBOX_RIGHT_VALUE_X1, MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 8), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", !!( pState->Gamepad.wButtons & XINPUT_GAMEPAD_START ) );

    pScreen->DrawText( MENUBOX_RIGHT_COL_X1,   MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 9), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"Back" );
    pScreen->DrawText( MENUBOX_RIGHT_VALUE_X1, MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 9), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", !!( pState->Gamepad.wButtons & XINPUT_GAMEPAD_BACK ) );

    pScreen->DrawText( MENUBOX_RIGHT_COL_X1,   MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 10), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"Left Thumb" );
    pScreen->DrawText( MENUBOX_RIGHT_VALUE_X1, MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 10), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", !!( pState->Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ) );

    pScreen->DrawText( MENUBOX_RIGHT_COL_X1,   MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 11), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"Right Thumb" );
    pScreen->DrawText( MENUBOX_RIGHT_VALUE_X1, MENUBOX_TEXTAREA_Y1 + ( ( ITEM_SELECTOR_BORDER_SPACE + FONT_DEFAULT_HEIGHT ) * 11), SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"- '%d'", !!( pState->Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ) );

    // Check to see if the user wishes to change ports
    if( Controllers.IsControlPressed( PORT_ANY, CONTROL_START ) &&
        Controllers.IsControlPressed( PORT_ANY, CONTROL_BACK ) &&
        Controllers.IsButtonPressed( PORT_ANY, BUTTON_LEFT_TRIGGER ) &&
        Controllers.IsButtonPressed( PORT_ANY, BUTTON_RIGHT_TRIGGER ) )
    {
        if( ( GetTickCount() - m_keyPressDelayTimer ) > KEY_PRESS_DELAY )
        {
          ++m_port;

          if( m_port >= NUM_XBOX_PORTS )
          {
              m_port = 0;
          }

          m_keyPressDelayTimer = GetTickCount();
        }
    }

    // Check to see if the user wishes to reboot back to the shell
    if( Controllers.IsControlPressed( PORT_ANY, CONTROL_LEFT_THUMB ) &&
        Controllers.IsControlPressed( PORT_ANY, CONTROL_RIGHT_THUMB ) &&
        Controllers.IsButtonPressed( PORT_ANY, BUTTON_LEFT_TRIGGER ) &&
        Controllers.IsButtonPressed( PORT_ANY, BUTTON_RIGHT_TRIGGER ) )
    {
        // Boot back to the shell
        XLaunchNewImage( NULL, NULL );
    }
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
    bool bEOF = false;

    while( !bEOF )
    {
        itemType = fgetwc( menuFile );
        if( feof( menuFile ) )
        {
            bEOF = true;
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
                bEOF = true;
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
HRESULT CMenuScreen::Init( CXBoxVideo* pScreen, char* menuFileName )
{
    XDBGTRC( APP_TITLE_NAME_A, "CMenuScreen::Init()" );

    // Get our screen information
    ImportScreenItems( menuFileName );
    
    return S_OK;
}


// Add a text item to our current screen from a file
HRESULT CMenuScreen::AddTextItemFromFile( FILE* menuFile )
{
    struct _TEXTITEM* pTextItem = new struct _TEXTITEM;
    if( !pTextItem )
    {
        XDBGERR( APP_TITLE_NAME_A, "CMenuScreen::AddTextItemFromFile():Out of memory!!" );
        return E_OUTOFMEMORY;
    }

    ZeroMemory( pTextItem->text, sizeof( WCHAR ) * SCREEN_ITEM_TEXT_LENGTH );
    
    fscanf( menuFile,   "%f", &(pTextItem->XPos) );
    fscanf( menuFile,   "%f", &(pTextItem->YPos) );
    fscanf( menuFile,   "%X", &(pTextItem->fgcolor) );
    fscanf( menuFile,   "%X", &(pTextItem->bgcolor) );
    fwscanf( menuFile, L"%s",   pTextItem->text );

    XDBGTRC( APP_TITLE_NAME_A, "CMenuScreen::AddTextItemFromFile():\n  XPos: %f\n  YPos: %f\n  fgcol: %X\n  bgcol: %X\n  text: %S", pTextItem->XPos, pTextItem->YPos, pTextItem->fgcolor, pTextItem->bgcolor, pTextItem->text );
    
    WCHAR* pChar;
    while( pChar = wcsstr( pTextItem->text, L"_" ) )
    {
        *pChar = L' ';
    }

    m_textItems.AddNode( pTextItem );

    return S_OK;
}


// Add a panel item to our current screen
HRESULT CMenuScreen::AddPanelItemFromFile( FILE* menuFile )
{
    struct _PANELITEM* pPanelItem = new struct _PANELITEM;
    if( !pPanelItem )
    {
        XDBGERR( APP_TITLE_NAME_A, "CMenuScreen::AddPanelItemFromFile():Out of memory!!" );
        return E_OUTOFMEMORY;
    }

    fscanf( menuFile,   "%f", &(pPanelItem->X1Pos) );
    fscanf( menuFile,   "%f", &(pPanelItem->Y1Pos) );
    fscanf( menuFile,   "%f", &(pPanelItem->X2Pos) );
    fscanf( menuFile,   "%f", &(pPanelItem->Y2Pos) );
    fscanf( menuFile,   "%x", &(pPanelItem->color) );

    XDBGTRC( APP_TITLE_NAME_A, "CMenuScreen::AddPanelItemFromFile():\n  X1Pos: %f\n  Y1Pos: %f\n  X2Pos: %f\n  Y2Pos: %f\n  color: %X", pPanelItem->X1Pos, pPanelItem->Y1Pos, pPanelItem->X2Pos, pPanelItem->Y2Pos, pPanelItem->color );

    m_panelItems.AddNode( pPanelItem );

    return S_OK;
}


// Add a line to our current screen
HRESULT CMenuScreen::AddLineItemFromFile( FILE* menuFile )
{
    struct _PANELITEM* pLineItem = new struct _PANELITEM;
    if( !pLineItem )
    {
        XDBGERR( APP_TITLE_NAME_A, "CMenuScreen::AddLineItemFromFile():Out of memory!!" );
        return E_OUTOFMEMORY;
    }

    fscanf( menuFile,   "%f", &(pLineItem->X1Pos) );
    fscanf( menuFile,   "%f", &(pLineItem->Y1Pos) );
    fscanf( menuFile,   "%f", &(pLineItem->X2Pos) );
    fscanf( menuFile,   "%f", &(pLineItem->Y2Pos) );
    fscanf( menuFile,   "%d", &(pLineItem->width) );
    fscanf( menuFile,   "%x", &(pLineItem->color) );

    XDBGTRC( APP_TITLE_NAME_A, "CMenuScreen::AddLineItemFromFile():\n  X1Pos: %f\n  Y1Pos: %f\n  X2Pos: %f\n  Y2Pos: %f\n  width: %d\n  color: %X", pLineItem->X1Pos, pLineItem->Y1Pos, pLineItem->X2Pos, pLineItem->Y2Pos, pLineItem->width, pLineItem->color );

    m_lineItems.AddNode( pLineItem );

    return S_OK;
}


// Add a line to our current screen
HRESULT CMenuScreen::AddOutlineItemFromFile( FILE* menuFile )
{
    struct _PANELITEM* pLineItem = new struct _PANELITEM;
    if( !pLineItem )
    {
        XDBGERR( APP_TITLE_NAME_A, "CMenuScreen::AddOutlineItemFromFile():Out of memory!!" );
        return E_OUTOFMEMORY;
    }

    fscanf( menuFile,   "%f", &(pLineItem->X1Pos) );
    fscanf( menuFile,   "%f", &(pLineItem->Y1Pos) );
    fscanf( menuFile,   "%f", &(pLineItem->X2Pos) );
    fscanf( menuFile,   "%f", &(pLineItem->Y2Pos) );
    fscanf( menuFile,   "%d", &(pLineItem->width) );
    fscanf( menuFile,   "%x", &(pLineItem->color) );

    XDBGTRC( APP_TITLE_NAME_A, "CMenuScreen::AddOutlineItemFromFile():\n  X1Pos: %f\n  Y1Pos: %f\n  X2Pos: %f\n  Y2Pos: %f\n  width: %d\n  color: %X", pLineItem->X1Pos, pLineItem->Y1Pos, pLineItem->X2Pos, pLineItem->Y2Pos, pLineItem->width, pLineItem->color );

    m_outlineItems.AddNode( pLineItem );

    return S_OK;
}


// Add a Bitmap Item to our Screen from a file
HRESULT CMenuScreen::AddBitmapItemFromFile( FILE* menuFile )
{
    BitmapFile* pBitmapItem = new BitmapFile;
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

    pBitmapItem->read( x, y, filename );

    XDBGTRC( APP_TITLE_NAME_A, "CMenuScreen::AddBitmapItemFromFile():\n  XPos: %d\n  YPos: %d\n  Bitmap: %s", x, y, filename );

    m_bitmapItems.AddNode( pBitmapItem );

    return S_OK;

}

// Add a panel item to our current screen
HRESULT CMenuScreen::AddHeaderItemFromFile( FILE* menuFile )
{
    struct _TEXTITEM* pHeaderItem = new struct _TEXTITEM;
    if( !pHeaderItem )
    {
        XDBGERR( APP_TITLE_NAME_A, "CMenuScreen::AddHeaderItemFromFile():Out of memory!!" );
        return E_OUTOFMEMORY;
    }

    ZeroMemory( pHeaderItem->text, sizeof( WCHAR ) * SCREEN_ITEM_TEXT_LENGTH );
    
    fscanf( menuFile,   "%f", &(pHeaderItem->XPos) );
    fscanf( menuFile,   "%f", &(pHeaderItem->YPos) );
    fscanf( menuFile,   "%X", &(pHeaderItem->fgcolor) );
    fscanf( menuFile,   "%X", &(pHeaderItem->bgcolor) );
    fwscanf( menuFile, L"%s",   pHeaderItem->text );

    XDBGTRC( APP_TITLE_NAME_A, "CMenuScreen::AddHeaderItemFromFile():\n  XPos: %f\n  YPos: %f\n  fgcol: %X\n  bgcol: %X\n  text: %S", pHeaderItem->XPos, pHeaderItem->YPos, pHeaderItem->fgcolor, pHeaderItem->bgcolor, pHeaderItem->text );
    
    WCHAR* pChar;
    while( pChar = wcsstr( pHeaderItem->text, L"_" ) )
    {
        *pChar = L' ';
    }

    m_headerItems.AddNode( pHeaderItem );

    return S_OK;
}
