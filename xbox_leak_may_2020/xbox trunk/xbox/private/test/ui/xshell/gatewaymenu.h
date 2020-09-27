/*****************************************************
*** gatewaymenu.h
***
*** Header file for our XShell Gateway menu class.
***
*** by Victor Blanco
*** December 2nd, 2000
***
*****************************************************/

#ifndef _GATEWAYMENU_H_
#define _GATEWAYMENU_H_

#include "keypadmenu.h"  // Base menu type

class CGatewayMenu : public CKeypadMenu
{
public:
    // Constructors and Destructors
    CGatewayMenu( CXItem* pParent );
    ~CGatewayMenu();

    // Public Members
    void Enter();                           // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );    // Perform our actions (draw the screen items, etc)
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );

    // Public Methods

private:
};

#endif // _GATEWAYMENU_H_