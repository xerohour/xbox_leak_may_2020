/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    depthbuf.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __DEPTHBUF_H__
#define __DEPTHBUF_H__

#define NUM_WAFFLE_SLICES 20

//******************************************************************************
class CDepthBufferTest : public CScene {

protected:

    LVERTEX                         m_prBluePlane[4];
    LVERTEX                         m_prGreenWaffle[NUM_WAFFLE_SLICES*2];
    UINT                            m_uDepthBufferType;
    D3DFORMAT                       m_fmtd;
    BOOL                            m_bFloatDepth;
    BOOL                            m_bD16;
    D3DZBUFFERTYPE                  m_d3dzbt;
    TCHAR                           m_szBufferDesc[2048];
    float                           m_fAngle;

public:

                                    CDepthBufferTest();
                                    ~CDepthBufferTest();

protected:

    virtual BOOL                    Prepare();
    virtual BOOL                    Initialize();
    virtual void                    Efface();
    virtual void                    Update();
    virtual BOOL                    Render();

    virtual BOOL                    UpdateBufferFormat(BOOL bD16, BOOL bFloat);

    virtual void                    ProcessInput();
    virtual BOOL                    InitView();

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);
};

#endif //__DEPTHBUF_H__
