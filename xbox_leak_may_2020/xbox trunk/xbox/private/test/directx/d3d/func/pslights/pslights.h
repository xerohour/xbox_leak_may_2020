/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    pslights.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __PSLIGHTS_H__
#define __PSLIGHTS_H__

#define MAX_LIGHTS                  50

#define FVF_BUMPVERTEX             (D3DFVF_XYZ | D3DFVF_TEX4 | \
                                    D3DFVF_TEXCOORDSIZE2(0)  | \
                                    D3DFVF_TEXCOORDSIZE3(1)  | \
                                    D3DFVF_TEXCOORDSIZE3(2)  | \
                                    D3DFVF_TEXCOORDSIZE3(3))

//******************************************************************************
typedef struct _BUMPVERTEX {
    D3DXVECTOR3 vPosition;
    float       u, v;         // Base and bump (normal) texture coordinates
    D3DXVECTOR3 vTangent;
    D3DXVECTOR3 vBinormal;
    D3DXVECTOR3 vNormal;

    _BUMPVERTEX() {}
    _BUMPVERTEX(const D3DXVECTOR3& _vPosition, float _u, float _v) { 
        vPosition.x = _vPosition.x; vPosition.y = _vPosition.y; vPosition.z = _vPosition.z;
        u = _u; v = _v;
    }
} BUMPVERTEX, *PBUMPVERTEX;

//******************************************************************************
typedef struct _POINTLIGHT {
    D3DLIGHT8       light;
    D3DCOLOR        cDiffuse;
    D3DCOLOR        cAmbient;
    D3DCOLOR        cSpecular;
    D3DCOLOR        cEmissive;
    D3DXVECTOR3     vDirection;
    float           fDistance;
//    D3DXVECTOR3     vOffsetPolarity;
    CPerlinNoise1D  PNDirection[3];
    CPerlinNoise1D  PNDistance;
} POINTLIGHT, *PPOINTLIGHT;

//******************************************************************************
class CPSLights : public CScene {

protected:

    CTexture8*                      m_pd3dtBase;
    CTexture8*                      m_pd3dtBump;
    CCubeTexture8*                  m_pd3dtcNormal;
    CTexture8*                      m_pd3dtLight;

    CVertexBuffer8*                 m_pd3drSphere;
    LPWORD                          m_pwSphere;
    UINT                            m_uNumSphereIndices;

    DWORD                           m_dwVShader1;
    DWORD                           m_dwVShader2;
    DWORD                           m_dwPShader1;
    DWORD                           m_dwPShader2;

    POINTLIGHT                      m_Lights[MAX_LIGHTS];
    UINT                            m_uNumLights;

    D3DMATERIAL8                    m_material;
    D3DCOLORVALUE                   m_dcvAmbient;

    D3DXMATRIX                      m_mWorld;

    BOOL                            m_bCompleteLight;
    BOOL                            m_bInitialShowFPS;

    D3DXVECTOR3                     m_vSphereRot;
    static CPerlinNoise1D           PNSphereRotX;
    static CPerlinNoise1D           PNSphereRotY;
    static CPerlinNoise1D           PNSphereRotZ;

public:

                                    CPSLights();
                                    ~CPSLights();

protected:

    virtual BOOL                    Prepare();
    virtual BOOL                    Initialize();

    virtual void                    AddLight();

    virtual BOOL                    CreateShaders();
    virtual void                    ReleaseShaders();

    virtual BOOL                    CreateSphere(float fRadius, UINT uStepsU, UINT uStepsV);
    virtual void                    ReleaseSphere();

    virtual CTexture8*              CreateLightTexture(UINT uLength);

    virtual BOOL                    ComputeTangentTransforms(BUMPVERTEX* prVertices, LPWORD pwIndices, 
                                            UINT uNumIndices, BOOL bInterpolate);
    virtual BOOL                    CalculateTangentTerms(D3DXVECTOR3* pvTangent, D3DXVECTOR3* pvBinormal, 
                                            BUMPVERTEX* prVertices, LPWORD pwIndices, 
                                            UINT uNumIndices, BOOL bInterpolate);

    virtual void                    Efface();
    virtual void                    Update();
    virtual BOOL                    Render();

    virtual void                    ProcessInput();
    virtual BOOL                    InitView();

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);
};

#endif //__PSLIGHTS_H__
