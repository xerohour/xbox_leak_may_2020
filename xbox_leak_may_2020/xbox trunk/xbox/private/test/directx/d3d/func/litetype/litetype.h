/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    litetype.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __LITETYPE_H__
#define __LITETYPE_H__

#define FVF_CLVERTEX                    (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)

#define LIGHT_RANGE                     10.0f

#define SCREEN_DIM                      30           // Must be multiple of 2

#define STRESS_CLUSTERS                 7
#define STRESS_MAX_LIGHTS_PER_CLUSTER   8
#define MIN_DECAY                       15
#define MAX_DECAY                       30

#define MAX_LIGHTS_PER_CLUSTER          16

//******************************************************************************
typedef struct _CLVERTEX {
    D3DXVECTOR3 vPosition;
    D3DXVECTOR3 vNormal;
    DWORD       dwDiffuse;
    DWORD       dwSpecular;
    float       tu, tv;

    _CLVERTEX() {};
    _CLVERTEX(const D3DVECTOR& v, const D3DVECTOR& n, DWORD _dwDiffuse, DWORD _dwSpecular, float _tu, float _tv) {
        vPosition = v; vNormal = n; dwDiffuse = _dwDiffuse; dwSpecular = _dwSpecular; tu = _tu, tv = _tv;};
} CLVERTEX, *PCLVERTEX;

//******************************************************************************
typedef struct _LIGHTCLUSTER {
    D3DLIGHT8*       plight;
    UINT             uNumLights;
    bool             bAmbient;
    UINT             uDecay;
    BOOL             bSpecular;
} LIGHTCLUSTER, *PLIGHTCLUSTER;

//******************************************************************************
class CLightType : public CScene {

protected:

    float                           m_fScreenDepth;
    float                           m_fDepthOffset;
    PLIGHTCLUSTER                   m_plc;
    UINT                            m_uNumClusters;
    D3DCOLOR                        m_cAmbient;
    UINT                            m_uAttenuation;
    bool                            m_bTexture;
    bool                            m_bColored;
    BOOL                            m_bStress;
    BOOL                            m_bLighting;
    BOOL                            m_bSpecular;
    BOOL                            m_bColorVertex;

    CTexture8*                      m_pd3dt;

    PCLVERTEX                       m_prScreenXY, m_prScreenYZ;
    LPWORD                          m_pwScreenXY, m_pwScreenYZ;
    UINT                            m_uNumVerticesXY, m_uNumVerticesYZ,
                                    m_uNumIndicesXY, m_uNumIndicesYZ;

public:

                                    CLightType();
                                    ~CLightType();

    virtual BOOL                    Create(CDisplay* pDisplay);
    virtual int                     Exhibit(int *pnExitCode);

protected:

    virtual BOOL                    Prepare();
    virtual BOOL                    Setup();
    virtual BOOL                    Initialize();
    virtual void                    Efface();
    virtual void                    Update();
    virtual BOOL                    Render();

    virtual void                    UpdateAttenuation(UINT uAttenuation);
    virtual void                    UpdateLightColor(bool bColored);

    virtual void                    ProcessInput();
    virtual BOOL                    InitView();

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);
};

#endif //__LITETYPE_H__
