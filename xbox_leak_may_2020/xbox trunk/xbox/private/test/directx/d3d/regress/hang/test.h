/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    test.h

Description:

    Test functions.

*******************************************************************************/

#ifndef __TEST_H__
#define __TEST_H__

#define FVF_MVERTEX        (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2)

//******************************************************************************
// Structures
//******************************************************************************

//******************************************************************************
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
class CTest {

protected:

    LPDIRECT3D8                     m_pd3d;
    LPDIRECT3DDEVICE8               m_pDevice;

    LPDIRECT3DVERTEXBUFFER8         m_pd3dr;

    D3DPRESENT_PARAMETERS           m_d3dpp;

    D3DLIGHT8                       m_light;
    D3DMATERIAL8                    m_material;

    MVERTEX                         m_prVertices[4];
    LPDIRECT3DTEXTURE8              m_pd3dt1;
    LPDIRECT3DTEXTURE8              m_pd3dt2;
    D3DXMATRIX                      m_mTexture0;
    D3DXMATRIX                      m_mTexture1;

public:

                                    CTest();
                                    ~CTest();

    virtual BOOL                    Create();
    virtual BOOL                    StartGraphics();
    virtual void                    StopGraphics();
    virtual BOOL                    Run();
};

//******************************************************************************
void DebugString(LPCTSTR szFormat, ...);
BOOL ResultFailed(HRESULT hr, LPCTSTR sz);

#endif //__TEST_H__
