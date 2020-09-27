/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    ball.cpp

*******************************************************************************/

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "ball.h"

struct MATRIXMODE
{
    D3DTRANSFORMSTATETYPE   dwFirstMatrix;
    D3DTRANSFORMSTATETYPE   dwSecondMatrix;
    DWORD                   dwVertexType;
    DWORD                   dwMatrixMode;
    LPSTR                   pszModeName;
};

MATRIXMODE  m_pBallMode[] = 
{
    { D3DTRANSFORMSTATE_WORLD,  D3DTRANSFORMSTATE_WORLD1, D3DFVF_VERTEXB1, D3DVBLEND_1WEIGHT,  "1,2"},
    { D3DTRANSFORMSTATE_WORLD,  D3DTRANSFORMSTATE_WORLD2, D3DFVF_VERTEXB2, D3DVBLEND_2WEIGHTS, "1,3"},
    { D3DTRANSFORMSTATE_WORLD,  D3DTRANSFORMSTATE_WORLD3, D3DFVF_VERTEXB3, D3DVBLEND_3WEIGHTS, "1,4"},
    { D3DTRANSFORMSTATE_WORLD1, D3DTRANSFORMSTATE_WORLD2, D3DFVF_VERTEXB2, D3DVBLEND_2WEIGHTS, "2,3"},
    { D3DTRANSFORMSTATE_WORLD1, D3DTRANSFORMSTATE_WORLD3, D3DFVF_VERTEXB3, D3DVBLEND_3WEIGHTS, "2,4"},
    { D3DTRANSFORMSTATE_WORLD2, D3DTRANSFORMSTATE_WORLD3, D3DFVF_VERTEXB3, D3DVBLEND_3WEIGHTS, "3,4"}
};

//******************************************************************************
//
// Function:
//
//     ValidateDisplay
//
// Description:
//
//     Evaluate the given display information in order to determine whether or
//     not the display is capable of rendering the scene.  If not, the given 
//     display will not be included in the display list.
//
// Arguments:
//
//     CDirect3D8* pd3d                 - Pointer to the Direct3D object
//
//     D3DCAPS8* pd3dcaps               - Capabilities of the device
//
//     D3DDISPLAYMODE*                  - Display mode into which the device
//                                        will be placed
//
// Return Value:
//
//     TRUE if the scene can be rendered using the given display, FALSE if
//     it cannot.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ValidateDisplay)(CDirect3D8* pd3d, D3DCAPS8* pd3dcaps, D3DDISPLAYMODE* pd3ddm) {

    // Check vertex blending support
    return (!(pd3dcaps->MaxVertexBlendMatrices == 0 ||
            pd3dcaps->MaxVertexBlendMatrices == 1 ||
            pd3dcaps->MaxActiveLights == 0 ||
            !(pd3dcaps->VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS)));
}

//******************************************************************************
//
// Function:
//
//     ExhibitScene
//
// Description:
//
//     Create the scene, pump messages, process user input,
//     update the scene, render the scene, and release the scene when finished.
//
// Arguments:
//
//     CDisplay* pDisplay           - Pointer to the Display object
//
//     int* pnExitCode              - Optional pointer to an integer that will
//                                    be set to the exit value contained in the 
//                                    wParam parameter of the WM_QUIT message 
//                                    (if received)
//
// Return Value:
//
//     TRUE if the display remains functional on exit, FALSE otherwise.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ExhibitScene)(CDisplay* pDisplay, int* pnExitCode) {

    Ball*    pBall;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

#ifdef UNDER_XBOX
    if (!(GetStartupContext() & TSTART_STRESS)) {
        pDisplay->ReleaseDevice();
        pDisplay->GetDirect3D8()->SetPushBufferSize(786432, 32768);
        pDisplay->CreateDevice();
    }
#endif

    // Create the scene
    pBall = new Ball();
    if (!pBall) {
        return FALSE;
    }

    // Initialize the scene
    if (!pBall->Create(pDisplay)) {
        pBall->Release();
        return FALSE;
    }

    bRet = pBall->Exhibit(pnExitCode);

    // Clean up the scene
    pBall->Release();

