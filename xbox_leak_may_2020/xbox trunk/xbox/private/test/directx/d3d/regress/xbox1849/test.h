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

    VERTEX                          m_prQuad[4];
    LVERTEX                         m_plrQuad[4];

    WORD                            m_pwIndices[6];

    CTexture8*                      m_pd3dt;

public:

                                    CTest();
                                    ~CTest();

protected:

    virtual BOOL                    Prepare();
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
