/*****************************************************
*** networkmenu.h
***
*** Header file for our XShell Network menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** May 19th, 2001
***
*****************************************************/

#ifndef _NETWORKMENU_H_
#define _NETWORKMENU_H_

/*
enum SettingsValues
{
    ENUM_DISPLAYSETTING = 0,
    ENUM_IPADDRESS,
    ENUM_SUBNETMASK,
    ENUM_GATEWAY,
    ENUM_MACHINENAME,

    ENUM_SETTINGS_MAX
};
*/

#include "menuscreen.h"

// Sub-Menus
#include "ipmenu.h"
#include "dnsmenu.h"
#include "subnetmenu.h"
#include "gatewaymenu.h"
#include "machinenamemenu.h"

class CNetworkMenu : public CMenuScreen
{
public:
    CNetworkMenu( CXItem* pParent );
    ~CNetworkMenu();

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
    CIPMenu          m_IPMenu;              // IP Menu Object to get input from the user when entering an IP Address
    CSubnetMenu      m_SubnetMenu;          // Subnet Menu Object to get input from the user when entering a Subnet Mask
	CDNSMenu	     m_DNSMenu;				// DNS Menu Object to set the Primary and Secondary DNS Addresses on the Xbox
    CGatewayMenu     m_GatewayMenu;         // Gateway Object to get input from the user when entering a Gateway
    CMachineNameMenu m_MachineNameMenu;     // MachineName Object to get input from the user when entering a Machine Name
};

#endif // _NETWORKMENU_H_
