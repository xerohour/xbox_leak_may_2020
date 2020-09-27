/*****************************************************
*** xitem.cpp
***
*** CPP file for our abstract base class for a
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

#include "menuspch.h"
#include "xitem.h"

// Constructor
CXItem::CXItem( CXItem* pParent ) :
m_pParent( NULL ),
m_bUpdateTexture( TRUE ),
m_pwszTitle( NULL )
{
    SetParent( pParent );

    m_pwszTitle = new WCHAR[XITEM_TITLE_SIZE];
    if( NULL == m_pwszTitle )
    {
        XDBGWRN( MENUS_APP_TITLE_NAME_A, "CXItem::CXItem():Failed to allocate memory!!" );
    }
    else
    {
        SetTitle( L"Default Name" );
    }
};

// Destructor
CXItem::~CXItem()
{
    if( NULL != m_pwszTitle )
    {
        delete[] m_pwszTitle;
        m_pwszTitle = NULL;
    }
}


// Set the Title of the menu
HRESULT CXItem::SetTitle( WCHAR* pwszTitle )
{
    if( !pwszTitle )
    {
        XDBGWRN( MENUS_APP_TITLE_NAME_A, "CXItem::SetTitle():Invalid argument passed in!!" );

        return E_INVALIDARG;
    }

    ZeroMemory( m_pwszTitle, sizeof( WCHAR ) * XITEM_TITLE_SIZE );
    lstrcpyn( m_pwszTitle, pwszTitle, XITEM_TITLE_SIZE );

    return S_OK;
}
