/*****************************************************
*** olusermenu.h
***
*** Header file for our XShell Online User menu 
*** class.  This menu will enumerate users for a
*** particular memory area and allow the user to
*** select one.
***
*** by James N. Helm
*** February 15th, 2002
***
*****************************************************/

#ifndef _OLUSERMENU_H_
#define _OLUSERMENU_H_

#include "menuscreen.h"

class COLUserMenu : public CMenuScreen
{
public:
    COLUserMenu( CXItem* pParent );
    ~COLUserMenu();

    // Must be overridden for this to be a Item on our XShell program
    void Enter();   // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );
    BOOL GetCancelled() { return m_bCancelled; };           // Used to determine if the user cancelled the menu
    unsigned int GetValue() { return GetSelectedItem(); };  // Return the index of the selected user
    void SetInfo( PXONLINE_USER aUsers,                     // Store the user information in to the menu
                  unsigned int uiNumUsers );

    // Public Methods
    HRESULT Init( char* menuFileName );     // Initialize our font and settings
    void    Leave( CXItem* pItem );         // Should be called whenever a user deactivates this item
private:
    // Private Properties
    unsigned int    m_uiTopItemIndex;                               // Index of the top item being displayed
    BOOL            m_bCancelled;                                   // Used to determine if the user cancelled the Menu
    PXONLINE_USER   m_paHDUsers;                                    // Users that will be read from the HD

    // Private Methods
    void AdjustDisplayArrows();                     // Adjust the UP and DOWN arrows on the screen
};

#endif // _OLUSERMENU_H_