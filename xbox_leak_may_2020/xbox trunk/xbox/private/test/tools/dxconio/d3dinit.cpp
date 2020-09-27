/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    d3dinit.cpp

Description:

    Direct3D initialization routines.

*******************************************************************************/

#ifndef UNDER_XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif // UNDER_XBOX
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <d3dx8.h>
#include "util.h"
#include "main.h"
#include "d3denum.h"
#include "d3dinit.h"
#include "texture.h"
#include "fontmap.h"

#ifdef UNDER_XBOX
#define DEMO_HACK
#endif

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

// View and projection transform data
#define VIEW_POS        D3DXVECTOR3(0.0f, 0.0f, 0.0f)
#define VIEW_AT         D3DXVECTOR3(0.0f, 0.0f, 1.0f)
#define VIEW_UP         D3DXVECTOR3(0.0f, 1.0f, 0.0f)
#define PROJ_NEAR       0.1f
#define PROJ_FAR        1000.0f
#define PROJ_FOV        (M_PI / 4.0f)

// Resource identifiers
#define IDM_DEFAULT_FILE_DISPLAY    1
#define IDM_DEFAULT_FILE_EXIT       2

#ifndef IDC_STATIC
#define IDC_STATIC                  (-1)
#endif
#define IDC_DISPLAY_ADAPTER         101
#define IDC_DISPLAY_DEVICE          102
#define IDC_DISPLAY_MODE            103
#define IDC_DISPLAY_FULLSCREEN      104
#define IDC_DISPLAY_DEPTHBUFFER     105
#define IDC_DISPLAY_ANTIALIAS       106

//******************************************************************************
// Structures
//******************************************************************************

//******************************************************************************
// Local function prototypes
//******************************************************************************

namespace DXCONIO {

static BOOL             InitDeviceState();
static BOOL             SetViewport(DWORD dwWidth, DWORD dwHeight);

static BOOL             CreateDevice(PADAPTERDESC padpd, PDEVICEDESC pdevd, 
                                PDISPLAYMODE pdm, BOOL bWindowed, 
                                BOOL bDepthBuffer, BOOL bAntialias);

static BOOL             RestoreFrameBuffer();

static HMENU            CreateMainMenu();
static BOOL CALLBACK    DisplayDlgProc(HWND hDlg, UINT message, WPARAM wParam, 
                                LPARAM lParam);
static void             AddDlgItemTemplate(LPWORD* ppw, WORD wClass, 
                                LPWSTR wszText, WORD wId, short x, short y, 
                                short cx, short cy, DWORD dwStyle);


//******************************************************************************
// Globals
//******************************************************************************

LPDIRECT3D8             g_pd3d = NULL;
LPDIRECT3DDEVICE8       g_pd3dDevice = NULL;

D3DPRESENT_PARAMETERS   g_d3dpp;
D3DSURFACE_DESC         g_d3dsdBack;

PADAPTERDESC            g_padpdList = NULL;
DISPLAYDESC             g_disdCurrent = {0};

HWND                    g_hWnd = NULL;
RECT                    g_rectWnd;
static RECT             g_rectBorder;

KEYSDOWN                g_kdKeys = 0;

CAMERA                  g_cam;

float                   g_fCameraTDelta = 0.5f;
float                   g_fCameraRDelta = 0.011415f;

float                   g_fTimeDilation = 1.0f;

BOOL                    g_bActive = TRUE;

static BOOL             g_bDisplayFPS = TRUE;

static SETDEVICEPROC    g_pfnSetDevice;
static CLEARDEVICEPROC  g_pfnClearDevice;
static SUBWNDPROC       g_pfnSubWndProc = NULL;

#ifndef UNDER_XBOX

//******************************************************************************
// WndProc
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     WndProc
//
// Description:
//
//     Window procedure to process messages for the main application window
//
// Arguments:
//
//     HWND hWnd                - Application window
//
//     UINT message             - Message to process
//
//     WPARAM wParam            - First message parameter
//
//     LPARAM lParam            - Second message parameter
//
// Return Value:
//
//     0 if the message was handled, the return of DefWindowProc otherwise.
//
//******************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    // If the application supplied a window procedure, give it a chance to
    // handle the message first
    if (g_pfnSubWndProc) {
        LRESULT lr;
        if (g_pfnSubWndProc(&lr, hWnd, uMsg, wParam, lParam)) {
            return lr;  // The message was handled, return the given LRESULT
        }
    }

