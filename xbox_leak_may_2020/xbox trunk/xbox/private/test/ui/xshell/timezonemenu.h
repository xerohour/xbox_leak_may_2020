/*****************************************************
*** timezonemenu.h
***
*** Header file for our XShell Time Zone menu 
*** class.  This is a standard menu contains a list
*** of menu items to be selected.
***
*** by James N. Helm
*** June 26th, 2001
***
*****************************************************/

#ifndef _TIMEZONEMENU_H_
#define _TIMEZONEMENU_H_

#include "menuscreen.h"

class CTimeZoneMenu : public CMenuScreen
{
public:
    CTimeZoneMenu( CXItem* pParent );
    ~CTimeZoneMenu();

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
    unsigned int    m_uiTopItemIndex;   // Index of the top item being displayed

    // Private Methods
    void AdjustDisplayArrows();                     // Adjust the UP and DOWN arrows on the screen
};

#define TIMEZONEMENU_FONT_HEIGHT            17
#define TIMEZONEMENU_NUM_ITEMS_TO_DISPLAY   8
#define TIMEZONEMENU_FILENAME_A             FILE_DATA_MENU_DIRECTORY_A "\\timezone.mnu"

#endif // _TIMEZONEMENU_H_
