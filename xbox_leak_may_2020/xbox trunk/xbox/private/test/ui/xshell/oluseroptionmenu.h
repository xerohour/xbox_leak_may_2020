/*****************************************************
*** oluseroptionmenu.h
***
*** Header file for our Online User Option menu class
*** This menu will present the user with a list of
*** options that can be performed on/with an Online
*** user.
***
*** by James N. Helm
*** February 15th, 2002
***
*****************************************************/

#ifndef _OLUSEROPTIONMENU_H_
#define _OLUSEROPTIONMENU_H_

const unsigned int gc_uiOLUSEROPTION_NUM_ROWS =         1;     
const unsigned int gc_uiOLUSEROPTION_NUM_COLS =         2;     
                                                
const unsigned int gc_uiOLUSEROPTION_BUTTON_SPACER =    35;     
const unsigned int gc_uiOLUSEROPTION_BUTTON_HEIGHT =    40;     
const unsigned int gc_uiOLUSEROPTION_BUTTON_WIDTH =     100;     
                                                
const unsigned int gc_uiOLUSEROPTION_TOPROW_Y1 =        135;     
const unsigned int gc_uiOLUSEROPTION_TOPROW_X1 =        (unsigned int)MENUBOX_TEXTAREA_X_CENTER - ( gc_uiOLUSEROPTION_BUTTON_SPACER / 2 ) - gc_uiOLUSEROPTION_BUTTON_WIDTH;
                                                
const float gc_fOLUSEROPTION_TEXT_YPOS =                24.0f;
                                                
const float gc_fOLUSEROPTION_LINE_WIDTH =               1.0f;     

const WCHAR gc_pwszOLUSEROPTION_COPY_TEXT[] =           L"Copy";
const WCHAR gc_pwszOLUSEROPTION_DELETE_TEXT[] =         L"Delete";

const unsigned int gc_uiOLUSEROPTION_NUM_TEXT_LINES =   5;

enum OLUSEROPTION_OPTIONS
{
    ENUM_OLUSEROPTION_NONE,
    ENUM_OLUSEROPTION_COPY,
    ENUM_OLUSEROPTION_DELETE
};

#include "menuscreen.h"  // Base menu type
#include "key.h"

class COLUserOptionMenu : public CMenuScreen
{
public:
    // Constructors and Destructors
    COLUserOptionMenu( CXItem* pParent );
    virtual ~COLUserOptionMenu();

    // Process drawing and input for a menu screen
    virtual void Enter();   // This will be called whenever this menu is entered / activated
    virtual void Action( CUDTexture* pTexture );
    virtual void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    virtual void HandleInput( enum CONTROLS buttonPressed, BOOL bFirstPress );

    virtual HRESULT Init( char* menuFileName );                 // Initialize the Menu
    void SetText( WCHAR* pwszMessage );                         // Used to set the text message that will be displayed to the user

    unsigned int GetOption() { return m_uiSelectedOption; };    // Get the value of what the user selected
    BOOL GetCancelled() { return m_bCancelled; };               // Used to determine if the user cancelled the keyboard

protected:
    // Properties
    WCHAR   m_apwszTextMessage[gc_uiNUM_TEXT_LINES][MAX_PATH+1];	// Used to hold the text message
    BOOL    m_bCancelled;                                           // Used to determine if the user cancelled the menu
	int     m_xKeypos;                                              // Current X position on the Keyboard
    int     m_yKeypos;                                              // Current Y position on the Keyboard
	CKey    m_keyPad[gc_uiOLUSEROPTION_NUM_COLS][gc_uiOLUSEROPTION_NUM_ROWS];     // Each Key displayed on the Keyboard
    unsigned int m_uiSelectedOption;                                // Used to determine the option that the user selected
	
    // Private Methods
    void Leave( CXItem* pItem );                        // This will be called whenever this menu is entered / activated
	virtual void GenerateKeypad(void);                  // Create the keypad that will be used
	virtual BOOL renderKeypad( CUDTexture* pTexture );  // Draw the keypad on to the our texture
};

#endif // _OLUSEROPTIONMENU_H_