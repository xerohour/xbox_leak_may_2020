/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    persist.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __PERSIST_H__
#define __PERSIST_H__

//******************************************************************************
class CPersistDisplay : public CScene {

protected:

    CTexture8*                      m_pd3dt;
    D3DPRESENT_PARAMETERS           m_d3dppReset;

public:

                                    CPersistDisplay();
                                    ~CPersistDisplay();

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

#endif //__PERSIST_H__
