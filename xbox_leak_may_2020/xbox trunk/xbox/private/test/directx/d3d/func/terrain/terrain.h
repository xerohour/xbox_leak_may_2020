/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    terrain.h

Author:


Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#define NUM_GRID			96
#define NUM_GRID_VERTICES	(NUM_GRID*NUM_GRID)
#define NUM_GRID_INDICES	((NUM_GRID-1)*(NUM_GRID-1)*2*3)
#define GRID_WIDTH			80.0

//******************************************************************************
class CTerrain : public CScene {

protected:

    CTexture8*                      m_pd3dt;
    CAMERA                          m_cam;

    CVertexBuffer8*                 m_pd3dr;

public:

                                    CTerrain();
                                    ~CTerrain();

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

#endif //__TERRAIN_H__
