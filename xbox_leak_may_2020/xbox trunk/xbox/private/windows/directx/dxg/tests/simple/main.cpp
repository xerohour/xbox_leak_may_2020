//  RECUI.CPP
//
//      Harddisk recovery UI
//
//  Created 12-Aug-2000 [jont]

#include <stdio.h>
#ifdef XBOX
#include <SDKCommon.h>
#else // XBOX
#include <windows.h>
#include <d3d8.h>
#include <d3dx8.h>
#endif // XBOX

#include "bitfont.h"

extern "C" ULONG DebugPrint(PCHAR Format, ...);

extern "C" { extern BOOL D3D__Parser; }

#define CheckHR(x) { HRESULT _hr = (x); if (FAILED(_hr)) { DisplayError(L#x, _hr); } }

#define BACKGROUND_COLOR 0x00000000

//  Globals
IDirect3DDevice8* g_pdev = NULL;
IDirect3DSurface8* g_pback = NULL;
IDirect3DVertexBuffer8* g_pvb = NULL;
IDirect3DVertexBuffer8* g_pvbBackVerts = NULL;
IDirect3DVertexBuffer8* g_pvbBackColors = NULL;
DWORD g_dwFrames = 0;
DWORD g_dwCounter = 0;
BOOL g_Clockwise = TRUE;
D3DSURFACE_DESC g_BackDesc;
float g_fSecsPerTick;

D3DXMATRIX  matView;
D3DXMATRIX  matInverseView;
D3DXMATRIX  matProjectionViewport;
D3DXMATRIX  matComposite;

BitFont m_Font;

// Define our vertices

typedef struct
{
    float x, y, z;
    float nx, ny, nz;
    DWORD color;
} BASIC_VERT;

#define FVF_BASIC_VERT (D3DFVF_XYZ  | D3DFVF_NORMAL | D3DFVF_DIFFUSE)

BASIC_VERT Verts[] =
{
    {  0.0f, 2.0f, 2.0f, 0.0f, 1.0f, -2.0f, 0xffff0000 },
    {  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, -2.0f, 0xff00ff00 },
    { -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, -2.0f, 0xff0000ff },
};

DWORD PassThroughProgram[] =
{
    0x000a2078,
    0x00000000, 0x0020001b, 0x0836086c, 0x1f000ff8,
    0x00000000, 0x0400001b, 0x04360bfc, 0x10017ff8,
    0x00000000, 0x0040001b, 0x0436186c, 0x1f000ff8,
    0x00000000, 0x0060201b, 0x0436086c, 0x3070f800,
    0x00000000, 0x0020061b, 0x0836086c, 0x1070f818,
    0x00000000, 0x0020081b, 0x0836086c, 0x1070f820,
    0x00000000, 0x002008ff, 0x0836086c, 0x1070f828,
    0x00000000, 0x00200200, 0x0836086c, 0x1070f830,
    0x00000000, 0x0020121b, 0x0836086c, 0x1070f848,
    0x00000000, 0x0020141b, 0x0836086c, 0x1070f850,
};

DWORD BadStateProgram[] =
{
    0x00017378,
    0xdeadbeef, 0xdeadbeef, 0xdeadbeef, 0xdeadbeef, 
};

//------------------------------------------------------------------------------
// Define our background triangle.

static struct { float x,y,z,w; } BackVerts[] =
{
    {320.0f,   0.0f, 0.5f, 1.0f},
    {527.0f, 360.0f, 0.5f, 1.0f},
    {113.0f, 360.0f, 0.5f, 1.0f},
};

static struct { DWORD skip,color; } BackColors[] =
{
    {0xdeadbeef,0xffffffff}, 
    {0xdeadbeef,0xffff0000}, 
    {0xdeadbeef,0xff0000ff}, 
};

//  GetTime
//      Returns an accurate time in seconds

float GetTime(void)
{
    LARGE_INTEGER qwTime;

    QueryPerformanceCounter(&qwTime);
    return ((float)qwTime.QuadPart) * g_fSecsPerTick;
}


//  DisplayError
//      Helper to display error messages.  This just shoves them out through the
//      debug console.

void DisplayError(const WCHAR* szCall, HRESULT hr)
{
    WCHAR szErr[512];
    unsigned cch;

    if (hr)
    {
        cch = _snwprintf(szErr, sizeof(szErr), L"FATAL: %s failed 0x%08lX: ", szCall, hr);
        D3DXGetErrorString(hr, &szErr[cch], sizeof (szErr));
    }
    else
    {
        wcscpy(szErr, szCall);
    }

    wcscat(szErr, L"\n");

    OutputDebugString(szErr);

    _asm int 3;
}


