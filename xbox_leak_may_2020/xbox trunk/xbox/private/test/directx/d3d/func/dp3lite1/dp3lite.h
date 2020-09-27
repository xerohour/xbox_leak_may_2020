/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    dp3lite.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __DP3LITE_H__
#define __DP3LITE_H__

//******************************************************************************
class CDiffuseMap : public CScene {

protected:

    CCubeTexture8*                  m_pd3dtcNorm;
    CCubeTexture8*                  m_pd3dtcHVec;
    CVertexBuffer8*                 m_pd3drSphere2;
    CVertexBuffer8*                 m_pd3drSphere0;
    CIndexBuffer8*                  m_pd3diSphere;
    PVERTEX                         m_prSphere2;
    PCVERTEX                        m_prSphere0;
    DWORD                           m_dwSphereVertices;
    LPWORD                          m_pwSphere;
    DWORD                           m_dwSphereIndices;
    D3DLIGHT8                       m_light;
    D3DXMATRIX                      m_mWorld;
    D3DXVECTOR3                     m_vSpherePos[6];
    D3DXVECTOR3                     m_vLightDir;
    float                           m_fAngle;

public:

                                    CDiffuseMap();
                                    ~CDiffuseMap();

    virtual BOOL                    Create(CDisplay* pDisplay);
    virtual int                     Exhibit(int *pnExitCode);

protected:

    virtual BOOL                    Initialize();
    virtual void                    Efface();
    virtual void                    Update();
    virtual BOOL                    Render();

    virtual void                    ProcessInput();
    virtual BOOL                    InitView();

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);
};

#endif //__DP3LITE_H__
