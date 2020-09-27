/*--
Copyright (c) 1999 - 2000 Microsoft Corporation - Xbox SDK

Module Name:

    Main.cpp

Abstract:

    Complete DX8 application to draw a screen space triangle.

Revision History:

    Derived from a DX8 sample.
--*/
#include <SDKCommon.h>

#define CheckHR(x) if ((x) != S_OK) _asm int 3;

bool                       bQuit   = false;
IDirect3DDevice8*          pDev  = NULL;
IDirect3DVertexBuffer8*    pVB   = NULL;

extern "C"
{
    BOOL _fltused;
}

//------------------------------------------------------------------------------
bool InitD3D
(
    HWND hWnd
)
//--------------------------------------
{
    static int doneOnce; 
    if (!doneOnce)       
    {                    
        { ; }  
        doneOnce=1;      
    }                    

    // Create D3D 8.
    IDirect3D8 *pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    if (pD3D == NULL)
        return false;

    // Set the screen mode.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth           = 640;
    d3dpp.BackBufferHeight          = 480;
    d3dpp.BackBufferFormat          = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount           = 1;
    d3dpp.Windowed                  = false;   // Must be false for Xbox.
    d3dpp.EnableAutoDepthStencil    = true;
    d3dpp.AutoDepthStencilFormat    = D3DFMT_UNKNOWN_D24S8;
    d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_RefreshRateInHz= 60;
    d3dpp.hDeviceWindow             = hWnd;

    // Create the device.
    if (pD3D->CreateDevice
    (
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &pDev
    ) != S_OK)
    {
        return false;
    }

    // Now we no longer need the D3D interface so lets free it.
    pD3D->Release();

    return true;
}

static struct { float x,y,z,w; DWORD color; } Vertices0[] =
{
    {   0,   0, 0.5f, 1, 0xff000000 },
    {   0, 480, 0.5f, 1, 0xffff0000 },
    { 640,   0, 0.5f, 1, 0xff00ff00 },
    { 640, 480, 0.5f, 1, 0xff0000ff }
};

static struct { float x,y,z,w; DWORD color; float u,v; } Vertices[] =
{
    {   0,   0, 0.5f, 1, 0xffffffff, 0.25f, 0.25f },
    {   0, 480, 0.5f, 1, 0xffffffff, 0.25f, 0.75f },
    {   0,   0,    0, 0,          0,     0,     0 },
    { 640,   0, 0.5f, 1, 0xffffffff, 0.75f, 0.25f },
    { 640, 480, 0.5f, 1, 0xffffffff, 0.75f, 0.75f }
};

static short Indices[] =
{
    0, 1, 3, 1, 3, 4
};


//------------------------------------------------------------------------------
void Paint
(
)
//--------------------------------------
{
    IDirect3DSurface8* pBackBuffer;
    IDirect3DVertexBuffer8* pVertexBuffer;
    IDirect3DIndexBuffer8* pIndexBuffer;
    IDirect3DTexture8* pTexture;
    D3DLOCKED_RECT lockData;
    DWORD* pdwData;
    BYTE* pbData;
    INT i;

    CheckHR(pDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0X00ffffff, 1.0, 0));

    CheckHR(pDev->CreateVertexBuffer(sizeof(Vertices), D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1,
            D3DUSAGE_WRITEONLY, D3DPOOL_DEFAULT, &pVertexBuffer));

    CheckHR(pVertexBuffer->Lock(0, sizeof(Vertices), &pbData, 0));
    memcpy(pbData, Vertices, sizeof(Vertices));
    CheckHR(pVertexBuffer->Unlock());

    CheckHR(pDev->CreateTexture(2, 2, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture));

    CheckHR(pTexture->LockRect(0, &lockData, NULL, 0));

    pdwData = (DWORD*) lockData.pBits;
    *(pdwData)     = 0xffff0000;
    *(pdwData + 1) = 0xff00ff00;
    pdwData = (DWORD*) ((BYTE*) pdwData + lockData.Pitch);
    *(pdwData)     = 0xff0000ff;
    *(pdwData + 1) = 0xff000000;

    CheckHR(pTexture->UnlockRect(0));

    CheckHR(pDev->SetTexture(0, pTexture));

    CheckHR(pDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR));

    CheckHR(pDev->SetVertexShader(D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1));

    CheckHR(pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));

    CheckHR(pDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 
                                         0, // MinVertexIndex
                                         5, // NumVertices
                                         2, // PrimitiveCount
                                         &Indices, // pIndexData
                                         D3DFMT_INDEX16, // IndexDataFormat
                                         Vertices,
                                         sizeof(Vertices[0])));

    pDev->Present(NULL, NULL, NULL, NULL);

    pTexture->Release();

    pVertexBuffer->Release();
}

#if !defined(XBOX)

//------------------------------------------------------------------------------
LRESULT WINAPI MessageHandler
(
    HWND    hWnd,
    UINT    Msg,
    WPARAM  wParam,
    LPARAM  lParam
)
//--------------------------------------
{
    if (Msg == WM_DESTROY)
        PostQuitMessage(0);

    return DefWindowProc(hWnd, Msg, wParam, lParam);
}

//------------------------------------------------------------------------------
int APIENTRY WinMain
(
    HINSTANCE   hInstance,
    HINSTANCE   hPrevInstance,
    LPSTR       lpCmdLine,
    int         nCmdShow
)
//--------------------------------------
{
    MSG msg;

    WNDCLASSEX classex;
    ZeroMemory( &classex, sizeof(classex) );

    classex.cbSize          = sizeof(WNDCLASSEX);
    classex.style           = CS_CLASSDC;
    classex.lpfnWndProc     = MessageHandler;
    classex.hInstance       = hInstance;
    classex.lpszClassName   = L"XBOX";
    RegisterClassEx(&classex);

    HWND hwnd = CreateWindowEx(WS_EX_TOPMOST, L"XBOX", L"XBOX Basic Shell",
                          WS_VISIBLE|WS_POPUP,
                          CW_USEDEFAULT,CW_USEDEFAULT, 0, 0,
                          NULL, NULL, hInstance, NULL);

    if (!InitD3D(hwnd))
    {
        return EXIT_FAILURE;
    }

    ShowCursor(false);

    while (!bQuit)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (!GetMessage(&msg, NULL, 0, 0))
            {
                bQuit = true;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);

            Paint();
        }
    }

    return EXIT_SUCCESS;
}

#else

extern "C"
DWORD
WINAPI
TestMain()
{
    _asm nop;
    _asm int 3;
    _asm nop;

    if (!InitD3D(NULL))
    {
        return(0);
    }

    while (TRUE)
    {
        Paint();
    }

    return(TRUE);
}

#endif // defined(XBOX)
