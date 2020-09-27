//-----------------------------------------------------------------------------
// FILE: PSPRITE.C
//
// Desc: Point sprite sample. based on Kilgards gl pointburst.c
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT

#ifdef XBOX
#include <xtl.h>
#else
#include <windows.h>
#include <mmsystem.h>
#include <D3DX8.h>
#endif
#include <stdio.h>
#include "D3DApp.h"
#include "D3DFile.h"
#include "D3DUtil.h"
#include "DXUtil.h"

#include <xgraphics.h>

#include "bitfont.h"

#ifndef XBOX
#undef CheckHR
#define CheckHR(x) { HRESULT _hr = (x); if (FAILED(_hr)) { _asm { int 3 } } }
#endif

/*
 * Globals and stuff
 */
IDirect3DDevice8    *pDev  = NULL;          // d3d device
IDirect3DTexture8   *g_pFloorTex = NULL;    // floor texture
IDirect3DTexture8   *g_pSpriteTex = NULL;   // sprite texture
LPDIRECT3DSURFACE8  g_pBackBuffer = NULL;   // our backbuffer

const float         g_fRadius = 8.0f;       // radius of floor disk
const int           g_cMaxPoints = 2001;    // maximum count of points
int                 g_numPoints = 500;      // current count of points
int                 g_clivepoints = 0;      // count of live points

float               g_angle = 0;            // view rotation around Y axis
float               g_time;                 // current time
bool                g_fpausetime = false;   // pause the particles
bool                g_fspin = true;         // I spin you right round baby...

HANDLE              g_hInpDevice = NULL;    // input handle

BitFont             g_bitfont;

static const float rgPointScale[3][3] =
{
    { 1/5.0f, 0.0, 0.0 },   // constant
    { 0.0, 1/5.0f, 0.0 },   // linear
    { 0.25f, 0.0, 1/60.0f } // quadratic
};
int g_iPointScale = 2;                      // defualt to quadratic

bool g_fPointScaleEnable = true;
float g_fPointSize = 10.0f;

// our point data
struct POINTDATA
{
    float time;
    float velocity[2];
    float direction[2];
} rgPointData[g_cMaxPoints];

// point rendering data
struct MYD3DPOINTVERTEX
{
    float sx, sy, sz;
    D3DCOLOR color;
} rgPoints[g_cMaxPoints];

// our floor
static const int g_cfloorverts = 20;
struct MYD3DTLVERTEX
{
    float    sx, sy, sz; // Screen coordinates
    DWORD    color;      // Vertex color
    float    tu, tv;     // Texture coordinates
} g_floorverts[g_cfloorverts];

// handle joystick input
void HandleInput();
BOOL InitD3D(HWND hWnd);
void DeinitD3D();

static float float_rand(void) { return rand() / (float)RAND_MAX; }
#define RANDOM_RANGE(lo, hi) ((lo) + (hi - lo) * float_rand())

// helper routine to convert floats to dwords for pointsize, etc.
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

