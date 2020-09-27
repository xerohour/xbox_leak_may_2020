/*****************************************************
*** menuscreen.h
***
*** Header file for our XDash CFG Generic menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
*** 
*** by James N. Helm
*** November 2nd, 2000
*** 
*****************************************************/

#ifndef _MENUSCREEN_H_
#define _MENUSCREEN_H_

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

    // Must be overridden for this to be a Item on our XDashConfig program
    virtual void Action( CXBoxVideo* Screen );
    virtual void HandleInput( enum BUTTONS buttonPressed, bool bFirstPress );

    // Public Methods
    virtual void AddItem( CXDCItem* pScreen );                                  // Add an item to the current screen
    virtual WCHAR* GetItemDescription( unsigned int itemIndex ) const;          // Get the description of an item on our screen
    virtual WCHAR* GetItemTitle( unsigned int itemIndex ) const;                // Get the title of an item on our screen
    virtual unsigned int GetNumItems() const { return m_uicNumItems; };         // Return the number of menu items on our current screen
    virtual unsigned int GetSelectedItem() const { return m_uiSelectedItem; };  // Get the indext of the currently selected item on the screen
    virtual HRESULT SetSelectedItem( unsigned int itemNumber );                 // Set the currently selected item on the screen

private:
    CLinkedList< CXDCItem* > m_Items;   // Items that live underneath this menu
    unsigned int m_uicNumItems;         // Num of items that are on the current screen
    int m_uiSelectedItem;               // The currently selected item on the screen


};

#endif // _MENUSCREEN_H_