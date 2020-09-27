/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    d3dbvt.cpp

Description:

    Direct3D Build Verification Tests.

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
#include "util.h"

#ifdef UNDER_XBOX
#define DEMO_HACK
#endif

//******************************************************************************
// Globals
//******************************************************************************

HINSTANCE                       g_hInstance;
HWND                            g_hWnd = NULL;
static LPDIRECT3D8              g_pd3d8 = NULL;
static LPDIRECT3DDEVICE8        g_pd3dd8 = NULL;
static LPDIRECT3DTEXTURE8       g_pd3dt8 = NULL;
static LPDIRECT3DCUBETEXTURE8   g_pd3dtc8 = NULL;
static LPDIRECT3DVOLUMETEXTURE8 g_pd3dtv8 = NULL;
static LPDIRECT3DVERTEXBUFFER8  g_pd3dr8 = NULL;
static LPDIRECT3DINDEXBUFFER8   g_pd3di8 = NULL;
static LPDIRECT3DTEXTURE8       g_pd3dtSurf8 = NULL;
static LPDIRECT3DSURFACE8       g_pd3ds8 = NULL;
static LPDIRECT3DCUBETEXTURE8   g_pd3dtcSurf8 = NULL;
static LPDIRECT3DSURFACE8       g_pd3dsc8 = NULL;
static LPDIRECT3DVOLUMETEXTURE8 g_pd3dtvVol8 = NULL;
static LPDIRECT3DVOLUME8        g_pd3dv8 = NULL;
#ifndef UNDER_XBOX
static LPDIRECT3DSWAPCHAIN8     g_pd3dsw8 = NULL;
#endif // !UNDER_XBOX

GUID GUID_PRIVATEDATA = {
    0x6a2cb1ee, 0x8781, 0x11d1, {0x7c, 0xe9, 0x0, 0xaa, 0x0, 0xbd, 0xcb, 0xb0}
};

//******************************************************************************
// Function prototypes
//******************************************************************************

#ifndef UNDER_XBOX
static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif // UNDER_XBOX
static BOOL             SetViewport(DWORD dwWidth, DWORD dwHeight);

using namespace D3DBVT;

//******************************************************************************
// Test table
//******************************************************************************
static TESTTABLEENTRY g_pTestTable[] = {

    {TEXT("IDirect3D8"),                 TEXT("AddRef"),                             IDIRECT3D8,        TUNK_AddRef},
    {TEXT("IDirect3D8"),                 TEXT("Release"),                            IDIRECT3D8,        TUNK_Release},
#ifndef UNDER_XBOX
    {TEXT("IDirect3D8"),                 TEXT("QueryInterface"),                     IDIRECT3D8,        TUNK_QueryInterface},
#endif // !UNDER_XBOX
    {TEXT("IDirect3D8"),                 TEXT("GetAdapterCount"),                    0,                 TD3D8_GetAdapterCount},
    {TEXT("IDirect3D8"),                 TEXT("GetAdapterDisplayMode"),              0,                 TD3D8_GetAdapterDisplayMode},
    {TEXT("IDirect3D8"),                 TEXT("GetAdapterIdentifier"),               0,                 TD3D8_GetAdapterIdentifier},
    {TEXT("IDirect3D8"),                 TEXT("GetAdapterModeCount"),                0,                 TD3D8_GetAdapterModeCount},
#ifndef UNDER_XBOX
    {TEXT("IDirect3D8"),                 TEXT("GetAdapterMonitor"),                  0,                 TD3D8_GetAdapterMonitor},
#endif // !UNDER_XBOX
    {TEXT("IDirect3D8"),                 TEXT("GetDeviceCaps"),                      0,                 TD3D8_GetDeviceCaps},
#ifndef UNDER_XBOX
    {TEXT("IDirect3D8"),                 TEXT("RegisterSoftwareDevice"),             0,                 TD3D8_RegisterSoftwareDevice},
#endif // !UNDER_XBOX
    {TEXT("IDirect3D8"),                 TEXT("CheckDeviceFormat"),                  0,                 TD3D8_CheckDeviceFormat},
    {TEXT("IDirect3D8"),                 TEXT("CheckDeviceMultiSampleType"),         0,                 TD3D8_CheckDeviceMultiSampleType},
    {TEXT("IDirect3D8"),                 TEXT("CheckDeviceType"),                    0,                 TD3D8_CheckDeviceType},
    {TEXT("IDirect3D8"),                 TEXT("CheckDepthStencilMatch"),             0,                 TD3D8_CheckDepthStencilMatch},
    {TEXT("IDirect3D8"),                 TEXT("CreateDevice"),                       0,                 TD3D8_CreateDevice},
    {TEXT("IDirect3D8"),                 TEXT("EnumAdapterModes"),                   0,                 TD3D8_EnumAdapterModes},

    {TEXT("IDirect3DDevice8"),           TEXT("AddRef"),                             IDEVICE8,          TUNK_AddRef},
    {TEXT("IDirect3DDevice8"),           TEXT("Release"),                            IDEVICE8,          TUNK_Release},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DDevice8"),           TEXT("QueryInterface"),                     IDEVICE8,          TUNK_QueryInterface},
    {TEXT("IDirect3DDevice8"),           TEXT("CreateAdditionalSwapChain"),          0,                 TDEV8_CreateAdditionalSwapChain},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DDevice8"),           TEXT("CreateCubeTexture"),                  0,                 TDEV8_CreateCubeTexture},
    {TEXT("IDirect3DDevice8"),           TEXT("CreateDepthStencilSurface"),          0,                 TDEV8_CreateDepthStencilSurface},
    {TEXT("IDirect3DDevice8"),           TEXT("CreateImageSurface"),                 0,                 TDEV8_CreateImageSurface},
    {TEXT("IDirect3DDevice8"),           TEXT("CreateIndexBuffer"),                  0,                 TDEV8_CreateIndexBuffer},
    {TEXT("IDirect3DDevice8"),           TEXT("CreateRenderTarget"),                 0,                 TDEV8_CreateRenderTarget},
    {TEXT("IDirect3DDevice8"),           TEXT("CreateTexture"),                      0,                 TDEV8_CreateTexture},
    {TEXT("IDirect3DDevice8"),           TEXT("CreateVertexBuffer"),                 0,                 TDEV8_CreateVertexBuffer},
    {TEXT("IDirect3DDevice8"),           TEXT("CreateVolumeTexture"),                0,                 TDEV8_CreateVolumeTexture},
    {TEXT("IDirect3DDevice8"),           TEXT("ApplyStateBlock"),                    0,                 TDEV8_ApplyStateBlock},
    {TEXT("IDirect3DDevice8"),           TEXT("BeginStateBlock"),                    0,                 TDEV8_BeginStateBlock},
    {TEXT("IDirect3DDevice8"),           TEXT("EndStateBlock"),                      0,                 TDEV8_EndStateBlock},
    {TEXT("IDirect3DDevice8"),           TEXT("CreateStateBlock"),                   0,                 TDEV8_CreateStateBlock},
    {TEXT("IDirect3DDevice8"),           TEXT("DeleteStateBlock"),                   0,                 TDEV8_DeleteStateBlock},
    {TEXT("IDirect3DDevice8"),           TEXT("CaptureStateBlock"),                  0,                 TDEV8_CaptureStateBlock},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DDevice8"),           TEXT("GetClipStatus"),                      0,                 TDEV8_GetClipStatus},
    {TEXT("IDirect3DDevice8"),           TEXT("SetClipStatus"),                      0,                 TDEV8_SetClipStatus},
#endif
    {TEXT("IDirect3DDevice8"),           TEXT("GetRenderState"),                     0,                 TDEV8_GetRenderState},
    {TEXT("IDirect3DDevice8"),           TEXT("SetRenderState"),                     0,                 TDEV8_SetRenderState},
    {TEXT("IDirect3DDevice8"),           TEXT("GetRenderTarget"),                    0,                 TDEV8_GetRenderTarget},
    {TEXT("IDirect3DDevice8"),           TEXT("SetRenderTarget"),                    0,                 TDEV8_SetRenderTarget},
    {TEXT("IDirect3DDevice8"),           TEXT("GetTransform"),                       0,                 TDEV8_GetTransform},
    {TEXT("IDirect3DDevice8"),           TEXT("SetTransform"),                       0,                 TDEV8_SetTransform},
    {TEXT("IDirect3DDevice8"),           TEXT("MultiplyTransform"),                  0,                 TDEV8_MultiplyTransform},
    {TEXT("IDirect3DDevice8"),           TEXT("Clear"),                              0,                 TDEV8_Clear},
    {TEXT("IDirect3DDevice8"),           TEXT("GetViewport"),                        0,                 TDEV8_GetViewport},
    {TEXT("IDirect3DDevice8"),           TEXT("SetViewport"),                        0,                 TDEV8_SetViewport},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DDevice8"),           TEXT("GetClipPlane"),                       0,                 TDEV8_GetClipPlane},
    {TEXT("IDirect3DDevice8"),           TEXT("SetClipPlane"),                       0,                 TDEV8_SetClipPlane},
#endif
    {TEXT("IDirect3DDevice8"),           TEXT("GetLight"),                           0,                 TDEV8_GetLight},
    {TEXT("IDirect3DDevice8"),           TEXT("SetLight"),                           0,                 TDEV8_SetLight},
    {TEXT("IDirect3DDevice8"),           TEXT("LightEnable"),                        0,                 TDEV8_LightEnable},
    {TEXT("IDirect3DDevice8"),           TEXT("GetLightEnable"),                     0,                 TDEV8_GetLightEnable},
    {TEXT("IDirect3DDevice8"),           TEXT("GetMaterial"),                        0,                 TDEV8_GetMaterial},
    {TEXT("IDirect3DDevice8"),           TEXT("SetMaterial"),                        0,                 TDEV8_SetMaterial},
    {TEXT("IDirect3DDevice8"),           TEXT("GetTexture"),                         0,                 TDEV8_GetTexture},
    {TEXT("IDirect3DDevice8"),           TEXT("SetTexture"),                         0,                 TDEV8_SetTexture},
    {TEXT("IDirect3DDevice8"),           TEXT("GetTextureStageState"),               0,                 TDEV8_GetTextureStageState},
    {TEXT("IDirect3DDevice8"),           TEXT("SetTextureStageState"),               0,                 TDEV8_SetTextureStageState},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DDevice8"),           TEXT("UpdateTexture"),                      0,                 TDEV8_UpdateTexture},
    {TEXT("IDirect3DDevice8"),           TEXT("ValidateDevice"),                     0,                 TDEV8_ValidateDevice},
    {TEXT("IDirect3DDevice8"),           TEXT("GetCurrentTexturePalette"),           0,                 TDEV8_GetCurrentTexturePalette},
    {TEXT("IDirect3DDevice8"),           TEXT("SetCurrentTexturePalette"),           0,                 TDEV8_SetCurrentTexturePalette},
    {TEXT("IDirect3DDevice8"),           TEXT("GetPaletteEntries"),                  0,                 TDEV8_GetPaletteEntries},
    {TEXT("IDirect3DDevice8"),           TEXT("SetPaletteEntries"),                  0,                 TDEV8_SetPaletteEntries},
#endif
    {TEXT("IDirect3DDevice8"),           TEXT("CreateVertexShader"),                 0,                 TDEV8_CreateVertexShader},
    {TEXT("IDirect3DDevice8"),           TEXT("DeleteVertexShader"),                 0,                 TDEV8_DeleteVertexShader},
    {TEXT("IDirect3DDevice8"),           TEXT("GetVertexShader"),                    0,                 TDEV8_GetVertexShader},
    {TEXT("IDirect3DDevice8"),           TEXT("SetVertexShader"),                    0,                 TDEV8_SetVertexShader},
    {TEXT("IDirect3DDevice8"),           TEXT("GetVertexShaderConstant"),            0,                 TDEV8_GetVertexShaderConstant},
    {TEXT("IDirect3DDevice8"),           TEXT("SetVertexShaderConstant"),            0,                 TDEV8_SetVertexShaderConstant},
    {TEXT("IDirect3DDevice8"),           TEXT("GetVertexShaderDeclaration"),         0,                 TDEV8_GetVertexShaderDeclaration},
    {TEXT("IDirect3DDevice8"),           TEXT("GetVertexShaderFunction"),            0,                 TDEV8_GetVertexShaderFunction},
    {TEXT("IDirect3DDevice8"),           TEXT("CreatePixelShader"),                  0,                 TDEV8_CreatePixelShader},
    {TEXT("IDirect3DDevice8"),           TEXT("DeletePixelShader"),                  0,                 TDEV8_DeletePixelShader},
    {TEXT("IDirect3DDevice8"),           TEXT("GetPixelShader"),                     0,                 TDEV8_GetPixelShader},
    {TEXT("IDirect3DDevice8"),           TEXT("SetPixelShader"),                     0,                 TDEV8_SetPixelShader},
    {TEXT("IDirect3DDevice8"),           TEXT("GetPixelShaderConstant"),             0,                 TDEV8_GetPixelShaderConstant},
    {TEXT("IDirect3DDevice8"),           TEXT("SetPixelShaderConstant"),             0,                 TDEV8_SetPixelShaderConstant},
    {TEXT("IDirect3DDevice8"),           TEXT("GetPixelShaderFunction"),             0,                 TDEV8_GetPixelShaderFunction},
    {TEXT("IDirect3DDevice8"),           TEXT("Present"),                            0,                 TDEV8_Present},
    {TEXT("IDirect3DDevice8"),           TEXT("Reset"),                              0,                 TDEV8_Reset},
    {TEXT("IDirect3DDevice8"),           TEXT("BeginScene"),                         0,                 TDEV8_BeginScene},
    {TEXT("IDirect3DDevice8"),           TEXT("EndScene"),                           0,                 TDEV8_EndScene},
    {TEXT("IDirect3DDevice8"),           TEXT("DrawIndexedPrimitive"),               0,                 TDEV8_DrawIndexedPrimitive},
    {TEXT("IDirect3DDevice8"),           TEXT("DrawIndexedPrimitiveUP"),             0,                 TDEV8_DrawIndexedPrimitiveUP},
    {TEXT("IDirect3DDevice8"),           TEXT("DrawPrimitive"),                      0,                 TDEV8_DrawPrimitive},
    {TEXT("IDirect3DDevice8"),           TEXT("DrawPrimitiveUP"),                    0,                 TDEV8_DrawPrimitiveUP},
    {TEXT("IDirect3DDevice8"),           TEXT("DeleteSurfaceHandle"),                0,                 TDEV8_DeleteSurfaceHandle},
    {TEXT("IDirect3DDevice8"),           TEXT("DrawRectSurface"),                    0,                 TDEV8_DrawRectSurface},
    {TEXT("IDirect3DDevice8"),           TEXT("DrawTriSurface"),                     0,                 TDEV8_DrawTriSurface},
    {TEXT("IDirect3DDevice8"),           TEXT("GetStreamSource"),                    0,                 TDEV8_GetStreamSource},
    {TEXT("IDirect3DDevice8"),           TEXT("SetStreamSource"),                    0,                 TDEV8_SetStreamSource},
    {TEXT("IDirect3DDevice8"),           TEXT("GetIndices"),                         0,                 TDEV8_GetIndices},
    {TEXT("IDirect3DDevice8"),           TEXT("SetIndices"),                         0,                 TDEV8_SetIndices},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DDevice8"),           TEXT("GetAvailableTextureMem"),             0,                 TDEV8_GetAvailableTextureMem},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DDevice8"),           TEXT("GetCreationParameters"),              0,                 TDEV8_GetCreationParameters},
    {TEXT("IDirect3DDevice8"),           TEXT("GetDeviceCaps"),                      0,                 TDEV8_GetDeviceCaps},
    {TEXT("IDirect3DDevice8"),           TEXT("GetDirect3D"),                        0,                 TDEV8_GetDirect3D},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DDevice8"),           TEXT("GetInfo"),                            0,                 TDEV8_GetInfo},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DDevice8"),           TEXT("GetRasterStatus"),                    0,                 TDEV8_GetRasterStatus},
    {TEXT("IDirect3DDevice8"),           TEXT("GetBackBuffer"),                      0,                 TDEV8_GetBackBuffer},
    {TEXT("IDirect3DDevice8"),           TEXT("GetDepthStencilSurface"),             0,                 TDEV8_GetDepthStencilSurface},
    {TEXT("IDirect3DDevice8"),           TEXT("GetGammaRamp"),                       0,                 TDEV8_GetGammaRamp},
    {TEXT("IDirect3DDevice8"),           TEXT("SetGammaRamp"),                       0,                 TDEV8_SetGammaRamp},
    {TEXT("IDirect3DDevice8"),           TEXT("CopyRects"),                          0,                 TDEV8_CopyRects},
    {TEXT("IDirect3DDevice8"),           TEXT("GetFrontBufferRectangle"),            0,                 TDEV8_GetFrontBufferRectangle},
    {TEXT("IDirect3DDevice8"),           TEXT("ProcessVertices"),                    0,                 TDEV8_ProcessVertices},
    {TEXT("IDirect3DDevice8"),           TEXT("ResourceManagerDiscardBytes"),        0,                 TDEV8_ResourceManagerDiscardBytes},
    {TEXT("IDirect3DDevice8"),           TEXT("TestCooperativeLevel"),               0,                 TDEV8_TestCooperativeLevel},
    {TEXT("IDirect3DDevice8"),           TEXT("SetCursorPosition"),                  0,                 TDEV8_SetCursorPosition},
    {TEXT("IDirect3DDevice8"),           TEXT("SetCursorProperties"),                0,                 TDEV8_SetCursorProperties},
    {TEXT("IDirect3DDevice8"),           TEXT("ShowCursor"),                         0,                 TDEV8_ShowCursor},