//=========================================================================
// Create our points
//=========================================================================
void makePointList(void)
{
    static const D3DCOLOR colorSet[] =
    {
        D3DCOLOR_COLORVALUE(0.7f, 0.7f, 0.4f, 0.5f),
        D3DCOLOR_COLORVALUE(0.7f, 0.7f, 0.4f, 0.5f),
        D3DCOLOR_COLORVALUE(0.8f, 0.8f, 0.7f, 0.5f),
        D3DCOLOR_COLORVALUE(1.0f, 1.0f, 0.0f, 0.5f),
        D3DCOLOR_COLORVALUE(0.9f, 0.9f, 0.6f, 0.5f),
        D3DCOLOR_COLORVALUE(1.0f, 1.0f, 0.0f, 0.5f),
        D3DCOLOR_COLORVALUE(1.0f, 1.0f, 0.5f, 0.5f),
    };
    const int cColorSets = sizeof(colorSet) / sizeof(colorSet[0]);
    static const float fMEAN_VELOCITY = 3.0f;

    for(int i = 0; i < g_numPoints; i++)
    {
        rgPoints[i].sx = 0.0;
        rgPoints[i].sy = 0.0;
        rgPoints[i].sz = 0.0;
        rgPointData[i].time = 0.0;

        float direction = RANDOM_RANGE(0.0f, 360.0f) * D3DX_PI / 180.0f;

        rgPointData[i].direction[0] = cosf(direction);  // X
        rgPointData[i].direction[1] = sinf(direction);  // Z

        float angle = (RANDOM_RANGE(60.0f, 70.0f)) * D3DX_PI / 180.0f;
        float velocity = fMEAN_VELOCITY + RANDOM_RANGE(-0.8f, 1.0f);

        rgPointData[i].velocity[0] = velocity * cosf(angle); // X,Z
        rgPointData[i].velocity[1] = velocity * sinf(angle); // Y

        rgPoints[i].color = colorSet[rand() % cColorSets];
    }

    g_time = 0.0;
    g_clivepoints = g_numPoints;
}

//=========================================================================
// Update the point sprite positions
//=========================================================================
void updatePointList(DWORD dwTicks)
{
    float fTIME_DELTA = min(.025f, .0015f * dwTicks);

    for(int i = 0; i < g_clivepoints; i++)
    {
        static const float fGRAVITY = 2.0f;
        float distance = rgPointData[i].velocity[0] * g_time;

        // X and Z
        rgPoints[i].sx = rgPointData[i].direction[0] * distance;
        rgPoints[i].sz = rgPointData[i].direction[1] * distance;

        // Y
        rgPoints[i].sy =
            (rgPointData[i].velocity[1] - 0.5f * fGRAVITY * rgPointData[i].time) *
             rgPointData[i].time;

        // if we hit the ground, bounce the point upward again.
        if(rgPoints[i].sy <= 0.0)
        {
            if(distance <= g_fRadius)
            {
                rgPointData[i].velocity[1] *= 0.8f; // 80% of previous up velocity.
                rgPointData[i].time = 0.0;          // reset the particles sense of up time.
            }
            else if(distance > g_fRadius + 1.0f)
            {
                // Particle has hit ground past the distance duration of
                // the particles. Die little particle die.
                rgPointData[i] = rgPointData[--g_clivepoints];
                rgPoints[i--] = rgPoints[g_clivepoints];
                continue;
            }
        }

        rgPointData[i].time += fTIME_DELTA;
    }

    g_time += fTIME_DELTA;

    if(!g_clivepoints)
        makePointList();
}

//=========================================================================
// Create a texture of width & height using the szFmt pattern
//=========================================================================
IDirect3DTexture8 *CreateTexture(DWORD dwWidth, DWORD dwHeight,
    const char *szFmt[])
{
    IDirect3DTexture8 *pTexture = NULL;

    pDev->CreateTexture(dwWidth, dwHeight, 1, 0,
        D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture);
    if(pTexture)
    {
        D3DLOCKED_RECT lockRect;

        HRESULT hr = pTexture->LockRect(0, &lockRect, NULL, 0);

        PVOID pBits = LocalAlloc(0, dwWidth * dwHeight * 4);

        DWORD dwLine = (DWORD)pBits;
        LONG lPitch = lockRect.Pitch;

        int xFmt = 0;
        for(DWORD x = 0; x < dwWidth; x++)
        {
            DWORD dwAddr = dwLine;
            const char *szLineFmt = szFmt[xFmt];

            if(szLineFmt)
                xFmt++;

            int yFmt = 0;
            for(DWORD y = 0; y < dwHeight; y++)
            {
                char ch = (szLineFmt && szLineFmt[yFmt]) ?
                    szLineFmt[yFmt++] : 0;

                *(DWORD*)dwAddr = (ch == 'x') ?
                    D3DCOLOR_ARGB(0xff, 0x1f, 0x1f, 0x8f) :
                    D3DCOLOR_ARGB(0x00, 0, 0, 0);

                dwAddr += sizeof(DWORD);
            }
            dwLine += lPitch;
        }

        XGSwizzleRect(pBits, 
                      0,
                      NULL,
                      lockRect.pBits,
                      dwWidth,
                      dwHeight,
                      NULL, 
                      4);

        LocalFree(pBits);

        // Unlock the map so it can be used
        pTexture->UnlockRect(0);
   }

   return pTexture;
}

