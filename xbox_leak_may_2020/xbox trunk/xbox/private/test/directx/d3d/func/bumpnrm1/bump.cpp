/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    d3dbasic.cpp

Description:

    Direct3D Immediate-Mode Sample.

*******************************************************************************/

#define D3D_OVERLOADS

#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <ddraw.h>
#include <d3d.h>
#include "d3dinit.h"
#include "main.h"
#include "material.h"
#include "texture.h"
#include "input.h"
#include "util.h"
#include "bump.h"

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define VIEW_ROTATION_DELTA        0.031415f
#define VIEW_TRANSLATION_DELTA     0.5f

#define SPHERE_U                   32
#define SPHERE_V                   32

//******************************************************************************
// Local function prototypes
//******************************************************************************

static bool                        CreateSphere(D3DVALUE fRadius, LPD3DVERTEX* pprVertices, LPDWORD pdwNumVertices);
static void                        ReleaseSphere(LPD3DVERTEX pr);
static bool                        FadeOut(LPD3DTLVERTEX ptlr, D3DVALUE fAlpha);

//******************************************************************************
// Globals
//******************************************************************************

CAMERA                             g_cam;

//******************************************************************************
//
// Function:
//
//     Render
//
// Description:
//
//     Render a scene.
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
void Render(void) {

    DDSURFACEDESC2              ddsd;
    D3DMATRIX                   mWorld, mView;
    D3DRECT                     d3drect;
    HRESULT                     hr;
    D3DTLVERTEX                 ptlrFade[4];
    D3DVALUE                    fFade;
    LPD3DVERTEX                 prVertices;
    DWORD                       dwNumVertices;
    LPDIRECT3DMATERIAL3         pd3dm = NULL;
    D3DMATERIALHANDLE           hMaterial;
    LPDIRECT3DTEXTURE2          pd3dt = NULL;
    LPDIRECT3DTEXTURE2          pd3dtBump = NULL;
    D3DVALUE                    fTheta = M_PI / 75.0f;
    UINT                        uFadeCount = 60;
    D3DVALUE                    fDir = 1.0f;
    D3DVECTOR                   vBumpDir = D3DVECTOR(0.0f, 0.0f, 1.0f);
    D3DVALUE                    fBumpAngle = 0.0f;
    D3DVALUE                    fIntensity = 0.9f;
    D3DVALUE                    fAmbient = 1.0f;
    D3DLIGHT2                   light;

    // Set the view position
    g_cam.vPosition     = D3DVECTOR(0.0f, 0.0f, -250.0f);
    g_cam.vInterest     = D3DVECTOR(0.0f, 0.0f, 0.0f);
    g_cam.fRoll         = 0.0f;
//    g_cam.vPosition     = D3DVECTOR(2.22496f, 5.56248f, -50.0686f);
//    g_cam.vInterest     = D3DVECTOR(78.847f, 227.889f, 34.7847f);
//    g_cam.fRoll         = 0.392699f;
    g_cam.fFieldOfView  = M_PI / D3DVAL(4.0f);
    g_cam.fNearPlane    = D3DVAL(0.1f);
    g_cam.fFarPlane     = D3DVAL(1000.0f);

    if (!SetView(&g_cam)) {
        g_bQuit = true;
        return;
    }

    // Initialize the world matrix
    InitMatrix(&mWorld,
        D3DVAL(cos(fTheta)), D3DVAL( 0.0f), D3DVAL(-sin(fTheta)), D3DVAL( 0.0f),
        D3DVAL( 0.0f), D3DVAL( 1.0f), D3DVAL( 0.0f), D3DVAL( 0.0f),
        D3DVAL(sin(fTheta)), D3DVAL( 0.0f), D3DVAL(cos(fTheta)), D3DVAL( 0.0f),
        D3DVAL( 0.0f), D3DVAL( 0.0f), D3DVAL( 0.0f), D3DVAL( 1.0f)
    );

    // Initialize the clear rect and flags
    memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);

    hr = g_pddsBack->GetSurfaceDesc(&ddsd);
    if (ResultFailed(hr, TEXT("IDirectDrawSurface4::GetSurfaceDesc"))) {
        g_bQuit = true;
        return;
    }

    d3drect.x1 = 0;
    d3drect.y1 = 0;
    d3drect.x2 = ddsd.dwWidth;
    d3drect.y2 = ddsd.dwHeight;

    // Initialize the vertices that will be used to fade out the scene
    if (uFadeCount) {
        fFade = 1.0f / D3DVAL(uFadeCount);
#ifndef UNDER_CE
        ptlrFade[0] = D3DTLVERTEX(D3DVECTOR(0.0f, 480.0f, 0.00001f), 100000.0f, RGBA_MAKE(0,0,0,255), RGBA_MAKE(0,0,0,0), 0.0f, 1.0f);
        ptlrFade[1] = D3DTLVERTEX(D3DVECTOR(0.0f, 0.0f, 0.00001f), 100000.0f, RGBA_MAKE(0,0,0,255), RGBA_MAKE(0,0,0,0), 0.0f, 0.0f);
        ptlrFade[2] = D3DTLVERTEX(D3DVECTOR(640.0f, 0.0f, 0.00001f), 100000.0f, RGBA_MAKE(0,0,0,255), RGBA_MAKE(0,0,0,0), 1.0f, 0.0f);
        ptlrFade[3] = D3DTLVERTEX(D3DVECTOR(640.0f, 480.0f, 0.00001f), 100000.0f, RGBA_MAKE(0,0,0,255), RGBA_MAKE(0,0,0,0), 1.0f, 1.0f);
#else
        ptlrFade[0] = D3DTLVERTEX(D3DVECTOR(0.0f, 480.0f, 0.15f), 50000.0f, RGBA_MAKE(0,0,0,255), RGBA_MAKE(0,0,0,0), 0.0f, 1.0f);
        ptlrFade[1] = D3DTLVERTEX(D3DVECTOR(0.0f, 0.0f, 0.15f), 50000.0f, RGBA_MAKE(0,0,0,255), RGBA_MAKE(0,0,0,0), 0.0f, 0.0f);
        ptlrFade[2] = D3DTLVERTEX(D3DVECTOR(640.0f, 0.0f, 0.15f), 50000.0f, RGBA_MAKE(0,0,0,255), RGBA_MAKE(0,0,0,0), 1.0f, 0.0f);
        ptlrFade[3] = D3DTLVERTEX(D3DVECTOR(640.0f, 480.0f, 0.15f), 50000.0f, RGBA_MAKE(0,0,0,255), RGBA_MAKE(0,0,0,0), 1.0f, 1.0f);
#endif // !UNDER_CE
    }

    // Initialize the vertices
    if (!CreateSphere(50.0f, &prVertices, &dwNumVertices)) {
        g_bQuit = true;
        return;
    }

    // Get the light data
    memset(&light, 0, sizeof(D3DLIGHT2));
    light.dwSize = sizeof(D3DLIGHT2);
    hr = g_pd3dLight->GetLight((LPD3DLIGHT)&light);
    if (ResultFailed(hr, TEXT("IDirect3DLight::GetLight"))) {
        g_bQuit = true;
        return;
    }

    // Create a material
    pd3dm = CreateMaterial(g_pd3dDevice, &hMaterial, RGB_MAKE(255, 255, 255));
    if (!pd3dm) {
        g_bQuit = true;
        return;
    }

    g_pd3dDevice->SetLightState(D3DLIGHTSTATE_MATERIAL, (DWORD)hMaterial);

    // Create a texture
    pd3dt = CreateTexture(g_pd3dDevice, TEXT("Texture"), false, PXF_RGBA_5551);
    if (!pd3dt) {
        pd3dm->Release();
        g_bQuit = true;
        return;
    }

    pd3dtBump = CreateBumpmap(g_pd3dDevice, TEXT("Bump"));
    if (!pd3dtBump) {
        pd3dt->Release();
        pd3dm->Release();
        g_bQuit = true;
        return;
    }

    hr = g_pd3dDevice->SetLightState(D3DLIGHTSTATE_BUMPDIRECTION, (DWORD)&D3DVECTOR(0.0f, -1.0f, 1.0f));
    ResultFailed(hr, TEXT("IDirect3DDevice3::SetLightState(D3DLIGHTSTATE_BUMPDIRECTION, ...)"));
    g_pd3dDevice->SetLightState(D3DLIGHTSTATE_BUMPINTENSITY, *(unsigned long *)&fIntensity);
    ResultFailed(hr, TEXT("IDirect3DDevice3::SetLightState(D3DLIGHTSTATE_BUMPINTENSITY, ...)"));
    g_pd3dDevice->SetLightState(D3DLIGHTSTATE_BUMPAMBIENT, *(unsigned long *)&fAmbient);
    ResultFailed(hr, TEXT("IDirect3DDevice3::SetLightState(D3DLIGHTSTATE_BUMPAMBIENT, ...)"));

    g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
    g_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);

    hr = g_pd3dDevice->SetTexture(0, pd3dtBump);
    ResultFailed(hr, TEXT("IDirect3DDevice3::SetTexture"));
    hr = g_pd3dDevice->SetTexture(1, pd3dt);
    ResultFailed(hr, TEXT("IDirect3DDevice3::SetTexture on a bumpmap"));

    SetColorStage(0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_BLENDTEXTUREALPHA);
    SetAlphaStage(0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG2);
    SetColorStage(1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
    SetAlphaStage(1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);

    while (!g_bQuit || (uFadeCount && !g_bReset)) {

        // Clear the rendering target
#ifndef UNDER_CE
        hr = g_pd3dViewport->Clear(1, &d3drect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER);
#else
        hr = g_pd3dViewport->Clear(1, &d3drect, D3DCLEAR_TARGET);
#endif // !UNDER_CE
        if (ResultFailed(hr, TEXT("IDirect3DViewport3::Clear"))) {
            goto next_frame;
        }

        // Begin the scene
        hr = g_pd3dDevice->BeginScene();
        if (ResultFailed(hr, TEXT("IDirect3DDevice3::BeginScene"))) {
            goto next_frame;
        }

        g_pd3dDevice->SetTexture(0, pd3dtBump);
        g_pd3dDevice->SetTexture(1, pd3dt);

        // Render
        hr = g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, 
                prVertices, dwNumVertices, D3DDP_DONOTUPDATEEXTENTS);
        if (ResultFailed(hr, TEXT("IDirect3DDevice3::DrawPrimitive"))) {
            goto next_frame;
        }

        // Update the rotation
        hr = g_pd3dDevice->MultiplyTransform(D3DTRANSFORMSTATE_WORLD, &mWorld);
        if (ResultFailed(hr, TEXT("IDirect3DDevice2::MultiplyTransform"))) {
            goto next_frame;
        }

        g_pd3dDevice->SetTexture(0, NULL);
        g_pd3dDevice->SetTexture(1, NULL);

        // Fade the scene to black on exit
        if (g_bQuit && !g_bReset && uFadeCount) {
            FadeOut(ptlrFade, fFade * D3DVAL(uFadeCount-- - 1));
        }

        // End the scene
        hr = g_pd3dDevice->EndScene();
        if (ResultFailed(hr, TEXT("IDirect3DDevice3::EndScene"))) {
            goto next_frame;
        }

        // Update the screen
        hr = FlipFrameBuffer();
        if (FAILED(hr)) {
            goto next_frame;
        }

        vBumpDir.x = D3DVAL(cos(fBumpAngle));
        vBumpDir.z = D3DVAL(sin(fBumpAngle));
        fBumpAngle += (M_PI / 200.0f);
        if (fBumpAngle > M_2PI) {
            fBumpAngle -= M_2PI;
        }

        hr = g_pd3dDevice->SetLightState(D3DLIGHTSTATE_BUMPDIRECTION, (DWORD)&vBumpDir);
        if (ResultFailed(hr, TEXT("IDirect3DDevice3::SetLightState(D3DLIGHTSTATE_BUMPDIRECTION, ...)"))) {
            goto next_frame;
        }

        light.dvDirection = vBumpDir;
        hr = g_pd3dLight->SetLight((LPD3DLIGHT)&light);
        if (ResultFailed(hr, TEXT("IDirect3DLight::SetLight"))) {
            goto next_frame;
        }

next_frame:

        // Update the user input and pump any system messages
        UpdateInput(&g_cam);

        // If rendering failed, determine whether or not to exit
        if (FAILED(hr)) {

            if (hr == DDERR_SURFACELOST) {

                // Rendering failed because video memory got freed
                // End the scene in case rendering failed within the scene
                g_pd3dDevice->EndScene();

                // Restore the video memory and continue rendering
                if (!RestoreFrameBuffer() || 
                    !RestoreTexture(g_pd3dDevice, pd3dt, TEXT("Texture"))) {
                    g_bQuit = true;
                }
            }

            else {

                // Rendering failed for another reason, exit the loop
                g_bQuit = true;
            }
        }
    }

    pd3dm->Release();
    pd3dt->Release();
}

