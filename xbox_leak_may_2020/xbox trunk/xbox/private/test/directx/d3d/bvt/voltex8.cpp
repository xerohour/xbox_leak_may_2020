/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    voltex8.cpp

Description:

    IDirect3DVolumeTexture8 interface BVTs.

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
// IDirect3DVolumeTexture8 inteface tests
//******************************************************************************

//******************************************************************************
TESTPROCAPI TVTX8_GetLevelDesc(PTESTTABLEENTRY ptte) {

    LPDIRECT3DVOLUMETEXTURE8    pd3dtv;
    D3DVOLUME_DESC              d3dvd;
    UINT                        uLevels;
    UINT                        uLength;
    UINT                        i;
    HRESULT                     hr;
    TRESULT                     tr = TR_PASS;

    pd3dtv = GetVolumeTexture8();
    if (!pd3dtv) {
        return TR_ABORT;
    }

    for (uLevels = 1; 0x1 << (uLevels - 1) != TEXTUREDIM; uLevels++);

    for (i = 0; i < uLevels; i++) {

        LogBeginVariation(TEXT("%s::GetLevelDesc on volume texture level %d"), ptte->szInterface, i);

        hr = pd3dtv->GetLevelDesc(i, &d3dvd);
        if (ResultFailed(hr, TEXT("IDirect3DVolumeTexture8::GetLevelDesc"))) {
            return TR_FAIL;
        }

        uLength = (0x1 << ((uLevels - 1) - i));

        if (d3dvd.Width != uLength) {
            Log(LOG_FAIL, TEXT("%s::GetLevelDesc returned an incorrect width of %d instead of %d for level %d"), ptte->szInterface, d3dvd.Width, uLength, i);
            tr = TR_FAIL;
        }

        if (d3dvd.Height != uLength) {
            Log(LOG_FAIL, TEXT("%s::GetLevelDesc returned an incorrect height of %d instead of %d for level %d"), ptte->szInterface, d3dvd.Height, uLength, i);
            tr = TR_FAIL;
        }

        if (d3dvd.Depth != uLength) {
            Log(LOG_FAIL, TEXT("%s::GetLevelDesc returned an incorrect depth of %d instead of %d for level %d"), ptte->szInterface, d3dvd.Depth, uLength, i);
            tr = TR_FAIL;
        }

        if (d3dvd.Size < uLength * uLength * 2) {
            Log(LOG_FAIL, TEXT("%s::GetLevelDesc returned an incorrect size of %d for level %d"), ptte->szInterface, d3dvd.Size, i);
            tr = TR_FAIL;
        }

#ifndef UNDER_XBOX

        if (d3dvd.Pool != D3DPOOL_MANAGED) {
            Log(LOG_FAIL, TEXT("%s::GetLevelDesc returned an incorrect pool for level %d"), ptte->szInterface, i);
            tr = TR_FAIL;
        }

#endif

        if (d3dvd.Usage != 0) {
            Log(LOG_FAIL, TEXT("%s::GetLevelDesc returned an incorrect usage for level %d"), ptte->szInterface, i);
            tr = TR_FAIL;
        }

        if (d3dvd.Type != D3DRTYPE_VOLUMETEXTURE) {
            Log(LOG_FAIL, TEXT("%s::GetLevelDesc returned an incorrect type for level %d"), ptte->szInterface, i);
            tr = TR_FAIL;
        }

        if (d3dvd.Format != D3DFMT_A1R5G5B5) {
            Log(LOG_FAIL, TEXT("%s::GetLevelDesc returned an incorrect format for level %d"), ptte->szInterface, i);
            tr = TR_FAIL;
        }

        LogEndVariation();
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TVTX8_LockBox(PTESTTABLEENTRY ptte) {

    LPDIRECT3DVOLUMETEXTURE8 pd3dtv;

    pd3dtv = GetVolumeTexture8();
    if (!pd3dtv) {
        return TR_ABORT;
    }

    return TSHR8_LockBox(ptte, (LPVOID)pd3dtv);
}

//******************************************************************************
TESTPROCAPI TVTX8_UnlockBox(PTESTTABLEENTRY ptte) {

    LPDIRECT3DVOLUMETEXTURE8 pd3dtv;

    pd3dtv = GetVolumeTexture8();
    if (!pd3dtv) {
        return TR_ABORT;
    }

    return TSHR8_UnlockBox(ptte, (LPVOID)pd3dtv);
}

#ifndef UNDER_XBOX

//******************************************************************************
TESTPROCAPI TVTX8_AddDirtyBox(PTESTTABLEENTRY ptte) {

    LPDIRECT3DVOLUMETEXTURE8 pd3dtv;
    D3DBOX                   box, *pbox[2];
    UINT                     i;
    HRESULT                  hr;
    TRESULT                  tr = TR_PASS;

    pd3dtv = GetVolumeTexture8();
    if (!pd3dtv) {
        return TR_ABORT;
    }

    box.Left = 11;
    box.Top = 9;
    box.Front = 21;
    box.Right = 12;
    box.Bottom = 17;
    box.Back = 37;
    pbox[0] = NULL;
    pbox[1] = &box;

    for (i = 0; i < 2; i++) {

        LogBeginVariation(TEXT("IDirect3DVolumeTexture8::AddDirtyBox on a %sbox"), i ? TEXT("sub") : TEXT("NULL "));

        hr = pd3dtv->AddDirtyBox(pbox[i]);
        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("%s::AddDirtyBox failed [0x%X]"), ptte->szInterface, hr);
            tr = TR_FAIL;
        }

        LogEndVariation();
    }

    return tr;
}