//=========================================================================
// Draw the floor
//=========================================================================
void DrawFloor()
{
    CheckHR(pDev->SetTexture(0, g_pFloorTex));

    CheckHR(pDev->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1));

    CheckHR(pDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, g_cfloorverts - 2,
        g_floorverts, sizeof(g_floorverts[0])));
}

//=========================================================================
// Draw the point sprites
//=========================================================================
void DrawPSprites()
{
    if(!g_clivepoints)
        return;

    CheckHR(pDev->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_ADD));
    CheckHR(pDev->SetTextureStageState(3, D3DTSS_COLORARG1, D3DTA_DIFFUSE));
    CheckHR(pDev->SetTextureStageState(3, D3DTSS_COLORARG2, D3DTA_TEXTURE));
    CheckHR(pDev->SetTextureStageState(3, D3DTSS_ALPHAOP, D3DTOP_MODULATE));
    CheckHR(pDev->SetTextureStageState(3, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE));
    CheckHR(pDev->SetTextureStageState(3, D3DTSS_ALPHAARG2, D3DTA_TEXTURE));

    CheckHR(pDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE));
    CheckHR(pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE));
    CheckHR(pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
    CheckHR(pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));

    // Set the render states for using point sprites
    CheckHR(pDev->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE));
    CheckHR(pDev->SetRenderState(D3DRS_POINTSCALEENABLE,  g_fPointScaleEnable));

    CheckHR(pDev->SetRenderState(D3DRS_POINTSIZE,     FtoDW(g_fPointSize)));
    CheckHR(pDev->SetRenderState(D3DRS_POINTSCALE_A,  FtoDW(rgPointScale[g_iPointScale][0])));
    CheckHR(pDev->SetRenderState(D3DRS_POINTSCALE_B,  FtoDW(rgPointScale[g_iPointScale][1])));
    CheckHR(pDev->SetRenderState(D3DRS_POINTSCALE_C,  FtoDW(rgPointScale[g_iPointScale][2])));

//    CheckHR(pDev->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(0.08f)));
//    CheckHR(pDev->SetRenderState(D3DRS_POINTSIZE_MAX, FtoDW(0.08f)));

    // set the point sprite texture
    CheckHR(pDev->SetTexture(3, g_pSpriteTex));

    // draw the little dudes
    CheckHR(pDev->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE));

    CheckHR(pDev->DrawPrimitiveUP(D3DPT_POINTLIST, g_clivepoints,
        &rgPoints, sizeof(rgPoints[0])));

    // clear our renderstates
    CheckHR(pDev->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE));
    CheckHR(pDev->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE));

    CheckHR(pDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE));
    CheckHR(pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE));
    CheckHR(pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE));
    CheckHR(pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE));

    CheckHR(pDev->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_DISABLE));
    CheckHR(pDev->SetTextureStageState(3, D3DTSS_ALPHAOP, D3DTOP_DISABLE));
    CheckHR(pDev->SetTexture(3, NULL));
}

