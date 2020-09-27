/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    rteapot.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __RTEAPOT_H__
#define __RTEAPOT_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define NUM_TEAPOTS                1

//******************************************************************************
class CTeapot : public CScene {

protected:

    CVertexBuffer8*                 m_pd3dr;
    CIndexBuffer8*                  m_pd3di;
    CVertexBuffer8*                 m_pd3drBack;
    CTexture8*                      m_pd3dt;
    D3DXMATRIX                      m_pmWorld[NUM_TEAPOTS];
    D3DXMATRIX                      m_pmRotation[NUM_TEAPOTS];
    MESH                            m_mesh;

public:

                                    CTeapot();
                                    ~CTeapot();

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

#endif //__RTEAPOT_H__
