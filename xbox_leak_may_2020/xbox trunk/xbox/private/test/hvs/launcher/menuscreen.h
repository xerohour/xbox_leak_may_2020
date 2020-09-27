/*****************************************************
*** menuscreen.h
***
*** Header file for our XShell Generic menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** December 1st , 2000
***
*****************************************************/

#ifndef _MENUSCREEN_H_
#define _MENUSCREEN_H_

#include "xitem.h"
#include "screenitems.h"

// Menu Area Boundaries                                             
const float         gc_fLEFT_MENU_AREA_BOUNDARY =                   0.0f;
const float         gc_fTOP_MENU_AREA_BOUNDARY =                    101.0f;
const float         gc_fRIGHT_MENU_AREA_BOUNDARY =                  639.0f;
const float         gc_fBOTTOM_MENU_AREA_BOUNDARY =                 375.0f;
const float         gc_fMENU_AREA_HORIZ_CENTER =                    ( gc_fRIGHT_MENU_AREA_BOUNDARY - gc_fLEFT_MENU_AREA_BOUNDARY ) / 2.0f;
const float         gc_fMENU_AREA_VERT_CENTER =                     ( gc_fBOTTOM_MENU_AREA_BOUNDARY - gc_fTOP_MENU_AREA_BOUNDARY ) / 2.0f;
                                                                    
// Menu Area Borders                                                
const float         gc_fLEFT_MENU_AREA_BORDER =                     40.0f;
const float         gc_fTOP_MENU_AREA_BORDER =                      10.0f;
const float         gc_fRIGHT_MENU_AREA_BORDER =                    40.0f;
const float         gc_fBOTTOM_MENU_AREA_BORDER =                   10.0f;

// Menu Area spacing                                                
const float         gc_fMENU_ITEM_VERT_SPACE =                      5.0f;
const float         gc_fMENU_ITEM_HORIZ_SPACE =                     5.0f;

// Menu Area Colors
const DWORD         gc_fMENU_ITEM_ENABLED_FG_COLOR =                gc_dwCOLOR_TEXT_GREEN;  // gc_dwCOLOR_DIM_YELLOW;
const DWORD         gc_fMENU_ITEM_ENABLED_BG_COLOR =                gc_dwCOLOR_BLACK;
const DWORD         gc_fMENU_ITEM_DISABLED_FG_COLOR =               gc_dwCOLOR_BLACK;
const DWORD         gc_fMENU_ITEM_DISABLED_BG_COLOR =               gc_dwCOLOR_BLACK;
const DWORD         gc_fMENU_ITEM_HIGHLIGHTED_FG_COLOR =            gc_dwCOLOR_TEXT_YELLOW; // gc_dwCOLOR_WHITE;
const DWORD         gc_fMENU_ITEM_HIGHLIGHTED_BG_COLOR =            gc_dwCOLOR_BLACK;

// Menu Select Bar Properties
const float         gc_fMENU_SELECTOR_BORDER =                      2.0f;
const DWORD         gc_dwMENU_SELECTOR_COLOR =                      gc_dwCOLOR_SELECTOR_BLACK; // gc_dwCOLOR_DARK_GREEN;

// Selected Item Colors
const DWORD         gc_fMENU_ITEM_SELECTED_FG_COLOR =               gc_dwCOLOR_WHITE;
const DWORD         gc_fMENU_ITEM_SELECTED_BG_COLOR =               gc_dwMENU_SELECTOR_COLOR;
const DWORD         gc_fMENU_ITEM_SELECTED_HL_FG_COLOR =            gc_dwCOLOR_WHITE;
const DWORD         gc_fMENU_ITEM_SELECTED_HL_BG_COLOR =            gc_dwMENU_SELECTOR_COLOR;

// Delay Interval Defaults
const DWORD         gc_dwMENU_BUTTONPRESS_INITIAL_DELAY =           400;    // milliseconds
const DWORD         gc_dwMENU_BUTTONPRESS_REPEAT_DELAY =            50;     // milliseconds

