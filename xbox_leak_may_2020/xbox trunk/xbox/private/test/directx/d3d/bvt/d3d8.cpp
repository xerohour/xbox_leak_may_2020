/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    d3d8.cpp

Description:

    IDirect3D8 interface BVTs.

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

#ifdef UNDER_XBOX
#define DEMO_HACK
#endif

#ifndef UNDER_XBOX
#define INITIAL_MODE_WIDTH      640
#define INITIAL_MODE_HEIGHT     480
#define INITIAL_MODE_REFRESH    60
#define INITIAL_MODE_FORMAT     D3DFMT_R5G6B5
#else
#define INITIAL_MODE_WIDTH      0
#define INITIAL_MODE_HEIGHT     0
#define INITIAL_MODE_REFRESH    0
#define INITIAL_MODE_FORMAT     0
#endif

//******************************************************************************
// IDirect3D8 inteface tests
//******************************************************************************

//******************************************************************************
TESTPROCAPI TD3D8_GetAdapterCount(PTESTTABLEENTRY ptte) {

    LPDIRECT3D8 pd3d;
    UINT        uCount;
    TRESULT     tr = TR_PASS;

    pd3d = GetDirect3D8();
    if (!pd3d) {
        return TR_ABORT;
    }

    uCount = pd3d->GetAdapterCount();
    if (uCount != 1) {
        Log(LOG_FAIL, TEXT("IDirect3D8::GetAdapterCount returned %d adapters"), uCount);
        tr = TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TD3D8_GetAdapterDisplayMode(PTESTTABLEENTRY ptte) {

    LPDIRECT3D8     pd3d;
    D3DDISPLAYMODE  d3ddm;
    HRESULT         hr;
    TRESULT         tr = TR_PASS;

    ReleaseDirect3D8();

    pd3d = GetDirect3D8();
    if (!pd3d) {
        return TR_ABORT;
    }

    hr = pd3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
    if (ResultFailed(hr, TEXT("IDirect3D8::GetAdapterDisplayMode"))) {
        return TR_FAIL;
    }

    if (d3ddm.Width != INITIAL_MODE_WIDTH || 
        d3ddm.Height != INITIAL_MODE_HEIGHT ||
        d3ddm.RefreshRate != INITIAL_MODE_REFRESH || 
        d3ddm.Format != INITIAL_MODE_FORMAT)
    {
        Log(LOG_FAIL, TEXT("IDirect3D8::GetAdapterDisplayMode returned an incorrect initial display mode"));
        return TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TD3D8_GetAdapterIdentifier(PTESTTABLEENTRY ptte) {

    LPDIRECT3D8             pd3d;
    D3DADAPTER_IDENTIFIER8  d3dai;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;

    pd3d = GetDirect3D8();
    if (!pd3d) {
        return TR_ABORT;
    }

    hr = pd3d->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &d3dai);
    if (ResultFailed(hr, TEXT("IDirect3D8::GetAdapterIdentifier"))) {
        return TR_FAIL;
    }

    // TODO: Verify the adapter identifier fields

    return tr;
}

//******************************************************************************
TESTPROCAPI TD3D8_GetAdapterModeCount(PTESTTABLEENTRY ptte) {

    LPDIRECT3D8 pd3d;
    UINT        uNumModes;
    TRESULT     tr = TR_PASS;

    pd3d = GetDirect3D8();
    if (!pd3d) {
        return TR_ABORT;
    }

    uNumModes = pd3d->GetAdapterModeCount(D3DADAPTER_DEFAULT);

    // TODO: Verify the correct number of modes are returned for each output type
    Log(LOG_COMMENT, TEXT("IDirect3D8::GetAdapterModeCount returned %d modes"), uNumModes);

    return tr;
}

#ifndef UNDER_XBOX

//******************************************************************************
TESTPROCAPI TD3D8_GetAdapterMonitor(PTESTTABLEENTRY ptte) {

    LPDIRECT3D8 pd3d;
    HMONITOR    hmon;
    HRESULT     hr;
    TRESULT     tr = TR_PASS;

    pd3d = GetDirect3D8();
    if (!pd3d) {
        return TR_ABORT;
    }

    hmon = pd3d->GetAdapterMonitor(D3DADAPTER_DEFAULT);
    if (hmon != NULL) {
        Log(LOG_FAIL, TEXT("IDirect3D8::GetAdapterMonitor"));
        return TR_FAIL;
    }

    return tr;
}

#endif // !UNDER_XBOX

//******************************************************************************
TESTPROCAPI TD3D8_GetDeviceCaps(PTESTTABLEENTRY ptte) {

    LPDIRECT3D8 pd3d;
    D3DCAPS8    d3dcaps;
    HRESULT     hr;
    TRESULT     tr = TR_PASS;

    pd3d = GetDirect3D8();
    if (!pd3d) {
        return TR_ABORT;
    }

    hr = pd3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dcaps);
    if (ResultFailed(hr, TEXT("IDirect3D8::GetDeviceCaps"))) {
        return TR_FAIL;
    }

    // TODO: Verify the device caps

    return tr;
}

#ifndef UNDER_XBOX

//******************************************************************************
TESTPROCAPI TD3D8_RegisterSoftwareDevice(PTESTTABLEENTRY ptte) {

    LPDIRECT3D8 pd3d;
    TCHAR       szError[512];
    HRESULT     hr;
    TRESULT     tr = TR_PASS;

    pd3d = GetDirect3D8();
    if (!pd3d) {
        return TR_ABORT;
    }

    hr = pd3d->RegisterSoftwareDevice((LPVOID)TD3D8_RegisterSoftwareDevice);
    if (hr != E_NOTIMPL) {
        D3DXGetErrorString(hr, szError, 512);
        Log(LOG_FAIL, TEXT("IDirect3D8::RegisterSoftwareDevice returned %s instead of D3DERR_UNSUPPORTED"), szError);
        tr = TR_FAIL;
    }

    return tr;
}

#endif // !UNDER_XBOX

//******************************************************************************
TESTPROCAPI TD3D8_CheckDeviceFormat(PTESTTABLEENTRY ptte) {

    LPDIRECT3D8 pd3d;
    HRESULT     hr;
    TRESULT     tr = TR_PASS;

    pd3d = GetDirect3D8();
    if (!pd3d) {
        return TR_ABORT;
    }

    hr = pd3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, 0, D3DRTYPE_TEXTURE, D3DFMT_A4R4G4B4);
    if (hr != D3D_OK) {
        tr = TR_FAIL;
    }

    hr = pd3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE, D3DFMT_A4R4G4B4);
    if (hr != D3DERR_NOTAVAILABLE) {
        tr = TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TD3D8_CheckDeviceMultiSampleType(PTESTTABLEENTRY ptte) {

    LPDIRECT3D8 pd3d;
    HRESULT     hr;
    TRESULT     tr = TR_PASS;

    pd3d = GetDirect3D8();
    if (!pd3d) {
        return TR_ABORT;
    }

#ifndef UNDER_XBOX
    hr = pd3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, FALSE, D3DMULTISAMPLE_4_SAMPLES);
#else
    hr = pd3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, FALSE, D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR);
#endif
    if (ResultFailed(hr, TEXT("IDirect3D8::CheckDeviceMultisampleType"))) {
        tr = TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TD3D8_CheckDeviceType(PTESTTABLEENTRY ptte) {

    LPDIRECT3D8 pd3d;
    HRESULT     hr;
    TRESULT     tr = TR_PASS;

    pd3d = GetDirect3D8();
    if (!pd3d) {
        return TR_ABORT;
    }

    hr = pd3d->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X1R5G5B5, D3DFMT_X1R5G5B5, FALSE);
    if (ResultFailed(hr, TEXT("IDirect3D8::CheckDeviceType"))) {
        tr = TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TD3D8_CheckDepthStencilMatch(PTESTTABLEENTRY ptte) {

    LPDIRECT3D8 pd3d;
    HRESULT     hr;
    TRESULT     tr = TR_PASS;

    pd3d = GetDirect3D8();
    if (!pd3d) {
        return TR_ABORT;
    }

#ifndef UNDER_XBOX
    hr = pd3d->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8, D3DFMT_D24S8);
#else
    hr = pd3d->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_LIN_X8R8G8B8, D3DFMT_A8R8G8B8, D3DFMT_LIN_F24S8);
#endif
    if (ResultFailed(hr, TEXT("IDirect3D8::CheckDeviceType"))) {
        tr = TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TD3D8_CreateDevice(PTESTTABLEENTRY ptte) {

    D3DPRESENT_PARAMETERS   d3dpp;
    LPDIRECT3D8             pd3d;
    LPDIRECT3DDEVICE8       pd3dd;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;

    ReleaseDevice8();

    pd3d = GetDirect3D8();
    if (!pd3d) {
        return TR_ABORT;
    }

    // Initialize the presentation parameters
    memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    d3dpp.BackBufferWidth                   = 640;
    d3dpp.BackBufferHeight                  = 480;
    d3dpp.BackBufferFormat                  = D3DFMT_A8R8G8B8;
    d3dpp.MultiSampleType                   = D3DMULTISAMPLE_4_SAMPLES;
    d3dpp.AutoDepthStencilFormat            = D3DFMT_D24S8;
    d3dpp.FullScreen_RefreshRateInHz        = 0;
    d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_ONE;
    d3dpp.BackBufferCount                   = 2;
    d3dpp.SwapEffect                        = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                     = NULL;
    d3dpp.Windowed                          = FALSE;
    d3dpp.EnableAutoDepthStencil            = TRUE;

    // Create the device
    hr = pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pd3dd);
    if (ResultFailed(hr, TEXT("IDirect3D8::CreateDevice failed"))) {
        return FALSE;
    }

    // Verify the interface
    d3dpp.EnableAutoDepthStencil = FALSE;
    d3dpp.BackBufferCount = 1;
    pd3dd->Reset(&d3dpp);

    pd3dd->Release();

    return tr;
}

//******************************************************************************
TESTPROCAPI TD3D8_EnumAdapterModes(PTESTTABLEENTRY ptte) {

    LPDIRECT3D8     pd3d;
    D3DDISPLAYMODE  d3ddm;
    UINT            uNumModes, i;
    HRESULT         hr;
    TRESULT         tr = TR_PASS;

    pd3d = GetDirect3D8();
    if (!pd3d) {
        return TR_ABORT;
    }

    uNumModes = pd3d->GetAdapterModeCount(D3DADAPTER_DEFAULT);

    for (i = 0; i < uNumModes; i++) {
        hr = pd3d->EnumAdapterModes(D3DADAPTER_DEFAULT, i, &d3ddm);
        if (ResultFailed(hr, TEXT("IDirect3D8::EnumAdapterModes"))) {
            tr = TR_FAIL;
        }
        else {
            Log(LOG_COMMENT, TEXT("Adapter mode %d - Width: %4d, Height: %4d, Refresh: %3d, Format: 0x%X"), i, d3ddm.Width, d3ddm.Height, d3ddm.RefreshRate, d3ddm.Format);
        }
    }

    return tr;
}

/*
//******************************************************************************
TESTPROCAPI TD3D8_GetAdapterCount(PTESTTABLEENTRY ptte) {

    LPDIRECT3D8 pd3d;
    HRESULT     hr;
    TRESULT     tr = TR_PASS;

    pd3d = GetDirect3D8();
    if (!pd3d) {
        return TR_ABORT;
    }

    return tr;
}
*/