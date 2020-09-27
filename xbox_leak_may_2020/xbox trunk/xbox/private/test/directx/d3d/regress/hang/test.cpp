/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    test.cpp

Description:

    Test functions.

*******************************************************************************/

#include <xtl.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <xgraphics.h>
#include <xgmath.h>
#include "test.h"

//******************************************************************************
_MVERTEX::_MVERTEX(const D3DXVECTOR3& v, const D3DXVECTOR3& n, float _u0, 
                   float _v0, float _u1, float _v1)
{
    vPosition = v;
    vNormal = n;
    u0 = _u0;
    v0 = _v0;
    u1 = _u1;
    v1 = _v1;
}

//******************************************************************************
// main
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     main
//
// Description:
//
//     Entry point for the application.
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
void __cdecl main()
{
    CTest* pTest;

    // Seed the pseudo-random number generator
    srand(GetTickCount());

    pTest = new CTest();
    if (!pTest) {
        return;
    }

    if (!pTest->Create()) {
        DebugString(TEXT("CTest::Create failed"));
        delete pTest;
        return;
    }

    pTest->Run();

    delete pTest;
}

//******************************************************************************
CTest::CTest() {

    m_pd3d = NULL;
    m_pDevice = NULL;
    m_pd3dr = NULL;

    m_pd3dt1 = NULL;
    m_pd3dt2 = NULL;

    m_prVertices[0] = MVERTEX(D3DXVECTOR3(-10.0f,-10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f, 0.0f, 1.0f);
    m_prVertices[1] = MVERTEX(D3DXVECTOR3(-10.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f, 0.0f, 0.0f);
    m_prVertices[2] = MVERTEX(D3DXVECTOR3( 10.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f, 1.0f, 0.0f);
    m_prVertices[3] = MVERTEX(D3DXVECTOR3( 10.0f,-10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f, 1.0f, 1.0f);

    D3DXMatrixIdentity(&m_mTexture0);
    D3DXMatrixIdentity(&m_mTexture1);
    m_mTexture0._32 += 0.005f;
    m_mTexture1._31 += 0.005f;
}

//******************************************************************************
CTest::~CTest() {

    if (m_pd3d) {
        m_pd3d->Release();
        m_pd3d = NULL;
    }
}

//******************************************************************************
BOOL CTest::Create() {

    m_pd3d = Direct3DCreate8(D3D_SDK_VERSION);
    if (!m_pd3d) {
        DebugString(TEXT("Direct3DCreate8 failed"));
        return FALSE;
    }

    // Initialize the presentation parameters
    memset(&m_d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    m_d3dpp.BackBufferWidth                   = 640;
    m_d3dpp.BackBufferHeight                  = 480;
    m_d3dpp.BackBufferFormat                  = D3DFMT_A8R8G8B8;
    m_d3dpp.MultiSampleType                   = D3DMULTISAMPLE_NONE;
    m_d3dpp.AutoDepthStencilFormat            = D3DFMT_D24S8;
    m_d3dpp.FullScreen_RefreshRateInHz        = 0;
    m_d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_ONE;
    m_d3dpp.BackBufferCount                   = 1;
    m_d3dpp.SwapEffect                        = D3DSWAPEFFECT_DISCARD;
    m_d3dpp.hDeviceWindow                     = NULL;
    m_d3dpp.Windowed                          = FALSE;
    m_d3dpp.EnableAutoDepthStencil            = TRUE;

    // Initialize a directional light
    memset(&m_light, 0, sizeof(D3DLIGHT8));
    m_light.Type = D3DLIGHT_DIRECTIONAL;
    m_light.Diffuse.r = 1.0f;
    m_light.Diffuse.g = 1.0f;
    m_light.Diffuse.b = 1.0f;
    m_light.Diffuse.a = 1.0f;
    m_light.Specular.r = 1.0f;
    m_light.Specular.g = 1.0f;
    m_light.Specular.b = 1.0f;
    m_light.Specular.a = 1.0f;
    m_light.Ambient.r = 1.0f;
    m_light.Ambient.g = 1.0f;
    m_light.Ambient.b = 1.0f;
    m_light.Ambient.a = 1.0f;
    m_light.Direction = D3DXVECTOR3(0.0f, -1.0f, 1.0f);

    // Initialize a material
    memset(&m_material, 0, sizeof(D3DMATERIAL8));
    m_material.Diffuse.r = 1.0f;
    m_material.Diffuse.g = 1.0f;
    m_material.Diffuse.b = 1.0f;
    m_material.Diffuse.a = 1.0f;
    m_material.Specular.r = 1.0f;
    m_material.Specular.g = 1.0f;
    m_material.Specular.b = 1.0f;
    m_material.Specular.a = 1.0f;
    m_material.Ambient.r = 0.2f;
    m_material.Ambient.g = 0.2f;
    m_material.Ambient.b = 0.2f;
    m_material.Ambient.a = 0.2f;

    return TRUE;
}

//******************************************************************************
BOOL CTest::StartGraphics() {

    static D3DVIEWPORT8     viewport = {0, 0, 640, 480, 0.0f, 1.0f};
    XGMATRIX                mTransform;
    HRESULT                 hr;

    // Create the device
    hr = m_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_d3dpp, &m_pDevice);
    if (ResultFailed(hr, TEXT("IDirect3D8::CreateDevice failed"))) {
        return FALSE;
    }

    // Set a viewport
    hr = m_pDevice->SetViewport(&viewport);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetViewport"))) {
        return FALSE;
    }

    // Initialize the view matrix
    XGMatrixLookAtLH(&mTransform, &XGVECTOR3(0.0f, 0.0f, -50.0f), &XGVECTOR3(0.0f, 0.0f, 0.0f), &XGVECTOR3(0.0f, 1.0f, 0.0f));
    hr = m_pDevice->SetTransform(D3DTS_VIEW, &mTransform);
    if (FAILED(hr)) {
        DebugString(TEXT("IDirect3DDevice8::SetTransform failed"));
    }

    // Initialize the projection matrix
    XGMatrixPerspectiveFovLH(&mTransform, 3.14159f / 4.0f, 640.0f / 480.0f, 0.1f, 1000.0f);
    hr = m_pDevice->SetTransform(D3DTS_PROJECTION, &mTransform);
    if (FAILED(hr)) {
        DebugString(TEXT("IDirect3DDevice8::SetTransform failed"));
    }

    hr = m_pDevice->CreateTexture(256, 256, 1, 0, D3DFMT_A1R5G5B5, 0, &m_pd3dt1);
    if (FAILED(hr)) {
        return FALSE;
    }

    hr = m_pDevice->CreateTexture(256, 256, 1, 0, D3DFMT_A1R5G5B5, 0, &m_pd3dt2);
    if (FAILED(hr)) {
        return FALSE;
    }

    hr = m_pDevice->SetLight(0, &m_light);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetLight"))) {
        return FALSE;
    }

    hr = m_pDevice->LightEnable(0, TRUE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::LightEnable"))) {
        return FALSE;
    }

    hr = m_pDevice->SetMaterial(&m_material);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetMaterial"))) {
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
void CTest::StopGraphics() {

    if (m_pd3dt1) {
        m_pd3dt1->Release();
        m_pd3dt1 = NULL;
    }
    if (m_pd3dt2) {
        m_pd3dt2->Release();
        m_pd3dt2 = NULL;
    }
    if (m_pDevice) {
        m_pDevice->Release();
        m_pDevice = NULL;
    }
}

//******************************************************************************
BOOL CTest::Run() {

    float fRunningTime = 0.0f, fTime, fLastTime, fTimeFreq, fFPS, fPPS;
    LARGE_INTEGER qwCounter;

    QueryPerformanceFrequency(&qwCounter);
    fTimeFreq = 1.0f / (float)qwCounter.QuadPart;
    QueryPerformanceCounter(&qwCounter);
    fLastTime = (float)qwCounter.QuadPart * fTimeFreq;

    do {

        StartGraphics();

        // Clear the rendering target
        m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

        // Begin the scene
        m_pDevice->BeginScene();

        // Disable lighting
        m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	    m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	    m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	    m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);

        m_pDevice->SetTexture(0, m_pd3dt1);
        m_pDevice->SetTexture(1, m_pd3dt2);

	    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	    m_pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);

        m_pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
        m_pDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

        m_pDevice->SetVertexShader(FVF_MVERTEX);

        m_pDevice->SetTransform(D3DTS_TEXTURE0, &m_mTexture0);
        m_pDevice->SetTransform(D3DTS_TEXTURE1, &m_mTexture1);

        m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prVertices, sizeof(MVERTEX));

        // End the scene
        m_pDevice->EndScene();

        // Update the screen
        m_pDevice->Present(NULL, NULL, NULL, NULL);

        QueryPerformanceCounter(&qwCounter);
        fTime = (float)qwCounter.QuadPart * fTimeFreq;

        if (fTime - fLastTime > 1.0f) {
            fRunningTime += (fTime - fLastTime);
            DebugString(TEXT("The system has been running for %3.3f seconds"), fRunningTime);
            QueryPerformanceCounter(&qwCounter);
            fLastTime = (float)qwCounter.QuadPart * fTimeFreq;
        }

        StopGraphics();

    } while (TRUE);

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     DebugString
//
// Description:
//
//     Take the formatted output and send the output to the debugger.
//
// Arguments:
//
//     LPCTSTR szFormat         - Formatting string describing the output
//
//     Variable argument list   - Data to be placed in the output string
//
// Return Value:
//
//     None.
//
//******************************************************************************
void DebugString(LPCTSTR szFormat, ...) {

    va_list vl;
    TCHAR*  szBuffer = (LPTSTR)HeapAlloc(GetProcessHeap(), 0, 2304 * sizeof(TCHAR));
    if (!szBuffer) {
        OutputDebugString(TEXT("Insufficient memory for logging buffer allocation\n"));
        __asm int 3;
    }

    va_start(vl, szFormat);
    _vstprintf(szBuffer, szFormat, vl);
    va_end(vl);

    _tcscat(szBuffer, TEXT("\r\n"));

    OutputDebugString(szBuffer);

    HeapFree(GetProcessHeap(), 0, szBuffer);
}

//******************************************************************************
//
// Function:
//
//     ResultFailed
//
// Description:
//
//     Test a given return code: if the code is an error, output a debug 
//     message with the error value.  If the code is a warning, output
//     a debug message with the warning value.
//
// Arguments:
//
//     HRESULT hr               - Return code to test for an error
//
//     LPCTSTR sz               - String describing the method that produced 
//                                the return code
//
// Return Value:
//
//     TRUE if the given return code is an error, FALSE otherwise.
//
//******************************************************************************
BOOL ResultFailed(HRESULT hr, LPCTSTR sz) {

    TCHAR szError[256];

    if (SUCCEEDED(hr)) {
        return FALSE;
    }

//    XGGetErrorString(hr, szError, 256);

    if (FAILED(hr)) {
        DebugString(TEXT("%s failed with %s [0x%X]"), sz, szError, hr);
        return TRUE;
    }
    else {
        DebugString(TEXT("%s returned %s [0x%X]"), sz, szError, hr);
    }

    return FALSE;
}
