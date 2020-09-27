/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    basetex8.cpp

Description:

    IDirect3DBaseTexture8 interface BVTs.

*******************************************************************************/

#ifndef UNDER_XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif // UNDER_XBOX
#include <tchar.h>
#include <d3dx8.h>
#include "d3dbvt.h"
#include "log.h"

using namespace D3DBVT;

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

//******************************************************************************
// IDirect3DBaseTexture8 helper functions
//******************************************************************************

//******************************************************************************
LPDIRECT3DBASETEXTURE8 GetBaseTexture8(D3DIFACETYPE d3dift) {

    LPDIRECT3DBASETEXTURE8 pd3dtb = NULL;

    switch (d3dift) {

        case ITEXTURE8:
            pd3dtb = (LPDIRECT3DBASETEXTURE8)GetTexture8();
            break;
        case ICUBETEXTURE8:
            pd3dtb = (LPDIRECT3DBASETEXTURE8)GetCubeTexture8();
            break;
        case IVOLUMETEXTURE8:
            pd3dtb = (LPDIRECT3DBASETEXTURE8)GetVolumeTexture8();
            break;
    }

    return pd3dtb;
}

//******************************************************************************
// IDirect3DBaseTexture8 inteface tests
//******************************************************************************

//******************************************************************************
TESTPROCAPI TBTX8_GetLevelCount(PTESTTABLEENTRY ptte) {

    LPDIRECT3DBASETEXTURE8  pd3dtb;
    UINT                    uLevels;
    DWORD                   dwLevels;
    TRESULT                 tr = TR_PASS;

    pd3dtb = GetBaseTexture8(ptte->dwData);
    if (!pd3dtb) {
        return TR_ABORT;
    }

    for (uLevels = 1; 0x1 << (uLevels - 1) != TEXTUREDIM; uLevels++);

    dwLevels = pd3dtb->GetLevelCount();
    if (dwLevels != uLevels) {
        Log(LOG_FAIL, TEXT("%s::GetLevelCount returned an incorrect number of levels: %d"), ptte->szInterface, dwLevels);
        tr = TR_FAIL;
    }

    return tr;
}

#ifndef UNDER_XBOX

//******************************************************************************
TESTPROCAPI TBTX8_GetLOD(PTESTTABLEENTRY ptte) {

    LPDIRECT3DBASETEXTURE8  pd3dtb;
    DWORD                   dwLOD;
    TRESULT                 tr = TR_PASS;

    pd3dtb = GetBaseTexture8(ptte->dwData);
    if (!pd3dtb) {
        return TR_ABORT;
    }

    dwLOD = pd3dtb->GetLOD();
    if (dwLOD != 0) {
        Log(LOG_FAIL, TEXT("%s::GetLOD returned a level of detail of %d for a nonmanaged texture"), ptte->szInterface, dwLOD);
        tr = TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TBTX8_SetLOD(PTESTTABLEENTRY ptte) {

    LPDIRECT3DBASETEXTURE8  pd3dtb;
    UINT                    uLevels;
    DWORD                   dwLOD;
    TRESULT                 tr = TR_PASS;

    pd3dtb = GetBaseTexture8(ptte->dwData);
    if (!pd3dtb) {
        return TR_ABORT;
    }

    for (uLevels = 1; 0x1 << (uLevels - 1) != TEXTUREDIM; uLevels++);

    pd3dtb->SetLOD(rand() % uLevels);

    dwLOD = pd3dtb->SetLOD(rand() % uLevels);
    if (dwLOD != 0) {
        Log(LOG_FAIL, TEXT("%s::SetLOD returned a level of detail of %d for a nonmanaged resource"), ptte->szInterface, dwLOD);
        tr = TR_FAIL;
    }

    return tr;
}

#endif // !UNDER_XBOX