//******************************************************************************
// Support functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     SetView
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
//     true on success, false on failure.
//
//******************************************************************************
bool SetView(PCAMERA pcam) {

    DDSURFACEDESC2  ddsd;
    D3DMATRIX       mTransform;
    HRESULT         hr;

    // Initialize the view matrix
    SetView(&mTransform, &pcam->vPosition, &pcam->vInterest, 
            &D3DVECTOR(D3DVAL(sin(pcam->fRoll)), D3DVAL(cos(pcam->fRoll)), 
            0.0f));

    hr = g_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &mTransform);
    if (ResultFailed(hr, TEXT("IDirect3DDevice3::SetTransform"))) {
        return false;
    }

    // Initialize the projection matrix
    memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    hr = g_pddsBack->GetSurfaceDesc(&ddsd);
    if (ResultFailed(hr, TEXT("IDirectDrawSurface4::GetSurfaceDesc"))) {
        return false;
    }

    SetPerspectiveProjection(&mTransform, pcam->fNearPlane, pcam->fFarPlane, 
            pcam->fFieldOfView, D3DVAL(ddsd.dwHeight) / D3DVAL(ddsd.dwWidth));

    hr = g_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &mTransform);
    if (ResultFailed(hr, TEXT("IDirect3DDevice3::SetTransform"))) {
        return false;
    }

    return true;
}

