#ifdef XBOX
#include <xtl.h>
#else // XBOX
#include <windows.h>
#include <d3d8.h>
#include <d3dx8.h>
#endif // XBOX

DWORD dwFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
struct { float x,y,z; DWORD color; } Verts[] =
{
    { -1.0f, -1.0f, 0.0f, 0xffff0000, },
    {  0.0f,  1.0f, 0.0f, 0xff00ff00, },
    {  1.0f, -1.0f, 0.0f, 0xff0000ff, },
};

BOOL g_bUseWBuffer = 1;
D3DXVECTOR3 g_vEye;
IDirect3DDevice8* g_pDev;

void Initialize()
{
    D3DXMATRIX matWorld, matView, matProj;

    D3DXMatrixIdentity( &matWorld );
    g_pDev->SetTransform( D3DTS_WORLD, &matWorld );

    g_vEye = D3DXVECTOR3( 0.0f, 0.0f, -1.5f );
    D3DXVECTOR3 vAt( 0.0f, 0.0f, 1.0f );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );

    D3DXMatrixLookAtLH( &matView, &g_vEye, &vAt, &vUp );
    g_pDev->SetTransform( D3DTS_VIEW, &matView );

    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_pDev->SetTransform( D3DTS_PROJECTION, &matProj );
}

void FrameMove()
{
    D3DXMATRIX matRotate, matView;
    D3DXVECTOR3 axis( 1.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vAt( 0.0f, 0.0f, 1.0f );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );

    D3DXMatrixRotationAxis( &matRotate, &axis, 0.0006f );
    D3DXVec3TransformCoord( &g_vEye, &g_vEye, &matRotate );

    D3DXMatrixLookAtLH( &matView, &g_vEye, &vAt, &vUp );

    g_pDev->SetTransform( D3DTS_VIEW, &matView );
}

void Render()
{
    g_pDev->BeginScene();

    g_pDev->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00ffffff, 1.0f, 0L );

    g_pDev->SetRenderState( D3DRS_LIGHTING, FALSE );
    g_pDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	if (g_bUseWBuffer)
		g_pDev->SetRenderState( D3DRS_ZENABLE, D3DZB_USEW );
	else
		g_pDev->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

    g_pDev->SetVertexShader(dwFVF);
    g_pDev->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 3, Verts, sizeof(Verts[0]) );
    g_pDev->EndScene();

    g_pDev->Present( NULL, NULL, NULL, NULL );
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
        &g_pDev) != S_OK)
    {
        return FALSE;
    }

    // Now we no longer need the D3D interface so lets free it.
    IDirect3D8_Release(pD3D);

    Initialize();

    return TRUE;
}

#ifdef XBOX

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

