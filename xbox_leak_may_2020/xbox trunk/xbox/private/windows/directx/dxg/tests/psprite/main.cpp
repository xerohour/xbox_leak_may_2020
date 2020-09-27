/*--
Copyright (c) 1999 - 2000 Microsoft Corporation - Xbox SDK

Module Name:

    Main.cpp

Abstract:

    Complete DX8 application to draw a screen space triangle.

Revision History:

    Derived from a DX8 sample.
--*/

#ifdef XBOX
#include <xtl.h>
#else // XBOX
#include <windows.h>
#include <d3d8.h>
#include <d3dx8.h>
#endif // XBOX

const GUID DECLSPEC_SELECTANY IID_IUnknown =
{ 0x00000000, 0x0000, 0x0000, { 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };

#define timeGetTime GetTickCount
#define g_pd3dDevice pDev

IDirect3DDevice8* pDev  = NULL;

DWORD               g_dwVertexShaderHandle;  // Handle for the vertex shader
LPDIRECT3DTEXTURE8  g_pTexture = NULL;

struct CUSTOMVERTEX
{ 
    FLOAT x,y,z; 
    float size; 
};

CUSTOMVERTEX g_Vertices[] =
{
    { -0.6f, -1.0f, 0.0f, 10.1f, },
    {  0.0f,  1.0f, 0.0f, 30.5f, },
    {  0.6f, -1.0f, 0.0f, 60.9f, },
};

DWORD dwShaderVertexDecl[] =
{
    D3DVSD_STREAM( 0 ),
    D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),    // Position
    D3DVSD_REG( 1, D3DVSDT_FLOAT1 ),    // Point size
    D3DVSD_END()
};

BOOL InitApp()
{
#ifdef XBOX

    // Load the pre-compiled vertex shader microcode
    HANDLE hFile = CreateFileA( "D:\\media\\Shader.xvu", GENERIC_READ, FILE_SHARE_READ, 
                                NULL, OPEN_EXISTING, 
                                FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, NULL );
    if( hFile == INVALID_HANDLE_VALUE )
    {
        OutputDebugStringA( "ERROR: Could not load the vertex shader!\n" );
        return E_FAIL;
    }

    DWORD dwSize = GetFileSize( hFile, NULL );
    BYTE* pData  = new BYTE[dwSize];
    ReadFile( hFile, pData, dwSize, &dwSize, NULL );
    CloseHandle( hFile );

    HRESULT hr = g_pd3dDevice->CreateVertexShader( dwShaderVertexDecl,
                                                   (DWORD*)pData,
                                                   &g_dwVertexShaderHandle, 0 );

    delete pData;

#else // XBOX

    ID3DXBuffer *pShader;
    D3DXAssembleShaderFromFileA("shader.vsh", 0, NULL, &pShader, NULL);

    HRESULT hr = g_pd3dDevice->CreateVertexShader( dwShaderVertexDecl,
                                                   (DWORD*)pShader->GetBufferPointer(),
                                                   &g_dwVertexShaderHandle, 0 );

    pShader->Release();

#endif // XBOX

    if (FAILED(hr))
        return FALSE;

#ifdef XBOX
    D3DXCreateTextureFromFileA(g_pd3dDevice, "D:\\media\\wall.bmp", &g_pTexture);
#else // XBOX
    D3DXCreateTextureFromFileA(g_pd3dDevice, "wall.bmp", &g_pTexture);
#endif // XBOX

    if (g_pTexture == NULL)
        return FALSE;

    return TRUE;
}

VOID Render()
{
    // Clear the backbuffer and the zbuffer
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
                         D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
    
    // Begin the scene
    g_pd3dDevice->BeginScene();

    // Set state
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    g_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, true);
    g_pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE, true);

    float size = 40.0f;
    g_pd3dDevice->SetRenderState( D3DRS_POINTSIZE, *((DWORD*)&size));

