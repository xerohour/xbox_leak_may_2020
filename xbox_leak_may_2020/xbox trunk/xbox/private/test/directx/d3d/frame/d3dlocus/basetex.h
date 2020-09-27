/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    basetex.h

Description:

    IDirect3DBaseTexture classes.

*******************************************************************************/

#ifndef __BASETEX_H__
#define __BASETEX_H__

//******************************************************************************
// CBaseTexture
//******************************************************************************

//******************************************************************************
class CBaseTexture : public CResource {

protected:

public:

                        CBaseTexture();
                        ~CBaseTexture();
};

//******************************************************************************
class CBaseTexture8 : public CResource8 {

protected:

    LPDIRECT3DBASETEXTURE8 m_pd3dtb;

public:

                        CBaseTexture8();
                        ~CBaseTexture8();
    virtual BOOL        Create(CDevice8* pDevice, LPDIRECT3DBASETEXTURE8 pd3dtb);

    virtual LPDIRECT3DBASETEXTURE8 GetIDirect3DBaseTexture8();
/*
    virtual BOOL        AddObject(LPVOID pvInterface, LPVOID pvObject);
    virtual BOOL        RemoveObject(LPVOID pvInterface);
    virtual BOOL        FindObject(LPVOID pvInterface, LPVOID* ppvObject);
*/
    // Information

    virtual DWORD       GetLevelCount();

    // Detail

    virtual DWORD       GetLOD();
    virtual DWORD       SetLOD(DWORD dwNewLOD);
};

//******************************************************************************
// Client base texture function prototypes
//******************************************************************************

DWORD CCBaseTexture8_GetLevelCount(CBaseTexture8* pBaseTexture, CClient* pClient);
DWORD CCBaseTexture8_GetLOD(CBaseTexture8* pBaseTexture, CClient* pClient);
DWORD CCBaseTexture8_SetLOD(CBaseTexture8* pBaseTexture, CClient* pClient, DWORD dwNewLOD);

#endif // __BASETEX_H__
