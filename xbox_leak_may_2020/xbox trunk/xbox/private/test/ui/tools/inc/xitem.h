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
*****************************************************/

#ifndef _XDCITEM_H_
#define _XDCITEM_H_

#include "udtexture.h"

#define XITEM_TITLE_SIZE    100

class CXItem
{
public:
    // Constructors and Destructors
    CXItem( CXItem* pParent );
    virtual ~CXItem();

    // Functions that MUST be overridden
    virtual void Action( CUDTexture* pTexture ) = 0;
    virtual void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress ) = 0;
    virtual void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress ) = 0;
    virtual void HandleInput( enum JOYSTICK joystick, int nThumbLY, int nThumbLX, BOOL bFirstYPress, BOOL bFirstXPress ) = 0;
    
    virtual void Enter() = 0;                       // Should be called whenever the menu becomes active
    virtual void Leave( CXItem* pItem ) = 0;        // Should be called whenever user is leaving the menu

    virtual CXItem* GetParent() const { return m_pParent; };
    virtual void SetParent( CXItem* pParent ) { m_pParent = pParent; };
    virtual HRESULT SetTitle( WCHAR* pwszTitle );       // Set the Title name of the object
    virtual WCHAR* GetTitle() { return m_pwszTitle; };  // Get the Title of the object

    virtual CXItem* GetItemPtr( enum XShellMenuIds menuID ) { return NULL; };  // Items can override this as needed

protected:
    BOOL m_bUpdateTexture;          // An item can use this to determine if the texture should be updated
    WCHAR* m_pwszTitle;             // Used to hold the object title

private:
    CXItem* m_pParent;              // Pointer to the parent screen of this item
};

#endif // _XDCITEM_H_