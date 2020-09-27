/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    dragnfly.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __DRAGNFLY_H__
#define __DRAGNFLY_H__

#define FVF_DVERTEX              (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//******************************************************************************
// Structures
//******************************************************************************

//******************************************************************************
typedef struct _DVERTEX {
    D3DXVECTOR3 vPosition;
    D3DXVECTOR3 vNormal;
    D3DCOLOR    cDiffuse;
    float       u0, v0;

    _DVERTEX() {}
    _DVERTEX(const D3DVECTOR& v, const D3DVECTOR& n, D3DCOLOR _cDiffuse, float _u0, float _v0) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        vNormal.x = n.x; vNormal.y = n.y; vNormal.z = n.z;
        cDiffuse = _cDiffuse;
        u0 = _u0; v0 = _v0;
    }
} DVERTEX, *PDVERTEX;

//******************************************************************************
class CDragonfly : public CScene {

protected:

    MESH                            m_mesh;
    PDVERTEX                        m_prVertices;
    CVertexBuffer8*                 m_pd3dr;
    CIndexBuffer8*                  m_pd3di;

public:

                                    CDragonfly();
                                    ~CDragonfly();

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

#endif //__DRAGNFLY_H__
