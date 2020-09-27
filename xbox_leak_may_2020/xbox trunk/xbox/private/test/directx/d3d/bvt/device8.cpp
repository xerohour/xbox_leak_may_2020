/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    dev8.cpp

Description:

    IDirect3DDevice8 interface BVTs.

*******************************************************************************/

#define D3DCOMPILE_BEGINSTATEBLOCK 1

#ifndef UNDER_XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif // UNDER_XBOX
#include <tchar.h>
#include <d3dx8.h>
#include <xgraphics.h>
#include "d3dbvt.h"
#include "log.h"
#include "util.h"

using namespace D3DBVT;

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define FLT_RND     ((-1.0f + 2.0f * RND()) * FLT_MAX)
// #define FLT_MAX 3.402823466e+38F

//******************************************************************************
// Globals
//******************************************************************************

char g_szVertexShader[] = {

    ";\n"
    "; Vertex input registers:\n"
    ";\n"
    "; v0            Vertex position\n"
    "; v3            Vertex normal\n"
    "; v7            Vertex texture coordinates\n"
    ";\n"
    "; Constant registers:\n"
    ";\n"
    "; c0 - c3       Transposed concatenation of world, view, and projection matrices\n"
    ";\n"
    "; c4            Normalized light direction\n"
    "; c5            View position\n"
    "; c6.x          Material power\n"
    "; c6.y          0.0f\n"
    "; c6.z          1.0f\n"
    "; c7            Diffuse base color (diffuse light color modulated with diffuse material color)\n"
    "; c8            Diffuse offset color (ambient room color modulated with ambient material color +\n"
    ";                                     ambient light color modulated with ambient material color +\n"
    ";                                     emissive material color)\n"
    "; c9            Specular base color (specular light color modulated with specular material color)\n"
    ";\n"

    "vs.1.0								; Vertex shader version 1.0\n"

    "; Transform the vertex\n"

    "dp4 oPos.x, v0, c0\n"
    "dp4 oPos.y, v0, c1\n"
    "dp4 oPos.z, v0, c2\n"
    "dp4 oPos.w, v0, c3\n"

    "; Light the vertex with a directional light\n"

    "dp3 r2.x, v3, -c4                   ; Diffuse intensity = dotproduct(vertex normal, light direction)\n"

    "add r0, v0, -c5                     ; Calculate a vector from the eye to the vertex\n"
    "dp3 r1.w, r0, r0                    ; nm3 r0, r0\n"
    "rsq r1.w, r1.w\n"
    "mul r1.xyz, r0, r1.w\n"
    "add r0, r1, c4                      ; Calculate the half-vector\n"
    "dp3 r1.w, r0, r0                    ; nm3 r0, r0\n"
    "rsq r1.w, r1.w\n"
    "mul r1.xyz, r0, r1.w\n"
    "dp3 r2.y, v3, -r1                   ; Specular intensity = dotproduct(vertex normal, light/eye half-vector)\n"

    "mov r2.w, c6.x                      ; Material power\n"

    "lit r3, r2                          ; Light coefficients\n"

    ";mad r0.xyz, r3.y, c7.xyz, c8.xyz    ; Diffuse color = diffuse intensity * diffuse base color + diffuse offset color\n"
    "mul r0.xyz, r3.y, c7.xyz\n"
    "add r0.xyz, r0.xyz, c8.xyz\n"

    "mov r0.w, c7.w                      ; Diffuse alpha = diffuse base alpha\n"
    "min oD0, r0, r3.w                   ; Clamp diffuse colors to 1.0f\n"

    "mul r0.xyz, r3.z, c9.xyz            ; Specular color = specular intensity * specular base color\n"
    "mov r0.w, c9.w                      ; Specular alpha = specular base alpha\n"
    "min oD1, r0, r3.w                   ; Clamp specular colors to 1.0f\n"

    "mov oT0, v7                         ; Propagate texture coordinates\n"
};

char g_szPixelShader[] = {

    ";\n"
    "; Color input registers:\n"
    ";\n"
    "; v0            Diffuse color\n"
    "; v1            Specular color\n"
    ";\n"

    "ps.1.0              ; Pixel shader version 1.0\n"
    "tex t0              ; Sample the texture set in stage 0\n"
    "mul r1, t0, v0      ; Modulate the pixel's diffuse color with the texel color\n"
    "add r0.xyz, r1, v1  ; Add the pixel's specular color\n"
    "mov r0.a, r1.a      ; Use the modulated diffuse/texel alpha for the pixel\n"
};

//******************************************************************************
BOOL IsCompressedFormat(D3DFORMAT fmt) {

    switch (fmt) {
        case D3DFMT_DXT1:
        case D3DFMT_DXT2:
#ifndef UNDER_XBOX
        case D3DFMT_DXT3:
#endif
        case D3DFMT_DXT4:
#ifndef UNDER_XBOX
        case D3DFMT_DXT5:
#endif
            return TRUE;
        default:
            return FALSE;
    }
}

//******************************************************************************
// IDirect3DDevice8 inteface tests
//******************************************************************************

#ifndef UNDER_XBOX

//******************************************************************************
TESTPROCAPI TDEV8_CreateAdditionalSwapChain(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8       pd3dd;
    LPDIRECT3DSWAPCHAIN8    pd3dsw;
    D3DPRESENT_PARAMETERS   d3dpp;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    // Initialize the presentation parameters
    memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    d3dpp.BackBufferWidth                   = 640;
    d3dpp.BackBufferHeight                  = 480;
    d3dpp.BackBufferFormat                  = D3DFMT_X8R8G8B8;
    d3dpp.MultiSampleType                   = D3DMULTISAMPLE_NONE;
    d3dpp.AutoDepthStencilFormat            = D3DFMT_D24S8;
    d3dpp.FullScreen_RefreshRateInHz        = 0;
    d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.BackBufferCount                   = 1;
    d3dpp.SwapEffect                        = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                     = g_hWnd;
    d3dpp.Windowed                          = FALSE;
    d3dpp.EnableAutoDepthStencil            = FALSE;

    pd3dsw = (LPDIRECT3DSWAPCHAIN8)DEAD_PTR;

    hr = pd3dd->CreateAdditionalSwapChain(&d3dpp, &pd3dsw);
    if (hr != E_NOTIMPL) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::CreateAdditionalSwapChain returned 0x%X instead of E_NOTIMPL"), hr);
        tr = TR_FAIL;
        if (SUCCEEDED(hr)) {
            pd3dsw->Release();
        }
    }
    else {
        if (pd3dsw != NULL) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::CreateAdditionalSwapChain failed to set the swap chain pointer to NULL on failure"));
            tr = TR_FAIL;
        }
    }

    return tr;
}

#endif // !UNDER_XBOX