//=========================================================================
// Spit out our current state
//=========================================================================
void DrawText()
{
#if 0
    char szBuf[256];
    DWORD dwHeight;
    DWORD dwY = 10;

    g_bitfont.GetTextStringLength(NULL, &dwHeight, NULL);
    dwHeight += 2;

    D3DCOLOR colFore = D3DCOLOR_XRGB(255, 255, 255);
    D3DCOLOR colBack = D3DCOLOR_XRGB(0, 0, 0);

    dwY += dwHeight;

    sprintf(szBuf, "# Points: %d", g_numPoints);
    //g_bitfont.DrawText(g_pBackBuffer, szBuf, 10, dwY,
    //    0, colFore, colBack);
    dwY += dwHeight;

    sprintf(szBuf, "PointSize: %.3f", g_fPointSize);
    //g_bitfont.DrawText(g_pBackBuffer, szBuf, 10, dwY,
    //    0, colFore, colBack);
    dwY += dwHeight;

    sprintf(szBuf, "PointScaleEnable: %s",
        g_fPointScaleEnable ? "true" : "false");
    //g_bitfont.DrawText(g_pBackBuffer, szBuf, 10, dwY,
    //    0, colFore, colBack);
    dwY += dwHeight;

    static const char *rgszScale[] = { "constant", "linear", "quadratic" };
    sprintf(szBuf, "PointScale: %s", rgszScale[g_iPointScale]);
    //g_bitfont.DrawText(g_pBackBuffer, szBuf, 10, dwY,
    //    0, colFore, colBack);
    dwY += dwHeight;

    if(g_fpausetime)
    {
    //    g_bitfont.DrawText(g_pBackBuffer, "Paused", 10, dwY,
    //        0, colFore, colBack);
    }
#endif
}

//=========================================================================
// Update the point sprites and paint the scene
//=========================================================================
void Render(DWORD dwTicks)
{
    HRESULT hr;

    // Clear the frame buffer, Zbuffer.
    CheckHR(pDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
        0x00006688, 1.0, 0));

    CheckHR(pDev->BeginScene());

    if(g_fspin)
    {
        D3DXMATRIX matRotate;

        g_angle += dwTicks * .018f;

        D3DXMatrixRotationY(&matRotate, g_angle * D3DX_PI / 180);
        pDev->SetTransform(D3DTS_WORLD, &matRotate);
    }

    // draw the floor
    DrawFloor();

    // draw the happy faces
    DrawPSprites();

    CheckHR(pDev->EndScene());

    DrawText();

    // Present!
    hr = pDev->Present(NULL, NULL, NULL, NULL);
    if(hr != D3DERR_DEVICELOST)
        CheckHR(hr);
}

//=========================================================================
// input, update, render
//=========================================================================
void MainLoop()
{
    static DWORD dwLastTickCount = GetTickCount();

    DWORD dwTickCount = GetTickCount();
    DWORD dwTicks = dwTickCount - dwLastTickCount;

    dwLastTickCount = dwTickCount;

    HandleInput();

    if(!g_fpausetime)
        updatePointList(dwTicks);

    Render(dwTicks);
}

#ifndef XBOX

//=========================================================================
// Our useless WIN32 message handler. Almost useless.
//=========================================================================
LRESULT WINAPI
MessageHandler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (Msg == WM_DESTROY)
        PostQuitMessage(0);

    return DefWindowProc(hWnd, Msg, wParam, lParam);
}

//=========================================================================
// WIN32 WinMain
//=========================================================================
int APIENTRY
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    HWND hwnd;

    WNDCLASSEX classex;
    ZeroMemory(&classex, sizeof(classex));

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

    if(!InitD3D(hwnd))
        return EXIT_FAILURE;

    ShowCursor(FALSE);

    bool bQuit = false;
    while(!bQuit)
    {
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            bQuit |= (msg.message == WM_QUIT);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        MainLoop();
    }

    DeinitD3D();
    return EXIT_SUCCESS;
}

#else

//=========================================================================
// Xbox main. Much better.
//=========================================================================
void __cdecl main()
{

    if(!InitD3D(NULL))
        return;

    for(;;)
        MainLoop();


    // whatever
    DeinitD3D();
}

#endif // defined(XBOX)

// Emulate Xbox input functions
#ifndef XBOX

