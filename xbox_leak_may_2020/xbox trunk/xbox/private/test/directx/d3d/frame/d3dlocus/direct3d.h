/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    direct3d.h

Description:

    IDirect3D classes.

*******************************************************************************/

#ifndef __DIRECT3D_H__
#define __DIRECT3D_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#if defined(DEBUG) || defined(_DEBUG)
#define D3DLOCUS_DEBUG
#endif

#define ERR_NOTCREATED          CO_E_NOTINITIALIZED
#define ERR_LOCUSFAIL           E_FAIL

//******************************************************************************
// CDirect3D
//******************************************************************************

//******************************************************************************
class CDirect3D : public CObject {

protected:

public:

                        CDirect3D();
                        ~CDirect3D();
};

//******************************************************************************
class CDirect3D8 : public CDirect3D {

protected:

    LPDIRECT3D8         m_pd3d;

public:

                        CDirect3D8();
                        ~CDirect3D8();
    virtual BOOL        Create();

    virtual LPDIRECT3D8 GetIDirect3D8();

    // IUnknown

    virtual ULONG       AddRef();
    virtual ULONG       Release();

    // Information

    virtual UINT        GetAdapterCount();
    virtual HRESULT     GetAdapterDisplayMode(UINT uAdapter, D3DDISPLAYMODE* pd3ddm);
    virtual HRESULT     GetAdapterIdentifier(UINT uAdapter, DWORD dwFlags, D3DADAPTER_IDENTIFIER8* pd3dai);
    virtual UINT        GetAdapterModeCount(UINT uAdapter);
    virtual HMONITOR    GetAdapterMonitor(UINT uAdapter);
    virtual HRESULT     GetDeviceCaps(UINT uAdapter, D3DDEVTYPE d3ddt, D3DCAPS8* pd3dcaps);

    // Enumeration

    virtual HRESULT     EnumAdapterModes(UINT uAdapter, UINT uMode, D3DDISPLAYMODE* pd3ddm);

    // Resistration

    virtual HRESULT     RegisterSoftwareDevice(void* pfnInitialize);
    
    // Verification

    virtual HRESULT     CheckDeviceFormat(UINT uAdapter, D3DDEVTYPE d3ddt, D3DFORMAT fmtAdapter, DWORD dwUsage, D3DRESOURCETYPE d3drt, D3DFORMAT fmtCheck);
    virtual HRESULT     CheckDeviceMultiSampleType(UINT uAdapter, D3DDEVTYPE d3ddt, D3DFORMAT fmtRenderTarget, BOOL bWindowed, D3DMULTISAMPLE_TYPE d3dmst);
    virtual HRESULT     CheckDeviceType(UINT uAdapter, D3DDEVTYPE d3ddtCheck, D3DFORMAT fmtDisplayMode, D3DFORMAT fmtBackBuffer, BOOL bWindowed);
    virtual HRESULT     CheckDepthStencilMatch(UINT uAdapter, D3DDEVTYPE d3ddt, D3DFORMAT fmtAdapter, D3DFORMAT fmtrRenderTarget, D3DFORMAT fmtdDepthStencil);

    // Creation

    virtual HRESULT     CreateDevice(UINT uAdapter, D3DDEVTYPE d3ddt, HWND hWndFocus, DWORD dwBehaviorFlags, D3DPRESENT_PARAMETERS* pd3dpp, class CDevice8** ppDevice);

#ifdef UNDER_XBOX
    virtual HRESULT     SetPushBufferSize(DWORD dwPushBufferSize, DWORD dwKickOffSize);
#endif
};

class CCDirect3D8 : public CDirect3D8 {

protected:

    CClient*            m_pClient;

public:
                        CCDirect3D8();
                        ~CCDirect3D8();
    virtual BOOL        Create(CClient* pClient);

    // IUnknown

    virtual ULONG       AddRef();
    virtual ULONG       Release();

    // Information

    virtual UINT        GetAdapterCount();
    virtual HRESULT     GetAdapterDisplayMode(UINT uAdapter, D3DDISPLAYMODE* pd3ddm);
    virtual HRESULT     GetAdapterIdentifier(UINT uAdapter, DWORD dwFlags, D3DADAPTER_IDENTIFIER8* pd3dai);
    virtual UINT        GetAdapterModeCount(UINT uAdapter);
    virtual HMONITOR    GetAdapterMonitor(UINT uAdapter);
    virtual HRESULT     GetDeviceCaps(UINT uAdapter, D3DDEVTYPE d3ddt, D3DCAPS8* pd3dcaps);

    // Enumeration

    virtual HRESULT     EnumAdapterModes(UINT uAdapter, UINT uMode, D3DDISPLAYMODE* pd3ddm);

    // Resistration

    virtual HRESULT     RegisterSoftwareDevice(void* pfnInitialize);
    
    // Verification

    virtual HRESULT     CheckDeviceFormat(UINT uAdapter, D3DDEVTYPE d3ddt, D3DFORMAT fmtAdapter, DWORD dwUsage, D3DRESOURCETYPE d3drt, D3DFORMAT fmtCheck);
    virtual HRESULT     CheckDeviceMultiSampleType(UINT uAdapter, D3DDEVTYPE d3ddt, D3DFORMAT fmtRenderTarget, BOOL bWindowed, D3DMULTISAMPLE_TYPE d3dmst);
    virtual HRESULT     CheckDeviceType(UINT uAdapter, D3DDEVTYPE d3ddtCheck, D3DFORMAT fmtDisplayMode, D3DFORMAT fmtBackBuffer, BOOL bWindowed);
    virtual HRESULT     CheckDepthStencilMatch(UINT uAdapter, D3DDEVTYPE d3ddt, D3DFORMAT fmtAdapter, D3DFORMAT fmtrRenderTarget, D3DFORMAT fmtdDepthStencil);

    // Creation

    virtual HRESULT     CreateDevice(UINT uAdapter, D3DDEVTYPE d3ddt, HWND hWndFocus, DWORD dwBehaviorFlags, D3DPRESENT_PARAMETERS* pd3dpp, class CDevice8** ppDevice);
};

#endif // __DIRECT3D_H__