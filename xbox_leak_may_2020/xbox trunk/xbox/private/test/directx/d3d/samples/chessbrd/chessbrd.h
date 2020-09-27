/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    chessbrd.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __CHESSBRD_H__
#define __CHESSBRD_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define CAMERA_ROTATION_DELTA        0.011780625f//0.0157075f
#define CAMERA_TRANSLATION_DELTA     0.5f

#define PAWN                         0
#define ROOK                         1
#define KNIGHT                       2
#define BISHOP                       3
#define QUEEN                        4
#define KING                         5
#define FALLENKING                   6

//******************************************************************************
// Structures
//******************************************************************************

typedef struct SVIEW {
    D3DXVECTOR3 vInterest;
    float       fTranslation;
    float       fXRotation;
    float       fYRotation;
} SVIEW, *PSVIEW;

typedef struct _CHESSMESH {
    LPDIRECT3DVERTEXBUFFER8  pd3dr;
    LPDIRECT3DINDEXBUFFER8   pd3di;
    UINT                     uNumVertices;
    UINT                     uNumIndices;
    UINT                     uNumPrimitives;
} CHESSMESH, *PCHESSMESH;

typedef struct _CHESSPIECE {
    PCHESSMESH      pcmMesh;
    D3DXMATRIX      mWorld;
} CHESSPIECE, *PCHESSPIECE;

//******************************************************************************
// Function prototypes
//******************************************************************************

int         ExhibitScene(CDisplay* pDisplay);
BOOL        ValidateDisplay(LPDIRECT3D8* pd3d, D3DCAPS8* pd3dcaps, 
                            D3DDISPLAYMODE* pd3ddm);

//******************************************************************************
class CChessboard : public CScene {

protected:

    LPDIRECT3DCUBETEXTURE8          m_pd3dtcNorm;
    LPDIRECT3DCUBETEXTURE8          m_pd3dtcHVec;
    CHESSMESH                       m_cmChessMesh[6];
    CHESSPIECE                      m_cpPiece[7];
    CHESSPIECE                      m_cpReflection[7];
    D3DXVECTOR3                     m_vLightDir;
    D3DXMATRIX                      m_mView;
    LPDIRECT3DVERTEXBUFFER8         m_pd3drBSurface;
    LPDIRECT3DVERTEXBUFFER8         m_pd3drBEdge;
    LPDIRECT3DVERTEXBUFFER8         m_pd3drScreen;
    VERTEX                          m_prBoardSurface[4];
    VERTEX                          m_prBoardEdge[5][4];
    LVERTEX                         m_plrScreen[4][4];
    D3DMATERIAL8                    m_matPawn;
    D3DMATERIAL8                    m_matBoardEdge;
    D3DMATERIAL8                    m_matBoardSurface;
    LPDIRECT3DTEXTURE8              m_pd3dtBoardSurface;
    LPDIRECT3DTEXTURE8              m_pd3dtBoardEdge;
    LPDIRECT3DTEXTURE8              m_pd3dtPawn;
    LPDIRECT3DTEXTURE8              m_pd3dtFallen;
    D3DXMATRIX                      m_mBoard, m_mPawn, m_mReflection;
    SVIEW                           m_svView;
    SVIEW                           m_svInitial;
    float                           m_fTranslationDelta;
    float                           m_fXRotationDelta;
    float                           m_fYRotationDelta;
    BOOL                            m_bRotateCamera;

public:

                                    CChessboard();
                                    ~CChessboard();

    virtual BOOL                    Create(CDisplay* pDisplay);
    virtual int                     Exhibit(int *pnExitCode);

protected:

    virtual BOOL                    Setup();
    virtual BOOL                    Initialize();
    virtual void                    Efface();
    virtual void                    Update();
    virtual BOOL                    Render();

    virtual void                    ProcessInput();
    virtual BOOL                    InitView();

    virtual void                    SetViewTransform(PSVIEW psv);

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);
};

#endif //__CHESSBRD_H__
