/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    misorbit.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __MISORBIT_H__
#define __MISORBIT_H__

//******************************************************************************
// Structures
//******************************************************************************

typedef struct _SHAPE {
    REVOLUTION          rv;
    CTexture8*          pd3dt;
    D3DXMATRIX          mWorld;
} SHAPE, *PSHAPE;

//******************************************************************************
class CMisorbit : public CScene {

protected:

    SHAPE                           m_shSphere, m_shCylinder, m_shCone;
    PSHAPE                          m_psh[3];
    D3DXMATRIX                      m_mRotation;
    float                           m_fAngleOfRotation;
    BOOL                            m_bSwitch;

#ifdef UNDER_XBOX
    TCHAR                           m_szCPUUsage[32];
    float                           m_fInvCPUUsage;
    TLVERTEX                        m_prUsageBar[4];
#endif // UNDER_XBOX

public:

                                    CMisorbit();
                                    ~CMisorbit();

    virtual BOOL                    Create(CDisplay* pDisplay);
    virtual int                     Exhibit(int *pnExitCode);

protected:

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

#endif //__MISORBIT_H__
