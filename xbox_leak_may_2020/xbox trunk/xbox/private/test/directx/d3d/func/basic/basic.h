/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    basic.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __BASIC_H__
#define __BASIC_H__

//******************************************************************************
class CBasicTest : public CScene {

protected:

    CTexture8*                      m_pd3dt;

    CVertexBuffer8*                 m_pd3drVertices;
    VERTEX                          m_prVertices[4];
    UINT                            m_uNumVertices;
    D3DXMATRIX                      m_mWorld;

public:

                                    CBasicTest();
                                    ~CBasicTest();

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

#endif //__BASIC_H__