#ifndef UNDER_XBOX
    {TEXT("IDirect3DSwapChain8"),        TEXT("AddRef"),                             ISWAPCHAIN8,       TUNK_AddRef},
    {TEXT("IDirect3DSwapChain8"),        TEXT("Release"),                            ISWAPCHAIN8,       TUNK_Release},
    {TEXT("IDirect3DSwapChain8"),        TEXT("QueryInterface"),                     ISWAPCHAIN8,       TUNK_QueryInterface},
    {TEXT("IDirect3DSwapChain8"),        TEXT("Present"),                            0,                 TSWC8_Present},
    {TEXT("IDirect3DSwapChain8"),        TEXT("GetBackBuffer"),                      0,                 TSWC8_GetBackBuffer},
#endif // !UNDER_XBOX

    {TEXT("IDirect3DTexture8"),          TEXT("AddRef"),                             ITEXTURE8,         TUNK_AddRef},
    {TEXT("IDirect3DTexture8"),          TEXT("Release"),                            ITEXTURE8,         TUNK_Release},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DTexture8"),          TEXT("QueryInterface"),                     ITEXTURE8,         TUNK_QueryInterface},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DTexture8"),          TEXT("GetType"),                            ITEXTURE8,         TRES8_GetType},
    {TEXT("IDirect3DTexture8"),          TEXT("GetDevice"),                          ITEXTURE8,         TRES8_GetDevice},
    {TEXT("IDirect3DTexture8"),          TEXT("GetPrivateData"),                     ITEXTURE8,         TRES8_GetPrivateData},
    {TEXT("IDirect3DTexture8"),          TEXT("SetPrivateData"),                     ITEXTURE8,         TRES8_SetPrivateData},
    {TEXT("IDirect3DTexture8"),          TEXT("FreePrivateData"),                    ITEXTURE8,         TRES8_FreePrivateData},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DTexture8"),          TEXT("GetPriority"),                        ITEXTURE8,         TRES8_GetPriority},
    {TEXT("IDirect3DTexture8"),          TEXT("SetPriority"),                        ITEXTURE8,         TRES8_SetPriority},
    {TEXT("IDirect3DTexture8"),          TEXT("PreLoad"),                            ITEXTURE8,         TRES8_PreLoad},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DTexture8"),          TEXT("GetLevelCount"),                      ITEXTURE8,         TBTX8_GetLevelCount},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DTexture8"),          TEXT("GetLOD"),                             ITEXTURE8,         TBTX8_GetLOD},
    {TEXT("IDirect3DTexture8"),          TEXT("SetLOD"),                             ITEXTURE8,         TBTX8_SetLOD},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DTexture8"),          TEXT("GetLevelDesc"),                       ITEXTURE8,         TTEX8_GetLevelDesc},
    {TEXT("IDirect3DTexture8"),          TEXT("LockRect"),                           ITEXTURE8,         TTEX8_LockRect},
    {TEXT("IDirect3DTexture8"),          TEXT("UnlockRect"),                         ITEXTURE8,         TTEX8_UnlockRect},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DTexture8"),          TEXT("AddDirtyRect"),                       ITEXTURE8,         TTEX8_AddDirtyRect},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DTexture8"),          TEXT("GetSurfaceLevel"),                    ITEXTURE8,         TTEX8_GetSurfaceLevel},

    {TEXT("IDirect3DCubeTexture8"),      TEXT("AddRef"),                             ICUBETEXTURE8,     TUNK_AddRef},
    {TEXT("IDirect3DCubeTexture8"),      TEXT("Release"),                            ICUBETEXTURE8,     TUNK_Release},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DCubeTexture8"),      TEXT("QueryInterface"),                     ICUBETEXTURE8,     TUNK_QueryInterface},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DCubeTexture8"),      TEXT("GetType"),                            ICUBETEXTURE8,     TRES8_GetType},
    {TEXT("IDirect3DCubeTexture8"),      TEXT("GetDevice"),                          ICUBETEXTURE8,     TRES8_GetDevice},
    {TEXT("IDirect3DCubeTexture8"),      TEXT("GetPrivateData"),                     ICUBETEXTURE8,     TRES8_GetPrivateData},
    {TEXT("IDirect3DCubeTexture8"),      TEXT("SetPrivateData"),                     ICUBETEXTURE8,     TRES8_SetPrivateData},
    {TEXT("IDirect3DCubeTexture8"),      TEXT("FreePrivateData"),                    ICUBETEXTURE8,     TRES8_FreePrivateData},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DCubeTexture8"),      TEXT("GetPriority"),                        ICUBETEXTURE8,     TRES8_GetPriority},
    {TEXT("IDirect3DCubeTexture8"),      TEXT("SetPriority"),                        ICUBETEXTURE8,     TRES8_SetPriority},
    {TEXT("IDirect3DCubeTexture8"),      TEXT("PreLoad"),                            ICUBETEXTURE8,     TRES8_PreLoad},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DCubeTexture8"),      TEXT("GetLevelCount"),                      ICUBETEXTURE8,     TBTX8_GetLevelCount},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DCubeTexture8"),      TEXT("GetLOD"),                             ICUBETEXTURE8,     TBTX8_GetLOD},
    {TEXT("IDirect3DCubeTexture8"),      TEXT("SetLOD"),                             ICUBETEXTURE8,     TBTX8_SetLOD},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DCubeTexture8"),      TEXT("GetLevelDesc"),                       ICUBETEXTURE8,     TCTX8_GetLevelDesc},
    {TEXT("IDirect3DCubeTexture8"),      TEXT("LockRect"),                           ICUBETEXTURE8,     TCTX8_LockRect},
    {TEXT("IDirect3DCubeTexture8"),      TEXT("UnlockRect"),                         ICUBETEXTURE8,     TCTX8_UnlockRect},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DCubeTexture8"),      TEXT("AddDirtyRect"),                       ICUBETEXTURE8,     TCTX8_AddDirtyRect},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DCubeTexture8"),      TEXT("GetCubeMapSurface"),                  ICUBETEXTURE8,     TCTX8_GetCubeMapSurface},

    {TEXT("IDirect3DVolumeTexture8"),    TEXT("AddRef"),                             IVOLUMETEXTURE8,   TUNK_AddRef},
    {TEXT("IDirect3DVolumeTexture8"),    TEXT("Release"),                            IVOLUMETEXTURE8,   TUNK_Release},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DVolumeTexture8"),    TEXT("QueryInterface"),                     IVOLUMETEXTURE8,   TUNK_QueryInterface},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DVolumeTexture8"),    TEXT("GetType"),                            IVOLUMETEXTURE8,   TRES8_GetType},
    {TEXT("IDirect3DVolumeTexture8"),    TEXT("GetDevice"),                          IVOLUMETEXTURE8,   TRES8_GetDevice},
    {TEXT("IDirect3DVolumeTexture8"),    TEXT("GetPrivateData"),                     IVOLUMETEXTURE8,   TRES8_GetPrivateData},
    {TEXT("IDirect3DVolumeTexture8"),    TEXT("SetPrivateData"),                     IVOLUMETEXTURE8,   TRES8_SetPrivateData},
    {TEXT("IDirect3DVolumeTexture8"),    TEXT("FreePrivateData"),                    IVOLUMETEXTURE8,   TRES8_FreePrivateData},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DVolumeTexture8"),    TEXT("GetPriority"),                        IVOLUMETEXTURE8,   TRES8_GetPriority},
    {TEXT("IDirect3DVolumeTexture8"),    TEXT("SetPriority"),                        IVOLUMETEXTURE8,   TRES8_SetPriority},
    {TEXT("IDirect3DVolumeTexture8"),    TEXT("PreLoad"),                            IVOLUMETEXTURE8,   TRES8_PreLoad},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DVolumeTexture8"),    TEXT("GetLevelCount"),                      IVOLUMETEXTURE8,   TBTX8_GetLevelCount},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DVolumeTexture8"),    TEXT("GetLOD"),                             IVOLUMETEXTURE8,   TBTX8_GetLOD},
    {TEXT("IDirect3DVolumeTexture8"),    TEXT("SetLOD"),                             IVOLUMETEXTURE8,   TBTX8_SetLOD},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DVolumeTexture8"),    TEXT("GetLevelDesc"),                       0,                 TVTX8_GetLevelDesc},
    {TEXT("IDirect3DVolumeTexture8"),    TEXT("LockBox"),                            IVOLUMETEXTURE8,   TVTX8_LockBox},
    {TEXT("IDirect3DVolumeTexture8"),    TEXT("UnlockBox"),                          IVOLUMETEXTURE8,   TVTX8_UnlockBox},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DVolumeTexture8"),    TEXT("AddDirtyBox"),                        0,                 TVTX8_AddDirtyBox},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DVolumeTexture8"),    TEXT("GetVolumeLevel"),                     0,                 TVTX8_GetVolumeLevel},

    {TEXT("IDirect3DVertexBuffer8"),     TEXT("AddRef"),                             IVERTEXBUFFER8,    TUNK_AddRef},
    {TEXT("IDirect3DVertexBuffer8"),     TEXT("Release"),                            IVERTEXBUFFER8,    TUNK_Release},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DVertexBuffer8"),     TEXT("QueryInterface"),                     IVERTEXBUFFER8,    TUNK_QueryInterface},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DVertexBuffer8"),     TEXT("GetType"),                            IVERTEXBUFFER8,    TRES8_GetType},
    {TEXT("IDirect3DVertexBuffer8"),     TEXT("GetDevice"),                          IVERTEXBUFFER8,    TRES8_GetDevice},
    {TEXT("IDirect3DVertexBuffer8"),     TEXT("GetPrivateData"),                     IVERTEXBUFFER8,    TRES8_GetPrivateData},
    {TEXT("IDirect3DVertexBuffer8"),     TEXT("SetPrivateData"),                     IVERTEXBUFFER8,    TRES8_SetPrivateData},
    {TEXT("IDirect3DVertexBuffer8"),     TEXT("FreePrivateData"),                    IVERTEXBUFFER8,    TRES8_FreePrivateData},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DVertexBuffer8"),     TEXT("GetPriority"),                        IVERTEXBUFFER8,    TRES8_GetPriority},
    {TEXT("IDirect3DVertexBuffer8"),     TEXT("SetPriority"),                        IVERTEXBUFFER8,    TRES8_SetPriority},
    {TEXT("IDirect3DVertexBuffer8"),     TEXT("PreLoad"),                            IVERTEXBUFFER8,    TRES8_PreLoad},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DVertexBuffer8"),     TEXT("GetDesc"),                            0,                 TVRB8_GetDesc},
    {TEXT("IDirect3DVertexBuffer8"),     TEXT("Lock"),                               IVERTEXBUFFER8,    TVRB8_Lock},
    {TEXT("IDirect3DVertexBuffer8"),     TEXT("Unlock"),                             IVERTEXBUFFER8,    TVRB8_Unlock},

    {TEXT("IDirect3DIndexBuffer8"),      TEXT("AddRef"),                             IINDEXBUFFER8,     TUNK_AddRef},
    {TEXT("IDirect3DIndexBuffer8"),      TEXT("Release"),                            IINDEXBUFFER8,     TUNK_Release},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DIndexBuffer8"),      TEXT("QueryInterface"),                     IINDEXBUFFER8,     TUNK_QueryInterface},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DIndexBuffer8"),      TEXT("GetType"),                            IINDEXBUFFER8,     TRES8_GetType},
    {TEXT("IDirect3DIndexBuffer8"),      TEXT("GetDevice"),                          IINDEXBUFFER8,     TRES8_GetDevice},
    {TEXT("IDirect3DIndexBuffer8"),      TEXT("GetPrivateData"),                     IINDEXBUFFER8,     TRES8_GetPrivateData},
    {TEXT("IDirect3DIndexBuffer8"),      TEXT("SetPrivateData"),                     IINDEXBUFFER8,     TRES8_SetPrivateData},
    {TEXT("IDirect3DIndexBuffer8"),      TEXT("FreePrivateData"),                    IINDEXBUFFER8,     TRES8_FreePrivateData},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DIndexBuffer8"),      TEXT("GetPriority"),                        IINDEXBUFFER8,     TRES8_GetPriority},
    {TEXT("IDirect3DIndexBuffer8"),      TEXT("SetPriority"),                        IINDEXBUFFER8,     TRES8_SetPriority},
    {TEXT("IDirect3DIndexBuffer8"),      TEXT("PreLoad"),                            IINDEXBUFFER8,     TRES8_PreLoad},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DIndexBuffer8"),      TEXT("GetDesc"),                            0,                 TIXB8_GetDesc},
    {TEXT("IDirect3DIndexBuffer8"),      TEXT("Lock"),                               IINDEXBUFFER8,     TIXB8_Lock},
    {TEXT("IDirect3DIndexBuffer8"),      TEXT("Unlock"),                             IINDEXBUFFER8,     TIXB8_Unlock},

    {TEXT("IDirect3DSurface8"),          TEXT("AddRef"),                             ISURFACE8,         TUNK_AddRef},
    {TEXT("IDirect3DSurface8"),          TEXT("Release"),                            ISURFACE8,         TUNK_Release},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DSurface8"),          TEXT("QueryInterface"),                     ISURFACE8,         TUNK_QueryInterface},
