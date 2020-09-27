/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    skulls.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __SKULLS_H__
#define __SKULLS_H__

//******************************************************************************
class CSkulls : public CScene {

protected:

    BOOL                            m_bAdvanceView;

    CVertexBuffer8*                 m_pd3dr;
    CIndexBuffer8*                  m_pd3di;
    MESH                            m_mesh;
    PLVERTEX                        m_plr;
    D3DXMATRIX*                     m_pmView;
    D3DMATRIX                       m_mWorld[3];
    UINT                            m_uSteps;
    LVERTEX                         m_plrScreen[3][4];
    CVertexBuffer8*                 m_pd3drScreen;
    UINT                            m_j;

public:

                                    CSkulls();
                                    ~CSkulls();

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

#endif //__SKULLS_H__
