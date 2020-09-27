/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    tunnel.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __TUNNEL_H__
#define __TUNNEL_H__

//#define CRASH

//******************************************************************************
class CTunnel : public CScene {

protected:

    CTexture8*                      m_pd3dt;
#ifdef CRASH
    UINT                            m_uMaxFrames;
    CTexture8*                      m_pd3dtErrorBox;
    TLVERTEX                        m_ptlrErrorBox[4];
#endif // CRASH

public:

                                    CTunnel();
                                    ~CTunnel();

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

#endif //__TUNNEL_H__
