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

// Constructor
CMenuScreen::CMenuScreen( void )
: m_pwszMenuTitle( NULL ),
m_uiSelectedItem( 0 ),
m_uiTopItemIndex( 0 ),
m_bUpdateTexture( TRUE ),
m_bSingleList( TRUE ),
m_BackPtr( NULL ),
m_dwKeyPressDelayTimer( GetTickCount() ),
m_dwKeyPressDelayInterval( 0l ),
m_dwKeyPressInitialDelay( 0l ),
m_dwKeyPressRepeatDelay( 0l ),
m_fLeftMenuAreaBoundary( 0.0f ),
m_fTopMenuAreaBoundary( 0.0f ),
m_fRightMenuAreaBoundary( 0.0f ),
m_fBottomMenuAreaBoundary( 0.0f ),
m_fLeftMenuAreaBorder( 0.0f ),
m_fTopMenuAreaBorder( 0.0f ),
m_fRightMenuAreaBorder( 0.0f ),
m_fBottomMenuAreaBorder( 0.0f ),
m_fMenuItemVertSpace( 0.0f ),
m_fMenuItemHorizSpace( 0.0f ),
m_iUpArrowXPos( 0 ),
m_iUpArrowYPos( 0 ),
m_iDownArrowXPos( 0 ),
m_iDownArrowYPos( 0 ),
m_uiMenuItemFontHeight( 0 ),
m_uiMenuItemFontAliasLevel( 0 ),
m_fMenuItemSelectorBorder( 0.0f ),
m_dwMenuItemEnabledFGColor( 0l ),
m_dwMenuItemEnabledBGColor( 0l ),
m_dwMenuItemDisabledFGColor( 0l ),
m_dwMenuItemDisabledBGColor( 0l ),
m_dwMenuItemHighlightedFGColor( 0l ),
m_dwMenuItemHighlightedBGColor( 0l ),
m_dwSelectedItemFGColor( 0l ),
m_dwSelectedItemBGColor( 0l ),
m_dwSelectedItemHLFGColor( 0l ),
m_dwSelectedItemHLBGColor( 0l )
{
    m_ScreenItems.SetDeleteDataItem( TRUE );
    m_MenuItems.SetDeleteDataItem( TRUE );

    // Set our menu properties to the defaults
    SetMenuAreaBoundaries();
    SetMenuAreaBorders();
    SetMenuItemVertSpace();
    SetMenuItemFontProperties();
    SetMenuItemColors();
    SetMenuItemSelectedColors();
    SetMenuItemSelectorProperties();
    SetDelayIntervals();
    SetArrowPositions();

	m_UpArrow.UpdateItem( "d:\\media\\images\\uparrowsm.bmp", "d:\\media\\images\\uparrowsm_alpha.bmp", m_iUpArrowXPos, m_iUpArrowYPos );
	m_DownArrow.UpdateItem( "d:\\media\\images\\downarrowsm.bmp", "d:\\media\\images\\downarrowsm_alpha.bmp", m_iDownArrowXPos, m_iDownArrowYPos );
}


// Destructor
CMenuScreen::~CMenuScreen( void )
{
    if( NULL != m_pwszMenuTitle )
    {
        delete[] m_pwszMenuTitle;
        m_pwszMenuTitle = NULL;
    }

    // Clear out our linked lists
    ClearScreenItems();
    ClearMenuItems();
}


// Draws a menu on to the screen
void CMenuScreen::Action( CUDTexture* pTexture )
{
    // Check to see if we should update our texture.  If not, bail
    if( !m_bUpdateTexture )
        return;
    else
        m_bUpdateTexture = FALSE;

    pTexture->Lock();

    // Draw the screen items
    RenderScreenItems( pTexture );

    // Draw the screen title
    RenderMenuTitle( pTexture );

    // Render the highlighter (selector)
    RenderSelector( pTexture );
   
    // Render the Menu Items
    RenderMenuItems( pTexture );

    // Draw the display arrows
    RenderDisplayArrows( pTexture );

    pTexture->Unlock();
}


// Render the display arrows which will indicate more info off screen
void CMenuScreen::RenderDisplayArrows( CUDTexture* pTexture )
{
    // Don't render anything if we don't have any menu items
    if( 0 == GetNumMenuItems() )
        return;

    if( 0 != GetTopItemIndex() )
		m_UpArrow.Render( pTexture );

    if( GetBottomItemIndex() < ( GetNumMenuItems() - 1 ) )
		m_DownArrow.Render( pTexture );

	// Render the line if necessary
	if( ( 0 != GetTopItemIndex() ) || ( GetBottomItemIndex() < ( GetNumMenuItems() - 1 ) ) )
		pTexture->DrawLine( (float)m_iUpArrowXPos - 3, (float)m_iUpArrowYPos, (float)m_iUpArrowXPos - 3, (float)m_iDownArrowYPos + m_DownArrow.GetBitmapHeight(), 1, m_dwMenuItemEnabledFGColor );
}


