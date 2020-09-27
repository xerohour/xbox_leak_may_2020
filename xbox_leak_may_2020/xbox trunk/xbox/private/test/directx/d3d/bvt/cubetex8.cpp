/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    cubetex8.cpp

Description:

    IDirect3DCubeTexture8 interface BVTs.

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
// IDirect3DCubeTexture8 inteface tests
//******************************************************************************

//******************************************************************************
TESTPROCAPI TCTX8_GetLevelDesc(PTESTTABLEENTRY ptte) {

    LPDIRECT3DCUBETEXTURE8  pd3dtc;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;

    pd3dtc = GetCubeTexture8();
    if (!pd3dtc) {
        return TR_ABORT;
    }

    return TSHR8_GetLevelDesc(ptte, (LPVOID)pd3dtc);
}

//******************************************************************************
TESTPROCAPI TCTX8_LockRect(PTESTTABLEENTRY ptte) {

    LPDIRECT3DCUBETEXTURE8  pd3dtc;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;

    pd3dtc = GetCubeTexture8();
    if (!pd3dtc) {
        return TR_ABORT;
    }

    return TSHR8_LockRect(ptte, (LPVOID)pd3dtc);
}

//******************************************************************************
TESTPROCAPI TCTX8_UnlockRect(PTESTTABLEENTRY ptte) {

    LPDIRECT3DCUBETEXTURE8  pd3dtc;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;

    pd3dtc = GetCubeTexture8();
    if (!pd3dtc) {
        return TR_ABORT;
    }

    return TSHR8_UnlockRect(ptte, (LPVOID)pd3dtc);
}

#ifndef UNDER_XBOX

//******************************************************************************
TESTPROCAPI TCTX8_AddDirtyRect(PTESTTABLEENTRY ptte) {

    LPDIRECT3DCUBETEXTURE8  pd3dtc;

    pd3dtc = GetCubeTexture8();
    if (!pd3dtc) {
        return TR_ABORT;
    }

    return TSHR8_AddDirtyRect(ptte, (LPVOID)pd3dtc);
}

#endif // !UNDER_XBOX

//******************************************************************************
TESTPROCAPI TCTX8_GetCubeMapSurface(PTESTTABLEENTRY ptte) {

    LPDIRECT3DCUBETEXTURE8  pd3dtc;

    pd3dtc = GetCubeTexture8();
    if (!pd3dtc) {
        return TR_ABORT;
    }

    return TSHR8_GetSurfaceLevel(ptte, (LPVOID)pd3dtc);
}
