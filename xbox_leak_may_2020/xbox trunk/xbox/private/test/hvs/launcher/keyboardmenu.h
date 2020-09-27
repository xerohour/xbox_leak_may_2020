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

const unsigned int gc_uiKEYBOARD_NUM_ROWS =         4;
const unsigned int gc_uiKEYBOARD_NUM_COLS =         10;

const unsigned int gc_uiKEYBOARD_TOPROW_Y1 =        200;
const unsigned int gc_uiKEYBOARD_TOPROW_X1 =        110;

const unsigned int gc_uiKEYBOARD_TEXTSTRING_Y1 =    140;
const unsigned int gc_uiKEYBOARD_VALUE_Y1 =         165;

const float gc_fKEYBOARD_LINE_WIDTH =               1.0f;

#include "menuscreen.h"  // Base menu type
#include "key.h"

enum KeyboardStates
{
    ENUM_KEYBOARDSTATE_FIRST_STATE = 0,

    ENUM_KEYBOARDSTATE_LOWER_ALPHA = 0,
    ENUM_KEYBOARDSTATE_UPPER_ALPHA,
    ENUM_KEYBOARDSTATE_SYMBOLS,
    
    ENUM_KEYBOARDSTATE_NUM_STATES
};

class CKeyboardMenu : public CMenuScreen
{
public:
    // Constructors and Destructors
    CKeyboardMenu( void );
    virtual ~CKeyboardMenu( void );

    // Process drawing and input for a menu screen
    virtual void Enter( void );   // This will be called whenever this menu is entered / activated
    virtual void Action( CUDTexture* pTexture );
    virtual void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    virtual void HandleInput( enum CONTROLS buttonPressed, BOOL bFirstPress );

    virtual HRESULT Init( XFONT* pMenuItemFont,             // Initialize the Menu
                          XFONT* pMenuTitleFont );

    HRESULT SetInfo( WCHAR* pwszTextString,                 // This should be called before the Keyboard is invoked
                     unsigned int uiLength,
                     WCHAR* pwszValueDefault = NULL );

    BOOL GetCancelled( void ) { return m_bCancelled; };     // Used to determine if the user cancelled the keyboard
    WCHAR* GetValue( void ) { return m_pwszValueBuffer; };  // Return a pointer to the user entered value

protected:
    // Properties
    WCHAR*          m_pwszTextString;                                           // The string that will be displayed to the user
    WCHAR           m_pwszValueBuffer[MAX_PATH+1];                              // Will hold the information that is collected
    unsigned int    m_uiLength;                                                 // Maximum size of the string to collect
    BOOL            m_bCancelled;                                               // Used to determine if the user cancelled the keyboard
	int             m_xKeypos;                                                  // Current X position on the Keyboard
    int             m_yKeypos;                                                  // Current Y position on the Keyboard
    unsigned char   m_ucState;                                                  // Used to determine which keys are displayed to the user
    BOOL            m_bShift;                                                   // Used to determine if the user has "shifted" the keyboard
	CKey            m_keyPad[gc_uiKEYBOARD_NUM_COLS][gc_uiKEYBOARD_NUM_ROWS];   // Each Key displayed on the Keyboard
    BOOL            m_bFirstPress;                                              // Used to determine if the user has pressed a key before
	
    // Private Methods
	virtual void GenerateKeypad(void);                  // Create the keypad that will be used
	virtual BOOL renderKeypad( CUDTexture* pTexture );  // Draw the keypad on to the our texture

};

#endif // _MACHINENAMEMENU_H_