// Status Bar Text Position & Color Info
const float         gc_fSTATUS_TEXT_COL1_XPOS =                     gc_fLEFT_MENU_AREA_BOUNDARY + gc_fLEFT_MENU_AREA_BORDER;
const float         gc_fSTATUS_TEXT_COL2_XPOS =                     gc_fLEFT_MENU_AREA_BOUNDARY + gc_fLEFT_MENU_AREA_BORDER + 200; // gc_fSTATUS_TEXT_COL1_XPOS + 200.0f;
const float         gc_fSTATUS_TEXT_COL3_XPOS =                     gc_fLEFT_MENU_AREA_BOUNDARY + gc_fLEFT_MENU_AREA_BORDER + 400; // gc_fSTATUS_TEXT_COL2_XPOS + 200.0f;
const float         gc_fSTATUS_TEXT_ROW1_YPOS =                     gc_fBOTTOM_MENU_AREA_BOUNDARY + 15.0f;
const float         gc_fSTATUS_TEXT_ROW2_YPOS =                     gc_fBOTTOM_MENU_AREA_BOUNDARY + gc_uiDEFAULT_FONT_HEIGHT + 19.0f; // gc_fSTATUS_TEXT_ROW1_YPOS + gc_uiDEFAULT_FONT_HEIGHT + 4;

const DWORD         gc_dwSTATUS_TEXT_FG_COLOR =                     gc_fMENU_ITEM_ENABLED_FG_COLOR;
const DWORD         gc_dwSTATUS_TEXT_BG_COLOR =                     gc_fMENU_ITEM_ENABLED_BG_COLOR;

// Default Arrow Positions
const int           gc_iUPARROWXPOS =                               575;
const int           gc_iUPARROWYPOS =                               105;
const int           gc_iDOWNARROWXPOS =                             gc_iUPARROWXPOS;
const int           gc_iDOWNARROWYPOS =                             330;

class CMenuScreen : public CXItem
{
public:
    // Constructors and Destructors
    CMenuScreen( void );
    virtual ~CMenuScreen( void );

    // Public Methods
    virtual void Action( CUDTexture* pTexture );                // Renders to the texture and calls any per-frame processing
    
    virtual void HandleInput( enum BUTTONS buttonPressed,       // Handles input from the buttons
                              BOOL bFirstPress );
    
    virtual void HandleInput( enum CONTROLS controlPressed,     // Handles input from the controls
                              BOOL bFirstPress );
    
    virtual void HandleInput( enum JOYSTICK joystick,           // Handles input from the joysticks
                              int nThumbY,
                              int nThumbX,
                              BOOL bFirstYPress,
                              BOOL bFirstXPress );
    virtual void Enter( void );                                 // This should be called whenever the menu is entered or activated

    virtual void         ClearMenuItems( void );                                        // Clear all menu items from the list
    virtual void         ClearScreenItems( void ) { m_ScreenItems.EmptyList(); };       // Clear all screen items from the list
    virtual HRESULT      Init( XFONT* pMenuItemFont,                                    // Initialize the Menu
                               XFONT* pMenuTitleFont );
    virtual void         SetBackPtr( CXItem* pBackPtr ) { m_BackPtr = pBackPtr; };      // Set the back-pointer to specficy where navigating back should send the user
    virtual void         GoBack( void );                                                // Go back to the previous menu
    virtual unsigned int GetNumMenuItems( void )                                        // Return the number of menu items on our current menu
        { return m_MenuItems.GetNumItems(); };
    virtual unsigned int GetNumScreenItems( void )                                      // Return the number of Screen items on our current menu
        { return m_ScreenItems.GetNumItems(); };
    virtual unsigned int GetSelectedItem( void ) const                                  // Get the index of the currently selected item on the screen
        { return m_uiSelectedItem; };
    virtual int GetSelectedItemValue( void )                                            // Get the value of the currently selected item on the screen
        { return m_MenuItems[GetSelectedItem()]->m_nItemValue; };
    virtual HRESULT      SetSelectedItem( unsigned int uiItemNumber );                  // Set the currently selected item on the screen
    virtual WCHAR*       GetMenuTitle( void ) { return m_pwszMenuTitle; };              // Return a pointer to the menu title
    virtual HRESULT      SetMenuTitle( XFONT* pMenuTitleFont,                           // Return a pointer to the menu title
                                       WCHAR* pwszMenuTitle );
    virtual unsigned int GetSelectorPosition( void );                                   // Get the position of the selector on the menu
    virtual void         SetSingleListMenu( BOOL bMenuType );                           // Allows user to set a single, or multi list menu
    virtual BOOL         IsSingleListMenu( void ) { return m_bSingleList; };            // Used to determine if we are using a single or multi list menu