//******************************************************************************
//
// Function:
//
//     UpdateInput
//
// Description:
//
//     Update input from the user by pumping messages and polling the joystick.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     true on success, false on failure.
//
//******************************************************************************
bool UpdateInput(PCAMERA pcam) {

    MSG                 msg;
    DIJOYSTATE          dijsNext;
    static DIJOYSTATE   dijsLast = {0};
    static LONG         lJoyMaxX = (g_lJoyMaxX - g_lJoyMinX) / 2;
    static LONG         lJoyCenterX = g_lJoyMinX + lJoyMaxX;
    static LONG         lJoyMaxY = (g_lJoyMaxY - g_lJoyMinY) / 2;
    static LONG         lJoyCenterY = g_lJoyMinY + lJoyMaxY;
    static KEYSDOWN     kdLastKeys = 0;
    static D3DVECTOR    vViewTranslation = D3DVECTOR(0.0f);
    static D3DVECTOR    vViewRotation = D3DVECTOR(0.0f);
    static D3DMATRIX    mViewRotationX = D3DMATRIX(
        D3DVAL( 1.0f), D3DVAL( 0.0f), D3DVAL( 0.0f), D3DVAL( 0.0f),
        D3DVAL( 0.0f), D3DVAL( 1.0f), D3DVAL( 0.0f), D3DVAL( 0.0f),
        D3DVAL( 0.0f), D3DVAL( 0.0f), D3DVAL( 1.0f), D3DVAL( 0.0f),
        D3DVAL( 0.0f), D3DVAL( 0.0f), D3DVAL( 0.0f), D3DVAL( 1.0f)
    );
    static D3DMATRIX    mViewRotationY = mViewRotationX;

    // Pump messages
    do {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                g_bQuit = true;
                return false;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    } while (!g_bActive);

    // Poll the keyboard
    if (g_kdKeys != kdLastKeys) {

        if (g_kdKeys & KEY_PAGEDOWN) {
            vViewTranslation.x = -VIEW_TRANSLATION_DELTA;
        }
        else if (g_kdKeys & KEY_DELETE) {
            vViewTranslation.x = VIEW_TRANSLATION_DELTA;
        }
        else {
            vViewTranslation.x = 0.0f;
        }

        if (g_kdKeys & KEY_HOME) {
            vViewTranslation.y = -VIEW_TRANSLATION_DELTA;
        }
        else if (g_kdKeys & KEY_END) {
            vViewTranslation.y = VIEW_TRANSLATION_DELTA;
        }
        else {
            vViewTranslation.y = 0.0f;
        }

        if (g_kdKeys & KEY_ADD || g_kdKeys & KEY_PAGEUP) {
            vViewTranslation.z = -VIEW_TRANSLATION_DELTA;
        }
        else if (g_kdKeys & KEY_SUBTRACT || g_kdKeys & KEY_INSERT) {
            vViewTranslation.z = VIEW_TRANSLATION_DELTA;
        }
        else {
            vViewTranslation.z = 0.0f;
        }

        if (g_kdKeys & KEY_LEFT) {
            vViewRotation.y = -VIEW_ROTATION_DELTA;
        }
        else if (g_kdKeys & KEY_RIGHT) {
            vViewRotation.y = VIEW_ROTATION_DELTA;
        }
        else {
            vViewRotation.y = 0.0f;
        }

        if (g_kdKeys & KEY_UP) {
            vViewRotation.x = -VIEW_ROTATION_DELTA;
        }
        else if (g_kdKeys & KEY_DOWN) {
            vViewRotation.x = VIEW_ROTATION_DELTA;
        }
        else {
            vViewRotation.x = 0.0f;
        }
    }

    kdLastKeys = g_kdKeys;

    // Poll the joystick
    if (GetJoystickState(&dijsNext)) {

        // X Button (Quit)
        if (dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_X]] 
                            != dijsLast.rgbButtons[g_rgbButtons[JOYSTICK_X]]) {
            if (dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_X]] & 0x80) {
                g_bQuit = true;
                return false;
            }
        }

        // Y Button (Toggle specular highlights)
        if (dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_Y]] 
                            != dijsLast.rgbButtons[g_rgbButtons[JOYSTICK_Y]]) {
            if (dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_Y]] & 0x80) {

                BOOL bSpecularOn;
                if (SUCCEEDED(g_pd3dDevice->GetRenderState(
                                    D3DRENDERSTATE_SPECULARENABLE, 
                                    (LPDWORD)&bSpecularOn))) {
                    bSpecularOn = !bSpecularOn;
                    g_pd3dDevice->SetRenderState(
                                    D3DRENDERSTATE_SPECULARENABLE, 
                                    (DWORD)bSpecularOn);
                }
            }
        }

        // A Button (Toggle fill mode)
        if (dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_A]] 
                            != dijsLast.rgbButtons[g_rgbButtons[JOYSTICK_A]]) {
            if (dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_A]] & 0x80) {

                D3DFILLMODE d3dfm;
                if (SUCCEEDED(g_pd3dDevice->GetRenderState(
                                    D3DRENDERSTATE_FILLMODE, (LPDWORD)&d3dfm))) {
                    if (d3dfm == D3DFILL_SOLID) {
                        d3dfm = D3DFILL_WIREFRAME;
                    }
                    else {
                        d3dfm = D3DFILL_SOLID;
                    }
                    g_pd3dDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, 
                                    (DWORD)d3dfm);
                }
            }
        }

        // B Button (Toggle shade mode)
        if (dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_B]] 
                            != dijsLast.rgbButtons[g_rgbButtons[JOYSTICK_B]]) {
            if (dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_B]] & 0x80) {

                D3DSHADEMODE d3dsm;
                if (SUCCEEDED(g_pd3dDevice->GetRenderState(
                                    D3DRENDERSTATE_SHADEMODE, 
                                    (LPDWORD)&d3dsm))) {
                    if (d3dsm == D3DSHADE_GOURAUD) {
                        d3dsm = D3DSHADE_FLAT;
                    }
                    else {
                        d3dsm = D3DSHADE_GOURAUD;
                    }
                    g_pd3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, 
                                    (DWORD)d3dsm);
                }
            }
        }

        // Left keypad
        if (dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_LA]] 
                            != dijsLast.rgbButtons[g_rgbButtons[JOYSTICK_LA]]) {
            if (dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_LA]] & 0x80) {
                vViewTranslation.x = VIEW_TRANSLATION_DELTA;
            }
            else {
                vViewTranslation.x = 0.0f;
            }
        }

        // Right keypad
        if (dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_RA]] 
                            != dijsLast.rgbButtons[g_rgbButtons[JOYSTICK_RA]]) {
            if (dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_RA]] & 0x80) {
                vViewTranslation.x = -VIEW_TRANSLATION_DELTA;
            }
            else {
                vViewTranslation.x = 0.0f;
            }
        }

        // Up keypad
        if (dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_UA]] 
                            != dijsLast.rgbButtons[g_rgbButtons[JOYSTICK_UA]]) {
            if (dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_UA]] & 0x80) {
                vViewTranslation.y = -VIEW_TRANSLATION_DELTA;
            }
            else {
                vViewTranslation.y = 0.0f;
            }
        }

        // Down keypad
        if (dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_DA]] 
                            != dijsLast.rgbButtons[g_rgbButtons[JOYSTICK_DA]]) {
            if (dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_DA]] & 0x80) {
                vViewTranslation.y = VIEW_TRANSLATION_DELTA;
            }
            else {
                vViewTranslation.y = 0.0f;
            }
        }

        // Left trigger
        if (dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_LTRIG]] 
                            != dijsLast.rgbButtons[g_rgbButtons[JOYSTICK_LTRIG]]) {
            vViewTranslation.z = (D3DVAL(dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_LTRIG]]) 
                                / D3DVAL(0xFF)) * VIEW_TRANSLATION_DELTA;
        }

        // Right trigger
        if (dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_RTRIG]] 
                            != dijsLast.rgbButtons[g_rgbButtons[JOYSTICK_RTRIG]]) {

            vViewTranslation.z = (D3DVAL(dijsNext.rgbButtons[g_rgbButtons[JOYSTICK_RTRIG]]) 
                                / D3DVAL(0xFF)) * -VIEW_TRANSLATION_DELTA;
        }

        // Joystick x-axis
        if (dijsNext.lX != dijsLast.lX) {
            vViewRotation.y = D3DVAL(dijsNext.lX - lJoyCenterX) 
                                / D3DVAL(lJoyMaxX) * VIEW_ROTATION_DELTA;
        }

        // Joystick y-axis
        if (dijsNext.lY != dijsLast.lY) {
            vViewRotation.x = D3DVAL(dijsNext.lY - lJoyCenterY) 
                                / D3DVAL(lJoyMaxY) * VIEW_ROTATION_DELTA;
        }
    }

    memcpy(&dijsLast, &dijsNext, sizeof(DIJOYSTATE));

    if (pcam) {

        D3DMATRIX mView, mViewTransform;
        D3DVECTOR vTranslation, vDirection, vUp, vProj, vCross;
        D3DVALUE fMagnitude;

        mViewRotationY._11 = D3DVAL(cos(vViewRotation.y));
        mViewRotationY._13 = D3DVAL(-sin(vViewRotation.y));
        mViewRotationY._31 = -mViewRotationY._13;
        mViewRotationY._33 = mViewRotationY._11;

        mViewRotationX._22 = D3DVAL(cos(vViewRotation.x));
        mViewRotationX._23 = D3DVAL(sin(vViewRotation.x));
        mViewRotationX._32 = -mViewRotationX._23;
        mViewRotationX._33 = mViewRotationX._22;

        vDirection = pcam->vInterest - pcam->vPosition;
        fMagnitude = Magnitude(vDirection);
        vDirection = Normalize(vDirection);
        vUp = D3DVECTOR(D3DVAL(sin(pcam->fRoll)), D3DVAL(cos(pcam->fRoll)), 0.0f);
        vProj = Normalize(vUp - (vDirection * DotProduct(vUp, vDirection)));
        vCross = CrossProduct(vProj, vDirection);

        InitMatrix(&mViewTransform,
            D3DVAL(vCross.x), D3DVAL(vCross.y), D3DVAL(vCross.z), D3DVAL( 0.0f),
            D3DVAL(vProj.x), D3DVAL(vProj.y), D3DVAL(vProj.z), D3DVAL( 0.0f),
            D3DVAL(vDirection.x), D3DVAL(vDirection.y), D3DVAL(vDirection.z), D3DVAL( 0.0f),
            D3DVAL( 0.0f), D3DVAL( 0.0f), D3DVAL( 0.0f), D3DVAL( 1.0f)
        );

        MultiplyMatrix(&mView, &mViewRotationX, &mViewTransform);
        MultiplyMatrix(&mViewTransform, &mViewRotationY, &mView);
        pcam->vInterest = MultiplyVectorMatrix(&D3DVECTOR(0.0f, 0.0f, fMagnitude), &mViewTransform)
                            + pcam->vPosition;
        vTranslation = MultiplyVectorMatrix(&vViewTranslation, &mViewTransform);
        pcam->vPosition -= vTranslation;
        pcam->vInterest -= vTranslation;

        SetView(&mView, &pcam->vPosition, &pcam->vInterest, &vUp);

        g_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &mView);
    }

    return true;
}

