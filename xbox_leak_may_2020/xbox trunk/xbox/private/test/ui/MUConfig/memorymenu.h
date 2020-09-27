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

void FillDisk( char* );

#ifndef _MEMORYMENU_H_
#define _MEMORYMENU_H_

#include "menuscreen.h"  // Base menu type
#include "memoryunit.h"

// Sub-Menus
#include "muoptions.h"
#include "hdoptions.h"

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
    HRESULT Init( IDirect3DDevice8* pD3DDevice, char* menuFileName );       // Initialize our font and settings

private:
    DWORD m_dwInsertedMUs;          // Bit mask of the MU's that are inserted
    DWORD m_dwMUInsertions;         // Used to determine which MU's were inserted since the last check
    DWORD m_dwMURemovals;           // Used to determine which MU's were removed since the last check
    CMemoryUnit m_MemoryUnits[8];   // Memory Units that can be inserted, or removed

    CMUOptions m_MenuMUOptions;     // MU Options Sub-Menu
    CHDOptions m_MenuHDOptions;     // HD Options Sub-Menu

    // Private Methods
    // void Leave( CXItem* pItem );    // This should be called whenever a user leaves this menu
};

#endif // _MEMORYMENU_H_
