/*****************************************************
*** muname.h
***
*** Header file for our XShell MU Name class.
*** This screen will name a selected Memory Unit.
***
*** by James N. Helm
*** February 15th, 2001
***
*****************************************************/

#ifndef _MUNAME_H_
#define _MUNAME_H_

#include "keyboardmenu.h"   // Base Menu Type
#include "memoryunit.h"

class CMUName : public CKeyboardMenu
{
public:
    CMUName( CXItem* pParent );
    ~CMUName();

    // Must be overridden for this to be a Item on our XShell program
    void Enter();   // Should be called when this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );

    // Public Methods
    HRESULT SetMemoryUnit( CMemoryUnit* pMemoryUnit );  // Set the address of the Memory Unit to be used

private:
    CMemoryUnit* m_pMemoryUnit;                     // Active Memory Unit
    WCHAR m_pwszMUName[MAX_MUNAME];                 // The name of the current MU

    // Private Methods
    void Leave( CXItem* pItem );                                        // This should be called whenever a user leaves this menu
};

#endif // _MUNAME_H_
