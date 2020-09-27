/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    d3dsnow.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __D3DSNOW_H__
#define __D3DSNOW_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define NUM_FLAKES              50000
#define DEPTH_FRONT_PLANE       (float)(0.25f)
#define DEPTH_BACK_PLANE        (float)(15.0f)
#define CELLS_X                 10
#define CELLS_Y                 10
#define CELLS_Z                 10
#define OFFSET_X                (float)(0.004f)
#define OFFSET_Y                (float)(0.010f)

#define DEPTH_REFERENCE_PLANE   (float)(0.4f)
#define NUM_PLANES              7

//******************************************************************************
// Structures
//******************************************************************************

//******************************************************************************
typedef struct _SNOWCELL {
    float       x, y;
    UINT        uz;
    UINT        uMesh;
} SNOWCELL, *PSNOWCELL;

//******************************************************************************
typedef struct _SNOWSECTION {
    float       z;
    float       fMaxViewX,
                fMaxViewY;
    float       fLengthX,
                fLengthY;
} SNOWSECTION, *PSNOWSECTION;

//******************************************************************************
class CSnow : public CScene {

protected:

    CTexture8*                      m_pd3dtBackground;
    CTexture8*                      m_pd3dtSnowFlake;
    CTexture8*                      m_pd3dtSnowSheet;

    D3DXMATRIX                      m_mWorld;
    float                           m_fMaxU, m_fMaxV;
    PSNOWCELL                       m_pscList;
    UINT                            m_uNumCells;
    SNOWSECTION                     m_pssList[CELLS_Z];
    CVertexBuffer8**                m_ppd3drSnowMesh;
    D3DXVECTOR3**                   m_pprSnowMesh;
    UINT                            m_uNumSnowVertices;
    VERTEX                          m_prSnowSheet[NUM_PLANES][4];

    float                           m_fFlakeDim;

public:

                                    CSnow();
                                    ~CSnow();

    virtual BOOL                    Create(CDisplay* pDisplay);
    virtual int                     Exhibit(int *pnExitCode);

protected:

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

#endif //__D3DSNOW_H__
