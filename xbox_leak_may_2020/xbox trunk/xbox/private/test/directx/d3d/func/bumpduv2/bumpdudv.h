/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    test.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __BUMPDUDV2_H__
#define __BUMPDUDV2_H__

#define FVF_BUMPVERTEX          (D3DFVF_XYZRHW | D3DFVF_TEX1)

//******************************************************************************
typedef struct _BUMPVERTEX {
    D3DXVECTOR3 vPosition;
    float       fRHW;
    float       u0, v0;

    _BUMPVERTEX() {}
    _BUMPVERTEX(const D3DVECTOR& v, float _fRHW, float _u0, float _v0) 
    { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z; fRHW = _fRHW;
        u0 = _u0; v0 = _v0;
    }
} BUMPVERTEX, *PBUMPVERTEX;

//******************************************************************************
class CBumpDuDv2 : public CScene {

protected:

    CVertexBuffer8*                 m_pd3drGrid;
    LPWORD                          m_pwGrid;
    UINT                            m_uGridIndices;

    CTexture8*                      m_pd3dtBase;
    CTexture8*                      m_pd3dtBump;

public:

                                    CBumpDuDv2();
                                    ~CBumpDuDv2();

protected:

    virtual BOOL                    Prepare();
    virtual BOOL                    Initialize();
    virtual void                    Efface();
    virtual void                    Update();
    virtual BOOL                    Render();

    virtual BOOL                    VerifyFrameBuffer();

    virtual void                    ProcessInput();
    virtual BOOL                    InitView();

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);
};

#endif //__BUMPDUDV2_H__