//******************************************************************************
bool CreateSphere(D3DVALUE fRadius, LPD3DVERTEX* pprVertices, LPDWORD pdwNumVertices) {

    LPD3DVERTEX prVertices, prFinal, pr;
    UINT        uNumVertices;
    UINT        uOffset = 0;
    UINT        uStepsU = SPHERE_U, uStepsV = SPHERE_V;
    UINT        i, j;
    D3DVALUE    fX, fY, fTX, fSinY, fCosY;

    if (!pprVertices || !pdwNumVertices) {
        return false;
    }

    *pprVertices = NULL;
    *pdwNumVertices = 0;

    // Allocate a temporary vertex buffer
    prVertices = new D3DVERTEX[uStepsU * uStepsV];
    if (!prVertices) {
        return false;
    }

    // Allocate the vertex memory
    uNumVertices = (uStepsV - 1) * (uStepsU + 1) * 2 - 2;

    prFinal = (LPD3DVERTEX)MemAlloc32((uNumVertices + 1) * sizeof(D3DVERTEX));
    if (!prFinal) {
        delete [] prVertices;
        return false;
    }

    // Create the sphere
    for (j = 0; j < uStepsV; j++) {

        fY = D3DVAL(j) / D3DVAL(uStepsV - 1);
        fSinY = D3DVAL(sin(fY * M_PI));
        fCosY = D3DVAL(cos(fY * M_PI));

        for (i = 0; i < uStepsU; i++) {

            pr = &prVertices[uStepsU * j + i];
            fX = D3DVAL(i) / D3DVAL(uStepsU - 1);
            fTX = fX * M_2PI;

            pr->nx = D3DVAL(sin(fTX)) * fSinY;
            pr->ny = fCosY;
            pr->nz = -D3DVAL(cos(fTX)) * fSinY;
            pr->x = pr->nx * fRadius;
            pr->y = pr->ny * fRadius;
            pr->z = pr->nz * fRadius;
            pr->tu = fX;
            pr->tv = fY;
        }
    }

    uOffset = 0;

    for (j = 0; j < uStepsV - 1; j++) {

        for (i = 0; i < uStepsU; i++) {
            prFinal[uOffset++] = prVertices[(j + 1) * uStepsU + i];
            prFinal[uOffset++] = prVertices[j * uStepsU + i];
        }

        if (j < uStepsV - 2) {
            prFinal[uOffset++] = prVertices[(j + 2) * uStepsU - 1];
            prFinal[uOffset++] = prVertices[(j + 2) * uStepsU - 1];
        }
    }

    delete [] prVertices;

    *pprVertices = prFinal;
    *pdwNumVertices = uNumVertices;

    return true;
}

