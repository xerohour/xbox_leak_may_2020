/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    fogmode.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __FOGMODE_H__
#define __FOGMODE_H__

//******************************************************************************
class CFogMode : public CScene {

protected:

    BOOL                            m_bFog;
    UINT                            m_uMode;
    BOOL                            m_bTable;
    D3DXMATRIX                      m_mWorld;
    CTexture8*                      m_pd3dtSphere;
    CTexture8*                      m_pd3dtPlane;
    PVERTEX                         m_prSphere;
    LPWORD                          m_pwSphere;
    DWORD                           m_dwSphereVertices;
    DWORD                           m_dwSphereIndices;
    VERTEX                          m_prPlane[4];

public:

                                    CFogMode();
                                    ~CFogMode();

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

#endif //__FOGMODE_H__
