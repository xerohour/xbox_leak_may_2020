/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    dlight.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __DLIGHT_H__
#define __DLIGHT_H__

#define MAX_DLIGHTS		50
#define NUM_DLIGHTS		4
#define MAX_RANGE		30.0f
#define TARGET_RADIUS	10.0f

#define MESH_SIZE	8
#define SPHERE_VERTICES	(2+MESH_SIZE*MESH_SIZE*2)
#define SPHERE_INDICES	((MESH_SIZE*4 + MESH_SIZE*4*(MESH_SIZE-1))*3)

//******************************************************************************
class CDLight : public CScene {

protected:

    float                           m_tic;
    D3DXVECTOR3                     m_vLPos[MAX_DLIGHTS];
    D3DCOLOR                        m_cLCol[MAX_DLIGHTS];
    D3DXMATRIX                      m_mTranslate;
    D3DXMATRIX                      m_mScale;
    D3DCAPS8                        m_d3dcaps;

public:

                                    CDLight();
                                    ~CDLight();

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

#endif //__DLIGHT_H__
