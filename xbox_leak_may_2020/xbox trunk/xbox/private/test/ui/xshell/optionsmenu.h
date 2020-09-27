/*****************************************************
*** optionsmenu.h
***
*** Header file for our XShell Options menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** May 19th, 2001
***
*****************************************************/

#ifndef _OPTIONSMENU_H_
#define _OPTIONSMENU_H_

#include "menuscreen.h"

// Sub-Menus
#include "gameregionmenu.h"
#include "certmenu.h"
#include "avregionmenu.h"
#include "videoresmenu.h"
#include "memorymenu.h"
#include "networkmenu.h"

class COptionsMenu : public CMenuScreen
{
public:
    COptionsMenu( CXItem* pParent );
    ~COptionsMenu();

    // Must be overridden for this to be a Item on our XShell program
    void Enter();   // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );

    // Public Methods
    HRESULT Init( char* menuFileName );                 // Initialize our font and settings
    CXItem* GetItemPtr( enum XShellMenuIds menuID );    // Returns a pointer to a sub-menu

private:
    CGameRegionMenu m_GameRegionMenu;           // Allows user to select the Xbox Game Region
    CAVRegionMenu   m_AVRegionMenu;             // Allows user to select the Xbox AV Region
    CVideoResMenu   m_VideoResMenu;             // Allows the user to set the XDK Launcher Video Resolution
    CMemoryMenu     m_MemoryMenu;               // Memory Unit options menu
    CNetworkMenu    m_NetworkMenu;              // Allows user to configure Xbox Network Settings
    CCertMenu       m_CertMenu;                 // Allows the user to execute the certification tools
};

#endif // _OPTIONSMENU_H_
