/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    unknown.cpp

Description:

    IUnknown interface BVTs.

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
// IUnknown helper functions
//******************************************************************************

#ifndef UNDER_XBOX

//******************************************************************************
IUnknown* GetUnknown(D3DIFACETYPE d3dift) {

    IUnknown* punk = NULL;

    switch (d3dift) {

        case IDIRECT3D8:
            punk = (IUnknown*)GetDirect3D8();
            break;
        case IDEVICE8:
            punk = (IUnknown*)GetDevice8();
            break;
        case ISWAPCHAIN8:
            punk = (IUnknown*)GetSwapChain8();
            break;
        case ITEXTURE8:
            punk = (IUnknown*)GetTexture8();
            break;
        case ICUBETEXTURE8:
            punk = (IUnknown*)GetCubeTexture8();
            break;
        case IVOLUMETEXTURE8:
            punk = (IUnknown*)GetVolumeTexture8();
            break;
        case IVERTEXBUFFER8:
            punk = (IUnknown*)GetVertexBuffer8();
            break;
        case IINDEXBUFFER8:
            punk = (IUnknown*)GetIndexBuffer8();
            break;
        case ISURFACE8:
            punk = (IUnknown*)GetSurface8();
            break;
        case IVOLUME8:
            punk = (IUnknown*)GetVolume8();
            break;
    }

    return punk;
}

#endif // !UNDER_XBOX

//******************************************************************************
// IUnknown inteface tests
//******************************************************************************

//******************************************************************************
TESTPROCAPI TUNK_AddRef(PTESTTABLEENTRY ptte) {

    IUnknown*   punk;
    ULONG       uRef1, uRef2;
    TRESULT     tr = TR_PASS;

#ifndef UNDER_XBOX

    punk = GetUnknown(ptte->dwData);
    if (!punk) {
        return TR_ABORT;
    }

    uRef1 = punk->AddRef();
    uRef2 = punk->AddRef();

    if (uRef2 != uRef1 + 1) {
        Log(LOG_FAIL, TEXT("%s::AddRef failed to increment the reference count of the object"), ptte->szInterface);
        tr = TR_FAIL;
    }

    punk->Release();
    punk->Release();

#else

#define VERIFY_ADDREF(ptr) \
    uRef1 = ptr->AddRef(); \
    uRef2 = ptr->AddRef(); \
    if (uRef2 != uRef1 + 1) { \
        Log(LOG_FAIL, TEXT("%s::AddRef failed to increment the reference count of the object"), ptte->szInterface); \
        tr = TR_FAIL; \
    } \
    ptr->Release(); \
    ptr->Release();

    switch (ptte->dwData) {

        case IDIRECT3D8:
//            VERIFY_ADDREF(GetDirect3D8())
            break;
        case IDEVICE8:
            VERIFY_ADDREF(GetDevice8())
            break;
        case ITEXTURE8:
            VERIFY_ADDREF(GetTexture8())
            break;
        case ICUBETEXTURE8:
            VERIFY_ADDREF(GetCubeTexture8())
            break;
        case IVOLUMETEXTURE8:
            VERIFY_ADDREF(GetVolumeTexture8())
            break;
        case IVERTEXBUFFER8:
            VERIFY_ADDREF(GetVertexBuffer8())
            break;
        case IINDEXBUFFER8:
            VERIFY_ADDREF(GetIndexBuffer8())
            break;
        case ISURFACE8:
            VERIFY_ADDREF(GetSurface8())
            break;
        case IVOLUME8:
            VERIFY_ADDREF(GetVolume8())
            break;
    }

#endif // UNDER_XBOX

    return tr;
}

