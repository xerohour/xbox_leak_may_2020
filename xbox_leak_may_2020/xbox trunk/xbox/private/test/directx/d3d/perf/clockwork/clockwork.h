/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    clockwork.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __CLOCKWORK_H__
#define __CLOCKWORK_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define ORBIT_FRAMES            420

#define SPHERE_U                16
#define SPHERE_V                16

#define BOUND_RADIUS            48.0f//(D3DVAL(NUM_GEARS) * 0.5f)
#define VIEW_Z                  (-(BOUND_RADIUS + 16.0f))

//******************************************************************************
class CStateChangePerf : public CScene {

protected:

    D3DXMATRIX                      m_mWorld;
    D3DXMATRIX                      m_mProj;
    PVERTEX                         m_prSphere;
    DWORD                           m_dwSphereVertices;
    LPWORD                          m_pwSphere;
    DWORD                           m_dwSphereIndices;
    DWORD                           m_dwSpherePrimitives;
    LPDIRECT3DVERTEXBUFFER8         m_pd3drSphere;
    LPDIRECT3DINDEXBUFFER8          m_pd3diSphere;
    CClockwork*                     m_pClockwork;
    LPDIRECT3DTEXTURE8              m_pd3dtBackground;
    D3DMATERIAL8                    m_material;
    D3DLIGHT8                       m_light;
    float                           m_fOrbitAngle;
    CAMERA                          m_cam;

    float                           m_fTimeFreq;
    float                           m_fStartTime;
    UINT                            m_uOrbitFrame;

    FRAMESTATS                      m_fsStats;
    BOOL                            m_bProgShader;

    DWORD                           m_dwVShader;
    DWORD                           m_dwPShader;

public:

                                    CStateChangePerf();
                                    ~CStateChangePerf();

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
};

#endif //__CLOCKWORK_H__