//******************************************************************************
TESTPROCAPI TDEV8_CreateCubeTexture(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8       pd3dd;
    LPDIRECT3DCUBETEXTURE8  pd3dtc;
    UINT                    i, j, k, uNumFormats;
    D3DFORMAT*              pfmt;
    D3DLOCKED_RECT          d3dlr;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;
    DWORD                   dwUsage[] = {
                                0,
                                D3DUSAGE_RENDERTARGET,
#ifndef UNDER_XBOX
                                D3DUSAGE_DEPTHSTENCIL,
#endif // !UNDER_XBOX
                            };
    UINT                    uLevels[] = {
                                0,
                                1,
                                2,
                                9,
                                10
                            };
    D3DFORMAT               fmt[] = {
                                D3DFMT_A8R8G8B8,
                                D3DFMT_X8R8G8B8,
                                D3DFMT_R5G6B5,
                                D3DFMT_X1R5G5B5,
                                D3DFMT_A1R5G5B5,
                                D3DFMT_A4R4G4B4,
                                D3DFMT_A8,
                                D3DFMT_P8,
                                D3DFMT_L8,
                                D3DFMT_A8L8,
                                D3DFMT_V8U8,
                                D3DFMT_L6V5U5,
                                D3DFMT_X8L8V8U8,
                                D3DFMT_Q8W8V8U8,
                                D3DFMT_V16U16,
#ifndef UNDER_XBOX
                                D3DFMT_UYVY,
                                D3DFMT_YUY2,
#endif
                                D3DFMT_DXT1,
                                D3DFMT_DXT2,
                                D3DFMT_DXT3,
                                D3DFMT_DXT4,
                                D3DFMT_DXT5,
#ifdef UNDER_XBOX
                                D3DFMT_A8B8G8R8,
                                D3DFMT_B8G8R8A8,
                                D3DFMT_R4G4B4A4,
                                D3DFMT_R5G5B5A1,
                                D3DFMT_R8G8B8A8,
                                D3DFMT_R8B8,
                                D3DFMT_G8B8,
                                D3DFMT_AL8,
                                D3DFMT_L16,
#if 0
                                D3DFMT_LIN_A1R5G5B5,
                                D3DFMT_LIN_A4R4G4B4,
                                D3DFMT_LIN_A8,
                                D3DFMT_LIN_A8B8G8R8,
                                D3DFMT_LIN_A8R8G8B8,
                                D3DFMT_LIN_B8G8R8A8,
                                D3DFMT_LIN_G8B8,
                                D3DFMT_LIN_R4G4B4A4,
                                D3DFMT_LIN_R5G5B5A1,
                                D3DFMT_LIN_R5G6B5,
                                D3DFMT_LIN_R6G5B5,
                                D3DFMT_LIN_R8B8,
                                D3DFMT_LIN_R8G8B8A8,
                                D3DFMT_LIN_X1R5G5B5,
                                D3DFMT_LIN_X8R8G8B8,
                                D3DFMT_LIN_A8L8,
                                D3DFMT_LIN_AL8,
                                D3DFMT_LIN_L16,
                                D3DFMT_LIN_L8,
#endif
#endif // UNDER_XBOX
                            };

    D3DFORMAT               fmtr[] = {
                                D3DFMT_X1R5G5B5,
                                D3DFMT_R5G6B5,
                                D3DFMT_X8R8G8B8,
                                D3DFMT_A8R8G8B8,
#if 0 // UNDER_XBOX
                                D3DFMT_LIN_X1R5G5B5,
                                D3DFMT_LIN_R5G6B5,
                                D3DFMT_LIN_X8R8G8B8,
                                D3DFMT_LIN_A8R8G8B8,
#endif // UNDER_XBOX
                            };

#ifndef UNDER_XBOX
    D3DFORMAT               fmtd[] = {
                                D3DFMT_D16_LOCKABLE,
                                D3DFMT_D16,
                                D3DFMT_D24S8,
#ifdef UNDER_XBOX
                                D3DFMT_F16,
                                D3DFMT_F24S8,
#if 0
                                D3DFMT_LIN_D24S8,
                                D3DFMT_LIN_F24S8,
                                D3DFMT_LIN_D16,
                                D3DFMT_LIN_F16,
#endif
#endif // UNDER_XBOX
                            };
#endif // !UNDER_XBOX

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    for (i = 0; i < countof(dwUsage); i++) {

        LogBeginVariation(TEXT("IDirect3DDevice8::CreateCubeTexture with a usage of %d"), dwUsage[i]);

#ifndef UNDER_XBOX
        if (i == 2) {
            pfmt = fmtd;
            uNumFormats = countof(fmtd);
        }
        else if (i == 1) {
#else
        if (i == 1) {
#endif // UNDER_XBOX
            pfmt = fmtr;
            uNumFormats = countof(fmtr);
        }
        else {
            pfmt = fmt;
            uNumFormats = countof(fmt);
        }

        for (j = 0; j < uNumFormats; j++) {

            LogBeginVariation(TEXT("Cube textures with a format of %d"), pfmt[j]);

            for (k = 0; k < countof(uLevels); k++) {

                hr = pd3dd->CreateCubeTexture(512, uLevels[k], dwUsage[i], pfmt[j], D3DPOOL_MANAGED, &pd3dtc);
                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("IDirect3DDevice8::CreateCubeTexture failed given %d levels [0x%X]"), uLevels[k], hr);
                    tr = TR_FAIL;
                }
                else {
                    // Verify the texture
                    hr = pd3dtc->LockRect(D3DCUBEMAP_FACE_NEGATIVE_Z, 0, &d3dlr, NULL, 0);
                    if (FAILED(hr)) {
                        Log(LOG_FAIL, TEXT("IDirect3DCubeTexture8::LockRect failed on created texture [0x%X]"), hr);
                        tr = TR_FAIL;
                    }
                    else {
                        *((LPBYTE)d3dlr.pBits) = 0x3C;
                        pd3dtc->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_Z, 0);
                    }
                    pd3dtc->Release();
                }
            }

            LogEndVariation();
        }

        LogEndVariation();
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_CreateDepthStencilSurface(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8       pd3dd;
    LPDIRECT3DSURFACE8      pd3ds;
    UINT                    i, j, k, l;
    D3DLOCKED_RECT          d3dlr;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;
    UINT                    uWidth[] = {
                                640,
                                512,
                                213,
                            };
    UINT                    uHeight[] = {
                                480,
                                512,
                                7
                            };
    D3DFORMAT               fmtd[] = {
                                D3DFMT_D16_LOCKABLE,
                                D3DFMT_D16,
                                D3DFMT_D24S8,
#ifdef UNDER_XBOX
                                D3DFMT_F16,
                                D3DFMT_F24S8,
                                D3DFMT_LIN_D24S8,
                                D3DFMT_LIN_F24S8,
                                D3DFMT_LIN_D16,
                                D3DFMT_LIN_F16,
#endif // UNDER_XBOX
                            };
    D3DMULTISAMPLE_TYPE     mst[] = {
                                D3DMULTISAMPLE_NONE,
#ifndef UNDER_XBOX
                                D3DMULTISAMPLE_2_SAMPLES,
                                D3DMULTISAMPLE_4_SAMPLES,
#endif
                            };

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    for (j = 0; j < countof(fmtd); j++) {

        LogBeginVariation(TEXT("Depth/stencil buffers with a format of %d"), fmtd[j]);

        for (k = 0; k < countof(mst); k++) {

            LogBeginVariation(TEXT("Depth/stencil buffers with a multisample type of %d samples"), k * 2);

            for (l = 0; l < countof(uWidth); l++) {

#ifdef UNDER_XBOX
                if (!XGIsSwizzledFormat(fmtd[j])) {
#endif // UNDER_XBOX
                    hr = pd3dd->CreateDepthStencilSurface(uWidth[l], uHeight[l], fmtd[j], mst[k], &pd3ds);
#ifdef UNDER_XBOX
                }
                else {
                    hr = pd3dd->CreateDepthStencilSurface(uWidth[1], uHeight[1], fmtd[j], mst[k], &pd3ds);
                }
#endif // UNDER_XBOX
                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("IDirect3DDevice8::CreateDepthStencilSurface failed for a %d x %d surface [0x%X]"), uWidth[l], uHeight[l], hr);
                    tr = TR_FAIL;
                }
                else {
                    // Verify the surface
                    hr = pd3ds->LockRect(&d3dlr, NULL, 0);
                    if (FAILED(hr)) {
                        Log(LOG_FAIL, TEXT("IDirect3DSurface8::LockRect failed on created %d x %d surface [0x%X]"), uWidth[l], uHeight[l], hr);
                        tr = TR_FAIL;
                    }
                    else {
                        *((LPBYTE)d3dlr.pBits) = 0x3C;
                        pd3ds->UnlockRect();
                    }
                    pd3ds->Release();
                }
            }

            LogEndVariation();
        }

        LogEndVariation();
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_CreateImageSurface(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8       pd3dd;
    LPDIRECT3DSURFACE8      pd3ds;
    UINT                    i, j;
    D3DLOCKED_RECT          d3dlr;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;
    UINT                    uWidth[] = {
                                640,
                                512,
                                212,
                            };
    UINT                    uHeight[] = {
                                480,
                                512,
                                8
                            };
    D3DFORMAT               fmt[] = {
                                D3DFMT_A8R8G8B8,
                                D3DFMT_X8R8G8B8,
                                D3DFMT_R5G6B5,
                                D3DFMT_X1R5G5B5,
                                D3DFMT_A1R5G5B5,
                                D3DFMT_A4R4G4B4,
                                D3DFMT_A8,
                                D3DFMT_P8,
                                D3DFMT_L8,
                                D3DFMT_A8L8,
                                D3DFMT_V8U8,
                                D3DFMT_L6V5U5,
                                D3DFMT_X8L8V8U8,
                                D3DFMT_Q8W8V8U8,
                                D3DFMT_V16U16,
                                D3DFMT_UYVY,
                                D3DFMT_YUY2,
                                D3DFMT_DXT1,
                                D3DFMT_DXT2,
                                D3DFMT_DXT3,
                                D3DFMT_DXT4,
                                D3DFMT_DXT5,
#ifdef UNDER_XBOX
                                D3DFMT_A8B8G8R8,
                                D3DFMT_B8G8R8A8,
                                D3DFMT_R4G4B4A4,
                                D3DFMT_R5G5B5A1,
                                D3DFMT_R8G8B8A8,
                                D3DFMT_R8B8,
                                D3DFMT_G8B8,
                                D3DFMT_AL8,
                                D3DFMT_L16,
                                D3DFMT_LIN_A1R5G5B5,
                                D3DFMT_LIN_A4R4G4B4,
                                D3DFMT_LIN_A8,
                                D3DFMT_LIN_A8B8G8R8,
                                D3DFMT_LIN_A8R8G8B8,
                                D3DFMT_LIN_B8G8R8A8,
                                D3DFMT_LIN_G8B8,
                                D3DFMT_LIN_R4G4B4A4,
                                D3DFMT_LIN_R5G5B5A1,
                                D3DFMT_LIN_R5G6B5,
                                D3DFMT_LIN_R6G5B5,
                                D3DFMT_LIN_R8B8,
                                D3DFMT_LIN_R8G8B8A8,
                                D3DFMT_LIN_X1R5G5B5,
                                D3DFMT_LIN_X8R8G8B8,
                                D3DFMT_LIN_A8L8,
                                D3DFMT_LIN_AL8,
                                D3DFMT_LIN_L16,
                                D3DFMT_LIN_L8,
#endif // UNDER_XBOX
                            };

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    for (i = 0; i < countof(fmt); i++) {

        LogBeginVariation(TEXT("Image surfaces with a format of %d"), fmt[i]);

        for (j = 0; j < countof(uWidth); j++) {

#ifdef UNDER_XBOX
                if (!(XGIsSwizzledFormat(fmt[i]) || IsCompressedFormat(fmt[i]))) {
#endif // UNDER_XBOX
                    hr = pd3dd->CreateImageSurface(uWidth[j], uHeight[j], fmt[i], &pd3ds);
#ifdef UNDER_XBOX
                }
                else {
                    hr = pd3dd->CreateImageSurface(uWidth[1], uHeight[1], fmt[i], &pd3ds);
                }
#endif // UNDER_XBOX
            if (FAILED(hr)) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::CreateImageSurface failed for a %d x %d surface [0x%X]"), uWidth[j], uHeight[j], hr);
                tr = TR_FAIL;
            }
            else {
                // Verify the surface
                hr = pd3ds->LockRect(&d3dlr, NULL, 0);
                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("IDirect3DSurface8::LockRect failed on created %d x %d surface [0x%X]"), uWidth[j], uHeight[j], hr);
                    tr = TR_FAIL;
                }
                else {
                    *((LPBYTE)d3dlr.pBits) = 0x3C;
                    pd3ds->UnlockRect();
                }
                pd3ds->Release();
            }
        }

        LogEndVariation();
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_CreateIndexBuffer(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8       pd3dd;
    LPDIRECT3DINDEXBUFFER8  pd3di;
    LPBYTE                  pdata;
    UINT                    i, j;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;
    D3DFORMAT               fmt[] = {
                                D3DFMT_INDEX16,
#ifndef UNDER_XBOX
                                D3DFMT_INDEX32,
#endif // !UNDER_XBOX
                            };
    DWORD                   dwUsage[] = {
                                0,
                                D3DUSAGE_POINTS,
                                D3DUSAGE_RTPATCHES,
                                D3DUSAGE_WRITEONLY,
                                D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
                                D3DUSAGE_RTPATCHES |D3DUSAGE_WRITEONLY,
                            };

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    for (i = 0; i < countof(dwUsage); i++) {

        LogBeginVariation(TEXT("IDirect3DDevice8::CreateIndexBuffer with a usage of %d"), dwUsage[i]);

        for (j = 0; j < countof(fmt); j++) {

            hr = pd3dd->CreateIndexBuffer(11561, dwUsage[i], fmt[j], D3DPOOL_MANAGED, &pd3di);
            if (FAILED(hr)) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::CreateIndexBuffer failed for a buffer with %d bit indices [0x%X]"), j ? 32 : 16, hr);
                tr = TR_FAIL;
            }
            else {
                // Verify the buffer
                hr = pd3di->Lock(13, 511, &pdata, 0);
                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("IDirect3DIndexBuffer8::Lock failed on a buffer with %d bit indices [0x%X]"), j ? 32 : 16, hr);
                    tr = TR_FAIL;
                }
                else {
                    *pdata = 0x3C;
                    pd3di->Unlock();
                }
                pd3di->Release();
            }
        }

        LogEndVariation();
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_CreateRenderTarget(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8       pd3dd;
    LPDIRECT3DSURFACE8      pd3ds;
    UINT                    i, j, k, l;
    D3DLOCKED_RECT          d3dlr;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;
    DWORD                   dwLockable[] = {
                                TRUE,
                                FALSE,
                            };
    UINT                    uWidth[] = {
                                640,
                                512,
                                211,
                            };
    UINT                    uHeight[] = {
                                480,
                                512,
                                7
                            };
    D3DFORMAT               fmtr[] = {
                                D3DFMT_X1R5G5B5,
                                D3DFMT_R5G6B5,
                                D3DFMT_X8R8G8B8,
                                D3DFMT_A8R8G8B8,
#ifdef UNDER_XBOX
                                D3DFMT_LIN_X1R5G5B5,
                                D3DFMT_LIN_R5G6B5,
                                D3DFMT_LIN_X8R8G8B8,
                                D3DFMT_LIN_A8R8G8B8,
#endif
                            };
    D3DMULTISAMPLE_TYPE     mst[] = {
                                D3DMULTISAMPLE_NONE,
#ifndef UNDER_XBOX
                                D3DMULTISAMPLE_2_SAMPLES,
                                D3DMULTISAMPLE_4_SAMPLES,
#endif
                            };

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    for (i = 0; i < countof(dwLockable); i++) {

        LogBeginVariation(TEXT("IDirect3DDevice8::CreateRenderTarget for a %slocable target"), dwLockable[i] ? TEXT("") : TEXT("non-"));

        for (j = 0; j < countof(fmtr); j++) {

            LogBeginVariation(TEXT("Render targets with a format of %d"), fmtr[j]);

            for (k = 0; k < countof(mst); k++) {

                LogBeginVariation(TEXT("Render targets with a multisample type of %d samples"), k * 2);

                for (l = 0; l < countof(uWidth); l++) {

#ifdef UNDER_XBOX
                    if (!XGIsSwizzledFormat(fmtr[j])) {
#endif // UNDER_XBOX
                        hr = pd3dd->CreateRenderTarget(uWidth[l], uHeight[l], fmtr[j], mst[k], dwLockable[i], &pd3ds);
#ifdef UNDER_XBOX
                    }
                    else {
                        hr = pd3dd->CreateRenderTarget(uWidth[1], uHeight[1], fmtr[j], mst[k], dwLockable[i], &pd3ds);
                    }
#endif // UNDER_XBOX
                    if (FAILED(hr)) {
                        Log(LOG_FAIL, TEXT("IDirect3DDevice8::CreateRenderTarget failed for a %d x %d surface [0x%X]"), uWidth[l], uHeight[l], hr);
                        tr = TR_FAIL;
                    }
                    else {
                        // Verify the surface
                        hr = pd3ds->LockRect(&d3dlr, NULL, 0);
                        if (FAILED(hr)) {
                            Log(LOG_FAIL, TEXT("IDirect3DSurface8::LockRect failed on created %d x %d surface [0x%X]"), uWidth[l], uHeight[l], hr);
                            tr = TR_FAIL;
                        }
                        else {
                            *((LPBYTE)d3dlr.pBits) = 0x3C;
                            pd3ds->UnlockRect();
                        }
                        pd3ds->Release();
                    }
                }

                LogEndVariation();
            }

            LogEndVariation();
        }

        LogEndVariation();
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_CreateTexture(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8       pd3dd;
    LPDIRECT3DTEXTURE8      pd3dt;
    UINT                    i, j, k, l, uNumFormats;
    D3DFORMAT*              pfmt;
    D3DLOCKED_RECT          d3dlr;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;
    DWORD                   dwUsage[] = {
                                0,
                                D3DUSAGE_RENDERTARGET,
                                D3DUSAGE_DEPTHSTENCIL,
                            };
    UINT                    uLevels[] = {
                                0,
                                1,
                                2,
                            };
    UINT                    uWidth[] = {
                                512,
                                128,
                                212,
                            };
    UINT                    uHeight[] = {
                                512,
                                256,
                                8,
                            };
    D3DFORMAT               fmt[] = {
                                D3DFMT_A8R8G8B8,
                                D3DFMT_X8R8G8B8,
                                D3DFMT_R5G6B5,
                                D3DFMT_X1R5G5B5,
                                D3DFMT_A1R5G5B5,
                                D3DFMT_A4R4G4B4,
                                D3DFMT_A8,
                                D3DFMT_P8,
                                D3DFMT_L8,
                                D3DFMT_A8L8,
                                D3DFMT_V8U8,
                                D3DFMT_L6V5U5,
                                D3DFMT_X8L8V8U8,
                                D3DFMT_Q8W8V8U8,
                                D3DFMT_V16U16,
                                D3DFMT_UYVY,
                                D3DFMT_YUY2,
                                D3DFMT_DXT1,
                                D3DFMT_DXT2,
                                D3DFMT_DXT3,
                                D3DFMT_DXT4,
                                D3DFMT_DXT5,
#ifdef UNDER_XBOX
                                D3DFMT_A8B8G8R8,
                                D3DFMT_B8G8R8A8,
                                D3DFMT_R4G4B4A4,
                                D3DFMT_R5G5B5A1,
                                D3DFMT_R8G8B8A8,
                                D3DFMT_R8B8,
                                D3DFMT_G8B8,
                                D3DFMT_AL8,
                                D3DFMT_L16,
                                D3DFMT_LIN_A1R5G5B5,
                                D3DFMT_LIN_A4R4G4B4,
                                D3DFMT_LIN_A8,
                                D3DFMT_LIN_A8B8G8R8,
                                D3DFMT_LIN_A8R8G8B8,
                                D3DFMT_LIN_B8G8R8A8,
                                D3DFMT_LIN_G8B8,
                                D3DFMT_LIN_R4G4B4A4,
                                D3DFMT_LIN_R5G5B5A1,
                                D3DFMT_LIN_R5G6B5,
                                D3DFMT_LIN_R6G5B5,
                                D3DFMT_LIN_R8B8,
                                D3DFMT_LIN_R8G8B8A8,
                                D3DFMT_LIN_X1R5G5B5,
                                D3DFMT_LIN_X8R8G8B8,
                                D3DFMT_LIN_A8L8,
                                D3DFMT_LIN_AL8,
                                D3DFMT_LIN_L16,
                                D3DFMT_LIN_L8,
#endif // UNDER_XBOX
                            };

    D3DFORMAT               fmtr[] = {
                                D3DFMT_X1R5G5B5,
                                D3DFMT_R5G6B5,
                                D3DFMT_X8R8G8B8,
                                D3DFMT_A8R8G8B8,
#ifdef UNDER_XBOX
                                D3DFMT_LIN_X1R5G5B5,
                                D3DFMT_LIN_R5G6B5,
                                D3DFMT_LIN_X8R8G8B8,
                                D3DFMT_LIN_A8R8G8B8,
#endif // UNDER_XBOX
                            };

    D3DFORMAT               fmtd[] = {
                                D3DFMT_D16_LOCKABLE,
                                D3DFMT_D16,
                                D3DFMT_D24S8,
#ifdef UNDER_XBOX
                                D3DFMT_F16,
                                D3DFMT_F24S8,
                                D3DFMT_LIN_D24S8,
                                D3DFMT_LIN_F24S8,
                                D3DFMT_LIN_D16,
                                D3DFMT_LIN_F16,
#endif // UNDER_XBOX
                            };

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    for (i = 0; i < countof(dwUsage); i++) {

        LogBeginVariation(TEXT("IDirect3DDevice8::CreateTexture with a usage of %d"), dwUsage[i]);

        if (i == 2) {
            pfmt = fmtd;
            uNumFormats = countof(fmtd);
        }
        else if (i == 1) {
            pfmt = fmtr;
            uNumFormats = countof(fmtr);
        }
        else {
            pfmt = fmt;
            uNumFormats = countof(fmt);
        }

        for (j = 0; j < uNumFormats; j++) {

            LogBeginVariation(TEXT("Textures with a format of %d"), pfmt[j]);

            for (k = 0; k < countof(uLevels); k++) {

                for (l = 0; l < countof(uWidth); l++) {

#ifdef UNDER_XBOX
                    if (!(XGIsSwizzledFormat(pfmt[j]) || IsCompressedFormat(pfmt[j]))) {
#endif // UNDER_XBOX
                        hr = pd3dd->CreateTexture(uWidth[l], uHeight[l], 1, dwUsage[i], pfmt[j], D3DPOOL_MANAGED, &pd3dt);
#ifdef UNDER_XBOX
                    }
                    else {
                        hr = pd3dd->CreateTexture(uWidth[l%2], uHeight[l%2], uLevels[k], dwUsage[i], pfmt[j], D3DPOOL_MANAGED, &pd3dt);
                    }
#endif // UNDER_XBOX
                    if (FAILED(hr)) {
                        Log(LOG_FAIL, TEXT("IDirect3DDevice8::CreateTexture failed given a %d x %d texture with %d levels [0x%X]"), uWidth[l], uHeight[l], uLevels[k], hr);
                        tr = TR_FAIL;
                    }
                    else {
                        // Verify the texture
                        hr = pd3dt->LockRect(0, &d3dlr, NULL, 0);
                        if (FAILED(hr)) {
                            Log(LOG_FAIL, TEXT("IDirect3DTexture8::LockRect failed on created texture [0x%X]"), hr);
                            tr = TR_FAIL;
                        }
                        else {
                            *((LPBYTE)d3dlr.pBits) = 0x3C;
                            pd3dt->UnlockRect(0);
                        }
                        pd3dt->Release();
                    }
                }
            }

            LogEndVariation();
        }

        LogEndVariation();
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_CreateVertexBuffer(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8       pd3dd;
    LPDIRECT3DVERTEXBUFFER8 pd3dr;
    LPBYTE                  pdata;
    UINT                    i, j;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;
    DWORD                   dwFVF[] = {
                                0,
                                D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1,
                                D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEXCOORDSIZE3(0),
                                D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX4 | D3DFVF_XYZB3,
                                D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1,
                                D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1,
                            };
    DWORD                   dwUsage[] = {
                                0,
                                D3DUSAGE_POINTS,
                                D3DUSAGE_RTPATCHES,
                                D3DUSAGE_WRITEONLY,
                                D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
                                D3DUSAGE_RTPATCHES | D3DUSAGE_WRITEONLY,
                            };

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    for (i = 0; i < countof(dwUsage); i++) {

        LogBeginVariation(TEXT("IDirect3DDevice8::CreateVertexBuffer with a usage of %d"), dwUsage[i]);

        for (j = 0; j < countof(dwFVF); j++) {

            hr = pd3dd->CreateVertexBuffer(2147, dwUsage[i], dwFVF[j], D3DPOOL_MANAGED, &pd3dr);
            if (FAILED(hr)) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::CreateVertexBuffer failed for a buffer with an FVF of 0x%X [0x%X]"), dwFVF[j], hr);
                tr = TR_FAIL;
            }
            else {
                // Verify the buffer
                hr = pd3dr->Lock(13, 511, &pdata, 0);
                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("IDirect3DVertexBuffer8::Lock failed on 0x%X FVF buffer [0x%X]"), dwFVF[j], hr);
                    tr = TR_FAIL;
                }
                else {
                    *pdata = 0x3C;
                    pd3dr->Unlock();
                }
                pd3dr->Release();
            }
        }

        LogEndVariation();
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_CreateVolumeTexture(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8       pd3dd;
    LPDIRECT3DVOLUMETEXTURE8 pd3dtv;
    UINT                    i, j, k, l;
    D3DLOCKED_BOX           d3dlb;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;
    DWORD                   dwUsage[] = {
                                0,
                            };
    UINT                    uLevels[] = {
                                0,
                                1,
                                2,
                            };
    UINT                    uWidth[] = {
                                256,
                                128,
                                1,
                            };
    UINT                    uHeight[] = {
                                64,
                                256,
                                8,
                            };
    UINT                    uDepth[] = {
                                256,
                                64,
                                512,
                            };
    D3DFORMAT               fmt[] = {
                                D3DFMT_A8R8G8B8,
                                D3DFMT_X8R8G8B8,
                                D3DFMT_R5G6B5,
                                D3DFMT_X1R5G5B5,
                                D3DFMT_A1R5G5B5,
                                D3DFMT_A4R4G4B4,
                                D3DFMT_A8,
                                D3DFMT_P8,
                                D3DFMT_L8,
                                D3DFMT_A8L8,
                                D3DFMT_V8U8,
                                D3DFMT_L6V5U5,
                                D3DFMT_X8L8V8U8,
                                D3DFMT_Q8W8V8U8,
                                D3DFMT_V16U16,
#ifndef UNDER_XBOX
                                D3DFMT_UYVY,
                                D3DFMT_YUY2,
#endif
                                D3DFMT_DXT1,
                                D3DFMT_DXT2,
                                D3DFMT_DXT3,
                                D3DFMT_DXT4,
                                D3DFMT_DXT5,
#ifdef UNDER_XBOX
                                D3DFMT_A8B8G8R8,
                                D3DFMT_B8G8R8A8,
                                D3DFMT_R4G4B4A4,
                                D3DFMT_R5G5B5A1,
                                D3DFMT_R8G8B8A8,
                                D3DFMT_R8B8,
                                D3DFMT_G8B8,
                                D3DFMT_AL8,
                                D3DFMT_L16,
#if 0 // Non-swizzled textures cannot be volume textures
                                D3DFMT_LIN_A1R5G5B5,
                                D3DFMT_LIN_A4R4G4B4,
                                D3DFMT_LIN_A8,
                                D3DFMT_LIN_A8B8G8R8,
                                D3DFMT_LIN_A8R8G8B8,
                                D3DFMT_LIN_B8G8R8A8,
                                D3DFMT_LIN_G8B8,
                                D3DFMT_LIN_R4G4B4A4,
                                D3DFMT_LIN_R5G5B5A1,
                                D3DFMT_LIN_R5G6B5,
                                D3DFMT_LIN_R6G5B5,
                                D3DFMT_LIN_R8B8,
                                D3DFMT_LIN_R8G8B8A8,
                                D3DFMT_LIN_X1R5G5B5,
                                D3DFMT_LIN_X8R8G8B8,
                                D3DFMT_LIN_A8L8,
                                D3DFMT_LIN_AL8,
                                D3DFMT_LIN_L16,
                                D3DFMT_LIN_L8,
#endif
#endif // UNDER_XBOX
                            };

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    for (i = 0; i < countof(dwUsage); i++) {

        LogBeginVariation(TEXT("IDirect3DDevice8::CreateVolumeTexture with a usage of %d"), dwUsage[i]);

        for (j = 0; j < countof(fmt); j++) {

            LogBeginVariation(TEXT("Volume textures with a format of %d"), fmt[j]);

            for (k = 0; k < countof(uLevels); k++) {

                for (l = 0; l < countof(uWidth); l++) {

                    hr = pd3dd->CreateVolumeTexture(uWidth[l], uHeight[l], uDepth[l], uLevels[k], dwUsage[i], fmt[j], D3DPOOL_MANAGED, &pd3dtv);
                    if (FAILED(hr)) {
                        Log(LOG_FAIL, TEXT("IDirect3DDevice8::CreateVolumeTexture failed given a %d x %d x%d texture with %d levels [0x%X]"), uWidth[l], uHeight[l], uDepth[l], uLevels[k], hr);
                        tr = TR_FAIL;
                    }
                    else {
                        // Verify the texture
                        hr = pd3dtv->LockBox(0, &d3dlb, NULL, 0);
                        if (FAILED(hr)) {
                            Log(LOG_FAIL, TEXT("IDirect3DVolumeTexture8::LockBox failed on created volume texture [0x%X]"), hr);
                            tr = TR_FAIL;
                        }
                        else {
                            *((LPBYTE)d3dlb.pBits) = 0x3C;
                            pd3dtv->UnlockBox(0);
                        }
                        pd3dtv->Release();
                    }
                }
            }

            LogEndVariation();
        }

        LogEndVariation();
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_ApplyStateBlock(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    DWORD               dwSet, dwGet;
    DWORD               dwToken;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    hr = pd3dd->BeginStateBlock();
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::BeginStateBlock"))) {
        return TR_ABORT;
    }

    dwSet = (DWORD)RGBA_MAKE(127, 32, 255, 69);

    hr = pd3dd->SetRenderState(D3DRS_TEXTUREFACTOR, dwSet);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        return TR_ABORT;
    }

    hr = pd3dd->EndStateBlock(&dwToken);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::EndStateBlock"))) {
        return TR_ABORT;
    }

    hr = pd3dd->SetRenderState(D3DRS_TEXTUREFACTOR, 0);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        return TR_ABORT;
    }

    hr = pd3dd->ApplyStateBlock(dwToken);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::ApplyStateBlock"))) {
        return TR_FAIL;
    }

    hr = pd3dd->GetRenderState(D3DRS_TEXTUREFACTOR, &dwGet);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetRenderState"))) {
        return TR_ABORT;
    }

    if (dwGet != dwSet) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::ApplyStateBlock failed to correctly apply the block"));
        tr = TR_FAIL;
    }

    hr = pd3dd->DeleteStateBlock(dwToken);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::DeleteStateBlock"))) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_BeginStateBlock(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    DWORD               dwToken;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    hr = pd3dd->BeginStateBlock();
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::BeginStateBlock"))) {
        return TR_FAIL;
    }

    hr = pd3dd->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        return TR_ABORT;
    }

#ifdef INVALID_TESTS
    hr = pd3dd->BeginStateBlock();
    if (hr != D3DERR_INVALIDCALL) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::BeginStateBlock returned 0x%X when called with recording already in progress"), hr);
        tr = TR_FAIL;
    }
#endif // INVALID_TESTS

    hr = pd3dd->EndStateBlock(&dwToken);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::EndStateBlock"))) {
        return TR_ABORT;
    }

    hr = pd3dd->DeleteStateBlock(dwToken);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::DeleteStateBlock"))) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_EndStateBlock(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    DWORD               dwToken;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

#ifdef INVALID_TESTS
    hr = pd3dd->EndStateBlock(&dwToken);
    if (hr != D3DERR_INVALIDCALL) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::EndStateBlock returned 0x%X when called without a prior call to BeginStateBlock"), hr);
        tr = TR_FAIL;
    }
