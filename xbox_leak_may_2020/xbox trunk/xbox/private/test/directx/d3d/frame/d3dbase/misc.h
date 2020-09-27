/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    misc.h

Author:

    Matt Bronder

Description:

    Miscellaneous functions.

*******************************************************************************/

#ifndef __MISC_H__
#define __MISC_H__

//******************************************************************************
// Precomputed fractions of pi
//******************************************************************************

#define M_PI        3.14159265359f
#define M_2PI       6.28318530718f
#define M_PIDIV2    1.57079632680f
#define M_3PIDIV2   4.71238898039f
#define M_PIDIV4    0.78539816340f
#define M_3PIDIV4   2.35619449019f
#define M_5PIDIV4   3.92699081699f
#define M_7PIDIV4   5.49778714378f

//******************************************************************************
// Macros
//******************************************************************************

#define countof(x)  (sizeof(x) / sizeof(*(x)))

#define RND()       ((float)rand() / (float)RAND_MAX)
#define FRND(x)     (RND() * (x))
#define NRND()      (-1.0f + 2.0f * RND())
#define SFRND(x)    (NRND() * (x))
//#define FABS(x)     ((float)((DWORD)(x) & 0x7FFF))
#define FABS(x)     ((x) < 0.0f ? -(x) : (x))

#ifndef RGB_MAKE
/*
 * Format of CI colors is
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |    alpha      |         color index           |   fraction    |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
#define CI_GETALPHA(ci)    ((ci) >> 24)
#define CI_GETINDEX(ci)    (((ci) >> 8) & 0xffff)
#define CI_GETFRACTION(ci) ((ci) & 0xff)
#define CI_ROUNDINDEX(ci)  CI_GETINDEX((ci) + 0x80)
#define CI_MASKALPHA(ci)   ((ci) & 0xffffff)
#define CI_MAKE(a, i, f)    (((a) << 24) | ((i) << 8) | (f))

/*
 * Format of RGBA colors is
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |    alpha      |      red      |     green     |     blue      |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
#define RGBA_GETALPHA(rgb)      ((rgb) >> 24)
#define RGBA_GETRED(rgb)        (((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb)      (((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)       ((rgb) & 0xff)
#define RGBA_MAKE(r, g, b, a)   ((D3DCOLOR) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))

/* D3DRGB and D3DRGBA may be used as initialisers for D3DCOLORs
 * The float values must be in the range 0..1
 */
#define D3DRGB(r, g, b) \
    (0xff000000L | ( ((long)((r) * 255)) << 16) | (((long)((g) * 255)) << 8) | (long)((b) * 255))
#define D3DRGBA(r, g, b, a) \
    (   (((long)((a) * 255)) << 24) | (((long)((r) * 255)) << 16) \
    |   (((long)((g) * 255)) << 8) | (long)((b) * 255) \
    )

/*
 * Format of RGB colors is
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |    ignored    |      red      |     green     |     blue      |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
#define RGB_GETRED(rgb)         (((rgb) >> 16) & 0xff)
#define RGB_GETGREEN(rgb)       (((rgb) >> 8) & 0xff)
#define RGB_GETBLUE(rgb)        ((rgb) & 0xff)
#define RGBA_SETALPHA(rgba, x) (((x) << 24) | ((rgba) & 0x00ffffff))
#define RGB_MAKE(r, g, b)       ((D3DCOLOR) (((r) << 16) | ((g) << 8) | (b)))
#define RGBA_TORGB(rgba)       ((D3DCOLOR) ((rgba) & 0xffffff))
#define RGB_TORGBA(rgb)        ((D3DCOLOR) ((rgb) | 0xff000000))

#endif

// Clip flags
#define CLIP_LEFT               0x0001;
#define CLIP_RIGHT              0x0002;
#define CLIP_BOTTOM             0x0004;
#define CLIP_TOP                0x0008;
#define CLIP_FRONT              0x0010;
#define CLIP_BACK               0x0020;

//******************************************************************************
// Resource format validation
//******************************************************************************

HRESULT CheckCurrentDeviceFormat(LPDIRECT3DDEVICE8 pDevice, DWORD dwUsage, 
                        D3DRESOURCETYPE d3drt, D3DFORMAT fmt);

//******************************************************************************
// Vertex transformation routine
//******************************************************************************

HRESULT TransformVertices(LPDIRECT3DDEVICE8 pDevice, LPVOID pvDst, LPVOID pvSrc, 
                        DWORD dwNumVertices, DWORD dwStrideDst, DWORD dwStrideSrc, 
                        LPDWORD pdwClip = NULL);

//******************************************************************************
// Directional lighting function
//******************************************************************************

BOOL LightVertices(PVERTEX prSrc, PLVERTEX prDst, UINT uNumVertices, 
                        D3DMATERIAL8* pmaterial, D3DLIGHT8* plight, 
                        D3DCOLOR cAmbient, D3DXVECTOR3* pvViewPos);

BOOL LightVertices(LPVOID pvNormal, DWORD dwStrideNormal,
                        D3DCOLOR* pcDiffuse, DWORD dwStrideDiffuse,
                        UINT uNumVertices, D3DMATERIAL8* pmaterial, 
                        D3DLIGHT8* plight, D3DCOLOR cAmbient, 
                        LPVOID pvPosition = NULL, DWORD dwStridePosition = 0,
                        D3DCOLOR* pcSpecular = NULL, DWORD dwStrideSpecular = 0,
                        D3DXVECTOR3* pvViewPos = NULL);

//******************************************************************************
// Matrix utility function prototypes
//******************************************************************************

void                InitMatrix(D3DMATRIX* pd3dm,
                        float _11, float _12, float _13, float _14,
                        float _21, float _22, float _23, float _24,
                        float _31, float _32, float _33, float _34,
                        float _41, float _42, float _43, float _44);

void                SetView(D3DXMATRIX* pd3dm, LPD3DXVECTOR3 pvPos, 
                        LPD3DXVECTOR3 pvAt, LPD3DXVECTOR3 pvUp);

void                SetPerspectiveProjection(D3DMATRIX* pd3dm, float fFront, 
                        float fBack, float fFieldOfView, float fAspect);

void                SetParallelProjection(D3DMATRIX* pd3dm, float fFront, 
                        float fBack, float fFieldOfView, float fAspect);
//******************************************************************************
// Geometry functions
//******************************************************************************

BOOL                CreateSphere(PVERTEX* pprVertices, LPDWORD pdwNumVertices,
                        LPWORD* ppwIndices, LPDWORD pdwNumIndices,
                        float fRadius = 1.0f, UINT uStepsU = 16, 
                        UINT uStepsV = 16, float fTileU = 1.0f, float fTileV = 1.0f);

void                ReleaseSphere(PVERTEX* ppr, LPWORD* ppw);

//******************************************************************************
// Miscellaneous functions
//******************************************************************************

inline DWORD        F2DW(float f) {return *((LPDWORD)&f);}

#endif // __MISC_H__