#endif // !UNDER_XBOX
    {TEXT("IDirect3DSurface8"),          TEXT("GetContainer"),                       0,                 TSUR8_GetContainer},
    {TEXT("IDirect3DSurface8"),          TEXT("GetDevice"),                          ISURFACE8,         TSUR8_GetDevice},
    {TEXT("IDirect3DSurface8"),          TEXT("GetDesc"),                            0,                 TSUR8_GetDesc},
    {TEXT("IDirect3DSurface8"),          TEXT("LockRect"),                           ISURFACE8,         TSUR8_LockRect},
    {TEXT("IDirect3DSurface8"),          TEXT("UnlockRect"),                         ISURFACE8,         TSUR8_UnlockRect},
    {TEXT("IDirect3DSurface8"),          TEXT("GetPrivateData"),                     ISURFACE8,         TSUR8_GetPrivateData},
    {TEXT("IDirect3DSurface8"),          TEXT("SetPrivateData"),                     ISURFACE8,         TSUR8_SetPrivateData},
    {TEXT("IDirect3DSurface8"),          TEXT("FreePrivateData"),                    ISURFACE8,         TSUR8_FreePrivateData},

    {TEXT("IDirect3DVolume8"),           TEXT("AddRef"),                             IVOLUME8,          TUNK_AddRef},
    {TEXT("IDirect3DVolume8"),           TEXT("Release"),                            IVOLUME8,          TUNK_Release},
#ifndef UNDER_XBOX
    {TEXT("IDirect3DVolume8"),           TEXT("QueryInterface"),                     IVOLUME8,          TUNK_QueryInterface},
#endif // !UNDER_XBOX
#ifndef DEMO_HACK
    {TEXT("IDirect3DVolume8"),           TEXT("GetContainer"),                       0,                 TVOL8_GetContainer},
#endif
    {TEXT("IDirect3DVolume8"),           TEXT("GetDevice"),                          IVOLUME8,          TVOL8_GetDevice},
    {TEXT("IDirect3DVolume8"),           TEXT("GetDesc"),                            0,                 TVOL8_GetDesc},
    {TEXT("IDirect3DVolume8"),           TEXT("LockBox"),                            IVOLUME8,          TVOL8_LockBox},
    {TEXT("IDirect3DVolume8"),           TEXT("UnlockBox"),                          IVOLUME8,          TVOL8_UnlockBox},
    {TEXT("IDirect3DVolume8"),           TEXT("GetPrivateData"),                     IVOLUME8,          TVOL8_GetPrivateData},
    {TEXT("IDirect3DVolume8"),           TEXT("SetPrivateData"),                     IVOLUME8,          TVOL8_SetPrivateData},
    {TEXT("IDirect3DVolume8"),           TEXT("FreePrivateData"),                    IVOLUME8,          TVOL8_FreePrivateData},

// ##REMOVE: Temporary video test
    {TEXT("IDirect3DDevice8"),           TEXT("Present"),                            0,                 TTMP_VideoTest},
// ##REMOVE END
};

//******************************************************************************
void ExecuteTests(HINSTANCE hInstance, HANDLE hLog) {

    TCHAR   szName[2048];
    UINT    uCount = countof(g_pTestTable);
    UINT    uPass = 0, uFail = 0, uSkip = 0, uAbort = 0;
    DWORD   dwSeed;
    UINT    i;
    TRESULT tr;

    g_hInstance = hInstance;

    // Create a heap for the tests
    if (!CreateHeap()) {
        return;
    }

    CreateLog(hLog);

    // Seed the random number generator
    dwSeed = GetTickCount();
    srand(dwSeed);

    Log(LOG_TEXT, TEXT("********************************************************************************\n"));
    Log(LOG_TEXT, TEXT("* TEST SUITE                                                                   *\n"));
    Log(LOG_TEXT, TEXT("*                                                                              *\n"));
    Log(LOG_TEXT, TEXT("* Name:              Direct3D Build Verification Tests                         *\n"));
    Log(LOG_TEXT, TEXT("* Number of tests:   %-10d                                                *\n"), uCount);
    Log(LOG_TEXT, TEXT("* Seed:              %-20d                                      *\n"), dwSeed);
    Log(LOG_TEXT, TEXT("*                                                                              *\n"));
    Log(LOG_TEXT, TEXT("********************************************************************************\n\n"));

    for (i = 0; i < uCount; i++) {

        LogComponent(TEXT("Direct3D"), g_pTestTable[i].szInterface);
        LogFunction(g_pTestTable[i].szMethod);
        wsprintf(szName, TEXT("%s::%s"), g_pTestTable[i].szInterface, g_pTestTable[i].szMethod);
        Log(LOG_TEXT, TEXT("\n********************************************************************************\n"));
        Log(LOG_TEXT, TEXT("* TEST STARTING                                                                *\n"));
        Log(LOG_TEXT, TEXT("*                                                                              *\n"));
        Log(LOG_TEXT, TEXT("* Name:   %-68s *\n"), szName);
        Log(LOG_TEXT, TEXT("*                                                                              *\n"));
        Log(LOG_TEXT, TEXT("********************************************************************************\n"));
        LogBeginVariation(g_pTestTable[i].szMethod);
        tr = g_pTestTable[i].pfnTest(&g_pTestTable[i]);
        if (tr == TR_PASS) {
            Log(LOG_PASS, szName);
        }
        LogEndVariation();
        Log(LOG_TEXT, TEXT("********************************************************************************\n"));
        Log(LOG_TEXT, TEXT("* TEST COMPLETED                                                               *\n"));
        Log(LOG_TEXT, TEXT("*                                                                              *\n"));
        Log(LOG_TEXT, TEXT("* Name:     %-66s *\n"), szName);
        switch (tr) {
            case TR_FAIL:
                Log(LOG_TEXT, TEXT("* Result:   %-7s"), TEXT("Failed"));
                uFail++;
                break;
            case TR_ABORT:
                Log(LOG_TEXT, TEXT("* Result:   %-7s"), TEXT("Aborted"));
                uAbort++;
                break;
            case TR_SKIP:
                Log(LOG_TEXT, TEXT("* Result:   %-7s"), TEXT("Skipped"));
                uSkip++;
                break;
            case TR_PASS:
                Log(LOG_TEXT, TEXT("* Result:   %-7s"), TEXT("Passed"));
                uPass++;
                break;
        }
        Log(LOG_TEXT, TEXT("                                                            *\n"));
        Log(LOG_TEXT, TEXT("*                                                                              *\n"));
        Log(LOG_TEXT, TEXT("********************************************************************************\n"));
    }

    // Print results
    Log(LOG_TEXT, TEXT("\n\n********************************************************************************\n"));
    Log(LOG_TEXT, TEXT("* RESULT SUMMARY                                                               *\n"));
    Log(LOG_TEXT, TEXT("********************************************************************************\n"));
    Log(LOG_TEXT, TEXT("* Passed:        %10d                                                    *\n"), uPass);
    Log(LOG_TEXT, TEXT("* Failed:        %10d                                                    *\n"), uFail);
    Log(LOG_TEXT, TEXT("* Aborted:       %10d                                                    *\n"), uAbort);
    Log(LOG_TEXT, TEXT("* Skipped:       %10d                                                    *\n"), uSkip);
    Log(LOG_TEXT, TEXT("* -------------------------                                                    *\n"));
    Log(LOG_TEXT, TEXT("* Total:         %10d                                                    *\n"), uCount);
    Log(LOG_TEXT, TEXT("********************************************************************************\n\n"));

    // Release Direct3D objects
    ReleaseDeviceObjects8();
    ReleaseDevice8();
    ReleaseDirect3D8();

    ReleaseLog();

    // Release the heap
    ReleaseHeap();
}

//******************************************************************************
LPDIRECT3D8 GetDirect3D8() {

    if (g_pd3dd8) {
        ReleaseDevice8();
    }

    if (g_pd3d8) {
        return g_pd3d8;
    }

    g_pd3d8 = Direct3DCreate8(D3D_SDK_VERSION);
    if (!g_pd3d8) {
        Log(LOG_ABORT, TEXT("Direct3DCreate8 failed"));
    }

    return g_pd3d8;
}

//******************************************************************************
LPDIRECT3D8 GetDeviceDirect3D8() {

    if (g_pd3d8) {
        return g_pd3d8;
    }

    g_pd3d8 = Direct3DCreate8(D3D_SDK_VERSION);
    if (!g_pd3d8) {
        Log(LOG_ABORT, TEXT("Direct3DCreate8 failed"));
    }

    return g_pd3d8;
}

//******************************************************************************
void ReleaseDirect3D8() {
    
    if (g_pd3d8) {
        g_pd3d8->Release();
        g_pd3d8 = NULL;
    }
}

//******************************************************************************
LPDIRECT3DDEVICE8 GetDevice8() {

#ifndef UNDER_XBOX
	WNDCLASS                wc;
    MSG                     msg;
#endif
    D3DPRESENT_PARAMETERS   d3dpp;
    D3DXMATRIX              mTransform;
    UINT                    i;
    HRESULT                 hr;

    if (g_pd3dd8) {
        return g_pd3dd8;
    }

    GetDirect3D8();

#ifndef UNDER_XBOX
    
    // Register the window class
    memset(&wc, 0, sizeof(WNDCLASS));
    wc.style            = 0;
    wc.lpfnWndProc      = WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = g_hInstance;
    wc.hIcon            = NULL;
    wc.hbrBackground    = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = TEXT("Direct3D BVT");

    if (!RegisterClass(&wc)) {
        Log(LOG_ABORT, TEXT("RegisterClass failed"));
        return NULL;
    }

    // Create the main application window
    g_hWnd = CreateWindowEx(0, TEXT("Direct3D BVT"), 
                            TEXT("Direct3D BVT"), 
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0,
                            GetSystemMetrics(SM_CXSCREEN), 
                            GetSystemMetrics(SM_CYSCREEN),
                            NULL, NULL, g_hInstance, NULL);

    if (!g_hWnd) {
        Log(LOG_ABORT, TEXT("CreateWindowEx failed"));
        ReleaseDevice8();
		return NULL;
    }

    UpdateWindow(g_hWnd);

    // Pump any messages
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

#else

    g_hWnd = NULL;

#endif // UNDER_XBOX

    // Initialize the presentation parameters
    memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    d3dpp.BackBufferWidth                   = TARGET_WIDTH;
    d3dpp.BackBufferHeight                  = TARGET_HEIGHT;
#ifndef UNDER_XBOX
    d3dpp.BackBufferFormat                  = D3DFMT_R5G6B5;
#else
    d3dpp.BackBufferFormat                  = D3DFMT_LIN_R5G6B5;
#endif
    d3dpp.MultiSampleType                   = D3DMULTISAMPLE_NONE;
#ifndef UNDER_XBOX
    d3dpp.AutoDepthStencilFormat            = D3DFMT_D16;
#else
    d3dpp.AutoDepthStencilFormat            = D3DFMT_LIN_D16;
#endif
    d3dpp.FullScreen_RefreshRateInHz        = 0;
    d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.BackBufferCount                   = 1;
    d3dpp.SwapEffect                        = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                     = g_hWnd;
    d3dpp.Windowed                          = FALSE;
    d3dpp.EnableAutoDepthStencil            = TRUE;

    // Create the device
    hr = g_pd3d8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dd8);
    if (FAILED(hr)) {
        Log(LOG_ABORT, TEXT("IDirect3D8::CreateDevice failed"));
        return NULL;
    }

    // Initialize the viewport
    if (!SetViewport(640, 480)) {
        Log(LOG_WARN, TEXT("SetViewport failed"));
    }

    // Initialize the view matrix
    SetView(&mTransform, &D3DXVECTOR3(0.0f, 0.0f, -20.0f), &D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
            &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

    hr = g_pd3dd8->SetTransform(D3DTS_VIEW, &mTransform);
    if (FAILED(hr)) {
        Log(LOG_WARN, TEXT("IDirect3DDevice8::SetTransform failed"));
    }

    // Initialize the projection matrix
    SetPerspectiveProjection(&mTransform, 0.1f, 1000.0f, 
            M_PI / 4.0f, 480.0f / 640.0f);

    hr = g_pd3dd8->SetTransform(D3DTS_PROJECTION, &mTransform);
    if (FAILED(hr)) {
        Log(LOG_WARN, TEXT("IDirect3DDevice8::SetTransform failed"));
    }

    // Set ambient lighting
    hr = g_pd3dd8->SetRenderState(D3DRS_AMBIENT, 
                                      (DWORD)RGBA_MAKE(128, 128, 128, 128));
    if (FAILED(hr)) {
        Log(LOG_WARN, TEXT("IDirect3DDevice8::SetRenderState failed"));
    }

    // Set the texture filters
    for (i = 0; i < 4; i++) {
        hr = g_pd3dd8->SetTextureStageState(i, D3DTSS_MINFILTER, (DWORD)D3DTEXF_LINEAR);
        if (FAILED(hr)) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetTextureStageState"));
        }

        hr = g_pd3dd8->SetTextureStageState(i, D3DTSS_MAGFILTER, (DWORD)D3DTEXF_LINEAR);
        if (FAILED(hr)) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetTextureStageState"));
        }
    }

    // Set blend modes
    hr = g_pd3dd8->SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
    if (FAILED(hr)) {
        Log(LOG_WARN, TEXT("IDirect3DDevice8::SetRenderState failed"));
    }

    hr = g_pd3dd8->SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
    if (FAILED(hr)) {
        Log(LOG_WARN, TEXT("IDirect3DDevice8::SetRenderState failed"));
    }

    // Modulate color and alpha texture stages
    hr = g_pd3dd8->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    if (FAILED(hr)) {
        Log(LOG_WARN, TEXT("IDirect3DDevice8::SetTextureStageState"));
    }

    hr = g_pd3dd8->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    if (FAILED(hr)) {
        Log(LOG_WARN, TEXT("IDirect3DDevice8::SetTextureStageState"));
    }

    hr = g_pd3dd8->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    if (FAILED(hr)) {
        Log(LOG_WARN, TEXT("IDirect3DDevice8::SetTextureStageState"));
    }

    // Set the alpha comparison function
    hr = g_pd3dd8->SetRenderState(D3DRS_ALPHAFUNC, (DWORD)D3DCMP_GREATEREQUAL);
    if (FAILED(hr)) {
        Log(LOG_WARN, TEXT("IDirect3DDevice8::SetRenderState failed"));
    }

    // Set the alpha reference value to opaque
    hr = g_pd3dd8->SetRenderState(D3DRS_ALPHAREF, 0xFF);
    if (FAILED(hr)) {
        Log(LOG_WARN, TEXT("IDirect3DDevice8::SetRenderState failed"));
    }

    // Turn on specular highlights
    hr = g_pd3dd8->SetRenderState(D3DRS_SPECULARENABLE, (DWORD)TRUE);
    if (FAILED(hr)) {
        Log(LOG_WARN, TEXT("IDirect3DDevice8::SetRenderState failed"));
    }

    // Enable dithering
    hr = g_pd3dd8->SetRenderState(D3DRS_DITHERENABLE, (DWORD)TRUE);
    if (FAILED(hr)) {
        Log(LOG_WARN, TEXT("IDirect3DDevice8::SetRenderState failed"));
    }

    // Disable per-vertex color
    hr = g_pd3dd8->SetRenderState(D3DRS_COLORVERTEX, (DWORD)FALSE);
    if (FAILED(hr)) {
        Log(LOG_WARN, TEXT("IDirect3DDevice8::SetRenderState failed"));
    }

    return g_pd3dd8;
}