#endif // INVALID_TESTS

    hr = pd3dd->BeginStateBlock();
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::BeginStateBlock"))) {
        return TR_ABORT;
    }

    hr = pd3dd->EndStateBlock(&dwToken);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::EndStateBlock"))) {
        return TR_FAIL;
    }

    hr = pd3dd->DeleteStateBlock(dwToken);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::DeleteStateBlock"))) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_CreateStateBlock(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    UINT                i;
    DWORD               dwToken;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;
    D3DSTATEBLOCKTYPE   sbt[] = {
                            D3DSBT_ALL,
                            D3DSBT_VERTEXSTATE,
                            D3DSBT_PIXELSTATE,
                        };

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    for (i = 0; i < countof(sbt); i++) {

        hr = pd3dd->CreateStateBlock(sbt[i], &dwToken);
        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::CreateStateBlock with type %d failed [0x%X]"), sbt[i], hr);
            tr = TR_FAIL;
        }
        else {
            hr = pd3dd->DeleteStateBlock(dwToken);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::DeleteStateBlock"))) {
                tr = TR_ABORT;
            }
        }
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_DeleteStateBlock(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    DWORD               dwToken;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

#ifdef INVALID_TESTS
    hr = pd3dd->DeleteStateBlock(0);
    if (hr != D3DERR_INVALIDCALL) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::DeleteStateBlock returned 0x%X when called on a non-existant block"), hr);
        return TR_FAIL;
    }
#endif // INVALID_TESTS

    hr = pd3dd->BeginStateBlock();
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::BeginStateBlock"))) {
        return TR_ABORT;
    }

    hr = pd3dd->EndStateBlock(&dwToken);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::EndStateBlock"))) {
        return TR_ABORT;
    }

    hr = pd3dd->DeleteStateBlock(dwToken);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::DeleteStateBlock on a recorded block"))) {
        tr = TR_FAIL;
    }

    hr = pd3dd->CreateStateBlock(D3DSBT_ALL, &dwToken);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::BeginStateBlock"))) {
        return TR_ABORT;
    }

    hr = pd3dd->DeleteStateBlock(dwToken);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::DeleteStateBlock on a created block"))) {
        tr = TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_CaptureStateBlock(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    DWORD               dwSet, dwGet;
    DWORD               dwToken;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    hr = pd3dd->BeginStateBlock();
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::BeginStateBlock"))) {
        return TR_ABORT;
    }

    hr = pd3dd->SetRenderState(D3DRS_TEXTUREFACTOR, rand() % 0xFFFFFFFF);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        return TR_ABORT;
    }

    hr = pd3dd->EndStateBlock(&dwToken);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::EndStateBlock"))) {
        return TR_ABORT;
    }

    dwSet = (DWORD)RGBA_MAKE(27, 132, 55, 169);

    hr = pd3dd->SetRenderState(D3DRS_TEXTUREFACTOR, dwSet);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        return TR_ABORT;
    }

    hr = pd3dd->CaptureStateBlock(dwToken);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CaptureStateBlock"))) {
        return TR_FAIL;
    }

    hr = pd3dd->ApplyStateBlock(dwToken);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::ApplyStateBlock"))) {
        return TR_ABORT;
    }

    hr = pd3dd->GetRenderState(D3DRS_TEXTUREFACTOR, &dwGet);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetRenderState"))) {
        return TR_ABORT;
    }

    if (dwGet != dwSet) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::CaptureStateBlock failed to correctly capture the block"));
        tr = TR_FAIL;
    }

    hr = pd3dd->DeleteStateBlock(dwToken);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::DeleteStateBlock"))) {
        return TR_ABORT;
    }

    return tr;
}

#ifndef UNDER_XBOX

//******************************************************************************
TESTPROCAPI TDEV8_GetClipStatus(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_SetClipStatus(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    UINT                i;
    D3DCLIPSTATUS8      d3dcsSet, d3dcsGet;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;
    DWORD               dwClip[] = {
// ##HACK
//                            D3DCS_ALL,
                            D3DCS_BACK | D3DCS_BOTTOM | D3DCS_FRONT | D3DCS_LEFT | D3DCS_RIGHT | D3DCS_TOP | D3DCS_PLANE0 | D3DCS_PLANE1 | D3DCS_PLANE1 | D3DCS_PLANE3 | D3DCS_PLANE4 | D3DCS_PLANE5,
                            D3DCS_BACK,
                            D3DCS_BOTTOM,
                            D3DCS_FRONT,
                            D3DCS_LEFT,
                            D3DCS_RIGHT,
                            D3DCS_TOP,
                            D3DCS_PLANE0,
                            D3DCS_PLANE1,
                            D3DCS_PLANE2,
                            D3DCS_PLANE3,
                            D3DCS_PLANE4,
                            D3DCS_PLANE5,
                            D3DCS_BACK | D3DCS_BOTTOM | D3DCS_FRONT | D3DCS_LEFT | D3DCS_RIGHT | D3DCS_TOP | D3DCS_PLANE0 | D3DCS_PLANE1 | D3DCS_PLANE1 | D3DCS_PLANE3 | D3DCS_PLANE4 | D3DCS_PLANE5,
                        };

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    Log(LOG_FAIL, TEXT("Line 136 of d3dtypes8.h incorrectly puts a semicolon at the end of the #define for D3DCS_ALL"));

    for (i = 0; i < countof(dwClip) - 1; i++) {

        d3dcsSet.ClipUnion = dwClip[i];
        d3dcsSet.ClipIntersection = dwClip[i+1];

        hr = pd3dd->SetClipStatus(&d3dcsSet);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetClipStatus"))) {
            tr = TR_FAIL;
        }
        else {
            hr = pd3dd->GetClipStatus(&d3dcsGet);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetClipStatus"))) {
                tr = TR_FAIL;
            }
            else {
                if (memcmp(&d3dcsSet, &d3dcsGet, sizeof(D3DCLIPSTATUS8))) {
                    Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetClipStatus failed to set the clip status for union 0x%X and intersection 0x%X"), d3dcsSet.ClipUnion, d3dcsSet.ClipIntersection);
                    tr = TR_FAIL;
                }
            }
        }
    }

    return tr;
}

#endif // !UNDER_XBOX

//******************************************************************************
#define TestRenderState(state, value)                       \
    dwRStateSet = (DWORD)(value);                           \
    hr = pd3dd->SetRenderState(state, dwRStateSet);         \
    if (FAILED(hr)) {                                       \
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetRenderState failed for ") TEXT(#state)); \
        tr = TR_FAIL;                                       \
    }                                                       \
    else {                                                  \
        hr = pd3dd->GetRenderState(state, &dwRStateGet);    \
        if (FAILED(hr)) {                                   \
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetRenderState failed for ") TEXT(#state)); \
            tr = TR_FAIL;                                   \
        }                                                   \
        else {                                              \
            if (dwRStateSet != dwRStateGet) {               \
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetRenderState returned %d for a state set as ") TEXT(#state), dwRStateGet); \
                tr = TR_FAIL;                               \
            }                                               \
        }                                                   \
    }

//******************************************************************************
#define FailRenderState(state, value)                       \
    dwRStateSet = (DWORD)(value);                           \
    hr = pd3dd->SetRenderState(state, dwRStateSet);         \
    if (hr != E_NOTIMPL) {                                  \
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetRenderState returned 0x%X instead of E_NOTIMPL for ") TEXT(#state), hr); \
        tr = TR_FAIL;                                       \
    }                                                       \
                                                            \
    hr = pd3dd->GetRenderState(state, &dwRStateGet);        \
    if (hr != E_NOTIMPL) {                                  \
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetRenderState returned 0x%X instead of E_NOTIMPL for ") TEXT(#state), hr); \
        tr = TR_FAIL;                                       \
    }

//******************************************************************************
TESTPROCAPI TDEV8_GetRenderState(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    DWORD               dwState;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    hr = pd3dd->GetRenderState(D3DRS_SHADEMODE, &dwState);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetRenderState(D3DRS_SHADEMODE, ..."))) {
        tr = TR_FAIL;
    }
    else {
        if (dwState != D3DSHADE_GOURAUD) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetRenderState(D3DRS_SHADEMODE, ...) returned %d"), dwState);
            tr = TR_FAIL;
        }
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_SetRenderState(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    DWORD               dwRStateSet, dwRStateGet;
#ifndef UNDER_XBOX
    D3DLINEPATTERN      d3dlp;
#endif
    float               fStart, fEnd, fDensity;
    float               fPSize, fPSizeMin, fPSizeMax, fPScaleA, fPScaleB, fPScaleC;
    DWORD               dwBlock;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    hr = pd3dd->CreateStateBlock(D3DSBT_ALL, &dwBlock);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateStateBlock"))) {
        return TR_ABORT;
    }

    TestRenderState(D3DRS_ZENABLE, D3DZB_USEW);                        // Z Enable
    TestRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);                // Fill Mode
    TestRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);                   // Shade Mode
#ifndef UNDER_XBOX
    d3dlp.wRepeatFactor = 1;
    d3dlp.wLinePattern = 0xF3C7;
    TestRenderState(D3DRS_LINEPATTERN, *((DWORD*)&d3dlp));             // Line Pattern
#endif // !UNDER_XBOX
    TestRenderState(D3DRS_ZWRITEENABLE, FALSE);                        // Z Writes
    TestRenderState(D3DRS_ALPHATESTENABLE, TRUE);                      // Alpha tests
#ifndef UNDER_XBOX
    TestRenderState(D3DRS_LASTPIXEL, FALSE);                           // Last pixel
#endif // !UNDER_XBOX
    TestRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);                // Source blend
    TestRenderState(D3DRS_DESTBLEND, D3DBLEND_INVDESTALPHA);           // Destination blend
    TestRenderState(D3DRS_CULLMODE, D3DCULL_CW);                       // Back-face culling mode
    TestRenderState(D3DRS_ZFUNC, D3DCMP_GREATEREQUAL);                 // Z Comparison function
    TestRenderState(D3DRS_ALPHAREF, 1);                                // Reference alpha value
    TestRenderState(D3DRS_ALPHAFUNC, D3DCMP_NEVER);                    // Alpha comparison function
    TestRenderState(D3DRS_DITHERENABLE, TRUE);                         // Enable dithering
    TestRenderState(D3DRS_ALPHABLENDENABLE, TRUE);                     // Enable alpha blending
    TestRenderState(D3DRS_FOGENABLE, TRUE);                            // Enable fog
    TestRenderState(D3DRS_SPECULARENABLE, TRUE);                       // Specular highlights
    TestRenderState(D3DRS_FOGCOLOR, 
                    RGBA_MAKE(rand()%256, rand()%256, rand()%256, rand()%256)); // Fog Color
    TestRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP);                   // Fog mode
    fStart = (float)(rand() % 10);
    TestRenderState(D3DRS_FOGSTART, *((LPDWORD)(&fStart)));            // Fog start
    fEnd = (float)(rand()%90 + 10);
    TestRenderState(D3DRS_FOGEND, *((LPDWORD)(&fEnd)));                // Fog end
    fDensity = RND();
    TestRenderState(D3DRS_FOGDENSITY, *((LPDWORD)(&fDensity)));        // Fog density
    TestRenderState(D3DRS_EDGEANTIALIAS, TRUE);                        // Antialias edges
    TestRenderState(D3DRS_ZBIAS, rand()%16);                           // Z bias
    TestRenderState(D3DRS_RANGEFOGENABLE, TRUE);                       // Enable range-based fog
    TestRenderState(D3DRS_STENCILENABLE, TRUE);                        // Enable stenciling
    TestRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_REPLACE);          // Operation to perform on fail
    TestRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_REPLACE);         // Operation if stencil test passes and z test fails
    TestRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);          // Operation if stencil and z tests pass
    TestRenderState(D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL);              // Stencil comparison function
    TestRenderState(D3DRS_STENCILREF, 1);                              // Stencil test reference value
    TestRenderState(D3DRS_STENCILMASK, 0x5C6F3D12);                    // Stencil mask
    TestRenderState(D3DRS_STENCILWRITEMASK, 0x5CC7E632);               // Stecil write mask
    TestRenderState(D3DRS_TEXTUREFACTOR, 
                    RGBA_MAKE(rand()%256, rand()%256, rand()%256, rand()%256)); // Texture factor
    TestRenderState(D3DRS_WRAP0, 
                    D3DWRAPCOORD_0 | D3DWRAPCOORD_1 | D3DWRAPCOORD_2 | D3DWRAPCOORD_3); // Texture wrapping
    TestRenderState(D3DRS_WRAP1, 
                    D3DWRAPCOORD_0 | D3DWRAPCOORD_1 | D3DWRAPCOORD_2 | D3DWRAPCOORD_3); // Texture wrapping
    TestRenderState(D3DRS_WRAP2, 
                    D3DWRAPCOORD_0 | D3DWRAPCOORD_1 | D3DWRAPCOORD_2 | D3DWRAPCOORD_3); // Texture wrapping
    TestRenderState(D3DRS_WRAP3, 
                    D3DWRAPCOORD_0 | D3DWRAPCOORD_1 | D3DWRAPCOORD_2 | D3DWRAPCOORD_3); // Texture wrapping
#ifndef UNDER_XBOX
    TestRenderState(D3DRS_WRAP4, 
                    D3DWRAPCOORD_0 | D3DWRAPCOORD_1 | D3DWRAPCOORD_2 | D3DWRAPCOORD_3); // Texture wrapping
    TestRenderState(D3DRS_WRAP5, 
                    D3DWRAPCOORD_0 | D3DWRAPCOORD_1 | D3DWRAPCOORD_2 | D3DWRAPCOORD_3); // Texture wrapping
    TestRenderState(D3DRS_WRAP6, 
                    D3DWRAPCOORD_0 | D3DWRAPCOORD_1 | D3DWRAPCOORD_2 | D3DWRAPCOORD_3); // Texture wrapping
    TestRenderState(D3DRS_WRAP7, 
                    D3DWRAPCOORD_0 | D3DWRAPCOORD_1 | D3DWRAPCOORD_2 | D3DWRAPCOORD_3); // Texture wrapping
    TestRenderState(D3DRS_CLIPPING, FALSE);                            // Clipping
#endif // !UNDER_XBOX
    TestRenderState(D3DRS_LIGHTING, FALSE);                            // Lighting
    TestRenderState(D3DRS_AMBIENT, 
                    RGBA_MAKE(rand()%256, rand()%256, rand()%256, rand()%256)); // Ambient
#ifndef UNDER_XBOX
    TestRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_EXP);                  // Fog vertex mode
#endif // !UNDER_XBOX
    TestRenderState(D3DRS_COLORVERTEX, FALSE);                         // Per-vertex color
    TestRenderState(D3DRS_LOCALVIEWER, FALSE);                         // Camera-relative specular
    TestRenderState(D3DRS_NORMALIZENORMALS, TRUE);                     // Normalization of vertex normals
    TestRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR2);       // Diffuse material source
    TestRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR1);      // Specular material source
    TestRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);     // Ambient material source
    TestRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);      // Emissive material source
    TestRenderState(D3DRS_VERTEXBLEND, D3DVBF_2WEIGHTS);               // Vertex blend
#ifndef UNDER_XBOX
    TestRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE4);             // Clip plane enable
#endif // !UNDER_XBOX
    fPSize = 9.6f;
    TestRenderState(D3DRS_POINTSIZE, *((LPDWORD)(&fPSize)));           // Sprite point size
    fPSizeMin = 9.8f;
    TestRenderState(D3DRS_POINTSIZE_MIN, *((LPDWORD)(&fPSizeMin)));    // Minimum sprite point size
    TestRenderState(D3DRS_POINTSPRITEENABLE, TRUE);                    // Enable point sprite texturing
    TestRenderState(D3DRS_POINTSCALEENABLE, TRUE);                     // Enable point scaling
    fPScaleA = 2.3f;
    TestRenderState(D3DRS_POINTSCALE_A, *((LPDWORD)&fPScaleA));      // Point scale attenuation
    fPScaleB = 7.4f;
    TestRenderState(D3DRS_POINTSCALE_B, *((LPDWORD)(&fPScaleB)));      // Point scale attenuation
    fPScaleC = 32.1f;
    TestRenderState(D3DRS_POINTSCALE_C, *((LPDWORD)(&fPScaleC)));      // Point scale attenuation
    TestRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);                // Enable multisampling
    TestRenderState(D3DRS_MULTISAMPLEMASK, 0x234C4F7A);                // Multisample mask value
    TestRenderState(D3DRS_PATCHSEGMENTS, 3);                           // Patch segments
#ifndef UNDER_XBOX
    TestRenderState(D3DRS_DEBUGMONITORTOKEN, 0);                       // ???
#endif // !UNDER_XBOX
    fPSizeMax = 9.4f;
    TestRenderState(D3DRS_POINTSIZE_MAX, *((LPDWORD)(&fPSizeMax)));    // Maximum sprite point size
#ifndef UNDER_XBOX
    TestRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE)              // Indexed vertex blending
#endif // !UNDER_XBOX
    TestRenderState(D3DRS_COLORWRITEENABLE, 
                    D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_GREEN); // Per-channel target write
#ifndef UNDER_XBOX
    FailRenderState(D3DRS_ZVISIBLE, TRUE);                             // Enable z checking
#endif // !UNDER_XBOX

#ifndef UNDER_XBOX
    hr = pd3dd->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, TRUE);  // Enable software vertex processing
    if (hr != E_NOTIMPL) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetRenderState returned 0x%X instead of E_NOTIMPL for D3DRS_SOFTWAREVERTEXPROCESSING"), hr);
        tr = TR_FAIL;
    }
#endif // !UNDER_XBOX

    // Clean up the mess we made
    hr = pd3dd->ApplyStateBlock(dwBlock);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::ApplyStateBlock"))) {
        tr = TR_ABORT;
    }

    pd3dd->DeleteStateBlock(dwBlock);

    ReleaseDevice8();

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetRenderTarget(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    LPDIRECT3DSURFACE8  pd3dsBack, pd3dsTarget;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    hr = pd3dd->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pd3dsBack);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetBackBuffer"))) {
        return TR_ABORT;
    }

    hr = pd3dd->GetRenderTarget(&pd3dsTarget);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetRenderTarget"))) {
        tr = TR_FAIL;
    }
    else {
        if (pd3dsBack != pd3dsTarget) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetRenderTarget returned a target other than the back buffer"));
            tr = TR_FAIL;
        }

        pd3dsTarget->Release();
    }

    pd3dsBack->Release();

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_SetRenderTarget(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    LPDIRECT3DSURFACE8  pd3ds1, pd3dsd1, pd3ds2, pd3dsd2, pd3ds3, pd3dsd3;
    LPDIRECT3DTEXTURE8  pd3dt;
    ULONG               uRef1, uRef2;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    // Save the original target
    hr = pd3dd->GetRenderTarget(&pd3ds1);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetRenderTarget"))) {
        return TR_ABORT;
    }

    hr = pd3dd->GetDepthStencilSurface(&pd3dsd1);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDepthStencilSurface"))) {
        pd3ds1->Release();
        return TR_ABORT;
    }

    // Create a new target
#ifndef UNDER_XBOX
    hr = pd3dd->CreateDepthStencilSurface(512, 512, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, &pd3dsd2);
#else
    hr = pd3dd->CreateDepthStencilSurface(512, 512, D3DFMT_LIN_D24S8, D3DMULTISAMPLE_NONE, &pd3dsd2);
#endif // UNDER_XBOX
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateDepthStencilSurface"))) {
        pd3dsd1->Release();
        pd3ds1->Release();
        return TR_ABORT;
    }

    hr = pd3dd->CreateTexture(512, 512, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pd3dt);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateTexture"))) {
        pd3dsd2->Release();
        pd3ds1->Release();
        pd3dsd1->Release();
        return TR_ABORT;
    }

    hr = pd3dt->GetSurfaceLevel(0, &pd3ds2);
    pd3dt->Release();
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
        pd3dsd2->Release();
        pd3ds1->Release();
        pd3dsd1->Release();
        return TR_ABORT;
    }

    pd3ds1->AddRef();
    uRef1 = pd3ds1->Release();
    pd3dsd1->AddRef();
    uRef2 = pd3dsd1->Release();

    // Set the target
    hr = pd3dd->SetRenderTarget(pd3ds2, pd3dsd2);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderTarget"))) {
        pd3ds2->Release();
        pd3dsd2->Release();
        pd3ds1->Release();
        pd3dsd1->Release();
        return TR_FAIL;
    }

    // Verify the reference counts of the old buffers were correctly decremented
