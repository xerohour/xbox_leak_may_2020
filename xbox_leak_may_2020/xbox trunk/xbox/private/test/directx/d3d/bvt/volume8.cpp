/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    volume8.cpp

Description:

    IDirect3DVolume8 interface BVTs.

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

#ifdef UNDER_XBOX
#define DEMO_HACK
#endif

using namespace D3DBVT;

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

//******************************************************************************
// IDirect3DVolume8 inteface tests
//******************************************************************************

//******************************************************************************
TESTPROCAPI TVOL8_GetContainer(PTESTTABLEENTRY ptte) {

    LPVOID                  pvContainer1, pvContainer2 = DEAD_PTR,
                            pvContainer3;
    LPDIRECT3DVOLUME8       pd3dv = NULL;
    ULONG                   uRef1, uRef2;
    HRESULT                 hr1, hr2;
    TRESULT                 tr = TR_PASS;

    pd3dv = GetVolume8((LPDIRECT3DVOLUMETEXTURE8*)&pvContainer3);
    if (!pd3dv) {
        return TR_ABORT;
    }

    ((IDirect3DVolumeTexture8*)pvContainer3)->AddRef();
    uRef1 = ((IDirect3DVolumeTexture8*)pvContainer3)->Release();

#ifndef UNDER_XBOX
    hr1 = pd3dv->GetContainer(IID_IDirect3DVolumeTexture8, &pvContainer1);
    hr2 = pd3dv->GetContainer(IID_IDirect3DTexture8, &pvContainer2);
#else
    hr1 = pd3dv->GetContainer((LPDIRECT3DBASETEXTURE8*)&pvContainer1);
#endif // UNDER_XBOX

    if (FAILED(hr1)) {
        Log(LOG_FAIL, TEXT("%s::GetContainer failed [0x%X]"), ptte->szInterface, hr1);
        return TR_FAIL;
    }
#ifndef UNDER_XBOX
    if (SUCCEEDED(hr2)) {
        Log(LOG_FAIL, TEXT("%s::GetContainer(IID_IDirect3DTexture8, ...) succeeded on a volume texture surface"), ptte->szInterface);
        tr = TR_FAIL;
    }

    if (pvContainer2 != NULL) {
        Log(LOG_FAIL, TEXT("%s::GetContainer failed to clean the given container pointer when given an invalid reference identifier"), ptte->szInterface);
        tr = TR_FAIL;
    }
#endif // !UNDER_XBOX

    ((IDirect3DVolumeTexture8*)pvContainer3)->AddRef();
    uRef2 = ((IDirect3DVolumeTexture8*)pvContainer3)->Release();

    if (uRef2 != uRef1 + 1) {
        Log(LOG_FAIL, TEXT("%s::GetContainer failed to increment the reference count of the container object"), ptte->szInterface);
        tr = TR_FAIL;
    }

    ((IDirect3DVolumeTexture8*)pvContainer3)->Release();

    if (pvContainer1 != pvContainer3) {
        Log(LOG_FAIL, TEXT("%s::GetContainer returned an incorrect pointer to the container object: 0x%X"), ptte->szInterface, pvContainer1);
        tr = TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TVOL8_GetDevice(PTESTTABLEENTRY ptte) {

    LPDIRECT3DVOLUME8   pd3dv;

    pd3dv = GetVolume8();
    if (!pd3dv) {
        return TR_ABORT;
    }

    return TSHR8_GetDevice(ptte, (LPVOID)pd3dv);
}

//******************************************************************************
TESTPROCAPI TVOL8_GetDesc(PTESTTABLEENTRY ptte) {

    LPDIRECT3DVOLUME8        pd3dv;
    LPDIRECT3DVOLUMETEXTURE8 pd3dtv;
    D3DVOLUME_DESC           d3dvd1, d3dvd2;
    HRESULT                  hr;
    TRESULT                  tr = TR_PASS;

    pd3dv = GetVolume8(&pd3dtv);
    if (!pd3dv) {
        return TR_ABORT;
    }

    hr = pd3dtv->GetLevelDesc(0, &d3dvd1);
    if (ResultFailed(hr, TEXT("IDirect3DVolumeTexture8::GetLevelDesc"))) {
        return TR_ABORT;
    }

    hr = pd3dv->GetDesc(&d3dvd2);
    if (ResultFailed(hr, TEXT("IDirect3DVolume8::GetDesc"))) {
        return TR_FAIL;
    }

    if (d3dvd1.Format != d3dvd2.Format ||
        d3dvd1.Usage != d3dvd2.Usage ||
        d3dvd1.Size != d3dvd2.Size ||
        d3dvd1.Width != d3dvd2.Width ||
        d3dvd1.Height != d3dvd2.Height ||
        d3dvd1.Depth != d3dvd2.Depth)
    {
        Log(LOG_FAIL, TEXT("IDirect3DVolume8::GetDesc returned an incorrect volume description"));
        tr = TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TVOL8_LockBox(PTESTTABLEENTRY ptte) {

    LPDIRECT3DVOLUME8   pd3dv;

    pd3dv = GetVolume8();
    if (!pd3dv) {
        return TR_ABORT;
    }

    return TSHR8_LockBox(ptte, (LPVOID)pd3dv);
}

//******************************************************************************
TESTPROCAPI TVOL8_UnlockBox(PTESTTABLEENTRY ptte) {

    LPDIRECT3DVOLUME8   pd3dv;

    pd3dv = GetVolume8();
    if (!pd3dv) {
        return TR_ABORT;
    }

    return TSHR8_UnlockBox(ptte, (LPVOID)pd3dv);
}

//******************************************************************************
TESTPROCAPI TVOL8_GetPrivateData(PTESTTABLEENTRY ptte) {

    LPDIRECT3DVOLUME8   pd3dv;

    pd3dv = GetVolume8();
    if (!pd3dv) {
        return TR_ABORT;
    }

    return TSHR8_GetPrivateData(ptte, (LPVOID)pd3dv);
}

//******************************************************************************
TESTPROCAPI TVOL8_SetPrivateData(PTESTTABLEENTRY ptte) {

    LPDIRECT3DVOLUME8   pd3dv;

    pd3dv = GetVolume8();
    if (!pd3dv) {
        return TR_ABORT;
    }

    return TSHR8_SetPrivateData(ptte, (LPVOID)pd3dv);
}

//******************************************************************************
TESTPROCAPI TVOL8_FreePrivateData(PTESTTABLEENTRY ptte) {

    LPDIRECT3DVOLUME8   pd3dv;

    pd3dv = GetVolume8();
    if (!pd3dv) {
        return TR_ABORT;
    }

    return TSHR8_FreePrivateData(ptte, (LPVOID)pd3dv);
}