    switch (uMsg) {

        case WM_ACTIVATEAPP:

            // Suspend the application if it is running in full screen mode
            if (!g_disdCurrent.bWindowed) {
                g_bActive = (BOOL)wParam;
            }

            break;

        case WM_MOVE:

            if (!MoveWindowTarget((short)LOWORD(lParam), (short)HIWORD(lParam))) {
                DestroyWindow(hWnd);
            }

            break;

        case WM_SIZE:

            if (wParam != SIZE_MINIMIZED) {
                if (!SizeWindowTarget(LOWORD(lParam), HIWORD(lParam))) {
                    DestroyWindow(hWnd);
                }
            }
            break;

        case WM_SETCURSOR:

            if (g_bActive && !g_disdCurrent.bWindowed) {
                SetCursor(NULL);
                return TRUE;
            }
            break;

        case WM_MOUSEMOVE:

            if (g_pd3dDevice && g_bActive) {
                POINT point;
                GetCursorPos(&point);
                ScreenToClient(hWnd, &point);
                g_pd3dDevice->SetCursorPosition(point.x, point.y, D3DCURSOR_IMMEDIATE_UPDATE);
            }
            break;

        case WM_ENTERMENULOOP:

            if (g_pd3dDevice && !g_disdCurrent.bWindowed) {
// ##REVIEW: Is FlipToGDISurface gone or has it been changed to something else?  Can we simply remove the call and still have things work?
//                g_pd3dDevice->FlipToGDISurface();
                DrawMenuBar(g_hWnd);
                RedrawWindow(g_hWnd, NULL, NULL, RDW_FRAME);
            }
            break;

        case WM_EXITMENULOOP:

            // Do not include the amount of time the menu was up in game time calculations
//            g_bTimeSync = TRUE;
            break;

        case WM_POWERBROADCAST:

            switch( wParam )
            {
                case PBT_APMQUERYSUSPEND:
                    if (g_pd3dDevice && !g_disdCurrent.bWindowed) {
//                        g_pd3dDevice->FlipToGDISurface();
                        DrawMenuBar(g_hWnd);
                        RedrawWindow(g_hWnd, NULL, NULL, RDW_FRAME);
                    }
                    break;

                case PBT_APMRESUMESUSPEND:
                    // Do not include the amount of time power management was up in game time calculations
//                    g_bTimeSync = TRUE;
                    break;
            }
            break;
/*
        case WM_SYSCOMMAND:
            switch (wParam) {
                case SC_MOVE:
                case SC_SIZE:
                case SC_MAXIMIZE:
                case SC_MONITORPOWER:
                    if (!g_disdCurrent.bWindowed) {
                        return 1;
                    }
                    break;
            }
            break;
*/
        case WM_COMMAND:

            switch (LOWORD(wParam)) {

                case IDM_DEFAULT_FILE_DISPLAY:
                    SelectDisplay();
                    return 0;

                case IDM_DEFAULT_FILE_EXIT:
                    SendMessage(hWnd, WM_CLOSE, 0, 0);
                    return 0;
            }

            break;

        case WM_KEYDOWN:

            switch (wParam) {

                case VK_LEFT:
                    g_kdKeys |= KEY_LEFT;
                    return 0;

                case VK_RIGHT:
                    g_kdKeys |= KEY_RIGHT;
                    return 0;

                case VK_UP:
                    g_kdKeys |= KEY_UP;
                    return 0;

                case VK_DOWN:
                    g_kdKeys |= KEY_DOWN;
                    return 0;

                case VK_ADD:
                    g_kdKeys |= KEY_ADD;
                    return 0;

                case VK_SUBTRACT:
                    g_kdKeys |= KEY_SUBTRACT;
                    return 0;

                case VK_INSERT:
                    g_kdKeys |= KEY_INSERT;
                    return 0;

                case VK_DELETE:
                    g_kdKeys |= KEY_DELETE;
                    return 0;

                case VK_HOME:
                    g_kdKeys |= KEY_HOME;
                    return 0;

                case VK_END:
                    g_kdKeys |= KEY_END;
                    return 0;

                case VK_PRIOR:
                    g_kdKeys |= KEY_PAGEUP;
                    return 0;

                case VK_NEXT:
                    g_kdKeys |= KEY_PAGEDOWN;
                    return 0;

                case VK_SHIFT:
                    g_kdKeys |= KEY_SHIFT;
                    return 0;

                case VK_RETURN: // Toggle pause state
                    g_bPaused = !g_bPaused;
                    return 0;

                case VK_ESCAPE: // Exit
                    SendMessage(hWnd, WM_CLOSE, 0, 0);
                    return 0;
            }

            break;

// REVIEW: Don't reference the device pointer in any of these handlers
// instead call a function (i.e. OnPointFill) which will make the
// necessary device method call
        case WM_KEYUP:

            switch (wParam) {

                case VK_LEFT:
                    g_kdKeys &= ~KEY_LEFT;
                    return 0;

                case VK_RIGHT:
                    g_kdKeys &= ~KEY_RIGHT;
                    return 0;

                case VK_UP:
                    g_kdKeys &= ~KEY_UP;
                    return 0;

                case VK_DOWN:
                    g_kdKeys &= ~KEY_DOWN;
                    return 0;

                case VK_ADD:
                    g_kdKeys &= ~KEY_ADD;
                    return 0;

                case VK_SUBTRACT:
                    g_kdKeys &= ~KEY_SUBTRACT;
                    return 0;

                case VK_INSERT:
                    g_kdKeys &= ~KEY_INSERT;
                    return 0;

                case VK_DELETE:
                    g_kdKeys &= ~KEY_DELETE;
                    return 0;

                case VK_HOME:
                    g_kdKeys &= ~KEY_HOME;
                    return 0;

                case VK_END:
                    g_kdKeys &= ~KEY_END;
                    return 0;

                case VK_PRIOR:
                    g_kdKeys &= ~KEY_PAGEUP;
                    return 0;

                case VK_NEXT:
                    g_kdKeys &= ~KEY_PAGEDOWN;
                    return 0;

                case VK_SHIFT:
                    g_kdKeys &= ~KEY_SHIFT;
                    return 0;

                case VK_SPACE:
                    g_bPaused = TRUE;
                    g_bAdvance = TRUE;
                    return 0;

                case VK_F2:     // Open display dialog
                    SelectDisplay();
                    return 0;
/*
                case VK_F3:     // Gouraud shading
                    g_pd3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, 
                                                 (DWORD)D3DSHADE_GOURAUD);
                    return 0;

                case VK_F5: {   // Toggle z-buffering
                    BOOL bZBufferOn;
                    if (SUCCEEDED(g_pd3dDevice->GetRenderState(
                                  D3DRENDERSTATE_ZENABLE, 
                                  (LPDWORD)&bZBufferOn))) {
                        bZBufferOn = !bZBufferOn;
                        g_pd3dDevice->SetRenderState(
                                  D3DRENDERSTATE_ZENABLE, 
                                  (DWORD)bZBufferOn);
                    }
                    return 0;
                }

                case VK_F7: {   // Toggle specular highlights
                    BOOL bSpecularOn;
                    if (SUCCEEDED(g_pd3dDevice->GetRenderState(
                                  D3DRENDERSTATE_SPECULARENABLE, 
                                  (LPDWORD)&bSpecularOn))) {
                        bSpecularOn = !bSpecularOn;
                        g_pd3dDevice->SetRenderState(
                                  D3DRENDERSTATE_SPECULARENABLE, 
                                  (DWORD)bSpecularOn);
                    }
                    return 0;
                }

                case VK_F10: {  // Toggle perspective correction
                    BOOL bPerspectiveOn;
                    if (SUCCEEDED(g_pd3dDevice->GetRenderState(
                                  D3DRENDERSTATE_TEXTUREPERSPECTIVE, 
                                  (LPDWORD)&bPerspectiveOn))) {
                        bPerspectiveOn = !bPerspectiveOn;
                        g_pd3dDevice->SetRenderState(
                                  D3DRENDERSTATE_TEXTUREPERSPECTIVE, 
                                  (DWORD)bPerspectiveOn);
                    }
                    return 0;
                }
*/
            }

            break;


        case WM_SYSKEYUP:

            switch (wParam) {

                case VK_RETURN:
                    UpdateDisplay(g_disdCurrent.pdm, !g_disdCurrent.bWindowed,
                                  g_disdCurrent.bDepthBuffer, 
                                  g_disdCurrent.bAntialias);
                    return 0;
            }

            break;

        case WM_CHAR:

            switch (wParam) {

                case TEXT('F'):
                case TEXT('f'):
                    g_bDisplayFPS = !g_bDisplayFPS;
                    return 0;

                case TEXT('T'):
                case TEXT('t'):
                    if (g_fTimeDilation < 1.5f && g_fTimeDilation > 0.26f) {
                        g_fTimeDilation /= 2.0f;
                    }
                    else if (g_fTimeDilation < 0.26f) {
                        g_fTimeDilation = 2.0f;
                    }
                    else if (g_fTimeDilation >= 1.5f) {
                        g_fTimeDilation = 1.0f;
                    }
                    return 0;

                case TEXT('D'):
                case TEXT('d'):
                    UpdateDisplay(g_disdCurrent.pdm, g_disdCurrent.bWindowed, 
                        !g_disdCurrent.bDepthBuffer, g_disdCurrent.bAntialias);
                    return 0;

                case TEXT('A'):
                case TEXT('a'):
                    UpdateDisplay(g_disdCurrent.pdm, g_disdCurrent.bWindowed, 
                        g_disdCurrent.bDepthBuffer, !g_disdCurrent.bAntialias);
                    return 0;

/*
                case TEXT('P'):      // Point fill
                case TEXT('p'):
                    g_pd3dDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, 
                                                        (DWORD)D3DFILL_POINT);
                    return 0;

                case TEXT('W'):      // Wireframe fill
                case TEXT('w'):
                    g_pd3dDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, 
                                                        (DWORD)D3DFILL_WIREFRAME);
                    return 0;

                case TEXT('S'):      // Solid fill
                case TEXT('s'):
                    g_pd3dDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, 
                                                        (DWORD)D3DFILL_SOLID);
                    return 0;

                case TEXT('D'):      // Toggle dithering
                case TEXT('d'): {
                    BOOL bDitheringOn;
                    if (SUCCEEDED(g_pd3dDevice->GetRenderState(
                                  D3DRENDERSTATE_DITHERENABLE, 
                                  (LPDWORD)&bDitheringOn))) {
                        bDitheringOn = !bDitheringOn;
                        g_pd3dDevice->SetRenderState(
                                  D3DRENDERSTATE_DITHERENABLE, 
                                  (DWORD)bDitheringOn);
                    }
                    return 0;
                }

                case TEXT('A'):      // Toggle anti-aliasing
                case TEXT('a'): {
                    BOOL bAntialias;
                    if (SUCCEEDED(g_pd3dDevice->GetRenderState(
                                  D3DRENDERSTATE_ANTIALIAS, 
                                  (LPDWORD)&bAntialias))) {
                        bAntialias = !bAntialias;
                        g_pd3dDevice->SetRenderState(
                                  D3DRENDERSTATE_ANTIALIAS, 
                                  (DWORD)bAntialias);
                    }
                    return 0;
                }

                case TEXT('O'):      // Point filtering
                case TEXT('o'):
                    g_pd3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, 
                                                 (DWORD)D3DFILTER_NEAREST);
                    g_pd3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, 
                                                 (DWORD)D3DFILTER_NEAREST);
                    return 0;

                case TEXT('L'):      // Bi-linear filtering
                case TEXT('l'):
                    g_pd3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, 
                                                 (DWORD)D3DFILTER_LINEAR);
                    g_pd3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, 
                                                 (DWORD)D3DFILTER_LINEAR);
                    return 0;
*/
            }

            break;

        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

#endif // UNDER_XBOX

//******************************************************************************
// Initialization functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     InitDeviceState
//
// Description:
//
//     Initialize the default render states.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
static BOOL InitDeviceState() {

    D3DMATRIX       mTransform;
    BOOL            bRet = TRUE;
    UINT            i;
    HRESULT         hr;

    // Begin the scene
    hr = g_pd3dDevice->BeginScene();
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::BeginScene"))) {
        return FALSE;
    }

    // Set ambient lighting
    hr = g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 
                                      (DWORD)RGBA_MAKE(128, 128, 128, 128));
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Enable z-buffering
    hr = g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, (DWORD)D3DZB_FALSE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    hr = g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, (DWORD)FALSE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    hr = g_pd3dDevice->SetRenderState(D3DRS_ZFUNC, (DWORD)D3DCMP_LESSEQUAL);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Set the texture filters
#ifndef DEMO_HACK
    for (i = 0; i < 4; i++) {
#else
    for (i = 0; i < 2; i++) {
#endif
        hr = g_pd3dDevice->SetTextureStageState(i, D3DTSS_MINFILTER, (DWORD)D3DTEXF_POINT);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
            bRet = FALSE;
        }

        hr = g_pd3dDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, (DWORD)D3DTEXF_POINT);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
            bRet = FALSE;
        }

        hr = g_pd3dDevice->SetTextureStageState(i, D3DTSS_MIPFILTER, (DWORD)D3DTEXF_NONE);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
            bRet = FALSE;
        }
    }

    // Use gouraud shading
    hr = g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, (DWORD)D3DSHADE_GOURAUD);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Set blend modes
    hr = g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    hr = g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }
    
    // Modulate color and alpha texture stages
    hr = SetColorStage(0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
        bRet = FALSE;
    }

    hr = SetAlphaStage(0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
        bRet = FALSE;
    }

    hr = g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
        bRet = FALSE;
    }

    hr = g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
        bRet = FALSE;
    }

    // Set the alpha comparison function
    hr = g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, (DWORD)D3DCMP_GREATEREQUAL);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Set the alpha reference value to opaque
    hr = g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0xFF);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Use a solid fill mode
    hr = g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_SOLID);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Disable lighting
    hr = g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, (DWORD)FALSE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }
    
    // Turn off specular highlights
    hr = g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, (DWORD)FALSE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Disable texture wrapping
    for (i = 0; i < 8; i++) {
        hr = g_pd3dDevice->SetRenderState((D3DRENDERSTATETYPE)((DWORD)D3DRS_WRAP0 + i), (DWORD)0);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
            bRet = FALSE;
        }
    }

    // Enable dithering
    hr = g_pd3dDevice->SetRenderState(D3DRS_DITHERENABLE, (DWORD)TRUE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Disable per-vertex color
    hr = g_pd3dDevice->SetRenderState(D3DRS_COLORVERTEX, (DWORD)FALSE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // End the scene
    hr = g_pd3dDevice->EndScene();
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::EndScene"))) {
        bRet = FALSE;
    }

    // Initialize the world matrix to the identity matrix
    InitMatrix(&mTransform,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    hr = g_pd3dDevice->SetTransform(D3DTS_WORLD, &mTransform);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTransform"))) {
        return FALSE;
    }

    // Initialize the camera
    if (!SetCameraView(&g_cam)) {
        return FALSE;
    }

    return bRet;
}

