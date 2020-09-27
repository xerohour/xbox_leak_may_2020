/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    bumpnorm.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __BUMPNORM_H__
#define __BUMPNORM_H__

//******************************************************************************
class CBumpNormal1 : public CScene {

protected:

    CTexture8*                      m_pd3dtBase;
    CTexture8*                      m_pd3dtBump;
    CTexture8*                      m_pd3dtEnv;
    CCubeTexture8*                  m_pd3dtcNormal;

    CVertexBuffer8*                 m_pd3drSphere;
    CIndexBuffer8*                  m_pd3diSphere;
    PNVERTEX                        m_prSphere;
    NVERTEX                         m_prQuad[4];
    PVERTEX                         m_prSphere2;
    DWORD                           m_dwSphereVertices;
    LPWORD                          m_pwSphere;
    DWORD                           m_dwSphereIndices;

    D3DXMATRIX                      m_mWorld;

    D3DXVECTOR3                     m_vLightDir;

PMATRIX3X3 m_pmTangent;

public:

                                    CBumpNormal1();
                                    ~CBumpNormal1();

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

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);
};

#endif //__BUMPNORM_H__