//******************************************************************************
TESTPROCAPI TUNK_Release(PTESTTABLEENTRY ptte) {

    IUnknown*   punk;
    ULONG       uRef1, uRef2;
    TRESULT     tr = TR_PASS;

#ifndef UNDER_XBOX

    punk = GetUnknown(ptte->dwData);
    if (!punk) {
        return TR_ABORT;
    }

    punk->AddRef();
    punk->AddRef();

    uRef1 = punk->Release();
    uRef2 = punk->Release();

    if (uRef2 != uRef1 - 1) {
        Log(LOG_FAIL, TEXT("%s::Release failed to decrement the reference count of the object"), ptte->szInterface);
        tr = TR_FAIL;
    }

#else

#define VERIFY_RELEASE(ptr) \
    ptr->AddRef(); \
    ptr->AddRef(); \
    uRef1 = ptr->Release(); \
    uRef2 = ptr->Release(); \
    if (uRef2 != uRef1 - 1) { \
        Log(LOG_FAIL, TEXT("%s::Release failed to decrement the reference count of the object"), ptte->szInterface); \
        tr = TR_FAIL; \
    }

    switch (ptte->dwData) {

        case IDIRECT3D8:
//            VERIFY_RELEASE(GetDirect3D8())
            break;
        case IDEVICE8:
            VERIFY_RELEASE(GetDevice8())
            break;
        case ITEXTURE8:
            VERIFY_RELEASE(GetTexture8())
            break;
        case ICUBETEXTURE8:
            VERIFY_RELEASE(GetCubeTexture8())
            break;
        case IVOLUMETEXTURE8:
            VERIFY_RELEASE(GetVolumeTexture8())
            break;
        case IVERTEXBUFFER8:
            VERIFY_RELEASE(GetVertexBuffer8())
            break;
        case IINDEXBUFFER8:
            VERIFY_RELEASE(GetIndexBuffer8())
            break;
        case ISURFACE8:
            VERIFY_RELEASE(GetSurface8())
            break;
        case IVOLUME8:
            VERIFY_RELEASE(GetVolume8())
            break;
    }

#endif // UNDER_XBOX

    return tr;
}

//******************************************************************************
TESTPROCAPI TUNK_QueryInterface(PTESTTABLEENTRY ptte) {

    IUnknown*   punk;
    IUnknown*   punkQ;
    ULONG       uRef1, uRef2;
    HRESULT     hr;
    TRESULT     tr = TR_PASS;

#ifndef UNDER_XBOX

    punk = GetUnknown(ptte->dwData);
    if (!punk) {
        return TR_ABORT;
    }

    // ##TODO: If QueryInterface ends up being supported, verify it succeeds on
    // the object's interface and all of its base interfaces and fails on all
    // other interfaces of d3d

    punk->AddRef();
    uRef1 = punk->Release();

    hr = punk->QueryInterface(IID_IUnknown, (LPVOID*)&punkQ);
    if (FAILED(hr)) {
        Log(LOG_FAIL, TEXT("%s::QueryInterface(IID_IUnknown, ...) failed [0x%X]"), ptte->szInterface, hr);
        return TR_FAIL;
    }

    punk->AddRef();
    uRef2 = punk->Release();

    if (uRef2 != uRef1 + 1) {
        Log(LOG_FAIL, TEXT("%s::QueryInterface failed to increment the reference count of the object"), ptte->szInterface);
        return TR_FAIL;
    }

    punk->Release();

#else

#if 0

#define VERIFY_QUERYINTERFACE(ptr) \
    ptr->AddRef(); \
    uRef1 = ptr->Release(); \
\
    hr = ptr->QueryInterface(IID_IUnknown, (LPVOID*)&punkQ); \
    if (FAILED(hr)) { \
        Log(LOG_FAIL, TEXT("%s::QueryInterface(IID_IUnknown, ...) failed [0x%X]"), ptte->szInterface, hr); \
        return TR_FAIL; \
    } \
\
    ptr->AddRef(); \
    uRef2 = ptr->Release(); \
\
    if (uRef2 != uRef1 + 1) { \
        Log(LOG_FAIL, TEXT("%s::QueryInterface failed to increment the reference count of the object"), ptte->szInterface); \
        return TR_FAIL; \
    } \
\
    ptr->Release();

    switch (ptte->dwData) {

        case IDIRECT3D8:
            VERIFY_QUERYINTERFACE(GetDirect3D8())
            break;
        case IDEVICE8:
            VERIFY_QUERYINTERFACE(GetDevice8())
            break;
        case ITEXTURE8:
            VERIFY_QUERYINTERFACE(GetTexture8())
            break;
        case ICUBETEXTURE8:
            VERIFY_QUERYINTERFACE(GetCubeTexture8())
            break;
        case IVOLUMETEXTURE8:
            VERIFY_QUERYINTERFACE(GetVolumeTexture8())
            break;
        case IVERTEXBUFFER8:
            VERIFY_QUERYINTERFACE(GetVertexBuffer8())
            break;
        case IINDEXBUFFER8:
            VERIFY_QUERYINTERFACE(GetIndexBuffer8())
            break;
        case ISURFACE8:
            VERIFY_QUERYINTERFACE(GetSurface8())
            break;
        case IVOLUME8:
            VERIFY_QUERYINTERFACE(GetVolume8())
            break;
    }

#else
    tr = TR_SKIP;
#endif
#endif // UNDER_XBOX

    return tr;
}