#ifdef UNDER_XBOX
    if (!(GetStartupContext() & TSTART_STRESS)) {
        pDisplay->ReleaseDevice();
        pDisplay->GetDirect3D8()->SetPushBufferSize(524288, 32768);
        pDisplay->CreateDevice();
    }
#endif

    return bRet;
}

Ball::Ball()
{
//    m_pMaterial = NULL;
//    m_pLight = NULL;
    m_pChainTexture = NULL;
    m_pLogoTexture = NULL;
    m_pWallTexture = NULL;

    m_pSphereVerticesB1 = NULL;
    m_dwSphereVertices = 0;
    m_pSphereIndices = NULL;
    m_dwSphereIndices = 0;

    m_pBoxVerticesB1 = NULL;
    m_pBoxVerticesB2 = NULL;
    m_pBoxVerticesB3 = NULL;
    m_dwBoxVertices = 0;

    m_pVerticesB1 = NULL;
    m_pVerticesB2 = NULL;
    m_pVerticesB3 = NULL;

    m_pWallVertices = NULL;

    
    m_dwTranslations = 100;
    m_dwMatrixModes = 6;
    m_dwRasterModes = 3;
    m_dwTotalTests = m_dwRasterModes * m_dwMatrixModes * m_dwTranslations;
    
    m_dwCurrentTestNumber = 0;
    m_dwCurrentTranslation = 0;
    m_dwCurrentMatrixMode = 0;
    m_dwCurrentRasterMode = 0;
    m_dwNumFailed = 0;

	m_szTestName = TEXT("Ball");
	m_szCommandKey = TEXT("ball");
}

Ball::~Ball()
{
//    RELEASE(m_pMaterial);
//    RELEASE(m_pLight);
    ReleaseTexture(m_pChainTexture);
    ReleaseTexture(m_pLogoTexture);
    ReleaseTexture(m_pWallTexture);

    if (m_pSphereVerticesB1)
    {
        delete [] m_pSphereVerticesB1;
        m_pSphereVerticesB1 = 0;
    }
    m_dwSphereVertices = 0;
    if (m_pSphereIndices)
    {
        delete [] m_pSphereIndices;
        m_pSphereIndices = 0;
    }
    m_dwSphereIndices = 0;

    if (m_pBoxVerticesB1)
    {
        delete [] m_pBoxVerticesB1;
        m_pBoxVerticesB1 = 0;
    }
    if (m_pBoxVerticesB2)
    {
        delete [] m_pBoxVerticesB2;
        m_pBoxVerticesB2 = 0;
    }
    if (m_pBoxVerticesB3)
    {
        delete [] m_pBoxVerticesB3;
        m_pBoxVerticesB3 = 0;
    }
    m_dwBoxVertices = 0;

    if (m_pVerticesB1)
    {
        delete [] m_pVerticesB1;
        m_pVerticesB1 = 0;
    }
    if (m_pVerticesB2)
    {
        delete [] m_pVerticesB2;
        m_pVerticesB2 = 0;
    }
    if (m_pVerticesB3)
    {
        delete [] m_pVerticesB3;
        m_pVerticesB3 = 0;
    }

    if (m_pWallVertices)
    {
        delete [] m_pWallVertices;
        m_pWallVertices = 0;
    }
}


