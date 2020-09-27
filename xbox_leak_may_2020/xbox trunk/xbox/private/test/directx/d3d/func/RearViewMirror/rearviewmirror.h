/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    rearviewmirror.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __REARVIEWMIRROR_H__
#define __REARVIEWMIRROR_H__

// Flexible vertex format for a lit vertex with 2 texture coordinates
#define FVF_MLVERTEX       (D3DFVF_XYZ | D3DFVF_TEX2)

// Lit vertex format with two texture coordinates (for multistage texturing)
typedef struct _MLVERTEX {
    D3DXVECTOR3 vectPosition;
    float       u0, v0;
    float       u1, v1;
} MLVERTEX, *PMLVERTEX;

//******************************************************************************
class CRearViewMirror : public CScene {

protected:

    D3DXMATRIX                      m_mCube;
    D3DXMATRIX                      m_mMirror;
    D3DXMATRIX                      m_mView;
    D3DXMATRIX                      m_mProj;
    CTexture8*                      m_pd3dtTrench;
    CTexture8*                      m_pd3dtShade;
    CTexture8*                      m_pd3dtMirror;
    CSurface8*                      m_pd3dsMirror;
    CSurface8*                      m_pd3dsZMirror;
    CSurface8*                      m_pd3dsBack;
    CSurface8*                      m_pd3dsDepth;
    MLVERTEX                        m_rgvertMirror[4];
    VERTEX                          m_rgvertTrench[12];
    WORD				            m_rgwTrenchIndices[18];
    VERTEX                          m_rgvertCube[8];
    WORD                            m_rgwCubeIndices[36];
    D3DMATERIAL8                    m_material[5];
    D3DMATERIAL8                    m_matWhite;
    CAMERA                          m_cam;

public:

                                    CRearViewMirror();
                                    ~CRearViewMirror();

    virtual BOOL                    Create(CDisplay* pDisplay);
    virtual int                     Exhibit(int *pnExitCode);

protected:

    virtual BOOL                    Prepare();
    virtual BOOL                    Setup();
    virtual BOOL                    Initialize();
    virtual void                    Efface();
    virtual void                    Update();
    virtual BOOL                    Render();

    virtual void                    ProcessInput();
    virtual BOOL                    InitView();

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);

    BOOL                            ReplaceRenderTarget(CSurface8* pd3ds, 
                                        CSurface8* pd3dsZ, CSurface8** ppd3dsOld, 
                                        CSurface8** ppd3dsZOld, D3DRECT* pd3drect);
    BOOL                            SetDepthBuffer(CSurface8* pd3ds);
    BOOL                            SetReflectedView(PCAMERA pcam, 
                                        PMLVERTEX prVertices, D3DXMATRIX* pmWorld);
    HRESULT                         SetReflectionTextureCoords(PMLVERTEX prVertices, 
                                        UINT uNumVertices);
};

#endif //__REARVIEWMIRROR_H__
