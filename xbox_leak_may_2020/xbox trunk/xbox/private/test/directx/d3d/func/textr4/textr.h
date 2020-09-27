/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    textr.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __TEXTR_H__
#define __TEXTR_H__

//******************************************************************************
class CTextr4 : public CScene {

protected:

    VERTEX                          m_prVertices[4];
    CTexture8*                      m_pd3dt;
    D3DXMATRIX                      m_mScale;
    float                           m_fOffset;

public:

                                    CTextr4();
                                    ~CTextr4();

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

#endif //__TEXTR_H__
