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

#include "screenitems.h"

class CMenuScreen : public CXItem
{
public:
    // Constructors and Destructors
    CMenuScreen( CXItem* pParent );
    virtual ~CMenuScreen();

    // Public Methods
    virtual void Action( CUDTexture* pTexture );
    virtual void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    virtual void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress );
    virtual void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );

    virtual HRESULT      Init( char* menuFileName );                                    // Initialize the Menu
    virtual void         Enter();                                                       // This should be called whenever the menu is entered or activated
    virtual unsigned int GetNumItems() { return m_uiNumItems; };                        // Return the number of menu items on our current screen
    virtual int          GetSelectedItem() const { return m_nSelectedItem; };           // Get the indext of the currently selected item on the screen
    virtual unsigned int GetFontHeightOfItem() const { return m_uiFontHeightOfItem; };  // The font height of a screen item
    virtual HRESULT      SetSelectedItem( int itemNumber );                             // Set the currently selected item on the screen
    virtual void         UpdateTexture() { m_bUpdateTexture = TRUE; };                  // Call this when the texture should be updated
                                                                                        // TODO: Replace everybody's update with this function call

protected:
    CLinkedList< CScreenItem* >       m_ScreenItems;        // List of pointers to screen items that will be rendered

    WCHAR*            m_pwszMenuTitle;                      // Title of the current menu
    int               m_nSelectedItem;                      // The currently selected item in the list
    unsigned int      m_uiFontHeightOfItem;                 // The font height of an item
    unsigned int      m_uiNumItems;                         // The number of items on the menu
    DWORD             m_keyPressDelayTimer;                 // Used to temper the speed of holding down the d-pad
    DWORD             m_keyPressDelayInterval;              // The current interval to delay before processing a keypress
    int               m_nJoystickDeadZone;                  // The dead zone for the joysticks

    // Methods
    virtual void    Leave( CXItem* pItem );                     // Should be called whenever a user deactivates this item
    virtual BOOL HandleInputDelay( BOOL bFirstPress );          // Handle the default delay before for Input - TRUE if button should be processed
    virtual HRESULT ImportScreenItems( char* menuFileName );    // Add item info to the current screen
    virtual HRESULT AddTextItemFromFile( FILE* menuFile );      // Add a Text Item to our Screen from a file
    virtual HRESULT AddPanelItemFromFile( FILE* menuFile );     // Add a Panel Item to our Screen from a file
    virtual HRESULT AddHeaderItemFromFile( FILE* menuFile );    // Add a Header Item to our Screen from a file
    virtual HRESULT AddLineItemFromFile( FILE* menuFile );      // Add a Line Item to our Screen from a file
    virtual HRESULT AddOutlineItemFromFile( FILE* menuFile );   // Add a Outline Item to our Screen from a file
    virtual HRESULT AddBitmapItemFromFile( FILE* menuFile );    // Add a Bitmap Item to our Screen from a file
};

class Key
{
public:
	int xorigin, yorigin;		// x,y coordinate of the key's upper left pixel
	int width, height;			// width and height of the key
	DWORD selectColor;			// color to hilight the key with upon selection
	WCHAR* resultChar;			// value attached to the key
	BOOL render;				// should the key be rendered?

	// Constructors and Destructors
	Key();
	~Key();

	void define(int x, int y, int w, int h, DWORD color);
	void defineText(WCHAR* result);
	void setRender(BOOL value);
	BOOL getRender(void);
};

#endif // _MENUSCREEN_H_