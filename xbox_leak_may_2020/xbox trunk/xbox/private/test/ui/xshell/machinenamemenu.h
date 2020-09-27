/*****************************************************
*** machinenamemenu.h
***
*** Header file for our XShell Machine Name menu
*** class.
***
*** by Victor Blanco
*** December 2nd, 2000
***
*****************************************************/

#ifndef _MACHINENAMEMENU_H_
#define _MACHINENAMEMENU_H_

#include "keyboardmenu.h"  // Base Menu Type

class CMachineNameMenu : public CKeyboardMenu
{
public:
    // Constructors and Destructors
    CMachineNameMenu( CXItem* pParent );
    ~CMachineNameMenu();

    // Must be overridden for this to be a Item on our XShell program
    void Enter();   // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );

    // Public Methods
    HRESULT SetMachineName( char* MachineName );                // Sets the MachineName in to our local member
    HRESULT SetMachineName( WCHAR* MachineName );               // Sets the MachineName in to our local member

private:
	WCHAR	 m_MachineName[DATAVALUE_MACHINENAME_LENGTH + 1];   // Holds the MachineName until the user is ready to commit
    BOOL     m_bDisplayInUseMessage;                            // Used when the machine name is already in use

    // Private Methods
    void Leave( CXItem* pItem );                                // This should be called whenever a user leaves this menu
};

#endif // _MACHINENAMEMENU_H_