//******************************************************************************
void ReleaseDevice8() {

    if (g_pd3dd8) {
        g_pd3dd8->Release();
        g_pd3dd8 = NULL;
    }

#ifndef UNDER_XBOX

    DestroyWindow(g_hWnd);

    // Unregister the window class
    UnregisterClass(TEXT("Direct3D BVT"), g_hInstance);

#endif // UNDER_XBOX
    
    ReleaseDirect3D8();
}

//******************************************************************************
static BOOL SetViewport(DWORD dwWidth, DWORD dwHeight) {

    D3DVIEWPORT8    viewport;
    HRESULT         hr;

    // Set a viewport for the device
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Width = dwWidth;
    viewport.Height = dwHeight;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;

    hr = g_pd3dd8->SetViewport(&viewport);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetViewport"))) {
        return FALSE;
    }

    return TRUE;
}

#ifndef UNDER_XBOX
//******************************************************************************
LPDIRECT3DSWAPCHAIN8 GetSwapChain8() {

    LPDIRECT3DDEVICE8       pd3dd;
    D3DPRESENT_PARAMETERS   d3dpp;
    HRESULT                 hr;

    if (g_pd3dsw8) {
        return g_pd3dsw8;
    }

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return NULL;
    }

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
    d3dpp.Windowed                          = FALSE;
    d3dpp.EnableAutoDepthStencil            = TRUE;

    hr = pd3dd->CreateAdditionalSwapChain(&d3dpp, &g_pd3dsw8);
    ResultFailed(hr, TEXT("IDirect3DDevice8::CreateAdditionalSwapChain"));

    return g_pd3dsw8;
}
#endif // !UNDER_XBOX

//******************************************************************************
LPDIRECT3DTEXTURE8 GetTexture8() {

    LPDIRECT3DDEVICE8 pd3dd;
    HRESULT           hr;

    if (g_pd3dt8) {
        return g_pd3dt8;
    }

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return NULL;
    }

    hr = pd3dd->CreateTexture(TEXTUREDIM, TEXTUREDIM, 0, 0, D3DFMT_A1R5G5B5, D3DPOOL_MANAGED, &g_pd3dt8);
    ResultFailed(hr, TEXT("IDirect3DDevice8::CreateTexture"));

    return g_pd3dt8;
}

//******************************************************************************
LPDIRECT3DCUBETEXTURE8 GetCubeTexture8() {

    LPDIRECT3DDEVICE8 pd3dd;
    HRESULT           hr;

    if (g_pd3dtc8) {
        return g_pd3dtc8;
    }

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return NULL;
    }

    hr = pd3dd->CreateCubeTexture(TEXTUREDIM, 0, 0, D3DFMT_A1R5G5B5, D3DPOOL_MANAGED, &g_pd3dtc8);
    ResultFailed(hr, TEXT("IDirect3DDevice8::CreateCubeTexture"));

    return g_pd3dtc8;
}

//******************************************************************************
LPDIRECT3DVOLUMETEXTURE8 GetVolumeTexture8() {

    LPDIRECT3DDEVICE8 pd3dd;
    HRESULT           hr;

    if (g_pd3dtv8) {
        return g_pd3dtv8;
    }

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return NULL;
    }

    hr = pd3dd->CreateVolumeTexture(TEXTUREDIM, TEXTUREDIM, TEXTUREDIM, 0, 0, D3DFMT_A1R5G5B5, D3DPOOL_MANAGED, &g_pd3dtv8);
    ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVolumeTexture"));

    return g_pd3dtv8;
}

//******************************************************************************
LPDIRECT3DVERTEXBUFFER8 GetVertexBuffer8() {

    LPDIRECT3DDEVICE8 pd3dd;
    HRESULT           hr;

    if (g_pd3dr8) {
        return g_pd3dr8;
    }

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return NULL;
    }

    hr = pd3dd->CreateVertexBuffer(VERTBUFFSIZE, 0, VERTBUFFFORMAT, D3DPOOL_MANAGED, &g_pd3dr8);
    ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexBuffer"));

    return g_pd3dr8;
}

//******************************************************************************
LPDIRECT3DINDEXBUFFER8 GetIndexBuffer8() {

    LPDIRECT3DDEVICE8 pd3dd;
    HRESULT           hr;

    if (g_pd3di8) {
        return g_pd3di8;
    }

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return NULL;
    }

    hr = pd3dd->CreateIndexBuffer(INDXBUFFSIZE, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &g_pd3di8);
    ResultFailed(hr, TEXT("IDirect3DDevice8::CreateIndexBuffer"));

    return g_pd3di8;
}

//******************************************************************************
LPDIRECT3DSURFACE8 GetSurface8(LPDIRECT3DTEXTURE8* ppd3dt) {

    LPDIRECT3DDEVICE8 pd3dd;
    HRESULT           hr;

    if (!g_pd3ds8) {

        if (ppd3dt) {
            *ppd3dt = NULL;
        }

        pd3dd = GetDevice8();
        if (!pd3dd) {
            return NULL;
        }

        hr = pd3dd->CreateTexture(TEXTUREDIM, TEXTUREDIM, 0, 0, D3DFMT_A1R5G5B5, D3DPOOL_MANAGED, &g_pd3dtSurf8);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateTexture"))) {
            return NULL;
        }

        hr = g_pd3dtSurf8->GetSurfaceLevel(0, &g_pd3ds8);
        if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
            g_pd3dtSurf8->Release();
            g_pd3dtSurf8 = NULL;
        }
    }

    if (ppd3dt) {
        *ppd3dt = g_pd3dtSurf8;
    }

    return g_pd3ds8;
}

//******************************************************************************
LPDIRECT3DSURFACE8 GetCubeSurface8(LPDIRECT3DCUBETEXTURE8* ppd3dtc) {

    LPDIRECT3DDEVICE8 pd3dd;
    HRESULT           hr;

    if (!g_pd3dsc8) {

        if (ppd3dtc) {
            *ppd3dtc = NULL;
        }

        pd3dd = GetDevice8();
        if (!pd3dd) {
            return NULL;
        }

        hr = pd3dd->CreateCubeTexture(TEXTUREDIM, 0, 0, D3DFMT_A1R5G5B5, D3DPOOL_MANAGED, &g_pd3dtcSurf8);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateCubeTexture"))) {
            return NULL;
        }

        hr = g_pd3dtcSurf8->GetCubeMapSurface(D3DCUBEMAP_FACE_POSITIVE_X, 0, &g_pd3dsc8);
        if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
            g_pd3dtcSurf8->Release();
            g_pd3dtcSurf8 = NULL;
        }
    }

    if (ppd3dtc) {
        *ppd3dtc = g_pd3dtcSurf8;
    }

    return g_pd3dsc8;
}

//******************************************************************************
LPDIRECT3DVOLUME8 GetVolume8(LPDIRECT3DVOLUMETEXTURE8* ppd3dtv) {

    LPDIRECT3DDEVICE8 pd3dd;
    HRESULT           hr;

    if (!g_pd3dv8) {

        if (ppd3dtv) {
            *ppd3dtv = NULL;
        }

        pd3dd = GetDevice8();
        if (!pd3dd) {
            return NULL;
        }

        hr = pd3dd->CreateVolumeTexture(TEXTUREDIM, TEXTUREDIM, TEXTUREDIM, 0, 0, D3DFMT_A1R5G5B5, D3DPOOL_MANAGED, &g_pd3dtvVol8);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVolumeTexture"))) {
            return NULL;
        }

        hr = g_pd3dtvVol8->GetVolumeLevel(0, &g_pd3dv8);
        if (ResultFailed(hr, TEXT("IDirect3DVolumeTexture8::GetVolumeLevel"))) {
            g_pd3dtvVol8->Release();
            g_pd3dtvVol8 = NULL;
        }
    }

    if (ppd3dtv) {
        *ppd3dtv = g_pd3dtvVol8;
    }

    return g_pd3dv8;
}

//******************************************************************************
void ReleaseDeviceObjects8() {

#ifndef UNDER_XBOX
    if (g_pd3dsw8) {
        g_pd3dsw8->Release();
        g_pd3dsw8 = NULL;
    }
#endif // !UNDER_XBOX
    if (g_pd3dt8) {
        g_pd3dt8->Release();
        g_pd3dt8 = NULL;
    }
    if (g_pd3dtc8) {
        g_pd3dtc8->Release();
        g_pd3dtc8 = NULL;
    }
    if (g_pd3dtv8) {
        g_pd3dtv8->Release();
        g_pd3dtv8 = NULL;
    }
    if (g_pd3dr8) {
        g_pd3dr8->Release();
        g_pd3dr8 = NULL;
    }
    if (g_pd3di8) {
        g_pd3di8->Release();
        g_pd3di8 = NULL;
    }
    if (g_pd3dtSurf8) {
        g_pd3dtSurf8->Release();
        g_pd3dtSurf8 = NULL;
    }
    if (g_pd3ds8) {
        g_pd3ds8->Release();
        g_pd3ds8 = NULL;
    }
    if (g_pd3dtcSurf8) {
        g_pd3dtcSurf8->Release();
        g_pd3dtcSurf8 = NULL;
    }
    if (g_pd3dsc8) {
        g_pd3dsc8->Release();
        g_pd3dsc8 = NULL;
    }
    if (g_pd3dtvVol8) {
        g_pd3dtvVol8->Release();
        g_pd3dtvVol8 = NULL;
    }
    if (g_pd3dv8) {
        g_pd3dv8->Release();
        g_pd3dv8 = NULL;
    }
}

//******************************************************************************
// ##REMOVE: Temporary video test
#define NUM_ITERATIONS 256

typedef struct _COMPLEXNUM {
    float a;
    float b;
} COMPLEXNUM, *PCOMPLEXNUM;

D3DXVECTOR3 CubicInterpolate(D3DXVECTOR3* pv, float f) {

    D3DXVECTOR3 a, b, c;
    float fSq = f * f;

    a = (*(pv + 3) - *(pv + 2)) - (*pv - *(pv + 1));
    b = (*pv - *(pv + 1)) - a;
    c = *(pv + 2) - *pv;

    return a * (fSq * f) + b * fSq + c * f + *(pv + 1);
}

TESTPROCAPI TTMP_VideoTest(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    LPDIRECT3DSURFACE8  pdds;
    D3DLOCKED_RECT      d3dlr;
    LPWORD              pwPixel;
    PCOMPLEXNUM         pcnFractal, pcnZ;
    float               a, b;
    UINT                zooms, iterations;
    COMPLEXNUM          cnBase;
    float               fDelta, fA, fB;
    WORD                pwPalette[NUM_ITERATIONS];
    UINT                i, j;
    D3DXVECTOR3         vColor;
    D3DXVECTOR3         pvControl[7];
    UINT                puControl[5];
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    srand(GetTickCount());

    ReleaseDevice8();

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_FAIL;
    }

//    puControl[0] = 0;
//    puControl[2] = NUM_ITERATIONS / 2;
//    puControl[1] = puControl[2] / 2;
//    puControl[3] = puControl[1] + puControl[2];
//    puControl[4] = NUM_ITERATIONS;

    puControl[0] = 0;
    puControl[3] = NUM_ITERATIONS / 2;
    puControl[2] = puControl[3] / 2;
    puControl[1] = puControl[2] / 2;
    puControl[4] = NUM_ITERATIONS;

//    for (i = 0; i < NUM_ITERATIONS; i++) {
//        pwPalette[i] = ((BYTE)((float)0 / (float)(NUM_ITERATIONS-1) * 255.0f) & 0xF8) << 8 |
//                       ((BYTE)((float)i / (float)(NUM_ITERATIONS-1) * 255.0f) & 0xFC) << 5 |
//                       ((BYTE)((float)255 / (float)(NUM_ITERATIONS-1) * 255.0f) & 0xF8) >> 3;
//    }

    pcnFractal = (PCOMPLEXNUM)MemAlloc32(TARGET_WIDTH * TARGET_HEIGHT * sizeof(COMPLEXNUM));
    if (!pcnFractal) {
        return TR_ABORT;
    }
    memset(pcnFractal, 0, TARGET_WIDTH * TARGET_HEIGHT * sizeof(COMPLEXNUM));

    hr = pd3dd->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pdds);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetBackBuffer"))) {
        MemFree32(pcnFractal);
        return TR_FAIL;
    }

    cnBase.a = -1.75f;
    cnBase.b = 0.9375f;
    fDelta = 0.00390625f;

//    cnBase.a = -1.0f;
//    cnBase.b = 0.4f;
//    fDelta = 0.0005f;

//    cnBase.a = -1.1f;
//    cnBase.b = 0.38f;
//    fDelta = 0.00025f;

#ifndef UNDER_XBOX
    for (zooms = 0; zooms < 8; zooms++) {
#else
    for (zooms = 0; zooms < 2; zooms++) {
#endif // UNDER_XBOX

        for (i = 0; i < 7; i++) {
            pvControl[i] = D3DXVECTOR3(RND(), RND(), RND());;
        }

        for (i = 0; i < 4; i++) {
            for (j = puControl[i]; j < puControl[i+1]; j++) {
                vColor = CubicInterpolate(&pvControl[i], (float)(j - puControl[i]) / (float)(puControl[i+1] - puControl[i]));
                pwPalette[j] = ((BYTE)(vColor.x * 255.0f) & 0xF8) << 8 |
                               ((BYTE)(vColor.y * 255.0f) & 0xFC) << 5 |
                               ((BYTE)(vColor.z * 255.0f) & 0xF8) >> 3;
            }
        }

        for (iterations = 0; iterations < NUM_ITERATIONS; iterations++) {

#ifndef UNDER_XBOX
            hr = pdds->LockRect(&d3dlr, NULL, 0);
#else
            hr = pdds->LockRect(&d3dlr, NULL, D3DLOCK_TILED);
#endif // UNDER_XBOX
            if (ResultFailed(hr, TEXT("IDirect3DSurface8::LockRect"))) {
                pdds->Release();
                MemFree32(pcnFractal);
                return TR_FAIL;
            }

            pwPixel = (LPWORD)d3dlr.pBits;
            pcnZ = pcnFractal;

            for (i = 0, fB = cnBase.b; i < TARGET_HEIGHT; i++, fB -= fDelta) {

                for (j = 0, fA = cnBase.a; j < TARGET_WIDTH; j++, fA += fDelta) {

                    if (pcnZ[j].a == FLT_MAX) {
                        pwPixel[j] = pwPalette[(UINT)pcnZ[j].b];
                    }
                    else {

                        a = pcnZ[j].a;
                        b = pcnZ[j].b;
                        pcnZ[j].a = (a * a - b * b) + fA;
                        pcnZ[j].b = (2.0f * a * b) + fB;
                        if (pcnZ[j].a * pcnZ[j].a + pcnZ[j].b * pcnZ[j].b > 4.0f) {
                            pcnZ[j].a = FLT_MAX;
                            pcnZ[j].b = (float)iterations;
                            pwPixel[j] = pwPalette[iterations];
                        }
                        else {
                            pwPixel[j] = 0;
                        }
                    }
                }

                pwPixel += d3dlr.Pitch / 2;
                pcnZ += TARGET_WIDTH;
            }
    
            hr = pdds->UnlockRect();
            if (ResultFailed(hr, TEXT("IDirect3DSurface8::UnlockRect"))) {
                pdds->Release();
                MemFree32(pcnFractal);
                return TR_FAIL;
            }

            hr = pd3dd->Present(NULL, NULL, NULL, NULL);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::Present"))) {
                pdds->Release();
                MemFree32(pcnFractal);
                return TR_FAIL;
            }
        }

        for (iterations = 0; iterations < 51200; iterations++) {
            i = rand() % TARGET_HEIGHT;
            j = rand() % TARGET_WIDTH;
            pcnZ = &pcnFractal[i * TARGET_WIDTH + j];
            fB = 1.0f + 1.0f * (float)(iterations / 100);
            fA = (float)NUM_ITERATIONS / 2.0f - fB;
            fB = fA + fB + fB;
            if (pcnZ->a == FLT_MAX && pcnZ->b > fA && pcnZ->b < fB) {
                cnBase.a += (float)j * fDelta;
                cnBase.b -= (float)i * fDelta;
                break;
            }
        }
        if (iterations == 51200) {
            zooms = (UINT)-2;
        }

        fDelta /= 4.0f;
        cnBase.a -= (160.0f * fDelta);
        cnBase.b += (120.0f * fDelta);

        memset(pcnFractal, 0, TARGET_WIDTH * TARGET_HEIGHT * sizeof(COMPLEXNUM));
    }

    MemFree32(pcnFractal);
    pdds->Release();

    return tr;
}
/*
TESTPROCAPI TTMP_VideoTest(PTESTTABLEENTRY ptte) {

    LPDIRECT3DDEVICE8   pd3dd;
    LPDIRECT3DSURFACE8  pdds;
    D3DLOCKED_RECT      d3dlr;
    LPWORD              pwPixel;
    UINT                i, j;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_FAIL;
    }

    hr = pd3dd->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pdds);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetBackBuffer"))) {
        return TR_FAIL;
    }

    hr = pdds->LockRect(&d3dlr, NULL, 0);
    if (ResultFailed(hr, TEXT("IDirect3DSurface8::LockRect"))) {
        return TR_FAIL;
    }

    pwPixel = (LPWORD)d3dlr.pBits;

    for (i = 0; i < TARGET_HEIGHT; i++) {

        for (j = 0; j < TARGET_WIDTH; j++) {

            pwPixel[j] = 0xFFFF;
        }

        pwPixel += d3dlr.Pitch / 2;
    }
    
    hr = pdds->UnlockRect();
    if (ResultFailed(hr, TEXT("IDirect3DSurface8::UnlockRect"))) {
        return TR_FAIL;
    }

    hr = pd3dd->Present(NULL, NULL, NULL, NULL);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::Present"))) {
        return TR_FAIL;
    }

    pdds->Release();

Sleep(1000);
    return tr;
}
*/
// ##REMOVE END

