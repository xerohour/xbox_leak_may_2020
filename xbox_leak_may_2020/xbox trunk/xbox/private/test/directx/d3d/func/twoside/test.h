/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    test.h

Description:

    Test functions.

*******************************************************************************/

#ifndef __TEST_H__
#define __TEST_H__

//******************************************************************************
typedef struct _LVERTEX {
    XGVECTOR3   vPosition;
    D3DCOLOR    cDiffuseF;
    D3DCOLOR    cSpecularF;
    D3DCOLOR    cDiffuseB;
    D3DCOLOR    cSpecularB;

    _LVERTEX() { }
    _LVERTEX(const D3DVECTOR& v, D3DCOLOR _cDiffuseF, D3DCOLOR _cSpecularF, D3DCOLOR _cDiffuseB, D3DCOLOR _cSpecularB) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        cDiffuseF = _cDiffuseF; cSpecularF = _cSpecularF;
        cDiffuseB = _cDiffuseB; cSpecularB = _cSpecularB;
    }
} LVERTEX, *PLVERTEX;

//******************************************************************************
class CTest {

protected:

    LPDIRECT3D8                     m_pd3d;
    LPDIRECT3DDEVICE8               m_pDevice;

    LPDIRECT3DVERTEXBUFFER8         m_pd3dr;

    DWORD                           m_dwShader;

    float                           m_fTheta;

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
