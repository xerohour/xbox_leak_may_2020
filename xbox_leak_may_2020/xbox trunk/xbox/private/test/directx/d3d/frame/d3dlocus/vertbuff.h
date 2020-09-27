/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    vertbuff.h

Description:

    IDirect3DVertexBuffer classes.

*******************************************************************************/

#ifndef __VERTBUFF_H__
#define __VERTBUFF_H__

//******************************************************************************
// CVertexBuffer
//******************************************************************************

//******************************************************************************
class CVertexBuffer : public CResource {

protected:

public:

                        CVertexBuffer();
                        ~CVertexBuffer();
};

//******************************************************************************
class CVertexBuffer8 : public CResource8 {

protected:

    LPDIRECT3DVERTEXBUFFER8 m_pd3dr;

public:

                        CVertexBuffer8();
                        ~CVertexBuffer8();
    virtual BOOL        Create(CDevice8* pDevice, LPDIRECT3DVERTEXBUFFER8 pd3dr);

    virtual LPDIRECT3DVERTEXBUFFER8 GetIDirect3DVertexBuffer8();

    // Information

    virtual HRESULT     GetDesc(D3DVERTEXBUFFER_DESC* pd3dvbd);

    // Locking

    virtual HRESULT     Lock(UINT uOffsetToLock, UINT uSizeToLock, BYTE** ppdata, DWORD dwFlags);
    virtual HRESULT     Unlock();
};

//******************************************************************************
class CCVertexBuffer8 : public CVertexBuffer8 {

protected:

    CClient*            m_pClient;

    RLOCKDESC           m_rlockd;

public:

                        CCVertexBuffer8();
                        ~CCVertexBuffer8();
    virtual BOOL        Create(CClient* pClient, CDevice8* pDevice, LPDIRECT3DVERTEXBUFFER8 pd3dr);

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

    // Information

    virtual HRESULT     GetDesc(D3DVERTEXBUFFER_DESC* pd3dvbd);

    // Locking

    virtual HRESULT     Lock(UINT uOffsetToLock, UINT uSizeToLock, BYTE** ppdata, DWORD dwFlags);
    virtual HRESULT     Unlock();
};

#endif // __VERTBUFF_H__
