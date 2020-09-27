/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    mstage.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __MSTAGE_H__
#define __MSTAGE_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

// Flexible vertex format for a diffusely lit vertex with 4 texture coordinates
#define FVF_MVERTEX        (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX4)

//******************************************************************************
// Structures
//******************************************************************************

//******************************************************************************
// Unlit vertex format with two texture coordinates (for multistage texturing)
typedef struct _MVERTEX {

    D3DXVECTOR3 vPosition;
    D3DXVECTOR3 vNormal;
    float       u0, v0;
    float     u1, v1;
    float     u2, v2;
    float     u3, v3;

    _MVERTEX() {};
    _MVERTEX(const D3DXVECTOR3& v, const D3DXVECTOR3& n, float _u0, float _v0,
             float _u1, float _v1, float _u2, float _v2,
             float _u3, float _v3);

} MVERTEX, *PMVERTEX;

//******************************************************************************
class CMStage2 : public CScene {

protected:

    PMVERTEX                        m_prVertices;
    DWORD                           m_dwNumVertices;
    CTexture8*                      m_pd3dt[8];
    CTexture8*                      m_pd3dtBack;
    D3DXMATRIX                      m_mTexture[4];

public:

                                    CMStage2();
                                    ~CMStage2();

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

#endif //__MSTAGE_H__
