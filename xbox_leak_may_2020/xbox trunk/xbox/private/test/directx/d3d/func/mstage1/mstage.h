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

// Flexible vertex format for a diffusely lit vertex with 2 texture coordinates
#define FVF_MVERTEX        (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2)

//******************************************************************************
// Structures
//******************************************************************************

//******************************************************************************
// Unlit vertex format with two texture coordinates (for multistage texturing)
typedef struct _MVERTEX {

    D3DXVECTOR3 vPosition;
    D3DXVECTOR3 vNormal;
    float       u0, v0;
    float       u1, v1;

    _MVERTEX() {};
    _MVERTEX(const D3DXVECTOR3& v, const D3DXVECTOR3& n, float _u0, float _v0,
             float _u1, float _v1);

} MVERTEX, *PMVERTEX;

//******************************************************************************
class CMStage1 : public CScene {

protected:

    MVERTEX                         m_prVertices[4];
    CTexture8*                      m_pd3dt1;
    CTexture8*                      m_pd3dt2;
    D3DXMATRIX                      m_mTexture0;
    D3DXMATRIX                      m_mTexture1;

public:

                                    CMStage1();
                                    ~CMStage1();

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
