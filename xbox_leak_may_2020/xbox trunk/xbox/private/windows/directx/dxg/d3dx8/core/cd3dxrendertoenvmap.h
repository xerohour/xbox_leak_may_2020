///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       CD3DXRenderToEnvMap.h
//  Content:    Render to surface object
//
///////////////////////////////////////////////////////////////////////////

#ifndef __D3DXRenderToEnvMap_H__
#define __D3DXRenderToEnvMap_H__

class CD3DXRenderToEnvMap : public ID3DXRenderToEnvMap
{
public:
    CD3DXRenderToEnvMap();
   ~CD3DXRenderToEnvMap();

    HRESULT Init(LPDIRECT3DDEVICE8 pDevice, UINT Size, D3DFORMAT Format, 
        BOOL DepthStencil, D3DFORMAT DepthStencilFormat);

    HRESULT Begin();
    HRESULT BeginScene(D3DCUBEMAP_FACES Face);
    HRESULT EndScene();

    HRESULT RenderSphere();
    HRESULT RenderHemisphere();
    HRESULT RenderParabolic();


    // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);


    // ID3DXRenderToEnvMap
    STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE8* ppDevice);
    STDMETHOD(GetDesc)(THIS_ D3DXRTE_DESC* pDesc);

    STDMETHOD(BeginCube)(THIS_ 
        LPDIRECT3DCUBETEXTURE8 pCubeTex);

    STDMETHOD(BeginSphere)(THIS_
        LPDIRECT3DTEXTURE8 pTex);

    STDMETHOD(BeginHemisphere)(THIS_ 
        LPDIRECT3DTEXTURE8 pTexZPos,
        LPDIRECT3DTEXTURE8 pTexZNeg);

    STDMETHOD(BeginParabolic)(THIS_ 
        LPDIRECT3DTEXTURE8 pTexZPos,
        LPDIRECT3DTEXTURE8 pTexZNeg);

    STDMETHOD(Face)(THIS_ D3DCUBEMAP_FACES Face);

    STDMETHOD(End)(THIS);


protected:
    UINT                    m_cRef;
    LPDIRECT3DDEVICE8       m_pDevice;
    D3DXRTE_DESC            m_Desc;

    LPDIRECT3DSURFACE8      m_pColor;
    LPDIRECT3DSURFACE8      m_pDepth;
    LPDIRECT3DSURFACE8      m_pColorOld;
    LPDIRECT3DSURFACE8      m_pDepthOld;
    LPDIRECT3DTEXTURE8      m_pFace[6];
    LPDIRECT3DCUBETEXTURE8  m_pCubeTex;
    LPDIRECT3DTEXTURE8      m_pTex[2];

    DWORD                   m_dwMode;
    D3DCUBEMAP_FACES        m_Face;
    BOOL                    m_bInScene;
    BOOL                    m_bRenderTarget;
    DWORD                   m_dwStateBlock;
    D3DVIEWPORT8            m_Viewport;
};

#endif