/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    mirror.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __MIRROR_H__
#define __MIRROR_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

// Flexible vertex format for a diffusely lit vertex with 2 texture coordinates
#define FVF_MVERTEX (D3DFVF_XYZ | D3DFVF_TEX2)

//******************************************************************************
// Lit vertex format with two texture coordinates (for multistage texturing)
typedef struct _MVERTEX {
    D3DXVECTOR3 vPosition;
    float       u0, v0;
    float       u1, v1;
} MVERTEX, *PMVERTEX;

//******************************************************************************
class CMirror : public CScene {

protected:

    D3DXMATRIX                      m_mWorld;
    D3DXMATRIX                      m_mView;
    D3DXMATRIX                      m_mProj;
    BOOL                            m_bShade;
    VERTEX                          m_prFloor[4];
    MVERTEX                         m_prMirror[4];
    CTexture8*                      m_pd3dtFloor;
    CTexture8*                      m_pd3dtShade;
    CTexture8*                      m_pd3dtMirror;
    CSurface8*                      m_pd3dsMirror;
    CSurface8*                      m_pd3dsZMirror;
    CSurface8*                      m_pd3dsBack;
    CSurface8*                      m_pd3dsDepth;

public:

                                    CMirror();
                                    ~CMirror();

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
                                        PMVERTEX prVertices, D3DXMATRIX* pmWorld);
    HRESULT                         SetReflectionTextureCoords(PMVERTEX prVertices, 
                                        UINT uNumVertices);
};

#endif //__MIRROR_H__