//******************************************************************************
// Shared tests
//******************************************************************************

//******************************************************************************
TESTPROCAPI TSHR8_GetDevice(PTESTTABLEENTRY ptte, LPVOID pvObject) {

    LPDIRECT3DDEVICE8   pd3dd, pd3ddR;
    ULONG               uRef1, uRef2;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

    pd3dd->AddRef();
    uRef1 = pd3dd->Release();

    switch (ptte->dwData) {
        case ISURFACE8:
            hr = ((LPDIRECT3DSURFACE8)pvObject)->GetDevice(&pd3ddR);
            break;
        case IVOLUME8:
            hr = ((LPDIRECT3DVOLUME8)pvObject)->GetDevice(&pd3ddR);
            break;
        default:
            hr = ((LPDIRECT3DRESOURCE8)pvObject)->GetDevice(&pd3ddR);
            break;
    }

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
TESTPROCAPI TSHR8_GetPrivateData(PTESTTABLEENTRY ptte, LPVOID pvObject) {

    HRESULT             hr;
    BYTE                pdataSet[2971];
    BYTE                pdataGet[2972];
    DWORD               dwSize;
    UINT                i;
    TRESULT             tr = TR_PASS;

    for (i = 0; i < 2971; i++) {
        pdataSet[i] = rand() % 256;
    }
    memcpy(pdataGet, pdataSet, 2971);

    LogBeginVariation(TEXT("%s::GetPrivateData when no data has been set"), ptte->szInterface);
    {
        dwSize = PRESET;
        switch (ptte->dwData) {
            case ISURFACE8:
                hr = ((LPDIRECT3DSURFACE8)pvObject)->GetPrivateData(GUID_PRIVATEDATA, (LPVOID)pdataGet, &dwSize);
                break;
            case IVOLUME8:
                hr = ((LPDIRECT3DVOLUME8)pvObject)->GetPrivateData(GUID_PRIVATEDATA, (LPVOID)pdataGet, &dwSize);
                break;
            default:
                hr = ((LPDIRECT3DRESOURCE8)pvObject)->GetPrivateData(GUID_PRIVATEDATA, (LPVOID)pdataGet, &dwSize);
                break;
        }

        if (hr != D3DERR_NOTFOUND) {
            Log(LOG_FAIL, TEXT("%s::GetPrivateData returned 0x%X instead of D3DERR_NOTFOUND when given an identifier to non-existant data"), ptte->szInterface);
            tr = TR_FAIL;
        }
        if (dwSize != PRESET) {
            Log(LOG_FAIL, TEXT("%s::GetPrivateData incorrectly modified the data size pointer on failure"), ptte->szInterface);
            tr = TR_FAIL;
        }
        if (memcmp(pdataGet, pdataSet, 2971)) {
            Log(LOG_FAIL, TEXT("%s::GetPrivateData incorrectly modified the data buffer on failure"), ptte->szInterface);
            tr = TR_FAIL;
        }
    }
    LogEndVariation();
    
    switch (ptte->dwData) {
        case ISURFACE8:
            hr = ((LPDIRECT3DSURFACE8)pvObject)->SetPrivateData(GUID_PRIVATEDATA, (LPVOID)pdataSet, 2971, 0);
            break;
        case IVOLUME8:
            hr = ((LPDIRECT3DVOLUME8)pvObject)->SetPrivateData(GUID_PRIVATEDATA, (LPVOID)pdataSet, 2971, 0);
            break;
        default:
            hr = ((LPDIRECT3DRESOURCE8)pvObject)->SetPrivateData(GUID_PRIVATEDATA, (LPVOID)pdataSet, 2971, 0);
            break;
    }

    if (FAILED(hr)) {
        Log(LOG_FAIL, TEXT("%s::SetPrivateData failed [0x%X]"), ptte->szInterface, hr);
        return TR_FAIL;
    }

    LogBeginVariation(TEXT("%s::GetPrivateData given too small a buffer"), ptte->szInterface);
    {
        memset(pdataGet, 0, 2971);
        dwSize = 2970;
        switch (ptte->dwData) {
            case ISURFACE8:
                hr = ((LPDIRECT3DSURFACE8)pvObject)->GetPrivateData(GUID_PRIVATEDATA, (LPVOID)pdataGet, &dwSize);
                break;
            case IVOLUME8:
                hr = ((LPDIRECT3DVOLUME8)pvObject)->GetPrivateData(GUID_PRIVATEDATA, (LPVOID)pdataGet, &dwSize);
                break;
            default:
                hr = ((LPDIRECT3DRESOURCE8)pvObject)->GetPrivateData(GUID_PRIVATEDATA, (LPVOID)pdataGet, &dwSize);
                break;
        }

        if (hr != D3DERR_MOREDATA) {
            Log(LOG_FAIL, TEXT("%s::GetPrivateData returned 0x%X instead of D3DERR_MOREDATA when given too small a buffer"), ptte->szInterface);
            tr = TR_FAIL;
        }
        if (dwSize != 2971) {
            Log(LOG_FAIL, TEXT("%s::GetPrivateData set the wrong size for the data buffer when given too small a buffer"), ptte->szInterface);
            tr = TR_FAIL;
        }
        for (i = 0; i < 2971; i++) {
            if (pdataGet[i]) {
                Log(LOG_FAIL, TEXT("%s::GetPrivateData incorrectly modified the data buffer on failure from too small a buffer"), ptte->szInterface);
                tr = TR_FAIL;
                break;
            }
        }
    }
    LogEndVariation();

    LogBeginVariation(TEXT("%s::GetPrivateData given a correct buffer size"), ptte->szInterface);
    {
        memset(pdataGet, 0, 2971);
        pdataGet[2971] = 0x4C;
        dwSize = 3000;
        switch (ptte->dwData) {
            case ISURFACE8:
                hr = ((LPDIRECT3DSURFACE8)pvObject)->GetPrivateData(GUID_PRIVATEDATA, (LPVOID)pdataGet, &dwSize);
                break;
            case IVOLUME8:
                hr = ((LPDIRECT3DVOLUME8)pvObject)->GetPrivateData(GUID_PRIVATEDATA, (LPVOID)pdataGet, &dwSize);
                break;
            default:
                hr = ((LPDIRECT3DRESOURCE8)pvObject)->GetPrivateData(GUID_PRIVATEDATA, (LPVOID)pdataGet, &dwSize);
                break;
        }

        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("%s::GetPrivateData failed [0x%X]"), ptte->szInterface, hr);
            return TR_FAIL;
        }
#ifndef UNDER_XBOX
        if (dwSize != 3000) {
            Log(LOG_FAIL, TEXT("%s::GetPrivateData incorrectly modified the size for the data buffer on success"), ptte->szInterface);
            tr = TR_FAIL;
        }
#endif // !UNDER_XBOX
        if (memcmp(pdataGet, pdataSet, 2971)) {
            Log(LOG_FAIL, TEXT("%s::GetPrivateData returned incorrect data on success"), ptte->szInterface);
            tr = TR_FAIL;
        }
        if (pdataGet[2971] != 0x4C) {
            Log(LOG_FAIL, TEXT("%s::GetPrivate data incorrectly modified data past the end of the buffer on success"), ptte->szInterface);
            tr = TR_FAIL;
        }
    }
    LogEndVariation();

    switch (ptte->dwData) {
        case ISURFACE8:
            hr = ((LPDIRECT3DSURFACE8)pvObject)->FreePrivateData(GUID_PRIVATEDATA);
            break;
        case IVOLUME8:
            hr = ((LPDIRECT3DVOLUME8)pvObject)->FreePrivateData(GUID_PRIVATEDATA);
            break;
        default:
            hr = ((LPDIRECT3DRESOURCE8)pvObject)->FreePrivateData(GUID_PRIVATEDATA);
            break;
    }

    if (FAILED(hr)) {
        Log(LOG_FAIL, TEXT("%s::FreePrivateData failed [0x%X]"), ptte->szInterface, hr);
        return TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TSHR8_SetPrivateData(PTESTTABLEENTRY ptte, LPVOID pvObject) {

    HRESULT             hr;
    LPDIRECT3DDEVICE8   pd3dd;
    IUnknown*           punk;
    ULONG               uRef1, uRef2;
    DWORD               dwSize;
    TRESULT             tr = TR_PASS;

    pd3dd = GetDevice8();
    if (!pd3dd) {
        return TR_ABORT;
    }

#ifndef UNDER_XBOX

    pd3dd->AddRef();
    uRef1 = pd3dd->Release();

    LogBeginVariation(TEXT("%s::SetPrivateData on an interface"), ptte->szInterface);
    {
        switch (ptte->dwData) {
            case ISURFACE8:
                hr = ((LPDIRECT3DSURFACE8)pvObject)->SetPrivateData(GUID_PRIVATEDATA, (LPVOID)&pd3dd, sizeof(IUnknown*), D3DSPD_IUNKNOWN);
                break;
            case IVOLUME8:
                hr = ((LPDIRECT3DVOLUME8)pvObject)->SetPrivateData(GUID_PRIVATEDATA, (LPVOID)&pd3dd, sizeof(IUnknown*), D3DSPD_IUNKNOWN);
                break;
            default:
                hr = ((LPDIRECT3DRESOURCE8)pvObject)->SetPrivateData(GUID_PRIVATEDATA, (LPVOID)&pd3dd, sizeof(IUnknown*), D3DSPD_IUNKNOWN);
                break;
        }

        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("%s::SetPrivateData failed on D3DSPD_IUNKNOWN [0x%X]"), ptte->szInterface, hr);
            return TR_FAIL;
        }

        pd3dd->AddRef();
        uRef2 = pd3dd->Release();

        if (uRef2 != uRef1 + 1) {
            Log(LOG_FAIL, TEXT("%s::SetPrivateData failed to increment the reference count on the given object"), ptte->szInterface);
            tr = TR_FAIL;
        }

        dwSize = sizeof(IUnknown*);
        switch (ptte->dwData) {
            case ISURFACE8:
                hr = ((LPDIRECT3DSURFACE8)pvObject)->GetPrivateData(GUID_PRIVATEDATA, (LPVOID)&punk, &dwSize);
                break;
            case IVOLUME8:
                hr = ((LPDIRECT3DVOLUME8)pvObject)->GetPrivateData(GUID_PRIVATEDATA, (LPVOID)&punk, &dwSize);
                break;
            default:
                hr = ((LPDIRECT3DRESOURCE8)pvObject)->GetPrivateData(GUID_PRIVATEDATA, (LPVOID)&punk, &dwSize);
                break;
        }

        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("%s::GetPrivateData failed [0x%X]"), ptte->szInterface, hr);
            return TR_FAIL;
        }

        if ((LPVOID)punk != (LPVOID)pd3dd) {
            Log(LOG_FAIL, TEXT("%s::GetPrivateData returned an incorrect pointer for an interface set with D3DSPD_IUNKNOWN"), ptte->szInterface);
            tr = TR_FAIL;
        }

        switch (ptte->dwData) {
            case ISURFACE8:
                hr = ((LPDIRECT3DSURFACE8)pvObject)->FreePrivateData(GUID_PRIVATEDATA);
                break;
            case IVOLUME8:
                hr = ((LPDIRECT3DVOLUME8)pvObject)->FreePrivateData(GUID_PRIVATEDATA);
                break;
            default:
                hr = ((LPDIRECT3DRESOURCE8)pvObject)->FreePrivateData(GUID_PRIVATEDATA);
                break;
        }

        pd3dd->AddRef();
        uRef2 = pd3dd->Release();

        if (uRef1 != uRef2) {
            Log(LOG_FAIL, TEXT("%s::FreePrivateData failed to decrement the reference count on the interface whose pointer was set"), ptte->szInterface);
            tr = TR_FAIL;
        }
    }
    LogEndVariation();

    LogBeginVariation(TEXT("%s::SetPrivateData to replace an interface pointer with data"), ptte->szInterface);
    {
        switch (ptte->dwData) {
            case ISURFACE8:
                hr = ((LPDIRECT3DSURFACE8)pvObject)->SetPrivateData(GUID_PRIVATEDATA, (LPVOID)&pd3dd, sizeof(IUnknown*), D3DSPD_IUNKNOWN);
                break;
            case IVOLUME8:
                hr = ((LPDIRECT3DVOLUME8)pvObject)->SetPrivateData(GUID_PRIVATEDATA, (LPVOID)&pd3dd, sizeof(IUnknown*), D3DSPD_IUNKNOWN);
                break;
            default:
                hr = ((LPDIRECT3DRESOURCE8)pvObject)->SetPrivateData(GUID_PRIVATEDATA, (LPVOID)&pd3dd, sizeof(IUnknown*), D3DSPD_IUNKNOWN);
                break;
        }

        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("%s::SetPrivateData failed on D3DSPD_IUNKNOWN [0x%X]"), ptte->szInterface, hr);
            return TR_FAIL;
        }

        switch (ptte->dwData) {
            case ISURFACE8:
                hr = ((LPDIRECT3DSURFACE8)pvObject)->SetPrivateData(GUID_PRIVATEDATA, (LPVOID)&pd3dd, sizeof(IUnknown*), 0);
                break;
            case IVOLUME8:
                hr = ((LPDIRECT3DVOLUME8)pvObject)->SetPrivateData(GUID_PRIVATEDATA, (LPVOID)&pd3dd, sizeof(IUnknown*), 0);
                break;
            default:
                hr = ((LPDIRECT3DRESOURCE8)pvObject)->SetPrivateData(GUID_PRIVATEDATA, (LPVOID)&pd3dd, sizeof(IUnknown*), 0);
                break;
        }

        pd3dd->AddRef();
        uRef2 = pd3dd->Release();

        if (uRef1 != uRef2) {
            Log(LOG_FAIL, TEXT("%s::SetPrivateData failed to decrement the reference count on the interface whose pointer was replaced"), ptte->szInterface);
            tr = TR_FAIL;
        }
    }
    LogEndVariation();

    switch (ptte->dwData) {
        case ISURFACE8:
            hr = ((LPDIRECT3DSURFACE8)pvObject)->FreePrivateData(GUID_PRIVATEDATA);
            break;
        case IVOLUME8:
            hr = ((LPDIRECT3DVOLUME8)pvObject)->FreePrivateData(GUID_PRIVATEDATA);
            break;
        default:
            hr = ((LPDIRECT3DRESOURCE8)pvObject)->FreePrivateData(GUID_PRIVATEDATA);
            break;
    }

    if (FAILED(hr)) {
        Log(LOG_FAIL, TEXT("%s::FreePrivateData failed [0x%X]"), ptte->szInterface, hr);
        return TR_FAIL;
    }

