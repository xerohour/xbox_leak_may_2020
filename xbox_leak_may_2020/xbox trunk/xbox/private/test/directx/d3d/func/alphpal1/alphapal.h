/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    alphapal.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __ALPHAPAL_H__
#define __ALPHAPAL_H__

#define MAX_DECAY   50

//******************************************************************************
typedef struct _APTEXTURE {
    CTexture8*           pd3dt;
    D3DTEXTUREFILTERTYPE d3dtft;
    UINT                 uDecay;
} APTEXTURE, *PAPTEXTURE;

//******************************************************************************
class CAlphaPalette1 : public CScene {

protected:

    VERTEX                          m_prVertices[4];
    CTexture8*                      m_pd3dtBackground;
    APTEXTURE                       m_patx[64];

public:

                                    CAlphaPalette1();
                                    ~CAlphaPalette1();

    virtual BOOL                    Create(CDisplay* pDisplay);
    virtual int                     Exhibit(int *pnExitCode);

protected:

    BOOL                            InitTexture(PAPTEXTURE patx, UINT i);

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

#endif //__ALPHAPAL_H__
