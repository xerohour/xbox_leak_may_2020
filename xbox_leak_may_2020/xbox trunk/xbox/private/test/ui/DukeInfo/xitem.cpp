/*****************************************************
*** xitem.cpp
***
*** CPP file for our abstract base class for a
*** generic screen of our XDash Config program.
*** 
*** by James N. Helm
*** November 2nd, 2000
***
*** Modified 12/03/2000
***   by James N. Helm for XShell
*** 
*****************************************************/

#include "stdafx.h"
#include "xitem.h"

// Constructor
CXItem::CXItem() :
m_pParent( NULL )
{
};


CXItem::CXItem( CXItem* pParent ) :
m_pParent( NULL )
{
    SetParent( pParent );
};

// Destructor
CXItem::~CXItem()
{ 
}