#endif // !UNDER_XBOX

//******************************************************************************
TESTPROCAPI TVTX8_GetVolumeLevel(PTESTTABLEENTRY ptte) {

    LPDIRECT3DVOLUMETEXTURE8    pd3dtv;
    LPDIRECT3DVOLUME8           pd3dv, pd3dv2;
    D3DVOLUME_DESC              d3dvd;
    ULONG                       uRef1, uRef2;
    UINT                        uLevels;
    UINT                        uLength;
    UINT                        i;
    HRESULT                     hr;
    TRESULT                     tr = TR_PASS;

    pd3dtv = GetVolumeTexture8();
    if (!pd3dtv) {
        return TR_ABORT;
    }

    for (uLevels = 1; 0x1 << (uLevels - 1) != TEXTUREDIM; uLevels++);

    for (i = 0; i < uLevels; i++) {

        LogBeginVariation(TEXT("IDirect3DVolumeTexture8::GetVolumeLevel on volume level %d"), i);

        hr = pd3dtv->GetVolumeLevel(i, &pd3dv);
        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("%s::%s failed for level %d [0x%X]"), ptte->szInterface, ptte->szMethod, i, hr);
            return TR_FAIL;
        }

        pd3dv->AddRef();
        uRef1 = pd3dv->Release();

        hr = pd3dtv->GetVolumeLevel(i, &pd3dv2);
        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("%s::%s failed for level %d [0x%X]"), ptte->szInterface, ptte->szMethod, i, hr);
            pd3dv->Release();
            return TR_FAIL;
        }

#ifndef UNDER_XBOX
        if (pd3dv2 != pd3dv) {
            Log(LOG_FAIL, TEXT("%s::%s returned an incorrect volume pointer for level %d"), ptte->szInterface, ptte->szMethod, i);
            tr = TR_FAIL;
        }
#endif // !UNDER_XBOX

        pd3dv->AddRef();
        uRef2 = pd3dv->Release();

#ifndef UNDER_XBOX
        if (uRef2 != uRef1 + 1) {
            Log(LOG_FAIL, TEXT("%s::%s failed to increment the reference count of the volume for level %d"), ptte->szInterface, ptte->szMethod, i);
        }
#endif // !UNDER_XBOX

        pd3dv2->Release();

        hr = pd3dv->GetDesc(&d3dvd);
        pd3dv->Release();

        if (ResultFailed(hr, TEXT("IDirect3DVolume8::GetDesc"))) {
            tr = TR_FAIL;
        }
        else {

            uLength = (0x1 << ((uLevels - 1) - i));

            if (d3dvd.Width != uLength) {
                Log(LOG_FAIL, TEXT("IDirect3DVolume8::GetDesc returned an incorrect width of %d instead of %d for level %d"), ptte->szInterface, d3dvd.Width, uLength, i);
                tr = TR_FAIL;
            }

            if (d3dvd.Height != uLength) {
                Log(LOG_FAIL, TEXT("IDirect3DVolume8::GetDesc returned an incorrect height of %d instead of %d for level %d"), ptte->szInterface, d3dvd.Height, uLength, i);
                tr = TR_FAIL;
            }

            if (d3dvd.Depth != uLength) {
                Log(LOG_FAIL, TEXT("IDirect3DVolume8::GetDesc returned an incorrect depth of %d instead of %d for level %d"), ptte->szInterface, d3dvd.Depth, uLength, i);
                tr = TR_FAIL;
            }

            if (d3dvd.Size < uLength * uLength * uLength * 2) {
                Log(LOG_FAIL, TEXT("IDirect3DVolume8::GetDesc returned an incorrect size of %d for level %d"), ptte->szInterface, d3dvd.Size, i);
                tr = TR_FAIL;
            }

#ifndef UNDER_XBOX

            if (d3dvd.Pool != D3DPOOL_MANAGED) {
                Log(LOG_FAIL, TEXT("IDirect3DVolume8::GetDesc returned an incorrect pool for level %d"), ptte->szInterface, i);
                tr = TR_FAIL;
            }

#endif

            if (d3dvd.Usage != 0) {
                Log(LOG_FAIL, TEXT("IDirect3DVolume8::GetDesc returned an incorrect usage for level %d"), ptte->szInterface, i);
                tr = TR_FAIL;
            }

            if (d3dvd.Type != D3DRTYPE_VOLUME) {
                Log(LOG_FAIL, TEXT("IDirect3DVolume8::GetDesc returned an incorrect type for level %d"), ptte->szInterface, i);
                tr = TR_FAIL;
            }

            if (d3dvd.Format != D3DFMT_A1R5G5B5) {
                Log(LOG_FAIL, TEXT("IDirect3DVolume8::GetDesc returned an incorrect format for level %d"), ptte->szInterface, i);
                tr = TR_FAIL;
            }
        }

        LogEndVariation();
    }

    return tr;
}
