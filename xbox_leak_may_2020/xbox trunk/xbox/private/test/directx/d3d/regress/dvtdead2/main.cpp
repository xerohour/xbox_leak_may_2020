/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    test.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include <xtl.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <d3dx8.h>
#include <xgraphics.h>

//******************************************************************************
#define FVF_LVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)

#define RGBA_MAKE(r, g, b, a)   ((D3DCOLOR) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))

//******************************************************************************
typedef struct _LVERTEX {
    D3DXVECTOR3 vPosition;
    D3DCOLOR    cDiffuse;
    D3DCOLOR    cSpecular;
    float       u0, v0;

    _LVERTEX() { }
    _LVERTEX(const D3DVECTOR& v, D3DCOLOR _cDiffuse, D3DCOLOR _cSpecular, float _u0, float _v0) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        cDiffuse = _cDiffuse; cSpecular = _cSpecular;
        u0 = _u0; v0 = _v0;
    }
} LVERTEX, *PLVERTEX;

//******************************************************************************
class CReproTest {

protected:

    LPDIRECT3D8                     m_pd3d;
    LPDIRECT3DDEVICE8               m_pDevice;
    LPDIRECT3DVERTEXBUFFER8         m_pd3dr;
    DWORD                           m_dwVShader;

public:

                                    CReproTest();
                                    ~CReproTest();

    virtual BOOL                    Create();
    virtual BOOL                    Run();

protected:

    virtual BOOL                    InitDeviceState();
    virtual BOOL                    SetViewport(DWORD dwWidth, DWORD dwHeight);
};

//******************************************************************************
void InitMatrix(D3DMATRIX* pd3dm,
         float _11, float _12, float _13, float _14,
         float _21, float _22, float _23, float _24,
         float _31, float _32, float _33, float _34,
         float _41, float _42, float _43, float _44);

void SetView(LPD3DXMATRIX pd3dm, LPD3DXVECTOR3 pvPos, 
         LPD3DXVECTOR3 pvAt, LPD3DXVECTOR3 pvUp);

void SetPerspectiveProjection(D3DMATRIX* pd3dm, float fFront, 
         float fBack, float fFieldOfView, float fAspect);

void DebugString(LPCTSTR szFormat, ...);

//******************************************************************************
// WinMain
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     WinMain
//
// Description:
//
//     Entry point for the application.
//
// Arguments:
//
//     HINSTANCE hInstance      - Application instance handle
//
//     HINSTANCE hPrevInstance  - Previous instance of the application (always
//                                NULL for Win32 applications)
//
//     LPSTR szCmdLine          - Pointer to a string specifying the command
//                                line used in launching the application
//                                (excluding the program name)
//
//     int nCmdShow             - State specifying how the window is to be 
//                                displayed on creation
//
// Return Value:
//
//     0 on success, -1 on failure.
//
//******************************************************************************
void __cdecl main()
{
    CReproTest* pRTest;

    // Create the display
    pRTest = new CReproTest();
    if (!pRTest) {
        return;
    }

    // Initialize the display
    if (!pRTest->Create()) {
        DebugString(TEXT("CDisplay::Create failed"));
        delete pRTest;
        return;
    }

    // Render the scene
    pRTest->Run();

    // Clean up
    delete pRTest;
}

//******************************************************************************
CReproTest::CReproTest() {

    m_pd3d = NULL;
    m_pDevice = NULL;
    m_pd3dr = NULL;
    m_dwVShader = 0xFFFFFFFF;
}

//******************************************************************************
CReproTest::~CReproTest() {

    if (m_dwVShader != 0xFFFFFFFF) {
        m_pDevice->SetVertexShader(D3DFVF_XYZ);
        m_pDevice->DeleteVertexShader(m_dwVShader);
    }

    if (m_pd3dr) {
        m_pd3dr->Release();
        m_pd3dr = NULL;
    }

    if (m_pDevice) {
        m_pDevice->Release();
        m_pDevice = NULL;
    }

    if (m_pd3d) {
        m_pd3d->Release();
        m_pd3d = NULL;
    }
}

