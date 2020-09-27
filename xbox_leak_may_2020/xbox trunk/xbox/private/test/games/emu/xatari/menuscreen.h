/*****************************************************
*** menuscreen.h
***
*** Header file for our Generic menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** November 17th, 2000
***
*****************************************************/

#ifndef _MENUSCREEN_H_
#define _MENUSCREEN_H_

#include <time.h>
#include "xdcitem.h"
#include "xboxvideo.h"
#include "usbmanager.h"

class CMenuScreen : public CXDCItem
{
public:
    // Constructors and Destructors
    CMenuScreen();
    CMenuScreen( CXDCItem* pParent, WCHAR* pwszTitle, WCHAR* pwszDescription );
    ~CMenuScreen();

    // Must be overridden for this to be a Item on our program
    virtual void Action( CXBoxVideo* Screen );
    virtual void HandleInput( enum BUTTONS buttonPressed, bool bFirstPress );
	virtual void HandleInput( enum CONTROLS controlPressed, bool bFirstPress );

    // Public Methods
    virtual void         AddItem( WCHAR* pItem );								// Add an item to the current screen
    virtual WCHAR*       GetItemDescription( unsigned int itemIndex ) const;    // Get the description of an item on our screen
	virtual WCHAR*       GetItemName ( unsigned int itemIndex ) const;          // Get the name of the item based on the passed in index
    virtual unsigned int GetNumItems() const { return m_uicNumItems; };         // Return the number of menu items on our current screen
	virtual unsigned int GetPage() const { return m_uiPage; };					// The currently displayed page on our menu
	virtual unsigned int GetPageStart() const;									// The starting index of the item on the current page
	virtual unsigned int GetPageEnd() const;									// The ending index of the item on the current page
	virtual unsigned int GetColNumber( unsigned int index ) const;				// Get the column number of the specified item
	virtual unsigned int GetRowNumber( unsigned int index ) const;				// Get the row number of the specified item
    virtual unsigned int GetSelectedItem() const { return m_uiSelectedItem; };	// Get the indext of the currently selected item on the screen
    virtual HRESULT      SetSelectedItem( unsigned int itemNumber );            // Set the currently selected item on the screen

private:
    CLinkedList< WCHAR* > m_Items;				// Items that live underneath this menu
    unsigned int          m_uicNumItems;		// Num of items that are on the current screen
    int                   m_uiSelectedItem;		// The currently selected item on the screen
	unsigned int          m_uiPage;             // Current Page being displayed (in case there are a LOT of ROMs)
};

#endif // _MENUSCREEN_H_