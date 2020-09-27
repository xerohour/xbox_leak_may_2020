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

/*****************************************************
/* CTextItem()
/****************************************************/
class CTextItem : public CScreenItem
{
public:
    // Constructors and Destructors
    CTextItem();
    ~CTextItem();

    // Must create this for all CScreenItem classes
    void Render( CUDTexture* pTexture );

    // Allow the user to update the item
    void UpdateItem( WCHAR* pwszText, float fXPos, float fYPos, DWORD dwFGColor, DWORD dwBGColor );

private:
    WCHAR* m_pwszText;
    float m_fXPos;
    float m_fYPos;
    DWORD m_dwFGColor;
    DWORD m_dwBGColor;
};


/*****************************************************
/* CPanelItem()
/****************************************************/
class CPanelItem : public CScreenItem
{
public:
    // Constructors and Destructors
    CPanelItem();
    ~CPanelItem();

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
/* CLineItem()
/****************************************************/
class CLineItem : public CScreenItem
{
public:
    // Constructors and Destructors
    CLineItem();
    ~CLineItem();

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
/* COutlineItem()
/****************************************************/
class COutlineItem : public CScreenItem
{
public:
    // Constructors and Destructors
    COutlineItem();
    ~COutlineItem();

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
/* CBitmapItem()
/****************************************************/
class CBitmapItem : public CScreenItem
{
public:
    // Constructors and Destructors
    CBitmapItem();
    ~CBitmapItem();

    // Must create this for all CScreenItem classes
    void Render( CUDTexture* pTexture );

    // Allow the user to update the item
    void UpdateItem( char* pszFilename, int nXPos, int nYPos );

private:
    BitmapFile m_Bitmap;

    char* m_pszFilename;
    int m_nXPos;
    int m_nYPos;
};

#endif //_SCREENITEMS_H_