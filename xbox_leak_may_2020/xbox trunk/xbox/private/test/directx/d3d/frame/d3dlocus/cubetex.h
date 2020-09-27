/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    cubetex.h

Description:

    IDirect3DTexture classes.

*******************************************************************************/

#ifndef __CUBETEX_H__
#define __CUBETEX_H__

//******************************************************************************
// CBaseTexture
//******************************************************************************

//******************************************************************************
class CCubeTexture : public CBaseTexture {

protected:

public:

                        CCubeTexture();
                        ~CCubeTexture();
};

//******************************************************************************
class CCubeTexture8 : public CBaseTexture8 {

protected:

    LPDIRECT3DCUBETEXTURE8 m_pd3dtc;

public:

                        CCubeTexture8();
                        ~CCubeTexture8();
    virtual BOOL        Create(CDevice8* pDevice, LPDIRECT3DCUBETEXTURE8 pd3dtc);

    virtual LPDIRECT3DCUBETEXTURE8 GetIDirect3DCubeTexture8();

    // Information

    virtual HRESULT     GetLevelDesc(UINT uLevel, D3DSURFACE_DESC* pd3dsd);

    // Locking surfaces

    virtual HRESULT     LockRect(D3DCUBEMAP_FACES d3dcf, UINT uLevel, D3DLOCKED_RECT* pd3dlr, RECT* prect, DWORD dwFlags);
    virtual HRESULT     UnlockRect(D3DCUBEMAP_FACES d3dcf, UINT uLevel);

    // Miscellaneous

    virtual HRESULT     AddDirtyRect(D3DCUBEMAP_FACES d3dcf, RECT* prectDirty);
    virtual HRESULT     GetCubeMapSurface(D3DCUBEMAP_FACES d3dcf, UINT uLevel, CSurface8** ppCubeMapSurface);
};

//******************************************************************************
class CCCubeTexture8 : public CCubeTexture8 {

protected:

    CClient*            m_pClient;

    PSLOCKDESC          m_pslockd[6];

public:

                        CCCubeTexture8();
                        ~CCCubeTexture8();
    virtual BOOL        Create(CClient* pClient, CDevice8* pDevice, LPDIRECT3DCUBETEXTURE8 pd3dtc);

    // IUnknown

    virtual ULONG       AddRef();
    virtual ULONG       Release();

    // Resource information

    virtual 
    D3DRESOURCETYPE     GetType();
    virtual HRESULT     GetDevice(CDevice8** ppDevice);

    // Resource private surface data

    virtual HRESULT     GetPrivateData(REFGUID refguid, void* pvData, DWORD* pdwSizeOfData);
    virtual HRESULT     SetPrivateData(REFGUID refguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags);
    virtual HRESULT     FreePrivateData(REFGUID refguid);

    // Resource management

    virtual DWORD       GetPriority();
    virtual DWORD       SetPriority(DWORD dwNewPriority);
    virtual void        PreLoad();

    // Base texture information

    virtual DWORD       GetLevelCount();

    // Base texture detail

    virtual DWORD       GetLOD();
    virtual DWORD       SetLOD(DWORD dwNewLOD);

    // Information

    virtual HRESULT     GetLevelDesc(UINT uLevel, D3DSURFACE_DESC* pd3dsd);

    // Locking surfaces

    virtual HRESULT     LockRect(D3DCUBEMAP_FACES d3dcf, UINT uLevel, D3DLOCKED_RECT* pd3dlr, RECT* prect, DWORD dwFlags);
    virtual HRESULT     UnlockRect(D3DCUBEMAP_FACES d3dcf, UINT uLevel);

    // Miscellaneous

    virtual HRESULT     AddDirtyRect(D3DCUBEMAP_FACES d3dcf, RECT* prectDirty);
    virtual HRESULT     GetCubeMapSurface(D3DCUBEMAP_FACES d3dcf, UINT uLevel, CSurface8** ppCubeMapSurface);
};

#endif // __CUBETEX_H__
