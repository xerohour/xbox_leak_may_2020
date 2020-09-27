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
class CTest : public CScene {

protected:

    PVERTEX                         m_prSphere;
    LPWORD                          m_pwSphere;
    DWORD                           m_dwSphereVertices;
    DWORD                           m_dwSphereIndices;

    BOOL                            m_bSpecular;

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
