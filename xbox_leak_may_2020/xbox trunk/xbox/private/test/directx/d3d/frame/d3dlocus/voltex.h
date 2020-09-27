/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    voltex.h

Description:

    IDirect3DVolumeTexture classes.

*******************************************************************************/

#ifndef __VOLTEX_H__
#define __VOLTEX_H__

//******************************************************************************
// CVolumeTexture
//******************************************************************************

//******************************************************************************
class CVolumeTexture : public CBaseTexture {

protected:

public:

                        CVolumeTexture();
                        ~CVolumeTexture();
};

//******************************************************************************
class CVolumeTexture8 : public CBaseTexture8 {

protected:

    LPDIRECT3DVOLUMETEXTURE8 m_pd3dtv;

public:

                        CVolumeTexture8();
                        ~CVolumeTexture8();
    virtual BOOL        Create(CDevice8* pDevice, LPDIRECT3DVOLUMETEXTURE8 pd3dtv);

    virtual LPDIRECT3DVOLUMETEXTURE8 GetIDirect3DVolumeTexture8();

    // Information

    virtual HRESULT     GetLevelDesc(UINT uLevel, D3DVOLUME_DESC* pd3dvd);

    // Locking surfaces

    virtual HRESULT     LockBox(UINT uLevel, D3DLOCKED_BOX* pd3dlb, D3DBOX* pbox, DWORD dwFlags);
    virtual HRESULT     UnlockBox(UINT uLevel);

    // Miscellaneous

    virtual HRESULT     AddDirtyBox(D3DBOX* pboxDirty);
    virtual HRESULT     GetVolumeLevel(UINT uLevel, CVolume8** ppVolumeLevel);
};

//******************************************************************************
class CCVolumeTexture8 : public CVolumeTexture8 {

protected:

    CClient*            m_pClient;

    PVLOCKDESC          m_pvlockd;

public:

                        CCVolumeTexture8();
                        ~CCVolumeTexture8();
    virtual BOOL        Create(CClient* pClient, CDevice8* pDevice, LPDIRECT3DVOLUMETEXTURE8 pd3dtv);

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

    virtual HRESULT     GetLevelDesc(UINT uLevel, D3DVOLUME_DESC* pd3dvd);

    // Locking surfaces

    virtual HRESULT     LockBox(UINT uLevel, D3DLOCKED_BOX* pd3dlb, D3DBOX* pbox, DWORD dwFlags);
    virtual HRESULT     UnlockBox(UINT uLevel);

    // Miscellaneous

    virtual HRESULT     AddDirtyBox(D3DBOX* pboxDirty);
    virtual HRESULT     GetVolumeLevel(UINT uLevel, CVolume8** ppVolumeLevel);
};

#endif // __VOLTEX_H__
