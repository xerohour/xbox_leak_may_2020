/*****************************************************
*** rootmenu.h
***
*** Header file for our root menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** March 28th, 2001
***
*****************************************************/

#ifndef _ROOTMENU_H_
#define _ROOTMENU_H_

// Base menu type
#include "menuscreen.h"

// Sub-menus
#include "memorymenu.h"
#include "stmenu.h"
#include "entrymenu.h"
#include "xbconfigmenu.h"

class CRootMenu : public CMenuScreen
{
public:
    CRootMenu( CXItem* pParent );
    ~CRootMenu();

    // Must be overridden for this to be a Item on our XShell program
    // void Enter();   // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );

    // Public Methods
    HRESULT Init( IDirect3DDevice8* pD3DDevice, char* menuFileName );		// Initialize our font and settings

private:
    // Private properties
    BOOL          m_bProcessing;                                            // Used to determine we are processing information
	// Sub-Menus
	CMemoryMenu   m_MenuMemory;												// Displays the memory areas available to mount/config (main menu)
    CSTMenu       m_MenuST;                                                 // Deal with Creating soundtracks
	CEntryMenu    m_MenuEntry;												// Reboot to the Dash to various entry points
	CXBConfigMenu m_MenuXBConfig;											// Allow the user to configure various Xbox options

    // Private Methods
    void InstallMUConfig();                                                 // Install MUConfig to the Hard Disk
    //void Leave( CXItem* pItem );											// This should be called whenever a user leaves this menu
};

#endif // _ROOTMENU_H_
