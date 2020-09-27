/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    volume.h

Description:

    IDirect3DSurface classes.

*******************************************************************************/

#ifndef __VOLUME_H__
#define __VOLUME_H__

//******************************************************************************
// CVolume
//******************************************************************************

//******************************************************************************
class CVolume : public CObject {

protected:

public:

                        CVolume();
                        ~CVolume();
};

//******************************************************************************
class CVolume8 : public CVolume {

protected:

    LPDIRECT3DVOLUME8   m_pd3dv;
    CDevice8*           m_pDevice;

public:

                        CVolume8();
                        ~CVolume8();
    virtual BOOL        Create(CDevice8* pDevice, LPDIRECT3DVOLUME8 pd3ds);

    virtual 
    LPDIRECT3DVOLUME8   GetIDirect3DVolume8();

    // IUnknown

    virtual ULONG       AddRef();
    virtual ULONG       Release();

    // Information

    virtual HRESULT     GetContainer(REFIID riid, CVolumeTexture8** ppContainer);
    virtual HRESULT     GetDevice(CDevice8** ppDevice);
    virtual HRESULT     GetDesc(D3DVOLUME_DESC* pd3dvd);

    // Locking surfaces

    virtual HRESULT     LockBox(D3DLOCKED_BOX* pd3dlb, D3DBOX* pbox, DWORD dwFlags);
    virtual HRESULT     UnlockBox();

    // Private surface data

    virtual HRESULT     GetPrivateData(REFGUID refguid, void* pvData, DWORD* pdwSizeOfData);
    virtual HRESULT     SetPrivateData(REFGUID refguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags);
    virtual HRESULT     FreePrivateData(REFGUID refguid);
};

//******************************************************************************
class CCVolume8 : public CVolume8 {

protected:

    CClient*            m_pClient;

    VLOCKDESC           m_vlockd;

public:

                        CCVolume8();
                        ~CCVolume8();
    virtual BOOL        Create(CClient* pClient, CDevice8* pDevice, LPDIRECT3DVOLUME8 pd3ds);

    // IUnknown

    virtual ULONG       AddRef();
    virtual ULONG       Release();

    // Information

    virtual HRESULT     GetContainer(REFIID riid, CVolumeTexture8** ppContainer);
    virtual HRESULT     GetDevice(CDevice8** ppDevice);
    virtual HRESULT     GetDesc(D3DVOLUME_DESC* pd3dvd);

    // Locking surfaces

    virtual HRESULT     LockBox(D3DLOCKED_BOX* pd3dlb, D3DBOX* pbox, DWORD dwFlags);
    virtual HRESULT     UnlockBox();

    // Private surface data

    virtual HRESULT     GetPrivateData(REFGUID refguid, void* pvData, DWORD* pdwSizeOfData);
    virtual HRESULT     SetPrivateData(REFGUID refguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags);
    virtual HRESULT     FreePrivateData(REFGUID refguid);
};

#endif // __VOLUME_H__