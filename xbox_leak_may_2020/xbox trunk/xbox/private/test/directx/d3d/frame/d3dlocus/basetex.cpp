/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    basetex.cpp

Author:

    Matt Bronder

Description:

    CBaseTexture methods.

*******************************************************************************/

#include "d3dlocus.h"

//******************************************************************************
// CBaseTexture
//******************************************************************************

//******************************************************************************
CBaseTexture::CBaseTexture() {
}

//******************************************************************************
CBaseTexture::~CBaseTexture() {
}

//******************************************************************************
// CBaseTexture8
//******************************************************************************

//******************************************************************************
CBaseTexture8::CBaseTexture8() {

    m_pd3dtb = NULL;
}

//******************************************************************************
CBaseTexture8::~CBaseTexture8() {
}

//******************************************************************************
BOOL CBaseTexture8::Create(CDevice8* pDevice, LPDIRECT3DBASETEXTURE8 pd3dtb) {

    if (m_pd3dtb) {
        return FALSE;
    }

    if (pDevice && pd3dtb) {

        if (CResource8::Create(pDevice, (LPDIRECT3DRESOURCE8)pd3dtb)) {
            m_pd3dtb = pd3dtb;
        }
    }

    return (BOOL)m_pd3dtb;
}

//******************************************************************************
LPDIRECT3DBASETEXTURE8 CBaseTexture8::GetIDirect3DBaseTexture8() {

    return m_pd3dtb;
}

//******************************************************************************
DWORD CBaseTexture8::GetLevelCount() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtb) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dtb->GetLevelCount();
}

//******************************************************************************
DWORD CBaseTexture8::GetLOD() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtb) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dtb->GetLOD();
#else
    Log(LOG_FAIL, TEXT("IDirect3DBaseTexture8::GetLOD is not implemented on XBox"));
    return 0;
#endif // UNDER_XBOX
}

//******************************************************************************
DWORD CBaseTexture8::SetLOD(DWORD dwNewLOD) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtb) {
        return 0;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dtb->SetLOD(dwNewLOD);
#else
    Log(LOG_FAIL, TEXT("IDirect3DBaseTexture8::SetLOD is not implemented on XBox"));
    return 0;
#endif // UNDER_XBOX
}

//******************************************************************************
// CCBaseTexture8
//******************************************************************************

//******************************************************************************
DWORD CCBaseTexture8_GetLevelCount(CBaseTexture8* pBaseTexture, CClient* pClient) {

    LPDIRECT3DBASETEXTURE8  pd3dtb = pBaseTexture->GetIDirect3DBaseTexture8();
    DWORD                   dwCountC, dwCountS;

#ifdef D3DLOCUS_DEBUG
    if (!pd3dtb) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    dwCountC = pd3dtb->GetLevelCount();

    if (pClient->CallMethod(MID_BTX8_GetLevelCount, pBaseTexture, &dwCountS)) {

        if (dwCountC != dwCountS) {
            Log(LOG_WARN, TEXT("IDirect3DBaseTexture8::GetLevelCount result inconsistent [C: %d, S: %d]"), dwCountC, dwCountS);
        }
    }

    return dwCountC;
}

//******************************************************************************
DWORD CCBaseTexture8_GetLOD(CBaseTexture8* pBaseTexture, CClient* pClient) {

    LPDIRECT3DBASETEXTURE8  pd3dtb = pBaseTexture->GetIDirect3DBaseTexture8();
    DWORD                   dwLODC, dwLODS;

#ifdef D3DLOCUS_DEBUG
    if (!pd3dtb) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    dwLODC = pd3dtb->GetLOD();

    if (pClient->CallMethod(MID_BTX8_GetLOD, pBaseTexture, &dwLODS)) {

        if (dwLODC != dwLODS) {
            Log(LOG_WARN, TEXT("IDirect3DBaseTexture8::GetLOD result inconsistent [C: %d, S: %d]"), dwLODC, dwLODS);
        }
    }

    return dwLODC;
#else
    Log(LOG_FAIL, TEXT("IDirect3DBaseTexture8::GetLOD is not implemented on XBox"));
    return 0;
#endif // UNDER_XBOX
}

//******************************************************************************
DWORD CCBaseTexture8_SetLOD(CBaseTexture8* pBaseTexture, CClient* pClient, DWORD dwNewLOD) {

    LPDIRECT3DBASETEXTURE8  pd3dtb = pBaseTexture->GetIDirect3DBaseTexture8();
    DWORD                   dwLODC, dwLODS;

#ifdef D3DLOCUS_DEBUG
    if (!pd3dtb) {
        return 0;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    dwLODC = pd3dtb->SetLOD(dwNewLOD);

    if (pClient->CallMethod(MID_BTX8_SetLOD, pBaseTexture, &dwLODS, dwNewLOD)) {

        if (dwLODC != dwLODS) {
            Log(LOG_WARN, TEXT("IDirect3DBaseTexture8::SetLOD result inconsistent [C: %d, S: %d]"), dwLODC, dwLODS);
        }
    }

    return dwLODC;
#else
    Log(LOG_FAIL, TEXT("IDirect3DBaseTexture8::SetLOD is not implemented on XBox"));
    return 0;
#endif // UNDER_XBOX
}
