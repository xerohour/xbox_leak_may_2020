/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    rsource8.cpp

Description:

    IDirect3DResource8 interface BVTs.

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
// IDirect3DResource8 helper functions
//******************************************************************************

//******************************************************************************
LPDIRECT3DRESOURCE8 GetResource8(D3DIFACETYPE d3dift) {

    LPDIRECT3DRESOURCE8 pd3dres = NULL;

    switch (d3dift) {

        case ITEXTURE8:
            pd3dres = (LPDIRECT3DRESOURCE8)GetTexture8();
            break;
        case ICUBETEXTURE8:
            pd3dres = (LPDIRECT3DRESOURCE8)GetCubeTexture8();
            break;
        case IVOLUMETEXTURE8:
            pd3dres = (LPDIRECT3DRESOURCE8)GetVolumeTexture8();
            break;
        case IVERTEXBUFFER8:
            pd3dres = (LPDIRECT3DRESOURCE8)GetVertexBuffer8();
            break;
        case IINDEXBUFFER8:
            pd3dres = (LPDIRECT3DRESOURCE8)GetIndexBuffer8();
            break;
    }

    return pd3dres;
}

//******************************************************************************
// IDirect3DResource8 inteface tests
//******************************************************************************

//******************************************************************************
TESTPROCAPI TRES8_GetType(PTESTTABLEENTRY ptte) {

    LPDIRECT3DRESOURCE8 pd3dres;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;
    D3DRESOURCETYPE     d3drt;
    D3DRESOURCETYPE     pd3drt[11] = {
                            (D3DRESOURCETYPE)0xFFFFFFFE,
                            (D3DRESOURCETYPE)0xFFFFFFFE,
                            (D3DRESOURCETYPE)0xFFFFFFFE,
                            (D3DRESOURCETYPE)0xFFFFFFFE,
                            D3DRTYPE_TEXTURE,
                            D3DRTYPE_CUBETEXTURE,
                            D3DRTYPE_VOLUMETEXTURE,
                            D3DRTYPE_VERTEXBUFFER,
                            D3DRTYPE_INDEXBUFFER,
                            (D3DRESOURCETYPE)0xFFFFFFFE,
                            (D3DRESOURCETYPE)0xFFFFFFFE,
                        };
                            
    pd3dres = GetResource8(ptte->dwData);
    if (!pd3dres) {
        return TR_ABORT;
    }

    d3drt = pd3dres->GetType();

    if (d3drt != pd3drt[ptte->dwData]) {
        Log(LOG_FAIL, TEXT("%s::GetType returned an incorrect type: %d"), ptte->szInterface, d3drt);
        tr = TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TRES8_GetDevice(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd, pd3ddR;
    LPDIRECT3DRESOURCE8 pd3dres;
    ULONG               uRef1, uRef2;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    pd3dres = GetResource8(ptte->dwData);
    if (!pd3dres) {
        return TR_ABORT;
    }

    pd3dd->AddRef();
    uRef1 = pd3dd->Release();

    hr = pd3dres->GetDevice(&pd3ddR);
    if (FAILED(hr)) {
        Log(LOG_FAIL, TEXT("%s::GetDevice failed [0x%X]"), ptte->szInterface, hr);
        return TR_FAIL;
    }

    pd3dd->AddRef();
    uRef2 = pd3dd->Release();

    if (uRef2 != uRef1 + 1) {
        Log(LOG_FAIL, TEXT("%s::GetDevice failed to increment the reference count of the device object"), ptte->szInterface);
        tr = TR_FAIL;
    }

    pd3dd->Release();

    if (pd3ddR != pd3dd) {
        Log(LOG_FAIL, TEXT("%s::GetDevice returned an incorrect pointer to the device object: 0x%X"), ptte->szInterface, pd3ddR);
        tr = TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TRES8_GetPrivateData(PTESTTABLEENTRY ptte) {

    LPDIRECT3DRESOURCE8 pd3dres;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dres = GetResource8(ptte->dwData);
    if (!pd3dres) {
        return TR_ABORT;
    }

    return TSHR8_GetPrivateData(ptte, (LPVOID)pd3dres);
}

//******************************************************************************
TESTPROCAPI TRES8_SetPrivateData(PTESTTABLEENTRY ptte) {

    LPDIRECT3DRESOURCE8 pd3dres;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dres = GetResource8(ptte->dwData);
    if (!pd3dres) {
        return TR_ABORT;
    }

    return TSHR8_SetPrivateData(ptte, (LPVOID)pd3dres);
}

//******************************************************************************
TESTPROCAPI TRES8_FreePrivateData(PTESTTABLEENTRY ptte) {

    LPDIRECT3DRESOURCE8 pd3dres;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dres = GetResource8(ptte->dwData);
    if (!pd3dres) {
        return TR_ABORT;
    }

    return TSHR8_FreePrivateData(ptte, (LPVOID)pd3dres);
}

#ifndef UNDER_XBOX

//******************************************************************************
TESTPROCAPI TRES8_GetPriority(PTESTTABLEENTRY ptte) {

    LPDIRECT3DRESOURCE8 pd3dres;
    DWORD               dwPriority;
    TRESULT             tr = TR_PASS;

    pd3dres = GetResource8(ptte->dwData);
    if (!pd3dres) {
        return TR_ABORT;
    }

    dwPriority = pd3dres->GetPriority();
    if (dwPriority != 0) {
        Log(LOG_FAIL, TEXT("%s::GetPriority returned a priority of %d for a nonmanaged resource"), ptte->szInterface, dwPriority);
        tr = TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TRES8_SetPriority(PTESTTABLEENTRY ptte) {

    LPDIRECT3DRESOURCE8 pd3dres;
    DWORD               dwPriority;
    TRESULT             tr = TR_PASS;

    pd3dres = GetResource8(ptte->dwData);
    if (!pd3dres) {
        return TR_ABORT;
    }

    pd3dres->SetPriority(rand());

    dwPriority = pd3dres->SetPriority(rand());
    if (dwPriority != 0) {
        Log(LOG_FAIL, TEXT("%s::SetPriority returned a priority of %d for a nonmanaged resource"), ptte->szInterface, dwPriority);
        tr = TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TRES8_PreLoad(PTESTTABLEENTRY ptte) {

    LPDIRECT3DRESOURCE8 pd3dres;
    TRESULT             tr = TR_PASS;

    pd3dres = GetResource8(ptte->dwData);
    if (!pd3dres) {
        return TR_ABORT;
    }

    pd3dres->PreLoad();

    return tr;
}

#endif // !UNDER_XBOX