#if 0
    pd3ds1->AddRef();
    pd3dsd1->AddRef();
    if (pd3ds1->Release() != uRef1 + 1) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetRenderTarget failed to decrement the ref count on the color buffer being replaced"));
        tr = TR_FAIL;
    }
    if (pd3dsd1->Release() != uRef2 + 1) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetRenderTarget failed to decrement the ref count on the depth buffer being replaced"));
        tr = TR_FAIL;
    }

    // Verify the reference counts of the new buffers were correctly incremented
    pd3ds2->AddRef();
    pd3dsd2->AddRef();
    if (pd3ds2->Release() != 2) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetRenderTarget failed to increment the ref count on the color buffer being set"));
        tr = TR_FAIL;
    }
    if (pd3dsd2->Release() != 2) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetRenderTarget failed to increment the ref count on the depth buffer being set"));
        tr = TR_FAIL;
    }
#endif

    // Verify the target was correctly set
    hr = pd3dd->GetRenderTarget(&pd3ds3);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetRenderTarget"))) {
        pd3ds2->Release();
        pd3dsd2->Release();
        pd3ds1->Release();
        pd3dsd1->Release();
        return TR_ABORT;
    }

    hr = pd3dd->GetDepthStencilSurface(&pd3dsd3);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDepthStencilSurface"))) {
        pd3ds2->Release();
        pd3dsd2->Release();
        pd3ds1->Release();
        pd3dsd1->Release();
        return TR_ABORT;
    }

    if (pd3ds3 != pd3ds2) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetRenderTarget failed to correctly set the color buffer"));
        tr = TR_FAIL;
    }
    if (pd3dsd3 != pd3dsd2) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetRenderTarget failed to correctly set the depth buffer"));
        tr = TR_FAIL;
    }

    pd3ds3->Release();
    pd3dsd3->Release();

    // Restore the original target
    hr = pd3dd->SetRenderTarget(pd3ds1, pd3dsd1);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderTarget"))) {
        tr = TR_ABORT;
    }

    // Release
    pd3ds2->Release();
    pd3dsd2->Release();
    pd3ds1->Release();
    pd3dsd1->Release();

    return tr;
}

/*
//******************************************************************************
TESTPROCAPI TDEV8_SetRenderTarget(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    LPDIRECT3DSURFACE8  pd3ds1, pd3dsd1, pd3ds2, pd3dsd2, pd3ds3, pd3dsd3;
    LPDIRECT3DTEXTURE8  pd3dt;
    ULONG               uRef1, uRef2;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    // Save the original target
    hr = pd3dd->GetRenderTarget(&pd3ds1);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetRenderTarget"))) {
        return TR_ABORT;
    }

    hr = pd3dd->GetDepthStencilSurface(&pd3dsd1);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDepthStencilSurface"))) {
        pd3ds1->Release();
        return TR_ABORT;
    }

    // Create a new target
#ifndef UNDER_XBOX
    hr = pd3dd->CreateDepthStencilSurface(512, 512, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, &pd3dsd2);
#else
    hr = pd3dd->CreateDepthStencilSurface(512, 512, D3DFMT_LIN_D24S8, D3DMULTISAMPLE_NONE, &pd3dsd2);
#endif // UNDER_XBOX
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateDepthStencilSurface"))) {
        pd3dsd1->Release();
        pd3ds1->Release();
        return TR_ABORT;
    }

    hr = pd3dd->CreateTexture(512, 512, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pd3dt);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateTexture"))) {
        pd3dsd2->Release();
        pd3ds1->Release();
        pd3dsd1->Release();
        return TR_ABORT;
    }

    hr = pd3dt->GetSurfaceLevel(0, &pd3ds2);
    pd3dt->Release();
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
        pd3dsd2->Release();
        pd3ds1->Release();
        pd3dsd1->Release();
        return TR_ABORT;
    }

    pd3ds1->AddRef();
    uRef1 = pd3ds1->Release();
    pd3dsd1->AddRef();
    uRef2 = pd3dsd1->Release();

    // Set the target
    hr = pd3dd->SetRenderTarget(pd3ds2, pd3dsd2);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderTarget"))) {
        pd3ds2->Release();
        pd3dsd2->Release();
        pd3ds1->Release();
        pd3dsd1->Release();
        return TR_FAIL;
    }

    // Verify the reference counts of the old buffers were correctly decremented
    pd3ds1->AddRef();
    pd3dsd1->AddRef();
    if (pd3ds1->Release() != uRef1 + 1) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetRenderTarget failed to decrement the ref count on the color buffer being replaced"));
        tr = TR_FAIL;
    }
    if (pd3dsd1->Release() != uRef2 + 1) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetRenderTarget failed to decrement the ref count on the depth buffer being replaced"));
        tr = TR_FAIL;
    }

    // Verify the reference counts of the new buffers were correctly incremented
    pd3ds2->AddRef();
    pd3dsd2->AddRef();
    if (pd3ds2->Release() != 2) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetRenderTarget failed to increment the ref count on the color buffer being set"));
        tr = TR_FAIL;
    }
    if (pd3dsd2->Release() != 2) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetRenderTarget failed to increment the ref count on the depth buffer being set"));
        tr = TR_FAIL;
    }

    // Verify the target was correctly set
    hr = pd3dd->GetRenderTarget(&pd3ds3);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetRenderTarget"))) {
        pd3ds2->Release();
        pd3dsd2->Release();
        pd3ds1->Release();
        pd3dsd1->Release();
        return TR_ABORT;
    }

    hr = pd3dd->GetDepthStencilSurface(&pd3dsd3);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDepthStencilSurface"))) {
        pd3ds2->Release();
        pd3dsd2->Release();
        pd3ds1->Release();
        pd3dsd1->Release();
        return TR_ABORT;
    }

    if (pd3ds3 != pd3ds2) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetRenderTarget failed to correctly set the color buffer"));
        tr = TR_FAIL;
    }
    if (pd3dsd3 != pd3dsd2) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetRenderTarget failed to correctly set the depth buffer"));
        tr = TR_FAIL;
    }

    pd3ds3->Release();
    pd3dsd3->Release();

    // Restore the original target
    hr = pd3dd->SetRenderTarget(pd3ds1, pd3dsd1);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderTarget"))) {
        tr = TR_ABORT;
    }

    // Release
    pd3ds2->Release();
    pd3dsd2->Release();
    pd3ds1->Release();
    pd3dsd1->Release();

    return tr;
}
*/

//******************************************************************************
TESTPROCAPI TDEV8_GetTransform(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

#ifndef UNDER_XBOX
#define NUM_TRANSFORMS  266
#else
#define NUM_TRANSFORMS  10
#endif // UNDER_XBOX

//******************************************************************************
TESTPROCAPI TDEV8_SetTransform(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8       pd3dd;
    D3DXMATRIX              mInitial[266];
    D3DXMATRIX              mSet, mGet;
    UINT                    i;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;
    D3DTRANSFORMSTATETYPE   tstType[NUM_TRANSFORMS] = {
                                D3DTS_VIEW,
                                D3DTS_PROJECTION,
                                D3DTS_TEXTURE0,
                                D3DTS_TEXTURE1,
                                D3DTS_TEXTURE2,
                                D3DTS_TEXTURE3,
#ifndef UNDER_XBOX
                                D3DTS_TEXTURE4,
                                D3DTS_TEXTURE5,
                                D3DTS_TEXTURE6,
                                D3DTS_TEXTURE7,
#endif // !UNDER_XBOX
                            };
    
    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    for (i = 10; i < NUM_TRANSFORMS; i++) {
        tstType[i] = D3DTS_WORLDMATRIX(i - 10);
    }

    for (i = 0; i < NUM_TRANSFORMS; i++) {
        hr = pd3dd->GetTransform(tstType[i], &mInitial[i]);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetTransform"))) {
            return TR_ABORT;
        }
    }

    for (i = 0; i < NUM_TRANSFORMS; i++) {

        mSet = D3DXMATRIX(FLT_RND, FLT_RND, FLT_RND, FLT_RND, 
                          FLT_RND, FLT_RND, FLT_RND, FLT_RND, 
                          FLT_RND, FLT_RND, FLT_RND, FLT_RND, 
                          FLT_RND, FLT_RND, FLT_RND, FLT_RND);
        if (tstType[i] == D3DTS_PROJECTION) {
            mSet._23 = RND() * FLT_MAX;
            mSet._34 = RND() * FLT_MAX;
        }

        hr = pd3dd->SetTransform(tstType[i], &mSet);
        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetTransform failed for type %d"), tstType[i]);
            tr = TR_FAIL;
        }
        else {
            hr = pd3dd->GetTransform(tstType[i], &mGet);
            if (FAILED(hr)) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetTransform failed for type %d"), tstType[i]);
                tr = TR_FAIL;
            }
            else {
                if (memcmp(&mGet, &mSet, sizeof(D3DXMATRIX))) {
                    Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetTransform incorrectly set the transform for type %d"), tstType[i]);
                    tr = TR_FAIL;
                }
            }
        }
    }

    // Restore the transforms
    for (i = 0; i < NUM_TRANSFORMS; i++) {
        hr = pd3dd->SetTransform(tstType[i], &mInitial[i]);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTransform"))) {
            tr = TR_ABORT;
        }
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_MultiplyTransform(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8       pd3dd;
    D3DXMATRIX              mInitial[NUM_TRANSFORMS];
    D3DXMATRIX              mSet, mGet, mMult, mResult;
    UINT                    i;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;
    D3DTRANSFORMSTATETYPE   tstType[266] = {
                                D3DTS_VIEW,
                                D3DTS_PROJECTION,
                                D3DTS_TEXTURE0,
                                D3DTS_TEXTURE1,
                                D3DTS_TEXTURE2,
                                D3DTS_TEXTURE3,
#ifndef UNDER_XBOX
                                D3DTS_TEXTURE4,
                                D3DTS_TEXTURE5,
                                D3DTS_TEXTURE6,
                                D3DTS_TEXTURE7,
#endif // !UNDER_XBOX
                            };
    
    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    for (i = 10; i < NUM_TRANSFORMS; i++) {
        tstType[i] = D3DTS_WORLDMATRIX(i - 10);
    }

    for (i = 0; i < NUM_TRANSFORMS; i++) {
        hr = pd3dd->GetTransform(tstType[i], &mInitial[i]);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetTransform"))) {
            return TR_ABORT;
        }
    }

    for (i = 0; i < NUM_TRANSFORMS; i++) {

        mSet = D3DXMATRIX(NRND()*1000.0f, NRND()*1000.0f, NRND()*1000.0f, NRND()*1000.0f, 
                          NRND()*1000.0f, NRND()*1000.0f, NRND()*1000.0f, NRND()*1000.0f, 
                          NRND()*1000.0f, NRND()*1000.0f, NRND()*1000.0f, NRND()*1000.0f, 
                          NRND()*1000.0f, NRND()*1000.0f, NRND()*1000.0f, NRND()*1000.0f);
        if (tstType[i] == D3DTS_PROJECTION) {
            mSet._23 = FABS(mSet._23);
            mSet._34 = FABS(mSet._34);
        }

        hr = pd3dd->SetTransform(tstType[i], &mSet);
        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetTransform failed for type %d"), tstType[i]);
            tr = TR_FAIL;
        }
        else {

            mMult = D3DXMATRIX(NRND()*1000.0f, NRND()*1000.0f, NRND()*1000.0f, NRND()*1000.0f, 
                               NRND()*1000.0f, NRND()*1000.0f, NRND()*1000.0f, NRND()*1000.0f, 
                               NRND()*1000.0f, NRND()*1000.0f, NRND()*1000.0f, NRND()*1000.0f, 
                               NRND()*1000.0f, NRND()*1000.0f, NRND()*1000.0f, NRND()*1000.0f);
            if (tstType[i] == D3DTS_PROJECTION) {
                mMult._23 = FABS(mMult._23);
                mMult._34 = FABS(mMult._34);
            }

            hr = pd3dd->MultiplyTransform(tstType[i], &mMult);
            if (FAILED(hr)) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::MultiplyTransform failed for type %d"), tstType[i]);
                tr = TR_FAIL;
            }
            else {
                hr = pd3dd->GetTransform(tstType[i], &mGet);
                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetTransform failed for type %d"), tstType[i]);
                    tr = TR_FAIL;
                }
                else {
                    D3DXMatrixMultiply(&mResult, &mMult, &mSet);
#ifndef UNDER_XBOX
                    if (memcmp(&mGet, &mResult, sizeof(D3DXMATRIX))) {
                        Log(LOG_FAIL, TEXT("IDirect3DDevice8::MultiplyTransform incorrently multiplied the transform for type %d"), tstType[i]);
                        tr = TR_FAIL;
                    }
#else
                    float* pfElementG, *pfElementR;
                    UINT j;
                    for (j = 0, pfElementG = &mGet._11, pfElementR = &mResult._11; j < 16; j++, pfElementG++, pfElementR++) {
                        if (FABS(*pfElementR - *pfElementG) > 1.0f) {
                            Log(LOG_FAIL, TEXT("IDirect3DDevice8::MultiplayTransform incorrectly multiplied the transform on element %d, returning %3.3f (expected %3.3f)"), j, *pfElementG, *pfElementR);
                            tr = TR_FAIL;
                            break;
                        }
                    }
#endif // UNDER_XBOX
                }
            }
        }
    }

    // Restore the transforms
    for (i = 0; i < NUM_TRANSFORMS; i++) {
        hr = pd3dd->SetTransform(tstType[i], &mInitial[i]);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTransform"))) {
            tr = TR_ABORT;
        }
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_Clear(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    UINT                i, j;
    D3DRECT*            prc[2];
    DWORD               dwCount[2];
    D3DRECT             rc[10];
    HRESULT             hr;
    TRESULT             tr = TR_PASS;
    DWORD               dwFlags[] = {
                            D3DCLEAR_TARGET,
                            D3DCLEAR_ZBUFFER,
                            D3DCLEAR_STENCIL,
                            D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                            D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
                            D3DCLEAR_STENCIL | D3DCLEAR_TARGET,
                            D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL
                        };

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    rc[0].x1 = 0;
    rc[0].y1 = 0;
    rc[0].x2 = 50;
    rc[0].y2 = 100;
    for (i = 1; i < 10; i++) {
        rc[i].x1 = rc[0].x1 + i;
        rc[i].y1 = rc[0].y1 + i;
        rc[i].x2 = rc[0].x2 + i;
        rc[i].y2 = rc[0].y2 + i;
    }

    prc[0] = NULL;
    prc[1] = rc;
    dwCount[0] = 0;
    dwCount[1] = 10;

    for (i = 0; i < countof(dwFlags); i++) {

        for (j = 0; j < 2; j++) {
            
            LogBeginVariation(TEXT("IDirect3DDevice8::Clear of %s using 0x%X flags"), j ? TEXT("rectangles") : TEXT("the entire render target"), dwFlags[i]);
            hr = pd3dd->Clear(dwCount[j], prc[j], dwFlags[i], RGBA_MAKE(rand() % 256, rand() % 256, rand() % 256, rand() % 256), RND(), rand() % 256);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::Clear"))) {
                tr = TR_FAIL;
            }
            LogEndVariation();
        }
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetViewport(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_SetViewport(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    D3DVIEWPORT8        vpSet[2], vpGet, vpInitial;
    UINT                i;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    pd3dd->GetViewport(&vpInitial);

    memset(vpSet, 0, 2 * sizeof(D3DVIEWPORT8));
    vpSet[0].X = 0;
    vpSet[0].Y = 0;
    vpSet[0].Width = TARGET_WIDTH;
    vpSet[0].Height = TARGET_HEIGHT;
    vpSet[0].MinZ = 0.0f;
    vpSet[0].MaxZ = 1.0f;
    vpSet[1].X = rand() % (TARGET_WIDTH - 1);
    vpSet[1].Y = rand() % (TARGET_HEIGHT - 1);
    vpSet[1].Width = rand() % (TARGET_WIDTH - vpSet[1].X) + 1;
    vpSet[1].Height = rand() % (TARGET_HEIGHT - vpSet[1].Y) + 1;
    vpSet[1].MinZ = FRND(0.95f);
    vpSet[1].MaxZ = vpSet[1].MinZ + FRND(1.0f - vpSet[1].MinZ);

    for (i = 0; i < 2; i++) {
        hr = pd3dd->SetViewport(&vpSet[i]);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetViewport"))) {
            tr = TR_FAIL;
        }
        else {
            hr = pd3dd->GetViewport(&vpGet);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetViewport"))) {
                tr = TR_FAIL;
            }
            else {
                if (memcmp(&vpSet[i], &vpGet, sizeof(D3DVIEWPORT8))) {
                    Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetViewport failed to correctly set the viewport information"));
                    tr = TR_FAIL;
                }
            }
        }
    }

    pd3dd->SetViewport(&vpInitial);

    return tr;
}

#ifndef UNDER_XBOX

//******************************************************************************
TESTPROCAPI TDEV8_GetClipPlane(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    TRESULT             tr = TR_PASS;

return TR_SKIP;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_SetClipPlane(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    D3DCAPS8            d3dcaps;
    D3DXVECTOR3         vPlaneNormal, vPointOnPlane;
    D3DXVECTOR4         vClipPlaneSet[D3DMAXUSERCLIPPLANES];
    D3DXVECTOR4         vClipPlaneGet;
    UINT                i;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

return TR_SKIP;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    hr = pd3dd->GetDeviceCaps(&d3dcaps);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDeviceCaps"))) {
        return TR_ABORT;
    }

    if (d3dcaps.MaxUserClipPlanes != 6) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetCaps reported an incorrect number of maximum user-defined clip planes (%d)"), d3dcaps.MaxUserClipPlanes);
        tr = TR_FAIL;
    }

    for (i = 0; i < D3DMAXUSERCLIPPLANES; i++) {
        D3DXVec3Normalize(&vPlaneNormal, &D3DXVECTOR3(NRND(), NRND(), NRND()));
        vPointOnPlane = D3DXVECTOR3(NRND() * 10.0f, NRND() * 10.0f, NRND() * 10.0f);
        vClipPlaneSet[i] = D3DXVECTOR4(vPlaneNormal.x, vPlaneNormal.y, vPlaneNormal.z,
                            -vPlaneNormal.x * vPointOnPlane.x - vPlaneNormal.y * vPointOnPlane.y
                            - vPlaneNormal.z * vPointOnPlane.z);
        hr = pd3dd->SetClipPlane(i, (float*)&vClipPlaneSet[i]);
        if (i >= d3dcaps.MaxUserClipPlanes) {
            if (hr != D3DERR_INVALIDCALL) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetClipPlane returned 0x%X instead of D3DERR_INVALIDCALL for an out of range plane index %d"), hr, i);
                tr = TR_FAIL;
            }
        }
        else {
            if (FAILED(hr)) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetClipPlane failed on index %d [0x%X]"), i, hr);
                tr = TR_FAIL;
            }
            else {
                hr = pd3dd->GetClipPlane(i, (float*)&vClipPlaneGet);
                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetClipPlane failed on index %d [0x%X]"), i, hr);
                    tr = TR_FAIL;
                }
                else {
                    if (memcmp(&vClipPlaneGet, &vClipPlaneSet[i], sizeof(D3DXVECTOR4))) {
                        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetClipPlane incorrectly set the clip plane for index %d"), i);
                        tr = TR_FAIL;
                    }
                }
            }
        }
    }

    for (i = 0; i < d3dcaps.MaxUserClipPlanes; i++) {
        hr = pd3dd->GetClipPlane(i, (float*)&vClipPlaneGet);
        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetClipPlane failed on index %d [0x%X]"), i, hr);
            tr = TR_FAIL;
        }
        else {
            if (memcmp(&vClipPlaneGet, &vClipPlaneSet[i], sizeof(D3DXVECTOR4))) {
                Log(LOG_FAIL, TEXT("The clip plane for index %d failed to persist"), i);
                tr = TR_FAIL;
            }
        }
    }

    return tr;
}

