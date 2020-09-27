/*****************************************************
*** dvdregionmenu.h
***
*** Header file for our DVD Region menu class.  This
*** class will allow a user to select a new DVD
*** region
***
*** by James Helm
*** May 12th, 2001
***
*****************************************************/

#ifndef _DVDREGIONMENU_H_
#define _DVDREGIONMENU_H_

#include "keypadmenu.h"  // Base menu type

class CDVDRegionMenu : public CKeypadMenu
{
public:
    // Constructors and Destructor
    CDVDRegionMenu( CXItem* pParent );
    ~CDVDRegionMenu();

    // Methods
    void Enter();                                                       // This will be called whenever the user enters this menu
    void Action( CUDTexture* pTexture );  // Perform our actions (draw the screen items, etc)
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );   // Handle input to the buttons
    void HandleInput( enum CONTROLS buttonPressed, BOOL bFirstPress );

private:
    // Properties
    CKey m_keyPad[MENU_DVDREGION_NUM_COLS][MENU_DVDREGION_NUM_ROWS];  // Keypad

    void GenerateKeypad(void);
	BOOL renderKeypad( CUDTexture* pTexture );

    DWORD GetDVDRegion();                           // Get the DVD Region in a 'menu-friendly' format
    HRESULT SetDVDRegion( DWORD dwDVDRegion );      // Set the DVD Region using our 'menu-friendly' format
};

#endif // _DVDREGIONMENU_H_