// Render the menu title on to the menu
void CMenuScreen::RenderMenuTitle( CUDTexture* pTexture )
{
    // TODO: Should this be in a different place?
    // Render the App Title here
    XFONT_SetTextHeight( GetHeaderFont(), 25 );
    pTexture->DrawText( GetHeaderFont(), gc_fLEFT_MENU_AREA_BORDER, 30, gc_dwCOLOR_TEXT_YELLOW, gc_dwCOLOR_DARK_GREY, L"HVS Launcher" );
    XFONT_SetTextHeight( GetHeaderFont(), gc_uiDEFAULT_FONT_HEIGHT );

    pTexture->DrawText( GetHeaderFont(), gc_fLEFT_MENU_AREA_BORDER, 60, gc_dwCOLOR_TEXT_GREEN, gc_dwCOLOR_DARK_GREY, m_pwszMenuTitle );
}


// Render the screen items properly on our menu
void CMenuScreen::RenderScreenItems( CUDTexture* pTexture )
{
    // Don't render anything if we don't have any screen items
    if( 0 == GetNumScreenItems() )
        return;

    m_ScreenItems.MoveFirst(); // Start at the 1st Item
    for( unsigned int x = 0; x < GetNumScreenItems(); ++x )
    {
        m_ScreenItems.GetCurrentNode()->Render( pTexture );
        m_ScreenItems.MoveNext();
    }
}


// Render the menu items properly on our menu
void CMenuScreen::RenderMenuItems( CUDTexture* pTexture )
{
    // Don't render anything if we don't have any menu items
    if( 0 == GetNumMenuItems() )
        return;

    if( IsSingleListMenu() )
    //
    // Render the menu items for a single list menu
    //
    {
        m_MenuItems.MoveTo( GetTopItemIndex() );    // Start at menu item that needs to be displayed
        for( unsigned int x = GetTopItemIndex(); x <= GetBottomItemIndex(); ++x )
        {
            // Determine where on the screen the menu item should be
            float fXPos = m_fLeftMenuAreaBoundary + m_fLeftMenuAreaBorder;
            float fYPos = m_fTopMenuAreaBoundary + m_fTopMenuAreaBorder + ( ( m_fMenuItemVertSpace + m_uiMenuItemFontHeight ) * ( x - GetTopItemIndex() ) );

            // Distinguish between enabled and disabled items
            DWORD dwFGColor, dwBGColor;
            if( m_MenuItems.GetCurrentNode()->m_bEnabled )
            {
                // Check to see if it's highlighted
                if( m_MenuItems.GetCurrentNode()->m_bHighlighted )
                {
                    // Check to see if it's the currently selected item
                    if( x == GetSelectedItem() )
                    {
                        dwFGColor = m_dwSelectedItemHLFGColor;
                        dwBGColor = m_dwSelectedItemHLBGColor;
                    }
                    else
                    {
                        dwFGColor = m_dwMenuItemHighlightedFGColor;
                        dwBGColor = m_dwMenuItemHighlightedBGColor;
                    }

                }
                else
                {
                    // Check to see if it's our currently selected item
                    if( x == GetSelectedItem() )
                    {
                        dwFGColor = m_dwSelectedItemFGColor;
                        dwBGColor = m_dwSelectedItemBGColor;
                    }
                    else
                    {
                        dwFGColor = m_dwMenuItemEnabledFGColor;
                        dwBGColor = m_dwMenuItemEnabledBGColor;
                    }
                }
            }
            else
            {
                dwFGColor = m_dwMenuItemDisabledFGColor;
                dwBGColor = m_dwMenuItemDisabledBGColor;
            }

            // Draw the item text to the screen (left justified)
            if( m_MenuItems.GetCurrentNode()->m_bHighlighted )
            {
                int iPixelWidth = GetStringPixelWidth( GetBodyFont(), L"\x221A" ) + 4;
                pTexture->DrawText( m_MenuItems.GetCurrentNode()->m_pFont, fXPos - iPixelWidth, fYPos, dwFGColor, dwBGColor, L"%c", L'\x221A' );
                pTexture->DrawText( m_MenuItems.GetCurrentNode()->m_pFont, fXPos, fYPos, dwFGColor, dwBGColor, L"%ls", m_MenuItems.GetCurrentNode()->m_pwszItemName );
                // pTexture->DrawText( m_MenuItems.GetCurrentNode()->m_pFont, fXPos, fYPos, dwFGColor, dwBGColor, L"\x221A %ls", m_MenuItems.GetCurrentNode()->m_pwszItemName );
            }
            else
            {
                pTexture->DrawText( m_MenuItems.GetCurrentNode()->m_pFont, fXPos, fYPos, dwFGColor, dwBGColor, L"%ls", m_MenuItems.GetCurrentNode()->m_pwszItemName );
            }

            // Draw the value right justified, if needed
            if( ( NULL != m_MenuItems.GetCurrentNode()->m_pValueFont ) && ( NULL != m_MenuItems.GetCurrentNode()->m_pwszItemValue ) )
            {
                fXPos = m_fRightMenuAreaBoundary - GetStringPixelWidth( m_MenuItems.GetCurrentNode()->m_pValueFont, m_MenuItems.GetCurrentNode()->m_pwszItemValue ) - m_fRightMenuAreaBorder;
                pTexture->DrawText( m_MenuItems.GetCurrentNode()->m_pValueFont, fXPos, fYPos, gc_dwCOLOR_TEXT_YELLOW, dwBGColor, m_MenuItems.GetCurrentNode()->m_pwszItemValue );
            }
        
            m_MenuItems.MoveNext();
        }
    }
    else
    //
    // Render the menu items for a multi list menu
    //
    {
        // Draw a separator line in the middle of the menu
        pTexture->DrawLine( gc_fMENU_AREA_HORIZ_CENTER, gc_fTOP_MENU_AREA_BOUNDARY, gc_fMENU_AREA_HORIZ_CENTER, gc_fBOTTOM_MENU_AREA_BOUNDARY, 2, gc_dwCOLOR_BLACK );

        unsigned int uiRowNum = 0;

        m_MenuItems.MoveTo( GetTopItemIndex() );    // Start at menu item that needs to be displayed
        for( unsigned int x = GetTopItemIndex(); x <= GetBottomItemIndex(); ++x )
        {
            float fXPos = 0.0f;
            float fYPos = 0.0f;

            if( 0 == ( x % 2 ) )
            //
            // Left Side Menu Item
            //
            {
                fXPos = m_fLeftMenuAreaBoundary + m_fLeftMenuAreaBorder;
                fYPos = m_fTopMenuAreaBoundary + m_fTopMenuAreaBorder + ( ( m_fMenuItemVertSpace + m_uiMenuItemFontHeight ) * uiRowNum );
            }
            else
            //
            // Right Side Menu Item
            //
            {
                fXPos = gc_fMENU_AREA_HORIZ_CENTER + 6;
                fYPos = m_fTopMenuAreaBoundary + m_fTopMenuAreaBorder + ( ( m_fMenuItemVertSpace + m_uiMenuItemFontHeight ) * uiRowNum );

                uiRowNum++; // Increase our row number for the next loop
            }


            // Distinguish between enabled and disabled items
            if( m_MenuItems.GetCurrentNode()->m_bEnabled )
            {
                // Check to see if it's highlighted
                if( m_MenuItems.GetCurrentNode()->m_bHighlighted )
                    pTexture->DrawText( m_MenuItems.GetCurrentNode()->m_pFont, fXPos, fYPos, m_dwMenuItemHighlightedFGColor, m_dwMenuItemHighlightedBGColor, m_MenuItems.GetCurrentNode()->m_pwszItemName );
                else
                    pTexture->DrawText( m_MenuItems.GetCurrentNode()->m_pFont, fXPos, fYPos, m_dwMenuItemEnabledFGColor, m_dwMenuItemEnabledBGColor, m_MenuItems.GetCurrentNode()->m_pwszItemName );
            }
            else
                pTexture->DrawText( m_MenuItems.GetCurrentNode()->m_pFont, fXPos, fYPos, m_dwMenuItemDisabledFGColor, m_dwMenuItemDisabledBGColor, m_MenuItems.GetCurrentNode()->m_pwszItemName );
        
            m_MenuItems.MoveNext();
        }
    }
}

