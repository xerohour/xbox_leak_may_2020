/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    reflect.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __REFLECT_H__
#define __REFLECT_H__

//******************************************************************************
#define FVF_CUBEVERTEX          (D3DFVF_XYZ | D3DFVF_TEX1 | \
                                 D3DFVF_TEXCOORDSIZE3(0))

//******************************************************************************
typedef struct _CUBEVERTEX {
    D3DXVECTOR3   vPosition;
    float         u0, v0, t0;

    _CUBEVERTEX() { }
    _CUBEVERTEX(const D3DVECTOR& v, float _u0, float _v0, float _t0) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        u0 = _u0; v0 = _v0; t0 = _t0;
    }
} CUBEVERTEX, *PCUBEVERTEX;

//******************************************************************************
class CReflect0 : public CScene {

protected:

    CVertexBuffer8*                 m_pd3drCube;
    CIndexBuffer8*                  m_pd3diCube;
    CTexture8*                      m_pd3dtCube;

    CVertexBuffer8*                 m_pd3drSphere;
    CIndexBuffer8*                  m_pd3diSphere;
    DWORD                           m_dwSphereVertices;
    DWORD                           m_dwSphereIndices;

    CAMERA                          m_cam;

public:

                                    CReflect0();
                                    ~CReflect0();

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

    BOOL                            InitCube(LPCTSTR szTexture, float fRadius);
};

#endif //__REFLECT_H__
