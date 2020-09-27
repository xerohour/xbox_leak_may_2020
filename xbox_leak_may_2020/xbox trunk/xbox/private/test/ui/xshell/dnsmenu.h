/*****************************************************
*** dnsmenu.h
***
*** Header file for our XShell DNS address menu class.
***
*** by James Helm
*** November 13th, 2001
***
*****************************************************/

#ifndef _DNSMENU_H_
#define _DNSMENU_H_

#include "keypadmenu.h"  // Base menu type

class CDNSMenu : public CKeypadMenu
{
public:
    // Constructors and Destructors
    CDNSMenu( CXItem* pParent );
    ~CDNSMenu();

    // Public Methods
    void Enter();                           // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );    // Perform our actions (draw the screen items, etc)
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );

    void SetPrimaryDNS( BOOL bSetPrimary ); // Used to determine if we should set the Primary or Secondary DNS Address (TRUE for Primary)


private:
    void Leave( CXItem* pItem );            // This will be called whenever a user leaves this menu
    
    BOOL m_bSetPrimaryDNS;                  // If TRUE, the menu will set the Primary DNS Address, otherwise the Secondary DNS Address
};

#endif // _DNSMENU_H_