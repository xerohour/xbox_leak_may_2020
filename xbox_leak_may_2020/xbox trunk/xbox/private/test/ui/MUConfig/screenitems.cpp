/*****************************************************
*** screenitems.cpp
***
*** CPP file for our screen item classes.
*** This file will contain the definition for all
*** types of screen items
***
*** by James N. Helm
*** January 13th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "screenitems.h"

/*****************************************************
/* CTextItem()
/****************************************************/

// Constructor
CTextItem::CTextItem() :
CScreenItem(),
m_pwszText( NULL ),
m_fXPos( 0.0f ),
m_fYPos( 0.0f ),
m_dwFGColor( 0L ),
m_dwBGColor( 0L )
{
}

// Destructor
CTextItem::~CTextItem()
{
    // Clean up our allocated memory
    if( m_pwszText )
    {
        delete[] m_pwszText;
        m_pwszText = NULL;
    }
}

// Allows the user to update the information for the Text Item
void CTextItem::UpdateItem( WCHAR* pwszText, float fXPos, float fYPos, DWORD dwFGColor, DWORD dwBGColor )
{
    // Ensure we were passed a valid pointer
    if( !pwszText )
    {
        XDBGWRN( APP_TITLE_NAME_A, " CTextItem()::UpdateItem():pwszText was invalid!!" );

        return;
    }

    // If we have already set a text item, lets clear the memory so we can allocate new memory
    if( m_pwszText )
    {
        delete[] m_pwszText;
        m_pwszText = NULL;
    }

    // Create new memory and copy the string that was passed in by the user
    m_pwszText = _wcsdup( pwszText );

    // Check to see if we could allocate memory -- If not, assert!!
    if( !m_pwszText )
    {
        XDBGWRN( APP_TITLE_NAME_A, " CTextItem()::UpdateItem():Couldn't allocate memory!!" );
    }

    // Set our other parameters
    m_fXPos = fXPos;
    m_fYPos = fYPos;
    m_dwFGColor = dwFGColor;
    m_dwBGColor = dwBGColor;
}


// Used to render itself to the screen
void CTextItem::Render( IDirect3DDevice8* pD3DDevice, CUDTexture* pTexture )
{
    pTexture->DrawText( m_fXPos, m_fYPos, m_dwFGColor, m_dwBGColor, L"%s", m_pwszText );
}

/*****************************************************
/* CPanelItem()
/****************************************************/

// Constructor
CPanelItem::CPanelItem() :
CScreenItem(),
m_fX1Pos( 0.0f ),
m_fY1Pos( 0.0f ),
m_fX2Pos( 0.0f ),
m_fY2Pos( 0.0f ),
m_dwColor( 0L )
{
}

// Destructor
CPanelItem::~CPanelItem()
{
}

// Allows the user to update the information for the Panel Item
void CPanelItem::UpdateItem( float fX1Pos, float fY1Pos, float fX2Pos, float fY2Pos, DWORD dwColor )
{
    m_fX1Pos = fX1Pos;
    m_fY1Pos = fY1Pos;
    m_fX2Pos = fX2Pos;
    m_fY2Pos = fY2Pos;
    m_dwColor = dwColor;
}

// Used to render itself to the screen
void CPanelItem::Render( IDirect3DDevice8* pD3DDevice, CUDTexture* pTexture )
{
    pTexture->DrawBox( m_fX1Pos, m_fY1Pos, m_fX2Pos, m_fY2Pos, m_dwColor );
}


/*****************************************************
/* CLineItem()
/****************************************************/
// Constructor
CLineItem::CLineItem() :
CScreenItem(),
m_fX1Pos( 0.0f ),
m_fY1Pos( 0.0f ),
m_fX2Pos( 0.0f ),
m_fY2Pos( 0.0f ),
m_fWidth( 0.0f ),
m_dwColor( 0L )
{
}

// Destructor
CLineItem::~CLineItem()
{
}

// Allows the user to update the information for the Outline Item
void CLineItem::UpdateItem( float fX1Pos, float fY1Pos, float fX2Pos, float fY2Pos, float fWidth, DWORD dwColor )
{
    m_fX1Pos = fX1Pos;
    m_fY1Pos = fY1Pos;
    m_fX2Pos = fX2Pos;
    m_fY2Pos = fY2Pos;
    m_fWidth = fWidth;
    m_dwColor = dwColor;
}

// Used to render itself to the screen
void CLineItem::Render( IDirect3DDevice8* pD3DDevice, CUDTexture* pTexture )
{
    pTexture->DrawLine( m_fX1Pos, m_fY1Pos, m_fX2Pos, m_fY2Pos, m_fWidth, m_dwColor );
}


/*****************************************************
/* COutlineItem()
/****************************************************/
// Constructor
COutlineItem::COutlineItem() :
CScreenItem(),
m_fX1Pos( 0.0f ),
m_fY1Pos( 0.0f ),
m_fX2Pos( 0.0f ),
m_fY2Pos( 0.0f ),
m_fWidth( 0.0f ),
m_dwColor( 0L )
{
}

// Destructor
COutlineItem::~COutlineItem()
{
}

// Allows the user to update the information for the Outline Item
void COutlineItem::UpdateItem( float fX1Pos, float fY1Pos, float fX2Pos, float fY2Pos, float fWidth, DWORD dwColor )
{
    m_fX1Pos = fX1Pos;
    m_fY1Pos = fY1Pos;
    m_fX2Pos = fX2Pos;
    m_fY2Pos = fY2Pos;
    m_fWidth = fWidth;
    m_dwColor = dwColor;
}

// Used to render itself to the screen
void COutlineItem::Render( IDirect3DDevice8* pD3DDevice, CUDTexture* pTexture )
{
    pTexture->DrawOutline( m_fX1Pos, m_fY1Pos, m_fX2Pos, m_fY2Pos, m_fWidth, m_dwColor );
}


/*****************************************************
/* CBitmapItem()
/****************************************************/
// Constructor
CBitmapItem::CBitmapItem() :
CScreenItem(),
m_pszFilename( NULL ),
m_nXPos( 0 ),
m_nYPos( 0 )
{
}

// Destructor
CBitmapItem::~CBitmapItem()
{
    // Clean up our memory
    if( m_pszFilename )
    {
        delete[] m_pszFilename;
        m_pszFilename = NULL;
    }
}

// Allows the user to update the information for the Outline Item
void CBitmapItem::UpdateItem( char* pszFilename, int nXPos, int nYPos )
{
    // Ensure we were passed a valid pointer
    if( !pszFilename )
    {
        XDBGWRN( APP_TITLE_NAME_A, " CBitmapItem()::UpdateItem():pszFilename was invalid!!" );

        return;
    }

    // If we have already set a text item, lets clear the memory so we can allocate new memory
    if( m_pszFilename )
    {
        delete[] m_pszFilename;
        m_pszFilename = NULL;
    }

    // Create new memory and copy the string that was passed in by the user
    m_pszFilename = _strdup( pszFilename );

    // Check to see if we could allocate memory -- If not, assert!!
    if( !m_pszFilename )
    {
        XDBGWRN( APP_TITLE_NAME_A, " CBitmapItem()::UpdateItem():Couldn't allocate memory!!" );

        return;
    }

    m_nXPos = nXPos;
    m_nYPos = nYPos;

    m_Bitmap.read( m_nXPos, m_nYPos, m_pszFilename );
}

// Used to render itself to the screen
void CBitmapItem::Render( IDirect3DDevice8* pD3DDevice, CUDTexture* pTexture )
{
    m_Bitmap.render( pD3DDevice, pTexture->GetTextureSurface() );
}
