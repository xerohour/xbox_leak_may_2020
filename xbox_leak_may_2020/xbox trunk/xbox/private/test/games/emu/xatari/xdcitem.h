/*****************************************************
*** xdcitem.h
***
*** Header file for our abstract base class for a
*** generic screen of our Atari program.
***
*** by James N. Helm
*** November 17th, 2000
***
*****************************************************/

#ifndef _XDCITEM_H_
#define _XDCITEM_H_

#include "xboxvideo.h"
#include "usbmanager.h"
#include "linkedlist.h"

class CXDCItem
{
public:
    // Constructors and Destructors
    CXDCItem();
    CXDCItem( CXDCItem* pParent, WCHAR* pwszTitle, WCHAR* pwszDescription );
    ~CXDCItem();

    // Function that MUST be overridden
    virtual void Action( CXBoxVideo* Screen ) = 0;
    virtual void HandleInput( enum BUTTONS buttonPressed, bool bRepeatPress ) = 0;

    virtual WCHAR* GetDescription() const { return m_pwszDescription; };
    virtual WCHAR* GetFooterText() const { return m_pwszFooterText; };
    virtual WCHAR* GetTitle() const { return m_pwszTitle; };
    virtual CXDCItem* GetParent() const { return m_pParent; };

    virtual void SetDescription( const WCHAR* pwszDescription );
    virtual void SetFooterText( const WCHAR* pwszFooterText );
    virtual void SetParent( CXDCItem* pParent ) { m_pParent = pParent; };
    virtual void SetTitle( const WCHAR* pwszTitle );

private:
    WCHAR* m_pwszDescription;           // The Description of the screen
    WCHAR* m_pwszFooterText;            // Text that should be displayed in the footer
    WCHAR* m_pwszTitle;                 // The title of our current screen
    CXDCItem* m_pParent;                // Pointer to the parent screen of this item
};

#endif // _XDCITEM_H_