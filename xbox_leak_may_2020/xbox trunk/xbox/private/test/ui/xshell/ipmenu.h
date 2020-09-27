/*****************************************************
*** ipmenu.h
***
*** Header file for our XShell IP address menu class.
***
*** by Victor Blanco
*** December 2nd, 2000
***
*****************************************************/

#ifndef _IPMENU_H_
#define _IPMENU_H_

#include "keypadmenu.h"  // Base menu type

class CIPMenu : public CKeypadMenu
{
public:
    // Constructors and Destructors
    CIPMenu( CXItem* pParent );
    ~CIPMenu();

    // Public Methods
    void Enter();                           // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );    // Perform our actions (draw the screen items, etc)
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );

    void SetTitleIP( BOOL bSetTitle );      // Used to determine if we should set the Title IP, or the Debug IP Address (TRUE for Title)


private:
    void Leave( CXItem* pItem );            // This will be called whenever a user leaves this menu
    
    BOOL m_bSetTitleIP;                     // If TRUE, the menu will set the TITLE IP Address, otherwise the DEBUG IP Address
    BOOL m_bDisplayInUseMessage;            // Used when the IP Address is already in use
};

#endif // _IPMENU_H_