    virtual void         UpdateTexture( void ) { m_bUpdateTexture = TRUE; };            // Call this when the texture should be updated

    
    // Set the boundaries of the menu area (where the menu items will live)
    virtual void         SetMenuAreaBoundaries( float fX1 = gc_fLEFT_MENU_AREA_BOUNDARY,
                                                float fY1 = gc_fTOP_MENU_AREA_BOUNDARY,
                                                float fX2 = gc_fRIGHT_MENU_AREA_BOUNDARY,
                                                float fY2 = gc_fBOTTOM_MENU_AREA_BOUNDARY );

    // Set the borders of the menu area (where the menu items will live)
    virtual void         SetMenuAreaBorders( float fLeftBorder   = gc_fLEFT_MENU_AREA_BORDER,
                                             float fTopBorder    = gc_fTOP_MENU_AREA_BORDER,
                                             float fRightBorder  = gc_fRIGHT_MENU_AREA_BORDER,
                                             float fBottomBorder = gc_fBOTTOM_MENU_AREA_BORDER );

    // Set the vertical spacing between the menu items
    virtual void        SetMenuItemVertSpace( float fVertSpace = gc_fMENU_ITEM_VERT_SPACE,
                                              float fHorizSpace = gc_fMENU_ITEM_HORIZ_SPACE );

    // Set the font height of the menu items
    virtual void        SetMenuItemFontProperties( unsigned int uiFontHeight = gc_uiDEFAULT_FONT_HEIGHT,
                                                   unsigned int uiFontAliasLevel = gc_uiDEFAULT_FONT_ALIAS_LEVEL );
    // Set the menu item selector properties
    virtual void        SetMenuItemSelectorProperties( float fBorder = gc_fMENU_SELECTOR_BORDER,
                                                       DWORD dwColor = gc_dwMENU_SELECTOR_COLOR );

    // Set the amount of time a button should delay before repeating
    virtual void        SetDelayIntervals( DWORD dwInitialDelay = gc_dwMENU_BUTTONPRESS_INITIAL_DELAY,
                                           DWORD dwRepeatDelay = gc_dwMENU_BUTTONPRESS_REPEAT_DELAY );

    // Set the colors of the menu items
    virtual void        SetMenuItemColors( DWORD dwEnabledItemFGColor = gc_fMENU_ITEM_ENABLED_FG_COLOR,
                                           DWORD dwEnabledItemBGColor = gc_fMENU_ITEM_ENABLED_BG_COLOR,
                                           DWORD dwDisabledItemFGColor = gc_fMENU_ITEM_DISABLED_FG_COLOR,
                                           DWORD dwDisabledItemBGColor = gc_fMENU_ITEM_DISABLED_BG_COLOR,
                                           DWORD dwHighlightedItemFGColor = gc_fMENU_ITEM_HIGHLIGHTED_FG_COLOR,
                                           DWORD dwHighlightedItemBGColor = gc_fMENU_ITEM_HIGHLIGHTED_BG_COLOR );

    // Set the colors of the menu items
    virtual void        SetMenuItemSelectedColors( DWORD dwSelectedItemFGColor = gc_fMENU_ITEM_SELECTED_FG_COLOR,
                                                   DWORD dwSelectedItemBGColor = gc_fMENU_ITEM_SELECTED_BG_COLOR,
                                                   DWORD dwSelectedHLItemFGColor = gc_fMENU_ITEM_SELECTED_HL_FG_COLOR,
                                                   DWORD dwSelectedHLItemBGColor = gc_fMENU_ITEM_SELECTED_HL_BG_COLOR );