#endif // UNDER_XBOX

//******************************************************************************
TESTPROCAPI TDEV8_GetLight(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_SetLight(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    D3DLIGHT8           lightSet[10], lightGet;
    UINT                i, j;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;
    D3DLIGHTTYPE        d3dlt[] = {
                            D3DLIGHT_POINT,
                            D3DLIGHT_SPOT,
                            D3DLIGHT_DIRECTIONAL,
                        };

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    for (j = 0; j < 2; j++) {   // Overwrite existing lights on the second pass

        memset(lightSet, 0, 10 * sizeof(D3DLIGHT8));

        for (i = 0; i < 10; i++) {

            lightSet[i].Type = d3dlt[i % 3];
            lightSet[i].Diffuse.r = NRND();
            lightSet[i].Diffuse.g = NRND();
            lightSet[i].Diffuse.b = NRND();
            lightSet[i].Diffuse.a = NRND();
            lightSet[i].Specular.r = NRND();
            lightSet[i].Specular.g = NRND();
            lightSet[i].Specular.b = NRND();
            lightSet[i].Specular.a = NRND();
            lightSet[i].Ambient.r = NRND();
            lightSet[i].Ambient.g = NRND();
            lightSet[i].Ambient.b = NRND();
            lightSet[i].Ambient.a = NRND();
            lightSet[i].Position.x = NRND() * 100.0f;
            lightSet[i].Position.y = NRND() * 100.0f;
            lightSet[i].Position.z = NRND() * 100.0f;
            lightSet[i].Direction.x = NRND() * 100.0f;
            lightSet[i].Direction.y = NRND() * 100.0f;
            lightSet[i].Direction.z = NRND() * 100.0f;
            if (i < 3) {
                lightSet[i].Range = 0.0f;
            }
            else if (i < 6) {
                lightSet[i].Range = (float)sqrt(FLT_MAX);
            }
            else {
                lightSet[i].Range = RND() * (float)sqrt(FLT_MAX);
            }
            lightSet[i].Falloff = RND();
            lightSet[i].Attenuation0 = RND() * FLT_MAX;
            lightSet[i].Attenuation1 = RND() * FLT_MAX;
            lightSet[i].Attenuation2 = RND() * FLT_MAX;
            lightSet[i].Phi = RND() * M_PI;
            lightSet[i].Theta = RND() * lightSet[i].Phi;

            hr = pd3dd->SetLight(i, &lightSet[i]);
            if (FAILED(hr)) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetLight failed on index %d [0x%X]"), i, hr);
                tr = TR_FAIL;
            }
            else {
                hr = pd3dd->GetLight(i, &lightGet);
                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetLight failed on index %d [0x%X]"), i, hr);
                    tr = TR_FAIL;
                }
                else {
                    switch (lightSet[i].Type) {

                        case D3DLIGHT_SPOT:
                            if (memcmp(&lightGet, &lightSet[i], sizeof(D3DLIGHT8))) {
                                Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetLight incorrectly set the light for index %d"), i);
                                tr = TR_FAIL;
                            }
                            break;

                        case D3DLIGHT_DIRECTIONAL:
                            if (lightGet.Type != lightSet[i].Type ||
                                memcmp(&lightGet.Diffuse, &lightSet[i].Diffuse, sizeof(D3DCOLORVALUE)) ||
                                memcmp(&lightGet.Ambient, &lightSet[i].Ambient, sizeof(D3DCOLORVALUE)) ||
                                memcmp(&lightGet.Specular, &lightSet[i].Specular, sizeof(D3DCOLORVALUE)) ||
                                memcmp(&lightGet.Direction, &lightSet[i].Direction, sizeof(D3DVECTOR)))
                            {
                                Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetLight incorrectly set the light for index %d"), i);
                                tr = TR_FAIL;
                            }
                            break;

                        case D3DLIGHT_POINT:
                             if (lightGet.Type != lightSet[i].Type ||
                                memcmp(&lightGet.Diffuse, &lightSet[i].Diffuse, sizeof(D3DCOLORVALUE)) ||
                                memcmp(&lightGet.Ambient, &lightSet[i].Ambient, sizeof(D3DCOLORVALUE)) ||
                                memcmp(&lightGet.Specular, &lightSet[i].Specular, sizeof(D3DCOLORVALUE)) ||
                                memcmp(&lightGet.Position, &lightSet[i].Position, sizeof(D3DVECTOR)) ||
                                lightGet.Range != lightSet[i].Range ||
                                lightGet.Attenuation0 != lightSet[i].Attenuation0 ||
                                lightGet.Attenuation1 != lightSet[i].Attenuation1 ||
                                lightGet.Attenuation2 != lightSet[i].Attenuation2)
                            {
                                Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetLight incorrectly set the light for index %d"), i);
                                tr = TR_FAIL;
                            }
                            break;
                    }
                }
            }
        }

        for (i = 0; i < 10; i++) {

            hr = pd3dd->GetLight(i, &lightGet);
            if (FAILED(hr)) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetLight failed on index %d [0x%X]"), i, hr);
                tr = TR_FAIL;
            }
            else {
                switch (lightSet[i].Type) {

                    case D3DLIGHT_SPOT:
                        if (memcmp(&lightGet, &lightSet[i], sizeof(D3DLIGHT8))) {
                            Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetLight incorrectly set the light for index %d"), i);
                            tr = TR_FAIL;
                        }
                        break;

                    case D3DLIGHT_DIRECTIONAL:
                        if (lightGet.Type != lightSet[i].Type ||
                            memcmp(&lightGet.Diffuse, &lightSet[i].Diffuse, sizeof(D3DCOLORVALUE)) ||
                            memcmp(&lightGet.Ambient, &lightSet[i].Ambient, sizeof(D3DCOLORVALUE)) ||
                            memcmp(&lightGet.Specular, &lightSet[i].Specular, sizeof(D3DCOLORVALUE)) ||
                            memcmp(&lightGet.Direction, &lightSet[i].Direction, sizeof(D3DVECTOR)))
                        {
                            Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetLight incorrectly set the light for index %d"), i);
                            tr = TR_FAIL;
                        }
                        break;

                    case D3DLIGHT_POINT:
                         if (lightGet.Type != lightSet[i].Type ||
                            memcmp(&lightGet.Diffuse, &lightSet[i].Diffuse, sizeof(D3DCOLORVALUE)) ||
                            memcmp(&lightGet.Ambient, &lightSet[i].Ambient, sizeof(D3DCOLORVALUE)) ||
                            memcmp(&lightGet.Specular, &lightSet[i].Specular, sizeof(D3DCOLORVALUE)) ||
                            memcmp(&lightGet.Position, &lightSet[i].Position, sizeof(D3DVECTOR)) ||
                            lightGet.Range != lightSet[i].Range ||
                            lightGet.Attenuation0 != lightSet[i].Attenuation0 ||
                            lightGet.Attenuation1 != lightSet[i].Attenuation1 ||
                            lightGet.Attenuation2 != lightSet[i].Attenuation2)
                        {
                            Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetLight incorrectly set the light for index %d"), i);
                            tr = TR_FAIL;
                        }
                        break;
                }
            }
        }
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_LightEnable(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    D3DLIGHT8           lightSet[3], lightGet;
    BOOL                bEnabled;
    UINT                i, j;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;
    D3DLIGHTTYPE        d3dlt[] = {
                            D3DLIGHT_POINT,
                            D3DLIGHT_SPOT,
                            D3DLIGHT_DIRECTIONAL,
                        };

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    memset(lightSet, 0, 3 * sizeof(D3DLIGHT8));

    for (i = 0; i < 3; i++) {

        lightSet[i].Type = d3dlt[i];
        lightSet[i].Diffuse.r = NRND() * 2.0f;
        lightSet[i].Diffuse.g = NRND() * 2.0f;
        lightSet[i].Diffuse.b = NRND() * 2.0f;
        lightSet[i].Diffuse.a = NRND() * 2.0f;
        lightSet[i].Specular.r = NRND() * 2.0f;
        lightSet[i].Specular.g = NRND() * 2.0f;
        lightSet[i].Specular.b = NRND() * 2.0f;
        lightSet[i].Specular.a = NRND() * 2.0f;
        lightSet[i].Ambient.r = NRND() * 2.0f;
        lightSet[i].Ambient.g = NRND() * 2.0f;
        lightSet[i].Ambient.b = NRND() * 2.0f;
        lightSet[i].Ambient.a = NRND() * 2.0f;
        lightSet[i].Position.x = NRND() * 100.0f;
        lightSet[i].Position.y = NRND() * 100.0f;
        lightSet[i].Position.z = NRND() * 100.0f;
        lightSet[i].Direction.x = NRND() * 100.0f;
        lightSet[i].Direction.y = NRND() * 100.0f;
        lightSet[i].Direction.z = NRND() * 100.0f;
        lightSet[i].Range = RND() * (float)sqrt(FLT_MAX);
        lightSet[i].Falloff = RND();
        lightSet[i].Attenuation0 = RND() * FLT_MAX;
        lightSet[i].Attenuation1 = RND() * FLT_MAX;
        lightSet[i].Attenuation2 = RND() * FLT_MAX;
        lightSet[i].Phi = RND() * M_PI;
        lightSet[i].Theta = RND() * lightSet[i].Phi;

        hr = pd3dd->SetLight(i, &lightSet[i]);
        if (FAILED(hr)) {
            Log(LOG_ABORT, TEXT("IDirect3DDevice8::SetLight failed on index %d [0x%X]"), i, hr);
            return TR_ABORT;
        }

        hr = pd3dd->GetLightEnable(i, &bEnabled);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetLightEnable"))) {
            tr = TR_FAIL;
        }
        else {
            if (bEnabled) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetLightEnable incorrectly reported a disabled light as enabled"));
                tr = TR_FAIL;
            }
        }

        for (j = 0; j < 2; j++) { 
            hr = pd3dd->LightEnable(i, !j);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::LightEnable"))) {
                tr = TR_FAIL;
            }
            else {
                hr = pd3dd->GetLightEnable(i, &bEnabled);
                if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetLightEnable"))) {
                    tr = TR_FAIL;
                }
                else {
                    if (bEnabled != !j) {
                        Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetLightEnable incorrectly reported a light as %s"), bEnabled ? TEXT("enabled") : TEXT("disabled"));
                        tr = TR_FAIL;
                    }
                }
            }
        }
    }

    i = 100 + rand() % 900;

#ifdef INVALID_TESTS
    hr = pd3dd->GetLightEnable(i, &bEnabled);
    if (hr != D3DERR_INVALIDCALL) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetLightEnable returned 0x%X instead of D3DERR_INVALIDCALL on an invalid index %d"), hr, i);
        tr = TR_FAIL;
    }
#endif // INVALID_TESTS
    hr = pd3dd->LightEnable(i, FALSE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::LightEnable on an index out of bounds"))) {
        tr = TR_FAIL;
    }
    else {
        hr = pd3dd->GetLight(i, &lightGet);
        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetLight failed on index %d [0x%X]"), i, hr);
            tr = TR_FAIL;
        }
        else {
            if (lightGet.Type != D3DLIGHT_DIRECTIONAL ||
                lightGet.Diffuse.a != 0.0f ||
                lightGet.Diffuse.r != 1.0f ||
                lightGet.Diffuse.g != 1.0f ||
                lightGet.Diffuse.b != 1.0f ||
                lightGet.Specular.a != 0.0f ||
                lightGet.Specular.r != 0.0f ||
                lightGet.Specular.g != 0.0f ||
                lightGet.Specular.b != 0.0f ||
                lightGet.Ambient.a != 0.0f ||
                lightGet.Ambient.r != 0.0f ||
                lightGet.Ambient.g != 0.0f ||
                lightGet.Ambient.b != 0.0f ||
                lightGet.Position.x != 0.0f ||
                lightGet.Position.y != 0.0f ||
                lightGet.Position.z != 0.0f ||
                lightGet.Direction.x != 0.0f ||
                lightGet.Direction.y != 0.0f ||
                lightGet.Direction.z != 1.0f ||
                lightGet.Range != 0.0f ||
                lightGet.Falloff != 0.0f ||
                lightGet.Attenuation0 != 0.0f ||
                lightGet.Attenuation1 != 0.0f ||
                lightGet.Attenuation2 != 0.0f ||
                lightGet.Phi != 0.0f ||
                lightGet.Theta != 0.0f)
            {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::LightEnable incorrectly set a default light at index %d"), i);
                tr = TR_FAIL;
            }
        }
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetLightEnable(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetMaterial(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_SetMaterial(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    D3DMATERIAL8        materialSet, materialGet;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    materialSet.Diffuse.r = NRND() * 2.0f;
    materialSet.Diffuse.g = NRND() * 2.0f;
    materialSet.Diffuse.b = NRND() * 2.0f;
    materialSet.Diffuse.a = NRND() * 2.0f;
    materialSet.Specular.r = NRND() * 2.0f;
    materialSet.Specular.g = NRND() * 2.0f;
    materialSet.Specular.b = NRND() * 2.0f;
    materialSet.Specular.a = NRND() * 2.0f;
    materialSet.Ambient.r = NRND() * 2.0f;
    materialSet.Ambient.g = NRND() * 2.0f;
    materialSet.Ambient.b = NRND() * 2.0f;
    materialSet.Ambient.a = NRND() * 2.0f;
    materialSet.Emissive.r = NRND() * 2.0f;
    materialSet.Emissive.g = NRND() * 2.0f;
    materialSet.Emissive.b = NRND() * 2.0f;
    materialSet.Emissive.a = NRND() * 2.0f;
    materialSet.Power = RND() * 200.0f;

    hr = pd3dd->SetMaterial(&materialSet);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetMaterial"))) {
        return TR_FAIL;
    }

    hr = pd3dd->GetMaterial(&materialGet);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetMaterial"))) {
        return TR_FAIL;
    }

    if (memcmp(&materialSet, &materialGet, sizeof(D3DMATERIAL8))) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetMaterial incorrectly set the material properties in the device"));
        tr = TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetTexture(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_SetTexture(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8       pd3dd;
    LPDIRECT3DBASETEXTURE8  pd3dt[4], pd3dtGet;
    ULONG                   uRef1, uRef2;
    UINT                    i, j;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    pd3dt[0] = GetTexture8();
    pd3dt[1] = GetCubeTexture8();
    pd3dt[2] = GetVolumeTexture8();
    pd3dt[3] = NULL;
    if (!pd3dt[0] || !pd3dt[1] || !pd3dt[2]) {
        return TR_ABORT;
    }

    for (i = 0; i < 4; i++) {

        for (j = 0; j < 4; j++) {

            hr = pd3dd->SetTexture(j, pd3dt[i]);
            if (FAILED(hr)) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetTexture failed for texture %d on stage %d"), i, j);
                tr = TR_FAIL;
            }
            else {
                if (i != 3) {
                    pd3dt[i]->AddRef();
                    uRef1 = pd3dt[i]->Release();
                }
                hr = pd3dd->GetTexture(j, &pd3dtGet);
                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetTexture failed for texture %d on stage %d"), i, j);
                    tr = TR_FAIL;
                }
                else {
                    if (pd3dt[i] != pd3dtGet) {
                        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetTexture failed to correctly set texture %d on stage %d"), i, j);
                        tr = TR_FAIL;
                    }

                    if (i != 3) {
                        pd3dt[i]->AddRef();
                        uRef2 = pd3dt[i]->Release();
                        if (uRef2 != uRef1 + 1) {
                            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetTexture failed to increment the reference count on the texture %d on stage %d"), i, j);
                        }
                        else {
                            pd3dtGet->Release();
                        }
                    }
                }
            }
        }
    }

    return tr;
}