// Render the selector for our menu
void CMenuScreen::RenderSelector( CUDTexture* pTexture )
{
    // If we don't have any menu items, we can't have anything selected
    if( 0 == GetNumMenuItems() )
        return;

    float fX1Pos = 0.0f;
    float fY1Pos = 0.0f;
    float fX2Pos = 0.0f;
    float fY2Pos = 0.0f;

    if( IsSingleListMenu() )
    {
        // Render the Selector Bar for the selected Menu Item
        fX1Pos = m_fLeftMenuAreaBoundary + m_fLeftMenuAreaBorder - m_fMenuItemSelectorBorder;
        fY1Pos = m_fTopMenuAreaBoundary + m_fTopMenuAreaBorder + ( ( m_fMenuItemVertSpace + m_uiMenuItemFontHeight ) * GetSelectorPosition() ) - m_fMenuItemSelectorBorder;
        fX2Pos = m_fRightMenuAreaBoundary - m_fRightMenuAreaBorder + m_fMenuItemSelectorBorder;
        fY2Pos = fY1Pos + m_uiMenuItemFontHeight + ( 2 * m_fMenuItemSelectorBorder );
    }
    else    // Calculate selector for Multi List
    {
        if( 0 == ( GetSelectedItem() % 2 ) )
        //
        // Left Side Menu Item
        //
        {
            fX1Pos = m_fLeftMenuAreaBoundary + m_fLeftMenuAreaBorder - m_fMenuItemSelectorBorder;
            fY1Pos = m_fTopMenuAreaBoundary + m_fTopMenuAreaBorder + ( ( m_fMenuItemVertSpace + m_uiMenuItemFontHeight ) * ( GetSelectorPosition() / 2 ) ) - m_fMenuItemSelectorBorder;
            fX2Pos = gc_fMENU_AREA_HORIZ_CENTER - 6 + m_fMenuItemSelectorBorder;
            fY2Pos = fY1Pos + m_uiMenuItemFontHeight + ( 2 * m_fMenuItemSelectorBorder );
        }
        else
        //
        // Right Side Menu Item
        //
        {
            fX1Pos = gc_fMENU_AREA_HORIZ_CENTER + 6 - m_fMenuItemSelectorBorder;
            fY1Pos = m_fTopMenuAreaBoundary + m_fTopMenuAreaBorder + ( ( m_fMenuItemVertSpace + m_uiMenuItemFontHeight ) * ( GetSelectorPosition() / 2 ) ) - m_fMenuItemSelectorBorder;
            fX2Pos = m_fRightMenuAreaBoundary - m_fRightMenuAreaBorder + m_fMenuItemSelectorBorder;
            fY2Pos = fY1Pos + m_uiMenuItemFontHeight + ( 2 * m_fMenuItemSelectorBorder );
        }
    }

    pTexture->DrawBox( fX1Pos, fY1Pos, fX2Pos, fY2Pos, m_dwMenuItemSelectorColor );
}

