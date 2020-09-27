/*****************************************************
*** keypadmenu.h
***
*** Header file for our XShell keypad menu class.
***
*** by James Helm
*** December 5th, 2000
***
*****************************************************/

#ifndef _KEYPADMENU_H_
#define _KEYPADMENU_H_

#define KEYPAD_NUM_ROWS    4
#define KEYPAD_NUM_COLS    4

#include "menuscreen.h"  // Base menu type

class CKeypadMenu : public CMenuScreen
{
public:
    // Constructors and Destructors
    CKeypadMenu( CXItem* pParent );
    virtual ~CKeypadMenu();

    // Methods
    virtual void Enter();   // This will be called whenever this menu is entered / activated
    virtual void Action( CUDTexture* pTexture );
    virtual void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    virtual void HandleInput( enum CONTROLS buttonPressed, BOOL bFirstPress );
    virtual void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );

    HRESULT SetAddress( char* Address );                    // Sets the Address in to our local member
    HRESULT SetAddress( WCHAR* Address );                   // Sets the Address in to our local member

protected:
// Properties
    WCHAR	 m_pwszAddress[DATAVALUE_IPADDRESS_LENGTH + 1];     // Holds the Address until the user is ready to commit
    BOOL     m_bDropKeyPress;                                   // Used to prevent the 'instant' keypress when a user navigates in here
	int m_xKeypos;                                              // X Position of the current Key on the Keypad
    int m_yKeypos;                                              // Y Position of the current Key on the Keypad
	Key m_keyPad[KEYPAD_NUM_ROWS][KEYPAD_NUM_COLS];             // Information for each Key appearing on the Keypad

    // Private Methods
	virtual void GenerateKeypad(void);                      // Creates the keypad
	virtual BOOL renderKeypad( CUDTexture* pTexture );      // Renders the Keypad on to the texture
};

#endif // _KEYPADMENU_H_