#endif // !UNDER_XBOX
    
    return tr;
}

//******************************************************************************
TESTPROCAPI TSHR8_FreePrivateData(PTESTTABLEENTRY ptte, LPVOID pvObject) {

    HRESULT             hr;
    BYTE                pdataSet[2971];
    TRESULT             tr = TR_PASS;

    LogBeginVariation(TEXT("%s::FreePrivateData using a reference with no data set"), ptte->szInterface);
    {
        switch (ptte->dwData) {
            case ISURFACE8:
                hr = ((LPDIRECT3DSURFACE8)pvObject)->FreePrivateData(GUID_PRIVATEDATA);
                break;
            case IVOLUME8:
                hr = ((LPDIRECT3DVOLUME8)pvObject)->FreePrivateData(GUID_PRIVATEDATA);
                break;
            default:
                hr = ((LPDIRECT3DRESOURCE8)pvObject)->FreePrivateData(GUID_PRIVATEDATA);
                break;
        }

#ifndef UNDER_XBOX
        if (hr != D3DERR_NOTFOUND) {
            Log(LOG_FAIL, TEXT("%s::FreePrivateData returned 0x%X instead of D3DERR_NOTFOUND using a reference with no data set"), ptte->szInterface, hr);
            tr = TR_FAIL;
        }
#endif
    }
    LogEndVariation();

    switch (ptte->dwData) {
        case ISURFACE8:
            hr = ((LPDIRECT3DSURFACE8)pvObject)->SetPrivateData(GUID_PRIVATEDATA, (LPVOID)pdataSet, 2971, 0);
            break;
        case IVOLUME8:
            hr = ((LPDIRECT3DVOLUME8)pvObject)->SetPrivateData(GUID_PRIVATEDATA, (LPVOID)pdataSet, 2971, 0);
            break;
        default:
            hr = ((LPDIRECT3DRESOURCE8)pvObject)->SetPrivateData(GUID_PRIVATEDATA, (LPVOID)pdataSet, 2971, 0);
            break;
    }

    if (FAILED(hr)) {
        Log(LOG_FAIL, TEXT("%s::SetPrivateData failed [0x%X]"), ptte->szInterface, hr);
        return TR_FAIL;
    }

    LogBeginVariation(TEXT("%s::FreePrivateData on a block of data"), ptte->szInterface);
    {
        switch (ptte->dwData) {
            case ISURFACE8:
                hr = ((LPDIRECT3DSURFACE8)pvObject)->FreePrivateData(GUID_PRIVATEDATA);
                break;
            case IVOLUME8:
                hr = ((LPDIRECT3DVOLUME8)pvObject)->FreePrivateData(GUID_PRIVATEDATA);
                break;
            default:
                hr = ((LPDIRECT3DRESOURCE8)pvObject)->FreePrivateData(GUID_PRIVATEDATA);
                break;
        }

        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("%s::FreePrivateData failed [0x%X]"), ptte->szInterface, hr);
            return TR_FAIL;
        }
    }
    LogEndVariation();

    return tr;
}

