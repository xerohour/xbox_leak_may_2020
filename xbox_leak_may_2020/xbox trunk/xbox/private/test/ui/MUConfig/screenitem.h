/*****************************************************
*** screenitem.h
***
*** Header file for our abstract base class for a
*** generic screen item.
*** 
*** by James N. Helm
*** January 13th, 2001
*** 
*****************************************************/

#ifndef _SCREENITEM_H_
#define _SCREENITEM_H_

class CScreenItem
{
public:
    // Constructors and Destructors
    CScreenItem();
    virtual ~CScreenItem();

    // Every screen item should know how to render itself
    virtual void Render( IDirect3DDevice8* pD3DDevice, CUDTexture* pTexture ) = 0;

    virtual BOOL operator < ( const CScreenItem& item ) { return FALSE; };
    virtual BOOL operator > ( const CScreenItem& item ) { return FALSE; };
    virtual BOOL operator == ( const CScreenItem& item ) { return FALSE; };

protected:
};

#endif // _SCREENITEM_H_