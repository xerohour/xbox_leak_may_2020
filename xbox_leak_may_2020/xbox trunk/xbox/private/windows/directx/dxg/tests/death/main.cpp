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

IDirect3DDevice8*          g_pDev  = NULL;
BOOL                       g_Done;

#define PUSHBUFFERSIZE 32*1024*1024
#define SCRATCHSIZE 16*1024

bool InitD3D()
{
    IDirect3D8 *pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    if (pD3D == NULL)
        return false;

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth           = 640;
    d3dpp.BackBufferHeight          = 480;
    d3dpp.BackBufferFormat          = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount           = 1;
    d3dpp.Windowed                  = false;   // Must be false for Xbox.
    d3dpp.EnableAutoDepthStencil    = true;
    d3dpp.AutoDepthStencilFormat    = D3DFMT_D24S8;
    d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;

    // Create the device.
    if (pD3D->CreateDevice
    (
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        NULL,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &g_pDev
    ) != S_OK)
    {
        return false;
    }

    pD3D->Release();

    return true;
}

void __cdecl AllDone(DWORD Context)
{
    DbgPrint("...We survived!\n");
}

void StripedClear()
{
    D3DRECT rect;
    INT left;
    INT right;

    for (DWORD i = 0; i < 5; i++)
    {
        INT left = 0;

        while (left < 640)
        {
            rect.x1 = left;
            rect.x2 = min(640, left + 128);
            rect.y1 = 0;
            rect.y2 = 480;

            g_pDev->Clear(1, &rect, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, 0, 1.0, 0);

            left = rect.x2;
        }
    }
}

void Die2()
{
    D3DPushBuffer* pPushBuffer;
    DWORD offset;
    VOID* pScratch;
    BYTE* p;
    DWORD i;

    DbgPrint("Starting test...\n");

    pScratch = D3D_AllocContiguousMemory(SCRATCHSIZE, 0);

    g_pDev->CreatePushBuffer(PUSHBUFFERSIZE, FALSE, &pPushBuffer);
    g_pDev->BeginPushBuffer(pPushBuffer);
    do {
        StripedClear();
        g_pDev->GetPushBufferOffset(&offset);

    } while (offset < PUSHBUFFERSIZE - 1024);
    
    g_pDev->InsertCallback(D3DCALLBACK_READ, AllDone, 0);
    g_pDev->EndPushBuffer();

    g_pDev->RunPushBuffer(pPushBuffer, NULL);
    g_pDev->KickPushBuffer();

    while (TRUE)
        ;
}

void Die1()
{
    DWORD count = 0;
    while (TRUE)
    {
        StripedClear();
        g_pDev->InsertFence();
        if ((count % 100) == 0)
            DbgPrint("Count: %li\n", count);
        count++;
    }
}

void __cdecl main()
{
    InitD3D();

    Die2();
}
