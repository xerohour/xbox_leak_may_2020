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

#define FVF_BUMPVERTEX             (D3DFVF_XYZ | D3DFVF_TEX4 | \
                                    D3DFVF_TEXCOORDSIZE2(0)  | \
                                    D3DFVF_TEXCOORDSIZE3(1)  | \
                                    D3DFVF_TEXCOORDSIZE3(2)  | \
                                    D3DFVF_TEXCOORDSIZE3(3))

//******************************************************************************
typedef struct _BUMPVERTEX {
    D3DXVECTOR3 vPosition;
    float       u0, v0;         // Base and bump (normal) texture coordinates
    D3DXVECTOR3 vTangent;
    D3DXVECTOR3 vBinormal;
    D3DXVECTOR3 vNormal;

    _BUMPVERTEX() {}
    _BUMPVERTEX(const D3DVECTOR& v, float _u0, float _v0) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        u0 = _u0; v0 = _v0;
    }
} BUMPVERTEX, *PBUMPVERTEX;

//******************************************************************************
class CBumpNormal1 : public CScene {

protected:

    CTexture8*                      m_pd3dtBase;
    CTexture8*                      m_pd3dtBump;
    CCubeTexture8*                  m_pd3dtcNormal;

    CVertexBuffer8*                 m_pd3drSphere;
    CIndexBuffer8*                  m_pd3diSphere;
    PVERTEX                         m_prSphere;
    DWORD                           m_dwSphereVertices;
    LPWORD                          m_pwSphere;
    DWORD                           m_dwSphereIndices;

    DWORD                           m_dwVShader;
    DWORD                           m_dwPShader;

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
