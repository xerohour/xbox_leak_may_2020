/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    tex.h

Description:

    IDirect3DTexture classes.

*******************************************************************************/

#ifndef __TEX_H__
#define __TEX_H__

//******************************************************************************
// CBaseTexture
//******************************************************************************

//******************************************************************************
class CTexture : public CBaseTexture {

protected:

public:

                        CTexture();
                        ~CTexture();
};

//******************************************************************************
class CTexture8 : public CBaseTexture8 {

protected:

    LPDIRECT3DTEXTURE8  m_pd3dt;

public:

                        CTexture8();
                        ~CTexture8();
    virtual BOOL        Create(CDevice8* pDevice, LPDIRECT3DTEXTURE8 pd3dt);

    virtual 
    LPDIRECT3DTEXTURE8  GetIDirect3DTexture8();

    // Information

    virtual HRESULT     GetLevelDesc(UINT uLevel, D3DSURFACE_DESC* pd3dsd);

    // Locking surfaces

    virtual HRESULT     LockRect(UINT uLevel, D3DLOCKED_RECT* pd3dlr, RECT* prect, DWORD dwFlags);
    virtual HRESULT     UnlockRect(UINT uLevel);

    // Miscellaneous

    virtual HRESULT     AddDirtyRect(RECT* prectDirty);
    virtual HRESULT     GetSurfaceLevel(UINT uLevel, CSurface8** ppSurfaceLevel);
};

//******************************************************************************
class CCTexture8 : public CTexture8 {

protected:

    CClient*            m_pClient;

    PSLOCKDESC          m_pslockd;

public:

                        CCTexture8();
                        ~CCTexture8();
    virtual BOOL        Create(CClient* pClient, CDevice8* pDevice, LPDIRECT3DTEXTURE8 pd3dt);

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

    virtual HRESULT     LockRect(UINT uLevel, D3DLOCKED_RECT* pd3dlr, RECT* prect, DWORD dwFlags);
    virtual HRESULT     UnlockRect(UINT uLevel);

    // Miscellaneous

    virtual HRESULT     AddDirtyRect(RECT* prectDirty);
    virtual HRESULT     GetSurfaceLevel(UINT uLevel, CSurface8** ppSurfaceLevel);
};

#endif // __TEX_H__