// Initialize the Menu
HRESULT CMenuScreen::Init( XFONT* pMenuItemFont, XFONT* pMenuTitleFont )
{
    return S_OK;
}


// Add a text item to our current screen from a file
HRESULT CMenuScreen::AddTextItem( XFONT* pFont, WCHAR* pwszText, float fXPos, float fYPos, DWORD dwFGColor, DWORD dwBGColor )
{
    CTextItem* pTextItem = new CTextItem;
    if( !pTextItem )
    {
        DebugPrint( "CMenuScreen::AddTextItem():Out of memory!!\n" );

        return E_OUTOFMEMORY;
    }

    // Update the item with the information that we pulled from the file
    pTextItem->UpdateItem( pFont, pwszText, fXPos, fYPos, dwFGColor, dwBGColor );

    // Add the item to our list of Screen Items
    m_ScreenItems.AddNode( pTextItem );
    
    return S_OK;
}


// Add a panel item to our current screen
HRESULT CMenuScreen::AddPanelItem( float fX1Pos, float fY1Pos, float fX2Pos, float fY2Pos, DWORD dwColor )
{
    CPanelItem* pPanelItem = new CPanelItem;
    if( !pPanelItem )
    {
        DebugPrint( "CMenuScreen::AddPanelItem():Out of memory!!\n" );

        return E_OUTOFMEMORY;
    }

    // Update the item with the information that we pulled from the file
    pPanelItem->UpdateItem( fX1Pos, fY1Pos, fX2Pos, fY2Pos, dwColor );

    // Add the item to our list of Screen Items
    m_ScreenItems.AddNode( pPanelItem );

    return S_OK;
}


// Add a line to our current screen
HRESULT CMenuScreen::AddLineItem( float fX1Pos, float fY1Pos, float fX2Pos, float fY2Pos, float fWidth, DWORD dwColor )
{
    CLineItem* pLineItem = new CLineItem;
    if( !pLineItem )
    {
        DebugPrint( "CMenuScreen::AddLineItem():Out of memory!!\n" );

        return E_OUTOFMEMORY;
    }

    // Update the item with the information that we pulled from the file
    pLineItem->UpdateItem( fX1Pos, fY1Pos, fX2Pos, fY2Pos, fWidth, dwColor );

    // Add the item to our list of Screen Items
    m_ScreenItems.AddNode( pLineItem );

    return S_OK;
}


// Add a line to our current screen
HRESULT CMenuScreen::AddOutlineItem( float fX1Pos, float fY1Pos, float fX2Pos, float fY2Pos, float fWidth, DWORD dwColor )
{
    COutlineItem* pOutlineItem = new COutlineItem;
    if( !pOutlineItem )
    {
        DebugPrint( "CMenuScreen::AddOutlineItem():Out of memory!!\n" );

        return E_OUTOFMEMORY;
    }

    // Update the item with the information that we pulled from the file
    pOutlineItem->UpdateItem( fX1Pos, fY1Pos, fX2Pos, fY2Pos, fWidth, dwColor );

    // Add the item to our list of Screen Items
    m_ScreenItems.AddNode( pOutlineItem );

    return S_OK;
}


// Add a Bitmap Item to our Screen from a file
HRESULT CMenuScreen::AddBitmapItem( char* pszFilename, int nXPos, int nYPos )
{
    CBitmapItem* pBitmapItem = new CBitmapItem;
    if( !pBitmapItem )
    {
        DebugPrint( "CMenuScreen::AddBitmapItem():Out of memory!!\n" );

        return E_OUTOFMEMORY;
    }

    // Update the item with the information that we pulled from the file
    pBitmapItem->UpdateItem( pszFilename, nXPos, nYPos );

    // Add the item to our list of Screen Items
    m_ScreenItems.AddNode( pBitmapItem );

    return S_OK;
}


// Add a Bitmap Item to our Screen from a file
HRESULT CMenuScreen::AddBitmapAlphaItem( char* pszFilename, char* pszAlphaFilename, int nXPos, int nYPos )
{
    CBitmapAlphaItem* pBitmapAlphaItem = new CBitmapAlphaItem;
    if( NULL == pBitmapAlphaItem )
    {
        DebugPrint( "CMenuScreen::AddBitmapAlphaItem():Out of memory!!\n" );

        return E_OUTOFMEMORY;
    }

    // Update the item with the information that we pulled from the file
    pBitmapAlphaItem->UpdateItem( pszFilename, pszAlphaFilename, nXPos, nYPos );

    // Add the item to our list of Screen Items
    m_ScreenItems.AddNode( pBitmapAlphaItem );

    return S_OK;
}


