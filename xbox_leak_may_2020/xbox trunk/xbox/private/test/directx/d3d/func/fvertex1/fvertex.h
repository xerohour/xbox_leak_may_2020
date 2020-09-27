/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    fvertex.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __FVERTEX_H__
#define __FVERTEX_H__

//******************************************************************************
// Structures
//******************************************************************************

// Untransformed, unlit
#define D3DFVF_XYZ_NORMAL (D3DFVF_XYZ | D3DFVF_NORMAL)
typedef struct _VERTEX_XYZ_NORMAL {
    D3DVECTOR vPosition;
    D3DVECTOR vNormal;
} VERTEX_XYZ_NORMAL, *PVERTEX_XYZ_NORMAL;

#define D3DFVF_XYZ_NORMAL_TEX1 (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
typedef struct _VERTEX_XYZ_NORMAL_TEX1 {
    D3DVECTOR vPosition;
    D3DVECTOR vNormal;
    float     u0, v0;
} VERTEX_XYZ_NORMAL_TEX1, *PVERTEX_XYZ_NORMAL_TEX1;

#define D3DFVF_XYZ_NORMAL_DIFFUSE_SPECULAR (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR)
typedef struct _VERTEX_XYZ_NORMAL_DIFFUSE_SPECULAR {
    D3DVECTOR vPosition;
    D3DVECTOR vNormal;
    DWORD     cDiffuse;
    DWORD     cSpecular;
} VERTEX_XYZ_NORMAL_DIFFUSE_SPECULAR, *PVERTEX_XYZ_NORMAL_DIFFUSE_SPECULAR;

#define D3DFVF_XYZ_NORMAL_DIFFUSE_SPECULAR_TEX2 (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2)
typedef struct _VERTEX_XYZ_NORMAL_DIFFUSE_SPECULAR_TEX2 {
    D3DVECTOR vPosition;
    D3DVECTOR vNormal;
    DWORD     cDiffuse;
    DWORD     cSpecular;
    float     u0, v0;
    float     u1, v1;
} VERTEX_XYZ_NORMAL_DIFFUSE_SPECULAR_TEX2, *PVERTEX_XYZ_NORMAL_DIFFUSE_SPECULAR_TEX2;

// Untransformed, lit
#define D3DFVF_XYZ_TEX1 (D3DFVF_XYZ | D3DFVF_TEX1)
typedef struct _VERTEX_XYZ {
    D3DVECTOR vPosition;
    float     u0, v0;
} VERTEX_XYZ_TEX1, *PVERTEX_XYZ_TEX1;

#define D3DFVF_XYZ_DIFFUSE (D3DFVF_XYZ | D3DFVF_DIFFUSE)
typedef struct _VERTEX_XYZ_DIFFUSE {
    D3DVECTOR vPosition;
    DWORD     cDiffuse;
} VERTEX_XYZ_DIFFUSE, *PVERTEX_XYZ_DIFFUSE;

#define D3DFVF_XYZ_SPECULAR (D3DFVF_XYZ | D3DFVF_SPECULAR)
typedef struct _VERTEX_XYZ_SPECULAR {
    D3DVECTOR vPosition;
    DWORD     cSpecular;
} VERTEX_XYZ_SPECULAR, *PVERTEX_XYZ_SPECULAR;

#define D3DFVF_XYZ_DIFFUSE_SPECULAR (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR)
typedef struct _VERTEX_XYZ_DIFFUSE_SPECULAR {
    D3DVECTOR vPosition;
    DWORD     cDiffuse;
    DWORD     cSpecular;
} VERTEX_XYZ_DIFFUSE_SPECULAR, *PVERTEX_XYZ_DIFFUSE_SPECULAR;

#define D3DFVF_XYZ_RESERVED_DIFFUSE_TEX3 (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX3)
typedef struct _VERTEX_XYZ_RESERVED1_DIFFUSE_TEX3 {
    D3DVECTOR vPosition;
    DWORD     dwReserved;
    DWORD     cDiffuse;
    float     u0, v0;
    float     u1, v1;
    float     u2, v2;
} VERTEX_XYZ_RESERVED1_DIFFUSE_TEX3, *PVERTEX_XYZ_RESERVED1_DIFFUSE_TEX3;

// Transformed, lit
#define D3DFVF_XYZRHW_SPECULAR (D3DFVF_XYZRHW | D3DFVF_SPECULAR)
typedef struct _VERTEX_XYZRHW {
    D3DVECTOR vPosition;
    float     fRHW;
    DWORD     cSpecular;
} VERTEX_XYZRHW_SPECULAR, *PVERTEX_XYZRHW_SPECULAR;

#define D3DFVF_XYZRHW_TEX4 (D3DFVF_XYZRHW | D3DFVF_TEX4)
typedef struct _VERTEX_XYZRHW_TEX4 {
    D3DVECTOR   vPosition;
    float       fRHW;
    float       u0, v0;
    float       u1, v1;
    float       u2, v2;
    float       u3, v3;
} VERTEX_XYZRHW_TEX4, *PVERTEX_XYZRHW_TEX4;

#define D3DFVF_XYZRHW_DIFFUSE_SPECULAR (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR)
typedef struct _VERTEX_XYZRHW_DIFFUSE_SPECULAR {
    D3DVECTOR vPosition;
    float     fRHW;
    DWORD     cDiffuse;
    DWORD     cSpecular;
} VERTEX_XYZRHW_DIFFUSE_SPECULAR, *PVERTEX_XYZRHW_DIFFUSE_SPECULAR;

#define D3DFVF_XYZRHW_DIFFUSE_SPECULAR_TEX4 (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX4)
typedef struct _VERTEX_XYZRHW_DIFFUSE_SPECULAR_TEX4 {
    D3DVECTOR vPosition;
    float     fRHW;
    DWORD     cDiffuse;
    DWORD     cSpecular;
    float     u0, v0;
    float     u1, v1;
    float     u2, v2;
    float     u3, v3;
} VERTEX_XYZRHW_DIFFUSE_SPECULAR_TEX4, *PVERTEX_XYZRHW_DIFFUSE_SPECULAR_TEX4;

//******************************************************************************
class CFVertex : public CScene {

protected:

    CTexture8*                  m_pd3dt;
    MESH                        m_mesh;
    PTLVERTEX                   m_prtlVertices;

    PVERTEX_XYZ_NORMAL                          m_pr1;
    PVERTEX_XYZ_NORMAL_TEX1                     m_pr2;
    PVERTEX_XYZ_NORMAL_DIFFUSE_SPECULAR         m_pr3;
    PVERTEX_XYZ_NORMAL_DIFFUSE_SPECULAR_TEX2    m_pr4;

    PVERTEX_XYZ_TEX1                            m_plr1;
    PVERTEX_XYZ_DIFFUSE                         m_plr2;
    PVERTEX_XYZ_SPECULAR                        m_plr3;
    PVERTEX_XYZ_DIFFUSE_SPECULAR                m_plr4;
    PVERTEX_XYZ_RESERVED1_DIFFUSE_TEX3          m_plr5;

    PVERTEX_XYZRHW_SPECULAR                     m_ptlr1;
    PVERTEX_XYZRHW_TEX4                         m_ptlr2;
    PVERTEX_XYZRHW_DIFFUSE_SPECULAR             m_ptlr3;
    PVERTEX_XYZRHW_DIFFUSE_SPECULAR_TEX4        m_ptlr4;

public:

                                    CFVertex();
                                    ~CFVertex();

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

#endif //__FVERTEX_H__