// InitLight
//      Initializes a D3DLIGHT structure, setting the light position. The
//      diffuse color is set to white, specular and ambient left as black

VOID InitLight(D3DLIGHT8& light, D3DLIGHTTYPE ltType,
    float x, float y, float z)
{
    ZeroMemory(&light, sizeof(D3DLIGHT8));
    light.Type = ltType;
    light.Diffuse.r = 1.0f;
    light.Diffuse.g = 1.0f;
    light.Diffuse.b = 1.0f;
    light.Position.x = light.Direction.x = x;
    light.Position.y = light.Direction.y = y;
    light.Position.z = light.Direction.z = z;
    light.Range = 1000.0f;
}

//  InitD3D

BOOL InitD3D(HWND hWnd)
{
#if DBG
    D3D__Parser = TRUE;
#endif

    // Create D3D 8.
    IDirect3D8 *pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    if (pD3D == NULL)
        return FALSE;

    // Set the screen mode.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof (d3dpp));

    d3dpp.BackBufferWidth           = 640;
    d3dpp.BackBufferHeight          = 480;
    d3dpp.BackBufferFormat          = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount           = 1;
    d3dpp.Windowed                  = FALSE;   // Must be false for Xbox.
    d3dpp.EnableAutoDepthStencil    = TRUE;
    d3dpp.AutoDepthStencilFormat    = D3DFMT_D24S8;
    d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_RefreshRateInHz= 60;
    d3dpp.hDeviceWindow             = hWnd;
    d3dpp.Flags                     = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    // Create the device.
    if (pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
            D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pdev) != S_OK)
    {
        OutputDebugString(L"Couldn't create device!\n");
        return FALSE;
    }

    // Now we no longer need the D3D interface so lets free it.
    pD3D->Release();

    // Store render target surface descriptor

    CheckHR(g_pdev->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &g_pback));

    CheckHR(g_pback->GetDesc(&g_BackDesc));

    // Set world transform
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity(&matWorld);
    CheckHR(g_pdev->SetTransform(D3DTS_WORLD, &matWorld));

    // Set the view matrix
    D3DXMATRIX  matInverseViewTranspose;
    D3DXVECTOR3 vEyePt      = D3DXVECTOR3(0.0f, 1.0f, -5.0f);
    D3DXVECTOR3 vLookatPt   = D3DXVECTOR3(0.0f, 1.0f,  0.0f);
    D3DXVECTOR3 vUpVec      = D3DXVECTOR3(0.0f, 1.0f,  0.0f);
    D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
    CheckHR(g_pdev->SetTransform(D3DTS_VIEW, &matView));

    g_pdev->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE); 

    // Set projection transform
    D3DXMATRIX matProj;
    float fAspect = ((float)g_BackDesc.Height) / g_BackDesc.Width;

    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 3, fAspect, 1.0f, 10000.0f);
    CheckHR(g_pdev->SetTransform(D3DTS_PROJECTION, &matProj));

    // Set the model-view:
    CheckHR(g_pdev->GetProjectionViewportMatrix(&matProjectionViewport));
    D3DXMatrixInverse(&matInverseView, NULL, &matView);
    D3DXMatrixMultiply(&matComposite, &matView, &matProjectionViewport);
    CheckHR(g_pdev->SetModelView(&matView, &matInverseView, &matComposite));

    // Set default render states

    CheckHR(g_pdev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
    CheckHR(g_pdev->SetRenderState(D3DRS_DITHERENABLE, TRUE));
    CheckHR(g_pdev->SetRenderState(D3DRS_SPECULARENABLE, FALSE));
    CheckHR(g_pdev->SetRenderState(D3DRS_ZENABLE, FALSE));
    CheckHR(g_pdev->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE));

    // Set up the lighting states

    D3DMATERIAL8 material;
    ZeroMemory(&material, sizeof(material));
    material.Ambient.r = 1.0f;
    material.Ambient.g = 1.0f;
    material.Ambient.b = 1.0f;
    CheckHR(g_pdev->SetMaterial(&material));
    CheckHR(g_pdev->SetBackMaterial(&material));

    D3DLIGHT8 light;
    InitLight(light, D3DLIGHT_DIRECTIONAL, 0.0f, -1.0f, 1.0f);
    CheckHR(g_pdev->SetLight(0, &light));
    CheckHR(g_pdev->LightEnable(0, TRUE));
    CheckHR(g_pdev->SetRenderState(D3DRS_LIGHTING, TRUE));
    CheckHR(g_pdev->SetRenderState(D3DRS_TWOSIDEDLIGHTING, TRUE));
    CheckHR(g_pdev->SetRenderState(D3DRS_BACKAMBIENT, 0x33333333));
    CheckHR(g_pdev->SetRenderState(D3DRS_AMBIENT, 0xffff2020));

    return TRUE;
}