// Update an existing menu item
HRESULT CMenuScreen::UpdateMenuItem( unsigned int uiIndex, WCHAR* pwszItemName, BOOL bHighlighted, BOOL bEnabled, WCHAR* pwszItemValue/*=NULL*/ )
{
    if( NULL == pwszItemName || uiIndex >= GetNumMenuItems() )
    {
        DebugPrint( "CMenuScreen::AddMenuItem():Invalid argument(s) passed in!!\n" );

        return E_INVALIDARG;
    }

    UpdateTexture();

    wcsncpy( m_MenuItems[uiIndex]->m_pwszItemName, pwszItemName, 255 );
    m_MenuItems[uiIndex]->m_bHighlighted = bHighlighted;
    m_MenuItems[uiIndex]->m_bEnabled = bEnabled;

    if( NULL != pwszItemValue )
        wcsncpy( m_MenuItems[uiIndex]->m_pwszItemValue, pwszItemValue, 255 );

    return S_OK;
}

// Used to add Menu Items to the current Menu
HRESULT CMenuScreen::AddMenuItem( XFONT* pFont, WCHAR* pwszItemName, BOOL bHighlighted, BOOL bEnabled, int nItemValue, XFONT* pValueFont/*=NULL*/, WCHAR* pwszItemValue/*=NULL*/ )
{
    if( NULL == pwszItemName || NULL == pFont )
    {
        DebugPrint( "CMenuScreen::AddMenuItem():Invalid argument(s) passed in!!\n" );

        return E_INVALIDARG;
    }

    CMenuItem* pMenuItem = new CMenuItem;
    if( NULL == pMenuItem )
    {
        DebugPrint( "CMenuScreen::AddMenuItem():Ran out of memory!!\n" );

        return E_OUTOFMEMORY;
    }
    
    wcsncpy( pMenuItem->m_pwszItemName, pwszItemName, 255 );

    pMenuItem->m_pFont = pFont;
    pMenuItem->m_bHighlighted = bHighlighted;
    pMenuItem->m_bEnabled = bEnabled;
    pMenuItem->m_nItemValue = nItemValue;

    // Deal with the menu item value (if necessary)
    if( ( NULL != pValueFont ) && ( NULL != pwszItemValue ) )
    {
        wcsncpy( pMenuItem->m_pwszItemValue, pwszItemValue, 255 );
        pMenuItem->m_pValueFont = pValueFont;
    }

    m_MenuItems.AddNode( pMenuItem );

    // If this is our first menu item, select it
    if( 1 == m_MenuItems.GetNumItems() )
    {
        SetSelectedItem( 0 );
    }

	// Adjust our right border if we have enough menu items to display 'arrows'
	// BUGBUG: This is overriding our user defined values!!  That is bad
	if( m_MenuItems.GetNumItems() > GetNumMenuItemsToDisplay() )
		m_fRightMenuAreaBorder = gc_fRIGHT_MENU_AREA_BORDER + 37;
	else
		m_fRightMenuAreaBorder = gc_fRIGHT_MENU_AREA_BORDER;

    return S_OK;
}


// Returns the number of items that should appear on the menu
unsigned int CMenuScreen::GetNumMenuItemsToDisplay( void )
{
    float fPanelHeight = m_fBottomMenuAreaBoundary - m_fTopMenuAreaBoundary;
    unsigned int uiReturnVal = (unsigned int)( ( fPanelHeight - m_fTopMenuAreaBorder ) / ( m_uiMenuItemFontHeight + m_fMenuItemVertSpace ) );

    // Double the number of items on the menu if it's a multi list menu
    if( !IsSingleListMenu() )
        uiReturnVal *= 2;
    
    return uiReturnVal;
}


// Get the index of the top menu item that should be displayed
HRESULT CMenuScreen::SetTopItemIndex( unsigned int uiIndex )
{
    if( IsSingleListMenu() )
    {
        // Make sure the passed in index is either 0, or within the realm of display
        if( ( ( uiIndex + GetNumMenuItemsToDisplay() - 1 ) < GetNumMenuItems() ) || ( 0 == uiIndex ) )
        {
            m_uiTopItemIndex = uiIndex;
        }
        else
        {
            DebugPrint( "CMenuScreen::SetTopItemIndex():Invalid index passed!! - '%d'\n", uiIndex );

            return E_INVALIDARG;
        }
    }
    else
    {
        // Make sure the passed in index is either 0, or within the realm of display
        if( ( ( uiIndex + GetNumMenuItemsToDisplay() - 1 ) < ( GetNumMenuItems() + 1 ) ) || ( 0 == uiIndex ) )
        {
            m_uiTopItemIndex = uiIndex;
        }
        else
        {
            DebugPrint( "CMenuScreen::SetTopItemIndex():Invalid index passed!! - '%d'\n", uiIndex );

            return E_INVALIDARG;
        }
    }

    return S_OK;
}


// Get the index of the bottom menu item that should be displayed
unsigned int CMenuScreen::GetBottomItemIndex( void )
{
    unsigned int uiReturnVal = GetTopItemIndex() + GetNumMenuItemsToDisplay() - 1;
    
    if( uiReturnVal > ( GetNumMenuItems() - 1 ) )
        uiReturnVal = GetNumMenuItems() - 1;

    return uiReturnVal;
}


