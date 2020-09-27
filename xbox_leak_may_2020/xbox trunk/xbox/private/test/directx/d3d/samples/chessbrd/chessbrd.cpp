/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    chessbrd.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dbase.h"
#include "chessbrd.h"

//#define PIXEL_LIGHTING
//#define FALLENKING_ILLUSION

//******************************************************************************
//
// Function:
//
//     TextureProjectXY
//
// Description:
//
//     Initialize the texture coordinates of the given mesh for a parallel
//     projection onto the XY plane.
//
// Arguments:
//
//     PVERTEX pr               - Array of vertices on which to project the 
//                                texture
//
//     UINT uNumVertices        - Number of vertices in the array
//
//     float fX1                - Reference value of x at which tu will be zero
//
//     float fX2                - Reference value of x at which tu will be one
//
//     float fY1                - Reference value of y at which tv will be zero
//
//     float fY2                - Reference value of y at which tv will be one
//
// Return Value:
//
//     None.
//
//******************************************************************************
void TextureProjectXY(PVERTEX pr, UINT uNumVertices,
                      float fX1, float fX2, float fY1, float fY2) 
{
    float    fWidth, fHeight;
    UINT     i;

    if (!pr) {
        return;
    }

    fWidth = fX2 - fX1;
    fHeight = fY2 - fY1;

    for (i = 0; i < uNumVertices; i++) {

        pr[i].u0 = (pr[i].vPosition.x - fX1) / fWidth;
        pr[i].v0 = (pr[i].vPosition.y - fY1) / fHeight;
    }
}

/*
void TextureProjectXYD(PVERTEX pr, UINT uNumVertices,
                      float fX1, float fX2, float fY1, float fY2) 
{
    float    fWidth, fHeight;
    UINT     i;
    float    fLeft = FLT_MAX;
    float    fRight = -FLT_MAX;
    float    fTop = -FLT_MAX;
    float    fBottom = FLT_MAX;

    if (!pr) {
        return;
    }

    fWidth = fX2 - fX1;
    fHeight = fY2 - fY1;

    for (i = 0; i < uNumVertices; i++) {

        if (pr[i].vPosition.x < fLeft) {
            fLeft = pr[i].vPosition.x;
        }
        if (pr[i].vPosition.x > fRight) {
            fRight = pr[i].vPosition.x;
        }
        if (pr[i].vPosition.y > fTop) {
            fTop = pr[i].vPosition.y;
        }
        if (pr[i].vPosition.y < fBottom) {
            fBottom = pr[i].vPosition.y;
        }
        pr[i].u0 = (pr[i].vPosition.x - fX1) / fWidth;
        pr[i].v0 = (pr[i].vPosition.y - fY1) / fHeight;
    }

    DebugString(TEXT("x: %3.3f to %3.3f, y: %3.3f to %3.3f"), fLeft, fRight, fTop, fBottom);
}
*/

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
BOOL ExhibitScene(CDisplay* pDisplay, int* pnExitCode) {

    CChessboard*    pChessBoard;
    BOOL            bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pChessBoard = new CChessboard();
    if (!pChessBoard) {
        return FALSE;
    }

    // Initialize the scene
    if (!pChessBoard->Create(pDisplay)) {
        pChessBoard->Release();
        return FALSE;
    }

    bRet = pChessBoard->Exhibit(pnExitCode);

    // Clean up the scene
    pChessBoard->Release();

    return bRet;
}

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
//     LPDIRECT3D8* pd3d                - Pointer to the Direct3D object
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
BOOL ValidateDisplay(LPDIRECT3D8 pd3d, D3DCAPS8* pd3dcaps, D3DDISPLAYMODE* pd3ddm) {

    return TRUE;
}

//******************************************************************************
// CChessboard
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CChessboard
//
// Description:
//
//     Initialize the test.
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
CChessboard::CChessboard() {

    m_pd3dtBoardSurface = NULL;
    m_pd3dtBoardEdge = NULL;
    m_pd3dtPawn = NULL;
    m_pd3dtFallen = NULL;
    m_fTranslationDelta = 0.0f;
    m_fXRotationDelta = 0.0f;
    m_fYRotationDelta = 0.0f;
    m_bRotateCamera = TRUE;
    m_bFade = TRUE;
    memset(m_cmChessMesh, 0, 6 * sizeof(CHESSMESH));
    m_vLightDir = D3DXVECTOR3(1.0f, -1.0f, 1.0f);
}

//******************************************************************************
//
// Method:
//
//     ~CChessboard
//
// Description:
//
//     Clean up the test.
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
CChessboard::~CChessboard() {
}

