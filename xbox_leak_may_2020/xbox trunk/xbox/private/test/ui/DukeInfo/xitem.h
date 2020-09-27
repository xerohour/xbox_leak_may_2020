/*****************************************************
*** xitem.h
***
*** Header file for our abstract base class for a
*** generic screen of our XDash Config program.
***
*** by James N. Helm
*** November 2nd, 2000
***
*****************************************************/

#ifndef _XDCITEM_H_
#define _XDCITEM_H_

class CXItem
{
public:
    // Constructors and Destructors
    CXItem();
    CXItem( CXItem* pParent );
    ~CXItem();

    // Function that MUST be overridden
    virtual void Action( CXBoxVideo* Screen ) = 0;

    virtual CXItem* GetParent() const { return m_pParent; };

    virtual void SetParent( CXItem* pParent ) { m_pParent = pParent; };

private:
    CXItem* m_pParent;              // Pointer to the parent screen of this item
};

#endif // _XDCITEM_H_