//******************************************************************************
BOOL CReproTest::Create() {

    D3DPRESENT_PARAMETERS   d3dpp;
    LVERTEX                 plrVertices[3];
    LPBYTE                  pData;
    HRESULT                 hr;
    UINT                    i;
    LPXGBUFFER              pxgbufShader;
    DWORD                   dwSDecl[] = {
                                D3DVSD_STREAM(0),
                                D3DVSD_REG(0, D3DVSDT_FLOAT3),
                                D3DVSD_REG(1, D3DVSDT_D3DCOLOR),
                                D3DVSD_REG(2, D3DVSDT_D3DCOLOR),
                                D3DVSD_REG(3, D3DVSDT_FLOAT2),
                                D3DVSD_END()
                            };
    char                    szVShader[] = {
                                "vs.1.0\n"
                                "dp4 oPos.x, v0, c0\n"
                                "dp4 oPos.y, v0, c1\n"
                                "dp4 oPos.z, v0, c2\n"
                                "dp4 oPos.w, v0, c3\n"
                                "mov oD0, v1\n"
                            };

    m_pd3d = Direct3DCreate8(D3D_SDK_VERSION);
    if (!m_pd3d) {
        DebugString(TEXT("Direct3DCreate8 failed"));
    }

    // Initialize the presentation parameters
    memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    d3dpp.BackBufferWidth                   = 640;
    d3dpp.BackBufferHeight                  = 480;
    d3dpp.BackBufferFormat                  = D3DFMT_LIN_A8R8G8B8;
    d3dpp.MultiSampleType                   = D3DMULTISAMPLE_NONE;
    d3dpp.AutoDepthStencilFormat            = D3DFMT_LIN_D24S8;
    d3dpp.FullScreen_RefreshRateInHz        = 0;
    d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.BackBufferCount                   = 1;
    d3dpp.SwapEffect                        = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                     = NULL;
    d3dpp.Windowed                          = FALSE;
    d3dpp.EnableAutoDepthStencil            = TRUE;

    // Create the device
    m_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_pDevice);

    plrVertices[0] = LVERTEX(D3DXVECTOR3(-0.25f, -0.25f, 0.0f), RGBA_MAKE(255, 0, 0, 255), 0, 0.0f, 0.0f);
    plrVertices[1] = LVERTEX(D3DXVECTOR3( 0.0f,  0.25f, 0.0f), RGBA_MAKE(0, 0, 255, 255), 0, 0.0f, 0.0f);
    plrVertices[2] = LVERTEX(D3DXVECTOR3( 0.25f, -0.25f, 0.0f), RGBA_MAKE(0, 255, 0, 255), 0, 0.0f, 0.0f);

    m_pDevice->CreateVertexBuffer(60000 * sizeof(LVERTEX), 0, FVF_LVERTEX, D3DPOOL_DEFAULT, &m_pd3dr);

    m_pd3dr->Lock(0, 0, &pData, 0);

    for (i = 0; i < 20000; i++) {
        memcpy(pData, plrVertices, 3 * sizeof(LVERTEX));
        pData += 3 * sizeof(LVERTEX);
    }

    m_pd3dr->Unlock();

    AssembleShader("", szVShader, strlen(szVShader), 0, NULL, 
                        &pxgbufShader, NULL, NULL, NULL, NULL, NULL);

    m_pDevice->CreateVertexShader(dwSDecl, (LPDWORD)pxgbufShader->GetBufferPointer(), &m_dwVShader, 0);

    pxgbufShader->Release();

    return InitDeviceState();
}

//******************************************************************************
BOOL CReproTest::Run() {

    D3DXMATRIX  mView, mProj, mTransform;
    HRESULT     hr;
    UINT        i;
    static UINT uFrame = 0;

    do {

        if (++uFrame % 100 == 0) {
            DebugString(TEXT("Rendering frame %d"), uFrame);
        }

        // Clear the rendering target
        m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);

        // Begin the scene
        m_pDevice->BeginScene();

        m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
        m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
        m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        m_pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

        SetView(&mView, &D3DXVECTOR3(0.0f, 0.0f, -50.0f), &D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
                &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

        SetPerspectiveProjection(&mProj, 0.1f, 1000.0f, 
                3.14159f / 4.0f, 480.0f / 640.0f);

        D3DXMatrixMultiply(&mTransform, &mView, &mProj);
        D3DXMatrixTranspose(&mTransform, &mTransform);

        m_pDevice->SetVertexShaderConstant(0, &mTransform, 4);

        m_pDevice->SetVertexShader(m_dwVShader);

        m_pDevice->SetStreamSource(0, m_pd3dr, sizeof(LVERTEX));

        m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 20000);

        // End the scene
        m_pDevice->EndScene();

        // Update the screen
        m_pDevice->Present(NULL, NULL, NULL, NULL);

    } while (TRUE);

    return TRUE;
}

//******************************************************************************
BOOL CReproTest::InitDeviceState() {

    D3DXMATRIX mTransform;
    UINT       i;
    HRESULT    hr;

    // Initialize the viewport
    if (!SetViewport(640, 480)) {
        DebugString(TEXT("SetViewport failed"));
    }

    // Initialize the view matrix
    SetView(&mTransform, &D3DXVECTOR3(0.0f, 0.0f, -50.0f), &D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
            &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

    m_pDevice->SetTransform(D3DTS_VIEW, &mTransform);

    // Initialize the projection matrix
    SetPerspectiveProjection(&mTransform, 0.1f, 1000.0f, 
            3.14159f / 4.0f, 480.0f / 640.0f);

    m_pDevice->SetTransform(D3DTS_PROJECTION, &mTransform);

    m_pDevice->SetRenderState(D3DRS_AMBIENT, 0x20202020);
    m_pDevice->SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, (DWORD)TRUE);
    m_pDevice->SetRenderState(D3DRS_DITHERENABLE, (DWORD)TRUE);
    m_pDevice->SetRenderState(D3DRS_COLORVERTEX, (DWORD)FALSE);

    return TRUE;
}

