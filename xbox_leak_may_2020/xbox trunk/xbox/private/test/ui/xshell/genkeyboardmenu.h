/*****************************************************
*** genkeyboardmenu.h
***
*** Header file for our Generic Keyboard Menu class.
*** This is a base menu type
***
*** by James N. Helm
*** December 6th, 2000
***
*** Modified for XDK Launcher February 13th, 2002
***
*****************************************************/

#ifndef _GENKEYBOARDMENU_H_
#define _GENKEYBOARDMENU_H_

const unsigned int gc_uiGENKEYBOARD_NUM_ROWS =         4;
const unsigned int gc_uiGENKEYBOARD_NUM_COLS =         10;

const unsigned int gc_uiGENKEYBOARD_TOPROW_Y1 =        75;
const unsigned int gc_uiGENKEYBOARD_TOPROW_X1 =        8;

const unsigned int gc_uiGENKEYBOARD_TEXTSTRING_Y1 =    24;
const unsigned int gc_uiGENKEYBOARD_VALUE_Y1 =         48;

const float gc_fGENKEYBOARD_LINE_WIDTH =               1.0f;

#include "key.h"         // TODO: Replace all instances of "Key" class with "CKey" class
#include "menuscreen.h"  // Base menu type

enum GenKeyboardStates
{
    ENUM_GENKEYBOARDSTATE_FIRST_STATE = 0,
    ENUM_GENKEYBOARDSTATE_LOWER_ALPHA = 0,
    ENUM_GENKEYBOARDSTATE_UPPER_ALPHA,
    ENUM_GENKEYBOARDSTATE_SYMBOLS,
    
    ENUM_GENKEYBOARDSTATE_NUM_STATES
};

class CGenKeyboardMenu : public CMenuScreen
{
public:
    // Constructors and Destructors
    CGenKeyboardMenu( CXItem* pParent );
    virtual ~CGenKeyboardMenu();

    // Process drawing and input for a menu screen
    virtual void Enter();   // This will be called whenever this menu is entered / activated
    virtual void Action( CUDTexture* pTexture );
    virtual void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    virtual void HandleInput( enum CONTROLS buttonPressed, BOOL bFirstPress );

    virtual HRESULT Init( char* menuFileName );             // Initialize the Menu

    HRESULT SetInfo( WCHAR* pwszTextString,                 // This should be called before the Keyboard is invoked
                     unsigned int uiLength,
                     WCHAR* pwszValueDefault = NULL );

    BOOL GetCancelled() { return m_bCancelled; };           // Used to determine if the user cancelled the keyboard
    WCHAR* GetValue() { return m_pwszValueBuffer; };        // Return a pointer to the user entered value

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
	CKey            m_keyPad[gc_uiGENKEYBOARD_NUM_COLS][gc_uiGENKEYBOARD_NUM_ROWS];   // Each Key displayed on the Keyboard
    BOOL            m_bKeyFirstPress;                                           // Used to determine if the user has pressed a key before
	
    // Private Methods
	virtual void GenerateKeypad(void);                  // Create the keypad that will be used
	virtual BOOL renderKeypad( CUDTexture* pTexture );  // Draw the keypad on to the our texture
};

#endif // _GENKEYBOARDMENU_H_