// Set the boundaries of the menu area (where the menu items will live)
void CMenuScreen::SetMenuAreaBoundaries( float fX1/*=gc_fLEFT_MENU_AREA_BOUNDARY*/,
                                         float fY1/*=gc_fTOP_MENU_AREA_BOUNDARY*/,
                                         float fX2/*=gc_fRIGHT_MENU_AREA_BOUNDARY*/,
                                         float fY2/*=gc_fBOTTOM_MENU_AREA_BOUNDARY*/ )
{
    m_fLeftMenuAreaBoundary = fX1;        // Left boundary of where the menu items will appear
    m_fTopMenuAreaBoundary = fY1;         // Top boundary of where the menu items will appear
    m_fRightMenuAreaBoundary = fX2;       // Right boundary of where the menu items will appear
    m_fBottomMenuAreaBoundary = fY2;      // Bottom boundary of where the menu items will appear
}


// Set the borders of the menu area (where the menu items will live)
void CMenuScreen::SetMenuAreaBorders( float fLeftBorder/*=gc_fLEFT_MENU_AREA_BORDER*/,
                                      float fTopBorder/*=gc_fTOP_MENU_AREA_BORDER*/,
                                      float fRightBorder/*=gc_fRIGHT_MENU_AREA_BORDER*/,
                                      float fBottomBorder/*=gc_fBOTTOM_MENU_AREA_BORDER*/ )
{
    m_fLeftMenuAreaBorder = fLeftBorder;        // Left border of where the menu items will appear
    m_fTopMenuAreaBorder = fTopBorder;          // Top border of where the menu items will appear
    m_fRightMenuAreaBorder = fRightBorder;      // Right border of where the menu items will appear
    m_fBottomMenuAreaBorder = fBottomBorder;    // Bottom border of where the menu items will appear
}


// Set the amount of time a button should delay before repeating
void CMenuScreen::SetDelayIntervals( DWORD dwInitialDelay/*=gc_dwMENU_BUTTONPRESS_INITIAL_DELAY*/,
                                     DWORD dwRepeatDelay/*=gc_dwMENU_BUTTONPRESS_REPEAT_DELAY*/ )
{
    m_dwKeyPressInitialDelay = dwInitialDelay;
    m_dwKeyPressRepeatDelay = dwRepeatDelay;
}


void CMenuScreen::SetMenuItemVertSpace( float fVertSpace/*=gc_fMENU_ITEM_VERT_SPACE*/,
                                        float fHorizSpace/*=gc_fMENU_ITEM_HORIZ_SPACE*/ )
{
    m_fMenuItemVertSpace = fVertSpace;
    m_fMenuItemHorizSpace = fHorizSpace;
}

// Set the font height of the menu items
void CMenuScreen::SetMenuItemFontProperties( unsigned int uiFontHeight/*=gc_uiDEFAULT_FONT_HEIGHT*/,
                                             unsigned int uiFontAliasLevel/*=gc_uiDEFAULT_FONT_ALIAS_LEVEL*/ )
{
    m_uiMenuItemFontHeight = uiFontHeight;          // Font height of a menu item
    m_uiMenuItemFontAliasLevel = uiFontAliasLevel;  // Alias level of a menu item
}

// Set the menu item selector properties
void CMenuScreen::SetMenuItemSelectorProperties( float fBorder/*=gc_fMENU_SELECTOR_BORDER*/,
                                                 DWORD dwColor/*=gc_dwMENU_SELECTOR_COLOR*/ )
{
    m_fMenuItemSelectorBorder = fBorder;
    m_dwMenuItemSelectorColor = dwColor;
}

// Set the colors of the menu items
void CMenuScreen::SetMenuItemColors( DWORD dwEnabledItemFGColor/*=gc_fMENU_ITEM_ENABLED_FG_COLOR*/,
                                     DWORD dwEnabledItemBGColor/*=gc_fMENU_ITEM_ENABLED_BG_COLOR*/,
                                     DWORD dwDisabledItemFGColor/*=gc_fMENU_ITEM_DISABLED_FG_COLOR*/,
                                     DWORD dwDisabledItemBGColor/*=gc_fMENU_ITEM_DISABLED_BG_COLOR*/,
                                     DWORD dwHighlightedItemFGColor/*=gc_fMENU_ITEM_HIGHLIGHTED_FG_COLOR*/,
                                     DWORD dwHighlightedItemBGColor/*=gc_fMENU_ITEM_HIGHLIGHTED_BG_COLOR*/ )
{
    m_dwMenuItemEnabledFGColor = dwEnabledItemFGColor;          // Foreground color for an enabled menu item
    m_dwMenuItemEnabledBGColor = dwEnabledItemBGColor;          // Background color for an enabled menu item
    m_dwMenuItemDisabledFGColor = dwDisabledItemFGColor;        // Foreground color for a disabled menu item
    m_dwMenuItemDisabledBGColor = dwDisabledItemBGColor;        // Background color for a disabled menu item
    m_dwMenuItemHighlightedFGColor = dwHighlightedItemFGColor;  // Foreground color for a highlighted menu item
    m_dwMenuItemHighlightedBGColor = dwHighlightedItemBGColor;  // Background color for a highlighted menu item
}


