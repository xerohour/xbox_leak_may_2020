/*****************************************************
*** avregionmenu.h
***
*** Header file for our AV Region menu class.  This
*** class will allow a user to select a new AV
*** region
***
*** by James Helm
*** May 13th, 2001
***
*****************************************************/

#ifndef _AVREGIONMENU_H_
#define _AVREGIONMENU_H_

#include "keypadmenu.h"  // Base menu type

class CAVRegionMenu : public CKeypadMenu
{
public:
    // Constructors and Destructor
    CAVRegionMenu( CXItem* pParent );
    ~CAVRegionMenu();

    // Methods
    void Enter();                                                       // This will be called whenever the user enters this menu
    void Action( CUDTexture* pTexture );                                // Perform our actions (draw the screen items, etc)
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );   // Handle input to the buttons
    void HandleInput( enum CONTROLS buttonPressed, BOOL bFirstPress );

    HRESULT Init( char* menuFileName );                                 // Initialize our font and settings

private:
    // Properties
    Key m_keyPad[MENU_AVREGION_NUM_COLS][MENU_AVREGION_NUM_ROWS];   // Keypad
    unsigned int m_uiButtonWidth;                                   // Used to calculate our button width
    unsigned int m_uiButtonX1;                                      // Used to calculate the X1 position of our buttons


    void GenerateKeypad(void);
	BOOL renderKeypad( CUDTexture* pTexture );

    DWORD GetAVRegion();                            // Get the AV Region in a 'menu-friendly' format
    HRESULT SetAVRegion( DWORD dwAVRegion );        // Set the AV Region using our 'menu-friendly' format
};

#endif // _AVREGIONMENU_H_