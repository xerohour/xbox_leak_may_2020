/*****************************************************
*** gameregionmenu.h
***
*** Header file for our Game Region menu class.  This
*** class will allow a user to select a new game
*** region
***
*** by James Helm
*** May 11th, 2001
***
*****************************************************/

#ifndef _GAMEREGIONMENU_H_
#define _GAMEREGIONMENU_H_

#include "keypadmenu.h"  // Base menu type

class CGameRegionMenu : public CKeypadMenu
{
public:
    // Constructors and Destructor
    CGameRegionMenu( CXItem* pParent );
    ~CGameRegionMenu();

    // Methods
    void Enter();                                                       // This will be called whenever the user enters this menu
    void Action( CUDTexture* pTexture );                                // Perform our actions (draw the screen items, etc)
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );   // Handle input to the buttons
    void HandleInput( enum CONTROLS buttonPressed, BOOL bFirstPress );

private:
    // Properties
    Key m_keyPad[MENU_GAMEREGION_NUM_COLS][MENU_GAMEREGION_NUM_ROWS];   // Keypad

    void GenerateKeypad(void);
	BOOL renderKeypad( CUDTexture* pTexture );

    DWORD GetGameRegion();                          // Get the Game Region in a 'menu-friendly' format
    HRESULT SetGameRegion( DWORD dwGameRegion );    // Set the Game Region using our 'menu-friendly' format
};

#endif // _GAMEREGIONMENU_H_