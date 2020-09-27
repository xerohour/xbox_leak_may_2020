/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       regress.cpp
 *  Content:    Main file for the D3D regression test
 *
 ***************************************************************************/
 
#include "precomp.hpp"

//------------------------------------------------------------------------------
D3DDevice* InitD3D()
{
    D3DDevice* pDev = NULL;

    Direct3D* pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    if (pD3D == NULL)
        return NULL;

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
    d3dpp.FullScreen_RefreshRateInHz= 60;
    d3dpp.MultiSampleType           = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX;

    if (pD3D->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        NULL,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &pDev) != S_OK)
    {
        return NULL;
    }

    pD3D->Release();

    return pDev;
}

//------------------------------------------------------------------------------
void TestScissors(D3DDevice* pDev)
{
    D3DRECT originalRect;
    D3DRECT rects[2] = { { 100, 100, 350, 350 }, { 300, 300, 800, 400 } };
    D3DRECT getRects[2];
    BOOL Exclusive = TRUE;
    DWORD count;
    BOOL exclusive;

    CheckHR(pDev->GetScissors(&count, &exclusive, &originalRect));

    ASSERT((count == 1) && 
           (exclusive == FALSE) && 
           (originalRect.x1 == 0) &&
           (originalRect.y1 == 0) &&
           (originalRect.x2 == 640) &&
           (originalRect.y2 == 480));
    
    CheckHR(pDev->SetScissors(2, Exclusive, rects));
    CheckHR(pDev->GetScissors(&count, &exclusive, getRects));
    
    if ((count != 2) ||
        (exclusive != Exclusive) ||
        (memcmp(getRects, rects, 2 * sizeof(D3DRECT)) != 0))
    {
        RIP(("Failed"));
    }

    Exclusive = FALSE;
    
    CheckHR(pDev->SetScissors(2, Exclusive, rects));
    CheckHR(pDev->GetScissors(&count, &exclusive, getRects));
    
    if ((count != 2) ||
        (exclusive != Exclusive) ||
        (memcmp(getRects, rects, 2 * sizeof(D3DRECT)) != 0))
    {
        RIP(("Failed"));
    }

    // Restore the scissors to their original state:

    CheckHR(pDev->SetScissors(0, 0, NULL));
}

//------------------------------------------------------------------------------
void TestPeristentAttributes(D3DDevice* pDev)
{
    // We used to have a problem with doing these not in a Begin/End bracket:

    for (DWORD i = 0; i < 512000; i++)
    {
        pDev->SetVertexData4f(0, 0, 0, 0, 0);
    }
}

//------------------------------------------------------------------------------
void TestCreateDeviceAndReset()
{
    D3DDevice* pDev = NULL;
    D3DSurface* pColorSurface1;
    D3DSurface* pColorSurface2;
    D3DSurface* pZSurface;

    // Yes, it's legal to use a NULL pDev for this:
    
    pDev->CreateImageSurface(640, 480, D3DFMT_LIN_X1R5G5B5, &pColorSurface1);
    pDev->CreateImageSurface(640, 480, D3DFMT_LIN_X1R5G5B5, &pColorSurface2);
    pDev->CreateImageSurface(640, 480, D3DFMT_LIN_D16, &pZSurface);

    // Now create the object:

    Direct3D* pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    ASSERT(pD3D != NULL);

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth           = 640;
    d3dpp.BackBufferHeight          = 480;
    d3dpp.BackBufferFormat          = D3DFMT_LIN_X1R5G5B5;
    d3dpp.BackBufferCount           = 1;
    d3dpp.Windowed                  = false;   // Must be false for Xbox.
    d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_RefreshRateInHz= 60;
    d3dpp.MultiSampleType           = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX;
    d3dpp.BufferSurfaces[0]         = pColorSurface1;
    d3dpp.BufferSurfaces[1]         = pColorSurface2;
    d3dpp.DepthStencilSurface       = pZSurface;

    if (pD3D->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        NULL,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &pDev) != S_OK)
    {
        RIP(("Argh"));
    }

    pD3D->Release();

    pColorSurface1->Release();
    pColorSurface2->Release();
    pZSurface->Release();

    // Now create new surfaces and Reset the sucker:

    pDev->CreateImageSurface(640, 480, D3DFMT_LIN_X8R8G8B8, &pColorSurface1);
    pDev->CreateImageSurface(640, 480, D3DFMT_LIN_X8R8G8B8, &pColorSurface2);
    pDev->CreateImageSurface(640, 480, D3DFMT_LIN_D24S8, &pZSurface);

    d3dpp.BackBufferFormat          = D3DFMT_LIN_X8R8G8B8;
    d3dpp.MultiSampleType           = 0;
    d3dpp.BufferSurfaces[0]         = pColorSurface1;
    d3dpp.BufferSurfaces[1]         = pColorSurface2;
    d3dpp.DepthStencilSurface       = pZSurface;

    pDev->Reset(&d3dpp);

    pDev->Release();

    pColorSurface1->Release();
    pColorSurface2->Release();
    pZSurface->Release();
}

//------------------------------------------------------------------------------
void TestLargePushes(D3DDevice* pDev)
{
    DWORD i;

    D3DPushBuffer* pPushBuffer;

    pDev->CreatePushBuffer(136*1024, TRUE, &pPushBuffer);
    pDev->BeginPushBuffer(pPushBuffer);

    for (i = 0; i < 128*1024/8; i++)
    {
        pDev->Nop();
    }

    HRESULT hresult = pDev->EndPushBuffer();
    ASSERT(hresult == S_OK);

    for (i = 0; i < 256; i++)
    {
        pDev->Clear(0, NULL, D3DCLEAR_TARGET_R | D3DCLEAR_ZBUFFER, 0, 0, 0);
        pDev->Clear(0, NULL, D3DCLEAR_TARGET_R | D3DCLEAR_ZBUFFER, 0, 0, 0);
        pDev->Clear(0, NULL, D3DCLEAR_TARGET_R | D3DCLEAR_ZBUFFER, 0, 0, 0);
        pDev->Clear(0, NULL, D3DCLEAR_TARGET_R | D3DCLEAR_ZBUFFER, 0, 0, 0);
        pDev->Clear(0, NULL, D3DCLEAR_TARGET_R | D3DCLEAR_ZBUFFER, 0, 0, 0);
        pDev->RunPushBuffer(pPushBuffer, NULL);
        pDev->RunPushBuffer(pPushBuffer, NULL);
        pDev->RunPushBuffer(pPushBuffer, NULL);
        pDev->RunPushBuffer(pPushBuffer, NULL);
    }

    pPushBuffer->Release();
}

//------------------------------------------------------------------------------
void __cdecl main()
{
    // Before anything else, test for leaks:

    TestLeaks();

    // Put tests here that have to create their own D3D objects:

    TestCreateDeviceAndReset();

    // Put tests here that can inherit the pre-built D3D object (via pDev):

    D3DDevice* pDev = InitD3D();

    for (DWORD pass = 0; pass < 2; pass++)
    {
    #if DBG
        D3D__Parser = pass;
    #endif

        TestLargePushes(pDev);
        TestPushBuffers(pDev);
        TestTiles(pDev);
        TestScissors(pDev);
        TestPeristentAttributes(pDev);
    }

    // We're done:

    DbgPrint("\n>> D3D Regress: Done.  Success!\n");

    while (TRUE)
        ;
}
