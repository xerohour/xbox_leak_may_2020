/*****************************************************
*** memorymenu.h
***
*** Header file for our XShell Format menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** January 10th, 2001
***
*****************************************************/

#ifndef _MEMORYMENU_H_
#define _MEMORYMENU_H_

#include "menuscreen.h"  // Base menu type

// Sub Menus
#include "memoryoptions.h"

class CMemoryMenu : public CMenuScreen
{
public:
    CMemoryMenu( CXItem* pParent );
    ~CMemoryMenu();

    // Must be overridden for this to be a Item on our XShell program
    void Enter();   // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );

    // Public Methods
    HRESULT Init( char* menuFileName );     // Initialize our font and settings

private:
    // Menus
    CMemoryOptions m_MemoryOptionsMenu;     // The Memory Options Menu Screen

    // Private Methods
};

#endif // _MEMORYMENU_H_