#ifndef _XBOX_
#define     XDEVICE_TYPE_GAMEPAD        0
#define     XDEVICE_TYPE_CHAT_BUTTONS   1
#define     XDEVICE_TYPE_MEMORY_UNIT    2
#define     XDEVICE_TYPE_CHAT_AUDIO     3
#define     XDEVICE_TYPE_MODEM          4

#define     XDEVICE_PORT0               0
#define     XDEVICE_PORT1               1
#define     XDEVICE_PORT2               2
#define     XDEVICE_PORT3               3

#define     XDEVICE_NO_SLOT             0
#define     XDEVICE_TOP_SLOT            0
#define     XDEVICE_BOTTOM_SLOT         16

#define     XDEVICE_PORT0_MASK          (1 << XDEVICE_PORT0)
#define     XDEVICE_PORT1_MASK          (1 << XDEVICE_PORT1)
#define     XDEVICE_PORT2_MASK          (1 << XDEVICE_PORT2)
#define     XDEVICE_PORT3_MASK          (1 << XDEVICE_PORT3)
#define     XDEVICE_PORT0_TOP_MASK      (1 << (XDEVICE_PORT0 + XDEVICE_TOP_SLOT))
#define     XDEVICE_PORT1_TOP_MASK      (1 << (XDEVICE_PORT1 + XDEVICE_TOP_SLOT))
#define     XDEVICE_PORT2_TOP_MASK      (1 << (XDEVICE_PORT2 + XDEVICE_TOP_SLOT))
#define     XDEVICE_PORT3_TOP_MASK      (1 << (XDEVICE_PORT3 + XDEVICE_TOP_SLOT))
#define     XDEVICE_PORT0_BOTTOM_MASK   (1 << (XDEVICE_PORT0 + XDEVICE_BOTTOM_SLOT))
#define     XDEVICE_PORT1_BOTTOM_MASK   (1 << (XDEVICE_PORT1 + XDEVICE_BOTTOM_SLOT))
#define     XDEVICE_PORT2_BOTTOM_MASK   (1 << (XDEVICE_PORT2 + XDEVICE_BOTTOM_SLOT))
#define     XDEVICE_PORT3_BOTTOM_MASK   (1 << (XDEVICE_PORT3 + XDEVICE_BOTTOM_SLOT))

struct XINPUT_GAMEPAD
{
    WORD    wButtons;
    BYTE    bAnalogButtons[8];
    SHORT   sThumbLX;
    SHORT   sThumbLY;
    SHORT   sThumbRX;
    SHORT   sThumbRY;
};

#define XINPUT_GAMEPAD_DPAD_UP          0x00000001
#define XINPUT_GAMEPAD_DPAD_DOWN        0x00000002
#define XINPUT_GAMEPAD_DPAD_LEFT        0x00000004
#define XINPUT_GAMEPAD_DPAD_RIGHT       0x00000008
#define XINPUT_GAMEPAD_START            0x00000010
#define XINPUT_GAMEPAD_BACK             0x00000020
#define XINPUT_GAMEPAD_LEFT_THUMB       0x00000040
#define XINPUT_GAMEPAD_RIGHT_THUMB      0x00000080

#define XINPUT_GAMEPAD_A                0
#define XINPUT_GAMEPAD_B                1
#define XINPUT_GAMEPAD_X                2
#define XINPUT_GAMEPAD_Y                3
#define XINPUT_GAMEPAD_BLACK            4
#define XINPUT_GAMEPAD_WHITE            5
#define XINPUT_GAMEPAD_LEFT_TRIGGER     6
#define XINPUT_GAMEPAD_RIGHT_TRIGGER    7

struct XINPUT_STATE
{
    DWORD dwPacketNumber;
    XINPUT_GAMEPAD Gamepad;
};

#define XGetPortCount() 4
DWORD   XGetDevices( DWORD dwType );
BOOL    XGetDeviceChanges( DWORD dwType, DWORD* pdwInsertions, DWORD* pdwRemovals );
HANDLE  XInputOpen( DWORD dwType, DWORD dwPort, DWORD dwSlot, VOID* );
VOID    XInputClose( HANDLE hDevice );
DWORD   XInputGetState( HANDLE hDevice, XINPUT_STATE* pState );

