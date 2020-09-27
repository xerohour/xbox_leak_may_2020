/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    palette0.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __PALETTE0_H__
#define __PALETTE0_H__

//******************************************************************************
class CMPalette : public CScene {

protected:

    VERTEX                          m_prVertices[4];
    CTexture8*                      m_pd3dt[4][4]; // [stage][format]
    CPalette8*                      m_pd3dp[4][4];
    D3DXMATRIX                      m_mScale;
    float                           m_fOffset;

public:

                                    CMPalette();
                                    ~CMPalette();

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

#endif //__PALETTE0_H__
