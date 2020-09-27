/*--
Copyright (c) 1999 - 2000 Microsoft Corporation - Xbox SDK

Module Name:

    Main.cpp

Abstract:

    Complete DX8 application to draw a screen space triangle.

Revision History:

    Derived from a DX8 sample.
--*/

#include <stddef.h>
#include <stdio.h>

extern "C"
{
    #include <ntos.h>
    #include <nturtl.h>
    #include <av.h>
};

#include <xtl.h>
#include <xgraphics.h>

IDirect3DDevice8* g_pDev  = NULL;

bool InitD3D()
{
    IDirect3D8 *pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    if (pD3D == NULL)
        return false;

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth           = 640;
    d3dpp.BackBufferHeight          = 480;
    d3dpp.BackBufferFormat          = D3DFMT_A8R8G8B8;
    d3dpp.BackBufferCount           = 1;
    d3dpp.Windowed                  = false;   // Must be false for Xbox.
    d3dpp.EnableAutoDepthStencil    = true;
    d3dpp.AutoDepthStencilFormat    = D3DFMT_D24S8;
    d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;

    if (pD3D->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        NULL,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &g_pDev) != S_OK)
    {
        return false;
    }

    pD3D->Release();

    return true;
}

#define PUSHBUFFERSIZE 32768

void __cdecl main()
{
    D3DPushBuffer* pPushBuffer;
    DWORD offset;
    DWORD jumpOffset;

    InitD3D();

    g_pDev->CreatePushBuffer(32768, FALSE, &pPushBuffer);
    g_pDev->BeginPushBuffer(pPushBuffer);
    do {
        g_pDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, 0, 0, 0);
        g_pDev->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xff, 0, 0xff);
        g_pDev->GetPushBufferOffset(&offset);

    } while (offset < PUSHBUFFERSIZE - 1024);

    g_pDev->GetPushBufferOffset(&jumpOffset);
    g_pDev->Nop();
    g_pDev->EndPushBuffer();

    pPushBuffer->BeginFixup(NULL, 0);
    pPushBuffer->Jump(jumpOffset, 0);
    pPushBuffer->EndFixup();

    g_pDev->RunPushBuffer(pPushBuffer, NULL);
    g_pDev->KickPushBuffer();

    while (g_pDev->IsBusy())
        ;

    DbgPrint("Done!\n");

    while (TRUE)
        ;
}