#endif

void XInitDevices(){}

DWORD XGetDevices(DWORD dwType)
{
    return XDEVICE_PORT0_MASK;
}

BOOL XGetDeviceChanges(DWORD dwType, DWORD* pdwInsertions, DWORD* pdwRemovals)
{
    (*pdwInsertions) = 0L;
    (*pdwRemovals)   = 0L;
    return FALSE;
}

HANDLE XInputOpen(DWORD dwType, DWORD dwPort, DWORD dwSlot, VOID*)
{
    return (HANDLE)1;
}

VOID XInputClose(HANDLE hDevice)
{
}

DWORD XInputGetState(HANDLE hDevice, XINPUT_STATE* pState)
{
    // Read joystick 1 with the Win32 API.
    JOYINFOEX joy;
    ZeroMemory( &joy, sizeof(joy) );
    joy.dwSize  = sizeof(joy);
    joy.dwFlags = JOY_RETURNX|JOY_RETURNY|JOY_RETURNBUTTONS|JOY_RETURNPOV|
                  JOY_USEDEADZONE|JOY_RETURNZ|JOY_RETURNR;
    if( 0 != joyGetPosEx( 0, &joy ) )
        return 0L;

    // Fill in the XBGAMEPAD structure
    ZeroMemory( pState, sizeof(XINPUT_STATE) );

    // Convert joystick's thumbstick values to Xbox gamepad values
    pState->Gamepad.sThumbLX =  (SHORT)joy.dwXpos - 32768;
    pState->Gamepad.sThumbLY = -(SHORT)joy.dwYpos + 32767;
    pState->Gamepad.sThumbRX =  (SHORT)joy.dwZpos - 32768;
    pState->Gamepad.sThumbRY = -(SHORT)joy.dwRpos + 32767;
    if( pState->Gamepad.sThumbLX == -1 )
        pState->Gamepad.sThumbLX = 0;
    if( pState->Gamepad.sThumbRX == -1 )
        pState->Gamepad.sThumbRX = 0;

    // Convert joystick's button values to Xbox gamepad values
    if( joy.dwButtons & 0x001 )   pState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X]     = TRUE;
    if( joy.dwButtons & 0x002 )   pState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y]     = TRUE;
    if( joy.dwButtons & 0x004 )   pState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] = TRUE;
    if( joy.dwButtons & 0x008 )   pState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A]     = TRUE;
    if( joy.dwButtons & 0x010 )   pState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B]     = TRUE;
    if( joy.dwButtons & 0x020 )   pState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] = TRUE;
    if( joy.dwButtons & 0x100 )   pState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]  = TRUE;
    if( joy.dwButtons & 0x200 )   pState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] = TRUE;

    if( joy.dwButtons & 0x040 )   pState->Gamepad.wButtons |= XINPUT_GAMEPAD_LEFT_THUMB;
    if( joy.dwButtons & 0x080 )   pState->Gamepad.wButtons |= XINPUT_GAMEPAD_RIGHT_THUMB;
    if( joy.dwButtons & 0x400 )   pState->Gamepad.wButtons |= XINPUT_GAMEPAD_START;
    if( joy.dwButtons & 0x800 )   pState->Gamepad.wButtons |= XINPUT_GAMEPAD_BACK;

    if( joy.dwPOV == 0 )          pState->Gamepad.wButtons |= (XINPUT_GAMEPAD_DPAD_UP);
    else if( joy.dwPOV == 4500 )  pState->Gamepad.wButtons |= (XINPUT_GAMEPAD_DPAD_RIGHT|XINPUT_GAMEPAD_DPAD_UP);
    else if( joy.dwPOV == 9000 )  pState->Gamepad.wButtons |= (XINPUT_GAMEPAD_DPAD_RIGHT);
    else if( joy.dwPOV == 13500 ) pState->Gamepad.wButtons |= (XINPUT_GAMEPAD_DPAD_RIGHT|XINPUT_GAMEPAD_DPAD_DOWN);
    else if( joy.dwPOV == 18000 ) pState->Gamepad.wButtons |= (XINPUT_GAMEPAD_DPAD_DOWN);
    else if( joy.dwPOV == 22500 ) pState->Gamepad.wButtons |= (XINPUT_GAMEPAD_DPAD_LEFT|XINPUT_GAMEPAD_DPAD_DOWN);
    else if( joy.dwPOV == 27000 ) pState->Gamepad.wButtons |= (XINPUT_GAMEPAD_DPAD_LEFT);
    else if( joy.dwPOV == 31500 ) pState->Gamepad.wButtons |= (XINPUT_GAMEPAD_DPAD_LEFT|XINPUT_GAMEPAD_DPAD_UP);

    // For beneift of keyboard users, also trap the Escape key
    if( GetAsyncKeyState( VK_ESCAPE ) )
        pState->Gamepad.wButtons |= XINPUT_GAMEPAD_BACK;

    return 0L;
}