//******************************************************************************
static BOOL SetViewport(DWORD dwWidth, DWORD dwHeight) {

    D3DVIEWPORT8    viewport;
    HRESULT         hr;

    // Set a viewport for the device
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Width = dwWidth;
    viewport.Height = dwHeight;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;

    hr = g_pd3dDevice->SetViewport(&viewport);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetViewport"))) {
        return FALSE;
    }

    return TRUE;
}
/*
//******************************************************************************
//
// Function:
//
//     CreateDevice
//
// Description:
//
//     Create the device object using the given adapter, device, and display
//     mode information.
//
// Arguments:
//
//     PADAPTERDESC padpd       - The adapter to use in creating the device
//                                object
//
//     PDEVICEDESC pdevd        - The device to use in creating the device
//                                object.
//
//     PDISPLAYMODE pdm         - The display mode to use in creating the
//                                device object (if full screen)
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
//##HACK function (see replacement commented out)
BOOL CreateDevice(PADAPTERDESC padpd, PDEVICEDESC pdevd, PDISPLAYMODE pdm, 
                    BOOL bWindowed, BOOL bDepthBuffer, BOOL bAntialias)
{
    LPDIRECT3DSURFACE8      pd3ds;
    HRESULT                 hr;

    if (!padpd || !pdevd || !pdm) {
        return FALSE;
    }

    // Check if windowed mode is available
    if (!pdevd->bCanRenderWindowed) {
//        bWindowed = FALSE;
    }

    if (!ANTIALIAS_SUPPORTED(pdevd, pdm, bWindowed)) {
        bAntialias = FALSE;
    }

    // Initialize the presentation parameters
    memset(&g_d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    if (bWindowed) {
//        g_d3dpp.BackBufferFormat                  = D3DFMT_UNKNOWN;
        g_d3dpp.MultiSampleType                   = pdevd->d3dmstWindowed;
        g_d3dpp.AutoDepthStencilFormat            = D3DFMT_D16;
    }
    else {
        g_d3dpp.BackBufferWidth                   = pdm->d3ddm.Width;
        g_d3dpp.BackBufferHeight                  = pdm->d3ddm.Height;
        g_d3dpp.BackBufferFormat                  = pdm->d3ddm.Format;
        g_d3dpp.MultiSampleType                   = pdm->d3dmst;
        g_d3dpp.AutoDepthStencilFormat            = pdm->fmtd;
        g_d3dpp.FullScreen_RefreshRateInHz        = pdm->d3ddm.RefreshRate;
        // REVIEW: The following should be checked against D3DCAPS8 for support
        g_d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    }
    g_d3dpp.BackBufferCount                       = 1;
    g_d3dpp.SwapEffect                            = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.hDeviceWindow                         = g_hWnd;
    g_d3dpp.Windowed                              = bWindowed;
    g_d3dpp.EnableAutoDepthStencil                = bDepthBuffer;
    if (!bAntialias) {
        g_d3dpp.MultiSampleType                   = D3DMULTISAMPLE_NONE;
    }

    // Create the device
    hr = g_pd3d->CreateDevice(padpd->uAdapter, pdevd->d3ddt, g_hWnd, pdevd->dwBehavior, &g_d3dpp, &g_pd3dDevice);
    if (ResultFailed(hr, TEXT("IDirect3D8::CreateDevice"))) {
        return FALSE;
    }

    // Get the surface description of the back buffer
    hr = g_pd3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetBackBuffer"))) {
        return FALSE;
    }
    g_d3dsdBack = pd3ds->GetDesc();
    pd3ds->Release();

    // Initialize the viewport
    SetViewport(g_d3dsdBack.Width, g_d3dsdBack.Height);

    // Initialize the default states of the device
    if (!InitDeviceState()) {
        return FALSE;
    }

    g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, bAntialias);

    g_disdCurrent.padpd = padpd;
    g_disdCurrent.pdevd = pdevd;
    g_disdCurrent.pdm = pdm;
    g_disdCurrent.bWindowed = bWindowed;
    g_disdCurrent.bDepthBuffer = bDepthBuffer;
    g_disdCurrent.bAntialias = bAntialias;

    return TRUE;
}
*/

BOOL CreateDevice(PADAPTERDESC padpd, PDEVICEDESC pdevd, PDISPLAYMODE pdm, 
                    BOOL bWindowed, BOOL bDepthBuffer, BOOL bAntialias)
{
    LPDIRECT3DSURFACE8      pd3ds;
    HRESULT                 hr;

    if (!padpd || !pdevd || !pdm) {
        return FALSE;
    }

//##HACK
pdevd->bCanRenderWindowed = TRUE;
    // Check if windowed mode is available
    if (!pdevd->bCanRenderWindowed) {
        bWindowed = FALSE;
    }

    // Check if antialiasing is available
    if (!ANTIALIAS_SUPPORTED(pdevd, pdm, bWindowed)) {
        bAntialias = FALSE;
    }

    // Initialize the presentation parameters
    memset(&g_d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    if (bWindowed) {
        g_d3dpp.BackBufferFormat                  = D3DFMT_UNKNOWN;
        g_d3dpp.MultiSampleType                   = pdevd->d3dmstWindowed;
        g_d3dpp.AutoDepthStencilFormat            = pdevd->fmtdWindowed;
    }
    else {
        g_d3dpp.BackBufferWidth                   = pdm->d3ddm.Width;
        g_d3dpp.BackBufferHeight                  = pdm->d3ddm.Height;
        g_d3dpp.BackBufferFormat                  = pdm->d3ddm.Format;
        g_d3dpp.MultiSampleType                   = pdm->d3dmst;
        g_d3dpp.AutoDepthStencilFormat            = pdm->fmtd;
        g_d3dpp.FullScreen_RefreshRateInHz        = pdm->d3ddm.RefreshRate;
        // REVIEW: The following should be checked against D3DCAPS8 for support
        g_d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    }
    g_d3dpp.BackBufferCount                       = 1;
    g_d3dpp.SwapEffect                            = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.hDeviceWindow                         = g_hWnd;
    g_d3dpp.Windowed                              = bWindowed;
    g_d3dpp.EnableAutoDepthStencil                = bDepthBuffer;
    if (!bAntialias) {
        g_d3dpp.MultiSampleType                   = D3DMULTISAMPLE_NONE;
    }

    // Create the device
    hr = g_pd3d->CreateDevice(padpd->uAdapter, pdevd->d3ddt, g_hWnd, pdevd->dwBehavior, &g_d3dpp, &g_pd3dDevice);
    if (ResultFailed(hr, TEXT("IDirect3D8::CreateDevice"))) {
        return FALSE;
    }

    // Get the surface description of the back buffer
    hr = g_pd3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetBackBuffer"))) {
        return FALSE;
    }
    pd3ds->GetDesc(&g_d3dsdBack);
    pd3ds->Release();

    // Initialize the viewport
    SetViewport(g_d3dsdBack.Width, g_d3dsdBack.Height);

    // Initialize the default states of the device
    if (!InitDeviceState()) {
        return FALSE;
    }

    g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, bAntialias);

    g_disdCurrent.padpd = padpd;
    g_disdCurrent.pdevd = pdevd;
    g_disdCurrent.pdm = pdm;
    g_disdCurrent.bWindowed = bWindowed;
    g_disdCurrent.bDepthBuffer = bDepthBuffer;
    g_disdCurrent.bAntialias = bAntialias;

    return TRUE;
}

