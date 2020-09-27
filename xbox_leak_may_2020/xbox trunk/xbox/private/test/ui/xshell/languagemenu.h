/*****************************************************
*** languagemenu.h
***
*** Header file for our XShell language menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** June 2nd, 2001
***
*****************************************************/

#ifndef _LANGUAGEMENU_H_
#define _LANGUAGEMENU_H_

#include "menuscreen.h"

class CLanguageMenu : public CMenuScreen
{
public:
    CLanguageMenu( CXItem* pParent );
    ~CLanguageMenu();

    // Must be overridden for this to be a Item on our XShell program
    void Enter();   // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );

    // Public Methods
    HRESULT Init( char* menuFileName );     // Initialize our font and settings

private:

};

#endif // _LANGUAGEMENU_H_
