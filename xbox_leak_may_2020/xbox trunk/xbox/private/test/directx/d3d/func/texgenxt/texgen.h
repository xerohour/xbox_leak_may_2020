/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    texgen.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __TEXGENXT_H__
#define __TEXGENXT_H__

//******************************************************************************
class CTexGenExt : public CScene {

protected:

    CVERTEX                         m_rPoint;
    CAMERA                          m_cam;
    DWORD                           m_dwPShader;

    CCubeTexture8*                  m_pd3dtc;

public:

                                    CTexGenExt();
                                    ~CTexGenExt();

protected:

    virtual BOOL                    Prepare();
    virtual BOOL                    Initialize();
    virtual void                    Efface();
    virtual void                    Update();
    virtual BOOL                    Render();

    virtual CCubeTexture8*          CreateCubeTexture();

    virtual void                    ProcessInput();
    virtual BOOL                    InitView();

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);
};

#endif //__TEXGENXT_H__