UINT Ball::TestInitialize(void)
{
//    CImageLoader    Loader;
    CShapes*		pShape = NULL;
//    CImageData*		pImage;
    int             i;

    SetTestRange(1, m_dwTotalTests);

    // Check for D3D version
//    if (m_dwVersion < 0x0700)
//    {   
//        WriteToLog("Ball: Test requires DX7 or later.\n");
//        return D3DTESTINIT_SKIPALL;
//    }
    
    // Check vertex blending support
    if (m_d3dcaps.MaxVertexBlendMatrices == 0)
    {
        WriteToLog("Ball: Device does not support vertex blending.\n");
        return D3DTESTINIT_SKIPALL;
    }
    
    if (m_d3dcaps.MaxVertexBlendMatrices == 1)
    {
        WriteToLog("Ball: Device exports 1 in wMaxVertexBlendMatrices.\n");
        return D3DTESTINIT_ABORT;
    }
    
    // Check for lighting caps
    if (m_d3dcaps.MaxActiveLights == 0)
    {
        WriteToLog("Ball: Device does not support lighting\n");
        return D3DTESTINIT_SKIPALL;
    }

    // Check the device supports directional lights
    if (!(m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS))
    {
        WriteToLog("Ball: Device does not support directional lights\n");
        return D3DTESTINIT_SKIPALL;
    }

#ifdef UNDER_XBOX
    if (GetStartupContext() & TSTART_STRESS) {
        SetAPI(RP_DIPVB);
    }
#endif

    // Initialize sphere vertices
    pShape = new CShapes;
    if (NULL == pShape)
        return D3DTESTINIT_ABORT;
    if (!pShape->NewShape(CS_SPHERE)) {
        return D3DTESTINIT_ABORT;
    }

    m_dwSphereVertices = pShape->m_nVertices;
    m_pSphereVerticesB1 = new D3DVERTEXB1 [m_dwSphereVertices];
    if (NULL == m_pSphereVerticesB1)
        return D3DTESTINIT_ABORT;
    for (i = 0; i < (int)m_dwSphereVertices; i++)
    {
        m_pSphereVerticesB1[i].x = pShape->m_pVertices[i].x;
        m_pSphereVerticesB1[i].y = pShape->m_pVertices[i].y;
        m_pSphereVerticesB1[i].z = pShape->m_pVertices[i].z;
        m_pSphereVerticesB1[i].nx = pShape->m_pVertices[i].nx;
        m_pSphereVerticesB1[i].ny = pShape->m_pVertices[i].ny;
        m_pSphereVerticesB1[i].nz = pShape->m_pVertices[i].nz;
        m_pSphereVerticesB1[i].tu = pShape->m_pVertices[i].tu;
        m_pSphereVerticesB1[i].tv = pShape->m_pVertices[i].tv;
        m_pSphereVerticesB1[i].k1 = (float)sqrt(m_pSphereVerticesB1[i].y + 0.5f);
    }
    m_dwSphereIndices = pShape->m_nIndices;
    m_pSphereIndices = new WORD [m_dwSphereIndices];
    if (NULL == m_pSphereIndices)
        return D3DTESTINIT_ABORT;
    for (i = 0; i < (int)m_dwSphereIndices; i++)
        m_pSphereIndices[i] = pShape->m_pIndices[i];
    delete pShape;

    // Intialize temporary vertex arrays
    m_pVerticesB1 = new D3DVERTEXB1 [m_dwSphereVertices];
    if (NULL == m_pVerticesB1)
        return D3DTESTINIT_ABORT;
    m_pVerticesB2 = new D3DVERTEXB2 [m_dwSphereVertices];
    if (NULL == m_pVerticesB2)
        return D3DTESTINIT_ABORT;
    m_pVerticesB3 = new D3DVERTEXB3 [m_dwSphereVertices];
    if (NULL == m_pVerticesB3)
        return D3DTESTINIT_ABORT;

    // Initialize box vertices
    float   fMinX = -0.5f;
    float   fMaxX =  0.5f;
    int     iSegments = 50;
    float   fStepX = (fMaxX - fMinX) / iSegments;

    m_dwBoxVertices = 2*iSegments + 2;
    m_pBoxVerticesB1 = new D3DVERTEXB1 [m_dwBoxVertices];
    if (NULL == m_pBoxVerticesB1)
        return D3DTESTINIT_ABORT;
    for (i = 0; i < (iSegments + 1); i++)
    {
        // First vertex
        m_pBoxVerticesB1[2*i].x = fMinX + fStepX*i;
        m_pBoxVerticesB1[2*i].y = 0.5f;
        m_pBoxVerticesB1[2*i].z = 0.f;
        m_pBoxVerticesB1[2*i].nx = 0.f;
        m_pBoxVerticesB1[2*i].ny = 0.f;
        m_pBoxVerticesB1[2*i].nz = -1.f;
        m_pBoxVerticesB1[2*i].tu = 2.f * fStepX*i / (fMaxX - fMinX);
        m_pBoxVerticesB1[2*i].tv = 0.f;
        
        if (m_pBoxVerticesB1[2*i].x < -0.1f)
            m_pBoxVerticesB1[2*i].k1 = (float)sqrt((m_pBoxVerticesB1[2*i].x + 0.5f)*2.5f);
        else if (m_pBoxVerticesB1[2*i].x < 0.1f)
            m_pBoxVerticesB1[2*i].k1 = 1.f;
        else
            m_pBoxVerticesB1[2*i].k1 = (float)sqrt((0.5f - m_pBoxVerticesB1[2*i].x)*2.5f);

        // Second vertex
        m_pBoxVerticesB1[2*i+1].x = fMinX + fStepX*i;
        m_pBoxVerticesB1[2*i+1].y = -0.5f;
        m_pBoxVerticesB1[2*i+1].z = 0.f;
        m_pBoxVerticesB1[2*i+1].nx = 0.f;
        m_pBoxVerticesB1[2*i+1].ny = 0.f;
        m_pBoxVerticesB1[2*i+1].nz = -1.f;
        m_pBoxVerticesB1[2*i+1].tu = 2.f * fStepX*i / (fMaxX - fMinX);
        m_pBoxVerticesB1[2*i+1].tv = 1.f;
        
        m_pBoxVerticesB1[2*i+1].k1 = 0.f;
    }

    // Initialize wall vertices
    m_pWallVertices = new D3DTLVERTEX [4];
    if (NULL == m_pWallVertices)
        return D3DTESTINIT_ABORT;
    for (i = 0; i < 4; i++)
    {
        m_pWallVertices[i].sx = 0.f;
        m_pWallVertices[i].sy = 0.f;
        m_pWallVertices[i].sz = 0.99f;
        m_pWallVertices[i].rhw = 1.f;
        m_pWallVertices[i].color = RGBA_MAKE(255, 255, 255, 255);
        m_pWallVertices[i].dcSpecular = 0;
        m_pWallVertices[i].tu = 0.f;
        m_pWallVertices[i].tv = 0.f;
    }
    m_pWallVertices[0].sy = m_pWallVertices[3].sy = (float)m_pDisplay->GetHeight();
    m_pWallVertices[2].sx = m_pWallVertices[3].sx = (float)m_pDisplay->GetWidth();
    m_pWallVertices[2].tu = m_pWallVertices[3].tu = 1.f;
    m_pWallVertices[1].tv = m_pWallVertices[2].tv = 1.f;

    // Intialize textures
    // Chain
//    pImage = Loader.Load(CIL_BMP,"chain.bmp");
//    m_pChainTexture = CreateTexture(256,256,NULL,pImage);
//    RELEASE(pImage);
    m_pChainTexture = (CTexture8*)CreateTexture(m_pDevice, TEXT("chain.bmp"));
    if (m_pChainTexture == NULL)
        return D3DTESTINIT_ABORT;

    // Logo
//    pImage = Loader.Load(CIL_BMP,"smile.bmp");
//    m_pLogoTexture = CreateTexture(256,256,NULL,pImage);
//    RELEASE(pImage);
    m_pLogoTexture = (CTexture8*)CreateTexture(m_pDevice, TEXT("smile.bmp"));
    if (m_pLogoTexture == NULL)
        return D3DTESTINIT_ABORT;

    // Wall
//    pImage = Loader.Load(CIL_BMP,"bubbles.bmp");
//    m_pWallTexture = CreateTexture(256,256,NULL,pImage);
//    RELEASE(pImage);
    m_pWallTexture = (CTexture8*)CreateTexture(m_pDevice, TEXT("bubbles.bmp"));
    if (m_pWallTexture == NULL)
        return D3DTESTINIT_ABORT;

    return D3DTESTINIT_RUN;
}

