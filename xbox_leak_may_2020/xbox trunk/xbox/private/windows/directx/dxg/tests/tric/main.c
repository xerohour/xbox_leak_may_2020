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
#define _USE_XGMATH
#include <SDKCommon.h>
#include <d3dx8.h>
#else // XBOX
#include <windows.h>
#include <d3d8.h>
#endif // XBOX

#include <stdio.h>

#define CheckHR(x) { HRESULT _hr = (x); if (FAILED(_hr)) { _asm { int 3 } } }

BOOL                       bQuit   = FALSE;
IDirect3DDevice8*          pDev  = NULL;
DWORD                      dwFVF = D3DFVF_XYZRHW|D3DFVF_DIFFUSE;
DWORD                      dwCount;

#define LOG_SIZE 400

DWORD g_i;
DWORD g_Log[LOG_SIZE + 20];
BOOL g_RecordLog = TRUE;

//------------------------------------------------------------------------------
// Define our screen space triangle.
static struct { float x,y,z,w; DWORD color; } Verts[] =
{
    {320.0f,  50.0f, 0.5f, 1.0f, 0xffff0000, },
    {620.0f, 430.0f, 0.5f, 1.0f, 0xff00ff00, },
    { 20.0f, 430.0f, 0.5f, 1.0f, 0xff0000ff, },
};

//-----------------------------------------------------------------------------
DWORD GetTime()                                                                
{                                                                              
    _asm rdtsc                                                                 
}                                                                              

//-----------------------------------------------------------------------------
void __cdecl SwapCallback(D3DSWAPDATA* pData)
{
    g_Log[g_i++] = GetTime();
    g_Log[g_i++] = 0;

    memcpy(&g_Log[g_i], pData, sizeof(*pData));
    g_i += sizeof(*pData)/4;

    if (g_i > LOG_SIZE)
        _asm int 3;
}

//-----------------------------------------------------------------------------
void __cdecl VBlankCallback(D3DVBLANKDATA* pData)
{
    g_Log[g_i++] = GetTime();
    g_Log[g_i++] = 1;

    memcpy(&g_Log[g_i], pData, sizeof(*pData));
    g_i += sizeof(*pData)/4;

    if (g_i > LOG_SIZE)
        _asm int 3;
}

//-----------------------------------------------------------------------------
void PrintLog()
{
    CHAR charBuf[200];
    DWORD i = 0;

    while (i < g_i)
    {
        DWORD time = g_Log[i++];
        DWORD type = g_Log[i++];

        if (type == 0)
        {
            D3DSWAPDATA* pData = (D3DSWAPDATA*) &g_Log[i];
            i += sizeof(*pData)/4;

            sprintf(
                charBuf,
                "Swap -- Swap: %li  VBlank: %li  Missed: %li  Until: %li  Between: %li  Time: %lx\n",
                pData->Swap, pData->SwapVBlank, pData->MissedVBlanks, pData->TimeUntilSwapVBlank, 
                pData->TimeBetweenSwapVBlanks, time + pData->TimeUntilSwapVBlank);
        }
        else
        {
            D3DVBLANKDATA* pData = (D3DVBLANKDATA*) &g_Log[i];
            i += sizeof(*pData)/4;

            sprintf(
                charBuf,
                "VBlank - VBlank: %li  Swap: %li  Flags: %lx  Time: %lx\n",
                 pData->VBlank, pData->Swap, pData->Flags, time);
        }

        OutputDebugStringA(charBuf);
    }
}

//------------------------------------------------------------------------------
BOOL InitD3D(HWND hWnd)
{
    D3DPRESENT_PARAMETERS d3dpp;
    DWORD startTime;
    DWORD initializationTime;
    IDirect3D8 *pD3D;
    CHAR stringBuffer[200];
     
    startTime = timeGetTime();

    // Create D3D 8.
    pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    if (pD3D == NULL)
        return FALSE;

    // Set the screen mode.
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth                 = 640;
    d3dpp.BackBufferHeight                = 480;
    d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount                 = 2;
    d3dpp.Windowed                        = FALSE;   // Must be FALSE for Xbox.
    d3dpp.EnableAutoDepthStencil          = TRUE;
    d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
    d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                   = hWnd;
    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_TWO;

    // Create the device.

    if (IDirect3D8_CreateDevice
    (
        pD3D,
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &pDev
    ) != S_OK)
    {
        return FALSE;
    }

    // Now we no longer need the D3D interface so lets free it.
    IDirect3D8_Release(pD3D);

    // Clear the frame buffer, Zbuffer.
    // CheckHR( IDirect3DDevice8_Clear(pDev, 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0X00ff0000, 1.0, 0) );
    // CheckHR( IDirect3DDevice8_Present(pDev, NULL, NULL, NULL, NULL) );

    IDirect3DDevice8_BlockUntilIdle(pDev);

    initializationTime = timeGetTime() - startTime;

    sprintf(stringBuffer, "TriC: Total D3D initialization time: %lims\n", initializationTime);

    OutputDebugStringA(stringBuffer);

    return TRUE;
}

//------------------------------------------------------------------------------
#define FADE_LENGTH 1000

void SetRamp(int coefficient)
{
    int i, value;
    D3DGAMMARAMP ramp;

    for (i = 0; i < 256; i++) {
        value = i * coefficient / FADE_LENGTH;
        ramp.red[i] = (BYTE)value;
        ramp.green[i] = (BYTE)value;
        ramp.blue[i] = (BYTE)value;
    }

    IDirect3DDevice8_SetGammaRamp(pDev, D3DSGR_NO_CALIBRATION | D3DSGR_IMMEDIATE, &ramp);
}