//******************************************************************************
BOOL RestoreDisplay() {

    HRESULT hr;

    hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::Reset"))) {
        return FALSE;
    }

    InitDeviceState();

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     CreateDirect3D
//
// Description:
//
//     Initialize all DirectX objects needed for 3D rendering.
//
// Arguments:
//
//     PD3DCREATIONDESC pd3dcd          - Pointer to creation parameters
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CreateDirect3D(PD3DCREATIONDESC pd3dcd) {

    TCHAR           szPath[MAX_PATH];
    LPTSTR          szFile, sz;
#ifndef UNDER_XBOX
	WNDCLASS        wc;
#endif
    BOOL            bWindowedDefault = FALSE;
    BOOL            bDepthBufferDefault = TRUE;
    BOOL            bAntialiasDefault = FALSE;
    RECT            rect, rectWndDefault = {30, 30, 550, 460};
    POINT           point;
    PADAPTERDESC    padpd;
    PDEVICEDESC     pdevd;
    PDISPLAYMODE    pdm;

    // Release all objects if intialization has already taken place.
    ReleaseDirect3D();

    // Initialize the camera
    g_cam.vPosition     = VIEW_POS;
    g_cam.vInterest     = VIEW_AT;
    g_cam.fRoll         = 0.0f;
    g_cam.fFieldOfView  = PROJ_FOV;
    g_cam.fNearPlane    = PROJ_NEAR;
    g_cam.fFarPlane     = PROJ_FAR;

    // Set the creation parameters
    if (pd3dcd) {
        if (pd3dcd->d3ddt == D3DDEVTYPE_HAL ||
            pd3dcd->d3ddt == D3DDEVTYPE_SW ||
            pd3dcd->d3ddt == D3DDEVTYPE_REF)
        {
            g_devdDefault.d3ddt = pd3dcd->d3ddt;
        }
        if (pd3dcd->d3ddm.Width != 0 && pd3dcd->d3ddm.Height != 0) {
            memcpy(&g_dmDefault.d3ddm, &pd3dcd->d3ddm, sizeof(D3DDISPLAYMODE));
        }
        g_fmtdDefault = pd3dcd->fmtd;

        if (pd3dcd->rectWnd.right > 0 && pd3dcd->rectWnd.bottom > 0) {
            memcpy(&rectWndDefault, &pd3dcd->rectWnd, sizeof(RECT));
        }
        bWindowedDefault = pd3dcd->bWindowed;
        bDepthBufferDefault = pd3dcd->bDepthBuffer;
        bAntialiasDefault = pd3dcd->bAntialias;
    }

#ifndef UNDER_XBOX
    // Register the window class
    memset(&wc, 0, sizeof(WNDCLASS));
    wc.style            = 0;
    wc.lpfnWndProc      = WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = GetModuleHandle(NULL);
    wc.hIcon            = NULL;
    wc.hbrBackground    = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = TEXT("Direct3D Application");

    if (!RegisterClass(&wc)) {
        DebugString(TEXT("RegisterClass failed"));
        return FALSE;
    }

    // Get the name of the executable
    _tcscpy(szPath, GetCommandLine());
    szFile = _tcstok(szPath, TEXT(" \t"));
    sz = szFile + _tcslen(szFile) - 1;
    if (*sz == TEXT('"')) {
        *sz = TEXT('\0');
    }
    sz = szFile + _tcslen(szFile) - 4;
    if (!_tcsncmp(sz, TEXT(".exe"), 4)) {
        *sz = TEXT('\0');
    }
    for (sz = szFile + _tcslen(szFile); sz != szFile && *(sz - 1) != TEXT('\\'); sz--);

    // Create the main application window
    g_hWnd = CreateWindowEx(0, TEXT("Direct3D Application"), 
                            sz, 
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                            rectWndDefault.left, rectWndDefault.top,
                            rectWndDefault.right - rectWndDefault.left, 
                            rectWndDefault.bottom - rectWndDefault.top,
                            NULL, CreateMainMenu(), GetModuleHandle(NULL), NULL);

    if (!g_hWnd) {
        DebugString(TEXT("CreateWindowEx failed"));
		return FALSE;
    }

    UpdateWindow(g_hWnd);

    // Get the client area of the window
    if (!GetClientRect(g_hWnd, &rect)) {
        return FALSE;
    }

    // Convert the client window coordinates to screen coordinates
    point.x = rect.left;
    point.y = rect.top;
    if (!ClientToScreen(g_hWnd, &point)) {
        return FALSE;
    }

    // Save the screen coordinates of the client window
    SetRect(&g_rectWnd, point.x, point.y, 
            point.x + rect.right, point.y + rect.bottom);

    // Get the size offsets of the window border
    GetWindowRect(g_hWnd, &g_rectBorder);
    SetRect(&g_rectBorder, g_rectWnd.left - g_rectBorder.left,
            g_rectWnd.top - g_rectBorder.top,
            g_rectBorder.right - g_rectWnd.right,
            g_rectBorder.bottom - g_rectWnd.bottom);
#else
    g_hWnd = NULL;
#endif

    // Create the Direct3D object
    g_pd3d = Direct3DCreate8(D3D_SDK_VERSION);
    if (!g_pd3d) {
        DebugString(TEXT("Direct3DCreate8 failed"));
        return FALSE;
    }

    // Enumerate all display devices in the system and store the information in
    // a global driver list
    g_padpdList = EnumDisplays(g_pd3d);
    if (!g_padpdList) {
        ReleaseDirect3D();
        return FALSE;
    }

    // Select a default driver
    padpd = SelectAdapter(g_padpdList);
    if (!padpd) {
        ReleaseDirect3D();
        return FALSE;
    }

    // Select a default device
    pdevd = SelectDevice(padpd);
    if (!pdevd) {
        ReleaseDirect3D();
        return FALSE;
    }

    // Select a default display mode (in case of full screen)
    pdm = SelectDisplayMode(pdevd);
    if (!pdm) {
        ReleaseDirect3D();
        return FALSE;
    }

    // Create the device
    if (!CreateDevice(padpd, pdevd, pdm, bWindowedDefault, bDepthBufferDefault, bAntialiasDefault)) {
        ReleaseDirect3D();
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     ReleaseDirect3D
//
// Description:
//
//     Release all 3D rendering objects created by the InitDirect3D function.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void ReleaseDirect3D() {

    PADAPTERDESC padpd;
    PDEVICEDESC  pdevd;
    PDISPLAYMODE pdm;

    // Release the device
    if (g_pd3dDevice) {
        g_pd3dDevice->Release();
        g_pd3dDevice = NULL;
    }

    // Release the Direct3D object
    if (g_pd3d) {
        g_pd3d->Release();
        g_pd3d = NULL;
    }

#ifndef UNDER_XBOX
    MoveWindow(g_hWnd, g_rectWnd.left, g_rectWnd.top, 
               g_rectWnd.right - g_rectWnd.left, 
               g_rectWnd.bottom - g_rectWnd.top, TRUE);
#endif

    // Free the display device list
    for (padpd = g_padpdList; g_padpdList; padpd = g_padpdList) {
        g_padpdList = g_padpdList->padpdNext;
        for (pdevd = padpd->pdevdList; padpd->pdevdList; pdevd = padpd->pdevdList) {
            padpd->pdevdList = padpd->pdevdList->pdevdNext;
            for (pdm = pdevd->pdmList; pdevd->pdmList; pdm = pdevd->pdmList) {
                pdevd->pdmList = pdevd->pdmList->pdmNext;
                MemFree(pdm);
            }
            MemFree(pdevd);
        }
        MemFree(padpd);
    }

    // Reset any application supplied window procedure
    g_pfnSubWndProc = NULL;

#ifndef UNDER_XBOX
    // Unregister the window class
    UnregisterClass(TEXT("Direct3D Application"), GetModuleHandle(NULL));
#endif
}

//******************************************************************************
// Utilitiy functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     FlipFrameBuffer
//
// Description:
//
//     Bring the contents of the rendering target to the screen.  If the 
//     application is running in fullscreen mode, this is accomplished by
//     flipping the front and back buffers.  If the application is windowed,
//     the contents of the rendering target are copied to the primary surface.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     Result of the call to IDirectDrawSurface7::Flip for fullscreen mode.
//     Result of the call to IDirectDrawSurface7::BltFast for windowed mode.
//
//******************************************************************************
/*
HRESULT DisplayFrame() {

    HRESULT hr;

    if (g_bWindowed) {
        // Blt the contents of the rendering target to the primary surface
        hr = g_pddsFront->Blt(&g_rectWnd, g_pddsBack, NULL, DDBLT_WAIT, NULL);
        ResultFailed(hr, TEXT("IDirectDrawSurface7::Blt"));
    }

    else {

        // Flip the back and front buffers
        hr = g_pddsFront->Flip(NULL, DDFLIP_WAIT);
        ResultFailed(hr, TEXT("IDirectDrawSurface7::Flip"));
    }

    if (hr == DDERR_SURFACELOST) {

        // Rendering failed because video memory got freed
        // End the scene in case rendering failed within the scene
        g_pd3dDevice->EndScene();

        // Restore the video memory and continue rendering
        if (RestoreFrameBuffer() && RestoreTextures(g_pd3dDevice)) {
            hr = D3D_OK;
        }
    }

    return hr;
}
*/
//******************************************************************************
//
// Function:
//
//     RestoreFrameBuffer
//
// Description:
//
//     If the surface memory associated with either the primary surface or the  
//     z buffer has been freed, restore it.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
/*
static BOOL RestoreFrameBuffer(void) {

    HRESULT              hr;
    LPDIRECTDRAWSURFACE7 pddsZBuffer;
    DDSCAPS2             ddscaps;

    // If the primary surface has been lost, restore it
    if (g_pddsFront->IsLost() == DDERR_SURFACELOST) {

        hr = g_pddsFront->Restore();
        if (ResultFailed(hr, TEXT("IDirectDrawSurface7::Restore"))) {
            return FALSE;
        }
    }

    // If we are running in windowed mode, restore the back buffer as
    // well since (For full screen mode, the back buffer is an 
    // implicitly created surface and is restored when the front buffer
    // is restored)
    if (g_bWindowed && g_pddsBack->IsLost() == DDERR_SURFACELOST) {
        
        hr = g_pddsBack->Restore();
        if (ResultFailed(hr, TEXT("IDirectDrawSurface7::Restore"))) {
            return FALSE;
        }
    }

    // Get a pointer to the Z-buffer
    memset(&ddscaps, 0, sizeof(DDSCAPS2));
    ddscaps.dwCaps = DDSCAPS_ZBUFFER;

    hr = g_pddsBack->GetAttachedSurface(&ddscaps, &pddsZBuffer);
    if (hr == DDERR_NOTFOUND) {
        // The device isn't using a Z-buffer so we're finished
        return TRUE;
    }
    else if (ResultFailed(hr, TEXT("IDirectDrawSurface7::GetAttachedSurface"))) {
        return FALSE;
    }

    if (pddsZBuffer->IsLost() == DDERR_SURFACELOST) {

        hr = pddsZBuffer->Restore();
        if (ResultFailed(hr, TEXT("IDirectDrawSurface7::Restore"))) {
            pddsZBuffer->Release();
            return FALSE;
        }
    }

    pddsZBuffer->Release();

    return TRUE;
}
*/

#ifndef UNDER_XBOX

//******************************************************************************
BOOL MoveWindowTarget(int x, int y) {

    if (g_disdCurrent.bWindowed && g_pd3dDevice) {
        SetRect(&g_rectWnd, x, y, x + g_d3dsdBack.Width, y + g_d3dsdBack.Height);
    }

    return TRUE;
}

//******************************************************************************
BOOL SizeWindowTarget(int width, int height) {

    if (g_disdCurrent.bWindowed && g_pd3dDevice) {
        g_rectWnd.right = g_rectWnd.left + width;
        g_rectWnd.bottom = g_rectWnd.top + height;
        return UpdateDisplay(g_disdCurrent.pdm, TRUE, g_disdCurrent.bDepthBuffer, 
                             g_disdCurrent.bAntialias);
    }

    return TRUE;
}

#endif 

/*
//******************************************************************************
//##HACK function (see replacement commented out)
BOOL UpdateDisplay(PDISPLAYMODE pdm, BOOL bWindowed, BOOL bDepthBuffer, BOOL bAntialias) {

    DISPLAYDESC             disdLast;
    LPDIRECT3DSURFACE8      pd3ds;
    D3DPRESENT_PARAMETERS   g_d3dpp;
    D3DMATRIX               mTransform;
    HRESULT                 hr;

    if (!g_pd3dDevice) {
        return TRUE;
    }

    // Check if windowed mode is available
//    if (bWindowed && !g_disdCurrent.pdevd->bCanRenderWindowed) {
//        return FALSE;
//    }

    // If no display mode was given, use the current
    if (!pdm) {
        pdm = g_disdCurrent.pdm;
    }

    if (bAntialias && !ANTIALIAS_SUPPORTED(g_disdCurrent.pdevd, pdm, bWindowed)) {
        return FALSE;
    }

    if (bWindowed && !g_disdCurrent.bWindowed) {

        // Restore the window position
        MoveWindow(g_hWnd, g_rectWnd.left - g_rectBorder.left, 
                   g_rectWnd.top - g_rectBorder.top, 
                   g_rectWnd.right - (g_rectWnd.left - g_rectBorder.left) + g_rectBorder.right, 
                   g_rectWnd.bottom - (g_rectWnd.top - g_rectBorder.top) + g_rectBorder.bottom, TRUE);
//        MoveWindow(g_hWnd, g_rectWnd.left, g_rectWnd.top, 
//                   g_rectWnd.right - g_rectWnd.left, 
//                   g_rectWnd.bottom - g_rectWnd.top, TRUE);
    }

    memcpy(&disdLast, &g_disdCurrent, sizeof(DISPLAYDESC));
    g_disdCurrent.pdm = pdm;
    g_disdCurrent.bWindowed = bWindowed;
    g_disdCurrent.bDepthBuffer = bDepthBuffer;
    g_disdCurrent.bAntialias = bAntialias;

//##HACK
if (g_pfnClearDevice) {
    g_pfnClearDevice();
}
ReleaseFontMap();

    // Initialize the presentation parameters
    memset(&g_d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    if (bWindowed) {
//        g_d3dpp.BackBufferFormat                  = D3DFMT_UNKNOWN;
        g_d3dpp.MultiSampleType                   = g_disdCurrent.pdevd->d3dmstWindowed;
        g_d3dpp.AutoDepthStencilFormat            = D3DFMT_D16;
    }
    else {
        g_d3dpp.BackBufferWidth                   = pdm->d3ddm.Width;
        g_d3dpp.BackBufferHeight                  = pdm->d3ddm.Height;
        g_d3dpp.BackBufferFormat                  = pdm->d3ddm.Format;
        g_d3dpp.MultiSampleType                   = pdm->d3dmst;
        g_d3dpp.AutoDepthStencilFormat            = pdm->fmtd;
        g_d3dpp.FullScreen_RefreshRateInHz        = pdm->d3ddm.RefreshRate;
        // REVIEW: The following should be checked against D3DCAPS8 for support
        g_d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    }
    g_d3dpp.BackBufferCount                       = 1;
    g_d3dpp.SwapEffect                            = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.hDeviceWindow                         = g_hWnd;
    g_d3dpp.Windowed                              = bWindowed;
    g_d3dpp.EnableAutoDepthStencil                = bDepthBuffer;
    if (!bAntialias) {
        g_d3dpp.MultiSampleType                   = D3DMULTISAMPLE_NONE;
    }

    // Reset the device
//##HACK
g_pd3dDevice->Release();
//    hr = g_pd3dDevice->Reset(&g_d3dpp);
hr = g_pd3d->CreateDevice(g_disdCurrent.padpd->uAdapter, g_disdCurrent.pdevd->d3ddt, g_hWnd, g_disdCurrent.pdevd->dwBehavior, &g_d3dpp, &g_pd3dDevice);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::Reset"))) {
        return FALSE;
    }

    // Get the surface description of the back buffer
    hr = g_pd3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetBackBuffer"))) {
        return FALSE;
    }
    g_d3dsdBack = pd3ds->GetDesc();
    pd3ds->Release();

    // Initialize the viewport
    SetViewport(g_d3dsdBack.Width, g_d3dsdBack.Height);

    // Rebuild the projection transform using the new aspect ratio
    hr = g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &mTransform);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetTransform"))) {
        return FALSE;
    }

    mTransform._22 = 1.0f / ((float)g_d3dsdBack.Height / (float)g_d3dsdBack.Width);

    hr = g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &mTransform);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTransform"))) {
        return FALSE;
    }

    g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, bAntialias);

//##HACK
InitDeviceState();
if (!CreateFontMap()) {
return FALSE;
}
if (g_pfnSetDevice) {
    if (!g_pfnSetDevice()) {
        return FALSE;
    }
}

    // Do not include the amount of time the display took to update in game time calculations
    g_bTimeSync = TRUE;

    return TRUE;
}
*/

//******************************************************************************
BOOL UpdateDisplay(PDISPLAYMODE pdm, BOOL bWindowed, BOOL bDepthBuffer, BOOL bAntialias) {

    DISPLAYDESC             disdLast;
    LPDIRECT3DSURFACE8      pd3ds;
    D3DMATRIX               mTransform;
    HRESULT                 hr;

    if (!g_pd3dDevice) {
        return TRUE;
    }

    // Check if windowed mode is available
    if (bWindowed && !g_disdCurrent.pdevd->bCanRenderWindowed) {
        return FALSE;
    }

    // If no display mode was given, use the current
    if (!pdm) {
        pdm = g_disdCurrent.pdm;
    }

    // Check if antialiasing is available
    if (bAntialias && !ANTIALIAS_SUPPORTED(g_disdCurrent.pdevd, pdm, bWindowed)) {
        return FALSE;
    }

#ifndef UNDER_XBOX
    if (bWindowed && !g_disdCurrent.bWindowed) {

        // Restore the window position
        MoveWindow(g_hWnd, g_rectWnd.left - g_rectBorder.left, 
                   g_rectWnd.top - g_rectBorder.top, 
                   g_rectWnd.right - (g_rectWnd.left - g_rectBorder.left) + g_rectBorder.right, 
                   g_rectWnd.bottom - (g_rectWnd.top - g_rectBorder.top) + g_rectBorder.bottom, TRUE);
//        MoveWindow(g_hWnd, g_rectWnd.left, g_rectWnd.top, 
//                   g_rectWnd.right - g_rectWnd.left, 
//                   g_rectWnd.bottom - g_rectWnd.top, TRUE);
    }
#endif

    memcpy(&disdLast, &g_disdCurrent, sizeof(DISPLAYDESC));
    g_disdCurrent.pdm = pdm;
    g_disdCurrent.bWindowed = bWindowed;
    g_disdCurrent.bDepthBuffer = bDepthBuffer;
    g_disdCurrent.bAntialias = bAntialias;

//##HACK
if (g_pfnClearDevice) {
    g_pfnClearDevice();
}
//##HACK END

    // Initialize the presentation parameters
    memset(&g_d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    if (bWindowed) {
        g_d3dpp.BackBufferFormat                  = D3DFMT_UNKNOWN;
        g_d3dpp.MultiSampleType                   = g_disdCurrent.pdevd->d3dmstWindowed;
        g_d3dpp.AutoDepthStencilFormat            = g_disdCurrent.pdevd->fmtdWindowed;
    }
    else {
        g_d3dpp.BackBufferWidth                   = pdm->d3ddm.Width;
        g_d3dpp.BackBufferHeight                  = pdm->d3ddm.Height;
        g_d3dpp.BackBufferFormat                  = pdm->d3ddm.Format;
        g_d3dpp.MultiSampleType                   = pdm->d3dmst;
        g_d3dpp.AutoDepthStencilFormat            = pdm->fmtd;
        g_d3dpp.FullScreen_RefreshRateInHz        = pdm->d3ddm.RefreshRate;
        // REVIEW: The following should be checked against D3DCAPS8 for support
        g_d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    }
    g_d3dpp.BackBufferCount                       = 1;
    g_d3dpp.SwapEffect                            = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.hDeviceWindow                         = g_hWnd;
    g_d3dpp.Windowed                              = bWindowed;
    g_d3dpp.EnableAutoDepthStencil                = bDepthBuffer;
    if (!bAntialias) {
        g_d3dpp.MultiSampleType                   = D3DMULTISAMPLE_NONE;
    }

    // Resize the device
    hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::Reset"))) {
        // Restore the display description
        memcpy(&g_disdCurrent, &disdLast, sizeof(DISPLAYDESC));
        return FALSE;
    }

    // Get the surface description of the back buffer
    hr = g_pd3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetBackBuffer"))) {
        return FALSE;
    }
    pd3ds->GetDesc(&g_d3dsdBack);
    pd3ds->Release();

    // Initialize the viewport
    SetViewport(g_d3dsdBack.Width, g_d3dsdBack.Height);

    // Rebuild the projection transform using the new aspect ratio
    hr = g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &mTransform);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetTransform"))) {
        return FALSE;
    }

    mTransform._22 = 1.0f / ((float)g_d3dsdBack.Height / (float)g_d3dsdBack.Width);

    hr = g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &mTransform);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTransform"))) {
        return FALSE;
    }

    g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, bAntialias);

