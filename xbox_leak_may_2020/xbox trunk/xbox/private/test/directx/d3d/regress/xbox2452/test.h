/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    test.h

Author:

    Matt Bronder

Description:

    Direct3D repro test functions.

*******************************************************************************/

#ifndef __TEST_H__
#define __TEST_H__

//******************************************************************************
class CReproTest {

protected:

    HINSTANCE                       m_hInstance;
    HWND                            m_hWnd;

    LPDIRECT3D8                     m_pd3d;
    LPDIRECT3DDEVICE8               m_pDevice;

    LPDIRECT3DVERTEXBUFFER8         m_pd3dr;

    LPDIRECT3DTEXTURE8              m_pd3dt;

public:

                                    CReproTest();
                                    ~CReproTest();

    virtual BOOL                    Create(HINSTANCE hInstance);
    virtual BOOL                    Run();

protected:

    virtual BOOL                    InitDeviceState();
    virtual BOOL                    SetViewport(DWORD dwWidth, DWORD dwHeight);
};

//******************************************************************************
LRESULT CALLBACK    WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void                InitMatrix(D3DMATRIX* pd3dm,
                        float _11, float _12, float _13, float _14,
                        float _21, float _22, float _23, float _24,
                        float _31, float _32, float _33, float _34,
                        float _41, float _42, float _43, float _44);

void                SetView(LPD3DXMATRIX pd3dm, LPD3DXVECTOR3 pvPos, 
                        LPD3DXVECTOR3 pvAt, LPD3DXVECTOR3 pvUp);

void                SetPerspectiveProjection(D3DMATRIX* pd3dm, float fFront, 
                        float fBack, float fFieldOfView, float fAspect);

#endif //__TEST_H__