//  InitVB

BOOL InitVB(void)
{
    void* pVerts;
    void* pBackVerts;
    void* pBackColors;

    CheckHR(g_pdev->CreateVertexBuffer(sizeof (Verts), D3DUSAGE_WRITEONLY, 0,
        0, &g_pvb));

    g_pvb->Lock(0, sizeof(Verts), (BYTE**)(&pVerts), 0);
    memcpy((void*)pVerts, (void*)Verts, sizeof (Verts));
    g_pvb->Unlock();

    CheckHR(g_pdev->CreateVertexBuffer(sizeof (BackVerts), D3DUSAGE_WRITEONLY, 0,
        0, &g_pvbBackVerts));

    g_pvbBackVerts->Lock(0, sizeof(BackVerts), (BYTE**)(&pBackVerts), 0);
    memcpy((void*)pBackVerts, (void*)BackVerts, sizeof (BackVerts));
    g_pvbBackVerts->Unlock();

    CheckHR(g_pdev->CreateVertexBuffer(sizeof (BackColors), D3DUSAGE_WRITEONLY, 0,
        0, &g_pvbBackColors));

    g_pvbBackColors->Lock(0, sizeof(BackColors), (BYTE**)(&pBackColors), 0);
    memcpy((void*)pBackColors, (void*)BackColors, sizeof (BackColors));
    g_pvbBackColors->Unlock();

    return TRUE;
}


//  Init2D

BOOL Init2D(void)
{
    // Set up the timer
    LARGE_INTEGER qwTicksPerSec;
    QueryPerformanceFrequency(&qwTicksPerSec);
    g_fSecsPerTick = 1.0f / (float)qwTicksPerSec.QuadPart;

    return TRUE;
}


//  DisplayText

void DisplayText(const WCHAR* sz, int iY, DWORD dwColor)
{
    m_Font.DrawText(g_pback, sz, 0, iY, DRAWTEXT_TRANSPARENTBKGND, dwColor, 0);
}


//  UpdateFrameRate

WCHAR* UpdateFrameRate(void)
{
    static float fLastTime = 0.0f;
    float fTime = GetTime();
    float fFPS;
    static WCHAR szFrameRate[256];

    // Update the scene stats once per second
    if (fTime - fLastTime > 1.0f)
    {
        fFPS = g_dwFrames / (fTime - fLastTime);

        swprintf(szFrameRate, L"%.2f fps (%dx%dx%d)", fFPS,
            g_BackDesc.Width, g_BackDesc.Height,
            g_BackDesc.Format == D3DFMT_X8R8G8B8 ? 32 : 16);

        // Reset
        fLastTime  = fTime;
        g_dwFrames = 0;
    }

    return szFrameRate;
}


//  Paint

#define TRIANGLES 12000
WORD indices[3 * TRIANGLES];