bool Ball::ExecuteTest(UINT iTestNumber)
{
    int     i;
    int     num = iTestNumber - 1;
    
    m_dwCurrentTestNumber = (DWORD)iTestNumber;

    // Resolving test number
    m_dwCurrentTranslation = num % m_dwTranslations;
    num /= m_dwTranslations;
    m_dwCurrentMatrixMode = num % m_dwMatrixModes;
    num /= m_dwMatrixModes;
    m_dwCurrentRasterMode = num % m_dwRasterModes;
    num /= m_dwRasterModes;

    // Check the device suppors enough vertex blend matrices
    if (m_d3dcaps.MaxVertexBlendMatrices < 3 &&
        m_pBallMode[m_dwCurrentMatrixMode].dwSecondMatrix != D3DTRANSFORMSTATE_WORLD1)
    {
        SkipTests(m_dwTranslations - (iTestNumber-1)%m_dwTranslations);
        return false;
    }
    if (m_d3dcaps.MaxVertexBlendMatrices < 4 &&
        m_pBallMode[m_dwCurrentMatrixMode].dwSecondMatrix == D3DTRANSFORMSTATE_WORLD3)
    {
        SkipTests(m_dwTranslations - (iTestNumber-1)%m_dwTranslations);
        return false;
    }

    // Convert vertex format
    if (D3DFVF_VERTEXB1 == m_pBallMode[m_dwCurrentMatrixMode].dwVertexType)
    {
        memcpy(m_pVerticesB1, m_pSphereVerticesB1, sizeof(D3DVERTEXB1)*m_dwSphereVertices);
    }
    else if (D3DFVF_VERTEXB2 == m_pBallMode[m_dwCurrentMatrixMode].dwVertexType)
    {
        for (i = 0; i < (int)m_dwSphereVertices; i++)
        {
            m_pVerticesB2[i].x = m_pSphereVerticesB1[i].x;
            m_pVerticesB2[i].y = m_pSphereVerticesB1[i].y;
            m_pVerticesB2[i].z = m_pSphereVerticesB1[i].z;
            m_pVerticesB2[i].nx = m_pSphereVerticesB1[i].nx;
            m_pVerticesB2[i].ny = m_pSphereVerticesB1[i].ny;
            m_pVerticesB2[i].nz = m_pSphereVerticesB1[i].nz;
            m_pVerticesB2[i].tu = m_pSphereVerticesB1[i].tu;
            m_pVerticesB2[i].tv = m_pSphereVerticesB1[i].tv;
            switch (m_pBallMode[m_dwCurrentMatrixMode].dwFirstMatrix)
            {
            case D3DTRANSFORMSTATE_WORLD:
                m_pVerticesB2[i].k1 = m_pSphereVerticesB1[i].k1;
                m_pVerticesB2[i].k2 = 0.f;
                break;
            case D3DTRANSFORMSTATE_WORLD1:
                m_pVerticesB2[i].k1 = 0.f;
                m_pVerticesB2[i].k2 = m_pSphereVerticesB1[i].k1;
                break;
            }
        }
    }
    else if (D3DFVF_VERTEXB3 == m_pBallMode[m_dwCurrentMatrixMode].dwVertexType)
    {
        for (i = 0; i < (int)m_dwSphereVertices; i++)
        {
            m_pVerticesB3[i].x = m_pSphereVerticesB1[i].x;
            m_pVerticesB3[i].y = m_pSphereVerticesB1[i].y;
            m_pVerticesB3[i].z = m_pSphereVerticesB1[i].z;
            m_pVerticesB3[i].nx = m_pSphereVerticesB1[i].nx;
            m_pVerticesB3[i].ny = m_pSphereVerticesB1[i].ny;
            m_pVerticesB3[i].nz = m_pSphereVerticesB1[i].nz;
            m_pVerticesB3[i].tu = m_pSphereVerticesB1[i].tu;
            m_pVerticesB3[i].tv = m_pSphereVerticesB1[i].tv;
            switch (m_pBallMode[m_dwCurrentMatrixMode].dwFirstMatrix)
            {
            case D3DTRANSFORMSTATE_WORLD:
                m_pVerticesB3[i].k1 = m_pSphereVerticesB1[i].k1;
                m_pVerticesB3[i].k2 = 0.f;
                m_pVerticesB3[i].k3 = 0.f;
                break;
            case D3DTRANSFORMSTATE_WORLD1:
                m_pVerticesB3[i].k1 = 0.f;
                m_pVerticesB3[i].k2 = m_pSphereVerticesB1[i].k1;
                m_pVerticesB3[i].k3 = 0.f;
                break;
            case D3DTRANSFORMSTATE_WORLD2:
                m_pVerticesB3[i].k1 = 0.f;
                m_pVerticesB3[i].k2 = 0.f;
                m_pVerticesB3[i].k3 = m_pSphereVerticesB1[i].k1;
                break;
            }
        }
    }

    BeginTestCase(m_pBallMode[m_dwCurrentMatrixMode].pszModeName);
    WriteStatus("Mode", m_pBallMode[m_dwCurrentMatrixMode].pszModeName);
    return true;
}


