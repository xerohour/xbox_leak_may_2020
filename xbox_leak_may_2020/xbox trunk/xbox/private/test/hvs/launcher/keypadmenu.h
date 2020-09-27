/*****************************************************
*** keypadmenu.h
***
*** Header file for our Kaypad Menu class.
***
*** by James N. Helm
*** December 18th, 2001
***
*****************************************************/

#ifndef _KEYPADMENU_H_
#define _KEYPADMENU_H_

const unsigned int gc_uiKEYPAD_NUM_ROWS =         4;
const unsigned int gc_uiKEYPAD_NUM_COLS =         4;

const unsigned int gc_uiKEYPAD_TOPROW_Y1 =        200;
const unsigned int gc_uiKEYPAD_TOPROW_X1 =        205;

const unsigned int gc_uiKEYPAD_TEXTSTRING_Y1 =    140;
const unsigned int gc_uiKEYPAD_VALUE_Y1 =         165;

const float gc_fKEYPAD_LINE_WIDTH =               1.0f;

#include "menuscreen.h"  // Base menu type
#include "key.h"

class CKeypadMenu : public CMenuScreen
{
public:
    // Constructors and Destructors
    CKeypadMenu( void );
    virtual ~CKeypadMenu( void );

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
    WCHAR*          m_pwszTextString;                                       // The string that will be displayed to the user
    WCHAR           m_pwszValueBuffer[MAX_PATH+1];                          // Will hold the information that is collected
    unsigned int    m_uiLength;                                             // Maximum size of the string to collect
    BOOL            m_bCancelled;                                           // Used to determine if the user cancelled the keyboard
	int             m_xKeypos;                                              // Current X position on the Keyboard
    int             m_yKeypos;                                              // Current Y position on the Keyboard
	CKey            m_keyPad[gc_uiKEYPAD_NUM_COLS][gc_uiKEYPAD_NUM_ROWS];   // Each Key displayed on the Keyboard
    BOOL            m_bFirstPress;                                          // Used to determine if the user has pressed a key before
	
    // Private Methods
	virtual void GenerateKeypad(void);                  // Create the keypad that will be used
	virtual BOOL renderKeypad( CUDTexture* pTexture );  // Draw the keypad on to the our texture

};

#endif // _MACHINENAMEMENU_H_