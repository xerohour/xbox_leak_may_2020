/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    mstage.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __MSTAGE_H__
#define __MSTAGE_H__

//******************************************************************************
class CMStage4 : public CScene {

protected:

    VERTEX                          m_prVertices[4];
    CTexture8*                      m_pd3dt[8];
    CTexture8*                      m_pd3dtBackgrnd;

public:

                                    CMStage4();
                                    ~CMStage4();

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

#endif //__MSTAGE_H__
