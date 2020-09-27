/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    bumpdudv.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __BUMPDUDV_H__
#define __BUMPDUDV_H__

//******************************************************************************
class CBumpDuDv1 : public CScene {

protected:

    CTexture8*                      m_pd3dtBase;
    CTexture8*                      m_pd3dtBump;
    CTexture8*                      m_pd3dtEnv;

    CVertexBuffer8*                 m_pd3drSphere;
    CIndexBuffer8*                  m_pd3diSphere;
    PBVERTEX2                       m_prSphere;
    DWORD                           m_dwSphereVertices;
    LPWORD                          m_pwSphere;
    DWORD                           m_dwSphereIndices;

    D3DXMATRIX                      m_mWorld;

    D3DXVECTOR3                     m_vLightDir;

public:

                                    CBumpDuDv1();
                                    ~CBumpDuDv1();

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

#endif //__BUMPDUDV_H__