//******************************************************************************
#define TestTextureStageState(stage, state, value)                              \
    dwTStateSet = (DWORD)(value);                                               \
    hr = pd3dd->SetTextureStageState(stage, state, dwTStateSet);                \
    if (FAILED(hr)) {                                                           \
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetTextureStageState failed for ") TEXT(#state) TEXT(" on stage %d"), stage); \
        tr = TR_FAIL;                                                           \
    }                                                                           \
    else {                                                                      \
        hr = pd3dd->GetTextureStageState(stage, state, &dwTStateGet);           \
        if (FAILED(hr)) {                                                       \
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetTextureStageState failed for ") TEXT(#state) TEXT("on stage %d"), stage); \
            tr = TR_FAIL;                                                       \
        }                                                                       \
        else {                                                                  \
            if (dwTStateSet != dwTStateGet) {                                   \
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetTextureStageState returned %d for a state set as ") TEXT(#state) TEXT(" on stage %d"), hr, stage); \
                tr = TR_FAIL;                                                   \
            }                                                                   \
        }                                                                       \
    }

//******************************************************************************
TESTPROCAPI TDEV8_GetTextureStageState(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_SetTextureStageState(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    DWORD               dwBlock;
    DWORD               dwTStateSet, dwTStateGet;
    float               f00, f01, f10, f11, fBias, fLScale, fLOffset;
    UINT                i;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    hr = pd3dd->CreateStateBlock(D3DSBT_ALL, &dwBlock);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateStateBlock"))) {
        return TR_ABORT;
    }

    f00 = 0.5f;
    f01 = 4.23f;
    f10 = 0.1254f;
    f11 = 0.25f;
    fBias = 0.1f;
    fLScale = 2.0f;
    fLOffset = 11.2f;

    for (i = 0; i < 4; i++) {
        TestTextureStageState(i, D3DTSS_COLOROP,                D3DTOP_DOTPRODUCT3);
        TestTextureStageState(i, D3DTSS_COLORARG1,              D3DTA_DIFFUSE);
        TestTextureStageState(i, D3DTSS_COLORARG2,              D3DTA_TEXTURE);
        TestTextureStageState(i, D3DTSS_ALPHAOP,                D3DTOP_ADD);
        TestTextureStageState(i, D3DTSS_ALPHAARG1,              D3DTA_TEXTURE);
        TestTextureStageState(i, D3DTSS_ALPHAARG2,              D3DTA_DIFFUSE);
        TestTextureStageState(i, D3DTSS_BUMPENVMAT00,           *(LPDWORD)&f00);
        TestTextureStageState(i, D3DTSS_BUMPENVMAT01,           *(LPDWORD)&f01);
        TestTextureStageState(i, D3DTSS_BUMPENVMAT10,           *(LPDWORD)&f10);
        TestTextureStageState(i, D3DTSS_BUMPENVMAT11,           *(LPDWORD)&f11);
        TestTextureStageState(i, D3DTSS_TEXCOORDINDEX,          D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 1);
        TestTextureStageState(i, D3DTSS_ADDRESSU,               D3DTADDRESS_CLAMP);
        TestTextureStageState(i, D3DTSS_ADDRESSV,               D3DTADDRESS_BORDER);
        TestTextureStageState(i, D3DTSS_ADDRESSW,               D3DTADDRESS_CLAMP);
        TestTextureStageState(i, D3DTSS_BORDERCOLOR,            RGBA_MAKE(196, 64, 128, 32));
        TestTextureStageState(i, D3DTSS_MAGFILTER,              D3DTEXF_LINEAR);
        TestTextureStageState(i, D3DTSS_MINFILTER,              D3DTEXF_LINEAR);
        TestTextureStageState(i, D3DTSS_MIPFILTER,              D3DTEXF_POINT);
        TestTextureStageState(i, D3DTSS_MIPMAPLODBIAS,          *(LPDWORD)&fBias);
        TestTextureStageState(i, D3DTSS_MAXMIPLEVEL,            2);
        TestTextureStageState(i, D3DTSS_MAXANISOTROPY,          rand() % 5);
        TestTextureStageState(i, D3DTSS_BUMPENVLSCALE,          *(LPDWORD)&fLScale);
        TestTextureStageState(i, D3DTSS_BUMPENVLOFFSET,         *(LPDWORD)&fLOffset);
        TestTextureStageState(i, D3DTSS_TEXTURETRANSFORMFLAGS,  D3DTTFF_COUNT4 | D3DTTFF_PROJECTED);
    }

    // Restore the texture stage states
    hr = pd3dd->ApplyStateBlock(dwBlock);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::ApplyStateBlock"))) {
        tr = TR_ABORT;
    }

    pd3dd->DeleteStateBlock(dwBlock);

    return tr;
}

#ifndef UNDER_XBOX

//******************************************************************************
TESTPROCAPI TDEV8_UpdateTexture(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8        pd3dd;
    LPDIRECT3DTEXTURE8       pd3dtSrc, pd3dtDst;
    LPDIRECT3DCUBETEXTURE8   pd3dtcSrc, pd3dtcDst;
    LPDIRECT3DVOLUMETEXTURE8 pd3dtvSrc, pd3dtvDst;
    D3DLOCKED_RECT           d3dlr;
    D3DLOCKED_BOX            d3dlb;
    DWORD                    dwData;
    HRESULT                  hr;
    TRESULT                  tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    // Texture
    hr = pd3dd->CreateTexture(256, 64, 0, 0, D3DFMT_A4R4G4B4, D3DPOOL_SYSTEMMEM, &pd3dtSrc);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateTexture"))) {
        return TR_ABORT;
    }

    hr = pd3dtSrc->LockRect(1, &d3dlr, NULL, 0);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"))) {
        pd3dtSrc->Release();
        return TR_ABORT;
    }

    dwData = rand() % 0xFFFFFFFF + 1;
    *(LPDWORD)d3dlr.pBits = dwData;

    hr = pd3dtSrc->UnlockRect(1);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::UnlockRect"))) {
        pd3dtSrc->Release();
        return TR_ABORT;
    }

    hr = pd3dd->CreateTexture(256, 64, 3, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &pd3dtDst);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateTexture"))) {
        pd3dtSrc->Release();
        return TR_ABORT;
    }

    hr = pd3dd->UpdateTexture(pd3dtSrc, pd3dtDst);
    pd3dtSrc->Release();
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::UpdateTexture"))) {
        pd3dtDst->Release();
        tr = TR_FAIL;
    }
    else {

        hr = pd3dtDst->LockRect(1, &d3dlr, NULL, 0);
        if (ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"))) {
            pd3dtDst->Release();
            return TR_ABORT;
        }

        if (*(LPDWORD)d3dlr.pBits != dwData) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::UpdateTexture failed to correctly update the destination texture"));
            tr = TR_FAIL;
        }

        hr = pd3dtDst->UnlockRect(1);
        pd3dtDst->Release();
        if (ResultFailed(hr, TEXT("IDirect3DTexture8::UnlockRect"))) {
            return TR_ABORT;
        }
    }

    // Cube Texture
    hr = pd3dd->CreateCubeTexture(128, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pd3dtcSrc);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateCubeTexture"))) {
        return TR_ABORT;
    }

    hr = pd3dtcSrc->LockRect(D3DCUBEMAP_FACE_NEGATIVE_Y, 4, &d3dlr, NULL, 0);
    if (ResultFailed(hr, TEXT("IDirect3DCubeTexture8::LockRect"))) {
        pd3dtcSrc->Release();
        return TR_ABORT;
    }

    dwData = rand() % 0xFFFFFFFF + 1;
    *(LPDWORD)d3dlr.pBits = dwData;

    hr = pd3dtcSrc->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_Y, 4);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::UnlockRect"))) {
        pd3dtcSrc->Release();
        return TR_ABORT;
    }

    hr = pd3dd->CreateCubeTexture(32, 4, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pd3dtcDst);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateCubeTexture"))) {
        pd3dtcSrc->Release();
        return TR_ABORT;
    }

    hr = pd3dd->UpdateTexture(pd3dtcSrc, pd3dtcDst);
    pd3dtcSrc->Release();
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::UpdateTexture"))) {
        pd3dtcDst->Release();
        tr = TR_FAIL;
    }
    else {

        hr = pd3dtcDst->LockRect(D3DCUBEMAP_FACE_NEGATIVE_Y, 2, &d3dlr, NULL, 0);
        if (ResultFailed(hr, TEXT("IDirect3DCubeTexture8::LockRect"))) {
            pd3dtcDst->Release();
            return TR_ABORT;
        }

        if (*(LPDWORD)d3dlr.pBits != dwData) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::UpdateTexture failed to correctly update the destination cube texture"));
            tr = TR_FAIL;
        }

        hr = pd3dtcDst->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_Y, 2);
        pd3dtcDst->Release();
        if (ResultFailed(hr, TEXT("IDirect3DCubeTexture8::UnlockRect"))) {
            return TR_ABORT;
        }
    }

    // Volume Texture
    hr = pd3dd->CreateVolumeTexture(32, 256, 64, 0, 0, D3DFMT_R5G6B5, D3DPOOL_SYSTEMMEM, &pd3dtvSrc);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVolumeTexture"))) {
        return TR_ABORT;
    }

    hr = pd3dtvSrc->LockBox(3, &d3dlb, NULL, 0);
    if (ResultFailed(hr, TEXT("IDirect3DVolumeTexture8::LockBox"))) {
        pd3dtvSrc->Release();
        return TR_ABORT;
    }

    dwData = rand() % 0xFFFFFFFF + 1;
    *(LPDWORD)d3dlb.pBits = dwData;

    hr = pd3dtvSrc->UnlockBox(3);
    if (ResultFailed(hr, TEXT("IDirect3DVolumeTexture8::UnlockBox"))) {
        pd3dtvSrc->Release();
        return TR_ABORT;
    }

    hr = pd3dd->CreateVolumeTexture(32, 256, 64, 4, 0, D3DFMT_R5G6B5, D3DPOOL_MANAGED, &pd3dtvDst);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateTexture"))) {
        pd3dtvSrc->Release();
        return TR_ABORT;
    }

    hr = pd3dd->UpdateTexture(pd3dtvSrc, pd3dtvDst);
    pd3dtvSrc->Release();
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::UpdateTexture"))) {
        pd3dtvDst->Release();
        tr = TR_FAIL;
    }
    else {

        hr = pd3dtvDst->LockBox(3, &d3dlb, NULL, 0);
        if (ResultFailed(hr, TEXT("IDirect3DVolumeTexture8::LockBox"))) {
            pd3dtvDst->Release();
            return TR_ABORT;
        }

        if (*(LPDWORD)d3dlb.pBits != dwData) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::UpdateTexture failed to correctly update the destination volumetexture"));
            tr = TR_FAIL;
        }

        hr = pd3dtvDst->UnlockBox(1);
        pd3dtvDst->Release();
        if (ResultFailed(hr, TEXT("IDirect3DVolumeTexture8::UnlockBox"))) {
            return TR_ABORT;
        }
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_ValidateDevice(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    DWORD               dwPasses = 0;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    hr = pd3dd->ValidateDevice(&dwPasses);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::ValidateDevice"))) {
        tr = TR_FAIL;
    }
    else {
        if (dwPasses == 0) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::ValidateDevice reported an incorrect number of passes"));
            tr = TR_FAIL;
        }
    }
    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetCurrentTexturePalette(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_SetCurrentTexturePalette(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    UINT                uPalette, uPaletteGet;
    PALETTEENTRY        ppe[256];
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    uPalette = 100 + rand() % 200;

    hr = pd3dd->SetCurrentTexturePalette(uPalette);
    if (hr != D3DERR_INVALIDCALL) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetCurrentTexturePalette returned 0x%X instead of D3DERR_INVALIDCALL on an invalid entry"), hr);
        tr = TR_FAIL;
    }

    hr = pd3dd->SetPaletteEntries(uPalette, ppe);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetPaletteEntries"))) {
        return TR_ABORT;
    }

    hr = pd3dd->SetCurrentTexturePalette(uPalette);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetCurrentTexturePalette"))) {
        tr = TR_FAIL;
    }
    else {

        hr = pd3dd->GetCurrentTexturePalette(&uPaletteGet);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetCurrentTexturePalette"))) {
            tr = TR_FAIL;
        }
        else {
            if (uPalette != uPaletteGet) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetCurrentTexturePalette failed to set the current palette"));
                tr = TR_FAIL;
            }
        }
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetPaletteEntries(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_SetPaletteEntries(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    PALETTEENTRY        ppeSet[256], ppeGet[256];
    UINT                i, j;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;
    UINT                uPalette[] = {
                            0,
                            1,
                            16,382,
                            16,383
                        };

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    for (i = 0; i < 4; i++) {

        for (j = 0; j < 256; j++) {
            ppeSet[j].peRed = rand() % 256;
            ppeSet[j].peGreen = rand() % 256;
            ppeSet[j].peBlue = rand() % 256;
            ppeSet[j].peFlags = rand() % 256;
        }
        hr = pd3dd->SetPaletteEntries(uPalette[i], ppeSet);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetPaletteEntries"))) {
            tr = TR_FAIL;
        }
        else {
            hr = pd3dd->GetPaletteEntries(uPalette[i], ppeGet);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetPaletteEntries"))) {
                tr = TR_FAIL;
            }
            else {
                if (memcmp(ppeSet, ppeGet, 256 * sizeof(PALETTEENTRY))) {
                    Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetPaletteEntries failed to correctly set the entries for palette %d"), i);
                    tr = TR_FAIL;
                }
            }
        }
    }

    for (i = 0; i < 4; i++) {
        hr = pd3dd->SetPaletteEntries(uPalette[i], NULL);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetPaletteEntries using NULL"))) {
            tr = TR_FAIL;
        }
    }

    return tr;
}

#endif // !UNDER_XBOX

#ifndef UNDER_XBOX

#if 0

//******************************************************************************
HRESULT AssembleShader(BOOL bPixel, LPCTSTR szName, LPD3DXBUFFER* ppFunction) {

    LPD3DXBUFFER        pxbufConstants, pxbufShader, pxbufErrors;
    HRSRC               hrsrc;
    HGLOBAL             hg;
    LPBYTE              pData;
    DWORD               dwSize;
    HRESULT             hr;

    hrsrc = FindResource(g_hInstance, szName, RT_RCDATA);
    if (!hrsrc) {
        Log(LOG_ABORT, TEXT("FindResource failed to find the resource %s"), szName);
        return E_FAIL;
    }

    if (!(hg = LoadResource(g_hInstance, hrsrc))) {
        Log(LOG_ABORT, TEXT("LoadResource failed to load the resource %s"), szName);
        return E_FAIL;
    }
    if (!(pData = (LPBYTE)LockResource(hg))) {
        Log(LOG_ABORT, TEXT("LockResource failed to lock the resource %s"), szName);
        return E_FAIL;
    }

    dwSize = SizeofResource(g_hInstance, hrsrc);

    hr = D3DXAssembleShader((LPVOID)pData, dwSize, 0, &pxbufConstants, &pxbufShader, &pxbufErrors);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("D3DXAssembleVertexShader"));
        Log(LOG_ABORT, TEXT("Assembly errors:\n%s"), (LPSTR)pxbufErrors->GetBufferPointer());
        pxbufErrors->Release();
        return hr;
    }

    pxbufConstants->Release();

    *ppFunction = pxbufShader;
    return D3D_OK;
}

#else

//******************************************************************************
HRESULT AssembleShader(LPSTR szShader, LPD3DXBUFFER* ppFunction) {

    LPD3DXBUFFER        pxbufConstants, pxbufShader, pxbufErrors;
    HRESULT             hr;

    hr = D3DXAssembleShader((LPVOID)szShader, strlen(szShader), 0, &pxbufConstants, &pxbufShader, &pxbufErrors);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("D3DXAssembleVertexShader"));
        Log(LOG_ABORT, TEXT("Assembly errors:\n%s"), (LPSTR)pxbufErrors->GetBufferPointer());
        pxbufErrors->Release();
        return hr;
    }

    pxbufConstants->Release();

    *ppFunction = pxbufShader;
    return D3D_OK;
}

#endif

#endif // !UNDER_XBOX

//******************************************************************************
TESTPROCAPI TDEV8_CreateVertexShader(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    DWORD               dwShader;
#ifndef UNDER_XBOX
    LPD3DXBUFFER        pxbufShader;
#else
    LPXGBUFFER          pxbufShader;
#endif
    HRESULT             hr;
    TRESULT             tr = TR_PASS;
    DWORD               dwDeclFixed[] = {
                            D3DVSD_STREAM(0),
                            D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
                            D3DVSD_STREAM(4),
                            D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
                            D3DVSD_STREAM(7),
                            D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
                            D3DVSD_END()
                        };
    DWORD               dwDeclProg[] = {
                            D3DVSD_STREAM(0),
                            D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
                            D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
                            D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
                            D3DVSD_END()
                        };

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    // Fixed function shader
    hr = pd3dd->CreateVertexShader(dwDeclFixed, NULL, &dwShader, 0);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexShader for a fixed function shader"))) {
        tr = TR_FAIL;
    }
    else {
        hr = pd3dd->DeleteVertexShader(dwShader);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::DeleteVertexShader"))) {
            tr = TR_FAIL;
        }
    }

    // Programmable shader
#ifndef UNDER_XBOX
    hr = AssembleShader(g_szVertexShader, &pxbufShader);
#else
    hr = AssembleShader("<memory>", g_szVertexShader, strlen(g_szVertexShader), 0, NULL, 
                        &pxbufShader, NULL, NULL, NULL, NULL, NULL);
#endif
    if (ResultFailed(hr, TEXT("AssembleShader"))) {
        return TR_ABORT;
    }
    else {
        hr = pd3dd->CreateVertexShader(dwDeclProg, (LPDWORD)pxbufShader->GetBufferPointer(), &dwShader, 0);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexShader"))) {
            tr = TR_FAIL;
        }
        else {
            hr = pd3dd->DeleteVertexShader(dwShader);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::DeleteVertexShader"))) {
                tr = TR_FAIL;
            }
        }
        pxbufShader->Release();
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_DeleteVertexShader(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

#ifdef INVALID_TESTS
    // Attempt to delete a non-existant shader
    hr = pd3dd->DeleteVertexShader(0xFFFFFFFE);
    if (hr != D3DERR_INVALIDCALL) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::DeleteVertexShader returned 0x%X instead of D3DERR_INVALIDCALL"), hr);
        tr = TR_FAIL;
    }
#endif // INVALID_TESTS

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetVertexShader(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_SetVertexShader(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    UINT                i;
    DWORD               dwShaderGet, dwShaderSet;
#ifndef UNDER_XBOX
    LPD3DXBUFFER        pxbufShader;
#else
    LPXGBUFFER          pxbufShader;
#endif
    HRESULT             hr;
    TRESULT             tr = TR_PASS;
    DWORD               dwDeclProg[] = {
                            D3DVSD_STREAM(0),
                            D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
                            D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
                            D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
                            D3DVSD_END()
                        };
    DWORD               dwFVF[] = {
                            D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX4,
                            D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(0) | D3DFVF_TEXCOORDSIZE1(1),
                            D3DFVF_XYZB3 | D3DFVF_DIFFUSE | D3DFVF_SPECULAR,
                            D3DFVF_XYZRHW | D3DFVF_SPECULAR | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE4(0),
                            D3DFVF_XYZ | D3DFVF_SPECULAR | D3DFVF_TEX2
                        };

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    // Fixed-function
    for (i = 0; i < countof(dwFVF); i++) {
        hr = pd3dd->SetVertexShader(dwFVF[i]);
        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetVertexShader failed on FVF 0x%X [0x%X]"), dwFVF[i], hr);
            tr = TR_FAIL;
        }
        else {
            hr = pd3dd->GetVertexShader(&dwShaderGet);
            if (FAILED(hr)) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShader failed on FVF 0x%X [0x%X]"), dwFVF[i], hr);
                tr = TR_FAIL;
            }
            else {
                if (dwShaderGet != dwFVF[i]) {
                    Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetVertexShader failed to correctly set shader 0x%X (GetVertexShader returned 0x%X)"), dwFVF[i], dwShaderGet);
                    tr = TR_FAIL;
                }
            }
        }
    }

    // Programmable
#ifndef UNDER_XBOX
    hr = AssembleShader(g_szVertexShader, &pxbufShader);
#else
    hr = AssembleShader("<memory>", g_szVertexShader, strlen(g_szVertexShader), 0, NULL, 
                        &pxbufShader, NULL, NULL, NULL, NULL, NULL);