#endif

void HandleInput()
{
    if(g_hInpDevice)
    {
        XINPUT_STATE xinpstate;
        static XINPUT_STATE xinpstatelast;
#define FButtonDown(_btn) \
        (xinpstate.Gamepad.bAnalogButtons[_btn] && \
            (xinpstate.Gamepad.bAnalogButtons[_btn] != xinpstatelast.Gamepad.bAnalogButtons[_btn]))

        XInputGetState(g_hInpDevice, &xinpstate);

        if(FButtonDown(XINPUT_GAMEPAD_A))
            g_fspin = !g_fspin;

        if(FButtonDown(XINPUT_GAMEPAD_B))
            g_fpausetime = !g_fpausetime;

        if(FButtonDown(XINPUT_GAMEPAD_X))
        {
            g_numPoints += 500;
            if(g_numPoints >= g_cMaxPoints)
                g_numPoints = 500;

            // restart
            g_clivepoints = 0;
        }

        if(FButtonDown(XINPUT_GAMEPAD_Y))
            g_iPointScale = (g_iPointScale + 1) % (sizeof(rgPointScale) / sizeof(rgPointScale[0]));

        if(FButtonDown(XINPUT_GAMEPAD_BLACK))
            g_fPointScaleEnable = !g_fPointScaleEnable;

        if(FButtonDown(XINPUT_GAMEPAD_WHITE))
        {
            g_fPointSize += 10.0f;
            if(g_fPointSize > 70.0f)
                g_fPointSize = 2.0f;
        }

        xinpstatelast = xinpstate;
    }
}

