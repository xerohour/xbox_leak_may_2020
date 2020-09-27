/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    matsrc.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __MATSRC_H__
#define __MATSRC_H__

#define FVF_MSVERTEX                (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)

//******************************************************************************
typedef struct _MSVERTEX {
    D3DXVECTOR3 vPosition;
    D3DXVECTOR3 vNormal;
    D3DCOLOR    cDiffuse;
    D3DCOLOR    cSpecular;
    float       u0, v0;

    _MSVERTEX() {}
    _MSVERTEX(const D3DVECTOR& v, const D3DVECTOR& n, float _u0, float _v0) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        vNormal.x = n.x; vNormal.y = n.y; vNormal.z = n.z;
        cDiffuse = 0; cSpecular = 0;
        u0 = _u0; v0 = _v0;
    }
} MSVERTEX, *PMSVERTEX;

//******************************************************************************
class CMaterialSource : public CScene {

protected:

    MSVERTEX                        m_prQuad[4];
    D3DLIGHT8                       m_light;
    D3DMATERIAL8                    m_material;
    D3DLIGHT8                       m_lightWhite;
    D3DMATERIAL8                    m_materialWhite;
    D3DMATERIAL8                    m_materialBlack;
    D3DCOLOR                        m_cColor;
    D3DCOLORVALUE                   m_dcvColor;
    D3DCOLOR                        m_cWhite;
    D3DCOLORVALUE                   m_dcvWhite;
    DWORD                           m_vsh2SidedLighting;

public:

                                    CMaterialSource();
                                    ~CMaterialSource();

protected:

    virtual BOOL                    Prepare();
    virtual BOOL                    Initialize();
    virtual void                    Efface();
    virtual void                    Update();
    virtual BOOL                    Render();

    virtual BOOL                    Verify();

    virtual void                    ProcessInput();
    virtual BOOL                    InitView();

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);
};

#endif //__MATSRC_H__
