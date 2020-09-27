/*****************************************************
*** entrymenu.h
***
*** Header file for our Entry Point menu
*** class. This menu class will allow you to perform
*** various Entry Point operations.
***
*** by James N. Helm
*** April 23rd, 2001
***
*****************************************************/

#ifndef _ENTRYMENU_H_
#define _ENTRYMENU_H_

#include "menuscreen.h"  // Base menu type

class CEntryMenu : public CMenuScreen
{
public:
    CEntryMenu( CXItem* pParent );
    ~CEntryMenu();

    // Must be overridden for this to be a Item on our XShell program
    // void Enter();   // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );

    // Public Methods
    HRESULT Init( IDirect3DDevice8* pD3DDevice, char* menuFileName );       // Initialize our font and settings

private:
    unsigned int    m_uiTopItemIndex;       // Index of the top item being displayed
    unsigned int    m_uiNumItemsToDisplay;  // Used to hold the number of items to display on our menu

    WCHAR*  m_pwszValueBuffer;				// Buffer to hold info from our Keypad / Keyboard functions
	BOOL	m_bActionInitiated;				// Used to determine when the user is performing an action

    WCHAR*  m_pwszBuffer1;                  // Used to hold info from our Keypad / Keyboard functions
    WCHAR*  m_pwszBuffer2;                  // Used to hold info from our Keypad / Keyboard functions

    // Private Methods
    // void Leave( CXItem* pItem );                                        // This should be called whenever a user leaves this menu
};

#endif // _ENTRYMENU_H_
