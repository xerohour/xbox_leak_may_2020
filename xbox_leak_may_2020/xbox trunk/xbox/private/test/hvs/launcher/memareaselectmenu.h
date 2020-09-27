/*****************************************************
*** memareaselectmenu.h
***
*** Header file for our Memory Area Select Menu class.
*** This menu will allow the user to select from one
*** of the 9 available Memory Areas
***
*** by James N. Helm
*** November 30th, 2001
***
*****************************************************/

#ifndef _MEMAREASELECTMENU_H_
#define _MEMAREASELECTMENU_H_

#include "menuscreen.h"

class CMemAreaSelectMenu : public CMenuScreen
{
public:
    // Constructors and Destructor
    CMemAreaSelectMenu( void );
    ~CMemAreaSelectMenu( void );

    // Public Methods
    void Action( CUDTexture* pTexture );            // Draws a menu on to the screen
    HRESULT Init( XFONT* pMenuItemFont,             // Initialize the Menu
                  XFONT* pMenuTitleFont );
    void HandleInput( enum BUTTONS buttonPressed,   // Handles input (of the BUTTONS) for the current menu
                      BOOL bFirstPress );

    void Enter( void );                             // This should be called whenever the menu is entered or activated

    BOOL GetCancelled( void )                       // Will return TRUE if the user cancelled the selection
        { return m_bCancelled; };
    int GetSelectedAreaIndex( void )                // Return the index of the selected memory area (8 is Xbox Hard Disk, 0 through 7 are MUs)
        { return GetSelectedItemValue(); };
private:
    BOOL m_bCancelled;                              // Used to determine if the user cancelled the selection

    void UpdateMenuItems( void );                   // Update the menu items on our screen
};

#endif // _MEMAREASELECTMENU_H_