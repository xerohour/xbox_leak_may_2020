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

extern IDirect3DDevice8* g_pD3DDevice;  // Pointer to our Direct3D Device Object


/*****************************************************
/* CTextItem( void )
/****************************************************/

// Constructor
CTextItem::CTextItem( void ) :
CScreenItem(),
m_pFont( NULL ),
m_pwszText( NULL ),
m_fXPos( 0.0f ),
m_fYPos( 0.0f ),
m_dwFGColor( 0L ),
m_dwBGColor( 0L )
{
}

// Destructor
CTextItem::~CTextItem( void )
{
    // Clean up our allocated memory
    if( m_pwszText )
    {
        delete[] m_pwszText;
        m_pwszText = NULL;
    }
}

// Allows the user to update the information for the Text Item
void CTextItem::UpdateItem( XFONT* pFont, WCHAR* pwszText, float fXPos, float fYPos, DWORD dwFGColor, DWORD dwBGColor )
{
    // Ensure we were passed a valid pointer
    if( NULL == pwszText || NULL == pFont )
    {
        DebugPrint( "CTextItem()::UpdateItem():pwszText was invalid!!\n" );

        return;
    }

    // If we have already set a text item, lets clear the memory so we can allocate new memory
    if( m_pwszText )
    {
        delete[] m_pwszText;
        m_pwszText = NULL;
    }

    // Create new memory and copy the string that was passed in by the user
    int iStrLen = wcslen( pwszText );
    m_pwszText = new WCHAR[iStrLen + 1];
    if( !m_pwszText )
    {
        // Check to see if we could allocate memory -- If not, assert!!
        DebugPrint( "CTextItem()::UpdateItem():Couldn't allocate memory!!\n" );
    }
    else
    {
        ZeroMemory( m_pwszText, sizeof( WCHAR ) * (iStrLen + 1) );
        wcscpy( m_pwszText, pwszText );
    }
   
    // Set our other parameters
    m_pFont = pFont;
    m_fXPos = fXPos;
    m_fYPos = fYPos;
    m_dwFGColor = dwFGColor;
    m_dwBGColor = dwBGColor;
}


// Used to render itself to the screen
void CTextItem::Render( CUDTexture* pTexture )
{
    pTexture->DrawText( m_pFont, m_fXPos, m_fYPos, m_dwFGColor, m_dwBGColor, L"%s", m_pwszText );
}

/*****************************************************
/* CPanelItem( void )
/****************************************************/

// Constructor
CPanelItem::CPanelItem( void ) :
CScreenItem(),
m_fX1Pos( 0.0f ),
m_fY1Pos( 0.0f ),
m_fX2Pos( 0.0f ),
m_fY2Pos( 0.0f ),
m_dwColor( 0L )
{
}

// Destructor
CPanelItem::~CPanelItem( void )
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
void CPanelItem::Render( CUDTexture* pTexture )
{
    pTexture->DrawBox( m_fX1Pos, m_fY1Pos, m_fX2Pos, m_fY2Pos, m_dwColor );
}


/*****************************************************
/* CLineItem( void )
/****************************************************/
// Constructor
CLineItem::CLineItem( void ) :
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
CLineItem::~CLineItem( void )
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
void CLineItem::Render( CUDTexture* pTexture )
{
    pTexture->DrawLine( m_fX1Pos, m_fY1Pos, m_fX2Pos, m_fY2Pos, m_fWidth, m_dwColor );
}


/*****************************************************
/* COutlineItem( void )
/****************************************************/
// Constructor
COutlineItem::COutlineItem( void ) :
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
COutlineItem::~COutlineItem( void )
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
void COutlineItem::Render( CUDTexture* pTexture )
{
    pTexture->DrawOutline( m_fX1Pos, m_fY1Pos, m_fX2Pos, m_fY2Pos, m_fWidth, m_dwColor );
}


/*****************************************************
/* CBitmapItem( void )
/****************************************************/
// Constructor
CBitmapItem::CBitmapItem( void ) :
CScreenItem(),
m_pszFilename( NULL ),
m_nXPos( 0 ),
m_nYPos( 0 )
{
}

