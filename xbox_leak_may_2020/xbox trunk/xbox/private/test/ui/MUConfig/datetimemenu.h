/*****************************************************
*** datetimemenu.h
***
*** Header file for our DATE/TIME menu class.  This
*** menu will allow the user to select a different
*** date / time.
***
*** by James N. Helm
*** June 18th, 2001
***
*****************************************************/

#ifndef _DATETIMEMENU_H_
#define _DATETIMEMENU_H_

#include "menuscreen.h"  // Base menu type

class CDateTimeMenu : public CMenuScreen
{
public:
    CDateTimeMenu( CXItem* pParent );
    ~CDateTimeMenu();

    // Must be overridden for this to be a Item on our XShell program
    // void Enter();                                       // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed,
                      BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed,
                      BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick,
                      int nThumbY,
                      int nThumbX,
                      BOOL bFirstYPress,
                      BOOL bFirstXPress );

    // Public Methods
    HRESULT Init( IDirect3DDevice8* pD3DDevice, char* menuFileName );       // Initialize our font and settings

private:
    // Private Methods
    // void Leave( CXItem* pItem );                                        // This should be called whenever a user leaves this menu

    // Private Properties
    SYSTEMTIME  m_sysTime;              // Structure to hold our DATE/TIME information
    BOOL        m_bDateTimeModified;    // Used to determine if the user has modified the Date/Time
    DWORD       m_dwTickCount;          // Used to update our texture once a second
};

#endif // _DATETIMEMENU_H_
