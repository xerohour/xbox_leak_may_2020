/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    textr.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __TEXTR_H__
#define __TEXTR_H__

#define TEX_DIM_X                  11
#define TEX_DIM_Y                  11

//******************************************************************************
// Structures
//******************************************************************************

// Flexible vertex format transformed vertex with 1 texture coordinate
#define FVF_TVERTEX2       (D3DFVF_XYZRHW | D3DFVF_TEX1)

//******************************************************************************
// Structures
//******************************************************************************

//******************************************************************************
// Unlit vertex format with two texture coordinates (for multistage texturing)
typedef struct _TVERTEX2 {

    D3DXVECTOR3 vPosition;
    float       fRHW;
    float       tu, tv;

    _TVERTEX2() {};
    _TVERTEX2(const D3DXVECTOR3& v, float _tu, float _tv);

} TVERTEX2, *PTVERTEX2;

//******************************************************************************
typedef struct _TEXQUAD {
    TVERTEX2            pr[4];
    CTexture8*          pd3dt;
} TEXQUAD, *PTEXQUAD;

//******************************************************************************
class CTextr2 : public CScene {

protected:

    PTEXQUAD                        m_ptq;

public:

                                    CTextr2();
                                    ~CTextr2();

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

#endif //__TEXTR_H__