//******************************************************************************
TESTPROCAPI TSHR8_LockRect(PTESTTABLEENTRY ptte, LPVOID pvObject) {

    D3DLOCKED_RECT      d3dlr;
    RECT                rect, *prect[2];
    HRESULT             hr;
    UINT                i, j;
    BYTE                byte;
    D3DCUBEMAP_FACES    d3dcf;
    TRESULT             tr = TR_PASS;
    DWORD               dwFlags[] = {
                            0,
#ifndef UNDER_XBOX
                            D3DLOCK_DISCARD, 
                            D3DLOCK_NO_DIRTY_UPDATE, 
                            D3DLOCK_NOSYSLOCK, 
#endif
                            D3DLOCK_READONLY, 
                        };
    LPCTSTR             szFlags[] = {
                            TEXT("no"),
#ifndef UNDER_XBOX
                            TEXT("D3DLOCK_DISCARD"), 
                            TEXT("D3DLOCK_NO_DIRTY_UPDATE"), 
                            TEXT("D3DLOCK_NOSYSLOCK"), 
#endif
                            TEXT("D3DLOCK_READONLY"), 
                        };

    rect.left = 1;
    rect.top = 1;
    rect.right = 10;
    rect.bottom = 10;
    prect[0] = NULL;
    prect[1] = &rect;

    for (i = 0; i < 2; i++) {

        for (j = 0; j < countof(dwFlags); j++) {

            LogBeginVariation(TEXT("%s::LockRect using a %srect and %s flag"), ptte->szInterface, i ? TEXT("sub") : TEXT("NULL "), szFlags[j]);
            {
                d3dlr.Pitch = 0;
                d3dlr.pBits = NULL;

                switch (ptte->dwData) {

                    case ISURFACE8:
                        hr = ((LPDIRECT3DSURFACE8)pvObject)->LockRect(&d3dlr, prect[i], dwFlags[j]);
                        break;

                    case ITEXTURE8:
                        hr = ((LPDIRECT3DTEXTURE8)pvObject)->LockRect(0, &d3dlr, prect[i], dwFlags[j]);
                        break;

                    case ICUBETEXTURE8:
                        d3dcf = (D3DCUBEMAP_FACES)(rand() % 6);
                        hr = ((LPDIRECT3DCUBETEXTURE8)pvObject)->LockRect(d3dcf, 0, &d3dlr, prect[i], dwFlags[j]);
                        break;

                    default:
                        LogEndVariation();
                        return TR_ABORT;
                }

                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::LockRect failed [0x%X]"), ptte->szInterface, hr);
                    LogEndVariation();
                    return TR_FAIL;
                }

                if (d3dlr.Pitch < 2 * TEXTUREDIM) {
                    Log(LOG_FAIL, TEXT("%s::LockRect returned an incorrect pitch of %d for the locked rect"), ptte->szInterface, d3dlr.Pitch);
                    tr = TR_FAIL;
                }

                if (!d3dlr.pBits) {
                    Log(LOG_FAIL, TEXT("%s::LockRect returned a NULL pointer to resource memory"), ptte->szInterface);
                    tr = TR_FAIL;
                }
                else {

                    // Reference the memory
                    byte = *((LPBYTE)d3dlr.pBits);
                    if (j != 3) {
                        *((LPBYTE)d3dlr.pBits) = byte;
                    }
                }

                switch (ptte->dwData) {

                    case ISURFACE8:
                        hr = ((LPDIRECT3DSURFACE8)pvObject)->UnlockRect();
                        break;

                    case ITEXTURE8:
                        hr = ((LPDIRECT3DTEXTURE8)pvObject)->UnlockRect(0);
                        break;

                    case ICUBETEXTURE8:
                        hr = ((LPDIRECT3DCUBETEXTURE8)pvObject)->UnlockRect(d3dcf, 0);
                        break;

                    default:
                        LogEndVariation();
                        return TR_ABORT;
                }

                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::UnlockRect failed [0x%X]"), ptte->szInterface, hr);
                    LogEndVariation();
                    return TR_FAIL;
                }
            }

            LogEndVariation();
        }
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TSHR8_UnlockRect(PTESTTABLEENTRY ptte, LPVOID pvObject) {

    D3DLOCKED_RECT      d3dlr;
    RECT                rect, *prect[2];
    HRESULT             hr;
    UINT                i, j;
    WORD                wPixel;
    D3DCUBEMAP_FACES    d3dcf;
    TRESULT             tr = TR_PASS;
    DWORD               dwFlags[] = {
                            0,
#ifndef UNDER_XBOX
                            D3DLOCK_DISCARD, 
                            D3DLOCK_NO_DIRTY_UPDATE, 
                            D3DLOCK_NOSYSLOCK, 
#endif
                        };
    LPCTSTR             szFlags[] = {
                            TEXT("no"),
#ifndef UNDER_XBOX
                            TEXT("D3DLOCK_DISCARD"), 
                            TEXT("D3DLOCK_NO_DIRTY_UPDATE"), 
                            TEXT("D3DLOCK_NOSYSLOCK"), 
#endif
                        };

    rect.left = 1;
    rect.top = 1;
    rect.right = 10;
    rect.bottom = 10;
    prect[0] = NULL;
    prect[1] = &rect;

    for (i = 0; i < 2; i++) {

        for (j = 0; j < countof(dwFlags); j++) {

            LogBeginVariation(TEXT("%s::UnlockRect using a %srect and %s flag"), ptte->szInterface, i ? TEXT("sub") : TEXT("NULL "), szFlags[j]);
            {
                switch (ptte->dwData) {

                    case ISURFACE8:
                        hr = ((LPDIRECT3DSURFACE8)pvObject)->LockRect(&d3dlr, prect[i], dwFlags[j]);
                        break;

                    case ITEXTURE8:
                        hr = ((LPDIRECT3DTEXTURE8)pvObject)->LockRect(0, &d3dlr, prect[i], dwFlags[j]);
                        break;

                    case ICUBETEXTURE8:
                        d3dcf = (D3DCUBEMAP_FACES)(rand() % 6);
                        hr = ((LPDIRECT3DCUBETEXTURE8)pvObject)->LockRect(d3dcf, 0, &d3dlr, prect[i], dwFlags[j]);
                        break;

                    default:
                        LogEndVariation();
                        return TR_ABORT;
                }

                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::LockRect failed [0x%X]"), ptte->szInterface, hr);
                    LogEndVariation();
                    return TR_FAIL;
                }

                if (!d3dlr.pBits) {
                    Log(LOG_FAIL, TEXT("%s::LockRect returned a NULL pointer to resource memory"), ptte->szInterface);
                    tr = TR_FAIL;
                }
                else {

                    wPixel = rand() % 0xFFFF;
                    *((LPWORD)d3dlr.pBits) = wPixel;
                }

                switch (ptte->dwData) {

                    case ISURFACE8:
                        hr = ((LPDIRECT3DSURFACE8)pvObject)->UnlockRect();
                        break;

                    case ITEXTURE8:
                        hr = ((LPDIRECT3DTEXTURE8)pvObject)->UnlockRect(0);
                        break;

                    case ICUBETEXTURE8:
                        hr = ((LPDIRECT3DCUBETEXTURE8)pvObject)->UnlockRect(d3dcf, 0);
                        break;

                    default:
                        LogEndVariation();
                        return TR_ABORT;
                }

                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::UnlockRect failed [0x%X]"), ptte->szInterface, hr);
                    LogEndVariation();
                    return TR_FAIL;
                }

                d3dlr.pBits = NULL;

                switch (ptte->dwData) {

                    case ISURFACE8:
                        hr = ((LPDIRECT3DSURFACE8)pvObject)->LockRect(&d3dlr, prect[i], dwFlags[j]);
                        break;

                    case ITEXTURE8:
                        hr = ((LPDIRECT3DTEXTURE8)pvObject)->LockRect(0, &d3dlr, prect[i], dwFlags[j]);
                        break;

                    case ICUBETEXTURE8:
                        hr = ((LPDIRECT3DCUBETEXTURE8)pvObject)->LockRect(d3dcf, 0, &d3dlr, prect[i], dwFlags[j]);
                        break;

                    default:
                        LogEndVariation();
                        return TR_ABORT;
                }

                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::LockRect failed [0x%X]"), ptte->szInterface, hr);
                    LogEndVariation();
                    return TR_FAIL;
                }

                if (*((LPWORD)d3dlr.pBits) != wPixel) {
                    Log(LOG_FAIL, TEXT("Pixel data failed to persist after calling %s::UnlockRect (set 0x%X, retrieved 0x%X"), ptte->szInterface, wPixel, *((LPWORD)d3dlr.pBits));
                    tr = TR_FAIL;
                }

                switch (ptte->dwData) {

                    case ISURFACE8:
                        hr = ((LPDIRECT3DSURFACE8)pvObject)->UnlockRect();
                        break;

                    case ITEXTURE8:
                        hr = ((LPDIRECT3DTEXTURE8)pvObject)->UnlockRect(0);
                        break;

                    case ICUBETEXTURE8:
                        hr = ((LPDIRECT3DCUBETEXTURE8)pvObject)->UnlockRect(d3dcf, 0);
                        break;

                    default:
                        LogEndVariation();
                        return TR_ABORT;
                }

                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::UnlockRect failed [0x%X]"), ptte->szInterface, hr);
                    LogEndVariation();
                    return TR_FAIL;
                }

                if (i) {

                    d3dlr.pBits = NULL;

                    switch (ptte->dwData) {

                        case ISURFACE8:
                            hr = ((LPDIRECT3DSURFACE8)pvObject)->LockRect(&d3dlr, NULL, dwFlags[j]);
                            break;

                        case ITEXTURE8:
                            hr = ((LPDIRECT3DTEXTURE8)pvObject)->LockRect(0, &d3dlr, NULL, dwFlags[j]);
                            break;

                        case ICUBETEXTURE8:
                            hr = ((LPDIRECT3DCUBETEXTURE8)pvObject)->LockRect(d3dcf, 0, &d3dlr, NULL, dwFlags[j]);
                            break;

                        default:
                            LogEndVariation();
                            return TR_ABORT;
                    }

                    if (FAILED(hr)) {
                        Log(LOG_FAIL, TEXT("%s::LockRect failed [0x%X]"), ptte->szInterface, hr);
                        LogEndVariation();
                        return TR_FAIL;
                    }

                    if (*((LPWORD)((LPBYTE)d3dlr.pBits + d3dlr.Pitch + 2)) != wPixel) {
                        Log(LOG_FAIL, TEXT("Surface pointer inconsistent with the rectangle given in %s::LockRect"), ptte->szInterface);
                        tr = TR_FAIL;
                    }

                    switch (ptte->dwData) {

                        case ISURFACE8:
                            hr = ((LPDIRECT3DSURFACE8)pvObject)->UnlockRect();
                            break;

                        case ITEXTURE8:
                            hr = ((LPDIRECT3DTEXTURE8)pvObject)->UnlockRect(0);
                            break;

                        case ICUBETEXTURE8:
                            hr = ((LPDIRECT3DCUBETEXTURE8)pvObject)->UnlockRect(d3dcf, 0);
                            break;

                        default:
                            LogEndVariation();
                            return TR_ABORT;
                    }

                    if (FAILED(hr)) {
                        Log(LOG_FAIL, TEXT("%s::UnlockRect failed [0x%X]"), ptte->szInterface, hr);
                        LogEndVariation();
                        return TR_FAIL;
                    }
                }
            }

            LogEndVariation();
        }
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TSHR8_LockBox(PTESTTABLEENTRY ptte, LPVOID pvObject) {

    D3DLOCKED_BOX       d3dlb;
    D3DBOX              box, *pbox[2];
    HRESULT             hr;
    UINT                i, j;
    BYTE                byte;
    TRESULT             tr = TR_PASS;
    DWORD               dwFlags[] = {
                            0,
#ifndef UNDER_XBOX
                            D3DLOCK_DISCARD, 
                            D3DLOCK_NO_DIRTY_UPDATE, 
                            D3DLOCK_NOSYSLOCK, 
#endif
                            D3DLOCK_READONLY, 
                        };
    LPCTSTR             szFlags[] = {
                            TEXT("no"),
#ifndef UNDER_XBOX
                            TEXT("D3DLOCK_DISCARD"), 
                            TEXT("D3DLOCK_NO_DIRTY_UPDATE"), 
                            TEXT("D3DLOCK_NOSYSLOCK"), 
#endif
                            TEXT("D3DLOCK_READONLY"), 
                        };

    box.Left = 1;
    box.Top = 1;
    box.Front = 1;
    box.Right = 10;
    box.Bottom = 10;
    box.Back = 10;
    pbox[0] = NULL;
    pbox[1] = &box;

    for (i = 0; i < 2; i++) {

        for (j = 0; j < countof(dwFlags); j++) {

            LogBeginVariation(TEXT("%s::LockBox using a %sbox and %s flag"), ptte->szInterface, i ? TEXT("sub") : TEXT("NULL "), szFlags[j]);
            {
                d3dlb.RowPitch = 0;
                d3dlb.SlicePitch = 0;
                d3dlb.pBits = NULL;

                switch (ptte->dwData) {

                    case IVOLUME8:
                        hr = ((LPDIRECT3DVOLUME8)pvObject)->LockBox(&d3dlb, pbox[i], dwFlags[j]);
                        break;

                    case IVOLUMETEXTURE8:
                        hr = ((LPDIRECT3DVOLUMETEXTURE8)pvObject)->LockBox(0, &d3dlb, pbox[i], dwFlags[j]);
                        break;

                    default:
                        LogEndVariation();
                        return TR_ABORT;
                }

                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::LockBox failed [0x%X]"), ptte->szInterface, hr);
                    LogEndVariation();
                    return TR_FAIL;
                }

                if (d3dlb.RowPitch < 2 * TEXTUREDIM) {
                    Log(LOG_FAIL, TEXT("%s::LockBox returned an incorrect row pitch of %d for the locked box"), ptte->szInterface, d3dlb.RowPitch);
                    tr = TR_FAIL;
                }

                if (d3dlb.SlicePitch < TEXTUREDIM * d3dlb.RowPitch) {
                    Log(LOG_FAIL, TEXT("%s::LockBox returned an incorrect slice pitch of %d for the locked box"), ptte->szInterface, d3dlb.SlicePitch);
                    tr = TR_FAIL;
                }

                if (!d3dlb.pBits) {
                    Log(LOG_FAIL, TEXT("%s::LockBox returned a NULL pointer to resource memory"), ptte->szInterface);
                    tr = TR_FAIL;
                }
                else {

                    // Reference the memory
                    byte = *((LPBYTE)d3dlb.pBits);
                    if (j != 3) {
                        *((LPBYTE)d3dlb.pBits) = byte;
                    }
                }

                switch (ptte->dwData) {

                    case IVOLUME8:
                        hr = ((LPDIRECT3DVOLUME8)pvObject)->UnlockBox();
                        break;

                    case IVOLUMETEXTURE8:
                        hr = ((LPDIRECT3DVOLUMETEXTURE8)pvObject)->UnlockBox(0);
                        break;
                }

                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::UnlockBox failed [0x%X]"), ptte->szInterface, hr);
                    LogEndVariation();
                    return TR_FAIL;
                }
            }

            LogEndVariation();
        }
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TSHR8_UnlockBox(PTESTTABLEENTRY ptte, LPVOID pvObject) {

    D3DLOCKED_BOX       d3dlb;
    D3DBOX              box, *pbox[2];
    HRESULT             hr;
    UINT                i, j;
    WORD                wPixel;
    TRESULT             tr = TR_PASS;
    DWORD               dwFlags[] = {
                            0,
#ifndef UNDER_XBOX
                            D3DLOCK_DISCARD, 
                            D3DLOCK_NO_DIRTY_UPDATE, 
                            D3DLOCK_NOSYSLOCK, 
#endif
                        };
    LPCTSTR             szFlags[] = {
                            TEXT("no"),
#ifndef UNDER_XBOX
                            TEXT("D3DLOCK_DISCARD"), 
                            TEXT("D3DLOCK_NO_DIRTY_UPDATE"), 
                            TEXT("D3DLOCK_NOSYSLOCK"), 
#endif
                        };

    box.Left = 1;
    box.Top = 1;
    box.Front = 1;
    box.Right = 10;
    box.Bottom = 10;
    box.Back = 10;
    pbox[0] = NULL;
    pbox[1] = &box;

    for (i = 0; i < 2; i++) {

        for (j = 0; j < countof(dwFlags); j++) {

            LogBeginVariation(TEXT("%s::UnlockBox using a %sbox and %s flag"), ptte->szInterface, i ? TEXT("sub") : TEXT("NULL "), szFlags[j]);
            {
                switch (ptte->dwData) {

                    case IVOLUME8:
                        hr = ((LPDIRECT3DVOLUME8)pvObject)->LockBox(&d3dlb, pbox[i], dwFlags[j]);
                        break;

                    case IVOLUMETEXTURE8:
                        hr = ((LPDIRECT3DVOLUMETEXTURE8)pvObject)->LockBox(0, &d3dlb, pbox[i], dwFlags[j]);
                        break;

                    default:
                        LogEndVariation();
                        return TR_ABORT;
                }

                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::LockBox failed [0x%X]"), ptte->szInterface, hr);
                    LogEndVariation();
                    return TR_FAIL;
                }

                if (!d3dlb.pBits) {
                    Log(LOG_FAIL, TEXT("%s::LockBox returned a NULL pointer to resource memory"), ptte->szInterface);
                    tr = TR_FAIL;
                }
                else {

                    wPixel = rand() % 0xFFFF;
                    *((LPWORD)d3dlb.pBits) = wPixel;
                }

                switch (ptte->dwData) {

                    case IVOLUME8:
                        hr = ((LPDIRECT3DVOLUME8)pvObject)->UnlockBox();
                        break;

                    case IVOLUMETEXTURE8:
                        hr = ((LPDIRECT3DVOLUMETEXTURE8)pvObject)->UnlockBox(0);
                        break;
                }

                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::UnlockBox failed [0x%X]"), ptte->szInterface, hr);
                    LogEndVariation();
                    return TR_FAIL;
                }

                d3dlb.pBits = NULL;

                switch (ptte->dwData) {

                    case IVOLUME8:
                        hr = ((LPDIRECT3DVOLUME8)pvObject)->LockBox(&d3dlb, pbox[i], dwFlags[j]);
                        break;

                    case IVOLUMETEXTURE8:
                        hr = ((LPDIRECT3DVOLUMETEXTURE8)pvObject)->LockBox(0, &d3dlb, pbox[i], dwFlags[j]);
                        break;
                }

                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::LockBox failed [0x%X]"), ptte->szInterface, hr);
                    LogEndVariation();
                    return TR_FAIL;
                }

                if (*((LPWORD)d3dlb.pBits) != wPixel) {
                    Log(LOG_FAIL, TEXT("Pixel data failed to persist after calling %s::UnlockBox"), ptte->szInterface);
                    tr = TR_FAIL;
                }

                switch (ptte->dwData) {

                    case IVOLUME8:
                        hr = ((LPDIRECT3DVOLUME8)pvObject)->UnlockBox();
                        break;

                    case IVOLUMETEXTURE8:
                        hr = ((LPDIRECT3DVOLUMETEXTURE8)pvObject)->UnlockBox(0);
                        break;
                }

                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::UnlockBox failed [0x%X]"), ptte->szInterface, hr);
                    LogEndVariation();
                    return TR_FAIL;
                }

                if (i) {

                    d3dlb.pBits = NULL;

                    switch (ptte->dwData) {

                        case IVOLUME8:
                            hr = ((LPDIRECT3DVOLUME8)pvObject)->LockBox(&d3dlb, NULL, dwFlags[j]);
                            break;

                        case IVOLUMETEXTURE8:
                            hr = ((LPDIRECT3DVOLUMETEXTURE8)pvObject)->LockBox(0, &d3dlb, NULL, dwFlags[j]);
                            break;
                    }

                    if (FAILED(hr)) {
                        Log(LOG_FAIL, TEXT("%s::LockBox failed [0x%X]"), ptte->szInterface, hr);
                        LogEndVariation();
                        return TR_FAIL;
                    }

                    if (*((LPWORD)((LPBYTE)d3dlb.pBits + d3dlb.SlicePitch + d3dlb.RowPitch + 2)) != wPixel) {
                        Log(LOG_FAIL, TEXT("Surface pointer inconsistent with the box given in %s::LockBox"), ptte->szInterface);
                        tr = TR_FAIL;
                    }

                    switch (ptte->dwData) {

                        case IVOLUME8:
                            hr = ((LPDIRECT3DVOLUME8)pvObject)->UnlockBox();
                            break;

                        case IVOLUMETEXTURE8:
                            hr = ((LPDIRECT3DVOLUMETEXTURE8)pvObject)->UnlockBox(0);
                            break;
                    }

                    if (FAILED(hr)) {
                        Log(LOG_FAIL, TEXT("%s::UnlockBox failed [0x%X]"), ptte->szInterface, hr);
                        LogEndVariation();
                        return TR_FAIL;
                    }
                }
            }

            LogEndVariation();
        }
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TSHR8_GetLevelDesc(PTESTTABLEENTRY ptte, LPVOID pvObject) {

    D3DSURFACE_DESC     d3dsd;
    UINT                uLength;
    UINT                uLevels;
    UINT                i;
    D3DRESOURCETYPE     d3drt;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    for (uLevels = 1; 0x1 << (uLevels - 1) != TEXTUREDIM; uLevels++);

    for (i = 0; i < uLevels; i++) {

        switch (ptte->dwData) {

            case ITEXTURE8:
                LogBeginVariation(TEXT("IDirect3DTexture8::GetLevelDesc on texture level %d"), i);
                hr = ((LPDIRECT3DTEXTURE8)pvObject)->GetLevelDesc(i, &d3dsd);
                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::%s failed for level %d [0x%X]"), ptte->szInterface, ptte->szMethod, i, hr);
                    return TR_FAIL;
                }
                d3drt = D3DRTYPE_TEXTURE;
                break;

            case ICUBETEXTURE8:
                LogBeginVariation(TEXT("IDirect3DCubeTexture8::GetLevelDesc on cube texture level %d"), i);
                hr = ((LPDIRECT3DCUBETEXTURE8)pvObject)->GetLevelDesc(i, &d3dsd);
                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::%s failed for level %d [0x%X]"), ptte->szInterface, ptte->szMethod, i, hr);
                    return TR_FAIL;
                }
                d3drt = D3DRTYPE_CUBETEXTURE;
                break;

            default:
                return TR_ABORT;
        }

        uLength = (0x1 << ((uLevels - 1) - i));

        if (d3dsd.Width != uLength) {
            Log(LOG_FAIL, TEXT("%s::GetLevelDesc returned an incorrect width of %d instead of %d for level %d"), ptte->szInterface, d3dsd.Width, uLength, i);
            tr = TR_FAIL;
        }

        if (d3dsd.Height != uLength) {
            Log(LOG_FAIL, TEXT("%s::GetLevelDesc returned an incorrect height of %d instead of %d for level %d"), ptte->szInterface, d3dsd.Height, uLength, i);
            tr = TR_FAIL;
        }

        if (d3dsd.Size < uLength * uLength * 2) {
            Log(LOG_FAIL, TEXT("%s::GetLevelDesc returned an incorrect size of %d for level %d"), ptte->szInterface, d3dsd.Size, i);
            tr = TR_FAIL;
        }

#ifndef UNDER_XBOX
        if (d3dsd.Pool != D3DPOOL_MANAGED) {
            Log(LOG_FAIL, TEXT("%s::GetLevelDesc returned an incorrect pool for level %d"), ptte->szInterface, i);
            tr = TR_FAIL;
        }
#endif // !UNDER_XBOX

        if (d3dsd.Usage != 0) {
            Log(LOG_FAIL, TEXT("%s::GetLevelDesc returned an incorrect usage for level %d"), ptte->szInterface, i);
            tr = TR_FAIL;
        }

        if (d3dsd.Type != d3drt) {
            Log(LOG_FAIL, TEXT("%s::GetLevelDesc returned an incorrect type for level %d"), ptte->szInterface, i);
            tr = TR_FAIL;
        }

        if (d3dsd.Format != D3DFMT_A1R5G5B5) {
            Log(LOG_FAIL, TEXT("%s::GetLevelDesc returned an incorrect format for level %d"), ptte->szInterface, i);
            tr = TR_FAIL;
        }

        LogEndVariation();
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TSHR8_GetSurfaceLevel(PTESTTABLEENTRY ptte, LPVOID pvObject) {

    LPDIRECT3DSURFACE8  pd3ds, pd3ds2;
    D3DSURFACE_DESC     d3dsd;
    ULONG               uRef1, uRef2;
    UINT                uLevels;
    UINT                uLength;
    UINT                i;
    D3DCUBEMAP_FACES    d3dcf;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    for (uLevels = 1; 0x1 << (uLevels - 1) != TEXTUREDIM; uLevels++);

    for (i = 0; i < uLevels; i++) {

        switch (ptte->dwData) {

            case ITEXTURE8:
                LogBeginVariation(TEXT("IDirect3DTexture8::GetSurfaceLevel on texture level %d"), i);
                hr = ((LPDIRECT3DTEXTURE8)pvObject)->GetSurfaceLevel(i, &pd3ds);
                break;

            case ICUBETEXTURE8:
                LogBeginVariation(TEXT("IDirect3DCubeTexture8::GetCubeMapSurface on cube texture level %d"), i);
                d3dcf = (D3DCUBEMAP_FACES)(rand() % 6);
                hr = ((LPDIRECT3DCUBETEXTURE8)pvObject)->GetCubeMapSurface(d3dcf, i, &pd3ds);
                break;

            default:
                return TR_ABORT;
        }

        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("%s::%s failed for level %d [0x%X]"), ptte->szInterface, ptte->szMethod, i, hr);
            return TR_FAIL;
        }

        pd3ds->AddRef();
        uRef1 = pd3ds->Release();

        switch (ptte->dwData) {

            case ITEXTURE8:
                hr = ((LPDIRECT3DTEXTURE8)pvObject)->GetSurfaceLevel(i, &pd3ds2);
                break;

            case ICUBETEXTURE8:
                hr = ((LPDIRECT3DCUBETEXTURE8)pvObject)->GetCubeMapSurface(d3dcf, i, &pd3ds2);
                break;

            default:
                pd3ds->Release();
                return TR_ABORT;
        }

        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("%s::%s failed for level %d [0x%X]"), ptte->szInterface, ptte->szMethod, i, hr);
            pd3ds->Release();
            return TR_FAIL;
        }