#ifdef XBOX
    g_pd3dDevice->SetTexture(3, g_pTexture);

    g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLORARG2, D3DTA_CURRENT);
    g_pd3dDevice->SetTextureStageState(3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
#else // XBOX
    g_pd3dDevice->SetTexture(0, g_pTexture);

    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
#endif // XBOX

    // Draw the vertices using the vertex shader
    g_pd3dDevice->SetVertexShader( g_dwVertexShaderHandle );
    g_pd3dDevice->DrawPrimitiveUP( D3DPT_POINTLIST, 3, g_Vertices, 
                                   sizeof(g_Vertices[0]) );

    // End the scene
    g_pd3dDevice->EndScene();
    
    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

VOID FrameMove()
{
    // Rotate around the Y axis
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixRotationX( &matWorld, timeGetTime()/600.0f );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up.
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 0.0f,-3.0f ), 
                                  &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 
                                  &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIX matProj;
    float aspectRatio = 480.f / 640.f;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, aspectRatio, 1.0f, 800.0f );

    // Calculate concatenated World x ( View x Projection) matrix.
    // We transpose the matrix at the end because that's how matrix math
    // works in vertex shaders. (Because the vertex shader DP4 operator works on
    // rows, not on columns.)
    D3DXMATRIX mat;
    D3DXMatrixMultiply( &mat, &matView, &matProj );
    D3DXMatrixMultiply( &mat, &matWorld, &mat );
    D3DXMatrixTranspose( &mat, &mat );
    g_pd3dDevice->SetVertexShaderConstant( 0, &mat, 4 );
}

BOOL InitD3D(HWND hWnd)
{
    D3DPRESENT_PARAMETERS d3dpp;
    IDirect3D8 *pD3D;

    // Create D3D 8.
    pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    if (pD3D == NULL)
        return FALSE;

    // Set the screen mode.
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth                 = 640;
    d3dpp.BackBufferHeight                = 480;
    d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount                 = 1;
    d3dpp.Windowed                        = FALSE;   // Must be FALSE for Xbox.
    d3dpp.EnableAutoDepthStencil          = TRUE;
    d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
    d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                   = hWnd;
    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Create the device.

    if (IDirect3D8_CreateDevice(
        pD3D,
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &pDev) != S_OK)
    {
        return FALSE;
    }

    // Now we no longer need the D3D interface so lets free it.
    IDirect3D8_Release(pD3D);

    return InitApp();
}

#ifdef XBOX

//-----------------------------------------------------------------------------

void __cdecl main()
{
    if (!InitD3D(NULL))
    {
        return;
    }

    while (TRUE)
    {
        FrameMove();
        Render();
    }
}

#else // XBOX

LRESULT WINAPI MessageHandler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (Msg == WM_DESTROY)
        PostQuitMessage(0);

    return DefWindowProc(hWnd, Msg, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    BOOL bQuit = FALSE;
    MSG msg;
    HWND hwnd;

    WNDCLASSEX classex;
    ZeroMemory( &classex, sizeof(classex) );

    classex.cbSize          = sizeof(WNDCLASSEX);
    classex.style           = CS_CLASSDC;
    classex.lpfnWndProc     = MessageHandler;
    classex.hInstance       = hInstance;
    classex.lpszClassName   = L"XBOX";
    RegisterClassEx(&classex);

    hwnd = CreateWindowEx(WS_EX_TOPMOST, L"XBOX", L"XBOX Basic Shell",
                          WS_VISIBLE|WS_POPUP,
                          CW_USEDEFAULT,CW_USEDEFAULT, 0, 0,
                          NULL, NULL, hInstance, NULL);

    if (!InitD3D(hwnd))
    {
        return EXIT_FAILURE;
    }

    ShowCursor(FALSE);

    while (!bQuit)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (!GetMessage(&msg, NULL, 0, 0))
            {
                bQuit = TRUE;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        FrameMove();
        Render();
    }

    return EXIT_SUCCESS;
}

#endif // XBOX
