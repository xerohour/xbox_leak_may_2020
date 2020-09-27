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

//#define LIMITED

#define ORBIT_FRAMES            420

#define SPHERE_U                16
#define SPHERE_V                16

#ifdef LIMITED
#define NUM_GEARS               50
#define BOUND_RADIUS            32.0f//(D3DVAL(NUM_GEARS) * 0.5f)
#else
#define NUM_GEARS               200
#define BOUND_RADIUS            48.0f//(D3DVAL(NUM_GEARS) * 0.5f)
#endif // LIMITED

#define VIEW_Z                  (-(BOUND_RADIUS + 16.0f))

//******************************************************************************
// Function prototypes
//******************************************************************************

int         ExhibitScene(CDisplay* pDisplay);
BOOL        ValidateDisplay(LPDIRECT3D8 pd3d, D3DCAPS8* pd3dcaps, 
                            D3DDISPLAYMODE* pd3ddm);

//******************************************************************************
class CClockwork : public CScene {

protected:

    D3DXMATRIX                      m_mWorld;
    PVERTEX                         m_prSphere;
    DWORD                           m_dwSphereVertices;
    LPWORD                          m_pwSphere;
    DWORD                           m_dwSphereIndices;
    LPDIRECT3DVERTEXBUFFER8         m_pd3drSphere;
    LPDIRECT3DINDEXBUFFER8          m_pd3diSphere;
    CLOCKWORK                       m_cw;
    LPDIRECT3DTEXTURE8              m_pd3dtBackground;
    float                           m_fOrbitAngle;
    CAMERA                          m_cam;

public:

                                    CClockwork();
                                    ~CClockwork();

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
