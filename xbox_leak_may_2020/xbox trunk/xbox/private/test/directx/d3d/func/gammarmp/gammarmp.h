/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    gammarmp.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __TEST_H__
#define __TEST_H__

typedef struct _FGAMMARAMP {
    float red[256];
    float green[256];
    float blue[256];
} FGAMMARAMP, *PFGAMMARAMP;

//******************************************************************************
class CGammaRamp : public CScene {

protected:

    static CPerlinNoise2D           m_NoiseR;
    static CPerlinNoise2D           m_NoiseG;
    static CPerlinNoise2D           m_NoiseB;
    CTexture8*                      m_pd3dt;
    FGAMMARAMP                      m_fgr;
    D3DGAMMARAMP                    m_d3dgr;

public:

                                    CGammaRamp();
                                    ~CGammaRamp();

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