//******************************************************************************
//
// Method:
//
//     Create
//
// Description:
//
//     Prepare the test for rendering.
//
// Arguments:
//
//     CDisplay* pDisplay               - Pointer to a Display object.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CChessboard::Create(CDisplay* pDisplay) {

    D3DXMATRIX m;

    // Intialize the board surface vertices
    m_prBoardSurface[0] = VERTEX(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 2.0f);
    m_prBoardSurface[1] = VERTEX(D3DXVECTOR3(0.0f, 0.0f, 10.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
    m_prBoardSurface[2] = VERTEX(D3DXVECTOR3(10.0f, 0.0f, 10.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 2.0f, 0.0f);
    m_prBoardSurface[3] = VERTEX(D3DXVECTOR3(10.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 2.0f, 2.0f);

    // Intialize the board edge vertices
    m_prBoardEdge[0][0] = VERTEX(D3DXVECTOR3(-1.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 12.0f);
    m_prBoardEdge[0][1] = VERTEX(D3DXVECTOR3(-1.0f, 0.0f, 11.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
    m_prBoardEdge[0][2] = VERTEX(D3DXVECTOR3(0.0f, 0.0f, 10.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 1.0f, 1.0f);
    m_prBoardEdge[0][3] = VERTEX(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 1.0f, 11.0f);

    m_prBoardEdge[1][0] = VERTEX(D3DXVECTOR3(0.0f, 0.0f, 10.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 1.0f, 1.0f);
    m_prBoardEdge[1][1] = VERTEX(D3DXVECTOR3(-1.0f, 0.0f, 11.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
    m_prBoardEdge[1][2] = VERTEX(D3DXVECTOR3(11.0f, 0.0f, 11.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 12.0f, 0.0f);
    m_prBoardEdge[1][3] = VERTEX(D3DXVECTOR3(10.0f, 0.0f, 10.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 11.0f, 1.0f);

    m_prBoardEdge[2][0] = VERTEX(D3DXVECTOR3(10.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 11.0f, 11.0f);
    m_prBoardEdge[2][1] = VERTEX(D3DXVECTOR3(10.0f, 0.0f, 10.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 11.0f, 1.0f);
    m_prBoardEdge[2][2] = VERTEX(D3DXVECTOR3(11.0f, 0.0f, 11.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 12.0f, 0.0f);
    m_prBoardEdge[2][3] = VERTEX(D3DXVECTOR3(11.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 12.0f, 12.0f);

    m_prBoardEdge[3][0] = VERTEX(D3DXVECTOR3(-1.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 12.0f);
    m_prBoardEdge[3][1] = VERTEX(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 1.0f, 11.0f);
    m_prBoardEdge[3][2] = VERTEX(D3DXVECTOR3(10.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 11.0f, 11.0f);
    m_prBoardEdge[3][3] = VERTEX(D3DXVECTOR3(11.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 12.0f, 12.0f);

    m_prBoardEdge[4][0] = VERTEX(D3DXVECTOR3(11.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), 12.0f, 12.0f);
    m_prBoardEdge[4][1] = VERTEX(D3DXVECTOR3(11.0f, 0.0f, 11.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), 12.0f, 0.0f);
    m_prBoardEdge[4][2] = VERTEX(D3DXVECTOR3(-1.0f, 0.0f, 11.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), 0.0f, 0.0f);
    m_prBoardEdge[4][3] = VERTEX(D3DXVECTOR3(-1.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), 0.0f, 12.0f);

    // Initialize the reflection screen
    m_plrScreen[0][0] = LVERTEX(D3DXVECTOR3(-101.0f, 0.0f, -101.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[0][1] = LVERTEX(D3DXVECTOR3(-1.0f, 0.0f, -1.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[0][2] = LVERTEX(D3DXVECTOR3(11.0f, 0.0f, -1.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[0][3] = LVERTEX(D3DXVECTOR3(111.0f, 0.0f, -101.0f), 0, 0, 0.0f, 0.0f);

    m_plrScreen[1][0] = LVERTEX(D3DXVECTOR3(111.0f, 0.0f, -101.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[1][1] = LVERTEX(D3DXVECTOR3(11.0f, 0.0f, -1.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[1][2] = LVERTEX(D3DXVECTOR3(11.0f, 0.0f, 11.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[1][3] = LVERTEX(D3DXVECTOR3(111.0f, 0.0f, 111.0f), 0, 0, 0.0f, 0.0f);

    m_plrScreen[2][0] = LVERTEX(D3DXVECTOR3(111.0f, 0.0f, 111.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[2][1] = LVERTEX(D3DXVECTOR3(11.0f, 0.0f, 11.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[2][2] = LVERTEX(D3DXVECTOR3(-1.0f, 0.0f, 11.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[2][3] = LVERTEX(D3DXVECTOR3(-101.0f, 0.0f, 111.0f), 0, 0, 0.0f, 0.0f);

    m_plrScreen[3][0] = LVERTEX(D3DXVECTOR3(-101.0f, 0.0f, 111.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[3][1] = LVERTEX(D3DXVECTOR3(-1.0f, 0.0f, 11.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[3][2] = LVERTEX(D3DXVECTOR3(-1.0f, 0.0f, -1.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[3][3] = LVERTEX(D3DXVECTOR3(-101.0f, 0.0f, -101.0f), 0, 0, 0.0f, 0.0f);

    // Initialize world transforms
    InitMatrix(&m_mBoard,
        (float)( 1.0), (float)( 0.0), (float)( 0.0), (float)( 0.0),
        (float)( 0.0), (float)( 1.0), (float)( 0.0), (float)( 0.0),
        (float)( 0.0), (float)( 0.0), (float)( 1.0), (float)( 0.0),
        (float)(-5.0), (float)(-5.0), (float)(-5.0), (float)( 1.0)
    );

    InitMatrix(&m,
        (float)( 0.3), (float)( 0.0), (float)( 0.0), (float)( 0.0),
        (float)( 0.0), (float)( 0.3), (float)( 0.0), (float)( 0.0),
        (float)( 0.0), (float)( 0.0), (float)( 0.3), (float)( 0.0),
        (float)( 0.0), (float)( 0.0), (float)( 0.0), (float)( 1.0)
    );

    InitMatrix(&m_mPawn,
        (float)( 1.0), (float)( 0.0), (float)( 0.0), (float)( 0.0),
        (float)( 0.0), (float)( 1.0), (float)( 0.0), (float)( 0.0),
        (float)( 0.0), (float)( 0.0), (float)( 1.0), (float)( 0.0),
        (float)( 0.6), (float)(-5.0), (float)( 0.6), (float)( 1.0)
    );

    InitMatrix(&m_mReflection,
        (float)( 1.0), (float)( 0.0), (float)( 0.0), (float)( 0.0),
        (float)( 0.0), (float)( cos(M_PI)), (float)( sin(M_PI)), (float)( 0.0),
        (float)( 0.0), (float)(-sin(M_PI)), (float)( cos(M_PI)), (float)( 0.0),
        (float)( 0.0), (float)( 0.0), (float)( 0.0), (float)( 1.0)
    );

    D3DXMatrixMultiply(&m_mReflection, &m, &m_mReflection);
    D3DXMatrixMultiply(&m_mReflection, &m_mReflection, &m_mPawn);

    // Create the pawn material
    SetMaterial(&m_matPawn, RGBA_MAKE(255, 255, 255, 255), 
                                RGBA_MAKE(64, 64, 64, 255), RGBA_MAKE(255, 255, 255, 255), 
                                RGBA_MAKE(0, 0, 0, 255), 40.0f);

    // Create the board edge material
    SetMaterial(&m_matBoardEdge, RGBA_MAKE(255, 255, 255, 255), 
                                RGBA_MAKE(192, 192, 192, 255), RGBA_MAKE(96, 96, 96, 255), 
                                RGBA_MAKE(0, 0, 0, 255), 10.0f);

    // Create the board surface material
    SetMaterial(&m_matBoardSurface, RGB_MAKE(255, 255, 255), RGB_MAKE(192, 192, 192),
                                RGB_MAKE(96, 96, 96), RGB_MAKE(0, 0, 0), 40.0f);
    SetMaterialTranslucency(&m_matBoardSurface, 0.4f);

    return CScene::Create(pDisplay);
}

//******************************************************************************
//
// Method:
//
//     Exhibit
//
// Description:
//
//     Execute the test.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     The exit value contained in the wParam parameter of the WM_QUIT message.
//
//******************************************************************************
int CChessboard::Exhibit(int *pnExitCode) {

    return CScene::Exhibit(pnExitCode);
}

//******************************************************************************
//
// Method:
//
//     Setup
//
// Description:
//
//     Obtain the device interface pointer from the display, save the current
//     state of the device, and initialize the background vertices to the
//     dimensions of the render target.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the scene was successfully prepared for initialization, FALSE if
//     it was not.
//
//******************************************************************************
BOOL CChessboard::Setup() {

    return CScene::Setup();
}

#define SQUAREORIGIN -4.4f
#define SQUAREDIM    1.25f

//******************************************************************************
//
// Method:
//
//     Initialize
//
// Description:
//
//     Initialize the device and all device objects to be used in the test (or
//     at least all device resource objects to be created in the video, 
//     non-local video, or default memory pools).
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the test was successfully initialized for rendering, FALSE if
//     it was not.
//
//******************************************************************************
BOOL CChessboard::Initialize() {

    D3DXMATRIX mRotate, mTransform, m;
    MESH       mesh;
    UINT       i, j;
    LPTSTR     pszChessMesh[6] = {
                    TEXT("lpawn.xdg"),
                    TEXT("lrook.xdg"), 
                    TEXT("sknight.xdg"),
                    TEXT("lbishop.xdg"),
                    TEXT("lqueen.xdg"),
                    TEXT("lking.xdg")
    };

    if (!CScene::Initialize()) {
        return FALSE;
    }

    // Create the textures
    m_pd3dtBoardSurface = (LPDIRECT3DTEXTURE8)CreateTexture(m_pDevice, TEXT("chess.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 1.0f, COLORKEY_NONE, D3DPOOL_MANAGED, 256, 256);
    m_pd3dtBoardEdge = (LPDIRECT3DTEXTURE8)CreateTexture(m_pDevice, TEXT("edge.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 1.0f, COLORKEY_NONE, D3DPOOL_MANAGED, 128, 128);
    m_pd3dtPawn = (LPDIRECT3DTEXTURE8)CreateTexture(m_pDevice, TEXT("pine.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 1.0f, COLORKEY_NONE, D3DPOOL_MANAGED, 128, 128);
    m_pd3dtFallen = (LPDIRECT3DTEXTURE8)CreateTexture(m_pDevice, TEXT("pine2.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 1.0f, COLORKEY_NONE, D3DPOOL_MANAGED, 128, 128);
#ifdef UNDER_XBOX
    m_pd3dtBoardSurface->MoveResourceMemory(D3DMEM_VIDEO);
    m_pd3dtBoardEdge->MoveResourceMemory(D3DMEM_VIDEO);
    m_pd3dtPawn->MoveResourceMemory(D3DMEM_VIDEO);
    m_pd3dtFallen->MoveResourceMemory(D3DMEM_VIDEO);
#endif // UNDER_XBOX

#ifdef PIXEL_LIGHTING

    m_pd3dtcNorm = CreateNormalMap(m_pDevice, 256, 0.075f, D3DFMT_A8R8G8B8, TRUE);
    if (!m_pd3dtcNorm) {
        return FALSE;
    }

    m_pd3dtcHVec = CreateHalfVectorMap(m_pDevice, &m_vLightDir, 32, D3DFMT_A8R8G8B8, TRUE);
    if (!m_pd3dtcHVec) {
        return FALSE;
    }

#endif

    for (i = 0; i < 6; i++) {

        if (!CreateMesh(pszChessMesh[i], &mesh)) {
            return FALSE;
        }

        TextureProjectXY(mesh.prVertices, mesh.uNumVertices, -8.584f, 8.584f, 0.0f, 24.714f);

#ifndef PIXEL_LIGHTING

        InitMatrix(&m,
            (float)( 0.1), (float)( 0.0), (float)( 0.0), (float)( 0.0),
            (float)( 0.0), (float)( 0.1), (float)( 0.0), (float)( 0.0),
            (float)( 0.0), (float)( 0.0), (float)( 0.1), (float)( 0.0),
            (float)( 0.0), (float)( 0.0), (float)( 0.0), (float)( 1.0)
        );

        for (j = 0; j < mesh.uNumVertices; j++) {
            D3DXVec3TransformCoord(&mesh.prVertices[j].vPosition, &mesh.prVertices[j].vPosition, &m);
        }

#endif

        m_cmChessMesh[i].pd3dr = CreateVertexBuffer(m_pDevice, mesh.prVertices, mesh.uNumVertices * sizeof(VERTEX), 0, FVF_VERTEX);
        if (!m_cmChessMesh[i].pd3dr) {
            return FALSE;
        }

#ifdef UNDER_XBOX
        m_cmChessMesh[i].pd3dr->MoveResourceMemory(D3DMEM_VIDEO);
#endif

        m_cmChessMesh[i].pd3di = CreateIndexBuffer(m_pDevice, mesh.pwIndices, mesh.uNumIndices * sizeof(WORD));
        if (!m_cmChessMesh[i].pd3di) {
            return FALSE;
        }

        m_cmChessMesh[i].uNumVertices = mesh.uNumVertices;
        m_cmChessMesh[i].uNumIndices = mesh.uNumIndices;
        m_cmChessMesh[i].uNumPrimitives = mesh.uNumIndices / 3;

        ReleaseMesh(&mesh);
    }

    for (i = 0; i < 7; i++) {
        if (i < 6) {
            j = i;
        }
        else {
            j = 5;
        }
        m_cpPiece[i].pcmMesh = &m_cmChessMesh[j];
        m_cpReflection[i].pcmMesh = &m_cmChessMesh[j];
        m_cpPiece[i].mWorld = m_mIdentity;
#ifdef PIXEL_LIGHTING
        m_cpPiece[i].mWorld._11 = 0.1f;
        m_cpPiece[i].mWorld._22 = 0.1f;
        m_cpPiece[i].mWorld._33 = 0.1f;
#endif
        m_cpPiece[i].mWorld._42 = -5.0f;
        m_cpReflection[i].mWorld = m_mIdentity;
#ifdef PIXEL_LIGHTING
        m_cpReflection[i].mWorld._11 = 0.1f;
        m_cpReflection[i].mWorld._22 = 0.1f;
        m_cpReflection[i].mWorld._33 = 0.1f;
#endif
        m_cpReflection[i].mWorld._42 = -5.0f;
    }
    m_cpPiece[PAWN].mWorld._41 = m_cpReflection[PAWN].mWorld._41 = SQUAREORIGIN + SQUAREDIM * 2.0f;
    m_cpPiece[PAWN].mWorld._43 = m_cpReflection[PAWN].mWorld._43 = SQUAREORIGIN + SQUAREDIM * 2.0f;
    m_cpPiece[ROOK].mWorld._41 = m_cpReflection[ROOK].mWorld._41 = SQUAREORIGIN + SQUAREDIM * 4.0f;
    m_cpPiece[ROOK].mWorld._43 = m_cpReflection[ROOK].mWorld._43 = SQUAREORIGIN + SQUAREDIM * 1.0f;
    m_cpPiece[KNIGHT].mWorld._41 = m_cpReflection[KNIGHT].mWorld._41 = SQUAREORIGIN + SQUAREDIM * 5.0f;
    m_cpPiece[KNIGHT].mWorld._43 = m_cpReflection[KNIGHT].mWorld._43 = SQUAREORIGIN + SQUAREDIM * 3.0f;
    m_cpPiece[BISHOP].mWorld._41 = m_cpReflection[BISHOP].mWorld._41 = SQUAREORIGIN + SQUAREDIM * 5.0f;
    m_cpPiece[BISHOP].mWorld._43 = m_cpReflection[BISHOP].mWorld._43 = SQUAREORIGIN + SQUAREDIM * 2.0f;
    m_cpPiece[QUEEN].mWorld._41 = m_cpReflection[QUEEN].mWorld._41 = SQUAREORIGIN + SQUAREDIM * 2.0f;
    m_cpPiece[QUEEN].mWorld._43 = m_cpReflection[QUEEN].mWorld._43 = SQUAREORIGIN + SQUAREDIM * 4.0f;
    m_cpPiece[KING].mWorld._41 = m_cpReflection[KING].mWorld._41 = SQUAREORIGIN + SQUAREDIM * 4.0f;
    m_cpPiece[KING].mWorld._43 = m_cpReflection[KING].mWorld._43 = SQUAREORIGIN + SQUAREDIM * 6.0f;
    m_cpPiece[FALLENKING].mWorld._41 = m_cpReflection[FALLENKING].mWorld._41 = SQUAREORIGIN + SQUAREDIM * 5.0f;
    m_cpPiece[FALLENKING].mWorld._43 = m_cpReflection[FALLENKING].mWorld._43 = SQUAREORIGIN + SQUAREDIM * 5.0f;

    InitMatrix(&mRotate,
        (float)( 1.0), (float)( 0.0), (float)( 0.0), (float)( 0.0),
        (float)( 0.0), (float)( cos(M_PI)), (float)( sin(M_PI)), (float)( 0.0),
        (float)( 0.0), (float)(-sin(M_PI)), (float)( cos(M_PI)), (float)( 0.0),
        (float)( 0.0), (float)( 0.0), (float)( 0.0), (float)( 1.0)
    );

    m = m_mIdentity;
    m._11 = (float)cos(M_PI);
    m._13 = -(float)sin(M_PI);
    m._31 = (float)sin(M_PI);
    m._33 = (float)cos(M_PI);

    D3DXMatrixMultiply(&mRotate, &m, &mRotate);

    for (i = 0; i < 6; i++) {
        D3DXMatrixMultiply(&m_cpReflection[i].mWorld, &mRotate, &m_cpReflection[i].mWorld);
    }

#ifndef FALLENKING_ILLUSION
    mRotate = m_mIdentity;
#endif

    mTransform = m_mIdentity;
    mTransform._11 = (float)cos(M_PIDIV4);
    mTransform._13 = (float)-sin(M_PIDIV4);
    mTransform._31 = (float)sin(M_PIDIV4);
    mTransform._33 = (float)cos(M_PIDIV4);

    mRotate._22 = (float)cos(M_PIDIV2 * 1.035f);
    mRotate._23 = (float)sin(M_PIDIV2 * 1.035f);
    mRotate._32 = (float)-sin(M_PIDIV2 * 1.035f);
    mRotate._33 = (float)cos(M_PIDIV2 * 1.035f);

    D3DXMatrixMultiply(&m_cpPiece[FALLENKING].mWorld, &mTransform, &m_cpPiece[FALLENKING].mWorld);
    D3DXMatrixMultiply(&m_cpPiece[FALLENKING].mWorld, &mRotate, &m_cpPiece[FALLENKING].mWorld);

    mRotate._22 = (float)cos(M_PIDIV2 * 0.965f);
    mRotate._23 = (float)sin(M_PIDIV2 * 0.965f);
    mRotate._32 = (float)-sin(M_PIDIV2 * 0.965f);
    mRotate._33 = (float)cos(M_PIDIV2 * 0.965f);

    D3DXMatrixMultiply(&m_cpReflection[FALLENKING].mWorld, &mTransform, &m_cpReflection[FALLENKING].mWorld);
    D3DXMatrixMultiply(&m_cpReflection[FALLENKING].mWorld, &mRotate, &m_cpReflection[FALLENKING].mWorld);

    mTransform = m_mIdentity;
    mTransform._42 = 0.475f;
    D3DXMatrixMultiply(&m_cpPiece[FALLENKING].mWorld, &m_cpPiece[FALLENKING].mWorld, &mTransform);

    mTransform = m_mIdentity;
    mTransform._42 = -0.475f;
    D3DXMatrixMultiply(&m_cpReflection[FALLENKING].mWorld, &m_cpReflection[FALLENKING].mWorld, &mTransform);

    m_pd3drBSurface = CreateVertexBuffer(m_pDevice, m_prBoardSurface, 4 * sizeof(VERTEX), 0, FVF_VERTEX);
    if (!m_pd3drBSurface) {
        return FALSE;
    }

#ifdef UNDER_XBOX
    m_pd3drBSurface->MoveResourceMemory(D3DMEM_VIDEO);
#endif

    m_pd3drBEdge = CreateVertexBuffer(m_pDevice, m_prBoardEdge, 20 * sizeof(VERTEX), 0, FVF_VERTEX);
    if (!m_pd3drBEdge) {
        return FALSE;
    }

#ifdef UNDER_XBOX
    m_pd3drBEdge->MoveResourceMemory(D3DMEM_VIDEO);
#endif

    m_pd3drScreen = CreateVertexBuffer(m_pDevice, m_plrScreen, 16 * sizeof(LVERTEX), 0, FVF_LVERTEX);
    if (!m_pd3drScreen) {
        return FALSE;
    }

#ifdef UNDER_XBOX
    m_pd3drScreen->MoveResourceMemory(D3DMEM_VIDEO);
#endif

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     Efface
//
// Description:
//
//     Release all device resource objects (or at least those objects created
//     in video memory, non-local video memory, or the default memory pools)
//     and restore the device to its initial state.
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
void CChessboard::Efface() {

    CScene::Efface();
}

//******************************************************************************
//
// Method:
//
//     Update
//
// Description:
//
//     Update the state of the scene to coincide with the given time.
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
void CChessboard::Update() {

    float fDelta = m_fTimeDelta / 0.0075f;

    if (m_bRotateCamera) {
        m_fYRotationDelta = CAMERA_ROTATION_DELTA * fDelta;
    }

    m_svView.fTranslation += m_fTranslationDelta * fDelta;
    if (m_svView.fTranslation < 0.0f) {
        m_svView.fTranslation = 0.0f;
    }
    m_svView.fXRotation += m_fXRotationDelta * fDelta;
    if (m_svView.fXRotation < -M_PIDIV2) {
        m_svView.fXRotation = -M_PIDIV2;
    }
    else if (m_svView.fXRotation > M_PIDIV2) {
        m_svView.fXRotation = M_PIDIV2;
    }
    m_svView.fYRotation += m_fYRotationDelta * fDelta;
    if (m_svView.fYRotation < 0.0f) {
        m_svView.fYRotation += M_2PI;
    }
    else if (m_svView.fYRotation > M_2PI) {
        m_svView.fYRotation -= M_2PI;
    }
}

//******************************************************************************
//
// Method:
//
//     Render
//
// Description:
//
//     Render the test scene.
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
BOOL CChessboard::Render() {

    D3DXVECTOR3 vLightDir;
    DWORD       dwClearFlags = D3DCLEAR_TARGET;
    UINT        i;
    HRESULT     hr;

#ifndef UNDER_XBOX
    hr = m_pDevice->TestCooperativeLevel();
    if (FAILED(hr)) {
        if (hr == D3DERR_DEVICELOST) {
            return TRUE;
        }
        if (hr == D3DERR_DEVICENOTRESET) {
            if (!Reset()) {
                return FALSE;
            }
        }
    }
#endif

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    }
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view
    SetViewTransform(&m_svView);

    m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_VERTEX);

    // Render the pawn's reflection
    m_pDevice->SetMaterial(&m_matPawn);

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

#ifdef PIXEL_LIGHTING

    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    // Transform the light into camera space
    vLightDir.x = m_vLightDir.x * m_mView._11 + m_vLightDir.y * m_mView._21 + m_vLightDir.z * m_mView._31;
    vLightDir.y = m_vLightDir.x * m_mView._12 + m_vLightDir.y * m_mView._22 + m_vLightDir.z * m_mView._32;
    vLightDir.z = m_vLightDir.x * m_mView._13 + m_vLightDir.y * m_mView._23 + m_vLightDir.z * m_mView._33;

    // Update the cubemap with a light direction in camera space
    UpdateHalfVectorMap(m_pd3dtcHVec, &vLightDir);

    m_pDevice->SetTexture(0, m_pd3dtcNorm);
    m_pDevice->SetTexture(1, m_pd3dtcHVec);
    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
    m_pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
    m_pDevice->SetTextureStageState(3, D3DTSS_TEXCOORDINDEX, 0);
    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);
    SetColorStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_DOTPRODUCT3);
    SetColorStage(m_pDevice, 2, D3DTA_CURRENT, D3DTA_CURRENT, D3DTOP_MODULATE);
    SetColorStage(m_pDevice, 3, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);

#else

    m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);

#endif // PIXEL_LIGHTING

    if (!(m_svView.fXRotation > 0.0f && (m_svView.fTranslation * sin(m_svView.fXRotation)) > 0.895f)) {

        for (i = 0; i < 7; i++) {
#ifdef PIXEL_LIGHTING
            m_pDevice->SetTexture(3, i < 6 ? m_pd3dtPawn : m_pd3dtFallen);
#else
            m_pDevice->SetTexture(0, i < 6 ? m_pd3dtPawn : m_pd3dtFallen);
#endif // PIXEL_LIGHTING
            m_pDevice->SetTransform(D3DTS_WORLD, &m_cpReflection[i].mWorld);
            m_pDevice->SetStreamSource(0, m_cpReflection[i].pcmMesh->pd3dr, sizeof(VERTEX));
            m_pDevice->SetIndices(m_cpReflection[i].pcmMesh->pd3di, 0);
            m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_cpReflection[i].pcmMesh->uNumVertices, 0, m_cpReflection[i].pcmMesh->uNumPrimitives);
        }
    }

    for (i = 0; i < 7; i++) {
#ifdef PIXEL_LIGHTING
        m_pDevice->SetTexture(3, i < 6 ? m_pd3dtPawn : m_pd3dtFallen);
#else
        m_pDevice->SetTexture(0, i < 6 ? m_pd3dtPawn : m_pd3dtFallen);
#endif // PIXEL_LIGHTING
        m_pDevice->SetTransform(D3DTS_WORLD, &m_cpPiece[i].mWorld);
        m_pDevice->SetStreamSource(0, m_cpPiece[i].pcmMesh->pd3dr, sizeof(VERTEX));
        m_pDevice->SetIndices(m_cpPiece[i].pcmMesh->pd3di, 0);
        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_cpPiece[i].pcmMesh->uNumVertices, 0, m_cpPiece[i].pcmMesh->uNumPrimitives);
    }

    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

    m_pDevice->SetTransform(D3DTS_WORLD, &m_mBoard);

    // Render the pawn reflection screen
    m_pDevice->SetVertexShader(FVF_LVERTEX);

    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pDevice->SetStreamSource(0, m_pd3drScreen, sizeof(LVERTEX));

    for (i = 0; i < 4; i++) {
        m_pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN,
                i * 4, 2);
    }

    m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

    // Render the edge of the chessboard
    m_pDevice->SetMaterial(&m_matBoardEdge);
    m_pDevice->SetTexture(0, m_pd3dtBoardEdge);

    m_pDevice->SetVertexShader(FVF_VERTEX);

    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

    m_pDevice->SetStreamSource(0, m_pd3drBEdge, sizeof(VERTEX));

    for (i = 0; i < 5; i++) {
        m_pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN,
                i * 4, 2);
    }

    // Render the surface of the chessboard
    m_pDevice->SetMaterial(&m_matBoardSurface);
    m_pDevice->SetTexture(0, m_pd3dtBoardSurface);

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    m_pDevice->SetStreamSource(0, m_pd3drBSurface, sizeof(VERTEX));

    m_pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 
            0, 2);

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    // Display the adapter mode
    ShowDisplayMode();

    // Display the frame rate
    ShowFrameRate();

    // Display the console
    m_pDisplay->ShowConsole();

    // Fade out the scene on exit
    if (m_pfnFade) {
        (this->*m_pfnFade)();
    }

    // End the scene
    m_pDevice->EndScene();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     ProcessInput
//
// Description:
//
//     Process user input for the scene.
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
void CChessboard::ProcessInput() {

    KEYSDOWN kdKeys;

    CScene::ProcessInput();
    kdKeys = m_pDisplay->GetKeyState();

    m_fTranslationDelta = 0.0f;
    m_fXRotationDelta = 0.0f;
    m_fYRotationDelta = 0.0f;

    // A Button
    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_X)) {
        m_bRotateCamera = TRUE;
    }
    
    // Left trigger
    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_LTRIG)) {
        m_fTranslationDelta = -CAMERA_TRANSLATION_DELTA;
    }

    // Right trigger
    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_RTRIG)) {
        m_fTranslationDelta = CAMERA_TRANSLATION_DELTA;
    }

    // Joystick x-axis
    m_fYRotationDelta = m_jsJoys.f1X * CAMERA_ROTATION_DELTA;
    if (m_jsJoys.f1X != 0.0f) {
        m_bRotateCamera = FALSE;
    }

    // Joystick y-axis
    m_fXRotationDelta = m_jsJoys.f1Y * CAMERA_ROTATION_DELTA;

    if (kdKeys & KEY_LEFT) {
        m_fYRotationDelta = -CAMERA_ROTATION_DELTA;
        m_bRotateCamera = FALSE;
    }

    if (kdKeys & KEY_RIGHT) {
        m_fYRotationDelta = CAMERA_ROTATION_DELTA;
        m_bRotateCamera = FALSE;
    }

    if (kdKeys & KEY_UP) {
        m_fXRotationDelta = -CAMERA_ROTATION_DELTA;
    }

    if (kdKeys & KEY_DOWN) {
        m_fXRotationDelta = CAMERA_ROTATION_DELTA;
    }

    if (kdKeys & KEY_ADD || kdKeys & KEY_PAGEUP) {
        m_fTranslationDelta = -CAMERA_TRANSLATION_DELTA;
    }

    if (kdKeys & KEY_SUBTRACT || kdKeys & KEY_INSERT) {
        m_fTranslationDelta = CAMERA_TRANSLATION_DELTA;
    }
}

//******************************************************************************
//
// Method:
//
//     InitView
//
// Description:
//
//     Initialize the camera view in the scene.
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
BOOL CChessboard::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_svInitial.vInterest = D3DXVECTOR3(0.0f, -4.1f, 0.0f);
    m_svInitial.fTranslation = (float)(11.75f);
    m_svInitial.fXRotation = -0.189f;
    m_svInitial.fYRotation = M_2PI;
    memcpy(&m_svView, &m_svInitial, sizeof(SVIEW));
    SetViewTransform(&m_svView);
    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     SetViewTransform
//
// Description:
//
//     Set the view transform according to the fields of the given CAMERA.
//
// Arguments:
//
//     LPDIRECT3DDEVICE2 pd3dDevice   - Direct3DDevice object
//
//     PCAMERA pcam             - Camera data
//
// Return Value:
//
//     Initialized view matrix.
//
//******************************************************************************
void CChessboard::SetViewTransform(PSVIEW psv) {

    D3DXMATRIX m1, m2;

    // Translate the view away from the interest by the given distance
    InitMatrix(&m1,
        (float)( 1.0f), (float)( 0.0f), (float)( 0.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 1.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)(psv->fTranslation), (float)( 1.0f)
    );
    m2 = m1;

    // Rotate the view around the x axis
    m1._43 = 0.0f;
    m1._22 = (float)(cos(psv->fXRotation));
    m1._23 = (float)(sin(psv->fXRotation));
    m1._32 = -m1._23;
    m1._33 = m1._22;
    D3DXMatrixMultiply(&m2, &m1, &m2);

    // Rotate the view around the y axis
    m1._22 = 1.0f;
    m1._23 = 0.0f;
    m1._32 = 0.0f;
    m1._11 = (float)(cos(psv->fYRotation));
    m1._13 = (float)(-sin(psv->fYRotation));
    m1._31 = -m1._13;
    m1._33 = m1._11;
    D3DXMatrixMultiply(&m2, &m1, &m2);

    // Translate the view to position it at the camera interest
    m1._11 = 1.0f;
    m1._13 = 0.0f;
    m1._31 = 0.0f;
    m1._33 = 1.0f;
    m1._41 = -psv->vInterest.x;
    m1._42 = -psv->vInterest.y;
    m1._43 = -psv->vInterest.z;
    D3DXMatrixMultiply(&m_mView, &m1, &m2);

    m_pDevice->SetTransform(D3DTS_VIEW, &m_mView);
}

//******************************************************************************
// Scene window procedure (pseudo-subclassed off the main window procedure)
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     WndProc
//
// Description:
//
//     Scene window procedure to process messages received by the main 
//     application window.
//
// Arguments:
//
//     LRESULT* plr             - Result of the message processing
//
//     HWND hWnd                - Application window
//
//     UINT uMsg                - Message to process
//
//     WPARAM wParam            - First message parameter
//
//     LPARAM lParam            - Second message parameter
//
// Return Value:
//
//     TRUE if the message was handled, FALSE otherwise.
//
//******************************************************************************
BOOL CChessboard::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    *plr = 0;

#ifndef UNDER_XBOX

    switch (uMsg) {

        case WM_KEYDOWN:

            switch (wParam) {

                case VK_RETURN: {
                    BOOL bRet = !m_bRotateCamera;
                    m_bRotateCamera = TRUE;
                    if (bRet) {
                        return TRUE;
                    }
                }
            }

            break;

        case WM_CHAR:

            switch (wParam) {

                case TEXT('O'):
                case TEXT('o'):
                    // Reset the camera to its initial position
                    memcpy(&m_svView, &m_svInitial, sizeof(SVIEW));
                    return TRUE;
            }

            break;
    }

#endif // !UNDER_XBOX

    return CScene::WndProc(plr, hWnd, uMsg, wParam, lParam);
}