void Ball::SceneRefresh(void)
{
    D3DMATRIX Matrix;

    if (BeginScene())
    {
        if (m_dwCurrentRasterMode == 0)
        {
            SetTexture(0, 0);
        }
        else
        {
            SetRenderState(D3DRENDERSTATE_FILLMODE, (DWORD)D3DFILL_SOLID);
        
            // Wall rendering
            SetTexture(0, m_pWallTexture);
            SetRenderState(D3DRENDERSTATE_VERTEXBLEND, (DWORD)D3DVBLEND_DISABLE);
            SetRenderState(D3DRENDERSTATE_WRAP0, (DWORD)0);
            RenderPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, m_pWallVertices, 4, NULL, 0, 0);
        }
        
        // Sphere rendering (source)
        if (m_dwCurrentRasterMode == 1)
        {
            SetTexture(0, m_pLogoTexture);
        }
        else if (m_dwCurrentRasterMode == 2)
        {
            D3DMATRIX   M;

            InitMatrix(&M,
                0.5f,  0.0f,  0.f,   0.f,
                0.0f,  0.5f,  0.f,   0.f,
                0.0f,  0.0f,  0.f,   0.f,
                0.5f,  0.5f,  0.f,   0.f
            );
            SetTransform(D3DTRANSFORMSTATE_TEXTURE0, &M);
            SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)D3DTSS_TCI_CAMERASPACEPOSITION);
			SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)D3DTTFF_COUNT2);
        }
        SetRenderState(D3DRENDERSTATE_WRAP0, (DWORD)(D3DWRAP_U | D3DWRAP_V));
        Matrix = IdentityMatrix();
        if (D3DTRANSFORMSTATE_WORLD != m_pBallMode[m_dwCurrentMatrixMode].dwFirstMatrix && D3DTRANSFORMSTATE_WORLD != m_pBallMode[m_dwCurrentMatrixMode].dwSecondMatrix)
            SetTransform(D3DTRANSFORMSTATE_WORLD,  &Matrix);
        if (D3DTRANSFORMSTATE_WORLD1 != m_pBallMode[m_dwCurrentMatrixMode].dwFirstMatrix && D3DTRANSFORMSTATE_WORLD1 != m_pBallMode[m_dwCurrentMatrixMode].dwSecondMatrix)
            SetTransform(D3DTRANSFORMSTATE_WORLD1, &Matrix);
        if (D3DTRANSFORMSTATE_WORLD2 != m_pBallMode[m_dwCurrentMatrixMode].dwFirstMatrix && D3DTRANSFORMSTATE_WORLD2 != m_pBallMode[m_dwCurrentMatrixMode].dwSecondMatrix)
            SetTransform(D3DTRANSFORMSTATE_WORLD2, &Matrix);
        if (D3DTRANSFORMSTATE_WORLD3 != m_pBallMode[m_dwCurrentMatrixMode].dwFirstMatrix && D3DTRANSFORMSTATE_WORLD3 != m_pBallMode[m_dwCurrentMatrixMode].dwSecondMatrix)
            SetTransform(D3DTRANSFORMSTATE_WORLD3, &Matrix);
        
        Matrix = GenerateSphereModelMatrix(m_dwCurrentTestNumber, 0);
        SetTransform(m_pBallMode[m_dwCurrentMatrixMode].dwFirstMatrix, &Matrix);
        Matrix = GenerateSphereModelMatrix(m_dwCurrentTestNumber, 1);
        SetTransform(m_pBallMode[m_dwCurrentMatrixMode].dwSecondMatrix, &Matrix);
        
        SetRenderState(D3DRENDERSTATE_VERTEXBLEND, (DWORD)m_pBallMode[m_dwCurrentMatrixMode].dwMatrixMode);
        switch(m_pBallMode[m_dwCurrentMatrixMode].dwVertexType)
        {
        case D3DFVF_VERTEXB1:
            RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEXB1, m_pVerticesB1, m_dwSphereVertices, 
              m_pSphereIndices, m_dwSphereIndices, CD3D_SRC);
            break;
        case D3DFVF_VERTEXB2:
            RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEXB2, m_pVerticesB2, m_dwSphereVertices, 
              m_pSphereIndices, m_dwSphereIndices, CD3D_SRC);
            break;
        case D3DFVF_VERTEXB3:
            RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEXB3, m_pVerticesB3, m_dwSphereVertices, 
              m_pSphereIndices, m_dwSphereIndices, CD3D_SRC);
            break;
        }

        // Sphere rendering (reference)
        Matrix = GenerateSphereModelMatrix(m_dwCurrentTestNumber, 0);
        SetTransform(D3DTRANSFORMSTATE_WORLD, &Matrix);
        Matrix = GenerateSphereModelMatrix(m_dwCurrentTestNumber, 1);
        SetTransform(D3DTRANSFORMSTATE_WORLD1, &Matrix);
        SetRenderState(D3DRENDERSTATE_VERTEXBLEND, (DWORD)D3DVBLEND_1WEIGHT);
        RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEXB1, m_pSphereVerticesB1, m_dwSphereVertices, 
            m_pSphereIndices, m_dwSphereIndices, CD3D_REF);

        // Box rendering
        if (m_dwCurrentRasterMode == 1 || m_dwCurrentRasterMode == 2)
            SetTexture(0, m_pChainTexture);
        Matrix = GenerateBoxModelMatrix(m_dwCurrentTestNumber, 0);
        SetTransform(D3DTRANSFORMSTATE_WORLD, &Matrix);
        Matrix = GenerateBoxModelMatrix(m_dwCurrentTestNumber, 1);
        SetTransform(D3DTRANSFORMSTATE_WORLD1, &Matrix);
        SetRenderState(D3DRENDERSTATE_VERTEXBLEND, (DWORD)D3DVBLEND_1WEIGHT);
        SetRenderState(D3DRENDERSTATE_WRAP0, (DWORD)0);
		SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)D3DTTFF_DISABLE);
        SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
        RenderPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEXB1, m_pBoxVerticesB1, m_dwBoxVertices, NULL, 0, 0);

        EndScene();
    }
}


