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

//******************************************************************************
// Function prototypes
//******************************************************************************

int         ExhibitScene(CDisplay* pDisplay);
BOOL        ValidateDisplay(CDirect3D8* pd3d, D3DCAPS8* pd3dcaps, 
                            D3DDISPLAYMODE* pd3ddm);

//******************************************************************************
class CTest : public CScene {

protected:

    CTexture8*                      m_pd3dtPawn;

    REVOLUTION                      m_rvPawn;

    D3DXMATRIX                      m_mWorld;
    float                           m_fScale;

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
