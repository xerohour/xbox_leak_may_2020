/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    cogs.h

Author:

    Matt Bronder

Description:

    Clockwork animation and rendering.

*******************************************************************************/

#ifndef __COGS_H__
#define __COGS_H__

#define NUM_COG_TEXTURES        17
#define NUM_SHAFT_TEXTURES      7

#define SHAFT_RADIUS            0.25f
#define S_COG_RADIUS            (0.375f * 4.0f / 1.5f)
#define M_COG_RADIUS            (0.375f * 6.0f / 1.5f)
#define L_COG_RADIUS            (0.375f * 10.0f / 1.5f)
#define S_COG_WIDTH             (0.375f * 4.0f / 1.5f)
#define M_COG_WIDTH             (0.375f * 2.0f / 1.5f)
#define L_COG_WIDTH             (0.375f * 1.0f / 1.5f)
#define TOOTH_HEIGHT            (0.375f * 2.6f / 1.5f)
#define TOOTH_WIDTH_BASE        (0.375f * 2.6f / 1.5f)
#define TOOTH_WIDTH_TOP         (0.375f * 1.54f / 1.5f)
#define MIN_COG_SPACING         ((L_COG_RADIUS + TOOTH_HEIGHT) * 2.0f)
#define S_COG_SIDES             6
#define M_COG_SIDES             8
#define L_COG_SIDES             12

#define GEAR_LENGTH             ((3.0f * MIN_COG_SPACING) + FRND(5.0f))

#define CYLCAPS_NONE            0
#define CYLCAPS_TOP             1
#define CYLCAPS_BOTTOM          2
#define CYLCAPS_BOTH            3

typedef enum _COGSIZE {
    COGSIZE_SMALL = 0,
    COGSIZE_MEDIUM = 1,
    COGSIZE_LARGE = 2
} COGSIZE, *PCOGSIZE;

typedef struct _D3DRECT3D {
    D3DXVECTOR3         v1;     // -x,-y,-z
    D3DXVECTOR3         v2;     // +x,+y,+z
} D3DRECT3D, *PD3DRECT3D;

typedef struct _COG {
    DWORD                       dwSides;
    PVERTEX                     prCenter;
    LPDIRECT3DVERTEXBUFFER8     pd3drCenter;
    DWORD                       dwCenterVertices;
    LPWORD                      pwCenter;
    DWORD                       dwCenterIndices;
    LPDIRECT3DINDEXBUFFER8      pd3diCenter;
    PVERTEX*                    pprTeeth;
    LPDIRECT3DVERTEXBUFFER8     pd3drTeeth;
    DWORD                       dwTextureIndex;
} COG, *PCOG;

typedef struct _GEAR {

    COG                         cog[3];
    PVERTEX                     prShaft;
    LPDIRECT3DVERTEXBUFFER8     pd3drShaft;
    D3DXMATRIX                  mWorld;
    D3DXMATRIX                  mRotate;
    DWORD                       dwClip[4];

    DWORD                       dwShaftTextureIndex;
    int                         nMaterialIndex;
    int                         nLightIndex;
    int                         nSpecular;

    D3DXVECTOR3                 vPos;
    D3DXVECTOR3                 vDir;
    float                       fAngularVelocity;
    float                       fSpin;
    COGSIZE                     csCogSize[2];
    float                       fCogOffset[2];

    struct _GEAR*               pgearNext[2];

} GEAR, *PGEAR;

typedef struct _CLOCKWORK {

    PVERTEX                     prUnitShaft;

    DWORD                       dwShaftVertices;
    LPWORD                      pwShaft;
    DWORD                       dwShaftIndices;

    DWORD                       dwToothVertices;
    LPWORD                      pwTooth;
    DWORD                       dwToothIndices;

    LPDIRECT3DINDEXBUFFER8      pd3diShaft;
    LPDIRECT3DINDEXBUFFER8      pd3diTooth;

    PGEAR                       pgearRoot;

    LPDIRECT3DTEXTURE8          pd3dtCog[NUM_COG_TEXTURES];
    LPDIRECT3DTEXTURE8          pd3dtShaft[NUM_SHAFT_TEXTURES];

} CLOCKWORK, *PCLOCKWORK;

BOOL      CreateCog(PCOG pcog, DWORD dwSides, float fRadius, float fWidth, 
                    float fOffset = 0.0f);
void      ReleaseCog(PCOG pcog);

BOOL      CreateClockwork(LPDIRECT3DDEVICE8 pDevice, PCLOCKWORK pcw);
void      UpdateClockwork(PCLOCKWORK pcw, float fTimeDilation);
void      RenderClockwork(LPDIRECT3DDEVICE8 pDevice, PCLOCKWORK pcw);
void      ReleaseClockwork(PCLOCKWORK pcw);

PGEAR     CreateGear(PCLOCKWORK pcw, PCOGSIZE pcsCogSize, float* pfCogOffset, 
                    D3DXVECTOR3* pvPos, D3DXVECTOR3* pvDir, 
                    float fAngularVelocity, float fRotation, 
                    PD3DRECT3D prectBox, DWORD dwDepth = 1);
PGEAR     AddAttachedGear(PCLOCKWORK pcw, PGEAR pgearParent, UINT uCog, 
                    DWORD dwDepth);
void      AddGear(PCLOCKWORK pcw, PGEAR pgear, DWORD dwDepth = 1);
void      RotateGear(PCLOCKWORK pcw, PGEAR pgear, float fTheta);
void      RenderGear(LPDIRECT3DDEVICE8 pDevice, PCLOCKWORK pcw, PGEAR pgear);
void      ReleaseGear(PGEAR pgear);

BOOL      CreateCylinder(PVERTEX* pprVertices, LPDWORD pdwNumVertices, 
                    LPWORD* ppwIndices, LPDWORD pdwNumIndices, 
                    DWORD dwSides, float fRadius, float fWidth,
                    DWORD dwCylinderCaps = CYLCAPS_BOTH);

void      ReleaseCylinder(PVERTEX prVertices, LPWORD pwIndices);

void      SetRect3D(PD3DRECT3D prect, D3DXVECTOR3* pv1, 
                    D3DXVECTOR3* pv2);
BOOL      GearCollision(PD3DRECT3D prectBox1, PD3DRECT3D prectBox2);

#endif // __COGS_H__