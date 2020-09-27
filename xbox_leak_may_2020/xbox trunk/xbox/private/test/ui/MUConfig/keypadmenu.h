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

#include "key.h"
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

    HRESULT SetInfo( WCHAR* pwszTextString,                 // This should be called before the Keyboard is invoked
                     WCHAR* pwszValueBuffer,
                     unsigned int uiValueSize );

protected:
    // Properties
    WCHAR* m_pwszTextString;                                // The string that will be displayed to the user
    WCHAR* m_pwszValueBuffer;                               // Will hold the information that is collected
    unsigned int m_uiValueSize;                             // The length of the value you wish to collect
    BOOL m_bDropKeyPress;                                   // Used to prevent the 'instant' keypress when a user navigates in here
	int m_xKeypos;                                          // X Position of the current Key on the Keypad
    int m_yKeypos;                                          // Y Position of the current Key on the Keypad
	CKey m_keyPad[KEYPAD_NUM_ROWS][KEYPAD_NUM_COLS];        // Information for each Key appearing on the Keypad

    // Private Methods
	virtual void GenerateKeypad(void);                      // Creates the keypad
	virtual BOOL renderKeypad( CUDTexture* pTexture );      // Renders the Keypad on to the texture
};

#endif // _KEYPADMENU_H_