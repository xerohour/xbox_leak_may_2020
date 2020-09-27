/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    test.h

Description:

    Test functions.

*******************************************************************************/

#ifndef __TEST_H__
#define __TEST_H__

#define FVF_VERTEX             (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

//******************************************************************************
typedef struct _VERTEX {
    D3DXVECTOR3 vPosition;
    D3DXVECTOR3 vNormal;
    float       u0, v0;

    _VERTEX() { }
    _VERTEX(const D3DVECTOR& v, const D3DVECTOR& normal, float _u0, float _v0) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        vNormal = normal; u0 = _u0; v0 = _v0;
    }
} VERTEX, *PVERTEX;

//******************************************************************************
class CTest {

protected:

    HINSTANCE                       m_hInstance;
    HWND                            m_hWnd;

    LPDIRECT3D8                     m_pd3d;
    LPDIRECT3DDEVICE8               m_pDevice;

    LPDIRECT3DVERTEXBUFFER8         m_pd3dr;

public:

                                    CTest();
                                    ~CTest();

    virtual BOOL                    Create();
    virtual BOOL                    Run();
};

//******************************************************************************
void DebugString(LPCTSTR szFormat, ...);
BOOL ResultFailed(HRESULT hr, LPCTSTR sz);

#endif //__TEST_H__