    // Set the positions of the arrows on our menu
    virtual void        SetArrowPositions( int iUpArrowXPos = gc_iUPARROWXPOS,
                                           int iUpArrowYPos = gc_iUPARROWYPOS,
                                           int iDownArrowXPos = gc_iDOWNARROWXPOS,
                                           int iDownArrowYPos = gc_iDOWNARROWYPOS );


    // Deal with a scrolling menu
    virtual unsigned int GetNumMenuItemsToDisplay( void );                  // Returns the number of items that should appear on the menu
    virtual unsigned int GetTopItemIndex( void )                            // Get the index of the top menu item that should be displayed
        { return m_uiTopItemIndex; };
    virtual unsigned int GetBottomItemIndex( void );                        // Get the index of the bottom menu item that should be displayed
    virtual HRESULT      SetTopItemIndex( unsigned int uiIndex );           // Set the index of the top menu item that should be displayed

protected:
    CLinkedList< CScreenItem* > m_ScreenItems;                  // List of pointers to screen items that will be rendered
    CLinkedList< CMenuItem* >   m_MenuItems;                    // List of pointers to our menu items that will be rendered
    WCHAR*                      m_pwszMenuTitle;                // Title of the current menu
    XFONT*                      m_pMenuTitleFont;               // Font used to render the menu title
    unsigned int                m_uiSelectedItem;               // The currently selected item in the list (-1 if nothing is selected)
    unsigned int                m_uiTopItemIndex;               // Index of the top item that is to be displayed
    BOOL                        m_bUpdateTexture;               // Used to determine when the texture should be updated
    BOOL                        m_bSingleList;                  // Used to determine if our menu is a single, or double list menu
    CXItem*                     m_BackPtr;                      // Used to determine where the user should go if they press "Back"
    CBitmapAlphaItem            m_UpArrow;                      // The up arrow displayed when more info is off the top of the screen
    CBitmapAlphaItem            m_DownArrow;                    // The up down displayed when more info is off the bottom of the screen
	/*
    CBitmapItem					m_UpArrow;                      // The up arrow displayed when more info is off the top of the screen
    CBitmapItem					m_DownArrow;                    // The up down displayed when more info is off the bottom of the screen
	*/

    // Menu Repeat-Rate Timers
    DWORD                       m_dwKeyPressDelayTimer;         // Used to track how much time has passed
    DWORD                       m_dwKeyPressDelayInterval;      // Used to track our current interval
    DWORD                       m_dwKeyPressInitialDelay;       // Amount of time to delay after an initial button press  
    DWORD                       m_dwKeyPressRepeatDelay;        // Amount of time to delay after a button is held down

    // Menu area Boundaries
    float                       m_fLeftMenuAreaBoundary;        // Left boundary of where the menu items will appear
    float                       m_fTopMenuAreaBoundary;         // Top boundary of where the menu items will appear
    float                       m_fRightMenuAreaBoundary;       // Right boundary of where the menu items will appear
    float                       m_fBottomMenuAreaBoundary;      // Bottom boundary of where the menu items will appear

    // Menu area Borders
    float                       m_fLeftMenuAreaBorder;          // Left border of where the menu items will appear
    float                       m_fTopMenuAreaBorder;           // Top border of where the menu items will appear
    float                       m_fRightMenuAreaBorder;         // Right border of where the menu items will appear
    float                       m_fBottomMenuAreaBorder;        // Bottom border of where the menu items will appear

    float                       m_fMenuItemVertSpace;           // Set the vertial space between each menu item
    float                       m_fMenuItemHorizSpace;          // Set the vertial space between each menu item

    // Arrow positions
    int                         m_iUpArrowXPos;                 // X Position of our up arrow
    int                         m_iUpArrowYPos;                 // Y Position of our up arrow
    int                         m_iDownArrowXPos;               // X Position of our down arrow
    int                         m_iDownArrowYPos;               // Y Position of our down arrow

    // Menu Item Font info
    unsigned int                m_uiMenuItemFontHeight;         // Font height of a menu item
    unsigned int                m_uiMenuItemFontAliasLevel;     // Alias level of a menu item

    // Menu Selector border
    float                       m_fMenuItemSelectorBorder;      // Border surrounding the selected item
    DWORD                       m_dwMenuItemSelectorColor;      // Color of the selector bar