// Destructor
CBitmapItem::~CBitmapItem( void )
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
        DebugPrint( "CBitmapItem()::UpdateItem():pszFilename was invalid!!\n" );

        return;
    }

    // If we have already set a text item, lets clear the memory so we can allocate new memory
    if( m_pszFilename )
    {
        delete[] m_pszFilename;
        m_pszFilename = NULL;
    }

    // Create new memory and copy the string that was passed in by the user
    int iStrLen = strlen( pszFilename );
    m_pszFilename = new char[iStrLen + 1];
    if( !m_pszFilename )
    {
        // Check to see if we could allocate memory -- If not, assert!!
        DebugPrint( "CBitmapItem()::UpdateItem():Couldn't allocate memory!!\n" );

        return;
    }
    else
    {
        ZeroMemory( m_pszFilename, iStrLen + 1 );
        strcpy( m_pszFilename, pszFilename );
    }

    m_nXPos = nXPos;
    m_nYPos = nYPos;

    m_Bitmap.read( m_nXPos, m_nYPos, m_pszFilename );
}

// Used to render itself to the screen
void CBitmapItem::Render( CUDTexture* pTexture )
{
    m_Bitmap.render( g_pD3DDevice, pTexture->GetTextureSurface() );
}


/*****************************************************
/* CBitmapAlphaItem( void )
/****************************************************/
// Constructor
CBitmapAlphaItem::CBitmapAlphaItem( void ) :
m_pszFilename( NULL ),
m_pszAlphaFilename( NULL ),
m_nXPos( 0 ),
m_nYPos( 0 )
{
}

// Destructor
CBitmapAlphaItem::~CBitmapAlphaItem( void )
{
    // Clean up our memory
    if( NULL != m_pszFilename )
    {
        delete[] m_pszFilename;
        m_pszFilename = NULL;
    }

    if( NULL != m_pszAlphaFilename )
    {
        delete[] m_pszAlphaFilename;
        m_pszAlphaFilename = NULL;
    }
}

// Allows the user to update the information for the Outline Item
void CBitmapAlphaItem::UpdateItem( char* pszFilename, char* pszAlphaFilename, int nXPos, int nYPos )
{
    // Ensure we were passed a valid pointer
    if( NULL == pszFilename || NULL == pszAlphaFilename )
    {
        DebugPrint( "CBitmapItem()::UpdateItem():pszFilename was invalid!!\n" );

        return;
    }

    // If we have already set a text item, lets clear the memory so we can allocate new memory
    if( NULL != m_pszFilename )
    {
        delete[] m_pszFilename;
        m_pszFilename = NULL;
    }

    if( NULL != m_pszAlphaFilename )
    {
        delete[] m_pszAlphaFilename;
        m_pszAlphaFilename = NULL;
    }

    // Create new memory and copy the string that was passed in by the user
    m_pszFilename = new char[strlen( pszFilename ) + 1];
    if( !m_pszFilename )
    {
        // Check to see if we could allocate memory -- If not, assert!!
        DebugPrint( "CBitmapItem()::UpdateItem():Couldn't allocate memory!!\n" );

        return;
    }
    else
    {
        strcpy( m_pszFilename, pszFilename );
    }

    m_pszAlphaFilename = new char[strlen( pszAlphaFilename ) + 1];
    if( NULL == m_pszAlphaFilename )
    {
        // Check to see if we could allocate memory -- If not, assert!!
        DebugPrint( "CBitmapItem()::UpdateItem():Couldn't allocate memory!!\n" );

        return;
    }
    else
    {
        strcpy( m_pszAlphaFilename, pszAlphaFilename );
    }

    m_nXPos = nXPos;
    m_nYPos = nYPos;

    m_Bitmap.readWithAlpha( m_nXPos, m_nYPos, m_pszFilename, m_pszAlphaFilename );
}

// Used to render itself to the screen
void CBitmapAlphaItem::Render( CUDTexture* pTexture )
{
    m_Bitmap.render( g_pD3DDevice, pTexture->GetTextureSurface() );
}


/******************************
*** Menu Item Class
******************************/
CMenuItem::CMenuItem( void )
: m_pFont( NULL ),
m_pValueFont( NULL ),
m_bHighlighted( FALSE ),
m_bEnabled( TRUE ),
m_nItemValue( 0 )
{
    ZeroMemory( m_pwszItemName, sizeof( WCHAR ) * 256 );
    ZeroMemory( m_pwszItemValue, sizeof( WCHAR ) * 256 );
};

CMenuItem::~CMenuItem( void )
{
}