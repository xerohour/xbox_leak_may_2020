/*****************************************************
*** settingsmenu.h
***
*** Header file for our XShell Settings menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** December 2nd, 2000
***
*****************************************************/

#ifndef _SETTINGSMENU_H_
#define _SETTINGSMENU_H_

enum SettingsValues
{
    ENUM_DISPLAYSETTING = 0,
    ENUM_IPADDRESS,
    ENUM_SUBNETMASK,
    ENUM_GATEWAY,
    ENUM_MACHINENAME
};

#define NUM_MAX_ENUM    ENUM_MACHINENAME   // This should be the highest enum from the list above (SettingsValues)

#include "ipmenu.h"
#include "subnetmenu.h"
#include "gatewaymenu.h"
#include "machinenamemenu.h"
#include "dispsetmenu.h"

class CSettingsMenu : public CMenuScreen
{
public:
    CSettingsMenu();
    CSettingsMenu( CXItem* pParent );
    ~CSettingsMenu();

    // Must be overridden for this to be a Item on our XShell program
    void Action( CXBoxVideo* Screen );
    void HandleInput( enum BUTTONS buttonPressed, bool bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, bool bFirstPress );
    void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, bool bFirstYPress, bool bFirstXPress );

    // Public Methods
    char* GetMachineName() { return m_XBoxSettings.GetSetting( XSETTINGS_MACHINE_NAME ); }; // Get the machine name from the settings object
    HRESULT Init( CXBoxVideo* pScreen, char* menuFileName );                                // Initialize our font and settings

private:
    CXSettings m_XBoxSettings;                      // Settings that are stored on the XBox
    CIPMenu m_IPMenu;                               // IP Menu Object to get input from the user when entering an IP Address
    CSubnetMenu m_SubnetMenu;                       // Subnet Menu Object to get input from the user when entering a Subnet Mask
    CGatewayMenu m_GatewayMenu;                     // Gateway Object to get input from the user when entering a Gateway
    CMachineNameMenu m_MachineNameMenu;             // MachineName Object to get input from the user when entering a Machine Name
    CDisplaySettingsMenu m_DisplaySettingsMenu;     // Display Settings menu Object to get the Display Setting the user wishes
};

#endif // _SETTINGSMENU_H_