//##HACK
InitDeviceState();
if (g_pfnSetDevice) {
    if (!g_pfnSetDevice()) {
        return FALSE;
    }
}
//##HACK END

    // Do not include the amount of time the display took to update in game time calculations
//    g_bTimeSync = TRUE;

    return TRUE;
}

//******************************************************************************
void DisplayFrameRate() {
/*
    if (g_bDisplayFPS) {

        TCHAR sz[256];
        float fHeight = (float)g_d3dsdBack.Height / 20.0f;
        float fWidth = FONT_ASPECT * fHeight;
        UINT  uLen;
        sprintf(sz, TEXT("%.02f fps"), g_fFPS);
        uLen = _tcslen(sz);
        DisplayText(sz, uLen, (float)(g_d3dsdBack.Width / 2) - (float)(uLen / 2) * fWidth, 
                    (float)g_d3dsdBack.Height - (fHeight * 2.0f), fHeight, 
                    RGBA_MAKE(255, 255, 0, 255));
    }
*/
}

//******************************************************************************
void InitSetDeviceProc(SETDEVICEPROC pfnSetDevice) {

    g_pfnSetDevice = pfnSetDevice;
}

//******************************************************************************
void InitClearDeviceProc(CLEARDEVICEPROC pfnClearDevice) {

    g_pfnClearDevice = pfnClearDevice;
}

