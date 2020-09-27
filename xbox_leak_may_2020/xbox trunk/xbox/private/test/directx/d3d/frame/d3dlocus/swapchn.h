/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    swapchn.h

Description:

    IDirect3DSwapChain classes.

*******************************************************************************/

#ifndef __SWAPCHN_H__
#define __SWAPCHN_H__

#ifdef UNDER_XBOX
#define DEMO_HACK
#endif

#ifndef UNDER_XBOX

//******************************************************************************
// CSwapChain
//******************************************************************************

//******************************************************************************
class CSwapChain : public CObject {

protected:

public:

                        CSwapChain();
                        ~CSwapChain();
};

//******************************************************************************
class CSwapChain8 : public CSwapChain {

protected:

    LPDIRECT3DSWAPCHAIN8 m_pd3dsw;
    CDevice8*            m_pDevice;

public:

                        CSwapChain8();
                        ~CSwapChain8();
    virtual BOOL        Create(CDevice8* pDevice, LPDIRECT3DSWAPCHAIN8 pd3dsw);

    virtual LPDIRECT3DSWAPCHAIN8 GetIDirect3DSwapChain8();

    // IUnknown

    virtual ULONG       AddRef();
    virtual ULONG       Release();

    // Presentation

    virtual HRESULT     Present(RECT* prectSrc, RECT* prectDst, HWND hDstWindowOverride, RGNDATA* prgnDirtyRegion);

    // Surface management

    virtual HRESULT     GetBackBuffer(UINT uBackBuffer, D3DBACKBUFFER_TYPE bbt, CSurface8** ppBackBuffer);
};

//******************************************************************************
class CCSwapChain8 : public CSwapChain8 {

protected:

    CClient*            m_pClient;

public:

                        CCSwapChain8();
                        ~CCSwapChain8();
    virtual BOOL        Create(CClient* pClient, CDevice8* pDevice, LPDIRECT3DSWAPCHAIN8 pd3dsw);

    // IUnknown

    virtual ULONG       AddRef();
    virtual ULONG       Release();

    // Presentation

    virtual HRESULT     Present(RECT* prectSrc, RECT* prectDst, HWND hDstWindowOverride, RGNDATA* prgnDirtyRegion);

    // Surface management

    virtual HRESULT     GetBackBuffer(UINT uBackBuffer, D3DBACKBUFFER_TYPE bbt, CSurface8** ppBackBuffer);
};

#endif // !UNDER_XBOX

#endif // __SWAPCHN_H__