void Paint(void)
{
    DWORD get0[4];
    DWORD get1[4];
    DWORD clear[192][4];
    DWORD junk;
    DWORD hBackShader;
    DWORD declaration[] = 
    {
        D3DVSD_STREAM(0),
        D3DVSD_REG(0, D3DVSDT_FLOAT4),
        D3DVSD_STREAM(1),
        D3DVSD_SKIP(1),
        D3DVSD_REG(3, D3DVSDT_D3DCOLOR),
        D3DVSD_CONST(-96, 2),
        0x3f800000, 0x3f800000, 0x00000000, 0x3f800000,
        0x3f080000, 0x3f080000, 0x00000000, 0x00000000,
        D3DVSD_CONST(94, 2),
        0x3f800000, 0x3f800000, 0x00000000, 0x3f800000,
        0x3f080000, 0x3f080000, 0x00000000, 0x00000000,

        D3DVSD_END()
    };

    // Clear the frame buffer, Zbuffer.
    g_pdev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, BACKGROUND_COLOR, 1.0, 0);

    g_pdev->BeginScene();

    // Use a vertex shader program with all 192 constant registers to draw
    // the background triangle.
    CheckHR(g_pdev->SetShaderConstantMode(D3DSCM_192CONSTANTS));
    for (INT blah = 0; blah < 192; blah++)
    {
        clear[blah][0] = blah;
        clear[blah][1] = blah;
        clear[blah][2] = blah;
        clear[blah][3] = blah;
    }
    CheckHR(g_pdev->CreateVertexShader(NULL, BadStateProgram, &junk, 0));
    CheckHR(g_pdev->LoadVertexShader(junk, 0));
    CheckHR(g_pdev->DeleteVertexShader(junk));
    CheckHR(g_pdev->CreateVertexShader(declaration, NULL, &junk, 0));
    CheckHR(g_pdev->DeleteVertexShader(junk));
    CheckHR(g_pdev->SetVertexShaderConstant(-96, &clear[0][0], 58));   // Skip -38 and -37
    CheckHR(g_pdev->SetVertexShaderConstant(-36, &clear[60][0], 132));
    CheckHR(g_pdev->CreateVertexShader(declaration, PassThroughProgram, &hBackShader, 0));
    CheckHR(g_pdev->LoadVertexShader(hBackShader, 3));
    CheckHR(g_pdev->GetVertexShaderConstant(-96, get0, 1));
    CheckHR(g_pdev->GetVertexShaderConstant(94, get1, 1));
    if ((get0[0] != 0x3f800000) ||
        (get0[1] != 0x3f800000) ||
        (get0[2] != 0x00000000) ||
        (get0[3] != 0x3f800000) ||
        (get1[0] != 0x3f800000) ||
        (get1[1] != 0x3f800000) ||
        (get1[2] != 0x00000000) ||
        (get1[3] != 0x3f800000))
    {
        _asm int 3;
    }

    if ((g_dwCounter++ % 60) == 0)
        g_Clockwise = !g_Clockwise;

    CheckHR(g_pdev->SetRenderState(D3DRS_FRONTFACE, g_Clockwise ? D3DFRONT_CW : D3DFRONT_CCW));
    CheckHR(g_pdev->SelectVertexShader(hBackShader, 3));
    CheckHR(g_pdev->SetStreamSource(0, g_pvbBackVerts, sizeof(BackVerts[0])));
    CheckHR(g_pdev->SetStreamSource(1, g_pvbBackColors, sizeof(BackColors[0])));
    // CheckHR(g_pdev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1));

    for (DWORD i = 0; i < 3 * TRIANGLES; i += 3)
    {
        indices[i] = 0;
        indices[i + 1] = 1;
        indices[i + 2] = 2;
    }

    DWORD count = 0;
    while (TRUE)
    {
        CheckHR(g_pdev->DrawIndexedVertices(D3DPT_TRIANGLELIST, 3 * TRIANGLES, indices));
        count++;
        if (1) // ((count % 100) == 0)
        {
            CHAR buffer[200];
            sprintf(buffer, "Count: %li\n", count);
            OutputDebugStringA(buffer);
        }
    }

    // Draw the foreground triangle using stream 0.
    CheckHR(g_pdev->SetShaderConstantMode(D3DSCM_96CONSTANTS));
    CheckHR(g_pdev->SetModelView(&matView, &matInverseView, &matComposite));
    CheckHR(g_pdev->SetShaderConstantMode(D3DSCM_96CONSTANTS));
    CheckHR(g_pdev->SetStreamSource(0, g_pvb, sizeof (BASIC_VERT)));
    CheckHR(g_pdev->SetVertexShader(FVF_BASIC_VERT));
    CheckHR(g_pdev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1));
    CheckHR(g_pdev->DeleteVertexShader(hBackShader));

    // Draw some text
    DisplayText(UpdateFrameRate(), 0, 0xffffffff);

    g_pdev->EndScene();

    // Flip the buffers.
    g_dwFrames++;
    g_pdev->Present(NULL, NULL, NULL, NULL);
}


#if defined(XBOX)

extern "C" { extern BOOL D3D__SingleStepPusher; }

void __cdecl main()
{
    #if DBG
        D3D__SingleStepPusher = TRUE;
    #endif

    if (!InitD3D(NULL) || !InitVB() || !Init2D())
        return;

    while (TRUE)
    {
        Paint();
    }
}

#else

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
    BOOL bQuit = FALSE;

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

    if (!InitD3D(hwnd) || !InitVB() || !Init2D())
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
        }

        Paint();
    }

    return EXIT_SUCCESS;
}

#endif
