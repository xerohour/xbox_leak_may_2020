/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    hightide.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __HIGHTIDE_H__
#define __HIGHTIDE_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define MAX_LIGHT_ELEVATION     0
#define MIN_LIGHT_ELEVATION     0
#define LIGHT_ELEVATION_DELTA   (M_PI / 200.0f)
#define LIGHT_ROTATION_DELTA1   (M_PI / 200.0f)
#define LIGHT_ROTATION_DELTA2   (M_PI / 100.0f)

#define LIGHT_CONE_ANGLE    1.121997376282f

#define CLIFF_GRID_WIDTH    30.0f
#define CLIFF_GRID_HEIGHT   3.0f
#define CLIFF_GRID_OFFSET   0.2f
#define CLIFF_GRID_DIM_X    10
#define CLIFF_GRID_DIM_Y    10

#define BEACH_GRID_WIDTH    24.0f
#define BEACH_GRID_HEIGHT   10.0f
#define BEACH_GRID_DEPTH    1.0f
#define BEACH_GRID_OFFSET   0.05f
#define BEACH_GRID_DIM_X    80
#define BEACH_GRID_DIM_Y    40

#define WAVE_GRID_WIDTH     16.0f
#define WAVE_GRID_HEIGHT    12.0f
// WAVE_GRID_DIM's must be greater than 50 unless the sqrt lookup table's range 
// is expanded
#define WAVE_GRID_DIM_X     100
#define WAVE_GRID_DIM_Y     100
#define WAVE_UNIT_LENGTH_X  0.16f
#define WAVE_UNIT_LENGTH_Y  0.12f
#define NUM_WAVE_SOURCES    1

#define SQRT_TABLE_SIZE     1024

#define FVF_HHVERTEX        (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

//******************************************************************************
// Structures
//******************************************************************************

//******************************************************************************
typedef struct _HHVERTEX {
    float x;
    float y;
    float z;
    float nx;
    float ny;
    float nz;
    float tu;
    float tv;

    _HHVERTEX() {}
    _HHVERTEX(const D3DVECTOR& v, const D3DVECTOR& n, float _u0, float _v0) { 
        x = v.x; y = v.y; z = v.z;
        nx = n.x; ny = n.y; nz = n.z;
        tu = _u0; tv = _v0;
    }
} HHVERTEX, *PHHVERTEX;

//******************************************************************************
typedef struct _WAVESOURCE {
    D3DXVECTOR3 vPos;
    float       fWavelength;
    float       fFrequency;     // Phase shift in radians per render
    float       fAmplitude;
    float       fPhase;
} WAVESOURCE, *PWAVESOURCE;

//******************************************************************************
typedef struct _LIGHTHOUSE {
    D3DXMATRIX          mWorld;
    REVOLUTION          rvBase;
    REVOLUTION          rvTrunk;
    REVOLUTION          rvBalcony;
    REVOLUTION          rvPortal;
    REVOLUTION          rvRoof;
    PLVERTEX            prPortal;
    D3DMATERIAL8        material;
    CTexture8*          pd3dtTrunk;
    CTexture8*          pd3dtRoof;
} LIGHTHOUSE, *PLIGHTHOUSE;

//******************************************************************************
class CHighTide : public CScene {

protected:

    float                           m_fLightElevation;
    float                           m_fLightRotation;
    BOOL                            m_bRotateLight;
    float                           m_fRotationDelta;
    float                           m_fElevationDelta;
    D3DXMATRIX                      m_mConeRotation, m_mConePosition, 
                                    m_mLightDirection, m_mLightCone;
    D3DLIGHT8                       m_light;
    D3DMATERIAL8                    m_matLightCone;
    D3DMATERIAL8                    m_matWave;
    D3DMATERIAL8                    m_matWhite;
    CTexture8*                      m_pd3dtBeach;
    CTexture8*                      m_pd3dtBackground;
    LIGHTHOUSE                      m_lhLighthouse;
    REVOLUTION                      m_rvLightCone;
    PHHVERTEX                       m_prCliff;
    UINT                            m_uNumCliffVertices;
    LPWORD                          m_pwCliff;
    UINT                            m_uNumCliffIndices;
    PHHVERTEX                       m_prBeach;
    UINT                            m_uNumBeachVertices;
    LPWORD                          m_pwBeach;
    UINT                            m_uNumBeachIndices;
    PWAVESOURCE                     m_pws;
//    PHHVERTEX                       m_prWave;
    CVertexBuffer8*                 m_pd3drWave;
    UINT                            m_uNumWaveVertices;
    LPWORD                          m_pwWave;
    UINT                            m_uNumWaveIndices;
    float*                          m_pfBaseAngle;
    float                           m_fMaxIntensity;
    float                           m_pfSinLookup[1024];
    float                           m_pfSqrtLookup[SQRT_TABLE_SIZE];

public:

                                    CHighTide();
                                    ~CHighTide();

    virtual BOOL                    Create(CDisplay* pDisplay);
    virtual int                     Exhibit(int *pnExitCode);

protected:

    virtual BOOL                    Setup();
    virtual BOOL                    Initialize();
    virtual void                    Efface();
    virtual void                    Update();
    virtual BOOL                    Render();

//    BOOL                            CreateLighthouse(PLIGHTHOUSE plh);
//    BOOL                            RenderLighthouse(PLIGHTHOUSE plh, float fIntensity);
//    void                            ReleaseLighthouse(PLIGHTHOUSE plh);
//    void                            GenerateCliffNormals(PHHVERTEX pr);
//    void                            GenerateBeachNormals(PHHVERTEX pr);
//    void                            GenerateWaveNormals(PHHVERTEX pr, float* pfSqrtLookup);

    virtual void                    ProcessInput();
    virtual BOOL                    InitView();

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);
};

#endif //__HIGHTIDE_H__
