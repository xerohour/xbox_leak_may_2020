/*--

Copyright (c) 1999 - 2001 Microsoft Corporation - Xbox SDK

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

IDirect3DDevice8* pDev  = NULL;

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
    d3dpp.EnableAutoDepthStencil    = true;
    d3dpp.AutoDepthStencilFormat    = D3DFMT_D24S8;
    d3dpp.MultiSampleType           = D3DMULTISAMPLE_NONE;

    if (pD3D->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        NULL,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &pDev) != S_OK)
    {
        return false;
    }

    pD3D->Release();

    return true;
}

#define NOP_COUNT 20000

DWORD i;
BYTE nopBuffer[NOP_COUNT + 1];

void __cdecl main()
{
    DWORD j;

    InitD3D();

    for (j = 0; j < NOP_COUNT; j++)
    {
        nopBuffer[j] = 0x90;        // nop
    }
    nopBuffer[NOP_COUNT] = 0xc3;    // ret

    Sleep(2000);

    for (i = 0; ; i++)
    {
        _asm 
        {
            lea esi, nopBuffer
            call esi
        }

        if ((i % 1000) == 0)
        {
            DbgPrint("Iteration... %li\n", i);
        }

        pDev->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xff, 0, 0xff);
        pDev->KickPushBuffer();
        while (pDev->IsBusy())
            ;
    }
}
