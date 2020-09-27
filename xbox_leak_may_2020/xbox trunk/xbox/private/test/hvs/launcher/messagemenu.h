/*****************************************************
*** messagemenu.h
***
*** Header file for our Message Menu class.
*** This menu will present a message to the user, and
*** simply allow them to navigate back from where they
*** came.
***
*** by James N. Helm
*** December 11th, 2001
***
*****************************************************/

#ifndef _MESSAGEMENU_H_
#define _MESSAGEMENU_H_

#include "menuscreen.h"

const float gc_fMESSAGE_TEXT_YPOS =                 125.0f;
const float gc_fOK_BUTTON_YPOS =                    315.0f;
const float gc_fOK_BUTTON_BORDER =                  10.0f;

const unsigned int gc_uiMESS_MENU_NUM_TEXT_LINES =  5;

class CMessageMenu : public CMenuScreen
{
public:
    // Constructors and Destructor
    CMessageMenu( void );
    ~CMessageMenu( void );

    // Public Methods
    HRESULT Init( XFONT* pMenuItemFont,             // Initialize the Menu
                  XFONT* pMenuTitleFont );
    void HandleInput( enum BUTTONS buttonPressed,   // Handles input (of the BUTTONS) for the current menu
                      BOOL bFirstPress );
    void SetText( WCHAR* pwszMessage );             // Used to set the text message that will be displayed to the user
protected:
    void RenderMenuItems( CUDTexture* pTexture );   // Render the menu items properly on our menu

private:
    WCHAR m_apwszTextMessage[gc_uiMESS_MENU_NUM_TEXT_LINES][MAX_PATH+1];	// Used to hold the text message
};

#endif // _MESSAGEMENU_H_