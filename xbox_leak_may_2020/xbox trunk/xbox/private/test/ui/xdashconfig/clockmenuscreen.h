/*****************************************************
*** clockmenuscreen.h
***
*** Header file for our XDash CFG Clock menu class.
*** This clock menu contains a list of time and 
*** date objects that will be manipulated
*** 
*** by James N. Helm
*** November 2nd, 2000
*** 
*****************************************************/

#ifndef _CLOCKMENUSCREEN_H_
#define _CLOCKMENUSCREEN_H_

#include "xdcitem.h"
#include "xboxvideo.h"
#include "usbmanager.h"

enum CLOCKITEMS
{
    MONTH,
    DAY,
    YEAR,
    HOUR,
    MINUTE,
    SECOND
};

// Months of the year
static const WCHAR* g_apszMonths[12] = {
    L"January",
    L"February",
    L"March",
    L"April",
    L"May",
    L"June",
    L"July",
    L"August",
    L"September",
    L"October",
    L"November",
    L"December"
};

#define NUM_CLOCK_ITEMS     6

class CClockMenuScreen : public CXDCItem
{
public:
    CClockMenuScreen();
    CClockMenuScreen( CXDCItem* pParent, WCHAR* pwszTitle, WCHAR* pwszDescription );
    ~CClockMenuScreen();

    void Action( CXBoxVideo* Screen );
    void HandleInput( enum BUTTONS buttonPressed, bool bFirstPress );

    HRESULT SetSelectedItem( unsigned int itemNumber );                 // Set the currently selected item
    WCHAR* GetItemDescription( unsigned int itemIndex ) const;          // Get the description of an item
    unsigned int GetNumItems() const { return m_uicNumItems; };         // Return the number of menu items on our current screen
    unsigned int GetSelectedItem() const { return m_uiSelectedItem; };  // Get the indext of the currently selected item on the screen


private:
    SYSTEMTIME m_sysUserTime;                   // Holds the current / user selected time
    bool m_fUserAdjustedTime;                   // Used to determine if the user has selected a new time
    WCHAR m_Items[NUM_CLOCK_ITEMS][20];         // Descriptions of each of the clock items
    unsigned int m_uicNumItems;                 // Num of items that are on the current screen
    int m_uiSelectedItem;                       // The currently selected item on the screen

};

#endif // _CLOCKMENUSCREEN_H_