D3DMATRIX Ball::GenerateSphereModelMatrix(DWORD dwFrame, DWORD nMatrix)
{
    D3DMATRIX   Matrix;
    float       n = (float)(dwFrame % 100);
    float       k;

    // Back movement like mirror of down movement
    if (n > 50)
        n = 100 - n;
    
    // Translation
    if (nMatrix == 1 && n > 40)
        k = -0.14f;
    else
        k = 0.5f - 0.0004f * n * n;
    Matrix = MatrixMult(TranslateMatrix(0.f, k, 0.f), RotateYMatrix(pi/2.f));

    // Additional scale at the end of the path
    if (n > 40)
    {
        k = 1.f + float(n - 40) * 0.015f;
        Matrix = MatrixMult(ScaleMatrix(k, 1.f, 1.f), Matrix);
    }
    return Matrix;
}

D3DMATRIX Ball::GenerateBoxModelMatrix(DWORD dwFrame, DWORD nMatrix)
{
    D3DMATRIX   Matrix;
    float       n = (float)(dwFrame % 100);
    float       k = 0.f;

    // Back movement like mirror of down movement
    if (n > 50)
        n = 100 - n;
    
    Matrix = MatrixMult(TranslateMatrix(0.f, -1.0f, 0.f), ScaleMatrix(2.0f, 1.0f, 1.f));

    if (nMatrix == 0) 
    {
        if (n > 40)
            k = -0.15f - 0.003f * float(n - 40);
        else if (n > 35) 
            k = -0.03f * float(n - 35);
        Matrix = MatrixMult(Matrix, TranslateMatrix(0.f, k, 0.f));
    }

    return Matrix;
}

