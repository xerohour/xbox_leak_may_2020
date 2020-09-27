/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    stencil.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __STENCIL_H__
#define __STENCIL_H__

//******************************************************************************
class CStencil0 : public CScene {

protected:

    CTexture8*                      m_pd3dt;
    CVertexBuffer8*                 m_pd3dr;
    VERTEX                          m_prQuad[4];
    D3DXMATRIX                      m_mWorld, m_mRotation, m_mShadow;
    D3DMATERIAL8                    m_matObject;
    D3DMATERIAL8                    m_matShadow;
    D3DXVECTOR4                     m_vLightPos;

public:

                                    CStencil0();
                                    ~CStencil0();

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

#endif //__STENCIL_H__
