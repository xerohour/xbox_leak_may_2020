/*****************************************************
*** keyboardmenu.h
***
*** Header file for our Keyboard Menu class.
*** This is a base menu type
***
*** by James N. Helm
*** December 6th, 2000
***
*****************************************************/

#ifndef _KEYBOARDMENU_H_
#define _KEYBOARDMENU_H_

#define KEYBOARD_NUM_ROWS    4
#define KEYBOARD_NUM_COLS    10

#include "menuscreen.h"  // Base menu type

class CKeyboardMenu : public CMenuScreen
{
public:
    // Constructors and Destructors
    CKeyboardMenu( CXItem* pParent );
    virtual ~CKeyboardMenu();

    // Process drawing and input for a menu screen
    virtual void Enter();   // This will be called whenever this menu is entered / activated
    virtual void Action( CUDTexture* pTexture );
    virtual void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    virtual void HandleInput( enum CONTROLS buttonPressed, BOOL bFirstPress );
    virtual void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );

    // Public Methods

protected:
	int         m_xKeypos;                                          // Current X position on the Keyboard
    int         m_yKeypos;                                          // Current Y position on the Keyboard
	Key         m_keyPad[KEYBOARD_NUM_COLS][KEYBOARD_NUM_ROWS];     // Each Key displayed on the Keyboard
    BOOL        m_bDropKeyPress;                                    // Used to prevent the 'instant' keypress when a user navigates in here
    BOOL        m_bShift;                                           // Used to determine if the keyboard is in "Shift" mode
	
    // Private Methods
	virtual void GenerateKeypad(void);                  // Create the keypad that will be used
	virtual BOOL renderKeypad( CUDTexture* pTexture );  // Draw the keypad on to the our texture
};

#endif // _MACHINENAMEMENU_H_