//******************************************************************************
void InitSubWindowProc(SUBWNDPROC pfnSubWndProc) {

    g_pfnSubWndProc = pfnSubWndProc;
}

//******************************************************************************
//
// Function:
//
//     SetCameraView
//
// Description:
//
//     Update the view and projection matrices based on the given camera data.
//
// Arguments:
//
//     PCAMERA pcam                 - Pointer to the camera information.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SetCameraView(PCAMERA pcam) {

    D3DXMATRIX      mTransform;
    HRESULT         hr;

    // Initialize the view matrix
    SetView(&mTransform, &pcam->vPosition, &pcam->vInterest, 
            &D3DXVECTOR3((float)sin(pcam->fRoll), (float)cos(pcam->fRoll), 
            0.0f));

    hr = g_pd3dDevice->SetTransform(D3DTS_VIEW, &mTransform);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTransform"))) {
        return FALSE;
    }

    // Initialize the projection matrix
    SetPerspectiveProjection(&mTransform, pcam->fNearPlane, pcam->fFarPlane, 
            pcam->fFieldOfView, (float)g_d3dsdBack.Height / (float)g_d3dsdBack.Width);

    hr = g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &mTransform);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTransform"))) {
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     UpdateCamera
//
// Description:
//
//     Update the camera state by polling user input from the keyboard
//
// Arguments:
//
//     PCAMERA pcam                 - Pointer to the camera information.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
void UpdateCamera(PCAMERA pcam) {

#ifndef UNDER_XBOX
    static KEYSDOWN     kdLastKeys = 0;
    static D3DXVECTOR3  vViewTranslation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    static D3DXVECTOR3  vViewRotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    static D3DXMATRIX   mViewRotationX = D3DXMATRIX(
                            1.0f, 0.0f, 0.0f, 0.0f,
                            0.0f, 1.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f
                        );
    static D3DXMATRIX   mViewRotationY = mViewRotationX;

    // Poll the keyboard
    if (g_kdKeys != kdLastKeys) {

        if (g_kdKeys & KEY_PAGEDOWN) {
            vViewTranslation.x = -g_fCameraTDelta;
        }
        else if (g_kdKeys & KEY_DELETE) {
            vViewTranslation.x = g_fCameraTDelta;
        }
        else {
            vViewTranslation.x = 0.0f;
        }

        if (g_kdKeys & KEY_HOME) {
            vViewTranslation.y = -g_fCameraTDelta;
        }
        else if (g_kdKeys & KEY_END) {
            vViewTranslation.y = g_fCameraTDelta;
        }
        else {
            vViewTranslation.y = 0.0f;
        }

        if (g_kdKeys & KEY_ADD || g_kdKeys & KEY_PAGEUP) {
            vViewTranslation.z = -g_fCameraTDelta;
        }
        else if (g_kdKeys & KEY_SUBTRACT || g_kdKeys & KEY_INSERT) {
            vViewTranslation.z = g_fCameraTDelta;
        }
        else {
            vViewTranslation.z = 0.0f;
        }

        if (g_kdKeys & KEY_LEFT) {
            vViewRotation.y = -g_fCameraRDelta;
        }
        else if (g_kdKeys & KEY_RIGHT) {
            vViewRotation.y = g_fCameraRDelta;
        }
        else {
            vViewRotation.y = 0.0f;
        }

        if (g_kdKeys & KEY_UP) {
            vViewRotation.x = -g_fCameraRDelta;
        }
        else if (g_kdKeys & KEY_DOWN) {
            vViewRotation.x = g_fCameraRDelta;
        }
        else {
            vViewRotation.x = 0.0f;
        }
    }

    kdLastKeys = g_kdKeys;

    if (pcam) {

        D3DXMATRIX  mView, mViewTransform;
        D3DXVECTOR3 vTranslation, vDirection, vUp, vProj, vCross;
        D3DXVECTOR3 vViewTrans = vViewTranslation;
        float       fMagnitude, fDot, fScaleX, fScaleY, fTheta;
        BOOL        bOrbit = (BOOL)(g_kdKeys & KEY_SHIFT);

        if (!bOrbit) {
            vDirection = pcam->vInterest - pcam->vPosition;
        }
        else {
            vDirection = pcam->vPosition - pcam->vInterest;
        }
        fMagnitude = D3DXVec3Length(&vDirection);
        D3DXVec3Normalize(&vDirection, &vDirection);
        vUp = D3DXVECTOR3((float)sin(pcam->fRoll), (float)cos(pcam->fRoll), 0.0f);
        // Project the direction vector into a plane orthogonal to the up vector.
        // The resulting vector will originate at the point the projected direction
        // vector intersects with the plane and terminate at the up vector/
        // The DotProduct gives the ratio between the magnitude of the direction
        // vector and the scalar projection of the direction vector onto the up
        // vector.  Multiplying by this scales the direction vector out to the
        // point that it will intersect with the up vector's orthogonal plane
        fDot = D3DXVec3Dot(&vUp, &vDirection);
        D3DXVec3Normalize(&vProj, &(vUp - (vDirection * fDot)));
        D3DXVec3Cross(&vCross, &vProj, &vDirection);

        fScaleY = (fDot < 0.0f) ? -fDot : fDot;
        fTheta = (float)acos(fScaleY);
        fScaleY = fTheta / M_PIDIV2;

        if (bOrbit) {
            fScaleY *= -3.0f;
            fScaleX = 3.0f;
        }
        else {
            fScaleX = 1.0f;
        }

        mViewRotationY._11 = (float)cos(vViewRotation.y * fScaleY);
        mViewRotationY._13 = (float)-sin(vViewRotation.y * fScaleY);
        mViewRotationY._31 = -mViewRotationY._13;
        mViewRotationY._33 = mViewRotationY._11;

        mViewRotationX._22 = (float)cos(vViewRotation.x * fScaleX);
        mViewRotationX._23 = (float)sin(vViewRotation.x * fScaleX);
        mViewRotationX._32 = -mViewRotationX._23;
        mViewRotationX._33 = mViewRotationX._22;

        InitMatrix(&mViewTransform,
            vCross.x,       vCross.y,       vCross.z,       0.0f,
            vProj.x,        vProj.y,        vProj.z,        0.0f,
            vDirection.x,   vDirection.y,   vDirection.z,   0.0f,
            0.0f,           0.0f,           0.0f,           1.0f
        );

        D3DXMatrixMultiply(&mView, &mViewRotationX, &mViewTransform);
        D3DXMatrixMultiply(&mViewTransform, &mViewRotationY, &mView);

        if (!bOrbit) {
            D3DXVec3TransformCoord(&pcam->vInterest, 
                                &D3DXVECTOR3(0.0f, 0.0f, fMagnitude), &mViewTransform);
            pcam->vInterest += pcam->vPosition;
        }
        else {
            D3DXVec3TransformCoord(&pcam->vPosition, 
                                &D3DXVECTOR3(0.0f, 0.0f, fMagnitude), &mViewTransform);
            pcam->vPosition += pcam->vInterest;
            vViewTrans.x = -vViewTrans.x;
            vViewTrans.z = -vViewTrans.z;
        }
        D3DXVec3TransformCoord(&vTranslation, &vViewTrans, &mViewTransform);
        pcam->vPosition -= vTranslation;
        pcam->vInterest -= vTranslation;

        if (fDot != 0.0f) {
            float fPol = (fDot < 0.0f) ? 1.0f : -1.0f;
            if ((fPol * vViewRotation.x * fScaleX) > fTheta) {
                pcam->fRoll += M_PI;
                if (pcam->fRoll > M_PI) {
                    pcam->fRoll -= M_2PI;
                }
            }
        }
    }
#endif
}

#ifndef UNDER_XBOX

//******************************************************************************
// Menu functions
//******************************************************************************

//******************************************************************************
static HMENU CreateMainMenu() {

    HMENU hMenu, hMenuFile;

    // If the application has supplied a menu, load and return it
    hMenu = LoadMenu(GetModuleHandle(NULL), TEXT("IDR_MENU"));
    if (hMenu) {
        return hMenu;
    }

    // Otherwise create a default menu
    hMenu = CreateMenu();

    hMenuFile = CreateMenu();

    AppendMenu(hMenuFile, MF_STRING, IDM_DEFAULT_FILE_DISPLAY, TEXT("&Display...\tF2"));
    AppendMenu(hMenuFile, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenuFile, MF_STRING, IDM_DEFAULT_FILE_EXIT, TEXT("E&xit\tEsc"));

    AppendMenu(hMenu, MF_POPUP, (UINT)hMenuFile, TEXT("&File"));

    return hMenu;
}

