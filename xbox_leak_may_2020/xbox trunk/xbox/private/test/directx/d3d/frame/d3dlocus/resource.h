/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    resource.h

Description:

    IDirect3DResource classes.

*******************************************************************************/

#ifndef __RESOURCE_H__
#define __RESOURCE_H__

//******************************************************************************
// CResource
//******************************************************************************

//******************************************************************************
class CResource : public CObject {

protected:

public:

                        CResource();
                        ~CResource();
};

//******************************************************************************
class CResource8 : public CResource {

protected:

    LPDIRECT3DRESOURCE8 m_pd3dres;
    CDevice8*           m_pDevice;

public:

                        CResource8();
                        ~CResource8();
    virtual BOOL        Create(CDevice8* pDevice, LPDIRECT3DRESOURCE8 pd3dres);

    virtual 
    LPDIRECT3DRESOURCE8 GetIDirect3DResource8();
    virtual CDevice8*   GetDevice8();

    // IUnknown

    virtual ULONG       AddRef();
    virtual ULONG       Release();

    // Information

    virtual 
    D3DRESOURCETYPE     GetType();
    virtual HRESULT     GetDevice(CDevice8** ppDevice);

    // Private surface data

    virtual HRESULT     GetPrivateData(REFGUID refguid, void* pvData, DWORD* pdwSizeOfData);
    virtual HRESULT     SetPrivateData(REFGUID refguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags);
    virtual HRESULT     FreePrivateData(REFGUID refguid);

    // Resource management

    virtual DWORD       GetPriority();
    virtual DWORD       SetPriority(DWORD dwNewPriority);
    virtual void        PreLoad();

#ifdef UNDER_XBOX

    // Miscellaneous

    virtual BOOL        IsBusy();
    virtual void        BlockUntilNotBusy();
    virtual void        MoveResourceMemory(D3DMEMORY d3dmem);
    virtual void        Register(void *pvBase);

#endif
};

//******************************************************************************
// Client resource function prototypes
//******************************************************************************

ULONG           CCResource8_AddRef(CResource8* pResource, CClient* pClient);
ULONG           CCResource8_Release(CResource8* pResource, CClient* pClient);

D3DRESOURCETYPE CCResource8_GetType(CResource8* pResource, CClient* pClient);
HRESULT         CCResource8_GetDevice(CResource8* pResource, CClient* pClient, CDevice8** ppDevice);

HRESULT         CCResource8_GetPrivateData(CResource8* pResource, CClient* pClient, REFGUID refguid, void* pvData, DWORD* pdwSizeOfData);
HRESULT         CCResource8_SetPrivateData(CResource8* pResource, CClient* pClient, REFGUID refguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags);
HRESULT         CCResource8_FreePrivateData(CResource8* pResource, CClient* pClient, REFGUID refguid);

DWORD           CCResource8_GetPriority(CResource8* pResource, CClient* pClient);
DWORD           CCResource8_SetPriority(CResource8* pResource, CClient* pClient, DWORD dwNewPriority);
void            CCResource8_PreLoad(CResource8* pResource, CClient* pClient);

#endif // __RESOURCE_H__