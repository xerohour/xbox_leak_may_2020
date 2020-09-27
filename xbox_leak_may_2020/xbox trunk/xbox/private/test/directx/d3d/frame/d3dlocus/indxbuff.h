/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    indxbuff.h

Description:

    IDirect3DIndexBuffer classes.

*******************************************************************************/

#ifndef __INDXBUFF_H__
#define __INDXBUFF_H__

//******************************************************************************
// CIndexBuffer
//******************************************************************************

//******************************************************************************
class CIndexBuffer : public CResource {

protected:

public:

                        CIndexBuffer();
                        ~CIndexBuffer();
};

//******************************************************************************
class CIndexBuffer8 : public CResource8 {

protected:

    LPDIRECT3DINDEXBUFFER8 m_pd3di;

public:

                        CIndexBuffer8();
                        ~CIndexBuffer8();
    virtual BOOL        Create(CDevice8* pDevice, LPDIRECT3DINDEXBUFFER8 pd3di);

    virtual LPDIRECT3DINDEXBUFFER8 GetIDirect3DIndexBuffer8();

    // Information

    virtual HRESULT     GetDesc(D3DINDEXBUFFER_DESC* pd3dibd);

    // Locking

    virtual HRESULT     Lock(UINT uOffsetToLock, UINT uSizeToLock, BYTE** ppdata, DWORD dwFlags);
    virtual HRESULT     Unlock();
};

//******************************************************************************
class CCIndexBuffer8 : public CIndexBuffer8 {

protected:

    CClient*            m_pClient;

    ILOCKDESC           m_ilockd;

public:

                        CCIndexBuffer8();
                        ~CCIndexBuffer8();
    virtual BOOL        Create(CClient* pClient, CDevice8* pDevice, LPDIRECT3DINDEXBUFFER8 pd3di);

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

    virtual HRESULT     GetDesc(D3DINDEXBUFFER_DESC* pd3dibd);

    // Locking

    virtual HRESULT     Lock(UINT uOffsetToLock, UINT uSizeToLock, BYTE** ppdata, DWORD dwFlags);
    virtual HRESULT     Unlock();
};

#endif // __INDXBUFF_H__
