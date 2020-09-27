/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    portal.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __PORTAL_H__
#define __PORTAL_H__

// Flexible vertex format for a lit vertex with 2 texture coordinates
#define FVF_MLVERTEX       (D3DFVF_XYZ | D3DFVF_TEX2)

// Lit vertex format with two texture coordinates (for multistage texturing)
typedef struct _MLVERTEX {
    D3DXVECTOR3 vectPosition;
    float       u0, v0;
    float       u1, v1;
} MLVERTEX, *PMLVERTEX;

//******************************************************************************
class CPortal : public CScene {

protected:

    D3DXMATRIX                      m_mWorld;
    D3DXMATRIX                      m_mView;
    D3DXMATRIX                      m_mProj;
    CTexture8*                      m_pd3dtFloor;
    CTexture8*                      m_pd3dtWall;
    CTexture8*                      m_pd3dtShade;
    CTexture8*                      m_pd3dtMirror;
    CSurface8*                      m_pd3dsMirror;
    CSurface8*                      m_pd3dsZMirror;
    CSurface8*                      m_pd3dsBack;
    CSurface8*                      m_pd3dsDepth;
    VERTEX                          m_rgvertFloors[8];
    VERTEX                          m_rgvertWalls[16];
    WORD                            m_rgwFloorIndices[12];
    WORD                            m_rgwWallIndices[24];
    MLVERTEX                        m_rgvertMirror[4];
    CAMERA                          m_cam;

public:

                                    CPortal();
                                    ~CPortal();

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
};

#endif //__PORTAL_H__
