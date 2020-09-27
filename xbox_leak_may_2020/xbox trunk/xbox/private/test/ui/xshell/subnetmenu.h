/*****************************************************
*** subnetmenu.h
***
*** Header file for our XShell subnet mask menu class.
***
*** by Victor Blanco
*** December 2nd, 2000
***
*****************************************************/

#ifndef _SUBNETMENU_H_
#define _SUBNETMENU_H_

#include "keypadmenu.h"

class CSubnetMenu : public CKeypadMenu
{
public:
    // Constructors and Destructors
    CSubnetMenu( CXItem* pParent );
    ~CSubnetMenu();

    // Public Members
    void Enter();                           // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );    // Perform our actions (draw the screen items, etc)
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );

    // Public Methods

private:
};

#endif // _SUBNETMENU_H_