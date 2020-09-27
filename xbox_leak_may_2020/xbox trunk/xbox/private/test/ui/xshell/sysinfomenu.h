/*****************************************************
*** sysinfomenu.h
***
*** Header file for our XShell System Information menu 
*** class.  This menu will display system information
*** and allow the user to scroll through this list.
***
*** by James N. Helm
*** August 7th, 2001
***
*****************************************************/

#ifndef _SYSINFOMENU_H_
#define _SYSINFOMENU_H_

#include "menuscreen.h"
#include "ntddscsi.h"

class CSysInfoMenu : public CMenuScreen
{
public:
    CSysInfoMenu( CXItem* pParent );
    ~CSysInfoMenu();

    // Must be overridden for this to be a Item on our XShell program
    void Enter();   // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );

    // Public Methods
    HRESULT Init( char* menuFileName );     // Initialize our font and settings
    void    Leave( CXItem* pItem );         // Should be called whenever a user deactivates this item
private:
    // Private Properties
    unsigned int    m_uiTopItemIndex;                           // Index of the top item being displayed
    WCHAR*          m_pawszMenuItems[SYSINFOMENU_NUM_ITEMS];    // Used to hold our displayed menu items
    BOOL            m_bDropKeyPress;                            // Used to prevent the 'instant' keypress when a user navigates in here

    DWORD           m_dwTickCount;                              // Used to update our texture every second

    // Private Methods
    void AdjustDisplayArrows();         // Adjust the UP and DOWN arrows on the screen
};

#endif // _SYSINFOMENU_H_
