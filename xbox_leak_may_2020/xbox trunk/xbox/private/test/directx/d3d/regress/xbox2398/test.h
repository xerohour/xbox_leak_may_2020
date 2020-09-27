/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    test.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __TEST_H__
#define __TEST_H__

#define FVF_VERTEXDP3   (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE3(0) | D3DFVF_TEXCOORDSIZE3(1) | D3DFVF_TEXCOORDSIZE3(2) | D3DFVF_TEXCOORDSIZE3(3))

//******************************************************************************
typedef struct _VERTEXDP3 {
    D3DXVECTOR3   vPosition;
    D3DXVECTOR3   vNormal;
    float         u0, v0, t0;
    MATRIX3X3     mTransform;

    _VERTEXDP3() { }
    _VERTEXDP3(const D3DVECTOR& v, const D3DVECTOR& n, float _u0, float _v0, float _t0) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        vNormal.x = n.x; vNormal.y = n.y; vNormal.z = n.z;
        u0 = _u0; v0 = _v0; t0 = _t0;
    }
} VERTEXDP3, *PVERTEXDP3;

//******************************************************************************
class CTest : public CScene {

protected:

    CCubeTexture8*                  m_pd3dt1;
    CCubeTexture8*                  m_pd3dt2;
    VERTEXDP3                       m_prQuad[4];
    DWORD                           m_dwPShader;

public:

                                    CTest();
                                    ~CTest();

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

#endif //__TEST_H__