//------------------------------------------------------------------------------
void Fade()
{
    static int startTime;
    static BOOL fFadeIn = FALSE, fStart = FALSE, fWait = FALSE;
    int delta;

    if (!fStart) {
        startTime = GetTickCount();
        fStart = TRUE;
    }

    delta = GetTickCount() - startTime;

    if (fWait) {

        if (delta >= FADE_LENGTH) {
            fStart = FALSE;
            fWait = FALSE;
        }

    } else if (fFadeIn) {

        if (delta >= FADE_LENGTH) {
            SetRamp(FADE_LENGTH);
            fFadeIn = FALSE;
            fStart = FALSE;
            fWait = TRUE;
        } else {
            SetRamp(delta);
        }

    } else {

        if (delta >= FADE_LENGTH) {
            SetRamp(0);
            fFadeIn = TRUE;
            fStart = FALSE;
            fWait = TRUE;
        } else {
            SetRamp(FADE_LENGTH - delta);
        }
    }
}

//------------------------------------------------------------------------------
void DrawVerticesUP()
{
    static int iteration;

    DWORD count = 3;
    DWORD stride = sizeof(Verts[0]);

    if (iteration++ & 1)
    {
        CheckHR( IDirect3DDevice8_DrawVerticesUP(pDev, 
                                                 D3DPT_TRIANGLELIST,
                                                 count,                // VertexCount
                                                 Verts,                // pVertexStreamZeroData
                                                 stride) );            // VertexZeroStreamStride
    }
    else
    {
        DWORD* pPush;
        DWORD dwords = stride * count;

        // The "+ 5" is to reserve enough overhead for the encoding parameters.
        // It can safely be more, but not less.

        CheckHR( IDirect3DDevice8_BeginPush(pDev, dwords + 5, &pPush) );

        pPush[0] = D3DPUSH_ENCODE(D3DPUSH_SET_BEGIN_END, 1);
        pPush[1] = D3DPT_TRIANGLELIST;

        // NOTE: A maximum of 2047 dwords can be specified to D3DPUSH_ENCODE.
        //       If there is more than 2047 dwords of vertex data, simply split
        //       the data into multiple D3DPUSH_ENCODE(D3DPUSH_INLINE_ARRAY) 
        //       sections.

        pPush[2] = D3DPUSH_ENCODE(D3DPUSH_NOINCREMENT_FLAG | D3DPUSH_INLINE_ARRAY, dwords);
        pPush += 3;

        memcpy(pPush, Verts, 4*dwords);
        pPush += dwords;

        pPush[0] = D3DPUSH_ENCODE(D3DPUSH_SET_BEGIN_END, 1);
        pPush[1] = 0;
        pPush += 2;

        CheckHR( IDirect3DDevice8_EndPush(pDev, pPush) );
    }
}

//------------------------------------------------------------------------------
void Paint()
{
    static DWORD count;

    count++;
    if (count == 1)
    {
        IDirect3DDevice8_SetVerticalBlankCallback(pDev, VBlankCallback);
        IDirect3DDevice8_SetSwapCallback(pDev, SwapCallback);
    }

    if (count == 15)
        Sleep(160);

    if (count == 20)
    {
        IDirect3DDevice8_BlockUntilIdle(pDev);
        IDirect3DDevice8_SetVerticalBlankCallback(pDev, NULL);
        IDirect3DDevice8_SetSwapCallback(pDev, NULL);
        PrintLog();
    }

    CheckHR( IDirect3DDevice8_Clear(pDev, 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0X00404040, 1.0, 0) );

    CheckHR( IDirect3DDevice8_SetVertexShader(pDev, dwFVF) );

    CheckHR( IDirect3DDevice8_SetRenderState(pDev, D3DRS_CULLMODE, D3DCULL_NONE) );

    DrawVerticesUP();

    Fade();

    CheckHR( IDirect3DDevice8_Present(pDev, NULL, NULL, NULL, NULL) );
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

            Paint();
        }
    }

    return EXIT_SUCCESS;
}

#else

//-----------------------------------------------------------------------------

void __cdecl main()
{
//    _asm int 3;

    BOOL Toggle = FALSE;
    DWORD Time = GetTickCount();
    D3DPRESENT_PARAMETERS d3dpp;
    IDirect3DSurface8 *pSurface;

    if (!InitD3D(NULL))
    {
        return;
    }

    D3DDevice_GetPersistedSurface(&pSurface);

    while (TRUE)
    {
        Paint();
    
        if (GetTickCount() - Time > 2000)
        {
            // Set the screen mode.
            ZeroMemory(&d3dpp, sizeof(d3dpp));

            d3dpp.BackBufferWidth                 = 640;
            d3dpp.BackBufferHeight                = 480;
            d3dpp.BackBufferFormat                = Toggle ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;
            d3dpp.BackBufferCount                 = 1;
            d3dpp.Windowed                        = FALSE;   // Must be FALSE for Xbox.
            d3dpp.EnableAutoDepthStencil          = TRUE;
            d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
            d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
            d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

            Toggle = !Toggle;
            Time = GetTickCount();

            D3DDevice_PersistDisplay();
            D3DDevice_Reset(&d3dpp);
        }
    }
}

#endif // defined(XBOX)
