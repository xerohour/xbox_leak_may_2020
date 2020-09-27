/*****************************************************
*** savegamemenu.h
***
*** Header file for our Game Title menu class.  This
*** menu will list all of the Saved Games for a
*** specified Game Title, and allow the user to
*** select one
***
*** by James N. Helm
*** June 17th, 2001
***
*****************************************************/

#ifndef _SAVEGAMEMENU_H_
#define _SAVEGAMEMENU_H_

#include "menuscreen.h"

class CSaveGameMenu : public CMenuScreen
{
public:
    CSaveGameMenu( CXItem* pParent );
    ~CSaveGameMenu();

    // Must be overridden for this to be a Item on our XShell program
    void Enter();                                       // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );                // Called each frame -- renders screen, and performs other necessary actions
                 
    void HandleInput( enum BUTTONS buttonPressed,       // Handles Button input for this screen
                      BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed,     // Handles Control input for this screen
                      BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick,           // Handles Joystick input for this screen
                      int nThumbY,
                      int nThumbX,
                      BOOL bFirstYPress,
                      BOOL bFirstXPress );

    // Public Methods
    HRESULT Init( IDirect3DDevice8* pD3DDevice, // Initialize our font and settings
                  char* menuFileName );
    void    Leave( CXItem* pItem );             // Should be called whenever a user deactivates this item
    void    SetInfo( CGameTitle* pGameTitle );  // Sets the Xbox Game object the menu should use

private:
    // Private Properties
    unsigned int    m_uiTopItemIndex;           // Index of the top item being displayed
    CGameTitle*     m_pGameTitle;               // Pointer to our Game Title Object

    // Private Methods
    // void AdjustDisplayArrows();                     // Adjust the UP and DOWN arrows on the screen
};

#define SAVE_GAME_MENU_ITEM_FONT_HEIGHT        SCREEN_DEFAULT_FONT_HEIGHT
#define SAVE_GAME_MENU_NUM_ITEMS_TO_DISPLAY    9

#endif // _SAVEGAMEMENU_H_