//=========================================================================
// Initialize d3d and a bunch of renderstates
//=========================================================================
BOOL InitD3D(HWND hWnd)
{
    D3DPRESENT_PARAMETERS d3dpp;

    // Create D3D 8.
    IDirect3D8 *pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    if(pD3D == NULL)
        return FALSE;

    // Set the screen mode.
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth           = 640;
    d3dpp.BackBufferHeight          = 480;
    d3dpp.BackBufferFormat          = D3DFMT_R5G6B5;
    d3dpp.BackBufferCount           = 1;
    d3dpp.Windowed                  = FALSE;   // Must be FALSE for Xbox.
    d3dpp.EnableAutoDepthStencil    = TRUE;
    d3dpp.AutoDepthStencilFormat    = D3DFMT_D16;
    d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_RefreshRateInHz= 60;
    d3dpp.hDeviceWindow             = hWnd;
    d3dpp.Flags                     = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    // Create the device.
    HRESULT hr = pD3D->CreateDevice
    (
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &pDev
    );
    if(hr != S_OK)
        return FALSE;

    CheckHR(pDev->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &g_pBackBuffer));

    D3DXVECTOR3 g_vEye(0.0f, 2.0f, 8.0f);
    D3DXVECTOR3 g_vAt(0.0f, 1.0f, 0.0f);
    D3DXVECTOR3 g_vUp(0.0f, 1.0f, 0.0f);

    D3DXMATRIX matView;
    D3DXMatrixLookAtLH(&matView, &g_vEye, &g_vAt, &g_vUp);
    pDev->SetTransform(D3DTS_VIEW, &matView);

    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj,
                              D3DX_PI * 40.0f / 180.0f,
                              640.0f  / 480.0f,
                              0.5f, 40.0f);
    pDev->SetTransform(D3DTS_PROJECTION, &matProj);

    static const char *circles[] =
    {
        "....xxxx....",
        "..xxxxxxxx..",
        ".xxxxxxxxxx.",
        ".xxx....xxx.",
        "xxx......xxx",
        "xxx......xxx",
        "xxx......xxx",
        "xxx......xxx",
        ".xxx....xxx.",
        ".xxxxxxxxxx.",
        "..xxxxxxxx..",
        "....xxxx....",
        NULL
    };
    g_pFloorTex = CreateTexture(16, 16, circles);

    static const char *happyAndrew[] =
    {
        "....xxxx....",
        "..xxxxxxxx..",
        ".xxx....xxx.",
        ".xx..xx..xx.",
        "xx..xxxx..xx",
        "xxxxx..xxxxx",
        "xxxxx..xxxxx",
        "xxxxxxxxxxxx",
        ".xx..xx..xx.",
        ".xx..xx..xx.",
        "..xxxxxxxx..",
        "....xxxx....",
        NULL
    };
    g_pSpriteTex = CreateTexture(16, 16, happyAndrew);

    CheckHR(pDev->SetRenderState(D3DRS_LIGHTING, FALSE));
    CheckHR(pDev->SetRenderState(D3DRS_ZENABLE, TRUE));

    CheckHR(pDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD));
    CheckHR(pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE));
    CheckHR(pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE));

    CheckHR(pDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE));
    CheckHR(pDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE));
    CheckHR(pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE));

    CheckHR(pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW));

    // create our points
    makePointList();

    // create floor
    DWORD color = 0;//D3DCOLOR_COLORVALUE(0.5f, 1.0f, 0f, 1.0f);
    color = D3DCOLOR_COLORVALUE(0, 0, .5f, 1.0f);

    // center point
    g_floorverts[0].sx = 0.0;
    g_floorverts[0].sy = -0.05f;
    g_floorverts[0].sz = 0.0;
    g_floorverts[0].color = color;
    g_floorverts[0].tu = 0.0;
    g_floorverts[0].tv = 0.0;

    // create fan around center point
    int ivert = 1;
    float angle = 0;
    for(ivert = 1; ivert < g_cfloorverts; ivert++)
    {
        g_floorverts[ivert].sx = cosf(angle) * g_fRadius;
        g_floorverts[ivert].sy = -0.05f;
        g_floorverts[ivert].sz = sinf(angle) * g_fRadius;

        g_floorverts[ivert].color = color;
        g_floorverts[ivert].tu = cosf(angle) * g_fRadius;
        g_floorverts[ivert].tv = sinf(angle) * g_fRadius;
        angle += 2 * D3DX_PI / (g_cfloorverts - 2);
    }

    // No longer need the D3D interface so free it.
    pD3D->Release();

    g_hInpDevice = XInputOpen(XDEVICE_TYPE_GAMEPAD, 0, XDEVICE_NO_SLOT, NULL);
    return TRUE;
}

//=========================================================================
// Whack all our alloc'd stuff
//=========================================================================
void DeinitD3D()
{
    if(g_pFloorTex)
        g_pFloorTex->Release();
    if(g_pSpriteTex)
        g_pSpriteTex->Release();
    if(g_pBackBuffer)
        g_pBackBuffer->Release();
    if(pDev)
        pDev->Release();

    XInputClose(g_hInpDevice);
    g_hInpDevice = NULL;
}

