/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    innrloop.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __INNRLOOP_H__
#define __INNRLOOP_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define VIEW_ROTATION_DELTA     0.031415f
#define VIEW_TRANSLATION_DELTA  0.5f

#define LIGHT_POS               D3DXVECTOR3(10.0f, 10.0f, -10.0f)
#define LIGHT_DIR               D3DXVECTOR3(-1.0f, -1.0f, 1.0f)

#define FILLMODE                0
#define CLIPPING                1
#define TEXTURING               2
#define WRAPPING                3
#define LIGHTING                4
#define VERTEXFOG               5
#define SPECULAR                6
#define MAX_PARAM               6

#define GETSTATE(istate, param) (((0x7 << ((MAX_PARAM - param) * 3)) & istate) \
                                                 >> ((MAX_PARAM - param) * 3))

#define FVF_VT_VERTEX   (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX4)
#define FVF_VT_LVERTEX  (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX4)
#define FVF_VT_TLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX4)

typedef struct _VT_VERTEX {
    D3DVECTOR       vPosition;
    D3DVECTOR       vNormal;
    DWORD           dwDiffuse;
    DWORD           dwSpecular;
    float           fTexCoord[4][2];
} VT_VERTEX, *PVT_VERTEX;

typedef struct _VT_LVERTEX {
    D3DVECTOR       vPosition;
    DWORD           dwDiffuse;
    DWORD           dwSpecular;
    float           fTexCoord[4][2];
} VT_LVERTEX, *PVT_LVERTEX;

typedef struct _VT_TLVERTEX {
    D3DVECTOR       vPosition;
    float           fRHW;
    DWORD           dwDiffuse;
    DWORD           dwSpecular;
    float           fTexCoord[4][2];
} VT_TLVERTEX, *PVT_TLVERTEX;

//******************************************************************************
class CSphereForms {

private:

    PVT_VERTEX                  m_prGeneral,      // PNIL, PIL, LIL, LIS, LNIS, TIL, TIS, TIF, TNIF
                                m_prLineList,     // LNIL
                                m_prTriList,      // TNIL
                                m_prTriStrip;     // TNIS
    UINT                        m_urGeneral,
                                m_urLineList,
                                m_urTriList,
                                m_urTriStrip;

    LPWORD                      m_pwGeneral,      // PIL, TIF, LIS
                                m_pwLineList,     // LIL
                                m_pwTriList,      // TIL
                                m_pwTriStrip;     // TIS
    UINT                        m_uwGeneral,
                                m_uwLineList,
                                m_uwTriList,
                                m_uwTriStrip;

public:

                                CSphereForms();
                                ~CSphereForms();
    BOOL                        Create(UINT uSphereVertices);

    HRESULT                     Render(CDevice8* pDevice,
                                       D3DPRIMITIVETYPE d3dpt,
                                       DWORD dwVertexType,
                                       BOOL bIndexed = FALSE,
                                       BOOL bStrided = FALSE, 
                                       DWORD dwFlags = 0);
};

//******************************************************************************
class CInnerLoop : public CScene {

protected:

    CSphereForms                    m_sf;
    CTexture8*                      m_pd3dt[4];
    int                             m_nParam[MAX_PARAM+1];
    int                             m_nInitial[MAX_PARAM+1];
    DWORD                           m_dwInitialState;
    DWORD                           m_dwSphere;
    int                             m_nOffset;
    BOOL                            m_bWait;

public:

                                    CInnerLoop();
                                    ~CInnerLoop();

    virtual BOOL                    Create(CDisplay* pDisplay, DWORD dwInitialState, DWORD dwSphere);
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

#endif //__INNRLOOP_H__