// Set the colors of the menu items
void CMenuScreen::SetMenuItemSelectedColors( DWORD dwSelectedItemFGColor/*=gc_fMENU_ITEM_SELECTED_FG_COLOR*/,
                                             DWORD dwSelectedItemBGColor/*=gc_fMENU_ITEM_SELECTED_BG_COLOR*/,
                                             DWORD dwSelectedHLItemFGColor/*=gc_fMENU_ITEM_SELECTED_HL_FG_COLOR*/,
                                             DWORD dwSelectedHLItemBGColor/*=gc_fMENU_ITEM_SELECTED_HL_BG_COLOR*/ )
{
    m_dwSelectedItemFGColor = dwSelectedItemFGColor;
    m_dwSelectedItemBGColor = dwSelectedItemBGColor;
    m_dwSelectedItemHLFGColor = dwSelectedHLItemFGColor;
    m_dwSelectedItemHLBGColor = dwSelectedHLItemBGColor;
}

// Set the positions of the arrows on our menu
void CMenuScreen::SetArrowPositions( int iUpArrowXPos/*=gc_iUPARROWXPOS*/,
                                     int iUpArrowYPos/*=gc_iUPARROWYPOS*/,
                                     int iDownArrowXPos/*=gc_iDOWNARROWXPOS*/,
                                     int iDownArrowYPos/*=gc_iDOWNARROWYPOS*/ )
{
    m_iUpArrowXPos = iUpArrowXPos;
    m_iUpArrowYPos = iUpArrowYPos;
    m_iDownArrowXPos = iDownArrowXPos;
    m_iDownArrowYPos = iDownArrowYPos;
}

// Return a pointer to the menu title
HRESULT CMenuScreen::SetMenuTitle( XFONT* pMenuTitleFont, WCHAR* pwszMenuTitle )
{
    if( NULL == pwszMenuTitle || NULL == pMenuTitleFont )
    {
        DebugPrint( "CMenuScreen::SetMenuTitle():Invalid argument(s) passed in!!\n" );
        
        return E_INVALIDARG;
    }

    if( NULL != m_pwszMenuTitle )
    {
        delete[] m_pwszMenuTitle;
        m_pwszMenuTitle = NULL;
    }

    unsigned long ulStrLen = wcslen( pwszMenuTitle ) + 1;
    m_pwszMenuTitle = new WCHAR[ulStrLen];
    if( NULL == m_pwszMenuTitle )
    {
        DebugPrint( "CMenuScreen::SetMenuTitle():Out of Memory!!\n" );

        return E_OUTOFMEMORY;
    }
    
    ZeroMemory( m_pwszMenuTitle, sizeof( WCHAR ) * ulStrLen );

    wcscpy( m_pwszMenuTitle, pwszMenuTitle );

    return S_OK;
}


// Handles input from the buttons
void CMenuScreen::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // Process the default button delay
    if( !HandleInputDelay( bFirstPress ) )
        return;

    // Handle Controls being pressed
    switch( buttonPressed )
    {
    case BUTTON_B:
        {
            GoBack();

            break;
        }
    }
}


// Handles input (of the CONTROLS) for the current menu
void CMenuScreen::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
    if( IsSingleListMenu() )
        HandleControlInputSingleList( controlPressed, bFirstPress );
    else
        HandleControlInputMultiList( controlPressed, bFirstPress );
}


// Handle control input for a single list menu
void CMenuScreen::HandleControlInputSingleList( enum CONTROLS controlPressed, BOOL bFirstPress )
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
            if( GetNumMenuItems() > 0 )
            {
                if( GetSelectedItem() > 0 )
                {
                    UpdateTexture();
                    SetSelectedItem( GetSelectedItem() - 1 );

                    // Adjust the screen appropriately
                    if( GetSelectedItem() < GetTopItemIndex() )
                        SetTopItemIndex( GetTopItemIndex() - 1 );
                }
            }
            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            // Only act if we have any items on our screen
            if( GetNumMenuItems() != 0 )
            {
                if( GetSelectedItem() < ( GetNumMenuItems() - 1 ) )
                {
                    UpdateTexture();
                    SetSelectedItem( GetSelectedItem() + 1 );

                    // Adjust the screen appropriately
                    if( GetSelectedItem() > GetBottomItemIndex() )
                        SetTopItemIndex( GetTopItemIndex() + 1 );
                }
            }
            break;
        }
	}
}