//******************************************************************************
static BOOL CALLBACK DisplayDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    static PDISPLAYDESC pdisd;
    PADAPTERDESC        padpd;
    PDEVICEDESC         pdevd;
    PDISPLAYMODE        pdm;
    TCHAR               szMode[512];
    int                 sel;
    TCHAR*              pszDevice[] = {
                            TEXT(""),
                            TEXT("NULL"),
                            TEXT("Abstraction (HAL)"),
                            TEXT("Reference (REF)"),
                            TEXT("Emulation (SW)"),
                        };

    switch (uMsg) {

        case WM_INITDIALOG:

            // ##REVIEW: Change all of the g_disdCurrent references in WM_INITDIALOG to pdisd references?
            pdisd = (PDISPLAYDESC)lParam;

            // Fill the adapter list
            for (padpd = g_padpdList; padpd; padpd = padpd->padpdNext) {
                // REVIEW: Adapter info strings are currently char...if they aren't changed to TCHAR a conversion must be performed
                sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_ADAPTER, CB_ADDSTRING, 0, (LPARAM)padpd->d3dai.Description);
                SendDlgItemMessage(hDlg, IDC_DISPLAY_ADAPTER, CB_SETITEMDATA, (WPARAM)sel, (LPARAM)padpd);
                if (g_disdCurrent.padpd == padpd) {
                    SendDlgItemMessage(hDlg, IDC_DISPLAY_ADAPTER, CB_SETCURSEL, (WPARAM)sel, 0);
                }
            }

            // Fill the device list
            for (pdevd = g_disdCurrent.padpd->pdevdList; pdevd; pdevd = pdevd->pdevdNext) {
                sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_ADDSTRING, 0, (LPARAM)pszDevice[pdevd->d3ddt]);
                SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_SETITEMDATA, (WPARAM)sel, (LPARAM)pdevd);
                if (g_disdCurrent.pdevd == pdevd) {
                    SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_SETCURSEL, (WPARAM)sel, 0);
                }
            }

            // Fill the display mode
            for (pdm = g_disdCurrent.pdevd->pdmList; pdm; pdm = pdm->pdmNext) {
                if (pdm->d3ddm.RefreshRate) {
                    wsprintf(szMode, TEXT("%d x %d x %d  %d Hz"), pdm->d3ddm.Width, pdm->d3ddm.Height, FormatToBitDepth(pdm->d3ddm.Format), pdm->d3ddm.RefreshRate);
                }
                else {
                    wsprintf(szMode, TEXT("%d x %d x %d"), pdm->d3ddm.Width, pdm->d3ddm.Height, FormatToBitDepth(pdm->d3ddm.Format));
                }
                sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_ADDSTRING, 0, (LPARAM)szMode);
                SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_SETITEMDATA, (WPARAM)sel, (LPARAM)pdm);
                if (pdm == g_disdCurrent.pdm) {
                    SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_SETCURSEL, (WPARAM)sel, 0);
                }
            }

            SendDlgItemMessage(hDlg, IDC_DISPLAY_FULLSCREEN, BM_SETCHECK, (BOOL)!g_disdCurrent.bWindowed, 0);
            EnableWindow(GetDlgItem(hDlg, IDC_DISPLAY_FULLSCREEN), g_disdCurrent.pdevd->bCanRenderWindowed);
            EnableWindow(GetDlgItem(hDlg, IDC_DISPLAY_MODE), !g_disdCurrent.bWindowed);

            SendDlgItemMessage(hDlg, IDC_DISPLAY_DEPTHBUFFER, BM_SETCHECK, g_disdCurrent.bDepthBuffer, 0);
            if (ANTIALIAS_SUPPORTED(g_disdCurrent.pdevd, g_disdCurrent.pdm, g_disdCurrent.bWindowed)) {
                SendDlgItemMessage(hDlg, IDC_DISPLAY_ANTIALIAS, BM_SETCHECK, g_disdCurrent.bAntialias, 0);
            }
            else {
                SendDlgItemMessage(hDlg, IDC_DISPLAY_ANTIALIAS, BM_SETCHECK, FALSE, 0);
                EnableWindow(GetDlgItem(hDlg, IDC_DISPLAY_ANTIALIAS), FALSE);
            }

            return TRUE;

        case WM_COMMAND:

            switch (LOWORD(wParam)) {

                case IDC_DISPLAY_FULLSCREEN:
                    if (HIWORD(wParam) == BN_CLICKED) {
                        PDEVICEDESC pdevd;
                        PDISPLAYMODE pdm;
                        BOOL bAntialias;
                        BOOL bFullscreen = SendDlgItemMessage(hDlg, IDC_DISPLAY_FULLSCREEN, BM_GETCHECK, 0, 0);
                        EnableWindow(GetDlgItem(hDlg, IDC_DISPLAY_MODE), bFullscreen);

                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETCURSEL, 0, 0);
                        pdevd = (PDEVICEDESC)SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETITEMDATA, (WPARAM)sel, 0);
                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_GETCURSEL, 0, 0);
                        pdm = (PDISPLAYMODE)SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_GETITEMDATA, (WPARAM)sel, 0);
                        bAntialias = ANTIALIAS_SUPPORTED(pdevd, pdm, !bFullscreen);
                        if (!bAntialias) {
                            SendDlgItemMessage(hDlg, IDC_DISPLAY_ANTIALIAS, BM_SETCHECK, FALSE, 0);
                        }
                        EnableWindow(GetDlgItem(hDlg, IDC_DISPLAY_ANTIALIAS), bAntialias);
                        return TRUE;
                    }
                    break;

                case IDC_DISPLAY_ADAPTER:

                    if (HIWORD(wParam) == CBN_SELCHANGE) {

                        PADAPTERDESC padpdCurrent;
                        PDEVICEDESC  pdevdDefault, pdevdLast;
                        int          def;

                        // Get the current device selection
                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETCURSEL, 0, 0);
                        pdevdLast = (PDEVICEDESC)SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETITEMDATA, (WPARAM)sel, 0);

                        // Clear the device list
                        SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_RESETCONTENT, 0, 0);

                        // Get the new adapter
                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_ADAPTER, CB_GETCURSEL, 0, 0);
                        padpdCurrent = (PADAPTERDESC)SendDlgItemMessage(hDlg, IDC_DISPLAY_ADAPTER, CB_GETITEMDATA, (WPARAM)sel, 0);

                        // Select a default device for the adapter
                        pdevdDefault = SelectDevice(padpdCurrent);

                        // Add the devices for the new adapter
                        for (pdevd = padpdCurrent->pdevdList; pdevd; pdevd = pdevd->pdevdNext) {

                            sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_ADDSTRING, 0, (LPARAM)pszDevice[pdevd->d3ddt]);
                            SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_SETITEMDATA, (WPARAM)sel, (LPARAM)pdevd);

                            // If the new adapter supports the same device that was selected for the old
                            // adapter, select the device again
                            if (pdevd->d3ddt == pdevdLast->d3ddt) {
                                SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_SETCURSEL, (WPARAM)sel, 0);
                            }

                            // If the device matches the default device, save the index
                            if (pdevd == pdevdDefault) {
                                def = sel;
                            }
                        }

                        // If no device has been selected (the new adapter does not support the old selected device)
                        // select the default as current
                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETCURSEL, 0, 0);
                        if (sel == CB_ERR) {
                            SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_SETCURSEL, (WPARAM)def, 0);
                        }

                        // Update the display mode list for the new device
                        SendMessage(hDlg, WM_COMMAND, MAKELPARAM(IDC_DISPLAY_DEVICE, CBN_SELCHANGE), (LPARAM)GetDlgItem(hDlg, IDC_DISPLAY_DEVICE));

                        return TRUE;
                    }
                    break;

                case IDC_DISPLAY_DEVICE:

                    if (HIWORD(wParam) == CBN_SELCHANGE) {

                        PDEVICEDESC  pdevdCurrent;
                        PDISPLAYMODE pdmDefault, pdmLast;
                        int          def;
                        BOOL         bEnable;

                        // Get the current display mode selection
                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_GETCURSEL, 0, 0);
                        pdmLast = (PDISPLAYMODE)SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_GETITEMDATA, (WPARAM)sel, 0);

                        // Clear the display mode list
                        SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_RESETCONTENT, 0, 0);

                        // Get the new device
                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETCURSEL, 0, 0);
                        pdevdCurrent = (PDEVICEDESC)SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETITEMDATA, (WPARAM)sel, 0);

                        // Select a default display mode for the device
                        pdmDefault = SelectDisplayMode(pdevdCurrent);

                        // Add the display modes for the new device
                        for (pdm = pdevdCurrent->pdmList; pdm; pdm = pdm->pdmNext) {

                            if (pdm->d3ddm.RefreshRate) {
                                wsprintf(szMode, TEXT("%d x %d x %d  %d Hz"), pdm->d3ddm.Width, pdm->d3ddm.Height, FormatToBitDepth(pdm->d3ddm.Format), pdm->d3ddm.RefreshRate);
                            }
                            else {
                                wsprintf(szMode, TEXT("%d x %d x %d"), pdm->d3ddm.Width, pdm->d3ddm.Height, FormatToBitDepth(pdm->d3ddm.Format));
                            }
                            sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_ADDSTRING, 0, (LPARAM)szMode);
                            SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_SETITEMDATA, (WPARAM)sel, (LPARAM)pdm);

                            // If the new device supports the same display mode that was selected for the old
                            // device, select the mode again
                            if (pdm->d3ddm.Width == pdmLast->d3ddm.Width &&
                                pdm->d3ddm.Height == pdmLast->d3ddm.Height &&
                                pdm->d3ddm.Format == pdmLast->d3ddm.Format &&
                                pdm->d3ddm.RefreshRate == pdmLast->d3ddm.RefreshRate) 
                            {
                                SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_SETCURSEL, (WPARAM)sel, 0);
                            }

                            // If the display mode matches the default, save the index
                            if (pdm == pdmDefault) {
                                def = sel;
                            }
                        }

                        // If no mode has been selected (the new device does not support the old selected mode)
                        // select the default as current
                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_GETCURSEL, 0, 0);
                        if (sel == CB_ERR) {
                            SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_SETCURSEL, (WPARAM)def, 0);
                        }

                        // If the device does not supported rendering in a windowed state, select and
                        // disable the full screen check box
                        if (!pdevdCurrent->bCanRenderWindowed) {
                            SendDlgItemMessage(hDlg, IDC_DISPLAY_FULLSCREEN, BM_SETCHECK, (WPARAM)TRUE, 0);
                        }
                        EnableWindow(GetDlgItem(hDlg, IDC_DISPLAY_FULLSCREEN), pdevdCurrent->bCanRenderWindowed);
                        bEnable = SendDlgItemMessage(hDlg, IDC_DISPLAY_FULLSCREEN, BM_GETCHECK, 0, 0);
                        EnableWindow(GetDlgItem(hDlg, IDC_DISPLAY_MODE), bEnable);

                        // Update the antialias check box
                        SendMessage(hDlg, WM_COMMAND, MAKELPARAM(IDC_DISPLAY_MODE, CBN_SELCHANGE), (LPARAM)GetDlgItem(hDlg, IDC_DISPLAY_MODE));

                        return TRUE;
                    }
                    break;

                case IDC_DISPLAY_MODE:

                    if (HIWORD(wParam) == CBN_SELCHANGE) {
                        PDEVICEDESC pdevd;
                        PDISPLAYMODE pdm;
                        BOOL bAntialias;
                        BOOL bFullscreen = SendDlgItemMessage(hDlg, IDC_DISPLAY_FULLSCREEN, BM_GETCHECK, 0, 0);
                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETCURSEL, 0, 0);
                        pdevd = (PDEVICEDESC)SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETITEMDATA, (WPARAM)sel, 0);
                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_GETCURSEL, 0, 0);
                        pdm = (PDISPLAYMODE)SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_GETITEMDATA, (WPARAM)sel, 0);
                        bAntialias = ANTIALIAS_SUPPORTED(pdevd, pdm, !bFullscreen);
                        if (!bAntialias) {
                            SendDlgItemMessage(hDlg, IDC_DISPLAY_ANTIALIAS, BM_SETCHECK, FALSE, 0);
                        }
                        EnableWindow(GetDlgItem(hDlg, IDC_DISPLAY_ANTIALIAS), bAntialias);
                        return TRUE;
                    }
                    break;

                case IDOK:
                    // Update the current driver, device, window state, and display mode
                    sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_ADAPTER, CB_GETCURSEL, 0, 0);
                    pdisd->padpd = (PADAPTERDESC)SendDlgItemMessage(hDlg, IDC_DISPLAY_ADAPTER, CB_GETITEMDATA, (WPARAM)sel, 0);
                    sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETCURSEL, 0, 0);
                    pdisd->pdevd = (PDEVICEDESC)SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETITEMDATA, (WPARAM)sel, 0);
                    sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_GETCURSEL, 0, 0);
                    pdisd->pdm = (PDISPLAYMODE)SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_GETITEMDATA, (WPARAM)sel, 0);
                    pdisd->bWindowed = !((BOOL)SendDlgItemMessage(hDlg, IDC_DISPLAY_FULLSCREEN, BM_GETCHECK, 0, 0));
                    pdisd->bDepthBuffer = (BOOL)SendDlgItemMessage(hDlg, IDC_DISPLAY_DEPTHBUFFER, BM_GETCHECK, 0, 0);
                    pdisd->bAntialias = (BOOL)SendDlgItemMessage(hDlg, IDC_DISPLAY_ANTIALIAS, BM_GETCHECK, 0, 0);

                case IDCANCEL:
                    EndDialog(hDlg, wParam);
                    return TRUE;
            }

            break;
    }

    return FALSE;
}

