/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    surface8.cpp

Description:

    IDirect3DSurface8 interface BVTs.

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
// IDirect3DSurface8 inteface tests
//******************************************************************************

//******************************************************************************
TESTPROCAPI TSUR8_GetContainer(PTESTTABLEENTRY ptte) {

    LPVOID                  pvContainer1 = DEAD_PTR, pvContainer2 = DEAD_PTR,
                            pvContainer3;
    LPDIRECT3DSURFACE8      pd3ds = NULL;
    ULONG                   uRef1, uRef2;
    HRESULT                 hr1, hr2;
    UINT                    i;
    TRESULT                 tr = TR_PASS;

    for (i = 0; i < 2; i++) {

        if (i) {
            pd3ds = GetSurface8((LPDIRECT3DTEXTURE8*)&pvContainer3);
        }
        else {
            pd3ds = GetCubeSurface8((LPDIRECT3DCUBETEXTURE8*)&pvContainer3);
        }
        if (!pd3ds) {
            return TR_ABORT;
        }

#ifndef UNDER_XBOX

        ((IUnknown*)pvContainer3)->AddRef();
        uRef1 = ((IUnknown*)pvContainer3)->Release();

        hr1 = pd3ds->GetContainer(IID_IDirect3DTexture8, &pvContainer1);
        hr2 = pd3ds->GetContainer(IID_IDirect3DCubeTexture8, &pvContainer2);

        if (i) {

            if (FAILED(hr1)) {
                Log(LOG_FAIL, TEXT("%s::GetContainer failed [0x%X]"), ptte->szInterface, hr1);
                return TR_FAIL;
            }
            if (SUCCEEDED(hr2)) {
                Log(LOG_FAIL, TEXT("%s::GetContainer(IID_IDirect3DCubeTexture8, ...) succeeded on a texture surface"), ptte->szInterface);
                tr = TR_FAIL;
            }
        }
        else {

            if (FAILED(hr2)) {
                Log(LOG_FAIL, TEXT("%s::GetContainer failed [0x%X]"), ptte->szInterface, hr2);
                return TR_FAIL;
            }
            if (SUCCEEDED(hr1)) {
                Log(LOG_FAIL, TEXT("%s::GetContainer(IID_IDirect3DTexture8, ...) succeeded on a cube texture surface"), ptte->szInterface);
                tr = TR_FAIL;
            }
            pvContainer2 = (LPVOID)((DWORD)pvContainer1 ^ (DWORD)pvContainer2);
            pvContainer1 = (LPVOID)((DWORD)pvContainer1 ^ (DWORD)pvContainer2);
            pvContainer2 = (LPVOID)((DWORD)pvContainer1 ^ (DWORD)pvContainer2);
        }

        if (pvContainer2 != NULL) {
            Log(LOG_FAIL, TEXT("%s::GetContainer failed to clear the given container pointer when given an invalid reference identifier"), ptte->szInterface);
            tr = TR_FAIL;
        }

        ((IUnknown*)pvContainer3)->AddRef();
        uRef2 = ((IUnknown*)pvContainer3)->Release();
#else

        if (i) {
            ((IDirect3DTexture8*)pvContainer3)->AddRef();
            uRef1 = ((IDirect3DTexture8*)pvContainer3)->Release();
        }
        else {
            ((IDirect3DCubeTexture8*)pvContainer3)->AddRef();
            uRef1 = ((IDirect3DCubeTexture8*)pvContainer3)->Release();
        }

        hr1 = pd3ds->GetContainer((LPDIRECT3DBASETEXTURE8*)&pvContainer1);

        if (FAILED(hr1)) {
            Log(LOG_FAIL, TEXT("%s::GetContainer failed [0x%X]"), ptte->szInterface, hr1);
            return TR_FAIL;
        }

        if (i) {
            ((IDirect3DTexture8*)pvContainer3)->AddRef();
            uRef2 = ((IDirect3DTexture8*)pvContainer3)->Release();
        }
        else {
            ((IDirect3DCubeTexture8*)pvContainer3)->AddRef();
            uRef2 = ((IDirect3DCubeTexture8*)pvContainer3)->Release();
        }

#endif // UNDER_XBOX

        if (uRef2 != uRef1 + 1) {
            Log(LOG_FAIL, TEXT("%s::GetContainer failed to increment the reference count of the container object"), ptte->szInterface);
            tr = TR_FAIL;
        }

#ifndef UNDER_XBOX
        ((IUnknown*)pvContainer3)->Release();
#else
        ((LPDIRECT3DBASETEXTURE8)pvContainer1)->Release();
#endif // UNDER_XBOX

        if (pvContainer1 != pvContainer3) {
            Log(LOG_FAIL, TEXT("%s::GetContainer returned an incorrect pointer to the container object: 0x%X"), ptte->szInterface, pvContainer1);
            tr = TR_FAIL;
        }
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TSUR8_GetDevice(PTESTTABLEENTRY ptte) {

    LPDIRECT3DSURFACE8  pd3ds;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3ds = GetSurface8();
    if (!pd3ds) {
        return TR_ABORT;
    }

    return TSHR8_GetDevice(ptte, (LPVOID)pd3ds);
}

//******************************************************************************
TESTPROCAPI TSUR8_GetDesc(PTESTTABLEENTRY ptte) {

    LPDIRECT3DSURFACE8      pd3ds = NULL;
    D3DSURFACE_DESC         d3dsd1, d3dsd2;
    UINT                    i;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;

    for (i = 0; i < 2; i++) {

        if (i) {
            LPDIRECT3DTEXTURE8 pd3dt;
            pd3ds = GetSurface8(&pd3dt);
            if (!pd3dt) {
                return TR_ABORT;
            }
            hr = pd3dt->GetLevelDesc(0, &d3dsd1);
        }
        else {
            LPDIRECT3DCUBETEXTURE8 pd3dtc;
            pd3ds = GetCubeSurface8(&pd3dtc);
            if (!pd3dtc) {
                return TR_ABORT;
            }
            hr = pd3dtc->GetLevelDesc(0, &d3dsd1);
        }
        if (FAILED(hr) || !pd3ds) {
            return TR_ABORT;
        }

        hr = pd3ds->GetDesc(&d3dsd2);
        if (ResultFailed(hr, TEXT("IDirect3DSurface8::GetDesc"))) {
            return TR_FAIL;
        }

        if (d3dsd1.Format != d3dsd2.Format ||
            d3dsd1.Usage != d3dsd2.Usage ||
            d3dsd1.MultiSampleType != d3dsd2.MultiSampleType ||
            d3dsd1.Size != d3dsd2.Size ||
            d3dsd1.Width != d3dsd2.Width ||
            d3dsd1.Height != d3dsd2.Height)
        {
            Log(LOG_FAIL, TEXT("IDirect3DSurface8::GetDesc returned an incorrect surface description"));
            tr = TR_FAIL;
        }
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TSUR8_LockRect(PTESTTABLEENTRY ptte) {

    LPDIRECT3DSURFACE8  pd3ds;

    pd3ds = GetSurface8();
    if (!pd3ds) {
        return TR_ABORT;
    }

    return TSHR8_LockRect(ptte, (LPVOID)pd3ds);
}

//******************************************************************************
TESTPROCAPI TSUR8_UnlockRect(PTESTTABLEENTRY ptte) {

    LPDIRECT3DSURFACE8  pd3ds;

    pd3ds = GetSurface8();
    if (!pd3ds) {
        return TR_ABORT;
    }

    return TSHR8_UnlockRect(ptte, (LPVOID)pd3ds);
}

//******************************************************************************
TESTPROCAPI TSUR8_GetPrivateData(PTESTTABLEENTRY ptte) {

    LPDIRECT3DSURFACE8  pd3ds;

    pd3ds = GetSurface8();
    if (!pd3ds) {
        return TR_ABORT;
    }

    return TSHR8_GetPrivateData(ptte, (LPVOID)pd3ds);
}

//******************************************************************************
TESTPROCAPI TSUR8_SetPrivateData(PTESTTABLEENTRY ptte) {

    LPDIRECT3DSURFACE8  pd3ds;

    pd3ds = GetSurface8();
    if (!pd3ds) {
        return TR_ABORT;
    }

    return TSHR8_SetPrivateData(ptte, (LPVOID)pd3ds);
}

//******************************************************************************
TESTPROCAPI TSUR8_FreePrivateData(PTESTTABLEENTRY ptte) {

    LPDIRECT3DSURFACE8  pd3ds;

    pd3ds = GetSurface8();
    if (!pd3ds) {
        return TR_ABORT;
    }

    return TSHR8_FreePrivateData(ptte, (LPVOID)pd3ds);
}

