/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    test.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __TEST_H__
#define __TEST_H__

#define FVF_CLVERTEX              (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR)

//******************************************************************************
typedef struct _CLVERTEX {
    D3DXVECTOR3 vPosition;
    D3DXVECTOR3 vNormal;
    D3DCOLOR    cDiffuse;
    D3DCOLOR    cSpecular;

    _CLVERTEX() {}
    _CLVERTEX(const D3DVECTOR& v, const D3DVECTOR& n, D3DCOLOR _cDiffuse, D3DCOLOR _cSpecular) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        vNormal.x = n.x; vNormal.y = n.y; vNormal.z = n.z;
        cDiffuse = _cDiffuse; cSpecular = _cSpecular;
    }
} CLVERTEX, *PCLVERTEX;

//******************************************************************************
class CTest : public CScene {

protected:

    PVERTEX                         m_prSphere;
    DWORD                           m_dwSphereVertices;
    LPWORD                          m_pwSphere;
    DWORD                           m_dwSphereIndices;

    CVertexBuffer8*                 m_pd3drSphere;
    CIndexBuffer8*                  m_pd3diSphere;

public:

                                    CTest();
                                    ~CTest();

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

#endif //__TEST_H__
