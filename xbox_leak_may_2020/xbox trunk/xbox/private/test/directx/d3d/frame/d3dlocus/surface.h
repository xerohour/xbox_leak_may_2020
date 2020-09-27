/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    surface.h

Description:

    IDirect3DSurface classes.

*******************************************************************************/

#ifndef __SURFACE_H__
#define __SURFACE_H__

//******************************************************************************
// CSurface
//******************************************************************************

//******************************************************************************
class CSurface : public CObject {

protected:

public:

                        CSurface();
                        ~CSurface();
};

//******************************************************************************
class CSurface8 : public CSurface {

protected:

    LPDIRECT3DSURFACE8  m_pd3ds;
    CDevice8*           m_pDevice;

public:

                        CSurface8();
                        ~CSurface8();
    virtual BOOL        Create(CDevice8* pDevice, LPDIRECT3DSURFACE8 pd3ds);

    virtual 
    LPDIRECT3DSURFACE8  GetIDirect3DSurface8();

    // IUnknown

    virtual ULONG       AddRef();
    virtual ULONG       Release();

    // Information

    virtual HRESULT     GetContainer(REFIID riid, CBaseTexture8** ppContainer);
    virtual HRESULT     GetDevice(CDevice8** ppDevice);
    virtual HRESULT     GetDesc(D3DSURFACE_DESC* pd3dsd);

    // Locking surfaces

    virtual HRESULT     LockRect(D3DLOCKED_RECT* pd3dlr, RECT* prect, DWORD dwFlags);
    virtual HRESULT     UnlockRect();

    // Private surface data

    virtual HRESULT     GetPrivateData(REFGUID refguid, void* pvData, DWORD* pdwSizeOfData);
    virtual HRESULT     SetPrivateData(REFGUID refguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags);
    virtual HRESULT     FreePrivateData(REFGUID refguid);
};

//******************************************************************************
class CCSurface8 : public CSurface8 {

protected:

    CClient*            m_pClient;

    SLOCKDESC           m_slockd;

public:

                        CCSurface8();
                        ~CCSurface8();
    virtual BOOL        Create(CClient* pClient, CDevice8* pDevice, LPDIRECT3DSURFACE8 pd3ds);

    // IUnknown

    virtual ULONG       AddRef();
    virtual ULONG       Release();

    // Information

    virtual HRESULT     GetContainer(REFIID riid, CBaseTexture8** ppContainer);
    virtual HRESULT     GetDevice(CDevice8** ppDevice);
    virtual HRESULT     GetDesc(D3DSURFACE_DESC* pd3dsd);

    // Locking surfaces

    virtual HRESULT     LockRect(D3DLOCKED_RECT* pd3dlr, RECT* prect, DWORD dwFlags);
    virtual HRESULT     UnlockRect();

    // Private surface data

    virtual HRESULT     GetPrivateData(REFGUID refguid, void* pvData, DWORD* pdwSizeOfData);
    virtual HRESULT     SetPrivateData(REFGUID refguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags);
    virtual HRESULT     FreePrivateData(REFGUID refguid);
};

#endif // __SURFACE_H__