// Handle control input for a multi list menu
void CMenuScreen::HandleControlInputMultiList( enum CONTROLS controlPressed, BOOL bFirstPress )
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
            if( GetNumMenuItems() > 0 )
            {
                if( GetSelectedItem() >= 2 )
                {
                    UpdateTexture();
                    SetSelectedItem( GetSelectedItem() - 2 );

                    // TODO: Fix this when we need multi-list menus
                    /*
                    // Adjust the screen appropriately
                    if( GetSelectorPosition() > 1 )
                        SetSelectorPosition( GetSelectorPosition() - 2 );
                    else
                        SetTopItemIndex( GetTopItemIndex() - 2 );
                    */
                }
            }
            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            // Only act if we have any items on our screen
            if( GetNumMenuItems() != 0 )
            {
                if( ( GetSelectedItem() + 2 ) < GetNumMenuItems() )
                {
                    UpdateTexture();
                    SetSelectedItem( GetSelectedItem() + 2 );

                    // TODO: Fix this when we need multi-list menus
                    /*
                    // Adjust the screen appropriately
                    if( GetSelectorPosition() < ( GetNumMenuItemsToDisplay() - 2 ) )
                        SetSelectorPosition( GetSelectorPosition() + 2 );
                    else
                        SetTopItemIndex( GetTopItemIndex() + 2 );
                    */
                }
            }
            break;
        }
    case CONTROL_DPAD_RIGHT:
        {
            // Only act if we have any items on our screen
            if( GetNumMenuItems() > 0 )
            {
                if( ( 0 == ( GetSelectorPosition() % 2 ) ) && ( ( GetSelectedItem() + 1 ) < GetNumMenuItems() ) )
                {
                    UpdateTexture();
                    SetSelectedItem( GetSelectedItem() + 1 );

                    // TODO: Fix this when we need multi-list menus
                    /*
                    // Adjust the screen appropriately
                    SetSelectorPosition( GetSelectorPosition() + 1 );
                    */
                }
            }
            break;
        }
    case CONTROL_DPAD_LEFT:
        {
            // Only act if we have any items on our screen
            if( GetNumMenuItems() > 0 )
            {
                if( ( 0 != ( GetSelectorPosition() % 2 ) ) && ( ( (int)GetSelectedItem() - 1 ) >= 0 ) )
                {
                    UpdateTexture();
                    SetSelectedItem( GetSelectedItem() - 1 );

                    // TODO: Fix this when we need multi-list menus
                    /*
                    // Adjust the screen appropriately
                    SetSelectorPosition( GetSelectorPosition() - 1 );
                    */
                }
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
            if( nThumbY < 0 )  // Move the left joystick down
            {
                // Hand off this to the "DPad" handler
                HandleInput( CONTROL_DPAD_DOWN, bFirstYPress );
            }
            else if( nThumbY > 0 ) // Move left joystick up
            {
                // Hand off this to the "DPad" handler
                HandleInput( CONTROL_DPAD_UP, bFirstYPress );
            }

            // X
            if( nThumbX < 0 )       // Move the left joystick left
            {
                // Hand off this to the "DPad" handler
                HandleInput( CONTROL_DPAD_LEFT, bFirstXPress );
            }
            else if( nThumbX > 0 )  // Move the left joystick right
            {
                // Hand off this to the "DPad" handler
                HandleInput( CONTROL_DPAD_RIGHT, bFirstXPress );
            }
            break;
        }
    }
}


// Handle the default delay before for Input
BOOL CMenuScreen::HandleInputDelay( BOOL bFirstPress )
{
    // If this is a repeat button press, let's delay a bit
    if( bFirstPress )
    {
        m_dwKeyPressDelayTimer = GetTickCount();
        m_dwKeyPressDelayInterval = m_dwKeyPressInitialDelay;
    }
    else // Check to see if the repeat press is within our timer, otherwise bail
    {
        // If the interval is too small, bail
        if( ( GetTickCount() - m_dwKeyPressDelayTimer ) < m_dwKeyPressDelayInterval )
        {
            return FALSE;
        }
        m_dwKeyPressDelayTimer = GetTickCount();
        m_dwKeyPressDelayInterval = m_dwKeyPressRepeatDelay;
    }

    return TRUE;
}


// Sets the currently selected item on our screen
HRESULT CMenuScreen::SetSelectedItem( unsigned int uiItemNumber )
{
    if( uiItemNumber > GetNumMenuItems() )
    {
        DebugPrint( "CMenuScreen::SetSelectedItem():Tried to select item out of range! - '%d', MAX - '%d'\n", uiItemNumber, GetNumMenuItems() - 1 );

        return E_INVALIDARG;
    }

    m_uiSelectedItem = uiItemNumber;

    return S_OK;
}


// Allows user to set a single, or multi list menu
void CMenuScreen::SetSingleListMenu( BOOL bMenuType )
{
    m_bSingleList = bMenuType;  // TRUE for Single List, FALSE for Multi List
}


// Should be called whenever a user enters or activates this menu
void CMenuScreen::Enter( void )
{
    UpdateTexture();

    m_dwKeyPressDelayTimer = GetTickCount();
    m_dwKeyPressDelayInterval = m_dwKeyPressInitialDelay;
}


// Should be called whenever a user leaves the menu
void CMenuScreen::Leave( CXItem* pItem )
{
    SetPreviousMenu( this );
    SetCurrentMenu( pItem );
}

// Go back to the previous menu
void CMenuScreen::GoBack( void )
{
    if( NULL != m_BackPtr )
        Leave( m_BackPtr );
}


// Clear all menu items from the list
void CMenuScreen::ClearMenuItems( void )
{ 
    SetSelectedItem( 0 );
    SetTopItemIndex( 0 );

    m_MenuItems.EmptyList();
};


// Get the position of the selector on the menu
unsigned int CMenuScreen::GetSelectorPosition( void )
{
    return GetSelectedItem() - GetTopItemIndex();
};