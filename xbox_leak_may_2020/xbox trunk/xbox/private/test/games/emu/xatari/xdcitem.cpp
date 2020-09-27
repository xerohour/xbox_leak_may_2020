/*****************************************************
*** xdcitem.cpp
***
*** CPP file for our abstract base class for a
*** generic screen of our Atari program.
***
*** by James N. Helm
*** November 17th, 2000
***
*****************************************************/

#include "xdcitem.h"

// Constructor
CXDCItem::CXDCItem() :
m_pwszDescription( NULL ),
m_pwszFooterText( NULL ),
m_pParent( NULL ),
m_pwszTitle( NULL )
{
};


CXDCItem::CXDCItem( CXDCItem* pParent, WCHAR* pwszTitle, WCHAR* pwszDescription ) :
m_pwszDescription( NULL ),
m_pwszFooterText( NULL ),
m_pParent( NULL ),
m_pwszTitle( NULL )
{
    if( !pwszTitle || !pwszDescription )
    {
        DebugPrint( "CXDCItem::CXDCItem(): Invalid Parameters!!\n" );
        return;
    }

    SetParent( pParent );
    SetTitle( pwszTitle );
    SetDescription( pwszDescription );
};

// Destructor
CXDCItem::~CXDCItem()
{
    // Clean up the memory for the description if it's used
    if( m_pwszDescription )
    {
        delete[] m_pwszDescription;
        m_pwszDescription = NULL;
    }

    // Clean up the memory for the title if it's used
    if( m_pwszTitle )
    {
        delete[] m_pwszTitle;
        m_pwszTitle = NULL;
    }

    // Clean up the memory for the footertext if it's used
    if( m_pwszFooterText )
    {
        delete[] m_pwszFooterText;
        m_pwszFooterText = NULL;
    }
}

// Set the description of our current item
void CXDCItem::SetDescription( const WCHAR* pwszDescription )
{
    if( !pwszDescription )
    {
        DebugPrint( "CXDCItem::SetDescription(): pwszDescription was not a valid pointer!\n" );
        return;
    }

    if( m_pwszDescription )
    {
        delete[] m_pwszDescription;
        m_pwszDescription = NULL;
    }

    m_pwszDescription = _wcsdup( pwszDescription );
}

// Set the title of our current item
void CXDCItem::SetTitle( const WCHAR* pwszTitle )
{
    if( !pwszTitle )
    {
        DebugPrint( "CXDCItem::SetTitle(): pwszTitle was not a valid pointer!\n" );
        return;
    }

    if( m_pwszTitle )
    {
        delete[] m_pwszTitle;
        m_pwszTitle = NULL;
    }

    m_pwszTitle = _wcsdup( pwszTitle );
    if( !m_pwszTitle )
    {
        DebugPrint( "CXDCItem::SetTitle(): could not allocate memory!\n" );
    }
}

// Set the footer text of our current item
void CXDCItem::SetFooterText( const WCHAR* pwszFooterText )
{
    if( !pwszFooterText )
    {
        DebugPrint( "CXDCItem::SetFooterText(): pwszFooterText was not a valid pointer!\n" );
        return;
    }

    if( m_pwszFooterText )
    {
        delete[] m_pwszFooterText;
        m_pwszFooterText = NULL;
    }

    m_pwszFooterText = _wcsdup( pwszFooterText );
    if( !m_pwszFooterText )
    {
        DebugPrint( "CXDCItem::SetFooterText(): could not allocate memory!\n" );
    }
}