//******************************************************************************
void ReleaseSphere(LPD3DVERTEX pr) {

    MemFree32(pr);
}

//******************************************************************************
//
// Function:
//
//     FadeOut
//
// Description:
//
//     Fade the scene to black.
//
// Arguments:
//
//     LPD3DTLVERTEX ptlr           - Pointer to the vertices of the quad that
//                                    will be used to fade out the scene
//
//     D3DVALUE fAlpha              - Alpha amount to fade the scene out
//                                    (where 1.0 will have no effect and 0.0
//                                    will completely fade to black)
//
// Return Value:
//
//     true on success, false on failure.
//
//******************************************************************************
bool FadeOut(LPD3DTLVERTEX ptlr, D3DVALUE fAlpha) {

    LPDIRECT3DTEXTURE2 pd3dtCurrent;
    DWORD              dwSrcBlend, dwDstBlend, dwBlend, dwColorOp0, dwColorOp1, 
                       dwAlphaOp0, dwColorArg1, dwColorArg2, dwAlphaArg1, dwAlphaArg2;

    // Fade out the scene
    ptlr[0].color = ptlr[1].color = ptlr[2].color = ptlr[3].color 
                  = ((255 - (UINT)(fAlpha * 256.0f)) << 24);
    g_pd3dDevice->GetRenderState(D3DRENDERSTATE_SRCBLEND, &dwSrcBlend);
    g_pd3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
    g_pd3dDevice->GetRenderState(D3DRENDERSTATE_DESTBLEND, &dwDstBlend);
    g_pd3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
    g_pd3dDevice->GetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, &dwBlend);
    g_pd3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    g_pd3dDevice->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp0);
    g_pd3dDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &dwColorArg1);
    g_pd3dDevice->GetTextureStageState(0, D3DTSS_COLORARG2, &dwColorArg2);
    SetColorStage(0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG2);
    g_pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAlphaOp0);
    g_pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAlphaArg1);
    g_pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAARG2, &dwAlphaArg2);
    SetAlphaStage(0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG2);
    g_pd3dDevice->GetTextureStageState(1, D3DTSS_COLOROP, &dwColorOp1);
    g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    g_pd3dDevice->GetTexture(0, &pd3dtCurrent);
    g_pd3dDevice->SetTexture(0, NULL);
    g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, 
                                ptlr, 4, D3DDP_DONOTUPDATEEXTENTS);
    g_pd3dDevice->SetTexture(0, pd3dtCurrent);
    SetColorStage(0, dwColorArg1, dwColorArg2, (D3DTEXTUREOP)dwColorOp0);
    SetAlphaStage(0, dwAlphaArg1, dwAlphaArg2, (D3DTEXTUREOP)dwAlphaOp0);
    g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, dwColorOp1);
    g_pd3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, dwBlend);
    g_pd3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, dwSrcBlend);
    g_pd3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, dwDstBlend);
    return true;
}