#ifndef UNDER_XBOX
        if (pd3ds2 != pd3ds) {
            Log(LOG_FAIL, TEXT("%s::%s returned an incorrect surface pointer for level %d"), ptte->szInterface, ptte->szMethod, i);
            tr = TR_FAIL;
        }
#endif // !UNDER_XBOX

        pd3ds->AddRef();
        uRef2 = pd3ds->Release();

#ifndef UNDER_XBOX
        if (uRef2 != uRef1 + 1) {
            Log(LOG_FAIL, TEXT("%s::%s failed to increment the reference count of the surface for level %d"), ptte->szInterface, ptte->szMethod, i);
        }
#endif // !UNDER_XBOX

        pd3ds2->Release();

        hr = pd3ds->GetDesc(&d3dsd);
        pd3ds->Release();
        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("IDirect3DSurface8::GetDesc failed [0x%X]"), hr);
            return TR_FAIL;
        }

        uLength = (0x1 << ((uLevels - 1) - i));

        if (d3dsd.Width != uLength) {
            Log(LOG_FAIL, TEXT("IDirect3DSurface8::GetDesc returned an incorrect width of %d instead of %d for level %d"), ptte->szInterface, d3dsd.Width, uLength, i);
            tr = TR_FAIL;
        }

        if (d3dsd.Height != uLength) {
            Log(LOG_FAIL, TEXT("IDirect3DSurface8::GetDesc returned an incorrect height of %d instead of %d for level %d"), ptte->szInterface, d3dsd.Height, uLength, i);
            tr = TR_FAIL;
        }

        if (d3dsd.Size < uLength * uLength * 2) {
            Log(LOG_FAIL, TEXT("IDirect3DSurface8::GetDesc returned an incorrect size of %d for level %d"), ptte->szInterface, d3dsd.Size, i);
            tr = TR_FAIL;
        }

#ifndef UNDER_XBOX
        if (d3dsd.Pool != D3DPOOL_MANAGED) {
            Log(LOG_FAIL, TEXT("IDirect3DSurface8::GetDesc returned an incorrect pool for level %d"), ptte->szInterface, i);
            tr = TR_FAIL;
        }
#endif

        if (d3dsd.Usage != 0) {
            Log(LOG_FAIL, TEXT("IDirect3DSurface8::GetDesc returned an incorrect usage for level %d"), ptte->szInterface, i);
            tr = TR_FAIL;
        }

        if (d3dsd.Type != D3DRTYPE_SURFACE) {
            Log(LOG_FAIL, TEXT("IDirect3DSurface8::GetDesc returned an incorrect type for level %d"), ptte->szInterface, i);
            tr = TR_FAIL;
        }

        if (d3dsd.Format != D3DFMT_A1R5G5B5) {
            Log(LOG_FAIL, TEXT("IDirect3DSurface8::GetDesc returned an incorrect format for level %d"), ptte->szInterface, i);
            tr = TR_FAIL;
        }

        LogEndVariation();
    }

    return tr;
}

#ifndef UNDER_XBOX

//******************************************************************************
TESTPROCAPI TSHR8_AddDirtyRect(PTESTTABLEENTRY ptte, LPVOID pvObject) {

    RECT                rect, *prect[2];
    UINT                i;
    HRESULT             hr;
    TRESULT             tr = TR_PASS;

    rect.left = 11;
    rect.top = 9;
    rect.right = 12;
    rect.bottom = 17;
    prect[0] = NULL;
    prect[1] = &rect;

    for (i = 0; i < 2; i++) {

        switch (ptte->dwData) {

            case ITEXTURE8:
                LogBeginVariation(TEXT("IDirect3DTexture8::AddDirtyRect on a %srect"), i ? TEXT("sub") : TEXT("NULL "));
                hr = ((LPDIRECT3DTEXTURE8)pvObject)->AddDirtyRect(prect[i]);
                break;

            case ICUBETEXTURE8:
                LogBeginVariation(TEXT("IDirect3DCubeTexture8::AddDirtyRect on a %srect"), i ? TEXT("sub") : TEXT("NULL "));
                hr = ((LPDIRECT3DCUBETEXTURE8)pvObject)->AddDirtyRect((D3DCUBEMAP_FACES)(rand() % 6), prect[i]);
                break;

            default:
                return TR_ABORT;
        }

        if (FAILED(hr)) {
            Log(LOG_FAIL, TEXT("%s::AddDirtyRect failed [0x%X]"), ptte->szInterface, hr);
            tr = TR_FAIL;
        }

        LogEndVariation();
    }

    return tr;
}

#endif // !UNDER_XBOX

//******************************************************************************
TESTPROCAPI TSHR8_Lock(PTESTTABLEENTRY ptte, LPVOID pvObject) {

    LPBYTE              pdata;
    UINT                uOffset[2];
    UINT                uSize[2];
    HRESULT             hr;
    UINT                i, j;
    BYTE                byte;
    TRESULT             tr = TR_PASS;
    DWORD               dwFlags[] = {
                            0,
#ifndef UNDER_XBOX
                            D3DLOCK_DISCARD, 
                            D3DLOCK_NO_DIRTY_UPDATE, 
                            D3DLOCK_NOSYSLOCK, 
#endif
                            D3DLOCK_NOOVERWRITE,
                            D3DLOCK_READONLY, 
                        };
    LPCTSTR             szFlags[] = {
                            TEXT("no"),
#ifndef UNDER_XBOX
                            TEXT("D3DLOCK_DISCARD"), 
                            TEXT("D3DLOCK_NO_DIRTY_UPDATE"), 
                            TEXT("D3DLOCK_NOSYSLOCK"), 
#endif
                            TEXT("D3DLOCK_NOOVERWRITE"),
                            TEXT("D3DLOCK_READONLY"), 
                        };

    uOffset[0] = 0;
    uOffset[1] = VERTBUFFSIZE < INDXBUFFSIZE ? VERTBUFFSIZE : INDXBUFFSIZE;
    uOffset[1] /= 2;

    switch (ptte->dwData) {

        case IVERTEXBUFFER8:
            uSize[0] = VERTBUFFSIZE;
            uSize[1] = VERTBUFFSIZE - uOffset[1];
            break;

        case IINDEXBUFFER8:
            uSize[0] = INDXBUFFSIZE;
            uSize[1] = INDXBUFFSIZE - uOffset[1];
            break;

        default:
            return TR_ABORT;
    }

    for (i = 0; i < 2; i++) {

        for (j = 0; j < countof(dwFlags); j++) {

            LogBeginVariation(TEXT("%s::Lock %s an offset and using %s flag"), ptte->szInterface, i ? TEXT("with") : TEXT("without"), szFlags[j]);
            {
                pdata = NULL;

                switch (ptte->dwData) {

                    case IVERTEXBUFFER8:
                        hr = ((LPDIRECT3DVERTEXBUFFER8)pvObject)->Lock(uOffset[i], uSize[i], &pdata, dwFlags[j]);
                        break;

                    case IINDEXBUFFER8:
                        hr = ((LPDIRECT3DINDEXBUFFER8)pvObject)->Lock(uOffset[i], uSize[i], &pdata, dwFlags[j]);
                        break;
                }

                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::Lock failed [0x%X]"), ptte->szInterface, hr);
                    LogEndVariation();
                    return TR_FAIL;
                }

                if (!pdata) {
                    Log(LOG_FAIL, TEXT("%s::Lock returned a NULL pointer to buffer memory"), ptte->szInterface);
                    tr = TR_FAIL;
                }
                else {

                    // Reference the memory
                    byte = *pdata;
                    if (j != 4) {
                        *pdata = byte;
                    }
                }

                switch (ptte->dwData) {

                    case IVERTEXBUFFER8:
                        hr = ((LPDIRECT3DVERTEXBUFFER8)pvObject)->Unlock();
                        break;

                    case IINDEXBUFFER8:
                        hr = ((LPDIRECT3DINDEXBUFFER8)pvObject)->Unlock();
                        break;
                }

                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::Unlock failed [0x%X]"), ptte->szInterface, hr);
                    LogEndVariation();
                    return TR_FAIL;
                }
            }

            LogEndVariation();
        }
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TSHR8_Unlock(PTESTTABLEENTRY ptte, LPVOID pvObject) {

    LPBYTE              pdata;
    UINT                uOffset[2];
    UINT                uSize[2];
    HRESULT             hr;
    UINT                i, j;
    BYTE                byte;
    TRESULT             tr = TR_PASS;
    DWORD               dwFlags[] = {
                            0,
#ifndef UNDER_XBOX
                            D3DLOCK_DISCARD, 
                            D3DLOCK_NO_DIRTY_UPDATE, 
                            D3DLOCK_NOSYSLOCK, 
#endif
                            D3DLOCK_NOOVERWRITE,
                        };
    LPCTSTR             szFlags[] = {
                            TEXT("no"),
#ifndef UNDER_XBOX
                            TEXT("D3DLOCK_DISCARD"), 
                            TEXT("D3DLOCK_NO_DIRTY_UPDATE"), 
                            TEXT("D3DLOCK_NOSYSLOCK"), 
#endif
                            TEXT("D3DLOCK_NOOVERWRITE"),
                        };

    uOffset[0] = 0;
    uOffset[1] = VERTBUFFSIZE < INDXBUFFSIZE ? VERTBUFFSIZE : INDXBUFFSIZE;
    uOffset[1] /= 2;

    switch (ptte->dwData) {

        case IVERTEXBUFFER8:
            uSize[0] = rand() % 2 ? VERTBUFFSIZE : 0;
            uSize[1] = VERTBUFFSIZE - uOffset[1];
            break;

        case IINDEXBUFFER8:
            uSize[0] = rand() % 2 ? INDXBUFFSIZE : 0;
            uSize[1] = INDXBUFFSIZE - uOffset[1];
            break;

        default:
            return TR_ABORT;
    }

    for (i = 0; i < 2; i++) {

        for (j = 0; j < countof(dwFlags); j++) {

            LogBeginVariation(TEXT("%s::Unlock %s an offset and using %s flag"), ptte->szInterface, i ? TEXT("with") : TEXT("without"), szFlags[j]);
            {
                switch (ptte->dwData) {

                    case IVERTEXBUFFER8:
                        hr = ((LPDIRECT3DVERTEXBUFFER8)pvObject)->Lock(uOffset[i], uSize[i], &pdata, dwFlags[j]);
                        break;

                    case IINDEXBUFFER8:
                        hr = ((LPDIRECT3DINDEXBUFFER8)pvObject)->Lock(uOffset[i], uSize[i], &pdata, dwFlags[j]);
                        break;
                }

                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::Lock failed [0x%X]"), ptte->szInterface, hr);
                    LogEndVariation();
                    return TR_FAIL;
                }

                if (!pdata) {
                    Log(LOG_FAIL, TEXT("%s::Lock returned a NULL pointer to buffer memory"), ptte->szInterface);
                    tr = TR_FAIL;
                }
                else {

                    byte = rand() % 256;
                    *pdata = byte;
                }

                switch (ptte->dwData) {

                    case IVERTEXBUFFER8:
                        hr = ((LPDIRECT3DVERTEXBUFFER8)pvObject)->Unlock();
                        break;

                    case IINDEXBUFFER8:
                        hr = ((LPDIRECT3DINDEXBUFFER8)pvObject)->Unlock();
                        break;
                }

                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::Unlock failed [0x%X]"), ptte->szInterface, hr);
                    LogEndVariation();
                    return TR_FAIL;
                }

                pdata = NULL;

                switch (ptte->dwData) {

                    case IVERTEXBUFFER8:
                        hr = ((LPDIRECT3DVERTEXBUFFER8)pvObject)->Lock(uOffset[i], uSize[i], &pdata, dwFlags[j]);
                        break;

                    case IINDEXBUFFER8:
                        hr = ((LPDIRECT3DINDEXBUFFER8)pvObject)->Lock(uOffset[i], uSize[i], &pdata, dwFlags[j]);
                        break;
                }

                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::Lock failed [0x%X]"), ptte->szInterface, hr);
                    LogEndVariation();
                    return TR_FAIL;
                }

                if (*pdata != byte) {
                    Log(LOG_FAIL, TEXT("Buffer data failed to persist after calling %s::Rect"), ptte->szInterface);
                    tr = TR_FAIL;
                }

                switch (ptte->dwData) {

                    case IVERTEXBUFFER8:
                        hr = ((LPDIRECT3DVERTEXBUFFER8)pvObject)->Unlock();
                        break;

                    case IINDEXBUFFER8:
                        hr = ((LPDIRECT3DINDEXBUFFER8)pvObject)->Unlock();
                        break;
                }

                if (FAILED(hr)) {
                    Log(LOG_FAIL, TEXT("%s::Unlock failed [0x%X]"), ptte->szInterface, hr);
                    LogEndVariation();
                    return TR_FAIL;
                }

                if (i) {

                pdata = NULL;

                    switch (ptte->dwData) {

                        case IVERTEXBUFFER8:
                            hr = ((LPDIRECT3DVERTEXBUFFER8)pvObject)->Lock(0, 0, &pdata, dwFlags[j]);
                            break;

                        case IINDEXBUFFER8:
                            hr = ((LPDIRECT3DINDEXBUFFER8)pvObject)->Lock(0, 0, &pdata, dwFlags[j]);
                            break;
                    }

                    if (FAILED(hr)) {
                        Log(LOG_FAIL, TEXT("%s::Lock failed [0x%X]"), ptte->szInterface, hr);
                        LogEndVariation();
                        return TR_FAIL;
                    }

                    if (*(pdata + uOffset[1]) != byte) {
                        Log(LOG_FAIL, TEXT("Buffer pointer inconsistent with the offset given in %s::Lock"), ptte->szInterface);
                        tr = TR_FAIL;
                    }

                    switch (ptte->dwData) {

                        case IVERTEXBUFFER8:
                            hr = ((LPDIRECT3DVERTEXBUFFER8)pvObject)->Unlock();
                            break;

                        case IINDEXBUFFER8:
                            hr = ((LPDIRECT3DINDEXBUFFER8)pvObject)->Unlock();
                            break;
                    }

                    if (FAILED(hr)) {
                        Log(LOG_FAIL, TEXT("%s::Unlock failed [0x%X]"), ptte->szInterface, hr);
                        LogEndVariation();
                        return TR_FAIL;
                    }
                }
            }

            LogEndVariation();
        }
    }

    return tr;
}

#ifndef UNDER_XBOX

//******************************************************************************
// WndProc
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     WndProc
//
// Description:
//
//     Window procedure to process messages for the main application window
//
// Arguments:
//
//     HWND hWnd                - Application window
//
//     UINT message             - Message to process
//
//     WPARAM wParam            - First message parameter
//
//     LPARAM lParam            - Second message parameter
//
// Return Value:
//
//     0 if the message was handled, the return of DefWindowProc otherwise.
//
//******************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {

        case WM_SETCURSOR:

            SetCursor(NULL);
            return TRUE;

        case WM_MOUSEMOVE:

            if (g_pd3dd8) {
                POINT point;
                GetCursorPos(&point);
                ScreenToClient(hWnd, &point);
                g_pd3dd8->SetCursorPosition(point.x, point.y, D3DCURSOR_IMMEDIATE_UPDATE);
            }
            break;

        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

#endif // UNDER_XBOX
