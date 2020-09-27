/*****************************************************
*** xitem.h
***
*** Header file for our abstract base class for a
*** generic screen of our XDash Config program.
***
*** by James N. Helm
*** November 2nd, 2000
***
*** Modified
*** 12/03/2000 - by James N. Helm for XShell
*** 03/09/2001 - by James N. Helm for MU Config
*** 06/22/2001 - by James N. Helm for MU Config
***              Added Set/Get Title functions and
***              property
*** 11/21/2001 - by James N. Helm for HVS
                 Removed everything but action and
                 control functions
*****************************************************/

#ifndef _XDCITEM_H_
#define _XDCITEM_H_

#include "udtexture.h"

class CXItem
{
public:
    // Constructors and Destructors
    CXItem( void );
    virtual ~CXItem( void );

    // Functions that MUST be overridden
    virtual void Action( CUDTexture* pTexture ) = 0;
    virtual void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress ) = 0;
    virtual void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress ) = 0;
    virtual void HandleInput( enum JOYSTICK joystick, int nThumbLY, int nThumbLX, BOOL bFirstYPress, BOOL bFirstXPress ) = 0;
    
    virtual void Enter( void ) = 0;                     // Should be called whenever the menu becomes active
    virtual void Leave( CXItem* pItem ) = 0;            // Should be called whenever user is leaving the menu
protected:

private:
};

#endif // _XDCITEM_H_