    // Menu Item Colors
    DWORD                       m_dwMenuItemEnabledFGColor;     // Foreground color for an enabled menu item
    DWORD                       m_dwMenuItemEnabledBGColor;     // Background color for an enabled menu item
    DWORD                       m_dwMenuItemDisabledFGColor;    // Foreground color for a disabled menu item
    DWORD                       m_dwMenuItemDisabledBGColor;    // Background color for a disabled menu item
    DWORD                       m_dwMenuItemHighlightedFGColor; // Foreground color for a highlighted menu item
    DWORD                       m_dwMenuItemHighlightedBGColor; // Background color for a highlighted menu item

    // Selected Item Colors
    DWORD                       m_dwSelectedItemFGColor;        // Selected Item foreground color
    DWORD                       m_dwSelectedItemBGColor;        // Selected Item background color
    DWORD                       m_dwSelectedItemHLFGColor;      // Selected/Highlighted Item foreground color
    DWORD                       m_dwSelectedItemHLBGColor;      // Selected/Highlighted Item background color

    // Methods
    virtual void HandleControlInputSingleList( enum CONTROLS controlPressed,    // Handle control input for a single list menu
                                               BOOL bFirstPress );

    virtual void HandleControlInputMultiList( enum CONTROLS controlPressed,     // Handle control input for a double list menu
                                              BOOL bFirstPress );

    virtual BOOL HandleInputDelay( BOOL bFirstPress );          // Handle the default delay before for Input - TRUE if button should be processed

    virtual void RenderMenuItems( CUDTexture* pTexture );       // Render the menu items properly on our menu
    virtual void RenderSelector( CUDTexture* pTexture );        // Render the selector for our menu
    virtual void RenderScreenItems( CUDTexture* pTexture );     // Render the screen items properly on our menu
    virtual void RenderMenuTitle( CUDTexture* pTexture );       // Render the menu title on to the menu
    virtual void RenderDisplayArrows( CUDTexture* pTexture );   // Render the display arrows which will indicate more info off screen


    virtual void Leave( CXItem* pItem );                // Should be called whenever user is leaving the menu

    virtual HRESULT UpdateMenuItem( unsigned int uiIndex,   // Update an existing menu item
                                    WCHAR* pwszItemName,
                                    BOOL bHighlighted,
                                    BOOL bEnabled,
                                    WCHAR* pwszItemValue=NULL );

    virtual HRESULT AddMenuItem( XFONT* pFont,          // Used to add Menu Items to the current Menu
                                 WCHAR* pwszItemName,
                                 BOOL bHighlighted,
                                 BOOL bEnabled,
                                 int nItemValue,
                                 XFONT* pValueFont=NULL,
                                 WCHAR* pwszItemValue=NULL );

    virtual HRESULT AddTextItem( XFONT* pFont,          // Add a Text Item to our Screen object list
                                 WCHAR* pwszText,
                                 float fXPos,
                                 float fYPos,
                                 DWORD dwFGColor,
                                 DWORD dwBGColor );

    virtual HRESULT AddPanelItem( float fX1Pos,         // Add a PanelItem to our Screen object list
                                  float fY1Pos,
                                  float fX2Pos,
                                  float fY2Pos,
                                  DWORD dwColor );

    virtual HRESULT AddLineItem( float fX1Pos,          // Add a Line Item to our Screen object list
                                 float fY1Pos,
                                 float fX2Pos,
                                 float fY2Pos,
                                 float fWidth,
                                 DWORD dwColor );

    virtual HRESULT AddOutlineItem( float fX1Pos,       // Add a Outline Item to our Screen object list
                                    float fY1Pos,
                                    float fX2Pos,
                                    float fY2Pos,
                                    float fWidth,
                                    DWORD dwColor );

    virtual HRESULT AddBitmapItem( char* pszFilename,   // Add a Bitmap Item to our Screen object list
                                   int nXPos,
                                   int nYPos );

    virtual HRESULT AddBitmapAlphaItem( char* pszFilename,   // Add a Bitmap Item (with Alpha) to our Screen object list
                                        char* pszAlphaFilename,
                                        int nXPos,
                                        int nYPos );
};

#endif // _MENUSCREEN_H_
