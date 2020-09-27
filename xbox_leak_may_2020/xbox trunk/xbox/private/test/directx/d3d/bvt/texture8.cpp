/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    texture8.cpp

Description:

    IDirect3DTexture8 interface BVTs.

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
// IDirect3DTexture8 inteface tests
//******************************************************************************

//******************************************************************************
TESTPROCAPI TTEX8_GetLevelDesc(PTESTTABLEENTRY ptte) {

    LPDIRECT3DTEXTURE8  pd3dt;

    pd3dt = GetTexture8();
    if (!pd3dt) {
        return TR_ABORT;
    }

    return TSHR8_GetLevelDesc(ptte, (LPVOID)pd3dt);
}

//******************************************************************************
TESTPROCAPI TTEX8_LockRect(PTESTTABLEENTRY ptte) {

    LPDIRECT3DTEXTURE8  pd3dt;

    pd3dt = GetTexture8();
    if (!pd3dt) {
        return TR_ABORT;
    }

    return TSHR8_LockRect(ptte, (LPVOID)pd3dt);
}

//******************************************************************************
TESTPROCAPI TTEX8_UnlockRect(PTESTTABLEENTRY ptte) {

    LPDIRECT3DTEXTURE8  pd3dt;

    pd3dt = GetTexture8();
    if (!pd3dt) {
        return TR_ABORT;
    }

    return TSHR8_UnlockRect(ptte, (LPVOID)pd3dt);
}

#ifndef UNDER_XBOX

//******************************************************************************
TESTPROCAPI TTEX8_AddDirtyRect(PTESTTABLEENTRY ptte) {

    LPDIRECT3DTEXTURE8  pd3dt;

    pd3dt = GetTexture8();
    if (!pd3dt) {
        return TR_ABORT;
    }

    return TSHR8_AddDirtyRect(ptte, (LPVOID)pd3dt);
}

#endif // !UNDER_XBOX

//******************************************************************************
TESTPROCAPI TTEX8_GetSurfaceLevel(PTESTTABLEENTRY ptte) {

    LPDIRECT3DTEXTURE8  pd3dt;

    pd3dt = GetTexture8();
    if (!pd3dt) {
        return TR_ABORT;
    }

    return TSHR8_GetSurfaceLevel(ptte, (LPVOID)pd3dt);
}

