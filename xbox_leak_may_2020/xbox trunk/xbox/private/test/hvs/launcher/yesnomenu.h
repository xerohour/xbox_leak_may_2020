/*****************************************************
*** yesnomenu.h
***
*** Header file for our Yes/No Menu class.
*** This will allow the user to select Yes or No
***
*** by James N. Helm
*** December 13th, 2001
***
*****************************************************/

#ifndef _YESNOMENU_H_
#define _YESNOMENU_H_

const unsigned int gc_uiYESNO_NUM_ROWS =        1;     
const unsigned int gc_uiYESNO_NUM_COLS =        2;     
                                                
const unsigned int gc_uiYESNO_BUTTON_SPACER =   35;     
const unsigned int gc_uiYESNO_BUTTON_HEIGHT =   40;     
const unsigned int gc_uiYESNO_BUTTON_WIDTH =    100;     
                                                
const unsigned int gc_uiYESNO_TOPROW_Y1 =       285;     
const unsigned int gc_uiYESNO_TOPROW_X1 =       ( 640 / 2 ) - ( gc_uiYESNO_BUTTON_SPACER / 2 ) - gc_uiYESNO_BUTTON_WIDTH;
                                                
const float gc_fYESNO_TEXT_YPOS =               125.0f     ;
                                                
const float gc_fYESNO_LINE_WIDTH =              1.0f;     

const unsigned int gc_uiNUM_TEXT_LINES =        5;

#include "menuscreen.h"  // Base menu type
#include "key.h"

class CYesNoMenu : public CMenuScreen
{
public:
    // Constructors and Destructors
    CYesNoMenu( void );
    virtual ~CYesNoMenu( void );

    // Process drawing and input for a menu screen
    virtual void Enter( void );   // This will be called whenever this menu is entered / activated
    virtual void Action( CUDTexture* pTexture );
    virtual void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    virtual void HandleInput( enum CONTROLS buttonPressed, BOOL bFirstPress );

    virtual HRESULT Init( XFONT* pMenuItemFont,             // Initialize the Menu
                          XFONT* pMenuTitleFont );

    void SetText( WCHAR* pwszMessage );                     // Used to set the text message that will be displayed to the user

    BOOL GetCancelled( void ) { return m_bCancelled; };     // Used to determine if the user cancelled the keyboard

protected:
    // Properties
    WCHAR   m_apwszTextMessage[gc_uiNUM_TEXT_LINES][MAX_PATH+1];	// Used to hold the text message
    BOOL    m_bCancelled;                                           // Used to determine if the user cancelled the menu
	int     m_xKeypos;                                              // Current X position on the Keyboard
    int     m_yKeypos;                                              // Current Y position on the Keyboard
	CKey    m_keyPad[gc_uiYESNO_NUM_COLS][gc_uiYESNO_NUM_ROWS];     // Each Key displayed on the Keyboard
	
    // Private Methods
	virtual void GenerateKeypad(void);                  // Create the keypad that will be used
	virtual BOOL renderKeypad( CUDTexture* pTexture );  // Draw the keypad on to the our texture

};

#endif // _YESNOMENU_H_