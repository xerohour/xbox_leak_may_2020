/*****************************************************
*** screenitems.h
***
*** Header file for our screen item classes.
*** This file will contain the definition for all
*** types of screen items
*** 
*** by James N. Helm
*** January 13th, 2001
*** 
*****************************************************/

#ifndef _SCREENITEMS_H_
#define _SCREENITEMS_H_

#include "screenitem.h"
#include "bitmapfile.h"

/*****************************************************
/* CTextItem( void )
/****************************************************/
class CTextItem : public CScreenItem
{
public:
    // Constructors and Destructors
    CTextItem( void );
    ~CTextItem( void );

    // Must create this for all CScreenItem classes
    void Render( CUDTexture* pTexture );

    // Allow the user to update the item
    void UpdateItem( XFONT* pFont, WCHAR* pwszText, float fXPos, float fYPos, DWORD dwFGColor, DWORD dwBGColor );

private:
    XFONT* m_pFont;
    WCHAR* m_pwszText;
    float m_fXPos;
    float m_fYPos;
    DWORD m_dwFGColor;
    DWORD m_dwBGColor;
};


/*****************************************************
/* CPanelItem( void )
/****************************************************/
class CPanelItem : public CScreenItem
{
public:
    // Constructors and Destructors
    CPanelItem( void );
    ~CPanelItem( void );

    // Must create this for all CScreenItem classes
    void Render( CUDTexture* pTexture );

    // Allow the user to update the item
    void UpdateItem( float fX1Pos, float fY1Pos, float fX2Pos, float fY2Pos, DWORD dwColor );

private:
        float m_fX1Pos;
        float m_fY1Pos;
        float m_fX2Pos;
        float m_fY2Pos;
        DWORD m_dwColor;
};


/*****************************************************
/* CLineItem( void )
/****************************************************/
class CLineItem : public CScreenItem
{
public:
    // Constructors and Destructors
    CLineItem( void );
    ~CLineItem( void );

    // Must create this for all CScreenItem classes
    void Render( CUDTexture* pTexture );

    // Allow the user to update the item
    void UpdateItem( float fX1Pos, float fY1Pos, float fX2Pos, float fY2Pos, float fWidth, DWORD dwColor );

private:
        float m_fX1Pos;
        float m_fY1Pos;
        float m_fX2Pos;
        float m_fY2Pos;
        float m_fWidth;
        DWORD m_dwColor;
};


/*****************************************************
/* COutlineItem( void )
/****************************************************/
class COutlineItem : public CScreenItem
{
public:
    // Constructors and Destructors
    COutlineItem( void );
    ~COutlineItem( void );

    // Must create this for all CScreenItem classes
    void Render( CUDTexture* pTexture );

    // Allow the user to update the item
    void UpdateItem( float fX1Pos, float fY1Pos, float fX2Pos, float fY2Pos, float fWidth, DWORD dwColor );

private:
        float m_fX1Pos;
        float m_fY1Pos;
        float m_fX2Pos;
        float m_fY2Pos;
        float m_fWidth;
        DWORD m_dwColor;
};


/*****************************************************
/* CBitmapItem( void )
/****************************************************/
class CBitmapItem : public CScreenItem
{
public:
    // Constructors and Destructors
    CBitmapItem( void );
    ~CBitmapItem( void );

    // Must create this for all CScreenItem classes
    void Render( CUDTexture* pTexture );

    // Allow the user to update the item
    void UpdateItem( char* pszFilename, int nXPos, int nYPos );
	int GetBitmapHeight( void ) { return m_Bitmap.GetHeight(); };
	int GetBitmapWidth( void ) { return m_Bitmap.GetWidth(); };

private:
    BitmapFile m_Bitmap;

    char* m_pszFilename;
    int m_nXPos;
    int m_nYPos;
};


/*****************************************************
/* CBitmapAlphaItem( void )
/****************************************************/
class CBitmapAlphaItem : public CScreenItem
{
public:
    // Constructors and Destructors
    CBitmapAlphaItem( void );
    ~CBitmapAlphaItem( void );

    // Must create this for all CScreenItem classes
    void Render( CUDTexture* pTexture );

    // Allow the user to update the item
    void UpdateItem( char* pszFilename, char* pszAlphaFilename, int nXPos, int nYPos );
	int GetBitmapHeight( void ) { return m_Bitmap.GetHeight(); };
	int GetBitmapWidth( void ) { return m_Bitmap.GetWidth(); };

private:
    BitmapFile m_Bitmap;

    char* m_pszFilename;
    char* m_pszAlphaFilename;
    int m_nXPos;
    int m_nYPos;
};


/*****************************************************
/* CMenuItem( void )
/****************************************************/
class CMenuItem
{
public:
    // Constructors and Destructors
    CMenuItem( void );
    ~CMenuItem( void );

    XFONT* m_pFont;             // The font that will be used to render this menu item
    WCHAR m_pwszItemName[256];  // The text representation of the current menu item
    XFONT* m_pValueFont;        // The font that will be used to render the item value
    WCHAR m_pwszItemValue[256]; // The text representation of the current menu item value (can be NULL)
    BOOL m_bHighlighted;        // Used to determine if the current menu item is highlighted
    BOOL m_bEnabled;            // Used to determine if the current menu item is enabled
    int m_nItemValue;           // Used to determine which item is selected (Should be unique for items within the same menu)

    // Operators
    BOOL operator < ( const CMenuItem& item ) { return FALSE; };
    BOOL operator > ( const CMenuItem& item ) { return FALSE; };
    BOOL operator == ( const CMenuItem& item ) { return FALSE; };

private:
};

#endif //_SCREENITEMS_H_