#endif
    if (ResultFailed(hr, TEXT("AssembleShader"))) {
        return TR_ABORT;
    }
    else {
        hr = pd3dd->CreateVertexShader(dwDeclProg, (LPDWORD)pxbufShader->GetBufferPointer(), &dwShaderSet, 0);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexShader"))) {
            tr = TR_ABORT;
        }
        else {
            hr = pd3dd->SetVertexShader(dwShaderSet);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetVertexShader"))) {
                tr = TR_FAIL;
            }
            else {
                hr = pd3dd->GetVertexShader(&dwShaderGet);
                if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetVertexShader"))) {
                    tr = TR_FAIL;
                }
                else {
                    if (dwShaderGet != dwShaderSet) {
                        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetVertexShader failed to correctly set programmable shader 0x%X (GetVertexShader returned 0x%X)"), dwShaderSet, dwShaderGet);
                        tr = TR_FAIL;
                    }
                }
            }
            pd3dd->SetVertexShader(D3DFVF_XYZ);
            pd3dd->DeleteVertexShader(dwShaderSet);
        }
        pxbufShader->Release();
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetVertexShaderConstant(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

#define VSHADER_CONSTANT_REGISTERS 96

//******************************************************************************
TESTPROCAPI TDEV8_SetVertexShaderConstant(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    DWORD               dwRegister;
    DWORD               dwCount;
    float               fConstantSet[VSHADER_CONSTANT_REGISTERS][4];
    float               fConstantGet[VSHADER_CONSTANT_REGISTERS][4];
    UINT                i, j;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    // Clear the registers
    for (i = 0; i < VSHADER_CONSTANT_REGISTERS; i++) {
        for (j = 0; j < 4; j++) {
            fConstantSet[i][j] = 0.0f;
        }
    }

    hr = pd3dd->SetVertexShaderConstant(0, fConstantSet, VSHADER_CONSTANT_REGISTERS);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetVertexShaderConstant"))) {
        return TR_FAIL;
    }

    // Set data in some of the registers
    LogBeginVariation(TEXT("IDirect3DDevice8::SetVertexShaderConstant on a subset of registers"));

    dwCount = (rand() % (VSHADER_CONSTANT_REGISTERS / 2 - 1)) + 1;

    for (i = 0; i < dwCount; i++) {
        for (j = 0; j < 4; j++) {
            fConstantSet[i][j] = NRND() * 10000.0f;
        }
    }

    dwRegister = rand() % (VSHADER_CONSTANT_REGISTERS - dwCount);

    hr = pd3dd->SetVertexShaderConstant(dwRegister, fConstantSet, dwCount);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetVertexShaderConstant"))) {
        return TR_FAIL;
    }

    hr = pd3dd->GetVertexShaderConstant(dwRegister, fConstantGet, dwCount);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetVertexShaderConstant"))) {
        return TR_FAIL;
    }

    if (memcmp(fConstantSet, fConstantGet, dwCount * 4 * sizeof(float))) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetVertexShaderConstant failed to correctly set constant data"));
        return TR_FAIL;
    }

    LogEndVariation();

    // Verify registers outside the range were not modified
    LogBeginVariation(TEXT("IDirect3DDevice8::SetVertexShaderConstant not modifying registers outside the given range"));

    hr = pd3dd->GetVertexShaderConstant(0, fConstantGet, VSHADER_CONSTANT_REGISTERS);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetVertexShaderConstant"))) {
        return TR_FAIL;
    }

    for (i = 0; i < dwRegister; i++) {
        for (j = 0; j < 4; j++) {
            if (fConstantGet[i][j] != 0.0f) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetVertexShaderConstant incorrectly modified registers outside the given range"));
                return TR_FAIL;
            }
        }
    }

    for (i = dwRegister + dwCount; i < VSHADER_CONSTANT_REGISTERS; i++) {
        for (j = 0; j < 4; j++) {
            if (fConstantGet[i][j] != 0.0f) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetVertexShaderConstant incorrectly modified registers outside the given range"));
                return TR_FAIL;
            }
        }
    }

    for (i = dwRegister; i < dwCount; i++) {
        for (j = 0; j < 4; j++) {
            if (fConstantGet[i][j] != fConstantSet[i-dwRegister][j]) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetVertexShaderConstant failed to correctly set the constant data"));
                return TR_FAIL;
            }
        }
    }

    LogEndVariation();

    // Set all registers in one call
    LogBeginVariation(TEXT("IDirect3DDevice8::SetVertexShaderConstant on all registers at once"));

    for (i = 0; i < VSHADER_CONSTANT_REGISTERS; i++) {
        for (j = 0; j < 4; j++) {
            fConstantSet[i][j] = NRND() * 10000.0f;
        }
    }

    hr = pd3dd->SetVertexShaderConstant(0, fConstantSet, VSHADER_CONSTANT_REGISTERS);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetVertexShaderConstant"))) {
        return TR_FAIL;
    }

    hr = pd3dd->GetVertexShaderConstant(0, fConstantGet, VSHADER_CONSTANT_REGISTERS);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetVertexShaderConstant"))) {
        return TR_FAIL;
    }

    if (memcmp(fConstantSet, fConstantGet, VSHADER_CONSTANT_REGISTERS * 4 * sizeof(float))) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetVertexShaderConstant failed to correctly set constant data"));
        return TR_FAIL;
    }

    LogEndVariation();

    // Set the registers one array at a time
    LogBeginVariation(TEXT("IDirect3DDevice8::SetVertexShaderConstant one array at a time"));

    for (i = 0; i < VSHADER_CONSTANT_REGISTERS; i++) {
        for (j = 0; j < 4; j++) {
            fConstantSet[i][j] = NRND() * 10000.0f;
        }
    }

    for (i = 0; i < VSHADER_CONSTANT_REGISTERS; i++) {

        hr = pd3dd->SetVertexShaderConstant(i, fConstantSet[i], 1);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetVertexShaderConstant"))) {
            return TR_FAIL;
        }

        hr = pd3dd->GetVertexShaderConstant(i, fConstantGet[i], 1);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetVertexShaderConstant"))) {
            return TR_FAIL;
        }
    }

    if (memcmp(fConstantSet, fConstantGet, VSHADER_CONSTANT_REGISTERS * 4 * sizeof(float))) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetVertexShaderConstant failed to correctly set constant data"));
        return TR_FAIL;
    }

    LogEndVariation();

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetVertexShaderDeclaration(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    DWORD               dwShader;
#ifndef UNDER_XBOX
    LPD3DXBUFFER        pxbufShader;
#else
    LPXGBUFFER          pxbufShader;
#endif
    HRESULT             hr;
    TRESULT             tr = TR_PASS;
    DWORD               dwSize;
    BYTE                declGet[1024];
    DWORD               dwDeclFixed[] = {
                            D3DVSD_STREAM(0),
                            D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
                            D3DVSD_STREAM(1),
                            D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
                            D3DVSD_STREAM(2),
                            D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
                            D3DVSD_END()
                        };
    DWORD               dwDeclProg[] = {
                            D3DVSD_STREAM(0),
                            D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
                            D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
                            D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
                            D3DVSD_END()
                        };

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    // Fixed function shader
    hr = pd3dd->CreateVertexShader(dwDeclFixed, NULL, &dwShader, 0);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexShader for a fixed function shader"))) {
        tr = TR_ABORT;
    }
    else {
        dwSize = 1;
        declGet[0] = 0x5C;
        hr = pd3dd->GetVertexShaderDeclaration(dwShader, declGet, &dwSize);
#ifndef UNDER_XBOX
        if (hr != D3DERR_MOREDATA) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShaderDeclaration returned 0x%X instead of D3DERR_MOREDATA for a buffer and too small a size"), hr);
            tr = TR_FAIL;
        }
#endif // !UNDER_XBOX

        if (dwSize != sizeof(dwDeclFixed)) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShaderDeclaration returned an incorrect size of %d for the declaration buffer"), dwSize);
            tr = TR_FAIL;
        }

        if (declGet[0] != 0x5C) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShaderDeclaration incorrectly modified the declaration buffer when given too small a buffer"));
            tr = TR_FAIL;
        }

        dwSize = 1;
        hr = pd3dd->GetVertexShaderDeclaration(dwShader, NULL, &dwSize);
        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShaderDeclaration returned 0x%X instead of D3D_OK for a NULL buffer and too small a size"), hr);
            tr = TR_FAIL;
        }

        if (dwSize != sizeof(dwDeclFixed)) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShaderDeclaration returned an incorrect size of %d for the declaration buffer"), dwSize);
            tr = TR_FAIL;
        }

        dwSize = 1023;
        hr = pd3dd->GetVertexShaderDeclaration(dwShader, declGet, &dwSize);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetVertexShaderDeclaration"))) {
            tr = TR_FAIL;
        }

#ifndef UNDER_XBOX
        if (dwSize != 1023) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShaderDeclaration incorrectly modified the buffer size for the declaration buffer when given a size greater than the size of the declaration"));
            tr = TR_FAIL;
        }

        if (memcmp(declGet, dwDeclFixed, sizeof(dwDeclFixed))) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShaderDeclaration returned an incorrect declaration for the given fixed function shader"));
            tr = TR_FAIL;
        }
#endif // !UNDER_XBOX

        pd3dd->DeleteVertexShader(dwShader);
    }

    // Programmable shader
#ifndef UNDER_XBOX
    hr = AssembleShader(g_szVertexShader, &pxbufShader);
#else
    hr = AssembleShader("<memory>", g_szVertexShader, strlen(g_szVertexShader), 0, NULL, 
                        &pxbufShader, NULL, NULL, NULL, NULL, NULL);
#endif
    if (ResultFailed(hr, TEXT("AssembleShader"))) {
        return TR_ABORT;
    }
    else {
        hr = pd3dd->CreateVertexShader(dwDeclProg, (LPDWORD)pxbufShader->GetBufferPointer(), &dwShader, 0);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexShader"))) {
            tr = TR_ABORT;
        }
        else {

            dwSize = 1023;
            hr = pd3dd->GetVertexShaderDeclaration(dwShader, declGet, &dwSize);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetVertexShaderDeclaration"))) {
                tr = TR_FAIL;
            }

#ifndef UNDER_XBOX
            if (dwSize != 1023) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShaderDeclaration incorrectly modified the buffer size for the declaration buffer when given a size greater than the size of the declaration"));
                tr = TR_FAIL;
            }

            if (memcmp(declGet, dwDeclProg, sizeof(dwDeclProg))) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShaderDeclaration returned an incorrect declaration for the given programable shader"));
                tr = TR_FAIL;
            }
#endif // !UNDER_XBOX

            pd3dd->DeleteVertexShader(dwShader);
        }
        pxbufShader->Release();
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetVertexShaderFunction(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    DWORD               dwShader;
    BYTE                funcGet[8192];
    DWORD               dwSize;
#ifndef UNDER_XBOX
    LPD3DXBUFFER        pxbufShader;
#else
    LPXGBUFFER          pxbufShader;
#endif
    HRESULT             hr;
    TRESULT             tr = TR_PASS;
    DWORD               dwDeclProg[] = {
                            D3DVSD_STREAM(0),
                            D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
                            D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
                            D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
                            D3DVSD_END()
                        };

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

#ifndef UNDER_XBOX
    hr = AssembleShader(g_szVertexShader, &pxbufShader);
#else
    hr = AssembleShader("<memory>", g_szVertexShader, strlen(g_szVertexShader), 0, NULL, 
                        &pxbufShader, NULL, NULL, NULL, NULL, NULL);
#endif
    if (ResultFailed(hr, TEXT("AssembleShader"))) {
        return TR_ABORT;
    }
    else {
        hr = pd3dd->CreateVertexShader(dwDeclProg, (LPDWORD)pxbufShader->GetBufferPointer(), &dwShader, 0);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexShader"))) {
            tr = TR_ABORT;
        }
        else {

            dwSize = 1;
            funcGet[0] = 0xF1;
            hr = pd3dd->GetVertexShaderFunction(dwShader, funcGet, &dwSize);
#ifndef UNDER_XBOX
            if (hr != D3DERR_MOREDATA) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShaderFunction returned 0x%X instead of D3DERR_MOREDATA for a buffer and too small a size"), hr);
                tr = TR_FAIL;
            }
#endif // !UNDER_XBOX

            if (dwSize != pxbufShader->GetBufferSize()) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShaderFunction returned an incorrect size of %d for the function buffer"), dwSize);
                tr = TR_FAIL;
            }

            if (funcGet[0] != 0xF1) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShaderFunction incorrectly modified the function buffer when given too small a buffer"));
                tr = TR_FAIL;
            }

            dwSize = 1;
            hr = pd3dd->GetVertexShaderFunction(dwShader, NULL, &dwSize);
            if (FAILED(hr)) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShaderFunction returned 0x%X instead of D3D_OK for a NULL buffer and too small a size"), hr);
                tr = TR_FAIL;
            }

            if (dwSize != pxbufShader->GetBufferSize()) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShaderFunction returned an incorrect size of %d for the function buffer"), dwSize);
                tr = TR_FAIL;
            }

            dwSize = 8189;
            hr = pd3dd->GetVertexShaderFunction(dwShader, funcGet, &dwSize);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetVertexShaderFunction"))) {
                tr = TR_FAIL;
            }

#ifndef UNDER_XBOX
            if (dwSize != 8189) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShaderFunction incorrectly modified the buffer size for the function buffer when given a size greater than the size of the function"));
                tr = TR_FAIL;
            }

            if (memcmp(funcGet, pxbufShader->GetBufferPointer(), pxbufShader->GetBufferSize())) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShaderFunction returned an incorrect function for the given programmable shader"));
                tr = TR_FAIL;
            }
#endif // !UNDER_XBOX

            pd3dd->DeleteVertexShader(dwShader);
        }
        pxbufShader->Release();
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_CreatePixelShader(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    DWORD               dwShader;
#ifndef UNDER_XBOX
    LPD3DXBUFFER        pxbufShader;
#else
    LPXGBUFFER          pxbufShader;
#endif // UNDER_XBOX
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

#ifndef UNDER_XBOX
    hr = AssembleShader(g_szPixelShader, &pxbufShader);
#else
    hr = AssembleShader("<memory>", g_szPixelShader, strlen(g_szPixelShader), 0, NULL, 
                        &pxbufShader, NULL, NULL, NULL, NULL, NULL);
#endif // UNDER_XBOX
    if (ResultFailed(hr, TEXT("AssembleShader"))) {
        return TR_ABORT;
    }
    else {
#ifndef UNDER_XBOX
        hr = pd3dd->CreatePixelShader((LPDWORD)pxbufShader->GetBufferPointer(), &dwShader);
#else
        hr = pd3dd->CreatePixelShader((D3DPIXELSHADERDEF*)pxbufShader->GetBufferPointer(), &dwShader);
#endif // UNDER_XBOX
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreatePixelShader"))) {
            tr = TR_FAIL;
        }
        else {
            hr = pd3dd->DeletePixelShader(dwShader);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::DeletePixelShader"))) {
                tr = TR_FAIL;
            }
        }
        pxbufShader->Release();
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_DeletePixelShader(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

#ifdef INVALID_TESTS
    // Attempt to delete a non-existant shader
    hr = pd3dd->DeletePixelShader(0xFFFFFFFE);
    if (hr != D3DERR_INVALIDCALL) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::DeletePixelShader returned 0x%X instead of D3DERR_INVALIDCALL"), hr);
        tr = TR_FAIL;
    }
#endif // INVALID_TESTS

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetPixelShader(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_SetPixelShader(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    DWORD               dwShaderSet, dwShaderGet;
#ifndef UNDER_XBOX
    LPD3DXBUFFER        pxbufShader;
#else
    LPXGBUFFER          pxbufShader;
#endif // UNDER_XBOX
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    // Programmable shader
#ifndef UNDER_XBOX
    hr = AssembleShader(g_szPixelShader, &pxbufShader);
#else
    hr = AssembleShader("<memory>", g_szPixelShader, strlen(g_szPixelShader), 0, NULL, 
                        &pxbufShader, NULL, NULL, NULL, NULL, NULL);
#endif // UNDER_XBOX
    if (ResultFailed(hr, TEXT("AssembleShader"))) {
        return TR_ABORT;
    }
    else {
#ifndef UNDER_XBOX
        hr = pd3dd->CreatePixelShader((LPDWORD)pxbufShader->GetBufferPointer(), &dwShaderSet);
#else
        hr = pd3dd->CreatePixelShader((D3DPIXELSHADERDEF*)pxbufShader->GetBufferPointer(), &dwShaderSet);
#endif // UNDER_XBOX
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreatePixelShader"))) {
            tr = TR_ABORT;
        }
        else {
            hr = pd3dd->SetPixelShader(dwShaderSet);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetPixelShader"))) {
                tr = TR_FAIL;
            }
            else {
                hr = pd3dd->GetPixelShader(&dwShaderGet);
                if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetPixelShader"))) {
                    tr = TR_FAIL;
                }
                else {
                    if (dwShaderGet != dwShaderSet) {
                        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetPixelShader failed to set shader 0x%X (GetPixelShader returned 0x%X)"), dwShaderSet, dwShaderGet);
                        tr = TR_FAIL;
                    }
                }
            }

            pd3dd->SetPixelShader(NULL);
            pd3dd->DeletePixelShader(dwShaderSet);
        }
        pxbufShader->Release();
    }

    // Fixed-function shader
    hr = pd3dd->SetPixelShader(NULL);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetPixelShader"))) {
        tr = TR_FAIL;
    }
    else {
        hr = pd3dd->GetPixelShader(&dwShaderGet);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetPixelShader"))) {
            tr = TR_FAIL;
        }
        else {
            if (dwShaderGet != NULL) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetPixelShader failed to set the fixed-function shader (GetPixelShader returned 0x%X)"), dwShaderGet);
                tr = TR_FAIL;
            }
        }
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetPixelShaderConstant(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

#define PSHADER_CONSTANT_REGISTERS 8

//******************************************************************************
TESTPROCAPI TDEV8_SetPixelShaderConstant(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    DWORD               dwRegister;
    DWORD               dwCount;
    float               fConstantSet[PSHADER_CONSTANT_REGISTERS][4];
    float               fConstantGet[PSHADER_CONSTANT_REGISTERS][4];
    UINT                i, j;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;
#ifdef UNDER_XBOX
    DWORD               dwShader;
    LPXGBUFFER          pxbufShader;
#endif

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

#ifdef UNDER_XBOX
    hr = AssembleShader("<memory>", g_szPixelShader, strlen(g_szPixelShader), 0, NULL, 
                        &pxbufShader, NULL, NULL, NULL, NULL, NULL);
    if (ResultFailed(hr, TEXT("AssembleShader"))) {
        return TR_ABORT;
    }

    hr = pd3dd->CreatePixelShader((D3DPIXELSHADERDEF*)pxbufShader->GetBufferPointer(), &dwShader);
    pxbufShader->Release();
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreatePixelShader"))) {
        return TR_ABORT;
    }

    hr = pd3dd->SetPixelShader(dwShader);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetPixelShader"))) {
        pd3dd->DeletePixelShader(dwShader);
        return TR_ABORT;
    }
#endif // UNDER_XBOX

    // Clear the registers
    for (i = 0; i < PSHADER_CONSTANT_REGISTERS; i++) {
        for (j = 0; j < 4; j++) {
            fConstantSet[i][j] = 0.0f;
        }
    }

    hr = pd3dd->SetPixelShaderConstant(0, fConstantSet, PSHADER_CONSTANT_REGISTERS);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetPixelShaderConstant"))) {
        tr = TR_FAIL;
        goto exit;
    }

    // Set data in some of the registers
    LogBeginVariation(TEXT("IDirect3DDevice8::SetPixelShaderConstant on a subset of registers"));

    dwCount = rand() % (PSHADER_CONSTANT_REGISTERS / 2);

    for (i = 0; i < dwCount; i++) {
        for (j = 0; j < 4; j++) {
            fConstantSet[i][j] = (float)(rand() % 256) / 255.0f;
        }
    }

    dwRegister = rand() % (PSHADER_CONSTANT_REGISTERS - dwCount);

    hr = pd3dd->SetPixelShaderConstant(dwRegister, fConstantSet, dwCount);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetPixelShaderConstant"))) {
        tr = TR_FAIL;
        goto exit;
    }

    hr = pd3dd->GetPixelShaderConstant(dwRegister, fConstantGet, dwCount);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetPixelShaderConstant"))) {
        tr = TR_FAIL;
        goto exit;
    }

    if (memcmp(fConstantSet, fConstantGet, dwCount * 4 * sizeof(float))) {
        D3DXVECTOR4 *pvSet, *pvGet;
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetPixelShaderConstant failed to correctly set constant data"));
        for (i = 0, pvSet = (D3DXVECTOR4*)fConstantSet[0], pvGet = (D3DXVECTOR4*)fConstantGet[0]; i < dwCount; i++, pvSet++, pvGet++) {
            Log(LOG_COMMENT, TEXT("Register %d: Set <%3.3f, %3.3f, %3.3f, %3.3f>, Get <%3.3f, %3.3f, %3.3f, %3.3f>"), dwRegister + i, pvSet->x, pvSet->y, pvSet->z, pvSet->w, pvGet->x, pvGet->y, pvGet->z, pvGet->w);
        }
        tr = TR_FAIL;
        goto exit;
    }

    LogEndVariation();

    // Verify registers outside the range were not modified
    LogBeginVariation(TEXT("IDirect3DDevice8::SetPixelShaderConstant not modifying registers outside the given range"));

    hr = pd3dd->GetPixelShaderConstant(0, fConstantGet, PSHADER_CONSTANT_REGISTERS);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetPixelShaderConstant"))) {
        tr = TR_FAIL;
        goto exit;
    }

    for (i = 0; i < dwRegister; i++) {
        for (j = 0; j < 4; j++) {
            if (fConstantGet[i][j] != 0.0f) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetPixelShaderConstant incorrectly modified registers outside the given range"));
                tr = TR_FAIL;
                goto exit;
            }
        }
    }

    for (i = dwRegister + dwCount; i < PSHADER_CONSTANT_REGISTERS; i++) {
        for (j = 0; j < 4; j++) {
            if (fConstantGet[i][j] != 0.0f) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetPixelShaderConstant incorrectly modified registers outside the given range"));
                tr = TR_FAIL;
                goto exit;
            }
        }
    }

    for (i = dwRegister; i < dwCount; i++) {
        for (j = 0; j < 4; j++) {
            if (fConstantGet[i][j] != fConstantSet[i-dwRegister][j]) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetPixelShaderConstant failed to correctly set the constant data"));
                tr = TR_FAIL;
                goto exit;
            }
        }
    }

    LogEndVariation();

    // Set all registers in one call
    LogBeginVariation(TEXT("IDirect3DDevice8::SetPixelShaderConstant on all registers at once"));

    for (i = 0; i < PSHADER_CONSTANT_REGISTERS; i++) {
        for (j = 0; j < 4; j++) {
            fConstantSet[i][j] = (float)(rand() % 256) / 255.0f;
        }
    }

    hr = pd3dd->SetPixelShaderConstant(0, fConstantSet, PSHADER_CONSTANT_REGISTERS);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetPixelShaderConstant"))) {
        tr = TR_FAIL;
        goto exit;
    }

    hr = pd3dd->GetPixelShaderConstant(0, fConstantGet, PSHADER_CONSTANT_REGISTERS);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetPixelShaderConstant"))) {
        tr = TR_FAIL;
        goto exit;
    }

    if (memcmp(fConstantSet, fConstantGet, PSHADER_CONSTANT_REGISTERS * 4 * sizeof(float))) {
        D3DXVECTOR4 *pvSet, *pvGet;
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetPixelShaderConstant failed to correctly set constant data"));
        for (i = 0, pvSet = (D3DXVECTOR4*)fConstantSet[0], pvGet = (D3DXVECTOR4*)fConstantGet[0]; i < PSHADER_CONSTANT_REGISTERS; i++, pvSet++, pvGet++) {
            Log(LOG_COMMENT, TEXT("Register %d: Set <%3.3f, %3.3f, %3.3f, %3.3f>, Get <%3.3f, %3.3f, %3.3f, %3.3f>"), i, pvSet->x, pvSet->y, pvSet->z, pvSet->w, pvGet->x, pvGet->y, pvGet->z, pvGet->w);
        }
        tr = TR_FAIL;
        goto exit;
    }

    LogEndVariation();

    // Set the registers one array at a time
    LogBeginVariation(TEXT("IDirect3DDevice8::SetPixelShaderConstant one array at a time"));

    for (i = 0; i < PSHADER_CONSTANT_REGISTERS; i++) {
        for (j = 0; j < 4; j++) {
            fConstantSet[i][j] = (float)(rand() % 256) / 255.0f;
        }
    }

    for (i = 0; i < PSHADER_CONSTANT_REGISTERS; i++) {

        hr = pd3dd->SetPixelShaderConstant(i, fConstantSet[i], 1);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetPixelShaderConstant"))) {
            tr = TR_FAIL;
            goto exit;
        }

        hr = pd3dd->GetPixelShaderConstant(i, fConstantGet[i], 1);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetPixelShaderConstant"))) {
            tr = TR_FAIL;
            goto exit;
        }
    }

    if (memcmp(fConstantSet, fConstantGet, PSHADER_CONSTANT_REGISTERS * 4 * sizeof(float))) {
        D3DXVECTOR4 *pvSet, *pvGet;
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetPixelShaderConstant failed to correctly set constant data"));
        for (i = 0, pvSet = (D3DXVECTOR4*)fConstantSet[0], pvGet = (D3DXVECTOR4*)fConstantGet[0]; i < PSHADER_CONSTANT_REGISTERS; i++, pvSet++, pvGet++) {
            Log(LOG_COMMENT, TEXT("Register %d: Set <%3.3f, %3.3f, %3.3f, %3.3f>, Get <%3.3f, %3.3f, %3.3f, %3.3f>"), i, pvSet->x, pvSet->y, pvSet->z, pvSet->w, pvGet->x, pvGet->y, pvGet->z, pvGet->w);
        }
        tr = TR_FAIL;
        goto exit;
    }

    LogEndVariation();

