/*****************************************************
*** xbconfigmenu.h
***
*** Header file for our Xbox Config menu
*** class. This menu class will allow you to change
*** various options on the Xbox
***
*** by James N. Helm
*** May 10th, 2001
***
*****************************************************/

#ifndef _XBCONFIGMENU_H_
#define _XBCONFIGMENU_H_

#include "menuscreen.h"     // Base menu type

// Sub-Menus
#include "gameregionmenu.h" // Set Xbox Game Region
#include "dvdregionmenu.h"  // Set Xbox DVD Region
#include "avregionmenu.h"   // Set Xbox AV Region

class CXBConfigMenu : public CMenuScreen
{
public:
    CXBConfigMenu( CXItem* pParent );
    ~CXBConfigMenu();

    // Must be overridden for this to be a Item on our XShell program
    // void Enter();   // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );

    // Public Methods
    HRESULT Init( IDirect3DDevice8* pD3DDevice, char* menuFileName );       // Initialize our font and settings

private:
    CGameRegionMenu m_MenuGameRegion;                                       // Allow the user to set the Xbox Game Region
    CDVDRegionMenu  m_MenuDVDRegion;                                        // Allow the user to set the Xbox DVD Region
    CAVRegionMenu   m_MenuAVRegion;                                         // Allow the user to set the Xbox AV Region
    BOOL            m_bXboxReset;                                           // Used to determine if the user has reset the Xbox
    BOOL            m_bCDriveCorrupt;                                       // Used to determine if the user has corrupted the C Drive
    BOOL            m_bYDriveCorrupt;                                       // Used to determine if the user has corrupted the Y Drive
    BOOL            m_bTimeZoneReset;                                       // Used to determine if the user has Reset the TimeZone
    BOOL            m_bLanguageReset;                                       // Used to determine if the user has Reset the Language
    BOOL            m_bClockReset;                                          // Used to determine if the user has Reset the Clock

    unsigned int    m_uiTopItemIndex;                                       // Index of the top item being displayed
    unsigned int    m_uiNumItemsToDisplay;                                  // Used to hold the number of items to display on our menu

    // Private Methods
    // void Leave( CXItem* pItem );                                        // This should be called whenever a user leaves this menu
};

#define MUOPTIONS_MENU_NUM_ITEMS_TO_DISPLAY    9

#endif // _XBCONFIGMENU_H_
