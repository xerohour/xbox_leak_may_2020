///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       CD3DXSprite.h
//  Content:    Sprite support
//
///////////////////////////////////////////////////////////////////////////

#ifndef __CD3DXSprite_H__
#define __CD3DXSprite_H__


///////////////////////////////////////////////////////////////////////////
// CD3DXSprite //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

class CD3DXSprite : public ID3DXSprite
{
public:
    CD3DXSprite();
   ~CD3DXSprite();

    HRESULT Initialize(LPDIRECT3DDEVICE8 pDevice);

    // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    // ID3DXSprite
    STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE8* ppDevice);

    STDMETHOD(Begin)(THIS);

    STDMETHOD(Draw)(THIS_ LPDIRECT3DTEXTURE8  pSrcTexture, 
        CONST RECT* pSrcRect, CONST D3DXVECTOR2* pScaling, 
        CONST D3DXVECTOR2* pRotationCenter, FLOAT Rotation, 
        CONST D3DXVECTOR2* pTranslation, D3DCOLOR Color);

    STDMETHOD(DrawTransform)(THIS_ LPDIRECT3DTEXTURE8 pSrcTexture, 
        CONST RECT* pSrcRect, CONST D3DXMATRIX* pTransform, 
        D3DCOLOR Color);

    STDMETHOD(End)(THIS);

public:
    UINT                m_uRef;
    LPDIRECT3DDEVICE8   m_pDevice;
    BOOL                m_bBegin;
    DWORD               m_dwOldState;
    DWORD               m_dwNewState;
};

#endif //__CD3DXSprite_H__