/*--

    Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

--*/

#ifdef XBOX
#include <xtl.h>
#else
#include <D3DX8.h>
#endif

#include <stdio.h>
#include <stdlib.h>

IDirect3DDevice8 *pDev = NULL;
IDirect3DVertexBuffer8 *pVB = NULL;
IDirect3DTexture8 *pTexture = NULL;

DWORD dwFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

#ifndef XBOX
HWND GetWindow(HINSTANCE hInstance)
{
    HWND hwnd;

    // Register the windows class
    WNDCLASS wndClass = {CS_CLASSDC, DefWindowProc, 0, 0, hInstance,
                        NULL, NULL, (HBRUSH)GetStockObject(WHITE_BRUSH),
                        NULL, L"Dummy Window"};

    RegisterClass( &wndClass );

    // Create the render window
    hwnd = CreateWindow(L"Dummy Window", NULL, WS_MINIMIZE, 0, 0, 1, 1, 0,
                        NULL, hInstance, 0);

    ShowWindow(hwnd, SW_SHOWNORMAL);
    ShowCursor(FALSE);

    return hwnd;
}
#endif

static struct TheVerts { float x,y,z,w; DWORD color; float u, v;} Verts[] =
{
    {113.0f,   0.0f, 0.5f, 1.0f, 0xffffffff, 0.0, 0.0},
    {527.0f, 360.0f, 0.5f, 1.0f, 0xffffffff, 1.0, 1.0},
    {113.0f, 360.0f, 0.5f, 1.0f, 0xffffffff, 0.0, 1.0},
    {527.0f,   0.0f, 0.5f, 1.0f, 0xffffffff, 1.0, 0.0},
    {527.0f, 360.0f, 0.5f, 1.0f, 0xffffffff, 1.0, 1.0},
    {113.0f,   0.0f, 0.5f, 1.0f, 0xffffffff, 0.0, 0.0},
};

void InitD3D(HWND hwnd)
{
    IDirect3D8 *pD3D = Direct3DCreate8(D3D_SDK_VERSION);

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.BackBufferWidth           = 640;
    d3dpp.BackBufferHeight          = 480;
    d3dpp.BackBufferFormat          = D3DFMT_R5G6B5;
    d3dpp.BackBufferCount           = 1;
    d3dpp.Windowed                  = false;
    d3dpp.EnableAutoDepthStencil    = true;
    d3dpp.AutoDepthStencilFormat    = D3DFMT_D16;
    d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow             = hwnd;
    d3dpp.Flags                     = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
            D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pDev);

    pD3D->Release();
}

void InitVB()
{
    void* pVerts;

    pDev->CreateVertexBuffer(sizeof(Verts), D3DUSAGE_WRITEONLY, dwFVF, D3DPOOL_MANAGED, &pVB);

    pVB->Lock(0, sizeof(Verts), (BYTE **)(&pVerts), 0);
    memcpy((void*)pVerts, (void*)Verts, sizeof(Verts));
    pVB->Unlock();

    pDev->SetStreamSource(0, pVB, sizeof(Verts[0]));
    pDev->SetVertexShader(dwFVF);
}

void Paint()
{
    TheVerts* pVerts;
    pVB->Lock(0, sizeof(Verts), (BYTE **)(&pVerts), 0);

    static DWORD Time = GetTickCount();
    DWORD CurrentTime = GetTickCount();

    // 10 seconds per rotation
    float spin = 2 * 3.14159f * (float)(CurrentTime - Time) / 10000.0f;
    DWORD i;

    for (i = 0; i < sizeof(Verts) / sizeof(Verts[1]); i++)
    {

        float x = Verts[i].x;
        float y = Verts[i].y;

        x = x - 320.0f;
        y = y - 240.0f;

        pVerts[i].x = x * (float)cos(spin) - y * (float)sin(spin);
        pVerts[i].y = x * (float)sin(spin) + y * (float)cos(spin);

        pVerts[i].x += 320.0f;
        pVerts[i].y += 240.0f;
    }

    pVB->Unlock();

    pDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0X00C0C0C0, 1.0, 0);

    pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

    pDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    pDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

    pDev->BeginScene();

    pDev->SetTexture(0, pTexture);

    pDev->DrawPrimitive( D3DPT_TRIANGLELIST, 0, (sizeof(Verts) / sizeof(Verts[0])) / 3);
    pDev->EndScene();

    pDev->Present(NULL, NULL, NULL, NULL);
}

#ifdef XBOX
void __cdecl main()
#else
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
#endif
{
#ifdef XBOX
    HWND hwnd = NULL;
#else
    HWND hwnd = GetWindow(hInst);
#endif

    InitD3D(hwnd);
    InitVB();

    D3DXSetDXT3DXT5(FALSE);

    D3DXCreateTextureFromFileA(pDev, "d:\\media\\dxt3.dds", &pTexture);

    while (TRUE)
    {
        Paint();
    }

#ifndef XBOX
    return 0;
#endif
}

