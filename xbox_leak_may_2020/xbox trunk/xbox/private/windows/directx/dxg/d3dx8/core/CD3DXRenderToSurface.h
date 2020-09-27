///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       CD3DXRenderToSurface.h
//  Content:    Render to surface object
//
///////////////////////////////////////////////////////////////////////////

#ifndef __D3DXRENDERTOSURFACE_H__
#define __D3DXRENDERTOSURFACE_H__

class CD3DXRenderToSurface : public ID3DXRenderToSurface
{
public:
    CD3DXRenderToSurface();
   ~CD3DXRenderToSurface();

    HRESULT Init(LPDIRECT3DDEVICE8 pDevice, UINT Width, UINT Height,
        D3DFORMAT Format, BOOL DepthStencil, D3DFORMAT DepthStencilFormat);

    // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    // ID3DXRenderToSurface
    STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE8* ppDevice);
    STDMETHOD(GetDesc)(THIS_ D3DXRTS_DESC* pDesc);

    STDMETHOD(BeginScene)(THIS_ LPDIRECT3DSURFACE8 pSurface, CONST D3DVIEWPORT8* pViewport);
    STDMETHOD(EndScene)(THIS);


protected:
    UINT                m_cRef;
    LPDIRECT3DDEVICE8   m_pDevice;
    D3DXRTS_DESC        m_Desc;

    LPDIRECT3DSURFACE8  m_pColor;
    LPDIRECT3DSURFACE8  m_pColorOld;
    LPDIRECT3DSURFACE8  m_pZStencil;
    LPDIRECT3DSURFACE8  m_pZStencilOld;
    LPDIRECT3DSURFACE8  m_pSurface;

    BOOL                m_bRenderTarget;
    DWORD               m_dwStateBlock;
    D3DVIEWPORT8        m_Viewport;
};

#endif