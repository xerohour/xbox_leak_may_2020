/*****************************************************
*** eepromsettingsmenu.h
***
*** Header file for our XShell EEPROM settings menu 
*** class.  This menu will display all of the
*** EEPROM settings and allow the user to scroll
*** through this list.
***
*** by James N. Helm
*** June 6th, 2001
***
*****************************************************/

#ifndef _EEPROMSETTINGSMENU_H_
#define _EEPROMSETTINGSMENU_H_

#include "menuscreen.h"

class CEEPROMSettingsMenu : public CMenuScreen
{
public:
    CEEPROMSettingsMenu( CXItem* pParent );
    ~CEEPROMSettingsMenu();

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
    unsigned int    m_uiTopItemIndex;                                   // Index of the top item being displayed
    WCHAR*          m_pawszMenuItems[EEPROMMENU_NUM_ITEMS];             // Used to hold our displayed menu items

    // Private Methods
    void AdjustDisplayArrows();         // Adjust the UP and DOWN arrows on the screen
};

#endif // _EEPROMSETTINGSMENU_H_
