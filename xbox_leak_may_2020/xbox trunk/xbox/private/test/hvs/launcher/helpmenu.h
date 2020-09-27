/*****************************************************
*** helpmenu.h
***
*** Header file for our Help Menu class.
*** This menu will display help for whatever menu
*** specifies it
***
*** by James N. Helm
*** December 18th, 2001
***
*****************************************************/

#ifndef _HELPMENU_H_
#define _HELPMENU_H_

#include "menuscreen.h"

class CHelpMenu : public CMenuScreen
{
public:
    // Constructors and Destructor
    CHelpMenu( void );
    ~CHelpMenu( void );

    // Public Methods
    void RenderMenuItems( CUDTexture* pTexture );                   // Render the menu items properly on our menu
    void RenderSelector( CUDTexture* pTexture ) {};                 // Render the selector for our menu
    void Enter( void );                                             // This should be called whenever the menu is entered or activated
    HRESULT Init( XFONT* pMenuItemFont,                             // Initialize the Menu
                  XFONT* pMenuTitleFont );
    void HandleInput( enum JOYSTICK joystick,                       // Handle any requests for the joystick (thumb-pad)
                      int nThumbY,
                      int nThumbX,
                      BOOL bFirstYPress,
                      BOOL bFirstXPress );
    void HandleInput( enum BUTTONS buttonPressed,                   // Handles input (of the BUTTONS) for the current menu
                      BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed,                 // Handles input (of the CONTROLS) for the current menu
                      BOOL bFirstPress );
    void AddHelpItem( XFONT* pItemFont,                             // Add a help item to our menu class
                      WCHAR* pwszHelpItem,
                      XFONT* pValueFont,
                      WCHAR* pwszHelpValue );

    void ClearHelpItems( void );                                    // Clear all the help items from our screen
protected:
private:
};

#endif // _HELPMENU_H_