/*****************************************************
*** maformatcommand.h
***
*** H file for our XDash CFG Memory Area Format
*** command option.
*** This option will allow the user to format (clean)
*** a Memory Area of all of it's data when a user
*** selects "A"
*** 
*** by James N. Helm
*** November 3rd, 2000
*** 
*****************************************************/

#ifndef _MAFORMATCOMMAND_H_
#define _MAFORMATCOMMAND_H_

#include "xdcitem.h"
#include "xboxvideo.h"
#include "usbmanager.h"

#define NUM_MEMORY_AREAS    9

enum MEMAREAS
{
    XDISK,
    MU1,
    MU2,
    MU3,
    MU4,
    MU5,
    MU6,
    MU7,
    MU8
};

// Memory Areas
static const WCHAR* g_apszMemoryAreas[NUM_MEMORY_AREAS] = {
    L"XDisk",
    L"Memory Unit 1",
    L"Memory Unit 2",
    L"Memory Unit 3",
    L"Memory Unit 4",
    L"Memory Unit 5",
    L"Memory Unit 6",
    L"Memory Unit 7",
    L"Memory Unit 8"
};

class CMAFormatCommand : public CXDCItem
{
public:
    CMAFormatCommand();
    CMAFormatCommand( CXDCItem* pParent, WCHAR* pwszTitle, WCHAR* pwszDescription );
    ~CMAFormatCommand();

    void Action( CXBoxVideo* Screen );
    void HandleInput( enum BUTTONS buttonPressed, bool bFirstPress );

    HRESULT SetSelectedItem( unsigned int itemNumber );                 // Set the currently selected item
    WCHAR* GetItemDescription( unsigned int itemIndex ) const;          // Get the description of an item
    unsigned int GetNumItems() const { return m_uicNumItems; };         // Return the number of menu items on our current screen
    unsigned int GetSelectedItem() const { return m_uiSelectedItem; };  // Get the indext of the currently selected item on the screen


private:
    WCHAR m_Items[NUM_MEMORY_AREAS][40];        // Descriptions of each of the Memory Areas
    unsigned int m_uicNumItems;                 // Num of items that are on the current screen
    int m_uiSelectedItem;                       // The currently selected item on the screen
};

#endif // _MAFORMATCOMMAND_H_