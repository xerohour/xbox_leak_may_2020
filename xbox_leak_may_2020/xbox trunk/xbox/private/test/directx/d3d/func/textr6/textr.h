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

#define FVF_EBMVERTEX              (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2)

//******************************************************************************
typedef struct _EBMVERTEX {
    D3DXVECTOR3 vPosition;
    D3DXVECTOR3 vNormal;
    float       u0, v0;
    float       u1, v1;

    _EBMVERTEX() {}
    _EBMVERTEX(const D3DVECTOR& v, const D3DVECTOR& n, float _u0, float _v0, float _u1, float _v1) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        vNormal.x = n.x; vNormal.y = n.y; vNormal.z = n.z;
        u0 = _u0; v0 = _v0; u1 = _u1; v1 = _v1;
    }
} EBMVERTEX, *PEBMVERTEX;

//******************************************************************************
class CTextr6 : public CScene {

protected:

    EBMVERTEX                       m_prVertices[4];

public:

                                    CTextr6();
                                    ~CTextr6();

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