exit:

#ifdef UNDER_XBOX
    pd3dd->SetPixelShader(NULL);
    pd3dd->DeletePixelShader(dwShader);
#endif

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetPixelShaderFunction(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    DWORD               dwShader;
    BYTE                funcGet[8192];
    DWORD               dwSize;
#ifndef UNDER_XBOX
    LPD3DXBUFFER        pxbufShader;
#else
    LPXGBUFFER          pxbufShader;
#endif // UNDER_XBOX
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

#ifndef UNDER_XBOX
    hr = AssembleShader(g_szPixelShader, &pxbufShader);
#else
    hr = AssembleShader("<memory>", g_szPixelShader, strlen(g_szPixelShader), 0, NULL, 
                        &pxbufShader, NULL, NULL, NULL, NULL, NULL);
#endif // UNDER_XBOX
    if (ResultFailed(hr, TEXT("AssembleShader"))) {
        return TR_ABORT;
    }
    else {
#ifndef UNDER_XBOX
        hr = pd3dd->CreatePixelShader((LPDWORD)pxbufShader->GetBufferPointer(), &dwShader);
#else
        hr = pd3dd->CreatePixelShader((D3DPIXELSHADERDEF*)pxbufShader->GetBufferPointer(), &dwShader);
#endif // UNDER_XBOX
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreatePixelShader"))) {
            tr = TR_ABORT;
        }
        else {

#ifndef UNDER_XBOX

            dwSize = 1;
            funcGet[0] = 0x6E;
            hr = pd3dd->GetPixelShaderFunction(dwShader, funcGet, &dwSize);
            if (hr != D3DERR_MOREDATA) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetPixelShaderFunction returned 0x%X instead of D3DERR_MOREDATA for a buffer and too small a size"), hr);
                tr = TR_FAIL;
            }

            if (dwSize != pxbufShader->GetBufferSize()) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetPixelShaderFunction returned an incorrect size of %d for the function buffer"), dwSize);
                tr = TR_FAIL;
            }

            if (funcGet[0] != 0x6E) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetPixelShaderFunction incorrectly modified the function buffer when given too small a buffer"));
                tr = TR_FAIL;
            }

            dwSize = 1;
            hr = pd3dd->GetPixelShaderFunction(dwShader, NULL, &dwSize);
            if (FAILED(hr)) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetPixelShaderFunction returned 0x%X instead of D3D_OK for a NULL buffer and too small a size"), hr);
                tr = TR_FAIL;
            }

            if (dwSize != pxbufShader->GetBufferSize()) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetPixelShaderFunction returned an incorrect size of %d for the function buffer"), dwSize);
                tr = TR_FAIL;
            }

            dwSize = 8189;
            hr = pd3dd->GetPixelShaderFunction(dwShader, funcGet, &dwSize);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetPixelShaderFunction"))) {
                tr = TR_FAIL;
            }

            if (dwSize != 8189) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetPixelShaderFunction incorrectly modified the buffer size for the function buffer when given a size greater than the size of the function"));
                tr = TR_FAIL;
            }

            if (memcmp(funcGet, pxbufShader->GetBufferPointer(), pxbufShader->GetBufferSize())) {
                Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetPixelShaderFunction returned an incorrect function for the given programmable shader"));
                tr = TR_FAIL;
            }

#else
            D3DPIXELSHADERDEF d3dpsd;

            hr = pd3dd->GetPixelShaderFunction(dwShader, &d3dpsd);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetPixelShaderFunction"))) {
                tr = TR_FAIL;
            }
            else {
                if (memcmp(&d3dpsd, pxbufShader->GetBufferPointer(), pxbufShader->GetBufferSize())) {
                    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetPixelShaderFunction returned an incorrect definition for the given programmable shader"));
                    tr = TR_FAIL;
                }
            }
#endif

            pd3dd->DeletePixelShader(dwShader);
        }
        pxbufShader->Release();
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_Present(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    // ##TODO: If the copy and copyvsync swap effects are supported, test the
    // presentation of subrects and dirty regions

    hr = pd3dd->Present(NULL, NULL, NULL, NULL);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::Present"))) {
        return TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_Reset(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8       pd3dd;
    D3DPRESENT_PARAMETERS   d3dpp;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    // ##REVIEW: Will calling Reset cause UMA resources to be lost on XBox?

    // Initialize the presentation parameters
    memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    d3dpp.BackBufferWidth                   = TARGET_WIDTH;
    d3dpp.BackBufferHeight                  = TARGET_HEIGHT;
    d3dpp.BackBufferFormat                  = D3DFMT_X8R8G8B8;
#ifndef UNDER_XBOX
    d3dpp.MultiSampleType                   = D3DMULTISAMPLE_4_SAMPLES;
#else
    d3dpp.MultiSampleType                   = D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR;
#endif
    d3dpp.AutoDepthStencilFormat            = D3DFMT_D24S8;
    d3dpp.FullScreen_RefreshRateInHz        = 0;
    d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.BackBufferCount                   = 1;
    d3dpp.SwapEffect                        = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                     = g_hWnd;
    d3dpp.Windowed                          = FALSE;
    d3dpp.EnableAutoDepthStencil            = TRUE;

    hr = pd3dd->Reset(&d3dpp);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::Reset"))) {
        return TR_FAIL;
    }

    // Initialize the presentation parameters
    memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    d3dpp.BackBufferWidth                   = TARGET_WIDTH;
    d3dpp.BackBufferHeight                  = TARGET_HEIGHT;
    d3dpp.BackBufferFormat                  = D3DFMT_R5G6B5;
    d3dpp.MultiSampleType                   = D3DMULTISAMPLE_NONE;
    d3dpp.AutoDepthStencilFormat            = D3DFMT_D16;
    d3dpp.FullScreen_RefreshRateInHz        = 0;
    d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.BackBufferCount                   = 1;
    d3dpp.SwapEffect                        = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                     = g_hWnd;
    d3dpp.Windowed                          = TRUE;
// ##HACK
//    d3dpp.EnableAutoDepthStencil            = TRUE;
    d3dpp.EnableAutoDepthStencil            = FALSE;

    hr = pd3dd->Reset(&d3dpp);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::Reset"))) {
        return TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_BeginScene(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    hr = pd3dd->BeginScene();
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::BeginScene"))) {
        return TR_FAIL;
    }

    hr = pd3dd->BeginScene();
#ifndef UNDER_XBOX
    if (hr != D3DERR_INVALIDCALL) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::BeginScene returned 0x%X instead of D3DERR_INVALIDCALL when already in a scene"));
        tr = TR_FAIL;
    }
#endif

    pd3dd->EndScene();

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_EndScene(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    pd3dd->BeginScene();

    hr = pd3dd->EndScene();
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::EndScene"))) {
        return TR_FAIL;
    }

    hr = pd3dd->EndScene();
#ifndef UNDER_XBOX
    if (hr != D3DERR_INVALIDCALL) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::EndScene returned 0x%X instead of D3DERR_INVALIDCALL when not in a scene"));
        tr = TR_FAIL;
    }
#endif

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_DrawIndexedPrimitive(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_DrawIndexedPrimitiveUP(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_DrawPrimitive(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_DrawPrimitiveUP(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_DeleteSurfaceHandle(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_DrawRectSurface(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_DrawTriSurface(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetStreamSource(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    TRESULT             tr = TR_PASS;

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_SetStreamSource(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8       pd3dd;
    LPDIRECT3DVERTEXBUFFER8 pd3dr[8], pd3drGet;
    UINT                    uStrideGet;
    ULONG                   uRef1, uRef2;
    UINT                    i, j;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;
    DWORD                   dwFVF[] = {
                                D3DFVF_NORMAL,
                                D3DFVF_DIFFUSE,
                                D3DFVF_TEXCOORDSIZE1(1),
                                D3DFVF_XYZ,
                                D3DFVF_TEXCOORDSIZE4(3),
                                D3DFVF_SPECULAR,
                                D3DFVF_TEXCOORDSIZE2(2),
                                D3DFVF_TEXCOORDSIZE3(0),
                            };
    DWORD                   dwVSize[] = {
                                sizeof(D3DVECTOR),
                                sizeof(D3DCOLOR),
                                sizeof(float),
                                sizeof(D3DVECTOR),
                                4 * sizeof(float),
                                sizeof(D3DCOLOR),
                                2 * sizeof(float),
                                3 * sizeof(float)
                            };

/*
    DWORD                   dwFVF[] = {
                                D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX4,
                                D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEXCOORDSIZE3(0) | D3DFVF_TEXCOORDSIZE1(1),
                                D3DFVF_XYZB3 | D3DFVF_DIFFUSE | D3DFVF_SPECULAR,
                                D3DFVF_XYZRHW | D3DFVF_SPECULAR | D3DFVF_TEXCOORDSIZE4(0),
                                D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_SPECULAR | D3DFVF_TEX2,

                            };
    DWORD                   dwVSize[] = {
                                2 * sizeof(D3DVECTOR) + 8 * sizeof(float),
                                sizeof(D3DVECTOR) + sizeof(D3DCOLOR) + 4 * sizeof(float),
                                sizeof(D3DVECTOR) + 3 * sizeof(float) + 2 * sizeof(D3DCOLOR),
                                sizeof(D3DVECTOR) + sizeof(float) + sizeof(D3DCOLOR) + 4 * sizeof(float),
                                sizeof(D3DVECTOR) + sizeof(float) + sizeof(D3DCOLOR) + 
*/

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    for (i = 0; i < 8; i++) {
        hr = pd3dd->CreateVertexBuffer(dwVSize[i] * 912, 0, dwFVF[i], D3DPOOL_MANAGED, &pd3dr[i]);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexBuffer"))) {
            for (j = 0; j < i; j++) pd3dr[j]->Release();
            return TR_ABORT;
        }
    }

    for (i = 0; i < 8; i++) {
        hr = pd3dd->SetStreamSource(i, pd3dr[i], dwVSize[i]);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetStreamSource"))) {
            tr = TR_FAIL;
        }
    }

    if (tr == TR_PASS) {
        for (i = 0; i < 8; i++) {
            pd3dr[i]->AddRef();
            uRef1 = pd3dr[i]->Release();
            hr = pd3dd->GetStreamSource(i, &pd3drGet, &uStrideGet);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetStreamSource"))) {
                tr = TR_FAIL;
            }
            else {
                if (pd3drGet != pd3dr[i]) {
                    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetStreamSource returned an incorrect vertex buffer pointer for stream %d"), i);
                    tr = TR_FAIL;
                }
                if (uStrideGet != dwVSize[i]) {
                    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetStreamSource returned an incorrect stride for stream %d"), i);
                    tr = TR_FAIL;
                }
                pd3dr[i]->AddRef();
                uRef2 = pd3dr[i]->Release();
                if (uRef2 != uRef1 + 1) {
                    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetStreamSource failed to increment the reference count on the returned vertex buffer in stream %d"), i);
                    tr = TR_FAIL;
                }
                else {
                    pd3drGet->Release();
                }
            }
        }
    }

    for (i = 0; i < 8; i++) {
        pd3dr[i]->Release();
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetIndices(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_SetIndices(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8       pd3dd;
    LPDIRECT3DINDEXBUFFER8  pd3diSet, pd3diGet;
    UINT                    uBase;
    ULONG                   uRef1, uRef2;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;

    pd3dd = GetDevice8();
    pd3diSet = GetIndexBuffer8();
    if (!pd3dd || !pd3diSet) {
        return TR_ABORT;
    }

    hr = pd3dd->SetIndices(pd3diSet, 7);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetIndices"))) {
        return TR_FAIL;
    }

    pd3diSet->AddRef();
    uRef1 = pd3diSet->Release();

    hr = pd3dd->GetIndices(&pd3diGet, &uBase);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetIndices"))) {
        return TR_FAIL;
    }

    if (pd3diGet != pd3diSet) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetIndices returned an incorrect index buffer pointer"));
        tr = TR_FAIL;
    }
    if (uBase != 7) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetIndices returned an incorrect base index"));
        tr = TR_FAIL;
    }
    pd3diSet->AddRef();
    uRef2 = pd3diSet->Release();
    if (uRef2 != uRef1 + 1) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetIndices failed to increment the reference count on the returned index buffer"));
        tr = TR_FAIL;
    }
    else {
        pd3diGet->Release();
    }

    return tr;
}

#ifndef UNDER_XBOX

//******************************************************************************
TESTPROCAPI TDEV8_GetAvailableTextureMem(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    UINT                uAvailable;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    uAvailable = pd3dd->GetAvailableTextureMem();

    Log(LOG_COMMENT, TEXT("IDirect3DDevice8::GetAvailableTextureMem reported %d bytes of available texture memory"));

    return tr;
}

#endif // !UNDER_XBOX

//******************************************************************************
TESTPROCAPI TDEV8_GetCreationParameters(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8               pd3dd;
    D3DDEVICE_CREATION_PARAMETERS   d3ddcp;
    HRESULT                         hr;
    TRESULT                         tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    hr = pd3dd->GetCreationParameters(&d3ddcp);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetCreationParameters"))) {
        return TR_FAIL;
    }

    if (d3ddcp.AdapterOrdinal != 0 ||
        d3ddcp.DeviceType != D3DDEVTYPE_HAL ||
        d3ddcp.BehaviorFlags != D3DCREATE_HARDWARE_VERTEXPROCESSING ||
        d3ddcp.hFocusWindow != g_hWnd) 
    {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetCreationParameters"));
        tr = TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetDeviceCaps(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    D3DCAPS8            d3dcaps;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    hr = pd3dd->GetDeviceCaps(&d3dcaps);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDeviceCaps"))) {
        return TR_FAIL;
    }

    // ##TODO: Verify the device caps

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetDirect3D(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    LPDIRECT3D8         pd3d, pd3dGet;
    ULONG               uRef1, uRef2;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    pd3d = GetDeviceDirect3D8();
    if (!pd3dd || !pd3d) {
        return TR_ABORT;
    }

    pd3d->AddRef();
    uRef1 = pd3d->Release();

    hr = pd3dd->GetDirect3D(&pd3dGet);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDirect3D"))) {
        return TR_FAIL;
    }

    if (pd3dGet != pd3d) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetDirect3D returned the incorrect interface pointer"));
        tr = TR_FAIL;
    }

    pd3d->AddRef();
    uRef2 = pd3d->Release();
#ifndef UNDER_XBOX
    if (uRef2 != uRef1 + 1) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetDirect3D failed to increment the reference count on the Direct3D object"));
        tr = TR_FAIL;
    }
    else {
#endif
        pd3dGet->Release();
#ifndef UNDER_XBOX
    }
#endif

    return tr;
}

#ifndef UNDER_XBOX
//******************************************************************************
TESTPROCAPI TDEV8_GetInfo(PTESTTABLEENTRY ptte) {

#if 0
    LPDIRECT3DDEVICE8               pd3dd;
    D3DDEVINFO_TEXTUREMANAGER       d3dinfoD3DTexMan;
    D3DDEVINFO_TEXTUREMANAGER       d3dinfoTexMan;
    D3DDEVINFO_TEXTURING            d3dinfoTex;
    HRESULT                         hr;
    TRESULT                         tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    hr = pd3dd->GetInfo(D3DDEVINFOID_D3DTEXTUREMANAGER, &d3dinfoD3DTexMan, sizeof(d3dinfoD3DTexMan));
    if (hr != E_NOTIMPL) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetInfo returned 0x%X instead of E_NOTIMPL"), hr);
        tr = TR_FAIL;
    }

    hr = pd3dd->GetInfo(D3DDEVINFOID_TEXTUREMANAGER, &d3dinfoTexMan, sizeof(d3dinfoTexMan));
    if (hr != E_NOTIMPL) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetInfo returned 0x%X instead of E_NOTIMPL"), hr);
        tr = TR_FAIL;
    }

    hr = pd3dd->GetInfo(D3DDEVINFOID_TEXTURING, &d3dinfoTex, sizeof(d3dinfoTex));
    if (hr != E_NOTIMPL) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetInfo returned 0x%X instead of E_NOTIMPL"), hr);
        tr = TR_FAIL;
    }

    return tr;

#else
    return TR_FAIL; // Since D3DDEVINFO_TEXTUREMANAGER struct is currently #defined out of DX8
#endif
}
#endif // !UNDER_XBOX

//******************************************************************************
TESTPROCAPI TDEV8_GetRasterStatus(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    D3DRASTER_STATUS    d3drs;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    hr = pd3dd->GetRasterStatus(&d3drs);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetRasterStatus"))) {
        return TR_FAIL;
    }

    Log(LOG_COMMENT, TEXT("IDirect3DDevice8::GetRasterStatus reported the raster status as %s the vblank (scan line %d)"), d3drs.InVBlank ? TEXT("in") : TEXT("not in"), d3drs.ScanLine);

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetBackBuffer(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetDepthStencilSurface(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetGammaRamp(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_SetGammaRamp(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_CopyRects(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_GetFrontBufferRectangle(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_ProcessVertices(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_ResourceManagerDiscardBytes(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_TestCooperativeLevel(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_SetCursorPosition(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_SetCursorProperties(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TDEV8_ShowCursor(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    return tr;
}

