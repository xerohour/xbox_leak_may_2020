/*****************************************************
*** errormsgmenu.h
***
*** Header file for our XShell Error Message class.
*** This screen will display an error message and an
*** "ok" menu choice
***
*** by James N. Helm
*** January 17th, 2001
***
*****************************************************/

#ifndef _ERRORMSGMENU_H_
#define _ERRORMSGMENU_H_

#include "keypadmenu.h"

#define ERRORMSGMENU_NUM_ROWS    1
#define ERRORMSGMENU_NUM_COLS    1

class CErrorMsgMenu : public CKeypadMenu
{
public:
    CErrorMsgMenu( CXItem* pParent );
    ~CErrorMsgMenu();

    // Must be overridden for this to be a Item on our XShell program
    void Enter();   // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );

    // Public Methods
    void SetErrorCode( DWORD dwErrorCode ) { m_dwErrorCode = dwErrorCode; };    // Set the Error Code for the message the menu should display

private:
    void GenerateKeypad(void);
	BOOL renderKeypad( CUDTexture* pTexture );

    // Properties
    DWORD m_dwErrorCode;
};

#endif // _ERRORMSGMENU_H_
