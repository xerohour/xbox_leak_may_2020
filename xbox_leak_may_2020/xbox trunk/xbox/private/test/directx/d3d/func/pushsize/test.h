/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    test.h

Description:

    Test functions.

*******************************************************************************/

#ifndef __TEST_H__
#define __TEST_H__

#define FVF_LVERTEX             (D3DFVF_XYZ | D3DFVF_DIFFUSE | \
                                 D3DFVF_SPECULAR | D3DFVF_TEX1)

//******************************************************************************
typedef struct _LVERTEX {
    XGVECTOR3   vPosition;
    D3DCOLOR    cDiffuse;
    D3DCOLOR    cSpecular;
    float       u0, v0;

    _LVERTEX() { }
    _LVERTEX(const D3DVECTOR& v, D3DCOLOR _cDiffuse, D3DCOLOR _cSpecular, float _u0, float _v0) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        cDiffuse = _cDiffuse; cSpecular = _cSpecular;
        u0 = _u0; v0 = _v0;
    }
} LVERTEX, *PLVERTEX;

//******************************************************************************
class CTest {

protected:

    LPDIRECT3D8                     m_pd3d;
    LPDIRECT3DDEVICE8               m_pDevice;

    LPDIRECT3DVERTEXBUFFER8         m_pd3dr;

public:

                                    CTest();
                                    ~CTest();

    virtual BOOL                    Create();
    virtual BOOL                    CreateScene();
    virtual void                    ReleaseScene();
    virtual BOOL                    DisplayScene();
    virtual BOOL                    Run();
};

//******************************************************************************
void DebugString(LPCTSTR szFormat, ...);
BOOL ResultFailed(HRESULT hr, LPCTSTR sz);

#endif //__TEST_H__
