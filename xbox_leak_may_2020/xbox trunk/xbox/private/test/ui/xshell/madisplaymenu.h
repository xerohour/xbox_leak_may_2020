/*****************************************************
*** madisplaymenu.h
***
*** Header file for our XShell Memory Area Display 
*** menu class.  This class will list out all Memory
*** devices on the system, and display their names
*** in the proper format, with the amount of space
*** free on each device
***
*** by James N. Helm
*** June 7th, 2001
***
*****************************************************/

#ifndef _MADISPLAYMENU_H_
#define _MADISPLAYMENU_H_

#include "menuscreen.h"  // Base menu type
#include "xonlinefuncs.h"

enum MemAreaIndexMappings
{
    ENUM_MADISPLAY_MU1A,
    ENUM_MADISPLAY_MU1B,
    ENUM_MADISPLAY_MU2A,
    ENUM_MADISPLAY_MU2B,
    ENUM_MADISPLAY_MU3A,
    ENUM_MADISPLAY_MU3B,
    ENUM_MADISPLAY_MU4A,
    ENUM_MADISPLAY_MU4B,
    ENUM_MADISPLAY_HARD_DRIVE
};

class CMADisplayMenu : public CMenuScreen
{
public:
    CMADisplayMenu( CXItem* pParent );
    ~CMADisplayMenu();

    // Must be overridden for this to be a Item on our XShell program
    void Enter();   // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );
    BOOL GetCancelled() { return m_bCancelled; };           // Used to determine if the user cancelled the menu
    void SetInfo( BOOL bDisplaySize,                        // Sets the menu to allow certain actions
                  BOOL bAllowSelect );
    unsigned int GetSelectedMemArea();                      // Returns the Memory Area that the user selected

    // Public Methods
    HRESULT Init( char* menuFileName );     // Initialize our font and settings

private:
    // Private Properties
    BOOL m_bCancelled;                                              // Used to determine if the user cancelled the MA Display Menu
    BOOL m_bDisplaySize;                                            // Used to determine if the menu should display sizes, or num users
    BOOL m_bAllowSelect;                                            // Used to determine if the user should be allowed to select a menu or not
    XONLINE_USER  m_aOnlineUsers[XONLINE_MAX_STORED_ONLINE_USERS];  // Online users that will live on Memory Areas

    // Private Methods
    void Leave( CXItem* pItem );            // This should be called whenever a user leaves this menu
};

#define MEMORY_AREA_HIDDEN_BLOCK_BOUNDS 50000

#endif // _MADISPLAYMENU_H_
