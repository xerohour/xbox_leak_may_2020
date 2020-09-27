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

#define FVF_TLNODIFFUSE             (D3DFVF_XYZRHW | D3DFVF_SPECULAR)
#define FVF_TLNOSPECULAR            (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

//******************************************************************************
typedef struct _TLVNODIFFUSE {
    D3DXVECTOR3 vPosition;
    float       fRHW;
    D3DCOLOR    cSpecular;

    _TLVNODIFFUSE() {}
    _TLVNODIFFUSE(const D3DVECTOR& v, float _fRHW, D3DCOLOR _cSpecular) 
    { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z; fRHW = _fRHW;
        cSpecular = _cSpecular;
    }
} TLVNODIFFUSE, *PTLVNODIFFUSE;

//******************************************************************************
typedef struct _TLVNOSPECULAR {
    D3DXVECTOR3 vPosition;
    float       fRHW;
    D3DCOLOR    cDiffuse;

    _TLVNOSPECULAR() {}
    _TLVNOSPECULAR(const D3DVECTOR& v, float _fRHW, D3DCOLOR _cDiffuse) 
    { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z; fRHW = _fRHW;
        cDiffuse = _cDiffuse;
    }
} TLVNOSPECULAR, *PTLVNOSPECULAR;

//******************************************************************************
class CTest : public CScene {

protected:

    TLVNODIFFUSE                    m_prNoDiffuse[4];
    TLVNOSPECULAR                   m_prNoSpecular[4];

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