bool Ball::ProcessFrame(void)
{
	bool    bResult;

    // This range (0.00002) basically allows 1 pixel to be different
    bResult = GetCompareResult(0.05f);

    if (!bResult) {
		WriteToLog("$rBall: Test fails on frame %d\n", m_dwCurrentTestNumber);
		Fail();
		m_dwNumFailed++;
    }
	else {
		Pass();
	}
	EndTestCase();

	return true;
}

void Ball::TestCompleted(void)
{
    WriteToLog("$g%Ball test flush summary:\n");
    WriteToLog("$cErrors:\t\t$y%d\n",m_dwNumFailed);
}

bool Ball::TestTerminate(void)
{
//    RELEASE(m_pMaterial);
//    RELEASE(m_pLight);
    m_pDevice->LightEnable(0, FALSE);
    ReleaseTexture(m_pChainTexture);
    m_pChainTexture = NULL;
    ReleaseTexture(m_pLogoTexture);
    m_pLogoTexture = NULL;
    ReleaseTexture(m_pWallTexture);
    m_pWallTexture = NULL;

    if (m_pSphereVerticesB1)
    {
        delete [] m_pSphereVerticesB1;
        m_pSphereVerticesB1 = 0;
    }
    m_dwSphereVertices = 0;
    if (m_pSphereIndices)
    {
        delete [] m_pSphereIndices;
        m_pSphereIndices = 0;
    }
    m_dwSphereIndices = 0;

    if (m_pBoxVerticesB1)
    {
        delete [] m_pBoxVerticesB1;
        m_pBoxVerticesB1 = 0;
    }
    if (m_pBoxVerticesB2)
    {
        delete [] m_pBoxVerticesB2;
        m_pBoxVerticesB2 = 0;
    }
    if (m_pBoxVerticesB3)
    {
        delete [] m_pBoxVerticesB3;
        m_pBoxVerticesB3 = 0;
    }
    m_dwBoxVertices = 0;

    if (m_pVerticesB1)
    {
        delete [] m_pVerticesB1;
        m_pVerticesB1 = 0;
    }
    if (m_pVerticesB2)
    {
        delete [] m_pVerticesB2;
        m_pVerticesB2 = 0;
    }
    if (m_pVerticesB3)
    {
        delete [] m_pVerticesB3;
        m_pVerticesB3 = 0;
    }

    if (m_pWallVertices)
    {
        delete [] m_pWallVertices;
        m_pWallVertices = 0;
    }

    return true;
}