//******************************************************************************
static void AddDlgItemTemplate(LPWORD* ppw, WORD wClass, LPWSTR wszText, 
            WORD wId, short x, short y, short cx, short cy, DWORD dwStyle) 
{
    LPDLGITEMTEMPLATE pdit;
    LPWSTR            wsz;
    LPWORD            pw = *ppw;

    pdit = (LPDLGITEMTEMPLATE)((((DWORD)pw) + 3) & ~3);    // Align the data
    pdit->style           = WS_VISIBLE | WS_CHILD | dwStyle;
    pdit->dwExtendedStyle = 0;
    pdit->x               = x;
    pdit->y               = y;
    pdit->cx              = cx;
    pdit->cy              = cy;
    pdit->id              = wId;

    pw = (LPWORD)(pdit+1);
    *pw++ = 0xFFFF;                         // System class
    *pw++ = wClass;                         // Class ordinal
    wsz = (LPWSTR)pw;
    wcscpy(wsz, wszText);                   // Item text
    pw = (LPWORD)(wsz + wcslen(wsz) + 1);
    *pw++ = 0;                              // No creation data
    *ppw = pw;
}

//******************************************************************************
void SelectDisplay() {

    LPDLGTEMPLATE     pTemplate;
    LPWORD            pw;
    LPWSTR            wsz;
    DISPLAYDESC       disd;
    int               ret;

    // Save the current display state
    memcpy(&disd, &g_disdCurrent, sizeof(DISPLAYDESC));

    pTemplate = (LPDLGTEMPLATE)MemAlloc(sizeof(DLGTEMPLATE) + 3 * sizeof(WORD) + 
                                        512 * sizeof(WCHAR) + 
                                        11 * (sizeof(DLGITEMTEMPLATE) +
                                        (5 * sizeof(WORD) + 256 * sizeof(WCHAR))));
    if (!pTemplate) {
        return;
    }

    // Initialize the dialog template
    pTemplate->style = WS_POPUP | WS_CAPTION | WS_SYSMENU | DS_MODALFRAME | DS_CENTER | DS_SETFONT;
    pTemplate->cdit  = 11;
    pTemplate->x     = 0;
    pTemplate->y     = 0;
    pTemplate->cx    = 149;
    pTemplate->cy    = 138;
    pTemplate->dwExtendedStyle = 0;

    pw = (LPWORD)(pTemplate+1);
    *pw++ = 0;                                  // No menu
    *pw++ = 0;                                  // Default dialog class
    
    // Specify the caption for the dialog
    wsz = (LPWSTR)pw;
    wcscpy(wsz, L"Display");                    // Caption
    pw = (LPWORD)(wsz + wcslen(wsz) + 1);
    *pw++ = 8;                                  // 8 point font
    wsz = (LPWSTR)pw;
    wcscpy(wsz, L"MS Sans Serif");              // MS Sans Serif typeface
    pw = (LPWORD)(wsz + wcslen(wsz) + 1);

    // Specify the controls of the dialog
    AddDlgItemTemplate(&pw, 0x0085, L"", IDC_DISPLAY_ADAPTER, 13, 16, 123, 50, CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP);
    AddDlgItemTemplate(&pw, 0x0080, L"Adapter", IDC_STATIC, 7, 7, 135, 27, WS_GROUP | BS_GROUPBOX);
    AddDlgItemTemplate(&pw, 0x0085, L"", IDC_DISPLAY_DEVICE, 13, 48, 123, 50, CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP);
    AddDlgItemTemplate(&pw, 0x0080, L"Device", IDC_STATIC, 7, 38, 135, 27, WS_GROUP | BS_GROUPBOX);
    AddDlgItemTemplate(&pw, 0x0080, L"", IDC_STATIC, 7, 70, 135, 27, WS_GROUP | BS_GROUPBOX);
    AddDlgItemTemplate(&pw, 0x0080, L"F&ull screen", IDC_DISPLAY_FULLSCREEN, 13, 68, 50, 10, BS_AUTOCHECKBOX | WS_TABSTOP);
    AddDlgItemTemplate(&pw, 0x0085, L"", IDC_DISPLAY_MODE, 13, 79, 123, 100, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP);
    AddDlgItemTemplate(&pw, 0x0080, L"&Depth Buffer", IDC_DISPLAY_DEPTHBUFFER, 13, 102, 56, 10, BS_AUTOCHECKBOX | WS_TABSTOP);
    AddDlgItemTemplate(&pw, 0x0080, L"&Antialias", IDC_DISPLAY_ANTIALIAS, 83, 102, 42, 10, BS_AUTOCHECKBOX | WS_TABSTOP);
    AddDlgItemTemplate(&pw, 0x0080, L"OK", IDOK, 38, 117, 50, 14, BS_DEFPUSHBUTTON | WS_TABSTOP);
    AddDlgItemTemplate(&pw, 0x0080, L"Cancel", IDCANCEL, 92, 117, 50, 14, WS_TABSTOP);

    // Display the dialog in a full screen state
    if (g_pd3dDevice && !g_disdCurrent.bWindowed) {
//        g_pd3dDevice->FlipToGDISurface();
        DrawMenuBar(g_hWnd);
        RedrawWindow(g_hWnd, NULL, NULL, RDW_FRAME);
    }

    // Create the display dialog
    ret = DialogBoxIndirectParam(GetModuleHandle(NULL), pTemplate, g_hWnd, DisplayDlgProc, (LPARAM)&disd);

    MemFree(pTemplate);

    // Do not include the amount of time the dialog was open in game time calculations
//    g_bTimeSync = TRUE;

    if (ret != IDOK) {
        return;
    }

    // If the display driver or device has been changed, recreate everything
    if (g_disdCurrent.padpd != disd.padpd || g_disdCurrent.pdevd != disd.pdevd) {
        if (g_pfnClearDevice) {
            g_pfnClearDevice();
        }
        g_pd3dDevice->Release();
        g_pd3dDevice = NULL;
        if (!CreateDevice(disd.padpd, disd.pdevd, disd.pdm, disd.bWindowed, disd.bDepthBuffer, disd.bAntialias) ||
            (g_pfnSetDevice && !g_pfnSetDevice())) 
        {
            DestroyWindow(g_hWnd);
        }
/*
        // Save the current state of the display as the default
        SetRect(&g_rectWndDefault,
                g_rectWnd.left - g_rectBorder.left,
                g_rectWnd.top - g_rectBorder.top,
                g_rectWnd.right + g_rectBorder.right,
                g_rectWnd.bottom + g_rectBorder.bottom);
//        g_bWindowedDefault = disd.bWindowed;
        memcpy(&g_drvdDefault, disd.padpd, sizeof(DRIVERDESC));
        memcpy(&g_devdDefault, disd.pdevd, sizeof(DEVICEDESC));
        memcpy(&g_dmDefault, disd.pdm, sizeof(DISPLAYMODE));
        g_bRestart = TRUE;
        SendMessage(g_hWnd, WM_CLOSE, 0, 0);
*/
    }

    // If the state of the display has changed, update the display
    else if (g_disdCurrent.pdm != disd.pdm ||
             g_disdCurrent.bWindowed != disd.bWindowed ||
             g_disdCurrent.bDepthBuffer != disd.bDepthBuffer ||
             g_disdCurrent.bAntialias != disd.bAntialias) 
    {
        UpdateDisplay(disd.pdm, disd.bWindowed, disd.bDepthBuffer, disd.bAntialias);
    }
}

#endif

} // namespace DXCONIO