//******************************************************************************
BOOL CReproTest::SetViewport(DWORD dwWidth, DWORD dwHeight) {

    D3DVIEWPORT8    viewport;
    HRESULT         hr;

    // Set a viewport for the device
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Width = dwWidth;
    viewport.Height = dwHeight;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;

    m_pDevice->SetViewport(&viewport);

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     InitMatrix
//
// Description:
//
//     Initialize the given matrix with the given values.
//
// Arguments:
//
//     LPD3DMATRIX pd3dm        - Matrix to initialize
//
//     float _11 to _44         - Initialization values
//
// Return Value:
//
//     None.
//
//******************************************************************************
void InitMatrix(D3DMATRIX* pd3dm,
                float _11, float _12, float _13, float _14,
                float _21, float _22, float _23, float _24,
                float _31, float _32, float _33, float _34,
                float _41, float _42, float _43, float _44) 
{
    if (pd3dm) {

        pd3dm->_11 = _11;
        pd3dm->_12 = _12;
        pd3dm->_13 = _13;
        pd3dm->_14 = _14;
        pd3dm->_21 = _21;
        pd3dm->_22 = _22;
        pd3dm->_23 = _23;
        pd3dm->_24 = _24;
        pd3dm->_31 = _31;
        pd3dm->_32 = _32;
        pd3dm->_33 = _33;
        pd3dm->_34 = _34;
        pd3dm->_41 = _41;
        pd3dm->_42 = _42;
        pd3dm->_43 = _43;
        pd3dm->_44 = _44;
    }
}

//******************************************************************************
//
// Function:
//
//     SetView
//
// Description:
//
//     Align the given view matrix along the given direction and up vectors with
//     the position vector as the origin.
//
// Arguments:
//
//     LPD3DXMATRIX pd3dm       - View matrix to initialize
//
//     LPD3DXVECTOR3 pvPos      - View origin
//
//     LPD3DXVECTOR3 pvAt       - View interest vector
//
//     LPD3DXVECTOR3 pvUp       - View up vector
//
// Return Value:
//
//     Initialized view matrix.
//
//******************************************************************************
void SetView(LPD3DXMATRIX pd3dm, LPD3DXVECTOR3 pvPos, LPD3DXVECTOR3 pvAt, 
                                                      LPD3DXVECTOR3 pvUp) 
{
    D3DXVECTOR3  d, u, r;

    if (pd3dm && pvPos && pvAt && pvUp && !(*pvPos == *pvAt) 
                                 && !(*pvUp == D3DXVECTOR3(0.0f, 0.0f, 0.0f))) {

        D3DXVec3Normalize(&d, &(*pvAt - *pvPos));
        u = *pvUp;

        // Project the up vector into the plane of the direction vector
        D3DXVec3Normalize(&u, &(u - (d * D3DXVec3Dot(&u, &d))));

        // Get the cross product
        D3DXVec3Cross(&r, &u, &d);

        // Initialize the view transform
        InitMatrix(pd3dm,
            r.x, u.x, d.x, 0.0f,
            r.y, u.y, d.y, 0.0f,
            r.z, u.z, d.z, 0.0f,
            -(pvPos->x * r.x + pvPos->y * r.y + pvPos->z * r.z), 
            -(pvPos->x * u.x + pvPos->y * u.y + pvPos->z * u.z), 
            -(pvPos->x * d.x + pvPos->y * d.y + pvPos->z * d.z), 
            1.0f
        );
    }
}

//******************************************************************************
//
// Function:
//
//     SetPerspectiveProjection
//
// Description:
//
//     Initialize the given projection matrix using the given front and back
//     clipping planes, field of view, and aspect ratio.
//
// Arguments:
//
//     LPD3DMATRIX pd3dm        - Projection matrix to initialize
//
//     float fFront             - Front clipping plane
//
//     float fBack              - Back clipping plane
//
//     float fFieldOfView       - Angle, in radians, of the field of view
//
//     float fAspect            - Aspect ratio (y / x) of the view plane
//
// Return Value:
//
//     Initialized projection matrix.
//
//******************************************************************************
void SetPerspectiveProjection(D3DMATRIX* pd3dm, float fFront, float fBack, 
                                    float fFieldOfView, float fAspect) {

    float fTanHalfFOV = (float)tan((double)fFieldOfView / 2.0);
    float fFar = fBack / (fBack - fFront);

    InitMatrix(pd3dm,
        1.0f, 0.0f,           0.0f,                         0.0f,
        0.0f, 1.0f / fAspect, 0.0f,                         0.0f,
        0.0f, 0.0f,           fTanHalfFOV * fFar,           fTanHalfFOV,
        0.0f, 0.0f,           -fFront * fTanHalfFOV * fFar, 0.0f
    );
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
    TCHAR* szBuffer = new TCHAR[2304];
    if (!szBuffer) {
        OutputDebugString(TEXT("Insufficient memory for logging buffer allocation\n"));
        __asm int 3;
    }

    va_start(vl, szFormat);
    _vstprintf(szBuffer, szFormat, vl);
    va_end(vl);

    _tcscat(szBuffer, TEXT("\r\n"));

    OutputDebugString(szBuffer);

    delete [] szBuffer;
}
